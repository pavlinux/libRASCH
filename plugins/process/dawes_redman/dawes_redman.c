/*----------------------------------------------------------------------------
 * dawes_redman.c  -  calculates fetal heart rate variations using the Dawes/Redman criteria
 *
 * Description:
 * The plugin calculates the fetal heart rate variations using the Dawes/Redman criteria.
 * The estimation of the baseline of the fetal heart rate is done according the descriptions
 * in the various papers from Dawes and Redman. For the digital filter, a Butterworth filter
 * with order 4 and corner-frequency of 0.0017Hz (approx. 0.1 min-1)is used (using the 3.75sec
 * epoch-values results in a samplerate of approx. 0.2667Hz). The filter was designed on the
 * website of Tony Fisher, using the mkfilter cgi-script. This can be found at
 * http://www-users.cs.york.ac.uk/~fisher/mkfilter/ .
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2003-2006, Raphael Schneider
 * 
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

#include "dawes_redman.h"


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
	if (ra_plugin_get_by_name(ra, "gui-dawes-redman", 1))
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
	pi->options = calloc(1, sizeof(struct ra_dawes_redman));
	set_option_offsets(ps->info.opt, pi->options);
	set_default_options(pi->options);

	return pi;
} /* pl_get_proc_handle() */


void
set_default_options(struct ra_dawes_redman *opt)
{
	opt->eh = NULL;

	opt->use_start_end_pos = 0;
	opt->start_pos = -1;
	opt->end_pos = -1;

	opt->rh = NULL;
	opt->ch_num = 0;

	opt->use_ignore_value = 1;
	opt->ignore_value = 0.0;

	opt->filter_maternal_pulse = 0;

	opt->ignore_marked_regions = 1;
	opt->ignore_noise_regions = 1;
} /* set_default_options() */


void
set_option_offsets(struct ra_option_infos *opt_inf, struct ra_dawes_redman *opt)
{
	unsigned char *p = (unsigned char *)opt;
	int idx = 0;

	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->eh) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->use_start_end_pos) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->start_pos) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->end_pos) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->rh) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->ch_num) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->use_ignore_value) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->ignore_value) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->filter_maternal_pulse) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->ignore_marked_regions) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->ignore_noise_regions) - p);

	/* consistency check (useful when changing option structure) */
	if (idx != num_options)
	{
		fprintf(stderr, "critical error in dawes_redman.c"
			" - set_option_offsets():\n  number of options(%ld) != idx(%d)\n",
			num_options, idx);
		exit(-1);
	}
} /* set_option_offsets() */


