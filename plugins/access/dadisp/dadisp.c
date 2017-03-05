/*----------------------------------------------------------------------------
 * dadisp.c  -  Handle signals saved a DaDisp signal file format
 *
 * Description:
 * The plugin provides access to recordings saved in a DaDisp signal file
 * format. Because I have not used the program DaDisp but have only some
 * files, this plugin supports only this format (files with the extension
 * 'dsp' and a ASCII header at the beginning).
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2005-2007, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define _LIBRASCH_BUILD
#include <ra.h>
#include <ra_priv.h>
#include <handle_plugin.h>

#include <ra_endian.h>
#include <pl_general.h>
#include <ra_file.h>
#include <ra_estimate_ch_infos.h>

#include "dadisp.h"



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
	if (ra || ps)
		;

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
	int ret = 0;
	struct info *i;
	FILE *fp = NULL;
	char fn[MAX_PATH_RA];
	char buf[12];

	i = malloc(sizeof(struct info));
	if (!i)
		goto error_open;
	memset(i, 0, sizeof(struct info));

	/* split file in dir and filename w/o extension */
	split_filename(name, SEPARATOR, i->dir, i->name, i->ext);
	if (i->ext[0] == '\0')
		strcpy(i->ext, "dsp");

	sprintf(fn, "%s%c%s.%s", i->dir, SEPARATOR, i->name, i->ext);
	fp = ra_fopen(fn, "rb", 0);
	if (fp == NULL)
		goto error_open;

	/* check if file starts with 'DATASET' (all the files I have
	   start this way, therefore I check this) */
	if (fread(buf, 1, 7, fp) != 7)
		goto error_open;
	if (strncmp(buf, "DATASET", 7) != 0)
		goto error_open;

	ret = 1;

 error_open:
	if (i)
		free(i);
	if (fp)
		fclose(fp);

	return ret;
} /* pl_check_meas() */


int
pl_find_meas(const char *dir, struct find_meas *f)
{
	return find_meas(dir, "*.dsp", PLUGIN_NAME, f, pl_check_meas);
} /* pl_find_meas() */


meas_handle
pl_open_meas(ra_handle ra, const char *file, int fast)
{
	struct ra_meas *meas;
	struct info *i = NULL;
	char mask[MAX_PATH_RA];

	if (fast)
		;

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

	if (get_data(i) == -1)
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

	if (i->ch_type_infos)
		free(i->ch_type_infos);

	if (i->comment)
		free(i->comment);

	if (i->ci)
	{
		for (j = 0; j < i->num_ch; j++)
		{
			if (i->ci[j].label)
				free(i->ci[j].label);
			if (i->ci[j].unit)
				free(i->ci[j].unit);
		}
		free(i->ci);
	}

	free(i);
} /* free_info_struct() */



