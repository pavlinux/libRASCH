/*----------------------------------------------------------------------------
 * mit_bih.c  -  Handle signals recorded using the MIT/BIH Data Format
 *
 * Description:
 * The plugin provides access to recordings saved in the format used for
 * the MIT/BIH database(s) and the databases of provides by PhysioNet
 * (www.physionet.org).
 *
 * Author(s): Raphael Schneider (librasch@gmail.com)
 *
 * Copyright (C) 2002-2010, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

/*
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * TODO: check what adc-baseline is and how to use this value
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define _LIBRASCH_BUILD
#include <ra.h>
#include <ra_priv.h>
#include <handle_plugin.h>

#include <ra_endian.h>
#include <pl_general.h>
#include <ra_file.h>
#include <ra_estimate_ch_infos.h>
#include <ra_ecg.h>

#include "mit_bih.h"

/* header files from WFDBlib */
#include "ecgcodes.h"
/* the following 4 defines avoid not needed definitions in ecgmap.h */
#define map1
#define map2
#define mamap
#define annpos
#include "ecgmap.h"


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
	int ret = 0;
	FILE *fp = NULL;
	char file[MAX_PATH_RA];
	char dir[MAX_PATH_RA], fn[MAX_PATH_RA], ext[MAX_PATH_RA];
	int ext_empty = 0;

	split_filename(name, SEPARATOR, dir, fn, ext);
	if ((ext[0] != '\0') && (RA_STRNICMP(ext, "hea", 3) != 0))
		goto error_open;

	if (ext[0] == '\0')
	{
		ext_empty = 1;
		strcpy(ext, "hea");
	}

	sprintf(file, "%s%c%s.%s", dir, SEPARATOR, fn, ext);
	fp = ra_fopen(file, "rb", 0);
	if (fp == NULL)
	{
		/* .hea not found, try .header */
		if (ext_empty)
		{
			strcpy(ext, "header");
			sprintf(file, "%s%c%s.%s", dir, SEPARATOR, fn, ext);
			fp = ra_fopen(file, "rb", 0);
			if (fp == NULL)
				goto error_open;
		}
		else
			goto error_open;
	}

	ret = 1;

 error_open:
	if (fp)
		fclose(fp);

	return ret;
} /* pl_check_meas() */


int
pl_find_meas(const char *dir, struct find_meas *f)
{
	int n;

	n = find_meas(dir, "*.hea", PLUGIN_NAME, f, pl_check_meas);
	n = find_meas(dir, "*.header", PLUGIN_NAME, f, pl_check_meas);

	return n;
} /* pl_find_meas() */


