/*----------------------------------------------------------------------------
 * adi_ascii.c  -  Handle signals recorded with Powerlab and saved as ASCII files
 *
 * Description:
 * The plugin provides access to signals recorded with Powerlab from ADInstruments
 * and exported as an ASCII file.
 *
 * Author(s): Raphael Schneider (librasch@gmail.com)
 *
 * Copyright (C) 2005-2010, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define _LIBRASCH_BUILD
#include <ra.h>
#include <ra_priv.h>
#include <handle_plugin.h>

#include <ra_endian.h>
#include <pl_general.h>
#include <ra_file.h>
#include <ra_estimate_ch_infos.h>

#include "adi_ascii.h"



LIBRAAPI int
load_ra_plugin(struct plugin_struct *ps)
{
	strcpy(ps->info.name, PLUGIN_NAME);
	strcpy(ps->info.desc, PLUGIN_DESC);
	sprintf(ps->info.build_ts, "%s  %s", __DATE__, __TIME__);
	ps->info.type = PLUGIN_TYPE;
	ps->info.license = PLUGIN_LICENSE;
	sprintf(ps->info.version, PLUGIN_VERSION);

	ps->access.check_meas = pl_check_meas;
	ps->access.find_meas = pl_find_meas;

	ps->access.open_meas = pl_open_meas;
	ps->access.close_meas = pl_close_meas;

	ps->access.get_info_id = pl_get_info_id;
	ps->access.get_info_idx = pl_get_info_idx;

	ps->access.get_raw = pl_get_raw;
	ps->access.get_raw_unit = pl_get_raw_unit;

	return 0;
} /* load_ra_plugin() */


LIBRAAPI int
init_ra_plugin(struct librasch *ra, struct plugin_struct *ps)
{
	if (!ra || !ps)
		return -1;

	return 0;
} /* init_ra_plugin() */



LIBRAAPI int
fini_ra_plugin(void)
{
	return 0;
} /* fini_plugin() */


int
pl_check_meas(const char *name)
{
	struct info *i;
	FILE *fp = NULL;
	char fn[MAX_PATH_RA];
	char buf[200];

	i = malloc(sizeof(struct info));
	if (!i)
		goto wrong;
	memset(i, 0, sizeof(struct info));

	/* split file in dir and filename w/o extension */
	split_filename(name, SEPARATOR, i->dir, i->name, i->ext);
	sprintf(fn, "%s%c%s.%s", i->dir, SEPARATOR, i->name, i->ext);
	fp = ra_fopen(fn, "r", 0);
	if (fp == NULL)
		goto wrong;
	if (fgets(buf, 200, fp) == NULL)
		goto wrong;
	if (strncmp(buf, "Interval", strlen("Interval")) != 0)
		goto wrong;

	if (fgets(buf, 200, fp) == NULL)
		goto wrong;
	if (strncmp(buf, "ExcelDateTime", strlen("ExcelDateTime")) != 0)
		goto wrong;

	if (fgets(buf, 200, fp) == NULL)
		goto wrong;
	if (strncmp(buf, "TimeFormat", strlen("TimeFormat")) != 0)
		goto wrong;

	if (fgets(buf, 200, fp) == NULL)
		goto wrong;
	if (strncmp(buf, "ChannelTitle", strlen("ChannelTitle")) != 0)
		goto wrong;

	if (fgets(buf, 200, fp) == NULL)
		goto wrong;
	if (strncmp(buf, "Range", strlen("Range")) != 0)
		goto wrong;

	/* Skip the test for 'UnitName' as this information can be missing
	if (fgets(buf, 200, fp) == NULL)
		goto wrong;
	if (strncmp(buf, "UnitName", strlen("UnitName")) != 0)
		goto wrong;
	*/

	fclose(fp);
	fp = NULL;

	return 1;

 wrong:
	if (i)
		free(i);
	if (fp)
		fclose(fp);

	return 0;
} /* pl_check_meas() */


int
pl_find_meas(const char *dir, struct find_meas *f)
{
	return find_meas(dir, "*.txt", PLUGIN_NAME, f, pl_check_meas);
} /* pl_find_meas() */


