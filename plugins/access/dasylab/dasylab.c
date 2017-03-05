/*----------------------------------------------------------------------------
 * dasylab.c  -  Handle signals recorded using DasyLab
 *
 * Description:
 * The plugin provides access to recordings saved in the format used by
 * the DasyLab-Systems.
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
#include <time.h>
#include <ctype.h>

#define _LIBRASCH_BUILD
#include <ra.h>
#include <ra_priv.h>
#include <handle_plugin.h>

#include <ra_endian.h>
#include <pl_general.h>
#include <ra_file.h>
#include <ra_estimate_ch_infos.h>
#include "dasylab.h"



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
	char id[4];

	i = malloc(sizeof(struct info));
	if (!i)
		goto error_open;
	memset(i, 0, sizeof(struct info));

	/* split file in dir and filename w/o extension */
	split_filename(name, SEPARATOR, i->dir, i->name, i->ext);
	if (i->ext[0] == '\0')
		strcpy(i->ext, "DDF");

	sprintf(fn, "%s%c%s.%s", i->dir, SEPARATOR, i->name, i->ext);
	fp = ra_fopen(fn, "rb", 0);
	if (fp == NULL)
		goto error_open;
	if (fread(id, 1, 4, fp) != 4)
		goto error_open;
	/* check if id is ok; ignore version info */
	if (strncmp(id, "DTDF", 4) != 0)
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
}/* pl_check_meas() */


int
pl_find_meas(const char *dir, struct find_meas *f)
{
	return find_meas(dir, "*.DDF", PLUGIN_NAME, f, pl_check_meas);
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
	if (i->ext[0] == '\0')
		strcpy(i->ext, "DDF");

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
	struct info *i = ((struct ra_meas *) mh)->priv;

	free_info_struct(i);

	return 0;
} /* close_meas() */


void
free_info_struct(struct info *i)
{
	int j;

	if (!i)
		return;

	if (i->ci.f0 != NULL)
		free(i->ci.f0);

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
	char buf[4];
	int ret;
	int n;

	sprintf(file, "%s%c%s.%s", i->dir, SEPARATOR, i->name, i->ext);
	fp = ra_fopen(file, "rb", 0);
	if (fp == NULL)
	{
		_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-dasylab");
		goto error;
	}

	/* check if file is complete (last 4 bytes in file must be "DTDF" */
	fseek(fp, -4, SEEK_END);
	fread(buf, 1, 4, fp);
	if (strncmp(buf, "DTDF", 4) != 0)
	{
		_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-dasylab");
		goto error;
	}
	fseek(fp, 0, SEEK_SET);

	if (read_global_header(i, fp) != 0)
		goto error;

	switch (i->hdr.file_format)
	{
	case 0:
		ret = read_format0(i, fp);
		break;
	case 1:
		ret = read_format1(i, fp);
		break;
	case 2:
		ret = read_format2(i, fp);
		break;
	default:
		_ra_set_error(i->ra, RA_ERR_UNSUPPORTED, "plugin-dasylab");
		ret = -1;
		break;
	}
	if (ret != 0)
		goto error;

	fclose(fp);

	/* try to get channel infos */
	i->ch_type_infos = malloc(sizeof(struct ra_est_ch_infos) * i->hdr.num_channels);
	i->mm_sec = 1;
	sprintf(file_wo_ext, "%s%c%s", i->dir, SEPARATOR, i->name);
	for (n = 0; n < i->hdr.num_channels; n++)
	{
		if ((i->hdr.file_format != 1)
		    || (ra_est_ch_type(i->ra, i->dir, file_wo_ext, 0, i->ci.f1[n].name, &(i->ch_type_infos[n])) == 0))
		{
			i->ch_type_infos[n].type = RA_CH_TYPE_UNKNOWN;
			i->ch_type_infos[n].mm_sec = 1;
			i->ch_type_infos[n].mm_unit = 1;
			i->ch_type_infos[n].offset = 0;
			i->ch_type_infos[n].centered = 0;
			i->ch_type_infos[n].center_value = 0;
		}
		if (i->mm_sec < i->ch_type_infos[n].mm_sec)
			i->mm_sec = i->ch_type_infos[n].mm_sec;
	}


	return 0;

 error:
	if (fp)
		fclose(fp);
	return -1;
} /* get_data() */