meas_handle
pl_open_meas(ra_handle ra, const char *file, int fast)
{
	struct ra_meas *meas;
	struct info *i = NULL;
	char mask[MAX_PATH_RA];
	char fn[MAX_PATH_RA];

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
	/* handle case when no file extension was given */
	if (i->ext[0] == '\0')
	{
		/* no extension given (it has to be either 'hea' or 'header' because we 
		   passed pl_check_meas() function) */
		FILE *fp;

		sprintf(fn, "%s%c%s.hea", i->dir, SEPARATOR, i->name);
		fp = ra_fopen(fn, "rb", 0);
		if (fp != NULL)
		{
			strcpy(i->ext, "hea");
			fclose(fp);
		}
		else
			strcpy(i->ext, "header");
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
	long l;

	if (!i)
		return;

	if (i->ri.signals)
		free(i->ri.signals);

	if (i->ri.info_strings)
	{
		for (j = 0; j < i->ri.num_infos; j++)
			free(i->ri.info_strings[j]);
		free(i->ri.info_strings);
	}

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

	if (i->ecg)
		free(i->ecg);
	if (i->text_annot)
	{
		for (l = 0; l < i->num_text_annot; l++)
		{
			if (i->text_annot[l].text)
				free(i->text_annot[l].text);
		}
		free(i->text_annot);
	}

	free(i);
} /* free_info_struct() */



int
get_data(struct info *i, int fast)
{
	int ret = -1;
	char fn[MAX_PATH_RA];
	FILE *fp;
	char buf[255];

	sprintf(fn, "%s%c%s.%s", i->dir, SEPARATOR, i->name, i->ext);
	fp = fopen(fn, "r");
	if (!fp)
		goto error;

	init_info(i);
	
	if (get_next_line(buf, 255, fp) != 0)
	{
		_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-mit/bih");
		goto error;  /* at least the record line must be available */
	}
	
	if (handle_rec_line(buf, i) != 0)
		goto error;

	if (i->ri.num_segments > 0)
	{
		_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-mit/bih");
		goto error;	/* don't support multi-segment records at the moment */
	}

	if ((ret = read_signal_infos(i, fp)) != 0)
		goto error;

	if ((ret = read_info_lines(i, fp)) != 0)
		goto error;

	if ((ret = get_channel_infos(i)) != 0)
		goto error;

	if (!fast)
	{
		if (i->ri.num_samples == 0)
		{
			if ((ret = estimate_samplenumber(i)) != 0)
			{
				_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-mit/bih");
				goto error;
			}
		}

		if ((ret = get_annotations(i)) != 0)
			goto error;

		/* if we have no beats, try there is a RRI file we can read */
		if ((ret = get_annot_from_rri_files(i)) != 0)
			goto error;
	}
    
	ret = 0;

 error:
	if (fp)
		fclose(fp);
	
	return ret;
} /* get_data() */


int
handle_rec_line(char *data, struct info *i)
{
	char *next;
	char sep;
	char *curr;

	/* record name */
	if ((curr = get_sep(data, &sep, &next, "/")) == NULL) return -1;
	strcpy(i->ri.rec_name, curr);
	if (sep == '/')
	{
		/* number of segments */
		if ((curr = get_sep(next, &sep, &next, NULL)) == NULL) return -1;
		i->ri.num_segments = atoi(curr);
	}

	/* number of signals */
	if ((curr = get_sep(next, &sep, &next, NULL)) == NULL) return -1;
	i->ri.num_signals = atoi(curr);

	/* from this point the remaining data is optional */
	/* samplerate */
	if ((curr = get_sep(next, &sep, &next, "/")) == NULL) return 0;
	i->ri.samplerate = atof(curr);
	if (sep == '/')
	{
		/* counter frequency */
		if ((curr = get_sep(next, &sep, &next, "(")) == NULL) return 0;
		i->ri.counter_freq = atof(curr);
		i->ri.counter_freq_valid = 1;

		if (sep == '(')
		{
			/* base counter value */
			if ((curr = get_sep(next, &sep, &next, NULL)) == NULL) return 0;
			i->ri.counter_value = atof(curr);
			i->ri.counter_value_valid = 1;
		}
	}

	/* number of samples per signal */
	if ((curr = get_sep(next, &sep, &next, NULL)) == NULL) return 0;
	i->ri.num_samples = atol(curr);
	
	/* base time */
	if ((curr = get_sep(next, &sep, &next, NULL)) == NULL) return 0;
	string_to_time(&(i->ri.base_time), curr);

	/* base date */
	if ((curr = get_sep(next, &sep, &next, NULL)) == NULL) return 0;
	string_to_date(&(i->ri.base_date), curr);

	return 0;
}  /* handle_rec_line() */


void
init_info(struct info *i)
{
	i->ri.samplerate = 250;
	i->ri.counter_freq = i->ri.samplerate;
} /* init_info() */


int
read_signal_infos(struct info *i, FILE *fp)
{
	char *last_file_name = NULL;
	int last_slot = 0;
	long slot_size = 0;
	char buf[255];
	int j;
	
	if (i->ri.num_signals <= 0)
		return 0;

	i->ri.signals = calloc(i->ri.num_signals, sizeof(struct signal_info));
	for (j = 0; j < i->ri.num_signals; j++)
	{
		if (get_next_line(buf, 255, fp) == -1)
		{
			_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-mit/bih");
			return -1;  /* there must be at least num-signals line */
		}
		init_signal(i->ri.signals + j);
		if (handle_signal_line(i, buf, i->ri.signals + j, last_file_name, last_slot) != 0)
		{
			_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-mit/bih");
			return -1;
		}
		last_file_name = i->ri.signals[j].file_name;
		
		/* If slot of current signal is 0 perhaps a new signal-file has started. Now in the previous
		   signal-infos, the number of slots in the previous signal-file must be set. */
		if (i->ri.signals[j].slot == 0)
		{
			int k;
			for (k = (j-1); k >= 0; k--)
			{
				i->ri.signals[k].num_slots = last_slot+1;
				i->ri.signals[k].slot_size = slot_size;
				if (i->ri.signals[k].slot == 0)
					break;
			}
			slot_size = 0;
		}
		i->ri.signals[j].slot_offset = slot_size;
		slot_size += i->ri.signals[j].samples_per_frame;
		last_slot = i->ri.signals[j].slot;
	}
	if (last_slot >= 0)
	{
		int k;
		for (k = (i->ri.num_signals - 1); k >= 0; k--)
		{
			i->ri.signals[k].num_slots = last_slot+1;
			i->ri.signals[k].slot_size = slot_size;
			if (i->ri.signals[k].slot == 0)
				break;
		}
	}

	return 0;
} /* read_signal_infos() */


void
init_signal(struct signal_info *si)
{
	si->adc_gain = 200;
	si->num_slots = 1;
} /* init_signal() */


int
handle_signal_line(struct info *i, char *data, struct signal_info *si, char *last_file_name, int last_slot)
{
	char *next;
	char sep;
	char *curr;

	if ((data == NULL) || (data[0] == '\0'))
		return -1;

	/* get file-name */
	next = strchr(data, ' ');
	if (next)
	{
		*next = '\0';
		next++;
	}
	strcpy(si->file_name, data);
	/* special handling of rec-name is needed when the recording was
	   exported with the GE MARS system */
	if (strncmp(si->file_name, "MITSIG", 6) == 0)
		sprintf(si->file_name, "%s.sig", i->name);

	if (last_file_name && (strcmp(si->file_name, last_file_name) == 0))
		si->slot = last_slot + 1;

	/* get format */
	if ((curr = get_sep(next, &sep, &next, "x:+")) == NULL) return -1;
	si->format = atoi(curr);
	if (sep == 'x')
	{
		/* samples per frame */
		if ((curr = get_sep(next, &sep, &next, ":+")) == NULL) return 0;
		si->samples_per_frame = atoi(curr);
	}
	else
		si->samples_per_frame = 1;

	if (sep == ':')
	{
		/* skew */
		if ((curr = get_sep(next, &sep, &next, "+")) == NULL) return 0;
		si->skew = atoi(curr);
	}
	if (sep == '+')
	{
		/* offset */
		if ((curr = get_sep(next, &sep, &next, NULL)) == NULL) return 0;
		si->offset = atoi(curr);
	}

	/* ADC gain (number of sample-steps for 1 unit) */
	if ((curr = get_sep(next, &sep, &next, "(/")) == NULL) return 0;
	si->adc_gain = atof(curr);
	if (si->adc_gain <= 0.0)
		si->adc_gain = 200.0;
	if (sep == '(')
	{
		/* ADC baseline offset */
		if ((curr = get_sep(next, &sep, &next, "/")) == NULL) return 0;
		si->adc_baseline = atof(curr);
	}
	if (sep == '/')
	{
		/* unit */
		if ((curr = get_sep(next, &sep, &next, NULL)) == NULL) return 0;
		strcpy(si->unit, curr);
	}

	/* number of bits */
	if ((curr = get_sep(next, &sep, &next, NULL)) == NULL) return 0;
	si->num_bits = atoi(curr);

	/* zero value */
	if ((curr = get_sep(next, &sep, &next, NULL)) == NULL) return 0;
	si->zero_value = atoi(curr);

	/* initial value of sample 0 if differential format is used */
	if ((curr = get_sep(next, &sep, &next, NULL)) == NULL) return 0;
	si->initial_value = atoi(curr);

	/* checksum */
	if ((curr = get_sep(next, &sep, &next, NULL)) == NULL) return 0;
	si->checksum = (short)atoi(curr);
	
	/* block-size (see description of WFDB) */
	if ((curr = get_sep(next, &sep, &next, NULL)) == NULL) return 0;
	si->block_size = atoi(curr);

	/* signal description */
	if (next)
	{
		strcpy(si->description, next);
		/* again, special handling of ECG exported in GE MARS system */
		if (strcmp(next, "MARS export") == 0)
		{
			if (i->ri.num_signals == 12)
			{
				switch (si->slot)
				{
				case 0: strcpy(si->description, "I"); break;
				case 1: strcpy(si->description, "II"); break;
				case 2: strcpy(si->description, "III"); break;
				case 3: strcpy(si->description, "aVR"); break;
				case 4: strcpy(si->description, "aVL"); break;
				case 5: strcpy(si->description, "aVF"); break;
				case 6: strcpy(si->description, "V1"); break;
				case 7: strcpy(si->description, "V2"); break;
				case 8: strcpy(si->description, "V3"); break;
				case 9: strcpy(si->description, "V4"); break;
				case 10: strcpy(si->description, "V5"); break;
				case 11: strcpy(si->description, "V6"); break;
				}
			}
			else
				sprintf(si->description, "ecg-ch#%d", si->slot+1);
		}
		/* and special handling of ECG exported with Getemed system;
		   just say that they are all ecg channels even when some of
		   them could be for example respiration channels, but at the
		   moment I don't know a better way */
		if (strcmp(next, "CardioDay export") == 0)
			sprintf(si->description, "ecg-ch#%d", si->slot+1);
	}

	return 0;
} /* handle_signal_line() */


int
read_info_lines(struct info *i, FILE *fp)
{
	char buf[255];

	while (fgets(buf, 255, fp))
	{
		int cnt = 0;

		i->ri.num_infos++;
		i->ri.info_strings = realloc(i->ri.info_strings, sizeof(char *) * i->ri.num_infos);
		while ((buf[cnt] == '#') || (buf[cnt] == ' '))
			cnt++;

		i->ri.info_strings[i->ri.num_infos - 1] = malloc(strlen(buf+cnt)+1);
		strcpy(i->ri.info_strings[i->ri.num_infos - 1], buf + cnt);
	}

	return 0;
} /* read_info_lines() */


int
get_channel_infos(struct info *i)
{
	int j;
	char file_wo_ext[MAX_PATH_RA];

	/* try to get channel infos */
	i->ch_type_infos = malloc(sizeof(struct ra_est_ch_infos) * i->ri.num_signals);
	i->mm_sec = 1;
	sprintf(file_wo_ext, "%s%c%s", i->dir, SEPARATOR, i->name);
	for (j = 0; j < i->ri.num_signals; j++)
	{
		if (ra_est_ch_type(i->ra, i->dir, file_wo_ext, 0,
				   i->ri.signals[j].description, &(i->ch_type_infos[j])) == 0)
		{
			i->ch_type_infos[j].type = RA_CH_TYPE_UNKNOWN;
			i->ch_type_infos[j].mm_sec = 1;
			i->ch_type_infos[j].mm_unit = 1;
			i->ch_type_infos[j].offset = 0;
			i->ch_type_infos[j].centered = 0;
			i->ch_type_infos[j].center_value = 0;

 			/* not found but try some channel names in MIT/BIH-files  */
 			if ((RA_STRICMP(i->ri.signals[j].description, "I") == 0)
 			    || (RA_STRICMP(i->ri.signals[j].description, "II") == 0)
 			    || (RA_STRICMP(i->ri.signals[j].description, "III") == 0)
 			    || (RA_STRICMP(i->ri.signals[j].description, "VX") == 0)
 			    || (RA_STRICMP(i->ri.signals[j].description, "VY") == 0)
 			    || (RA_STRICMP(i->ri.signals[j].description, "VZ") == 0))
 			{
 				ra_est_ch_type(i->ra, i->dir, file_wo_ext, 0, "ecg", &(i->ch_type_infos[j]));
 			}
		}
		else
		{
			/* check if unit is micro-something and scale amp.resolution down */
			if (i->ri.signals[j].unit[0] == 'u')
				i->ch_type_infos[j].mm_unit /= 1000.0;
		}

		if (i->mm_sec < i->ch_type_infos[j].mm_sec)
			i->mm_sec = i->ch_type_infos[j].mm_sec;
	}

	return 0;
} /* get_channel_infos() */


int
get_next_line(char *buf, int size, FILE *fp)
{
    int ret = -1;
    size_t i;

    while (fgets(buf, size, fp))
    {
	if (buf[0] != '#')
	{
	    ret = 0;
	    break;
	}
    }

    /* remove trailing newline and carriage return */
    for (i = (strlen(buf)-1); i >= 0; i--)
    {
	    if ((buf[i] == '\n') || (buf[i] == '\r'))
		    buf[i] = '\0';
	    else
		    break;
    }

    return ret;
}  /* get_next_line() */


char *
get_sep(char *data, char *sep, char **next, const char *sep_allowed)
{
	size_t i, j, num_sep;

	if (sep == NULL)
		return NULL;
	*sep = '\0';

	if ((data == NULL) || (*data == '\0'))
		return NULL;

	num_sep = 0;
	if (sep_allowed)
		num_sep = strlen(sep_allowed);

	for (i = 0; i < strlen(data); i++)
	{
		int sep_found = 0;

		if (data[i] == ' ')
			sep_found = 1;
		else if (sep_allowed != NULL)
		{
			for (j = 0; j < num_sep; j++)
			{
				if (sep_allowed[j] == data[i])
				{
					sep_found = 1;
					break;
				}
			}
		}

		if (sep_found)
		{
			*sep = data[i];
			data[i] = '\0';  /* so strcpy and friends works */
			i++;  /* to point to pos after seperator */
			break;
		}
	}

	while ((data[i] == ' ') || (data[i] == '\t'))
		i++;

	*next = data+i;

	return data;
}  /* get_sep() */


void
string_to_date(struct date *d, char *s)
{
	char *p;

	d->day = (short)atoi(s);
	p = strchr(s, '/');
	if (!p) return;
	d->month = (short)atoi(p+1);
	p = strchr(p+1, '/');
	if (!p) return;
	d->year = (short)atoi(p+1);
}  /* string_to_date() */


void
string_to_time(struct time *d, char *s)
{
	char *p;

	d->hour = (short)atoi(s);
	p = strchr(s, ':');
	if (!p) return;
	d->min = (short)atoi(p+1);
	p = strchr(p+1, ':');
	if (!p) return;
	d->sec = (short)atoi(p+1);
}  /* string_to_time() */


int
get_annotations(struct info *i)
{
	char fn[MAX_PATH_RA], annot_ext[MAX_PATH_RA];
	FILE *fp;
	long l;
	int is_darwin_exported_file;

	if (get_annot_file_extension(i, annot_ext) != 0)
		return 0;

	sprintf(fn, "%s%c%s.%s", i->dir, SEPARATOR, i->name, annot_ext);
	fp = ra_fopen(fn, "rb", 0);
	if (fp)
	{
		read_wfdb_annot(i, &(i->num_ecg_annot), &(i->ecg), fp, i->ri.samplerate);
		fclose(fp);
	}
	else
		return -1;

	if (RA_STRICMP(i->ext, "tom") == 0)
		is_darwin_exported_file = 1;

	i->num_ecg_use = 0;
	for (l = 0; l < i->num_ecg_annot; l++)
	{
		switch (i->ecg[l].annot_orig)
		{
		case NORMAL:
		case LBBB:
		case RBBB:
		case ABERR:
		case APC:
		case SVPB:
		case AESC:
		case SVESC:
		case BBB:
			SET_CLASS(ECG_CLASS_SINUS, i->ecg[l].annot_ra);
			i->ecg[l].use_it = 1;
			break;
		case PVC:
		case FUSION:
		case NPC:
		case VESC:
		case NESC:
		case RONT:
			SET_CLASS(ECG_CLASS_VENT, i->ecg[l].annot_ra);
			i->ecg[l].use_it = 1;
			break;
		case PACE:
		case PFUS:
			SET_CLASS(ECG_CLASS_PACED, i->ecg[l].annot_ra);
			if (is_darwin_exported_file)
			{
				switch (i->ecg[l].annot_sub_type)
				{
				  /* according to email from company '1' should be atrial paced and
				     '2' should be ventricular paced but it looks like that they are
				     exchanged */
				case 2: 
					SET_SUBCLASS(ECG_CLASS_PACED_ATRIUM, i->ecg[l].annot_ra);
					break;
				case 1:
					SET_SUBCLASS(ECG_CLASS_PACED_VENTRICLE, i->ecg[l].annot_ra);
					break;
				case 3:
					SET_SUBCLASS(ECG_CLASS_PACED_ATRIUM, i->ecg[l].annot_ra);
					SET_SUBCLASS(ECG_CLASS_PACED_VENTRICLE, i->ecg[l].annot_ra);
					break;
				}
			}
			i->ecg[l].use_it = 1;
			break;
		case UNKNOWN:
		case ARFCT:
			SET_CLASS(ECG_CLASS_ARTIFACT, i->ecg[l].annot_ra);
			i->ecg[l].use_it = 1;
			break;
		}

		if (i->ecg[l].use_it)
			i->num_ecg_use++;
	}

	return 0;
} /* get_annotations() */


int
get_annot_file_extension(struct info *i, char *annot_ext)
{
	int ret, m, n, num, num_ok;
	char **files, mask[MAX_PATH_RA], *p;

	num = 0;
	files = NULL;

	/* get all files with the filename set to the rec-name */
	sprintf(mask, "%s.*", i->name);
	find_files(i->dir, mask, &files, &num, NULL);

	/* ignore files with the extensions 'hea', 'dat' and 'sig' and the
	   data-files */
	num_ok = num;
	for (n = 0; n < num; n++)
	{
		int remove = 0;
		char *p = strrchr(files[n], '.');
		if (p != NULL)
		{
			if (RA_STRNICMP(p+1, "hea", 3) == 0) remove = 1; /* MIT header file */
			if (RA_STRNICMP(p+1, "header", 6) == 0) remove = 1; /* MIT header file */
			if (RA_STRNICMP(p+1, "dat", 3) == 0) remove = 1; /* MIT signal file */
			if (RA_STRNICMP(p+1, "mte", 3) == 0) remove = 1; /* libRASCH eval-file */
			if (RA_STRNICMP(p+1, "lre", 3) == 0) remove = 1; /* dito */
			if (RA_STRNICMP(p+1, "sig", 3) == 0) remove = 1; /* GE-MARS ext used for signal files */
			if (RA_STRNICMP(p+1, "daw", 3) == 0) remove = 1; /* Darwin ECG file */
			if (RA_STRNICMP(p+1, "txt", 3) == 0) remove = 1; /* Getemed qrs-txt export file */
			if (p[strlen(files[n])-1] == '~') remove = 1; /* backup file */

			for (m = 0; m < i->ri.num_signals; m++)
			{
				char *p2 = strrchr(i->ri.signals[m].file_name, '.');

				if (p2 == NULL)
					continue;

				if (strcmp(p+1, p2+1) == 0)
				{
					remove = 1;
					break;
				}
			}
		}
		else
			remove = 1;

		if (remove)
		{
			ra_free_mem(files[n]);
			files[n] = NULL;
			num_ok--;
		}
	}

	if (num_ok <= 0)
		return -1;

	/* the remaining file (hopefully only one) is in all probability the
	   annotation file */
	/* TODO: handle more than one annotation files; now use the first one */
	ret = -1;
	for (n = 0; n < num; n++)
	{
		if (files[n] == NULL)
			continue;

		p = strrchr(files[n], '.');
		strncpy(annot_ext, p+1, MAX_PATH_RA);
		ret = 0;
		break;
	}

	/* clean up */
	for (n = 0; n < num; n++)
	{
		if (files[n])
			ra_free_mem(files[n]);
	}
	ra_free_mem(files);

	return ret;
} /* get_annot_file_extension() */


int
read_wfdb_annot(struct info *i, long *num, struct annot_info **ai, FILE *fp, double samplerate)
{
	int ret = -1; 
	int aha_format = 0;
	unsigned char b[2];

	if (fread(b, 1, 2, fp) != 2)
		return ret;
	aha_format = is_aha_format(b);

	if (aha_format)
		ret = read_aha_annot(num, ai, fp, samplerate);
	else
		ret = read_mit_annot(i, num, ai, fp);

	return ret;
} /* read_wfdb_annot() */


/*
 * Function tries to figure out, if annotation file is stored
 * in MIT or AHA format. If stored in AHA format, a vaule != 0
 * is returned.
 *
 * Comment from file annot.c/WFDB library (www.physionet.org):
 *    Try to figure out what format the file is in.  AHA-format files
 *    begin with a null byte and an ASCII character which is one
 *    of the legal AHA annotation codes other than '[' or ']'.
 *    MIT annotation files cannot begin in this way.
 *
 */
int
is_aha_format(unsigned char *b)
{
	int is_aha = 0;

	if ((b[0] == 0) && (ammap(b[1]) != NOTQRS) && (b[1] != '[') && (b[1] != ']'))
		is_aha = 1;

	return is_aha;
} /* is_aha_format() */


int
read_aha_annot(long *num, struct annot_info **ai, FILE *fp, double samplerate)
{
	int ret = -1;
	size_t l, n, cnt;
	struct aha_annot_info *buf = NULL;
	int is_mit_aha = 0;

	*num = 0;

	/* read data in buffer */
	fseek(fp, 0, SEEK_END);
	n = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	n /= sizeof(struct aha_annot_info);

	*ai = calloc(n, sizeof(struct annot_info));
	buf = calloc(n, sizeof(struct aha_annot_info));
	if (fread(buf, sizeof(struct aha_annot_info), n, fp) != n)
		goto error;
		
	/* check if original AHA annotations or WFDB written AHA annotations */
	if (buf[0].mit_subtype != 0)
		is_mit_aha = 1;

	/* decode data */
	cnt = 0;
	for (l = 0; l < n; l++)
	{
		long i;

		/* handle endianess */
		le32_to_cpu(buf[l].time);
		le16_to_cpu(buf[l].serial_no);

		/* get position */
		i = (long)(buf[l].time & 0x00ff0000) << 8;
		i |= (long)(buf[l].time & 0xff000000) >> 8;
		i |= (long)(buf[l].time & 0x000000ff) << 8;
		i |= (long)(buf[l].time & 0x0000ff00) >> 8;

		/* if orignal AHA annotation, than position is in msec */
		if (!is_mit_aha)
			i = (long)((double)i * (samplerate / 1000.0));
			
		/* if qrs-complex -> save data in ai array */
		if (ammap(buf[l].annot) != NOTQRS)
		{
			(*ai)[cnt].annot_orig = ammap(buf[l].annot);
			(*ai)[cnt].pos = i;
			(*ai)[cnt].serial_no = buf[l].serial_no;
			cnt++;
		}
	}
	*num = (long)cnt;

	ret = 0;

 error:
	if (buf)
		free(buf);

	return ret;
} /* read_aha_annot() */


int
read_mit_annot(struct info *i, long *num, struct annot_info **ai, FILE *fp)
{
	int ret = -1;
	size_t pos_save, cnt, buf_pos, n;
	unsigned short *buf = NULL;
	int textual_annot = 0;
	int annot_num = 0;
	int annot_sub_type = 0;
	int annot_ch = 0;

	*num = 0;

	/* read data in buffer */
	fseek(fp, 0, SEEK_END);
	n = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	n /= 2;

	*ai = calloc(n, sizeof(struct annot_info));
	buf = malloc(2 * n);
	if (fread(buf, 2, n, fp) != n)
		goto error;
		
	/* decode data */
	pos_save = 0;
	cnt = 0;
	buf_pos = 0;
	while (buf_pos < n)
	{
		long a, t;
		int add;
		
		le16_to_cpu(buf[buf_pos]);

		a = (buf[buf_pos] & 0xfc00) >> 10;
		t = buf[buf_pos] & 0x03ff;

/* 		fprintf(stderr, "bufpos:%d  -  code: %d  time: %d  (elapsed time: %d)", buf_pos, a, t, pos_save); */
		if (a >= 59)
		{
			char *c;

			switch (a)
			{
			case 59: /* SKIP */
				if ((buf_pos + 2) >= n)
					break;
				t = (long)(buf[buf_pos+1]) << 16;
				t += (long)(buf[buf_pos+2]);
				
				buf_pos += 2;
				pos_save += t;
/* 				fprintf(stderr, "  -> time to skip: %d", t); */
				break;
			case 60: /* num */
				annot_num = t;
				break;
			case 61: /* sub type */
				annot_sub_type = t;
				if ((cnt > 0) && ((size_t)((*ai)[cnt-1].pos) == pos_save))
					(*ai)[cnt-1].annot_sub_type = t;
				break;
			case 62: /* channel */
				annot_ch = t;
				break;
			case 63: /* AUX */
				if (textual_annot)
				{
					c = (char *)&(buf[buf_pos+1]);
					i->num_text_annot++;
					i->text_annot = realloc(i->text_annot, sizeof(struct annot_info) * i->num_text_annot);
					i->text_annot[i->num_text_annot-1].pos = (long)pos_save;
					i->text_annot[i->num_text_annot-1].text = calloc(t + 1, sizeof(char));
					strncpy(i->text_annot[i->num_text_annot-1].text, c, t);

					i->text_annot[i->num_text_annot-1].annot_num = annot_num;
					i->text_annot[i->num_text_annot-1].annot_sub_type = annot_sub_type;
					i->text_annot[i->num_text_annot-1].annot_ch = annot_ch;
				}

				add = t / 2;
				if (t % 2)
					add++;
				buf_pos += add;
/* 				fprintf(stderr, " -> aux-info (add=%d): %s", add, c); */
				break;
			default:
				break;
			}
		}
		else
		{
			if ((a == NOTE) || (a == MEASURE))
				textual_annot = 1;
			else
				textual_annot = 0;

			if (isqrs(a) || (a == ARFCT))
			{
				(*ai)[cnt].annot_orig = a;
				(*ai)[cnt].pos = (long)pos_save + t;
				(*ai)[cnt].annot_sub_type = annot_sub_type;
				cnt++;
			}

			pos_save += t;
		}
		buf_pos++;
/* 		fprintf(stderr, "\n"); */
	}
	*num = (long)cnt;

	ret = 0;

 error:
	if (buf)
		free(buf);

	return ret;
} /* read_mit_annot() */


int
estimate_samplenumber(struct info *i)
{
	char fn[MAX_PATH_RA];
	FILE *fp;
	size_t len;

	sprintf(fn, "%s%c%s", i->dir, SEPARATOR, i->ri.signals[0].file_name);
	if ((fp = fopen(fn, "rb")) == NULL)
		return -1;

	fseek(fp, 0, SEEK_END);
	len = ftell(fp);
	fclose(fp);

	/* number of samples (in bytes) */
	i->ri.num_samples = (long)len / i->ri.signals[0].slot_size;

	/* correct number of samples according to sample-size */
	switch (i->ri.signals[0].format)
	{
	case 8:
	case 80:
		/* do nothing because 1 byte -> 1 sample */
		break;
	case 310:
	case 311:
		i->ri.num_samples = (long)((double)i->ri.num_samples * (3.0/4.0));
		break;
	case 212:
		i->ri.num_samples = (long)((double)i->ri.num_samples * (2.0/3.0));
		break;
	case 16:
	case 61:
	case 160:
		i->ri.num_samples /= 2;
		break;
	}

	return 0;
} /* estimate_samplenumber() */


int
get_annot_from_rri_files(struct info *i)
{
	char fn[MAX_PATH_RA];
	FILE *fp;
	int ret = 0;

	/* try to read annotations from Getemed qrs.txt file */
	sprintf(fn, "%s%c%s.txt", i->dir, SEPARATOR, i->name);
	fp = fopen(fn, "r");
	if (fp == NULL)
		return ret;
	if (check_if_getemed_rri(fp) == 0)
	{
		fclose(fp);
		return ret;
	}

	ret = read_getemed_rri(i, fp);

	fclose(fp);

	return ret;
} /* get_annot_from_rri_files() */


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
read_getemed_rri(struct info *i, FILE *fp)
{
	int ret = -1;
	long buf_size = 0;
	char buf[100];
	long curr_pos = 0;
	double pos_scale;

	fseek(fp, 0, SEEK_SET);

	/* skip first 3 lines */
	fgets(buf, 100, fp);
	fgets(buf, 100, fp);
	fgets(buf, 100, fp);

	/* read beats data */
	curr_pos = 0;
	pos_scale = i->ri.samplerate / 1024.0;
	while (fgets(buf, 100, fp))
	{
		size_t line_pos, len;
		double pos_temp;

		if (i->num_ecg_annot == buf_size)
		{
			buf_size += 1000;
			i->ecg = realloc(i->ecg, sizeof(struct annot_info) * buf_size);
		}
		i->num_ecg_annot++;
		pos_temp = atof(buf);
		i->ecg[i->num_ecg_annot-1].pos = (long)(pos_temp * pos_scale);

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
			i->ecg[i->num_ecg_annot-1].annot_orig = ECG_CLASS_SINUS;
			break;
		case 'V':
		case 'B':
			i->ecg[i->num_ecg_annot-1].annot_orig = ECG_CLASS_VENT;
			break;
		case 'P':
			i->ecg[i->num_ecg_annot-1].annot_orig = ECG_CLASS_PACED;
			break;
		default:
			i->ecg[i->num_ecg_annot-1].annot_orig = ECG_CLASS_ARTIFACT;
			break;
		}
		i->ecg[i->num_ecg_annot-1].annot_ra = i->ecg[i->num_ecg_annot-1].annot_orig;
		i->ecg[i->num_ecg_annot-1].use_it = 1;
	}
	i->num_ecg_use = i->num_ecg_annot;

	ret = 0;

	return ret;
} /* read_getemed_rri() */


