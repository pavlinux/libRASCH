/*----------------------------------------------------------------------------
 * edf.c  -  Handle signals recorded using European Data Format(+)
 *
 * Description:
 * The plugin provides access to recordings saved in the European Data Format (EDF)
 * and the EDF+ format.
 *
 * Remarks:
 * If the EDF+ file is a non-contiguous signal, than each data-block is
 * handled as a single session. (Recommendation from Jesus Olivan.)
 *
 * Author(s): Raphael Schneider (librasch@gmail.com)
 *
 * Copyright (C) 2003-2009, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define _LIBRASCH_BUILD
#include <ra.h>
#include <ra_priv.h>
#include <handle_plugin.h>

#include <ra_endian.h>
#include <pl_general.h>
#include <ra_file.h>
#include <ra_estimate_ch_infos.h>

#include "edf.h"


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

	ps->access.get_eval_info = pl_get_eval_info;
	ps->access.get_class_num = pl_get_class_num;
	ps->access.get_class_info = pl_get_class_info;
	ps->access.get_prop_num = pl_get_prop_num;
	ps->access.get_prop_info = pl_get_prop_info;
	ps->access.get_ev_info = pl_get_ev_info;
	ps->access.get_ev_value = pl_get_ev_value;

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
	struct edf_header_ascii hdr;

	i = malloc(sizeof(struct info));
	if (!i)
		goto error_open;
	memset(i, 0, sizeof(struct info));

	/* split file in dir and filename w/o extension */
	split_filename(name, SEPARATOR, i->dir, i->name, i->ext);
	/* if no extension given, try if there is a file with either 'edf'
	   or 'rec' extension */
	if (i->ext[0] == '\0')
	{
		if (get_extension(i) != 0)
			goto error_open;
	}

	/* if extension is not 'edf' or 'rec' file don't need to be an EDF-file */
	if ((RA_STRICMP(i->ext, "edf") != 0) && (RA_STRICMP(i->ext, "rec") != 0))
		goto error_open;

	sprintf(fn, "%s%c%s.%s", i->dir, SEPARATOR, i->name, i->ext);
	fp = ra_fopen(fn, "rb", 0);
	if (fp == NULL)
		goto error_open;
	if (fread(&hdr, sizeof(struct edf_header_ascii), 1, fp) != 1)
		goto error_open;

	/* in EDF and EDF+, the version field contains '0   ' */
	if (strncmp(hdr.version, "0   ", 4) != 0)
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
get_extension(struct info *i)
{
	int j;
	char fn[MAX_PATH_RA];
	FILE *fp;

	for (j = 0; j < _num_supported_extensions; j++)
	{
		sprintf(fn, "%s%c%s.%s", i->dir, SEPARATOR, i->name, _supported_extensions[j]);
		if ((fp = ra_fopen(fn, "r", 0)) != NULL)
		{
			fclose(fp);
			strcpy(i->ext, _supported_extensions[j]);
			break;
		}
	}
	
	if (i->ext[0] == '\0')
		return -1;

	return 0;
} /* get_extension() */


int
pl_find_meas(const char *dir, struct find_meas *f)
{
	int ret = 1;

	if (find_meas(dir, "*.edf", PLUGIN_NAME, f, pl_check_meas) == 0)
		ret = 0;
	if (find_meas(dir, "*.rec", PLUGIN_NAME, f, pl_check_meas) == 0)
		ret = 0;

	return ret;
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
	if (i->ext[0] == '\0')
		if (get_extension(i) != 0)
			goto error_open;

	if (get_data(i) == -1)
		goto error_open;

	sprintf(mask, "%s*", i->name);
	find_files(i->dir, mask, &(i->files), &(i->num_files), &(i->size));

	/* set session infos */
	if (i->non_contiguous)
		set_edf_sessions(meas);
	else
		set_one_session(meas, NULL, NULL);

	return meas;

 error_open:
	perror(__FILE__);
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

	if (i->annot)
		free(i->annot);

	if (i->session)
		free(i->session);

	if (i->start_time_rec_block)
		free(i->start_time_rec_block);

	free(i);
} /* free_info_struct() */