int
read_global_header(struct info *i, FILE * fp)
{
	char buf[MAX_INFO_SIZE];	/* seems to be enough */
	char value[MAX_INFO_SIZE];
	long p;

	fgets(buf, MAX_INFO_SIZE, fp);
	if (strncmp(buf, "DTDF", 4) != 0)
	{
		_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-dasylab");
		return -1;
	}

	/* find end of info-strings */
	while (buf[0] != 0x1a)
		fread(buf, 1, 1, fp);
	p = ftell(fp) + 2;	/* and skip CPU-id */
	fseek(fp, 5, SEEK_SET);

	while (ftell(fp) < p)
	{
		fgets(buf, MAX_INFO_SIZE, fp);
		if (strncmp(buf, "WORKSHEET", 9) == 0)
		{
			get_value(buf, value);
			strncpy(i->hdr.infos.worksheet, value, MAX_INFO_SIZE);
		}
		else if (strncmp(buf, "AUTHOR", 6) == 0)
		{
			get_value(buf, value);
			strncpy(i->hdr.infos.author, value, MAX_INFO_SIZE);
		}
		else if (strncmp(buf, "COMPANY", 7) == 0)
		{
			get_value(buf, value);
			strncpy(i->hdr.infos.company, value, MAX_INFO_SIZE);
		}
		else if (strncmp(buf, "DIVISON", 8) == 0)
		{
			get_value(buf, value);
			strncpy(i->hdr.infos.division, value, MAX_INFO_SIZE);
		}
		else if (strncmp(buf, "TITLE", 5) == 0)
		{
			get_value(buf, value);
			strncpy(i->hdr.infos.title, value, MAX_INFO_SIZE);
		}
		else if (strncmp(buf, "DATA FILE", 9) == 0)
		{
			get_value(buf, value);
			strncpy(i->hdr.infos.data_file, value, MAX_INFO_SIZE);
		}
	}
	fseek(fp, p, SEEK_SET);

	if (fread(&(i->hdr.hdr_size), sizeof(WORD), 1, fp) != 1)
	{
		_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-dasylab");
		return -1;
	}
	le16_to_cpu(i->hdr.hdr_size);

	if (fread(&(i->hdr.file_format), sizeof(WORD), 1, fp) != 1)
	{
		_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-dasylab");
		return -1;
	}
	le16_to_cpu(i->hdr.file_format);

	if (fread(&(i->hdr.file_version), sizeof(WORD), 1, fp) != 1)
	{
		_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-dasylab");
		return -1;
	}
	le16_to_cpu(i->hdr.file_version);

	if (fread(&(i->hdr.hdr2_size), sizeof(WORD), 1, fp) != 1)
	{
		_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-dasylab");
		return -1;
	}
	le16_to_cpu(i->hdr.hdr2_size);

	if (fread(&(i->hdr.ch_hdr_size), sizeof(WORD), 1, fp) != 1)
	{
		_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-dasylab");
		return -1;
	}
	le16_to_cpu(i->hdr.ch_hdr_size);

	if (fread(&(i->hdr.datablock_hdr_size), sizeof(WORD), 1, fp) != 1)
	{
		_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-dasylab");
		return -1;
	}
	le16_to_cpu(i->hdr.datablock_hdr_size);

	if (fread(&(i->hdr.use_extra_data_file), sizeof(WORD), 1, fp) != 1)
	{
		_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-dasylab");
		return -1;
	}
	le16_to_cpu(i->hdr.use_extra_data_file);

	if (fread(&(i->hdr.num_channels), sizeof(WORD), 1, fp) != 1)
	{
		_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-dasylab");
		return -1;
	}
	le16_to_cpu(i->hdr.num_channels);

	if (fread(&(i->hdr.samplerate), sizeof(double), 1, fp) != 1)
	{
		_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-dasylab");
		return -1;
	}
	i->hdr.samplerate = 1.0 / i->hdr.samplerate;

	if (fread(&(i->hdr.start_time), sizeof(DWORD), 1, fp) != 1)
	{
		_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-dasylab");
		return -1;
	}
	le32_to_cpu(i->hdr.start_time);

	i->hdr2_start = ftell(fp);

	return 0;
} /* read_global_header() */


