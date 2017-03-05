/*----------------------------------------------------------------------------
 * ideeq_ascii.c  -  Handle signals recorded with IDEEQ and saved as ASCII file
 *
 * Description:
 * The plugin provides access to signals recorded with IDEEQ
 * and exported as an ASCII file.
 *
 * Author(s): Raphael Schneider (librasch@gmail.com)
 *
 * Copyright (C) 2009, Raphael Schneider
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

#include "ideeq_ascii.h"



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

	ps->access.get_eval_info = pl_get_eval_info;
	ps->access.get_class_num = pl_get_class_num;
	ps->access.get_class_info = pl_get_class_info;
	ps->access.get_prop_num = pl_get_prop_num;
	ps->access.get_prop_info = pl_get_prop_info;
	ps->access.get_ev_info = pl_get_ev_info;
	ps->access.get_ev_value = pl_get_ev_value;
/*	ps->access.get_ev_value_all = pl_get_ev_value_all;*/

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
	/* skip first line (is empty) */
	if (fgets(buf, 200, fp) == NULL)
		goto wrong;
	if (fgets(buf, 200, fp) == NULL)
		goto wrong;
	if (strncmp(buf, "IDEEQ", strlen("IDEEQ")) != 0)
		goto wrong;

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
	char buf[2000];
	double temp;
	double *data = NULL;
	long l;
	int j;
	int dec_point_is_comma = 0;

	sprintf(file, "%s%c%s.%s", i->dir, SEPARATOR, i->name, i->ext);
	fp = ra_fopen(file, "r", 0);
	if (fp == NULL)
	{
		_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-ideeq-ascii");
		goto error;
	}

	/* assess if decimal-point is on this system is a comma */
	temp = atof("0,1");
	if (temp == 0.1)
		dec_point_is_comma = 1;
	
	i->samplerate = get_samplerate(fp, dec_point_is_comma);
	if (i->samplerate <= 0)
	{
		_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-ideeq-ascii");
		goto error;
	}
	i->start_sample_su = get_start_su(fp, dec_point_is_comma, i->samplerate);
	calc_rec_start_ts(i);

	ret = 0;
	while (fgets(buf, 2000, fp))
	{
		/* when lines are starting with numbers, we are done with the header */
		if ((buf[0] >= '0') && (buf[0] <= '9'))
		{
			ret = 0;
			break;
		}

		if (strncmp(buf, "Name", strlen("Name")) == 0)
		{
		}
		else if (strncmp(buf, "Identification", strlen("Identification")) == 0)
		{
		}
		else if (strncmp(buf, "Date of birth", strlen("Date of birth")) == 0)
		{
		}
		else if (strncmp(buf, "Sexe", strlen("Sexe")) == 0)
		{
		}
		else if (strncmp(buf, "Additional information", strlen("Additional information")) == 0)
		{
			/* at the moment ignore data */
			continue;
		}
		else if (strncmp(buf, "Filename", strlen("Filename")) == 0)
		{
			/* at the moment ignore data */
			continue;
		}
		else if (strncmp(buf, "Description", strlen("Description")) == 0)
		{
			/* at the moment ignore data */
			continue;
		}
		else if (strncmp(buf, "Date and time of recording", strlen("Date and time of recording")) == 0)
		{
			/* ret = parse_timestamp(&(i->ts_rec_start), fp); */
		}
		else if (strncmp(buf, "Remarks", strlen("Remarks")) == 0)
		{
			/* at the moment ignore data */
			continue;
		}
		else if (strncmp(buf, "Event list", strlen("Event list")) == 0)
		{
			ret = read_event_list(i, fp, dec_point_is_comma);
		}
		else if (strncmp(buf, "Time (sec)", strlen("Time (sec)")) == 0)
		{
			ret = get_channel_info(i, buf);
		}
		if (ret != 0)
			goto error;
	}

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

		} while (fgets(buf, 2000, fp));
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


double
get_samplerate(FILE *fp, int dec_point_is_comma)
{
	long pos_save;
	char buf[2000];
	double samplerate = -1;
	double time1, time2;

	pos_save = ftell(fp);

	while (fgets(buf, 2000, fp))
	{
		if ((buf[0] >= '0') && (buf[0] <= '9'))
			break;
	}
	if (feof(fp))
		goto error;

	time1 = ascii_to_double(buf, dec_point_is_comma);
	
	fgets(buf, 2000, fp);
	if (feof(fp))
		goto error;
	time2 = ascii_to_double(buf, dec_point_is_comma);

	/* assuming that time is in seconds (TODO: check if test if times are in sec is necessary) */
	samplerate = 1 / (time2 - time1);

error:
	fseek(fp, pos_save, SEEK_SET);

	return samplerate;
} /* get_samplerate() */


