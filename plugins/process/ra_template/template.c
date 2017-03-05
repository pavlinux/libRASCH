/*----------------------------------------------------------------------------
 * template.c  -  combine events in templates
 *
 * Description:
 * The plugin group events which have similar raw-data (e.g. QRS-complexes
 * which have a similar morphology).
 *
 * Author(s): Raphael Schneider (librasch@gmail.com)
 *
 * Copyright (C) 2002-2010, Raphael Schneider
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
#include <ra_ch_list.h>
#include "template.h"

#undef fprintf

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

	/* set option infos */
	ps->info.num_options = num_options;
	ps->info.opt = ra_alloc_mem(sizeof(options));
	memcpy(ps->info.opt, options, sizeof(options));

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
	if (ra_plugin_get_by_name(ra, "gui-template", 1))
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
		return NULL;

	memset(pi, 0, sizeof(struct proc_info));
	pi->handle_id = RA_HANDLE_PROC;
	pi->plugin = pl;

	/* init options */
	pi->options = calloc(1, sizeof(struct ra_template));
	set_option_offsets(ps->info.opt, pi->options);
	set_default_options(pi->options);

	return pi;
} /* pl_get_proc_handle() */


void
set_default_options(struct ra_template *opt)
{
	opt->rh = NULL;
	opt->sh = NULL;

	opt->use_class = 1;
	opt->clh = NULL;
	opt->pos_prop = NULL;
	opt->pos_ch = -1;
	opt->save_in_class = 1;

	opt->num_data = 0;
	opt->data = NULL;
	opt->data_is_pos = 0;

	opt->templ_name = NULL;
	opt->templ_corr = NULL;

	opt->corr_win_before = 0;
	opt->corr_win_after = 0;
	opt->corr_step = -1;

	opt->accept = 0.0;
	opt->slope_accept_low = 0.0;
	opt->slope_accept_high = 0.0;

	opt->template_win_before = 0;
	opt->template_win_after = 0;

	opt->num_ch = 0;
	opt->ch = NULL;

	opt->align_events = 1;
} /* set_default_options() */


void
set_option_offsets(struct ra_option_infos *opt_inf, struct ra_template *opt)
{
	unsigned char *p = (unsigned char *)opt;
	int idx = 0;

	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->rh) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->sh) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->use_class) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->clh) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->pos_prop) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->pos_ch) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->save_in_class) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->num_data) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->data) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->data_is_pos) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->templ_name) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->templ_corr) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->corr_win_before) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->corr_win_after) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->corr_step) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->accept) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->slope_accept_low) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->slope_accept_high) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->template_win_before) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->template_win_after) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->num_ch) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->ch) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->align_events) - p);

	/* consistency check (useful when changing option structure) */
	if (idx != num_options)
	{
		fprintf(stderr, "critical error in template.c - set_option_offsets():\n"
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
		struct ra_template *opt;

		opt = (struct ra_template *)pi->options;
		if (opt->data)
			ra_free_mem(opt->data);
		if (opt->pos_prop)
			ra_free_mem(opt->pos_prop);
		if (opt->templ_name)
			ra_free_mem(opt->templ_name);
		if (opt->templ_corr)
			ra_free_mem(opt->templ_corr);
		if (opt->ch)
			ra_free_mem(opt->ch);
		free(opt);
	}

	ra_free_mem(proc);
} /* pl_free_proc_handle() */


