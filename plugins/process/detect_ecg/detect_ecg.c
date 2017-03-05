/*----------------------------------------------------------------------------
 * detect_ecg.c  -  performs ECG beat-detection and morphology measures
 *
 * Description:
 * The plugin performs a beat detection in ecgs and it measures the
 * morphology parameters on a beat-to-beat basis. The results
 * will be saved in the libRASCH-evaluation-file of the measurement.
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2005-2006, Raphael Schneider
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
#include <pl_general.h>
#include <ra_detect.h>
#include "detect_ecg.h"


LIBRAAPI int
load_ra_plugin(struct plugin_struct *ps)
{
	strcpy(ps->info.name, PLUGIN_NAME);
	strcpy(ps->info.desc, PLUGIN_DESC);
	ps->info.type = PLUGIN_TYPE;
	ps->info.license = PLUGIN_LICENSE;
	sprintf(ps->info.build_ts, "%s  %s", __DATE__, __TIME__);
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
	ps->info.create_class = ra_alloc_mem(sizeof(create_class));
	memcpy(ps->info.create_class, create_class, sizeof(create_class));
	ps->info.num_create_prop = num_create_prop;
	ps->info.create_prop = ra_alloc_mem(sizeof(create_prop));
	memcpy(ps->info.create_prop, create_prop, sizeof(create_prop));

	ps->call_gui = NULL;	/* will be set in init_ra_plugin() */
	ps->proc.get_proc_handle = pl_get_proc_handle;
	ps->proc.free_proc_handle = pl_free_proc_handle;
	ps->proc.do_processing = pl_do_processing;

	return 0;
} /* init_plugin() */


LIBRAAPI int
init_ra_plugin(struct librasch *ra, struct plugin_struct *ps)
{
	/* check if gui is available */
	if (ra_plugin_get_by_name(ra, "gui-detect-simple", 1))
		ps->call_gui = pl_call_gui;

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
		_ra_set_error(pl, 0, "plugin-detect-simple");
		return NULL;
	}

	memset(pi, 0, sizeof(struct proc_info));
	pi->handle_id = RA_HANDLE_PROC;
	pi->plugin = pl;

	/* init options */
	pi->options = calloc(1, sizeof(struct ra_detect));
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
set_default_options(struct ra_detect *opt)
{
	opt->num_ch = 0;
	opt->ch = NULL;
	opt->combine_beats = 1;
	opt->min_beat_distance = 0.2;
	opt->save_in_eval = 0;
	opt->eh = NULL;
	opt->clh = NULL;

	opt->use_region = 0;
	opt->region_start = 0;
	opt->region_end = 0;
	opt->region_start_is_beatpos = 0;

	opt->num_events = 0;
	opt->events = NULL;

	opt->filter_powerline_noise = 0;
	opt->filter_baseline_wander = 0;
	opt->do_interpolation = 0;
	opt->check_for_calibration = 0;
/* 	opt->t_wave_algorithm = T_WAVE_END_ALGO_DEFAULT; */
	opt->t_wave_algorithm = T_WAVE_END_ALGO_TANGENT;

	opt->force_p_type = 0;
	opt->force_qrs_type = 0;
	opt->force_t_type = 0;

	opt->check_wave_at_cursor = 0;
	opt->edit_cursor_pos = -1;
	opt->wave_to_check = -1;

	opt->thresh_p = 1.0;
	opt->thresh_qrs = 1.0;
	opt->thresh_t = 1.0;
} /* set_default_options() */


void
set_option_offsets(struct ra_option_infos *opt_inf, struct ra_detect *opt)
{
	unsigned char *p = (unsigned char *)opt;
	int idx = 0;

	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->num_ch) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->ch) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->combine_beats) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->min_beat_distance) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->save_in_eval) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->eh) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->clh) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->use_region) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->region_start) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->region_end) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->region_start_is_beatpos) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->num_events) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->events) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->filter_powerline_noise) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->filter_baseline_wander) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->check_for_calibration) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->do_interpolation) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->t_wave_algorithm) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->force_p_type) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->force_qrs_type) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->force_t_type) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->check_wave_at_cursor) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->edit_cursor_pos) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->wave_to_check) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->thresh_p) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->thresh_qrs) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->thresh_t) - p);

	/* consistency check (useful when changing option structure) */
	if (idx != num_options)
	{
		fprintf(stderr, "critical error in detect_ecg.c - set_option_offsets():\n"
			"  number of options(%ld) != idx(%d)\n", num_options, idx);
		exit(-1);
	}
} /* set_option_offsets() */


