/*----------------------------------------------------------------------------
 * freq_analysis.c  -  perform frequency analysis
 *
 * Description:
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2003-2005, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#define _LIBRASCH_BUILD
#include <ra.h>
#include <ra_priv.h>
#include <pl_general.h>
#include <ra_eval.h>
#include <ra_ecg.h>
#include "freq_analysis.h"
#include "frequency_fftw3.h"


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

	ps->call_gui = NULL;	/* will be set in init_ra_plugin(), eventually */
	ps->proc.get_proc_handle = pl_get_proc_handle;
	ps->proc.free_proc_handle = pl_free_proc_handle;
	ps->proc.do_processing = pl_do_processing;

	return 0;
} /* init_plugin() */


LIBRAAPI int
init_ra_plugin(struct librasch *ra, struct plugin_struct *ps)
{
	/* check if gui is available */
	if (ra_plugin_get_by_name(ra, "gui-freq-analysis", 1))
		ps->call_gui = pl_call_gui;

	return 0;
} /* init_ra_plugin() */


LIBRAAPI int
fini_ra_plugin(void)
{
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
		return NULL;

	memset(pi, 0, sizeof(struct proc_info));
	pi->handle_id = RA_HANDLE_PROC;
	pi->plugin = pl;

	/* init options */
	pi->options = calloc(1, sizeof(struct ra_freq_analysis));
	set_option_offsets(ps->info.opt, pi->options);
	set_default_options(pi->options);

	return pi;
} /* pl_get_proc_handle() */


void
set_default_options(struct ra_freq_analysis *opt)
{
	memset(opt, 0, sizeof(struct ra_freq_analysis));

	opt->window = ra_alloc_mem(strlen("hanning") + 1);
	strcpy(opt->window, "hanning");
	opt->remove_mean = 1;
	
	/* opt->method not used at the moment */

	opt->save_in_eval = 0;
} /* set_default_options() */


void
set_option_offsets(struct ra_option_infos *opt_inf, struct ra_freq_analysis *opt)
{
	unsigned char *p = (unsigned char *)opt;
	int idx = 0;

	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->use_events) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->clh) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->prop_value) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->use_event_pos) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->prop_pos) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->use_time) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->num_events) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->events) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->num_ignore_events) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->ignore_events) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->rh) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->ch) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->use_ignore_value) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->ignore_value) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->use_start_end_pos) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->start_pos) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->end_pos) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->num_values) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->values) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->pos) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->pos_samplerate) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->samplerate) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->smooth_data) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->smooth_method) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->smooth_width) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->window) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->remove_mean) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->remove_trend) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->method) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->num_freq) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->smooth_spec) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->save_in_eval) - p);

	/* consistency check (useful when changing option structure) */
	if (idx != num_options)
	{
		fprintf(stderr, "critical error in freq_analysis.c - set_option_offsets():\n  number of options(%ld) != idx(%d)\n", num_options, idx);
		exit(-1);
	}
} /* set_option_offsets() */


void
pl_free_proc_handle(proc_handle proc)
{
	struct proc_info *pi = proc;
	
	if (pi->options)
	{
		struct ra_freq_analysis *opt = (struct ra_freq_analysis *)pi->options;

		if (opt->prop_value)
			ra_free_mem(opt->prop_value);
		if (opt->prop_pos)
			ra_free_mem(opt->prop_pos);
		if (opt->events)
			ra_free_mem(opt->events);
		if (opt->ignore_events)
			ra_free_mem(opt->ignore_events);
		if (opt->values)
			ra_free_mem(opt->values);
		if (opt->pos)
			ra_free_mem(opt->pos);
		if (opt->smooth_method)
			ra_free_mem(opt->smooth_method);
		if (opt->window)
			ra_free_mem(opt->window);
		if (opt->method)
			ra_free_mem(opt->method);
		free(opt);
	}
	       
	if (pi->results)
	{
		int i;
		value_handle *t;

		t = pi->results;
		for (i = 0; i < (pi->num_results * pi->num_result_sets); i++)
			ra_value_free(t[i]);
		free(t);
	}

	ra_free_mem(proc);
} /* pl_free_proc_handle() */


