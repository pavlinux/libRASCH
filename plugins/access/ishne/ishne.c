/*----------------------------------------------------------------------------
 * ishne.c  -  Handle signals saved using ISHNE standard output for Holter ECG
 *
 * Description:
 * The plugin provides access to recordings saved in the 'ISHNE standard output
 * for Holter ECG' format.
 *
 * Author(s): Raphael Schneider (librasch@gmail.com)
 *
 * Copyright (C) 2002-2010, Raphael Schneider
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
#include <ra_ecg.h>
#include "ishne.h"



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
	ps->access.get_ev_value_all = pl_get_ev_value_all;

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
	char id[8];

	i = malloc(sizeof(struct info));
	if (!i)
		goto error_open;
	memset(i, 0, sizeof(struct info));

	/* split file in dir and filename w/o extension */
	split_filename(name, SEPARATOR, i->dir, i->name, i->ext);
	if (i->ext[0] == '\0')
		strcpy(i->ext, "ecg");

	sprintf(fn, "%s%c%s.%s", i->dir, SEPARATOR, i->name, i->ext);
	fp = ra_fopen(fn, "rb", 0);
	if (fp == NULL)
		goto error_open;
	if (fread(id, 1, 8, fp) != 8)
		goto error_open;
	/* check if id is ok; ignore version info */
	if (strncmp(id, "ISHNE1.0", 8) != 0)
		goto error_open;

	fclose(fp);
	fp = NULL;

	free(i);

	return 1;

 error_open:
	if (i)
		free(i);
	if (fp)
		fclose(fp);

	return 0;
} /* pl_check_meas() */


int
pl_find_meas(const char *dir, struct find_meas *f)
{
	return find_meas(dir, "*.ecg", PLUGIN_NAME, f, pl_check_meas);
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
	memset(i, 0, sizeof(struct info));
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

	free(i);
} /* free_info_struct() */



int
get_data(struct info *i)
{
	int ret = -1;
	char file[MAX_PATH_RA];
	FILE *fp;
	int n;
	WORD checksum, checksum_calc;
	BYTE *buf = NULL;
	size_t buf_size;

	sprintf(file, "%s%c%s.%s", i->dir, SEPARATOR, i->name, i->ext);
	fp = ra_fopen(file, "rb", 0);
	if (fp == NULL)
	{
		_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-ishne");
		goto error;
	}

	/* skip id */
	fseek(fp, 8, SEEK_SET);

	if (fread(&checksum, sizeof(WORD), 1, fp) != 1)
	{
		_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-ishne");
		goto error;
	}

	if (fread(&(i->hdr), sizeof(struct header), 1, fp) != 1)
	{
		_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-ishne");
		goto error;
	}

	/* save header */
	buf_size = sizeof(struct header);
	buf = malloc(buf_size);
	memcpy(buf, &(i->hdr), sizeof(struct header));

	/* handle endianess */
	le32_to_cpu(i->hdr.var_block_size);
	le32_to_cpu(i->hdr.num_samples);
	le32_to_cpu(i->hdr.var_block_offset);
	le32_to_cpu(i->hdr.ecg_offset);
	le16_to_cpu(i->hdr.version);
	le16_to_cpu(i->hdr.sex);
	le16_to_cpu(i->hdr.race);

	handle_endian_ishne_date(&(i->hdr.birthday));
	handle_endian_ishne_date(&(i->hdr.rec_date));
	handle_endian_ishne_date(&(i->hdr.output_date));
	handle_endian_ishne_time(&(i->hdr.rec_time));

	le16_to_cpu(i->hdr.birthday.day);
	le16_to_cpu(i->hdr.birthday.month);
	le16_to_cpu(i->hdr.birthday.year);

	le16_to_cpu(i->hdr.rec_date.day);
	le16_to_cpu(i->hdr.output_date.day);
	le16_to_cpu(i->hdr.rec_time.hour);

	le16_to_cpu(i->hdr.num_leads);
	le16_to_cpu(i->hdr.pacemaker_code);
	le16_to_cpu(i->hdr.samplerate);
	for (n = 0; n < 12; n++)
	{
		le16_to_cpu(i->hdr.lead_specification[n]);
		le16_to_cpu(i->hdr.lead_qualitay[n]);
		le16_to_cpu(i->hdr.amp_resolution[n]);
	}

	/* read variable sized block */
	if (i->hdr.var_block_size > 0)
	{
		i->manufacturer_specific = malloc(i->hdr.var_block_size);
		if (fread(i->manufacturer_specific, 1, i->hdr.var_block_size, fp) !=
		    (size_t)i->hdr.var_block_size)
		{
			_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-ishne");
			goto error;
		}
	}

	/* check if we have to handle a file exported with the broken ELA imeplemantion */
	if (strncmp(i->hdr.copyright, "ELA medical 1999", 16) == 0)
		i->broken_ela_format = 1;

	/* check checksum */
	if (i->hdr.var_block_size > 0)
	{
		buf_size += i->hdr.var_block_size;
		buf = realloc(buf, buf_size);
		memcpy(buf+sizeof(struct header), i->manufacturer_specific, i->hdr.var_block_size);
	}

	checksum_calc = calc_checksum(buf, buf_size);
	if ((i->broken_ela_format == 0) && (checksum != checksum_calc))
	{
		_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-ishne");
		goto error;
	}

	/* try to find rr-interval files */
	if (find_rri_files(i) != 0)
		goto error;

	ret = 0;

 error:
	if (fp)
		fclose(fp);
	if (buf)
		free(buf);
	
	return ret;
} /* get_data() */


