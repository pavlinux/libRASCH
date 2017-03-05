/*----------------------------------------------------------------------------
 * read_rri.c  -  Handle RR interval-files (nearly the final design)
 *
 * Description:
 * The plugin provides access to RR interval files. It now supports the first
 * RR-interval format specific for libRASCH.
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

#define _LIBRASCH_BUILD
#include <ra.h>
#include <ra_priv.h>
#include <pl_general.h>

#include <ra_file.h>
#include <ra_ecg.h>
#include "read_rri.h"


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
	char buf[1000];
	FILE *fp = NULL;
	size_t n, len_read;
	char ext_comp[100];
	char fn[MAX_PATH_RA];
	int j, supported;

	/* up to now it is not supported */
	supported = 0;

	i = malloc(sizeof(struct info));
	if (!i)
		goto clean_up;
	memset(i, 0, sizeof(struct info));

	/* split file in dir and filename w/o extension */
	split_filename(name, SEPARATOR, i->dir, i->name, i->ext);
	for (n = 0; n < strlen(i->ext); n++)
		ext_comp[n] = (char)tolower(i->ext[n]);
	ext_comp[strlen(i->ext)] = '\0';
	if (i->ext[0] != '\0')
	{
		int found = 0;
		for (j = 0; j < _num_supported_extensions; j++)
		{
			if (strncmp(ext_comp, _supported_extensions[j], strlen(_supported_extensions[j])) == 0)
			{
				found = 1;
				break;
			}
		}
		if (!found)
			goto clean_up;
	}
	else
	{
		if (get_extension(i) != 0)
			goto clean_up;
	}

	sprintf(fn, "%s%c%s.%s", i->dir, SEPARATOR, i->name, i->ext);
	fp = ra_fopen(fn, "rb", 0);
	if (!fp)
		goto clean_up;

	/* all rri-files should have at least a length to contain the following string */
	len_read = strlen("Research Tools output from Reynolds Medical Pathfinder");
	if (fread(buf, 1, len_read, fp) != len_read)
		goto clean_up;

	/********** check if file is supported **********/
	/* The Reynolds RR-Tools RRI files for version 8.25.* and previous
	   have another format than the newer ones. Therefore check if it
	   this format first. */
	if (check_if_reynolds_old_rrtools(fp))
	{
		supported = 1;
		goto clean_up;
	}
	/* not the old one, maybe the new one */
	if (strncmp("Research Tools output from Reynolds Medical Pathfinder", buf, len_read) == 0)
	{
		supported = 1;
		goto clean_up;
	}

	if (check_if_getemed_rri(fp))
	{
		supported = 1;
		goto clean_up;
	}

	/* check if libRASCH RRI file */
	if (strncmp(buf, "librasch-rri-V", 14) == 0)
	{
		supported = 1;
		goto clean_up;
	}

	/* if extension is NOT '.txt' and we have not yet found support for
	   the file, then we presumably support it (indicated by the file-extension) */
	if (RA_STRICMP(i->ext, "txt") == 0)
		goto clean_up;

	supported = 1;

clean_up:
	if (fp)
		fclose(fp);
	if (i)
		free(i);

	return supported;
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
	int n;

	n = find_meas(dir, "*.rri", PLUGIN_NAME, f, pl_check_meas);
	n = find_meas(dir, "*.qrs", PLUGIN_NAME, f, pl_check_meas);
	n = find_meas(dir, "*.dat", PLUGIN_NAME, f, pl_check_meas);
	/* TODO: add handling of '?' wildchar in find_meas() */
	n = find_meas(dir, "*.mib", PLUGIN_NAME, f, pl_check_meas);
	n = find_meas(dir, "*.mis", PLUGIN_NAME, f, pl_check_meas);
	n = find_meas(dir, "*.mif", PLUGIN_NAME, f, pl_check_meas);
	n = find_meas(dir, "*.occ", PLUGIN_NAME, f, pl_check_meas);
	n = find_meas(dir, "*.txt", PLUGIN_NAME, f, pl_check_meas);

	return n;
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
	if (i->ext[0] == '\0')
		if (get_extension(i) != 0)
			goto error_open;

	if (RA_STRICMP(i->ext, "qrs") == 0)
		i->samplerate = 128;
	else
		i->samplerate = 1000;

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
	if (i->beats)
		free(i->beats);

	free(i);
} /* free_info_struct() */



