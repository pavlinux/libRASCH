/*----------------------------------------------------------------------------
 * ctg.c  -  Handle cardiotocogram signals exported from GMT system
 *
 * Description:
 * The plugin provides access to CTG's exported from the GMT system. The exported raw data
 * (three files: fetal heart rate child 1, fetal heart rate child 2 and uterine contraction) is
 * packed in a directory with the extension '.ctg'. Additionally some information about the 
 * mother and the recording is stored in the file 'ctg_info.txt'.
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2003-2007, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define _LIBRASCH_BUILD
#include <ra.h>
#include <ra_priv.h>
#include <handle_plugin.h>

#include <ra_endian.h>
#include <pl_general.h>
#include <ra_file.h>
#include <ra_estimate_ch_infos.h>

#include "ctg.h"



LIBRAAPI int
load_ra_plugin(struct plugin_struct *ps)
{
	strcpy(ps->info.name, PLUGIN_NAME);
	strcpy(ps->info.desc, PLUGIN_DESC);
	sprintf(ps->info.build_ts, "%s  %s", __DATE__, __TIME__);
	ps->info.type = PLUGIN_TYPE;
	ps->info.license = PLUGIN_LICENSE;
	sprintf(ps->info.version, PLUGIN_VERSION);

	ps->info.num_results = 0;
	ps->info.num_options = 0;

	ps->access.check_meas = pl_check_meas;
	ps->access.find_meas = pl_find_meas;

	ps->access.open_meas = pl_open_meas;
	ps->access.close_meas = pl_close_meas;

	ps->access.get_info_id = pl_get_info_id;
	ps->access.get_info_idx = pl_get_info_idx;

	ps->access.set_info = pl_set_info;

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

	i = malloc(sizeof(struct info));
	if (!i)
		goto error_open;
	memset(i, 0, sizeof(struct info));

	/* split file in dir and filename w/o extension */
	split_filename(name, SEPARATOR, i->dir, i->name, i->ext);
	if (RA_STRICMP(i->ext, "ctg") != 0)
		goto error_open;

	if (!check_file(i, "ctg1krs.dat"))
		goto error_open;
	if (!check_file(i, "ctg2krs.dat"))
		goto error_open;
	if (!check_file(i, "ctgukrs.dat"))
		goto error_open;
	if (!check_file(i, "ctg_info.txt"))
		goto error_open;

	ret = 1;

 error_open:
	if (i)
		free(i);

	return ret;
} /* pl_check_meas() */


int
check_file(struct info *i, const char *file)
{
	int ret = 0;
	FILE *fp;
	char fn[MAX_PATH_RA];

	sprintf(fn, "%s%c%s.%s%c%s", i->dir, SEPARATOR, i->name, i->ext, SEPARATOR, file);
	fp = ra_fopen(fn, "r", 1);
	if (fp)
	{
		fclose(fp);
		ret = 1;
	}

	return ret;
} /* check_file() */


int
pl_find_meas(const char *dir, struct find_meas *f)
{
	return find_meas(dir, "*.ctg", PLUGIN_NAME, f, pl_check_meas);
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

	sprintf(mask, "%s%c%s.%s%c", i->dir, SEPARATOR, i->name, i->ext, SEPARATOR);
	find_files(mask, "*", &(i->files), &(i->num_files), &(i->size));

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

	if (i->ch_type_infos)
		free(i->ch_type_infos);

	if (i->pi.name)
		free(i->pi.name);
	if (i->pi.forename)
		free(i->pi.forename);
	if (i->pi.birthday)
		free(i->pi.birthday);
	if (i->pi.gender)
		free(i->pi.gender);
	if (i->pi.gestation_date)
		free(i->pi.gestation_date);

	if (i->files)
	{
		for (j = 0; j < i->num_files; j++)
		{
			if (i->files[j])
				ra_free_mem(i->files[j]);
		}
		ra_free_mem(i->files);
	}

	free(i);
} /* free_info_struct() */


