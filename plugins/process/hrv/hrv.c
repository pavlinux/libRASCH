/*----------------------------------------------------------------------------
 * hrv.c  -  calculate heart rate variability (HRV) parameters
 *
 * Description:
 * The plugin calculates the heart rate variability (HRV) parameters. In the time
 * domain the parameters, which are recommended from the Task Force for HRV, are
 * calculated. The parameters from the frequency domain are calculated on the
 * power-spectrum of the complete tachogram using FFT.
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002-2005, Raphael Schneider
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
#include "hrv.h"
#include "statistics.h"


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
	if (ra_plugin_get_by_name(ra, "gui-hrv", 1))
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
	struct proc_info *pi;
	struct plugin_struct *ps = (struct plugin_struct *)pl;

	pi = (struct proc_info *)ra_alloc_mem(sizeof(struct proc_info));
	if (!pi)
		return NULL;

	memset(pi, 0, sizeof(struct proc_info));
	pi->handle_id = RA_HANDLE_PROC;
	pi->plugin = pl;

	/* init options */
	pi->options = calloc(1, sizeof(struct ra_hrv));
	set_option_offsets(ps->info.opt, pi->options);
	set_default_options(pi->options);

	return pi;
} /* pl_get_proc_handle() */


void
set_default_options(struct ra_hrv *opt)
{
	opt->eh = NULL;
	opt->use_class = 1;
	opt->clh = NULL;
	opt->ch = -1;
	opt->prop_value = NULL;
	opt->use_event_pos = 0;
	opt->prop_pos = NULL;
	opt->prop_annot = NULL;
	opt->prop_temporal = NULL;
	opt->ignore_annot = 0;
	
	opt->use_start_end_pos = 0;
	opt->start_pos = -1;
	opt->end_pos = -1;

	opt->num_events = 0;
	opt->events = NULL;

	opt->num_ignore_events = 0;
	opt->ignore_events = NULL;

	opt->num_values = 0;
	opt->values = NULL;
	opt->pos = NULL;
	opt->samplerate = 1;

	opt->use_min_max = 1;
	opt->min_value = MIN_RRI;
	opt->max_value = MAX_RRI;

	opt->use_user_freq_band = 1;
	opt->user_freq_start = 0.0;
	opt->user_freq_end = 0.0;

	opt->poincare_lag = 1;

	opt->dfa_use_only_nn = 0;
	opt->dfa_segment_size = DFA_DEFAULT_SEGMENT;
	opt->dfa_alpha1_alpha2_cut = DFA_ALPHA1_ALPHA2_CUT;

	opt->dfa_use_user_range = 0;

	opt->skip_time_domain = 0;
	opt->skip_freq_domain = 0;
	opt->skip_nld_analysis = 0;

	opt->save_in_eval = 1;
} /* set_default_options() */


void
set_option_offsets(struct ra_option_infos *opt_inf, struct ra_hrv *opt)
{
	unsigned char *p = (unsigned char *)opt;
	int idx = 0;

	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->eh) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->use_class) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->clh) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->ch) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->prop_value) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->use_event_pos) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->prop_pos) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->prop_annot) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->prop_temporal) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->ignore_annot) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->use_start_end_pos) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->start_pos) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->end_pos) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->num_events) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->events) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->num_ignore_events) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->ignore_events) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->num_values) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->values) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->pos) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->samplerate) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->use_min_max) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->min_value) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->max_value) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->use_user_freq_band) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->user_freq_start) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->user_freq_end) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->poincare_lag) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->dfa_use_only_nn) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->dfa_segment_size) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->dfa_alpha1_alpha2_cut) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->dfa_use_user_range) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->dfa_user_lower_limit) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->dfa_user_upper_limit) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->skip_time_domain) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->skip_freq_domain) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->skip_nld_analysis) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->save_in_eval) - p);

	/* consistency check (useful when changing option structure) */
	if (idx != num_options)
	{
		fprintf(stderr, "critical error in hrv.c - set_option_offsets():\n  number of options(%ld) != idx(%d)\n", num_options, idx);
		exit(-1);
	}
} /* set_option_offsets() */


