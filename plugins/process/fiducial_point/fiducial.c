/*----------------------------------------------------------------------------
 * fiducial.c  -  finds fiducial point of a QRS-complex
 *
 * Description:
 * The plugin finds the fiducial-points of the QRS-complexes in an ecg.
 *
 * Author(s): Raphael Schneider (librasch@gmail.com)
 *
 * Copyright (C) 2002-2009, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define _LIBRASCH_BUILD
#include <ra.h>
#include <ra_priv.h>
#include <pl_general.h>
#include "fiducial.h"


LIBRAAPI int
load_ra_plugin(struct plugin_struct *ps)
{
	strcpy(ps->info.name, PLUGIN_NAME);
	strcpy(ps->info.desc, PLUGIN_DESC);
	sprintf(ps->info.build_ts, "%s  %s", __DATE__, __TIME__);
	ps->info.type = PLUGIN_TYPE;
	ps->info.license = PLUGIN_LICENSE;
	sprintf(ps->info.version, PLUGIN_VERSION);

	/* set result infos */
	ps->info.num_results = num_results;
	ps->info.res = ra_alloc_mem(sizeof(results));
	memcpy(ps->info.res, results, sizeof(results));

	/* set option infos */
	ps->info.num_options = num_options;
	ps->info.opt = ra_alloc_mem(sizeof(options));
	memcpy(ps->info.opt, options, sizeof(options));

	ps->call_gui = NULL;
	ps->proc.get_proc_handle = pl_get_proc_handle;
	ps->proc.free_proc_handle = pl_free_proc_handle;
	ps->proc.do_processing = pl_do_processing;

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
	/* clean up */

	return 0;
} /* fini_plugin() */


int
pl_call_gui(proc_handle proc)
{
	if (proc)
		;

	return 0;
} /* pl_call_gui() */


proc_handle
pl_get_proc_handle(plugin_handle pl)
{
	struct plugin_struct *ps = (struct plugin_struct *)pl;

	struct proc_info *pi = (struct proc_info *)ra_alloc_mem(sizeof(struct proc_info));
	if (!pi)
	{
		_ra_set_error(pl, 0, "plugin-fiducial");
		return NULL;
	}

	memset(pi, 0, sizeof(struct proc_info));
	pi->handle_id = RA_HANDLE_PROC;
	pi->plugin = pl;

	/* init options */
	pi->options = calloc(1, sizeof(struct ra_fiducial_point));
	set_option_offsets(ps->info.opt, pi->options);
	set_default_options(pi->options);

	return pi;
} /* pl_get_proc_handle() */


void
set_default_options(struct ra_fiducial_point *opt)
{
	opt->use_class = 1;
	opt->clh = NULL;
	opt->pos_prop = NULL;
	opt->save_in_class = 1;
	opt->num_data = 0;
	opt->data = NULL;
	opt->data_is_pos = 0;
	opt->ch = 0;
	opt->win_len = 0.2;
	opt->corr_len = 0.015;
} /* set_default_options() */


void
set_option_offsets(struct ra_option_infos *opt_inf, struct ra_fiducial_point *opt)
{
	unsigned char *p = (unsigned char *)opt;
	int idx = 0;

	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->use_class) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->clh) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->pos_prop) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->save_in_class) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->num_data) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->data) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->data_is_pos) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->ch) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->win_len) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->corr_len) - p);

	/* consistency check (useful when changing option structure) */
	if (idx != num_options)
	{
		fprintf(stderr, "critical error in fiducial.c - set_option_offsets():\n  number of options(%ld) != idx(%d)\n", num_options, idx);
		exit(-1);
	}
} /* set_option_offsets() */



void
pl_free_proc_handle(proc_handle proc)
{
	struct proc_info *pi = proc;
	
	if (pi->options)
	{
		struct ra_fiducial_point *opt;

		opt = (struct ra_fiducial_point *)pi->options;
		if (opt->data)
			ra_free_mem(opt->data);
		free(opt);
	}
	       
	if (pi->results)
	{
		int i;
		value_handle *res;

		res = pi->results;
		for (i = 0; i < num_results; i++)
			ra_value_free(res[i]);
		free(res);
	}

	ra_free_mem(proc);
} /* pl_free_proc_handle() */