int
pl_get_info_id(any_handle h, int id, value_handle vh)
{
	int ret = 0;
	struct ra_meas *meas;
	struct info *i;
	char t[MAX_PATH_RA];

	meas = ra_meas_handle_from_any_handle(h);
	i = meas->priv;

	switch (id)
	{
	case RA_INFO_NUM_OBJ_INFOS_L:
		ra_value_set_long(vh, _num_infos_obj);
		break;
	case RA_INFO_NUM_REC_GEN_INFOS_L:
		ra_value_set_long(vh, _num_infos_rec_gen);
		break;
	case RA_INFO_NUM_REC_DEV_INFOS_L:
		ra_value_set_long(vh, 0);
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
	case RA_INFO_REC_GENERAL:
		if ((index >= 0) && (index < _num_infos_rec_gen))
			idx_arr = _infos_rec_gen;
		else
			idx_arr = NULL;
		break;
	case RA_INFO_REC_CHANNEL:
		if ((index >= 0) && (index < _num_infos_rec_ch))
			idx_arr = _infos_rec_ch;
		else
			idx_arr = NULL;
		break;
	case RA_INFO_OBJECT:
		if ((index >= 0) && (index < _num_infos_obj))
			idx_arr = _infos_obj;
		else
			idx_arr = NULL;
		break;
	default:
		idx_arr = NULL;
		break;
	}

	if (idx_arr)
		return get_info_id(h, idx_arr[index], vh);

	_ra_set_error(h, RA_ERR_UNKNOWN_INFO, "plugin-mit/bih");

	return -1;
} /* pl_get_info_idx() */


