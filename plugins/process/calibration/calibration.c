/*----------------------------------------------------------------------------
 * calibration.c  -  measure calibration signals in a recording
 *
 * Description:
 * The plugin tries to detect and measure a calibration signal in a
 * recording channel.
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2004-2005, Raphael Schneider
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
#include "calibration.h"


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
	if (ra_plugin_get_by_name(ra, "gui-calibration", 1))
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
	{
		_ra_set_error(pl, 0, "plugin-calibration");
		return NULL;
	}

	memset(pi, 0, sizeof(struct proc_info));
	pi->handle_id = RA_HANDLE_PROC;
	pi->plugin = pl;

	/* init options */
	pi->options = calloc(1, sizeof(struct ra_calibration));
	set_option_offsets(ps->info.opt, pi->options);
	set_default_options(pi->options);

	return pi;
} /* pl_get_proc_handle() */


void
set_default_options(struct ra_calibration *opt)
{
	opt->rh = NULL;
	opt->ch = -1;
	opt->start_pos = 0;
	opt->end_pos = 0;

	opt->use_data = 0;
	opt->num_data = 0;
	opt->data = NULL;
	opt->samplerate = 0;

	opt->type = CALIB_TYPE_AUTO;
	opt->difference_lag = 1;
	opt->min_calib_amp = 50;
	opt->segment_length = 60;
	opt->min_level_length = 0.08;
	opt->calib_cycle_length = 1;
	opt->get_cycle_length = 1;
	opt->min_cycles = 5;
} /* set_default_options() */