int
get_data(struct info *i)
{
	int ret = -1;
	char file[MAX_PATH_RA];
	char file_wo_ext[MAX_PATH_RA];
	FILE *fp;
	struct edf_header_ascii hdr;
	int j, num_annot_ch;
	int *annot_ch = NULL;
	char *buf = NULL;
	long ch_offset;
	int num_ch_sub = 0;
	size_t buf_size;

	sprintf(file, "%s%c%s.%s", i->dir, SEPARATOR, i->name, i->ext);
	fp = ra_fopen(file, "rb", 0);
	if (fp == NULL)
	{
		_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-edf");
		goto error;
	}

	memset(&hdr, 0, sizeof(struct edf_header_ascii));
	if (fread(&hdr, sizeof(struct edf_header_ascii), 1, fp) != 1)
	{
		_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-edf");
		goto error;
	}

	memset(&(i->hdr), 0, sizeof(struct edf_header));
	
	strncpy(i->hdr.version, hdr.version, 8);

	strncpy(i->hdr.pat_id, hdr.pat_id, 80);	
	for (j = (int)strlen(i->hdr.pat_id)-1; j >= 0; j--) /* remove trailing spaces */
	{
		if (i->hdr.pat_id[j] != ' ')
			break;

		i->hdr.pat_id[j] = '\0';
	}

	strncpy(i->hdr.rec_id, hdr.rec_id, 80);
	for (j = (int)strlen(i->hdr.rec_id)-1; j >= 0; j--) /* remove trailing spaces */
	{
		if (i->hdr.rec_id[j] != ' ')
			break;

		i->hdr.rec_id[j] = '\0';
	}
	
	i->hdr.rec_ts.day = ((hdr.rec_date[0]-'0')*10) + (hdr.rec_date[1]-'0');
	i->hdr.rec_ts.mon = ((hdr.rec_date[3]-'0')*10) + (hdr.rec_date[4]-'0');
	i->hdr.rec_ts.year = ((hdr.rec_date[6]-'0')*10) + (hdr.rec_date[7]-'0');
	/* handle y2k */
	if (i->hdr.rec_ts.year >= 85)
		i->hdr.rec_ts.year += 1900;
	else
		i->hdr.rec_ts.year += 2000;
	i->hdr.rec_ts.hour = ((hdr.rec_time[0]-'0')*10) + (hdr.rec_time[1]-'0');
	i->hdr.rec_ts.min = ((hdr.rec_time[3]-'0')*10) + (hdr.rec_time[4]-'0');
	i->hdr.rec_ts.sec = ((hdr.rec_time[6]-'0')*10) + (hdr.rec_time[7]-'0');
	i->hdr.num_hdr_bytes = atol(hdr.num_hdr_bytes);
	i->hdr.num_recs = atol(hdr.num_recs);
	i->hdr.duration = (double)atof(hdr.duration);
	i->hdr.num_ch = atol(hdr.num_ch);

	if (strncmp(hdr.reserved, "EDF+", 4) == 0)
	{
		i->edf_plus = 1;
		if (hdr.reserved[4] == 'D')
			i->non_contiguous = 1;

		/* when I understand the description of EDF+ correct, then only in the EDf+ format
		   the subfields of the patient-id and the recording-id field have a specific format */
		process_pat_id(i, i->hdr.pat_id);
		process_rec_id(i, i->hdr.rec_id);
	}

	i->ci = calloc(i->hdr.num_ch, sizeof(struct channel_info));
	/* alloc mem for the largest "object" */
	buf_size = sizeof(char) * 80 * i->hdr.num_ch;
	buf = malloc(buf_size);
	/* label */
	memset(buf, '\0', buf_size);
	if (fread(buf, 16, i->hdr.num_ch, fp) != (size_t)i->hdr.num_ch)
	{
		_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-edf");
		goto error;
	}
	for (j = 0; j < i->hdr.num_ch; j++)
		strncpy(i->ci[j].label, &(buf[j*16]), 16);
	/* transducer */
	memset(buf, '\0', buf_size);
	if (fread(buf, 80, i->hdr.num_ch, fp) != (size_t)i->hdr.num_ch)
	{
		_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-edf");
		goto error;
	}
	for (j = 0; j < i->hdr.num_ch; j++)
		strncpy(i->ci[j].transducer, &(buf[j*80]), 80);
	/* unit */
	memset(buf, '\0', buf_size);
	if (fread(buf, 8, i->hdr.num_ch, fp) != (size_t)i->hdr.num_ch)
	{
		_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-edf");
		goto error;
	}
	for (j = 0; j < i->hdr.num_ch; j++)
		strncpy(i->ci[j].unit, &(buf[j*8]), 8);
	/* physical minimum */
	memset(buf, '\0', buf_size);
	if (fread(buf, 8, i->hdr.num_ch, fp) != (size_t)i->hdr.num_ch)
	{
		_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-edf");
		goto error;
	}
	for (j = 0; j < i->hdr.num_ch; j++)
		i->ci[j].phys_min = atof(&(buf[j*8]));
	/* physical maximum */
	memset(buf, '\0', buf_size);
	if (fread(buf, 8, i->hdr.num_ch, fp) != (size_t)i->hdr.num_ch)
	{
		_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-edf");
		goto error;
	}
	for (j = 0; j < i->hdr.num_ch; j++)
		i->ci[j].phys_max = atof(&(buf[j*8]));
	/* digital minimun */
	memset(buf, '\0', buf_size);
	if (fread(buf, 8, i->hdr.num_ch, fp) != (size_t)i->hdr.num_ch)
	{
		_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-edf");
		goto error;
	}
	for (j = 0; j < i->hdr.num_ch; j++)
		i->ci[j].digital_min = atol(&(buf[j*8]));
	/* digital maximum */
	memset(buf, '\0', buf_size);
	if (fread(buf, 8, i->hdr.num_ch, fp) != (size_t)i->hdr.num_ch)
	{
		_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-edf");
		goto error;
	}
	for (j = 0; j < i->hdr.num_ch; j++)
		i->ci[j].digital_max = atol(&(buf[j*8]));
	/* prefiltering */
	memset(buf, '\0', buf_size);
	if (fread(buf, 80, i->hdr.num_ch, fp) != (size_t)i->hdr.num_ch)
	{
		_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-edf");
		goto error;
	}
	for (j = 0; j < i->hdr.num_ch; j++)
		strncpy(i->ci[j].prefiltering, &(buf[j*80]), 80);
	/* nr of samples */
	memset(buf, '\0', buf_size);
	if (fread(buf, 8, i->hdr.num_ch, fp) != (size_t)i->hdr.num_ch)
	{
		_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-edf");
		goto error;
	}
	ch_offset = 0;
	num_annot_ch = 0;
	for (j = 0; j < i->hdr.num_ch; j++)
	{
		size_t k;
		char lc_label[MAX_FIELD_LEN];

		i->ci[j].num_samples = atol(&(buf[j*8]));

		/* calc channel samplerate */
		if (i->hdr.duration > 0)
			i->ci[j].samplerate = (double)(i->ci[j].num_samples)/i->hdr.duration;

		/* calc channel start-pos */
		i->ci[j].start_filepos = i->hdr.num_hdr_bytes + ch_offset;
		ch_offset += (i->ci[j].num_samples * 2);

		/* check if values stored are float values */
		if (strncmp(i->ci[j].unit, "Filtered", 8) == 0)
		{
			double *val;
			double ymin, a;

			i->ci[j].float_values = 1;

			i->ci[j].ymin = atof(i->ci[j].prefiltering+25);
			i->ci[j].a = atof(i->ci[j].prefiltering+37);
			strncpy(i->ci[j].unit, i->ci[j].prefiltering+14, 8);
			i->ci[j].unit[8] = '\0';

			/* correct phys-min/-max */
			ymin = i->ci[j].ymin;
			a = i->ci[j].a;

			val = &(i->ci[j].phys_min);
			if (*val < 0)
				*val = (float)(-ymin * exp(-a * (*val)));
			if (*val > 0)
				*val = (float)(ymin * exp(a * (*val)));
			val = &(i->ci[j].phys_max);
			if (*val < 0)
				*val = (float)(-ymin * exp(-a * (*val)));
			if (*val > 0)
				*val = (float)(ymin * exp(a * (*val)));
		}

		/* remove blanks after unit */
		for (k = strlen(i->ci[j].unit)-1; k >= 0; k--)
		{
			if (i->ci[j].unit[k] != ' ')
			{
				i->ci[j].unit[k+1] = '\0';
				break;
			}
		}
		
		/* remove blanks after the channel label */
		for (k = strlen(i->ci[j].label)-1; k >= 0; k--)
		{
			if (i->ci[j].label[k] != ' ')
			{
				i->ci[j].label[k+1] = '\0';
				break;
			}
		}

		/* check if annotation-channel
		   (suggestion from Jesus Olivan: it is an annotation channel,
		   when the word 'annotation' (ignore case) can be found in the label) */
		memset(lc_label, 0, MAX_FIELD_LEN);
		for (k = 0; k < strlen(i->ci[j].label); k++)
			lc_label[k] = (char)tolower(i->ci[j].label[k]);
		/* if (i->edf_plus && (strstr(lc_label, "annotation") != NULL)) */
		if (strstr(lc_label, "annotation") != NULL)
		{
			num_annot_ch++;
			annot_ch = (int *)realloc(annot_ch, sizeof(int) * num_annot_ch);
			annot_ch[num_annot_ch-1] = j;
			num_ch_sub++;
			continue;
		}

		/* get max. samplerate. this is needed to give annotation timestamps in sample units */
		if (i->max_samplerate < i->ci[j].samplerate)
			i->max_samplerate = i->ci[j].samplerate;
	}
	i->hdr.num_ch -= num_ch_sub; /* annotation-channel(s) will not be counted */
	i->record_size = ch_offset;

	/* there are some recordings (e.g. when exported from IDEEQ), the 'EDF+' identifier
	   is not set correctly; but when we have a channel with 'annotation' in is name,
	   it is presumably an EDF+ file */
	if (!i->edf_plus && (num_annot_ch > 0))
	{
		i->edf_plus = 1;
		process_pat_id(i, i->hdr.pat_id);
		process_rec_id(i, i->hdr.rec_id);
	}


	if (i->hdr.num_recs == -1)
		get_number_of_records(i, fp);

	/* handle annotation channel here, after all needed infos are set */
	if (num_annot_ch > 0)
	{
		i->start_time_rec_block = malloc(sizeof(double) * i->hdr.num_recs);
		for (j = 0; j < num_annot_ch; j++)
			handle_annotations(i, annot_ch[j], fp);
	}

	/* try to get channel infos */
	i->ch_type_infos = malloc(sizeof(struct ra_est_ch_infos) * i->hdr.num_ch);
	memset(i->ch_type_infos, 0, sizeof(struct ra_est_ch_infos) * i->hdr.num_ch);
	i->mm_sec = 1;
	sprintf(file_wo_ext, "%s%c%s", i->dir, SEPARATOR, i->name);
	for (j = 0; j < i->hdr.num_ch; j++)
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

	if (i->non_contiguous && (num_annot_ch > 0)) /* annot-ch needed for start-time of rec-block */
	{
		if ((ret = combine_sessions(i)) != 0)
		    goto error;
	}
	else
	{
		i->num_sessions = 1;
		i->session = calloc(1, sizeof(struct session_info));
		i->session[0].start_blk = 0;
		i->session[0].end_blk = i->hdr.num_recs-1;
	}

	ret = 0;

 error:
	if (fp)
		fclose(fp);
	if (buf)
		free(buf);
	if (annot_ch)
		free(annot_ch);

	return ret;
} /* get_data() */


