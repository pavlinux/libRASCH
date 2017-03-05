/*----------------------------------------------------------------------------
 * ART.c  -  Handle signals recorded for ART-Study
 *
 * Description:
 * The plugin handles signals which are recorded for the ART-Study. To access the
 * recorded data, the plugins for portilab and portapres are used.
 *
 * Author(s): Raphael Schneider (librasch@gmail.com)
 *
 * Copyright (C) 2002-2010, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $ID: $
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
#include "ART.h"


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
} /* init_plugin() */


LIBRAAPI int
init_ra_plugin(struct librasch *ra, struct plugin_struct *ps)
{
	if (!ps)
		return -1;

	_porti_plugin = ra_plugin_get_by_name(ra, "poly5/tms32", 1);
	if (!_porti_plugin)
	{
		_ra_set_error(ra, RA_ERR_PLUGIN_MISSING, "plugin-ART");
		return -1;
	}

	_porta_plugin = ra_plugin_get_by_name(ra, "portapres", 1);
	if (!_porta_plugin)
	{
		_ra_set_error(ra, RA_ERR_PLUGIN_MISSING, "plugin-ART");
		return -1;
	}

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

	i = malloc(sizeof(struct info));
	if (!i)
		goto error_open;
	memset(i, 0, sizeof(struct info));

	/* split file in dir and filename w/o extension */
	split_filename(name, SEPARATOR, i->dir, i->name, i->ext);
	if (i->ext[0] == '\0')
		strcpy(i->ext, "ART");

	/* check if files SAMPLES.S00 (from Portilab), art_info.txt (with patient info)
	   and portapres_m2.dat (Posrtapres data file) are available */
	sprintf(file, "%s%c%s.%s%cSAMPLES.S00", i->dir, SEPARATOR, i->name, i->ext, SEPARATOR);
	if ((fp = ra_fopen(file, "rb", 1)) == NULL)
		goto error_open;
	fclose(fp);
	sprintf(file, "%s%c%s.%s%cart_info.txt", i->dir, SEPARATOR, i->name, i->ext, SEPARATOR);
	if ((fp = ra_fopen(file, "rb", 1)) == NULL)
		goto error_open;
	fclose(fp);
	sprintf(file, "%s%c%s.%s%cportapres_m2.dat", i->dir, SEPARATOR, i->name, i->ext, SEPARATOR);
	if ((fp = ra_fopen(file, "rb", 1)) == NULL)
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
	return find_meas(dir, "*.ART", PLUGIN_NAME, f, pl_check_meas);
} /* pl_find_meas() */