int
get_info_id(any_handle h, int id, value_handle vh)
{
	struct ra_meas *meas;
	struct info *i;
	int ret = 0;

	/* get infos about measurement info */
	struct meta_info *meta_inf = get_meta_info(id);

	/* all var's ok? */
	if ((vh == NULL) || (meta_inf == NULL) || (h == NULL))
	{
		_ra_set_error(h, RA_ERR_ERROR, "plugin-mit/bih");
		return -1;
	}

	meas = ra_meas_handle_from_any_handle(h);
	i = meas->priv;

	set_meta_info(vh, meta_inf->name, meta_inf->desc, meta_inf->id);

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
	    ((ch < 0) || (ch >= i->ri.num_signals)))
	{
		_ra_set_error(i->ra, RA_ERR_OUT_OF_RANGE, "plugin-mit/bih");
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
		ra_value_set_string(vh, "MIT-BIH recording");
		break;
	case RA_INFO_REC_GEN_TIME_C:
		sprintf(t, "%02d:%02d:%02d", i->ri.base_time.hour,
			i->ri.base_time.min, i->ri.base_time.sec);
		ra_value_set_string(vh, t);
		break;
	case RA_INFO_REC_GEN_DATE_C:
		sprintf(t, "%02d.%02d.%d", i->ri.base_date.day,
			i->ri.base_date.month, i->ri.base_date.year);
		ra_value_set_string(vh, t);
		break;
	case RA_INFO_REC_GEN_DURATION_SEC_D:
		d = ((double) i->ri.num_samples / (double) i->ri.samplerate);
		ra_value_set_double(vh, d);
		break;
	case RA_INFO_REC_GEN_NUMBER_L:
		ra_value_set_long(vh, 0);
		break;
	case RA_INFO_REC_GEN_NUM_SUB_REC_L:
		ra_value_set_long(vh, 0);
		break;
	case RA_INFO_REC_GEN_NUM_CHANNEL_L:
		ra_value_set_long(vh, i->ri.num_signals);
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
		if (i->ri.signals[ch].description)
			ra_value_set_string(vh, i->ri.signals[ch].description);
		else
		{
			sprintf(t, gettext("lead-%ld"), ch + 1);
			ra_value_set_string(vh, t);
		}
		break;
	case RA_INFO_REC_CH_DESC_C:
		ra_value_set_string(vh, "");
		break;
	case RA_INFO_REC_CH_NUM_SAMPLES_L:
		ra_value_set_long(vh, i->ri.num_samples * i->ri.signals[ch].samples_per_frame);
		break;
	case RA_INFO_REC_CH_SAMPLERATE_D:
		ra_value_set_double(vh, (double)i->ri.samplerate * (double)i->ri.signals[ch].samples_per_frame);
		break;
	case RA_INFO_REC_CH_NUM_BITS_L:
		ra_value_set_long(vh, i->ri.signals[ch].num_bits);
		break;
	case RA_INFO_REC_CH_AMP_RESOLUTION_D:
		ra_value_set_double(vh, i->ri.signals[ch].adc_gain);
		break;
	case RA_INFO_REC_CH_SAMPLE_TYPE_L:
		ra_value_set_long(vh, RA_CH_SAMPLE_TYPE_INT);
		break;
	case RA_INFO_REC_CH_UNIT_C:
		ra_value_set_string(vh, i->ri.signals[ch].unit);
		break;
	case RA_INFO_REC_GEN_MM_SEC_D:
		ra_value_set_double(vh, i->mm_sec);
		break;
	case RA_INFO_REC_CH_CENTER_VALUE_D:
		if (i->ch_type_infos[ch].centered)
			ra_value_set_double(vh, i->ch_type_infos[ch].center_value);
		else
			ret = -1;
		break;
	case RA_INFO_REC_CH_CENTER_SAMPLE_D:
		ra_value_set_double(vh, i->ri.signals[ch].zero_value);
		break;
	case RA_INFO_REC_CH_MM_UNIT_D:
		ra_value_set_double(vh, i->ch_type_infos[ch].mm_unit);
		break;
	case RA_INFO_REC_CH_TYPE_L:
		ra_value_set_long(vh, i->ch_type_infos[ch].type);
		break;
	default:
		_ra_set_error(i->ra, RA_ERR_UNKNOWN_INFO, "plugin-mit/bih");
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
		ra_value_set_string(vh, i->name);
		break;
	default:
		_ra_set_error(i->ra, RA_ERR_UNKNOWN_INFO, "plugin-mit/bih");
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
} /* pl_get_raw_b() */


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
read_raw(struct info *i, int ch, size_t start, size_t num, double *data, DWORD *data_raw)
{
	char fn[MAX_PATH_RA];
	size_t l, start_use, num_use, num_read, num_ret = 0;
	FILE *fp = NULL;
	short *buf = NULL;
	long num_samples;
	size_t start_frame;
	size_t frame_offset;
	size_t frame_read;

	if (ch >= i->ri.num_signals)
	{
		_ra_set_error(i->ra, RA_ERR_OUT_OF_RANGE, "plugin-mit/bih");
		goto error;
	}

	start_frame = start / i->ri.signals[ch].samples_per_frame;
	start_use = start_frame * i->ri.signals[ch].slot_size;
	frame_offset = start % i->ri.signals[ch].samples_per_frame;

	num_samples = i->ri.num_samples * i->ri.signals[ch].samples_per_frame;
	frame_read = (frame_offset + num) / i->ri.signals[ch].samples_per_frame;
	if ((frame_offset + num) % i->ri.signals[ch].samples_per_frame)
		frame_read++;
	if ((long)(start_frame + frame_read) >= i->ri.num_samples)
	{
		if ((size_t)(i->ri.num_samples) < start_frame)
		{
			_ra_set_error(i->ra, RA_WARN_NO_DATA, "plugin-mit/bih");
			goto error;
		}
		frame_read = i->ri.num_samples - start_frame;
	}

	num_use = frame_read * i->ri.signals[ch].slot_size;
	buf = malloc(sizeof(short) * num_use);

	sprintf(fn, "%s%c%s", i->dir, SEPARATOR, i->ri.signals[ch].file_name);
	if ((fp = ra_fopen(fn, "rb", 0)) == NULL)
	{
		_ra_set_error(i->ra, RA_ERR_MEAS_CORRUPT, "plugin-mit/bih");
		goto error;
	}

	if (i->ri.signals[ch].format == 8)
		num_read = read_raw_8bit(fp, start_use, num_use, buf, 1);
	else if (i->ri.signals[ch].format == 80)
		num_read = read_raw_8bit(fp, start_use, num_use, buf, 0);
	else if (i->ri.signals[ch].format == 310)
		num_read = read_raw_10bit(fp, start_use, num_use, buf, 1);
	else if (i->ri.signals[ch].format == 311)
		num_read = read_raw_10bit(fp, start_use, num_use, buf, 0);
	else if (i->ri.signals[ch].format == 212)
		num_read = read_raw_12bit(fp, start_use, num_use, buf);
	else if (i->ri.signals[ch].format == 16)
		num_read = read_raw_16bit(fp, start_use, num_use, buf, 0, 0);
	else if (i->ri.signals[ch].format == 61)
		num_read = read_raw_16bit(fp, start_use, num_use, buf, 0, 1);
	else if (i->ri.signals[ch].format == 160)
		num_read = read_raw_16bit(fp, start_use, num_use, buf, 32768, 0);

	for (l = 0; l < num_read; l += i->ri.signals[ch].slot_size)
	{
		 long m;

		for (m = (long)frame_offset; m < i->ri.signals[ch].samples_per_frame; m++)
		{
			size_t offset = l+i->ri.signals[ch].slot_offset + m;

			if ((offset >= num_read) || (num_ret >= num))
				break;

			if (data)
				data[num_ret] = (double)(buf[offset] - i->ri.signals[ch].zero_value) / i->ri.signals[ch].adc_gain;
			if (data_raw)
				data_raw[num_ret] = (DWORD)(buf[offset] - i->ri.signals[ch].adc_baseline);
			num_ret++;
		}
		if (num_ret >= num)
			break;

		frame_offset = 0; /* only needed for first frame */
	}

 error:
	if (buf)
		free(buf);
	if (fp)
		fclose(fp);

	return num_ret;
} /* read_raw() */