void
process_pat_id(struct info *i, char *id_text)
{
	char *p;

	p = get_sub_field(id_text, i->pi.id);
	p = get_sub_field(p, i->pi.gender);
	p = get_sub_field(p, i->pi.birthday);
	p = get_sub_field(p, i->pi.name);

	p = strchr(i->pi.name, '_');
	if (p)
	{
		*p = '\0';
		strncpy(i->pi.forename, p+1, MAX_FIELD_LEN);
	}
} /* process_pat_id() */


void
process_rec_id(struct info *i, char *id_text)
{
	char *p, dummy[MAX_FIELD_LEN];

	p = get_sub_field(id_text, dummy);
	p = get_sub_field(p, i->ri.startdate);
	p = get_sub_field(p, i->ri.investigation_code);
	p = get_sub_field(p, i->ri.investigator_code);
	p = get_sub_field(p, i->ri.equipment_code);
} /* process_rec_id() */


char *
get_sub_field(char *field, char *dest)
{
	char *next;

	if (!field)
		return NULL;

	next = strchr(field, ' ');
	if (next)
		*next = '\0';
	if (field[0] != 'X')
		strncpy(dest, field, MAX_FIELD_LEN);
	
	if (!next)
		return NULL;
	else
		return (next+1);
} /* get_sub_field() */