meas_handle
pl_open_meas(ra_handle ra, const char *file, int fast)
{
	struct ra_meas *meas;
	struct info *i = NULL;
	char mask[MAX_PATH_RA];

	if (!pl_check_meas(file))
		goto error_open;

	if ((meas = ra_alloc_mem(sizeof(struct ra_meas))) == NULL)
		goto error_open;
	memset(meas, 0, sizeof(struct ra_meas));
	if ((i = calloc(1, sizeof(struct info))) == NULL)
		goto error_open;
	meas->priv = i;
	i->ra = ra;

	/* split file in dir and filename w/o extension */
	split_filename(file, SEPARATOR, i->dir, i->name, i->ext);

	if (get_data(i, fast) == -1)
		goto error_open;

	sprintf(mask, "%s*", i->name);
	find_files(i->dir, mask, &(i->files), &(i->num_files), &(i->size));

	/* set session infos */
	set_one_session(meas, NULL, NULL);

	return meas;

 error_open:
	if (i)
		free_info_struct(i);

	return NULL;
} /* open_meas() */


int
pl_close_meas(meas_handle mh)
{
	struct ra_meas *meas = (struct ra_meas *)mh;
	struct info *i;

	if (!mh)
		return -1;
	i = meas->priv;

	free_info_struct(i);

	return 0;
} /* close_meas() */



void
free_info_struct(struct info *i)
{
	int j;

	if (i == NULL)
		return;

	if (i->files)
	{
		for (j = 0; j < i->num_files; j++)
		{
			if (i->files[j])
				ra_free_mem(i->files[j]);
		}
		ra_free_mem(i->files);
	}

	if (i->ci)
		free(i->ci);
	if (i->samples)
	{
		/* use c-lib free() functions instead of ra-mem functions */
		for (j = 0; j < i->num_ch; j++)
			free(i->samples[j]);
		free(i->samples);
	}

	free(i);
} /* free_info_struct() */