int
pl_do_processing(proc_handle proc)
{
	int ret = 0;
	struct proc_info *pi = proc;
	struct ra_fiducial_point *opt;
	value_handle *res;
	struct info i;
	long l;

	if (!pi)
		return -1;

	opt = (struct ra_fiducial_point *)pi->options;

	pi->num_result_sets = 1;
	pi->num_results = num_results;

	/* init results */
	/* first check if there are already some results are stored (maybe from a
	   run before with the same proc-handle) */
	if (pi->results)
	{
		int i;
		value_handle *t;

		t = pi->results;
		for (i = 0; i < num_results; i++)
			ra_value_free(t[i]);
		free(t);
		pi->results = NULL;
	}
	res = malloc(sizeof(value_handle *) * (pi->num_results * pi->num_result_sets));
	for (l = 0; l < (pi->num_results * pi->num_result_sets); l++)
	{
		int curr_idx = l % pi->num_results;

		value_handle vh = ra_value_malloc();
		set_meta_info(vh, results[curr_idx].name, results[curr_idx].desc, RA_INFO_NONE);
		res[l] = vh;
	}
	pi->results = res;

	if (opt->use_class && (opt->clh == NULL))
	{
		_ra_set_error(pi, RA_ERR_INFO_MISSING, "plugin-fiducial");
		return -1;
	}

	memset(&i, 0, sizeof(struct info));

	if ((ret = get_data(pi, &i)) != 0)
		goto error;
	
	if ((ret = find_fiducial(pi, i.num, i.pos)) != 0)
		goto error;

	ra_value_set_long_array(res[POS], i.pos, i.num);

	if ((ret == 0) && opt->save_in_class && (opt->use_class || !opt->data_is_pos))
		ret = save_results(pi, &i);

 error:
	if (i.pos)
		free(i.pos);
	if (i.index)
		free(i.index);

	return ret;
} /* pl_do_processing() */


int
get_data(struct proc_info *pi, struct info *i)
{
	int ret;
	struct ra_fiducial_point *opt = (struct ra_fiducial_point *)pi->options;
	value_handle vh;

	vh = ra_value_malloc();
	ra_value_set_number(vh, opt->ch);
	ra_info_get(pi->mh, RA_INFO_CH_XSCALE_D, vh);
	i->x_scale = ra_value_get_double(vh);
	ra_value_free(vh);

	if ((opt->clh != NULL) && (opt->pos_prop != NULL) && (opt->pos_prop[0] != '\0'))
	{
		i->ph_pos = ra_prop_get(opt->clh, opt->pos_prop);
		if (i->ph_pos == NULL)
		{
			_ra_set_error(pi, RA_ERR_WRONG_EV_PROP, "plugin-fiducial");
			return -1;
		}
	}

	if (opt->use_class)
		ret = get_data_from_eval(pi, i);
	else
		ret = get_data_from_opt(pi, i);

	return ret;
} /* get_data() */


int
get_data_from_eval(struct proc_info *pi, struct info *i)
{
	int ret;
	struct ra_fiducial_point *opt = (struct ra_fiducial_point *)pi->options;

	if ((opt->pos_prop != NULL) && (opt->pos_prop[0] != '\0'))
		ret = get_data_from_ev_prop(i);
	else
		ret = get_data_from_ev_class(pi, i);

	return ret;
} /* get_data_from_eval() */


int
get_data_from_ev_prop(struct info *i)
{
	int ret = 0;
	value_handle vh_pos = NULL;
	value_handle vh_id = NULL;
	const long *id_arr;
	long l, val, n, cnt = 0;
	class_handle clh;
	
	vh_id = ra_value_malloc();
	vh_pos = ra_value_malloc();

	clh = ra_class_get_handle(i->ph_pos);
	ra_class_get_events(clh, -1, -1, 0, 1, vh_id);
	id_arr = ra_value_get_long_array(vh_id);
		
	n = ra_value_get_num_elem(vh_id);
	i->pos = (long *)malloc(sizeof(long) * n);
	i->index = (long *)malloc(sizeof(long) * n);

	for (l = 0; l < n; l++)
	{
		ra_prop_get_value(i->ph_pos, id_arr[l], -1, vh_pos);
		
		val = ra_value_get_long(vh_pos);
		i->pos[cnt] = (long)((double)val * i->x_scale);
		i->index[cnt] = id_arr[l];
		cnt++;
	}
	i->num = cnt;
	
	if (i->num < n)
	{
		i->pos = realloc(i->pos, sizeof(long) * i->num);
		i->index = realloc(i->index, sizeof(long) * i->num);
	}

	ra_value_free(vh_pos);
	ra_value_free(vh_id);

	return ret;
} /* get_data_from_ev_prop() */