int
get_number_of_records(struct info *i, FILE *fp)
{
	long len, save;

	save = ftell(fp);
	fseek(fp, 0, SEEK_END);
	len = ftell(fp);
	fseek(fp, save, SEEK_SET);

	len -= i->hdr.num_hdr_bytes;
	i->hdr.num_recs = len / (long)i->record_size;

	return 0;
} /* get_number_of_records() */


int
handle_annotations(struct info *i, int ch, FILE *fp)
{
	char *buf;
	long l, fpos_save, buf_size;

	fpos_save = ftell(fp);

	buf_size = i->ci[ch].num_samples * 2;
	buf = malloc(sizeof(char) * buf_size);
	for (l = 0; l < i->hdr.num_recs; l++)
	{
		char *p = NULL;
		long pos;

		pos = (long)i->ci[ch].start_filepos + (l * (long)i->record_size);
		fseek(fp, pos, SEEK_SET);
		
		fread(buf, 1, buf_size, fp);
		p = buf;
		do
		{
			long size = buf_size - (p - buf);
			p = get_annot(i, l, p, size);
		} while (p != NULL);
	}

	free(buf);
	fseek(fp, fpos_save, SEEK_SET);
	
	return 0;
} /* handle_annotations() */


char *
get_annot(struct info *i, long record_block, char *p, long buf_size)
{
	double onset, duration;
/*	char *p_on, *p_dur, *p_text; */
	long l, curr;
	int dur_avail;

	if (buf_size <= 0)
		return NULL;

	/* first get times */
	dur_avail = 0;
	curr = 0;
	onset = 0.0;
	duration = 0.0;
	for (l = curr; l < buf_size; l++)
	{
		if ((p[l] == 20) || (p[l] == 21))
		{
			if (p[l] == 21)
				dur_avail = 1;
			p[l] = '\0';
			onset = atof(p+curr);
			curr = l+1;
			break;
		}
	}
	if (dur_avail)
	{
		for (l = curr; l < buf_size; l++)
		{
			if (p[l] == 20)
			{
				p[l] = '\0';
				duration = atof(p+curr);
				curr = l+1;
				break;
			}
		}
	}

	/* now get annotations */
	for (l = curr; l < buf_size; l++)
	{
		if (p[l] != 20)
			continue;
	
		if (l == curr)
		{
			/* starting time of the current record-block */
			i->start_time_rec_block[record_block] = onset;
			curr = l+1;
			if (p[l+1] == 0)
				break;
			continue;
		}

		p[l] = '\0';	/* set end-of-string at position of seperator [20] */

		i->num_annot++;
		i->annot = realloc(i->annot, sizeof(struct annotations) * i->num_annot);
		i->annot[i->num_annot-1].onset = onset;
		i->annot[i->num_annot-1].duration = duration;
		duration = 0;	/* TODO: read EDF+ specification why this is necessary */
		i->annot[i->num_annot-1].text = calloc(strlen(p+curr)+1, sizeof(char));
		strcpy(i->annot[i->num_annot-1].text, (p+curr));

		curr = l+1;

		if (p[l+1] == 0)
			break;
	}

	return (p + l + 2);
} /* get_annot() */