size_t
read_raw_8bit(FILE *fp, size_t start, size_t num, short *buf, int diff)
{
	size_t l, n = 0;
	unsigned char *read_buf;

	read_buf = malloc(num);

	if (diff)
	{
		/* format 8 */
	}
	else
	{
		/* format 80 */
		if (fseek(fp, (long)start, SEEK_SET) < 0)
			return 0;

		n = fread(read_buf, 1, num, fp);
		for (l = 0; l < n; l++)
			buf[l] = (short)(read_buf[l]) - 128;
	}

	free(read_buf);

	return n;
} /* read_raw_8bit() */


size_t
read_raw_10bit(FILE *fp, size_t start, size_t num, short *buf, int format_310)
{
	size_t start_use;
	size_t n_read, n, n_ret, l, j;
	unsigned long *buf_read = NULL;
	short *buf_temp = NULL;

	start_use = start / 3;
	if (fseek(fp, (long)start_use*4, SEEK_SET) < 0)
		return 0;

	n_read = num;
	n_read /= 3;
	if (start % 3)
		n_read++;

	buf_read = malloc(n_read * 4);
	n = fread(buf_read, 4, num, fp);

	for (j = 0; j < n; j++)
		le32_to_cpu(buf_read[j]);

	buf_temp = malloc(n_read * 3);
	n_ret = 0;
	if (format_310)
	{
		/* format 310 */
		for (l = 0; l < n; l++)
		{
			buf_temp[n_ret++] = (short)((buf_read[l] & 0x000008ff) >> 1);
			buf_temp[n_ret++] = (short)((buf_read[l] & 0x08ff0000) >> 17);
			buf_temp[n_ret++] = (short)((buf_read[l] & 0x0000ffff) >> 11) + (short)(buf_read[l] >> 27);
		}
	}
	else
	{
		/* format 311 */
		for (l = 0; l < n; l++)
		{
			buf_temp[n_ret++] = (short)(buf_read[l] & 0x000004ff);
			buf_temp[n_ret++] = (short)((buf_read[l] & 0x000ffc00) >> 10);
			buf_temp[n_ret++] = (short)((buf_read[l] & 0x4ff00000) >> 20);
		}
	}

	if (n_ret > (size_t)num)
		n_ret = num;
	memcpy(buf, buf_temp, sizeof(short) * n_ret);

	free(buf_temp);
	free(buf_read);

	return n_ret;
} /* read_raw_10bit() */