void
get_value(char *buf, char *value)
{
	size_t l;
	size_t val_pos = 0;
	int found_equal = 0;
	int val_ok = 0;
	size_t len = strlen(buf);

	memset(value, 0, MAX_INFO_SIZE);

	for (l = 0; l < len; l++)
	{
		char c = buf[l];

		if ((c == '\r') || (c == '\n'))
			break;

		if (c == '=')
			found_equal = 1;

		if (found_equal && !val_ok && isalpha(c))
			val_ok = 1;

		if (val_ok)
		{
			if (val_pos >= (MAX_INFO_SIZE - 1))
				break;
			value[val_pos] = c;
			val_pos++;
			continue;
		}
	}
} /* get_value() */


int
read_format0(struct info *i, FILE * fp)
{
	if (i || fp)
		;

	return 0;
} /* read_format0() */


int
read_format1(struct info *i, FILE * fp)
{
	int n;
	int ret = 0;

	if (fread(&(i->hdr2.f1), sizeof(struct global_header_f1), 1, fp) != 1)
	{
		_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-dasylab");
		return -1;
	}
	i->ch_hdr_start = ftell(fp);

	i->ci.f1 = malloc(sizeof(struct ch_header_f1) * i->hdr.num_channels);
	for (n = 0; n < i->hdr.num_channels; n++)
	{
		if (read_ch_hdr1(&(i->ci.f1[n]), fp) != 0)
		{
			_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-dasylab");
			ret = -1;
			break;
		}
	}
	i->data_start = ftell(fp);

	return ret;
} /* read_format1() */


int
read_ch_hdr1(struct ch_header_f1 *hdr, FILE * fp)
{
	double dummy[2];
	long pos;
	char c;

	if (fread(&(hdr->hdr_size), sizeof(WORD), 1, fp) != 1)
		return -1;
	le16_to_cpu(hdr->hdr_size);
	if (fread(&(hdr->ch_number), sizeof(WORD), 1, fp) != 1)
		return -1;
	le16_to_cpu(hdr->ch_number);
	if (fread(&(hdr->block_size), sizeof(WORD), 1, fp) != 1)
		return -1;
	le16_to_cpu(hdr->block_size);
	if (fread(&(hdr->samplerate), sizeof(double), 1, fp) != 1)
		return -1;
	hdr->samplerate = 1.0 / hdr->samplerate;
	if (fread(&(hdr->ch_type), sizeof(WORD), 1, fp) != 1)
		return -1;
	le16_to_cpu(hdr->ch_type);
	if (fread(&(hdr->flags), sizeof(WORD), 1, fp) != 1)
		return -1;
	le16_to_cpu(hdr->flags);

	if (fread(&dummy, sizeof(double), 2, fp) != 2)
		return -1;

	pos = 0;
	do
	{
		fread(&c, 1, 1, fp);
		hdr->unit[pos++] = c;
	}
	while (c);

	pos = 0;
	do
	{
		fread(&c, 1, 1, fp);
		hdr->name[pos++] = c;
	}
	while (c);

	return 0;
} /* read_ch_hdr1() */



int
read_format2(struct info *i, FILE * fp)
{
	if (i || fp)
		;

	return 0;
} /* read_format2() */