int
pl_do_processing(proc_handle proc)
{
	int ret = 1;
	struct proc_info *pi = proc;
	double *values = NULL;
	double *pos = NULL;
	long l, num;
	value_handle *res;
	struct ra_freq_analysis *opt;
	eval_handle eh;

	if (!pi)
		return -1;

	opt = (struct ra_freq_analysis *)pi->options;

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

	/* if rec-handle was not set in options, use root recording */
	if (!opt->rh)
		opt->rh = ra_rec_get_first(pi->mh, 0);

	if (!opt->clh)
		eh = ra_eval_get_default(pi->mh);
	else
		eh = ra_eval_get_handle(opt->clh);

	if ((ret = get_data(pi->mh, opt, &values, &pos, &num)) != 0)
		goto clean_up;

	if ((num > 0) && opt->use_time)
	{
		if ((ret = interpolate_values(&values, &pos, &num, opt->samplerate)) != 0)
			goto clean_up;
	}
	if (num <= 0)
	{
		_ra_set_error(pi->ra, RA_WARN_NO_DATA, "plugin-freq-analysis");
		goto clean_up;
	}

	if ((ret = pre_process_data(opt, &values, &num)) != 0)
		goto clean_up;

	if ((ret = do_freq_analysis(opt, values, num, res)) != 0)
		goto clean_up;

	if ((ret = post_process_spec(opt, res)) != 0)
		goto clean_up;

	if (opt->save_in_eval)
		save(eh, res);

	ret = 0;

 clean_up:
	if (values)
		free(values);
	if (pos)
		free(pos);

	return ret;
} /* pl_do_processing() */


double
get_samplerate(any_handle h)
{
	value_handle vh;
	double samplerate = 0.0;

	meas_handle mh = ra_meas_handle_from_any_handle(h);

	vh = ra_value_malloc();
	ra_info_get(mh, RA_INFO_MAX_SAMPLERATE_D, vh);
	samplerate = ra_value_get_double(vh);
	ra_value_free(vh);

	return samplerate;
} /* get_samplerate() */


int
get_data(meas_handle mh, struct ra_freq_analysis *opt, double **values, double **pos, long *num)
{
	int ret = -1;
	long l;

	if (opt->num_values > 0)
	{
		if ((opt->values == NULL) || (opt->use_time && ((opt->pos == NULL) || (opt->pos_samplerate <= 0.0))))
		{
			_ra_set_error(mh, RA_ERR_INFO_MISSING, "plugin-freq-analysis");
			return ret;
		}

		*values = malloc(sizeof(double) * opt->num_values);
		memcpy(*values, opt->values, sizeof(double) * opt->num_values);
		*num = opt->num_values;
		if (opt->use_time)
		{
			*pos = malloc(sizeof(double) * opt->num_values);
			for (l = 0; l < opt->num_values; l++)
				(*pos)[l] = opt->pos[l] / opt->pos_samplerate;
		}

		ret = 0;
	}
	else if (opt->use_events)
		ret = get_event_data(opt, values, pos, num);
	else
		ret = get_raw_data(opt, values, pos, num);

	return ret;
} /* get_data() */