size_t
read_raw_12bit(FILE *fp, size_t start, size_t num, short *buf)
{
	size_t start_use;
	size_t n_read, n;
	size_t cnt_data, cnt_buf, l;
	unsigned char *buf_read = NULL;

	start_use = start / 2;
	if (fseek(fp, (long)start_use*3, SEEK_SET) < 0)
		return 0;

	n_read = num;
	n_read /= 2;
	if (start % 2)
		n_read++;

	buf_read = malloc(n_read * 3);
	n = fread(buf_read, 1, n_read*3, fp);

	cnt_data = 0;
	cnt_buf = 0;
	if (start % 2)
	{
		buf[cnt_data] = (short)(((buf_read[1] & 0xf0) << 4) + buf_read[2]);
		if (buf[cnt_data] & 0x800)
			buf[cnt_data] |= ~(0xfff);
		cnt_data++;
		cnt_buf += 3;
	}

	for (l = cnt_buf; l < n; l += 3)
	{
		if (cnt_data >= (size_t)num)
			break;

		buf[cnt_data] = (short)(buf_read[l] + ((buf_read[l+1] & 0x0f) << 8));
		if (buf[cnt_data] & 0x800)
			buf[cnt_data] |= ~(0xfff);
		cnt_data++;
		if (cnt_data < (size_t)num)
		{
			buf[cnt_data] = (short)(((buf_read[l+1] & 0xf0) << 4) + buf_read[l+2]);
			if (buf[cnt_data] & 0x800)
				buf[cnt_data] |= ~(0xfff);
			cnt_data++;
		}
	}

	free(buf_read);

	return cnt_data;
} /* read_raw_12bit() */