long
get_start_su(FILE *fp, int dec_point_is_comma, double samplerate)
{
	long pos_save;
	char buf[2000];
	long start_sample = 0;
	double first_time;

	pos_save = ftell(fp);

	while (fgets(buf, 2000, fp))
	{
		if ((buf[0] >= '0') && (buf[0] <= '9'))
			break;
	}
	if (feof(fp))
		goto error;

	first_time = ascii_to_double(buf, dec_point_is_comma);
	start_sample = (long)(first_time * samplerate);

error:
	fseek(fp, pos_save, SEEK_SET);

	return start_sample;
} /* get_start_su() */


void
calc_rec_start_ts(struct info *i)
{
	double curr_ms;

	curr_ms = (i->start_sample_su / i->samplerate) * 1000;

	/* days */
	i->ts_rec_start.day = (short)(curr_ms / (1000 * 60 * 60 * 24));
	curr_ms -= ((double)i->ts_rec_start.day * (1000 * 60 * 60 * 24));
	/* hours */
	i->ts_rec_start.hour = (short)(curr_ms / (1000 * 60 * 60));
	curr_ms -= ((double)i->ts_rec_start.hour * (1000 * 60 * 60));
	/* minutes */
	i->ts_rec_start.min = (short)(curr_ms / (1000 * 60));
	curr_ms -= ((double)i->ts_rec_start.min * (1000 * 60));
	/* seconds */
	i->ts_rec_start.sec = (short)(curr_ms / (1000));
	curr_ms -= ((double)i->ts_rec_start.sec * (1000));
	/* milli-seconds */
	i->ts_rec_start.msec = (short)curr_ms;
} /* calc_rec_start_ts() */


double
ascii_to_double(char *buf, int dec_point_is_comma)
{
	double ret;
	char *c;

	if (dec_point_is_comma)
	{
		c = strchr(buf, '.');
		if (c)
			*c = ',';
	}
	else
	{
		c = strchr(buf, ',');
		if (c)
			*c = '.';
	}

	ret = atof(buf);

	return ret;
}  /* ascii_to_double() */


int
read_event_list(struct info *i, FILE *fp, int dec_point_is_comma)
{
	int ret = 0;
	char buf[2000];
	char *p;
	double start, end;

	if (i->events)
	{
		free(i->events);
		i->events = NULL;
		i->num_event_entries = 0;
	}

	while (fgets(buf, 2000, fp))
	{
		if ((buf[0] == '\n') || (buf[0] == '\r'))
			break;

		p = buf;

		/* so far, only comments are processed */
		if (strncmp(p, "Comment", strlen("Comment")) != 0)
			continue;

		p = strchr(p, '\t');
		if (!p)
			continue;
		start = ascii_to_double(p+1, dec_point_is_comma);
		start *= i->samplerate;

		p = strchr(p+1, '\t');
		if (!p)
			continue;
		end = ascii_to_double(p+1, dec_point_is_comma);
		end *= i->samplerate;
		end += start;

		p = strchr(p+1, '\t');
		if (!p)
			continue;
	
		i->num_event_entries++;
		i->events = realloc(i->events, sizeof(struct event_entry) * i->num_event_entries);

		i->events[i->num_event_entries-1].start_su = (long)start - i->start_sample_su;
		i->events[i->num_event_entries-1].end_su = (long)end - i->start_sample_su;

		strncpy(i->events[i->num_event_entries-1].text, p+1, 1023);
		i->events[i->num_event_entries-1].text[1023] = '\0';

		/* check if there are newline or carrige-return characters and remove them */
		p = strchr(i->events[i->num_event_entries-1].text, '\r');
		if (p)
			*p = '\0';
		p = strchr(i->events[i->num_event_entries-1].text, '\n');
		if (p)
			*p = '\0';
	}

	return ret;
}  /* read_event_list() */


