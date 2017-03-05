/*----------------------------------------------------------------------------
 * ecg.c  -  perform ecg specific actions after reading original evaluation or doing detect using libRASCH
 *
 * Description:
 * The plugin performs a finer classification of the qrs-complexes of an ecg,
 * using from a previous evaluation only the information of the general annotation
 * of a beat (coming from atrium, from ventricle, paced beat or artifact). 
 * Using this plugin allows to have in libRASCH a consistent sub-classification
 * (eg premature normal beat) independent of the original evaluation system used.
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
#include <math.h>

#define _LIBRASCH_BUILD
#include <ra.h>
#include <ra_priv.h>
#include <pl_general.h>
#include <ra_ecg.h>
#include <ra_eval.h>
#include <ra_detect.h>

#include "ecg.h"



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

	/* set auto-create infos */
	ps->info.num_create_class = num_create_class;
	if (num_create_class > 0)
	{
		ps->info.create_class = ra_alloc_mem(sizeof(create_class));
		memcpy(ps->info.create_class, create_class, sizeof(create_class));
	}
	else
		ps->info.create_class = NULL;
	ps->info.num_create_prop = num_create_prop;
	if (num_create_prop > 0)
	{
		ps->info.create_prop = ra_alloc_mem(sizeof(create_prop));
		memcpy(ps->info.create_prop, create_prop, sizeof(create_prop));
	}
	else
		ps->info.create_prop = NULL;

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
	if (ra_plugin_get_by_name(ra, "gui-ecg", 1))
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
	pi->options = calloc(1, sizeof(struct ra_ecg_options));
	set_option_offsets(ps->info.opt, pi->options);
	set_default_options(pi->options);

	/* init auto-create structures to be sure we have the correct values */
	if (num_create_class > 0)
		memcpy(ps->info.create_class, create_class, sizeof(create_class));
	if (num_create_prop > 0)
		memcpy(ps->info.create_prop, create_prop, sizeof(create_prop));

	return pi;
} /* pl_get_proc_handle() */


void
set_default_options(struct ra_ecg_options *opt)
{
	opt->eh = NULL;
	opt->rh = NULL;
	opt->use_class = 1;
	opt->clh = NULL;

	opt->use_start_end_pos = 0;
	opt->start_pos = -1;
	opt->end_pos = -1;

	opt->num_ignore_events = 0;
	opt->ignore_events = NULL;

	opt->save_in_class = 1;
	opt->num_data = 0;
	opt->data = NULL;
	opt->annot = NULL;
	opt->flags = NULL;
	opt->data_is_pos = 0;

	opt->min_beat_distance = 50;

	opt->num_ref_rri = 5;
	opt->ref_normal_percent = 5;
	opt->normal_percent = 10;
	opt->normal_pre_percent = 20;
	opt->normal_late_percent = 20;
	opt->max_abs_difference = 200;
	opt->vpc_pre_percent = 20;
	opt->vpc_late_percent = 20;
	opt->min_interp_vpc_percent = 45;
	opt->max_interp_vpc_percent = 55;
	opt->time2get_new_ref_value = 60;
	opt->min_rri = 200;
	opt->max_rri = 10000;
	opt->pause = 2500;
	opt->do_summarize = 1;
	opt->get_respiration_values = 1;
	opt->skip_artifact_detection = 0;
} /* set_default_options() */


void
set_option_offsets(struct ra_option_infos *opt_inf, struct ra_ecg_options *opt)
{
	unsigned char *p = (unsigned char *)opt;
	int idx = 0;

	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->eh) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->rh) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->use_class) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->clh) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->use_start_end_pos) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->start_pos) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->end_pos) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->num_ignore_events) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->ignore_events) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->save_in_class) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->num_data) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->data) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->annot) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->flags) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->data_is_pos) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->min_beat_distance) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->num_ref_rri) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->ref_normal_percent) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->normal_percent) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->normal_pre_percent) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->normal_late_percent) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->max_abs_difference) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->vpc_pre_percent) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->vpc_late_percent) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->min_interp_vpc_percent) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->max_interp_vpc_percent) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->time2get_new_ref_value) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->min_rri) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->max_rri) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->pause) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->do_summarize) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->get_respiration_values) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->skip_artifact_detection) - p);

	/* consistency check (useful when changing option structure) */
	if (idx != num_options)
	{
		fprintf(stderr, "critical error in ecg.c - set_option_offsets():\n  number of options(%ld) != idx(%d)\n", num_options, idx);
		exit(-1);
	}
} /* set_option_offsets() */


void
pl_free_proc_handle(proc_handle proc)
{
	struct proc_info *pi = proc;
	
	if (pi->options)
	{
		struct ra_ecg_options *opt = pi->options;

		if (opt->data)
			ra_free_mem(opt->data);
		if (opt->annot)
			ra_free_mem(opt->annot);

		free(opt);
	}

	if (pi->results)
	{
		int i;
		value_handle *t;

		t = pi->results;
		for (i = 0; i < num_results; i++)
			ra_value_free(t[i]);
		free(t);
	}

	ra_free_mem(proc);
} /* pl_free_proc_handle() */


