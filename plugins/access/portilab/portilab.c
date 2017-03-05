/*----------------------------------------------------------------------------
 * portilab.c  -  Handle signals recorded using TMS ADC system and PortiLab
 *
 * Description:
 * The plugin provides access to data recorded with the TMS ADC-system and using
 * the PortiLab software.
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

#include <ra_endian.h>
#include <pl_general.h>
#include <ra_file.h>
#include <ra_estimate_ch_infos.h>

#include "portilab.h"



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
	struct porti_header hdr;

	i = malloc(sizeof(struct info));
	if (!i)
		goto error_open;
	memset(i, 0, sizeof(struct info));

	/* split file in dir and filename w/o extension */
	split_filename(name, SEPARATOR, i->dir, i->name, i->ext);
	if (i->ext[0] == '\0')
		strcpy(i->ext, "S00");  /* up to now all PortiLab files have S00 as extension */

	sprintf(fn, "%s%c%s.%s", i->dir, SEPARATOR, i->name, i->ext);
	fp = ra_fopen(fn, "rb", 0);
	if (fp == NULL)
		goto error_open;
	if (fread(&hdr, sizeof(struct porti_header), 1, fp) != 1)
		goto error_open;
	/* check if id is ok; ignore version info */
	if (strncmp(hdr.id, "POLY SAMPLE FILE", 16) != 0)
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
	return find_meas(dir, "*.s00", PLUGIN_NAME, f, pl_check_meas);
} /* pl_find_meas() */


meas_handle
pl_open_meas(ra_handle ra, const char *file, int fast)
{
	struct ra_meas *meas = NULL;
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
	if (i->ext[0] == '\0')
		strcpy(i->ext, "S00");  /* up to now all PortiLab files have S00 as extension */

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

	if (i->ci != NULL)
		free(i->ci);
	if (i->ch_type_infos)
		free(i->ch_type_infos);

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
	FILE *fd;
	struct signal_desc *desc = NULL;
	int j, count;

	sprintf(file, "%s%c%s.%s", i->dir, SEPARATOR, i->name, i->ext);
	fd = ra_fopen(file, "rb", 0);
	if (fd == NULL)
		goto error;

	if (fread(&(i->hdr), sizeof(struct porti_header), 1, fd) != 1)
		goto error;

	pascalstr2sz(i->hdr.meas_name);

	/* handle endianess */
	le16_to_cpu(i->hdr.version);
	le16_to_cpu(i->hdr.sample_rate);
	le16_to_cpu(i->hdr.storage_rate);
	le16_to_cpu(i->hdr.num_sig);
	le32_to_cpu(i->hdr.num_sample_periods);
	le32_to_cpu(i->hdr.num_data_blocks);
	le16_to_cpu(i->hdr.sample_periods_per_data_block);
	le16_to_cpu(i->hdr.size_sample_data_per_block);
	le16_to_cpu(i->hdr.compression);

	desc = malloc(sizeof(struct signal_desc) * i->hdr.num_sig);
	if (fread(desc, sizeof(struct signal_desc), i->hdr.num_sig, fd) != (size_t) i->hdr.num_sig)
		goto error;

	i->num_channels = 0;
	for (j = 0; j < i->hdr.num_sig; j++)
	{
		i->num_channels++;
		pascalstr2sz(desc[j].name);
		pascalstr2sz(desc[j].unit);
		if ((strncmp(desc[j].name, "(Lo)", 4) == 0)
		    || (strncmp(desc[j].name, "(Hi)", 4) == 0))
			j++;
	}

	i->ci = malloc(sizeof(struct channel_info) * i->num_channels);
	count = 0;
	for (j = 0; j < i->hdr.num_sig; j++)
	{
		memcpy(&(i->ci[count].signal_desc), &(desc[j]), sizeof(struct signal_desc));

		/* handle endianess */
		le16_to_cpu(i->ci[count].signal_desc.index);
		le16_to_cpu(i->ci[count].signal_desc.offset);
		/* TODO: check if float needs endianess handling also */

		if ((strncmp(desc[j].name, "(Lo)", 4) == 0)
		    || (strncmp(desc[j].name, "(Hi)", 4) == 0))
		{
			i->ci[count].bit32 = 1;
			strcpy(i->ci[count].signal_desc.name, &(desc[j].name[5]));
			j++;
		}
		else
		{
			i->ci[count].bit32 = 0;
		}
		count++;
	}
	free(desc);
	desc = NULL;

	/* try to get channel infos */
	i->ch_type_infos = malloc(sizeof(struct ra_est_ch_infos) * i->num_channels);
	i->mm_sec = 1;
	sprintf(file_wo_ext, "%s%c%s", i->dir, SEPARATOR, i->name);
	for (j = 0; j < i->num_channels; j++)
	{
		if (ra_est_ch_type(i->ra, i->dir, file_wo_ext, 0, i->ci[j].signal_desc.name, &(i->ch_type_infos[j])) == 0)
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
			if (i->ci[j].signal_desc.unit[0] == 'u')
				i->ch_type_infos[j].mm_unit /= 1000.0;
		}

		if (i->mm_sec < i->ch_type_infos[j].mm_sec)
			i->mm_sec = i->ch_type_infos[j].mm_sec;
	}

	if (fd)
		fclose(fd);

	return 0;