/* 
   FIXME: add support for handling other templates than qrs-complexes.
*/
int
pl_do_processing(proc_handle proc)
{
	int ret = -1;
	struct proc_info *pi = proc;
	long num;
	int i;
	struct event_templ_info *evi = NULL;
	struct info inf;
	struct ra_template *opt;

	memset(&inf, 0, sizeof(struct info));

	if (!pi)
		return -1;

	opt = (struct ra_template *)pi->options;
	if (opt == NULL)
	{
		_ra_set_error(proc, RA_ERR_INFO_MISSING, "plugin-template");
		return -1;
	}

	if ((opt->ch == NULL) || (opt->num_ch <= 0))
	{
		/* no channels given, maybe there is a template summary already available */
		if (opt->sh)
			ret = get_summary_channels(opt);

		if (ret != 0)
		{
			_ra_set_error(proc, RA_ERR_INFO_MISSING, "plugin-template");
			goto error;
		}
	}

	if (opt->rh == NULL)
		opt->rh = ra_rec_get_first(pi->mh, 0);
	if (opt->rh == NULL)
	{
		_ra_set_error(proc, RA_ERR_INFO_MISSING, "plugin-template");
		return -1;
	}

	if ((ret = get_data(pi, &evi, &num)) != 0)
		goto error;

	if ((ret = get_templates(pi, &inf, evi, num)) != 0)
		goto error;

	if (opt->save_in_class)
		ret = save_templates(pi, &inf, evi, num);
	if (ret != 0)
		goto error;

	ret = 0;

 error:
	free(inf.samplerate);
	free(inf.templ_win_before);
	free(inf.templ_win_after);
	free(inf.templ_win);
	free(inf.corr_win_before);
	free(inf.corr_win_after);
	free(inf.corr_win);
	free(inf.corr_step);
	free(inf.corr_offset);

	if (inf.corr_buf)
	{
		long l;

		for (l = 0; l < opt->num_ch; l++)
			free(inf.corr_buf[l]);
		free(inf.corr_buf);
	}

	/* free mem of templates */
	for (i = 0; i < inf.n_temp; i++)
	{
		int j;

		for (j = 0; j < opt->num_ch; j++)
		{
			if (inf.ts[i].data && inf.ts[i].data[j])
				free(inf.ts[i].data[j]);
			if (inf.ts[i].data_sum && inf.ts[i].data_sum[j])
				free(inf.ts[i].data_sum[j]);
		}

		if (inf.ts[i].data)
			free(inf.ts[i].data);
		if (inf.ts[i].data_sum)
			free(inf.ts[i].data_sum);
	}
	if (inf.ts)
		free(inf.ts);

	if (evi)
		free(evi);

	return ret;
} /* pl_do_processing() */


int
get_summary_channels(struct ra_template *opt)
{
	value_handle vh;
	long l;

	if (opt->sh == NULL)
		return -1;

	vh = ra_value_malloc();
	if (ra_info_get(opt->sh, RA_INFO_SUM_NUM_CH_L, vh) != 0)
	{
		ra_value_free(vh);
		return -1;
	}
	opt->num_ch = ra_value_get_long(vh);
	if (opt->num_ch <= 0)
	{
		ra_value_free(vh);
		return -1;
	}

	opt->ch = ra_alloc_mem(sizeof(long) * opt->num_ch);
	for (l = 0; l < opt->num_ch; l++)
	{
		ra_value_set_number(vh, l);

		if (ra_info_get(opt->sh, RA_INFO_SUM_CH_NUM_L, vh) != 0)
		{
			ra_value_free(vh);
			return -1;
		}
		opt->ch[l] = ra_value_get_long(vh);
	}

	ra_value_free(vh);

	return 0;
} /* get_summary_channels() */


int
get_data(struct proc_info *pi, struct event_templ_info **evi, long *num)
{
	int ret = 0;
	long l;
	struct ra_template *opt = (struct ra_template *)pi->options;

	*evi = NULL;
	*num = 0;

	if (opt->use_class && (opt->clh == NULL))
		return -1;

	if (opt->use_class || (opt->data_is_pos == 0))
		ret = get_data_from_class(opt, evi, num);
	else
	{
		if (opt->data == NULL)
			return -1;

		*num = opt->num_data;
		*evi = (struct event_templ_info *)calloc(*num, sizeof(struct event_templ_info));
		for (l = 0; l < (*num); l++)
		{
			(*evi)[l].pos = opt->data[l];
			(*evi)[l].event_id = -1;
		}
	}

	for (l = 0; l < (*num); l++)
		(*evi)[l].pos_orig = (*evi)[l].pos;

	return ret;
} /* get_data() */