int
combine_sessions(struct info *i)
{
	long l;
	double next_start = -1.0;

	i->num_sessions = 0;
	i->session = NULL;
	for (l = 0; l < i->hdr.num_recs; l++)
	{
		if (next_start != i->start_time_rec_block[l])
		{
			i->num_sessions++;
			i->session = realloc(i->session, sizeof(struct session_info) * i->num_sessions);
			i->session[i->num_sessions-1].start_blk = l;
			next_start = i->start_time_rec_block[l];
		}

		next_start += i->hdr.duration;

		/* check if end of session */
		if ((l == (i->hdr.num_recs-1)) || (next_start != i->start_time_rec_block[l+1]))
		{
			/* then set remaining data of current session */
			i->session[i->num_sessions-1].end_blk = l;
		}
	}

	return 0;
} /* combine_sessions() */


int
set_edf_sessions(struct ra_meas *meas)
{
	long l;
	struct info *i = (struct info *)meas->priv;

	if (!i->non_contiguous)	/* only non-contiguous signals have multiple sessions */
		return 0;

	meas->sessions = ra_alloc_mem(sizeof(struct ra_session) * i->num_sessions);
	memset(meas->sessions, 0, sizeof(struct ra_session) * i->num_sessions);
	meas->num_sessions = i->num_sessions;

	for (l = 0; l < i->num_sessions; l++)
	{
/*  Perhaps this will be set in the future, leave it here to have a clue that this information can be set.
		if (name)
			strncpy(meas->sessions[0].name, name, RA_INFO_NAME_MAX);
		if (desc)
		strncpy(meas->sessions[0].desc, desc, RA_INFO_DESC_MAX);*/
		
		meas->sessions[l].root_rec = ra_alloc_mem(sizeof(struct ra_rec));
		memset(meas->sessions[l].root_rec, 0, sizeof(struct ra_rec));
		meas->sessions[l].root_rec->meas = meas;
		meas->sessions[l].root_rec->handle_id = RA_HANDLE_REC;

#ifdef WIN32
#pragma warning(push)
#pragma warning(disable:4312)
#endif
		meas->sessions[l].root_rec->private_data = (void *)l; /* save index */
#ifdef WIN32
#pragma warning(pop)
#endif
	}

	return 0;
} /* set_edf_sessions() */


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
		ra_value_set_string_utf8(vh, t);
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

	_ra_set_error(h, RA_ERR_UNKNOWN_INFO, "plugin-edf");

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
		_ra_set_error(h, RA_ERR_ERROR, "plugin-edf");
		return -1;
	}

	set_meta_info(vh, meta_inf->name, meta_inf->desc, meta_inf->id);

	meas = ra_meas_handle_from_any_handle(h);
	i = meas->priv;

	/* get asked information */
	if ((id >= RA_INFO_RECORDING_START) && (id < RA_INFO_RECORDING_END))
		ret = get_info_rec(h, i, id, vh);
	if ((id >= RA_INFO_OBJECT_START) && (id < RA_INFO_OBJECT_END))
		ret = get_info_obj(i, id, vh);

	return ret;
} /* get_info_id() */