void
pl_free_proc_handle(proc_handle proc)
{
	struct proc_info *pi = proc;
	
	if (pi->options)
	{
		struct ra_detect *opt;

		opt = (struct ra_detect *)pi->options;
		if (opt->ch)
			ra_free_mem(opt->ch);
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
	struct detect_result dr;
	struct ra_detect *opt;
	value_handle *res;
	long l;

	memset(&dr, 0, sizeof(struct detect_result));

	/* if no rec-handle was set, use root recording */
	if (pi->rh == NULL)
		pi->rh = ra_rec_get_first(pi->mh, 0);

	opt = (struct ra_detect *)pi->options;

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

	/* check if channels are given */
	if ((opt->num_ch == 0) || (opt->ch == NULL))
	{
		/* no -> get all ECG channels */
		if (opt->ch != NULL)
		{
			free(opt->ch);
			opt->ch = NULL;
		}

		ret = get_ecg_channels(pi->rh, &(opt->ch), &(opt->num_ch));
		if (ret != 0)
			goto error;		
	}

	if (opt->num_ch == 0)
	{
		_ra_set_error(pi->ra, RA_WARN_NO_DATA, "plugin-detect-ecg");
		goto error;
	}

	/* calc interpolation factors (if requested) */
	dr.interp_factor = calloc(opt->num_ch, sizeof(long));
	if (opt->do_interpolation)
	{
		long l;
		value_handle vh = ra_value_malloc();

		for (l = 0; l < opt->num_ch; l++)
		{
			double samplerate, temp;

			dr.interp_factor[l] = 1;

			ra_value_set_number(vh, opt->ch[l]);
			if (ra_info_get(pi->rh, RA_INFO_REC_CH_SAMPLERATE_D, vh) == 0)
				samplerate = ra_value_get_double(vh);
			else
			{
				ra_value_free(vh);
				goto error;
			}

			if (samplerate >= 500)
				continue;

			temp = samplerate / 10;
			if (((samplerate/10.0)-(double)temp) != 0)
				dr.interp_factor[l] = (long)(512 / samplerate);
			else
				dr.interp_factor[l] = (long)(500 / samplerate);
		}
		ra_value_free(vh);
	}

	if (opt->num_events > 0)
	{
		ret = get_beat_infos(opt->num_events, opt->events, opt->num_ch, opt->ch, opt->clh, &dr);
		if (ret != 0)
			goto error;
	}
	else
	{
		value_handle vh;
		long num_samples, num_elem;
		double samplerate;

		vh = ra_value_malloc();
		ra_value_set_number(vh, opt->ch[0]);
		if (ra_info_get(pi->rh, RA_INFO_REC_CH_NUM_SAMPLES_L, vh) == 0)
			num_samples = ra_value_get_long(vh);
		else
			num_samples = 0;
		if (ra_info_get(pi->rh, RA_INFO_REC_CH_SAMPLERATE_D, vh) == 0)
			samplerate = ra_value_get_double(vh);
		else
			samplerate = 1;
		ra_value_free(vh);

		num_elem = (long)((double)num_samples / samplerate);
		num_elem *= 2;	/* get memory for two beats/second */

		init_beat_mem(&(dr.b), num_elem, opt->num_ch);
	}

	ret = main_detect(pi, &dr);
	if (ret != 0)
		goto error;

	ret = store_results(pi, &dr);
	if (ret != 0)
		goto error;

 error:
	if (dr.ch_in_sig)
		free(dr.ch_in_sig);

	if (dr.samplerate)
		free(dr.samplerate);
	if (dr.interp_factor)
		free(dr.interp_factor);
	if (dr.calib_offset)
		free(dr.calib_offset);
	if (dr.ch_found)
		free(dr.ch_found);

	free_beat_mem(&(dr.b));

	if (dr.pos)
		free(dr.pos);
	if (dr.annot)
		free(dr.annot);

	if (dr.p_start)
		free(dr.p_start);
	if (dr.p_end)
		free(dr.p_end);
	if (dr.p_width)
		free(dr.p_width);

	if (dr.qrs_start)
		free(dr.qrs_start);
	if (dr.qrs_end)
		free(dr.qrs_end);
	if (dr.qrs_width)
		free(dr.qrs_width);

	if (dr.t_start)
		free(dr.t_start);
	if (dr.t_end)
		free(dr.t_end);
	if (dr.t_width)
		free(dr.t_width);

	if (dr.pq)
		free(dr.pq);
	if (dr.qt)
		free(dr.qt);
	if (dr.qtc)
		free(dr.qtc);

	return ret;
} /* pl_do_processing() */


int
get_ecg_channels(rec_handle rh, long **ch, long *num_ch)
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
	*ch = ra_alloc_mem(sizeof(long) * n);

	for (l = 0; l < n; l++)
	{
		ra_value_set_number(vh, l);
		if ((ret = ra_info_get(rh, RA_INFO_REC_CH_TYPE_L, vh)) != 0)
			break;
		if (ra_value_get_long(vh) == RA_CH_TYPE_ECG)
		{
			(*ch)[*num_ch] = l;
			(*num_ch)++;
		}
	}

	if ((ret != 0) || (*num_ch <= 0))
	{
		free(*ch);
		*ch = NULL;
	}

	ra_value_free(vh);

	return ret;
} /* get_ecg_channels() */