meas_handle
pl_open_meas(ra_handle ra, const char *file, int fast)
{
	struct ra_meas *meas;
	struct info *i = NULL;
	struct ra_rec *rh;
	char fn[MAX_PATH_RA];

	if (fast) /* to get rid of compiler warning */
		;

	/* check if signal can be handled with this module */
	if (!pl_check_meas(file))
		goto error_open; /* error will be set by librasch */

	if ((meas = ra_alloc_mem(sizeof(struct ra_meas))) == NULL)
	{
		_ra_set_error(ra, 0, "plugin-ART");
		goto error_open;
	}
	memset(meas, 0, sizeof(struct ra_meas));
	if ((i = calloc(1, sizeof(struct info))) == NULL)
	{
		_ra_set_error(ra, 0, "plugin-ART");
		goto error_open;
	}
	meas->priv = i;
	i->ra = ra;

	/* split file in dir and filename w/o extension */
	split_filename(file, SEPARATOR, i->dir, i->name, i->ext);
	if (i->ext[0] == '\0')
		strcpy(i->ext, "ART");

	if (get_data(i) == -1)
		goto error_open;

	sprintf(fn, "%s%c%s.%s%c", i->dir, SEPARATOR, i->name, i->ext, SEPARATOR);
	find_files(fn, "*", &(i->files), &(i->num_files), &(i->size));

	/* create tree for recordings, use private var to store rec type */
	set_one_session(meas, NULL, NULL);
	meas->sessions[0].root_rec->private_data = (void *)ART_RECORDING;

	rh = meas->sessions[0].root_rec;

	rh->child = ra_alloc_mem(sizeof(struct ra_rec)); /* porti recording */
	memset(rh->child, 0, sizeof(struct ra_rec));
	rh->child->parent = rh;
	rh->child->private_data = (void *)PORTILAB_RECORDING;
	rh->child->meas = meas;
	rh->child->handle_id = RA_HANDLE_REC;

/* TODO: think about how to handle the two recordings correct wrt ra_view */
/*     rh->child->next = ra_alloc_mem(sizeof(struct ra_rec_handle));  /\* portapres recording *\/ */
/*     memset(rh->child->next, 0, sizeof(struct ra_rec_handle)); */
/*     rh->child->next->parent = rh; */
/*     rh->child->next->private_data = (void *)PORTAPRES_RECORDING; */
/*     rh->child->next->meas = meas; */
/*     rh->child->next->handle_id = RA_HANDLE_REC; */
/*     rh->child->next->prev = rh->child; */


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

	if (i->porti_meas)
		ra_meas_close(i->porti_meas);

	if (i->porta_meas)
		ra_meas_close(i->porta_meas);

	if (i->pi.name != NULL)
		free(i->pi.name);
	if (i->pi.forename != NULL)
		free(i->pi.forename);
	if (i->pi.birthday != NULL)
		free(i->pi.birthday);
	if (i->pi.gender != NULL)
		free(i->pi.gender);

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
	int ret;

	if ((ret = open_single_meas(i)) != 0)
		return ret;

	if ((ret = read_patinfo(i)) != 0)
		return ret;

	return ret;
} /* get_data() */


int
open_single_meas(struct info *i)
{
	int ret = 0;
	char fn[MAX_PATH_RA];
	value_handle vh;
	rec_handle rh;
	struct ra_meas *meas;

	sprintf(fn, "%s%c%s.%s%cSAMPLES.S00", i->dir, SEPARATOR, i->name, i->ext, SEPARATOR);
	meas = (struct ra_meas *)_porti_plugin->access.open_meas(i->ra, fn, 0);
	if (!meas)
		return -1;
	/* set infos in meas-struct which are done normally in ra_meas_open() */
	meas->ra = i->ra;
	meas->handle_id = RA_HANDLE_MEAS;
	meas->p = _porti_plugin;
	i->porti_meas = meas;

	sprintf(fn, "%s%c%s.%s%cportapres_m2.dat", i->dir, SEPARATOR, i->name, i->ext, SEPARATOR);
	meas = _porta_plugin->access.open_meas(i->ra, fn, 0);
	if (!meas)
		return -1;
	/* set infos in meas-struct which are done normally in ra_meas_open() */
	meas->ra = i->ra;
	meas->handle_id = RA_HANDLE_MEAS;
	meas->p = _porta_plugin;
	i->porta_meas = meas;

	i->num_channels = 0;
	vh = ra_value_malloc();
	rh = ra_rec_get_first(i->porti_meas, 0);
	ret = _porti_plugin->access.get_info_id(rh, RA_INFO_REC_GEN_NUM_CHANNEL_L, vh);
	if (ret == 0)
		i->num_channels += ra_value_get_long(vh);

	rh = ra_rec_get_first(i->porta_meas, 0);
	ret = _porta_plugin->access.get_info_id(rh, RA_INFO_REC_GEN_NUM_CHANNEL_L, vh);
	if (ret == 0)
		i->num_channels += ra_value_get_long(vh);

	ra_value_free(vh);

	return 0;
} /* open_single_meas() */