int
get_event_data(struct ra_freq_analysis *opt, double **values, double **pos, long *num)
{
	int ret = 0;
	long l, cnt, last_ok_idx;
	struct event_infos *ev = NULL;

	*values = NULL;
	*pos = NULL;
	*num = 0;

	/* check if we want a time-based series and if the needed pos event-prop was given */
	if (opt->use_time && !opt->prop_pos)
	{
		_ra_set_error(opt->clh, RA_ERR_EV_PROP_MISSING, "plugin-freq-analysis");
		return -1;
	}

	if ((ret = get_events(opt, &ev, num)) != 0)
		return ret;

	/* store values in array 'values' and interpolate (linear) skipped events
	   (when freq.analysis is based on event-number and not on time) */
	*values = malloc(sizeof(double) * (*num));
	if (opt->use_time)
		*pos = malloc(sizeof(double) * (*num));
	cnt = 0;
	last_ok_idx = -1;
	for (l = 0; l < (*num); l++)
	{
		if (ev[l].dont_use_event)
			continue;
		
		if (opt->use_time)
		{
			(*values)[cnt] = ev[l].value;
			(*pos)[cnt] = ev[l].pos;
		}
		else
		{
			if ((last_ok_idx < (l-1)) && (last_ok_idx != -1))
			{
				long m, n;
				double add;
				
				n = l - last_ok_idx;
				add = (ev[l].value - ev[last_ok_idx].value) / (double)n;
				
				for (m = 1; m < n; m++)
					(*values)[cnt++] = ev[last_ok_idx].value + (m * add);
			}
			
			last_ok_idx = l;
			(*values)[cnt] = ev[l].value;
			cnt++;
		}
	}
	*num = cnt;

	if (ev)
		free(ev);

	return ret;
} /* get_event_data() */


int
get_events(struct ra_freq_analysis *opt, struct event_infos **ev, long *num)
{
	int ret = -1;
	prop_handle evp, pos;
	long l, m;
	long *ev_list = NULL;
	value_handle vh;

	if (opt->clh == NULL)
		return -1;

	evp = pos = NULL;
	if (opt->prop_value)
		evp = ra_prop_get(opt->clh, opt->prop_value);
	if (opt->prop_pos)
		pos = ra_prop_get(opt->clh, opt->prop_pos);
	if (!evp || ((opt->use_time || opt->use_start_end_pos) && !pos))
	{
		_ra_set_error(opt->clh, RA_ERR_UNKNOWN_EV_PROP, "plugin-freq-analysis");
		return -1;
	}

	get_event_list(opt, &ev_list, num);
	if (*num <= 0)
		goto go_out;

	*ev = calloc(*num, sizeof(struct event_infos));
	if (*ev == NULL)
	{
		_ra_set_error(opt->clh, 0, "plugin-freq-analysis");
		goto go_out;
	}

	vh = ra_value_malloc();

	for (l = 0; l < *num; l++)
	{
		(*ev)[l].id = ev_list[l];
		ra_prop_get_value(evp, ev_list[l], opt->ch, vh);
		(*ev)[l].value = ra_value_get_double(vh);
	}
	if (pos)
	{
		/* event positions are stored in sample units and we want position-values in seconds */
		double samplerate = get_samplerate(opt->clh);
		for (l = 0; l < *num; l++)
		{
			ra_prop_get_value(pos, ev_list[l], opt->ch, vh);
			(*ev)[l].pos = ra_value_get_double(vh) / samplerate;
		}
	}

	ra_value_free(vh);

	/* mark events which are in ignore_events-array */
	for (l = 0; l < opt->num_ignore_events; l++)
	{
		for (m = 0; m < *num; m++)
		{
			if (opt->ignore_events[l] == (*ev)[m].id)
			{
				(*ev)[m].dont_use_event = 1;
				break;
			}
		}
	}

	ret = 0;
 go_out:
	if (ev_list)
		free(ev_list);

	return ret;
} /* get_events() */


