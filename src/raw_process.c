/**
 * \file raw_process.c
 *
 * This file provides the implementation for the ra_raw_process and ra_raw_process_unit
 * API functions. 
 *
 * \author Raphael Schneider (rasch@med1.med.tum.de)
 */

/*--------------------------------------------------------------------------
 *
 * Copyright (C) 2005-2006, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *
 *--------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define _LIBRASCH_BUILD
#include <ra_priv.h>
#include <ra_version.h>
#include <ra.h>

/* function prototypes only used here */
double _ra_get_samplerate(rec_handle rh, long ch);
int filter_powerline_noise(double *data, size_t num, double samplerate, value_handle task_data);
int remove_mean(double *data, size_t num);



/**
 * \brief common processing tasks for raw-data (e.g. powerline-noise filter)
 * \param <task> id of the processing task (see RA_RAW_PROC_* defines)
 * \param <task_data> task specific data
 * \param <num_data> number of raw-data samples
 * \param <data> raw-data
 * \param <data_high> upper 32bits of raw-data (if we have 64bit samples; not supported yet)
 * \param <rh> recording handle the raw-data comes from
 * \param <ch> channel of the raw-data
 *
 * This function performs common processing tasks for the raw-data, the tasks
 * are independent of signal-type. For example, one task is to filer
 * powerline noise. For a complete set of available tasks, see the RA_RAW_PROC_*
 * defines in the ra_defines.h header file.
 * 
 */
LIBRAAPI int
ra_raw_process(long task, value_handle task_data, size_t num_data, DWORD *data, DWORD *data_high,
	       rec_handle rh, unsigned int ch)
{
	int ret = 0;
	long l;
	double *data_d = NULL;

	if (rh == NULL)
		return -1;

	if (data_high != NULL)
	{
		_ra_set_error(rh, RA_ERR_UNSUPPORTED, "libRASCH");
		return -1;
	}

	if (ch < 0)
	{
		_ra_set_error(rh, RA_ERR_OUT_OF_RANGE, "libRASCH");
		return -1;
	}
	if ((num_data <= 0) || (data == NULL))
	{
		_ra_set_error(rh, RA_ERR_INFO_MISSING, "libRASCH");
		return -1;
	}

	_ra_set_error(rh, RA_ERR_NONE, "libRASCH");

	data_d = malloc(sizeof(double) * num_data);
	for (l = 0; l < (long)num_data; l++)
		data_d[l] = data[l];

	switch (task)
	{
	case RA_RAW_PROC_RM_POWERLINE_NOISE:
		ret = filter_powerline_noise(data_d, num_data, _ra_get_samplerate(rh, ch), task_data);
		break;
	case RA_RAW_PROC_RM_MEAN:
		ret = remove_mean(data_d, num_data);
		break;
	default:
		_ra_set_error(rh, RA_ERR_OUT_OF_RANGE, "libRASCH");
		ret = -1;
		break;
	}

	if (ret == 0)
	{
		for (l = 0; l < (long)num_data; l++)
		{
			long v = (long)(data_d[l]); /* TODO: think about rounding */
			data[l] = v;
		} 
	}

	if (data_d)
		free(data_d);

	return ret;
} /* ra_raw_process() */


LIBRAAPI int
ra_raw_process_unit(long task, value_handle task_data, size_t num_data, double *data,
		    rec_handle rh, unsigned int ch)
{
	int ret = 0;

	if (rh == NULL)
		return -1;

	if (ch < 0)
	{
		_ra_set_error(rh, RA_ERR_OUT_OF_RANGE, "libRASCH");
		return -1;
	}
	if ((num_data <= 0) || (data == NULL))
	{
		_ra_set_error(rh, RA_ERR_INFO_MISSING, "libRASCH");
		return -1;
	}

	_ra_set_error(rh, RA_ERR_NONE, "libRASCH");

	switch (task)
	{
	case RA_RAW_PROC_RM_POWERLINE_NOISE:
		ret = filter_powerline_noise(data, num_data, _ra_get_samplerate(rh, ch), task_data);
		break;
	case RA_RAW_PROC_RM_MEAN:
		ret = remove_mean(data, num_data);
		break;
	default:
		_ra_set_error(rh, RA_ERR_OUT_OF_RANGE, "libRASCH");
		ret = -1;
		break;
	}

	return ret;
} /* ra_raw_process_unit() */


double
_ra_get_samplerate(rec_handle rh, long ch)
{
	value_handle vh;
	double samplerate = -1;

	vh = ra_value_malloc();
	ra_value_set_number(vh, ch);
	if (ra_info_get(rh, RA_INFO_REC_CH_SAMPLERATE_D, vh) == 0)
		samplerate = ra_value_get_double(vh);
	ra_value_free(vh);

	return samplerate;
} /* _ra_get_samplerate() */