int
get_data(struct info *i, int fast)
{
	int ret = -1;
	char file[MAX_PATH_RA];
	FILE *fp;
	char buf[100];

	sprintf(file, "%s%c%s.%s", i->dir, SEPARATOR, i->name, i->ext);
	fp = ra_fopen(file, "rb", 0);
	if (fp == NULL)
	{
		_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-read-rri");
		goto error;
	}

	fgets(buf, 100, fp);
	if (strncmp("librasch-rri-", buf, strlen("librasch-rri-")) == 0)
		ret = read_librasch_rri(i, fp, buf, fast);
	else if (check_if_reynolds_old_rrtools(fp))
	{
		ret = read_reynolds_rri_old(i, fp, fast);
	}
	else if (strncmp("Research Tools output from Reynolds Medical Pathfinder", buf,
			 strlen("Research Tools output from Reynolds Medical Pathfinder")) == 0)
	{
		ret = read_reynolds_rri(i, fp, fast);
	}
	else if (buf[0] == '#')
	{
		ret = read_marquette_rri(i, fp, fast);
	}
	else if (RA_STRICMP(i->ext, "occ") == 0)
	{
		ret = read_altair_occ(i, fp);
	}
	else if (RA_STRICMP(i->ext, "qrs") == 0)
	{
		int is_qrsdk_file = check_if_qrsdk(fp);
		if (is_qrsdk_file)
			ret = read_qrsdk(i, fp, fast);
		else
			ret = read_other_formats(i, fp, 1);
	}
	else if(check_if_getemed_rri(fp))
	{
		ret = read_getemed_rri(i, fp, fast);
	}
	else
		ret = read_other_formats(i, fp, 0);

 error:
	if (fp)
		fclose(fp);
	
	return ret;
} /* get_data() */


int
check_if_qrsdk(FILE* fp)
{
	struct qrsdk_header hdr;
	int hour, min, sec;
	int day, year;

	/* go back to the beginning */
	fseek(fp, 0, SEEK_SET);

	if (fread(&hdr, sizeof(struct qrsdk_header), 1, fp) != 1)
		return 0;

	/* first check if samplerate makes sense */
	if ((hdr.samplerate < 30) || (hdr.samplerate > 5000))
		return 0;

	/* now check rec-time (format: HH:MM:SS) */
	hour = atoi(hdr.rec_time);
	min = atoi(hdr.rec_time+3);
	sec = atoi(hdr.rec_time+6);
	if ((hour < 0) || (hour >= 24) || (min < 0) || (min >= 60)
	    || (sec < 0) || (sec >= 60))
	{
		return 0;
	}

	/* and now check the rec-date (format: DD-MMM-YY) */
	day = atoi(hdr.rec_date);
	year = atoi(hdr.rec_date+7);
	if ((day < 0) || (day >= 32) || (year < 0) || (year >= 100))
		return 0;

	/* TODO: calc and check checksum */

	return 1;
} /* check_if_qrsdk() */


int
check_if_reynolds_old_rrtools(FILE *fp)
{
	int ret = 0;	
	unsigned char buf[2*128];

	if (!fp)
		return ret;

	fseek(fp, 0, SEEK_SET);
	if (fread(buf, 1, 2*128, fp) != (2*128))
		return ret;

	/* first block has to be 0x10 id */
	if (buf[0] != 0x10)
		return ret;
	/* number of bytes can't be more than 128 */
	if (buf[1] > 128)
		return ret;
	/* id of next block has to be either 0x10, 0x11 or 0x13 */
	if ((buf[128] == 0x10) || (buf[128] == 0x11) || (buf[128] == 0x13))
		ret = 1;

	return ret;
} /* check_if_reynolds_old_rrtools() */


int
check_if_getemed_rri(FILE *fp)
{
	char buf[100];
	long pos, template_num;
	int annot;
	char *p;
	int ok;

	ok = 0;

	fseek(fp, 0, SEEK_SET);

	/* get 4th line */
	fgets(buf, 100, fp);
	fgets(buf, 100, fp);
	fgets(buf, 100, fp);
	fgets(buf, 100, fp);

	pos = atoi(buf);
	annot = toupper(buf[16]);
	p = strchr(buf+16, '\t');
	if (!p)
		return ok;
	template_num = atoi(p+1);

	if ((pos > 0) && (annot >= 'A') && (annot <= 'Z') && (template_num > 0))
		ok = 1;

	return ok;
} /* check_if_getemed_rri() */