int
get_data(struct info *i)
{
	char file[MAX_PATH_RA];
	char file_wo_ext[MAX_PATH_RA];
	FILE *fp;
	int j;

	sprintf(file, "%s%c%s.%s%cctg1krs.dat", i->dir, SEPARATOR, i->name, i->ext, SEPARATOR);
	fp = ra_fopen(file, "rb", 1);
	if (fp == NULL)
		goto error;
	fseek(fp, 0, SEEK_END);
	i->ci[0].num_samples = ftell(fp);
	fclose(fp);
	/* we know that the fetal heart rate is saved 4 times per second */
	i->ci[0].samplerate = 4.0;
	strcpy(i->ci[0].name, "FHR1");
	strcpy(i->ci[0].desc, "fetal heart rate child #1");
	strcpy(i->ci[0].unit, "bpm");
	i->ci[0].min_value = 80;
	i->ci[0].max_value = 200;

	sprintf(file, "%s%c%s.%s%cctg1krs.dat", i->dir, SEPARATOR, i->name, i->ext, SEPARATOR);
	fp = ra_fopen(file, "rb", 1);
	if (fp == NULL)
		goto error;
	fseek(fp, 0, SEEK_END);
	i->ci[1].num_samples = ftell(fp);
	fclose(fp);
	i->ci[1].samplerate = 4.0;
	strcpy(i->ci[1].name, "FHR2");
	strcpy(i->ci[1].desc, "fetal heart rate child #2");
	strcpy(i->ci[1].unit, "bpm");
	i->ci[1].min_value = 80;
	i->ci[1].max_value = 200;

	sprintf(file, "%s%c%s.%s%cctgukrs.dat", i->dir, SEPARATOR, i->name, i->ext, SEPARATOR);
	fp = ra_fopen(file, "rb", 1);
	if (fp == NULL)
		goto error;
	fseek(fp, 0, SEEK_END);
	i->ci[2].num_samples = ftell(fp);
	fclose(fp);
	/* we know that the uterine contraction is saved once per second */
	i->ci[2].samplerate = 1.0;
	strcpy(i->ci[2].name, "UC");
	strcpy(i->ci[2].desc, "uterine contractions");
	strcpy(i->ci[2].unit, "mmHg");
	i->ci[2].min_value = 0;
	i->ci[2].max_value = 100;

	i->num_channels = 3;

	if (read_patinfo(i) != 0)
		goto error;

	/* try to get channel infos */
	i->ch_type_infos = malloc(sizeof(struct ra_est_ch_infos) * i->num_channels);
	i->mm_sec = -1;
	sprintf(file_wo_ext, "%s%c%s%c%s", i->dir, SEPARATOR, i->name, SEPARATOR, i->ext);
	for (j = 0; j < i->num_channels; j++)
	{
		if (ra_est_ch_type(i->ra, i->dir, file_wo_ext, 1, i->ci[j].name, &(i->ch_type_infos[j])) == 0)
		{
			i->ch_type_infos[j].type = RA_CH_TYPE_UNKNOWN;
			i->ch_type_infos[j].mm_sec = 1;
			i->ch_type_infos[j].mm_unit = 1;
			i->ch_type_infos[j].offset = 0;
			i->ch_type_infos[j].centered = 0;
			i->ch_type_infos[j].center_value = 0;
		}
		else
			if (i->mm_sec < i->ch_type_infos[j].mm_sec)
				i->mm_sec = i->ch_type_infos[j].mm_sec;
	}
	/* if no channel-info was found, use 1 mm/sec */
	if (i->mm_sec == -1)
		i->mm_sec = 1;

	return 0;

error:
	_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-ctg-rasch");

	return -1;
} /* get_data() */