int
get_data_from_class(struct ra_template *opt, struct event_templ_info **evi, long *num)
{
	int ret = 0;
	prop_handle pos_ph = NULL;
	long l, pos, dummy;
	value_handle vh;

	if (opt->clh == NULL)
		return -1;

	if ((opt->pos_prop != NULL) && (opt->pos_prop[0] != '\0'))
	{
		pos_ph = ra_prop_get(opt->clh, opt->pos_prop);
		if (pos_ph == NULL)
			return -1;
	}

	if (opt->use_class)
		return get_all_events(opt, pos_ph, evi, num);

	/* use only the events stored in opt->data */
	*num = opt->num_data;
	*evi = (struct event_templ_info *)calloc(*num, sizeof(struct event_templ_info));
	vh = ra_value_malloc();
	for (l = 0; l < *num; l++)
	{
		if (pos_ph)
		{
			ret = ra_prop_get_value(pos_ph, opt->data[l], opt->pos_ch, vh);
			(*evi)[l].pos = ra_value_get_long(vh);
		}
		else
		{
			ret = ra_class_get_event_pos(opt->clh, opt->data[l], &pos, &dummy);
			(*evi)[l].pos = pos;
		}

		if (ret != 0)
			break;

		(*evi)[l].event_id = opt->data[l];
	}
	ra_value_free(vh);
	
	if (ret != 0)
	{
		free(*evi);
		*num = 0;
	}

	return ret;
} /* get_data_from_class() */


int
get_all_events(struct ra_template *opt, prop_handle pos_ph, struct event_templ_info **evi, long *num)
{
	int ret = 0;
	long l, n, dummy;
	const long *ev_ids;
	value_handle vh_id, vh_pos;

	*num = 0;
	*evi = NULL;
	
	vh_id = ra_value_malloc();
	vh_pos = ra_value_malloc();

	if (pos_ph)
	{
		ra_class_get_events(opt->clh, -1, -1, 0, 1, vh_id);
		ev_ids = ra_value_get_long_array(vh_id);
		
		n = ra_value_get_num_elem(vh_id);
		*evi = (struct event_templ_info *)calloc(n, sizeof(struct event_templ_info));

		*num = 0;
		for (l = 0; l < n; l++)
		{
			ra_prop_get_value(pos_ph, ev_ids[l], opt->pos_ch, vh_pos);

			(*evi)[*num].pos = ra_value_get_long(vh_pos);
			(*evi)[*num].event_id = ev_ids[l];
			(*num)++;
		}
		if (n != *num)
			*evi = (struct event_templ_info *)realloc(*evi, sizeof(struct event_templ_info) * (*num));
	}
	else
	{		
		ra_class_get_events(opt->clh, -1, -1, 0, 1, vh_id);
		*num = ra_value_get_num_elem(vh_id);
		*evi = (struct event_templ_info *)calloc(*num, sizeof(struct event_templ_info));
		
		ev_ids = ra_value_get_long_array(vh_id);
		for (l = 0; l < *num; l++)
		{
			ra_class_get_event_pos(opt->clh, ev_ids[l], &((*evi)[l].pos), &dummy);
			(*evi)[l].event_id = ev_ids[l];
		}
	}

	ra_value_free(vh_id);
	ra_value_free(vh_pos);

	return ret;
} /* get_all_events() */