int
read_librasch_rri(struct info *i, FILE *fp, char *version, int fast)
{
	int ret = -1;
	long buf_size = 0;
	char buf[100];
	char *p;
	long curr_pos = 0;
	int dec_point_is_comma = 0;

	version[0] = '\0';

	/* read header (strings are UTF-8 encoded) */
	/* (ignore version because only V1 exists and up to now handle only samplerate) */
	while (fgets(buf, 100, fp))
	{
		/* if comment, go to next line */
		if (buf[0] == '#')
			continue;

		if (buf[0] == '-')
			break;	/* end of header */

		/* remove newline/cr */
		p = strchr(buf, '\n');
		if (p)
			*p = '\0';
		p = strchr(buf, '\r');
		if (p)
			*p = '\0';

		/* if no value, go to next line */
		p = strchr(buf, '=');
		if (!p)
			continue;

		if (strncmp(buf, "samplerate", strlen("samplerate")) == 0)
		{
			i->samplerate = atof(p+1);
		}
		else if (strncmp(buf, "name", strlen("name")) == 0)
		{
			strncpy(i->lastname, p+1, MAX_FIELD_LEN);
		}
		else if (strncmp(buf, "forename", strlen("forename")) == 0)
		{
			strncpy(i->forename, p+1, MAX_FIELD_LEN);
		}
		else if (strncmp(buf, "birthday", strlen("birthday")) == 0)
		{
			strncpy(i->birthday, p+1, MAX_FIELD_LEN);
		}
		else if (strncmp(buf, "rec-date", strlen("rec-date")) == 0)
		{
			strncpy(i->rec_date, p+1, MAX_FIELD_LEN);
		}
		else if (strncmp(buf, "rec-time", strlen("rec-time")) == 0)
		{
			strncpy(i->rec_time, p+1, MAX_FIELD_LEN);
		}
		else if (strncmp(buf, "id", strlen("id")) == 0)
		{
			strncpy(i->id, p+1, MAX_FIELD_LEN);
		}
		else if (strncmp(buf, "pos-type", strlen("pos-type")) == 0)
		{
			if (strncmp(p+1, "qrs-pos", strlen("qrs-pos")) == 0)
				i->pos_is_rri = 0;
			else if (strncmp(p+1, "rri", 3) == 0)
			{
				i->pos_is_rri = 1;
				i->rri_in_msec = 0;
				if (strncmp(p+1, "rri-msec", 8) == 0)
					i->rri_in_msec = 1;
			}
		}
	}

	/* do not read rr intervals when in fast-mode */
	if (fast)
		return 0;

	/* assess if decimal-point is on this system a comma
	   (info is needed when rr intervals are in msec) */
	sprintf(buf, "%f", 1.2);
	if (strchr(buf, ','))
		dec_point_is_comma = 1;

	/* read beats data */
	curr_pos = 0;
	while (fgets(buf, 100, fp))
	{
		if (i->num_beats == buf_size)
		{
			buf_size += 1000;
			i->beats = realloc(i->beats, sizeof(struct beat_info) * buf_size);
		}
		i->num_beats++;
		if (i->pos_is_rri)
		{
			double rri = atof(buf);
			if (i->rri_in_msec)
			{
				if (dec_point_is_comma)
				{
					char *p = strchr(buf, '.');
					if (p)
						*p = ',';
					rri = atof(buf);
				}
				rri *= (i->samplerate/1000.0);
			}
			curr_pos += (long)rri;
			i->beats[i->num_beats-1].pos = curr_pos;
		}
		else
			i->beats[i->num_beats-1].pos = atol(buf);
		
		p = strrchr(buf, '\t');
		if (!p)
			goto error;

		else
		{
			switch (*(p+1))
			{
			case 'n':
			case 'N':
			case 'S':
				i->beats[i->num_beats-1].annotation = ECG_CLASS_SINUS;
				break;
			case 'V':
				i->beats[i->num_beats-1].annotation = ECG_CLASS_VENT;
				break;
			case 'P':
				i->beats[i->num_beats-1].annotation = ECG_CLASS_PACED;
				break;
			default:
				i->beats[i->num_beats-1].annotation = ECG_CLASS_ARTIFACT;
				break;
			}
		}
	}

	ret = 0;

 error:
	return ret;
} /* read_librasch_rri() */