int
get_data(struct info *i)
{
	int ret = 0;
	char file[MAX_PATH_RA];
	char file_wo_ext[MAX_PATH_RA];
	FILE *fp;
	int j;
	char buf[4000];
	int buf_size = 4000;
	int data_start = 0;
	char *v;

	sprintf(file, "%s%c%s.%s", i->dir, SEPARATOR, i->name, i->ext);
	fp = ra_fopen(file, "rb", 0);
	if (fp == NULL)
	{
		_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-dadisp");
		return -1;
	}

	while (fgets(buf, buf_size, fp))
	{
		if ((strncmp(buf, "DATA", 4) == 0) && (buf[4] == 0xd))
		{
			data_start = 1;
			break;
		}

		v = buf + strlen(buf) - 1;
		while (v > buf)
		{
			if ((*v != 0xd) && (*v != 0xa))
				break;

			*v = '\0';
			v--;
		}
		v = strchr(buf, ' ');
		if (v == NULL)
			continue;
		
		if (strncmp(buf, "NUM_SERIES", 10) == 0)
		{
			i->num_ch = atoi(v+1);
			i->ci = (struct ch_info *)malloc(sizeof(struct ch_info) * i->num_ch);
			memset(i->ci, 0, sizeof(struct ch_info) * i->num_ch);
		}
		else if (strncmp(buf, "STORAGE_MODE", 12) == 0)
		{
			if (strcmp(v+1, "interlaced") == 0)
				i->storage_mode = DATA_INTERLACED;
			else
				ret = -1;
		}
		else if (strncmp(buf, "SERIES", 6) == 0)
		{
			ret = get_ch_labels(i, v+1);
		}
		else if (strncmp(buf, "DATE", 4) == 0)
		{
			ret = set_rec_date(i, v+1);
		}
		else if (strncmp(buf, "TIME", 4) == 0)
		{
			ret = set_rec_time(i, v+1);
		}
		else if (strncmp(buf, "FILE_TYPE", 9) == 0)
		{
			if (strcmp(v+1, "DOUBLE") == 0)
				i->storage_mode = RA_CH_SAMPLE_TYPE_DOUBLE;
		}
		else if (strncmp(buf, "INTERVAL", 8) == 0)
		{
			double interval;

			interval = strtod(v+1, NULL);
			if (interval > 0.0)
				i->samplerate = 1.0 / interval;
			else
			{
				_ra_set_error(i->ra, RA_ERR_MEAS_MISSING_INFO, "plugin-dadisp");
				ret = -1;
			}
		}
		else if (strncmp(buf, "VERT_UNITS", 10) == 0)
		{
			ret = set_ch_units(i, v+1);
		}
		else if (strncmp(buf, "HORZ_UNITS", 10) == 0)
		{
			; 	/* do nothing */
		}
		else if (strncmp(buf, "COMMENT",7 ) == 0)
		{
			i->comment = (char *)malloc(sizeof(char) * (strlen(v+1) + 1));
			strcpy(i->comment, v+1);
		}

		if (ret != 0)
			break;
	}

	i->data_start = ftell(fp);
	fseek(fp, 0, SEEK_END);
	i->num_samples = ftell(fp) - i->data_start;
	i->num_samples /= (i->num_ch * sizeof(double));

	fclose(fp);

	if ((ret != 0) || (data_start != 1))
		return (ret != 0 ? ret : -1);

	/* try to get channel infos */
	i->ch_type_infos = malloc(sizeof(struct ra_est_ch_infos) * i->num_ch);
	memset(i->ch_type_infos, 0, sizeof(struct ra_est_ch_infos) * i->num_ch);
	i->mm_sec = 1;
	sprintf(file_wo_ext, "%s%c%s", i->dir, SEPARATOR, i->name);
	for (j = 0; j < i->num_ch; j++)
	{
		if (ra_est_ch_type(i->ra, i->dir, file_wo_ext, 0, i->ci[j].label, &(i->ch_type_infos[j])) == 0)
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

	return ret;
} /* get_data() */


int
get_ch_labels(struct info *i, char *buf)
{
	int ret = 0;
	char *next, *p;
	int curr;

	if ((i == NULL) || (i->ci == NULL) | (i->num_ch <= 0) || (buf == NULL) || (buf[0] == '\0'))
		return 0;

	curr = 0;
	p = buf;
	do
	{
		if (curr >= (i->num_ch))
		{
			ret = -1;
			break;
		}

		next = strchr(p, ',');
		if (next)
		{
			*next = '\0';
			next++;
		}

		i->ci[curr].label = (char *)malloc(sizeof(char) * (strlen(p) + 1));
		strcpy(i->ci[curr].label, p);
		curr++;
		p = next;
	} while (p);
	
	return ret;
} /* get_ch_labels() */


int
set_ch_units(struct info *i, char *buf)
{
	int ret = 0;
	int j;

	if ((i == NULL) || (i->ci == NULL) | (i->num_ch <= 0) || (buf == NULL) || (buf[0] == '\0'))
		return 0;
	
	for (j = 0; j < i->num_ch; j++)
	{
		i->ci[j].unit = (char *)malloc(sizeof(char) * (strlen(buf) + 1));
		strcpy(i->ci[j].unit, buf);
	}
	
	return ret;
} /* set_ch_units() */


int
set_rec_date(struct info *i, char *buf)
{
	char *p;

	if ((i == NULL) || (buf == NULL) || (buf[0] == '\0'))
		return 0;

 	i->rec_date.day = i->rec_date.month = i->rec_date.year = 0;

	p = buf;
	i->rec_date.day = atoi(p);
	while (isdigit(*p))
		p++;
	if ((*p == '\0') || (*(p+1) == '\0'))
		return 0;
	i->rec_date.month = atoi(p+1);

	p++;
	while (isdigit(*p))
		p++;
	if ((*p == '\0') || (*(p+1) == '\0'))
		return 0;
	i->rec_date.year = atoi(p+1);

	if (i->rec_date.year < 2000)
	{
		if (i->rec_date.year < 80)
			i->rec_date.year += 2000;
		else 
			i->rec_date.year += 1900;
	}

	return 0;
} /* set_rec_date() */


int
set_rec_time(struct info *i, char *buf)
{
	char *p;

	if ((i == NULL) || (buf == NULL) || (buf[0] == '\0'))
		return 0;

 	i->rec_time.hour = i->rec_time.min = i->rec_time.sec = i->rec_time.msec = 0;

	p = buf;
	i->rec_time.hour = atoi(p);
	while (isdigit(*p))
		p++;
	if ((*p == '\0') || (*(p+1) == '\0'))
		return 0;
	i->rec_time.min = atoi(p+1);

	p++;
	while (isdigit(*p))
		p++;
	if ((*p == '\0') || (*(p+1) == '\0'))
		return 0;
	i->rec_time.sec = atoi(p+1);

	p++;
	while (isdigit(*p))
		p++;
	if ((*p == '\0') || (*(p+1) == '\0'))
		return 0;
	i->rec_time.msec = atoi(p+1);

	return 0;
} /* set_rec_time() */


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

	_ra_set_error(h, RA_ERR_UNKNOWN_INFO, "plugin-ishne");

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
		_ra_set_error(h, RA_ERR_ERROR, "plugin-ishne");
		return -1;
	}

	set_meta_info(vh, meta_inf->name, meta_inf->desc, meta_inf->id);

	meas = ra_meas_handle_from_any_handle(h);
	i = meas->priv;

	/* get asked information */
	if ((id >= RA_INFO_RECORDING_START) && (id < RA_INFO_RECORDING_END))
		ret = get_info_rec(i, id, vh);
	else
		ret = -1;

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
		_ra_set_error(i->ra, RA_ERR_OUT_OF_RANGE, "plugin-ishne");
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
		ra_value_set_string(vh, "DaDisp Signal File");
		break;
	case RA_INFO_REC_GEN_TIME_C:
 		sprintf(t, "%02d:%02d:%02d", i->rec_time.hour,
 			i->rec_time.min, i->rec_time.sec);
 		ra_value_set_string(vh, t);
		break;
	case RA_INFO_REC_GEN_DATE_C:
 		sprintf(t, "%02d.%02d.%d", i->rec_date.day,
 			i->rec_date.month, i->rec_date.year);
 		ra_value_set_string(vh, t);
		break;
	case RA_INFO_REC_GEN_DURATION_SEC_D:
		d = ((double)i->num_samples / (double)i->samplerate);
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
		ra_value_set_string_utf8(vh, i->ci[ch].label);
		break;
	case RA_INFO_REC_CH_DESC_C:
		ra_value_set_string(vh, "");
		break;
	case RA_INFO_REC_CH_NUM_SAMPLES_L:
		ra_value_set_long(vh, i->num_samples);
		break;
	case RA_INFO_REC_CH_SAMPLERATE_D:
		ra_value_set_double(vh, (double)i->samplerate);
		break;
	case RA_INFO_REC_CH_AMP_RESOLUTION_D:
		ra_value_set_double(vh, 1.0);
		break;
	case RA_INFO_REC_CH_SAMPLE_TYPE_L:
		ra_value_set_long(vh, RA_CH_SAMPLE_TYPE_DOUBLE);
		break;
	case RA_INFO_REC_CH_UNIT_C:
		ra_value_set_string_utf8(vh, i->ci[ch].unit);
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
		_ra_set_error(i->ra, RA_ERR_UNKNOWN_INFO, "plugin-ishne");
		ret = -1;
		break;
	}

	return ret;
} /* get_info_rec() */