int
read_patinfo(struct info *i)
{
	char fn[MAX_PATH_RA];
	char buf[200];
	size_t pos, l;
	FILE *fp;

	sprintf(fn, "%s%c%s.%s%cart_info.txt", i->dir, SEPARATOR, i->name, i->ext, SEPARATOR);
	fp = ra_fopen(fn, "r", 1);
	if (!fp)
	{
		_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-ART");
		return -1;
	}

	memset(buf, 0, sizeof(char) * 200);
	do
	{
		if (fgets(buf, 199, fp) == NULL)
			continue;

		/* kill \n or \r */
		pos = strlen(buf);
		if (pos <= 0)
			continue;
		for (l = pos - 1; l >= 0; l--)
		{
			if ((buf[l] == '\n') || (buf[l] == '\r'))
				buf[l] = '\0';
			else
				break;
		}

		if (strncmp(buf, "name", 4) == 0)
		{
			size_t len = strlen(buf) - 5;
			if (len > 0)
			{
				i->pi.name = malloc(len+1);
				strcpy(i->pi.name, &(buf[5]));
			}
		}
		else if (strncmp(buf, "forename", 8) == 0)
		{
			size_t len = strlen(buf) - 9;
			if (len > 0)
			{
				i->pi.forename = malloc(len+1);
				strcpy(i->pi.forename, &(buf[9]));
			}
		}
		else if (strncmp(buf, "birthday", 8) == 0)
		{
			size_t len = strlen(buf) - 9;
			if (len > 0)
			{
				i->pi.birthday = malloc(len+1);
				strcpy(i->pi.birthday, &(buf[9]));
			}
		}
		else if (strncmp(buf, "gender", 6) == 0)
		{
			size_t len = strlen(buf) - 7;
			if (len > 0)
			{
				i->pi.gender = malloc(len+1);
				strcpy(i->pi.gender, &(buf[7]));
			}
		}
		else if (strncmp(buf, "age", 3) == 0)
		{
			if (strlen(buf) >= 5)
				i->pi.age = atoi(&(buf[4]));
		}
		else if (strncmp(buf, "height", 6) == 0)
		{
			if (strlen(buf) >= 8)
				i->pi.height = atoi(&(buf[7]));
		}
		else if (strncmp(buf, "weight", 6) == 0)
		{
			if (strlen(buf) >= 8)
				i->pi.weight = atoi(&(buf[7]));
		}
		else if (strncmp(buf, "offset", 6) == 0)
		{
			if (strlen(buf) >= 8)
				i->portapres_offset = atoi(buf+7);
		}
	}
	while (!feof(fp));

	fclose(fp);

	return 0;
} /* read_patinfo() */


int
pl_get_info_id(any_handle h, int id, value_handle vh)
{
	int ret = 0;
	int type;
	struct ra_meas *meas;
	struct info *i;
	char t[MAX_PATH_RA];

	meas = ra_meas_handle_from_any_handle(h);
	i = meas->priv;

	/* first try to get info from here */
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
		sprintf(t, "%s%c%s.%s", i->dir, SEPARATOR, i->name, i->ext);
		ra_value_set_string(vh, t);
		break;
	case RA_INFO_DIR_L:
		ra_value_set_long(vh, 1);	/* saved in dir */
		break;
	default:
		ret = get_info_id(h, id, vh);
		break;
	}

	/* if not successfull, try subrecordings (if any) */
	/* TODO: think about if only rec-handles are needed to handled */
	type = *((unsigned short *)h);
	if ((type == RA_HANDLE_REC) && (ret != 0))
	{
		/* check if sub-recording */
		void * rec_type = ((struct ra_rec *)h)->private_data;
		if (rec_type == (void *)PORTILAB_RECORDING)
			return _porti_plugin->access.get_info_id(h, id, vh);
		if (rec_type == (void *)PORTAPRES_RECORDING)
			return _porta_plugin->access.get_info_id(h, id, vh);
	}

	return ret;
} /* pl_get_info_id() */