int
read_reynolds_rri_old(struct info *i, FILE *fp, int fast)
{
	long num, curr_pos, buf_pos;
	unsigned char buf[128];
	unsigned short rri, annot;
	int j, rec_time_set, hour, min;

	i->samplerate = 128;

	/* get length of file as an estimate of the number of RRI in the file */
	fseek(fp, 0, SEEK_END);
	num = ftell(fp) / 2; /* 2 Bytes for each RRI  */

	/* go back to the beginning */
	fseek(fp, 0, SEEK_SET);	

	i->beats = malloc(sizeof(struct beat_info) * num);
	i->num_beats = 0;
	curr_pos = 0;
	rec_time_set = 0;
	while (!feof(fp))
	{
		if (fread(buf, 1, 128, fp) != 128)
			break;

		if (buf[0] == 0x10)
		{
			if (!rec_time_set)
			{
				/* get rec-time */
				hour = ((buf[2] & 0xf0) >> 4)*10 + (buf[2] & 0x0f);
				min = ((buf[3] & 0xf0) >> 4)*10 + (buf[3] & 0x0f);
				sprintf(i->rec_time, "%02d:%02d:00", hour, min);

				rec_time_set = 1;
			}
			/* if in fast mode, only get recording time and nothing else */
			if (fast)
				break;

			continue;
		}
		else if (buf[0] == 0x13)
		{
			/* end of recording */
			break;
		}		
		
		buf_pos = 4;
		for (j = 0; j < ((buf[1]-4) / 2); j++)
		{
			annot = buf[buf_pos] & 0x3f;

			rri = buf[buf_pos+1];
			rri |= ((buf[buf_pos] & 0xc0) << 2);
			
			curr_pos += rri;
			i->beats[i->num_beats].pos = curr_pos;

			if (((annot & 0x07) == 2) || ((annot & 0x07) == 4))
				i->beats[i->num_beats].annotation = ECG_CLASS_SINUS;
			else if ((annot & 0x07) == 3)
				i->beats[i->num_beats].annotation = ECG_CLASS_VENT;
			else if (annot & 0x00)
				i->beats[i->num_beats].annotation = ECG_CLASS_PACED;
			else
				i->beats[i->num_beats].annotation = ECG_CLASS_ARTIFACT;

			i->num_beats++;
			buf_pos += 2;
		}
	}

	return 0;
} /* read_reynolds_rri_old() */


int
read_reynolds_rri(struct info *i, FILE *fp, int fast)
{
	long num, curr_pos, pos, l;
	unsigned short *buf;
	struct reynolds_hdr hdr;
	int hour, min;

	i->samplerate = 1000;

	/* go back to the beginning */
	fseek(fp, 0, SEEK_SET);	

	/* read header */
	if (fread(&hdr, sizeof(struct reynolds_hdr), 1, fp) != 1)
		return -1;

	hour = (int)((double)(hdr.start_time_min) / 60.0);
	min = hdr.start_time_min - (hour * 60);
	sprintf(i->rec_time, "%02d:%02d:00", hour, min);

	if (fast)
		return 0;

	/* get number of RR intervals */
	pos = ftell(fp);
	fseek(fp, 0, SEEK_END);
	num = (ftell(fp) - pos) / 2; /* 2 Bytes for each RRI  */
	fseek(fp, pos, SEEK_SET);

	buf = malloc(sizeof(short) * num);
	num = (long)fread(buf, 2, num, fp);

	i->beats = malloc(sizeof(struct beat_info) * num);
	i->num_beats = 0;
	curr_pos = hdr.first_interval_ms;
	for (l = 0; l < num; l++)
	{
		unsigned int rri, annot;

		rri = buf[l] & 0x1fff;
		annot = (buf[l] & 0xe000) >> 13;

		curr_pos += rri;

		if (annot == NULL_BEAT)
			continue;

		i->beats[i->num_beats].pos = curr_pos;

		switch (annot)
		{
		case NORMAL_BEAT:
			i->beats[i->num_beats].annotation = ECG_CLASS_SINUS;
			break;
		case ABERRANT_BEAT:
			i->beats[i->num_beats].annotation = ECG_CLASS_VENT;
			break;
		case PACED_BEAT:
			i->beats[i->num_beats].annotation = ECG_CLASS_PACED;
			break;
		default:
			i->beats[i->num_beats].annotation = ECG_CLASS_ARTIFACT;
			break;
		}

		i->num_beats++;
	}

	free(buf);

	return 0;
} /* read_reynolds_rri() */