int
get_data(struct info *i, int fast)
{
	int ret = -1;
	char file[MAX_PATH_RA];
	char file_wo_ext[MAX_PATH_RA];
	FILE *fp;
	char buf[200];
	struct elements elem;
	double temp;
	double *data = NULL;
	long l;
	int j;
	int dec_point_is_comma = 0;

	memset(&elem, 0, sizeof(struct elements));

	sprintf(file, "%s%c%s.%s", i->dir, SEPARATOR, i->name, i->ext);
	fp = ra_fopen(file, "r", 0);
	if (fp == NULL)
	{
		_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-adi-ascii");
		goto error;
	}

	/* assess if decimal-point is on this system is a comma */
	temp = atof("0,1");
	if (temp == 0.1)
		dec_point_is_comma = 1;
	
	while (fgets(buf, 200, fp))
	{
		if ((buf[0] >= '0') && (buf[0] <= '9'))
		{
			ret = 0;
			break;
		}

		if (get_data_elem(buf, &elem, dec_point_is_comma) != 0)
			break;

		if (strncmp(buf, "Interval", strlen("Interval")) == 0)
		{
			if (elem.num != 1)
				goto error;
			i->samplerate = 1.0 / elem.value[0];
		}
		else if (strncmp(buf, "ExcelDateTime", strlen("ExcelDateTime")) == 0)
		{
			if (elem.num != 2)
				goto error;
			parse_timestamp(&(i->ts_rec_start), elem.data[1]);
		}
		else if (strncmp(buf, "TimeFormat", strlen("TimeFormat")) == 0)
		{
			if (elem.num != 1)
				goto error;
			if (strcmp(elem.data[0], "StartOfBlock") == 0)
				i->time_format = TIME_FORMAT_START_OF_BLOCK;
		}
		else if (strncmp(buf, "ChannelTitle", strlen("ChannelTitle")) == 0)
		{
			if (prep_ch_info(i, elem.num) != 0)
				goto error;
			for (l = 0; l < i->num_ch; l++)
				strncpy(i->ci[l].name, elem.data[l], MAX_NAME-1);
		}
		else if (strncmp(buf, "Range", strlen("Range")) == 0)
		{
			if (prep_ch_info(i, elem.num) != 0)
				goto error;
			for (l = 0; l < i->num_ch; l++)
			{
				i->ci[l].range = elem.value[l];
				i->ci[l].bit_res = i->ci[l].range / pow(2, 16);
			}
			/* TODO: maybe process the unit also */
		}
		else if (strncmp(buf, "UnitName", strlen("UnitName")) == 0)
		{
			if (prep_ch_info(i, elem.num) != 0)
				goto error;
			for (l = 0; l < i->num_ch; l++)
			{
				if (strcmp(elem.data[l], "*") == 0)
					continue;
				strncpy(i->ci[l].unit, elem.data[l], MAX_UNIT-1);
			}
		}
		else if (strncmp(buf, "TopValue", strlen("TopValue")) == 0)
		{
			if (prep_ch_info(i, elem.num) != 0)
				goto error;
			for (l = 0; l < i->num_ch; l++)
			{
				if (strcmp(elem.data[l], "*") == 0)
					continue;
				i->ci[l].top_value = elem.value[l];
			}
		}
		else if (strncmp(buf, "BottomValue", strlen("BottomValue")) == 0)
		{
			if (prep_ch_info(i, elem.num) != 0)
				goto error;
			for (l = 0; l < i->num_ch; l++)
			{
				if (strcmp(elem.data[l], "*") == 0)
					continue;
				i->ci[l].bottom_value = elem.value[l];
			}
		}
	}
	if (ret != 0)
		goto error;

	/* try to get channel infos */
	i->ch_type_infos = malloc(sizeof(struct ra_est_ch_infos) * i->num_ch);
	memset(i->ch_type_infos, 0, sizeof(struct ra_est_ch_infos) * i->num_ch);
	i->mm_sec = 1;
	sprintf(file_wo_ext, "%s%c%s", i->dir, SEPARATOR, i->name);
	for (j = 0; j < i->num_ch; j++)
	{
		if (ra_est_ch_type(i->ra, i->dir, file_wo_ext, 0, i->ci[j].name, &(i->ch_type_infos[j])) == 0)
		{
			i->ch_type_infos[j].type = RA_CH_TYPE_UNKNOWN;
			i->ch_type_infos[j].mm_sec = 1;
			i->ch_type_infos[j].mm_unit = 1;
			i->ch_type_infos[j].offset = 0;
			i->ch_type_infos[j].centered = 0;
			i->ch_type_infos[j].center_value = 0;
		}
		else
		{
			/* check if unit is micro-something and scale amp.resolution down */
			/* TODO: find better way (more general) handling units in micro, milli,... */
			if (i->ci[j].unit[0] == 'u')
				i->ch_type_infos[j].mm_unit /= 1000.0;
		}

		if (i->mm_sec < i->ch_type_infos[j].mm_sec)
			i->mm_sec = i->ch_type_infos[j].mm_sec;
	}

	/* read raw data */
	if (!fast)
	{
		data = (double *)malloc(sizeof(double) * i->num_ch);
		i->samples = (double **)calloc(i->num_ch, sizeof(double *));
		do
		{
			if (get_data_line(buf, data, i->num_ch, dec_point_is_comma) != 0)
			{
				ret = -1;
				break;
			}

			i->num_samples++;
			if (i->num_samples >= i->raw_buf_size)
			{
				i->raw_buf_size += RAW_BUF_ADD;
				for (l = 0; l < i->num_ch; l++)
					i->samples[l] = realloc(i->samples[l], sizeof(double) * i->raw_buf_size);
			}

			for (l = 0; l < i->num_ch; l++)
			{
				if (data[l] == HUGE_VAL)
				{
					if (i->num_samples > 1)
						i->samples[l][i->num_samples-1] = i->samples[l][i->num_samples-2];
					else
						i->samples[l][i->num_samples-1] = 0;
				}
				else
					i->samples[l][i->num_samples-1] = data[l];
			}

		} while (fgets(buf, 200, fp));
		if (ret != 0)
			goto error;
	}

 error:
	if (fp)
		fclose(fp);

	if (data)
		free(data);

	return ret;
} /* get_data() */


int
get_data_elem(char *buf, struct elements *elem, int dec_point_is_comma)
{
	char *p, *pn, *c;

	if (buf == NULL)
		return -1;

	elem->num = 0;
	p = buf;
	while ((p != NULL) && ((p = strchr(p, '\t')) != NULL))
	{
		p += 1;
		pn = strchr(p, '\t');
		if (pn)
			*pn = '\0';

		elem->num += 1;
		c = strchr(p, '\r');
		if (c)
			*c = '\0';
		c = strchr(p, '\n');
		if (c)
			*c = '\0';
		strncpy(elem->data[elem->num-1], p, 200);
		elem->data[elem->num-1][strlen(p)] = '\0';
		if (dec_point_is_comma)
		{
			c = strchr(p, '.');
			if (c)
				*c = ',';
		}
		else
		{
			c = strchr(p, ',');
			if (c)
				*c = '.';
		}
		elem->value[elem->num-1] = atof(p);

		if (pn)
			*pn = '\t';
	}

	return 0;
} /* get_data_elem() */