int
pl_get_info_idx(any_handle h, int info_type, int index, value_handle vh)
{
	int ret = -1;
	int *idx_arr = NULL;
	unsigned short type;

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

	/* first try to get info from here */
	if (idx_arr)
		ret = get_info_id(h, idx_arr[index], vh);

	/* if not successfull, try subrecordings (if any) */
	type = *((unsigned short *)h);
	if ((type == RA_HANDLE_REC) && (ret != 0))
	{
		/* check if sub-recording */
		void *rec_type = ((struct ra_rec *)h)->private_data;
		if (rec_type == (void *)PORTILAB_RECORDING)
			return _porti_plugin->access.get_info_idx(h, info_type, index, vh);
		if (rec_type == (void *)PORTAPRES_RECORDING)
			return _porta_plugin->access.get_info_idx(h, info_type, index, vh);
	}

	if (ret != 0)
		_ra_set_error(h, RA_ERR_UNKNOWN_INFO, "plugin-ART");

	return ret;
} /* pl_get_info_idx() */

int
get_info_id(any_handle h, int id, value_handle vh)
{
	int ret = 0;
	struct ra_meas *meas;
	struct info *i;
	struct meta_info *meta_inf;

	/* all var's ok? */
	if ((vh == NULL) || (h == NULL))
	{
		_ra_set_error(h, RA_ERR_ERROR, "plugin-ART");
		return -1;
	}

	/* get infos about asked measurement info */
	meta_inf = get_meta_info(id);
	if (meta_inf == NULL)
	{
		_ra_set_error(h, RA_ERR_UNKNOWN_INFO, "plugin-ART");
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
	rec_handle rh;

	switch (id)
	{
	case RA_INFO_REC_GEN_TYPE_L:
		ra_value_set_long(vh, RA_REC_TYPE_TIMESERIES);
		break;
	case RA_INFO_REC_GEN_NAME_C:
		ra_value_set_string(vh, i->name);
		break;
	case RA_INFO_REC_GEN_DESC_C:
		ra_value_set_string(vh, "ART");
		break;
	case RA_INFO_REC_GEN_NUMBER_L:
		ra_value_set_long(vh, 0);
		break;
	case RA_INFO_REC_GEN_NUM_SUB_REC_L:
		ra_value_set_long(vh, 2);
		break;
	case RA_INFO_REC_GEN_NUM_CHANNEL_L:
		ra_value_set_long(vh, i->num_channels);
		break;
	case RA_INFO_REC_GEN_OFFSET_L:
		ra_value_set_long(vh, 0);
		break;
		/* take time and date infos from portilab recording */
	case RA_INFO_REC_GEN_TIME_C:
	case RA_INFO_REC_GEN_DATE_C:
	case RA_INFO_REC_GEN_DURATION_SEC_D:
		rh = ra_rec_get_first(i->porti_meas, 0);
		ret = _porti_plugin->access.get_info_id(rh, id, vh);
		break;
	case RA_INFO_REC_GEN_PATH_C:
		sprintf(t, "%s%c%s.%s", i->dir, SEPARATOR, i->name, i->ext);
		ra_value_set_string(vh, t);
		break;
	case RA_INFO_REC_GEN_DIR_L:
		ra_value_set_long(vh, 1);	/* saved in dir */
		break;
	case RA_INFO_REC_GEN_MM_SEC_D:
		ra_value_set_double(vh, 50.0);
		break;
	case RA_INFO_REC_CH_NAME_C:
		switch (ra_value_get_number(vh))
		{
		case 0:
			ra_value_set_string(vh, "ECG-X");
			break;	/* ecg-X */
		case 1:
			ra_value_set_string(vh, "ECG-Y");
			break;	/* ecg-Y */
		case 2:
			ra_value_set_string(vh, "ECG-Z");
			break;	/* ecg-Z */
		case 3:
			ra_value_set_string(vh, "ABP");
			break;	/* blood pressure */
		case 4:
			ra_value_set_string(vh, "RESP");
			break;	/* breathing */
		case 5:
			ra_value_set_string(vh, "ABP (Portapres)");
			break;	/* blood pressure (Portapres recording) */
		}
		break;
	case RA_INFO_REC_CH_CENTER_VALUE_D:
		/* channel 3 and 5 (portapres recording) are blood pressure */
		if ((ra_value_get_number(vh) == 3) || (ra_value_get_number(vh) == 5))
			ret = -1;
		else
			ra_value_set_double(vh, 0.0);
		break;
	case RA_INFO_REC_CH_CENTER_SAMPLE_D:
		ra_value_set_double(vh, 0.0);
		break;
	case RA_INFO_REC_CH_MIN_UNIT_D:
	case RA_INFO_REC_CH_MAX_UNIT_D:
		ret = get_info_rec_ch(i, id, vh);
		break;
	case RA_INFO_REC_CH_MM_UNIT_D:
		switch (ra_value_get_number(vh))
		{
		case 0:
			ra_value_set_double(vh, 0.005);
			break;	/* ecg-X */
		case 1:
			ra_value_set_double(vh, 0.005);
			break;	/* ecg-Y */
		case 2:
			ra_value_set_double(vh, 0.005);
			break;	/* ecg-Z */
		case 3:
			ra_value_set_double(vh, 0.2);
			break;	/* blood pressure */
		case 4:
			ra_value_set_double(vh, 10);
			break;	/* breathing */
		case 5:
			ra_value_set_double(vh, 0.2);
			break;	/* blood pressure (Portapres recording) */
		}
		break;
	case RA_INFO_REC_CH_TYPE_L:
		switch (ra_value_get_number(vh))
		{
		case 0:
			ra_value_set_long(vh, RA_CH_TYPE_ECG);
			break;
		case 1:
			ra_value_set_long(vh, RA_CH_TYPE_ECG);
			break;
		case 2:
			ra_value_set_long(vh, RA_CH_TYPE_ECG);
			break;
		case 3:
			ra_value_set_long(vh, RA_CH_TYPE_RR);
			break;
		case 4:
			ra_value_set_long(vh, RA_CH_TYPE_RESP);
			break;
		case 5:
			ra_value_set_long(vh, RA_CH_TYPE_RR);
			break;
		}
		break;
	default:
		ret = get_info_rec_ch(i, id, vh);
		break;
	}

	return ret;
} /* get_info_rec() */

int
get_info_rec_ch(struct info *i, int id, value_handle vh)
{
	int ret = 0;

	if ((id >= RA_INFO_REC_CHANNEL_START) && (id < RA_INFO_REC_CHANNEL_END))
	{
		if ((ra_value_get_number(vh) < 0) || (ra_value_get_number(vh) >= i->num_channels))
			return -1;

		if (ra_value_get_number(vh) < NUM_PORTILAB_CHANNELS)
		{
			rec_handle rh = ra_rec_get_first(i->porti_meas, 0);

			ret = _porti_plugin->access.get_info_id(rh, id, vh);
		}
		else
		{
			long n;

			rec_handle rh = ra_rec_get_first(i->porta_meas, 0);

			n = ra_value_get_number(vh);
			ra_value_set_number(vh, (n - NUM_PORTILAB_CHANNELS));
			ret = _porta_plugin->access.get_info_id(rh, id, vh);
			ra_value_set_number(vh, n);
		}
	}
	else
	{
		_ra_set_error(i->ra, RA_ERR_UNKNOWN_INFO, "plugin-ART");
		ret = -1;
	}

	return ret;
} /* get_info_rec_ch() */


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
	case RA_INFO_OBJ_PERSON_GENDER_C:
		set_utf8(vh, i->pi.gender);
		ra_value_info_set_modifiable(vh, 1);
		break;
	case RA_INFO_OBJ_PERSON_AGE_L:
		ra_value_set_long(vh, i->pi.age);
		ra_value_info_set_modifiable(vh, 1);
		break;
	case RA_INFO_OBJ_PERSON_HEIGHT_L:
		ra_value_set_long(vh, i->pi.height);
		ra_value_info_set_modifiable(vh, 1);
		break;
	case RA_INFO_OBJ_PERSON_WEIGHT_L:
		ra_value_set_long(vh, i->pi.weight);
		ra_value_info_set_modifiable(vh, 1);
		break;

	default:
		_ra_set_error(i->ra, RA_ERR_UNKNOWN_INFO, "plugin-ART");
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

	_ra_set_error(i->ra, RA_ERR_NONE, "plugin-pathfinder");

	/* At the moment only the patient data can be set.
	   TODO: add code to set rec-date and time (write it to art_info.txt AND
	         handle it in the Portilab and Portapres files/plugins) */
	if (id == -1)
		id_use = ra_value_get_info(vh);
	else
		id_use = id;
	switch (id_use)
	{
	case RA_INFO_OBJ_PERSON_NAME_C:
		ret = write_single_pat_info(i, &(i->pi.name), "Patient", "name", vh);
		break;
	case RA_INFO_OBJ_PERSON_FORENAME_C:
		ret = write_single_pat_info(i, &(i->pi.forename), "Patient", "forename", vh);
		break;
	case RA_INFO_OBJ_PERSON_BIRTHDAY_C:
		ret = write_single_pat_info(i, &(i->pi.birthday), "Patient", "birthday", vh);
		break;
	case RA_INFO_OBJ_PERSON_GENDER_C:
		ret = write_single_pat_info(i, &(i->pi.gender), "Patient", "gender", vh);
		break;
	case RA_INFO_OBJ_PERSON_AGE_L:
		i->pi.age = ra_value_get_long(vh);
		ret = write_single_pat_info(i, NULL, "Patient", "age", vh);
		break;
	case RA_INFO_OBJ_PERSON_HEIGHT_L:
		i->pi.height = ra_value_get_long(vh);
		ret = write_single_pat_info(i, NULL, "Patient", "height", vh);
		break;
	case RA_INFO_OBJ_PERSON_WEIGHT_L:
		i->pi.weight = ra_value_get_long(vh);
		ret = write_single_pat_info(i, NULL, "Patient", "weight", vh);
		break;
	default:
		_ra_set_error(i->ra, RA_ERR_UNSUPPORTED, "plugin-pathfinder");
		ret = -1;
		break;
	}
	
	return ret;
} /* pl_set_info() */