int
get_channel_info(struct info *i, char *buf)
{
	char *p = buf;
	char *next;
	char *c;

	if (i->ci)
	{
		free(i->ci);
		i->ci = NULL;
		i->num_ch = 0;
	}

	/* skip time column */
	p = strchr(p, '\t');
	p++;
	do
	{
		next = strchr(p, '\t');
		if (next)
		{
			*next = '\0';
			next++;
		}

		i->num_ch++;
		i->ci = realloc(i->ci, sizeof(struct ch_info) * i->num_ch);

		c = strrchr(p, ')');
		if (c)
		{
			*c = '\0';
			c = strrchr(p, '(');
			if (c)
			{
				strcpy(i->ci[i->num_ch-1].unit, c+1);
				if (*(c-1) == ' ')
					*(c-1) = '\0';
			}
		}

		strcpy(i->ci[i->num_ch-1].name, p);

		i->ci[i->num_ch-1].bit_res = pow(2,16);

		p = next;
	} while (p);

	return 0;
}  /* get_ch_info() */


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

	_ra_set_error(h, RA_ERR_UNKNOWN_INFO, "plugin-ideeq-ascii");

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
		_ra_set_error(h, RA_ERR_ERROR, "plugin-ideeq-ascii");
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
		_ra_set_error(i->ra, RA_ERR_OUT_OF_RANGE, "plugin-ideeq-ascii");
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
		ra_value_set_string(vh, "IDEEQ recording (text-file)");
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
		ra_value_set_double(vh, 4.8828);
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
	case RA_INFO_REC_CH_CENTER_SAMPLE_D:
		ra_value_set_double(vh, pow(2, 15));
		break;
	case RA_INFO_REC_CH_MM_UNIT_D:
		ra_value_set_double(vh, i->ch_type_infos[ch].mm_unit);
		break;
	case RA_INFO_REC_CH_TYPE_L:
		ra_value_set_long(vh, i->ch_type_infos[ch].type);
		break;
	default:
		_ra_set_error(i->ra, RA_ERR_UNKNOWN_INFO, "plugin-ideeq-ascii");
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
		_ra_set_error(i->ra, RA_ERR_UNKNOWN_INFO, "plugin-ideeq-ascii");
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
	if ((ch < 0) || (ch >= i->num_ch) || (start >= (size_t)i->num_samples)
		|| ((data == NULL) && (data_raw == NULL)))
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
		double center_value = pow(2, 15);

		for (j = 0; j < (int)n_use; j++)
			data_raw[j] = (DWORD)((i->samples[ch][start+j] * 4.8828) + center_value);
	}

	return n_use;
} /* read_raw() */


int
pl_get_eval_info(meas_handle mh, value_handle name, value_handle desc, value_handle add_ts,
		 value_handle modify_ts, value_handle user, value_handle host, value_handle prog)
{
	struct info *i;
	char t[MAX_PATH_RA];
	
	if (!mh)
		return RA_ERR_ERROR;
	i = ((struct ra_meas *)mh)->priv;

	ra_value_set_string(name, "original");
	ra_value_set_string(desc, "Evaluation stored in IDEEQ file(s)");

	sprintf(t, "%02d.%02d.%d %02d:%02d:%02d", i->ts_rec_start.day,
		i->ts_rec_start.mon, i->ts_rec_start.year, i->ts_rec_start.hour,
		i->ts_rec_start.min, i->ts_rec_start.sec);

	ra_value_set_string(add_ts, t);
	ra_value_set_string(modify_ts, t);

	ra_value_reset(user);
	ra_value_reset(host);
	ra_value_set_string(prog, "");

	return 0;
} /* pl_get_eval_info() */


long
pl_get_class_num(meas_handle mh)
{
	int num = 0;
	struct info *i = ((struct ra_meas *) mh)->priv;

	if (i->num_event_entries > 0)
		num += 1;  /* annotations */

	return num;
} /* pl_get_class_num() */


int
pl_get_class_info(meas_handle mh, unsigned long class_num, value_handle id, value_handle name, value_handle desc)
{
	int ret;
	unsigned long n;

	n = (unsigned long)pl_get_class_num(mh);
	if (class_num > n)
	{
		_ra_set_error(mh, RA_ERR_OUT_OF_RANGE, "plugin-ideeq-ascii");
		return -1;
	}

	ra_value_set_string(id, "annotation");
	ret = fill_predef_class_info_ascii("annotation", name, desc);

	return ret;
} /* pl_get_class_info() */


long
pl_get_prop_num(class_handle clh, unsigned long class_num)
{
	struct eval_class *c = (struct eval_class *)clh;

	if (class_num)
		;

	if (c->id == EVENT_CLASS_ANNOT)
		return 1;  /* text of the annotation */
	
	_ra_set_error(clh, RA_ERR_OUT_OF_RANGE, "plugin-ideeq-ascii");

	return -1;
} /* pl_get_prop_num() */


int
pl_get_prop_info(class_handle clh, unsigned long class_num, unsigned long prop_num, value_handle id,
				 value_handle type, value_handle len,
		 value_handle name, value_handle desc, value_handle unit,
		 value_handle use_minmax, value_handle min, value_handle max,
		 value_handle has_ign_value, value_handle ign_value)
{
	int ret = -1;
	struct eval_class *c = (struct eval_class *)clh;

	if (class_num)
		;

	if ((c->id == EVENT_CLASS_ANNOT) && (prop_num == 0))
	{
		ra_value_set_string(id, "annotation");
		ret = fill_predef_prop_info_ascii("annotation", type, len, name, desc, unit,
						  use_minmax, min, max, has_ign_value, ign_value);
	}
	else
		_ra_set_error(clh, RA_ERR_OUT_OF_RANGE, "plugin-ideeq-ascii");

	return ret;
} /* pl_get_prop_info() */