int
read_templates_from_eval(struct proc_info *pi, struct info *inf)
{
	struct ra_template *opt = (struct ra_template *)pi->options;
	value_handle vh = NULL;
	long *part_ids = NULL;
	long l, m;

	if (opt->sh == NULL)
		return -1;
	inf->sh = opt->sh;

	vh = ra_value_malloc();

	ra_sum_get_part_all(inf->sh, vh);
	inf->n_temp = ra_value_get_num_elem(vh);
	inf->ts = calloc(inf->n_temp, sizeof(struct template_single));
	part_ids = malloc(sizeof(long) * inf->n_temp);
	memcpy(part_ids, ra_value_get_long_array(vh), sizeof(long) * inf->n_temp);
	for (l = 0; l < inf->n_temp; l++)
	{
		inf->ts[l].new_template = 0;
		inf->ts[l].part_id = part_ids[l];

		inf->ts[l].fiducial_offset = inf->templ_win_before[0];
		inf->ts[l].type = RA_VALUE_TYPE_DOUBLE;

		ra_sum_get_part_events(inf->sh, part_ids[l], vh);
		inf->ts[l].num_events = ra_value_get_num_elem(vh);
		inf->ts[l].buf_size = ra_value_get_num_elem(vh);
		inf->ts[l].ev_ids = malloc(sizeof(long) * inf->ts[l].num_events);
		memcpy(inf->ts[l].ev_ids, ra_value_get_long_array(vh), sizeof(long) * inf->ts[l].num_events);

		inf->ts[l].data = malloc(sizeof(double *) * opt->num_ch);
		inf->ts[l].data_sum = malloc(sizeof(double *) * opt->num_ch);
		for (m = 0; m < opt->num_ch; m++)
		{
			inf->ts[l].data[m] = malloc(sizeof(double) * inf->templ_win[m]);
			inf->ts[l].data_sum[m] = malloc(sizeof(double) * inf->templ_win[m]);
			ra_sum_get_part_data(inf->sh, part_ids[l], m, 0, vh);
			memcpy(inf->ts[l].data[m], ra_value_get_double_array(vh), sizeof(double) * inf->templ_win[m]);
		}
	}
	
	ra_value_free(vh);

	if (part_ids)
		free(part_ids);

	return 0;
} /* read_templates_from_eval() */


int
get_templates(struct proc_info *pi, struct info *inf, struct event_templ_info *evi, long num)
{
	int ret = 0;
	struct ra_template *opt = (struct ra_template *)pi->options;
	long l, start;
	value_handle vh;
	int percent, percent_save;
	char s[100];

	if (pi->callback)
	{
		char s[100];
		sprintf(s, gettext("find templates"));
		(*pi->callback) (s, 0);
	}

	/* init variables for speedup */
	inf->samplerate = calloc(opt->num_ch, sizeof(double));
	inf->templ_win_before = calloc(opt->num_ch, sizeof(long));
	inf->templ_win_after = calloc(opt->num_ch, sizeof(long));
	inf->templ_win = calloc(opt->num_ch, sizeof(long));
	inf->corr_win_before = calloc(opt->num_ch, sizeof(long));
	inf->corr_win_after = calloc(opt->num_ch, sizeof(long));
	inf->corr_win = calloc(opt->num_ch, sizeof(long));
	inf->corr_step = calloc(opt->num_ch, sizeof(long));
	inf->corr_offset = calloc(opt->num_ch, sizeof(long));
	inf->corr_buf = calloc(opt->num_ch, sizeof(double *));
	vh = ra_value_malloc();
	for (l = 0; l < opt->num_ch; l++)
	{
		ra_value_set_number(vh, opt->ch[l]);
		if (ra_info_get(opt->rh, RA_INFO_REC_CH_SAMPLERATE_D, vh) == 0)
		{
			inf->samplerate[l] = ra_value_get_double(vh);

			inf->templ_win_before[l] = (long)(opt->template_win_before * inf->samplerate[l]);
			inf->templ_win_after[l] = (long)(opt->template_win_after * inf->samplerate[l]);
			inf->templ_win[l] = inf->templ_win_before[l] + inf->templ_win_after[l];

			inf->corr_win_before[l] = (long)(opt->corr_win_before * inf->samplerate[l]);
			inf->corr_win_after[l] = (long)(opt->corr_win_after * inf->samplerate[l]);
			inf->corr_win[l] = inf->corr_win_before[l] + inf->corr_win_after[l];

			if (opt->corr_step == -1.0)
				inf->corr_step[l] = 1;
			else
				inf->corr_step[l] = (long)(opt->corr_step * inf->samplerate[l]);

			inf->corr_buf[l] = calloc(inf->templ_win[l], sizeof(double));

			
		}
	}
	ra_value_free(vh);

	/* get templates from eval-file (if available) */
	read_templates_from_eval(pi, inf);

	start = 0;
	percent_save = -1;
	for (l = start; l < num; l++)
	{
		percent = (int)(((double)l / (double)num) * 100.0);
		if (pi->callback && (percent != percent_save))
		{
			sprintf(s, gettext("find templates (number found: %d)"), inf->n_temp);
			(*pi->callback) (s, percent);
			percent_save = percent;
		}

		assign_template(pi, inf, evi+l);
	}

	if (pi->callback)
	{
		sprintf(s, gettext("find templates (number found: %d)"), inf->n_temp);
		(*pi->callback)(s, 100);
	}

	/* align events on templates (if wanted) */
	if (opt->align_events)
		align_events(opt, evi, num);

	return ret;
} /* get_templates() */


