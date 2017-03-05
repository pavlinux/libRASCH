/*----------------------------------------------------------------------------
 * ap_morphology.c  -  get systolic and diastolyc values and positions of bloodpressure wave
 *
 * Description:
 * The plugin get systolic and diastolic values and positions from bloodpressure
 * waves. Additionally it tries to find calibration-intervals (eg in the Portapres
 * system it is possible to enable calibration during the measurement).
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

#define _LIBRASCH_BUILD
#include <ra.h>
#include <ra_priv.h>
#include <pl_general.h>
#include "ap_morphology.h"


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
	if (ra_plugin_get_by_name(ra, "gui-rr-morphology", 1))
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
	if (!proc)
		return -1;

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
	pi->options = calloc(1, sizeof(struct ra_ap_morphology));
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
set_default_options(struct ra_ap_morphology *opt)
{
	opt->eh = NULL;

	opt->use_class = 1;
	opt->clh = NULL;
	opt->clh_calib = NULL;
	opt->save_in_class = 1;

	opt->num_ch = -1;
	opt->ch = NULL;

	opt->num_pos = 0;
	opt->pos = NULL;

	opt->pos_are_region = 0;
	opt->pos_are_event_id = 0;
	opt->rr_pos = 0;

	opt->dont_search_calibration = 0;
	opt->dont_search_positions = 0;
} /* set_default_options() */