int
read_marquette_rri(struct info *i, FILE *fp, int fast)
{
	int ret = -1;
	long curr_pos, buf_size;
	char buf[100];
	char *p, annot;

	/* RRI values are given in msec */
	i->samplerate = 1000;

	/* read header */
	fseek(fp, 0, SEEK_SET);
	while (fgets(buf, 100, fp))
	{
		/* if comment, go to next line */
		if (buf[0] == '#')
			continue;
		
		if (strncmp(buf, "End header", strlen("End header")) == 0)
			break;	/* end of header */
		
		/* remove newline/cr */
		p = strchr(buf, '\n');
		if (p)
			*p = '\0';
		p = strchr(buf, '\r');
		if (p)
			*p = '\0';

		/* if no value, go to next line */
		p = strchr(buf, '=');
		if (!p)
			continue;

/*		if (strncmp(buf, "samplerate", strlen("samplerate")) == 0)
		{
			i->samplerate = atof(p+1);
			}*/
		if (strncmp(buf, "Study code", strlen("Study code")) == 0)
		{
			strncpy(i->lastname, p+1, MAX_FIELD_LEN);
			to_utf8_inplace(i->lastname, MAX_FIELD_LEN, "ISO8859-1");
		}
		else if (strncmp(buf, "Record number code", strlen("Record number code")) == 0)
		{
			strncpy(i->id, p+1, MAX_FIELD_LEN);
			to_utf8_inplace(i->id, MAX_FIELD_LEN, "ISO8859-1");
		}
		else if (strncmp(buf, "Start date", strlen("Start date")) == 0)
		{
			strncpy(i->rec_date, p+1, MAX_FIELD_LEN);
		}
		else if (strncmp(buf, "Start time", strlen("Start time")) == 0)
		{
			strncpy(i->rec_time, p+1, MAX_FIELD_LEN);
		}
	}

	if (fast)
		return 0;

	/* read RRI's */
	curr_pos = 0;
	buf_size = 0;
	while (fgets(buf, 100, fp))
	{
		if (i->num_beats == buf_size)
		{
			buf_size += 1000;
			i->beats = realloc(i->beats, sizeof(struct beat_info) * buf_size);
		}
		annot = buf[0];
		curr_pos += atol(buf+1);
		i->beats[i->num_beats].pos = curr_pos;
		
		switch (annot)
		{
		case 'Q':
		case 'A':
			i->beats[i->num_beats].annotation = ECG_CLASS_SINUS;
			break;
		case 'V':
			i->beats[i->num_beats].annotation = ECG_CLASS_VENT;
			break;
		case 'P':
			i->beats[i->num_beats].annotation = ECG_CLASS_PACED;
			break;
		default:
			i->beats[i->num_beats].annotation = ECG_CLASS_ARTIFACT;
			break;
		}
		i->num_beats++;
	}

	ret = 0;

	return ret;
} /* read_marquette_rri() */


int
read_altair_occ(struct info *i, FILE *fp)
{
	size_t num, l;
	struct altair_pca_ann *buf;

	i->samplerate = 200;

	/* get number of beats */
	fseek(fp, 0, SEEK_END);
	num = ftell(fp) / (sizeof(struct altair_pca_ann));
	fseek(fp, 0, SEEK_SET);

	/* read data from file */
	buf = malloc(sizeof(struct altair_pca_ann) * num);
	if (fread(buf, sizeof(struct altair_pca_ann), num, fp) != num)
	{
		free(buf);
		return -1;

	}

	i->beats = malloc(sizeof(struct beat_info) * num);
	i->num_beats = (long)num;
	for (l = 0; l < num; l++)
	{
		i->beats[l].pos = buf[l].atime;

		if ((buf[l].label >= FIRST_NRM_CODE) && (buf[l].label <= LAST_NRM_CODE))
			i->beats[l].annotation = ECG_CLASS_SINUS;
		else if ((buf[l].label >= FIRST_SVE_CODE) && (buf[l].label <= LAST_SVE_CODE))
			i->beats[l].annotation = ECG_CLASS_SINUS;
		else if ((buf[l].label >= FIRST_ABN_CODE) && (buf[l].label <= LAST_ABN_CODE))
			i->beats[l].annotation = ECG_CLASS_VENT;
		else
			i->beats[l].annotation = ECG_CLASS_ARTIFACT;
	}

	free(buf);

	return 0;
} /* read_altair_occ() */