size_t
read_raw_16bit(FILE *fp, size_t start, size_t num, short *buf, int offset, int big_endian)
{
	size_t n = 0, l;
	unsigned short *read_buf;

	if (fseek(fp, (long)start*2, SEEK_SET) < 0)
		return 0;

	read_buf = malloc(num*2);
	n = fread(read_buf, 2, num, fp);

	if (big_endian)
	{
		/* format 61 */
		for (l = 0; l < n; l++)
		{
			buf[l] = read_buf[l];
			be16_to_cpu(buf[l]);
		}
	}
	else
	{
		/* format 16 or 160 */
		for (l = 0; l < n; l++)
		{
			le16_to_cpu(read_buf[l]);
			buf[l] = (short)(read_buf[l] - offset);
		}
	}

	free(read_buf);

	return n;
} /* read_raw_16bit() */


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
	ra_value_set_string(desc, "Evaluation stored in MIT/BIH files");

	sprintf(t, "%02d.%02d.%d %02d:%02d:%02d", i->ri.base_date.day,
		i->ri.base_date.month, i->ri.base_date.year, i->ri.base_time.hour,
		i->ri.base_time.min, i->ri.base_time.sec);

	ra_value_set_string(add_ts, t);
	ra_value_set_string(modify_ts, t);

	ra_value_reset(user);
	ra_value_reset(host);
	ra_value_set_string(prog, "");

	return 0;
} /* pl_get_eval_info() */


long
pl_get_class_num(meas_handle mh)
{
	int num = 0;
	struct info *i = ((struct ra_meas *) mh)->priv;

	if (i->num_ecg_use > 0)
		num += 1;  /* heart-beats */
	if (i->num_text_annot > 0)
		num += 1;  /* annotations */

	return num;
} /* pl_get_class_num() */