int
read_datablock_infos(struct info *i)
{
	char fn[MAX_PATH_RA];
	FILE *fp;
	struct datablock_header_f1 dh;

	get_data_file(i, fn);
	fp = ra_fopen(fn, "rb", 0);
	if (!fp)
	{
		_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-dasylab");
		return -1;
	}

	fseek(fp, (i->data_start + sizeof(struct data_header_f1)), SEEK_SET);	/* skip data-hdr */

	i->di = malloc(sizeof(struct datablock_infos) * i->hdr.num_channels);
	memset(i->di, 0, sizeof(struct datablock_infos) * i->hdr.num_channels);

	for (;;)
	{
		struct datablock_infos *curr;

		if (fread(&dh, sizeof(struct datablock_header_f1), 1, fp) != 1)
			break;

		curr = &(i->di[dh.ch]);

		curr->num_blocks++;
		curr->num_samples += dh.num_samples;
		curr->offsets = realloc(curr->offsets, sizeof(long) * curr->num_blocks);
		curr->offsets[curr->num_blocks - 1] = ftell(fp);
		curr->single_block_size =
			realloc(curr->single_block_size, sizeof(long) * curr->num_blocks);
		curr->single_block_size[curr->num_blocks - 1] = dh.num_samples;
		fseek(fp, (i->ci.f1[dh.ch].block_size * sizeof(float)), SEEK_CUR);
	}
	fclose(fp);

	i->di_ok = 1;

	return 0;
} /* read_datablock_infos() */


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

	_ra_set_error(h, RA_ERR_UNKNOWN_INFO, "plugin-dasylab");

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
		_ra_set_error(h, RA_ERR_ERROR, "plugin-dasylab");
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
	struct tm *ti;
	long ch;

	ch = ra_value_get_number(vh);

	if ((id >= RA_INFO_REC_CHANNEL_START) && (id < RA_INFO_REC_CHANNEL_END) &&
	    ((ch < 0) || (ch >= i->hdr.num_channels)))
	{
		_ra_set_error(i->ra, RA_ERR_OUT_OF_RANGE, "plugin-dasylab");
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
		ra_value_set_string(vh, "DasyLab signal");
		break;
	case RA_INFO_REC_GEN_TIME_C:
		ti = localtime((const time_t *)&(i->hdr.start_time));
		sprintf(t, "%02d:%02d:%02d", ti->tm_hour, ti->tm_min, ti->tm_sec);
		ra_value_set_string(vh, t);
		break;
	case RA_INFO_REC_GEN_DATE_C:
		ti = localtime((const time_t *)&(i->hdr.start_time));
		sprintf(t, "%02d.%02d.%d", ti->tm_mday, ti->tm_mon + 1, ti->tm_year + 1900);
		ra_value_set_string(vh, t);
		break;
	case RA_INFO_REC_GEN_DURATION_SEC_D:
		if (!i->di_ok)
			read_datablock_infos(i);
		d = ((double) i->di[0].num_samples / (double) i->hdr.samplerate);
		ra_value_set_double(vh, d);
		break;
	case RA_INFO_REC_GEN_NUMBER_L:
		ra_value_set_long(vh, 0);
		break;
	case RA_INFO_REC_GEN_NUM_SUB_REC_L:
		ra_value_set_long(vh, 0);
		break;
	case RA_INFO_REC_GEN_NUM_CHANNEL_L:
		ra_value_set_long(vh, i->hdr.num_channels);
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
		switch (i->hdr.file_format)
		{
		case 0:
			ra_value_set_string(vh, "");
			break;
		case 1:
			ra_value_set_string(vh, i->ci.f1[ch].name);
			break;
		case 2:
			ra_value_set_string(vh, "");
			break;
		}
		break;
	case RA_INFO_REC_CH_DESC_C:
		ra_value_set_string(vh, "");
		break;
	case RA_INFO_REC_CH_NUM_SAMPLES_L:
		if (!i->di_ok)
			read_datablock_infos(i);
		ra_value_set_long(vh, i->di[ch].num_samples);
		break;
	case RA_INFO_REC_CH_SAMPLERATE_D:
		ra_value_set_double(vh, (double) i->hdr.samplerate);
		break;
	case RA_INFO_REC_CH_NUM_BITS_L:
		ra_value_set_long(vh, 32);
		break;
	case RA_INFO_REC_CH_AMP_RESOLUTION_D:
		ra_value_set_double(vh, 1.0);	/* adjustment will be done in this module */
		break;
	case RA_INFO_REC_CH_SAMPLE_TYPE_L:
		ra_value_set_long(vh, RA_CH_SAMPLE_TYPE_FLOAT);
		break;
	case RA_INFO_REC_CH_UNIT_C:
		switch (i->hdr.file_format)
		{
		case 0:
			ra_value_set_string(vh, "");
			break;
		case 1:
			ra_value_set_string(vh, i->ci.f1[ch].unit);
			break;
		case 2:
			ra_value_set_string(vh, "");
			break;
		}
		break;
	case RA_INFO_REC_CH_CENTER_VALUE_D:
		if (i->ch_type_infos[ch].centered)
			ra_value_set_double(vh, i->ch_type_infos[ch].center_value);
		else
			ret = -1;
		break;
	case RA_INFO_REC_CH_MIN_UNIT_D:
		ra_value_set_double(vh, i->ch_type_infos[ch].offset);
		break;
	case RA_INFO_REC_CH_MM_UNIT_D:
		ra_value_set_double(vh, i->ch_type_infos[ch].mm_unit);
		break;
	case RA_INFO_REC_CH_TYPE_L:
		ra_value_set_long(vh, i->ch_type_infos[ch].type);
		break;
	default:
		_ra_set_error(i->ra, RA_ERR_UNKNOWN_INFO, "plugin-dasylab");
		ret = -1;
		break;
	}

	return ret;
} /* get_info_rec() */