int
read_qrsdk(struct info *i, FILE *fp, int fast)
{
	struct qrsdk_header hdr;
	struct qrsdk_beat *read_buf;
	long l, cnt, end_pos, curr_pos;

	/* go back to the beginning */
	fseek(fp, 0, SEEK_SET);

	if (fread(&hdr, sizeof(struct qrsdk_header), 1, fp) != 1)
		return -1;

	/* save data from header */
	strncpy(i->lastname, hdr.pat_name, 40);
	to_utf8_inplace(i->lastname, MAX_FIELD_LEN, "ISO8859-1");
	strncpy(i->id, hdr.pat_id, 16);
	to_utf8_inplace(i->id, MAX_FIELD_LEN, "ISO8859-1");
	strncpy(i->rec_time, hdr.rec_time, 10);
	strncpy(i->rec_date, hdr.rec_date, 10);
	strncpy(i->save_date, hdr.save_date, 10);
	i->samplerate = hdr.samplerate;

	/* do not read rr intervals when in fast-mode */
	if (fast)
		return 0;

	/* get number of beats */
	fseek(fp, 0, SEEK_END);
	end_pos = ftell(fp);
	fseek(fp, sizeof(struct qrsdk_header), SEEK_SET);
	i->num_beats = (end_pos - ftell(fp)) / sizeof(struct qrsdk_beat);

	read_buf = calloc(i->num_beats, sizeof(struct qrsdk_beat));
	i->beats = calloc(i->num_beats, sizeof(struct beat_info));
	if (fread(read_buf, sizeof(struct qrsdk_beat), i->num_beats, fp) != (size_t)i->num_beats)
	{
		free(read_buf);
		free(i->beats);
		i->beats = NULL;
		i->num_beats = 0;
		return -1;
	}

	cnt = 0;
	curr_pos = 0;
	for (l = 0; l < i->num_beats; l++)
	{
		if (read_buf[l].label == 255)
			break;

		curr_pos += read_buf[l].clock_inc;

		if (read_buf[l].label == 0)
			continue;

		switch (read_buf[l].label)
		{
		case 'N':
		case 'S':
		case 'J':
		case 'A':
		case 'B':
			i->beats[cnt].annotation = ECG_CLASS_SINUS;
			break;
		case 'V':
		case 'F':
		case 'E':
			i->beats[cnt].annotation = ECG_CLASS_VENT;
			break;
		case 'P':
			i->beats[cnt].annotation = ECG_CLASS_PACED;
			break;
		case '.':
		case '?':
		default:
			i->beats[cnt].annotation = ECG_CLASS_ARTIFACT;
			break;
		}
		i->beats[cnt].pos = curr_pos;

		cnt++;
	}
	i->num_beats = cnt;

	free(read_buf);

	return 0;
} /* read_qrsdk() */


int
read_getemed_rri(struct info *i, FILE *fp, int fast)
{
	int ret = -1;
	long buf_size = 0;
	char buf[100];
	long curr_pos = 0;
	char *p;

	i->samplerate = 1024;

	fseek(fp, 0, SEEK_SET);

	/* first line seems to be ID */
	fgets(buf, 100, fp);
	p = strchr(buf, '\n');
	if (p)
		*p = '\0';
	p = strchr(buf, '\r');
	if (p)
		*p = '\0';
	strncpy(i->id, buf, MAX_FIELD_LEN);

	/* get start date and time of recording */
	fgets(buf, 100, fp);
	strncpy(i->rec_date, buf, 10);
	i->rec_date[10] = '\0';
	strncpy(i->rec_time, buf+11, 8);
	i->rec_time[9] = '\0';

	/* do not read rr intervals when in fast-mode */
	if (fast)
		return 0;

	/* skip starting time, positions are already relative to rec-time */
	fgets(buf, 100, fp);

	/* read beats data */
	curr_pos = 0;
	while (fgets(buf, 100, fp))
	{
		size_t line_pos, len;

		if (i->num_beats == buf_size)
		{
			buf_size += 1000;
			i->beats = realloc(i->beats, sizeof(struct beat_info) * buf_size);
		}
		i->num_beats++;
		i->beats[i->num_beats-1].pos = atol(buf);

		/* look for postion of annotation */
		line_pos = 0;
		len = strlen(buf);
		while (line_pos < len)
		{
			if ((buf[line_pos] >= 'A') && (buf[line_pos] <= 'Z'))
				break;
			line_pos++;
		}

		switch (buf[line_pos])
		{
		case 'N':
		case 'S':
			i->beats[i->num_beats-1].annotation = ECG_CLASS_SINUS;
			break;
		case 'V':
		case 'B':
			i->beats[i->num_beats-1].annotation = ECG_CLASS_VENT;
			break;
		case 'P':
			i->beats[i->num_beats-1].annotation = ECG_CLASS_PACED;
			break;
		default:
			i->beats[i->num_beats-1].annotation = ECG_CLASS_ARTIFACT;
			break;
		}
	}

	ret = 0;

	return ret;
} /* read_getemed_rri() */