int
assign_template(struct proc_info *pi, struct info *inf, struct event_templ_info *evi)
{
	int ret;
	struct ra_template *opt = (struct ra_template *)pi->options;
	int templ_num;
	double corr;
	long offset;

	/* get template with best correlation */
	if ((ret = find_template(pi, inf, evi->pos, &templ_num, &corr, &offset)) !=0)
		return ret;
	
	/* if correlation below treshold -> new template else update template */
	if (corr >= opt->accept)
	{
		evi->pos += offset;
		evi->correlation = corr;
		/* up to now I have found no benefit of updating the template;
		   I have the impression that updating it is even counterproductive */
/* 		update_template(inf, opt, evi->pos, templ_num); */
	}
	else
	{
		templ_num = new_template(inf, opt, evi->pos);
		evi->correlation = 1;
	}
	
	/* assign template-number and store event-id */
	evi->template = templ_num;
	inf->ts[templ_num].num_events++;

	if (inf->ts[templ_num].buf_size < inf->ts[templ_num].num_events)
	{
		inf->ts[templ_num].buf_size += 100;
		inf->ts[templ_num].ev_ids = (long *)realloc(inf->ts[templ_num].ev_ids,
							    sizeof(long) * inf->ts[templ_num].buf_size);
	}
	inf->ts[templ_num].ev_ids[inf->ts[templ_num].num_events-1] = evi->event_id;

	return 0;
} /* assign_template() */


int
find_template(struct proc_info *pi, struct info *inf, long pos, int *templ_num, double *corr, long *offset)
{
	struct ra_template *opt = (struct ra_template *)pi->options;
	int i, j;
	long p;

	for (i = 0; i < opt->num_ch; i++)
	{
		p = pos - inf->templ_win_before[i];
		if (p < 0)
			p = 0;
	
		ra_raw_get_unit(opt->rh, opt->ch[i], p, inf->templ_win[i], inf->corr_buf[i]);
		ra_raw_process_unit(RA_RAW_PROC_RM_MEAN, NULL, inf->templ_win[i], inf->corr_buf[i], opt->rh, opt->ch[i]);
		/*ra_raw_process_unit(RA_RAW_PROC_RM_POWERLINE_NOISE, NULL, inf->templ_win[i],
				    inf->corr_buf[i], opt->rh, opt->ch[i]);*/
	}

	*templ_num = -1;
	*corr = 0.0;
	for (i = 0; i < inf->n_temp; i++)
	{
		double corr_sum = 0.0;
		double n_corr = (double)opt->num_ch;

		for (j = 0; j < opt->num_ch; j++)
		{
			double c;
			c = corr_template(inf, opt, i, j, &(inf->corr_offset[j]));
			if (c == -2)
				n_corr -= 1.0;
			else
				corr_sum += c;
		}

		corr_sum = corr_sum / n_corr;

		if (corr_sum > *corr)
		{
			*corr = corr_sum;
			*templ_num = i;
			/* offset has to be taken always from the same channel (using it from
			   different channels will blur the template) */
			/* TODO: decide if always the first channel will be used or if it
			   makes sense to choose it (add it to the options) */
			*offset = inf->corr_offset[0];

			if (*corr > opt->accept)
				break;
		}
	}

	return 0;		/* TODO: check if return value necessary */
} /* find_template */