void
pl_free_proc_handle(proc_handle proc)
{
	struct proc_info *pi = proc;
	
	if (pi->options)
	{
		struct ra_hrv *opt;

		opt = (struct ra_hrv *)pi->options;

		if (opt->prop_value)
			ra_free_mem(opt->prop_value);
		if (opt->prop_pos)
			ra_free_mem(opt->prop_pos);
		if (opt->prop_annot)
			ra_free_mem(opt->prop_annot);
		if (opt->prop_temporal)
			ra_free_mem(opt->prop_temporal);

		if (opt->events)
			ra_free_mem(opt->events);
		if (opt->ignore_events)
			ra_free_mem(opt->ignore_events);

		if (opt->values)
			ra_free_mem(opt->values);
		if (opt->pos)
			ra_free_mem(opt->pos);

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
	struct beats *b = NULL;
	struct beats *b_no_art = NULL;
	long l, m, num, n_runs, num_no_art;
	value_handle *res;
 	double *tacho_idx = NULL;
	struct ra_hrv *opt;

	if (!pi)
		return -1;

	opt = pi->options;

	if (opt->eh == NULL)
		opt->eh = ra_eval_get_default(pi->mh);
	if (opt->eh == NULL)
	{
		_ra_set_error(proc, RA_WARN_NO_DEFAULT_EVAL, "plugin-hrv");
		return -1;
	}

	n_runs = get_number_runs(opt);
	pi->num_result_sets = n_runs;
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

	for (l = 0; l < n_runs; l++)
	{
		if ((ret = get_values(opt, l, &b, &num)) != 0)
			goto clean_up;

		num_no_art = 0;
		if (opt->use_class)
		{
			if ((ret = filter_values(opt, &b, &num, &b_no_art, &num_no_art)) != 0)
				goto clean_up;
		}

		if (num <= 0)
		{
			_ra_set_error(pi->ra, RA_WARN_NO_DATA, "plugin-hrv");
			goto clean_up;
		}
		
		/* save tacho in results */
		tacho_idx = malloc(sizeof(double) * num);
		for (m = 0; m < num; m++)
			tacho_idx[m] = b[m].ev_id;
		ra_value_set_double_array(res[TACHO_INDEX + (pi->num_results * l)], tacho_idx, num);
		
		if (!opt->skip_time_domain)
		{
			if ((ret = calc_hrv_timedomain(b, num, res, l)) != 0)
				goto clean_up;
		}
		
		if (!opt->skip_freq_domain)
		{
			if ((ret = calc_hrv_freqdomain(pi, b, num, "hanning", res, l)) != 0)
				goto clean_up;
		}
		
		if (!opt->skip_nld_analysis)
		{
			if ((ret = calc_nld(opt, b, num, b_no_art, num_no_art, res)) != 0)
				goto clean_up;
		}

		if (opt->save_in_eval)
			save(opt->eh, res);
		
		if (tacho_idx)
		{
			free(tacho_idx);
			tacho_idx = NULL;
		}
		if (b)
		{
			free(b);
			b = NULL;
		}
		if (b_no_art)
		{
			free(b_no_art);
			b_no_art = NULL;
		}
	}

	ret = 0;

 clean_up:
 	if (tacho_idx)
 		free(tacho_idx);
	if (b)
		free(b);

	return ret;
} /* pl_do_processing() */


int
get_number_runs(struct ra_hrv *opt)
{
	value_handle vh;
	int n = 0;

	if (!opt->use_class || opt->clh)
		return 1;

	/* all 'heartbeat' classes have to be processed; get number of classes */
	vh = ra_value_malloc();
	ra_class_get(opt->eh, "heartbeat", vh);
	n = ra_value_get_num_elem(vh);
	ra_value_free(vh);

	return n;
} /* get_number_runs() */


double
get_samplerate(eval_handle eh)
{
	value_handle vh;
	double samplerate = 0.0;

	meas_handle mh = ra_meas_handle_from_any_handle(eh);

	vh = ra_value_malloc();
	ra_info_get(mh, RA_INFO_MAX_SAMPLERATE_D, vh);
	samplerate = ra_value_get_double(vh);
	ra_value_free(vh);

	return samplerate;
} /* get_samplerate() */


int
get_values(struct ra_hrv *opt, long run, struct beats **b, long *num)
{
	int ret = -1;
	class_handle clh_use;
	prop_handle prop_value, prop_pos, prop_annot, prop_temporal;
	value_handle vh, vh_rri, vh_pos, vh_annot, vh_temp;
	long l;
	long *ev_list = NULL;
	short *ok = NULL;
	const double *rri;
	const long *pos, *annot;
	const short *temp;
	
	if (!opt->use_class)
		return get_given_values(opt, b, num);

	vh = vh_rri = vh_pos = vh_annot = vh_temp = NULL;
	pos = annot = NULL;
	temp = NULL;

	vh = ra_value_malloc();

	clh_use = opt->clh;
	if (clh_use == NULL)
	{
		ra_class_get(opt->eh, "heartbeat", vh);
		if (run >= (long)ra_value_get_num_elem(vh))
			goto error;

		clh_use = (class_handle)(ra_value_get_voidp_array(vh)[run]);
	}
	if (clh_use == NULL)
		goto error;

	if (opt->prop_value)
		prop_value = ra_prop_get(clh_use, opt->prop_value);
	else
		prop_value = ra_prop_get(clh_use, "rri");

	if (opt->prop_pos)
		prop_pos = ra_prop_get(clh_use, opt->prop_pos);
	else
		prop_pos = ra_prop_get(clh_use, "qrs-pos");

	if (opt->prop_annot)
		prop_annot = ra_prop_get(clh_use, opt->prop_annot);
	else
		prop_annot = ra_prop_get(clh_use, "rri-annot");

	if (opt->prop_temporal)
		prop_temporal = ra_prop_get(clh_use, opt->prop_temporal);
	else
		prop_temporal = ra_prop_get(clh_use, "qrs-temporal");

	if (!prop_value || (!opt->ignore_annot && (!prop_annot || !prop_temporal))
		|| (!opt->skip_freq_domain && !opt->use_event_pos && !prop_pos))
	{
		_ra_set_error(opt->eh, RA_ERR_EV_PROP_MISSING, "plugin-hrv");
		goto error;
	}

	get_event_list(opt, clh_use, prop_pos, &ev_list, num);
	if ((*num <= 0) || (ev_list == NULL))
		goto error;

	*b = malloc(sizeof(struct beats) * (*num));
	if (*b == NULL)
	{
		_ra_set_error(opt->eh, 0, "plugin-hrv");
		goto error;
	}

	vh_rri = ra_value_malloc();
	ok = malloc(sizeof(short) * (*num));
	ra_prop_get_value_some(prop_value, ev_list, *num, opt->ch, ok, vh_rri);
	rri = ra_value_get_double_array(vh_rri);
	if (!opt->use_event_pos)
	{
		vh_pos = ra_value_malloc();
		ra_prop_get_value_some(prop_pos, ev_list, *num, opt->ch, ok, vh_pos);
		pos = ra_value_get_long_array(vh_pos);
	}
	if (!opt->ignore_annot)
	{
		vh_annot = ra_value_malloc();
		ra_prop_get_value_some(prop_annot, ev_list, *num, opt->ch, ok, vh_annot);
		annot = ra_value_get_long_array(vh_annot);

		vh_temp = ra_value_malloc();
		ra_prop_get_value_some(prop_temporal, ev_list, *num, opt->ch, ok, vh_temp);
		temp = ra_value_get_short_array(vh_temp);
	}

	for (l = 0; l < *num; l++)
	{
		long s, e;

		(*b)[l].idx = l;
		(*b)[l].ev_id = ev_list[l];

		/* rri already in msec; Before, the RRI values were stored as long values. Therefore
		   in the previous versions the part after the decimalpoint was removed to get the
		   same results as before. But I think it makes no sense, therefore we use now the
		   double values. Additionally, the double values were used for the freq-domain,
		   herefore it makes less sense to use different values (long and double) for the
		   time- and freq-domain calculations. */
/* 		(*b)[l].rri = (double)ra_value_get_long(vh); just in case we need it again */
		(*b)[l].rri = rri[l];

		if (opt->use_event_pos)
		{
			ra_class_get_event_pos(clh_use, ev_list[l], &s, &e);
			(*b)[l].pos = s;
		}
		else
		{
			if (!prop_pos)
				(*b)[l].pos = -1;
			else
				(*b)[l].pos = (double)(pos[l]);
		}

		if (!opt->ignore_annot)
		{
			(*b)[l].rri_class = annot[l];
			(*b)[l].qrs_temporal = temp[l];
		}
		else
		{
			(*b)[l].rri_class = -1;
			(*b)[l].qrs_temporal = -1;
		}
	}

	ret = ra_info_get(ra_meas_handle_from_any_handle(opt->eh), RA_INFO_MAX_SAMPLERATE_D, vh);
	if (ret != 0)
		goto error;

	opt->samplerate = ra_value_get_double(vh);
	
	ret = 0;

	/* remove of ignored events has to be done here because for some
	   HRV calcualtions the running number of the events are important */
	if (opt->num_ignore_events > 0)
		ret = remove_ignored_events(opt, b, num);

 error:
	if (ev_list)
		free(ev_list);
	if (ok)
		free(ok);
	if (vh)
		ra_value_free(vh);
	if (vh_rri)
		ra_value_free(vh_rri);
	if (vh_pos)
		ra_value_free(vh_pos);
	if (vh_annot)
		ra_value_free(vh_annot);
	if (vh_temp)
		ra_value_free(vh_temp);

	return ret;
} /* get_values() */


int
get_given_values(struct ra_hrv *opt, struct beats **b, long *num)
{
	long l;

	if (!opt->values || (!opt->skip_freq_domain && !opt->pos))
	{
		_ra_set_error(opt->eh, RA_ERR_INFO_MISSING, "plugin-hrv");
		return -1;
	}

	*num = opt->num_values;

	*b = malloc(sizeof(struct beats) * (*num));
	if (*b == NULL)
	{
		_ra_set_error(opt->eh, 0, "plugin-hrv");
		return -1;
	}

	for (l = 0; l < *num; l++)
	{
		(*b)[l].idx = l;
		(*b)[l].ev_id = -1;
		if (opt->pos)
			(*b)[l].pos = opt->pos[l];
		else
			(*b)[l].pos = -1;
		(*b)[l].rri = opt->values[l];
		(*b)[l].rri_class = -1;
		(*b)[l].qrs_temporal = -1;
	}

	return 0;
} /* get_given_values() */


int
get_event_list(struct ra_hrv *opt, class_handle clh, prop_handle pos, long **list, long *num)
{
	int ret = 0;
	value_handle vh, vh_pos;
	const long *ev_id;
	long l, n, s, e;
	long start_ev, end_ev;

	*num = 0;

	if (opt->num_events > 0)
	{
		if (opt->events == NULL)
		{
			_ra_set_error(clh, RA_ERR_INFO_MISSING, "plugin-hrv");
			return -1;
		}

		*num = opt->num_events;
		
		*list = malloc(sizeof(long) * opt->num_events);
		memcpy(*list, opt->events, sizeof(long) * opt->num_events);

		return 0;
	}

	vh = ra_value_malloc();
	vh_pos = ra_value_malloc();

	ra_class_get_events(clh, -1, -1, 0, 1, vh);
	n = ra_value_get_num_elem(vh);
	*list = malloc(sizeof(long) * n);
	ev_id = ra_value_get_long_array(vh);

	start_ev = end_ev = -1;
	if (opt->use_start_end_pos)
	{
		const long *p, *ev_id_pos;
		value_handle vh_ev_id_pos;

		vh_ev_id_pos = ra_value_malloc();

		ra_prop_get_value_all(pos, vh_ev_id_pos, NULL, vh_pos);
		p = ra_value_get_long_array(vh_pos);
		ev_id_pos = ra_value_get_long_array(vh_ev_id_pos);

		for (l = 0; l < n; l++)
		{
			if (opt->use_event_pos)
			{
				ra_class_get_event_pos(clh, ev_id[l], &s, &e);

				if (e < opt->start_pos)
					continue;
				if (start_ev == -1)
					start_ev = l;
				if (s > opt->end_pos)
				{
					end_ev = ev_id[l-1];
					break;
				}
				(*num)++;
			}
			else
			{
				if (p[l] < opt->start_pos)
					continue;
				if (start_ev == -1)
					start_ev = l;
				if (p[l] > opt->end_pos)
				{
					end_ev = ev_id[l-1];
					break;
				}
				(*num)++;
			}

		}

		ra_value_free(vh_ev_id_pos);

		memcpy(*list, ev_id+start_ev, sizeof(long) * (*num));
	}
	else
	{
		*num = n;
		memcpy(*list, ev_id, sizeof(long) * n);		
	}

	ra_value_free(vh);
	ra_value_free(vh_pos);

	return ret;
} /* get_event_list() */


int
remove_ignored_events(struct ra_hrv *opt, struct beats **b, long *num)
{
	int ret;
	long l, cnt, curr_ignore;
	struct beats *new_b;
	long *ev_list, *idx_found, num_idx_found;

	if (opt->ignore_events == NULL)
	{
		_ra_set_error(opt->eh, RA_ERR_INFO_MISSING, "plugin-hrt");
		return -1;
	}

	ev_list = ra_alloc_mem(sizeof(long) * (*num));
	for (l = 0; l < (*num); l++)
		ev_list[l] = (*b)[l].ev_id;

	if ((ret = get_intersection_long(ev_list, *num, opt->ignore_events, opt->num_ignore_events,
					 &idx_found, &num_idx_found)) != 0)
	{
		ra_free_mem(ev_list);
		_ra_set_error(opt->eh, RA_ERR_ERROR, "plugin-hrt");
		return ret;
	}

	ra_free_mem(ev_list);
	if (num_idx_found <= 0)
		return 0;

	new_b = (struct beats *)malloc(sizeof(struct beats) * (*num));
	cnt = 0;
	curr_ignore = 0;
	for (l = 0; l < *num; l++)
	{
		if ((curr_ignore < num_idx_found) && (idx_found[curr_ignore] == l))
		{
			curr_ignore++;
			continue;
		}
		
		new_b[cnt++] = (*b)[l];
	}

	memcpy(*b, new_b, sizeof(struct beats) * cnt);
	*b = realloc(*b, sizeof(struct beats) * cnt);
	*num = cnt;
	
	free(new_b);
	ra_free_mem(idx_found);

	return 0;
} /* remove_ignored_events() */


int
filter_values(struct ra_hrv *opt, struct beats **b, long *num,
	      struct beats **b_no_art, long *num_no_art)
{
	long l, nu;
	struct beats *bu = NULL;

	if (*b_no_art != NULL)
		return -1;

	bu = malloc(sizeof(struct beats) * (*num));
	*b_no_art = malloc(sizeof(struct beats) * (*num));
	nu = 0;
	*num_no_art = 0;
	for (l = 0; l < *num; l++)
	{
		/* check for sane value */
		if (opt->use_min_max)
		{
			if (((*b)[l].rri <= opt->min_value) || ((*b)[l].rri >= opt->max_value))
				continue;
		}

		if (!opt->ignore_annot)
		{
			if (IS_ARTIFACT((*b)[l].rri_class))
				continue;
		}

		/* store non-artifacts RR intervals */
		(*b_no_art)[*num_no_art] = (*b)[l];
		(*num_no_art)++;

		/* check for normal-to-normal beats */
		if (!opt->ignore_annot)
		{
			if (!IS_SINUS((*b)[l].rri_class) || ((*b)[l].qrs_temporal != ECG_TEMPORAL_NORMAL))
				continue;
		}

		bu[nu] = (*b)[l];
		nu++;
	}

	bu = realloc(bu, sizeof(struct beats) * nu);
	*num = nu;
	free(*b);
	*b = bu;

	*b_no_art = realloc(*b_no_art, sizeof(struct beats) * (*num_no_art));

	return 0;
} /* filter_values() */


int
calc_hrv_timedomain(struct beats *b, long num, value_handle *res, long run_num)
{
	calc_sdnn(b, num, res[SDNN + (run_num * num_results)]);
	calc_hrvi(b, num, res[HRVTI + (run_num * num_results)]);
	calc_sdann(b, num, res[SDANN + (run_num * num_results)]);
	calc_rmssd(b, num, res[RMSSD + (run_num * num_results)]);
	calc_pnn50(b, num, res[PNN50 + (run_num * num_results)]);

	return 0;
} /* calc_hrv_timedomain() */


void
calc_sdnn(struct beats *b, long num, value_handle res)
{
	long l;
	double *v;

	v = malloc(sizeof(double) * num);
	for (l = 0; l < num; l++)
		v[l] = b[l].rri;

	ra_value_set_double(res, stddev(v, num));

	free(v);
} /* calc_sdnn() */


void
calc_hrvi(struct beats *b, long num, value_handle res)
{
	long l, max_pos;
	double *v, *hist_d;
	long *hist;
	long hist_start, hist_size;
	double scale;

	/* "scale" rri because hrvi-values are based on NN interval
	   values with a binning of 7.8125 (128Hz samplerate) (see
	   HRV Guidelines in Europaen Heart Journal (1996) 17, 354-381;
 	   on page 356 and 357); rr-intervals are in milliseconds */
	scale = 128.0 / 1000.0;

	v = malloc(sizeof(double) * num);
	for (l = 0; l < num; l++)
 	{
 		v[l] = b[l].rri * scale;
 		if ((v[l] - (long)v[l]) >= 0.5)
 			v[l] += 1.0;
 	}

	if (histogram(v, num, &hist, &hist_start, &hist_size, 1) != 0)
	{
		free(v);
		return;
	}
	if (hist_size <= 0)
	{
		free(v);
		free(hist);
		return;
	}

	/* copy histogram to double-array because get_max() needs a double-array
	   (TODO: decide if histogram returns double-hist or add get_max_long()) */
	hist_d = malloc(sizeof(double) * hist_size);
	for (l = 0; l < hist_size; l++)
		hist_d[l] = hist[l];

	get_max(hist_d, hist_size, &max_pos);

	if (hist[max_pos] > 0)
	{
		double r = (double) num / hist[max_pos];
		ra_value_set_double(res, r);
	}

	free(v);
	free(hist);
	free(hist_d);
} /* calc_hrvi() */


void
calc_sdann(struct beats *b, long num, value_handle res)
{
	long l, cnt, cnt_mean, last_idx;
	double *v, *m;

	/* first make 5-min mean-rri values */
	v = malloc(sizeof(double) * num);
	m = malloc(sizeof(double) * num);
	last_idx = 1;
	cnt = cnt_mean = 0;
	for (l = 1; l < num; l++)
	{
		double d = b[l].pos - b[last_idx].pos;
		if (d < 300000.0)	/* 5min * 60sec * 1000msec */
		{
			v[cnt] = b[l].rri;
			cnt++;
			continue;
		}
		if (cnt > 100)	/* at least 100 normal-rri must be in a 5 min interval */
		{
			m[cnt_mean] = mean(v, cnt);
			cnt_mean++;
		}
		cnt = 0;
		last_idx = l;
	}

	/* and now calc SDANN */
	ra_value_set_double(res, stddev(m, cnt_mean));

	free(v);
	free(m);
} /* calc_sdann() */


void
calc_rmssd(struct beats *b, long num, value_handle res)
{
	long l, cnt;
	double *v;

	v = malloc(sizeof(double) * num);
	cnt = 0;
	for (l = 1; l < num; l++)
	{
		double t;

		/* only use successive rr-intervals */
		if ((b[l].idx - b[l - 1].idx) > 1)
			continue;

		t = b[l].rri - b[l - 1].rri;
		v[cnt] = t * t;
		cnt++;
	}

	ra_value_set_double(res, sqrt(mean(v, cnt)));

	free(v);
} /* calc_rmssd() */


void
calc_pnn50(struct beats *b, long num, value_handle res)
{
	long l, cnt;
	double *v;
	long *hist;
	long hist_start, hist_size;
	long num50 = 0;
	double r;

	v = malloc(sizeof(double) * num);
	cnt = 0;
	for (l = 1; l < num; l++)
	{
		/* only use successive rr-intervals */
		if ((b[l].idx - b[l - 1].idx) > 1)
			continue;

		v[cnt] = b[l].rri - b[l - 1].rri;
		cnt++;
	}

	if (histogram(v, cnt, &hist, &hist_start, &hist_size, 1) != 0)
	{
		free(v);
		return;
	}

	/* TODO: find "better" way, now it's brute force (but it works) */
	for (l = 0; l < hist_size; l++)
	{
		if (abs(hist_start + l) >= 50)
			num50 += hist[l];
	}
	r = ((double) num50 / (double) cnt) * 100.0;
	ra_value_set_double(res, r);

	if (v)
		free(v);
	if (hist)
		free(hist);
} /* calc_pnn50() */


int
calc_hrv_freqdomain(struct proc_info *pi, struct beats *b, long num, char *window, value_handle *res, long run_num)
{
	int ret = -1;
	double *spec = NULL;
	long n_spec = 0;
	long n_spec_save;
	double samplerate = 1000.0 / EQUIDIST_MS;

	/* calc HRV-freq parameters from "normal" periodogram  */
	if ((ret = get_periodogram(pi, b, num, window, 0, &spec, &n_spec)) != 0)
		return ret;
	ret = get_hrv_freqvalues(pi->options, spec, n_spec, res, run_num);
	if (spec)
		free(spec);
	n_spec_save = n_spec;

	/* calc power-law parameter (using a smoothed periodogram) */
	/* TODO: find better way of handling smoothed periodograms (particular how to handle n_spec_save) */
	if ((ret = get_periodogram(pi, b, num, window, 1, &spec, &n_spec)) != 0)
		return ret;
	ret = calc_power_law(spec, n_spec, samplerate, res, n_spec_save);
	if (spec)
		free(spec);

	return ret;
} /* calc_hrv_freqdomain() */


int
get_periodogram(struct proc_info *pi, struct beats *b, long num, char *window, int smooth_spec,
		double **spec, long *n_spec)
{
	plugin_handle pl;
	struct ra_hrv *opt;
	struct proc_info *pi_freq;
	value_handle vh;
	double *values, *pos;
	double samplerate;
	long l;

	opt = pi->options;

	if ((pl = ra_plugin_get_by_name(ra_lib_handle_from_any_handle(pi->mh), "freq-analysis", 0)) == NULL)
		return -1;

	if ((pi_freq = (struct proc_info *)ra_proc_get(pi->mh, pl, NULL)) == NULL)
		return -1;

	values = (double *)malloc(sizeof(double) * num);
	pos = (double *)malloc(sizeof(double) * num);
	for (l = 0; l < num; l++)
	{
		values[l] = b[l].rri;
		pos[l] = b[l].pos;
	}

	vh = ra_value_malloc();

	ra_value_set_double_array(vh, values, num);
	ra_lib_set_option(pi_freq, "values", vh);
	ra_value_set_double_array(vh, pos, num);
	ra_lib_set_option(pi_freq, "pos", vh);
	ra_value_set_double(vh, opt->samplerate);
	ra_lib_set_option(pi_freq, "pos_samplerate", vh);

	free(values);
	free(pos);

	ra_value_set_short(vh, 1);
	ra_lib_set_option((proc_handle)pi_freq, "use_time", vh);

	ra_value_set_short(vh, 1);
	ra_lib_set_option((proc_handle)pi_freq, "smooth_data", vh);
	ra_value_set_string(vh, "boxcar");
	ra_lib_set_option((proc_handle)pi_freq, "smooth_method", vh);
	ra_value_set_double(vh, 2.0);
	ra_lib_set_option((proc_handle)pi_freq, "smooth_width", vh);

	samplerate = 1000.0 / EQUIDIST_MS;
	ra_value_set_double(vh, samplerate);
	ra_lib_set_option((proc_handle)pi_freq, "samplerate", vh);
	
	ra_value_set_string(vh, window);
	ra_lib_set_option((proc_handle)pi_freq, "window", vh);
	ra_value_set_short(vh, 1);
	ra_lib_set_option((proc_handle)pi_freq, "remove_mean", vh);
	
	ra_value_set_short(vh, (short)smooth_spec);
	ra_lib_set_option((proc_handle)pi_freq, "smooth_spec", vh);

	if (ra_proc_do(pi_freq) != 0)
	{
		ra_proc_free(pi_freq);
		ra_value_free(vh);
		return -1;
	}

	/* only one result-set; needed result is on index '1' (PSD) */
	if (ra_proc_get_result(pi_freq, 1, 0, vh) == 0)
	{
		*n_spec = ra_value_get_num_elem(vh);
		*spec = malloc(sizeof(double) * ra_value_get_num_elem(vh));
		memcpy(*spec, ra_value_get_double_array(vh), sizeof(double) * ra_value_get_num_elem(vh));
	}

	ra_proc_free(pi_freq);
	ra_value_free(vh);

	return 0;
} /* get_periodogram() */


/* do not delete because to keep call of calc_welch_spec() */
/* int */
/* calc_spec(double *v, long n, double samplefreq, double **spec, long *n_spec, char *window) */
/* { */
/* 	int ret = 0; */

/* 	ret = calc_periodogram(v, n, samplefreq, spec, n_spec, window); */

/* /\*        ret = calc_welch_spec(v, n, samplefreq, spec, n_spec, 4096, 2048, window); *\/ */

/* 	return 0; */
/* } /\* calc_spec() *\/ */


int
get_hrv_freqvalues(struct ra_hrv *opt, double *spec, long n, value_handle *res, long run_num)
{
	calc_total_power(spec, n, res[TP + (run_num * num_results)]);
	calc_ulf_power(spec, n, res[ULF + (run_num * num_results)]);
	calc_vlf_power(spec, n, res[VLF + (run_num * num_results)]);
	calc_lf_power(spec, n, res[LF + (run_num * num_results)]);
	calc_hf_power(spec, n, res[HF + (run_num * num_results)]);

	if (ra_value_is_ok(res[TP + (run_num * num_results)])
	    && ra_value_is_ok(res[ULF + (run_num * num_results)])
	    && ra_value_is_ok(res[VLF + (run_num * num_results)]))
	{
		double norm = ra_value_get_double(res[TP + (run_num * num_results)])
			- ra_value_get_double(res[ULF + (run_num * num_results)])
			- ra_value_get_double(res[VLF + (run_num * num_results)]);

		if (ra_value_is_ok(res[LF + (run_num * num_results)]))
		{
			double v = (ra_value_get_double(res[LF + (run_num * num_results)]) / norm) * 100.0;
			ra_value_set_double(res[LF_NORM + (run_num * num_results)], v);
		}
		if (ra_value_is_ok(res[HF + (run_num * num_results)]))
		{
			double v = (ra_value_get_double(res[HF + (run_num * num_results)]) / norm) * 100.0;
			ra_value_set_double(res[HF_NORM + (run_num * num_results)], v);
		}
	}

	if (ra_value_is_ok(res[LF + (run_num * num_results)]) && ra_value_is_ok(res[HF + (run_num * num_results)]))
	{
		double ra = ra_value_get_double(res[LF + (run_num * num_results)])
			/ ra_value_get_double(res[HF + (run_num * num_results)]);
		ra_value_set_double(res[LF_HF_RATIO + (run_num * num_results)], ra);
	}

	if (opt->use_user_freq_band)
	{
		double user_power = calc_power(opt->user_freq_start, opt->user_freq_end, EQUIDIST_MS, spec, n);
		ra_value_set_double(res[USER_BAND + (run_num * num_results)], user_power);
	}

	return 0;
} /* get_hrv_freqvalues() */


int
calc_total_power(double *spec, long n, value_handle res)
{
	double v = calc_power(TOTAL_POWER_START, TOTAL_POWER_END, EQUIDIST_MS, spec, n);
	ra_value_set_double(res, v);

	return 0;
} /* calc_total_power() */


int
calc_ulf_power(double *spec, long n, value_handle res)
{
	double v = calc_power(ULF_START, ULF_END, EQUIDIST_MS, spec, n);
	ra_value_set_double(res, v);

	return 0;
} /* calc_ulf_power() */


int
calc_vlf_power(double *spec, long n, value_handle res)
{
	double v = calc_power(VLF_START, VLF_END, EQUIDIST_MS, spec, n);
	ra_value_set_double(res, v);

	return 0;
} /* calc_vlf_power() */


int
calc_lf_power(double *spec, long n, value_handle res)
{
	double v = calc_power(LF_START, LF_END, EQUIDIST_MS, spec, n);
	ra_value_set_double(res, v);

	return 0;
} /* calc_lf_power() */


int
calc_hf_power(double *spec, long n, value_handle res)
{
	double v = calc_power(HF_START, HF_END, EQUIDIST_MS, spec, n);
	ra_value_set_double(res, v);

	return 0;
} /* calc_hf_power() */


int
calc_power_law(double *spec, long n, double samplefreq, value_handle *res, long n_spec_orig)
{
 	long start_idx, end_idx, len, l, cnt;
 	double freq_step;
 	int begin;
	double *x, *y;
	double offset, slope;

 	freq_step = samplefreq / ((double)n_spec_orig * 2.0);
 	begin = (int)(log10(freq_step)-1.0);
 
 	start_idx = (long)((log10(POWER_LAW_START) - (double)begin) * 60.0);
 	if (start_idx < 0)
 		start_idx = 0;
 	end_idx = (long)((log10(POWER_LAW_END) - (double)begin) * 60.0);
 	if (end_idx >= n)
 		end_idx = (n-1);

 	len = end_idx - start_idx + 1;
 	x = calloc(len, sizeof(double));
 	y = calloc(len, sizeof(double));
 	cnt = 0;
 	for (l = start_idx; l <= end_idx; l++)
 	{
 		if (spec[l] == -1.0)
 			continue;

 		x[cnt] = (l - start_idx) / 60.0;
 		y[cnt] = log10(spec[l]);
 		++cnt;
 	}
 	
 	calc_regression(x, y, cnt, &offset, &slope);

	ra_value_set_double(res[POWER_LAW], slope);

 	if (x)
 		free(x);
 	if (y)
 		free(y);

	return 0;
} /* calc_power_law() */


double
calc_power(double start, double end, double sample_dist_ms, double *spec, long n)
{
	long start_idx, end_idx, l;
	double sum;
	double freq_step;

	/* don't use first value because start < x <= end */
	start_idx = calc_idx_from_freq((1000.0 / sample_dist_ms), n * 2, start) + 1;
	end_idx = calc_idx_from_freq((1000.0 / sample_dist_ms), n * 2, end);

	if (end_idx > n)
		end_idx = n;

	freq_step = (1000.0 / sample_dist_ms) / ((double) n * 2.0);
	sum = 0.0;
	for (l = start_idx; l <= end_idx; l++)
		sum += spec[l];
	sum *= freq_step;

	return sum;
} /* calc_power() */


long
calc_idx_from_freq(double samplefreq, long n_sample, double freq)
{
	double freq_step;
	long idx;

	freq_step = samplefreq / (double) n_sample;
	idx = (long) ((freq / freq_step) + 0.5);

	return idx;
}  /* calc_idx_from_freq() */


int
calc_nld(struct ra_hrv *opt, struct beats *b, long num, struct beats *b_no_art, long num_no_art, value_handle *res)
{
	calc_poincare(opt, b, num, res);
	calc_dfa(opt, b, num, b_no_art, num_no_art, res);

	return 0;
} /* calc_nld() */


int
calc_poincare(struct ra_hrv *opt, struct beats *b, long num, value_handle *res)
{
	long l, cnt;
	double *x, *y, *a;
	double mean_rri;

	x = malloc(sizeof(double) * num);
	y = malloc(sizeof(double) * num);
	cnt = 0;
	for (l = opt->poincare_lag; l < num; l++)
	{
		/* only use successive rr-intervals */
		if ((b[l].idx - b[l - opt->poincare_lag].idx) > opt->poincare_lag)
			continue;

		x[cnt] = b[l - opt->poincare_lag].rri;
		y[cnt] = b[l].rri;

		cnt++;
	}

	/* calc SD1 */
	a = malloc(sizeof(double) * cnt);
	for (l = 0; l < cnt; l++)
	{
		double t = y[l] - x[l];
		a[l] = sqrt((t * t) / 2.0);
	}
	ra_value_set_double(res[SD1], stddev(a, cnt));

	/* calc SD2 */
	mean_rri = mean(x, cnt);
	for (l = 0; l < cnt; l++)
	{
		double t = y[l] - (-x[l] + 2.0*mean_rri);
		a[l] = sqrt((t * t) / 2.0);
	}
	ra_value_set_double(res[SD2], stddev(a, cnt));

	free(a);
	free(x);
	free(y);

	return 0;
} /* calc_poincare() */


int
calc_dfa(struct ra_hrv *opt, struct beats *b, long num, struct beats *b_no_art,
	 long num_no_art, value_handle *res)
{
	int ret = 0;
	struct beats *bu;
	long nu, num_seg, l, num_scales, last_size, seg_size_use;
	double *x = NULL;
	double *scales = NULL;
	long *num_scale_values = NULL;
	double offset, slope;
	long a1_2_cut;

	bu = b_no_art;
	nu = num_no_art;
	if (opt->dfa_use_only_nn)
	{
		bu = b;
		nu = num;
	}

	if ((opt->dfa_segment_size > 0) && (nu >= opt->dfa_segment_size))
	{
		seg_size_use = opt->dfa_segment_size;
		num_seg = nu / opt->dfa_segment_size;
		last_size = nu % opt->dfa_segment_size;
		if (last_size)
			num_seg++;
		num_scales = (opt->dfa_segment_size/4) - 4;
	}
	else
	{
		seg_size_use = nu;
		num_seg = 1;
		num_scales = (nu/4) - 4;
		last_size = 0;
	}
	
	scales = calloc(num_scales, sizeof(double));
	num_scale_values = calloc(num_scales, sizeof(long));
	for (l = 0; l < num_seg; l++)
	{
		long size_use = seg_size_use;

		if (last_size && (l == (num_seg - 1)))
			size_use = last_size;

		ret = calc_dfa_scales(bu+(l*opt->dfa_segment_size), size_use, scales, num_scale_values);
		if (ret != 0)
			goto error;
	}

	x = malloc(sizeof(double) * num_scales);
	for (l = 0; l < num_scales; l++)
	{
		scales[l] = log10(scales[l] / num_scale_values[l]);
		x[l] = log10(l+4);
	}
	ra_value_set_double_array(res[DFA_X], x, num_scales);
	ra_value_set_double_array(res[DFA_Y], scales, num_scales);

	/* calc DFA parameters */
	calc_regression(x, scales, num_scales, &offset, &slope);
	ra_value_set_double(res[DFA], slope);
	ra_value_set_double(res[DFA_OFFSET], offset);

	a1_2_cut = opt->dfa_alpha1_alpha2_cut - 4 + 1;
	if ((a1_2_cut >= 3) && (a1_2_cut <= num_scales))
	{
		calc_regression(x, scales, a1_2_cut, &offset, &slope);
		ra_value_set_double(res[DFA1], slope);
		ra_value_set_double(res[DFA1_OFFSET], offset);
	}
	if (a1_2_cut < (num_scales-3))
	{
		calc_regression(x+a1_2_cut, scales+a1_2_cut, num_scales-a1_2_cut, &offset, &slope);
		ra_value_set_double(res[DFA2], slope);
		ra_value_set_double(res[DFA2_OFFSET], offset);
	}

	if (opt->dfa_use_user_range)
	{
		long lower_limit, upper_limit, num;

		lower_limit = opt->dfa_user_lower_limit - 4;
		if (lower_limit < 0)
			lower_limit = 0;
		upper_limit = opt->dfa_user_upper_limit - 4;
		if (upper_limit >= num_scales)
			upper_limit = num_scales - 1;

		num = upper_limit - lower_limit + 1;
		calc_regression(x+lower_limit, scales+lower_limit, num, &offset, &slope);
		ra_value_set_double(res[DFA_USER], slope);
		ra_value_set_double(res[DFA_USER_OFFSET], offset);
	}

 error:
	free(scales);
	free(num_scale_values);
	free(x);

	return ret;
} /* calc_dfa() */


int
calc_dfa_scales(struct beats *b, long len, double *scales, long *num_scale_values)
{
	long l, m, n;
	double *x, *y, *rri, *y_detrended;
	double rri_mean;
	long num_scales;

	num_scales = (len/4) - 4;

	x = malloc(sizeof(double) * num_scales);
	for (l = 0; l < num_scales; l++)
		x[l] = l;

	rri = malloc(sizeof(double) * len);
	for (l = 0; l < len; l++)
		rri[l] = b[l].rri;
	rri_mean = mean(rri, len);
	y = malloc(sizeof(double) * len);
	for (l = 0; l < len; l++)
	{
		y[l] = 0.0;
		for (m = 0; m <= l; m++)
			y[l] += rri[m] - rri_mean;
	}

	y_detrended = malloc(sizeof(double) * len);
	/* TODO: check why '+4' because this can results in a core dump
	   (because x is only num_scales large) */
/* 	for (l = 4; l < (num_scales+4); l++) */
	for (l = 4; l < num_scales; l++)
	{
		double scale_value = 0.0;
		long num_steps = len / l;

		for (m = 0; m < num_steps; m++)
		{
			double offset, slope, single_slope;
			calc_regression(x, y+(m*l), l, &offset, &slope);

			/* remove linear regression */
 			single_slope = slope / (double)l;
 			for (n = 0; n < l; n++)
 				y_detrended[m*l+n] = y[m*l+n] - ((double)n * single_slope) - offset;
		}

		for (m = 0; m < (l * num_steps); m++)
			scale_value += (y_detrended[m] * y_detrended[m]);
		scales[l-4] = sqrt(scale_value / (double)(l * num_steps));
		num_scale_values[l-4]++;
	}

	free(x);
	free(y);
	free(y_detrended);

	return 0;
} /* calc_dfa_scales() */



/* TODO: extend eval-structure to handle calculation-results */
int
save(proc_handle proc, eval_handle eh)
{
	if (proc || eh)
		;

	return 0;
} /* save() */