int
get_info_obj(struct info *i, int id, value_handle vh)
{
	int ret = 0;

	ra_value_reset(vh);

	switch (id)
	{
	default:
		_ra_set_error(i->ra, RA_ERR_UNKNOWN_INFO, "plugin-dasylab");
		ret = -1;
		break;
	}

	return ret;
} /* get_info_obj() */


int
pl_set_info(any_handle mh, value_handle vh)
{
	if (mh)
		;

	ra_value_reset(vh);

	return 0;
} /* pl_set_info() */


size_t
pl_get_raw(meas_handle mh, rec_handle rh, int ch, size_t start, size_t num_data, DWORD *data, DWORD *data_high)
{
	struct info *i = ((struct ra_meas *) mh)->priv;
	size_t num = 0;

	if (rh)
		;

	num = read_raw(i, ch, start, num_data, NULL, data, data_high);

	return num;
} /* pl_get_raw() */


size_t
pl_get_raw_unit(meas_handle mh, rec_handle rh, int ch, size_t start, size_t num_data, double *data)
{
	struct info *i = ((struct ra_meas *) mh)->priv;
	size_t num = 0;

	if (rh)
		;

	num = read_raw(i, ch, start, num_data, data, NULL, NULL);

	return num;
} /* pl_get_raw_unit() */


size_t
read_raw(struct info * i, int ch, size_t start, size_t num, double *data, DWORD *data_raw, DWORD *data_raw_high)
{
	size_t n = 0;
	size_t bl_start, bl_end;
	size_t num_use;

	if (!i->di_ok)
		read_datablock_infos(i);

	if (block_start_end(i, ch, start, num, &bl_start, &bl_end) != 0)
		return 0;

	num_use = num;
	if ((start + num_use) > (size_t)(i->di[ch].num_samples))
		num_use = i->di[ch].num_samples - start;
	if (num_use <= 0)
		return 0;

	switch (i->hdr.file_format)
	{
	case 0:
		n = get_raw_0(i, ch, start, num, bl_start, bl_end, data);
		break;
	case 1:
		n = get_raw_1(i, ch, start, num, bl_start, bl_end, data, data_raw);
		if (data_raw_high)
		{
			size_t l;
			for (l = 0; l < n; l++)
				data_raw_high[l] = 0;
		}
		break;
	case 2:
		n = get_raw_2(i, ch, start, num, bl_start, bl_end, data);
		break;
	default:
		_ra_set_error(i->ra, RA_ERR_UNSUPPORTED, "plugin-dasylab");
		n = 0;
		break;
	}

	return n;
} /* read_raw() */