int
prep_ch_info(struct info *i, int n)
{
	int j;

	if ((i == NULL) || (n <= 0))
		return -1;

	if (i->num_ch == 0)
	{
		i->num_ch = n;
		i->ci = calloc(i->num_ch, sizeof(struct ch_info));
		for (j = 0; j < n; j++)
		{
			i->ci[j].bit_res = pow(2, 16);
		}
	}
	else if (i->num_ch != n)
		return -1;

	return 0;
} /* prep_ch_info() */


int
parse_timestamp(struct timestamp *ts, char *string)
{
	char *p = string;

	ts->year = (short)atoi(p);
	p += 5;
	ts->mon = (short)atoi(p);
	p += 3;
	ts->day = (short)atoi(p);
	p += 3;
	ts->hour = (short)atoi(p);
	p += 3;
	ts->min = (short)atoi(p);
	p += 3;
	ts->sec = (short)atoi(p);
	p += 3;
	ts->msec = (short)atoi(p);

	return 0;
} /* parse_timestamp() */


int
get_data_line(char *buf, double *data, int n_ch, int dec_point_is_comma)
{
	int i;
	char *p, *c;



	p = buf;
	for (i = 0; i < n_ch; i++)
	{
		p = strchr(p, '\t');
		if (p == NULL)
			return -1;
		p++;
		if (dec_point_is_comma)
		{
			c = strchr(p, '.');
			if (c)
				*c = ',';
		}
		else
		{
			c = strchr(p, ',');
			if (c)
				*c = '.';
		}
		if (*p == 'N')
			data[i] = HUGE_VAL;
		else
			data[i] = atof(p);		
	}

	return 0;
} /* get_data_line() */


int
pl_get_info_id(any_handle h, int id, value_handle vh)
{
	int ret = 0;
	struct ra_meas *meas;
	struct info *i;
	char t[MAX_PATH_RA];

	meas = ra_meas_handle_from_any_handle(h);
	i = meas->priv;

	/* if asked for numbers of infos return them, if not go to get_info_id() */
	switch (id)
	{
	case RA_INFO_NUM_OBJ_INFOS_L:
		ra_value_set_long(vh, _num_infos_obj);
		break;
	case RA_INFO_NUM_REC_GEN_INFOS_L:
		ra_value_set_long(vh, _num_infos_rec_gen);
		break;
	case RA_INFO_NUM_REC_DEV_INFOS_L:
		ra_value_set_long(vh, _num_infos_rec_dev);
		break;
	case RA_INFO_NUM_REC_CH_INFOS_L:
		ra_value_set_long(vh, _num_infos_rec_ch);
		break;
	case RA_INFO_NUM_EVAL_INFOS_L:
		ra_value_set_long(vh, 0);
		break;
	case RA_INFO_SIZE_L:
		ra_value_set_long(vh, i->size);
		break;
	case RA_INFO_FILES_CA:
		ra_value_set_string_array(vh, (const char **)i->files, i->num_files);
		break;
	case RA_INFO_PATH_C:
		sprintf(t, "%s%c%s", i->dir, SEPARATOR, i->name);
		ra_value_set_string(vh, t);
		break;
	case RA_INFO_DIR_L:
		ra_value_set_long(vh, 0);	/* not saved in dir */
		break;
	default:
		ret = get_info_id(h, id, vh);
		break;
	}

	return ret;
} /* pl_get_info_id() */


int
pl_get_info_idx(any_handle h, int info_type, int index, value_handle vh)
{
	int *idx_arr = NULL;

	switch (info_type)
	{
	case RA_INFO_OBJECT:
		if ((index >= 0) && (index < _num_infos_obj))
			idx_arr = _infos_obj;
		else
			idx_arr = NULL;
		break;
	case RA_INFO_REC_GENERAL:
		if ((index >= 0) && (index < _num_infos_rec_gen))
			idx_arr = _infos_rec_gen;
		else
			idx_arr = NULL;
		break;
	case RA_INFO_REC_DEVICE:
		if ((index >= 0) && (index < _num_infos_rec_dev))
			idx_arr = _infos_rec_dev;
		else
			idx_arr = NULL;
		break;
	case RA_INFO_REC_CHANNEL:
		if ((index >= 0) && (index < _num_infos_rec_ch))
			idx_arr = _infos_rec_ch;
		else
			idx_arr = NULL;
		break;
	default:
		idx_arr = NULL;
		break;
	}

	if (idx_arr)
		return get_info_id(h, idx_arr[index], vh);

	_ra_set_error(h, RA_ERR_UNKNOWN_INFO, "plugin-adi-ascii");

	return -1;
} /* pl_get_info_idx() */