int
read_other_formats(struct info *i, FILE *fp, int qrs_file)
{
	int ret = -1;
	long buf_size = 0;
	char buf[100];

	/* make sure we are at the beginning */
	fseek(fp, 0, SEEK_END);
	if (qrs_file)
		fgets(buf, 100, fp); /* skip first line when a 'qrs' file */
	
	while (fgets(buf, 100, fp))
	{
		char *p;
		
		if (i->num_beats == buf_size)
		{
			buf_size += 1000;
			i->beats = realloc(i->beats, sizeof(struct beat_info) * buf_size);
		}
		i->num_beats++;
		i->beats[i->num_beats-1].pos = atol(buf);
		
		if (qrs_file)
			p = strrchr(buf, '\t');
		else
			p = strrchr(buf, ' ');
		
		if (!p)
			goto error;

		if (!qrs_file)
			i->beats[i->num_beats-1].annotation = atol(p+1);
		else
		{
			switch (*(p+1))
			{
			case 'n':
			case 'N':
			case 'S':
				i->beats[i->num_beats-1].annotation = ECG_CLASS_SINUS;
				break;
			case 'V':
				i->beats[i->num_beats-1].annotation = ECG_CLASS_VENT;
				break;
			case 'P':
				i->beats[i->num_beats-1].annotation = ECG_CLASS_PACED;
				break;
			default:
				i->beats[i->num_beats-1].annotation = ECG_CLASS_ARTIFACT;
				break;
			}
		}
	}

	ret = 0;

 error:
	return ret;
} /* read_other_formats() */


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

	_ra_set_error(h, RA_ERR_UNKNOWN_INFO, "plugin-read-rri");

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
		_ra_set_error(h, RA_ERR_ERROR, "plugin-read-rri");
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

	switch (id)
	{
	case RA_INFO_REC_GEN_TYPE_L:
		ra_value_set_long(vh, RA_REC_TYPE_TIMESERIES);
		break;
	case RA_INFO_REC_GEN_NAME_C:
		ra_value_set_string_utf8(vh, i->name);
		break;
	case RA_INFO_REC_GEN_DESC_C:
		ra_value_set_string_utf8(vh, "libRASCH RRI-file");
		break;
	case RA_INFO_REC_GEN_TIME_C:
		ra_value_set_string_utf8(vh, i->rec_time);
		break;
	case RA_INFO_REC_GEN_DATE_C:
		ra_value_set_string_utf8(vh, i->rec_date);
		break;
	case RA_INFO_REC_GEN_DURATION_SEC_D:
		d = ((double)i->beats[i->num_beats-1].pos / i->samplerate);
		ra_value_set_double(vh, d);
		break;
	case RA_INFO_REC_GEN_NUMBER_L:
		ra_value_set_long(vh, 0);
		break;
	case RA_INFO_REC_GEN_NUM_SUB_REC_L:
		ra_value_set_long(vh, 0);
		break;
	case RA_INFO_REC_GEN_NUM_CHANNEL_L:
		ra_value_set_long(vh, 1);  /* one channel to be able to return sample rate */
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
	case RA_INFO_REC_CH_SAMPLERATE_D:
		ra_value_set_double(vh, i->samplerate);
		break;
	default:
		_ra_set_error(i->ra, RA_ERR_UNKNOWN_INFO, "plugin-read-rri");
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
		if (i->id[0] != '\0')
			ra_value_set_string_utf8(vh, i->id);
		else
			ra_value_set_string_utf8(vh, i->name);
		break;
	case RA_INFO_OBJ_PERSON_NAME_C:
		ra_value_set_string_utf8(vh, i->lastname);
		break;
	case RA_INFO_OBJ_PERSON_FORENAME_C:
		ra_value_set_string_utf8(vh, i->forename);
		break;
	case RA_INFO_OBJ_PERSON_BIRTHDAY_C:
		ra_value_set_string_utf8(vh, i->birthday);
		break;
	default:
		_ra_set_error(i->ra, RA_ERR_UNKNOWN_INFO, "plugin-read-rri");
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


int
pl_get_eval_info(meas_handle mh, value_handle name, value_handle desc, value_handle add_ts,
		 value_handle modify_ts, value_handle user, value_handle host, value_handle prog)
{
	struct info *i;

	if (!mh)
		return RA_ERR_ERROR;
	i = ((struct ra_meas *)mh)->priv;

	ra_value_set_string(name, "original");
	ra_value_set_string(desc, "");

	if (i->save_date[0] != '\0')
	{
		ra_value_set_string(add_ts, i->save_date);
		ra_value_set_string(modify_ts, i->save_date);
	}
	else
	{
		ra_value_set_string(add_ts, i->rec_date);
		ra_value_set_string(modify_ts, i->rec_date);
	}

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

	return 1;  /* heartbeat */
} /* pl_get_class_num() */


int
pl_get_class_info(meas_handle mh, unsigned long class_num, value_handle id, value_handle name, value_handle desc)
{
	int ret;

	if (mh || class_num)
		;

	ra_value_set_string(id, "heartbeat");

	ret = fill_predef_class_info_ascii("heartbeat", name, desc);

	return ret;
} /* pl_get_class_info() */


long
pl_get_prop_num(class_handle clh, unsigned long class_num)
{
	if (clh || class_num)
		;

	return 2;  /* position and classification of qrs complex */
} /* pl_get_prop_num() */


int
pl_get_prop_info(class_handle clh, unsigned long class_num, unsigned long prop_num, value_handle id, value_handle type, value_handle len,
		 value_handle name, value_handle desc, value_handle unit,
		 value_handle use_minmax, value_handle min, value_handle max,
		 value_handle has_ign_value, value_handle ign_value)
{
	int ret = 0;

	if (class_num)
		;

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
		_ra_set_error(clh, RA_ERR_OUT_OF_RANGE, "plugin-read-rri");
		ret = -1;
		break;
	}

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

	start_su = (long *)malloc(sizeof(long) * i->num_beats);
	end_su = (long *)malloc(sizeof(long) * i->num_beats);

	for (l = 0; l < i->num_beats; l++)
	{
		start_su[l] = (long)i->beats[l].pos;
		end_su[l] = (long)i->beats[l].pos;
	}
	ra_value_set_long_array(start, start_su, i->num_beats);
	ra_value_set_long_array(end, end_su, i->num_beats);

	free(start_su);
	free(end_su);

	return 0;
} /* pl_get_ev_start_end() */