int
get_beat_infos(long num_events, long *events, long num_ch, long *ch, class_handle clh,
	       struct detect_result *dr)
{
	int ret = 0;
	long l, m;
	struct beats2 *b, *last;
	value_handle vh;
	struct beat_props props;

	if ((num_events <= 0) || (events == NULL) || (clh == NULL) || (dr == NULL))
		return -1;

	if ((ret = get_beat_props(clh, &props)) != 0)
		return ret;

	vh = ra_value_malloc();

	init_beat_mem(&(dr->b), num_events, num_ch);
	last = NULL;
	for (l = 0; l < num_events; l++)
	{
		long start, end;

		/* the position of the beat is the event-position, the position of the
		   fiducial point is set below */
		ra_class_get_event_pos(clh, events[l], &start, &end);
		b = beat_malloc(&(dr->b), 0, start);
		if (b == NULL)
		{
			ret = -1;
			break;
		}
		if (ra_prop_get_value(props.pos, events[l], -1, vh) == 0)
			SET_BEAT_FID_POS(b, ra_value_get_long(vh) * dr->interp_factor[0]);

		/* When beats are given by there event-id's it is not necessary, that
		   they are contiguous. Therefore clear the flag here. */
		b->contiguous = 0;

		if (ra_prop_get_value(props.rri, events[l], -1, vh) == 0)
			SET_HR(b, (60000.0 / ra_value_get_double(vh)));
		else
			SET_HR(b, 60.0);

		SET_EVENT_ID(b, events[l]);

		for (m = 0; m < num_ch; m++)
		{
			ret = fill_beat_ch_infos(&props, ch[m], m, dr->interp_factor[m], b);
			SET_BEAT_IN_CH(b, m, 1);
		}
 		if (ret != 0)
 			break;

		b->prev = last;
		if (last)
			last->next = b;

		last = b;
	}
	dr->num_beats = num_events;
	
	ra_value_free(vh);

	return ret;
} /* get_beat_infos() */


int
get_beat_props(class_handle clh, struct beat_props *p)
{
	if ((p->pos = ra_prop_get(clh, "qrs-pos")) == NULL)
		return -1;
	if ((p->noise = ra_prop_get(clh, "ecg-noise")) == NULL)
		return -1;
	if ((p->rri = ra_prop_get(clh, "rri")) == NULL)
		return -1;
	if ((p->flags = ra_prop_get(clh, "ecg-morph-flags")) == NULL)
		return -1;
	if ((p->pq = ra_prop_get(clh, "ecg-pq")) == NULL)
		return -1;
	if ((p->qt = ra_prop_get(clh, "ecg-pq")) == NULL)
		return -1;
	if ((p->qtc = ra_prop_get(clh, "ecg-pq")) == NULL)
		return -1;

	if ((p->p_type = ra_prop_get(clh, "ecg-p-type")) == NULL)
		return -1;
	if ((p->p_start = ra_prop_get(clh, "ecg-p-start")) == NULL)
		return -1;
	if ((p->p_peak_1 = ra_prop_get(clh, "ecg-p-peak-1")) == NULL)
		return -1;
	if ((p->p_peak_2 = ra_prop_get(clh, "ecg-p-peak-2")) == NULL)
		return -1;
	if ((p->p_end = ra_prop_get(clh, "ecg-p-end")) == NULL)
		return -1;
	if ((p->p_width = ra_prop_get(clh, "ecg-p-width")) == NULL)
		return -1;

	if ((p->qrs_type = ra_prop_get(clh, "ecg-qrs-type")) == NULL)
		return -1;
	if ((p->qrs_start = ra_prop_get(clh, "ecg-qrs-start")) == NULL)
		return -1;
	if ((p->q = ra_prop_get(clh, "ecg-q-peak")) == NULL)
		return -1;
	if ((p->r = ra_prop_get(clh, "ecg-r-peak")) == NULL)
		return -1;
	if ((p->s = ra_prop_get(clh, "ecg-s-peak")) == NULL)
		return -1;
	if ((p->rs = ra_prop_get(clh, "ecg-rs-peak")) == NULL)
		return -1;
	if ((p->qrs_end = ra_prop_get(clh, "ecg-qrs-end")) == NULL)
		return -1;
	if ((p->qrs_width = ra_prop_get(clh, "ecg-qrs-width")) == NULL)
		return -1;

	if ((p->t_type = ra_prop_get(clh, "ecg-t-type")) == NULL)
		return -1;
	if ((p->t_start = ra_prop_get(clh, "ecg-t-start")) == NULL)
		return -1;
	if ((p->t_peak_1 = ra_prop_get(clh, "ecg-t-peak-1")) == NULL)
		return -1;
	if ((p->t_peak_2 = ra_prop_get(clh, "ecg-t-peak-2")) == NULL)
		return -1;
	if ((p->t_end = ra_prop_get(clh, "ecg-t-end")) == NULL)
		return -1;
	if ((p->t_width = ra_prop_get(clh, "ecg-t-width")) == NULL)
		return -1;

	return 0;
} /* get_beat_props() */