int
filter_powerline_noise(double *data, size_t num, double samplerate, value_handle task_data)
{
	size_t l, m, n_corr, n_check, T;
	size_t start_idx, end_idx, end_idx_check, start_idx_temp;
	double pl_noise_freq, w0, w0_test;
	double re, im, re_t, im_t, amp, amp1, amp2;
	double *err_sig;
	double *temp, *lin_status;
	double max, mean;
	double phase, phase_test, v, threshold;
	int force_filter, in_linear_period;

	/* get options (TODO: implement 'more options processing' by using value-handles arrays as task-data) */
	force_filter = 0;
	if (task_data)
	{
		force_filter = ra_value_get_long(task_data);
	}

	pl_noise_freq = 50.0;
	w0 = pl_noise_freq / samplerate;

	/* get longest linear segment in the first 2 seconds of the data */
	T = (long)(samplerate / pl_noise_freq);
	n_check = (long)(samplerate * 2);
	temp = calloc(n_check, sizeof(double));
	if (n_check > num)
		n_check = num;
	for (l = 0; l < (n_check-T); l++)
		temp[l] = data[l+T] - data[l];
	lin_status = calloc(n_check, sizeof(double));
	max = 0.0;
	for (l = 0; l < (n_check-T-1); l++)
	{
		lin_status[l] = fabs(temp[l+1] - temp[l]);
		if (lin_status[l] > max)
			max = lin_status[l];
	}
	start_idx = end_idx = start_idx_temp = 0;
	threshold = max * 0.5;
	in_linear_period = 0;
	for (l = 0; l < (n_check-T-1); l++)
	{
		if (in_linear_period && (lin_status[l] > threshold))
		{
			if ((end_idx == 0) ||
			    ((end_idx - start_idx) < (l - start_idx_temp)))
			{
				start_idx = start_idx_temp;
				end_idx = l;
			}
			in_linear_period = 0;
		}
		else if (!in_linear_period && (lin_status[l] <= threshold))
		{
			start_idx_temp = l;
			in_linear_period = 1;
		}
	}
	if (in_linear_period)
	{
		if ((end_idx == 0) ||
		    ((end_idx - start_idx) < (l - start_idx_temp)))
		{
			start_idx = start_idx_temp;
			end_idx = l;
		}
	}	

	/* get mean */
	mean = 0.0;
	for (l = start_idx; l < end_idx; l++)
		mean += data[l];
	mean /= (double)(end_idx - start_idx);

	/* get amplitude and phase of PL noise */
	max = 0;
	phase = 0;
	amp = 0;
	for (phase_test = 0; phase_test < T; phase_test += 0.3)
	{
		v = 0;
		re = im = 0;
		for (l = start_idx; l < end_idx; l++)
		{
			re_t = cos(2*RA_PI*w0*((double)(l-start_idx)+phase_test));
			im_t = sin(2*RA_PI*w0*((double)(l-start_idx)+phase_test));
			re += ((data[l]-mean) * re_t);
			im += ((data[l]-mean) * im_t);
		}
		v = re;
		if (v > max)
		{
			max = v;
			phase = phase_test;

			amp = sqrt((re*re) + (im*im));
			amp /= (double)(end_idx - start_idx + 1);
			amp *= 2.0;
		}
	}

	/* check if there is really powerline noise */
	end_idx_check = end_idx;
/* 	if (end_idx_check > end_idx) */
/* 		end_idx_check = end_idx; */
	w0_test = (pl_noise_freq-5.0) / samplerate;
	re = im = 0;
	for (l = start_idx; l < end_idx_check; l++)
	{
		re_t = cos(2*RA_PI*w0_test*((double)(l-start_idx)));
		im_t = sin(2*RA_PI*w0_test*((double)(l-start_idx)));
		re += ((data[l]-mean) * re_t);
		im += ((data[l]-mean) * im_t);
	}
	amp1 = sqrt((re*re) + (im*im));
	amp1 /= (double)(end_idx_check - start_idx + 1);
	amp1 *= 2.0;
	w0_test = (pl_noise_freq+5.0) / samplerate;
	re = im = 0;
	for (l = start_idx; l < end_idx_check; l++)
	{
		re_t = cos(2*RA_PI*w0_test*((double)(l-start_idx)));
		im_t = sin(2*RA_PI*w0_test*((double)(l-start_idx)));
		re += ((data[l]-mean) * re_t);
		im += ((data[l]-mean) * im_t);
	}
	amp2 = sqrt((re*re) + (im*im));
	amp2 /= (double)(end_idx_check - start_idx + 1);
	amp2 *= 2.0;
/*   	fprintf(stderr, "45:%f  50:%f  65:%f", amp1, amp, amp2); */
	if ((((amp1 + amp2)/2)/amp) > 0.66)
	{
/*   		fprintf(stderr, " -> skip\n"); */
		free(temp);
		free(lin_status);
		return 0;
	}
/*   	fprintf(stderr, " -> OK\n"); */

	/* prepare error-signal */
	n_corr = T;
	err_sig = malloc(sizeof(double) * n_corr);
	for (l = 0; l < n_corr; l++)
		err_sig[l] = amp * cos(2*RA_PI*w0*((double)l+phase));
	
	/* filter data */
	m = T - (start_idx % T);
	for (l = 0; l < num; l++)
	{
		if (m >= T)
			m = 0;
		data[l] -= err_sig[m++];
	}

	free(err_sig);
	free(temp);
	free(lin_status);

	return 0;
} /* filter_powerline_noise() */


int
remove_mean(double *data, size_t num)
{
	size_t l;
	double m = 0.0;

	for (l = 0; l < num; l++)
		m += data[l];
	m /= (double)num;

	for (l = 0; l < num; l++)
		data[l] -= m;

	return 0;
} /* remove_mean() */