int
get_data_from_ev_class(struct proc_info *pi, struct info *i)
{
	int ret;
	struct ra_fiducial_point *opt = (struct ra_fiducial_point *)pi->options;
	value_handle vh;
	const long *id_arr;
	long l, start, end;

	vh = ra_value_malloc();
	if ((ret = ra_class_get_events(opt->clh, -1, -1, 0, 1, vh)) != 0)
	{
		ra_value_free(vh);
		return ret;
	}
		
	i->num = ra_value_get_num_elem(vh);
	if (i->num == 0)
	{
		ra_value_free(vh);
		_ra_set_error(pi, RA_WARN_NO_DATA, "plugin-fiducial");
		return -1;
	}
	
	i->pos = malloc(sizeof(long) * i->num);
	i->index = malloc(sizeof(long) * i->num);
	
	id_arr = ra_value_get_long_array(vh);	
	for (l = 0; l < i->num; l++)
	{			
		ra_class_get_event_pos(opt->clh, id_arr[l], &start, &end);
		if (start != end)
		{
			_ra_set_error(pi, RA_ERR_WRONG_EVENT_TYPE, "plugin-fiducial");
			ra_value_free(vh);
			return -1;
		}
		i->pos[l] = (long)((double)start * i->x_scale);
		i->index[l] = id_arr[l];
	}

	return 0;
} /* get_data_from_ev_class() */


int
get_data_from_opt(struct proc_info *pi, struct info *i)
{
	struct ra_fiducial_point *opt = (struct ra_fiducial_point *)pi->options;
	value_handle vh;
	long l, val, start, end;

	i->num = opt->num_data;
	if (i->num == 0)
	{
		_ra_set_error(pi, RA_WARN_NO_DATA, "plugin-fiducial");
		return -1;
	}
	
	i->pos = malloc(sizeof(long) * i->num);
	i->index = malloc(sizeof(long) * i->num);
	if (opt->data_is_pos)
	{
		for (l = 0; l < i->num; l++)
		{
			i->pos[l] = (long)((double)opt->data[l] * i->x_scale);
			i->index[l] = -1; /* set it to '-1' so it will be noticed (hopefully) when used */
		}
	}
	else
	{
		if (i->ph_pos)
		{
			vh = ra_value_malloc();
			for (l = 0; l < i->num; l++)
			{
				ra_prop_get_value(i->ph_pos, opt->data[l], -1, vh);
				val = ra_value_get_long(vh);				
				i->pos[l] = (long)((double)val * i->x_scale);
				i->index[l] = opt->data[l];
			}
			ra_value_free(vh);
		}
		else
		{
			for (l = 0; l < i->num; l++)
			{
				ra_class_get_event_pos(opt->clh, opt->data[l], &start, &end);
				i->pos[l] = (long)((double)start * i->x_scale);
				i->index[l] = opt->data[l];
			}
		}
	}

	return 0;
} /* get_data_from_opt() */