int
fill_beat_ch_infos(struct beat_props *p, long ch, long ch_idx, long interp_factor, struct beats2 *b)
{
	long event_id, pos;
	value_handle vh;	
	vh = ra_value_malloc();
	
	event_id = GET_EVENT_ID(b);
	pos = GET_BEAT_POS(b);

	/* general infos */
	if (ra_prop_get_value(p->noise, event_id, ch, vh) == 0)
		SET_NOISE(b, ch_idx, ra_value_get_double(vh));
	else
		SET_NOISE(b, ch_idx, -1);
	if (ra_prop_get_value(p->flags, event_id, ch, vh) == 0)
		SET_MORPH_FLAGS(b, ch_idx, ra_value_get_long(vh));
	else
		SET_MORPH_FLAGS(b, ch_idx, 0);

	if (ra_prop_get_value(p->pq, event_id, ch, vh) == 0)
		SET_PQ(b, ch_idx, ra_value_get_double(vh));
	else
		SET_PQ(b, ch_idx, -1);
	if (ra_prop_get_value(p->qt, event_id, ch, vh) == 0)
		SET_QT(b, ch_idx, ra_value_get_double(vh));
	else
		SET_QT(b, ch_idx, -1);
	if (ra_prop_get_value(p->qtc, event_id, ch, vh) == 0)
		SET_QTC(b, ch_idx, ra_value_get_double(vh));
	else
		SET_QTC(b, ch_idx, -1);
		    
	/* P-wave infos */
	if (ra_prop_get_value(p->p_type, event_id, ch, vh) == 0)
		SET_P_TYPE(b, ch_idx, ra_value_get_long(vh));
	else
		SET_P_TYPE(b, ch_idx, 0);
	SET_P_START(b, ch_idx, -1);
	if (ra_prop_get_value(p->p_start, event_id, ch, vh) == 0)
	{
		long pos_use;
		short offset = ra_value_get_short(vh) * (short)interp_factor;
		pos_use = (long)offset + pos;
		if (offset != INVALID_OFFSET_VALUE)
			SET_P_START(b, ch_idx, pos_use);
	}
	SET_P_PEAK_1(b, ch_idx, -1);
	if (ra_prop_get_value(p->p_peak_1, event_id, ch, vh) == 0)
	{
		long pos_use;
		short offset = ra_value_get_short(vh) * (short)interp_factor;
		pos_use = (long)offset + pos;
		if (offset != INVALID_OFFSET_VALUE)
			SET_P_PEAK_1(b, ch_idx, pos_use);
	}
	SET_P_PEAK_2(b, ch_idx, -1);
	if (ra_prop_get_value(p->p_peak_2, event_id, ch, vh) == 0)
	{
		long pos_use;
		short offset = ra_value_get_short(vh) * (short)interp_factor;
		pos_use = (long)offset + pos;
		if (offset != INVALID_OFFSET_VALUE)
			SET_P_PEAK_2(b, ch_idx, pos_use);
	}
	SET_P_END(b, ch_idx, -1);
	if (ra_prop_get_value(p->p_end, event_id, ch, vh) == 0)
	{
		long pos_use;
		short offset = ra_value_get_short(vh) * (short)interp_factor;
		pos_use = (long)offset + pos;
		if (offset != INVALID_OFFSET_VALUE)
			SET_P_END(b, ch_idx, pos_use);
	}
	if (ra_prop_get_value(p->p_width, event_id, ch, vh) == 0)
		SET_P_WIDTH(b, ch_idx, ra_value_get_double(vh));
	else
		SET_P_WIDTH(b, ch_idx, -1);
	
	/* QRS complex infos */
	if (ra_prop_get_value(p->qrs_type, event_id, ch, vh) == 0)
		SET_QRS_TYPE(b, ch_idx, ra_value_get_long(vh));
	else
		SET_QRS_TYPE(b, ch_idx, 0);
	SET_QRS_START(b, ch_idx, -1);
	if (ra_prop_get_value(p->qrs_start, event_id, ch, vh) == 0)
	{
		long pos_use;
		short offset = ra_value_get_short(vh) * (short)interp_factor;
		pos_use = (long)offset + pos;
		if (offset != INVALID_OFFSET_VALUE)
			SET_QRS_START(b, ch_idx, pos_use);
	}
	SET_Q(b, ch_idx, -1);
	if (ra_prop_get_value(p->q, event_id, ch, vh) == 0)
	{
		long pos_use;
		short offset = ra_value_get_short(vh) * (short)interp_factor;
		pos_use = (long)offset + pos;
		if (offset != INVALID_OFFSET_VALUE)
			SET_Q(b, ch_idx, pos_use);
	}
	SET_R(b, ch_idx, -1);
	if (ra_prop_get_value(p->r, event_id, ch, vh) == 0)
	{
		long pos_use;
		short offset = ra_value_get_short(vh) * (short)interp_factor;
		pos_use = (long)offset + pos;
		if (offset != INVALID_OFFSET_VALUE)
			SET_R(b, ch_idx, pos_use);
	}
	SET_S(b, ch_idx, -1);
	if (ra_prop_get_value(p->s, event_id, ch, vh) == 0)
	{
		long pos_use;
		short offset = ra_value_get_short(vh) * (short)interp_factor;
		pos_use = (long)offset + pos;
		if (offset != INVALID_OFFSET_VALUE)
			SET_S(b, ch_idx, pos_use);
	}
	SET_RS(b, ch_idx, -1);
	if (ra_prop_get_value(p->rs, event_id, ch, vh) == 0)
	{
		long pos_use;
		short offset = ra_value_get_short(vh) * (short)interp_factor;
		pos_use = (long)offset + pos;
		if (offset != INVALID_OFFSET_VALUE)
			SET_RS(b, ch_idx, pos_use);
	}
	SET_QRS_END(b, ch_idx, -1);
	if (ra_prop_get_value(p->qrs_end, event_id, ch, vh) == 0)
	{
		long pos_use;
		short offset = ra_value_get_short(vh) * (short)interp_factor;
		pos_use = (long)offset + pos;
		if (offset != INVALID_OFFSET_VALUE)
			SET_QRS_END(b, ch_idx, pos_use);
	}
	if (ra_prop_get_value(p->qrs_width, event_id, ch, vh) == 0)
		SET_QRS_WIDTH(b, ch_idx, ra_value_get_double(vh));
	else
		SET_QRS_WIDTH(b, ch_idx, -1);


	/* T-wave infos */
	if (ra_prop_get_value(p->t_type, event_id, ch, vh) == 0)
		SET_T_TYPE(b, ch_idx, ra_value_get_long(vh));
	else
		SET_T_TYPE(b, ch_idx, 0);
	SET_T_START(b, ch_idx, -1);
	if (ra_prop_get_value(p->t_start, event_id, ch, vh) == 0)
	{
		long pos_use;
		short offset = ra_value_get_short(vh) * (short)interp_factor;
		pos_use = (long)offset + pos;
		if (offset != INVALID_OFFSET_VALUE)
			SET_T_START(b, ch_idx, pos_use);
	}
	SET_T_PEAK_1(b, ch_idx, -1);
	if (ra_prop_get_value(p->t_peak_1, event_id, ch, vh) == 0)
	{
		long pos_use;
		short offset = ra_value_get_short(vh) * (short)interp_factor;
		pos_use = (long)offset + pos;
		if (offset != INVALID_OFFSET_VALUE)
			SET_T_PEAK_1(b, ch_idx, pos_use);
	}
	SET_T_PEAK_2(b, ch_idx, -1);
	if (ra_prop_get_value(p->t_peak_2, event_id, ch, vh) == 0)
	{
		long pos_use;
		short offset = ra_value_get_short(vh) * (short)interp_factor;
		pos_use = (long)offset + pos;
		if (offset != INVALID_OFFSET_VALUE)
			SET_T_PEAK_2(b, ch_idx, pos_use);
	}
	SET_T_END(b, ch_idx, -1);
	if (ra_prop_get_value(p->t_end, event_id, ch, vh) == 0)
	{
		long pos_use;
		short offset = ra_value_get_short(vh) * (short)interp_factor;
		pos_use = (long)offset + pos;
		if (offset != INVALID_OFFSET_VALUE)
			SET_T_END(b, ch_idx, pos_use);
	}
	if (ra_prop_get_value(p->t_width, event_id, ch, vh) == 0)
		SET_T_WIDTH(b, ch_idx, ra_value_get_double(vh));
	else
		SET_T_WIDTH(b, ch_idx, -1);

	ra_value_free(vh);

	return 0;
} /* fill_beat_ch_infos() */