int
read_patinfo(struct info *i)
{
	char fn[MAX_PATH_RA];
	char buf[200];
	size_t pos;
	FILE *fp;

	sprintf(fn, "%s%c%s.%s%cctg_info.txt", i->dir, SEPARATOR, i->name, i->ext, SEPARATOR);
	fp = ra_fopen(fn, "r", 1);
	if (!fp)
	{
		_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-ctg-rasch");
		return -1;
	}

	do
	{
		fgets(buf, 200, fp);

		/* kill \n or \r */
		for (pos = strlen(buf) - 1; pos >= 0; pos--)
		{
			if ((buf[pos] == '\n') || (buf[pos] == '\r'))
				buf[pos] = '\0';
			else
				break;
		}

		if (strncmp(buf, "name", 4) == 0)
		{
			size_t len = strlen(buf) - 4;
			i->pi.name = malloc(len*2);
			strcpy(i->pi.name, &(buf[5]));
			to_utf8_inplace(i->pi.name, len*2, "ISO8859-1");
		}
		else if (strncmp(buf, "forename", 8) == 0)
		{
			size_t len = strlen(buf) - 8;
			i->pi.forename = malloc(len*2);
			strcpy(i->pi.forename, &(buf[9]));
			to_utf8_inplace(i->pi.forename, len*2, "ISO8859-1");
		}
		else if (strncmp(buf, "birthday", 8) == 0)
		{
			size_t len = strlen(buf) - 8;
			i->pi.birthday = malloc(len*2);
			strcpy(i->pi.birthday, &(buf[9]));
			to_utf8_inplace(i->pi.birthday, len*2, "ISO8859-1");
		}
		else if (strncmp(buf, "gender", 6) == 0)
		{
			size_t len = strlen(buf) - 6;
			i->pi.gender = malloc(len*2);
			strcpy(i->pi.gender, &(buf[7]));
			to_utf8_inplace(i->pi.gender, len*2, "ISO8859-1");
		}
		else if (strncmp(buf, "num_fetus", 9) == 0)
		{
			i->pi.num_fetus = atol(&(buf[10]));
		}
		else if (strncmp(buf, "rec_ts", 6) == 0)
		{
			split_timestamp(i, &(buf[7]));
		}
		else if (strncmp(buf, "gestation_date", 14) == 0)
		{
			size_t len = strlen(buf) - 14;
			i->pi.gestation_date = malloc(len*2);
			strcpy(i->pi.gestation_date, &(buf[15]));
			to_utf8_inplace(i->pi.gestation_date, len*2, "ISO8859-1");
		}
	}
	while (!feof(fp));

	fclose(fp);

	return 0;
}				/* read_patinfo() */


