/*----------------------------------------------------------------------------
 * mortara_sparc.c  -  Handle ECGs stored in the (Mortara) SPARC format
 *
 * Description:
 * The plugin provides access to ECGs stored in the (Mortara) SPARC format.
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
#include <math.h>

#define _LIBRASCH_BUILD
#include <ra.h>
#include <ra_priv.h>
#include <handle_plugin.h>

#include <ra_endian.h>
#include <pl_general.h>
#include <ra_file.h>
#include <ra_estimate_ch_infos.h>

#include "mortara_sparc.h"


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
	struct info *i;
	FILE *fp = NULL;
	char fn[MAX_PATH_RA];

	i = malloc(sizeof(struct info));
	if (!i)
		goto wrong;
	memset(i, 0, sizeof(struct info));

	/* split file in dir and filename w/o extension */
	split_filename(name, SEPARATOR, i->dir, i->name, i->ext);
	if (i->ext == NULL)
		strncpy(i->ext, "xle", MAX_PATH_RA);
	sprintf(fn, "%s%c%s.%s", i->dir, SEPARATOR, i->name, i->ext);
	fp = ra_fopen(fn, "rb", 0);
	if (fp == NULL)
		goto wrong;

	if (fread(&(i->hdr), sizeof(struct hdr_struct), 1, fp) != 1)
		goto wrong;

	/* chars are little endian stored */
	if (strncmp((char *)i->hdr.name, "aFimyln ma:e", strlen("Family name:")) != 0)
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
	return find_meas(dir, "*.xle", PLUGIN_NAME, f, pl_check_meas);
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
	if (i->ext == NULL)
		strncpy(i->ext, "xle", MAX_PATH_RA);

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

	if (i->samples_orig)
		free(i->samples_orig);
	if (i->samples)
		free(i->samples);
 	if (i->samples_scaled)
 		free(i->samples_scaled);

	free(i);
} /* free_info_struct() */