void
handle_endian_ishne_date(struct ishne_date *d)
{
	le16_to_cpu(d->day);
	le16_to_cpu(d->month);
	le16_to_cpu(d->year);
}  /* handle_endian_ishne_date() */


void
handle_endian_ishne_time(struct ishne_time *t)
{
	short temp;

	temp = t->hour;
	le16_to_cpu(temp);
	t->hour = temp;

	temp = t->min;
	le16_to_cpu(temp);
	t->min = temp;

	temp = t->sec;
	le16_to_cpu(temp);
	t->sec = temp;
}  /* handle_endian_ishne_time() */


int
find_rri_files(struct info *i)
{
	int ret = 0;
	char fn[MAX_PATH_RA];
	FILE *fp;

	/* up to now only Ela-export files are supported */
	/* try to open a file with .txt as extension (normally used by Ela) */
	sprintf(fn, "%s%c%s.txt", i->dir, SEPARATOR, i->name);
	fp = ra_fopen(fn, "r", 0);
	if (fp == NULL)
		return 0;

	/* FIXME: make checks that this is really an Ela-file */
	ret = read_ela_rri(i, "txt");
	i->holter_system = SYS_ELA;

	return ret;
}  /* find_rri_files() */


int
read_ela_rri(struct info *i, char *ext)
{
	char file[MAX_PATH_RA];
	FILE *fp;
	char buf[255];  /* 255 char's should be enough, perhaps a dynamic solution can be
			   implemented (but has a very low priority) */
	int has_time = 0;
	long beat_info_size = 0;
	long curr_pos = 0;

	if (i->beats)
	{
		free(i->beats);
		i->beats = NULL;
		i->num_beats = 0;
	}

	sprintf(file, "%s%c%s.%s", i->dir, SEPARATOR, i->name, ext);
	fp = ra_fopen(file, "rb", 0);
	if (fp == NULL)
	{
		_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-ishne");
		return -1;
	}

	/* skip first two lines (data will be in ISHNE-file also) */
	fgets(buf, 255, fp);
	fgets(buf, 255, fp);

	/* try to estimate format used for the file
	   there are two formats, one with the position of the QRS-complex and
	   one only with RR-intervals */
	fgets(buf, 255, fp);
	if (buf[2] == ':')
		has_time = 1;

	do
	{
		char *rri_pos, *annot_pos;
		if (has_time)
			rri_pos = strchr(buf, '\t') + 1;
		else
			rri_pos = buf;

		annot_pos = strchr(rri_pos, '\t') + 1;

		if (beat_info_size <= i->num_beats)
		{
			beat_info_size += 10000;
			i->beats = realloc(i->beats, sizeof(struct beat_info) * beat_info_size);
		}
		
		curr_pos += atol(rri_pos);
		i->beats[i->num_beats].pos = curr_pos / i->hdr.samplerate;
		switch (*annot_pos)
		{
		case 'N':
			SET_CLASS(ECG_CLASS_SINUS, i->beats[i->num_beats].annotation);
			break;
		case 'V':
			SET_CLASS(ECG_CLASS_VENT, i->beats[i->num_beats].annotation);
			break;
		case 'P':  /* I think it is 'P', but I don't know */
			SET_CLASS(ECG_CLASS_PACED, i->beats[i->num_beats].annotation);
		case 'A':
		case 'C':
		default:
			SET_CLASS(ECG_CLASS_ARTIFACT, i->beats[i->num_beats].annotation);
			break;
		}

		i->num_beats++;
	} while (fgets(buf, 255, fp));

	/* resize beat-info-struct to actually used size */
	i->beats = realloc(i->beats, sizeof(struct beat_info) * i->num_beats);

	i->beats_ok = 1;

	fclose(fp);

	return 0;
} /* read_ela_rri() */


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
	    ((ch < 0) || (ch >= i->hdr.num_leads)))
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
		switch (i->holter_system)
		{
		case SYS_ELA:
			ra_value_set_string(vh, "Ela-ISHNE Holter");
			break;
		default:
			ra_value_set_string(vh, "ISHNE Holter");
			break;
		}
		break;
	case RA_INFO_REC_GEN_TIME_C:
		sprintf(t, "%02d:%02d:%02d", i->hdr.rec_time.hour,
			i->hdr.rec_time.min, i->hdr.rec_time.sec);
		ra_value_set_string(vh, t);
		break;
	case RA_INFO_REC_GEN_DATE_C:
		sprintf(t, "%02d.%02d.%d", i->hdr.rec_date.day,
			i->hdr.rec_date.month, i->hdr.rec_date.year);
		ra_value_set_string(vh, t);
		break;
	case RA_INFO_REC_GEN_DURATION_SEC_D:
		d = ((double) i->hdr.num_samples / (double) i->hdr.samplerate);
		ra_value_set_double(vh, d);
		break;
	case RA_INFO_REC_GEN_NUMBER_L:
		ra_value_set_long(vh, 0);
		break;
	case RA_INFO_REC_GEN_NUM_SUB_REC_L:
		ra_value_set_long(vh, 0);
		break;
	case RA_INFO_REC_GEN_NUM_CHANNEL_L:
		ra_value_set_long(vh, i->hdr.num_leads);
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
	case RA_INFO_REC_CH_NAME_C:
		sprintf(t, "ecg-ch%ld", ch + 1);
		ra_value_set_string(vh, t);
		break;
	case RA_INFO_REC_CH_DESC_C:
		ra_value_set_string(vh, "");
		break;
	case RA_INFO_REC_CH_NUM_SAMPLES_L:
		if (i->broken_ela_format)
			ra_value_set_long(vh, i->hdr.num_samples);
		else
			ra_value_set_long(vh, i->hdr.num_samples / i->hdr.num_leads);
		break;
	case RA_INFO_REC_CH_SAMPLERATE_D:
		ra_value_set_double(vh, (double) i->hdr.samplerate);
		break;
	case RA_INFO_REC_CH_NUM_BITS_L:
		ra_value_set_long(vh, 16);
		break;
	case RA_INFO_REC_CH_AMP_RESOLUTION_D:
		ra_value_set_double(vh, 1000000.0 / i->hdr.amp_resolution[ch]);
		break;
	case RA_INFO_REC_CH_SAMPLE_TYPE_L:
		ra_value_set_long(vh, RA_CH_SAMPLE_TYPE_INT);
		break;
	case RA_INFO_REC_CH_UNIT_C:
		ra_value_set_string(vh, "mV");
		break;
	case RA_INFO_REC_GEN_MM_SEC_D:
		ra_value_set_double(vh, 50.0);
		break;
	case RA_INFO_REC_CH_CENTER_VALUE_D:
		ra_value_set_double(vh, 0.0);
		break;
	case RA_INFO_REC_CH_CENTER_SAMPLE_D:
		ra_value_set_double(vh, 0.0);
		break;
	case RA_INFO_REC_CH_MM_UNIT_D:
		ra_value_set_double(vh, 10);
		break;
	case RA_INFO_REC_CH_TYPE_L:
		ra_value_set_long(vh, RA_CH_TYPE_ECG);
		break;
	default:
		_ra_set_error(i->ra, RA_ERR_UNKNOWN_INFO, "plugin-ishne");
		ret = -1;
		break;
	}

	return ret;
} /* get_info_rec() */