int
pl_get_ev_value(prop_handle ph, unsigned long class_num, unsigned long prop_num, long event, value_handle value, value_handle ch)
{
	int ret = 0;
	value_handle *vh;
	long ch_num[1];
	struct eval_property *p = (struct eval_property *)ph;
	struct info *i = ((struct ra_meas *)p->meas)->priv;

	if (class_num || prop_num)
		;

	if (event >= i->num_beats)
	{
		_ra_set_error(i->ra, RA_ERR_OUT_OF_RANGE, "plugin-read-rri");
		return -1;
	}

	vh = (value_handle *)malloc(sizeof(value_handle));
	vh[0] = ra_value_malloc();
	switch (p->id)
	{
	case EVENT_PROP_QRS_POS:
		ra_value_set_long(vh[0], i->beats[event].pos);
		break;
	case EVENT_PROP_QRS_ANNOT:
		ra_value_set_long(vh[0], i->beats[event].annotation);
		break;
	default:
		_ra_set_error(i->ra, RA_ERR_OUT_OF_RANGE, "plugin-read-rri");
		ret = -1;
		break;
	}

	ra_value_set_vh_array(value, vh, 1);

	ch_num[0] = -1;
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
	case EVENT_CLASS_HEARTBEAT:
		ret = get_all_beat_events(p, i, value, ch);
		break;
	}

	return ret;
} /* pl_get_ev_value_all() */


int
get_all_beat_events(struct eval_property *p, struct info *i, value_handle value, value_handle ch)
{
	int ret = 0;	
	long *v, *ch_num, l;

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
		_ra_set_error(i->ra, RA_ERR_OUT_OF_RANGE, "plugin-read-rri");
		ret = -1;
	}

	ra_value_set_long_array(value, v, i->num_beats);
	ra_value_set_long_array(ch, ch_num, i->num_beats);

	free(v);
	free(ch_num);

	return ret;
} /* get_all_beat_events() */