int
get_data(struct info *i, int fast)
{
	int ret = -1;
	char file[MAX_PATH_RA];
	FILE *fp;
	int j;
	long l, offset, offset_new;
	unsigned long *samples_temp = NULL;
	double scale;

	i->samplerate = 1000.0;

	sprintf(file, "%s%c%s.%s", i->dir, SEPARATOR, i->name, i->ext);
	fp = ra_fopen(file, "rb", 0);
	if (fp == NULL)
	{
		_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-adi-ascii");
		goto error;
	}

	if (fread(&(i->hdr), sizeof(struct hdr_struct), 1, fp) != 1)
		goto error;

	if (get_pat_info(i) != 0)
		goto error;
	if (get_times(i) != 0)
		goto error;

	if (!fast)
	{
		fseek(fp, 0, SEEK_END);
		i->num_samples = (ftell(fp) - (4 * 65536)) / (2 * NUM_CH_REC);
		if (i->num_samples <= 0)
			goto error;
		i->samples_orig = malloc(sizeof(short) * i->num_samples * NUM_CH_REC);

		if (fseek(fp, (4 * 65536), SEEK_SET) != 0)
			goto error;
		if (fread(i->samples_orig, sizeof(short), (i->num_samples * NUM_CH_REC), fp) != (size_t)(i->num_samples * NUM_CH_REC))
			goto error;

		/* calc sample-values which should be used (calc standard leads) */
		samples_temp = malloc(sizeof(unsigned long) * i->num_samples * NUM_CH_REC);
		for (j = 0; j < NUM_CH_REC; j++)
		{
			int above_upper_limit = 0;
			unsigned long last_value;
			offset = 0;

			last_value = i->samples_orig[j];
			for (l = 0; l < i->num_samples; l++)
			{
				unsigned long sample = i->samples_orig[offset + j];

				if (abs(sample - last_value) > 0xf000)
					above_upper_limit = (above_upper_limit + 1) % 2;
				last_value = sample;

				if (above_upper_limit)
					sample += 0x0000ffff;
				
				samples_temp[offset+j] = sample;

				offset += NUM_CH_REC;
			}		
		}

		i->samples_scaled = malloc(sizeof(double) * i->num_samples * NUM_CH);
		i->samples = malloc(sizeof(short) * i->num_samples * NUM_CH);
		scale = 15.0 / 16.0;
		offset_new = 0;
		offset = 0;
		for (l = 0; l < i->num_samples; l++)
		{
			double sub;
			short subs;

			/* leads I, II, III[=II-I] */
			i->samples_scaled[offset_new] = (double)samples_temp[offset] * scale;
			i->samples[offset_new] = (short)samples_temp[offset];

			i->samples_scaled[offset_new+1] = (double)samples_temp[offset+1] * scale;
			i->samples[offset_new+1] = (short)samples_temp[offset+1];

			i->samples_scaled[offset_new+2] = ((double)samples_temp[offset+1] - ((double)samples_temp[offset])) * scale;
			i->samples[offset_new+2] = (short)(samples_temp[offset+1] - samples_temp[offset]);

			/* leads -aVR[=(I+II)/2], aVL[=(I-III)/2], aVF[=(II+III)/2] */
			i->samples_scaled[offset_new+3] = (i->samples_scaled[offset_new] + i->samples_scaled[offset_new+1]) / 2.0;
			i->samples[offset_new+3] = (short)((i->samples[offset_new] + i->samples[offset_new+1]) / 2);

			i->samples_scaled[offset_new+4] = (i->samples_scaled[offset_new] - i->samples_scaled[offset_new+2]) / 2.0;
			i->samples[offset_new+4] = (short)((i->samples[offset_new] - i->samples[offset_new+2]) / 2);

			i->samples_scaled[offset_new+5] = (i->samples_scaled[offset_new+1] + i->samples_scaled[offset_new+2]) / 2.0;
			i->samples[offset_new+5] = (short)((i->samples[offset_new+1] + i->samples[offset_new+2]) / 2);

			/* leads V1-V6 */
			sub = (i->samples_scaled[offset_new] + i->samples_scaled[offset_new+1]) / 3.0;
			subs = (short)((i->samples[offset_new] + i->samples[offset_new+1]) / 3);

			i->samples_scaled[offset_new+6] = ((double)samples_temp[offset+2] *scale) - sub;
			i->samples[offset_new+6] = (short)samples_temp[offset+2] - subs;

			i->samples_scaled[offset_new+7] = ((double)samples_temp[offset+3] *scale) - sub;
			i->samples[offset_new+7] = (short)samples_temp[offset+3] - subs;

			i->samples_scaled[offset_new+8] = ((double)samples_temp[offset+4] *scale) - sub;
			i->samples[offset_new+8] = (short)samples_temp[offset+4] - subs;

			i->samples_scaled[offset_new+9] = ((double)samples_temp[offset+5] *scale) - sub;
			i->samples[offset_new+9] = (short)samples_temp[offset+5] - subs;

			i->samples_scaled[offset_new+10] = ((double)samples_temp[offset+6] *scale) - sub;
			i->samples[offset_new+10] = (short)samples_temp[offset+6] - subs;

			i->samples_scaled[offset_new+11] = ((double)samples_temp[offset+7] *scale) - sub;
			i->samples[offset_new+11] = (short)samples_temp[offset+7] - subs;

			offset_new += NUM_CH;
			offset += NUM_CH_REC;
		}

		/* set channel infos */
		memset(i->ch_type_infos, 0, sizeof(struct ra_est_ch_infos) * NUM_CH);
		for (j = 0; j < NUM_CH; j++)
		{
			memcpy(i->ch_type_infos + j, _inf, sizeof(struct ra_est_ch_infos));
			i->ch_type_infos[j].mm_unit = 0.01;
			i->ch_type_infos[j].type = RA_CH_TYPE_ECG;
		}
		i->mm_sec = _inf[0].mm_sec;
		strcpy(i->ci[0].name, "I");
		strcpy(i->ci[0].unit, "mV");
		strcpy(i->ci[1].name, "II");
		strcpy(i->ci[1].unit, "mV");
		strcpy(i->ci[2].name, "III");
		strcpy(i->ci[2].unit, "mV");
		strcpy(i->ci[3].name, "-aVR");
		strcpy(i->ci[3].unit, "mV");
		strcpy(i->ci[4].name, "aVL");
		strcpy(i->ci[4].unit, "mV");
		strcpy(i->ci[5].name, "aVF");
		strcpy(i->ci[5].unit, "mV");
		strcpy(i->ci[6].name, "V1");
		strcpy(i->ci[6].unit, "mV");
		strcpy(i->ci[7].name, "V2");
		strcpy(i->ci[7].unit, "mV");
		strcpy(i->ci[8].name, "V3");
		strcpy(i->ci[8].unit, "mV");
		strcpy(i->ci[9].name, "V4");
		strcpy(i->ci[9].unit, "mV");
		strcpy(i->ci[10].name, "V5");
		strcpy(i->ci[10].unit, "mV");
		strcpy(i->ci[11].name, "V6");
		strcpy(i->ci[11].unit, "mV");
	}  /* !fast */

	ret = 0;

 error:
	if (fp)
		fclose(fp);

	if (samples_temp)
		free(samples_temp);

	return ret;
} /* get_data() */


int
get_pat_info(struct info *i)
{
	if (extract_string((const char *)(i->hdr.forename), i->pi.forename) != 0)
		return -1;
	if (extract_string((const char *)(i->hdr.name), i->pi.name) != 0)
		return -1;
	if (extract_string((const char *)(i->hdr.id), i->pi.id) != 0)
		return -1;
	if (extract_string((const char *)(i->hdr.dob), i->pi.dob) != 0)
		return -1;
	if (extract_string((const char *)(i->hdr.gender), i->pi.gender) != 0)
		return -1;
	if (extract_string((const char *)(i->hdr.history), i->pi.history) != 0)
		return -1;
	if (extract_string((const char *)(i->hdr.comment), i->pi.comment) != 0)
		return -1;

	return 0;
} /* get_pat_info() */