int
get_event_list(struct ra_freq_analysis *opt, long **list, long *num)
{
	int ret = -1;
	long start_event, end_event;
	long l;
	prop_handle prop_pos;
	int check = 0;
	long last_event;
	value_handle vh = NULL;
	value_handle vh_pos = NULL;
	const long *ev_id;

	*num = 0;

	/* check if more than one selection-option was used */
	if (opt->use_start_end_pos)
		check++;
	if (opt->num_events > 0)
		check++;
	if (check > 1)
	{
		_ra_set_error(opt->clh, RA_ERR_ERROR, "plugin-freq-analysis");
		return -1;
	}

	vh = ra_value_malloc();
	ra_class_get_events(opt->clh, -1, -1, 0, 1, vh);
	ev_id = ra_value_get_long_array(vh);

	last_event = ra_value_get_num_elem(vh);
	if (last_event < 0)
		goto clean_up;

	if (opt->num_events > 0)
	{
		*list = malloc(sizeof(long) * opt->num_events);
		memcpy(*list, opt->events, sizeof(long) * opt->num_events);
		*num = opt->num_events;

		ret = 0;
		goto clean_up;
	}

	/* select events to use */
	vh_pos = ra_value_malloc();
	start_event = end_event = -1;
	if (opt->use_start_end_pos)
	{
		prop_pos = ra_prop_get(opt->clh, opt->prop_pos);
		if (prop_pos == NULL)
		{
			_ra_set_error(opt->clh, RA_ERR_UNKNOWN_EV_PROP, "plugin-freq-analysis");
			goto clean_up;
		}

		for (l = 0; l < *num; l++)
		{
			long p;

			ra_prop_get_value(prop_pos, ev_id[l], opt->ch, vh_pos);
			p = ra_value_get_long(vh_pos);
			
			if ((start_event != -1) && (p > opt->start_pos))
				start_event = l;
			if ((end_event != -1) && (p > opt->end_pos))
				end_event = l;

			if ((start_event != -1) && (end_event != -1))
				break;
		}
	}
	
	if (start_event == -1)
		start_event = 0;
	if ((end_event == -1) || (end_event > last_event))
		end_event = last_event;

	*num = end_event - start_event;
	*list = malloc(sizeof(long) * (*num));
	for (l = start_event; l < end_event; l++)
		(*list)[l-start_event] = ev_id[l];

	ret = 0;

 clean_up:
	if (vh)
		ra_value_free(vh);
	if (vh_pos)
		ra_value_free(vh_pos);
	
	return ret;
} /* get_event_list() */


int
get_raw_data(struct ra_freq_analysis *opt, double **values, double **pos, long *num)
{
	long start_pos, end_pos;
	value_handle vh;
	long l;
	double samplerate, pos_value, pos_add;

	if (!opt->rh || (opt->ch < 0))
		return -1;		

	vh = ra_value_malloc();

	/* get number of values */
	if (opt->use_start_end_pos)
	{
		start_pos = opt->start_pos;
		end_pos = opt->end_pos;
	}
	else
	{
		start_pos = 0;
		ra_value_set_number(vh, opt->ch);
		if (ra_info_get(opt->rh, RA_INFO_REC_CH_NUM_SAMPLES_L, vh) != 0)
		{
			ra_value_free(vh);
			return -1;
		}
		end_pos = ra_value_get_long(vh);
	}
	*num = end_pos - start_pos;

	/* get_samplerate */
	ra_value_set_number(vh, opt->ch);
	ra_info_get(opt->rh, RA_INFO_REC_CH_SAMPLERATE_D, vh);
	samplerate = ra_value_get_double(vh);
	pos_add = 1.0 / samplerate;

	/* get values */
	*values = malloc(sizeof(double) * (*num));
	*pos = malloc(sizeof(double) * (*num));
	ra_raw_get_unit(opt->rh, opt->ch, start_pos, *num, *values);
	/* if ignore-value than interpolate (linear) these values */
	if (opt->use_ignore_value)
	{
		long cnt, last_ok_idx;

		cnt = 0;
		last_ok_idx = -1;
		for (l = 0; l < (*num); l++)
		{
			if ((*values)[l] == opt->ignore_value)
				continue;
			
			if ((last_ok_idx < (l-1)) && (last_ok_idx != -1))
			{
				long m, n;
				double add;

				n = l - last_ok_idx;
				add = ((*values)[l] - (*values)[last_ok_idx]) / (double)n;

				for (m = 1; m < n; m++)
					(*values)[cnt++] = (*values)[last_ok_idx] + (m * add);
			}

			last_ok_idx = l;
			cnt++;
		}
		*num = cnt;
	}

	/* set position values */
	pos_value = 0.0;
	for (l = 0; l < (*num); l++)
	{
		(*pos)[l] = pos_value;
		pos_value += pos_add;
	}

	ra_value_free(vh);

	return 0;
} /* get_raw_data() */