int
store_results(struct proc_info *pi, struct detect_result *dr)
{
	int ret = 0;
	value_handle vh;
	double x_scale;
	value_handle *res;
	long *lbuf = NULL;
	long *ch = NULL;
	short *sbuf = NULL;
	double *dbuf = NULL;
	long l, num_values;
	struct ra_detect *opt;

	opt = (struct ra_detect *)pi->options;

	/* get scaling factor for the first channel to scale qrs positions */
	/* TODO: handle ecg-channels with different samplerates
	   (resp. think about if it is OK to have different samplerates) */
	vh = ra_value_malloc();
	ra_value_set_number(vh, opt->ch[0]);
	ra_info_get(pi->mh, RA_INFO_CH_XSCALE_D, vh);
	x_scale = ra_value_get_double(vh);
	ra_value_free(vh);

	/* save results in value-handles */
	res = pi->results;

	ra_value_set_long(res[NUM], dr->num_beats);

	if (dr->pos != NULL)
	{
		ra_value_set_long_array(res[POS], dr->pos, dr->num_beats);
		ra_value_set_number(res[ANNOT], EVENT_PROP_QRS_POS);
	}

	if (dr->annot != NULL)
	{
		ra_value_set_long_array(res[ANNOT], dr->annot, dr->num_beats);
		ra_value_set_number(res[ANNOT], EVENT_PROP_QRS_ANNOT);
	}

	num_values = dr->num_beats * (opt->num_ch + 1);	/* +1 for ch-independent values */
	lbuf = malloc(sizeof(long) * num_values);
	sbuf = malloc(sizeof(short) * num_values);
	dbuf = malloc(sizeof(double) * num_values);
	ch = malloc(sizeof(long) * num_values);

	ret = store_single_value(opt, &(dr->b), dr->ch_in_sig, BEATS_MORPH_FLAGS_L, NULL, 0,
				 RA_VALUE_TYPE_LONG, sbuf, lbuf, dbuf, ch, num_values,
				 CH_MORPH, MORPH_FLAGS, EVENT_PROP_ECG_MORPH_FLAGS, res);


	/* start-position shows if wave is valid; a value of -1 means that the
	   wave is not present or was filtered out in the post-process step */
	/* ********** P-wave ********** */
	ret = store_single_value(opt, &(dr->b), dr->ch_in_sig, BEATS_P_TYPE_L, NULL, 0,
				 RA_VALUE_TYPE_LONG, sbuf, lbuf, dbuf, ch, num_values,
				 CH_P_TYPE, P_TYPE, EVENT_PROP_ECG_P_TYPE, res);

	ret = store_single_value(opt, &(dr->b), dr->ch_in_sig, BEATS_P_START_L, dr->p_start, 1,
				 RA_VALUE_TYPE_SHORT, sbuf, lbuf, dbuf, ch, num_values,
				 CH_P, P_START, EVENT_PROP_ECG_P_START_OFFSET, res);

	ret = store_single_value(opt, &(dr->b), dr->ch_in_sig, BEATS_P_PEAK_1_L, NULL, 1,
				 RA_VALUE_TYPE_SHORT, sbuf, lbuf, dbuf, ch, num_values,
				 CH_P_PEAK_1, P_PEAK_1, EVENT_PROP_ECG_P_PEAK_1_OFFSET, res);

	ret = store_single_value(opt, &(dr->b), dr->ch_in_sig, BEATS_P_PEAK_2_L, NULL, 1,
				 RA_VALUE_TYPE_SHORT, sbuf, lbuf, dbuf, ch, num_values,
				 CH_P_PEAK_2, P_PEAK_2, EVENT_PROP_ECG_P_PEAK_2_OFFSET, res);

	ret = store_single_value(opt, &(dr->b), dr->ch_in_sig, BEATS_P_END_L, dr->p_end, 1,
				 RA_VALUE_TYPE_SHORT, sbuf, lbuf, dbuf, ch, num_values,
				 CH_P, P_END, EVENT_PROP_ECG_P_END_OFFSET, res);

	ret = store_single_value(opt, &(dr->b), dr->ch_in_sig, BEATS_P_WIDTH_D, dr->p_width, 0,
				 RA_VALUE_TYPE_DOUBLE, sbuf, lbuf, dbuf, ch, num_values,
				 CH_P, P_WIDTH, EVENT_PROP_ECG_P_WIDTH, res);


		
	/* ********** QRS complex ********** */
	ret = store_single_value(opt, &(dr->b), dr->ch_in_sig, BEATS_QRS_TYPE_L, NULL, 0,
				 RA_VALUE_TYPE_LONG, sbuf, lbuf, dbuf, ch, num_values,
				 CH_QRS_TYPE, QRS_TYPE, EVENT_PROP_ECG_QRS_TYPE, res);

	ret = store_single_value(opt, &(dr->b), dr->ch_in_sig, BEATS_QRS_START_L, dr->qrs_start, 1,
				 RA_VALUE_TYPE_SHORT, sbuf, lbuf, dbuf, ch, num_values,
				 CH_QRS, QRS_START, EVENT_PROP_ECG_QRS_START_OFFSET, res);

	ret = store_single_value(opt, &(dr->b), dr->ch_in_sig, BEATS_Q_L, NULL, 1,
				 RA_VALUE_TYPE_SHORT, sbuf, lbuf, dbuf, ch, num_values,
				 CH_Q_PEAK, Q_PEAK, EVENT_PROP_ECG_Q_PEAK_OFFSET, res);

	ret = store_single_value(opt, &(dr->b), dr->ch_in_sig, BEATS_R_L, NULL, 1,
				 RA_VALUE_TYPE_SHORT, sbuf, lbuf, dbuf, ch, num_values,
				 CH_R_PEAK, R_PEAK, EVENT_PROP_ECG_R_PEAK_OFFSET, res);

	ret = store_single_value(opt, &(dr->b), dr->ch_in_sig, BEATS_S_L, NULL, 1,
				 RA_VALUE_TYPE_SHORT, sbuf, lbuf, dbuf, ch, num_values,
				 CH_S_PEAK, S_PEAK, EVENT_PROP_ECG_S_PEAK_OFFSET, res);

	ret = store_single_value(opt, &(dr->b), dr->ch_in_sig, BEATS_RS_L, NULL, 1,
				 RA_VALUE_TYPE_SHORT, sbuf, lbuf, dbuf, ch, num_values,
				 CH_RS_PEAK, RS_PEAK, EVENT_PROP_ECG_RS_PEAK_OFFSET, res);

	ret = store_single_value(opt, &(dr->b), dr->ch_in_sig, BEATS_QRS_END_L, dr->qrs_end, 1,
				 RA_VALUE_TYPE_SHORT, sbuf, lbuf, dbuf, ch, num_values,
				 CH_QRS, QRS_END, EVENT_PROP_ECG_QRS_END_OFFSET, res);

	ret = store_single_value(opt, &(dr->b), dr->ch_in_sig, BEATS_QRS_WIDTH_D, dr->qrs_width, 0,
				 RA_VALUE_TYPE_DOUBLE, sbuf, lbuf, dbuf, ch, num_values,
				 CH_QRS, QRS_WIDTH, EVENT_PROP_ECG_QRS_WIDTH, res);
		
		
	/* ********** T-wave ********** */
	ret = store_single_value(opt, &(dr->b), dr->ch_in_sig, BEATS_T_TYPE_L, NULL, 0,
				 RA_VALUE_TYPE_LONG, sbuf, lbuf, dbuf, ch, num_values,
				 CH_T_TYPE, T_TYPE, EVENT_PROP_ECG_T_TYPE, res);
	ret = store_single_value(opt, &(dr->b), dr->ch_in_sig, BEATS_T_START_L, dr->t_start, 1,
				 RA_VALUE_TYPE_SHORT, sbuf, lbuf, dbuf, ch, num_values,
				 CH_T, T_START, EVENT_PROP_ECG_T_START_OFFSET, res);
	ret = store_single_value(opt, &(dr->b), dr->ch_in_sig, BEATS_T_PEAK_1_L, NULL, 1,
				 RA_VALUE_TYPE_SHORT, sbuf, lbuf, dbuf, ch, num_values,
				 CH_T_PEAK_1, T_PEAK_1, EVENT_PROP_ECG_T_PEAK_1_OFFSET, res);
	ret = store_single_value(opt, &(dr->b), dr->ch_in_sig, BEATS_T_PEAK_2_L, NULL, 1,
				 RA_VALUE_TYPE_SHORT, sbuf, lbuf, dbuf, ch, num_values,
				 CH_T_PEAK_2, T_PEAK_2, EVENT_PROP_ECG_T_PEAK_2_OFFSET, res);
	ret = store_single_value(opt, &(dr->b), dr->ch_in_sig, BEATS_T_END_L, dr->t_end, 1,
				 RA_VALUE_TYPE_SHORT, sbuf, lbuf, dbuf, ch, num_values,
				 CH_T, T_END, EVENT_PROP_ECG_T_END_OFFSET, res);
	ret = store_single_value(opt, &(dr->b), dr->ch_in_sig, BEATS_T_WIDTH_D, dr->t_width, 0,
				 RA_VALUE_TYPE_DOUBLE, sbuf, lbuf, dbuf, ch, num_values,
				 CH_T, T_WIDTH, EVENT_PROP_ECG_T_WIDTH, res);

		
	/* ********** calculated beat values ********** */
	ret = store_single_value(opt, &(dr->b), dr->ch_in_sig, BEATS_PQ_D, dr->pq, 0,
				 RA_VALUE_TYPE_DOUBLE, sbuf, lbuf, dbuf, ch, num_values,
				 CH_PQ, PQ, EVENT_PROP_ECG_PQ, res);
	ret = store_single_value(opt, &(dr->b), dr->ch_in_sig, BEATS_QT_D, dr->qt, 0,
				 RA_VALUE_TYPE_DOUBLE, sbuf, lbuf, dbuf, ch, num_values,
				 CH_QT, QT, EVENT_PROP_ECG_QT, res);
	ret = store_single_value(opt, &(dr->b), dr->ch_in_sig, BEATS_QTC_D, dr->qtc, 0,
				 RA_VALUE_TYPE_DOUBLE, sbuf, lbuf, dbuf, ch, num_values,
				 CH_QT, QTC, EVENT_PROP_ECG_QTC, res);
	ret = store_single_value(opt, &(dr->b), dr->ch_in_sig, BEATS_NOISE_D, dr->noise, 0,
				 RA_VALUE_TYPE_DOUBLE, sbuf, lbuf, dbuf, ch, num_values,
				 CH_NOISE, NOISE, EVENT_PROP_ECG_NOISE, res);

	free(sbuf);
	free(lbuf);
	free(dbuf);

	if (opt->save_in_eval)
	{
		if (opt->eh == NULL)
		{
			_ra_set_error(pi->ra, RA_ERR_INFO_MISSING, "plugin-detect-ecg");
			return -1;
		}

		if (opt->use_region || (opt->num_events > 0))
		{
			struct plugin_struct *pl;
			struct plugin_infos *pli;

			pl = pi->plugin;
			pli = &(pl->info);
			
			/* set class-handle in the auto-create structure(s) */
			pli->create_class[0].clh = opt->clh;
			if (opt->num_events > 0)
			{
				pli->create_class[0].event_ids = ra_alloc_mem(sizeof(long) * opt->num_events);
				memcpy(pli->create_class[0].event_ids, opt->events, sizeof(long) * opt->num_events);
				pli->create_class[0].num_events = opt->num_events;

				for (l = 0; l < num_create_prop; l++)
					pli->create_prop[l].use_mass_insert = 0;
			}

			ret = ra_eval_save_result(opt->eh, NULL, pi, 0);

			if (pli->create_class[0].event_ids)
				ra_free_mem(pli->create_class[0].event_ids);
		}
		else
			ret = ra_eval_save_result(opt->eh, NULL, pi, 0);
	}

	return ret;
} /* store_results() */