int
extract_string(const char *src, char *dest)
{
	char *p, buf[41];
	int j;

	if ((src == NULL) || (dest == NULL))
		return -1;

	memset(dest, 0, PAT_INFO_LEN);

	memcpy(buf, src, 40);
	buf[40] = '\0';
	for (j = 0; j < 20; j++)
	{
		char t = buf[j*2];
		buf[j*2] = buf[j*2 + 1];
		buf[j*2+1] = t;
	}

	p = strchr(buf, ':');
	if (p == NULL)
		return -1;
	p++;
	while (*p == ' ')
		p++;
	if ((*p == '\0') || ((p - src) >= PAT_INFO_LEN))
		return 0;

	strncpy(dest, p, PAT_INFO_LEN-1);

	return 0;
} /* extract_string() */


int
get_times(struct info *i)
{
	int j;
	char buf[9];
	
	memcpy(buf, i->hdr.rec_date, sizeof(char) * 8);
	for (j = 0; j < 4; j++)
	{
		char c = buf[j*2];
		buf[j*2] = buf[j*2+1];
		buf[j*2+1] = c;
	}
	buf[8] = '\0';

	if (strchr(buf, '.'))
	{
		/* MM.DD.YY format */
		i->rec_start.mon = (short)atoi(buf);
		i->rec_start.day = (short)atoi(buf+3);
		i->rec_start.year = (short)atoi(buf+6);
	}
	else
	{
		/* DD/MM/YY format */
		i->rec_start.day = (short)atoi(buf);
		i->rec_start.mon = (short)atoi(buf+3);
		i->rec_start.year = (short)atoi(buf+6);
	}
	if (i->rec_start.year > 90)
		i->rec_start.year += 1900;
	else
		i->rec_start.year += 2000;

	memcpy(buf, i->hdr.rec_time, sizeof(char) * 5);
	for (j = 0; j < 3; j++)
	{
		char c = buf[j*2];
		buf[j*2] = buf[j*2+1];
		buf[j*2+1] = c;
	}
	buf[5] = '\0';
	i->rec_start.hour = (short)atoi(buf);
	i->rec_start.min = (short)atoi(buf+3);
	i->rec_start.sec = i->rec_start.msec = 0;

	return 0;
} /* get_times() */


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
	    ((ch < 0) || (ch >= NUM_CH)))
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
		ra_value_set_string(vh, "Mortara SPARC");
		break;
	case RA_INFO_REC_GEN_TIME_C:
 		sprintf(t, "%02d:%02d:%02d", i->rec_start.hour,
 			i->rec_start.min, i->rec_start.sec);
 		ra_value_set_string(vh, t);
		break;
	case RA_INFO_REC_GEN_DATE_C:
 		sprintf(t, "%02d.%02d.%d", i->rec_start.day,
 			i->rec_start.mon, i->rec_start.year);
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
		ra_value_set_long(vh, NUM_CH);
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
		ra_value_set_long(vh, RA_CH_SAMPLE_TYPE_WORD);
		break;
	case RA_INFO_REC_CH_UNIT_C:
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

	switch (id)
	{
	case RA_INFO_OBJ_PERSON_NAME_C:
		ra_value_set_string_utf8(vh, i->pi.name);
		break;
	case RA_INFO_OBJ_PERSON_FORENAME_C:
		ra_value_set_string_utf8(vh, i->pi.forename);
		break;
	case RA_INFO_OBJ_PATIENT_ID_C:
		ra_value_set_string_utf8(vh, i->pi.id);
		break;
	case RA_INFO_OBJ_PERSON_BIRTHDAY_C:
		ra_value_set_string(vh, i->pi.dob);
		break;
	case RA_INFO_OBJ_PERSON_GENDER_C:
		ra_value_set_string_utf8(vh, i->pi.gender);
		break;
	case RA_INFO_OBJ_PATIENT_REASON_C:
		ra_value_set_string_utf8(vh, i->pi.history);
		break;
	case RA_INFO_OBJ_PERSON_COMMENT_C:
		ra_value_set_string_utf8(vh, i->pi.comment);
		break;
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
	size_t n_use;
	size_t offset;

	if (start >= (size_t)(i->num_samples))
		return 0;

	n_use = num;
	if ((start+n_use) >= (size_t)i->num_samples)
		n_use = i->num_samples - start;
	if (n_use < 0)
		return 0;

	offset = start * NUM_CH;
	for (j = 0; j < (int)n_use; j++)
	{
		if (data)
			data[j] = i->samples_scaled[offset + ch];
 		if (data_raw)
			data_raw[j] = (DWORD)i->samples[offset + ch];

		offset += NUM_CH;
	}

	return n_use;
} /* read_raw() */