/* expect a string with the format hh:mm:ss  dd.mm.yyyy */
/* TODO: remove expectation of format */
void
split_timestamp(struct info *i, char *string)
{
	if (strlen(string) < 20)
		return;

	i->ri.hour = atoi(string);
	i->ri.min = atoi(string+3);
	i->ri.sec = atoi(string+6);
	i->ri.day = atoi(string+10);
	i->ri.month = atoi(string+13);
	i->ri.year = atoi(string+16);
} /* split_timestamp() */


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
		ra_value_set_string_array(vh, (const char **)(i->files), i->num_files);
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

	_ra_set_error(h, RA_ERR_UNKNOWN_INFO, "plugin-ctg-rasch");

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
		_ra_set_error(h, RA_ERR_ERROR, "plugin-ctg-rasch");
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

	if ((id >= RA_INFO_REC_CHANNEL_START) && (id < RA_INFO_REC_CHANNEL_END) &&
	    ((ch < 0) || (ch >= i->num_channels)))
	{
		_ra_set_error(i->ra, RA_ERR_OUT_OF_RANGE, "plugin-ctg-rasch");
		return -1;
	}

	switch (id)
	{
	case RA_INFO_REC_GEN_TYPE_L:
		ra_value_set_long(vh, RA_REC_TYPE_TIMESERIES);
		break;
	case RA_INFO_REC_GEN_NAME_C:
		ra_value_set_string_utf8(vh, i->name);
		break;
	case RA_INFO_REC_GEN_DESC_C:
		ra_value_set_string(vh, "CTG signal");
		break;
	case RA_INFO_REC_GEN_TIME_C:
		sprintf(t, "%02d:%02d:%02d", i->ri.hour,
			i->ri.min, i->ri.sec);
		ra_value_set_string(vh, t);
		ra_value_info_set_modifiable(vh, 1);
		break;
	case RA_INFO_REC_GEN_DATE_C:
		sprintf(t, "%02d.%02d.%d", i->ri.day,
			i->ri.month, i->ri.year);
		ra_value_set_string(vh, t);
		ra_value_info_set_modifiable(vh, 1);
		break;
	case RA_INFO_REC_GEN_DURATION_SEC_D:
		d = ((double) i->ci[0].num_samples / i->ci[0].samplerate);
		ra_value_set_double(vh, d);
		break;
	case RA_INFO_REC_GEN_NUMBER_L:
		ra_value_set_long(vh, 0);
		break;
	case RA_INFO_REC_GEN_NUM_SUB_REC_L:
		ra_value_set_long(vh, 0);
		break;
	case RA_INFO_REC_GEN_NUM_CHANNEL_L:
		ra_value_set_long(vh, i->num_channels);
		break;
	case RA_INFO_REC_GEN_OFFSET_L:
		ra_value_set_long(vh, 0);
		break;
	case RA_INFO_REC_GEN_PATH_C:
		sprintf(t, "%s%c%s.%s", i->dir, SEPARATOR, i->name, i->ext);
		ra_value_set_string(vh, t);
		break;
	case RA_INFO_REC_GEN_DIR_L:
		ra_value_set_long(vh, 1);
		break;
	case RA_INFO_REC_GEN_MM_SEC_D:
		ra_value_set_double(vh, i->mm_sec);
		break;
	case RA_INFO_REC_CH_NAME_C:
		ra_value_set_string_utf8(vh, i->ci[ch].name);
		break;
	case RA_INFO_REC_CH_DESC_C:
		ra_value_set_string_utf8(vh, i->ci[ch].desc);
		break;
	case RA_INFO_REC_CH_NUM_SAMPLES_L:
		ra_value_set_long(vh, i->ci[ch].num_samples);
		break;
	case RA_INFO_REC_CH_SAMPLERATE_D:
		ra_value_set_double(vh, i->ci[ch].samplerate);
		break;
	case RA_INFO_REC_CH_NUM_BITS_L:
		ra_value_set_long(vh, 8);
		break;
	case RA_INFO_REC_CH_AMP_RESOLUTION_D:
		ra_value_set_double(vh, 1.0);	/* values are stored as actual values */
		break;
	case RA_INFO_REC_CH_SAMPLE_TYPE_L:
		ra_value_set_long(vh, RA_CH_SAMPLE_TYPE_INT);
		break;
	case RA_INFO_REC_CH_UNIT_C:
		ra_value_set_string_utf8(vh, i->ci[ch].unit);
		break;
	case RA_INFO_REC_CH_MIN_UNIT_D:
		ra_value_set_double(vh, i->ci[ch].min_value);
		break;
	case RA_INFO_REC_CH_MAX_UNIT_D:
		ra_value_set_double(vh, i->ci[ch].max_value);
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
	case RA_INFO_REC_CH_DO_IGNORE_VALUE_L:
		if (i->ch_type_infos[ch].type == RA_CH_TYPE_CTG_FHR)
			ra_value_set_long(vh, 1);
		else
			ra_value_set_long(vh, 0);
		break;
	case RA_INFO_REC_CH_IGNORE_VALUE_D:
		/* because only FHR has values to ignore, just set inf to 0.0 */
		ra_value_set_double(vh, 0.0);
		break;
	default:
		_ra_set_error(i->ra, RA_ERR_UNKNOWN_INFO, "plugin-ctg-rasch");
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
		set_utf8(vh, i->pi.name);
		ra_value_info_set_modifiable(vh, 1);
		break;
	case RA_INFO_OBJ_PERSON_FORENAME_C:
		set_utf8(vh, i->pi.forename);
		ra_value_info_set_modifiable(vh, 1);
		break;
	case RA_INFO_OBJ_PERSON_BIRTHDAY_C:
		set_utf8(vh, i->pi.birthday);
		ra_value_info_set_modifiable(vh, 1);
		break;
	case RA_INFO_OBJ_PERSON_GENDER_C: /* should be always female; but perhaps ??? */
		set_utf8(vh, i->pi.gender);
		ra_value_info_set_modifiable(vh, 1);
		break;
	case RA_INFO_OBJ_PREG_WOMAN_NUM_FETUS_L:
		ra_value_set_long(vh, i->pi.num_fetus);
		ra_value_info_set_modifiable(vh, 1);
		break;
	case RA_INFO_OBJ_PREG_WOMAN_GESTATION_DATE_C:
		set_utf8(vh, i->pi.gestation_date);
		ra_value_info_set_modifiable(vh, 1);
		break;
	default:
		_ra_set_error(i->ra, RA_ERR_UNKNOWN_INFO, "plugin-ctg-rasch");
		ret = -1;
		break;
	}

	return ret;
} /* get_info_obj() */