int
get_info_obj(struct info *i, int id, value_handle vh)
{
	char t[20];
	int ret = 0;

	switch (id)
	{
	case RA_INFO_OBJ_PATIENT_ID_C:
		ra_value_set_string(vh, i->hdr.subject_id);
		break;
	case RA_INFO_OBJ_PERSON_NAME_C:
		ra_value_set_string(vh, i->hdr.subject_name);
		break;
	case RA_INFO_OBJ_PERSON_FORENAME_C:
		ra_value_set_string(vh, i->hdr.subject_forename);
		break;
	case RA_INFO_OBJ_PERSON_BIRTHDAY_C:
		sprintf(t, "%02d.%02d.%d", i->hdr.birthday.day, i->hdr.birthday.month,
			i->hdr.birthday.year);
		ra_value_set_string(vh, t);
		break;
	case RA_INFO_OBJ_PERSON_GENDER_C:
		switch (i->hdr.sex)
		{
		case 1:
			ra_value_set_string(vh, "male");
			break;
		case 2:
			ra_value_set_string(vh, "female");
			break;
		case 0:
		default:
			ra_value_set_string(vh, "unknown");
			break;
		}
		break;
/*	case RA_INFO_OBJ_PERSON_RACE_C:
		switch (i->hdr.race)
		{
		case 1:
			ra_value_set_string(vh, "Caucasian");
			break;
		case 2:
			ra_value_set_string(vh, "Black");
			break;
		case 3:
			ra_value_set_string(vh, "Oriental");
			break;
		case 0:
		default:
			ra_value_set_string(vh, "unknown");
			break;
		}
		break;*/
	default:
		_ra_set_error(i->ra, RA_ERR_UNKNOWN_INFO, "plugin-ishne");
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
	char fn[MAX_PATH_RA];
	short *buf = NULL;
	size_t buf_size = 0;
	size_t num_ret = 0;
	FILE *fp = NULL;
	size_t n_use, data_start, offset;
	long samples_avail;
	
	n_use = num;
	if (i->broken_ela_format)
		samples_avail = i->hdr.num_samples;
	else
		samples_avail = i->hdr.num_samples / i->hdr.num_leads;

	if ((start+n_use) >= (size_t)samples_avail)
		n_use = samples_avail - start;
	if (n_use < 0)
		goto error;

	buf_size = n_use * i->hdr.num_leads;
	buf = malloc(sizeof(short) * buf_size);

	data_start = i->hdr.ecg_offset + (start * i->hdr.num_leads * sizeof(short));

	sprintf(fn, "%s%c%s.%s", i->dir, SEPARATOR, i->name, i->ext);
	if ((fp = ra_fopen(fn, "rb", 0)) == NULL)
	{
		_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-ishne");
		goto error;
	}

	fseek(fp, (long)data_start, SEEK_SET);
	if (fread(buf, sizeof(short), buf_size, fp) != buf_size)
	{
		_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-ishne");
		goto error;
	}

	offset = ch;
	for (j = 0; j < (int)n_use; j++)
	{
		if (data)
			data[j] = (double)buf[offset] * (i->hdr.amp_resolution[ch] / 1000000.0);
		if (data_raw)
			data_raw[j] = (DWORD)buf[offset];

		offset += i->hdr.num_leads;
	}
	num_ret = n_use;

 error:
	if (buf)
		free(buf);
	if (fp)
		fclose(fp);

	return num_ret;
} /* read_raw() */


WORD
calc_checksum(BYTE *buf, size_t size)
{
	size_t i;
	BYTE a, b;
	BYTE crc_hi, crc_lo;
	WORD checksum;

	crc_hi = crc_lo = 0xff;

	for (i = 0; i < size; i++)
	{
		a = buf[i];
		a = a ^ crc_hi;
		crc_hi = a;
		a = a >> 4;
		a = a ^ crc_hi;
		crc_hi = crc_lo;		
		crc_lo = a;
		a = (a << 4) | (a >> 4);
		b = a;
		a = (a << 1) | (a >> 7);
		a = a & 0x1f;
		crc_hi = a ^ crc_hi;
		a = b & 0xf0;
		crc_hi = a ^ crc_hi;
		b = (b << 1) | (b >> 7);
		b = b & 0xe0;
		crc_lo = b ^ crc_lo;
	}

	checksum = ((short)crc_hi << 8) | crc_lo;

	return checksum;
}  /* calc_checksum() */


int
pl_get_eval_info(meas_handle mh, value_handle name, value_handle desc, value_handle add_ts,
		 value_handle modify_ts, value_handle user, value_handle host, value_handle prog)
{
	struct info *i = ((struct ra_meas *) mh)->priv;
	char t[MAX_PATH_RA];

	if (!(i->beats_ok))
		get_data(i);

	if (!(i->beats_ok))
	{
		/* still no beats -> no rri file */
		return -1;
	}

	ra_value_set_string(name, "original");

	switch (i->holter_system)
	{
	case SYS_ELA:
		ra_value_set_string(desc, "Evaluation performed with Ela-System");
		break;
	default:
		ra_value_set_string(desc, "???");
		break;
	}

	sprintf(t, "%02d.%02d.%d %02d:%02d:%02d", i->hdr.rec_date.day,
		i->hdr.rec_date.month, i->hdr.rec_date.year, i->hdr.rec_time.hour,
		i->hdr.rec_time.min, i->hdr.rec_time.sec);

	ra_value_set_string(add_ts, t);
	ra_value_set_string(modify_ts, t);

	ra_value_reset(user);
	ra_value_reset(host);
	ra_value_reset(prog);

	return 0;
} /* pl_get_eval_info() */


long
pl_get_class_num(meas_handle mh)
{
	if (mh)
		;

	return 1; /* heartbeats */
} /* pl_get_class_num() */


int
pl_get_class_info(meas_handle mh, unsigned long class_num, value_handle id, value_handle name, value_handle desc)
{
	int ret;

	if (class_num > (unsigned long)pl_get_class_num(mh))
	{
		_ra_set_error(mh, RA_ERR_OUT_OF_RANGE, "plugin-ishne");
		return -1;
	}

	/* only one class, 'heartbeat' */
	ra_value_set_string(id, "heartbeat");
	ret = fill_predef_class_info_ascii("heartbeat", name, desc);

	return ret;
} /* pl_get_class_info() */


long
pl_get_prop_num(class_handle clh, unsigned long class_num)
{
	struct eval_class *c = (struct eval_class *)clh;

	if (class_num)
		;

	if (c->id == EVENT_CLASS_HEARTBEAT)
		return 2;  /* position and classification of qrs complex */
	
	_ra_set_error(clh, RA_ERR_OUT_OF_RANGE, "plugin-ishne");

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

	if (c->id != EVENT_CLASS_HEARTBEAT)
	{
		_ra_set_error(clh, RA_ERR_OUT_OF_RANGE, "plugin-ishne");
		return ret;
	}

	switch (prop_num)
	{
	case 0:		/* qrs position */
		ra_value_set_string(id, "qrs-pos");
		ret = fill_predef_prop_info_ascii("qrs-pos", type, len, name, desc, unit,
						  use_minmax, min, max, has_ign_value, ign_value);
		break;
	case 1:		/* qrs classification */
		ra_value_set_string(id, "qrs-annot");
		ret = fill_predef_prop_info_ascii("qrs-annot", type, len, name, desc, unit,
						  use_minmax, min, max, has_ign_value, ign_value);
		break;
	default:
		_ra_set_error(clh, RA_ERR_OUT_OF_RANGE, "plugin-ishne");
		ret = -1;
		break;
	}

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

	if (!(i->beats_ok))
		get_data(i);

	if (c->id != EVENT_CLASS_HEARTBEAT)
	{
		_ra_set_error(clh, RA_ERR_OUT_OF_RANGE, "plugin-ishne");
		return -1;
	}
	start_su = (long *)malloc(sizeof(long) * i->num_beats);
	end_su = (long *)malloc(sizeof(long) * i->num_beats);
	
	for (l = 0; l < i->num_beats; l++)
	{
		start_su[l] = (long)i->beats[l].pos;
		end_su[l] = start_su[l];
	}
	ra_value_set_long_array(start, start_su, i->num_beats);
	ra_value_set_long_array(end, end_su, i->num_beats);
	
	free(start_su);
	free(end_su);

	return ret;
} /* pl_get_ev_info() */


int
pl_get_ev_value(prop_handle ph, unsigned long class_num, unsigned long prop_num, long event, value_handle value, value_handle ch)
{
	int ret = 0;
	value_handle *vh;
	long ch_num[1];
	struct eval_property *p = (struct eval_property *)ph;
	struct info *i = ((struct ra_meas *)p->meas)->priv;
	struct eval_class *c = (struct eval_class *)p->evclass;

	if (prop_num || class_num)
		;

	if (!(i->beats_ok))
		get_data(i);

	if (c->id != EVENT_CLASS_HEARTBEAT)
	{
		_ra_set_error(i->ra, RA_ERR_OUT_OF_RANGE, "plugin-ishne");
		return -1;
	}

	if (event >= i->num_beats)
	{
		_ra_set_error(i->ra, RA_ERR_OUT_OF_RANGE, "plugin-ishne");
		return -1;
	}

	vh = (value_handle *)malloc(sizeof(value_handle));
	vh[0] = ra_value_malloc();
	ch_num[0] = -1;
	switch (p->id)
	{
	case EVENT_PROP_QRS_POS:
		ra_value_set_long(vh[0], i->beats[event].pos);
		break;
	case EVENT_PROP_QRS_ANNOT:
		ra_value_set_long(vh[0], i->beats[event].annotation);
		break;
	default:
		_ra_set_error(i->ra, RA_ERR_OUT_OF_RANGE, "plugin-ishne");
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
	long *v, *ch_num, l;

	if (prop_num || class_num)
		;

	if (c->id != EVENT_CLASS_HEARTBEAT)
	{
		_ra_set_error(i->ra, RA_ERR_OUT_OF_RANGE, "plugin-ishne");
		return -1;
	}

	v = malloc(sizeof(long) * i->num_beats);
	ch_num = malloc(sizeof(long) * i->num_beats);

	if (p->id == EVENT_PROP_QRS_POS)
	{
		for (l = 0; l < i->num_beats; l++)
		{
			v[l] = i->beats[l].pos;
			ch_num[l] = -1;
		}
	}
	else if (p->id == EVENT_PROP_QRS_ANNOT)
	{
		for (l = 0; l < i->num_beats; l++)
		{
			v[l] = i->beats[l].annotation;
			ch_num[l] = -1;
		}
	}
	else
	{
		_ra_set_error(i->ra, RA_ERR_OUT_OF_RANGE, "plugin-ishne");
		ret = -1;
	}

	ra_value_set_long_array(value, v, i->num_beats);
	ra_value_set_long_array(ch, ch_num, i->num_beats);

	free(v);
	free(ch_num);

	return ret;
} /* pl_get_ev_value_all() */