void
set_option_offsets(struct ra_option_infos *opt_inf, struct ra_calibration *opt)
{
	unsigned char *p = (unsigned char *)opt;
	int idx = 0;

	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->rh) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->ch) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->start_pos) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->end_pos) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->use_data) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->num_data) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->data) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->samplerate) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->type) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->difference_lag) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->min_calib_amp) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->segment_length) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->min_level_length) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->calib_cycle_length) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->get_cycle_length) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->min_cycles) - p);

	/* consistency check (useful when changing option structure) */
	if (idx != num_options)
	{
		fprintf(stderr, "critical error in calibration.c - set_option_offsets():\n"
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
		struct ra_calibration *opt;

		opt = (struct ra_calibration *)pi->options;
		if (opt->data)
			ra_free_mem(opt->data);
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
	double *values = NULL;
	long l, num;
	value_handle *res;

	if (!pi)
		return -1;

	/* if rec-handle was not set in options, use root recording */
	if (!((struct ra_calibration *)pi->options)->rh)
		((struct ra_calibration *)pi->options)->rh = ra_rec_get_first(pi->mh, 0);

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

	num = 0;
	if ((ret = get_data(pi->options, &values, &num)) != 0)
		goto clean_up;
	if (num <= 0)
	{
		_ra_set_error(pi->ra, RA_WARN_NO_DATA, "plugin-calibration");
		goto clean_up;
	}

	if ((ret = get_calibration(pi->options, values, num, res)) != 0)
		goto clean_up;

	ret = 0;

 clean_up:
	if (values)
		free(values);

	return ret;
} /* pl_do_processing() */


int
get_data(struct ra_calibration *opt, double **values, long *num)
{
	int ret = -1;

	if (opt->use_data)
	{
		if ((opt->num_data <= 0) || (opt->data == NULL) || (opt->samplerate <= 0))
		{
			_ra_set_error(opt->rh, RA_ERR_INFO_MISSING, "plugin-calibration");
			return ret;
		}

		*values = malloc(sizeof(double) * opt->num_data);
		memcpy(*values, opt->data, sizeof(double) * opt->num_data);
		*num = opt->num_data;

		ret = 0;
	}
	else
		ret = get_raw_data(opt, values, num);


	return ret;
} /* get_data() */


int
get_raw_data(struct ra_calibration *opt, double **values, long *num)
{
	int ret = 0;
	long start_pos, end_pos;
	long l, v;
	DWORD *lv;
	value_handle vh;

	if (!opt->rh || (opt->ch < 0))
	{
		_ra_set_error(opt->rh, RA_ERR_INFO_MISSING, "plugin-calibration");
		return -1;
	}

	/* get number of values */
	start_pos = opt->start_pos;
	end_pos = opt->end_pos;
	*num = end_pos - start_pos;

	/* get values */
	lv = malloc(sizeof(DWORD) * (*num));
	*num = (long)ra_raw_get(opt->rh, opt->ch, start_pos, *num, lv, NULL);
	*values = malloc(sizeof(double) * (*num));
	for (l = 0; l < *num; l++)
	{
		v = lv[l];
		(*values)[l] = (double)v;
	}
	free(lv);

	/* get samplerate */
	vh = ra_value_malloc();
	ra_value_set_number(vh, opt->ch);
	ra_info_get(opt->rh, RA_INFO_REC_CH_SAMPLERATE_D, vh);
	opt->samplerate = ra_value_get_double(vh);
	ra_value_free(vh);

	return ret;
} /* get_raw_data() */


int
get_calibration(struct ra_calibration *opt, double *values, long num, value_handle *res)
{
	int ret = -1;
	struct cycle_info *ci = NULL;
	long num_cycles = 0;
	long l, num_valid, num_valid_max;

	if ((ret = get_cycles(opt, values, num, &ci, &num_cycles)) != 0)
		goto error;
	if (num_cycles <= 0)
	{
		_ra_set_error(opt->rh, RA_WARN_NO_DATA, "plugin-calibration");
		goto error;
	}

	if ((ret = check_cycles_step1(opt, ci, num_cycles, values, num)) != 0)
	{
		_ra_set_error(opt->rh, RA_WARN_NO_DATA, "plugin-calibration");
		goto error;
	}

	if (opt->get_cycle_length)
	{
		if ((ret = get_calib_width(opt, values, num, ci, num_cycles)) != 0)
			goto error;

		/* if the cycle-length is measured, use this info to filter out cycles
		   with a different cycle-length (likely false positive cycles) */
		if ((ret = check_cycles_step2(opt, ci, num_cycles)) != 0)
		{
			_ra_set_error(opt->rh, RA_WARN_NO_DATA, "plugin-calibration");
			goto error;
		}
	}

	num_valid = 0;
	num_valid_max = 0;
	for (l = 1; l < num_cycles; l++)
	{
		if (!ci[l-1].valid || !ci[l].valid)
		{
			num_valid = 0;
			continue;
		}
		if ((ci[l-1].end + 1) != ci[l].start)
		{
			num_valid = 0;
			continue;
		}

		num_valid++;
		if (num_valid > num_valid_max)
			num_valid_max = num_valid;
	}
	if ((num_valid_max < opt->min_cycles) || (num_cycles == 0))
	{
		_ra_set_error(opt->rh, RA_WARN_NO_DATA, "plugin-calibration");
		goto error;
	}

	if ((ret = get_results(opt, values, num, ci, num_cycles, res)) != 0)
		goto error;

	ret = 0;
 error:
	if (ci)
		free(ci);

	return ret;
} /* get_calibration() */


int
get_cycles(struct ra_calibration *opt, double *values, long num, struct cycle_info **ci, long *num_cycles)
{
	int ret = -1;
	long l;
	double *der = NULL;
	long num_der = 0;
	int *flags = NULL;

	/* calc 1st derivative */
	der = (double *)malloc(sizeof(double) * num);
	for (l = 0; l < num-1; l++)
		der[l] = values[l+1] - values[l];
	num_der = num - 1;

	/* get flags of changes/non-changes */
	flags = (int *)malloc(sizeof(int) * num_der);
	if ((ret = get_change_flags(opt, der, num_der, flags)) != 0)
		goto error;

	/* select cycles (change from bottom->top followed by top->bottom) */
	if ((ret = select_cycles(flags, num_der, ci, num_cycles)) != 0)
		goto error;
	if (*num_cycles <= 0)
	{
		_ra_set_error(opt->rh, RA_WARN_NO_DATA, "plugin-calibration");
		goto error;
	}

	ret = 0;

 error:
	if (der)
		free(der);
	if (flags)
		free(flags);

	return ret;
} /* get_cycles() */


int
get_change_flags(struct ra_calibration *opt, double *data, long num, int *flags)
{
	long l;
	long seg_len_su, next_segment;
	double max_pos, max_neg, threshold_pos, threshold_neg;

	seg_len_su = (long)(opt->segment_length * opt->samplerate);
	next_segment = 0;
	threshold_pos = threshold_neg = 0.0;
	for (l = 0; l < num; l++)
	{
		if (l >= next_segment)
		{
			long n_search;

			n_search = seg_len_su;
			if ((l+n_search) >= (num - opt->difference_lag))
				n_search = (num - opt->difference_lag) - l - 1;

			get_max_diff(data+l, n_search, opt->difference_lag, &max_pos, &max_neg);
			threshold_pos = max_pos * (opt->min_calib_amp / 100.0);
			threshold_neg = max_neg * (opt->min_calib_amp / 100.0);
			next_segment += seg_len_su;
		}

		if (data[l] > threshold_pos)
			flags[l] = 1;
		else if (data[l] < threshold_neg)
			flags[l] = -1;
		else
			flags[l] = 0;
	}

	return 0;
} /* get_change_flags() */


int
get_max_diff(double *data, long n, long diff_lag, double *max_pos, double *max_neg)
{
	long l;

	*max_pos = 0;
	*max_neg = 0;
	for (l = diff_lag; l < n; l++)
	{
		double curr = data[l] - data[l-diff_lag];
		if (curr > *max_pos)
			*max_pos = curr;
		if (curr < *max_neg)
			*max_neg = curr;
	}

	return 0;
} /* get_max_diff() */


int
select_cycles(int *flags, long num, struct cycle_info **ci, long *num_cycles)
{
	long l;
	int state;
	long start;
	long num_low_level;
	long low_start, low_end;
	long num_high_level;
	long high_start, high_end;

	*ci = NULL;
	*num_cycles = 0;

	state = ST_LEVEL_LOW;
	num_low_level = num_high_level = 0;
	start = low_start = high_start = high_end = 0;
	for (l = 0; l < num; l++)
	{
		if ((state == ST_ERROR) && (flags[l] == 0))
		{
			state = ST_LEVEL_LOW;
			continue;
		}

		if ((state == ST_LEVEL_LOW) && (flags[l] == 0))
		{
			num_low_level++;
			continue;
		}
		if ((state == ST_LEVEL_LOW) && (flags[l] == 1))
		{
			low_end = l - 1;

			if ((*num_cycles) > 0)
			{				
				(*ci)[(*num_cycles)-1].start = start;
				(*ci)[(*num_cycles)-1].end = l-1;
				(*ci)[(*num_cycles)-1].num_low_level = num_low_level;
				(*ci)[(*num_cycles)-1].start_low = low_start;
				(*ci)[(*num_cycles)-1].end_low = low_end;
				(*ci)[(*num_cycles)-1].num_high_level = num_high_level;
				(*ci)[(*num_cycles)-1].start_high = high_start;
				(*ci)[(*num_cycles)-1].end_high = high_end;
			}

			(*num_cycles)++;
			*ci = (struct cycle_info *)realloc(*ci, sizeof(struct cycle_info) * (*num_cycles));
			memset(&((*ci)[(*num_cycles)-1]), 0, sizeof(struct cycle_info));
			start = l;
			state = ST_IN_CHANGE_UP;

			num_low_level = 0;
			num_high_level = 0;

			continue;
		}
		if ((state == ST_LEVEL_LOW) && (flags[l] == -1))
		{
			state = ST_ERROR;
			continue;
		}

		if ((state == ST_IN_CHANGE_UP) && (flags[l] == 0))
		{
			num_high_level = 1;
			high_start = l;
			state = ST_LEVEL_HIGH;
			continue;
		}
		if ((state == ST_IN_CHANGE_UP) && (flags[l] == 1))
		{
			continue;
		}
		if ((state == ST_IN_CHANGE_UP) && (flags[l] == -1))
		{
			state = ST_ERROR;
			continue;
		}

		if ((state == ST_LEVEL_HIGH) && (flags[l] == 0))
		{
			num_high_level++;
			continue;
		}
		if ((state == ST_LEVEL_HIGH) && (flags[l] == 1))
		{
			start = l;
			num_low_level = 0;
			num_high_level = 0;
			state = ST_IN_CHANGE_UP;
			continue;
		}
		if ((state == ST_LEVEL_HIGH) && (flags[l] == -1))
		{
			high_end = l-1;
			state = ST_IN_CHANGE_DOWN;
			continue;
		}

		if ((state == ST_IN_CHANGE_DOWN) && (flags[l] == 0))
		{
			num_low_level = 1;
			low_start = l;
			state = ST_LEVEL_LOW;
			continue;
		}
		if ((state == ST_IN_CHANGE_DOWN) && (flags[l] == 1))
		{
			state = ST_ERROR;
			continue;
		}
		if ((state == ST_IN_CHANGE_DOWN) && (flags[l] == -1))
		{
			continue;
		}
	}

	return 0;
} /* select_cycles() */


int
check_cycles_step1(struct ra_calibration *opt, struct cycle_info *ci, long num_cycles, double *data, long num)
{
	long l, m, n_valid, cnt;
	long min_level_len;
	long cycle_len_1_10, cycle_len_4;
	double mean;
	long change_cnt;

	min_level_len = (long)(opt->min_level_length * opt->samplerate);
	cycle_len_1_10 = (long)(opt->calib_cycle_length * opt->samplerate / 10.0);
	cycle_len_4 = (long)(opt->calib_cycle_length * opt->samplerate * 4.0);
	n_valid = 0;
	for (l = 0; l < num_cycles; l++)
	{
		/* check minimum length of staying on the same level */
		if ((ci[l].num_high_level < min_level_len) || (ci[l].num_low_level < min_level_len))
			continue;
		/* check if found cycle is less than 1/10th of the given cycle-length*/
		if ((ci[l].num_high_level + ci[l].num_low_level) < cycle_len_1_10)
			continue;
		/* check if found cycle is longer than four times the given cycle-length*/
		if ((ci[l].num_high_level + ci[l].num_low_level) > cycle_len_4)
			continue;

		/* calculate the changes of the values regarding the mean */
		mean = 0.0;
		cnt = 0;
		for (m = ci[l].start; m <= ci[l].end; m++)
		{
			if (cnt >= num)
				break;
			mean += data[m];
			cnt++;
		}
		mean /= (double)cnt;

		change_cnt = 0;
		cnt = 0;
		for (m = ci[l].start+1; m <= ci[l].end; m++)
		{
			if (cnt >= num)
				break;

			if ((data[m-1] >= mean) && (data[m] < mean))
				change_cnt++;
			if ((data[m-1] < mean) && (data[m] >= mean))
				change_cnt++;

			cnt++;
		}
		if (change_cnt < 10)
			ci[l].valid = 1;

		if (ci[l].valid)
			n_valid++;
	}

	if (n_valid > 0)
		return 0;
	else
		return -1;
} /* check_cycles_step1() */


int
check_cycles_step2(struct ra_calibration *opt, struct cycle_info *ci, long num_cycles)
{
	long l, n_valid;
	long min_cycle_len, max_cycle_len;

 	min_cycle_len = (long)(opt->calib_cycle_length * opt->samplerate * 0.95);
 	max_cycle_len = (long)(opt->calib_cycle_length * opt->samplerate * 1.01);
	n_valid = 0;
	for (l = 0; l < num_cycles; l++)
	{
		long s;

		if (!ci[l].valid)
			continue;

		s = ci[l].num_high_level + ci[l].num_low_level;
		if ((s < min_cycle_len) || (s > max_cycle_len))
			ci[l].valid = 0;

		if (ci[l].valid)
			n_valid++;
	}

	if (n_valid > 0)
		return 0;
	else
		return -1;
} /* check_cycles_step2() */


int
get_calib_width(struct ra_calibration *opt, double *data, long num, struct cycle_info *ci, long num_cycles)
{
	double *data_use;
	long l, num_data;
	double *corr;
	long num_corr;
	double corr_threshold, max_corr;
	long up_pos, down_pos, max_corr_pos;

	/* select data of valid cycles and make an artificial signal for further analysis
	   (for high- and low-level values use the mean instead the original data) */
	data_use = (double *)malloc(sizeof(double) * num);
	num_data = 0;
	for (l = 0; l < num_cycles; l++)
	{
		long m, cnt;
		double mean_high, mean_low;

		if (!ci[l].valid)
			continue;

		mean_high = 0.0;
		cnt = 0;
		for (m = ci[l].start_high; m <= ci[l].end_high; m++)
		{
			mean_high += data[m];
			cnt++;
		}
		mean_high /= (double)cnt;

		mean_low = 0.0;
		cnt = 0;
		for (m = ci[l].start_low; m <= ci[l].end_low; m++)
		{
			mean_low += data[m];
			cnt++;
		}
		mean_low /= (double)cnt;

		for (m = ci[l].start; m <= ci[l].end; m++)
		{
			if ((m >= ci[l].start_high) && (m <= ci[l].end_high))
				data_use[num_data] = mean_high;
			else if ((m >= ci[l].start_low) && (m <= ci[l].end_low))
				data_use[num_data] = mean_low;
			else
				data_use[num_data] = data[m];
			num_data++;
		}
	}

	/* first get auto-correlation of found calibration signal (with a max. lag of 120%
	   of given cycle length */
	num_corr = (long)(1.2 * opt->calib_cycle_length * opt->samplerate);
	corr = (double *)malloc(sizeof(double) * num_corr);
	acorf(data_use, num_data, corr, num_corr);

	/* now get the maximum absolute signal change in the correlation signal
	   (the change is measured between corr-values with a difference of AUTOCORR_DIFF_LAG) */
	corr_threshold = 0;
	for (l = 0; l < (num_corr - AUTOCORR_DIFF_LAG); l++)
	{
		double d;
		
		d = fabs(corr[l + AUTOCORR_DIFF_LAG] - corr[l]);
		if (d > corr_threshold)
			corr_threshold = d;
	}

	/* now look where the signal goes first up and then down over the above
	   meassured threshold (again use AUTOCORR_DIFF_LAG for the change measurement)*/
	up_pos = 0;
	down_pos = num_corr;
	corr_threshold *= 0.25;
	for (l = 0; l < (num_corr - AUTOCORR_DIFF_LAG); l++)
	{
		if ((up_pos == 0) && ((corr[l + AUTOCORR_DIFF_LAG] - corr[l]) >= corr_threshold))
			up_pos = l;
		if ((up_pos > 0) && ((corr[l + AUTOCORR_DIFF_LAG] - corr[l]) <= -corr_threshold))
		{
			down_pos = l + AUTOCORR_DIFF_LAG;
			break;
		}
	}	
	if (up_pos == 0)
	{
		free(corr);
		free(data_use);
		_ra_set_error(opt->rh, RA_ERR_ALGORITHM, "plugin-calibration");
		return -1;
	}

	/* find the position with the maximum correlation -> a new cycle starts */
	max_corr = corr[up_pos];
	max_corr_pos = up_pos;
	for (l = up_pos; l < down_pos; l++)
	{
		if (corr[l] > max_corr)
		{
			max_corr = corr[l];
			max_corr_pos = l;
		}
	}
	opt->calib_cycle_length = (double)max_corr_pos / opt->samplerate;

	free(corr);
	free(data_use);

	return 0;
} /* get_calib_width() */


int
acorf(const double *data, long num, double *corr, long max_lag)
{
	long l;
	double value;
	
	if ((max_lag < 1) || (num <= max_lag))
		return -1;

	corr[0] = 1.0;
	for (l = 1; l < max_lag; l++)
	{
		calc_correlation(data, data+l, (num-max_lag), &value);
		corr[l] = value;
	}

	return 0;
} /* acorf() */


int
calc_correlation(const double* x, const double* y, long len, double *corr)
{
	int ret = 0;
	long l;
	double mean_x = 0.0;
	double mean_y = 0.0;
	double var_x = 0.0;
	double var_y = 0.0;
	double co_var_xy = 0.0;
	double temp, sqrt_var_x, sqrt_var_y;

	if (len < 2)
	{
		*corr = 0.0;
		return -1;
	}

	for (l = 0; l < len; l++)
	{
		mean_x += x[l];
		mean_y += y[l];
	}
	mean_x /= (double)len;
	mean_y /= (double)len;

	for (l = 0; l < len; l++)
	{
		var_x += ((x[l] - mean_x) * (x[l] - mean_x));
		var_y += ((y[l] - mean_y) * (y[l] - mean_y));

		co_var_xy += ((x[l] - mean_x) * (y[l] - mean_y));
	}

	temp = (1.0/((double)len-1.0));

	sqrt_var_x = sqrt(temp * var_x);
	sqrt_var_y = sqrt(temp * var_y);

	if ((sqrt_var_x == 0.0) || (sqrt_var_y == 0.0))
		ret = -1;
	else
		*corr = ((temp * co_var_xy) / (sqrt_var_x * sqrt_var_y));

	return ret;
}  /* calc_correlation() */


int
get_periodogram(struct ra_calibration *opt, double *data, long num, double **spec, double **freq, long *n_spec)
{
	plugin_handle pl;
	struct proc_info *pi_freq;
	value_handle vh;

	pl = ra_plugin_get_by_name(ra_lib_handle_from_any_handle(opt->rh), "freq-analysis", 0);
	if (!pl)
		return -1;

	pi_freq = (struct proc_info *)ra_proc_get(ra_meas_handle_from_any_handle(opt->rh), pl, NULL);
	
	vh = ra_value_malloc();
	ra_value_set_short(vh, 1);
	ra_lib_set_option((proc_handle)pi_freq, "use_data", vh);
	ra_value_set_double_array(vh, data, num);
	ra_lib_set_option((proc_handle)pi_freq, "data", vh);

	ra_value_set_double(vh, opt->samplerate);
	ra_lib_set_option((proc_handle)pi_freq, "samplerate", vh);
	
	ra_value_set_string(vh, "");
	ra_lib_set_option((proc_handle)pi_freq, "window", vh);
	ra_value_set_short(vh, 1);
	ra_lib_set_option((proc_handle)pi_freq, "remove_mean", vh);
	ra_value_set_long(vh, num*20);
	ra_lib_set_option((proc_handle)pi_freq, "num_freq", vh);
	
	if (ra_proc_do(pi_freq) != 0)
	{
		ra_proc_free(pi_freq);
		ra_value_free(vh);
		return -1;
	}

	/* get frequency values */
	if (ra_proc_get_result(pi_freq, 1, 0, vh) == 0)
	{
		*n_spec = ra_value_get_num_elem(vh);
		*freq = malloc(sizeof(double) * ra_value_get_num_elem(vh));
		memcpy(*freq, ra_value_get_double_array(vh), sizeof(double) * ra_value_get_num_elem(vh));
	}
	/* get spectrum values */
	if (ra_proc_get_result(pi_freq, 1, 0, vh) == 0)
	{
		*spec = malloc(sizeof(double) * ra_value_get_num_elem(vh));
		memcpy(*spec, ra_value_get_double_array(vh), sizeof(double) * ra_value_get_num_elem(vh));
	}

	ra_proc_free(pi_freq);
	ra_value_free(vh);

	return 0;
} /* get_periodogram() */


int
get_results(struct ra_calibration *opt, double *data, long num, struct cycle_info *ci, long num_cycles, value_handle *res)
{
	int ret = 0;
	long l, m, cnt;
	int *level = NULL;
	long high_cnt, low_cnt;
	double *high_values, *low_values;
	double median_high, median_low;
	double *calib_data = NULL;
	long *calib_pos = NULL;
	long num_valid;
	double max_value, min_value;
	int range_check_nok;
	long out_of_range;

	level = (int *)calloc(num, sizeof(int));
	ret = select_levels(ci, num_cycles, level);

	high_cnt = low_cnt = 0;
	high_values = (double *)malloc(sizeof(double) * num);
	low_values = (double *)malloc(sizeof(double) * num);
	for (l = 0; l < num; l++)
	{		
		if (level[l] == CALIB_LEVEL_HIGH)
			high_values[high_cnt++] = data[l];
		if (level[l] == CALIB_LEVEL_LOW)
			low_values[low_cnt++] = data[l];
	}

	if ((high_cnt > 0) && (low_cnt > 0))
	{
		double height;
		double var, accuracy;

		qsort(high_values, high_cnt, sizeof(double), qsort_double_compare);
		median_high = high_values[high_cnt / 2];
		qsort(low_values, low_cnt, sizeof(double), qsort_double_compare);
		median_low = low_values[low_cnt / 2];

		ra_value_set_double(res[CALIB_HIGH_VALUE], median_high);
		ra_value_set_double(res[CALIB_LOW_VALUE], median_low);
		height = median_high - median_low;
		ra_value_set_double(res[CALIB_HEIGHT], height);

		/* calculate accuracy of calibration height */
		var = high_values[(long)((double)high_cnt * 0.75)] - 
			  high_values[(long)((double)high_cnt * 0.25)];
		var += low_values[(long)((double)low_cnt * 0.75)] - 
			   low_values[(long)((double)low_cnt * 0.25)];
		var /= 2.0;
		accuracy = ((height - var) / height) * 100.0;
		ra_value_set_double(res[CALIB_ACCURACY], accuracy);
	}
	else
	{
		_ra_set_error(opt->rh, RA_WARN_NO_DATA, "plugin-calibration");
		ret = 1;
	}

	range_check_nok = get_min_max_values(opt, &min_value, &max_value);

	/* store valid calibration signals in the results; store start- and
	   end-positions of valid calibration cycles; check if values hit
	   value range */
	calib_data = (double *)malloc(sizeof(double) * num);
	cnt = 0;
	calib_pos = (long *)malloc(sizeof(long) * num);
	num_valid = 0;
	out_of_range = 0;
	for (l = 0; l < num_cycles; l++)
	{
		if (!ci[l].valid)
			continue;

		for (m = ci[l].start; m <= ci[l].end; m++)
		{
			calib_data[cnt++] = data[m];
			if (!range_check_nok && ((data[m] == min_value) || (data[m] == max_value)))
				out_of_range = 1;
		}

		calib_pos[num_valid] = ci[l].start;
		calib_pos[num_valid+1] = ci[l].end;
		num_valid += 2;
	}

	ra_value_set_double_array(res[CALIB_DATA], calib_data, cnt);
	free(calib_data);
	ra_value_set_long_array(res[CALIB_CYCLE_POS], calib_pos, num_valid);
	free(calib_pos);
	if (range_check_nok == 0)
		ra_value_set_long(res[CALIB_OUT_OF_RANGE], out_of_range);

	/* store calibration width in the results */
	ra_value_set_double(res[CALIB_WIDTH], opt->calib_cycle_length);

	/* clean up */
	free(high_values);
	free(low_values);
	free(level);

	return ret;
} /* get_results() */


int
select_levels(struct cycle_info *ci, long num_cycles, int *level)
{
	long l, m;

	for (l = 0; l < num_cycles; l++)
	{
		if (!ci[l].valid)
			continue;

		for (m = ci[l].start_high; m <= ci[l].end_high; m++)
			level[m] = CALIB_LEVEL_HIGH;

		for (m = ci[l].start_low; m <= ci[l].end_low; m++)
			level[m] = CALIB_LEVEL_LOW;
	}

	return 0;
}  /* select_levels() */


int
get_min_max_values(struct ra_calibration *opt, double *min_value, double *max_value)
{
	value_handle vh;

	if (!opt->rh)
		return -1;

	vh = ra_value_malloc();
	ra_value_set_number(vh, opt->ch);
	if (ra_info_get(opt->rh, RA_INFO_REC_CH_NUM_BITS_L, vh) != 0)
	{
		ra_value_free(vh);
		return -1;
	}

	*min_value = 0.0;
	*max_value = pow(2.0, ra_value_get_long(vh)) - 1.0;

	ra_value_free(vh);

	return 0;
}  /* get_min_max_values() */


int
qsort_double_compare(const void *v1, const void *v2)
{
	double l1, l2;

	l1 = *(double *)v1;
	l2 = *(double *)v2;

	if (l1 > l2)
		return 1;
	else if (l1 < l2)
		return -1;

	return 0;
} /* qsort_double_compare() */




