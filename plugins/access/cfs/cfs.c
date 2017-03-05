/*----------------------------------------------------------------------------
 * cfs.c  -  Handle signals saved using the CED Filing System (CFS)
 *
 * Description:
 * The plugin provides access to recordings saved in the CED Filing System (CFS)
 * format. At the moment only equalspaced channels are supported.
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

#define _LIBRASCH_BUILD
#include <ra.h>
#include <ra_priv.h>
#include <handle_plugin.h>

#include <ra_endian.h>
#include <pl_general.h>
#include <ra_file.h>
#include <ra_estimate_ch_infos.h>

#include "cfs.h"



LIBRAAPI int
load_ra_plugin (struct plugin_struct *ps)
{
	strcpy (ps->info.name, PLUGIN_NAME);
	strcpy (ps->info.desc, PLUGIN_DESC);
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
init_ra_plugin (struct librasch *ra, struct plugin_struct *ps)
{
	if (!ra || !ps)
		return -1;

	return 0;
} /* init_ra_plugin() */



LIBRAAPI int
fini_ra_plugin (void)
{
	return 0;
} /* fini_plugin() */


int
pl_check_meas (const char *name)
{
	struct info *i;
	FILE *fp = NULL;
	char fn[MAX_PATH_RA];
	char id[8];

	i = malloc (sizeof (struct info));
	if (!i)
		goto error_open;
	memset (i, 0, sizeof (struct info));

	/* split file in dir and filename w/o extension */
	split_filename (name, SEPARATOR, i->dir, i->name, i->ext);
	sprintf (fn, "%s%c%s.%s", i->dir, SEPARATOR, i->name, i->ext);
	fp = ra_fopen (fn, "rb", 0);
	if (fp == NULL)
		goto error_open;
	if (fread (id, 1, 8, fp) != 8)
		goto error_open;
	/* check if id is ok; ignore version info */
	if (strncmp (id, "CEDFILE", 7) != 0)
		goto error_open;

	fclose (fp);
	fp = NULL;
	free (i);

	return 1;

 error_open:
	if (i)
		free (i);
	if (fp)
		fclose (fp);

	return 0;
} /* pl_check_meas() */


int
pl_find_meas(const char *dir, struct find_meas *f)
{
	return find_meas(dir, "*.CFS", PLUGIN_NAME, f, pl_check_meas);
}  /* pl_find_meas() */


meas_handle
pl_open_meas (ra_handle ra, const char *file, int fast)
{
	struct ra_meas *meas;
	struct info *i = NULL;
	char mask[MAX_PATH_RA];
	
	if (!pl_check_meas (file))
		goto error_open;
	
	if ((meas = ra_alloc_mem(sizeof(struct ra_meas))) == NULL)
		goto error_open;
	memset(meas, 0, sizeof(struct ra_meas));
	if ((i = calloc(1, sizeof (struct info))) == NULL)
	{
		_ra_set_error(ra, 0, "plugin-cfs");
		goto error_open;
	}
	meas->priv = i;
	i->ra = ra;

	/* split file in dir and filename w/o extension */
	split_filename (file, SEPARATOR, i->dir, i->name, i->ext);

	if (get_data (i, fast) == -1)
		goto error_open;

	sprintf(mask, "%s*", i->name);
	find_files(i->dir, mask, &(i->files), &(i->num_files), &(i->size));

	/* set session infos */
	set_one_session(meas, NULL, NULL);

	return meas;

 error_open:
	if (i)
		free_info_struct (i);

	return NULL;
} /* open_meas() */


int
pl_close_meas (meas_handle mh)
{
	struct info *i = ((struct ra_meas *)mh)->priv;

	free_info_struct (i);

	return 0;
} /* close_meas() */