int
store_single_value(struct ra_detect *opt, struct beats2_mem *beats_mem, int *ch_in_sig, long value_idx,
		   double *ch_indep_value, int calc_offset, int type,
		   short *sbuf, long *lbuf, double *dbuf, long *ch, long num_values,
		   long res_ch_idx, long res_idx, long res_number, value_handle *res)
{
	int ret = 0;
	long l, cnt;
	int i;
	struct beats2 *curr;
	long l_value;
	double d_value;

	cnt = 0;
	l = 0;
	if (ch_indep_value != NULL)
	{
		curr = get_first_beat(beats_mem);
		while (curr)
		{
			long pos = GET_BEAT_POS(curr);

			if (calc_offset)
			{
				if (ch_indep_value[l] >= 0)
					sbuf[cnt] = (short)(ch_indep_value[l] - pos);
				else
					sbuf[cnt] = INVALID_OFFSET_VALUE;
			}
			else
			{
				if (ch_indep_value[l] >= 0)
					dbuf[cnt] = ch_indep_value[l];
				else
					dbuf[cnt] = -1.0;
			}
			
			ch[cnt] = -1;
			cnt++;
			curr = curr->next;

			l++;
		}
	}

	for (i = 0; i < opt->num_ch; i++)
	{
		curr = get_first_beat(beats_mem);
		while ((curr != NULL) && (cnt < num_values))
		{
			long in_ch, pos;
			
			in_ch = GET_BEAT_IN_CH(curr, i);
			pos = GET_BEAT_POS(curr);
			
			if (calc_offset)
			{
				l_value = BEAT_VALUE_GET_LONG(curr, value_idx, i);
				if (in_ch && (l_value >= 0))
					sbuf[cnt] = (short)(l_value - pos);
				else
					sbuf[cnt] = INVALID_OFFSET_VALUE;
			}
			else
			{
				switch (type)
				{
				case RA_VALUE_TYPE_LONG:
					l_value = BEAT_VALUE_GET_LONG(curr, value_idx, i);
					if (in_ch && (l_value >= 0))
						lbuf[cnt] = l_value;
					else
						lbuf[cnt] = -1;
					break;
				case RA_VALUE_TYPE_DOUBLE:
					d_value = BEAT_VALUE_GET_DOUBLE(curr, value_idx, i);
					if (in_ch && (d_value >= 0))
						dbuf[cnt] = d_value;
					else
						dbuf[cnt] = -1.0;
					break;
				}
			}

			ch[cnt] = ch_in_sig[i];
			cnt++;
			curr = curr->next;
		}
	}

	if (cnt > 0)
	{
		if (res_ch_idx >= 0)
			ra_value_set_long_array(res[res_ch_idx], ch, cnt);

		if (calc_offset)
			ra_value_set_short_array(res[res_idx], sbuf, cnt);
		else if (type == RA_VALUE_TYPE_LONG)
			ra_value_set_long_array(res[res_idx], lbuf, cnt);
		else if (type == RA_VALUE_TYPE_DOUBLE)
			ra_value_set_double_array(res[res_idx], dbuf, cnt);
		else
			ret = -1;

		ra_value_set_number(res[res_idx], res_number);
	}

	return ret;
} /* store_single_value() */