int
pl_set_info(any_handle mh, value_handle vh)
{
	if (mh || vh)
		;

	return 0;
} /* pl_set_info() */


size_t
pl_get_raw(meas_handle mh, rec_handle rh, int ch, size_t start, size_t num_data, DWORD *data, DWORD *data_high)
{
	struct info *i = ((struct ra_meas *) mh)->priv;
	size_t num = 0;

	if (rh)
		;

	num = read_raw(i, ch, start, num_data, NULL, data);

	if (data_high)
	{
		size_t l;
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

	if (rh)
		;

	num = read_raw(i, ch, start, num_data, data, NULL);

	return num;
} /* pl_get_raw_unit() */


size_t
read_raw(struct info * i, int ch, size_t start, size_t num, double *data, DWORD *data_raw)
{
	int j;
	char fn[MAX_PATH_RA];
	double *buf = NULL;
	size_t buf_size = 0;
	size_t num_ret = 0;
	FILE *fp = NULL;
	size_t n_use, data_start, offset;

	data_start = i->data_start + (start * i->num_ch * sizeof(double));
	n_use = num;
	if ((start + n_use) > (size_t)(i->num_samples))
		n_use = i->num_samples - start;
	if (n_use <= 0)
		return 0;

	buf_size = n_use * i->num_ch;
	buf = (double *)malloc(sizeof(double) * buf_size);
	
	sprintf(fn, "%s%c%s.%s", i->dir, SEPARATOR, i->name, i->ext);
	if ((fp = ra_fopen(fn, "rb", 0)) == NULL)
	{
		_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-ishne");
		goto error;
	}

	fseek(fp, (long)data_start, SEEK_SET);
	if (fread(buf, sizeof(double), buf_size, fp) != buf_size)
	{
		_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-ishne");
		goto error;
	}

	offset = ch;
	for (j = 0; j < (int)n_use; j++)
	{
		if (data)
			data[j] = buf[offset];
		if (data_raw)
			data_raw[j] = (DWORD)buf[offset];

		offset += i->num_ch;
	}
	num_ret = n_use;

 error:
	if (buf)
		free(buf);
	if (fp)
		fclose(fp);

	return num_ret;
} /* read_raw() */