void
free_info_struct (struct info *i)
{
	int j;

	if (!i)
		return;

	if (i->lib_ch2file_ch)
		free (i->lib_ch2file_ch);

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
get_data (struct info *i, int fast)
{
	char file[MAX_PATH_RA];
	char file_wo_ext[MAX_PATH_RA];
	FILE *fp;
	int n;

	sprintf (file, "%s%c%s.%s", i->dir, SEPARATOR, i->name, i->ext);
	fp= ra_fopen (file, "rb", 0);
	if (fp == NULL)
	{
		_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-cfs");
		goto error;
	}

	if (read_global_header(i, fp) != 0)
		goto error;

	if (read_ch_info(i, fp) != 0)
		goto error;

	if (read_vars_info(&(i->file_var_info), i->global_hdr.num_file_vars, fp) != 0)
	{
		_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-cfs");
		goto error;
	}

	if (!fast)
	{
		if (read_vars_info(&(i->data_section_var_info), i->global_hdr.num_data_sections_vars, fp) != 0)
		{
			_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-cfs");
			goto error;
		}
	}

	fclose(fp);

	i->ch_type_info = malloc(sizeof(struct ra_est_ch_infos) * i->global_hdr.num_ch);
	i->mm_sec = 1;
	sprintf(file_wo_ext, "%s%c%s", i->dir, SEPARATOR, i->name);
	for (n = 0; n < i->global_hdr.num_ch; n++)
	{
		char name[22];
		strncpy(name, i->ch_info[n].name, i->ch_info[n].name_len);
		name[i->ch_info[n].name_len] = '\0';
		if (ra_est_ch_type(i->ra, i->dir, file_wo_ext, 0, name, &(i->ch_type_info[n])) == 0)
		{
			i->ch_type_info[n].type = RA_CH_TYPE_UNKNOWN;
			i->ch_type_info[n].mm_sec = 1;
			i->ch_type_info[n].mm_unit = 1;
			i->ch_type_info[n].offset = 0;
			i->ch_type_info[n].centered = 0;
			i->ch_type_info[n].center_value = 0;
		}
		
		/* correct mm_unit if necessary */
		if ((strcmp("muV", i->ch_info[n].y_unit) == 0) || (strcmp("myV", i->ch_info[n].y_unit) == 0))
			i->ch_type_info[n].mm_unit /= 1000.0;
		if (strcmp("nV", i->ch_info[n].y_unit) == 0)
			i->ch_type_info[n].mm_unit /= 1e6;

		if (i->mm_sec < i->ch_type_info[n].mm_sec)
			i->mm_sec = i->ch_type_info[n].mm_sec;
	}

	return 0;

 error:
	if (fp)
		fclose (fp);
	return -1;
} /* get_data() */


int
read_global_header(struct info *i, FILE *fp)
{
	if (fread(&(i->global_hdr), sizeof(struct cfs_global_hdr), 1, fp) != 1)
	{
		_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-cfs");		
		return -1;
	}

	/* handle endianess */
	le32_to_cpu(i->global_hdr.file_size);
	le16_to_cpu(i->global_hdr.num_ch);
	le16_to_cpu(i->global_hdr.num_file_vars);
	le16_to_cpu(i->global_hdr.num_data_sections_vars);
	le16_to_cpu(i->global_hdr.file_hdr_size);
	le16_to_cpu(i->global_hdr.data_section_hdr_size);
	le32_to_cpu(i->global_hdr.offset_last_data_section);
	le16_to_cpu(i->global_hdr.num_data_sections);
	le16_to_cpu(i->global_hdr.disk_block_rounding);
	le32_to_cpu(i->global_hdr.offset_pointer_table);

	return 0;
}  /* read_global_header() */


int
read_ch_info(struct info *i, FILE *fp)
{
	int j;

	i->ch_info = malloc(sizeof(struct cfs_ch_info) * i->global_hdr.num_ch);
	if (fread(i->ch_info, sizeof(struct cfs_ch_info), i->global_hdr.num_ch, fp) != (size_t)i->global_hdr.num_ch)
	{
		_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-cfs");
		return -1;
	}

	i->lib_ch2file_ch = malloc(sizeof(int) * i->global_hdr.num_ch);
	memset(i->lib_ch2file_ch, 0, sizeof(int) * i->global_hdr.num_ch);
	i->num_ch_used = 0;

	/* handle endianess and select ch's which will be used */
	for (j = 0; j < i->global_hdr.num_ch; j++)
	{
		if ((i->ch_info[j].data_kind == CFS_EQUALSPACED) && (strcmp(i->ch_info[j].x_unit, "sample") != 0)
			&& (i->ch_info[j].x_unit[0] != '\0'))
		{
			i->lib_ch2file_ch[i->num_ch_used++] = j;
		}

		le16_to_cpu(i->ch_info[j].space_between_elems);
		le16_to_cpu(i->ch_info[j].next_matrix_ch_data);
	}

	return 0;
}  /* read_ch_info() */


int
read_vars_info(struct cfs_variable_info **i, int n, FILE *fp)
{
	int j;

	*i = malloc(sizeof(struct cfs_variable_info) * n);
	if (fread(*i, sizeof(struct cfs_variable_info), n, fp) != (size_t)n)
		return -1;
    
	/* handle endianess */
	for (j = 0; j < n; j++)
	{
		le16_to_cpu((*i)[j].var_type);
		le16_to_cpu((*i)[j].offset);
	}

	return 0;
}  /* read_vars_info() */


int
read_datasection_info(struct info *i)
{
	int ret = -1;
	char file[MAX_PATH_RA];
	FILE *fp;
	WORD j;
	int cnt = i->global_hdr.num_data_sections;
	long offset = i->global_hdr.offset_last_data_section;

	if (i->data_section_read)
		return 0;

	i->data_section_read = 1;

	if (cnt <= 0)
		return 0;

	sprintf(file, "%s%c%s.%s", i->dir, SEPARATOR, i->name, i->ext);
	fp= ra_fopen(file, "rb", 0);
	if (fp == NULL)
	{
		_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-cfs");
		goto error;
	}

	i->data_section = malloc(sizeof(struct cfs_data_section) * i->global_hdr.num_data_sections);

	do
	{
		i->data_section[cnt-1].abs_section_pos = offset;

		fseek(fp, offset, SEEK_SET);
		if (fread(&(i->data_section[cnt-1].hdr), sizeof(struct cfs_data_section_hdr), 1, fp) != 1)
			goto error;

		i->data_section[cnt-1].ch_sample_start = malloc(sizeof(long) * i->global_hdr.num_ch);
		memset(i->data_section[cnt-1].ch_sample_start, 0, sizeof(long) * i->global_hdr.num_ch);

		i->data_section[cnt-1].ch_info = malloc(sizeof(struct cfs_data_section_ch_info) * i->global_hdr.num_ch);
		if (fread(i->data_section[cnt-1].ch_info, sizeof(struct cfs_data_section_ch_info), i->global_hdr.num_ch, fp)
		    != (size_t)i->global_hdr.num_ch)
		{
			_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-cfs");
			goto error;
		}

		offset = i->data_section[cnt-1].hdr.prev_data_section_hdr;
		cnt--;
	} while ((offset > 0) && cnt);

	for (j = 1; j < i->global_hdr.num_data_sections; j++)
	{
		int k;
		for (k = 0; k < i->global_hdr.num_ch; k++)
		{
			i->data_section[j].ch_sample_start[k] = i->data_section[j-1].ch_sample_start[k]
				+ i->data_section[j-1].ch_info[k].num_values;
		}
	}

	ret = 0;

 error:
	if (fp)
		fclose(fp);

	return ret;
}  /* read_datasection_info() */


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

	_ra_set_error(h, RA_ERR_UNKNOWN_INFO, "plugin-cfs");

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
		_ra_set_error(h, RA_ERR_ERROR, "plugin-cfs");
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
get_info_rec (struct info *i, int id, value_handle vh)
{
	int ret = 0;
	char t[MAX_PATH_RA];
	double d;
	long l;
	int j;
	long ch;

	ch = ra_value_get_number(vh);

	if ((id >= RA_INFO_REC_CHANNEL) && (id < RA_INFO_REC_CHANNEL_END) &&
	    ((ch < 0) || (ch >= i->num_ch_used)))
	{
		_ra_set_error(i->ra, RA_ERR_OUT_OF_RANGE, "plugin-cfs");
		return -1;
	}

	if (!i->data_section_read)
		read_datasection_info(i);

	switch (id)
	{
	case RA_INFO_REC_GEN_TYPE_L:
		ra_value_set_long(vh, RA_REC_TYPE_TIMESERIES);
		break;
	case RA_INFO_REC_GEN_NAME_C:
		ra_value_set_string(vh, i->name);
		break;
	case RA_INFO_REC_GEN_DESC_C:
		ra_value_set_string(vh, "CED Filing System (CFS)");
		break;
  	case RA_INFO_REC_GEN_TIME_C:
		strncpy(t, i->global_hdr.create_time, 8);
		t[8] = '\0';
		ra_value_set_string(vh, t);
		break; 
	case RA_INFO_REC_GEN_DATE_C:
		strncpy(t, i->global_hdr.create_date, 8);
		t[8] = '\0';
		ra_value_set_string(vh, t);
		break;
	case RA_INFO_REC_GEN_DURATION_SEC_D:
		d = 0.0;
		/* use data from channel 0 */
		for (j = 0; j < i->global_hdr.num_data_sections; j++)
		{
			double s = (double)(i->data_section[j].ch_info[i->lib_ch2file_ch[0]].num_values);
			s *= (double)(i->data_section[j].ch_info[i->lib_ch2file_ch[0]].x_inc);
			d += s;
		}
		ra_value_set_double(vh, d);
		break;
	case RA_INFO_REC_GEN_NUMBER_L:
		ra_value_set_long(vh, 0);
		break;
	case RA_INFO_REC_GEN_NUM_SUB_REC_L:
		ra_value_set_long(vh, 0);
		break;
	case RA_INFO_REC_GEN_NUM_CHANNEL_L:
		ra_value_set_long(vh, i->num_ch_used);
		break;
	case RA_INFO_REC_GEN_OFFSET_L:
		ra_value_set_long(vh, 0);
		break;
	case RA_INFO_REC_GEN_PATH_C:
		sprintf(t, "%s%c%s", i->dir, SEPARATOR, i->name);
		ra_value_set_string(vh, t);
		break;
	case RA_INFO_REC_GEN_DIR_L:
		ra_value_set_long(vh, 0);  /* not saved in dir */
		break;
	case RA_INFO_REC_GEN_MM_SEC_D:
		ra_value_set_double(vh, i->mm_sec);
		break;
	case RA_INFO_REC_CH_NAME_C:
		ra_value_set_string(vh, i->ch_info[i->lib_ch2file_ch[ch]].name);
		break;
	case RA_INFO_REC_CH_DESC_C:
		ra_value_set_string(vh, "");
		break;
	case RA_INFO_REC_CH_NUM_SAMPLES_L:
		l = 0;
		for (j = 0; j < i->global_hdr.num_data_sections; j++)
			l += i->data_section[j].ch_info[i->lib_ch2file_ch[ch]].num_values;
		ra_value_set_long(vh, l);
		break;
	case RA_INFO_REC_CH_SAMPLERATE_D:
		d = get_samplerate(i, i->lib_ch2file_ch[ch]);
		ra_value_set_double(vh, d);
		break;
	case RA_INFO_REC_CH_NUM_BITS_L:
		switch (i->ch_info[ch].data_type)
		{
		case CFS_INT1:
		case CFS_INT2:
			ra_value_set_long(vh, 16);
			break;
		case CFS_WRD1:
		case CFS_WRD2:
		case CFS_RL4:
			ra_value_set_long(vh, 32);
			break;
		case CFS_RL8:
			ra_value_set_long(vh, 64);
			break;
		}
		break;
  	case RA_INFO_REC_CH_AMP_RESOLUTION_D:
		ra_value_set_double(vh, 1.0);	/* adjustment will be done in this module */
		break;
	case RA_INFO_REC_CH_SAMPLE_TYPE_L:
		switch (i->ch_info[ch].data_type)
		{
		case CFS_INT1:
		case CFS_INT2:
			ra_value_set_long(vh, RA_CH_SAMPLE_TYPE_INT);
			break;
		case CFS_WRD1:
		case CFS_WRD2:
			ra_value_set_long(vh, RA_CH_SAMPLE_TYPE_WORD);
			break;
		case CFS_RL4:
			ra_value_set_long(vh, RA_CH_SAMPLE_TYPE_FLOAT);
			break;
		case CFS_RL8:
			ra_value_set_long(vh, RA_CH_SAMPLE_TYPE_DOUBLE);
			break;
		}
		break;
  	case RA_INFO_REC_CH_UNIT_C:
		ra_value_set_string(vh, i->ch_info[i->lib_ch2file_ch[ch]].y_unit);
		break;
	case RA_INFO_REC_CH_CENTER_VALUE_D:
		if (i->ch_type_info[i->lib_ch2file_ch[ch]].centered)
			ra_value_set_double(vh, i->ch_type_info[i->lib_ch2file_ch[ch]].center_value);
		else
			ret = -1;
		break;
	case RA_INFO_REC_CH_MIN_UNIT_D:
		ra_value_set_double(vh, i->ch_type_info[i->lib_ch2file_ch[ch]].offset);
		break;
	case RA_INFO_REC_CH_MM_UNIT_D:
		ra_value_set_double(vh, i->ch_type_info[i->lib_ch2file_ch[ch]].mm_unit);
		break;
	case RA_INFO_REC_CH_TYPE_L:
		ra_value_set_long(vh, i->ch_type_info[i->lib_ch2file_ch[ch]].type);
		break;
  	default:
		_ra_set_error(i->ra, RA_ERR_UNKNOWN_INFO, "plugin-cfs");
		ret = -1;
		break;
	}

	return ret;
} /* get_info_rec() */


int
get_info_obj (struct info *i, int id, value_handle vh)
{
	int ret = 0;
	if (!vh)
		return -1;

	switch (id)
	{
	default:
		_ra_set_error(i->ra, RA_ERR_UNKNOWN_INFO, "plugin-cfs");
		ret = -1;
		break;
	}

	return ret;
} /* get_info_obj() */


double
get_samplerate(struct info *i, int ch)
{
	double min_x_inc = 1000000.0;
	double samplerate;
	long j;

	for (j = 0; j < i->global_hdr.num_data_sections; j++)
	{
		if (min_x_inc > i->data_section[j].ch_info[ch].x_inc)
			min_x_inc = i->data_section[j].ch_info[ch].x_inc;
	}
	
	switch (i->ch_info[ch].x_unit[0])
	{
	case 'm':  /* milliseconds (I hope) */
		min_x_inc /= 1000.0;
		break;
	case 's':  /* seconds -> do nothing */
		break;
	}

	samplerate = 1.0 / min_x_inc;

	return samplerate;
}  /* get_samplerate() */


int
pl_set_info(any_handle h, value_handle vh)
{
	if (!h | !vh)
		return -1;

	return 0;
} /* pl_set_info() */


size_t
pl_get_raw(meas_handle mh, rec_handle rh, int ch, size_t start, size_t num_data, DWORD *data, DWORD *data_high)
{
	struct info *i = ((struct ra_meas *)mh)->priv;
	size_t num = 0;

	if (rh)  /* to get rid of compiler warning */
		;

	if (!i->data_section_read)
		read_datasection_info(i);

	num = read_raw(i, i->lib_ch2file_ch[ch], start, num_data, NULL, data, data_high);

	return num;
} /* pl_get_raw() */


size_t
pl_get_raw_unit(meas_handle mh, rec_handle rh, int ch, size_t start, size_t num_data, double *data)
{
	struct info *i = ((struct ra_meas *)mh)->priv;
	size_t j, num = 0;
	double *buf;

	if (rh)  /* to get rid of compiler warning */
		;

	if (!i->data_section_read)
		read_datasection_info(i);

	buf = malloc(sizeof(double) * num_data);
	num = read_raw(i, i->lib_ch2file_ch[ch], start, num_data, buf, NULL, NULL);
	for (j = 0; j < num; j++)
		data[j] = (double) buf[j];
	free (buf);

	return num;
} /* pl_get_raw_unit() */


size_t
read_raw(struct info *i, int ch, size_t start, size_t num, double *data, DWORD *data_raw, DWORD *data_raw_high)
{
	size_t n = 0;
	long bl_start, bl_end;
	int j;
	FILE *fp;
	char fn[MAX_PATH_RA];

	bl_start = bl_end = -1;
	for (j = 0; j < i->global_hdr.num_data_sections; j++)
	{
		size_t bs, be;
		bs = i->data_section[j].ch_sample_start[ch];
		be = bs + i->data_section[j].ch_info[ch].num_values;

		if ((start >= bs) && (start <= be))
			bl_start = j;

		if (((start+num) >= bs) && ((start+num) <= be))
			bl_end = j;

		if ((bl_start != -1) && (bl_end != -1))
			break;
	}
	if (bl_start == -1)
		return 0;

	if (bl_end == -1)
		bl_end = i->global_hdr.num_data_sections - 1;

	sprintf(fn, "%s%c%s.%s", i->dir, SEPARATOR, i->name, i->ext);
	fp = ra_fopen(fn, "rb", 0);
	if (!fp)
	{
		_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-cfs");
		return 0;
	}

	n = read_raw_data(i, fp, ch, start, num, data, data_raw, data_raw_high, bl_start, bl_end);

	fclose(fp);

	return n;
}  /* read_raw() */


size_t
read_raw_data(struct info *i, FILE *fp, int ch, size_t start, size_t num, double *data,
	      DWORD *data_raw, DWORD *data_raw_high, long bl_start, long bl_end)
{
	long j;
	size_t k;
	size_t n = 0;
	BYTE *b_buf = NULL;
	signed char *c_buf = NULL;
	WORD *w_buf = NULL;
	signed short *s_buf = NULL;
	float *f_buf = NULL;
	double *d_buf = NULL;

	if ((i->ch_info[ch].data_type == CFS_RL8) && (data_raw_high == NULL))
		return 0;

	switch (i->ch_info[ch].data_type)
	{
	case CFS_INT1: c_buf = malloc(num); break;
	case CFS_WRD1: b_buf = malloc(num); break;
	case CFS_INT2: s_buf = malloc(num * 2); break;
	case CFS_WRD2: w_buf = malloc(num * 2); break;
	case CFS_RL4:  f_buf = malloc(num * 4); break;
	case CFS_RL8:  d_buf = malloc(num * 8); break;
	}
	
	for (j = bl_start; j <= bl_end; j++)
	{
		size_t offset;
		size_t num_data;
		size_t pos;

		if (start > (size_t)i->data_section[j].ch_sample_start[ch])
			offset = start - i->data_section[j].ch_sample_start[ch];
		else
			offset = 0;

		num_data = num - n;
		if (num_data > (i->data_section[j].ch_info[ch].num_values - offset))
			num_data = (i->data_section[j].ch_info[ch].num_values - offset);

		/* now offset must be set to number of bytes (after this point offset is only
		   allowed to use in a byte context) */
		switch (i->ch_info[ch].data_type)
		{
		case CFS_INT2: offset *= 2; break;
		case CFS_WRD2: offset *= 2; break;
		case CFS_RL4: offset *= 4; break;
		case CFS_RL8: offset *= 8; break;
		}

		pos = i->data_section[j].hdr.start_ch_data + i->data_section[j].ch_info[ch].start + offset;
		fseek(fp, (long)pos, SEEK_SET);

		switch (i->ch_info[ch].data_type)
		{
 		case CFS_INT1:
			fread(c_buf + n, 1, num_data, fp);
			for (k = n; k < (n+num_data); k++)
			{
				if (data)
					data[k] = ((double)c_buf[k]) * i->data_section[j].ch_info[ch].y_scale;
				if (data_raw)
					data_raw[k] = (DWORD)c_buf[k];
			}
			break;
		case CFS_WRD1:
			fread(b_buf + n, 1, num_data, fp);
			for (k = n; k < (n+num_data); k++)
			{
				if (data)
					data[k] = ((double)b_buf[k]) * i->data_section[j].ch_info[ch].y_scale;
				if (data_raw)
					data_raw[k] = (DWORD)b_buf[k];
			}
			break;
		case CFS_INT2:
			fread(s_buf + n, 2, num_data, fp);
			for (k = n; k < (n+num_data); k++)
			{
				if (data)
					data[k] = ((double)s_buf[k]) * i->data_section[j].ch_info[ch].y_scale;
				if (data_raw)
					data_raw[k] = (DWORD)s_buf[k];
			}
			break;
		case CFS_WRD2:
			fread(w_buf + n, 2, num_data, fp);
			for (k = n; k < (n+num_data); k++)
			{
				if (data)
					data[k] = ((double)w_buf[k]) * i->data_section[j].ch_info[ch].y_scale;
				if (data_raw)
					data_raw[k] = (DWORD)w_buf[k];
			}
			break;
		case CFS_RL4:
			fread(f_buf + n, 4, num_data, fp);
			for (k = n; k < (n+num_data); k++)
			{
				if (data)
					data[k] = ((double)f_buf[k]) * i->data_section[j].ch_info[ch].y_scale;
				if (data_raw)
					data_raw[k] = ((DWORD *)f_buf)[k];
			}
			break;
		case CFS_RL8:
			fread(d_buf + n, 8, num_data, fp);
			for (k = n; k < (n+num_data); k++)
			{
				if (data)
					data[k] = d_buf[k] * i->data_section[k].ch_info[ch].y_scale;
				if (data_raw && data_raw_high)
				{
					double v = d_buf[k];

					/* TODO: check if this is correct */
					data_raw[k] = ((DWORD *)(&v))[1];
					data_raw_high[k] = ((DWORD *)(&v))[0];
				}

			}
			break;
		}
		
		n += num_data;
	}

	if (b_buf) free(b_buf);
	if (c_buf) free(c_buf);
	if (w_buf) free(w_buf);
	if (s_buf) free(s_buf);
	if (f_buf) free(f_buf);
	if (d_buf) free(d_buf);

	return n;
}  /* read_raw_data() */