int
set_utf8(value_handle vh, const char *string)
{
	char *buf;
	size_t len;

	len = strlen(string) * 2;
	buf = calloc(len, sizeof(char));
	strcpy(buf, string);
	to_utf8_inplace(buf, len, "ISO8859-1");
	ra_value_set_string_utf8(vh, buf);
	free(buf);

	return 0;
} /* set_utf8*/


int
pl_set_info(any_handle h, int id, value_handle vh)
{
	int ret = 0;
	meas_handle mh;
	struct info *i;
	int id_use;

	if (!h)
		return RA_ERR_ERROR;
	if ((mh = ra_meas_handle_from_any_handle(h)) == NULL)
		return RA_ERR_ERROR;

	i = ((struct ra_meas *) mh)->priv;

	if (id == -1)
		id_use = ra_value_get_info(vh);
	else
		id_use = id;
	switch (id_use)
	{
	case RA_INFO_OBJ_PERSON_NAME_C:
		ret = write_single_info(i, &(i->pi.name), vh);
		break;
	case RA_INFO_OBJ_PERSON_FORENAME_C:
		ret = write_single_info(i, &(i->pi.forename), vh);
		break;
	case RA_INFO_OBJ_PERSON_BIRTHDAY_C:
		ret = write_single_info(i, &(i->pi.birthday), vh);
		break;
	case RA_INFO_OBJ_PERSON_GENDER_C:
		ret = write_single_info(i, &(i->pi.gender), vh);
		break;
	case RA_INFO_OBJ_PREG_WOMAN_NUM_FETUS_L:
		i->pi.num_fetus = ra_value_get_long(vh);
		ret = write_single_info(i, NULL, vh);
		break;
	case RA_INFO_OBJ_PREG_WOMAN_GESTATION_DATE_C:
		ret = write_single_info(i, &(i->pi.gestation_date), vh);
		break;
	case RA_INFO_REC_GEN_TIME_C:
		if ((ret = parse_time_or_date(ra_value_get_string(vh), &(i->ri.hour), &(i->ri.min), &(i->ri.sec))) == 0)
			ret = write_single_info(i, NULL, vh);
		break;		
	case RA_INFO_REC_GEN_DATE_C:
		if ((ret = parse_time_or_date(ra_value_get_string(vh), &(i->ri.day), &(i->ri.month), &(i->ri.year))) == 0)
			ret = write_single_info(i, NULL, vh);
		break;		
	default:
		_ra_set_error(i->ra, RA_ERR_UNSUPPORTED, "plugin-ctg-rasch");
		ret = -1;
		break;
	}
	
	return ret;
} /* pl_set_info() */


int
parse_time_or_date(const char *string, int *part1, int *part2, int *part3)
{
	const char *p, *curr;
	int v[3], curr_val;
	
	if (!string || !part1 || !part2 || !part3)
		return -1;
	
	curr = string;
	curr_val = 0;
	while (curr[0] != '\0')
	{
		v[curr_val++] = atoi(curr);
		if (curr_val >= 3)
			break;
		
		p = curr;
		while ((*p != '\0') && (*p >= '0') && (*p <= '9'))
			p++;
		if (p == curr)
			break;
		curr = p + 1;
	}
	
	if (curr_val < 3)
		return -1;
	
	*part1 = v[0];
	*part2 = v[1];
	*part3 = v[2];
	
	return 0;
} /* parse_time_or_date() */