int
get_info_rec(any_handle h, struct info *i, int id, value_handle vh)
{
	int ret = 0;
	char t[MAX_PATH_RA];
	double d;
	int r, sec, min, hour;
	struct ra_rec *rec = (struct ra_rec *)h;
	long ch, session;

	if (!rec || (rec->handle_id != RA_HANDLE_REC))
		return -1;

	ch = ra_value_get_number(vh);

	if ((id >= RA_INFO_REC_CHANNEL_START) && (id < RA_INFO_REC_CHANNEL_END) &&
	    ((ch < 0) || (ch >= i->hdr.num_ch)))
	{
		_ra_set_error(i->ra, RA_ERR_OUT_OF_RANGE, "plugin-edf");
		return -1;
	}

	if (i->non_contiguous)
	{
#ifdef WIN32
#pragma warning(push)
#pragma warning(disable:4311)
#endif
		session = (long)(rec->private_data);
#ifdef WIN32
#pragma warning(pop)
#endif
	}
	else
		session = 0;

	switch (id)
	{
	case RA_INFO_REC_GEN_TYPE_L:
		ra_value_set_long(vh, RA_REC_TYPE_TIMESERIES);
		break;
	case RA_INFO_REC_GEN_NAME_C:
		ra_value_set_string_utf8(vh, i->name);
		break;
	case RA_INFO_REC_GEN_DESC_C:
		if (i->edf_plus)
			ra_value_set_string(vh, "EDF+");
		else
			ra_value_set_string(vh, "EDF");
		break;
	case RA_INFO_REC_GEN_TIME_C:
		if (i->non_contiguous)
		{
			sec = i->hdr.rec_ts.sec;
			if (i->start_time_rec_block && rec->private_data)
				sec += (int)i->start_time_rec_block[i->session[session].start_blk];
			r = sec / 60;
			sec %= 60;

			min = i->hdr.rec_ts.min + r;
			r = min / 60;
			min %= 60;

			hour = i->hdr.rec_ts.hour + r;
			/* TODO: handle wrapping of day */
			sprintf(t, "%02d:%02d:%02d", hour, min, sec);
		}
		else
			sprintf(t, "%02d:%02d:%02d", i->hdr.rec_ts.hour, i->hdr.rec_ts.min, i->hdr.rec_ts.sec);
		ra_value_set_string(vh, t);
		break;
	case RA_INFO_REC_GEN_DATE_C:
		sprintf(t, "%02d.%02d.%d", i->hdr.rec_ts.day,
			i->hdr.rec_ts.mon, i->hdr.rec_ts.year);
		ra_value_set_string(vh, t);
		break;
	case RA_INFO_REC_GEN_DURATION_SEC_D:
		d = i->hdr.duration * (double)(i->session[session].end_blk - i->session[session].start_blk + 1);
		ra_value_set_double(vh, d);
		break;
	case RA_INFO_REC_GEN_NUMBER_L:
		ra_value_set_long(vh, 0);
		break;
	case RA_INFO_REC_GEN_NUM_SUB_REC_L:
		ra_value_set_long(vh, 0);
		break;
	case RA_INFO_REC_GEN_NUM_CHANNEL_L:
		ra_value_set_long(vh, i->hdr.num_ch);
		break;
	case RA_INFO_REC_GEN_OFFSET_L:
		ra_value_set_long(vh, 0);
		break;
	case RA_INFO_REC_GEN_PATH_C:
		sprintf(t, "%s%c%s", i->dir, SEPARATOR, i->name);
		ra_value_set_string_utf8(vh, t);
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
		ra_value_set_string_utf8(vh, "");
		break;
	case RA_INFO_REC_CH_NUM_SAMPLES_L:
		ra_value_set_long(vh, (long)(i->ci[ch].num_samples * (i->session[session].end_blk - i->session[session].start_blk + 1)));
		break;
	case RA_INFO_REC_CH_SAMPLERATE_D:
		ra_value_set_double(vh, (double)i->ci[ch].samplerate);
		break;
	case RA_INFO_REC_CH_NUM_BITS_L:
		ra_value_set_long(vh, 16);
		break;
	case RA_INFO_REC_CH_AMP_RESOLUTION_D:
		ra_value_set_double(vh, 1.0);	/* adjustment will be done in this module */
		break;
	case RA_INFO_REC_CH_SAMPLE_TYPE_L:
		if (i->ci[ch].float_values)
			ra_value_set_long(vh, RA_CH_SAMPLE_TYPE_FLOAT2);
		else
			ra_value_set_long(vh, RA_CH_SAMPLE_TYPE_INT);
		break;
	case RA_INFO_REC_CH_UNIT_C:
		ra_value_set_string_utf8(vh, i->ci[ch].unit);
		break;
	case RA_INFO_REC_CH_MIN_ADC_D:
		ra_value_set_double(vh, i->ci[ch].digital_min);
		break;
	case RA_INFO_REC_CH_MAX_ADC_D:
		ra_value_set_double(vh, i->ci[ch].digital_max);
		break;
	case RA_INFO_REC_CH_MIN_UNIT_D:
		ra_value_set_double(vh, i->ci[ch].phys_min);
		break;
	case RA_INFO_REC_CH_MAX_UNIT_D:
		ra_value_set_double(vh, i->ci[ch].phys_max);
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
		_ra_set_error(i->ra, RA_ERR_UNKNOWN_INFO, "plugin-edf");
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
	case RA_INFO_OBJ_PATIENT_ID_C:
		ra_value_set_string_utf8(vh, i->pi.id);
		break;
	case RA_INFO_OBJ_PERSON_NAME_C:
		if (i->edf_plus)
			ra_value_set_string_utf8(vh, i->pi.name);
		else
			ra_value_set_string_utf8(vh, i->name);
		break;
	case RA_INFO_OBJ_PERSON_FORENAME_C:
		ra_value_set_string_utf8(vh, i->pi.forename);
		break;
	case RA_INFO_OBJ_PERSON_BIRTHDAY_C:
		ra_value_set_string(vh, i->pi.birthday);
		break;
	case RA_INFO_OBJ_PERSON_GENDER_C:
		ra_value_set_string_utf8(vh, i->pi.gender);
		break;
	default:
		_ra_set_error(i->ra, RA_ERR_UNKNOWN_INFO, "plugin-edf");
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

	num = read_raw(i, rh, ch, start, num_data, NULL, data);

	if (data_high)
	{
		size_t n;
		for (n = 0; n < num; n++)
			data_high[n] = 0;
	}

	return num;
} /* pl_get_raw() */


size_t
pl_get_raw_unit(meas_handle mh, rec_handle rh, int ch, size_t start, size_t num_data, double *data)
{
	struct info *i = ((struct ra_meas *) mh)->priv;
	size_t j, num = 0;

	float *buf = malloc(sizeof(float) * num_data);
	num = read_raw(i ,rh, ch, start, num_data, buf, NULL);
	for (j = 0; j < num; j++)
		data[j] = (double) buf[j];
	free(buf);

	return num;
} /* pl_get_raw_unit() */


size_t
read_raw(struct info *i, rec_handle rh, int ch, size_t start, size_t num, float *buf, DWORD *buf_raw)
{
	struct ra_rec *rec = (struct ra_rec *)rh;
	size_t j, k;
	char fn[MAX_PATH_RA];
	WORD *buf_blk = NULL;
	size_t num_ret = 0;
	FILE *fp = NULL;
	float adc_r, unit_r;
	size_t n_use;
	long session, n_samples_tot;
	size_t s_blk, s_off, e_blk;

	if (rec == 0)
		return 0;

	session = 0;
	if (i->non_contiguous)
	{
#ifdef WIN32
#pragma warning(push)
#pragma warning(disable:4311)
#endif
		session = (long)rec->private_data;
#ifdef WIN32
#pragma warning(pop)
#endif
	}
	n_samples_tot = (long)(i->ci[ch].num_samples * (i->session[session].end_blk - i->session[session].start_blk + 1));

	s_blk = i->session[session].start_blk + (start / i->ci[ch].num_samples);
	s_off = start % i->ci[ch].num_samples;
	e_blk = i->session[session].start_blk + ((start + num) / i->ci[ch].num_samples);
	if (((start + num) % i->ci[ch].num_samples) != 0)
		e_blk++;
	if (e_blk > i->session[session].end_blk)
		e_blk = i->session[session].end_blk;

	if ((s_blk >= (size_t)i->hdr.num_recs) || (start >= (size_t)n_samples_tot))
		return 0;

	sprintf(fn, "%s%c%s.%s", i->dir, SEPARATOR, i->name, i->ext);
	if ((fp = ra_fopen(fn, "rb", 0)) == NULL)
	{
		_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-edf");
		return 0;
	}

	if ((buf_blk = malloc(i->ci[ch].num_samples * sizeof(WORD))) == NULL)
	{
		_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-edf");
		fclose(fp);
		return 0;
	}

	adc_r = (float)(i->ci[ch].digital_max - i->ci[ch].digital_min);
	unit_r = (float)(i->ci[ch].phys_max - i->ci[ch].phys_min);

	num_ret = 0;
	for (j = s_blk; j <= e_blk; j++)
	{
		if (read_data_block(i, i->ci[ch].start_filepos, i->ci[ch].num_samples, (int)j, buf_blk, fp))
			break;

		/* how many samples left? */
		n_use = i->ci[ch].num_samples - s_off;
		if ((num_ret + n_use) > num)
			n_use = num - num_ret;

		if ((start + num_ret + n_use) > (size_t)n_samples_tot)
			n_use = n_samples_tot - start - num_ret;

		for (k = s_off; k < (s_off + n_use); k++)
		{
			float val;
			short t;

			t = buf_blk[k];
			le16_to_cpu(t);
			if (i->ci[ch].float_values)
			{
				double ymin, a;
				
				ymin = i->ci[ch].ymin;
				a = i->ci[ch].a;

				val = 0.0;
				if (t < 0)
					val = (float)(-ymin * exp(-a * (double)t));
				if (t > 0)
					val = (float)(ymin * exp(a * (double)t));
			}
			else
				val = (float)t;
			
			if (buf)
				buf[num_ret] = (float)(((val - i->ci[ch].digital_min) / adc_r) * unit_r + i->ci[ch].phys_min);
			if (buf_raw)
				buf_raw[num_ret] = (DWORD)t;
			num_ret++;
		}

		s_off = 0;	/* start at beginning of next blocks */
	}
	free(buf_blk);
	fclose(fp);

	return num_ret;
} /* read_raw() */


int
read_data_block(struct info *i, size_t start_filepos, size_t len, int blk_num, WORD * buf, FILE * fp)
{
	long pos = (long)(start_filepos + (blk_num * i->record_size));
	if (fseek(fp, pos, SEEK_SET) == -1)
	{
		_ra_set_error(i->ra, 0, "plugin-edf");
		return -1;
	}

	if (fread(buf, sizeof(WORD), len, fp) != len)
	{
		_ra_set_error(i->ra, 0, "plugin-edf");
		return -1;
	}

	return 0;
} /* read_data_block() */


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
	ra_value_set_string(desc, "Annotations stored in the EDF+ file");

	sprintf(t, "%02d.%02d.%d %02d:%02d:%02d", i->hdr.rec_ts.day,
		i->hdr.rec_ts.mon, i->hdr.rec_ts.year, i->hdr.rec_ts.hour,
		i->hdr.rec_ts.min, i->hdr.rec_ts.sec);

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

	return 1;  /* annotations */
} /* pl_get_class_num() */