int
pl_get_class_info(meas_handle mh, unsigned long class_num, value_handle id, value_handle name, value_handle desc)
{
	int ret;
	struct info *i = ((struct ra_meas *) mh)->priv;
	unsigned long n;

	n = (unsigned long)pl_get_class_num(mh);
	if (class_num > n)
	{
		_ra_set_error(mh, RA_ERR_OUT_OF_RANGE, "plugin-mit/bih");
		return -1;
	}

	/* first class are the heartbeats */
	if ((class_num == 0) && (i->num_ecg_use > 0))
	{
		ra_value_set_string(id, "heartbeat");
		ret = fill_predef_class_info_ascii("heartbeat", name, desc);
	}
	else
	{
		/* and the second (or when no heartbeats available, the first) are the annoations */
		ra_value_set_string(id, "annotation");
		ret = fill_predef_class_info_ascii("annotation", name, desc);
	}

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

	if (c->id == EVENT_CLASS_ANNOT)
		return 1;  /* text of the annotation */
	
	_ra_set_error(clh, RA_ERR_OUT_OF_RANGE, "plugin-mit/bih");

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

	if ((c->id == EVENT_CLASS_ANNOT) && (prop_num == 0))
	{
		ra_value_set_string(id, "annotation");
		ret = fill_predef_prop_info_ascii("annotation", type, len, name, desc, unit,
						  use_minmax, min, max, has_ign_value, ign_value);
	}
	else if (c->id == EVENT_CLASS_HEARTBEAT)
	{
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
			_ra_set_error(clh, RA_ERR_OUT_OF_RANGE, "plugin-mit/bih");
			ret = -1;
			break;
		}
	}
	else
		_ra_set_error(clh, RA_ERR_OUT_OF_RANGE, "plugin-mit/bih");

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

	if (c->id == EVENT_CLASS_HEARTBEAT)
	{
		start_su = (long *)malloc(sizeof(long) * i->num_ecg_use);
		end_su = (long *)malloc(sizeof(long) * i->num_ecg_use);
		
		for (l = 0; l < i->num_ecg_use; l++)
		{
			start_su[l] = (long)i->ecg[l].pos;
			end_su[l] = start_su[l];
		}
		ra_value_set_long_array(start, start_su, i->num_ecg_use);
		ra_value_set_long_array(end, end_su, i->num_ecg_use);
		
		free(start_su);
		free(end_su);
	}
	else if (c->id == EVENT_CLASS_ANNOT)
	{
		start_su = (long *)malloc(sizeof(long) * i->num_text_annot);
		end_su = (long *)malloc(sizeof(long) * i->num_text_annot);

		for (l = 0; l < i->num_text_annot; l++)
		{
			start_su[l] = (long)i->text_annot[l].pos;
			end_su[l] = start_su[l];
		}
		ra_value_set_long_array(start, start_su, i->num_text_annot);
		ra_value_set_long_array(end, end_su, i->num_text_annot);
		
		free(start_su);
		free(end_su);
	}
	else
	{
		_ra_set_error(clh, RA_ERR_OUT_OF_RANGE, "plugin-mit/bih");
		ret = -1;
	}

	return ret;
} /* pl_get_ev_start_end() */


int
pl_get_ev_value(prop_handle ph, unsigned long class_num, unsigned long prop_num, long event, value_handle value, value_handle ch)
{
	int ret = 0;
	value_handle *vh;
	long n, ch_num[1];
	struct eval_property *p = (struct eval_property *)ph;
	struct info *i = ((struct ra_meas *)p->meas)->priv;
	struct eval_class *c = (struct eval_class *)p->evclass;

	if (class_num || prop_num)
		;

	n = 0;
	switch(c->id)
	{
	case EVENT_CLASS_HEARTBEAT:
		n = i->num_ecg_use;
		break;
	case EVENT_CLASS_ANNOT:
		n = i->num_text_annot;
		break;
	}

	if (event >= n)
	{
		_ra_set_error(i->ra, RA_ERR_OUT_OF_RANGE, "plugin-mit/bih");
		return -1;
	}

	vh = (value_handle *)malloc(sizeof(value_handle));
	vh[0] = ra_value_malloc();
	ch_num[0] = -1;
	switch (p->id)
	{
	case EVENT_PROP_QRS_POS:
		ra_value_set_long(vh[0], i->ecg[event].pos);
		break;
	case EVENT_PROP_QRS_ANNOT:
		ra_value_set_long(vh[0], i->ecg[event].annot_ra);
		break;
	case EVENT_PROP_ANNOT:
		ra_value_set_string(vh[0], i->text_annot[event].text);
		ch_num[0] = i->text_annot[event].annot_ch;
		break;
	default:
		_ra_set_error(i->ra, RA_ERR_OUT_OF_RANGE, "plugin-mit/bih");
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

	if (class_num || prop_num)
		;

	switch(c->id)
	{
	case EVENT_CLASS_HEARTBEAT:
		ret = get_all_beat_events(p, i, value, ch);
		break;
	case EVENT_CLASS_ANNOT:
		ret = get_all_annot_events(p, i, value, ch);
		break;
	}

	return ret;
} /* pl_get_ev_value_all() */


int
get_all_beat_events(struct eval_property *p, struct info *i, value_handle value, value_handle ch)
{
	int ret = 0;	
	long *v, *ch_num, l;

	v = malloc(sizeof(long) * i->num_ecg_use);
	ch_num = malloc(sizeof(long) * i->num_ecg_use);

	if (p->id == EVENT_PROP_QRS_POS)
	{
		for (l = 0; l < i->num_ecg_use; l++)
		{
			v[l] = i->ecg[l].pos;
			ch_num[l] = -1;
		}
	}
	else if (p->id == EVENT_PROP_QRS_ANNOT)
	{
		for (l = 0; l < i->num_ecg_use; l++)
		{
			v[l] = i->ecg[l].annot_ra;
			ch_num[l] = -1;
		}
	}
	else
	{
		_ra_set_error(i->ra, RA_ERR_OUT_OF_RANGE, "plugin-mit/bih");
		ret = -1;
	}

	ra_value_set_long_array(value, v, i->num_ecg_use);
	ra_value_set_long_array(ch, ch_num, i->num_ecg_use);

	free(v);
	free(ch_num);

	return ret;
} /* get_all_beat_events() */


int
get_all_annot_events(struct eval_property *p, struct info *i, value_handle value, value_handle ch)
{
	char **v;
	long *ch_num, l;

	if (p->id != EVENT_PROP_ANNOT)
	{
		_ra_set_error(i->ra, RA_ERR_OUT_OF_RANGE, "plugin-mit/bih");
		return -1;
	}

	v = malloc(sizeof(char **) * i->num_text_annot);
	ch_num = malloc(sizeof(long) * i->num_text_annot);

	for (l = 0; l < i->num_text_annot; l++)
	{
		v[l] = malloc(sizeof(char) * (strlen(i->text_annot[l].text) + 1));
		strcpy(v[l], i->text_annot[l].text);
		ch_num[l] = i->text_annot[l].annot_ch;
	}

	ra_value_set_string_array(value, (const char **)v, i->num_text_annot);
	ra_value_set_long_array(ch, ch_num, i->num_text_annot);

	for (l = 0; l < i->num_text_annot; l++)
		free(v[l]);       
	free(v);
	free(ch_num);

	return 0;
} /* get_all_beat_events() */