double
corr_template(struct info *inf, struct ra_template *opt, int templ, int ch, long *offset)
{
	long l, num_steps;
	long corr_offset;
	double corr = 0.0;
	double corr_score = 0.0;
	double *buf;

	*offset = 0;

	num_steps = inf->templ_win[ch] - inf->corr_win[ch];
	corr_offset = inf->templ_win_before[ch] - inf->corr_win_before[ch];

	buf = inf->corr_buf[ch];
	for (l = 0; l <= num_steps; l += inf->corr_step[ch])
	{
		double corr_curr, off, slope;
		int ret;

		ret = calc_correlation(buf+corr_offset, &(inf->ts[templ].data[ch][l]), inf->corr_win[ch], &corr_curr);
		if (ret != 0) 
			continue;
		ret = calc_regression(buf+corr_offset, &(inf->ts[templ].data[ch][l]), inf->corr_win[ch], &off, &slope);
		if (ret != 0) 
			continue;

		if ((slope == 0) && (corr_curr == 0))
		{
			corr = -2;
			break;
		}

		if ((corr_curr > opt->accept) && (slope > opt->slope_accept_low) && (slope < opt->slope_accept_high))
		{
			/* score depends on correlation and how close the regression slopes are near 1 */
			double score = corr_curr - fabs(1 - slope);
			if (score > corr_score)
			{
				corr = corr_curr;
				*offset = corr_offset - l;  /* offset will be ADDED to the pos of the current beat */

				corr_score = score;
			}
		}
	}

	/* !!!!! TODO: correct offset with max-x-scale to handle multiple samplerates !!!!! */

	return corr;
} /* corr_template() */


/*  replaced by implementation below !!!! */
int
calc_correlation(double *x, double *y, long len, double *corr)
{
	double mean_x = 0.0;
	double mean_y = 0.0;
	double var_x = 0.0;
	double var_y = 0.0;
	double covar_xy = 0.0;
	long l;
	double temp, sqrt_var_x, sqrt_var_y;
	int ret = 0;

	if (len < 2)
		return -1;

	for (l = 0; l < len; l++)
	{
		mean_x += x[l];
		mean_y += y[l];
	}

	mean_x /= (double) len;
	mean_y /= (double) len;

	for (l = 0; l < len; l++)
	{
		var_x += ((x[l] - mean_x) * (x[l] - mean_x));
		var_y += ((y[l] - mean_y) * (y[l] - mean_y));

		covar_xy += ((x[l] - mean_x) * (y[l] - mean_y));
	}

	temp = 1.0 / ((double) len - 1.0);

	sqrt_var_x = sqrt(temp * var_x);
	sqrt_var_y = sqrt(temp * var_y);

	if ((sqrt_var_x == 0.0) || (sqrt_var_y == 0.0))
		ret = -1;
	else
		*corr = (temp * covar_xy) / (sqrt_var_x * sqrt_var_y);

	return ret;
} /* calc_correlation() */


int
calc_regression(double *x, double *y, long len, double *offset, double *slope)
{
	long l;
	double mean_x = 0.0;
	double sum_sq_x = 0.0;
	double mean_y = 0.0;
	double sum_x_t_type = 0.0;

	if (len < 2)
	{
		*offset = 0.0;
		*slope = 0.0;
		return -1;
	}

	for (l = 0; l < len; l++)
	{
		mean_x += x[l];
		sum_sq_x += (x[l] * x[l]);

		mean_y += y[l];

		sum_x_t_type += (x[l] * y[l]);
	}
	mean_x /= (double)len;
	mean_y /= (double)len;

	*slope = (sum_x_t_type - ((double)len * mean_x * mean_y));
	*slope /= (sum_sq_x - ((double)len * mean_x * mean_x));
	*offset = mean_y - (*slope)*mean_x;

	return 0;
} /* calc_regression() */


static int
cmp_long(const void *p1, const void *p2)
{
	return ((*(long *)p1) - (*(long *)p2));
} /* cmp_long() */