int
pl_get_class_info(meas_handle mh, unsigned long class_num, value_handle id, value_handle name, value_handle desc)
{
	int ret;

	if (mh || class_num)
		;

	ra_value_set_string(id, "annotation");

	ret = fill_predef_class_info_ascii("annotation", name, desc);

	return ret;
} /* pl_get_class_info() */


long
pl_get_prop_num(class_handle clh, unsigned long class_num)
{
	if (clh || class_num)
		;

	return 1;  /* annotations */
} /* pl_get_prop_num() */


int
pl_get_prop_info(class_handle clh, unsigned long class_num, unsigned long prop_num, value_handle id,
				 value_handle type, value_handle len, value_handle name, value_handle desc, value_handle unit,
				 value_handle use_minmax, value_handle min, value_handle max,
				 value_handle has_ign_value, value_handle ign_value)
{
	int ret = 0;

	if (prop_num || class_num || clh)
		;

	ra_value_set_string(id, "annotation");
	ret = fill_predef_prop_info_ascii("annotation", type, len, name, desc, unit,
					  use_minmax, min, max, has_ign_value, ign_value);

	return ret;
} /* pl_get_prop_info() */


int
pl_get_ev_info(class_handle clh, unsigned long class_num, value_handle start, value_handle end)
{
	long l, *start_su, *end_su;
	meas_handle mh;
	struct info *i;

	if (class_num)
		;

	mh = ra_meas_handle_from_any_handle(clh);
	i = ((struct ra_meas *)mh)->priv;

	start_su = (long *)malloc(sizeof(long) * i->num_annot);
	end_su = (long *)malloc(sizeof(long) * i->num_annot);

	for (l = 0; l < i->num_annot; l++)
	{
		if (i->max_samplerate > 0)
		{
			start_su[l] = (long)(i->annot[l].onset * i->max_samplerate);
			end_su[l] = start_su[l] + (long)(i->annot[l].duration * i->max_samplerate);
		}
		else
		{
			start_su[l] = (long)i->annot[l].onset;
			end_su[l] = start_su[l] + (long)i->annot[l].duration;
		}
	}
	ra_value_set_long_array(start, start_su, i->num_annot);
	ra_value_set_long_array(end, end_su, i->num_annot);

	free(start_su);
	free(end_su);

	return 0;
} /* pl_get_ev_info() */


int
pl_get_ev_value(prop_handle ph, unsigned long class_num, unsigned long prop_num, long event,
				value_handle value, value_handle ch)
{
	value_handle *vh;
	long ch_num[1];
	meas_handle mh;
	struct info *i;

	if (prop_num || class_num)
		;

	mh = ra_meas_handle_from_any_handle(ph);
	i = ((struct ra_meas *)mh)->priv;
	
	vh = (value_handle *)malloc(sizeof(value_handle));
	vh[0] = ra_value_malloc();
	ra_value_set_string_utf8(vh[0], i->annot[event].text);

	ch_num[0] = -1;

	ra_value_set_vh_array(value, vh, 1);
	ra_value_set_long_array(ch, ch_num, 1);

	ra_value_free(vh[0]);
	free(vh);

	return 0;
} /* pl_get_ev_value() */