int
write_single_pat_info(struct info *i, char **store, const char *group, const char *item, value_handle data)
{
	const char *s;
	char fn[MAX_PATH_RA], fn_temp[MAX_PATH_RA];
	FILE *fp, *fp_temp;
	int in_group = 0;
	char buf[BUF_SIZE];

	if (store)
	{
		s = ra_value_get_string(data);
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

	/* write art_info.txt file */
	sprintf(fn, "%s%c%s.%s%cart_info.txt", i->dir, SEPARATOR, i->name, i->ext, SEPARATOR);
	sprintf(fn_temp, "%s%c%s.%s%cart_info.txt.tmp", i->dir, SEPARATOR, i->name, i->ext, SEPARATOR);
	if ((fp = ra_fopen(fn, "r", 1)) == NULL)
	{
		_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-ART");
		return -1;
	}
	if ((fp_temp = ra_fopen(fn_temp, "w", 1)) == NULL)
	{
		fclose(fp);
		_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-ART");
		return -1;
	}
	
	while (fgets(buf, BUF_SIZE, fp))
	{
		if (buf[0] == '[')
		{
			if (strncmp(buf+1, group, strlen(group)) == 0)
				in_group = 1;
			else
				in_group = 0;
			fputs(buf, fp_temp);
			continue;
		}
		if (!in_group)
		{
			fputs(buf, fp_temp);
			continue;
		}
		if (strncmp(buf, item, strlen(item)) == 0)
		{
			/* we have string and long values */
			if (ra_value_get_type(data) == RA_VALUE_TYPE_CHAR)
			{
				char *utf8_buf = NULL;
				size_t len;
				
				len = strlen(ra_value_get_string_utf8(data)) + 1;
				utf8_buf = calloc(len, sizeof(char));
				if (len > 1)
					from_utf8(ra_value_get_string_utf8(data), utf8_buf, len, "ISO8859-1");
				fprintf(fp_temp, "%s=%s\n", item, utf8_buf);
				free(utf8_buf);
			}
			else
				fprintf(fp_temp, "%s=%ld\n", item, ra_value_get_long(data));
		}
		else
			fputs(buf, fp_temp);
	}

	fclose(fp);
	fclose(fp_temp);

	delete_file(fn);
	move_file(fn_temp, fn);

	return 0;
} /* write_single_pat_info() */


size_t
pl_get_raw(meas_handle mh, rec_handle rh, int ch, size_t start, size_t num_data, DWORD *data, DWORD *data_high)
{
	size_t num = 0;
	struct info *i = ((struct ra_meas *) mh)->priv;
	void *rec_type = NULL;

	if (rh != NULL)
		rec_type = ((struct ra_rec *) rh)->private_data;

	/* if no rec.handle or topmost rec.handle, decide which rec. by channel number */
	if ((rh == NULL) || ((rec_type != (void *)PORTILAB_RECORDING) && (rec_type != (void *)PORTAPRES_RECORDING)))
	{
		if (ch < NUM_PORTILAB_CHANNELS)
			num = _porti_plugin->access.get_raw(i->porti_meas, NULL, ch, start, num_data, data, data_high);
		else
		{
			num = _porta_plugin->access.get_raw(i->porta_meas, NULL, ch - NUM_PORTILAB_CHANNELS,
							    start - i->portapres_offset, num_data, data, data_high);
		}
	}
	else if (rec_type == (void *)PORTILAB_RECORDING)
		num = _porti_plugin->access.get_raw(i->porti_meas, NULL, ch, start, num_data, data, data_high);
	else if (rec_type == (void *)PORTAPRES_RECORDING)
	{
		num = _porta_plugin->access.get_raw(i->porta_meas, NULL, ch - NUM_PORTILAB_CHANNELS,
						    start - i->portapres_offset, num_data, data, data_high);
	}
	else
		_ra_set_error(mh, RA_ERR_ERROR, "plugin-ART");

	return num;
} /* pl_get_raw() */


size_t
pl_get_raw_unit(meas_handle mh, rec_handle rh, int ch, size_t start, size_t num_data, double *data)
{
	size_t num = 0;
	struct info *i = ((struct ra_meas *) mh)->priv;
	void *rec_type = NULL;

	if (rh != NULL)
		rec_type = ((struct ra_rec *) rh)->private_data;

	/* if no rec.handle or topmost rec.handle, decide which rec. by channel number */
	if ((rh == NULL) || ((rec_type != (void *)PORTILAB_RECORDING) && (rec_type != (void *)PORTAPRES_RECORDING)))
	{
		if (ch < NUM_PORTILAB_CHANNELS)
			num = _porti_plugin->access.get_raw_unit(i->porti_meas, NULL, ch, start, num_data, data);
		else
		{
			num = _porta_plugin->access.get_raw_unit(i->porta_meas, NULL, ch - NUM_PORTILAB_CHANNELS,
								 start - i->portapres_offset, num_data, data);
		}
	}
	else if (rec_type == (void *)PORTILAB_RECORDING)
		num = _porti_plugin->access.get_raw_unit(i->porti_meas, NULL, ch, start, num_data, data);
	else if (rec_type == (void *)PORTAPRES_RECORDING)
	{
		num = _porta_plugin->access.get_raw_unit(i->porta_meas, NULL, ch - NUM_PORTILAB_CHANNELS,
							 start - i->portapres_offset, num_data, data);
	}
	else
		_ra_set_error(mh, RA_ERR_ERROR, "plugin-ART");

	return num;
} /* pl_get_raw_unit() */