int
pl_get_ev_info(class_handle clh, unsigned long class_num, value_handle start, value_handle end)
{
	int ret = 0;
	long l, *start_su, *end_su;
	struct eval_class *c = (struct eval_class *)clh;
	struct info *i = ((struct ra_meas *)c->meas)->priv;

	if (class_num)
		;

	if (c->id == EVENT_CLASS_ANNOT)
	{
		start_su = (long *)malloc(sizeof(long) * i->num_event_entries);
		end_su = (long *)malloc(sizeof(long) * i->num_event_entries);

		for (l = 0; l < i->num_event_entries; l++)
		{
			start_su[l] = i->events[l].start_su;
			end_su[l] = i->events[l].end_su;
		}
		ra_value_set_long_array(start, start_su, i->num_event_entries);
		ra_value_set_long_array(end, end_su, i->num_event_entries);
		
		free(start_su);
		free(end_su);
	}
	else
	{
		_ra_set_error(clh, RA_ERR_OUT_OF_RANGE, "plugin-ideeq-ascii");
		ret = -1;
	}

	return ret;
} /* pl_get_ev_start_end() */


int
pl_get_ev_value(prop_handle ph, unsigned long class_num, unsigned long prop_num, long event, value_handle value, value_handle ch)
{
	int ret = 0;
	value_handle *vh;
	long n, ch_num[1];
	struct eval_property *p = (struct eval_property *)ph;
	struct info *i = ((struct ra_meas *)p->meas)->priv;
	struct eval_class *c = (struct eval_class *)p->evclass;

	if (class_num || prop_num)
		;

	n = 0;
	switch(c->id)
	{
	case EVENT_CLASS_ANNOT:
		n = i->num_event_entries;
		break;
	}

	if (event >= n)
	{
		_ra_set_error(i->ra, RA_ERR_OUT_OF_RANGE, "plugin-ideeq-ascii");
		return -1;
	}

	vh = (value_handle *)malloc(sizeof(value_handle));
	vh[0] = ra_value_malloc();
	ch_num[0] = -1;
	switch (p->id)
	{
	case EVENT_PROP_ANNOT:
		ra_value_set_string(vh[0], i->events[event].text);
		ch_num[0] = -1;
		break;
	default:
		_ra_set_error(i->ra, RA_ERR_OUT_OF_RANGE, "plugin-mit/bih");
		ret = -1;
		break;
	}

	ra_value_set_vh_array(value, vh, 1);

	ra_value_set_long_array(ch, ch_num, 1);

	ra_value_free(vh[0]);
	free(vh);

	return ret;
} /* pl_get_ev_value() */


int
pl_get_ev_value_all(prop_handle ph, unsigned long class_num, unsigned long prop_num, value_handle value, value_handle ch)
{
	int ret = 0;
	struct eval_property *p = (struct eval_property *)ph;
	struct info *i = ((struct ra_meas *)p->meas)->priv;
	struct eval_class *c = (struct eval_class *)p->evclass;

	if (class_num || prop_num)
		;

	switch(c->id)
	{
	case EVENT_CLASS_ANNOT:
		ret = get_all_annot_events(p, i, value, ch);
		break;
	}

	return ret;
} /* pl_get_ev_value_all() */


int
get_all_annot_events(struct eval_property *p, struct info *i, value_handle value, value_handle ch)
{
	char **v;
	long *ch_num, l;

	if (p->id != EVENT_PROP_ANNOT)
	{
		_ra_set_error(i->ra, RA_ERR_OUT_OF_RANGE, "plugin-ideeq-ascii");
		return -1;
	}

	v = malloc(sizeof(char **) * i->num_event_entries);
	ch_num = malloc(sizeof(long) * i->num_event_entries);

	for (l = 0; l < i->num_event_entries; l++)
	{
		v[l] = malloc(sizeof(char) * (strlen(i->events[l].text) + 1));
		strcpy(v[l], i->events[l].text);
		ch_num[l] = -1;
	}

	ra_value_set_string_array(value, (const char **)v, i->num_event_entries);
	ra_value_set_long_array(ch, ch_num, i->num_event_entries);

	for (l = 0; l < i->num_event_entries; l++)
		free(v[l]);       
	free(v);
	free(ch_num);

	return 0;
} /* get_all_beat_events() */