void
set_option_offsets(struct ra_option_infos *opt_inf, struct ra_ap_morphology *opt)
{
	unsigned char *p = (unsigned char *)opt;
	int idx = 0;

	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->eh) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->use_class) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->clh) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->clh_calib) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->save_in_class) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->num_ch) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->ch) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->num_pos) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->pos) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->pos_are_region) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->pos_are_event_id) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->rr_pos) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->dont_search_calibration) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->dont_search_positions) - p);

	/* consistency check (useful when changing option structure) */
	if (idx != num_options)
	{
		fprintf(stderr, "critical error in ap_morphology.c - set_option_offsets():\n"
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
		struct ra_ap_morphology *opt;

		opt = (struct ra_ap_morphology *)pi->options;
		if (opt->pos)
			ra_free_mem(opt->pos);
		if (opt->ch)
			ra_free_mem(opt->ch);
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
	struct ra_ap_morphology *opt;
	int rr_pos = 1;
	long num_ch, l, m;
	struct ch_data *ch = NULL;
	int one_ch_no_data = 0;
	value_handle vh;
	double x_scale = 1.0;
	struct plugin_infos *pli;
	value_handle *res;

	if (!pi)
		return -1;

	opt = (struct ra_ap_morphology *)pi->options;
	if (opt == NULL)
	{
		_ra_set_error(proc, RA_ERR_INFO_MISSING, "plugin-ap-morphology");
		return -1;
	}

	if (opt->eh == NULL)
		opt->eh = ra_eval_get_default(pi->mh);
	if (pi->rh == NULL)
		pi->rh = ra_rec_get_first(pi->mh, 0);
	if (opt->use_class && (opt->clh == NULL))
	{
		opt->clh = get_event_class(pi);
		if (opt->clh == NULL)
			return -1;
	}

	num_ch = init_ch_data(opt, pi->rh, &ch);

	if (num_ch <= 0)
		return -1;

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

	/* get "type" of positions */
	get_pos_property(opt->clh, &rr_pos);

	vh = ra_value_malloc();
	for (l = 0; l < num_ch; l++)
	{
		ch[l].num_morph = get_data(opt, ch[l].ch, &(ch[l].morph));
		if (ch[l].num_morph <= 0)
		{
			one_ch_no_data = 1;
			continue;
		}

		/* scale positions for rr-channel */
		ra_value_set_number(vh, ch[l].ch);
		ra_info_get(pi->mh, RA_INFO_CH_XSCALE_D, vh);
		x_scale = ra_value_get_double(vh);
		for (m = 0; m < ch[l].num_morph; m++)
		{
			ch[l].morph[m].pos = (long)((double)(ch[l].morph[m].pos) * x_scale);
			ch[l].morph[m].systolic_pos = (long)((double)(ch[l].morph[m].systolic_pos) * x_scale);
			ch[l].morph[m].diastolic_pos = (long)((double)(ch[l].morph[m].diastolic_pos) * x_scale);
			ch[l].morph[m].dpdt_min_pos = (long)((double)(ch[l].morph[m].dpdt_min_pos) * x_scale);
			ch[l].morph[m].dpdt_max_pos = (long)((double)(ch[l].morph[m].dpdt_max_pos) * x_scale);
		}


		/* first find rr values */
		ret = morphology_rr(pi->rh, ch[l].ch, ch[l].num_morph, ch[l].morph, rr_pos,
				    opt->dont_search_positions, pi->callback);

		/* and now search for calibration-sequences (calibration is done
		   in Portapres recordings) */
		ch[l].calib.num = 0;
		ch[l].calib.seq = NULL;
		if (!opt->dont_search_calibration)
		{
			ret = find_calibration(pi->rh, ch[l].ch, &(ch[l].calib), pi->callback);
			complete_calib_infos(&(ch[l].calib), ch[l].morph, ch[l].num_morph);
		}
		else
		{
			read_calibration(opt, opt->clh, ch[l].ch, &(ch[l].calib));
			for (m = 0; m < ch[l].calib.num; m++)
			{
				ch[l].calib.seq[m].start = (long)((double)(ch[l].calib.seq[m].start) * x_scale);
				ch[l].calib.seq[m].end = (long)((double)(ch[l].calib.seq[m].end) * x_scale);
			}
		}
		
		if (ch[l].calib.num > 0)
		{
			ret = get_bp_values_calibration(pi->rh, ch[l].ch, &(ch[l].calib),
							ch[l].morph, ch[l].num_morph, pi->callback);
		}
		
 		if (ret != 0)
 			goto error;
	}

	if (pi->callback)
		(*pi->callback)("save results", 0);

	save_in_res(pi, ch, num_ch, x_scale);

	if (pi->callback)
		(*pi->callback)(NULL, 20);

	if (opt->clh && opt->save_in_class)
	{
		if (pi->callback)
			(*pi->callback)(gettext("save RR-values"), 0);

		if (opt->use_class)
		{
			pli = &(((struct plugin_struct *)(pi->plugin))->info);

			/* set class handles for auto-store function */
			pli->create_class[0].clh = opt->clh;
			if (ch[l].calib.num == 0)
				num_create_class = 1;
			else
				pli->create_class[1].clh = opt->clh_calib;
			
			ret = ra_eval_save_result(ra_eval_get_handle(opt->clh), NULL, pi, 0);
		}
		else
			ret = save_single(pi, ch, num_ch, x_scale);
	}

 error:
	ra_value_free(vh);

	if (ch)
	{
		for (l = 0; l < num_ch; l++)
		{
			if (ch[l].morph)
				free(ch[l].morph);
			if (ch[l].calib.seq)
				free(ch[l].calib.seq);
		}
		free(ch);
	}

	if ((ret == 0) && (one_ch_no_data))
	{
		_ra_set_error(proc, RA_WARN_NOT_ALL_PROCESSED, "plugin-ap-morphology");
		ret = -1;
	}

	return ret;
} /* pl_do_processing() */


long
init_ch_data(struct ra_ap_morphology *opt, rec_handle rh, struct ch_data **ch)
{
	long l, num;
	int ch_num = -1;

	num = 0;
	if (opt->num_ch > 0)
	{
		if (opt->ch == NULL)
		{
			_ra_set_error(rh, RA_ERR_INFO_MISSING, "plugin-ap-morphology");
			return -1;
		}

		num = opt->num_ch;

		*ch = calloc(num, sizeof(struct ch_data));
		for (l = 0; l < num; l++)
			(*ch)[l].ch = opt->ch[l];
	}
	else
	{
		while ((ch_num = get_rr_channel(rh, ch_num)) >= 0)
		{
			num++;
			*ch = realloc(*ch, sizeof(struct ch_data) * num);
			memset((*ch)+(num-1), 0, sizeof(struct ch_data));
			(*ch)[num-1].ch = ch_num;
		}
	}

	return num;
} /* init_ch_data() */


class_handle
get_event_class(struct proc_info *pi)
{
	class_handle clh = NULL;
	struct ra_ap_morphology *opt = pi->options;
	value_handle vh = NULL;

	vh = ra_value_malloc();
	if (ra_class_get(opt->eh, "heartbeat", vh) != 0)
		goto error;
		
	clh = (class_handle)(ra_value_get_voidp_array(vh)[0]);
	if (clh == NULL)
	{
		_ra_set_error(pi->mh, RA_ERR_ERROR, "plugin-ap-morphology");
		goto error;
	}	

 error:
	if (vh)
		ra_value_free(vh);

	return clh;
} /* get_event_class() */


long
get_data(struct ra_ap_morphology *opt, long ch, struct morph_struct **morph)
{
	long num = 0;
	long l;

	if (!opt)
		return 0;

	if (opt->pos_are_region)  /* must be the first if() */
		num = get_beats_in_region(opt->clh, ch, morph, opt->pos, opt->num_pos);
	else if (opt->use_class && opt->clh)
		num = get_all_data_from_eval(opt->clh, ch, morph);
	else if (opt->pos_are_event_id)
		num = get_specific_data_from_eval(opt->clh, ch, morph, opt->pos, opt->num_pos);
	else
	{
		num = opt->num_pos;
		(*morph) = malloc(sizeof(struct morph_struct) * num);
		memset((*morph), 0, sizeof(struct morph_struct) * num);

		for (l = 0; l < num; l++)
		{
			(*morph)[l].pos = opt->pos[l];
			(*morph)[l].event_index = -1;
			(*morph)[l].event_id = -1;
		}
	}

	return num;
}  /* get_data() */


long
get_all_data_from_eval(class_handle clh, long ch, struct morph_struct **morph)
{
	prop_handle prop;
	int rr_pos;
	long num, l, ch_use;
	value_handle vh, vh_val;
	const long *ev_id;

	prop = get_pos_property(clh, &rr_pos);
	if (!prop)
		return -1;

	vh = ra_value_malloc();
	ra_class_get_events(clh, -1, -1, 0, 1, vh);
	num = ra_value_get_num_elem(vh);
	ev_id = ra_value_get_long_array(vh);

	(*morph) = malloc(sizeof(struct morph_struct) * num);
	memset((*morph), 0, sizeof(struct morph_struct) * num);

	ch_use = -1;
	if (rr_pos)
		ch_use = ch;

	vh_val = ra_value_malloc();
	for (l = 0; l < num; l++)
	{
		ra_prop_get_value(prop, ev_id[l], ch_use, vh_val);

		(*morph)[l].pos = ra_value_get_long(vh_val);
		(*morph)[l].event_index = l;
		(*morph)[l].event_id = ev_id[l];
		get_beat_data(clh, ch, (*morph)+l, ev_id[l]);

		/* it happens sometimes (e.g. BPH_4_hrv_.txt) that pos is negative
		   TODO: check why a negative position is stored in the first place */
		if ((*morph)[l].pos < 0)
			(*morph)[l].pos = 0;
	}

	ra_value_free(vh);
	ra_value_free(vh_val);

	return num;
}  /* get_all_data_from_eval() */


long
get_specific_data_from_eval(class_handle clh, long ch, struct morph_struct **morph, long *event_pos, long num_pos)
{
	prop_handle prop;
	int rr_pos;
	long num, l;
	long *pos_use = NULL;
	int *pos_ok = NULL;

	prop = get_pos_property(clh, &rr_pos);
	if (!prop)
		return -1;

	/* get positions from evaluation */
	num = num_pos;
	if (num == 1)
	{
		pos_use = malloc(sizeof(long) * 3);
		pos_ok = malloc(sizeof(long) * 3);

		pos_use[num-1] = ra_class_get_prev_event(clh, event_pos[0]);
		pos_ok[num-1] = 1;
		if (pos_use[num-1] != -1)
			num++;

		pos_use[num-1] = event_pos[0];
		pos_ok[num-1] = 0;

		pos_use[num] = ra_class_get_next_event(clh, event_pos[0]);
		pos_ok[num] = 1;
		if (pos_use[num] != -1)
			num++;
	}
	else
		pos_use = event_pos;

	(*morph) = calloc(num, sizeof(struct morph_struct));
	for (l = 0; l < num; l++)
	{
		get_beat_data(clh, ch, (*morph)+l, pos_use[l]);
		(*morph)[l].event_index = l;
	}

	if (num_pos == 1)
	{
		for (l = 0; l < num; l++)
			(*morph)[l].dont_search_pos = pos_ok[l];
	}

	if (num_pos == 1)
	{
		free(pos_use);
		free(pos_ok);
	}

	return num;
}  /* get_specific_data_from_eval() */


void
get_beat_data(class_handle clh, long ch, struct morph_struct *morph, long event)
{
	int rr_pos;
	long ch_use;
	prop_handle pos, syst, dias, mean, syst_pos, dias_pos, flags, ibi;
	prop_handle dpdt_min, dpdt_max, dpdt_min_pos, dpdt_max_pos;
	value_handle vh;
	syst = dias = mean = syst_pos = dias_pos = flags = ibi = NULL;
	dpdt_min = dpdt_max = dpdt_min_pos = dpdt_max_pos = NULL;

	pos = get_pos_property(clh, &rr_pos);

	syst = ra_prop_get(clh, "rr-systolic");
	dias = ra_prop_get(clh, "rr-diastolic");
	mean = ra_prop_get(clh, "rr-mean");
	syst_pos = ra_prop_get(clh, "rr-systolic-pos");
	dias_pos = ra_prop_get(clh, "rr-diastolic-pos");
	flags = ra_prop_get(clh, "rr-flags");
	ibi = ra_prop_get(clh, "ibi");
	dpdt_min = ra_prop_get(clh, "rr-dpdt-min");
	dpdt_max = ra_prop_get(clh, "rr-dpdt-max");
	dpdt_min_pos = ra_prop_get(clh, "rr-dpdt-min-pos");
	dpdt_max_pos = ra_prop_get(clh, "rr-dpdt-max-pos");

	vh = ra_value_malloc();
	ch_use = -1;
	if (rr_pos)
		ch_use = ch;
	if (ra_prop_get_value(pos, event, ch_use, vh) == 0)
		morph->pos = ra_value_get_long(vh);	
	morph->event_id = event;

	if (syst)
	{
		if (ra_prop_get_value(syst, event, ch, vh) == 0)
			morph->systolic = ra_value_get_double(vh);
	}
	if (dias)
	{
		if (ra_prop_get_value(dias, event, ch, vh) == 0)
			morph->diastolic = ra_value_get_double(vh);
	}
	if (mean)
	{
		if (ra_prop_get_value(mean, event, ch, vh) == 0)
			morph->mean = ra_value_get_double(vh);
	}
	if (syst_pos)
	{
		if (ra_prop_get_value(syst_pos, event, ch, vh) == 0)
			morph->systolic_pos = ra_value_get_long(vh);
	}
	if (dias_pos)
	{
		if (ra_prop_get_value(dias_pos, event, ch, vh) == 0)
			morph->diastolic_pos = ra_value_get_long(vh);
	}
	if (flags)
	{
		if (ra_prop_get_value(flags, event, ch, vh) == 0)
			morph->flags = ra_value_get_long(vh);
	}
	if (ibi)
	{
		if (ra_prop_get_value(ibi, event, ch, vh) == 0)
			morph->ibi = ra_value_get_double(vh);
	}
	if (dpdt_min)
	{
		if (ra_prop_get_value(dpdt_min, event, ch, vh) == 0)
			morph->dpdt_min = ra_value_get_double(vh);
	}
	if (dpdt_max)
	{
		if (ra_prop_get_value(dpdt_max, event, ch, vh) == 0)
			morph->dpdt_max = ra_value_get_double(vh);
	}
	if (dpdt_min_pos)
	{
		if (ra_prop_get_value(dpdt_min_pos, event, ch, vh) == 0)
			morph->dpdt_min_pos = ra_value_get_long(vh);
	}
	if (dpdt_max_pos)
	{
		if (ra_prop_get_value(dpdt_max_pos, event, ch, vh) == 0)
			morph->dpdt_max_pos = ra_value_get_long(vh);
	}
}  /* get_beat_data() */


long
get_beats_in_region(class_handle clh, long ch, struct morph_struct **morph, long *pos, long num_pos)
{
	long l, num, num_event, s, e, prev_event;
	value_handle vh;
	const long *ev_ids;
	prop_handle beat_pos;
	int dummy;

	if (num_pos != 2)
		return 0;

	beat_pos = get_pos_property(clh, &dummy);
	if (!pos)
		return -1;

	/* get event-id's from evaluation */
	vh = ra_value_malloc();
	ra_class_get_events(clh, -1, -1, 0, 1, vh);
	num_event = ra_value_get_num_elem(vh);
	ev_ids = ra_value_get_long_array(vh);

	/* check ALL events because we need to set the event before and
	   after the region */
	(*morph) = NULL;
	num = 0;
	for (l = 0; l < num_event; l++)
	{
		ra_class_get_event_pos(clh, ev_ids[l], &s, &e);

		if ((s >= pos[0]) && (e <= pos[1]))
		{
			prev_event = ra_class_get_prev_event(clh, ev_ids[l]);

			if ((num == 0) && (prev_event >= 0))
			{
				/* add beat before region */
				num = add_beat(clh, ch, morph, num, prev_event);
			}
		}

		if (e > pos[1])
		{
			prev_event = ra_class_get_prev_event(clh, ev_ids[l]);

			/* first check if a beat was already selected */
			if ((num == 0) && (prev_event >= 0))
			{
				/* no, so add beat before region */
				num = add_beat(clh, ch, morph, num, prev_event);
			}

			/* add beat after region */
			num = add_beat(clh, ch, morph, num, ev_ids[l]);
			break;
		}

		num = add_beat(clh, ch, morph, num, ev_ids[l]);
	}
	ra_value_free(vh);

	for (l = 0; l < num; l++)
		(*morph)[l].event_index = l;

	return num;
}  /* get_beats_in_region() */


long
add_beat(class_handle clh, long ch, struct morph_struct **morph, long num, long curr_event)
{
	(*morph) = realloc((*morph), sizeof(struct morph_struct) * (num + 1));
	memset((*morph) + num, 0, sizeof(struct morph_struct));
	get_beat_data(clh, ch, (*morph) + num, curr_event);

	return (num + 1);
} /* add_beat() */


prop_handle
get_pos_property(class_handle clh, int *rr_pos)
{
	prop_handle pos = NULL;

	/* first check if rr-positions available */
	*rr_pos = 1;
	pos = ra_prop_get(clh, "rr-pos");
	if (!pos)
	{
		*rr_pos = 0;
		/* no -> now check for qrs-positions */
		pos = ra_prop_get(clh, "qrs-pos");
	}

	if (!pos)
		_ra_set_error(clh, RA_ERR_EV_PROP_MISSING, "plugin-ap-morphology");

	return pos;
}  /* get_pos_property() */


int
morphology_rr(rec_handle rh, long rr_channel, long num, struct morph_struct *morph, int rr_pos,
	      int dont_search_pos, void (*callback) (const char *, int))
{
	double samplerate = 1.0;
	long pos_offset = 0;
	int percent_save = 0;
	char s[100];
	int cnt = 1;
	long l;
	
	if (callback)
	{
		sprintf(s, gettext("get bloodpressure values"));
		(*callback) (s, 0);
	}

	samplerate = get_samplerate(rh, rr_channel);

	/* check if positions are blood-pressure wave (rr_pos != 0)
	   or QRS-complexes (rr_pos == 0) and choose from what point
	   raw-data should be used for looking for blood-pressure-wave */
	if (rr_pos)
		pos_offset = -100;	/* -100 ms */
	else
		pos_offset = -50;		/* 0 ms */
	/*	pos_offset = 100;	/ * +100 ms */
	pos_offset = (long) ((double) pos_offset * samplerate / 1000.0);

	for (l = 0; l < num; l++)
	{
		int percent = (int) (((double) cnt / (double) num) * 100.0);
		if ((percent != percent_save) && callback)
		{
			(*callback) (s, percent);
			percent_save = percent;
		}

		if (!dont_search_pos && !morph[l].dont_search_pos)
			process_beat(rh, morph, num, l, pos_offset, rr_channel, samplerate);

		if (l >= 1)
		{
			/* now calc mean-pressure and inter-beat-interval for previous beat */
			morph[l-1].mean = calc_mean_pressure(rh, rr_channel, morph[l-1].diastolic_pos, morph[l].diastolic_pos);
			morph[l-1].ibi = (double)(morph[l].diastolic_pos - morph[l-1].diastolic_pos) / samplerate * 1000.0; /* in msec */
		}

		cnt++;
	}

	return 0;
} /* morphology_rr() */


double
get_samplerate(rec_handle rh, int ch)
{
	value_handle vh;
	double samplerate = 1.0;

	vh = ra_value_malloc();
	ra_value_set_number(vh, ch);
	if (ra_info_get(rh, RA_INFO_REC_CH_SAMPLERATE_D, vh) == 0)
		samplerate = ra_value_get_double(vh);
	ra_value_free(vh);

	return samplerate;
}  /* get_samplerate() */


void
process_beat(rec_handle rh, struct morph_struct *morph, long num, long curr, long pos_offset, int ch, double samplerate)
{
	unsigned long len, n, n_save;
	double *buf = NULL;
	long curr_ibi = -1;
	unsigned long pos;
	unsigned long max_search_wnd; 

	/* get search-length (max. 500 msec) */
	max_search_wnd = (long)(0.5 * samplerate);
	len = max_search_wnd;
	if (curr < (num-1))
	{
		len = morph[curr+1].pos - morph[curr].pos;
		if ((len > max_search_wnd) || (len < 0))
			len = max_search_wnd;
	}

	/* get raw-data */
	buf = malloc(sizeof(double) * len);
	pos = morph[curr].pos + pos_offset;
	if (pos < 0)
	{
		len -= pos;
		pos = 0;
	}
	n = (unsigned long)ra_raw_get_unit(rh, ch, pos, len, buf);
	n_save = n;

	/* find systole */
	morph[curr].systolic_pos = find_systole(buf, n, &(morph[curr].systolic));
	morph[curr].systolic_pos += pos;

	/* find diastole */
	if (curr > 0)
	{
		curr_ibi = morph[curr].systolic_pos - morph[curr-1].systolic_pos;
		curr_ibi = (long)(((double)curr_ibi / samplerate) * 1000.0);
	}

	n = morph[curr].systolic_pos - pos;
	if (n < 0)
		morph[curr].diastolic_pos = pos;
	else
	{
		morph[curr].diastolic_pos = find_diastole(buf, n, &(morph[curr].diastolic), samplerate,	curr_ibi);
		morph[curr].diastolic_pos += pos;
	}

	/* get dp/dt values */
	find_dpdt_values(buf, n_save, samplerate, &(morph[curr].dpdt_min), &(morph[curr].dpdt_max),
		&(morph[curr].dpdt_min_pos), &(morph[curr].dpdt_max_pos));
	morph[curr].dpdt_min_pos += pos;
	morph[curr].dpdt_max_pos += pos;

	if (buf)
		free(buf);
}  /* process_beat() */


int
get_rr_channel(rec_handle rh, int prev_rr_ch)
{
	value_handle vh;
	int num_ch, i;
	int ch = -1;

	vh = ra_value_malloc();
	if (ra_info_get(rh, RA_INFO_REC_GEN_NUM_CHANNEL_L, vh) == 0)
		num_ch = ra_value_get_long(vh);
	else
		num_ch = 0;

	for (i = 0; i < num_ch; i++)
	{
		ra_value_set_number(vh, i);
		if (ra_info_get(rh, RA_INFO_REC_CH_TYPE_L, vh) != 0)
			continue;

		if ((ra_value_get_long(vh) == RA_CH_TYPE_RR) && (i > prev_rr_ch))
		{
			ch = i;
			break;
		}
	}

	ra_value_free(vh);

	/* if RR channel was not found, set error only when no previous rr-channel was given */
	if ((ch < 0) && (prev_rr_ch < 0))
		_ra_set_error(rh, RA_ERR_SIGNAL_NOT_AVAIL, "plugin-ap-morphology");

	return ch;
} /* get_rr_channel() */


long
find_diastole(double *buf, unsigned long size, double *min_value, double samplerate, long curr_ibi)
{
	unsigned long i;
	unsigned long min_pos = 0;
	unsigned long max_pos = 0;
	unsigned long dias_pos = 0;
	unsigned int msec_20;
	double min, max, slope_sub, range, slope_threshold;

	msec_20 = (unsigned int)(0.02 * samplerate);
	min = buf[0];
	max = buf[0];
	for (i = 1; i < size; i++)
	{
		if (buf[i] < min)
		{
			min_pos = i;
			min = buf[i];
		}
		if (buf[i] > max)
		{
			max_pos = i;
			max = buf[i];
		}
	}
	dias_pos = min_pos;

	if (size <= msec_20)
		return dias_pos;

	/* find position where increase is above 2mmHg/20msec resp. 1mmHg/20msec
	   if inter-beat-interval is below 300msec; use this
	   values so the search is not so sensitive to noise */
	slope_sub = (500.0 - (double)curr_ibi) * 0.005;
	if (slope_sub < 0)
		slope_sub = 0.0;

	range = fabs(max - min);
	if (range > 80)
		slope_threshold = range / 40; /* 5% of pulse-pressure (when 120/80 5% is 2mmHg) */
	else
		slope_threshold = range / 20; /* 5% of pulse-pressure (when 120/80 5% is 2mmHg) */

	if (min_pos < (2*msec_20))
		min_pos = 0;
	else
		min_pos -= 2*msec_20;

	for (i = min_pos; i < (size - msec_20); i++)
	{
		double diff = buf[i+msec_20] - buf[i];
		if (diff <= 0)
			continue;

		if (diff > (slope_threshold - slope_sub))
		{
			dias_pos = i + msec_20;
			break;
		}
	}

	*min_value = buf[dias_pos];

	return dias_pos;
} /* find_diastole() */


long
find_systole(double *buf, unsigned long size, double *max_value)
{
	unsigned long i;
	unsigned long pos = 0;

	*max_value = buf[0];
	for (i = 0; i < size; i++)
	{
		if (*max_value < buf[i])
		{
			*max_value = buf[i];
			pos = i;
		}
	}

	return pos;
} /* find_systole() */


double
calc_mean_pressure(rec_handle rec, int ch, long start, long end)
{
	double mean = 0.0;
	long l, n, buf_size = end - start;
	double *buf = NULL;
	
	if ((end <= start) || (ch < 0) || (start < 0) || (end < 0))
		return mean;

	buf = malloc(sizeof(double) * buf_size);
	if (!buf)
		goto clean_up;

	n = (unsigned long)ra_raw_get_unit(rec, ch, start, buf_size, buf);
	if (n <= 0)
		goto clean_up;

	for (l = 0; l < n; l++)
		mean += buf[l];
	mean /= (double)n;

 clean_up:
	if (buf)
		free(buf);

	return mean;
}  /* calc_mean_pressure() */


int
find_dpdt_values(double *buf, unsigned long size, double samplerate,
				 double *min_value, double *max_value, long *min_pos, long *max_pos)
{
	double dpdt;
	unsigned long i;

	*min_value = *max_value = 0.0;
	*min_pos = *max_pos = 0;

	if (size < 5)
		return -1;

	/* Central differences formula, fourth order accurate:
           f'(i) = (-f(i+2) + 8f(i+1) - 8f(i-1) + f(i-2)) / (12h) + O(h^4)
           if central
             ddt_cnt = ...
               (-data_cnt(5:end) + 8*data_cnt(4:end-1) - 8*data_cnt(2:end-3) ...
               +data_cnt(1:end-4)) * ( ch_samplerates(4) / 12);
           end
        */
	for (i = 2; i < (size - 2); i++)
	{

		dpdt = -buf[i+2] + 8*buf[i+1] - 8*buf[i-1] + buf[i-2];
		dpdt /= (samplerate / 12);

		if (dpdt < *min_value)
		{
			*min_value = dpdt;
			*min_pos = i;
		}

		if (dpdt > *max_value)
		{
			*max_value = dpdt;
			*max_pos = i;
		}
	}

	return 0;
}  /* find_dpdt_values() */


int
find_calibration(rec_handle rh, long rr_channel, struct calib_info *calib, void (*callback) (const char *, int))
{
	long pos = 0;
	double buf[RAW_BUF_SIZE];
	long num_samples;
	value_handle vh;
	int in_calib_seq = 0;
	int no_change;
	double last = 0.0;
	long start, end;
	double samplerate;
	long min_calib_duration;
	int percent_save = -1;

	if (callback)
	{
		char s[100];
		strcpy(s, "find calibration sequences");
		(*callback) (s, 0);
	}


	vh = ra_value_malloc();
	ra_value_set_number(vh, rr_channel);
	if (ra_info_get(rh, RA_INFO_REC_CH_NUM_SAMPLES_L, vh) != 0)
		return -1;
	num_samples = ra_value_get_long(vh);

	samplerate = 0;
	if (ra_info_get(rh, RA_INFO_REC_CH_SAMPLERATE_D, vh) == 0)
		samplerate = ra_value_get_double(vh);
	ra_value_free(vh);
	min_calib_duration = (long)(0.5 * samplerate);	/* 500 msec */

	start = 0;
	no_change = 0;
	for (pos = 0; pos < num_samples; pos += RAW_BUF_SIZE)
	{
		long n, m;

		if (callback)
		{
			int percent = (int) ((double) pos * 100.0 / (double) num_samples);
			if (percent != percent_save)
			{
				(*callback) (NULL, percent);
				percent_save = percent;
			}
		}

		n = (unsigned long)ra_raw_get_unit(rh, rr_channel, pos, RAW_BUF_SIZE, buf);
		for (m = 0; m < n; m++)
		{
			if ((buf[m] > (last + 3.0)) || (buf[m] < (last - 3.0)))
			{
				if (in_calib_seq)
				{
					end = pos + m - 1;
					add_calibration(calib, start, end, rr_channel, samplerate);
				}
				start = pos + m;
				no_change = 0;
				in_calib_seq = 0;
				last = buf[m];
			}
			else
				no_change++;
			if (no_change >= min_calib_duration)
				in_calib_seq = 1;
		}
	}

	return 0;
} /* find_calibration() */


int
add_calibration(struct calib_info *calib, long start, long end, long channel, double samplerate)
{
	long max_dist = (long) samplerate;	/* distance of calibration sequences below this are one sequence */

	/* is it first calibration-seq or is it new a new one ? */
	if ((calib->num <= 0) || (calib->seq[calib->num - 1].end < (start - max_dist)))
	{
		calib->seq = realloc(calib->seq, sizeof(struct calibration) * (calib->num + 1));
		calib->seq[calib->num].start = start;
		calib->seq[calib->num].end = end;
		calib->seq[calib->num].channel = channel;
		calib->num += 1;
	}
	else
	{
		/* no -> current sequence belongs to last one */
		if (calib->seq[calib->num - 1].end > (start - max_dist))
			calib->seq[calib->num - 1].end = end;
	}

	return 0;
} /* add_calibration() */


int
complete_calib_infos(struct calib_info *calib, struct morph_struct *morph, long num)
{
	long l, last_idx;

	last_idx = 0;
	for (l = 0; l < calib->num; l++)
	{
		long before, after;

		before = find_beat(morph, num, last_idx, calib->seq[l].start, 0);
		if (before < 0)
			continue;
		calib->seq[l].beat_before = morph[before].event_id;
		calib->seq[l].beat_before_idx = morph[before].event_index;;

		after = find_beat(morph, num, last_idx, calib->seq[l].end, 1);
		if (after < 0)
			continue;
		calib->seq[l].beat_after = morph[after].event_id;
		calib->seq[l].beat_after_idx = morph[after].event_index;;
		last_idx = after;

		if ((after - before) < 2)
			continue;

		if (before < num)
		{
			calib->seq[l].pos_before = morph[before].pos;
			calib->seq[l].syst_before = morph[before].systolic;
			calib->seq[l].syst_pos_before = morph[before].systolic_pos;
			calib->seq[l].dias_before = morph[before].diastolic;
			calib->seq[l].dias_pos_before = morph[before].diastolic_pos;
			calib->seq[l].mean_before = morph[before].mean;
			calib->seq[l].ibi_before = morph[before].ibi;
			calib->seq[l].dpdt_min_before = morph[before].dpdt_min;
			calib->seq[l].dpdt_max_before = morph[before].dpdt_max;
			calib->seq[l].dpdt_min_pos_before = morph[before].dpdt_min_pos;
			calib->seq[l].dpdt_max_pos_before = morph[before].dpdt_max_pos;
		}

		if (after < num)
		{
			calib->seq[l].pos_after = morph[after].pos;
			calib->seq[l].syst_after = morph[after].systolic;
			calib->seq[l].syst_pos_after = morph[after].systolic_pos;
			calib->seq[l].dias_after = morph[after].diastolic;
			calib->seq[l].dias_pos_after = morph[after].diastolic_pos;
			calib->seq[l].mean_after = morph[after].mean;
			calib->seq[l].ibi_after = morph[after].ibi;
			calib->seq[l].dpdt_min_after = morph[before].dpdt_min;
			calib->seq[l].dpdt_max_after = morph[before].dpdt_max;
			calib->seq[l].dpdt_min_pos_after = morph[before].dpdt_min_pos;
			calib->seq[l].dpdt_max_pos_after = morph[before].dpdt_max_pos;
		}
	}

	return 0;
} /* complete_calib_infos() */


long
find_beat(struct morph_struct *morph, long num, long last_index, long pos, int after_pos)
{
	long l;
	long idx = last_index;

	for (l = last_index; l < num; l++)
	{
		if (!after_pos && (morph[l].systolic_pos > pos))
		{
			idx--;
			break;
		}
		if (after_pos && (morph[l].pos > pos) && !(morph[l].flags & AP_VALUE_INTERPOLATED))
		{
			idx = l;
			break;
		}

		if (!(morph[l].flags & AP_VALUE_INTERPOLATED))
			idx = l;
	}

	return idx;
} /* find_beat() */


int
get_bp_values_calibration(rec_handle rh, int rr_channel, struct calib_info *calib, struct morph_struct *morph,
			  long num, void (*callback) (const char *, int))
{
	long l, last_calib_seq;
	int percent_save = -1;
	double samplerate = 1.0;

	if (callback)
	{
		char s[100];
		strcpy(s, "interpolate calibration-sequences");
		(*callback) (s, 0);
	}

	last_calib_seq = 0;
	for (l = 0; l < num; l++)
	{
		long idx, before, after;
		double syst_diff, dias_diff, mean_diff, ibi_diff, syst_pos_diff, dias_pos_diff;
		double dpdt_min_diff, dpdt_max_diff, dpdt_min_pos_diff, dpdt_max_pos_diff;

		if (callback)
		{
			int percent = (int) ((double) l * 100.0 / (double) num);
			if (percent != percent_save)
			{
				(*callback) (NULL, percent);
				percent_save = percent;
			}
		}

		/* clear interpolated flag */
		morph[l].flags &= ~AP_VALUE_INTERPOLATED;

		if ((idx = get_calib_seq(morph[l].pos, calib, last_calib_seq)) < 0)
			continue;

		last_calib_seq = idx;

		before = calib->seq[idx].beat_before_idx;
		after = calib->seq[idx].beat_after_idx;

		syst_diff = (calib->seq[idx].syst_after - calib->seq[idx].syst_before) / (double)(after - before);
		dias_diff = (calib->seq[idx].dias_after - calib->seq[idx].dias_before) / (double)(after - before);
		mean_diff = (calib->seq[idx].mean_after - calib->seq[idx].mean_before) / (double)(after - before);
		ibi_diff = (calib->seq[idx].ibi_after - calib->seq[idx].ibi_before) / (double)(after - before);
		syst_pos_diff = (double)((calib->seq[idx].syst_pos_after - calib->seq[idx].pos_after) -
						(calib->seq[idx].syst_pos_before - calib->seq[idx].pos_before)) /
						(double)(after - before);
		dias_pos_diff = (double)((calib->seq[idx].dias_pos_after - calib->seq[idx].pos_after) -
						(calib->seq[idx].dias_pos_before - calib->seq[idx].pos_before)) /
						(double)(after - before);
		dpdt_min_diff = (calib->seq[idx].dpdt_min_after - calib->seq[idx].dpdt_min_before) / (double)(after - before);
		dpdt_max_diff = (calib->seq[idx].dpdt_max_after - calib->seq[idx].dpdt_max_before) / (double)(after - before);
		dpdt_min_pos_diff = (double)((calib->seq[idx].dpdt_min_pos_after - calib->seq[idx].pos_after) -
						(calib->seq[idx].dpdt_min_pos_before - calib->seq[idx].pos_before)) /
						(double)(after - before);
		dpdt_max_pos_diff = (double)((calib->seq[idx].dpdt_max_pos_after - calib->seq[idx].pos_after) -
						(calib->seq[idx].dpdt_max_pos_before - calib->seq[idx].pos_before)) /
						(double)(after - before);

		morph[l].systolic = calib->seq[idx].syst_before + (syst_diff * (morph[l].event_index - before));
		morph[l].diastolic = calib->seq[idx].dias_before + (dias_diff * (morph[l].event_index - before));
		morph[l].mean = calib->seq[idx].mean_before + (mean_diff * (morph[l].event_index - before));
		morph[l].ibi = calib->seq[idx].ibi_before + (ibi_diff * (morph[l].event_index - before));
 		morph[l].systolic_pos = morph[l].pos + (calib->seq[idx].syst_pos_before - calib->seq[idx].pos_before) +
			(long)(syst_pos_diff * (morph[l].event_index - before));
 		morph[l].diastolic_pos = morph[l].pos + (calib->seq[idx].dias_pos_before - calib->seq[idx].pos_before) +
			(long)(dias_pos_diff * (morph[l].event_index - before));
		morph[l].dpdt_min = calib->seq[idx].dpdt_min_before + (dpdt_min_diff * (morph[l].event_index - before));
		morph[l].dpdt_max = calib->seq[idx].dpdt_max_before + (dpdt_max_diff * (morph[l].event_index - before));
 		morph[l].dpdt_min_pos = morph[l].pos + (calib->seq[idx].dpdt_min_pos_before - calib->seq[idx].pos_before) +
			(long)(dpdt_min_pos_diff * (morph[l].event_index - before));
 		morph[l].dpdt_max_pos = morph[l].pos + (calib->seq[idx].dpdt_max_pos_before - calib->seq[idx].pos_before) +
			(long)(dpdt_max_pos_diff * (morph[l].event_index - before));

		/* set interpolated flag */
		morph[l].flags |= AP_VALUE_INTERPOLATED;
	}

	samplerate = get_samplerate(rh, rr_channel);

	for (l = 0; l < (num-1); l++)
	{
		morph[l].mean = calc_mean_pressure(rh, rr_channel, morph[l].diastolic_pos, morph[l+1].diastolic_pos);
		morph[l].ibi = (double)(morph[l+1].diastolic_pos - morph[l].diastolic_pos) / samplerate * 1000.0; /* in msec */
	}

	return 0;
} /* get_bp_values_calibration() */


long
get_calib_seq(long pos, struct calib_info *calib, long last_idx)
{
	long l;
	long ret = -1;

	for (l = last_idx; l < calib->num; l++)
	{
		if (pos < calib->seq[l].start)
			break;

		if (pos <= calib->seq[l].end)
		{
			ret = l;
			break;
		}
	}

	return ret;
} /* get_calib_seq() */


int
read_calibration(struct ra_ap_morphology *opt, class_handle clh, long ch, struct calib_info *calib)
{
	int ret = 0;
	prop_handle syst, dias, mean, ibi, dpdt_min, dpdt_max;
	prop_handle syst_pos, dias_pos, beat_pos, dpdt_min_pos, dpdt_max_pos;
	long l, idx, last_idx, num_beats;
	value_handle vh, vh_val, vh_beat;
	class_handle clh_calib;
	const long *ev_ids, *ev_ids_beat;

	vh = ra_value_malloc();

	clh_calib = opt->clh_calib;
	if (clh_calib == NULL)
	{
		ra_class_get(opt->eh, "rr-calibration", vh);
		if (ra_value_get_num_elem(vh) <= 0)
		{
			/* no calibration */
			ra_value_free(vh);
			return 0;
		}
		if (ra_value_get_num_elem(vh) > 1)
		{
			/* at the moment only one calibration class is supported
			   TODO: think about how to handle different calib-sequences 
			   in ap signals */
			ra_value_free(vh);
			return 0;
		}
	}	
	
	syst = ra_prop_get(clh, "rr-systolic");
	syst_pos = ra_prop_get(clh, "rr-systolic-pos");
	dias = ra_prop_get(clh, "rr-diastolic");
	dias_pos = ra_prop_get(clh, "rr-diastolic-pos");
	mean = ra_prop_get(clh, "rr-mean");
	ibi = ra_prop_get(clh, "ibi");
	dpdt_min = ra_prop_get(clh, "rr-dpdt-min");
	dpdt_max = ra_prop_get(clh, "rr-dpdt-max");
	dpdt_min_pos = ra_prop_get(clh, "rr-dpdt-min-pos");
	dpdt_max_pos = ra_prop_get(clh, "rr-dpdt-max-pos");

	/* first check if rr-postions available */
	beat_pos = ra_prop_get(clh, "rr-pos");
	if (!beat_pos)
		beat_pos = ra_prop_get(clh, "qrs-pos");
	
	if (!beat_pos || !syst_pos)
	{
		ra_value_free(vh);
		return -1;
	}

	ra_class_get_events(clh_calib, -1, -1, 0, 1, vh);
	calib->num = ra_value_get_num_elem(vh);
	ev_ids = ra_value_get_long_array(vh);
	calib->seq = malloc(sizeof(struct calibration) * calib->num);

	vh_beat = ra_value_malloc();
	ra_class_get_events(clh, -1, -1, 0, 1, vh_beat);
	num_beats = ra_value_get_num_elem(vh_beat);
	ev_ids_beat = ra_value_get_long_array(vh_beat);

	last_idx = 0;
	vh_val = ra_value_malloc();
	for (l = 0; l < calib->num; l++)
	{
		ret = ra_class_get_event_pos(clh_calib, ev_ids[l], &(calib->seq[l].start), &(calib->seq[l].end));
		if (ret != 0)
			break;
		calib->seq[l].channel = ch;

		idx = find_beat_eval(syst_pos, beat_pos, ch, ev_ids_beat, num_beats,
				     last_idx, calib->seq[l].start, 0);
		if (idx < 0)
		{
			ret = -1;
			break;
		}
		calib->seq[l].beat_before_idx = idx;
		calib->seq[l].beat_before = ev_ids_beat[idx];

		idx = find_beat_eval(syst_pos, beat_pos, ch, ev_ids_beat,
				     num_beats, last_idx, calib->seq[l].end, 1);
		if (idx < 0)
		{
			ret = -1;
			break;
		}
		calib->seq[l].beat_after_idx = idx;
		calib->seq[l].beat_after = ev_ids_beat[idx];

		ra_prop_get_value(beat_pos, calib->seq[l].beat_before, -1, vh_val);
		calib->seq[l].pos_before = ra_value_get_long(vh_val);

		ra_prop_get_value(syst, calib->seq[l].beat_before, ch, vh_val);
		calib->seq[l].syst_before = ra_value_get_double(vh_val);

		ra_prop_get_value(syst_pos, calib->seq[l].beat_before, ch, vh_val);
		calib->seq[l].syst_pos_before = ra_value_get_long(vh_val);

		ra_prop_get_value(dias, calib->seq[l].beat_before, ch, vh_val);
		calib->seq[l].dias_before = ra_value_get_double(vh_val);

		ra_prop_get_value(dias_pos, calib->seq[l].beat_before, ch, vh_val);
		calib->seq[l].dias_pos_before = ra_value_get_long(vh_val);

		ra_prop_get_value(mean, calib->seq[l].beat_before, ch, vh_val);
		calib->seq[l].mean_before = ra_value_get_double(vh_val);

		ra_prop_get_value(ibi, calib->seq[l].beat_before, ch, vh_val);
		calib->seq[l].ibi_before = ra_value_get_double(vh_val);

		ra_prop_get_value(dpdt_min, calib->seq[l].beat_before, ch, vh_val);
		calib->seq[l].dpdt_min_before = ra_value_get_double(vh_val);

		ra_prop_get_value(dpdt_max, calib->seq[l].beat_before, ch, vh_val);
		calib->seq[l].dpdt_max_before = ra_value_get_double(vh_val);

		ra_prop_get_value(dpdt_min_pos, calib->seq[l].beat_before, ch, vh_val);
		calib->seq[l].dpdt_min_pos_before = ra_value_get_long(vh_val);

		ra_prop_get_value(dpdt_max_pos, calib->seq[l].beat_before, ch, vh_val);
		calib->seq[l].dpdt_max_pos_before = ra_value_get_long(vh_val);


		ra_prop_get_value(beat_pos, calib->seq[l].beat_after, -1, vh_val);
		calib->seq[l].pos_after = ra_value_get_long(vh_val);

		ra_prop_get_value(syst, calib->seq[l].beat_after, ch, vh_val);
		calib->seq[l].syst_after = ra_value_get_double(vh_val);

		ra_prop_get_value(syst_pos, calib->seq[l].beat_after, ch, vh_val);
		calib->seq[l].syst_pos_after = ra_value_get_long(vh_val);

		ra_prop_get_value(dias, calib->seq[l].beat_after, ch, vh_val);
		calib->seq[l].dias_after = ra_value_get_double(vh_val);

		ra_prop_get_value(dias_pos, calib->seq[l].beat_after, ch, vh_val);
		calib->seq[l].dias_pos_after = ra_value_get_long(vh_val);

		ra_prop_get_value(mean, calib->seq[l].beat_after, ch, vh_val);
		calib->seq[l].mean_after = ra_value_get_double(vh_val);

		ra_prop_get_value(ibi, calib->seq[l].beat_after, ch, vh_val);
		calib->seq[l].ibi_after = ra_value_get_double(vh_val);

		ra_prop_get_value(dpdt_min, calib->seq[l].beat_after, ch, vh_val);
		calib->seq[l].dpdt_min_after = ra_value_get_double(vh_val);

		ra_prop_get_value(dpdt_max, calib->seq[l].beat_after, ch, vh_val);
		calib->seq[l].dpdt_max_after = ra_value_get_double(vh_val);

		ra_prop_get_value(dpdt_min_pos, calib->seq[l].beat_after, ch, vh_val);
		calib->seq[l].dpdt_min_pos_after = ra_value_get_long(vh_val);

		ra_prop_get_value(dpdt_max_pos, calib->seq[l].beat_after, ch, vh_val);
		calib->seq[l].dpdt_max_pos_after = ra_value_get_long(vh_val);

 		last_idx = calib->seq[l].beat_before_idx;
	}

	ra_value_free(vh);
	ra_value_free(vh_val);
	ra_value_free(vh_beat);

	return ret;
} /* read_calibration() */


long
find_beat_eval(prop_handle syst_pos_prop, prop_handle beat_pos_prop, long ch, const long *ev_ids,
	       long num, long last_index, long pos, int after_pos)
{
	long l;
	long idx = -1;
	value_handle vh;

	vh = ra_value_malloc();
	for (l = last_index; l < num; l++)
	{
		long syst_pos, beat_pos;

		ra_prop_get_value(syst_pos_prop, ev_ids[l], ch, vh);
		syst_pos = ra_value_get_long(vh);

		ra_prop_get_value(beat_pos_prop, ev_ids[l], ch, vh);
		beat_pos = ra_value_get_long(vh);

		if (!after_pos && (syst_pos > pos))
		{
			idx--;	/* because some parts of the pulse wave immediately
				   before the calibration are inside the calibration */
			break;
		}
		if (after_pos && (beat_pos > pos))
		{
			idx = l;
			break;
		}

		idx = l;
	}

	return idx;
} /* find_beat_eval() */


int
save_in_res(struct proc_info *pi, struct ch_data *ch, long num_ch, double x_scale)
{
	long l, m, num_all, curr;
	long *buf = NULL;
	double *d_buf = NULL;
	value_handle *res;

	num_all = 0;
	for (l = 0; l < num_ch; l++)
		num_all += ch[l].num_morph;
	
	res = pi->results;
	buf = malloc(sizeof(long) * num_all);
	d_buf = malloc(sizeof(double) * num_all);
	
	/* set morphology results */
	curr = 0;
	for (l = 0; l < num_ch; l++)
	{
		for (m = 0; m < ch[l].num_morph; m++)
			buf[curr++] = ch[l].morph[m].event_id;
	}
	ra_value_set_long_array(res[EV_ID], buf, num_all);
	
	curr = 0;
	for (l = 0; l < num_ch; l++)
	{
		for (m = 0; m < ch[l].num_morph; m++)
			buf[curr++] = ch[l].ch;
	}
	ra_value_set_long_array(res[CH], buf, num_all);
	
	curr = 0;
	for (l = 0; l < num_ch; l++)
	{
		for (m = 0; m < ch[l].num_morph; m++)
			d_buf[curr++] = ch[l].morph[m].systolic;
	}
	ra_value_set_double_array(res[SYST], d_buf, num_all);
	
	curr = 0;
	for (l = 0; l < num_ch; l++)
	{
		for (m = 0; m < ch[l].num_morph; m++)
			d_buf[curr++] = ch[l].morph[m].diastolic;
	}
	ra_value_set_double_array(res[DIAS], d_buf, num_all);
	
	curr = 0;
	for (l = 0; l < num_ch; l++)
	{
		for (m = 0; m < ch[l].num_morph; m++)
			d_buf[curr++] = ch[l].morph[m].mean;
	}
	ra_value_set_double_array(res[MEAN], d_buf, num_all);
	
	curr = 0;
	for (l = 0; l < num_ch; l++)
	{
		for (m = 0; m < ch[l].num_morph; m++)
			buf[curr++] = (long)((double)ch[l].morph[m].systolic_pos / x_scale);
	}
	ra_value_set_long_array(res[SYST_POS], buf, num_all);
	
	curr = 0;
	for (l = 0; l < num_ch; l++)
	{
		for (m = 0; m < ch[l].num_morph; m++)
			buf[curr++] = (long)((double)ch[l].morph[m].diastolic_pos / x_scale);
	}
	ra_value_set_long_array(res[DIAS_POS], buf, num_all);
	
	curr = 0;
	for (l = 0; l < num_ch; l++)
	{
		for (m = 0; m < ch[l].num_morph; m++)
			buf[curr++] = ch[l].morph[m].flags;
	}
	ra_value_set_long_array(res[FLAGS], buf, num_all);
	
	curr = 0;
	for (l = 0; l < num_ch; l++)
	{
		for (m = 0; m < ch[l].num_morph; m++)
			d_buf[curr++] = ch[l].morph[m].ibi;
	}
	ra_value_set_double_array(res[IBI], d_buf, num_all);

	curr = 0;
	for (l = 0; l < num_ch; l++)
	{
		for (m = 0; m < ch[l].num_morph; m++)
			d_buf[curr++] = ch[l].morph[m].dpdt_min;
	}
	ra_value_set_double_array(res[DPDT_MIN], d_buf, num_all);
	
	curr = 0;
	for (l = 0; l < num_ch; l++)
	{
		for (m = 0; m < ch[l].num_morph; m++)
			d_buf[curr++] = ch[l].morph[m].dpdt_max;
	}
	ra_value_set_double_array(res[DPDT_MAX], d_buf, num_all);
	
	curr = 0;
	for (l = 0; l < num_ch; l++)
	{
		for (m = 0; m < ch[l].num_morph; m++)
			buf[curr++] = (long)((double)ch[l].morph[m].dpdt_min_pos / x_scale);
	}
	ra_value_set_long_array(res[DPDT_MIN_POS], buf, num_all);
	
	curr = 0;
	for (l = 0; l < num_ch; l++)
	{
		for (m = 0; m < ch[l].num_morph; m++)
			buf[curr++] = (long)((double)ch[l].morph[m].dpdt_max_pos / x_scale);
	}
	ra_value_set_long_array(res[DPDT_MAX_POS], buf, num_all);
	
	free(buf);
	free(d_buf);

	/* set calibration results */
	num_all = 0;
	for (l = 0; l < num_ch; l++)
		num_all += ch[l].calib.num;

	buf = malloc(sizeof(long) * num_all);

	curr = 0;
	for (l = 0; l < num_ch; l++)
	{
		for (m = 0; m < ch[l].calib.num; m++)
			buf[curr++] = ch[l].calib.seq[m].start;
	}
	ra_value_set_long_array(res[CALIB_BEGIN], buf, num_all);

	curr = 0;
	for (l = 0; l < num_ch; l++)
	{
		for (m = 0; m < ch[l].calib.num; m++)
			buf[curr++] = ch[l].calib.seq[m].end;
	}
	ra_value_set_long_array(res[CALIB_END], buf, num_all);

	curr = 0;
	for (l = 0; l < num_ch; l++)
	{
		for (m = 0; m < ch[l].calib.num; m++)
			buf[curr++] = ch[l].ch;
	}
	ra_value_set_long_array(res[CALIB_CH], buf, num_all);
	
	curr = 0;
	for (l = 0; l < num_ch; l++)
	{
		for (m = 0; m < ch[l].calib.num; m++)
			buf[curr++] = AP_CALIB_PHYSIOCAL;
	}
	ra_value_set_long_array(res[CALIB_INFO], buf, num_all);
	
	return 0;
} /* save_in_res() */


int
save_single(struct proc_info *pi, struct ch_data *ch, long num_ch, double x_scale)
{
	long l, m;
	struct ra_ap_morphology *opt;
	prop_handle syst, dias, mean, syst_pos, dias_pos, flags, ibi, calib_info;
	prop_handle dpdt_min, dpdt_max, dpdt_min_pos, dpdt_max_pos;
	value_handle vh;
	
	syst = dias = mean = syst_pos = dias_pos = flags = ibi = calib_info = NULL;
	dpdt_min = dpdt_max = dpdt_min_pos = dpdt_max_pos = NULL;

	opt = pi->options;

	if ((syst = ra_prop_get(opt->clh, "rr-systolic")) == NULL)
		return -1;
	if ((dias = ra_prop_get(opt->clh, "rr-diastolic")) == NULL)
		return -1;
	if ((mean = ra_prop_get(opt->clh, "rr-mean")) == NULL)
		return -1;
	if ((syst_pos = ra_prop_get(opt->clh, "rr-systolic-pos")) == NULL)
		return -1;
	if ((dias_pos = ra_prop_get(opt->clh, "rr-diastolic-pos")) == NULL)
		return -1;
	if ((flags = ra_prop_get(opt->clh, "rr-flags")) == NULL)
		return -1;
	if ((ibi = ra_prop_get(opt->clh, "ibi")) == NULL)
		return -1;
	if ((dpdt_min = ra_prop_get(opt->clh, "rr-dpdt-min")) == NULL)
		return -1;
	if ((dpdt_max = ra_prop_get(opt->clh, "rr-dpdt-max")) == NULL)
		return -1;
	if ((dpdt_min_pos = ra_prop_get(opt->clh, "rr-dpdt-min-pos")) == NULL)
		return -1;
	if ((dpdt_max_pos = ra_prop_get(opt->clh, "rr-dpdt-max-pos")) == NULL)
		return -1;

	if (opt->clh_calib)
	{
 		if ((calib_info = ra_prop_get(opt->clh_calib, "rr-calibration-info")) == NULL)
 			return -1;
	}

	vh = ra_value_malloc();

	for (l = 0; l < num_ch; l++)
	{
		for (m = 0; m < ch[l].num_morph; m++)
		{
			if (pi->callback)
				(*pi->callback)(NULL, (long)(l / (num_ch * ch[l].num_morph)));

			ra_value_set_double(vh, ch[l].morph[m].systolic);
			ra_prop_set_value(syst, ch[l].morph[m].event_id, ch[l].ch, vh);

			ra_value_set_double(vh, ch[l].morph[m].diastolic);
			ra_prop_set_value(dias, ch[l].morph[m].event_id, ch[l].ch, vh);
			
			ra_value_set_double(vh, ch[l].morph[m].mean);
			ra_prop_set_value(mean, ch[l].morph[m].event_id, ch[l].ch, vh);
			
			ra_value_set_long(vh, (long)((double)ch[l].morph[m].systolic_pos / x_scale));
			ra_prop_set_value(syst_pos, ch[l].morph[m].event_id, ch[l].ch, vh);
			
			ra_value_set_long(vh, (long)((double)ch[l].morph[m].diastolic_pos / x_scale));
			ra_prop_set_value(dias_pos, ch[l].morph[m].event_id, ch[l].ch, vh);

			ra_value_set_long(vh, ch[l].morph[m].flags);
			ra_prop_set_value(flags, ch[l].morph[m].event_id, ch[l].ch, vh);
			
			ra_value_set_double(vh, ch[l].morph[m].ibi);
			ra_prop_set_value(ibi, ch[l].morph[m].event_id, ch[l].ch, vh);
			
			ra_value_set_double(vh, ch[l].morph[m].dpdt_min);
			ra_prop_set_value(dpdt_min, ch[l].morph[m].event_id, ch[l].ch, vh);
			
			ra_value_set_double(vh, ch[l].morph[m].dpdt_max);
			ra_prop_set_value(dpdt_max, ch[l].morph[m].event_id, ch[l].ch, vh);
			
			ra_value_set_long(vh, (long)((double)ch[l].morph[m].dpdt_min_pos / x_scale));
			ra_prop_set_value(dpdt_min_pos, ch[l].morph[m].event_id, ch[l].ch, vh);
			
			ra_value_set_long(vh, (long)((double)ch[l].morph[m].dpdt_max_pos / x_scale));
			ra_prop_set_value(dpdt_max_pos, ch[l].morph[m].event_id, ch[l].ch, vh);
		}
	}

	if (!opt->dont_search_calibration && calib_info)
	{
		ra_value_set_long(vh, AP_CALIB_PHYSIOCAL);

		for (l = 0; l < num_ch; l++)
		{
			for (m = 0; m < ch[l].calib.num; m++)
			{
				long s, e, ev_id;
			
				s = (long)((double)(ch[l].calib.seq[m].start) / x_scale);
				e = (long)((double)(ch[l].calib.seq[m].end) / x_scale);

				ev_id = ra_class_add_event(opt->clh_calib, s, e);
				ra_prop_set_value(calib_info, ev_id, ch[l].ch, vh);
			}
		}
	}

	ra_value_free(vh);

	return 0;
} /* save_single() */