int
pl_do_processing(proc_handle proc)
{
	int ret = 0;
	struct proc_info *pi = proc;
	struct beat_infos *beats = NULL;
	struct ra_ecg_options *opt;
	long l, run, num, n_runs;
	value_handle *res;
	double samplerate;
	class_handle clh_use;
	long num_ch;
	long *ch = NULL;
	double *ch_samplerate = NULL;

	num = 0;

	if (pi == NULL)
		return -1;

	if (pi->callback)
		(*pi->callback)(gettext("calculate ecg-values - init"), 0);
	
	opt = pi->options;

	if ((opt->use_class || !opt->data_is_pos) && opt->eh == NULL)
	{
		opt->eh = ra_eval_get_default(pi->mh);
		if (opt->eh == NULL)
		{
			_ra_set_error(proc, RA_WARN_NO_DEFAULT_EVAL, "plugin-ecg");
			return -1;
		}
	}
		
	/* if rec-handle was not set in options, use root recording */
	if (!opt->rh)
	{
		opt->rh = ra_rec_get_first(pi->mh, 0);
		if (opt->rh == NULL)
			goto error;
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

	samplerate = get_samplerate(pi->mh);
	if (pi->rh == NULL)
		pi->rh = ra_rec_get_first(pi->mh, 0);
	get_ecg_channels(pi->rh, &ch, &num_ch, &ch_samplerate);

	for (run = 0; run < n_runs; run++)
	{
		if (pi->callback)
		{
			char s[100];
			sprintf(s, "%s %ld/%ld", gettext_noop("calculate ecg-values - set"), run+1, n_runs);
			(*pi->callback)(s, 0);
		}

		if (beats)
		{
			free_beats_mem(beats, num);
			free(beats);
			beats = NULL;
			num = 0;
		}

		/* get beat-pos and class from evaluation */
		clh_use = NULL;
		if ((ret = get_eval_values(pi, run, &beats, &num, ch, num_ch, &clh_use)) != 0)
			goto error;

		if (num <= 0)
			continue;

		if (pi->callback)
			(*pi->callback)(NULL, 20);
				
		/* calc RR intervals */
		calc_rri(beats, num, samplerate);
		
		if (pi->callback)
			(*pi->callback)(NULL, 30);
		
		/* find beats which are on the same position */
		check_beats_on_same_pos(clh_use, beats, &num, opt->min_beat_distance, opt->save_in_class);
		calc_rri(beats, num, samplerate); /* redo rri-calc because to correct rri's if some beats were removed */
		
		if (pi->callback)
			(*pi->callback)(NULL, 40);
		
		/* calc reference rri (mean of last 5 normal rri) and grade RRIs according
		   to their temporal setting */
		calc_temporal_setting(opt, beats, num, samplerate);
		
		if (pi->callback)
			(*pi->callback)(NULL, 50);
		
		/* try to find artifacts (e.g. detection of t-waves as qrs-complexes) */
		if (!opt->skip_artifact_detection)
			filter_artifacts(opt, beats, num);
		
		if (pi->callback)
			(*pi->callback)(NULL, 60);
		
		/* sub classify beats (e.g. SVPCs, escape beats) */
		subclass_beats(opt, beats, num);
		
		if (pi->callback)
			(*pi->callback)(NULL, 70);
		
		/* after all beats are filtered and "sub classified", now "correct" RRI classifications */
		make_rri_classifications(beats, num);
		
		if (pi->callback)
			(*pi->callback)(NULL, 80);
		
		/* calc morphology values */
		calc_morphology_values(beats, num, num_ch, ch_samplerate);		

		if (pi->callback)
			(*pi->callback)(NULL, 85);
		
		/* check if I should do a summary */
		if (opt->do_summarize)
			make_summary(opt, run, beats, num, res);
		
		if (pi->callback)
			(*pi->callback)(NULL, 90);
		
		/* check if I have to get the respiration phase */
		if (opt->get_respiration_values)
			get_resp_phase(opt, run, beats, num, res, clh_use);

		if (pi->callback)
			(*pi->callback)(NULL, 100);
		
		if (pi->callback)
			(*pi->callback)("save results", 0);
		
		save_in_res(run, pi, beats, num);
		
		if (pi->callback)
			(*pi->callback)(NULL, 20);
		
		if (clh_use && opt->save_in_class)
		{
			if (pi->callback)
				(*pi->callback)(gettext("save ecg-values"), 0);
			
			if (opt->use_class)
			{				
				ret = ra_eval_save_result(ra_eval_get_handle(clh_use), clh_use, pi, run);
			}
			else
				ret = save_single(clh_use, beats, num);
		}
	}

	if (pi->callback)
		(*pi->callback)(NULL, 100);

 error:
	if (beats)
	{
		free_beats_mem(beats, num);
		free(beats);
		beats = NULL;
	}

	free(ch);
	free(ch_samplerate);

	return ret;
} /* pl_do_processing() */


int
get_number_runs(struct ra_ecg_options *opt)
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
get_samplerate(meas_handle mh)
{
	value_handle vh;
	double samplerate = 0.0;

	vh = ra_value_malloc();
	/* positions in evaluations are always at the highest samplerate */
	ra_info_get(mh, RA_INFO_MAX_SAMPLERATE_D, vh);
	samplerate = ra_value_get_double(vh);
	ra_value_free(vh);

	return samplerate;
} /* get_samplerate() */


void
free_beats_mem(struct beat_infos *beats, long num)
{
	long l;

	for (l = 0; l < num; l++)
		free_one_beat_mem(beats + l);
} /* free_beats_mem() */


void
free_one_beat_mem(struct beat_infos *b)
{
	if (b->morph_flags)
		free(b->morph_flags);

	if (b->p_start)
		free(b->p_start);
	if (b->p_end)
		free(b->p_end);
	
	if (b->qrs_start)
		free(b->qrs_start);
	if (b->qrs_end)
		free(b->qrs_end);
	
	if (b->t_start)
		free(b->t_start);
	if (b->t_end)
		free(b->t_end);
	
	if (b->p_width)
		free(b->p_width);
	if (b->qrs_width)
		free(b->qrs_width);
	if (b->t_width)
		free(b->t_width);
	
	if (b->pq)
		free(b->pq);
	if (b->qt)
		free(b->qt);
	if (b->qtc)
		free(b->qtc);
	if (b->qta)
		free(b->qta);

	if (b->resp_ch)
		free(b->resp_ch);
	if (b->resp_phase)
		free(b->resp_phase);
} /* free_one_beat_mem() */


int
comp_beats(const void* arg1, const void* arg2)
{
	struct beat_infos *b1 = (struct beat_infos *)arg1;
	struct beat_infos *b2 = (struct beat_infos *)arg2;

	return (b1->pos - b2->pos);
}  /* comp_beats() */


int
get_eval_values(struct proc_info *pi, long run, struct beat_infos **beats, long *num,
		long *ch, long num_ch, class_handle *clh_used)
{
	int ret = 0;
	struct ra_ecg_options *opt = pi->options;
	long l;

	if (opt->use_class)
		return get_data_from_class(pi, run, beats, num, ch, num_ch, clh_used);

	if (!opt->data || (opt->data_is_pos && !opt->annot))
	{
		_ra_set_error(pi->mh, RA_ERR_INFO_MISSING, "plugin-ecg");
		return -1;
	}

	*num = opt->num_data;
	*beats = (struct beat_infos *)calloc(*num, sizeof(struct beat_infos));
	
	if (opt->data_is_pos)
	{
		for (l = 0; l < *num; l++)
		{
			(*beats)[l].pos = opt->data[l];
			(*beats)[l].beat_class = opt->annot[l];
			(*beats)[l].event_id = -1;
			if (opt->flags)
				(*beats)[l].flags = opt->flags[l];
		}
	}
	else
		ret = get_single_values(pi, *beats, *num, ch, num_ch, clh_used);

	return ret;
} /* get_eval_values() */


int
get_single_values(struct proc_info *pi, struct beat_infos *beats, long num,
		  long *ch_map, long num_ch, class_handle *clh_used)
{
	struct ra_ecg_options *opt = pi->options;
	prop_handle pos, annot, ecg_flags;
	prop_handle morph_flags, p_start, p_end, qrs_start, qrs_end, t_start, t_end;
	long l, m;
	value_handle vh;

	if (opt->clh == NULL)
	{
		_ra_set_error(pi->mh, RA_ERR_INFO_MISSING, "plugin-ecg");
		return -1;
	}

	pos = ra_prop_get(opt->clh, "qrs-pos");
	annot = ra_prop_get(opt->clh, "qrs-annot");
	if (!pos || !annot)
	{
		_ra_set_error(opt->clh, RA_ERR_EV_PROP_MISSING, "plugin-ecg");
		return -1;
	}
	ecg_flags = ra_prop_get(opt->clh, "ecg-flags");

	morph_flags = ra_prop_get(opt->clh, "ecg-morph-flags");
	p_start = ra_prop_get(opt->clh, "ecg-p-start");
	p_end = ra_prop_get(opt->clh, "ecg-p-end");
	qrs_start = ra_prop_get(opt->clh, "ecg-qrs-start");
	qrs_end = ra_prop_get(opt->clh, "ecg-qrs-end");
	t_start = ra_prop_get(opt->clh, "ecg-t-start");
	t_end = ra_prop_get(opt->clh, "ecg-t-end");

	vh = ra_value_malloc();

	for (l = 0; l < num; l++)
	{
		long start_pos;

		ra_prop_get_value(pos, opt->data[l], -1, vh);
		beats[l].pos = ra_value_get_long(vh);
		ra_prop_get_value(annot, opt->data[l], -1, vh);
		beats[l].beat_class = ra_value_get_long(vh);

		if (ecg_flags)
		{
			if (ra_prop_get_value(ecg_flags, opt->data[l], -1, vh) == 0)
				beats[l].flags = ra_value_get_long(vh);
			else
				beats[l].flags = 0;
		}

		/* unmask beat-subclassifications; will be set in this module,
		   but only when beat-annotation was not set by the user */
		if (!(beats[l].flags & ECG_FLAG_USER_EDIT))
			beats[l].beat_class &= 0x0000ffff;

		beats[l].event_id = opt->data[l];
		ra_class_get_event_pos(opt->clh, beats[l].event_id, &start_pos, NULL);
		beats[l].event_pos = start_pos;

 		if (!morph_flags)
			continue;

		if (num_ch > 0)
		{
			beats[l].morph_flags = malloc(sizeof(long) * num_ch);
			beats[l].p_start = malloc(sizeof(long) * num_ch);
			beats[l].p_end = malloc(sizeof(long) * num_ch);
			beats[l].qrs_start = malloc(sizeof(long) * num_ch);
			beats[l].qrs_end = malloc(sizeof(long) * num_ch);
			beats[l].t_start = malloc(sizeof(long) * num_ch);
			beats[l].t_end = malloc(sizeof(long) * num_ch);
		}

		for (m = 0; m < num_ch; m++)
		{
			ra_prop_get_value(morph_flags, opt->data[l], ch_map[m], vh);
			beats[l].morph_flags[m] = ra_value_get_long(vh);

			ra_prop_get_value(p_start, opt->data[l], ch_map[m], vh);
			beats[l].p_start[m] = (long)(ra_value_get_short(vh));
			if (beats[l].p_start[m] != INVALID_OFFSET_VALUE)
				beats[l].p_start[m] += beats[l].event_pos;

			ra_prop_get_value(p_end, opt->data[l], ch_map[m], vh);
			beats[l].p_end[m] = (long)(ra_value_get_short(vh));
			if (beats[l].p_end[m] != INVALID_OFFSET_VALUE)
				beats[l].p_end[m] += beats[l].event_pos;

			ra_prop_get_value(qrs_start, opt->data[l], ch_map[m], vh);
			beats[l].qrs_start[m] = (long)(ra_value_get_short(vh));
			if (beats[l].qrs_start[m] != INVALID_OFFSET_VALUE)
				beats[l].qrs_start[m] += beats[l].event_pos;

			ra_prop_get_value(qrs_end, opt->data[l], ch_map[m], vh);
			beats[l].qrs_end[m] = (long)(ra_value_get_short(vh));
			if (beats[l].qrs_end[m] != INVALID_OFFSET_VALUE)
				beats[l].qrs_end[m] += beats[l].event_pos;

			ra_prop_get_value(t_start, opt->data[l], ch_map[m], vh);
			beats[l].t_start[m] = (long)(ra_value_get_short(vh));
			if (beats[l].t_start[m] != INVALID_OFFSET_VALUE)
				beats[l].t_start[m] += beats[l].event_pos;

			ra_prop_get_value(t_end, opt->data[l], ch_map[m], vh);
			beats[l].t_end[m] = (long)(ra_value_get_short(vh));
			if (beats[l].t_end[m] != INVALID_OFFSET_VALUE)
				beats[l].t_end[m] += beats[l].event_pos;
		}
	}

	*clh_used = opt->clh;

	ra_value_free(vh);

	return 0;
} /* get_single_values() */


int
get_ecg_channels(rec_handle rh, long **ch, long *num_ch, double **ch_samplerate)
{
	int ret = 0;
	value_handle vh;
	long l, n;

	*num_ch = 0;
	*ch = NULL;

	vh = ra_value_malloc();
	if ((ret = ra_info_get(rh, RA_INFO_REC_GEN_NUM_CHANNEL_L, vh)) != 0)
	{
		ra_value_free(vh);
		return ret;
	}

	n = ra_value_get_long(vh);
	*ch = malloc(sizeof(long) * n);
	*ch_samplerate = malloc(sizeof(double) * n);

	for (l = 0; l < n; l++)
	{
		ra_value_set_number(vh, l);
		if ((ret = ra_info_get(rh, RA_INFO_REC_CH_TYPE_L, vh)) != 0)
			break;
		if (ra_value_get_long(vh) == RA_CH_TYPE_ECG)
		{
			(*ch)[*num_ch] = l;

			ra_info_get(rh, RA_INFO_REC_CH_SAMPLERATE_D, vh);
			(*ch_samplerate)[*num_ch] = ra_value_get_double(vh);

			(*num_ch)++;
		}
	}

	if ((ret != 0) || (*num_ch <= 0))
	{
		free(*ch);
		*ch = NULL;
		free(*ch_samplerate);
		*ch_samplerate = NULL;
	}

	ra_value_free(vh);

	return ret;
} /* get_ecg_channels() */


int
sort_long(const void *v1, const void *v2)
{
	long l1, l2;
	l1 = *((long *)v1);
	l2 = *((long *)v2);

	return (int)(l1 - l2);
} /* sort_long() */


int
get_data_from_class(struct proc_info *pi, long run, struct beat_infos **beats, long *num,
		    long *ch, long num_ch, class_handle *clh_used)
{
	class_handle clh;
	prop_handle pos, annot, ecg_flags;
	value_handle morph_flags, p_start, p_end, qrs_start, qrs_end, t_start, t_end;
	long l, curr_pos, num_ch_elem, num_use;
	long *ch_to_idx = NULL;
	value_handle vh_id, vh_pos, vh_annot, vh_flags, vh;
	value_handle vh_morph_flags, vh_p_start, vh_p_end, vh_qrs_start, vh_qrs_end, vh_t_start, vh_t_end;
	value_handle vh_id_ch, vh_ch;
	const long *id_val, *pos_val, *annot_val, *flags_val, *morph_flags_val;
	const short *p_start_val, *p_end_val, *qrs_start_val, *qrs_end_val, *t_start_val, *t_end_val;
	const long *id_ch_val, *ch_val;
	struct ra_ecg_options *opt = pi->options;
	long *idx_found, num_idx_found, curr_ignore;
	int i;

	idx_found = NULL;
	num_idx_found = 0;
	num_ch_elem = 0;

	vh_morph_flags = vh_p_start = vh_p_end = vh_qrs_start = vh_qrs_end = vh_t_start = vh_t_end = NULL;
	vh_id_ch = vh_ch = NULL;

	id_val = pos_val = annot_val = flags_val = morph_flags_val = NULL;
	p_start_val = p_end_val = qrs_start_val = qrs_end_val = t_start_val = t_end_val = NULL;
	id_ch_val = ch_val = NULL;

	clh = get_event_class(pi, run);
	if (clh == NULL)
		return -1;
	
	pos = ra_prop_get(clh, "qrs-pos");
	annot = ra_prop_get(clh, "qrs-annot");
	if (!pos || !annot)
	{
		_ra_set_error(clh, RA_ERR_EV_PROP_MISSING, "plugin-ecg");
		return -1;
	}
	ecg_flags = ra_prop_get(clh, "ecg-flags");

	morph_flags = ra_prop_get(clh, "ecg-morph-flags");
	p_start = ra_prop_get(clh, "ecg-p-start");
	p_end = ra_prop_get(clh, "ecg-p-end");
	qrs_start = ra_prop_get(clh, "ecg-qrs-start");
	qrs_end = ra_prop_get(clh, "ecg-qrs-end");
	t_start = ra_prop_get(clh, "ecg-t-start");
	t_end = ra_prop_get(clh, "ecg-t-end");

	vh_id = ra_value_malloc();
	vh_pos = ra_value_malloc();
	vh_annot = ra_value_malloc();
	vh_flags = ra_value_malloc();

	ra_prop_get_value_all(pos, vh_id, NULL, vh_pos);
	ra_prop_get_value_all(annot, NULL, NULL, vh_annot);
	if (ecg_flags)
		ra_prop_get_value_all(ecg_flags, NULL, NULL, vh_flags);

	if (morph_flags)
	{
		vh_id_ch = ra_value_malloc();
		vh_ch = ra_value_malloc();
		vh_morph_flags = ra_value_malloc();
		vh_p_start = ra_value_malloc();
		vh_p_end = ra_value_malloc();
		vh_qrs_start = ra_value_malloc();
		vh_qrs_end = ra_value_malloc();
		vh_t_start = ra_value_malloc();
		vh_t_end = ra_value_malloc();

		ra_prop_get_value_all(morph_flags, vh_id_ch, vh_ch, vh_morph_flags);
		ra_prop_get_value_all(p_start, NULL, NULL, vh_p_start);
		ra_prop_get_value_all(p_end, NULL, NULL, vh_p_end);
		ra_prop_get_value_all(qrs_start, NULL, NULL, vh_qrs_start);
		ra_prop_get_value_all(qrs_end, NULL, NULL, vh_qrs_end);
		ra_prop_get_value_all(t_start, NULL, NULL, vh_t_start);
		ra_prop_get_value_all(t_end, NULL, NULL, vh_t_end);
	}

	*num = ra_value_get_num_elem(vh_pos);
	*beats = (struct beat_infos *)calloc(*num, sizeof(struct beat_infos));

	/* TODO: think how to be sure that they are long arrays */
	id_val = ra_value_get_long_array(vh_id);
	pos_val = ra_value_get_long_array(vh_pos);
	annot_val = ra_value_get_long_array(vh_annot);
	if (ecg_flags)
		flags_val = ra_value_get_long_array(vh_flags);

	if (morph_flags)
	{
		morph_flags_val = ra_value_get_long_array(vh_morph_flags);
		p_start_val = ra_value_get_short_array(vh_p_start);
		p_end_val = ra_value_get_short_array(vh_p_end);
		qrs_start_val = ra_value_get_short_array(vh_qrs_start);
		qrs_end_val = ra_value_get_short_array(vh_qrs_end);
		t_start_val = ra_value_get_short_array(vh_t_start);
		t_end_val = ra_value_get_short_array(vh_t_end);

		id_ch_val = ra_value_get_long_array(vh_id_ch);
		ch_val = ra_value_get_long_array(vh_ch);

		ch_to_idx = calloc(ch[num_ch-1]+1, sizeof(long));
		for (l = 0; l < num_ch; l++)
			ch_to_idx[ch[l]] = l;

		num_ch_elem = ra_value_get_num_elem(vh_morph_flags);
	}

	if (opt->num_ignore_events > 0)
	{
		get_intersection_long(id_val, *num, opt->ignore_events, opt->num_ignore_events,
				      &idx_found, &num_idx_found);
	}

	curr_pos = 0;
	num_use = -1;
	curr_ignore = 0;
	vh = ra_value_malloc();
	for (l = 0; l < *num; l++)
	{
		long start_pos;

		/* if we have to look for ignoring events (num_idx_found > 0), do it at the
		   beginning so we dont have to search in the idx_found array but can
		   only increment the idx_found-idx (idx_found is in ascending order) */
		if ((curr_ignore < num_idx_found) && (idx_found[curr_ignore] == l))
		{
			curr_ignore++;
			continue;
		}

		if (opt->use_start_end_pos && (pos_val[l] < opt->start_pos))
			continue;
		if (opt->use_start_end_pos && (pos_val[l] > opt->end_pos))
			break;

		num_use++;

		(*beats)[num_use].pos = pos_val[l];
		(*beats)[num_use].beat_class = annot_val[l];

		if (ecg_flags)
			(*beats)[num_use].flags = flags_val[l];

		/* unmask beat-subclassifications; will be set in this module,
		   but only when beat-annotation was not set by the user and
		   it is not a paced beat (because the origin of the stimulus
		   can only be set by the access-plugin) */
		if (!((*beats)[num_use].flags & ECG_FLAG_USER_EDIT) && !IS_PACED((*beats)[num_use].beat_class))
			(*beats)[num_use].beat_class &= 0x0000ffff;

		(*beats)[num_use].event_id = id_val[l];
		ra_class_get_event_pos(clh, id_val[l], &start_pos, NULL);
		(*beats)[num_use].event_pos = start_pos;

 		if (!morph_flags)
			continue;

		if (num_ch > 0)
		{
			(*beats)[num_use].morph_flags = calloc(num_ch, sizeof(long));
			(*beats)[num_use].p_start = calloc(num_ch, sizeof(long));
			(*beats)[num_use].p_end = calloc(num_ch, sizeof(long));
			(*beats)[num_use].qrs_start = calloc(num_ch, sizeof(long));
			(*beats)[num_use].qrs_end = calloc(num_ch, sizeof(long));
			(*beats)[num_use].t_start = calloc(num_ch, sizeof(long));
			(*beats)[num_use].t_end = calloc(num_ch, sizeof(long));
		}

		/* using values from ra_prop_get_value_all() is not working correctly,
		   therefor use single ra_prop_get_value() calls below */
		if (0)
		{
			/* when we ignored some beats or started not at the beginning, we have
			   to find the position in the ch-specific arrays */
			while ((curr_pos < num_ch_elem) && (id_val[l] != id_ch_val[curr_pos]))
				curr_pos++;

			while ((curr_pos < num_ch_elem) && (id_val[l] == id_ch_val[curr_pos]))
			{
				long ch_idx;
				
				if (ch_val[curr_pos] < 0)
				{
					curr_pos++;
					continue;
				}
				
				ch_idx = ch_to_idx[ch_val[curr_pos]];
				
				(*beats)[num_use].morph_flags[ch_idx] = morph_flags_val[curr_pos];
				
				(*beats)[num_use].p_start[ch_idx] = p_start_val[curr_pos] + (*beats)[num_use].event_pos;
				(*beats)[num_use].p_end[ch_idx] = p_end_val[curr_pos] + (*beats)[num_use].event_pos;
				
				(*beats)[num_use].qrs_start[ch_idx] = qrs_start_val[curr_pos] + (*beats)[num_use].event_pos;
				(*beats)[num_use].qrs_end[ch_idx] = qrs_end_val[curr_pos] + (*beats)[num_use].event_pos;
				
				(*beats)[num_use].t_start[ch_idx] = t_start_val[curr_pos] + (*beats)[num_use].event_pos;
				(*beats)[num_use].t_end[ch_idx] = t_end_val[curr_pos] + (*beats)[num_use].event_pos;
				
				curr_pos++;
			}
		}

		for (i = 0; i < num_ch; i++)
		{
			ra_prop_get_value(morph_flags, id_val[l], ch[i], vh);
			(*beats)[num_use].morph_flags[i] = ra_value_get_long(vh);

			ra_prop_get_value(p_start, id_val[l], ch[i], vh);
			(*beats)[num_use].p_start[i] = ra_value_get_short(vh);
			if ((*beats)[num_use].p_start[i] != INVALID_OFFSET_VALUE)
				(*beats)[num_use].p_start[i] += (*beats)[num_use].event_pos;

			ra_prop_get_value(p_end, id_val[l], ch[i], vh);
			(*beats)[num_use].p_end[i] = ra_value_get_short(vh);
			if ((*beats)[num_use].p_end[i] != INVALID_OFFSET_VALUE)
				(*beats)[num_use].p_end[i] += (*beats)[num_use].event_pos;

			ra_prop_get_value(qrs_start, id_val[l], ch[i], vh);
			(*beats)[num_use].qrs_start[i] = ra_value_get_short(vh);
			if ((*beats)[num_use].qrs_start[i] != INVALID_OFFSET_VALUE)
				(*beats)[num_use].qrs_start[i] += (*beats)[num_use].event_pos;

			ra_prop_get_value(qrs_end, id_val[l], ch[i], vh);
			(*beats)[num_use].qrs_end[i] = ra_value_get_short(vh);
			if ((*beats)[num_use].qrs_end[i] != INVALID_OFFSET_VALUE)
				(*beats)[num_use].qrs_end[i] += (*beats)[num_use].event_pos;

			ra_prop_get_value(t_start, id_val[l], ch[i], vh);
			(*beats)[num_use].t_start[i] = ra_value_get_short(vh);
			if ((*beats)[num_use].t_start[i] != INVALID_OFFSET_VALUE)
				(*beats)[num_use].t_start[i] += (*beats)[num_use].event_pos;

			ra_prop_get_value(t_end, id_val[l], ch[i], vh);
			(*beats)[num_use].t_end[i] = ra_value_get_short(vh);
			if ((*beats)[num_use].t_end[i] != INVALID_OFFSET_VALUE)
				(*beats)[num_use].t_end[i] += (*beats)[num_use].event_pos;
		}

	}
	ra_value_free(vh);
	*num = num_use + 1;
	*beats = (struct beat_infos *)realloc(*beats, sizeof(struct beat_infos) * (*num));

	if (idx_found)
		ra_free_mem(idx_found);

	ra_value_free(vh_id);
	ra_value_free(vh_pos);
	ra_value_free(vh_annot);
	ra_value_free(vh_flags);

	if (morph_flags)
	{
		ra_value_free(vh_morph_flags);
		ra_value_free(vh_p_start);
		ra_value_free(vh_p_end);
		ra_value_free(vh_qrs_start);
		ra_value_free(vh_qrs_end);
		ra_value_free(vh_t_start);
		ra_value_free(vh_t_end);

		ra_value_free(vh_id_ch);
		ra_value_free(vh_ch);

		if (ch_to_idx)
			free(ch_to_idx);
	}

	/* sort beats */
	if (*num > 0)
		qsort(*beats, *num, sizeof(struct beat_infos), comp_beats);

	*clh_used = clh;

	return 0;
} /* get_data_from_class() */


class_handle
get_event_class(struct proc_info *pi, long run)
{
	class_handle clh;
	struct ra_ecg_options *opt = pi->options;
	value_handle vh = NULL;

	clh = opt->clh;
	if (clh == NULL)
	{
		vh = ra_value_malloc();

		if (ra_class_get(opt->eh, "heartbeat", vh) != 0)
			goto error;

		if (run >= (long)ra_value_get_num_elem(vh))
		{
			_ra_set_error(pi->mh, RA_ERR_ERROR, "plugin-ecg");
			goto error;
		}
		
		clh = (class_handle)(ra_value_get_voidp_array(vh)[run]);

		if (clh == NULL)
		{
			_ra_set_error(pi->mh, RA_ERR_ERROR, "plugin-ecg");
			goto error;
		}	
	}

 error:
	if (vh)
		ra_value_free(vh);

	return clh;
} /* get_event_class() */


int
calc_rri(struct beat_infos *beats, long num, double samplerate)
{
	long l;
	double scale;

	if (num <= 0)
		return 0;

	beats[0].rri = 0;
	SET_CLASS(ECG_CLASS_ARTIFACT, beats[0].rri_class);

	scale = 1000.0 / samplerate;
	for (l = 1; l < num; l++)
	{
		/* rri as long value will be used for all the classification stuff because it is the
		   way it is done now in libRASCH (and I do not want to change it because some differences
		   in the calculations can happen)
		   TODO: check if the long value is really needed or if the double value can be used
		   for all the classification stuff */
		beats[l].rri_double = (double)(beats[l].pos - beats[l - 1].pos) * scale;
		beats[l].rri = (long)(beats[l].rri_double);
	}

	make_rri_classifications(beats, num);

	return 0;
} /* calc_rri() */


int
check_beats_on_same_pos(class_handle clh, struct beat_infos *beats, long *num, long min_beat_distance, long change_eval)
{
	long l;
	long n = *num;
	
	for (l = 1; l < n; l++)
	{
		while ((beats[l].rri <= min_beat_distance) && (l < n))
		{
			if (change_eval && clh && (beats[l].event_id != -1))
				ra_class_del_event(clh, beats[l].event_id);
			free_one_beat_mem(beats + l);
			if (l < (n-1))
				memmove(beats + l, beats + l + 1, (n - l - 1) * sizeof(struct beat_infos));
			beats[l].rri = beats[l].pos - beats[l - 1].pos;
			n--;
		}
	}
	*num = n;

	return 0;
} /* check_beats_on_same_pos() */


int _debug_flag = 0;
int
calc_temporal_setting(struct ra_ecg_options *opt, struct beat_infos *beats, long num, double samplerate)
{
	long l;
	int skip_next = 0;
	double rri_ref = -1.0;
	int num_ref_rri = 0;
	struct ref_value_infos ref_info;

	/* Even the reference-rri is only stored as long, here we use double because
	   this was done in the original implementation. Sometimes this change something. At
	   the moment I want to be as close on the original version as possible. Perhaps later I
	   will change something (if it makes sense). */
	/* init ref-value-calculation */
	memset(&ref_info, 0, sizeof(struct ref_value_infos));
	init_calc_ref(opt, &ref_info, samplerate);

	for (l = 0; l < num; l++)
	{
		if (skip_next)
		{
			skip_next = 0;
			continue;
		}

		beats[l].rri_ref = (int)rri_ref;
		beats[l].num_ref_rri = num_ref_rri;

		if (IS_SINUS(beats[l].beat_class))
			skip_next = temporal_normal(opt, beats, l, rri_ref);
		else if (IS_VENT(beats[l].beat_class))
			skip_next = temporal_vpc(opt, beats, l, rri_ref);
		else
			beats[l].temporal_group = ECG_TEMPORAL_NORMAL;

		/* calc ref-value (must be done here because the decision what
		   kind of rri the current is (normal, not normal) is based
		   on the current ref-value */
		calc_ref_value(&ref_info, &(beats[l]), &rri_ref, &num_ref_rri);
	}

	if (ref_info.buf)
		free(ref_info.buf);

	return 0;
} /* calc_temporal_setting() */


int
temporal_normal(struct ra_ecg_options *opt, struct beat_infos *beats, long curr_beat, double rri_ref)
{
	beats[curr_beat].temporal_group = ECG_TEMPORAL_NORMAL;

	if (rri_ref != -1)
	{
		double t;

		t = (double) rri_ref *(1.0 - (double) opt->normal_pre_percent / 100.0);
		if (beats[curr_beat].rri < (int) t)
			beats[curr_beat].temporal_group = ECG_TEMPORAL_PRE;
		else
		{
			t = (double) rri_ref *(1.0 + (double) opt->normal_late_percent / 100.0);
			if (beats[curr_beat].rri > (int) t)
				beats[curr_beat].temporal_group = ECG_TEMPORAL_LATE;
		}
	}

	/* now check if the distance to the previous beat is above +-max_abs_difference;
	   previous beat must be normal (classification and timing) */
	if ((curr_beat > 1) && IS_SINUS(beats[curr_beat - 1].rri_class) && (beats[curr_beat-1].temporal_group == ECG_TEMPORAL_NORMAL))
	{
		if (beats[curr_beat].rri < (beats[curr_beat - 1].rri - opt->max_abs_difference))
			beats[curr_beat].temporal_group |= ECG_ABS_PRE;

		if (beats[curr_beat].rri > (beats[curr_beat - 1].rri + opt->max_abs_difference))
			beats[curr_beat].temporal_group |= ECG_ABS_LATE;

	}

	return 0;		/* 0: do not skip next beat */
} /* temporal_normal() */


int
temporal_vpc(struct ra_ecg_options *opt, struct beat_infos *beats, long curr_beat, double rri_ref)
{
	int skip = 0;

	beats[curr_beat].temporal_group = ECG_TEMPORAL_NORMAL;

	if (rri_ref != -1)
	{
		double t;

		t = (double) rri_ref *(1.0 - (double) opt->vpc_pre_percent / 100.0);
		if (beats[curr_beat].rri < (int) t)
			beats[curr_beat].temporal_group = ECG_TEMPORAL_PRE;
		else
		{
			t = (double) rri_ref *(1.0 + (double) opt->vpc_late_percent / 100.0);
			if (beats[curr_beat].rri > (int) t)
				beats[curr_beat].temporal_group = ECG_TEMPORAL_LATE;
		}

		/* process normal beat after vpc here */
		if (IS_SINUS(beats[curr_beat + 1].beat_class))
		{
			t = (double) rri_ref *(1.0 + (double) opt->vpc_late_percent / 100.0);
			if (beats[curr_beat + 1].rri > (int) t)
				beats[curr_beat + 1].temporal_group = ECG_TEMPORAL_LATE;
			else
				beats[curr_beat + 1].temporal_group = ECG_TEMPORAL_NORMAL;
			
			/* set reference-rri information of next beat here because this will be skiped */
			beats[curr_beat+1].rri_ref = beats[curr_beat].rri_ref;
			beats[curr_beat+1].num_ref_rri = beats[curr_beat].num_ref_rri;

			skip = 1;	/* skip next beat */
		}
	}

	return skip;
} /* temporal_vpc() */


int
init_calc_ref(struct ra_ecg_options *opt, struct ref_value_infos *ref_info, double samplerate)
{
	if (ref_info->buf)
	{
		free(ref_info->buf);
		ref_info->buf = NULL;
	}
	ref_info->buf = malloc(sizeof(double) * opt->num_ref_rri);
	ref_info->last_beatpos_ok = 0;
	ref_info->num_buffer_rri_ok = 0;

	ref_info->opt = opt;

	ref_info->samplerate = samplerate;

	return 0;
} /* init_calc_ref() */


/**
 * calc_ref_value - calculate reference rr-interval
 * @beats: pointer to current beat-info
 * @opt: pointer to option-infos
 * @rri_ref: store reference rr-interval here
 * @num_ref_rri: store number of rri to calculate ref_rri here
 *
 */
int
calc_ref_value(struct ref_value_infos *inf, struct beat_infos *beat, double *rri_ref, int *num_ref_rri)
{
	*num_ref_rri = inf->num_buffer_rri_ok;

	if (IS_SINUS(beat->rri_class) && (beat->temporal_group & ECG_TEMPORAL_NORMAL))
		calc_ref_value_new(inf, beat, rri_ref, num_ref_rri);
	else
	{
		long sam;

		/* check if previous ref_value can be used or the previous is
		   to "old" to be used */
		sam = (long) ((double) inf->opt->time2get_new_ref_value * inf->samplerate);
		if ((beat->pos - inf->last_beatpos_ok) >= sam)
		{
			inf->curr_buf_pos = 0;
			inf->num_buffer_rri_ok = 0;
			inf->last_ref_value = -1;
			*rri_ref = -1;
			*num_ref_rri = 0;
		}
		else
		{
			double sum = 0.0;
			int i;
			
			for (i = 0; i < inf->num_buffer_rri_ok; i++)
				sum += inf->buf[i];
			if (inf->num_buffer_rri_ok > 0)
				*rri_ref = (sum / (double)inf->num_buffer_rri_ok);
			inf->last_ref_value = (int)(*rri_ref);

			*num_ref_rri = inf->num_buffer_rri_ok;
		}
	}

	return 0;
} /* calc_ref_value() */


int
calc_ref_value_new(struct ref_value_infos *inf, struct beat_infos *beat, double *rri_ref, int *num_ref_rri)
{
	/* store current rri in buffer */
	inf->last_beatpos_ok = beat->pos;
	inf->buf[inf->curr_buf_pos] = beat->rri;
	inf->curr_buf_pos = (inf->curr_buf_pos + 1) % inf->opt->num_ref_rri;

	(*num_ref_rri)++;
	if (*num_ref_rri >= inf->opt->num_ref_rri)
	{
		double percent;

		*num_ref_rri = inf->opt->num_ref_rri;
		inf->num_buffer_rri_ok = *num_ref_rri;

		percent = fabs(100.0 - ((double) beat->rri / (double) *rri_ref) * 100.0);

		/* check if current rri */
		if (percent <= inf->opt->ref_normal_percent)
		{
			*rri_ref = beat->rri;
			*num_ref_rri = 0;	/* in combination with a valid
						   ref_rri indicate that the current
						   ref_rri is the last normal-rri */
		}
		else
		{
			double sum = 0.0;
			int i;

			for (i = 0; i < *num_ref_rri; i++)
				sum += inf->buf[i];
			*rri_ref = (sum / (double) *num_ref_rri);
		}
	}
	else
	{
		/* not enough rri to calc ref-value -> return -1 as ref-value */
		inf->num_buffer_rri_ok = *num_ref_rri;
		*rri_ref = -1;
	}

	inf->last_ref_value = (int)(*rri_ref);

	return 0;
} /* calc_ref_value_new() */


int
subclass_beats(struct ra_ecg_options *opt, struct beat_infos *beats, long num)
{
	int ret;

	if ((ret = subclass_normals(opt, beats, num)) != 0)
		return ret;

	if ((ret = subclass_vpcs(opt, beats, num)) != 0)
		return ret;

	return 0;
} /* subclass_beats() */


int
subclass_normals(struct ra_ecg_options *opt, struct beat_infos *beats, long num)
{
	long l;
	int ret = 0;

	/* find SVPCs */
	for (l = 1; l < (num-1); l++)
	{
		if (!IS_SINUS(beats[l].rri_class) || (beats[l].flags & ECG_FLAG_USER_EDIT))
			continue;

		if (beats[l].temporal_group & ECG_TEMPORAL_PRE)
			SET_SUBCLASS(ECG_CLASS_SINUS_SVPC, beats[l].beat_class);
		else if ((beats[l].temporal_group & ECG_TEMPORAL_LATE) && (beats[l].rri > opt->pause))
			SET_SUBCLASS(ECG_CLASS_SINUS_PAUSE, beats[l].beat_class);
		else
			SET_SUBCLASS(ECG_CLASS_SINUS_NORMAL, beats[l].beat_class);
	}

	return ret;
} /* subclass_normals() */


int
subclass_vpcs(struct ra_ecg_options *opt, struct beat_infos *beats, long num)
{
	long l;
	int ret = 0;

	/* find premature, single VPCs and escape beats */
	for (l = 1; l < (num - 1); l++)
	{
		/* Up to now there is no need to check for user-edited beat classsification
		   as this is not critical for ventricular extra-beats; it is even
		   wanted. */
		if (!IS_VENT(beats[l].beat_class))
			continue;

		/* check if escape beat */
		if (beats[l].temporal_group & ECG_TEMPORAL_LATE)
		{
			SET_SUBCLASS(ECG_CLASS_VENT_ESCAPE, beats[l].beat_class);
			continue;
		}

		/* single vpc's need further checks */
		if (IS_SINUS(beats[l - 1].beat_class) && IS_SINUS(beats[l + 1].beat_class))
			subclass_single_vpc(opt, beats, num, l);
	}

	return ret;
} /* subclass_vpcs() */


void
subclass_single_vpc(struct ra_ecg_options *opt, struct beat_infos *beats, long num, long curr)
{
	int min, max;

	if (curr >= (num - 1))
		return;		/* we need to access the next beat also */


	/* this is at least a single VPC */
	SET_SUBCLASS(ECG_CLASS_VENT_SINGLE, beats[curr].beat_class);

	/* is vpc premature */
	if (!(beats[curr].temporal_group & ECG_TEMPORAL_PRE))
	{
		/* no, no further checks are necessary */
		return;
	}

	/* check if interpolated */
	min = (int) (((double) (opt->min_interp_vpc_percent) / 100.0) * beats[curr].rri_ref);
	max = (int) (((double) (opt->max_interp_vpc_percent) / 100.0) * beats[curr].rri_ref);

	if ((beats[curr].rri >= min) && (beats[curr].rri <= max)
	    && (beats[curr + 1].temporal_group == ECG_TEMPORAL_PRE))
	{
		SET_SUBCLASS(ECG_CLASS_VENT_INTERP, beats[curr].beat_class);
	}
	else
	{
		/* VPC is at least premature */
		SET_SUBCLASS(ECG_CLASS_VENT_SINGLE_PREM, beats[curr].beat_class);
		if (beats[curr + 1].temporal_group == ECG_TEMPORAL_LATE)
		{
			/* and has a compensatory pause */
			SET_SUBCLASS(ECG_CLASS_VENT_SINGLE_PREM_COMP, beats[curr].beat_class);
		}
	}
} /* subclass_single_vpc() */


int
filter_artifacts(struct ra_ecg_options *opt, struct beat_infos *beats, long num)
{
	long l;

	/* check for false positive VPCs between two normal beats
	   (e.g. T-waves or artifacts sensed as QRS-complexes) */
	for (l = 2; l < (num - 1); l++)
	{
		long temp_rri;
		double percent;
		double i_min, i_max;

		if (beats[l].flags & ECG_FLAG_USER_EDIT)
			continue;

		/* check if sum of two rri's is equal to the current reference rri */
		temp_rri = beats[l].rri + beats[l+1].rri;
		percent = fabs(100.0 - ((double)temp_rri / (double)beats[l].rri_ref) * 100.0);
		if (percent > opt->normal_percent)
			continue; /* sum is not in the range of the ref.rri */

		/* is the current beat a vent. beat and is this beat surrounded 
		   by normal beats (no SVPCs) */
		if (!IS_VENT(beats[l].beat_class) || !IS_SINUS(beats[l-1].beat_class)
		    || !IS_SINUS(beats[l+1].beat_class) || (beats[l-1].temporal_group & ECG_TEMPORAL_PRE))
		{
			continue; /* no, next beat */
		}

		/* is the beat inside the range of interponated VPCs */
		i_min = (double)beats[l].rri_ref * ((double)opt->min_interp_vpc_percent / 100.0);
		i_max = (double)beats[l].rri_ref * ((double)opt->max_interp_vpc_percent / 100.0);
		if ((beats[l].rri >= i_min) && (beats[l].rri <= i_max))
			continue; /* yes, probably a correct VPC, next beat */

		/* beat seems to be a false positive VPC (perhaps T-wave or artifact was sensed) */
		SET_CLASS(ECG_CLASS_ARTIFACT, beats[l].beat_class);
		SET_SUBCLASS(ECG_CLASS_ARTIFACT_TYPE2, beats[l].beat_class);

		SET_CLASS(ECG_CLASS_ARTIFACT, beats[l].rri_class);
		SET_SUBCLASS(ECG_CLASS_ARTIFACT_TYPE2, beats[l].rri_class);

		SET_CLASS(ECG_CLASS_ARTIFACT, beats[l+1].rri_class);
		SET_SUBCLASS(ECG_CLASS_ARTIFACT_TYPE2, beats[l+1].rri_class);
	}

	/* check if rr-interval is below the allowed minimum */
	for (l = 1; l < (num - 1); l++)
	{
		if (beats[l].flags & ECG_FLAG_USER_EDIT)
			continue;

		/* don't care if the previous beat is an artifact */
		if ((beats[l].rri < opt->min_rri) && !IS_ARTIFACT(beats[l-1].beat_class))
		{
			SET_CLASS(ECG_CLASS_ARTIFACT, beats[l].beat_class);
			SET_SUBCLASS(ECG_CLASS_ARTIFACT_TYPE1, beats[l].beat_class);
			
			SET_CLASS(ECG_CLASS_ARTIFACT, beats[l].rri_class);
			SET_SUBCLASS(ECG_CLASS_ARTIFACT_TYPE1, beats[l].rri_class);
			
			SET_CLASS(ECG_CLASS_ARTIFACT, beats[l+1].rri_class);
			SET_SUBCLASS(ECG_CLASS_ARTIFACT_TYPE1, beats[l+1].rri_class);
		}
	}

	/* check if after a VPC a QRS-complex was overlooked (but only when we have
	   more than 3 beats so the RR intervals we compare have sane values) */
	if (num > 3)
	{
		for (l = 2; l < num; l++)
		{
			long temp_rri;
			
			if (beats[l].flags & ECG_FLAG_USER_EDIT)
				continue;
			
			if (!IS_VENT(beats[l].rri_class))
				continue;
			
			temp_rri = beats[l-1].rri + beats[l-2].rri;
			if (beats[l].rri > (long)(1.5 * (double)temp_rri))
			{
				/* we found one */
				SET_CLASS(ECG_CLASS_ARTIFACT, beats[l].rri_class);
				SET_SUBCLASS(ECG_CLASS_ARTIFACT_TYPE3, beats[l].rri_class);
				SET_CLASS(ECG_CLASS_ARTIFACT, beats[l].beat_class);
				SET_SUBCLASS(ECG_CLASS_ARTIFACT_TYPE3, beats[l].beat_class);
			}
		}
	}

	return 0;
} /* filter_artifacts() */


int
make_rri_classifications(struct beat_infos *beats, long num)
{
	long l;
	int c1, c2;
	int sub_save, sub;

	for (l = 1; l < num; l++)
	{
		c1 = beats[l - 1].beat_class;
		c2 = beats[l].beat_class;

		/* save sub-classification
		   (TODO: sub-class is maybe not from the artifact, add code
		   to be sure the sub-class is the correct one) */
		sub_save = GET_SUBCLASS(beats[l].beat_class);

		if (IS_ARTIFACT(c1) || IS_ARTIFACT(c2))
		{
			SET_CLASS(ECG_CLASS_ARTIFACT, beats[l].rri_class);
			/* keep type of artifact */
			if (GET_SUBCLASS(c1) != 0)
				SET_SUBCLASS(GET_SUBCLASS(c1), beats[l].rri_class);
			else if (GET_SUBCLASS(c2) != 0)
				SET_SUBCLASS(GET_SUBCLASS(c2), beats[l].rri_class);
		}
		else if (IS_PACED(c1) || IS_PACED(c2))
			SET_CLASS(ECG_CLASS_PACED, beats[l].rri_class);
		else if (IS_VENT(c1) || IS_VENT(c2))
			SET_CLASS(ECG_CLASS_VENT, beats[l].rri_class);
		else if (IS_SVPC(c1) || IS_SVPC(c2))
		{
			SET_CLASS(ECG_CLASS_SINUS, beats[l].rri_class);
			SET_SUBCLASS(ECG_CLASS_SINUS_SVPC, beats[l].rri_class);
		}
		else
			SET_CLASS(ECG_CLASS_SINUS, beats[l].rri_class);

		/* if sub-classification was not set, use beat sub-classification */
		sub = GET_SUBCLASS(beats[l].rri_class);
		if (sub == 0)
			SET_SUBCLASS(sub_save, beats[l].rri_class);			
	}

	return 0;
} /* make_rri_classifications() */


int
calc_morphology_values(struct beat_infos *beats, long num_beats, long num_ch, double *ch_samplerate)
{
	int i;
	long l;
	double *ch_max_scale;
	double max_samplerate, max_to_msec;
	double min_p_start, max_p_end, min_qrs_start, max_qrs_end, min_t_start, max_t_end;
	
	if (beats[0].morph_flags == NULL)
		return 0;

	max_samplerate = ch_samplerate[0];
	for (i = 1; i < num_ch; i++)
	{
		if (ch_samplerate[i] > max_samplerate)
			max_samplerate = ch_samplerate[i];
	}
	max_to_msec = 1000.0 / max_samplerate;

	ch_max_scale = malloc(sizeof(double) * num_ch);
	for (i = 0; i < num_ch; i++)
		ch_max_scale[i] = max_samplerate / ch_samplerate[i];

	for (l = 0; l < num_beats; l++)
	{
		min_p_start = max_p_end = min_qrs_start = max_qrs_end = min_t_start = max_t_end = -1;

		beats[l].p_width = calloc(num_ch, sizeof(double));
		beats[l].qrs_width = calloc(num_ch, sizeof(double));
		beats[l].t_width = calloc(num_ch, sizeof(double));
		beats[l].pq = calloc(num_ch, sizeof(double));
		beats[l].qt = calloc(num_ch, sizeof(double));
		beats[l].qtc = calloc(num_ch, sizeof(double));
		beats[l].qta = calloc(num_ch, sizeof(double));
		
		for (i = 0; i < num_ch; i++)
		{
			double sp, ep, sq, eq, st, et;

			sp = ep = sq = eq = st = et = -1;

			if (beats[l].morph_flags[i] & ECG_P_WAVE_OK)
			{
				if (beats[l].p_start[i] != INVALID_OFFSET_VALUE)
				{
					sp = beats[l].p_start[i] * ch_max_scale[i];
					if ((min_p_start == -1) || (sp < min_p_start))
						min_p_start = sp;
				}

				if (beats[l].p_end[i] != INVALID_OFFSET_VALUE)
				{
					ep = beats[l].p_end[i] * ch_max_scale[i];
					if (ep > max_p_end)
						max_p_end = ep;
				}

				if (sp != ep)
					beats[l].p_width[i] = (ep - sp) * max_to_msec;
			}

			if (beats[l].morph_flags[i] & ECG_QRS_OK)
			{
				if (beats[l].qrs_start[i] != INVALID_OFFSET_VALUE)
				{
					sq = beats[l].qrs_start[i] * ch_max_scale[i];
					if ((min_qrs_start == -1) || (sq < min_qrs_start))
						min_qrs_start = sq;
				}

				if (beats[l].qrs_end[i] != INVALID_OFFSET_VALUE)
				{
					eq = beats[l].qrs_end[i] * ch_max_scale[i];
					if (eq > max_qrs_end)
						max_qrs_end = eq;
				}

				if (sq != eq)
					beats[l].qrs_width[i] = (eq - sq) * max_to_msec;
			}

			if (beats[l].morph_flags[i] & ECG_T_WAVE_OK)
			{
				if (beats[l].t_start[i] != INVALID_OFFSET_VALUE)
				{
					st = beats[l].t_start[i] * ch_max_scale[i];
					if ((min_t_start == -1) || (st < min_t_start))
						min_t_start = st;
				}

				if (beats[l].t_end[i] != INVALID_OFFSET_VALUE)
				{
					et = beats[l].t_end[i] * ch_max_scale[i];
					if (et > max_t_end)
						max_t_end = et;
				}

				if (st != et)
					beats[l].t_width[i] = (et - st) * max_to_msec;
			}
			
			if (sp != sq)
				beats[l].pq[i] = (sq - sp) * max_to_msec;
			else
				beats[l].pq[i] = -1;
			if (sq != et)
			{
				beats[l].qt[i] = (et - sq) * max_to_msec;
				beats[l].qtc[i] = (et - sq) * max_to_msec;
				beats[l].qta[i] = -1;
			}
			else
				beats[l].qt[i] = beats[l].qtc[i] = beats[l].qta[i] = -1;
		}
		
		if ((min_p_start != -1) && (max_p_end != -1))
			beats[l].p_width_all = (max_p_end - min_p_start) * max_to_msec;
		else
			beats[l].p_width_all = -1;

		if ((min_p_start != -1) && (min_qrs_start != 1))
			beats[l].pq_all = (min_qrs_start - min_p_start) * max_to_msec;
		else
			beats[l].pq_all = -1;

		if ((min_qrs_start != -1) && (max_qrs_end != -1))
			beats[l].qrs_width_all = (max_qrs_end - min_qrs_start) * max_to_msec;
		else
			beats[l].qrs_width_all = -1;

		if ((min_t_start != -1) && (max_t_end != -1))
			beats[l].t_width_all = (max_t_end - min_t_start) * max_to_msec;
		else
			beats[l].t_width_all = -1;

		if ((min_qrs_start != -1) && (max_t_end != -1))
		{
			beats[l].qt_all = (max_t_end - min_qrs_start) * max_to_msec;
			beats[l].qtc_all = (max_t_end - min_qrs_start) * max_to_msec;
			beats[l].qta_all = -1;
		}
		else
			beats[l].qt_all = beats[l].qtc_all = beats[l].qta_all = -1;
	}

	free(ch_max_scale);

	return 0;
} /* calc_morphology_values() */


int
make_summary(struct ra_ecg_options *opt, long run, struct beat_infos *beats, long num, value_handle *res)
{
	int ret = 0;

	ret = make_beat_summary(run, beats, num, res);
	if (ret != 0)
		return ret;

	ret = make_arrhythmia_summary(opt, run, beats, num, res);

	return ret;
} /* make_summary() */


int
make_beat_summary(long run, struct beat_infos *beats, long num, value_handle *res)
{
	long res_offset = run * num_results;

	ra_value_set_long(res[NUM_ALL+res_offset], num);
	
	get_beat_count(ECG_CLASS_UNKNOWN, -1, beats, num, res,
		       NUM_UNKNOWN+res_offset, IDX_UNKNOWN+res_offset);

	get_beat_count(ECG_CLASS_SINUS, -1, beats, num, res,
		       NUM_SINUS+res_offset, IDX_SINUS+res_offset);
	get_beat_count(ECG_CLASS_SINUS, ECG_CLASS_SINUS_NORMAL, beats, num, res,
		       NUM_NORMAL+res_offset, IDX_NORMAL+res_offset);
	get_beat_count(ECG_CLASS_SINUS, ECG_CLASS_SINUS_SVPC, beats, num, res,
		       NUM_SVPC+res_offset, IDX_SVPC+res_offset);
	get_beat_count(ECG_CLASS_SINUS, ECG_CLASS_SINUS_VPC, beats, num, res,
		       NUM_SINUS_VPC+res_offset, IDX_SINUS_VPC+res_offset);
	get_beat_count(ECG_CLASS_SINUS, ECG_CLASS_SINUS_PAUSE, beats, num, res,
		       NUM_PAUSE+res_offset, IDX_PAUSE+res_offset);

	get_beat_count(ECG_CLASS_VENT, -1, beats, num, res, NUM_VENT+res_offset, IDX_VENT+res_offset);
	get_beat_count(ECG_CLASS_VENT, ECG_CLASS_VENT_SINGLE, beats, num, res,
		       NUM_VENT_SINGLE+res_offset, IDX_VENT_SINGLE+res_offset);
	get_beat_count(ECG_CLASS_VENT, ECG_CLASS_VENT_SINGLE_PREM, beats, num, res,
		       NUM_VENT_SINGLE_PREM+res_offset, IDX_VENT_SINGLE_PREM+res_offset);
	get_beat_count(ECG_CLASS_VENT, ECG_CLASS_VENT_SINGLE_PREM_COMP, beats, num, res,
		       NUM_VENT_SINGLE_PREM_COMP+res_offset, IDX_VENT_SINGLE_PREM_COMP+res_offset);
	get_beat_count(ECG_CLASS_VENT, ECG_CLASS_VENT_INTERP, beats, num, res,
		       NUM_INTERP+res_offset, IDX_INTERP+res_offset);
	get_beat_count(ECG_CLASS_VENT, ECG_CLASS_VENT_ESCAPE, beats, num, res,
		       NUM_ESCAPE+res_offset, IDX_ESCAPE+res_offset);

	get_beat_count(ECG_CLASS_PACED, -1, beats, num, res,
		       NUM_PACED_BEATS+res_offset, IDX_PACED_BEATS+res_offset);

	get_beat_count(ECG_CLASS_ARTIFACT, -1, beats, num, res,
		       NUM_ARTIFACT+res_offset, IDX_ARTIFACT+res_offset);
	get_beat_count(ECG_CLASS_ARTIFACT, ECG_CLASS_ARTIFACT_TYPE1, beats, num, res,
		       NUM_ARTIFACT_TYPE1+res_offset, IDX_ARTIFACT_TYPE1+res_offset);
	get_beat_count(ECG_CLASS_ARTIFACT, ECG_CLASS_ARTIFACT_TYPE2, beats, num, res,
		       NUM_ARTIFACT_TYPE2+res_offset, IDX_ARTIFACT_TYPE2+res_offset);
	get_beat_count(ECG_CLASS_ARTIFACT, ECG_CLASS_ARTIFACT_TYPE3, beats, num, res,
		       NUM_ARTIFACT_TYPE3+res_offset, IDX_ARTIFACT_TYPE3+res_offset);

	return 0;
} /* make_beat_summary() */


int
get_beat_count(long base_cl, long sub_cl, struct beat_infos *beats, long num, value_handle *res,
	       long res_idx_num, long res_idx_idx)
{
	long l;
	long n = 0;
	long *idx = NULL;

	for (l = 0; l < num; l++)
	{
		if ((base_cl == GET_CLASS(beats[l].beat_class))
		    && ((sub_cl == -1) || (sub_cl & GET_SUBCLASS(beats[l].beat_class))))
		{
			n++;
			idx = realloc(idx, sizeof(long) * n);
			idx[n-1] = l;
		}
	}

	ra_value_set_long(res[res_idx_num], n);
	ra_value_set_long_array(res[res_idx_idx], idx, n);
	free(idx);

	return 0;
} /* get_beat_count() */


int
make_arrhythmia_summary(struct ra_ecg_options *opt, long run, struct beat_infos *beats, long num, value_handle *res)
{
	int ret = 0;

	ret = get_hr_values(opt, run, beats, num, res);
	if (ret != 0)
		return ret;

	ret = get_sinus_related_values(run, beats, num, res);
	if (ret != 0)
		return ret;

	ret = get_ventricular_related_values(run, beats, num, res);
	if (ret != 0)
		return ret;

	return ret;
} /* make_arrhythmia_summary() */


int
get_hr_values(struct ra_ecg_options *opt, long run, struct beat_infos *beats, long num, value_handle *res)
{
	double mean_hr = 0.0;
	long mean_hr_cnt = 0;
	double min_hr = 200.0;
	long min_pos = -1;
	double max_hr = 0.0;
	long max_pos = -1;
	long start = -1;
	long time = 0;
	long cnt = 0;
	long l, res_offset;

	res_offset = run * num_results;

	for (l = 1; l < num; l++)
	{
		if ((IS_NORMAL(beats[l].rri_class)) && (beats[l].rri < opt->max_rri))
		{
			if (start == -1)
				start = l;
			time += beats[l].rri;
			cnt++;
			
			mean_hr += (double)beats[l].rri;
			mean_hr_cnt++;

			if ((time / 1000) >= HR_LENGTH_SECONDS)
			{
				double curr_hr;
				
				curr_hr = 60000.0 / ((double)time/(double)cnt);
				if (curr_hr < min_hr)
				{
					min_hr = curr_hr;
					min_pos = start;
				}
				if (curr_hr > max_hr)
				{
					max_hr = curr_hr;
					max_pos = start;
				}
				time -= beats[start].rri;
				start++;
				cnt--;
			}
		}
		else
		{
			start = -1;
			time = 0;
			cnt = 0;
		}
	}

	ra_value_set_double(res[MIN_HR+res_offset], min_hr);
	ra_value_set_long(res[IDX_MIN_HR+res_offset], min_pos);
	ra_value_set_double(res[MAX_HR+res_offset], max_hr);
	ra_value_set_long(res[IDX_MAX_HR+res_offset], max_pos);

	mean_hr /= (double)mean_hr_cnt;
	mean_hr = 60000.0 / mean_hr;
	ra_value_set_double(res[MEAN_HR+res_offset], mean_hr);

	return 0;
} /* get_hr_values() */


int
get_sinus_related_values(long run, struct beat_infos *beats, long num, value_handle *res)
{
	int ret = 0;

	if ((ret = get_brady_tachy(run, beats, num, res)) != 0)
		return ret;

	if ((ret = get_sinus_salvos(run, beats, num, res)) != 0)
		return ret;

	return 0;
} /* get_sinus_related_values() */


int
get_brady_tachy(long run, struct beat_infos *beats, long num, value_handle *res)
{
	if (run || beats || num || res)
		;

	return 0;

	/* TODO: implement the code */

/* 	long num_brady = 0; */
/* 	struct arr_infos *brady = NULL; */
/* 	long num_tachy = 0; */
/* 	struct arr_infos *tachy = NULL; */

/* 	int cnt = 0; */
/* 	double rri = 0; */
/* 	long start_brady = 0; */
/* 	long start_tachy = 0; */
/* 	long l, res_offset; */

/* 	double rri_buf[NUM_RRI_4_BRADY_TACHY]; */
/* 	int curr_buf_pos = 0; */

/* 	res_offset = run * num_results; */

/* 	for (l = 0; l < num; l++) */
/* 	{ */
/* 		if (!IS_SINUS(beats[l].rri_class)) */
/* 		{ */
/* 			cnt = 0; */
/* 			curr_buf_pos = 0; */
/* 		} */

/* 		rri_buf[curr_buf_pos % NUM_RRI_4_BRADY_TACHY] = beats[l].rri; */
/* 		curr_buf_pos++; */
/* 		cnt++; */

/* 		if (cnt >= NUM_RRI_4_BRADY_TACHY) */
/* 		{ */
/* 			double hr; */
			
/* 			for (l = 0; l < NUM_RRI_4_BRADY_TACHY; l++) */
/* 				rri += rri_buf[l]; */
			
/* 			hr = 60000.0 / (rri/(double)NUM_RRI_4_BRADY_TACHY); */
/* 			if (hr < TACHY_HR) */
/* 			{ */
/* 				num_ssalvo++; */
/* 				ssalvo = realloc(ssalvo, sizeof(struct arr_infos) * num_ssalvo); */
/* 				ssalvo[num_ssalvo-1].hr = hr; */
/* 				ssalvo[num_ssalvo-1].start = start; */
/* 				ssalvo[num_ssalvo-1].length = l - start; */
/* 			} */
/* 			else */
/* 			{ */
/* 				num_svt++; */
/* 				svt = realloc(svt, sizeof(struct arr_infos) * num_svt); */
/* 				svt[num_svt-1].hr = hr; */
/* 				svt[num_svt-1].start = start; */
/* 				svt[num_svt-1].length = l - start; */
/* 			} */
			
/* 			num_ssalvo_svt++; */
/* 			ssalvo_svt = realloc(ssalvo_svt, sizeof(struct arr_infos) * num_ssalvo_svt); */
/* 			ssalvo_svt[num_ssalvo_svt-1].hr = hr; */
/* 			ssalvo_svt[num_ssalvo_svt-1].start = start; */
/* 			ssalvo_svt[num_ssalvo_svt-1].length = l - start; */
/* 		} */
/* 		cnt = 0; */
/* 		rri = 0; */
/* 		start = 0; */
/* 	} */

/* 	save_arr_info(num_ssalvo, ssalvo, res, NUM_SSALVO+res_offset, IDX_SSALVO+res_offset, */
/* 		      LEN_SSALVO+res_offset, HR_SSALVO+res_offset); */
/* 	save_arr_info(num_svt, svt, res, NUM_SVT+res_offset, IDX_SVT+res_offset, */
/* 		      LEN_SVT+res_offset, HR_SVT+res_offset); */
/* 	save_arr_info(num_ssalvo_svt, ssalvo_svt, res, NUM_SSALVO_SVT+res_offset, IDX_SSALVO_SVT+res_offset, */
/* 		      LEN_SSALVO_SVT+res_offset, HR_SSALVO_SVT+res_offset); */

/* 	free(ssalvo); */
/* 	free(svt); */
/* 	free(ssalvo_svt); */

/* 	return 0; */
} /* get_brady_tachy() */


int
get_sinus_salvos(long run, struct beat_infos *beats, long num, value_handle *res)
{
	long num_ssalvo = 0;
	struct arr_infos *ssalvo = NULL;
	long num_svt = 0;
	struct arr_infos *svt = NULL;
	long num_ssalvo_svt = 0;
	struct arr_infos *ssalvo_svt = NULL;

	int cnt = 0;
	long rri = 0;
	long start = 0;
	long l, res_offset;

	res_offset = run * num_results;

	for (l = 0; l < num; l++)
	{
		int skip = 0;

		if (IS_SVPC(beats[l].beat_class))
		{
			rri += beats[l].rri;
			cnt++;
			if (start == 0)
				start = l;
			skip = 1;
		}

		if (skip && (l < (num-1)))
			continue;

		if (cnt >= MIN_BEATS_SSALVO)
		{
			double hr;
			
			hr = 60000.0 / ((double)rri/(double)cnt);
			if (hr < TACHY_HR)
			{
				num_ssalvo++;
				ssalvo = realloc(ssalvo, sizeof(struct arr_infos) * num_ssalvo);
				ssalvo[num_ssalvo-1].hr = hr;
				ssalvo[num_ssalvo-1].start = start;
				ssalvo[num_ssalvo-1].length = l - start;
			}
			else
			{
				num_svt++;
				svt = realloc(svt, sizeof(struct arr_infos) * num_svt);
				svt[num_svt-1].hr = hr;
				svt[num_svt-1].start = start;
				svt[num_svt-1].length = l - start;
			}
			
			num_ssalvo_svt++;
			ssalvo_svt = realloc(ssalvo_svt, sizeof(struct arr_infos) * num_ssalvo_svt);
			ssalvo_svt[num_ssalvo_svt-1].hr = hr;
			ssalvo_svt[num_ssalvo_svt-1].start = start;
			ssalvo_svt[num_ssalvo_svt-1].length = l - start;
		}
		cnt = 0;
		rri = 0;
		start = 0;
	}

	save_arr_info(num_ssalvo, ssalvo, res, NUM_SSALVO+res_offset, IDX_SSALVO+res_offset,
		      LEN_SSALVO+res_offset, HR_SSALVO+res_offset);
	save_arr_info(num_svt, svt, res, NUM_SVT+res_offset, IDX_SVT+res_offset,
		      LEN_SVT+res_offset, HR_SVT+res_offset);
	save_arr_info(num_ssalvo_svt, ssalvo_svt, res, NUM_SSALVO_SVT+res_offset, IDX_SSALVO_SVT+res_offset,
		      LEN_SSALVO_SVT+res_offset, HR_SSALVO_SVT+res_offset);

	free(ssalvo);
	free(svt);
	free(ssalvo_svt);

	return 0;
} /* get_sinus_salvos() */


int
get_ventricular_related_values(long run, struct beat_infos *beats, long num, value_handle *res)
{
	int ret = 0;

	ret = get_salvos(run, beats, num, res);
	if (ret != 0)
		return ret;

	ret = get_ivr(run, beats, num, res);
	if (ret != 0)
		return ret;

	ret = get_couplet(run, beats, num, res);
	if (ret != 0)
		return ret;

	ret = get_bigeminy(run, beats, num, res);
	if (ret != 0)
		return ret;

	ret = get_trigeminy(run, beats, num, res);
	if (ret != 0)
		return ret;

	return 0;
} /* get_ventricular_related_values() */


int
get_salvos(long run, struct beat_infos *beats, long num, value_handle *res)
{
	long num_salvo = 0;
	struct arr_infos *salvo = NULL;
	long num_vt = 0;
	struct arr_infos *vt = NULL;
	long num_salvo_vt = 0;
	struct arr_infos *salvo_vt = NULL;
	
	int cnt = 0;
	long rri = 0;
	long start = 0;
	long l, res_offset;

	res_offset = run * num_results;

	for (l = 0; l < num; l++)
	{
		int skip = 0;

		if (IS_VENT(beats[l].beat_class))
		{
			rri += beats[l].rri;
			cnt++;
			if (start == 0)
				start = l;
			skip = 1;
		}

		if (skip && (l <= (num-1)))
			continue;

		if (cnt >= MIN_BEATS_SALVO)
		{
			double hr;
			
			hr = 60000.0 / ((double)rri/(double)cnt);
			if (hr < VT_HR)
			{
				num_salvo++;
				salvo = realloc(salvo, sizeof(struct arr_infos) * num_salvo);
				salvo[num_salvo-1].hr = hr;
				salvo[num_salvo-1].start = start;
				salvo[num_salvo-1].length = l - start;
			}
			else
			{
				num_vt++;
				vt = realloc(vt, sizeof(struct arr_infos) * num_vt);
				vt[num_vt-1].hr = hr;
				vt[num_vt-1].start = start;
				vt[num_vt-1].length = l - start;
			}
			
			num_salvo_vt++;
			salvo_vt = realloc(salvo_vt, sizeof(struct arr_infos) * num_salvo_vt);
			salvo_vt[num_salvo_vt-1].hr = hr;
			salvo_vt[num_salvo_vt-1].start = start;
			salvo_vt[num_salvo_vt-1].length = l - start;
		}
		cnt = 0;
		rri = 0;
		start = 0;
	}

	save_arr_info(num_salvo, salvo, res, NUM_SALVO, IDX_SALVO, LEN_SALVO, HR_SALVO);
	save_arr_info(num_vt, vt, res, NUM_VT, IDX_VT, LEN_VT, HR_VT);
	save_arr_info(num_salvo_vt, salvo_vt, res, NUM_SALVO_VT, IDX_SALVO_VT, LEN_SALVO_VT, HR_SALVO_VT);

	free(salvo);
	free(vt);
	free(salvo_vt);

	return 0;
} /* get_salvos() */


int
get_ivr(long run, struct beat_infos *beats, long num, value_handle *res)
{
	long num_ivr = 0;
	struct arr_infos *ivr = NULL;
	
	int cnt = 0;
	long rri = 0;
	long start = 0;
	long l, res_offset;

	res_offset = run * num_results;

	for (l = 0; l < num; l++)
	{
		int skip = 0;

		if (IS_VENT(beats[l].beat_class) && !(beats[l].temporal_group & ECG_TEMPORAL_PRE))
		{
			rri += beats[l].rri;
			cnt++;
			if (start == 0)
				start = l;
			skip = 1;
		}

		if (skip && (l < (num-1)))
			continue;

		if (cnt >= MIN_LENGTH_IVR)
		{
			double hr;
			
			hr = 60000.0 / ((double)rri/(double)cnt);
			
			num_ivr++;
			ivr = realloc(ivr, sizeof(struct arr_infos) * num_ivr);
			ivr[num_ivr-1].hr = hr;
			ivr[num_ivr-1].start = start;
			ivr[num_ivr-1].length = l - start;
		}
		cnt = 0;
		start = 0;
		rri = 0;
	}

	save_arr_info(num_ivr, ivr, res, NUM_IVR+res_offset, IDX_IVR+res_offset,
		      LEN_IVR+res_offset, HR_IVR+res_offset);

	free(ivr);

	return 0;
} /* get_ivr() */


int
get_couplet(long run, struct beat_infos *beats, long num, value_handle *res)
{
	long num_cpl = 0;
	struct arr_infos *cpl = NULL;
	long l, res_offset;

	res_offset = run * num_results;

	for (l = 1; l < (num-2); l++)
	{
		if (IS_VENT(beats[l].beat_class)
		    && ((beats[l].temporal_group & ECG_TEMPORAL_PRE) || (beats[l].temporal_group == ECG_TEMPORAL_UNKNOWN))
		    && IS_VENT(beats[l+1].beat_class)
		    && ((beats[l+1].temporal_group & ECG_TEMPORAL_PRE) || (beats[l+1].temporal_group == ECG_TEMPORAL_UNKNOWN))
		    && IS_SINUS(beats[l-1].beat_class) && IS_SINUS(beats[l+2].beat_class))
		{
			num_cpl++;
			cpl = realloc(cpl, sizeof(struct arr_infos) * num_cpl);
			cpl[num_cpl-1].start = l;
		}
	}
	
	save_arr_info(num_cpl, cpl, res, NUM_COUPLET+res_offset, IDX_COUPLET+res_offset, -1, -1);

	free(cpl);

	return 0;
} /* get_couplet() */


int
get_bigeminy(long run, struct beat_infos *beats, long num, value_handle *res)
{
	long num_big = 0;
	struct arr_infos *big = NULL;

	int cnt = 0;
	long start = 0;
	long l, res_offset;

	res_offset = run * num_results;

	for (l = 1; l < (num-1); l++)
	{
		int skip = 0;

		if (IS_VENT(beats[l].beat_class) && (beats[l].temporal_group & ECG_TEMPORAL_PRE)
		    && IS_SINUS(beats[l-1].beat_class)
		    && IS_SINUS(beats[l+1].beat_class)
		    && (beats[l+1].temporal_group & ECG_TEMPORAL_LATE))
		{
			if (start == 0)
				start = l;
			cnt++;
			l++;
			skip = 1;
		}
		
		if (skip && (l < (num-1)))
			continue;

		if (cnt >= MIN_LENGTH_BIGEMINY)
		{
			num_big++;
			big = realloc(big, sizeof(struct arr_infos) * num_big);
			big[num_big-1].start = start;
			big[num_big-1].length = l - start;
		}
		cnt = 0;
		start = 0;
	}

	save_arr_info(num_big, big, res, NUM_BIGEMINY+res_offset, IDX_BIGEMINY+res_offset,
		      LEN_BIGEMINY+res_offset, -1);

	free(big);

	return 0;
} /* get_bigeminy() */


int
get_trigeminy(long run, struct beat_infos *beats, long num, value_handle *res)
{
	long num_trig = 0;
	struct arr_infos *trig = NULL;

	int cnt = 0;
	long start = 0;
	long l, res_offset;

	res_offset = run * num_results;

	for (l = 1; l < (num-2); l++)
	{
		int skip = 0;

		if (IS_VENT(beats[l].beat_class) && (beats[l].temporal_group & ECG_TEMPORAL_PRE)
		    && IS_SINUS(beats[l-1].beat_class)
		    && IS_SINUS(beats[l+1].beat_class) && (beats[l+1].temporal_group & ECG_TEMPORAL_LATE)
		    && IS_SINUS(beats[l+2].beat_class) && (beats[l+2].temporal_group & ECG_TEMPORAL_NORMAL))
		{
			if (start == 0)
				start = l;
			cnt++;
			l += 2;
			skip = 1;
		}

		if (skip && (l < (num-1)))
			continue;

		if (cnt >= MIN_LENGTH_TRIGEMINY)
		{
			num_trig++;
			trig = realloc(trig, sizeof(struct arr_infos) * num_trig);
			trig[num_trig-1].start = start;
			trig[num_trig-1].length = l - start;
		}
		cnt = 0;
		start = 0;
	}

	save_arr_info(num_trig, trig, res, NUM_TRIGEMINY+res_offset, IDX_TRIGEMINY+res_offset,
		      LEN_TRIGEMINY+res_offset, -1);

	free(trig);

	return 0;
} /* get_trigeminy() */


int
save_arr_info(long num, struct arr_infos *inf, value_handle *res, long num_idx,
	      long idx_idx, long len_idx, long hr_idx)
{
	long l;

	if (num_idx != -1)
		ra_value_set_long(res[num_idx], num);

	if (num > 0)
	{
		long *buf;
		double *buf_d;

		buf = malloc(sizeof(long) * num);
		if (idx_idx != -1)
		{
			for (l = 0; l < num; l++)
				buf[l] = inf[l].start;
			ra_value_set_long_array(res[idx_idx], buf, num);
		}
		if (len_idx != -1)
		{
			for (l = 0; l < num; l++)
				buf[l] = inf[l].length;
			ra_value_set_long_array(res[len_idx], buf, num);
		}
		free(buf);

		buf_d = malloc(sizeof(double) * num);
		if (hr_idx != -1)
		{
			for (l = 0; l < num; l++)
				buf_d[l] = inf[l].hr;
			ra_value_set_double_array(res[hr_idx], buf_d, num);
		}
		free(buf_d);
	}

	return 0;
} /* save_arr_info() */


int
save_in_res(long run, struct proc_info *pi, struct beat_infos *beats, long num)
{
	value_handle *res;
	short *sbuf;
	long *lbuf;
	double *dbuf;
	long l, res_offset;

	sbuf = (short *)malloc(sizeof(short) * num);
	lbuf = (long *)malloc(sizeof(long) * num);
	dbuf = (double *)malloc(sizeof(double) * num);
	res = pi->results;
	
	res_offset = run * num_results;

	for (l = 0; l < num; l++)
		sbuf[l] = (short)beats[l].temporal_group;
	ra_value_set_short_array(res[QRS_TEMPORAL+res_offset], sbuf, num);

	for (l = 0; l < num; l++)
		lbuf[l] = beats[l].beat_class;
	ra_value_set_long_array(res[QRS_ANNOT+res_offset], lbuf, num);

	for (l = 0; l < num; l++)
		lbuf[l] = beats[l].flags;
	ra_value_set_long_array(res[ECG_FLAGS+res_offset], lbuf, num);

	for (l = 0; l < num; l++)
		dbuf[l] = beats[l].rri_double;
	ra_value_set_double_array(res[RRI+res_offset], dbuf, num);

	for (l = 0; l < num; l++)
		lbuf[l] = beats[l].rri_class;
	ra_value_set_long_array(res[RRI_ANNOT+res_offset], lbuf, num);

	for (l = 0; l < num; l++)
		dbuf[l] = beats[l].rri_ref;
	ra_value_set_double_array(res[RRI_REFVALUE+res_offset], dbuf, num);

	for (l = 0; l < num; l++)
		sbuf[l] = (short)beats[l].num_ref_rri;
	ra_value_set_short_array(res[RRI_NUM_REFVALUE+res_offset], sbuf, num);

	for (l = 0; l < num; l++)
		lbuf[l] = beats[l].event_id;
	ra_value_set_long_array(res[IDX_RRI+res_offset], lbuf, num);

	for (l = 0; l < num; l++)
		dbuf[l] = beats[l].p_width_all;
	ra_value_set_double_array(res[P_WIDTH+res_offset], dbuf, num);

	for (l = 0; l < num; l++)
		dbuf[l] = beats[l].qrs_width_all;
	ra_value_set_double_array(res[QRS_WIDTH+res_offset], dbuf, num);

	for (l = 0; l < num; l++)
		dbuf[l] = beats[l].t_width_all;
	ra_value_set_double_array(res[T_WIDTH+res_offset], dbuf, num);

	for (l = 0; l < num; l++)
		dbuf[l] = beats[l].pq_all;
	ra_value_set_double_array(res[PQ+res_offset], dbuf, num);

	for (l = 0; l < num; l++)
		dbuf[l] = beats[l].qt_all;
	ra_value_set_double_array(res[QT+res_offset], dbuf, num);

	for (l = 0; l < num; l++)
		dbuf[l] = beats[l].qtc_all;
	ra_value_set_double_array(res[QTC+res_offset], dbuf, num);

	free(sbuf);
	free(lbuf);
	free(dbuf);

	return 0;
} /* save_in_res() */


int
save_single(class_handle clh, struct beat_infos *beats, long num)
{
	long l;
	prop_handle qrs_temp, qrs_a, ecg_flags, rri, rri_a, rri_ref, rri_num_ref;
	prop_handle p_width, qrs_width, t_width;
	prop_handle pq, qt, qtc, qta;
	value_handle vh;

	if (clh == NULL)
		return -1;

	if ((qrs_temp = get_prop(clh, "qrs-temporal")) == NULL)
		return -1;
	if ((qrs_a = get_prop(clh, "qrs-annot")) == NULL)
		return -1;
	if ((ecg_flags = ra_prop_get(clh, "ecg-flags")) == NULL)
		return -1;
	if ((rri = get_prop(clh, "rri")) == NULL)
		return -1;
	if ((rri_a = get_prop(clh, "rri-annot")) == NULL)
		return -1;
	if ((rri_ref = get_prop(clh, "rri-refvalue")) == NULL)
		return -1;
	if ((rri_num_ref = get_prop(clh, "rri-num-refvalue")) == NULL)
		return -1;

	p_width = ra_prop_get(clh, "ecg-p-width");
	qrs_width = ra_prop_get(clh, "ecg-qrs-width");
	t_width = ra_prop_get(clh, "ecg-t-width");
	pq = ra_prop_get(clh, "ecg-pq");
	qt = ra_prop_get(clh, "ecg-qt");
	qtc = ra_prop_get(clh, "ecg-qtc");
	qta = ra_prop_get(clh, "ecg-qta");

	vh = ra_value_malloc();

	for (l = 1; l < num; l++)
	{
		ra_value_set_long(vh, beats[l].temporal_group);
		ra_prop_set_value(qrs_temp, beats[l].event_id, -1, vh);

		ra_value_set_long(vh, beats[l].beat_class);
		ra_prop_set_value(qrs_a, beats[l].event_id, -1, vh);

		ra_value_set_double(vh, beats[l].rri_double);
		ra_prop_set_value(rri, beats[l].event_id, -1, vh);

		ra_value_set_long(vh, beats[l].rri_class);
		ra_prop_set_value(rri_a, beats[l].event_id, -1, vh);

		ra_value_set_double(vh, beats[l].rri_ref);
		ra_prop_set_value(rri_ref, beats[l].event_id, -1, vh);

		ra_value_set_long(vh, beats[l].num_ref_rri);
		ra_prop_set_value(rri_num_ref, beats[l].event_id, -1, vh);

		ra_value_set_long(vh, beats[l].flags);
		ra_prop_set_value(ecg_flags, beats[l].event_id, -1, vh);

		if (p_width)
		{
			ra_value_set_double(vh, beats[l].p_width_all);
			ra_prop_set_value(p_width, beats[l].event_id, -1, vh);
		}
		if (qrs_width)
		{
			ra_value_set_double(vh, beats[l].qrs_width_all);
			ra_prop_set_value(qrs_width, beats[l].event_id, -1, vh);
		}
		if (t_width)
		{
			ra_value_set_double(vh, beats[l].t_width_all);
			ra_prop_set_value(t_width, beats[l].event_id, -1, vh);
		}
		if (pq)
		{
			ra_value_set_double(vh, beats[l].pq_all);
			ra_prop_set_value(pq, beats[l].event_id, -1, vh);
		}
		if (qt)
		{
			ra_value_set_double(vh, beats[l].qt_all);
			ra_prop_set_value(qt, beats[l].event_id, -1, vh);
		}
		if (qtc)
		{
			ra_value_set_double(vh, beats[l].qtc_all);
			ra_prop_set_value(qtc, beats[l].event_id, -1, vh);
		}
		if (qta)
		{
			ra_value_set_double(vh, beats[l].qta_all);
			ra_prop_set_value(qta, beats[l].event_id, -1, vh);
		}
	}

	ra_value_free(vh);

	return 0;
} /* save_single() */


prop_handle
get_prop(class_handle clh, const char *prop)
{
	prop_handle ph;

	ph = ra_prop_get(clh, prop);
	if (ph == NULL)
		ph = ra_prop_add_predef(clh, prop);

	return ph;
} /* get_prop() */



int
get_resp_phase(struct ra_ecg_options *opt, long run, struct beat_infos *beats, long num,
	       value_handle *res, class_handle clh_beats)
{
	int ret = -1;
	class_handle clh_resp;
	prop_handle resp_phase;
	value_handle vh = NULL;
	value_handle vh_event_id = NULL;
	value_handle vh_resp = NULL;
	const long *ev_id;
	long l, m, num_resp, curr_resp_ev, curr_start, curr_end;
	long num_resp_ch;
	long *resp_ch = NULL;

	if (find_resp_channel(opt->rh, &num_resp_ch, &resp_ch) != 0)
		goto go_out;

	vh = ra_value_malloc();
	if (ra_class_get(opt->eh, "respiration", vh) != 0)
		goto go_out;
	if (ra_value_get_num_elem(vh) <= 0)
		goto go_out;
	/* use first one (there should be only one respiration event-class) */
	clh_resp = (class_handle)(ra_value_get_voidp_array(vh)[0]);

	resp_phase = ra_prop_get(clh_resp, "resp-phase");
	if (!resp_phase)
		goto go_out;

	vh_event_id = ra_value_malloc();
/*	if (ra_class_get_events(clh_resp, 0, -1, 0, 1, vh_event_id) != 0) */
/*		goto go_out; */
	if (ra_class_get_events(clh_resp, beats[0].pos, beats[num-1].pos, 0, 1, vh_event_id) != 0)
		goto go_out;

	num_resp = ra_value_get_num_elem(vh_event_id);
	if (num_resp <= 0)
		goto go_out;
	ev_id = ra_value_get_long_array(vh_event_id);

	/* we have all the needed information, now get for each beat the
	   corresponding respiration phase (for each respiration channel) */
	curr_start = curr_end = -1;
	curr_resp_ev = 0;
	vh_resp = ra_value_malloc();
	for (l = 0; l < num; l++)
	{
		beats[l].num_resp_ch = num_resp_ch;
		beats[l].resp_ch = malloc(sizeof(long) * num_resp_ch);
		beats[l].resp_phase = malloc(sizeof(double) * num_resp_ch);

		if (beats[l].pos > curr_end)
		{
			while (curr_resp_ev < (num_resp-1))
			{
				curr_resp_ev++;
				ra_class_get_event_pos(clh_resp, curr_resp_ev, &curr_start, &curr_end);
				if (curr_end >= beats[l].pos)
					break;
			}
		}
		if (curr_resp_ev >= num_resp)
			break;

		if (beats[l].pos < curr_start)
		{
			for (m = 0; m < num_resp_ch; m++)
				beats[l].resp_phase[m] = -1000.0;
			continue;
		}

		for (m = 0; m < num_resp_ch; m++)
		{
			ra_prop_get_value(resp_phase, curr_resp_ev, resp_ch[m], vh_resp);
			beats[l].resp_ch[m] = resp_ch[m];
			beats[l].resp_phase[m] = ra_value_get_double(vh_resp);
		}
	}
	
	ret = save_resp_values(opt, run, beats, num, res, clh_beats, num_resp_ch);

go_out:
	if (resp_ch)
		free(resp_ch);
	if (vh)
		ra_value_free(vh);	
	if (vh_event_id)
		ra_value_free(vh_event_id);
	if (vh_resp)
		ra_value_free(vh_resp);

	return ret;
} /* get_resp_phase() */


int
find_resp_channel(rec_handle rh, long *num_ch, long **ch)
{
	long l, n;
	value_handle vh;

	vh = ra_value_malloc();
	if (ra_info_get(rh, RA_INFO_REC_GEN_NUM_CHANNEL_L, vh) != 0)
	{
		ra_value_free(vh);
		return -1;
	}
	n = ra_value_get_long(vh);

	*ch = malloc(sizeof(long) * n);
	*num_ch = 0;
	for (l = 0; l < n; l++)
	{
		ra_value_set_number(vh, l);
		if (ra_info_get(rh, RA_INFO_REC_CH_TYPE_L, vh) != 0)
			continue;

		if (ra_value_get_long(vh) == RA_CH_TYPE_RESP)
		{
			(*ch)[*num_ch] = l;
			(*num_ch)++;
		}
	}

	ra_value_free(vh);

	return 0;
} /* find_resp_channel() */


int
save_resp_values(struct ra_ecg_options *opt, long run, struct beat_infos *beats, long num,
		 value_handle *res, class_handle clh_beats, long num_ch_resp)
{
	long l, m, cnt;
	prop_handle ecg_resp_phase;
	value_handle vh;
	long *resp_ch_res = NULL;
	double *resp_phase_res = NULL;

	if (!opt->save_in_class)
		return 0;

	if (clh_beats == NULL)
		return -1;

	if ((ecg_resp_phase = get_prop(clh_beats, "ecg-resp-phase")) == NULL)
		return -1;

	resp_ch_res = malloc(sizeof(long) * num_ch_resp * num);
	resp_phase_res = malloc(sizeof(double) * num_ch_resp * num);

	vh = ra_value_malloc();
	cnt = 0;
	for (l = 0; l < num; l++)
	{
		if (beats[l].resp_phase == NULL)
			continue;

		for (m = 0; m < beats[l].num_resp_ch; m++)
		{
			if (beats[l].resp_phase[m] == -1000.0)
				continue;

			ra_value_set_double(vh, beats[l].resp_phase[m]);
			ra_prop_set_value(ecg_resp_phase, beats[l].event_id, beats[l].resp_ch[m], vh);

			resp_ch_res[cnt] = beats[l].resp_ch[m];
			resp_phase_res[cnt] = beats[l].resp_phase[m];
			cnt++;
		}
	}
	ra_value_free(vh);

	if (res)
	{
		long res_offset = run * num_results;
		ra_value_set_double_array(res[RESP_PHASE+res_offset], resp_phase_res, cnt);
		ra_value_set_long_array(res[RESP_CH+res_offset], resp_ch_res, cnt);
	}
	
	if (resp_ch_res)
		free(resp_ch_res);
	if (resp_phase_res)
		free(resp_phase_res);
					  
	return 0;
} /* save_resp_values() */