int
get_info_id(any_handle h, int id, value_handle vh)
{
	int ret = 0;
	struct ra_meas *meas;
	struct info *i;
	struct meta_info *meta_inf;

	/* get infos about asked measurement info */
	meta_inf = get_meta_info(id);

	/* all var's ok? */
	if ((vh == NULL) || (meta_inf == NULL) || (h == NULL))
	{
		_ra_set_error(h, RA_ERR_ERROR, "plugin-adi-ascii");
		return -1;
	}

	set_meta_info(vh, meta_inf->name, meta_inf->desc, meta_inf->id);

	meas = ra_meas_handle_from_any_handle(h);
	i = meas->priv;

	/* get asked information */
	if ((id >= RA_INFO_RECORDING_START) && (id < RA_INFO_RECORDING_END))
		ret = get_info_rec(i, id, vh);
	if ((id >= RA_INFO_OBJECT_START) && (id < RA_INFO_OBJECT_END))
		ret = get_info_obj(i, id, vh);

	return ret;
} /* get_info_id() */


int
get_info_rec(struct info *i, int id, value_handle vh)
{
	int ret = 0;
	char t[MAX_PATH_RA];
	double d;
	long ch;

	ch = ra_value_get_number(vh);

	if ((id >= RA_INFO_REC_CHANNEL) && (id < RA_INFO_REC_CHANNEL_END) &&
	    ((ch < 0) || (ch >= i->num_ch)))
	{
		_ra_set_error(i->ra, RA_ERR_OUT_OF_RANGE, "plugin-adi-ascii");
		return -1;
	}

	switch (id)
	{
	case RA_INFO_REC_GEN_TYPE_L:
		ra_value_set_long(vh, RA_REC_TYPE_TIMESERIES);
		break;
	case RA_INFO_REC_GEN_NAME_C:
		ra_value_set_string(vh, i->name);
		break;
	case RA_INFO_REC_GEN_DESC_C:
		ra_value_set_string(vh, "ADInstruments recording (text-file)");
		break;
	case RA_INFO_REC_GEN_TIME_C:
		sprintf(t, "%02d:%02d:%02d", i->ts_rec_start.hour,
			i->ts_rec_start.min, i->ts_rec_start.sec);
		ra_value_set_string(vh, t);
		break;
	case RA_INFO_REC_GEN_DATE_C:
		sprintf(t, "%02d.%02d.%d", i->ts_rec_start.day,
			i->ts_rec_start.mon, i->ts_rec_start.year);
		ra_value_set_string(vh, t);
		break;
	case RA_INFO_REC_GEN_DURATION_SEC_D:
		d = ((double)i->num_samples / i->samplerate);
		ra_value_set_double(vh, d);
		break;
	case RA_INFO_REC_GEN_NUMBER_L:
		ra_value_set_long(vh, 0);
		break;
	case RA_INFO_REC_GEN_NUM_SUB_REC_L:
		ra_value_set_long(vh, 0);
		break;
	case RA_INFO_REC_GEN_NUM_CHANNEL_L:
		ra_value_set_long(vh, i->num_ch);
		break;
	case RA_INFO_REC_GEN_OFFSET_L:
		ra_value_set_long(vh, 0);
		break;
	case RA_INFO_REC_GEN_PATH_C:
		sprintf(t, "%s%c%s", i->dir, SEPARATOR, i->name);
		ra_value_set_string(vh, t);
		break;
	case RA_INFO_REC_GEN_DIR_L:
		ra_value_set_long(vh, 0);	/* not saved in dir */
		break;
	case RA_INFO_REC_GEN_MM_SEC_D:
		ra_value_set_double(vh, i->mm_sec);
		break;
	case RA_INFO_REC_CH_NAME_C:
		ra_value_set_string(vh, i->ci[ch].name);
		break;
	case RA_INFO_REC_CH_DESC_C:
		ra_value_set_string(vh, "");
		break;
	case RA_INFO_REC_CH_NUM_SAMPLES_L:
		ra_value_set_long(vh, i->num_samples);
		break;
	case RA_INFO_REC_CH_SAMPLERATE_D:
		ra_value_set_double(vh, i->samplerate);
		break;
	case RA_INFO_REC_CH_NUM_BITS_L:
		ra_value_set_long(vh, 16);
		break;
	case RA_INFO_REC_CH_AMP_RESOLUTION_D:
		ra_value_set_double(vh, 1.0);	/* adjustment will be done in this module */
		break;
	case RA_INFO_REC_CH_SAMPLE_TYPE_L:
		ra_value_set_long(vh, RA_CH_SAMPLE_TYPE_FLOAT2);
		break;
	case RA_INFO_REC_CH_UNIT_C:
		if ((i->ci[ch].unit[0] == '\0') || (i->ci[ch].unit[0] == '*'))
			ra_value_set_string(vh, "mV");
		else
			ra_value_set_string_utf8(vh, i->ci[ch].unit);
		break;
	case RA_INFO_REC_CH_MIN_UNIT_D:
		ret = -1;
		break;
	case RA_INFO_REC_CH_MAX_UNIT_D:
		ret = -1;
		break;
	case RA_INFO_REC_CH_CENTER_VALUE_D:
		if (i->ch_type_infos[ch].centered)
			ra_value_set_double(vh, i->ch_type_infos[ch].center_value);
		else
			ret = -1;
		break;
	case RA_INFO_REC_CH_MM_UNIT_D:
		ra_value_set_double(vh, i->ch_type_infos[ch].mm_unit);
		break;
	case RA_INFO_REC_CH_TYPE_L:
		ra_value_set_long(vh, i->ch_type_infos[ch].type);
		break;
	default:
		_ra_set_error(i->ra, RA_ERR_UNKNOWN_INFO, "plugin-adi-ascii");
		ret = -1;
		break;
	}

	return ret;
} /* get_info_rec() */


