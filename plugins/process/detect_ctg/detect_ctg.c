/*----------------------------------------------------------------------------
 * detect_ctg.c  -  perform detections in cardiotocograms
 *
 * Description:
 * The plugin performs detections of events in a cardiotocogram.
 * At the moment only the position of the maximal uterine contraction will
 * be searched.
 *
 * Remarks:
 * In the future, the plugin will be combined with the detect-simple plugin.
 * For this step, the detect-simple plugin will be extended with options and
 * the possibility to return the detection results not only by saving them in
 * the evaluation file. But to get a faster result, the CTG-specific code
 * will be first implemented here.
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2003-2006, Raphael Schneider
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
#include <ra_ch_list.h>
#include <ra_detect_ctg.h>
#include "detect_ctg.h"


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
	ps->info.num_results = 0;

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
	if (ra_plugin_get_by_name(ra, "gui-detect-ctg", 1))
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
		_ra_set_error(pl, 0, "plugin-detect-ctg");
		return NULL;
	}

	memset(pi, 0, sizeof(struct proc_info));
	pi->handle_id = RA_HANDLE_PROC;
	pi->plugin = pl;

	/* init options */
	pi->options = calloc(1, sizeof(struct ra_detect_ctg));
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
set_default_options(struct ra_detect_ctg *opt)
{
	opt->num_ch = 0;
	opt->ch = NULL;
	opt->save_in_eval = 0;
	opt->eh = NULL;
	opt->clh = NULL;
} /* set_default_options() */


void
set_option_offsets(struct ra_option_infos *opt_inf, struct ra_detect_ctg *opt)
{
	unsigned char *p = (unsigned char *)opt;
	int idx = 0;

	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->num_ch) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->ch) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->save_in_eval) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->eh) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->clh) - p);

	/* consistency check (useful when changing option structure) */
	if (idx != num_options)
	{
		fprintf(stderr, "critical error in detect_ctg.c - set_option_offsets():\n"
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
		struct ra_detect_ctg *opt;

		opt = (struct ra_detect_ctg *)pi->options;
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
	struct ra_detect_ctg *opt;
	struct detect_result dr;
	value_handle *res;
	long l;

	memset(&dr, 0, sizeof(struct detect_result));

	/* if no rec-handle was set, use root recording */
	if (pi->rh == NULL)
		pi->rh = ra_rec_get_first(pi->mh, 0);

	opt = (struct ra_detect_ctg *)pi->options;

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

		ret = get_uc_channels(pi->rh, &(opt->ch), &(opt->num_ch));
		if (ret != 0)
			goto error;		
	}

	if (opt->num_ch == 0)
	{
		_ra_set_error(pi->ra, RA_WARN_NO_DATA, "plugin-detect-ctg");
		goto error;
	}


	ret = simple_detect(pi, &dr);
	if (ret != 0)
		goto error;

	ret = store_results(pi, &dr);
	if (ret != 0)
		goto error;

 error:
	while (dr.ev)
	{
		struct events *ev = dr.ev->next;
		free(dr.ev);
		dr.ev = ev;
	}

	return ret;
} /* pl_do_processing() */


int
get_uc_channels(rec_handle rh, long **ch, long *num_ch)
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
		if (ra_value_get_long(vh) == RA_CH_TYPE_CTG_UC)
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
} /* get_uc_channels() */


int
store_results(struct proc_info *pi, struct detect_result *dr)
{
	int ret = 0;
	value_handle *res;
	long *pos = NULL;
	long num_pos;
	struct ra_detect_ctg *opt;
	struct events *curr;

	opt = (struct ra_detect_ctg *)pi->options;

	/* save results in value-handles */
	res = pi->results;

	ra_value_set_long(res[NUM], dr->num_events);

	if (dr->ev != NULL)
	{
		pos = malloc(sizeof(long) * dr->num_events);
		curr = dr->ev;
		num_pos = 0;
		while (curr)
		{
			if (num_pos >= dr->num_events)
				break;
			pos[num_pos++] = curr->pos;
			curr = curr->next;
		}
		ra_value_set_long_array(res[POS], pos, num_pos);

		free(pos);
	}

	if (opt->save_in_eval)
	{
		if (opt->eh == NULL)
		{
			_ra_set_error(pi->ra, RA_ERR_INFO_MISSING, "plugin-detect-ctg");
			return -1;
		}

		ret = ra_eval_save_result(opt->eh, NULL, pi, 0);
	}

	return ret;
} /* store_results() */