void
pl_free_proc_handle(proc_handle proc)
{
	struct proc_info *pi = proc;
	
	if (pi->options)
	{
		struct ra_dawes_redman *opt;
		opt = (struct ra_dawes_redman *)pi->options;

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
	int ret = 1;
	struct proc_info *pi = proc;
	struct epoch * epochs = NULL;
	double *buf = NULL;
	long l, n_runs, num, num_valid_epoch;
	value_handle *res;
	struct ra_dawes_redman *opt;

	if (pi == NULL)
		return -1;

	opt = (struct ra_dawes_redman *)pi->options;

	/* if rec-handle was not set in options, use root recording */
	if (!((struct ra_dawes_redman *)pi->options)->rh)
		((struct ra_dawes_redman *)pi->options)->rh = ra_rec_get_first(pi->mh, 0);

	/* if no eval-handle was set, use default evaluation */
	if (!opt->eh)
		opt->eh = ra_eval_get_default(pi->mh);

	if ((ret = get_data(pi->options, &epochs, &num)) != 0)
		goto clean_up;
	if (num <= 0)
		goto clean_up;

	n_runs = 1;
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

	buf = malloc(sizeof(double) * num);
	for (l = 0; l < num; l++)
		buf[l] = epochs[l].epoch_bpm;
	ra_value_set_double_array(res[FHR_EPOCHS], buf, num);

	if ((ret = filter_data(epochs, num)) != 0)
		goto clean_up;

	if ((ret = estimate_baseline(epochs, num)) != 0)
		goto clean_up;

	num_valid_epoch = 0;
	for (l = 0; l < num; l++)
	{
		buf[l] = epochs[l].baseline_bpm;
		if ((epochs[l].percent_noise < 100) && !epochs[l].interpolated)
			num_valid_epoch++;
	}
	ra_value_set_double_array(res[FHR_BASELINE], buf, num);
	ra_value_set_long(res[NUM_VALID_EPOCHS], num_valid_epoch);

	if ((ret = get_fhr_variations(epochs, num, res)) != 0)
		goto clean_up;

/* 	if (pi->save_in_eval) */
/* 		save(opt->eh, res); */

	ret = 0;

 clean_up:
	if (epochs)
		free(epochs);
	if (buf)
		free(buf);

	return ret;
} /* pl_do_processing() */


int
get_data(struct ra_dawes_redman *opt, struct epoch **epochs, long *num)
{
	int ret;
	long s_use, e_use;
	long n, l, start;
	double *raw = NULL;
	value_handle vh;
	double samplerate;

	ret = 0;
	*num = 0;

	if (opt->rh == NULL)
		return -1;

	vh = ra_value_malloc();
	if (opt->use_start_end_pos)
	{
		s_use = opt->start_pos;
		e_use = opt->end_pos;
	}
	else
	{
		s_use = 0;
		e_use = 0;
		ra_value_set_number(vh, opt->ch_num);
		if (ra_info_get(opt->rh, RA_INFO_REC_CH_NUM_SAMPLES_L, vh) == 0)
			e_use = ra_value_get_long(vh);
	}
	ra_value_free(vh);

	n = e_use - s_use;
	if (n <= 0)
		return 0;

	raw = malloc(sizeof(double) * n);
	n = (long)ra_raw_get_unit(opt->rh, opt->ch_num, s_use, n, raw);

	identify_invalid_values(opt, raw, n);

	if (opt->eh)
	{
		double scale = get_samplerate(opt->rh, opt->ch_num) / get_samplerate(opt->eh, -1);
		handle_ignore_regions(opt, raw, n, scale);
	}

	/* remove not valid values at the beginning (if there are some) */
	start = 0;
	for (l = start; l < n; l++)
	{
		if (raw[l] != HUGE_VAL)
			break;
	}
	if (l != start)
	{
		n -= l;
		memmove(raw, raw+l, sizeof(double) * n);
	}

	/* remove not valid values at the end (if there are some) */
	for (l = (n-1); l >= 0; l--)
	{
		if (raw[l] != HUGE_VAL)
			break;
	}
	n = l+1;

	samplerate = get_samplerate(opt->rh, opt->ch_num);
	ret = calc_epochs(raw, n, epochs, num, samplerate);
	if (*num <= 0)
	{
		if (raw)
			free(raw);
		return 0;
	}

	/* Sometimes it happens that at the beginning that artifacts are recorded.
	   Therefore check in the first 5 minutes if the fhr is < 95 bpm or > 180 bpm.
	   Go out of the check if 5 minutes are over or when the first minute is ok. */
	start = 0;
	for (l = 0; l < *num; l += EPOCHS_MIN)
	{
		long m;
		int ok;

		if ((l >= *num) || (l >= (CHECK_BEGIN_MINUTES * EPOCHS_MIN)))
			break;

		ok = 1;
		for (m = l; m < (l + EPOCHS_MIN); m++)
		{
			if (((*epochs)[m].epoch_raw_bpm < CHECK_BEGIN_MIN_FHR)
			    || ((*epochs)[m].epoch_raw_bpm > CHECK_BEGIN_MAX_FHR))
			{
				ok = 0;
				break;
			}
		}
		if (ok)
			break;
	}
	if (l != start)
	{
		*num -= l;
		memmove(*epochs, (*epochs)+l, sizeof(struct epoch) * (*num));
	}
	if (*num <= 0)
	{
		if (raw)
			free(raw);
		return 0;
	}

	ret = interpolate_epochs(*epochs, *num);

	if (raw)
		free(raw);

	return ret;
} /* get_data() */


int
identify_invalid_values(struct ra_dawes_redman *opt, double *data, long n)
{
	long l;

	if (opt->use_ignore_value)
	{
		for (l = 0; l < n; l++)
		{
			if (data[l] == opt->ignore_value)
				data[l] = HUGE_VAL;
		}
	}

  	for (l = 0; l < n; l++)
  	{
  		if ((data[l] < MIN_FHR) || (data[l] > MAX_FHR))
  			data[l] = HUGE_VAL;
  	}

	for (l = 2; l < n; l++)
	{
		double s, perc;

		if ((data[l] == HUGE_VAL) || (data[l-1] == HUGE_VAL) || (data[l-2] == HUGE_VAL))
			continue;

		s = (data[l-2] + data[l-1]) / 2;
		perc = data[l] / s;
		if ((perc < 0.65) || (perc > 1.75))
		{
			long m;

			for (m = l-2; m <= l; m++)
				data[m] = HUGE_VAL;
		}
	}

	if (opt->filter_maternal_pulse)
	{
		double last;
		long start = 0;
		last = data[0];
		for (l = 0; l < n; l++)
		{
			if (data[l] != HUGE_VAL)
			{
				last = data[l];
				break;
			}
		}
		start = l + 1;

		for (l = start; l < n; l++)
		{
			if (data[l] == HUGE_VAL)
				continue;

			if (((last - data[l]) >= MIN_DIFF_MATERNAL_PULSE) && data[l] <= 100)
			{
				data[l] = HUGE_VAL;
				continue;
			}

			last = data[l];
		}
	}

	return 0;
} /* identify_invalid_values() */


double
get_samplerate(any_handle h, long ch_num)
{
	value_handle vh;
	double samplerate = 0.0;
	meas_handle mh;

	mh = ra_meas_handle_from_any_handle(h);

	vh = ra_value_malloc();

	if (ch_num == -1)
	{
		ra_info_get(mh, RA_INFO_MAX_SAMPLERATE_D, vh);
		samplerate = ra_value_get_double(vh);
	}
	else
	{
		ra_value_set_number(vh, ch_num);
		ra_info_get(h, RA_INFO_REC_CH_SAMPLERATE_D, vh);
		samplerate = ra_value_get_double(vh);
	}

	ra_value_free(vh);

	return samplerate;
} /* get_samplerate() */


void
handle_ignore_regions(struct ra_dawes_redman *opt, double *data, long n, double scale)
{
	prop_handle annot_prop;
	class_handle annot_clh;
	long l, num_annot;
	value_handle vh, vh_ev_ids;
	const void **clhs;
	const long *ev_ids;
	const char **annots;

	if (!opt->eh)
		return;

	vh = ra_value_malloc();
	ra_class_get(opt->eh, "annotation", vh);
	clhs = ra_value_get_voidp_array(vh);
	if (clhs == NULL)
	{
		ra_value_free(vh);
		return;
	}
	annot_clh = (class_handle)(clhs[0]);

	if ((annot_prop = ra_prop_get(annot_clh, "annotation")) == NULL)
	{
		ra_value_free(vh);
		return;
	}

	vh_ev_ids = ra_value_malloc();
	if (ra_prop_get_value_all(annot_prop, vh_ev_ids, NULL, vh) != 0)
	{
		ra_value_free(vh);
		ra_value_free(vh_ev_ids);
		return;
	}
	ev_ids = ra_value_get_long_array(vh_ev_ids);
	annots = ra_value_get_string_array(vh);

	num_annot = ra_value_get_num_elem(vh_ev_ids);
	for (l = 0; l < num_annot; l++)
	{
		long m, s, e;
		int ignore = 0;
		int noise = 0;

		if (ra_class_get_event_pos(annot_clh, ev_ids[l], &s, &e) != 0)
			continue;
		
		if (strstr(annots[l], "#IGNORE#") != NULL)
			ignore = 1;
		if (strstr(annots[l], "#NOISE#") != NULL)
			noise = 1;
		
		/* decide if region should be ignored */
		if ((opt->ignore_marked_regions && ignore) || (opt->ignore_noise_regions && noise))
		{
			s = (long)((double)s * scale);
			e = (long)((double)e * scale);
			
			if (e >= num_annot)
				e = num_annot-1;
			/* yes, set ignored values to HUGE_VAL */
			for (m = s; m <= e; m++)
			{
				if (m >= n)
					break;
				data[m] = HUGE_VAL;
			}
		}
	}
	ra_value_free(vh);
	ra_value_free(vh_ev_ids);
} /* handle_ignore_regions() */


int
calc_epochs(double *raw, long n_raw, struct epoch **epochs, long *n_epochs, double samplerate)
{
	long l, curr;
	double epoch_su;

	*n_epochs = 0;
	*epochs = malloc(sizeof(struct epoch) * n_raw);
	memset(*epochs, 0, sizeof(struct epoch) * n_raw);

	epoch_su = EPOCH_LEN * samplerate;

	curr = 0;
	while (curr < n_raw)
	{
		double sum = 0.0;
		long n_sum = 0;

		for (l = curr; l < (curr + epoch_su); l++)
		{
			if (l >= n_raw)
				break;

			if (raw[l] != HUGE_VAL)
			{
				sum += raw[l];
				n_sum++;
			}
		}
		if (n_sum <= 0)
		{
			(*epochs)[*n_epochs].epoch_raw_bpm = 0;
			(*epochs)[*n_epochs].percent_noise = 100;
		}
		else
		{
			(*epochs)[*n_epochs].epoch_raw_bpm = sum / n_sum;
			(*epochs)[*n_epochs].percent_noise = ((epoch_su - (double)n_sum) / epoch_su) * 100;
		}
		(*n_epochs)++;

		curr += (long)epoch_su;
	}

	*epochs = realloc(*epochs, sizeof(struct epoch) * (*n_epochs));

	for (l = 0; l < (*n_epochs); l++)
	{
		if ((*epochs)[l].epoch_raw_bpm == 0)
			(*epochs)[l].epoch_raw_ms = 0.0;
		else
			(*epochs)[l].epoch_raw_ms = 60000.0 / (*epochs)[l].epoch_raw_bpm;
	}

	return 0;
} /* calc_epochs() */


int
interpolate_epochs(struct epoch *epochs, long num)
{
	long mode, mode_cnt;
	long l, m;
	double last_ok;
	long last_ok_idx;
	int in_75_jump = 0;

	/* first get mode of epochs */
	mode = get_mode(epochs, num, &mode_cnt);

	/* copy raw epoch values to values which will be used */
	for (l = 0; l < num; l++)
	{
		epochs[l].epoch_bpm = epochs[l].epoch_raw_bpm;
		epochs[l].epoch_ms = epochs[l].epoch_raw_ms;
	}

	last_ok = (double)mode;
	last_ok_idx = 0;
	for (l = 1; l < num; l++)
	{
		double add, diff;
		long cnt;

		if (epochs[l].epoch_bpm == 0)
			continue;

		diff = fabs(epochs[l].epoch_ms - epochs[l-1].epoch_ms);
		if ((epochs[l-1].epoch_bpm != 0) && (diff >= 75))
			in_75_jump = 1;

 		if (in_75_jump == 1)
		{
			in_75_jump = 0;
 			continue;
		}

		if ((l - last_ok_idx) > 1)
		{
			add = (epochs[l].epoch_bpm - last_ok) / (l - last_ok_idx);
			cnt = 1;
			for (m = (last_ok_idx+1); m < l; m++)
			{
 				epochs[m].epoch_bpm = last_ok + (double)cnt * add;
				epochs[m].percent_noise = 100;
				epochs[m].interpolated = 1;
				cnt++;
			}
		}
		
		last_ok = epochs[l].epoch_bpm;
		last_ok_idx = l;
		in_75_jump = 0;
	}
	for (m = last_ok_idx; m < num; m++)
	{
		epochs[m].epoch_bpm = last_ok;
		epochs[m].percent_noise = 100;
		epochs[m].interpolated = 1;
	}

	for (l = 0; l < num; l++)
	{
		if (epochs[l].epoch_bpm == 0)
			epochs[l].epoch_ms = 0;
		else
			epochs[l].epoch_ms = 60000.0 / epochs[l].epoch_bpm;
	}

	return 0;
} /* interpolate_epochs() */


long
get_mode(struct epoch *epochs, long n, long *mode_cnt)
{
	long l;
	long mode;
	long *hist = NULL;
	long n_hist, hist_min, hist_max;
	
	get_histogram(epochs, n, &hist, &n_hist, &hist_min, &hist_max);

	*mode_cnt = 0;
	mode = 0;
	for (l = 0; l < n_hist; l++)
	{
		if ((l + hist_min) == 0)
			continue; /* skip zero */

		if (hist[l] > (*mode_cnt))
		{
			*mode_cnt = hist[l];
			mode = l + hist_min;
		}
	}

	if (hist)
		free(hist);

	return mode;
} /* get_mode() */


int
get_histogram(struct epoch *epochs, long n, long **hist, long *n_hist, long *min, long *max)
{
	double mi, ma;
	long l;

	mi = ma = epochs[0].epoch_bpm;
	for (l = 1; l < n; l++)
	{
		if (epochs[l].interpolated)
			continue;

		if (epochs[l].epoch_bpm < mi)
			mi = epochs[l].epoch_bpm;
		if (epochs[l].epoch_bpm > ma)
			ma = epochs[l].epoch_bpm;
	}
	*min = (long)(mi + 0.5);
	*max = (long)(ma + 0.5);
	*n_hist = (*max) - (*min) + 1;
	*hist = malloc(sizeof(long) * (*n_hist));
	memset(*hist, 0, sizeof(long) * (*n_hist));

	for (l = 0; l < n; l++)
	{
		long e;

		if (epochs[l].interpolated)
			continue;

		e = (long)(epochs[l].epoch_bpm + 0.5);
		(*hist)[e - (*min)]++;
	}

	return 0;
} /* get_histogram() */


/* Digital filter designed by mkfilter/mkshape/gencode   A.J. Fisher
   Command line: /www/usr/fisher/helpers/mkfilter -Bu -Lp -o 4 -a 6.3742032246e-03 0.0000000000e+00 -l */
int
filter_data(struct epoch *epochs, long n)
{
	long l;
	double *buf = NULL;
	double xv[FILT_NZEROS+1], yv[FILT_NPOLES+1];
	long mode, mode_cnt;
	double start;

	/* get start value */
	mode = get_mode(epochs, n, &mode_cnt);
	start = epochs[0].epoch_bpm;
	for (l = 1; l < 64; l++)
	{
		if (l >= n)
			break;

		if (fabs(epochs[l].epoch_bpm - (double)mode) < fabs(start - (double)mode))
			start = epochs[l].epoch_bpm;
	}

	/* first filter pass; store filterd data in buf */
	buf = malloc(sizeof(double) * n);
	for (l = 0; l < (FILT_NZEROS+1); l++)
		xv[l] = start / FILT_GAIN;
	for (l = 0; l < (FILT_NPOLES+1); l++)
		yv[l] = start;
	for (l = 0; l < n; l++)
	{
		xv[0] = xv[1]; xv[1] = xv[2]; xv[2] = xv[3]; xv[3] = xv[4]; 
		xv[4] = epochs[l].epoch_bpm / FILT_GAIN;
		yv[0] = yv[1]; yv[1] = yv[2]; yv[2] = yv[3]; yv[3] = yv[4]; 
		yv[4] =   (xv[0] + xv[4]) + 4 * (xv[1] + xv[3]) + 6 * xv[2]
			+ ( -0.9006264758 * yv[0]) + (  3.6967592426 * yv[1])
			+ ( -5.6914821245 * yv[2]) + (  3.8953469146 * yv[3]);
		buf[l] = yv[4];
	}

	/* second filter pass in opposite direction to remove phase shift; get data from buf */
	for (l = 0; l < (FILT_NZEROS+1); l++)
		xv[l] = buf[n-1] / FILT_GAIN;
	for (l = 0; l < (FILT_NPOLES+1); l++)
		yv[l] = buf[n-1];
	for (l = (n-1); l >= 0; l--)
	{
		xv[0] = xv[1]; xv[1] = xv[2]; xv[2] = xv[3]; xv[3] = xv[4]; 
		xv[4] = buf[l] / FILT_GAIN;
		yv[0] = yv[1]; yv[1] = yv[2]; yv[2] = yv[3]; yv[3] = yv[4]; 
		yv[4] =   (xv[0] + xv[4]) + 4 * (xv[1] + xv[3]) + 6 * xv[2]
			+ ( -0.9006264758 * yv[0]) + (  3.6967592426 * yv[1])
			+ ( -5.6914821245 * yv[2]) + (  3.8953469146 * yv[3]);
		epochs[l].filt_bpm = yv[4];
	}

	if (buf)
		free(buf);

	for (l = 0; l < n; l++)
		epochs[l].filt_ms = 60000.0 / epochs[l].filt_bpm;

	return 0;
} /* filter_data() */


int
estimate_baseline(struct epoch *epochs, long n)
{
	long l;
	long mod_mode;
	double mod_mode_ms;
	double bound_add = 60;

	mod_mode = get_mod_mode(epochs, n);
	mod_mode_ms = 60000 / (double)mod_mode;

	for (;;)
	{
		double lower_bound, upper_bound;
		long last_epoch;

		lower_bound = mod_mode_ms - bound_add;
		upper_bound = mod_mode_ms + bound_add;
		last_epoch = 0;
		for (l = 0; l < n; l++)
		{
			if (epochs[l].filt_ms < lower_bound)
				epochs[l].baseline_ms = lower_bound;
			else if (epochs[l].filt_ms > upper_bound)
				epochs[l].baseline_ms = upper_bound;
			else
			{
				epochs[l].baseline_ms = epochs[l].filt_ms;
				last_epoch = l;
			}

			if ((l-last_epoch) > (EPOCHS_MIN * 10))
				break;

			epochs[l].baseline_bpm = 60000 / epochs[l].baseline_ms;
		}
		if (l >= n)
			break;

		bound_add += 10;
	}

	return 0;
} /* estimate_baseline() */


long
get_mod_mode(struct epoch *epochs, long n)
{
	long l, cnt, cnt_12_5;
	long *hist = NULL;
	long n_hist, hist_min, hist_max;
	long mode, mode_cnt;
	long mod_mode;

	/* get "modified" mode */
	/* first get histogram */
	mode = get_mode(epochs, n, &mode_cnt);
	get_histogram(epochs, n, &hist, &n_hist, &hist_min, &hist_max);
	/* and than find peak which fulfill certain criteria  */
	cnt = 0;
	cnt_12_5 = (long)((double)n * 0.125) + 1;
	mod_mode = mode;
	for (l = 0; l < (n_hist-5); l++)
	{
		long m;
		int cont = 0;

		cnt += hist[l];
		if (cnt < cnt_12_5)
			continue;

		for (m = l; m < (l + 5); m++)
		{
			if (hist[l] <= hist[m])
			{
				cont = 1;
				break;
			}
		}
		if (cont)
			continue;

		if (((double)hist[l] / (double)n) >= 0.5)
		{
			mod_mode = hist_min + hist[l];
			break;
		}
		if (((60000/(double)hist[l]) - (60000/(double)mode_cnt)) <= 30)
		{
			mod_mode = hist_min + hist[l];
			break;
		}
	}

	if (hist)
		free(hist);

	return mod_mode;
} /* get_mod_mode() */


int
get_fhr_variations(struct epoch *epochs, long n, value_handle *res)
{
	double sig_lost = 0;
	double bhr = 0.0;
	long n_accel_10 = 0;
	long *pos_accel_10 = NULL;
	long n_accel_15 = 0;
	long *pos_accel_15 = NULL;
	long n_lost_beats_20 = 0;
	long n_lost_beats_21_100 = 0;
	long n_lost_beats_101 = 0;
	double *minute_range = NULL;
	double *minute_range_ms = NULL;
	long n_min = 0;
	long n_high_variations = 0;
	long n_low_variations = 0;
	double ltv_bpm = 0.0;
	double ltv_ms = 0.0;
	double ltv_high_bpm = 0.0;
	double ltv_high_ms = 0.0;
	double stv = 0.0;

	calc_signal_lost(epochs, n, &sig_lost);
	ra_value_set_double(res[SIGNAL_LOST], sig_lost);

	calc_basal_heart_rate(epochs, n, &bhr);
	ra_value_set_double(res[BASAL_FHR], bhr);

	find_accelerations(epochs, n, &n_accel_10, &pos_accel_10, &n_accel_15, &pos_accel_15);
	ra_value_set_long(res[ACCEL_10], n_accel_10);
	ra_value_set_long_array(res[POS_ACCEL_10], pos_accel_10, n_accel_10);
	ra_value_set_long(res[ACCEL_15], n_accel_15);
	ra_value_set_long_array(res[POS_ACCEL_15], pos_accel_15, n_accel_15);
	if (pos_accel_10)
		free(pos_accel_10);
	if (pos_accel_15)
		free(pos_accel_15);

	find_decelerations(epochs, n, &n_lost_beats_20, &n_lost_beats_21_100, &n_lost_beats_101);
	ra_value_set_long(res[LOST_BEATS_20], n_lost_beats_20);
	ra_value_set_long(res[LOST_BEATS_21_100], n_lost_beats_21_100);
	ra_value_set_long(res[LOST_BEATS_101], n_lost_beats_101);

	find_variations(epochs, n, &ltv_bpm, &ltv_ms, &ltv_high_bpm, &ltv_high_ms, &n_high_variations, &n_low_variations,
			&minute_range, &minute_range_ms, &n_min);
	ra_value_set_double_array(res[MINUTE_RANGE], minute_range, n_min);
	ra_value_set_double_array(res[MINUTE_RANGE_MS], minute_range_ms, n_min);
	ra_value_set_long(res[HIGH_VARIATIONS], n_high_variations);
	ra_value_set_long(res[LOW_VARIATIONS], n_low_variations);
	ra_value_set_double(res[LONG_TERM_VARIATIONS_BPM], ltv_bpm);
	ra_value_set_double(res[LONG_TERM_VARIATIONS_MS], ltv_ms);
	ra_value_set_double(res[LONG_TERM_VARIATIONS_HIGH_BPM], ltv_high_bpm);
	ra_value_set_double(res[LONG_TERM_VARIATIONS_HIGH_MS], ltv_high_ms);

	calc_short_term_variations(epochs, n, &stv);
	ra_value_set_double(res[SHORT_TERM_VARIATIONS], stv);

	if (minute_range)
		free(minute_range);
	if (minute_range_ms)
		free(minute_range_ms);

	return 0;
} /* get_fhr_variations() */


int
calc_signal_lost(struct epoch *epochs, long n, double *sig_lost)
{
	long l, cnt;
	
	*sig_lost = 0;

	cnt = 0;
	for (l = 0; l < n; l++)
		*sig_lost += epochs[l].percent_noise;
	*sig_lost /= (double)n;

	return 0;
} /* calc_signal_lost() */


int
find_accelerations(struct epoch *epochs, long n, long *n_accel_10, long **pos_accel_10,
		   long *n_accel_15, long **pos_accel_15)
{
	long l;
	long cnt_above, above_start;
	double max_excursion;

	*n_accel_10 = 0;
	*pos_accel_10 = NULL;
	*n_accel_15 = 0;
	*pos_accel_15 = NULL;	

	cnt_above = 0;
	max_excursion = 0;
	above_start = -1;
	for (l = 0; l < n; l++)
	{
		double diff = epochs[l].epoch_bpm - epochs[l].baseline_bpm;
		if (diff > 0)
		{
			if (above_start == -1)
				above_start = l;

			cnt_above++;
			if (diff > max_excursion)
				max_excursion = diff;
			continue;
		}

		if (cnt_above > 4)
		{
			long m, max_pos;
			double max;
			
			max = epochs[above_start].epoch_bpm;
			max_pos = above_start;

			for (m = above_start; m < l; m++)
			{
				epochs[m].in_accel = 1;
				if (epochs[m].epoch_bpm > max)
				{
					max = epochs[m].epoch_bpm;
					max_pos = m;
				}
			}

			if ((max_excursion > 10) && (max_excursion <= 15))
			{
				(*n_accel_10)++;
				(*pos_accel_10) = (long *)realloc((*pos_accel_10), sizeof(long) * (*n_accel_10));
				(*pos_accel_10)[(*n_accel_10) - 1] = max_pos;
			}
			if (max_excursion > 15)
			{
				(*n_accel_15)++;
				(*pos_accel_15) = (long *)realloc((*pos_accel_15), sizeof(long) * (*n_accel_15));
				(*pos_accel_15)[(*n_accel_15) - 1] = max_pos;
			}
		}

		cnt_above = 0;
		max_excursion = 0;
		above_start = -1;
	}

	return 0;
} /* find_accelerations() */


int
find_decelerations(struct epoch *epochs, long n, long *n_lost_beats_20, long *n_lost_beats_21_100, long *n_lost_beats_101)
{
	long l;
	long cnt_below, below_start;
	double max_excursion;

	*n_lost_beats_20 = 0;
	*n_lost_beats_21_100 = 0;
	*n_lost_beats_101 = 0;

	cnt_below = 0;
	max_excursion = 0;
	below_start = -1;
	for (l = 0; l < n; l++)
	{
		double diff = epochs[l].epoch_bpm - epochs[l].baseline_bpm;
		if (diff < 0)
		{
			if (below_start == -1)
				below_start = l;

			cnt_below++;
			if (diff < max_excursion)
				max_excursion = diff;
			continue;
		}

		if (((cnt_below > (EPOCHS_MIN/2)) && (max_excursion < -20)) ||
		    ((cnt_below > EPOCHS_MIN) && (max_excursion < -10)))
		{
			long m;
			double num_beats_baseline = 0;
			double num_beats_epoch = 0;
			double div = 1.0 / (double)EPOCHS_MIN;

			for (m = below_start; m < l; m++)
			{
				epochs[m].in_decel = 1;

				num_beats_baseline += (epochs[m].baseline_bpm / div);
				num_beats_epoch += (epochs[m].epoch_bpm / div);
			}

			diff = num_beats_baseline - num_beats_epoch;
			if (diff <= 20)
				(*n_lost_beats_20)++;
			if ((diff >= 21) && (diff <= 100))
				(*n_lost_beats_21_100)++;
			if (diff > 100)
				(*n_lost_beats_101)++;
		}

		cnt_below = 0;
		max_excursion = 0;
		below_start = -1;
	}

	return 0;
} /* find_decelerations() */


int
find_variations(struct epoch *epochs, long n, double *ltv_bpm, double *ltv_ms, double *ltv_high_bpm, double *ltv_high_ms,
		long *n_high_variations, long *n_low_variations, double **minute_range, double **minute_range_ms, long *n_min)
{
	long l, n_use;
	int *ignore_minute = NULL;
	long n_minutes_ok = 0;
	int *in_high_variation = NULL;
	int *in_low_variation = NULL;

	*ltv_bpm = 0.0;
	*ltv_ms = 0.0;
	*ltv_high_bpm = 0.0;
	*ltv_high_ms = 0.0;
	*n_high_variations = 0;
	*n_low_variations = 0;

	*n_min = (long)((double)n / EPOCHS_MIN);
	ignore_minute = calloc(*n_min, sizeof(int));
	*minute_range = calloc(*n_min, sizeof(double));
	*minute_range_ms = calloc(*n_min, sizeof(double));
	in_high_variation = calloc(*n_min, sizeof(int));
	in_low_variation = calloc(*n_min, sizeof(int));

	n_use = n - (n % EPOCHS_MIN);	/* use only complete minutes */
	for (l = 0; l < n_use; l += EPOCHS_MIN)
	{
		long m;
		double sig_lost = 0.0;

		for (m = l; m < (l + EPOCHS_MIN); m++)
		{
			if (epochs[m].in_decel)
			{
				ignore_minute[l/EPOCHS_MIN] = 1;
				break;
			}

			sig_lost += epochs[l].percent_noise;
		}
		sig_lost /= (double)EPOCHS_MIN;
		if (sig_lost > 50.0)
			ignore_minute[l/EPOCHS_MIN] = 1;
	}

	for (l = 0; l < n_use; l += EPOCHS_MIN)
	{
		long m;
		double max, min, max_ms, min_ms;
		
		if (ignore_minute[l/EPOCHS_MIN])
			continue;

		max = min = 0;
		max_ms = min_ms = 0.0;
		for (m = l; m < (l + EPOCHS_MIN); m++)
		{
			double diff = epochs[m].epoch_bpm - epochs[m].baseline_bpm;
			if (diff > max)
			{
				max = diff;
				max_ms = 60000.0/epochs[m].epoch_bpm - 60000.0/epochs[m].baseline_bpm;
			}
			if (diff < min)
			{
				min = diff;
				min_ms = 60000.0/epochs[m].epoch_bpm - 60000.0/epochs[m].baseline_bpm;
			}
		}
  		(*minute_range)[l/EPOCHS_MIN]= max + fabs(min);
 		(*minute_range_ms)[l/EPOCHS_MIN] = fabs(max_ms) + fabs(min_ms);
		(*ltv_bpm) += (*minute_range)[l/EPOCHS_MIN];
		(*ltv_ms) += (*minute_range_ms)[l/EPOCHS_MIN];
		n_minutes_ok++;
	}
	(*ltv_bpm) /= (double)n_minutes_ok;
	(*ltv_ms) /= (double)n_minutes_ok;

	for (l = 5; l < *n_min; l++)
	{
		long m;
		int below_cnt = 0;
		int above_cnt = 0;
		long s, e;

		for (m = (l - 5); m <= l; m++)
		{
			if (ignore_minute[m])
				continue;

 			if ((*minute_range_ms)[m] <= 30)
 				below_cnt++;
 			if ((*minute_range_ms)[m] >= 32)
 				above_cnt++;
		}

		s = (l - 5) * EPOCHS_MIN;
		e = (l + 1) * EPOCHS_MIN;

		if (below_cnt >= 5)
		{
			for (m = (l - 5); m <= l; m++)
				in_low_variation[m] = 1;
			for (m = s; m < e; m++)
				epochs[m].in_low_variation = 1;
		}
		if (above_cnt >= 5)
		{
			for (m = (l - 5); m <= l; m++)
				in_high_variation[m] = 1;
			for (m = s; m < e; m++)
				epochs[m].in_high_variation = 1;
		}
	}

	for (l = 0; l < *n_min; l++)
	{
		if (in_low_variation[l])
			(*n_low_variations)++;
		if (in_high_variation[l])
		{
			(*n_high_variations)++;
			(*ltv_high_bpm) += (*minute_range)[l];
			(*ltv_high_ms) += (*minute_range_ms)[l];
		}
	}
	if ((*n_high_variations) > 0)
	{
		(*ltv_high_bpm) /= (double)(*n_high_variations);
		(*ltv_high_ms) /= (double)(*n_high_variations);
	}

	if (ignore_minute)
		free(ignore_minute);
	if (in_low_variation)
		free(in_low_variation);
	if (in_high_variation)
		free(in_high_variation);

	return 0;
} /* find_variations() */


int
calc_short_term_variations(struct epoch *epochs, long n, double *svt)
{
	long n_min, l, n_use;
	int *ignore_minute = NULL;
	long n_minutes_ok = 0;
	
	*svt = 0.0;

	n_min = (long)((double)n / EPOCHS_MIN);
	ignore_minute = calloc(n_min, sizeof(int));

	n_use = n - (n % EPOCHS_MIN);	/* use only complete minutes */
	for (l = 0; l < n_use; l += EPOCHS_MIN)
	{
		long m;
		double sig_lost = 0.0;

		for (m = l; m < (l + EPOCHS_MIN); m++)
		{
			if (epochs[m].in_decel)
			{
				ignore_minute[l/EPOCHS_MIN] = 1;
				break;
			}

			sig_lost += epochs[m].percent_noise;
		}
		sig_lost /= (double)EPOCHS_MIN;
		if (sig_lost > 50.0)
			ignore_minute[l/EPOCHS_MIN] = 1;
	}

	for (l = 0; l < n_use; l += EPOCHS_MIN)
	{
		long m, cnt;
		double s = 0.0;
		
		if (ignore_minute[l/EPOCHS_MIN])
			continue;

		cnt = 0;
		for (m = l+1; m < (l + EPOCHS_MIN); m++)
		{
			if (epochs[m].interpolated || epochs[m-1].interpolated)
				continue;
			if ((epochs[m].epoch_ms == 0) || (epochs[m-1].epoch_ms == 0))
				continue;

			s += fabs((epochs[m].epoch_ms - epochs[m-1].epoch_ms));
			cnt++;
		}
		(*svt) += (s / (double)cnt);
		n_minutes_ok++;
	}

	if (n_minutes_ok > 0)
		(*svt) /= (double)n_minutes_ok;

	if (ignore_minute)
		free(ignore_minute);

	return 0;
} /* calc_short_term_variations() */


int
calc_basal_heart_rate(struct epoch *epochs, long n, double *bhr)
{
	long l, cnt;
	
	cnt = 0;
	for (l = 0; l < n; l++)
	{
		if (!epochs[l].in_low_variation)
			continue;
		(*bhr) += epochs[l].epoch_bpm;
		cnt++;
	}

	if (cnt > 0)
		(*bhr) /= (double)cnt;
	else
		(*bhr) = (double)get_mod_mode(epochs, n);

	return 0;
} /* calc_basal_heart_rate() */


/* TODO: extend eval-structure to handle calculation-results */
int
save(proc_handle proc, eval_handle eh)
{
	if (proc || eh)
		;

	return 0;
} /* save() */