long
get_median(long *values, unsigned long num_values)
{
	long median;

	long *v = malloc(sizeof(long) * num_values);
	memcpy(v, values, sizeof(long) * num_values);

	qsort(v, num_values, sizeof(long), cmp_long);

	if (num_values % 2)
		median = v[num_values / 2];
	else
		median = v[(num_values/2)-1] + v[num_values/2] / 2;
	
	return median;
} /* get_median() */


int
update_template(struct info *inf, struct ra_template *opt, long pos, int templ_num)
{
	int i;
	long l;
	double samplerate = 1.0;
	value_handle vh = NULL;
	double *buf = NULL;

	vh = ra_value_malloc();
	ra_value_set_number(vh, opt->ch[0]);
	if (ra_info_get(opt->rh, RA_INFO_REC_CH_SAMPLERATE_D, vh) == 0)
		samplerate = ra_value_get_double(vh);
	ra_value_free(vh);

	for (i = 0; i < opt->num_ch; i++)
	{
		double *curr_buf = inf->ts[templ_num].data[i];
		double *curr_buf_sum = inf->ts[templ_num].data_sum[i];

		buf = malloc(sizeof(double) * inf->templ_win[i]);

		ra_raw_get_unit(opt->rh, opt->ch[i], (pos - inf->templ_win_before[i]), inf->templ_win[i], buf);
		ra_raw_process_unit(RA_RAW_PROC_RM_MEAN, NULL, inf->templ_win[i], buf, opt->rh, opt->ch[i]);
		ra_raw_process_unit(RA_RAW_PROC_RM_POWERLINE_NOISE, NULL, inf->templ_win[i], buf, opt->rh, opt->ch[i]);

		for (l = 0; l < inf->templ_win[i]; l++)
		{
			curr_buf_sum[l] += buf[l];
			curr_buf[l] = curr_buf_sum[l] / (double)(inf->ts[templ_num].num_events + 1);
		}

		if (buf)
			free(buf);
	}

	return 0;
} /* update_template() */


int
new_template(struct info *inf, struct ra_template *opt, long pos)
{
	int i;
	long l;
	double samplerate = 1.0;
	value_handle vh = NULL;

	vh = ra_value_malloc();
	ra_value_set_number(vh, opt->ch[0]);
	if (ra_info_get(opt->rh, RA_INFO_REC_CH_SAMPLERATE_D, vh) == 0)
		samplerate = ra_value_get_double(vh);
	ra_value_free(vh);

	inf->ts = realloc(inf->ts, sizeof(struct template_single) * (inf->n_temp + 1));
	memset(inf->ts + inf->n_temp, 0, sizeof(struct template_single));
	inf->ts[inf->n_temp].new_template = 1;

	/* FIXME: add support for different samplerates */
	inf->ts[inf->n_temp].fiducial_offset = inf->templ_win_before[0];
	inf->ts[inf->n_temp].type = RA_VALUE_TYPE_DOUBLE;
	inf->ts[inf->n_temp].data = malloc(sizeof(double *) * opt->num_ch);
	inf->ts[inf->n_temp].data_sum = malloc(sizeof(double *) * opt->num_ch);
	for (i = 0; i < opt->num_ch; i++)
	{
		double *curr_buf;
		double *curr_buf_sum;
		unsigned long start_pos, num_samples, buf_offset;

		inf->ts[inf->n_temp].data[i] = calloc(inf->templ_win[i], sizeof(double));
		curr_buf = inf->ts[inf->n_temp].data[i];

		inf->ts[inf->n_temp].data_sum[i] = calloc(inf->templ_win[i], sizeof(double));
		curr_buf_sum = inf->ts[inf->n_temp].data_sum[i];

		if (pos >= inf->templ_win_before[i])
		{
			start_pos = pos - inf->templ_win_before[i];
			num_samples = inf->templ_win[i];
			buf_offset = 0;
		}
		else
		{
			start_pos = 0;
			buf_offset = inf->templ_win_before[i] - pos;
			num_samples = inf->templ_win[i] - buf_offset;
		}
		ra_raw_get_unit(opt->rh, opt->ch[i], start_pos, num_samples, curr_buf+buf_offset);

		ra_raw_process_unit(RA_RAW_PROC_RM_MEAN, NULL, inf->templ_win[i], curr_buf, opt->rh, opt->ch[i]);
		ra_raw_process_unit(RA_RAW_PROC_RM_POWERLINE_NOISE, NULL, inf->templ_win[i], curr_buf, opt->rh, opt->ch[i]);

		for (l = 0; l < inf->templ_win[i]; l++)
			curr_buf_sum[l] += curr_buf[l];
	}

	inf->n_temp++;

	return (inf->n_temp - 1);
} /* new_template() */


