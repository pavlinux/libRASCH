/*----------------------------------------------------------------------------
 * portapres.c  -  Handle signals recorded with Portapres-System
 *
 * Description:
 * The plugin provides access to blood-pressure data recorded with the Portapres
 * system.
 *
 * Remarks:
 * Up to now only a small subset of the binary-file can be read. It is planned to
 * add support to access the data exported with BeatScope.
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002-2007, Raphael Schneider
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
#include <pl_general.h>
#include <ra_file.h>
#include <ra_estimate_ch_infos.h>
#include "portapres.h"


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

	ps->access.open_meas = pl_open_meas;
	ps->access.close_meas = pl_close_meas;

	ps->access.open_meas = pl_open_meas;
	ps->access.close_meas = pl_close_meas;

	ps->access.get_info_id = pl_get_info_id;
	ps->access.get_info_idx = pl_get_info_idx;

	ps->access.get_raw = pl_get_raw;
	ps->access.get_raw_unit = pl_get_raw_unit;

	return 0;
} /* init_plugin() */


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
	char file[MAX_PATH_RA];
	BYTE buf[4];

	i = malloc(sizeof(struct info));
	if (!i)
		goto error_open;
	memset(i, 0, sizeof(struct info));

	/* split file in dir and filename w/o extension */
	split_filename(name, SEPARATOR, i->dir, i->name, i->ext);

	if (i->ext[0] == '\0')
		strcpy(i->ext, ".dat");

	sprintf(file, "%s%c%s.%s", i->dir, SEPARATOR, i->name, i->ext);
	fp = ra_fopen(file, "r", 0);
	if (fp == NULL)
		goto error_open;
	if (fread(buf, 1, 4, fp) != 4)
		goto error_open;
	/* check if first 4 bytes are aaaa 5555 (this seems to be equal in all Portapres files) */
	if ((buf[0] != 0xaa) || (buf[1] != 0xaa) || (buf[2] != 0x55) || (buf[3] != 0x55))
		goto error_open;

	fclose(fp);
	free(i);

	return 1;

 error_open:
	if (i)
		free(i);
	if (fp)
		fclose(fp);

	return 0;
} /* pl_check_meas() */



meas_handle
pl_open_meas(ra_handle ra, const char *name, int fast)
{
	struct ra_meas *meas = NULL;
	struct info *i = NULL;
	FILE *fp = NULL;
	char fn[MAX_PATH_RA];
	char mask[MAX_PATH_RA];

	/* check if signal can be handled with this module */
	if (!pl_check_meas(name))
		goto error_open;

	if ((meas = ra_alloc_mem(sizeof(struct ra_meas))) == NULL)
		goto error_open;
	memset(meas, 0, sizeof(struct ra_meas));
	if ((i = calloc(1, sizeof(struct info))) == NULL)
		goto error_open;
	meas->priv = i;
	i->ra = ra;

	/* split file in dir and filename w/o extension */
	split_filename(name, SEPARATOR, i->dir, i->name, i->ext);

	/* check if ART measurement */
	sprintf(fn, "%s%cart_info.txt", i->dir, SEPARATOR);
	fp = ra_fopen(fn, "r", 1);
	if (fp)
	{
		i->art = 1;
		fclose(fp);
	}

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

	if (meas)
		ra_free_mem(meas);

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

	FREE_INFO(i->ri.sig);
	FREE_INFO(i->ri.unknown);
	FREE_INFO(i->ri.name);
	FREE_INFO(i->ri.type);
	FREE_INFO(i->ri.pres_site);
	FREE_INFO(i->ri.pres_lvls);
	FREE_INFO(i->ri.RTF_cuf);
	FREE_INFO(i->ri.co_method);
	FREE_INFO(i->ri.config);

	FREE_INFO(i->pi.id);
	FREE_INFO(i->pi.name);
	FREE_INFO(i->pi.procedure);
	FREE_INFO(i->pi.info_4);
	FREE_INFO(i->pi.info_5);
	FREE_INFO(i->pi.info_6);
	FREE_INFO(i->pi.info_7);
	FREE_INFO(i->pi.info_8);
	FREE_INFO(i->pi.info_9);
	FREE_INFO(i->pi.info_10);

	FREE_INFO(i->bi);

	if (i->files)
	{
		for (j = 0; j < i->num_files; j++)
		{
			if (i->files[j])
				ra_free_mem(i->files[j]);
		}
		ra_free_mem(i->files);
	}

	if (i->cd)
		free(i->cd);

	if (i->ch_type_infos)
		free(i->ch_type_infos);

	free(i);
} /* free_info_struct() */