int
write_single_info(struct info *i, char **store, value_handle data)
{
	const char *s;
	char fn[MAX_PATH_RA];
	FILE *fp;
	char *buf = NULL;
	
	if (store)
	{
		s = ra_value_get_string_utf8(data);
		if (s && (s[0] != '\0'))
		{
			*store = realloc(*store, strlen(s)+1);
			strcpy(*store, s);
		}
		else
		{
			free(*store);
			*store = NULL;
		}
	}

	/* write ctg_info.txt file */
	sprintf(fn, "%s%c%s.%s%cctg_info.txt", i->dir, SEPARATOR, i->name, i->ext, SEPARATOR);
	if ((fp = ra_fopen(fn, "w", 1)) == NULL)
	{
		_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-ctg-rasch");
		return -1;
	}

	buf = realloc(buf, strlen(i->pi.name)+1);
	strcpy(buf, i->pi.name);
	from_utf8_inplace(buf, strlen(i->pi.name)+1, "ISO8859-1");
	fprintf(fp, "name=%s\n", buf);

	buf = realloc(buf, strlen(i->pi.forename)+1);
	strcpy(buf, i->pi.forename);
	from_utf8_inplace(buf, strlen(i->pi.forename)+1, "ISO8859-1");
	fprintf(fp, "forename=%s\n", buf);

	buf = realloc(buf, strlen(i->pi.birthday)+1);
	strcpy(buf, i->pi.birthday);
	from_utf8_inplace(buf, strlen(i->pi.birthday)+1, "ISO8859-1");
	fprintf(fp, "birthday=%s\n", buf);

	buf = realloc(buf, strlen(i->pi.gender)+1);
	strcpy(buf, i->pi.gender);
	from_utf8_inplace(buf, strlen(i->pi.gender)+1, "ISO8859-1");
	fprintf(fp, "gender=%s\n", buf);

	fprintf(fp, "num_fetus=%d\n", i->pi.num_fetus);

	fprintf(fp, "rec_ts=%02d:%02d:%02d  %02d.%02d.%d\n", i->ri.hour, i->ri.min, i->ri.sec,
		i->ri.day, i->ri.month, i->ri.year);

	buf = realloc(buf, strlen(i->pi.gestation_date)+1);
	strcpy(buf, i->pi.gestation_date);
	from_utf8_inplace(buf, strlen(i->pi.gestation_date)+1, "ISO8859-1");
	fprintf(fp, "gestation_date=%s\n", buf);

	if (buf)
		free(buf);

	fclose(fp);

	return 0;
} /* write_single_info() */


size_t
pl_get_raw(meas_handle mh, rec_handle rh, int ch, size_t start, size_t num_data, DWORD *data, DWORD *data_high)
{
	struct info *i = ((struct ra_meas *) mh)->priv;
	size_t j, num = 0;
	BYTE *buf = NULL;

	if (rh)
		;

	buf = malloc(num_data);
	num = read_raw(i, ch, start, num_data, buf);
	for (j = 0; j < num; j++)
		data[j] = (DWORD)buf[j];
	free(buf);

	if (data_high)
	{
		for (j = 0; j < num; j++)
			data_high[j] = 0;
	}

	return num;
} /* pl_get_raw() */


size_t
pl_get_raw_unit(meas_handle mh, rec_handle rh, int ch, size_t start, size_t num_data, double *data)
{
	struct info *i = ((struct ra_meas *) mh)->priv;
	size_t j, num = 0;
	BYTE *buf = NULL;

	if (rh)
		;

	buf = malloc(num_data);
	num = read_raw(i, ch, start, num_data, buf);
	for (j = 0; j < num; j++)
		data[j] = (double)buf[j];
	free(buf);

	return num;
} /* pl_get_raw_unit() */


size_t
read_raw(struct info * i, int ch, size_t start, size_t num, BYTE *buf)
{
	char fn[MAX_PATH_RA];
	size_t num_ret = 0;
	FILE *fp;

	switch (ch)
	{
	case 0:
		sprintf(fn, "%s%c%s.%s%cctg1krs.dat", i->dir, SEPARATOR, i->name, i->ext, SEPARATOR);
		break;
	case 1:
		sprintf(fn, "%s%c%s.%s%cctg2krs.dat", i->dir, SEPARATOR, i->name, i->ext, SEPARATOR);
		break;
	case 2:
		sprintf(fn, "%s%c%s.%s%cctgukrs.dat", i->dir, SEPARATOR, i->name, i->ext, SEPARATOR);
		break;
	default:
		_ra_set_error(i->ra, RA_ERR_OUT_OF_RANGE, "plugin-ctg-rasch");
		return num_ret;
		break;
	}

	if ((fp = ra_fopen(fn, "rb", 1)) == NULL)
	{
		_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-ctg-rasch");
		return 0;
	}
	fseek(fp, (long)start, SEEK_SET);
	num_ret = fread(buf, 1, num, fp);
	fclose(fp);

	return num_ret;
} /* read_raw() */