int
align_events(struct ra_template *opt, struct event_templ_info *evi, long num)
{
	prop_handle pos_ph = NULL;
	long l;
	value_handle vh;

	if (!opt->use_class && (opt->data_is_pos != 0))
		return 0;

	if (opt->clh == NULL)
		return -1;

	if ((opt->pos_prop != NULL) && (opt->pos_prop[0] != '\0'))
	{
		pos_ph = ra_prop_get(opt->clh, opt->pos_prop);
		if (pos_ph == NULL)
			return -1;
	}

	vh = ra_value_malloc();
	for (l = 0; l < num; l++)
	{
		int ret = 0;

		if (pos_ph)
		{
			ra_value_set_long(vh, evi[l].pos);
			ret = ra_prop_set_value(pos_ph, evi[l].event_id, opt->pos_ch, vh);
		}

		if (ret != 0)
			break;
	}
	ra_value_free(vh);

	return 0;
} /* align_events() */


int
save_templates(struct proc_info *pi, struct info *inf, struct event_templ_info *evi, long num_ev)
{
	int ret;
	struct ra_template *opt = (struct ra_template *)pi->options;
	sum_handle sh;
	char **dim_unit, **dim_name;
	long l, m, id;
	value_handle vh;

	/* both parameters are not used in the moment, but maybe later in the code
	   which is commented out at the end of the function */
	if (evi || num_ev)
		;

	if (opt->clh == NULL)
		return -1;

	if (inf->sh == NULL)
	{
		dim_unit = (char **)malloc(sizeof(char *));
		dim_unit[0] = (char *)malloc(sizeof(char) * 100);
		strncpy(dim_unit[0], "mV", 99);
		dim_name = (char **)malloc(sizeof(char *));
		dim_name[0] = (char *)malloc(sizeof(char) * 100);
		strncpy(dim_name[0], "samples", 99);

		sh = ra_sum_add(opt->clh, "template", "templates", "templates of events",
				1, (const char **)dim_unit, (const char **)dim_name);
		for (l = 0; l < opt->num_ch; l++)
			ra_sum_add_ch(sh, opt->ch[l], inf->templ_win_before[l]);

		free(dim_unit[0]);
		free(dim_unit);
		free(dim_name[0]);
		free(dim_name);
	}
	else
		sh = inf->sh;

	if (sh == NULL)
		return -1;

	vh = ra_value_malloc();
	for (l = 0; l < inf->n_temp; l++)
	{
		if (inf->ts[l].new_template == 0)
		{
			/* no new template, so set only the event-ids */
			ra_value_set_long_array(vh, inf->ts[l].ev_ids, inf->ts[l].num_events);
			ra_sum_set_part_events(sh, inf->ts[l].part_id, vh);
			continue;
		}

		id = ra_sum_add_part(sh, inf->ts[l].num_events, inf->ts[l].ev_ids);
		if (id < 0)
		{
			ret = -1;
			goto error;
		}

		for (m = 0; m < opt->num_ch; m++)
		{
			ra_value_set_double_array(vh, inf->ts[l].data[m], inf->templ_win[m]);
			if ((ret = ra_sum_set_part_data(sh, id, m, 0, vh)) != 0)
				goto error;
		}
	}

	ret = 0;

 error:
	return ret;
} /* save_templates() */