int
get_info_obj(struct info *i, int id, value_handle vh)
{
	int ret = 0;

	if (vh == NULL)
		return -1;

	switch (id)
	{
	default:
		_ra_set_error(i->ra, RA_ERR_UNKNOWN_INFO, "plugin-adi-ascii");
		ret = -1;
		break;
	}

	return ret;
} /* get_info_obj() */


int
pl_set_info(any_handle mh, value_handle vh)
{
	if (!mh || !vh)
		return -1;

	return 0;
} /* pl_set_info() */


size_t
pl_get_raw(meas_handle mh, rec_handle rh, int ch, size_t start, size_t num_data, DWORD *data, DWORD *data_high)
{
	struct info *i = ((struct ra_meas *) mh)->priv;
	size_t num = 0;

	if (rh) /* to get rid of compiler warning */
		;

	num = read_raw(i, ch, start, num_data, NULL, data);
	if (data_high)
	{
		unsigned long l;
		for (l = 0; l < num; l++)
			data_high[l] = 0;
	}

	return num;
} /* pl_get_raw() */


size_t
pl_get_raw_unit(meas_handle mh, rec_handle rh, int ch, size_t start, size_t num_data, double *data)
{
	struct info *i = ((struct ra_meas *) mh)->priv;
	size_t num = 0;
	
	if (rh) /* to get rid of compiler warning */
		;

	num = read_raw(i, ch, start, num_data, data, NULL);

	return num;
} /* pl_get_raw_unit() */


size_t
read_raw(struct info * i, int ch, size_t start, size_t num, double *data, DWORD *data_raw)
{
	int j;
	size_t n_use;

	/* first check for sane values */
	if ((ch < 0) || (ch >= i->num_ch) || (start >= (size_t)i->num_samples) || (data == NULL))
	{
		return 0;
	}

	n_use = num;
	if ((start+n_use) >= (size_t)i->num_samples)
		n_use = i->num_samples - start;
	if (n_use < 0)
		return 0;

	if (data)
		memcpy(data, &(i->samples[ch][start]), sizeof(double) * n_use);
	if (data_raw)
	{
		for (j = 0; j < (int)n_use; j++)
			data_raw[j] = (DWORD)(i->samples[ch][start+j] / i->ci[ch].bit_res);
	}

	return n_use;
} /* read_raw() */