int
get_data(struct info *i, int fast)
{
	int ret = 0;

	/* read infos from M2-Portapres-file */
	if ((ret = read_m2_file(i)) != 0)
		return ret;

	/* read header-file (created by BeatScope) */
	if ((ret = read_h00_file(i)) != 0)
		return ret;

	if (!fast)
	{
		/* read beat2beat-file (created by BeatScope) */
		if ((ret = read_b2b_file(i)) != 0)
			return ret;

		/* get raw-file infos (created by BeatScope) */
		if ((ret = read_raw_file(i)) != 0)
			return ret;
	}

	/* try to get channel infos (at the moment only when part of ART) */
	if (i->art)
	{
		char file_wo_ext[MAX_PATH_RA];
		int j;

		i->ch_type_infos = malloc(sizeof(struct ra_est_ch_infos) * i->num_channels);
		i->mm_sec = 1;
		sprintf(file_wo_ext, "%s%c%s", i->dir, SEPARATOR, i->name);
		for (j = 0; j < i->num_channels; j++)
		{
			if (ra_est_ch_type(i->ra, i->dir, file_wo_ext, 0, i->cd[j].name, &(i->ch_type_infos[j])) == 0)
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
				if (i->cd[j].unit[0] == 'u')
					i->ch_type_infos[j].mm_unit /= 1000.0;
			}
			
			if (i->mm_sec < i->ch_type_infos[j].mm_sec)
				i->mm_sec = i->ch_type_infos[j].mm_sec;
		}
	}

	return ret;
} /* get_data() */


int
read_m2_file(struct info *i)
{
	int ret = 0;

	if (i)
		;

	/* TODO: implement functionality */

	return ret;
} /* read_m2_file() */


int
read_h00_file(struct info *i)
{
	int ret = 0;

	if (i)
		;

	/* TODO: implement functionality */

	return ret;
} /* read_h00_file() */


int
read_b2b_file(struct info *i)
{
	int ret = 0;

	if (i)
		;

	/* TODO: implement functionality */

	return ret;
} /* read_b2b_file() */