int
block_start_end(struct info *i, int ch, size_t start, size_t num, size_t *bl_start, size_t *bl_end)
{
	size_t block_size = 1;
	switch (i->hdr.file_format)
	{
	case 0:
		block_size = 1;
		break;
	case 1:
		block_size = i->ci.f1[ch].block_size;
		break;
	case 2:
		block_size = i->ci.f2[ch].block_size;
		break;
	default:
		_ra_set_error(i->ra, RA_ERR_UNSUPPORTED, "plugin-dasylab");
		break;
	}

	*bl_start = start / block_size;
	*bl_end = start + num;
	if (*bl_end % block_size)
	{
		(*bl_end) /= block_size;
		(*bl_end)++;
	}
	else
		(*bl_end) /= block_size;

	return 0;
} /* block_start_end() */


size_t
get_raw_0(struct info * i, int ch, size_t start, size_t num, size_t bl_start,
	   size_t bl_end, double *data)
{
	if (i || ch || start || num || bl_start || bl_end || data)
		;

	_ra_set_error(i->ra, RA_ERR_UNSUPPORTED, "plugin-dasylab");

	return 0;
} /* get_raw_0() */


size_t
get_raw_1(struct info * i, int ch, size_t start, size_t num, size_t bl_start,
	   size_t bl_end, double *data, DWORD *data_raw)
{
	char fn[MAX_PATH_RA];
	FILE *fp;
	float *buf;
	size_t buf_pos = 0;
	size_t l;
	size_t num_data = num;

	get_data_file(i, fn);
	fp = ra_fopen(fn, "rb", 0);
	if (!fp)
	{
		_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-dasylab");
		return 0;
	}

	buf = malloc(sizeof(float) * num);
	for (l = bl_start; l <= bl_end; l++)
	{
		size_t in_block_start;
		size_t data_read;

		if (l >= (size_t)i->di[ch].num_blocks)
			break;

		if (l == bl_start)
			in_block_start = start % i->ci.f1[ch].block_size;
		else
			in_block_start = 0;

		data_read = i->di[ch].single_block_size[l] - in_block_start;
		if (data_read > num_data)
			data_read = num_data;

		fseek(fp, (long)(i->di[ch].offsets[l] + (in_block_start * sizeof(float))), SEEK_SET);
		if (fread(&(buf[buf_pos]), sizeof(float), data_read, fp) != (size_t)data_read)
			break;

		buf_pos += data_read;
		num_data -= data_read;
	}

	for (l = 0; l < buf_pos; l++)
	{
		if (data)
			data[l] = (double)buf[l];
		if (data_raw)
			data_raw[l] = ((DWORD *)buf)[l];
	}

	fclose(fp);
	free(buf);

	return (size_t) buf_pos;
} /* get_raw_1() */


size_t
get_raw_2(struct info * i, int ch, size_t start, size_t num, size_t bl_start,
	   size_t bl_end, double *data)
{
	if (i || ch || start || num || bl_start || bl_end || data)
		;

	_ra_set_error(i->ra, RA_ERR_UNSUPPORTED, "plugin-dasylab");

	return 0;
} /* get_raw_2() */


void
get_data_file(struct info *i, char *fn)
{
	if (i->hdr.use_extra_data_file)
		sprintf(fn, "%s%c%s", i->dir, SEPARATOR, i->hdr.infos.data_file);
	else
		sprintf(fn, "%s%c%s.%s", i->dir, SEPARATOR, i->name, i->ext);
} /* get_data_file() */