int
interpolate_values(double **value, double **pos, long *num, double samplerate)
{
	long l, num_i, idx_after;
	double *val_i, *pos_i;
	double range, pos_add, pos_before, pos_after, val_before, val_after, curr_pos;

	if ((samplerate <= 0.0) || (*num < 2))
		return -1;

	range = (*pos)[(*num)-1] - (*pos)[0]; /* range is in seconds */	
	num_i = (long)(range * samplerate);
	val_i = malloc(sizeof(double) * num_i);
	pos_i = malloc(sizeof(double) * num_i);

	pos_add = 1 / samplerate;
	idx_after = 1;
	pos_before = (*pos)[0];
	pos_after = (*pos)[1];
	val_before = (*value)[0];
	val_after = (*value)[1];
	curr_pos = pos_before;
	for (l = 0; l < num_i; l++)
	{
		double dist, slope;

		while (curr_pos > pos_after)
		{
			idx_after++;
			if (idx_after >= (*num))
				break;

			pos_before = (*pos)[idx_after-1];
			pos_after = (*pos)[idx_after];
			val_before = (*value)[idx_after-1];
			val_after = (*value)[idx_after];
		}
		if (idx_after >= (*num))
			break;

		dist = curr_pos - pos_before;
		slope = (val_after - val_before) / (pos_after - pos_before);
		val_i[l] = val_before + slope * dist;
		pos_i[l] = curr_pos;
		curr_pos += pos_add;
	}

	*num = l;
	free(*value);
	free(*pos);
	*value = val_i;
	*pos = pos_i;

	return 0;
} /* interpolate_values() */


int
pre_process_data(struct ra_freq_analysis *opt, double **values, long *num)
{
	double *t;
	long l, w;

	if (!opt->smooth_data)
		return 0;

	/* because at the moment only a boxcar-filter is available, we do not need to check method setting */

	t = malloc(sizeof(double) * (*num));

	w = (long)opt->smooth_width;
	for (l = 0; l < (w-1); l++)
		t[l] = (*values)[l];
	for (l = (w - 1); l < *num; l++)
	{
		long m;
		double s = 0.0;
		for (m = 0; m < w; m++)
			s += (*values)[l-m];
		t[l] = s / opt->smooth_width;
	}

	free(*values);
	*values = t;

	return 0;
} /* pre_process_data() */


int
do_freq_analysis(struct ra_freq_analysis *opt, double *values, long num, value_handle *res)
{
	int ret = -1;
	long l, n_spec;
	double *spec = NULL;
	double freq_step, curr_freq;

	n_spec = opt->num_freq;
	if (calc_periodogram(values, num, opt->samplerate, &spec, &n_spec, opt->remove_mean, opt->window) != 0)
		goto clean_up;
	ra_value_set_double_array(res[PSD], spec, n_spec);

	/* calc frequency value for every spectrum value (and store it in 'spec' because 'spec'
	   is no longer needed and I don't need to allocate another array) */
	curr_freq = 0;
	freq_step = opt->samplerate / (double)(n_spec * 2);
	for (l = 0; l < n_spec; l++)
	{
		spec[l] = curr_freq;
		curr_freq += freq_step;
	}
	ra_value_set_double_array(res[FREQ_AXIS], spec, n_spec);

	ret = 0;

 clean_up:
	if (spec)
		free(spec);

	return ret;
} /* do_freq_analysis() */


int
post_process_spec(struct ra_freq_analysis *opt, value_handle *res)
{
	int ret;
	double *smoothed = NULL;
	long n_sm = 0;

	if (!opt->smooth_spec)
		return 0;

 	ret = calc_log_freq_smoothed_spec(ra_value_get_double_array(res[PSD]), ra_value_get_num_elem(res[PSD]),
					  opt->samplerate, &smoothed, &n_sm);
	if (ret == 0)
		ra_value_set_double_array(res[PSD], smoothed, n_sm);

	free(smoothed);

	return ret;
} /* post_process_spec() */


/* TODO: extend eval-structure to handle calculation-results */
int
save(proc_handle proc, eval_handle eh)
{
	if (proc || eh)
		;

	return 0;
} /* save() */