int
find_fiducial(struct proc_info *pi, long num, long *pos)
{
	struct ra_fiducial_point *opt;
	rec_handle rh;
	value_handle vh;
	double samplerate;
	long reg_len;		/* number of samples for regression line */
	long window_width;
	double *buf = NULL;
	long i, n;
	int percent, percent_save = -1;
	char s[100];
	double diff, max_diff;
	long l, max_pos;

	if (pi->callback)
	{
		sprintf(s, gettext("find fiducial points"));
		(*pi->callback)(s, 0);
	}

	opt = (struct ra_fiducial_point *)pi->options;

	rh = ra_rec_get_first(pi->mh, 0);

	vh = ra_value_malloc();
	ra_value_set_number(vh, opt->ch);
	if (ra_info_get(rh, RA_INFO_REC_CH_SAMPLERATE_D, vh) != 0)
	{
		_ra_set_error(rh, RA_ERR_INFO_MISSING, "plugin-detect-simple");
		ra_value_free(vh);
		return -1;
	}
	samplerate = ra_value_get_double(vh);
	ra_value_free(vh);

	reg_len = (int)(opt->corr_len * samplerate);
	window_width = (int)(opt->win_len * samplerate);
	buf = malloc(sizeof(double) * (2 * window_width + reg_len));

	for (l = 0; l < num; l++)
	{
		int minmax_pos;
		int positive;
		unsigned long start_pos;
		unsigned long start_offset;
/* 		double min, max; */

		percent = (int) (((double) l / (double) num) * 100.0);
		if ((percent != percent_save) && pi->callback)
		{
			(*pi->callback)(NULL, percent);
			percent_save = percent;
		}

		/* check if beat is at the beginning and if so, shorten window
		   used for alignment (and calc offset) */
		if (pos[l] < window_width)
		{
			start_pos = 0;
			start_offset = window_width - pos[l];
		}
		else
		{
			start_pos = pos[l] - window_width;
			start_offset = 0;
		}

 		n = (long)ra_raw_get_unit(rh, opt->ch, start_pos, 2 * window_width + reg_len - start_offset, buf);
		ra_raw_process_unit(RA_RAW_PROC_RM_POWERLINE_NOISE, NULL, n, buf, rh, opt->ch);

 		positive = get_orientation(buf, n, &minmax_pos);

/* 		min = max = buf[0]; */
/* 		minmax_pos = 0; */
/* 		for (i = 0; i < n; i++) */
/* 		{ */
/* 			if (positive) */
/* 			{ */
/* 				if (buf[i] > max) */
/* 				{ */
/* 					max = buf[i]; */
/* 					max_pos = i; */
/* 				} */
/* 			} */
/* 			else */
/* 			{ */
/* 				if (buf[i] < min) */
/* 				{ */
/* 					min = buf[i]; */
/* 					max_pos = i; */
/* 				} */
/* 			} */
/* 		} */
/* 		pos[l] += (minmax_pos - window_width); */

 		max_diff = 0;
		max_pos = -reg_len / 2;
		for (i = 0; i < (n - reg_len); i++)
		{
			diff = buf[i+reg_len] - buf[i];
 			if (!positive)
 				diff = -diff;
			if (diff > max_diff)
			{
				max_diff = diff;
				max_pos = i - window_width + start_offset;
			}
		}
		pos[l] += (max_pos + reg_len/2);
	}
	if (pi->callback)
		(*pi->callback)(NULL, 100);

	if (buf)
		free(buf);

	return 0;
} /* find_fiducial() */


int
get_orientation(double *buf, int size, int *minmax_pos)
{
	double max, min;
	long max_pos, min_pos;

	int i;
	int ret;

	max = min = 0;
	max_pos = min_pos = 0;
	for (i = 0; i < size; i++)
	{
		if (buf[i] > max)
		{
			max = buf[i];
			max_pos = i;
		}
		if (buf[i] < min)
		{
			min = buf[i];
			min_pos = i;
		}
	}
	*minmax_pos = min_pos;
	ret = 1;  /* 1: positive orientation */
	/* negative peak must be really larger than positive peak
	   (to avoid "jitter" when complex is bi-phasic) */
	if ((1.5 * max) < fabs(min))
	{
		ret = 0;
		*minmax_pos = max_pos;
	}

	return ret;
} /* get_orientation() */


int
save_results(struct proc_info *pi, struct info *i)
{
	struct ra_fiducial_point *opt = (struct ra_fiducial_point *)pi->options;
	long l;
	value_handle vh;

	if (opt == NULL)
		return -1;

	vh = ra_value_malloc();

	if (i->ph_pos != NULL)
	{
		for (l = 0; l < i->num; l++)
		{
			ra_value_set_long(vh, i->pos[l]);
			ra_prop_set_value(i->ph_pos, i->index[l], -1, vh);
		}
	}
/* 	else */
/* 	{ */
/* 		for (l = 0; l < i->num; l++) */
/* 		{ */
/* 			if ((ret = ra_class_set_event_pos(opt->clh, i->index[l], i->pos[l], i->pos[l])) != 0) */
/* 			{ */
/* 				ra_eval_edit_cancel(opt->clh); */
/* 				return ret; */
/* 			} */
/* 		} */
/* 	} */

	ra_value_free(vh);

	return 0;
} /* save_results() */