error:
	if (fd)
		fclose(fd);

	if (desc)
		free(desc);

	_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-poly5/tms32");

	return -1;
} /* get_data() */


void
pascalstr2sz(char *string)
{
	unsigned char t;
	int len;
	char *s;

	t = string[0];
	len = t;
	s = malloc(len + 1);
	strncpy(s, &(string[1]), len);
	s[len] = '\0';
	strcpy(string, s);
	free(s);
} /* pascalstr2sz */


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

	_ra_set_error(h, RA_ERR_UNKNOWN_INFO, "plugin-poly5/tms32");

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
		_ra_set_error(h, RA_ERR_ERROR, "plugin-poly5/tms32");
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
		_ra_set_error(i->ra, RA_ERR_OUT_OF_RANGE, "plugin-poly5/tms32");
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
		ra_value_set_string(vh, "Portilab signal");
		break;
	case RA_INFO_REC_GEN_TIME_C:
		sprintf(t, "%02d:%02d:%02d", i->hdr.rec_start.hour,
			i->hdr.rec_start.min, i->hdr.rec_start.sec);
		ra_value_set_string(vh, t);
		break;
	case RA_INFO_REC_GEN_DATE_C:
		sprintf(t, "%02d.%02d.%d", i->hdr.rec_start.day,
			i->hdr.rec_start.mon, i->hdr.rec_start.year);
		ra_value_set_string(vh, t);
		break;
	case RA_INFO_REC_GEN_DURATION_SEC_D:
		d = ((double) i->hdr.num_sample_periods / (double) i->hdr.sample_rate);
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
		ra_value_set_string(vh, i->ci[ch].signal_desc.name);
		break;
	case RA_INFO_REC_CH_DESC_C:
		ra_value_set_string(vh, "");
		break;
	case RA_INFO_REC_CH_NUM_SAMPLES_L:
		ra_value_set_long(vh, i->hdr.num_sample_periods);
		break;
	case RA_INFO_REC_CH_SAMPLERATE_D:
		ra_value_set_double(vh, (double) i->hdr.sample_rate);
		break;
	case RA_INFO_REC_CH_NUM_BITS_L:
		if (i->ci[ch].bit32)
			ra_value_set_long(vh, 32);
		else
			ra_value_set_long(vh, 16);
		break;
	case RA_INFO_REC_CH_AMP_RESOLUTION_D:
		ra_value_set_double(vh, 1.0);	/* adjustment will be done in this module */
		break;
	case RA_INFO_REC_CH_SAMPLE_TYPE_L:
		if (i->ci[ch].bit32)
			ra_value_set_long(vh, RA_CH_SAMPLE_TYPE_FLOAT);
		else
			ra_value_set_long(vh, RA_CH_SAMPLE_TYPE_INT);
		break;
	case RA_INFO_REC_CH_UNIT_C:
		ra_value_set_string(vh, i->ci[ch].signal_desc.unit);
		break;
	case RA_INFO_REC_CH_MIN_UNIT_D:
		ra_value_set_double(vh, i->ci[ch].signal_desc.min_value_unit);
		break;
	case RA_INFO_REC_CH_MAX_UNIT_D:
		ra_value_set_double(vh, i->ci[ch].signal_desc.max_value_unit);
		break;
	case RA_INFO_REC_CH_MIN_ADC_D:
		ra_value_set_double(vh, i->ci[ch].signal_desc.min_value_adc);
		break;
	case RA_INFO_REC_CH_MAX_ADC_D:
		ra_value_set_double(vh, i->ci[ch].signal_desc.max_value_adc);
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
		_ra_set_error(i->ra, RA_ERR_UNKNOWN_INFO, "plugin-poly5/tms32");
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
		_ra_set_error(i->ra, RA_ERR_UNKNOWN_INFO, "plugin-poly5/tms32");
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
	size_t j, k;
	char fn[MAX_PATH_RA];
	WORD *buf_blk = NULL;
	size_t num_ret = 0;
	FILE *fp = NULL;
	float adc_r, unit_r;
	size_t n_use;
	size_t s_blk, s_off, e_blk;

	s_blk = start / i->hdr.sample_periods_per_data_block;
	s_off = start % i->hdr.sample_periods_per_data_block;
	e_blk = (start + num) / i->hdr.sample_periods_per_data_block;
	if (((start + num) % i->hdr.sample_periods_per_data_block) != 0)
		e_blk++;

	sprintf(fn, "%s%c%s.%s", i->dir, SEPARATOR, i->name, i->ext);
	if ((fp = ra_fopen(fn, "rb", 0)) == NULL)
	{
		_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-poly5/tms32");
		return 0;
	}

	if ((buf_blk = malloc(i->hdr.size_sample_data_per_block)) == NULL)
	{
		fclose(fp);
		_ra_set_error(i->ra, 0, "plugin-poly5/tms32");
		return 0;
	}

	adc_r = i->ci[ch].signal_desc.max_value_adc - i->ci[ch].signal_desc.min_value_adc;
	unit_r = i->ci[ch].signal_desc.max_value_unit - i->ci[ch].signal_desc.min_value_unit;
	for (j = s_blk; j <= e_blk; j++)
	{
		if (read_data_block(i, j, buf_blk, fp))
			break;

		/* how many samples left? */
		n_use = (size_t)i->hdr.sample_periods_per_data_block - s_off;
		if ((num_ret + n_use) > num)
			n_use = num - num_ret;

		if ((start + num_ret + n_use) > (size_t) i->hdr.num_sample_periods)
			n_use = i->hdr.num_sample_periods - start - num_ret;

		for (k = s_off; k < (s_off + n_use); k++)
		{
			float val;
			DWORD val_raw;
			size_t buf_pos = k * i->hdr.num_sig + i->ci[ch].signal_desc.index;

			if (i->ci[ch].bit32)
			{
				DWORD t;
				
				t = buf_blk[buf_pos] | (buf_blk[buf_pos + 1] << 16);
				le32_to_cpu(t);
				val = *(float *)&t;
				val_raw = t;
			}
			else
			{
			        short t;
				
				t = buf_blk[buf_pos];
				le16_to_cpu(t);
				val = t;
				val_raw = t;
			}

			if (num_ret >= num)
			{
				fprintf(stderr, "portilab.c: in _read_raw() data exceed buffersize\n");
				break;
			}
			if (data)
				data[num_ret] = (((double)val - i->ci[ch].signal_desc.min_value_adc) / adc_r) * unit_r + i->ci[ch].signal_desc.min_value_unit;
			if (data_raw)
				data_raw[num_ret] = val_raw;

			num_ret++;
		}

		s_off = 0;	/* start at beginning of next blocks */
	}
	free(buf_blk);
	fclose(fp);

	return num_ret;
} /* read_raw() */


int
read_data_block(struct info *i, size_t num, WORD * buf, FILE * fp)
{
	size_t len = i->hdr.size_sample_data_per_block / 2;

	size_t pos = 217 + i->hdr.num_sig * 136 + num * (86 + i->hdr.size_sample_data_per_block) + 86;
	if (fseek(fp, (long)pos, SEEK_SET) == -1)
	{
		_ra_set_error(i->ra, 0, "plugin-poly5/tms32");
		return -1;
	}

	if (fread(buf, sizeof(WORD), len, fp) != len)
	{
		_ra_set_error(i->ra, 0, "plugin-poly5/tms32");
		return -1;
	}

	return 0;
} /* read_data_block() */