int
read_raw_file(struct info *i)
{
	FILE *fp;

	/* at the moment only support for data exported for the ART project */
	if (!i->art)
		return 0;

	/* disable support for Portapres raw data for the moment */
	return 0;

	sprintf(i->raw_data_file, "%s%cPortapres_raw.adc", i->dir, SEPARATOR);
	fp = ra_fopen(i->raw_data_file, "r", 0);
	if (!fp)
		return 0;

	i->raw_data_avail = 1;
	i->num_channels = 1; /* use only the finger pressure, ignore the height channel */

	i->cd = malloc(sizeof(struct channel_desc) * i->num_channels);
	strncpy(i->cd[0].name, "ABP-Portapres", MAX_NAME_LEN);
	strncpy(i->cd[0].unit, "mmHg", MAX_NAME_LEN);
	i->cd[0].amp_res = 4.0; /* resolution is 0.25 mmHg */
	i->cd[0].samplerate = 99.7;

	fseek(fp, 0, SEEK_END);
	i->cd[0].num_samples = ftell(fp);
	i->cd[0].num_samples /= 4; /* channels, 2 bytes per channel */

	fclose(fp);

	return 0;
} /* read_raw_file() */


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

	_ra_set_error(h, RA_ERR_UNKNOWN_INFO, "plugin-portapres");

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
		_ra_set_error(h, RA_ERR_ERROR, "plugin-portapres");
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
	long ch;
	double d;
	char t[MAX_PATH_RA];

	ch = ra_value_get_number(vh);
	if ((id >= RA_INFO_REC_CHANNEL) && (id < RA_INFO_REC_CHANNEL_END) &&
	    ((ch < 0) || (ch >= i->num_channels)))
	{
		_ra_set_error(i->ra, RA_ERR_OUT_OF_RANGE, "plugin-portapres");
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
		ra_value_set_string(vh, "Portapres signal");
		break;
/*	case RA_INFO_REC_GEN_TIME_C:
		sprintf (t, "%02d:%02d:%02d", i->hdr.rec_start.hour,
			 i->hdr.rec_start.min, i->hdr.rec_start.sec);
		ra_value_set_string (mi, t);
		break;
	case RA_INFO_REC_GEN_DATE_C:
		sprintf (t, "%02d.%02d.%d", i->hdr.rec_start.day,
			 i->hdr.rec_start.mon, i->hdr.rec_start.year);
		minfo_string (mi, t);
		break;
*/
	case RA_INFO_REC_GEN_DURATION_SEC_D:
		if (i->num_channels > 0)
		{
			d = (double)i->cd[0].num_samples / i->cd[0].samplerate;
			ra_value_set_double(vh, d);
		}
		else
			ra_value_set_double(vh, 0.0);
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
		sprintf(t, "%s%c%s", i->dir, SEPARATOR, i->name);
		ra_value_set_string(vh, t);
		break;
	case RA_INFO_REC_GEN_DIR_L:
		ra_value_set_long(vh, 0);
		break;
	case RA_INFO_REC_CH_NAME_C:
		ra_value_set_string(vh, i->cd[ch].name);
		break;
	case RA_INFO_REC_CH_DESC_C:
		ra_value_set_string(vh, "");
		break;
	case RA_INFO_REC_CH_NUM_SAMPLES_L:
		ra_value_set_long(vh, i->cd[ch].num_samples);
		break;
	case RA_INFO_REC_CH_SAMPLERATE_D:
		ra_value_set_double(vh, i->cd[ch].samplerate);
		break;
	case RA_INFO_REC_CH_NUM_BITS_L:
		ra_value_set_long(vh, 8);
		break;
	case RA_INFO_REC_CH_AMP_RESOLUTION_D:
		ra_value_set_double(vh, i->cd[ch].amp_res);
		break;
	case RA_INFO_REC_CH_UNIT_C:
		ra_value_set_string(vh, i->cd[ch].unit);
		break;
	default:
		_ra_set_error(i->ra, RA_ERR_UNKNOWN_INFO, "plugin-portapres");
		ret = -1;
		break;
	}

	return ret;
} /* get_info_rec() */


int
get_info_obj(struct info *i, int id, value_handle vh)
{
	int ret = 0;

	if (vh)
		;

	switch (id)
	{
	default:
		_ra_set_error(i->ra, RA_ERR_UNKNOWN_INFO, "plugin-portapres");
		ret = -1;
		break;
	}

	return ret;
} /* get_info_obj() */


int
pl_set_info(meas_handle mh, value_handle vh)
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
	FILE *fp;
	char fn[MAX_PATH_RA];
	long offset;
	size_t num_to_read, num_read, num_avail, l, curr_pos;
	short *buf;
	size_t buf_num_elem;

	/* at the moment only raw-file exported for ART is supported
	   (using a priori information about the file structure) */
	if (!i->raw_data_avail || !i->art || (ch != 0))
		return 0;

	sprintf(fn, "%s%cPortapres_raw.adc", i->dir, SEPARATOR);
	fp = ra_fopen(fn, "r", 0);
	if (!fp)
		return 0;

	offset = (long)start * 4;
	fseek(fp, offset, SEEK_SET);

	num_to_read = num;
	if ((start+num) > (size_t)i->cd[ch].num_samples)
		num_to_read = i->cd[ch].num_samples - start;

	buf_num_elem = num_to_read * 2;
	buf = malloc(sizeof(short) * buf_num_elem);
	num_read = fread(buf, 4, num_to_read, fp);
	fclose(fp);

	num_avail = 0;
	curr_pos = 0;
	for (l = 0; num_read; l++)
	{
		if ((l >= num) || (curr_pos >= buf_num_elem))
			break;

		if (data)
			data[l] = (double)buf[curr_pos] / i->cd[ch].amp_res;
		else
			data_raw[l] = buf[curr_pos];
		num_avail++;

		curr_pos += 2;
	}
	free(buf);

	return num_avail;
} /* read_raw() */
