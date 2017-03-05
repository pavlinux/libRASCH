/*----------------------------------------------------------------------------
 * main_detect.c
 *
 * Perform a beat detection in an ECG. The implementation in this file assumes
 * that in the raw data only the heartbeats of one subject are recorded. ECGs
 * with heartbeats of multiple subjects (e.g. maternal and fetal heartbeats)
 * are not supported.
 *
 * Author(s): Raphael Schneider (librasch@gmail.com)
 *
 * Copyright (C) 2005-2009, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

/*
  TODO functionality:
  - check t-wave amplitude over all channels and remove t-wave values
    when the t-wave is less than e.g. 10% of maximum t-wave amplitude
    for this beat
  - check if beats are in the same template but have different morphology types
  - classification: check for change in main vector
  - create a seperate plugin for the calculations of ch-independent values
  - split templates
  - detect noise and skip these regions (for each channel separately)
*/


#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <memory.h>

#define _LIBRASCH_BUILD
#include <ra.h>
#include <ra_detect.h>
#include <ra_ecg.h>

#include "signal_processing.h"
#include "main_detect.h"
#include "statistics.h"

#include "get_peaks.h"

/* debug beat detection (writes some data to file) */
#ifdef WIN32
	/* Using fprintf() and printf() from libintl will crash on Win32.
	   The reason is maybe using different c-libs. */
	#undef fprintf
	#undef printf
#endif  /* WIN32 */
#define DETECT_DEBUG  0
int debug = 0;


int
main_detect(struct proc_info *pi, struct detect_result *dr)
{
	/* init */
	int i;
	char s[100];
	struct ra_detect *opt;
	struct beats2 *b;

	opt = (struct ra_detect *)pi->options;

	if ((opt->num_ch <= 0) || (opt->ch == NULL))
	{
		_ra_set_error(pi, RA_ERR_INFO_MISSING, "plugin-detect-simple");
		return -1;
	}

	dr->num_ch = opt->num_ch;
	dr->ch_in_sig = calloc(opt->num_ch, sizeof(int));
	dr->samplerate = calloc(opt->num_ch, sizeof(double));
	dr->calib_offset = calloc(opt->num_ch, sizeof(long));

	/* use every choosen channel for detection */
	for (i = 0; i < opt->num_ch; i++)
	{
		dr->ch_in_sig[i] = opt->ch[i];

		/* look for a calibration signal at the beginning of the channel
		   (but only if we doing a complete channel scan) */
		if ((opt->use_region == 0) && (opt->num_events == 0) && (opt->check_for_calibration))
		{
			if (pi->callback)
			{
				sprintf(s, gettext("check for calibration signal in channel %ld"), opt->ch[i] + 1);
				(*pi->callback) (s, 0);
			}
			check_calib(pi, opt->ch[i], &(dr->calib_offset[i]));
			if (pi->callback)
				(*pi->callback) (NULL, 100);
		}

		/* do beat detect */
		if (opt->num_events == 0)
		{
			if (pi->callback)
			{
				sprintf(s, gettext("detect beats in channel %ld"), opt->ch[i] + 1);
				(*pi->callback) (s, 0);
			}
			ch_detect(pi, opt->ch[i], i, &(dr->num_beats), &(dr->b),
				  &(dr->samplerate[i]), &(dr->interp_factor[i]), dr->calib_offset[i]);
			if (pi->callback)
				(*pi->callback) (NULL, 100);
		}
		else
		{
			if (pi->callback)
			{
				sprintf(s, gettext("re-evaluate some/all beats in channel %ld"), opt->ch[i] + 1);
				(*pi->callback) (s, 0);
			}
			ch_recalc(pi, opt->ch[i], i, &(dr->b), &(dr->samplerate[i]), &(dr->interp_factor[i]));
			if (pi->callback)
				(*pi->callback) (NULL, 100);
		}

		if (pi->callback)
		{
			sprintf(s, gettext("post-process channel %ld"), opt->ch[i] + 1);
			(*pi->callback) (s, 0);
		}
/*  		ch_post_process(pi, i, dr->b, dr->samplerate[i]); */
		if (pi->callback)
			(*pi->callback) (NULL, 100);
	}

	if (dr->num_beats <= 0)
		return -1;

	/* correct positions for interpolation factors */
	for (i = 0; i < opt->num_ch; i++)
	{
		long pos;

		if (dr->interp_factor[i] == 1)
			continue;
		
		b = get_first_beat(&(dr->b));
		while (b)
		{
			if (i == 0)
			{
				pos = GET_BEAT_POS(b);
				pos /= dr->interp_factor[i];
				SET_BEAT_POS(b, pos);
			}

			pos = GET_P_START(b, i);
			if (pos != -1)
			{
				pos /= dr->interp_factor[i];
				SET_P_START(b, i, pos);

				pos = GET_P_PEAK_1(b, i);
				pos /= dr->interp_factor[i];
				SET_P_PEAK_1(b, i, pos);

				pos = GET_P_PEAK_2(b, i);
				pos /= dr->interp_factor[i];
				SET_P_PEAK_2(b, i, pos);

				pos = GET_P_END(b, i);
				pos /= dr->interp_factor[i];
				SET_P_END(b, i, pos);
			}
			
			pos = GET_QRS_START(b, i);
			if (pos != -1)
			{
				pos /= dr->interp_factor[i];
				SET_QRS_START(b, i, pos);
			}

			pos = GET_Q(b, i);
			if (pos != -1)
			{
				pos /= dr->interp_factor[i];
				SET_Q(b, i, pos);
			}

			pos = GET_R(b, i);
			if (pos != -1)
			{
				pos /= dr->interp_factor[i];
				SET_R(b, i, pos);
			}

			pos = GET_S(b, i);
			if (pos != -1)
			{
				pos /= dr->interp_factor[i];
				SET_S(b, i, pos);
			}

			pos = GET_RS(b, i);
			if (pos != -1)
			{
				pos /= dr->interp_factor[i];
				SET_RS(b, i, pos);
			}

			pos = GET_QRS_END(b, i);
			if (pos != -1)
			{
				pos /= dr->interp_factor[i];
				SET_QRS_END(b, i, pos);
			}

			pos = GET_T_START(b, i);
			if (pos != -1)
			{
				pos /= dr->interp_factor[i];
				SET_T_START(b, i, pos);

				pos = GET_T_PEAK_1(b, i);
				pos /= dr->interp_factor[i];
				SET_T_PEAK_1(b, i, pos);

				pos = GET_T_PEAK_2(b, i);
				pos /= dr->interp_factor[i];
				SET_T_PEAK_2(b, i, pos);

				pos = GET_T_END(b, i);
				pos /= dr->interp_factor[i];
				SET_T_END(b, i, pos);
			}
			
			b = b->next;
		}

		dr->samplerate[i] /= dr->interp_factor[i];
		dr->interp_factor[i] = 1;
	}

	/* perform post-processing */
	if (pi->callback)
	{
		sprintf(s, gettext("post-process detection results"));
		(*pi->callback) (s, 0);
	}
/*  	post_process(pi, dr); */
	if (pi->callback)
		(*pi->callback) (NULL, 100);

	/* if doing no re-calc, do some post processing */
	if (opt->num_events <= 0)
	{
		/* calc ch independent values */
		if (pi->callback)
		{
			sprintf(s, gettext("detection post processing"));
			(*pi->callback) (s, 0);
		}
		calc_ch_indep_values(dr);
		if (pi->callback)
			(*pi->callback) (NULL, 100);

		/* try some classifications */
		if (pi->callback)
		{
			sprintf(s, gettext("classify beats"));
			(*pi->callback) (s, 0);
		}
		classify_beats(dr);
		if (pi->callback)
			(*pi->callback) (NULL, 100);

	}

	return 0;
} /* main_detect() */


int
check_calib(struct proc_info *pi, int ch, long *calib_offset)
{
	plugin_handle pl;
	struct proc_info *proc;
	value_handle vh;
	double samplerate;
	
	*calib_offset = 0;

	pl = ra_plugin_get_by_name(ra_lib_handle_from_any_handle(pi->rh), "calibration", 0);
	if (pl == NULL)
		return -1;

	vh = ra_value_malloc();
	if (ra_info_get(pi->rh, RA_INFO_REC_CH_SAMPLERATE_D, vh) == 0)
		samplerate = ra_value_get_double(vh);
	else
	{
		ra_value_free(vh);
		return -1;
	}

	proc = ra_proc_get(ra_meas_handle_from_any_handle(pi->rh), pl, NULL);
	ra_value_set_long(vh, ch);
	ra_lib_set_option(proc, "ch", vh);
	ra_value_set_double(vh, 30*60*samplerate);
	ra_lib_set_option(proc, "end_pos", vh);
	ra_value_set_double(vh, 15);
   	ra_lib_set_option(proc, "segment_length", vh);
	ra_value_set_long(vh, 0);
   	ra_lib_set_option(proc, "get_cycle_length", vh);

	if (ra_proc_do(proc) == 0)
	{		
		ra_proc_get_result(proc, 8, 0, vh);
		if (ra_value_is_ok(vh))
		{
			long n = ra_value_get_num_elem(vh);
			*calib_offset = ra_value_get_long_array(vh)[n-1];
		}
	}

	ra_proc_free(proc);
	ra_value_free(vh);

	return 0;
} /* check_calib() */


int
ch_detect(struct proc_info *pi, int ch, int ch_idx, long *n, struct beats2_mem *bmem,
	  double *samplerate, long *interp_factor, long pos_offset)
{
	int ret = 0;
	struct ch_info ci;
	struct data_buffer buf;
	double th_s0, th_s1, th_s2;
	
	int num_read = 0;
	long ch_pos = 0;
	long curr_pos, end_pos, l, blanktime, get_next_th;

	int state_prev, state, last_peak;
	long last_peak_idx;

	struct beats2 *curr_beat, *last_beat, *bhead;
	int percent_save = 0;
	int percent;
	long beat_range, next_beat;
	struct ra_detect *opt;

	double hr[HR_BUF_SIZE];
	int hr_buf_pos = 0;
	double curr_hr;
	long max_t_wave_end;

	opt = (struct ra_detect *)pi->options;
		
	memset(&ci, 0, sizeof(struct ch_info));
	ret = get_ch_infos(pi->rh, ch, opt->do_interpolation, &ci);
	if (ret != 0)
		return ret;

	*samplerate = ci.samplerate;
	*interp_factor = ci.interp_factor;
	
	memset(&buf, 0, sizeof(struct data_buffer));
	ret = prepare_data_access(&ci, &buf);
	if (ret != 0)
		return ret;

	beat_range = (long)(opt->min_beat_distance * ci.samplerate);
	
	/* init heartrate buffer with values of 80bpm */
	for (l = 0; l < HR_BUF_SIZE; l++)
		hr[l] = 80;
	
	curr_pos = -1;
	blanktime = 0;
	state = state_prev = last_peak = 0;
	last_peak_idx = 0;
	get_next_th = 0;
	
	if (opt->use_region)
	{
		curr_pos = opt->region_start * ci.interp_factor;
		end_pos = opt->region_end * ci.interp_factor;

		if (curr_pos == end_pos)
		{
			curr_pos -= (2*ci.ms100);
			end_pos += (2*ci.ms100);
		}
	}
	else
	{
		curr_pos = pos_offset * ci.interp_factor;
		end_pos = ci.num_samples * ci.interp_factor;
	}
	if (curr_pos < 0)
		curr_pos = 0;

	ch_pos = curr_pos;
	curr_beat = last_beat = bhead = get_first_beat(bmem);

	th_s0 = th_s1 = 0;
	while (curr_pos < end_pos)
	{
		if (blanktime > 0)
			blanktime--;
		
		curr_pos++;

		/* check if new data is needed in the read-buffer */
		if (curr_pos > ch_pos)
		{
			long num_to_read;
			long read_start = curr_pos;
			
			read_start -= (long)(ci.samplerate);
			if (read_start < 0)
				read_start = 0;
		
			num_to_read = BUFFER_SIZE;
			if ((end_pos - read_start) < num_to_read)
			{
				num_to_read = end_pos - read_start;
				/* if we processing a region read 1sec more data to have enough samples
				   getting the morphology data */
				if (opt->use_region)
					num_to_read += (long)(ci.samplerate);
			}
			if (num_to_read < ci.samplerate)
				break;

			num_read = get_data(pi->rh, ch, read_start, &buf, num_to_read,
					    ci.interp_factor, opt->filter_powerline_noise,
					    opt->filter_baseline_wander);
			if (num_read <= ci.samplerate)
				break;
			buf.data_offset = read_start;
			ch_pos = read_start + num_read - (long)(ci.samplerate);

			/* check if callback should be called */
			percent = (int) (((double)ch_pos / (double)ci.num_samples) * 100.0);
			if ((percent != percent_save) && pi->callback)
			{
				(*pi->callback) (NULL, percent);
				percent_save = percent;
			}

			get_next_th = 0;
		}
		
		/* calculate thresholds every 5 seconds; artifacts in the signal
		   have not so much influence as when the thresholds are calculated
		   every 100.000 (= BUFFER_SIZE) samples */
		get_next_th--;
  		if (get_next_th <= 0)
		{
			long start, num;

			/* special handling if only one beat is analyzed */
			if (opt->use_region && (opt->region_start == opt->region_end))
			{
				start = curr_pos - buf.data_offset;
				if (start < 0)
					start = 0;
				num = 4 * ci.ms100;
			}
			else
			{
				start = curr_pos - buf.data_offset;
				if (start < 0)
					start = 0;
				num = ci.s5;
				if ((start+num) > buf.num_data)
					num = buf.num_data - start;
			}

			th_s0 = th_s1 = th_s2 = 0.0;
			if (opt->use_region && (opt->region_start == opt->region_end))
			{
				double max_s0, min_s0;
				double max_s1, min_s1;
				double max_s2, min_s2;
				/* when we have to look for a beat at a given cursor position,
				   get the threshold values directly from the region around it */
				max_s0 = min_s0 = buf.s0[start+ci.ms100];
				max_s1 = min_s1 = buf.s1[start+ci.ms100];
				max_s2 = min_s2 = buf.s2[start+ci.ms100];
				for (l = (start+ci.ms100); l < (start+num-ci.ms100); l++)
				{
					if (max_s0 < buf.s0[l])
						max_s0 = buf.s0[l];
					if (min_s0 > buf.s0[l])
						min_s0 = buf.s0[l];

					if (max_s1 < buf.s1[l])
						max_s1 = buf.s1[l];
					if (min_s1 > buf.s1[l])
						min_s1 = buf.s1[l];

					if (max_s2 < buf.s2[l])
						max_s2 = buf.s2[l];
					if (min_s2 > buf.s2[l])
						min_s2 = buf.s2[l];
				}
				th_s0 = ((fabs(max_s0) > fabs(min_s0)) ? fabs(min_s0) : fabs(max_s0));
				th_s1 = ((fabs(max_s1) > fabs(min_s1)) ? fabs(min_s1) : fabs(max_s1));
				th_s2 = ((fabs(max_s2) > fabs(min_s2)) ? fabs(min_s2) : fabs(max_s2));
				th_s0 *= 0.33;
				th_s1 *= 0.33;
				th_s2 *= 0.33;
			}
			else
			{
				for (l = start; l < (start+num); l++)
				{
					th_s0 += (buf.s0[l] * buf.s0[l]);
					th_s1 += (buf.s1[l] * buf.s1[l]);
				}
				th_s0 = 2*sqrt(th_s0 / (double)num);
				th_s1 = 2*sqrt(th_s1 / (double)num);

			}
/* 			if (th_s1 < (ci.amp_res * MIN_AMP_STEPS)) */
/* 				th_s1 = ci.amp_res * MIN_AMP_STEPS; */

			get_next_th = ci.s5;
		}


		/* check if the signal is above/below the threshold and that we
		   have a state change */
		state_prev = state;
		if (buf.s1[curr_pos - buf.data_offset] > th_s1)
			state = 1;
		else if (buf.s1[curr_pos - buf.data_offset] < -th_s1)
			state = -1;
		else
			state = 0;
		/* if we are coming from a peak, store end and orientation of the peak */
		if ((state == 0) && (state_prev != 0))
		{
			last_peak_idx = curr_pos-1;
			last_peak = state_prev;
		}

		if ((state_prev == state) || (state == 0))
			continue;

		/* if the previous peak and the current peak are on the same side
		   we do not have a qrs complex */
		if (last_peak == state)
			continue;

		/* only contiguous (max allowed distance of 30 msec) pos/neg (neg/pos) peaks are allowed */
		if ((curr_pos - last_peak_idx) > ci.ms30)
		{
			last_peak = state;
			last_peak_idx = curr_pos;
			continue;
		}

		if (blanktime > 0)
			continue;

		/* ---------- a beat was found ---------- */
		next_beat = look_for_next_beat(buf.s1, num_read, curr_pos-buf.data_offset, th_s1, (3*ci.ms100), ci.ms30);

		if (curr_beat == NULL)
		{
			curr_beat = beat_malloc(bmem, ch_idx, curr_pos);
			bhead = curr_beat;
			(*n)++;
		}
		else
		{
			struct beats2 *prev_beat;

			curr_beat = find_beat(last_beat, curr_pos, beat_range, &prev_beat);
			if (curr_beat == NULL)
			{
				curr_beat = beat_malloc(bmem, ch_idx, curr_pos);
				(*n)++;
				if (prev_beat == NULL)
					prev_beat = bhead; /* if in this branch, list-head is available */

				if (prev_beat->next)
				{
					curr_beat->next = prev_beat->next;
					curr_beat->next->prev = curr_beat;
				}
				curr_beat->prev = prev_beat;
				prev_beat->next = curr_beat;
			}
			else
				SET_BEAT_IN_CH(curr_beat, ch_idx, 1);
		}

		curr_hr = 0.0;
		if (((*n) >= 2) && curr_beat->prev)
		{
			long pos, prev_pos;
			double rri;
			
			pos = GET_BEAT_POS(curr_beat);
			prev_pos = GET_BEAT_POS(curr_beat->prev);

			rri = (double)(pos - prev_pos) * 1000 / (*samplerate);
			if (rri > 300)
			{
				hr[hr_buf_pos] = 60000.0 / rri;
				hr_buf_pos = (hr_buf_pos + 1) % HR_BUF_SIZE;
				curr_hr = 0.0;
				for (l = 0; l < HR_BUF_SIZE; l++)
					curr_hr += hr[l];
				curr_hr /= (double)HR_BUF_SIZE;
			}
		}
		if (next_beat > 0)
			max_t_wave_end = next_beat;
		else
			max_t_wave_end = -1;

		if ((ret = get_morph_values(&buf, curr_beat, ch_idx, th_s0, th_s2, curr_hr, opt, &ci, max_t_wave_end)) != 0)
		{
			SET_BEAT_IN_CH(curr_beat, ch_idx, 0);
			continue;
		}

		calc_beat_values(curr_beat, ch_idx, ci.samplerate);

 		last_beat = curr_beat;
 		blanktime = 2 * ci.ms100;
	};
	
	free(buf.orig);
	free(buf.orig_deriv);
	free(buf.f0);
	free(buf.s0);
	free(buf.f1);
	free(buf.s1);
	free(buf.f2);
	free(buf.s2);
	free(buf.filter_baseline);
	free(buf.filter1);
	free(buf.filter2);
	free(buf.filter3);

	return ret;
} /* ch_detect() */


int
ch_recalc(struct proc_info *pi, int ch, int ch_idx, struct beats2_mem *bmem, double *samplerate, long *interp_factor)
{
	int ret = 0;
	struct ch_info ci;
	struct data_buffer buf;
	long l, num_to_read, read_start, num_read;
	long th_start, th_num;
	double th_s0, th_s1, th_s2;	
	struct beats2 *curr_beat;
	struct ra_detect *opt;
	double curr_hr;
	long cnt;
	int percent, percent_save;
	int qrs_changed;
	long pos, type, next_beat;
	long max_t_wave_end;

	opt = (struct ra_detect *)pi->options;
	
	memset(&ci, 0, sizeof(struct ch_info));
	ret = get_ch_infos(pi->rh, ch, opt->do_interpolation, &ci);
	if (ret != 0)
		return ret;
	*samplerate = ci.samplerate;
	*interp_factor = ci.interp_factor;
	
	memset(&buf, 0, sizeof(struct data_buffer));
	ret = prepare_data_access(&ci, &buf);
	if (ret != 0)
		return ret;

	cnt = 0;
	percent = 0;
	percent_save = -1;

	curr_hr = 80;
	num_to_read = 2 * (long)(ci.samplerate);
	curr_beat = get_first_beat(bmem);
	while (curr_beat)
	{
		cnt++;

		/* check if callback should be called */
		percent = (int) (((double)cnt / (double)opt->num_events) * 100.0);
		if ((percent != percent_save) && pi->callback)
		{
			(*pi->callback) (NULL, percent);
			percent_save = percent;
		}

		/* get data */
		pos = GET_BEAT_POS(curr_beat);
		read_start = pos - (long)(ci.samplerate);
		num_read = get_data(pi->rh, ch, read_start, &buf, num_to_read,
					    ci.interp_factor, opt->filter_powerline_noise,
					    opt->filter_baseline_wander);
		buf.data_offset = read_start;

		/* calc thresholds using all data (is slower but results are way better;
		   if using only a specific region, e.g. around the QRS complex, results
		   in higher thresholds which results in bad detection of P- and T-waves) */
		th_start = 0;
		th_num = num_read;
		th_s0 = th_s1 = th_s2 = 0.0;
		for (l = th_start; l < (th_start+th_num); l++)
		{
			th_s0 += (buf.s0[l] * buf.s0[l]);
			th_s1 += (buf.s1[l] * buf.s1[l]);
			th_s2 += (buf.s2[l] * buf.s2[l]);
		}
		th_s0 = 2*sqrt(th_s0 / (double)th_num);
		th_s1 = 2*sqrt(th_s1 / (double)th_num);
		th_s2 = 2*sqrt(th_s2 / (double)th_num);

		curr_hr = GET_HR(curr_beat);

		next_beat = look_for_next_beat(buf.s1, num_read, (long)(ci.samplerate), th_s1, (3*ci.ms100), ci.ms30);
		max_t_wave_end = -1;
		if (next_beat > 0)
		{
			max_t_wave_end = next_beat;
			if (debug) fprintf(stderr, "found next beat -> max-t-wave-end=%ld\n", max_t_wave_end);
		}

		/* only re-process wave where we want changes and leave the others un-touched */
		qrs_changed = 0;
		type = GET_QRS_TYPE(curr_beat, ch_idx);
		if ((opt->force_qrs_type != 0) && (opt->force_qrs_type != type))
		{
			long pos_save;
			pos_save = GET_BEAT_POS(curr_beat);
			if ((ret = get_qrs_complex(&buf, curr_beat, ch_idx, ci.ms100, ci.ms50, ci.ms30,
						   opt->force_qrs_type)) != 0)
			{
				SET_BEAT_IN_CH(curr_beat, ch_idx, 0);
				curr_beat = curr_beat->next;
				continue;
			}
			/* restore fiducial point of the beat (the correct one is set
			   by the fiducial-point plugin and not here in the qrs-detect code) */
			SET_BEAT_POS(curr_beat, pos_save);
			qrs_changed = 1;
		}
		type = GET_P_TYPE(curr_beat, ch_idx);
		if (((opt->force_p_type != 0) && (opt->force_p_type != type)) || qrs_changed)
		{
			/* if we here because the QRS complex morphology was changed,
			   force type of P-wave to the saved one */
			long force_type = opt->force_p_type;
			if (qrs_changed && (force_type == 0))
				force_type = type;

			if ((ret = get_p_wave(&buf, curr_beat, ch_idx, MIN_AMP_P_WAVE_SCALE * th_s2, ci.ms100, ci.ms30,
					      force_type)) != 0)
			{
				SET_BEAT_IN_CH(curr_beat, ch_idx, 0);
				curr_beat = curr_beat->next;
				continue;
			}
		}
		type = GET_T_TYPE(curr_beat, ch_idx);
		if (((opt->force_t_type != 0) && (opt->force_t_type != type)) || qrs_changed)
		{
			/* if we here because the QRS complex morphology was changed,
			   force type of T-wave to the saved one */
			long force_type = opt->force_t_type;
			if (qrs_changed && (force_type == 0))
				force_type = type;

			if ((ret = get_t_wave(&buf, curr_beat, ch_idx, MIN_AMP_T_WAVE_SCALE * th_s2, ci.ms100, ci.ms30,
					      curr_hr, force_type, opt->t_wave_algorithm, max_t_wave_end)) != 0)
			{
				SET_BEAT_IN_CH(curr_beat, ch_idx, 0);
				curr_beat = curr_beat->next;
				continue;
			}
		}

		calc_beat_values(curr_beat, ch_idx, ci.samplerate);

		curr_beat = curr_beat->next;
	}
	
	free(buf.orig);
	free(buf.orig_deriv);
	free(buf.f0);
	free(buf.s0);
	free(buf.f1);
	free(buf.s1);
	free(buf.f2);
	free(buf.s2);
	free(buf.filter1);
	free(buf.filter2);
	free(buf.filter3);

	return ret;
} /* ch_recalc() */


int
get_ch_infos(rec_handle rh, long ch, int do_interpolation, struct ch_info *ci)
{
	value_handle vh;
	long temp;

	/* get some infos about channel */
	vh = ra_value_malloc();
	ra_value_set_number(vh, ch);
	if (ra_info_get(rh, RA_INFO_REC_CH_NUM_SAMPLES_L, vh) == 0)
		ci->num_samples = ra_value_get_long(vh);
	if (ra_info_get(rh, RA_INFO_REC_CH_SAMPLERATE_D, vh) == 0)
		ci->samplerate = ra_value_get_double(vh);
	if (ra_info_get(rh, RA_INFO_REC_CH_AMP_RESOLUTION_D, vh) == 0)
		ci->amp_res = ra_value_get_double(vh);
	ra_value_free(vh);

	ci->interp_factor = 1;
	if (do_interpolation && (ci->samplerate < 500))
	{
		temp = (long)(ci->samplerate) / 10;
		if (((ci->samplerate/10.0)-(double)temp) != 0)
			ci->interp_factor = 512 / (long)(ci->samplerate);
		else
			ci->interp_factor = 500 / (long)(ci->samplerate);
/*     		ci->interp_factor=1; */
		ci->samplerate *= (double)(ci->interp_factor);
	}
	ci->num_samples *= ci->interp_factor;


	/* calc samplerate dependent variables */
	temp = (long)(ci->samplerate * 0.1);
	ci->ms100 = temp;
	if ((temp - (double)ci->ms100) >= 0.5)
		ci->ms100++;

	temp = (long)(ci->samplerate * 0.05);
	ci->ms50 = temp;
	if ((temp - (double)ci->ms50) >= 0.5)
		ci->ms50++;

	temp = (long)(ci->samplerate * 0.03);
	ci->ms30 = temp;
	if ((temp - (double)ci->ms30) >= 0.5)
		ci->ms30++;

	temp = (long)(ci->samplerate * 0.01);
	ci->ms10 = temp;
	if ((temp - (double)ci->ms10) >= 0.5)
		ci->ms10++;

	ci->s5 = (long)(5.0 * ci->samplerate);

	return 0;
} /* get_ch_infos() */


int
prepare_data_access(struct ch_info *ci, struct data_buffer *buf)
{
	/* calculate filter coefficients */
	/* filter for signal which will be used for wave boundaries of QRS */
 	buf->filter_len1 = 41;
	buf->filter1 = calloc(buf->filter_len1+1, sizeof(double));
	fir_filter_design(buf->filter_len1, (0.05 / ci->samplerate), (40.0/ ci->samplerate),
			  &(buf->filter1), &(buf->filter_len1));
	/* filter for signal which will be used for wave boundaries of P- and T-wave */
 	buf->filter_len3 = 41;
	buf->filter3 = calloc(buf->filter_len3+1, sizeof(double));
	fir_filter_design(buf->filter_len3, (0.05 / ci->samplerate), (30.0/ ci->samplerate),
			  &(buf->filter3), &(buf->filter_len3));
	/* filter for signal which will be used for the initial beat detection */
/*  	buf->filter_len2 = 21; */
 	buf->filter_len2 = 41;
	buf->filter2 = calloc(buf->filter_len2+1, sizeof(double));
 	fir_filter_design(buf->filter_len2, (15.0 / ci->samplerate), (60.0/ ci->samplerate),
 			  &(buf->filter2), &(buf->filter_len2));
/* 	fir_filter_design(buf->filter_len2, (42.5 / ci->samplerate), (105.0/ ci->samplerate), */
/* 			  &(buf->filter2), &(buf->filter_len2)); */

 	buf->filter_baseline_len = 41;
	buf->filter_baseline = calloc(buf->filter_baseline_len+1, sizeof(double));
	fir_filter_design(buf->filter_baseline_len, 0, (0.05 / ci->samplerate),
			  &(buf->filter_baseline), &(buf->filter_baseline_len));

	buf->orig = malloc(sizeof(double) * BUFFER_SIZE * ci->interp_factor);
	buf->orig_deriv = malloc(sizeof(double) * BUFFER_SIZE * ci->interp_factor);
	buf->f0 = malloc(sizeof(double) * BUFFER_SIZE * ci->interp_factor);
	buf->s0 = malloc(sizeof(double) * BUFFER_SIZE * ci->interp_factor);
	buf->f1 = malloc(sizeof(double) * BUFFER_SIZE * ci->interp_factor);
	buf->s1 = malloc(sizeof(double) * BUFFER_SIZE * ci->interp_factor);
	buf->f2 = malloc(sizeof(double) * BUFFER_SIZE * ci->interp_factor);
	buf->s2 = malloc(sizeof(double) * BUFFER_SIZE * ci->interp_factor);

	return 0;
} /* prepare_data_access() */


long
get_data(rec_handle rh, long ch, long ch_pos, struct data_buffer *buf, long num_to_read,
	 long interp_factor, int filter_pl_noise, int filter_baseline)
{
	long l, num_read;
	double mean, *baseline = NULL;
 	/*static int first = 1;*/

	num_read = (long)ra_raw_get_unit(rh, ch, ch_pos/interp_factor, num_to_read, buf->orig);
	buf->num_data = num_read;
	if (num_read <= 0)
		return 0;

	mean = 0.0;
	for (l = 0; l < num_read; l++)
		mean += buf->orig[l];
	mean /= (double)num_read;
	for (l = 0; l < num_read; l++)
		buf->orig[l] -= mean;

	/* remove powerline noise before interpolation */
	if (filter_pl_noise)
		ra_raw_process_unit(RA_RAW_PROC_RM_POWERLINE_NOISE, NULL, num_read, buf->orig, rh, ch);

	if (interp_factor != 1)
	{
		double *buf_temp;

		buf_temp = malloc(sizeof(double) * num_read);
		memcpy(buf_temp, buf->orig, sizeof(double) * num_read);
		interp(buf_temp, buf->orig, num_read, interp_factor);

		num_read *= interp_factor;
		buf->num_data *= interp_factor;
		free(buf_temp);
	}

	if (filter_baseline)
	{
		baseline = malloc(sizeof(double) * num_read);
		memcpy(baseline, buf->orig, sizeof(double)*num_read);
		filtfilt(buf->filter_baseline, NULL, buf->filter_baseline_len, baseline, num_read);
		for (l = 0; l < num_read; l++)
			buf->orig[l] -= baseline[l];
	}

	/* filter data */
	memcpy(buf->f0, buf->orig, sizeof(double)*num_read);
	memcpy(buf->f1, buf->orig, sizeof(double)*num_read);
	memcpy(buf->f2, buf->orig, sizeof(double)*num_read);

	filtfilt(buf->filter1, NULL, buf->filter_len1, buf->f0, num_read);
	filtfilt(buf->filter2, NULL, buf->filter_len2, buf->f1, num_read);
	filtfilt(buf->filter3, NULL, buf->filter_len3, buf->f2, num_read);
	
	/* and calc 1st derivative of the filterd data */
	for (l = 1; l < num_read; l++)
	{		
		buf->orig_deriv[l] = buf->orig[l] - buf->orig[l-1];
		buf->s0[l] = buf->f0[l] - buf->f0[l-1];
		buf->s1[l] = buf->f1[l] - buf->f1[l-1];
		buf->s2[l] = buf->f2[l] - buf->f2[l-1];
	}
	buf->orig_deriv[0] = buf->orig_deriv[1];
	buf->s0[0] = buf->s0[1];
	buf->s1[0] = buf->s1[1];
	buf->s2[0] = buf->s2[1];

	if (baseline)
		free(baseline);

/* code to export some raw data to make figures explaining the detection method
   (keep it here because it is maybe usefull later)
	if (first)
	{
		FILE *fp;

		fp = fopen("c:\\temp\\data.txt", "w");
		if (fp)
		{
			fprintf(fp, "orig\tderiv\tderiv.f1\tderiv.f2\n");
			for (l = 0; l < num_read; l++)
				fprintf(fp, "%f\t%f\t%f\t%f\n", buf->orig[l], buf->orig_deriv[l],
					buf->s0[l], buf->s1[l]);
			fclose(fp);
			first = 0;
		}
	}
 */
	return num_read;
} /* get_data() */


long
look_for_next_beat(double *data, long num_data, long pos, double threshold, long blanktime, long ms30)
{
	long next_beat = -1;
	long curr_pos = pos;
	long curr_blank = blanktime;
	long state_prev, state, last_peak_idx, last_peak;

	state = 0;
	last_peak_idx = pos;
	last_peak = 0;
	while (curr_pos < num_data)
	{
		curr_blank--;
		curr_pos++;

		/* check if the signal is above/below the threshold and that we
		   have a state change */
		state_prev = state;
		if (data[curr_pos] > threshold)
			state = 1;
		else if (data[curr_pos] < -threshold)
			state = -1;
		else
			state = 0;
		/* if we are coming from a peak, store end and orientation of the peak */
		if ((state == 0) && (state_prev != 0))
		{
			last_peak_idx = curr_pos-1;
			last_peak = state_prev;
		}
		
		if ((state_prev == state) || (state == 0))
			continue;
		
		/* if the previous peak and the current peak are on the same side
		   we do not have a qrs complex */
		if (last_peak == state)
			continue;
		
		/* only contiguous (max allowed distance of 30 msec) pos/neg (neg/pos) peaks are allowed */
		if ((curr_pos - last_peak_idx) > ms30)
		{
			last_peak = state;
			last_peak_idx = curr_pos;
			continue;
		}
		
		if (curr_blank > 0)
			continue;

		/* beat found */
		next_beat = curr_pos;
		break;
	}

	return next_beat;
} /* look_for_next_beat() */


struct beats2 *
beat_malloc(struct beats2_mem *mem, int ch_idx, long pos)
{
	struct beats2 *b;

	b = get_beat_mem(mem);

	SET_BEAT_POS(b, pos);
	SET_BEAT_IN_CH(b, ch_idx, 1);
	SET_EVENT_ID(b, -1);

	b->contiguous = 1;

	return b;
} /* beat_malloc() */


struct beats2 *
find_beat(struct beats2 *last_beat, long curr_pos, long beat_range, struct beats2 **prev_beat)
{
	struct beats2 *b = NULL;
	long pos;

	*prev_beat = NULL;

	if (last_beat == NULL)
		return NULL;

	pos = GET_BEAT_POS(last_beat);
	if ((pos + beat_range) < curr_pos)
	{
		*prev_beat = last_beat;
		last_beat = last_beat->next;
		while (last_beat)
		{
			pos = GET_BEAT_POS(last_beat);
			if ((pos + beat_range) < curr_pos)
			{
				*prev_beat = last_beat;
				last_beat = last_beat->next;
				continue;
			}
			pos = GET_BEAT_POS(last_beat);
			if (((pos - beat_range) < curr_pos) &&
			    (curr_pos < (pos + beat_range)))
			{
				b = last_beat;
				*prev_beat = b->prev;
			}
			break;
		}
	}
	else if ((pos - beat_range) > curr_pos)
	{
		last_beat = last_beat->prev;
		while (last_beat)
		{
			*prev_beat = last_beat->prev;
			pos = GET_BEAT_POS(last_beat);
			if ((pos - beat_range) > curr_pos)
			{
				last_beat = last_beat->prev;
				continue;
			}
			pos = GET_BEAT_POS(last_beat);
			if (((pos - beat_range) < curr_pos) &&
			    (curr_pos < (pos + beat_range)))
			{
				b = last_beat;
				*prev_beat = b->prev;
			}
			else
				*prev_beat = last_beat;
			break;
		}
	}
	else
	{
		b = last_beat;
		*prev_beat = b->prev;
	}

	return b;
} /* find_beat() */


int
get_morph_values(struct data_buffer *buf, struct beats2 *curr_beat, int ch_idx, double threshold_s0,
		 double threshold_s2, double curr_hr, struct ra_detect *opt, struct ch_info *ci, long max_t_wave_end)
{
	int ret;

	if ((ret = get_qrs_complex(buf, curr_beat, ch_idx, ci->ms100, ci->ms50, ci->ms30,
				   opt->force_qrs_type)) != 0)
	{
		return ret;
	}
	if ((ret = get_p_wave(buf, curr_beat, ch_idx, MIN_AMP_P_WAVE_SCALE * threshold_s2, ci->ms100, ci->ms30,
			      opt->force_p_type)) != 0)
	{
		return ret;
	}
	if ((ret = get_t_wave(buf, curr_beat, ch_idx, MIN_AMP_T_WAVE_SCALE * threshold_s2, ci->ms100, ci->ms30,
			      curr_hr, opt->force_t_type, opt->t_wave_algorithm, max_t_wave_end)) != 0)
	{
		return ret;
	}

	if ((ret = get_noise_level(buf, curr_beat, ch_idx, ci->ms30)) != 0)
		return ret;

	return 0;
} /* get_morph_values() */


int
get_noise_level(struct data_buffer *buf, struct beats2 *curr_beat, int ch_idx, long ms30)
{
	long beat_pos, t_pos, flags;
	double stdev_before, stdev_after;

	beat_pos = GET_BEAT_POS(curr_beat) - buf->data_offset;
	stdev_before = get_stddev(buf->orig, buf->num_data, beat_pos, 1, ms30, 30, 2);

	flags = GET_MORPH_FLAGS(curr_beat, ch_idx);
	if (flags & ECG_T_WAVE_OK)
	{
		t_pos = GET_T_END(curr_beat, ch_idx);
		stdev_after = get_stddev(buf->orig, buf->num_data, beat_pos + t_pos, 0, ms30, 30, 2);
	}
	else
		stdev_after = 0;

 	if (stdev_before > stdev_after)
		SET_NOISE(curr_beat, ch_idx, stdev_before);
 	else
 		SET_NOISE(curr_beat, ch_idx, stdev_after);

	return 0;
} /* get_noise_level() */


double
get_stddev(double *buf, long len, long pos, int before, long width, long num_steps,
	   long num_use)
{
	struct stdev_struct *stdevs;
	long l, cnt, start_pos, end_pos, shift;
	double m_stdev;

	stdevs = calloc(num_steps, sizeof(struct stdev_struct));

	shift = (long)((double)width / 3.0);

	if (before)
	{
		start_pos = pos - num_steps*shift;
		if (start_pos < 0)
			start_pos = 0;
		end_pos = pos;
	}
	else
	{
		start_pos = pos;
		end_pos = pos + num_steps*shift;
		if (end_pos >= (len-width))
			end_pos = len;
	}

	cnt = 0;
	for (l = start_pos; l < end_pos; l += shift)
	{
		if (cnt >= num_steps)
			break;

		stdevs[cnt].value = stddev(buf + l, width);
		stdevs[cnt].start = l;
		stdevs[cnt].end = l+width;
		cnt++;
	}
	qsort(stdevs, cnt, sizeof(struct stdev_struct), sort_func);

	m_stdev = 0.0;
	for (l = 0; l < num_use; l++)
		m_stdev += stdevs[l].value;
	m_stdev /= (double)num_use;
	free(stdevs);

	return m_stdev;
} /* get_stddev() */


int
get_qrs_complex(struct data_buffer *buf, struct beats2 *curr_beat, int ch_idx,
		long ms100, long ms50, long ms30, long force_type)
{
	int ret = 0;
	long l, m, start_pos, end_pos, pos, pos2;
	double threshold, max_peak;
	struct peak_buffer *peaks, *peaks_orig;
	long num_peaks, morph_type;
	double *sig_use;
	long max_pos;
	double area, max;
	int start_positive, end_positive;

	/* select area where we look for the QRS complex */
	pos = GET_BEAT_POS(curr_beat);
	start_pos = (pos - buf->data_offset) - 4*ms30;
	if (start_pos < 0)
		start_pos = 0;
	end_pos = (pos - buf->data_offset) + 2*ms100;

	/* s0 is better than s1 because when in the R or S wave is a notch,
	   this is seen as the end of the wave */
	sig_use = buf->s1;

	/* calculate the threshold */
	threshold = fabs(sig_use[start_pos]);
	for (l = start_pos; l <= end_pos; l++)
	{
		if (fabs(sig_use[l]) > threshold)
			threshold = fabs(sig_use[l]);
	}
   	threshold *= 0.075;

	/* get the peaks */
	peaks = NULL;
	if (debug) fprintf(stderr, "---------- QRS-Start ----------\n");
	get_peaks(buf, sig_use, start_pos, (end_pos - start_pos + 1), threshold, 0.0,
		  &peaks, &num_peaks, &morph_type, force_type, ms30, 1, 1);
	if (debug) fprintf(stderr, "---------- QRS-End ----------\n");

	if (num_peaks <= 1)
	{
		free(peaks);
		return -1;
	}

	/* and now look for the peak values in the original signal */
	peaks_orig = calloc(num_peaks, sizeof(struct peak_buffer));
	for (l = 0; l < num_peaks-1; l++)
	{
		double max = buf->orig[peaks[l].idx];
		long idx = peaks[l].start;

		for (m = peaks[l].idx; m <= peaks[l+1].idx; m++)
		{
			if (peaks[l].amp > 0)
			{
				if (buf->orig[m] > max)
				{
					max = buf->orig[m];
					idx = m;
				}
			}
			else
			{
				if (buf->orig[m] < max)
				{
					max = buf->orig[m];
					idx = m;
				}
			}
		}
		peaks_orig[l].amp = max;
		peaks_orig[l].idx = idx;

		peaks_orig[l].start = peaks[l].start;
		peaks_orig[l].end = peaks[l].end;
	}
	
	/* check types 1212 and 2121 if the first/last wave makes sense */
	if (morph_type == 1212)
	{
		if ((((peaks[0].end - peaks[0].start) > ms30) 
		     || ((peaks[1].idx - peaks[0].idx) > 2*ms30))
		    && (peaks_orig[0].amp < (0.5 * peaks_orig[1].amp)))
		{
			morph_type = 212;
			memcpy(peaks_orig+0, peaks_orig+1, sizeof(struct peak_buffer));
			memcpy(peaks_orig+1, peaks_orig+2, sizeof(struct peak_buffer));
		}
	}
	if (morph_type == 2121)
	{
		if (((peaks[2].end - peaks[2].start) > ms30)
		    && (peaks_orig[2].amp < (0.5 * peaks_orig[1].amp)))
		{
			morph_type = 121; /* TODO: check if this type is correct or if 212 is right (makes more sense for me now) */
		}
	}

	/* assign peaks according the found QRS morphology */
	SET_Q(curr_beat, ch_idx, -1);
	SET_R(curr_beat, ch_idx, -1);
	SET_S(curr_beat, ch_idx, -1);
	SET_RS(curr_beat, ch_idx, -1);
	SET_QRS_TYPE(curr_beat, ch_idx, morph_type);

	switch (morph_type)
	{
	case 1212:	/* NPNP -> QRS */
		SET_Q(curr_beat, ch_idx, (peaks_orig[0].idx + buf->data_offset));
		SET_R(curr_beat, ch_idx, (peaks_orig[1].idx + buf->data_offset));
		SET_S(curr_beat, ch_idx, (peaks_orig[2].idx + buf->data_offset));
		if (ch_idx == 0)
			SET_BEAT_POS(curr_beat, (peaks_orig[1].idx + buf->data_offset));

		start_positive = 0;
		end_positive = 0;
		break;
	case 2121:	/* PNPN -> RSR' */
		SET_R(curr_beat, ch_idx, (peaks_orig[0].idx + buf->data_offset));
		SET_S(curr_beat, ch_idx, (peaks_orig[1].idx + buf->data_offset));
		SET_RS(curr_beat, ch_idx, (peaks_orig[2].idx + buf->data_offset));
		if (ch_idx == 0)
		{
			if (peaks_orig[0].amp > (0.5 * peaks_orig[1].amp))
				SET_BEAT_POS(curr_beat, (peaks_orig[0].idx + buf->data_offset));
			else
				SET_BEAT_POS(curr_beat, (peaks_orig[1].idx + buf->data_offset));
		}

		start_positive = 1;
		end_positive = 1;
		break;
	case 212:	/* PNP -> RS */
		SET_R(curr_beat, ch_idx, (peaks_orig[0].idx + buf->data_offset));
		SET_S(curr_beat, ch_idx, (peaks_orig[1].idx + buf->data_offset));
		if (ch_idx == 0)
		{
			if (peaks_orig[0].amp > (0.5 * peaks_orig[1].amp))
				SET_BEAT_POS(curr_beat, (peaks_orig[0].idx + buf->data_offset));
			else
				SET_BEAT_POS(curr_beat, (peaks_orig[1].idx + buf->data_offset));
		}

		start_positive = 1;
		end_positive = 0;
		break;
	case 121:	/* NPN -> QR */
		SET_Q(curr_beat, ch_idx, (peaks_orig[0].idx + buf->data_offset));
		SET_R(curr_beat, ch_idx, (peaks_orig[1].idx + buf->data_offset));
		if (ch_idx == 0)
		{
			if (peaks_orig[0].amp > (0.5 * peaks_orig[1].amp))
				SET_BEAT_POS(curr_beat, (peaks_orig[0].idx + buf->data_offset));
			else
				SET_BEAT_POS(curr_beat, (peaks_orig[1].idx + buf->data_offset));
		}

		start_positive = 0;
		end_positive = 1;
		break;
	case 21:	/* PN -> R */
		SET_R(curr_beat, ch_idx, (peaks_orig[0].idx + buf->data_offset));
		if (ch_idx == 0)
			SET_BEAT_POS(curr_beat, (peaks_orig[0].idx + buf->data_offset));

		start_positive = 1;
		end_positive = 1;
		break;
	case 12:	/* NP -> S */
		SET_S(curr_beat, ch_idx, (peaks_orig[0].idx + buf->data_offset));
		if (ch_idx == 0)
			SET_BEAT_POS(curr_beat, (peaks_orig[0].idx + buf->data_offset));

		start_positive = 0;
		end_positive = 0;
		break;
	default:	/* ??? */
		fprintf(stderr, "\nunknown morphology type %ld\n", morph_type);
		free(peaks);
		return -1;
		break;
	}

	/* get start and end of the QRS complex */
	max_peak = fabs(peaks[0].amp);
	for (l = 1; l < num_peaks; l++)
	{
		if (max_peak < fabs(peaks[l].amp))
			max_peak = fabs(peaks[l].amp);
	}

	threshold = (0.05 * max_peak) / (fabs(peaks[0].amp) / max_peak);
	SET_QRS_START(curr_beat, ch_idx, (peaks[0].start + buf->data_offset));
	for (l = peaks[0].idx; l >= peaks[0].start; l--)
	{
		if (fabs(sig_use[l]) < threshold)
		{
			SET_QRS_START(curr_beat, ch_idx, (l + buf->data_offset));
			break;
		}
	}

	threshold = (0.25 * max_peak) / ((5.0 * fabs(peaks[num_peaks-1].amp)) / max_peak);
	SET_QRS_END(curr_beat, ch_idx, (peaks[num_peaks-1].end + buf->data_offset));
	for (l = peaks[num_peaks-1].idx; l <= peaks[num_peaks-1].end; l++)
	{
		if (fabs(sig_use[l]) < threshold)
		{
			SET_QRS_END(curr_beat, ch_idx, (l + buf->data_offset));
			break;
		}
	}

	/* use provisorial QRS-end as starting point */
	pos = GET_QRS_END(curr_beat, ch_idx);
	start_pos = pos - ms30 - buf->data_offset;
	end_pos = start_pos + (3*ms30);
	if (start_pos < ms50)
		start_pos = ms50;
	
	max = 0.0;
	max_pos = start_pos;
	for (l = start_pos; l < end_pos; l++)
	{
		area = 0.0;
		for (m = l-ms50; m < l; m++)
			area += (buf->f0[m] - buf->f0[l]);

		if (max < area)
		{
			max = area;
			max_pos = l;
		}
	}

	SET_QRS_END(curr_beat, ch_idx, (max_pos + buf->data_offset));

	start_pos = peaks_orig[0].idx - ms100;
	if (start_pos < 0)
		start_pos = 0;
	end_pos = peaks_orig[0].idx + 1;
/*  	pos = get_inflection_point(buf->f1, start_pos, end_pos, start_positive, 0); */
/*  	if (pos >= 0) */
/*  		SET_QRS_START(curr_beat, ch_idx, (pos + buf->data_offset)); */

/* 	start_pos = GET_QRS_START(curr_beat, ch_idx) - buf->data_offset; */
/* 	threshold = get_stddev(sig_use, start_pos, ms100, 1, ms30, 30, 2); */
/* 	threshold *= 2.0; */
/* 	for (l = peaks[0].idx; l >= 0; l--) */
/* 	{ */
/* 		if (fabs(sig_use[l]) < threshold) */
/* 		{ */
/* 			SET_QRS_START(curr_beat, ch_idx, (l + buf->data_offset)); */
/* 			break; */
/* 		} */
/* 	} */

	start_pos = peaks_orig[num_peaks-2].idx - ms30;
	end_pos = start_pos + 5*ms30;
	if (start_pos < 0)
		start_pos = 0;
	pos = get_inflection_point(buf->f1, start_pos, end_pos, end_positive, 1);
	if (pos >= 0)
		SET_QRS_END(curr_beat, ch_idx, (pos + buf->data_offset));

	pos = GET_QRS_START(curr_beat, ch_idx);
	pos2 = GET_QRS_END(curr_beat, ch_idx);
	if ((pos2 - pos) > ms30)
	{
		long flags;

		flags = GET_MORPH_FLAGS(curr_beat, ch_idx);
		flags |= ECG_QRS_OK;
		SET_MORPH_FLAGS(curr_beat, ch_idx, flags);
	}

	/* clean up */
	free(peaks);
	free(peaks_orig);
	
	return ret;
} /* get_qrs_complex() */


int
get_p_wave(struct data_buffer *buf, struct beats2 *curr_beat, int ch_idx, double min_amp,
	   long ms100, long ms30, long force_type)
{
	long l, start_pos, end_pos, pos, pos2, dist1, dist2;
	long flags;
	double max, max_peak, threshold, threshold_on, threshold_off;
	struct peak_buffer *peaks;
	long num_peaks, morph_type, peak_1_pos, peak_2_pos;
	double *sig_use;

	SET_P_START(curr_beat, ch_idx, -1);
	SET_P_END(curr_beat, ch_idx, -1);
	SET_P_PEAK_1(curr_beat, ch_idx, -1);
	SET_P_PEAK_2(curr_beat, ch_idx, -1);
	SET_P_TYPE(curr_beat, ch_idx, 0);

	sig_use = buf->s2;

	pos = GET_QRS_START(curr_beat, ch_idx);
	end_pos = pos - ms30 - buf->data_offset;
	if (end_pos <= 0)
		return 0;

	start_pos = end_pos - 2*ms100;
	if (start_pos < 0)
		start_pos = 0;

	max = fabs(sig_use[start_pos]);
	for (l = start_pos+1; l < end_pos; l++)
	{
		if (max < fabs(sig_use[l]))
			max = fabs(sig_use[l]);
	}
	threshold = 0.1 * max;

	/* get the peaks */
	peaks = NULL;
	if (debug) fprintf(stderr, "---------- P-Start ----------\n");
	get_peaks(buf, sig_use, start_pos, (end_pos - start_pos + 1), threshold, 0.1,
		  &peaks, &num_peaks, &morph_type, force_type, ms30, 1, 0);
	if (debug) fprintf(stderr, "---------- P-End ----------\n");
	if (num_peaks < 2)
	{
		free(peaks);
		return 0;
	}

	/* check if minimum amplitude is reached */
	max_peak = fabs(peaks[0].amp);
	for (l = 1; l < num_peaks; l++)
	{
		if (fabs(peaks[l].amp) > max_peak)
			max_peak = fabs(peaks[l].amp);
	}
	if (max_peak < min_amp)
	{
		/* no -> only low amplitude or no P-wave */
		free(peaks);
		return 0;
	}

	SET_P_TYPE(curr_beat, ch_idx, morph_type);

	threshold_on = 0.6 * fabs(peaks[0].amp);
	threshold_off = 0.6 * fabs(peaks[num_peaks-1].amp);

	SET_P_START(curr_beat, ch_idx, (peaks[0].start + buf->data_offset));
	for (l = peaks[0].idx; l >= peaks[0].start; l--)
	{
		if (fabs(sig_use[l]) < threshold_on)
		{
			SET_P_START(curr_beat, ch_idx, (l + buf->data_offset));
			break;
		}
	}

	SET_P_END(curr_beat, ch_idx, (peaks[num_peaks-1].end + buf->data_offset));
	for (l = peaks[num_peaks-1].idx; l <= peaks[num_peaks-1].end; l++)
	{
		if (fabs(sig_use[l]) < threshold_off)
		{
			SET_P_END(curr_beat, ch_idx, (l + buf->data_offset));
			break;
		}
	}

	start_pos = peaks[0].start - ms30;
	end_pos = peaks[0].end;
	pos = get_inflection_point(buf->f0, start_pos, end_pos, (peaks[0].amp > 0 ? 1 : 0), 0);
	if (pos >= 0)
		SET_P_START(curr_beat, ch_idx, (pos + buf->data_offset));

	start_pos = peaks[num_peaks-1].start;
	end_pos = peaks[num_peaks-1].end + ms30;
	/* check if the temporary end of the p-wave is after the start of the QRS complex
	   -> if so, correct the area where we are looking for the real end
	   (of course, we assume that the start of the QRS complex is correct) */
	pos = GET_QRS_START(curr_beat, ch_idx) - buf->data_offset;
	if (end_pos > pos)
		end_pos = pos - ms30;
	pos = get_inflection_point(buf->f0, start_pos, end_pos, (peaks[num_peaks-1].amp > 0 ? 0 : 1), 1);
	if (pos >= 0)
		SET_P_END(curr_beat, ch_idx, (pos + buf->data_offset));

	/* check if start and end if p-wave makes sense */
	pos = GET_P_END(curr_beat, ch_idx);
	pos2 = GET_QRS_START(curr_beat, ch_idx);
	if ((pos >= 0) && (pos2 >= 0))
		dist1 = pos2 - pos;
	else
		dist1 = 0;

	pos = GET_P_START(curr_beat, ch_idx);
	pos2 = GET_P_END(curr_beat, ch_idx);
	if ((pos >= 0) && (pos2 >= 0))
		dist2 = pos2 - pos;
	else
		dist2 = 0;

	if ((dist1 < ms30) || (dist2 < ms30))
	{
		flags = GET_MORPH_FLAGS(curr_beat, ch_idx);
		flags &= ~ECG_P_WAVE_OK;
		SET_MORPH_FLAGS(curr_beat, ch_idx, flags);

		free(peaks);
		return 0;
	}

	/* get positions of the peak(s) */
	peak_1_pos = peaks[0].idx;
	max_peak = buf->orig[peak_1_pos];
	for (l = peaks[0].idx; l <= peaks[1].idx; l++)
	{
		if (peaks[0].amp > 0)
		{
			if (buf->orig[l] > max_peak)
			{
				max_peak = buf->orig[l];
				peak_1_pos = l;
			}
		}
		else
		{
			if (buf->orig[l] < max_peak)
			{
				max_peak = buf->orig[l];
				peak_1_pos = l;
			}
		}
	}
	SET_P_PEAK_1(curr_beat, ch_idx, (peak_1_pos + buf->data_offset));
	
	if ((morph_type == 121) || (morph_type == 212))
	{
		peak_2_pos = peaks[1].idx;
		max_peak = buf->f0[peak_2_pos];
		for (l = peaks[1].idx; l <= peaks[2].idx; l++)
		{
			if (peaks[1].amp > 0)
			{
				if (buf->orig[l] > max_peak)
				{
					max_peak = buf->orig[l];
					peak_2_pos = l;
				}
			}
			else
			{
				if (buf->orig[l] < max_peak)
				{
					max_peak = buf->orig[l];
					peak_2_pos = l;
				}
			}
		}
		SET_P_PEAK_2(curr_beat, ch_idx, (peak_2_pos + buf->data_offset));
	}

	flags = GET_MORPH_FLAGS(curr_beat, ch_idx);
	flags |= ECG_P_WAVE_OK;
	SET_MORPH_FLAGS(curr_beat, ch_idx, flags);

	free(peaks);

	return 0;
} /* get_p_wave() */


int
get_t_wave(struct data_buffer *buf, struct beats2 *curr_beat, int ch_idx, double min_amp,
	   long ms100, long ms30, double hr, long force_type, long end_wave_algo, long end_pos_force)
{
	long l, start_pos, end_pos, pos, pos2, dist, qrs_end, try_cnt;
	double threshold, max_peak, max, rri;
	struct peak_buffer *peaks;
	long num_peaks, morph_type, peak_1_pos, peak_2_pos;
	double *sig_use;

	sig_use = buf->s2;

	SET_T_START(curr_beat, ch_idx, -1);
	SET_T_END(curr_beat, ch_idx, -1);
	SET_T_PEAK_1(curr_beat, ch_idx, -1);
	SET_T_PEAK_2(curr_beat, ch_idx, -1);
	SET_T_TYPE(curr_beat, ch_idx, 0);
	
	pos = GET_QRS_END(curr_beat, ch_idx);
	if (pos < 0)
		pos = GET_BEAT_POS(curr_beat) + ms100;
	qrs_end = pos;

	start_pos = (pos - buf->data_offset) + ms100;
	
	/* size of search window depends on current heart rate */
	if (hr <= 60)
		end_pos = 5*ms100;
	else if (hr >= 100)
		end_pos = 3*ms100;
	else
		end_pos = 5*ms100 - (((long)hr - 60) * 5);
	if (end_pos < 3*ms100)
		end_pos = 3*ms100;
	else if (end_pos > 5*ms100)
		end_pos = 5*ms100;
	end_pos += start_pos;
	
	/* check if end-pos is maybe after next QRS complex */	
	if (curr_beat->contiguous && curr_beat->next && GET_BEAT_IN_CH(curr_beat->next, ch_idx))
	{
		pos = GET_BEAT_POS(curr_beat->next);
		if (pos >= 0)
			end_pos = pos - buf->data_offset - ms100;
	}
	else
	{
		rri = 60000.0 / hr;
		pos = GET_QRS_START(curr_beat, ch_idx);
		if (pos < 0)
			pos = GET_BEAT_POS(curr_beat);
		if ((end_pos - (pos - buf->data_offset)) > rri)
			end_pos = (pos - buf->data_offset) + (long)rri - ms30;
	}
	end_pos -= ms100;
	if ((end_pos - start_pos) > 5*ms100)
		end_pos = start_pos + 5*ms100;
	
	if ((end_pos_force > 0) && (end_pos > end_pos_force))
		end_pos = end_pos_force - ms30;

	/* get max value for threshold */
	max = fabs(sig_use[start_pos]);
	for (l = start_pos+1; l < end_pos; l++)
	{
		if (max < fabs(sig_use[l]))
			max = fabs(sig_use[l]);
	}

	/* get the peaks */
	try_cnt = 2;
	threshold = 0.1 * max;
	peaks = NULL;
	num_peaks = 0;
	while (try_cnt >= 0)
	{
		if (debug) fprintf(stderr, "---------- T-Start ----------\n");
		get_peaks(buf, sig_use, start_pos, (end_pos - start_pos + 1), threshold, 0.1,
			  &peaks, &num_peaks, &morph_type, force_type, ms30, 1, 1);
		if (debug) fprintf(stderr, "---------- T-End ----------\n");
		if (num_peaks >= 2)
			break;

		/* try another time but now with decreased sensitivity */
		if (peaks)
		{
			free(peaks);
			peaks = NULL;
			num_peaks = 0;
		}
		threshold *= 0.75;
		try_cnt--;
	}
	if (num_peaks < 2)
	{
		if (peaks)
			free(peaks);
		return 0;
	}

	/* check if 4 peaks were found (3 peaks in the original signal)
	   -> at least one too much (if not counting U-waves) */
	if ((morph_type == 1212) || (morph_type == 2121))
	{
		/* decide wich one to remove */
		pos = GET_QRS_END(curr_beat, ch_idx);
		if ((peaks[0].start - pos) < (2*ms30))
		{
			num_peaks--;
			memcpy(peaks, peaks+1, sizeof(struct peak_buffer) * num_peaks);
			if (morph_type == 1212)
				morph_type = 212;
			else
				morph_type = 121;
		}
		else
		{
			num_peaks--;
			if (morph_type == 1212)
				morph_type = 121;
			else
				morph_type = 212;
		}
	}

	/* check if minimum amplitude is reached */
	max_peak = fabs(peaks[0].amp);
	for (l = 1; l < num_peaks; l++)
	{
		if (fabs(peaks[l].amp) > max_peak)
			max_peak = fabs(peaks[l].amp);
	}
	if (max_peak < min_amp)
	{
		/* no -> only low amplitude or no T-wave */
		free(peaks);
		return 0;
	}

	SET_T_TYPE(curr_beat, ch_idx, morph_type);

	SET_T_START(curr_beat, ch_idx, (peaks[0].start + buf->data_offset));
	SET_T_END(curr_beat, ch_idx, (peaks[num_peaks-1].end + buf->data_offset));

	start_pos = peaks[0].start - ms100;
	pos = GET_QRS_END(curr_beat, ch_idx);
	if (start_pos < (pos - buf->data_offset))
		start_pos = pos - buf->data_offset;
	end_pos = peaks[0].end + ms30;
	pos = get_inflection_point(buf->f0, start_pos, end_pos, (peaks[0].amp > 0 ? 1 : 0), 0);
	if (pos >= 0)
		SET_T_START(curr_beat, ch_idx, (pos + buf->data_offset));

	switch (end_wave_algo)
	{
	case T_WAVE_END_ALGO_TANGENT:
		t_wave_end_tangent(buf, curr_beat, ch_idx, peaks, num_peaks, ms30);
		break;
	case T_WAVE_END_ALGO_AREA:
		t_wave_end_area(buf, curr_beat, ch_idx, ms100, ms30);
		break;
	case T_WAVE_END_ALGO_LINE:
	default:
		start_pos = peaks[num_peaks-1].start - ms30;
		end_pos = peaks[num_peaks-1].end + ms100;
		if ((end_pos_force > 0) && (end_pos > end_pos_force))
			end_pos = end_pos_force - ms100;
		pos = get_inflection_point(buf->f0, start_pos, end_pos, (peaks[num_peaks-1].amp > 0 ? 0 : 1), 1);
		if (pos >= 0)
			SET_T_END(curr_beat, ch_idx, (pos + buf->data_offset));
		break;
	}

	/* get positions of the peak(s) */
	peak_1_pos = peaks[0].idx;
	max_peak = buf->f0[peak_1_pos];
	for (l = peaks[0].idx; l <= peaks[1].idx; l++)
	{
		if (peaks[0].amp > 0)
		{
			if (buf->orig[l] > max_peak)
			{
				max_peak = buf->orig[l];
				peak_1_pos = l;
			}
		}
		else
		{
			if (buf->orig[l] < max_peak)
			{
				max_peak = buf->orig[l];
				peak_1_pos = l;
			}
		}
	}
	SET_T_PEAK_1(curr_beat, ch_idx, (peak_1_pos + buf->data_offset));
	
	if ((morph_type == 121) || (morph_type == 212))
	{
		peak_2_pos = peaks[1].idx;
		max_peak = buf->f0[peak_2_pos];
		for (l = peaks[1].idx; l <= peaks[2].idx; l++)
		{
			if (peaks[1].amp > 0)
			{
				if (buf->orig[l] > max_peak)
				{
					max_peak = buf->orig[l];
					peak_2_pos = l;
				}
			}
			else
			{
				if (buf->orig[l] < max_peak)
				{
					max_peak = buf->orig[l];
					peak_2_pos = l;
				}
			}
		}
		SET_T_PEAK_2(curr_beat, ch_idx, (peak_2_pos + buf->data_offset));
	}

	free(peaks);

	pos = GET_T_START(curr_beat, ch_idx);
	pos2 = GET_T_END(curr_beat, ch_idx);
	dist = pos2 - pos;

	pos = GET_T_END(curr_beat, ch_idx);
	pos2 = GET_QRS_END(curr_beat, ch_idx);
	if ((pos > pos2) && (dist > ms100))
	{
		long flags;

		flags = GET_MORPH_FLAGS(curr_beat, ch_idx);
		flags |= ECG_T_WAVE_OK;
		SET_MORPH_FLAGS(curr_beat, ch_idx, flags);
	}

	return 0;
} /* get_t_wave() */

	
int
t_wave_end_tangent(struct data_buffer *buf, struct beats2 *curr_beat, int ch_idx,
		   struct peak_buffer *peaks, long num_peaks, long ms30)
{
	struct stdev_struct *stdevs;
	long l, m, num_stdev, cnt, width2, start_pos_iso, end_pos_iso;
	double x, *x_axis, iso, offset, slope;

	/* get T-wave with tangent method */
	/* first get iso-electric level after T-wave */
	num_stdev = 10;
	stdevs = calloc(num_stdev, sizeof(struct stdev_struct));

	start_pos_iso = peaks[num_peaks-1].end;
	end_pos_iso = start_pos_iso + num_stdev*ms30;
	
	cnt = 0;
	for (l = start_pos_iso; l < end_pos_iso; l += ms30)
	{
		if (cnt >= num_stdev)
			break;

		stdevs[cnt].value = stddev(buf->f0+l, ms30);
		stdevs[cnt].start = l;
		stdevs[cnt].end = l+ms30;
		cnt++;
	}
	qsort(stdevs, cnt, sizeof(struct stdev_struct), sort_func);

	iso = 0;
	cnt = 0;
	for (l = 0; l < 4; l++)
	{
		for (m = stdevs[l].start; m < stdevs[l].end; m++)
		{
			iso += buf->f0[m];
			cnt++;
		}
	}
	iso /= (double)cnt;
	free(stdevs);
	
 	width2 = (long)((double)ms30 / 2.0);
	x_axis = malloc(sizeof(double) * 2 * width2);
	for (l = 0; l < 2*width2; l++)
		x_axis[l] = l;

 	calc_regression(x_axis, buf->f0+peaks[num_peaks-1].idx-width2, 2*width2, &offset, &slope);
	free(x_axis);

	x = (iso - offset) / slope;
	SET_T_END(curr_beat, ch_idx, ((long)(x + peaks[num_peaks-1].idx - width2) + buf->data_offset));

	return 0;
} /* t_wave_end_tangent() */


int
sort_func(const void *p1, const void *p2)
{
	struct stdev_struct *s1 = (struct stdev_struct *)p1;
	struct stdev_struct *s2 = (struct stdev_struct *)p2;

	return (s1->value > s2->value);
} /* sort_func() */


int
t_wave_end_area_div(struct data_buffer *buf, struct beats2 *curr_beat, int ch_idx, long ms100, long ms30)
{
	long l, m, start_pos, end_pos, max_pos;
	double area1, area2, ratio, max;

	/* use provisorial T-end as starting point */
	start_pos = GET_T_END(curr_beat, ch_idx);
	start_pos -= (ms30 + buf->data_offset);
	end_pos = start_pos + (ms100);
	
	max = 0.0;
	max_pos = start_pos;
	for (l = start_pos; l < end_pos; l++)
	{
		area1 = 0.0;
		for (m = l-ms30; m < l; m++)
			area1 += buf->orig[m];

		area2 = 0.0;
		for (m = l; m < (l+ms30); m++)
			area2 += buf->orig[m];

		if (area2 == 0.0)
			ratio = 0.0;
		else
			ratio = area1 / area2;
		
		if (max < ratio)
		{
			max = ratio;
			max_pos = l;
		}
	}
	
	SET_T_END(curr_beat, ch_idx, (max_pos + buf->data_offset));

	return 0;
} /* t_wave_end_area_div() */


int
t_wave_end_area_diff(struct data_buffer *buf, struct beats2 *curr_beat, int ch_idx, long ms100, long ms30)
{
	long l, m, start_pos, end_pos, max_pos;
	double area1, area2, diff, max;

	/* use provisorial T-end as starting point */
	start_pos = GET_T_END(curr_beat, ch_idx);
	start_pos -= (ms30 + buf->data_offset);
	end_pos = start_pos + (ms100);
	
	max = 0.0;
	max_pos = start_pos;
	for (l = start_pos; l < end_pos; l++)
	{
		area1 = 0.0;
		for (m = l-ms30; m < l; m++)
			area1 += buf->orig[m];

		area2 = 0.0;
		for (m = l; m < (l+ms30); m++)
			area2 += buf->orig[m];

		diff = area1 - area2;
		if (max < diff)
		{
			max = diff;
			max_pos = l;
		}
	}

	SET_T_END(curr_beat, ch_idx, (max_pos + buf->data_offset));

	return 0;
} /* t_wave_end_area_diff() */


int
t_wave_end_area(struct data_buffer *buf, struct beats2 *curr_beat, int ch_idx, long ms100, long ms30)
{
	long l, m, start_pos, end_pos, max_pos;
	double area, max;

	/* use provisorial T-end as starting point */
	start_pos = GET_T_END(curr_beat, ch_idx);
	start_pos -= (ms30 + buf->data_offset);
	end_pos = start_pos + (2*ms30);
	
	max = 0.0;
	max_pos = start_pos;
	for (l = start_pos; l < end_pos; l++)
	{
		area = 0.0;
		for (m = l-ms100; m < l; m++)
			area += (buf->f0[m] - buf->f0[l]);

		if (max < area)
		{
			max = area;
			max_pos = l;
		}
	}

	SET_T_END(curr_beat, ch_idx, (max_pos + buf->data_offset));

	return 0;
} /* t_wave_end_area() */


int
t_wave_end_distance(struct data_buffer *buf, struct beats2 *curr_beat, int ch_idx, long ms100)
{
	long pos, start_pos, end_pos, inflection_point;

	/* use provisorial T-end as starting point */
	pos = GET_T_START(curr_beat, ch_idx);
	start_pos = pos - buf->data_offset;
	pos = GET_T_END(curr_beat, ch_idx);
	end_pos = pos + ms100 - buf->data_offset;

	if ((inflection_point = get_inflection_point(buf->f0, start_pos, end_pos, 1, 1)) != -1)
		SET_T_END(curr_beat, ch_idx, (inflection_point + buf->data_offset));

	return 0;
} /* t_wave_end_distance() */


long
get_inflection_point_new(double *data, long start_pos, long end_pos, int positive_peak, int get_end)
{	
	long l, peak_pos, inflection_pos, sp, ep, num, start_pos_use;
	double peak, max_dist, alpha;
	double *xr, *yr, *data_use;

	start_pos_use = start_pos;
	if (start_pos_use < 0)
		start_pos_use = 0;

	/* get peak */
	peak = data[start_pos_use];
	peak_pos = start_pos_use;
	for (l = start_pos_use; l <= (end_pos - (end_pos-start_pos_use)/2); l++)
	{
		if (positive_peak & (data[l] > peak))
		{
			peak = data[l];
			peak_pos = l;
		}
		if (!positive_peak & (data[l] < peak))
		{
			peak = data[l];
			peak_pos = l;
		}
	}

	/* rotate coordinate plane */
	/* first prepare data */
	if (get_end)
	{
		sp = peak_pos;
		ep = end_pos;
	}
	else
	{
		sp = start_pos_use;
		ep = peak_pos;
	}
	num = ep - sp + 1;

	data_use = malloc(sizeof(double) * num);
	for (l = 0; l < num; l++)
		data_use[l] = data[sp+l] - data[sp];

	/* then get angle */
	alpha = atan(data_use[num-1] / (double)num);
	
	/* and finally do rotation */
	xr = malloc(sizeof(double) * num);
	yr = malloc(sizeof(double) * num);
	for (l = 0; l < num; l++)
	{
		xr[l] = l * cos(alpha) + data_use[l] * sin(alpha);
		yr[l] = -l * sin(alpha) + data_use[l] * cos(alpha);
	}

	/* and now get the position with the max. distance */
	max_dist = 0;
	inflection_pos = 0;
	for (l = 1; l < num; l++)
	{
		if (positive_peak)
		{
			if (yr[l] < max_dist)
			{
				max_dist = yr[l];
				inflection_pos = l;
			}
		}
		else
		{
			if (yr[l] > max_dist)
			{
				max_dist = yr[l];
				inflection_pos = l;
			}
		}
	}
	inflection_pos += sp;

	free(xr);
	free(yr);
	free(data_use);

	return inflection_pos;
} /* get_inflection_point_new() */


long
get_inflection_point(double *data, long start_pos, long end_pos, int positive_peak, int get_end)
{	
	long l, peak_pos, inflection_pos, start_pos_use;
	double peak, step, diff, max_dist, curr_line;

	start_pos_use = start_pos;
	if (start_pos_use < 0)
		start_pos_use = 0;

	/* get peak (TODO: handle negative peak) */
	peak = data[start_pos_use];
	peak_pos = start_pos_use;
	for (l = start_pos_use; l <= end_pos; l++)
	{
		if (positive_peak & (data[l] > peak))
		{
			peak = data[l];
			peak_pos = l;
		}
		if (!positive_peak & (data[l] < peak))
		{
			peak = data[l];
			peak_pos = l;
		}
	}

	/* calc distance between signal and straight line from peak to start- or end-pos */
	if (get_end)
		step = (data[peak_pos] - data[end_pos]) / (double)(end_pos - peak_pos);
	else
		step = (data[peak_pos] - data[start_pos_use]) / (double)(peak_pos - start_pos_use);

	max_dist = 0.0;
	inflection_pos = peak_pos;
	curr_line = data[peak_pos];

	if (get_end)
	{
		for (l = peak_pos; l <= end_pos; l++)
		{
			diff = curr_line - data[l];
			
			if (positive_peak && (diff > max_dist))
			{
				max_dist = diff;
				inflection_pos = l;
			}
			if (!positive_peak && (diff < max_dist))
			{
				max_dist = diff;
				inflection_pos = l;
			}

			curr_line -= step;
		}
	}
	else
	{
		for (l = peak_pos; l >= start_pos_use; l--)
		{
			diff = curr_line - data[l];
			
			if (positive_peak && (diff > max_dist))
			{
				max_dist = diff;
				inflection_pos = l;
			}
			if (!positive_peak && (diff < max_dist))
			{
				max_dist = diff;
				inflection_pos = l;
			}

			curr_line -= step;
		}
	}

	return inflection_pos;
} /* get_inflection_point() */


int
calc_beat_values(struct beats2 *curr_beat, long ch_idx, double samplerate)
{
	double w, scale;
	long flags;
	long pos1, pos2;

	scale = 1000.0 / samplerate;

	flags = GET_MORPH_FLAGS(curr_beat, ch_idx);

	if (flags & ECG_P_WAVE_OK)
	{
		pos1 = GET_P_START(curr_beat, ch_idx);
		pos2 = GET_P_END(curr_beat, ch_idx);
		w = (double)(pos2 - pos1) * scale;
		SET_P_WIDTH(curr_beat, ch_idx, w);
	}
	else
		SET_P_WIDTH(curr_beat, ch_idx, -1);

	if (flags & ECG_QRS_OK)
	{
		pos1 = GET_QRS_START(curr_beat, ch_idx);
		pos2 = GET_QRS_END(curr_beat, ch_idx);
		w = (double)(pos2 - pos1) * scale;
		SET_QRS_WIDTH(curr_beat, ch_idx, w);
	}
	else
		SET_QRS_WIDTH(curr_beat, ch_idx, -1);

	if (flags & ECG_T_WAVE_OK)
	{
		pos1 = GET_T_START(curr_beat, ch_idx);
		pos2 = GET_T_END(curr_beat, ch_idx);
		w = (double)(pos2 - pos1) * scale;
		SET_T_WIDTH(curr_beat, ch_idx, w);
	}
	else
		SET_T_WIDTH(curr_beat, ch_idx, -1);

	if ((flags & ECG_P_WAVE_OK) && (flags & ECG_QRS_OK))
	{
		pos1 = GET_P_END(curr_beat, ch_idx);
		pos2 = GET_QRS_START(curr_beat, ch_idx);
		w = (double)(pos2 - pos1) * scale;
		SET_PQ(curr_beat, ch_idx, w);
	}
	else
		SET_PQ(curr_beat, ch_idx, -1);

	if ((flags & ECG_QRS_OK) && (flags & ECG_T_WAVE_OK))
	{
		pos1 = GET_QRS_START(curr_beat, ch_idx);
		pos2 = GET_T_END(curr_beat, ch_idx);
		w = (double)(pos2 - pos1) * scale;
		SET_QT(curr_beat, ch_idx, w);
		SET_QTC(curr_beat, ch_idx, w);
	}
	else
	{
		SET_QT(curr_beat, ch_idx, -1);
		SET_QTC(curr_beat, ch_idx, -1);
	}

	return 0;
} /* calc_beat_values() */


int
ch_post_process(struct proc_info *pi, int ch_idx, struct beats2 *bhead, double samplerate)
{
	int ret = 0;

	if (pi || ch_idx || bhead || samplerate)
		;

	return ret;

/* 	long cnt_1212, cnt_2121, cnt_212, cnt_121, cnt_all; */
/* 	struct beats *b; */
/* 	long ms50, ms30; */
/* 	double temp; */

/* 	temp = samplerate * 0.05; */
/* 	ms50 = temp; */
/* 	if ((temp - (double)ms50) >= 0.5) */
/* 		ms50++; */

/* 	temp = samplerate * 0.03; */
/* 	ms30 = temp; */
/* 	if ((temp - (double)ms30) >= 0.5) */
/* 		ms30++;	 */

/* 	b = bhead; */
/* 	cnt_1212 = cnt_2121 = cnt_212 = cnt_121 = cnt_all = 0; */
/* 	while (b) */
/* 	{ */
/* 		cnt_all++; */
/* 		switch (b->qrs_type[ch_idx]) */
/* 		{ */
/* 		case 1212: cnt_1212++; break; */
/* 		case 2121: cnt_2121++; break; */
/* 		case 212: cnt_212++; break; */
/* 		case 121: cnt_121++; break; */
/* 		} */
/* 		b = b->next; */
/* 	} */
/* 	if ((cnt_1212 > 0) && (cnt_212 > 0) && (((double)cnt_1212 / (double)cnt_all) > 0.3)) */
/* 		ret = check_q_wave(pi, ch_idx, bhead, ms50, ms30); */
/* 	if (ret != 0) */
/* 		return ret; */

/* 	if ((cnt_2121 > 0) && (cnt_212 > 0) && (((double)cnt_2121 / (double)cnt_all) > 0.3)) */
/* 		ret = check_rs_wave(pi, ch_idx, bhead, ms50, ms30); */
/* 	if (ret != 0) */
/* 		return ret; */

/* 	return ret; */
} /* ch_post_process() */


int
post_process(struct detect_result *dr)
{
	int ret = 0;
	double *ch_scale;
	int i;
	struct beats2 *b;

	/* if we threre is only one channel, we have nothing to do here */
	if (dr->num_ch <= 1)
		return 0;

	ch_scale = malloc(sizeof(double) * dr->num_ch);
	for (i = 0; i < dr->num_ch; i++)
		ch_scale[i] = 1000.0 / dr->samplerate[i];
	
	b = get_first_beat(&(dr->b));
	while (b)
	{
		/* check limits of P-wave, QRS complex and T-wave */
		check_limits(b, BEATS_P_START_L, BEATS_P_END_L, ch_scale, dr->num_ch,
			     LIMIT_P_START, LIMIT_P_END, ~ECG_P_WAVE_OK);
		check_limits(b, BEATS_QRS_START_L, BEATS_QRS_END_L, ch_scale, dr->num_ch,
			     LIMIT_QRS_START, LIMIT_QRS_END, ~ECG_QRS_OK);
 		check_limits(b, BEATS_T_START_L, BEATS_T_END_L, ch_scale, dr->num_ch,
 			     LIMIT_T_START, LIMIT_T_END, ~ECG_T_WAVE_OK);

		b = b->next;
	}

	return ret;
} /* post_process() */


int
check_limits(struct beats2 *b, long start_idx, long end_idx, double *ch_scale, long num_ch,
	     double limit_start, double limit_end, long clean_mask)
{
	long l, cnt, in_ch;
	double start, end;
		
	start = end = 0.0;
	cnt = 0;
	for (l = 0; l < num_ch; l++)
	{
		long start_pos, end_pos;

		in_ch = GET_BEAT_IN_CH(b, l);

		start_pos = BEAT_VALUE_GET_LONG(b, start_idx, l);
		end_pos = BEAT_VALUE_GET_LONG(b, end_idx, l);
		
		if (!in_ch || (start_pos == -1) || (end_pos == -1))
			continue;
		
		start += (double)start_pos;
		end += (double)end_pos;
		cnt++;
	}
	if (cnt == 0)
		return 0;
	
	start /= (double)cnt;
	end /= (double)cnt;

	for (l = 0; l < num_ch; l++)
	{
		long in_ch, start_pos, end_pos;

		in_ch = GET_BEAT_IN_CH(b, l);

		start_pos = BEAT_VALUE_GET_LONG(b, start_idx, l);
		end_pos = BEAT_VALUE_GET_LONG(b, end_idx, l);
		
		if (!in_ch || (start_pos == -1) || (end_pos == -1))
			continue;
		
		if ((fabs(start_pos - start) > (limit_start / ch_scale[l])) ||
		    (fabs(end_pos - end) > (limit_end / ch_scale[l])))
		{
			long flags;

			flags = GET_MORPH_FLAGS(b, l);
			flags |= clean_mask;
			SET_MORPH_FLAGS(b, l, flags);
			
			BEAT_VALUE_SET_LONG(b, start_idx, l, -1);
			BEAT_VALUE_SET_LONG(b, end_idx, l, -1);
		}
	}

	return 0;
} /* check_limits() */


int
calc_ch_indep_values(struct detect_result *dr)
{
	int i;
	long l;
	struct beats2 *b;
	double *ch_max_scale;
	double max_samplerate, max_to_msec;
	double min_p_start, max_p_end, min_qrs_start, max_qrs_end, min_t_start, max_t_end;
	double noise;

	dr->pos = malloc(sizeof(long) * dr->num_beats);
	dr->annot = malloc(sizeof(long) * dr->num_beats);

	dr->p_start = calloc(dr->num_beats, sizeof(double));
	dr->p_end = calloc(dr->num_beats, sizeof(double));
	dr->p_width = calloc(dr->num_beats, sizeof(double));
	dr->qrs_start = calloc(dr->num_beats, sizeof(double));
	dr->qrs_end = calloc(dr->num_beats, sizeof(double));
	dr->qrs_width = calloc(dr->num_beats, sizeof(double));
	dr->t_start = calloc(dr->num_beats, sizeof(double));
	dr->t_end = calloc(dr->num_beats, sizeof(double));
	dr->t_width = calloc(dr->num_beats, sizeof(double));
	dr->pq = calloc(dr->num_beats, sizeof(double));
	dr->qt = calloc(dr->num_beats, sizeof(double));
	dr->qtc = calloc(dr->num_beats, sizeof(double));
	dr->noise = calloc(dr->num_beats, sizeof(double));

	max_samplerate = dr->samplerate[0];
	for (i = 1; i < dr->num_ch; i++)
	{
		if (dr->samplerate[i] > max_samplerate)
			max_samplerate = dr->samplerate[i];
	}
	max_to_msec = 1000.0 / max_samplerate;

	ch_max_scale = malloc(sizeof(double) * dr->num_ch);
	for (i = 0; i < dr->num_ch; i++)
		ch_max_scale[i] = max_samplerate / dr->samplerate[i];

	b = get_first_beat(&(dr->b));
	for (l = 0; l < dr->num_beats; l++)
	{
		dr->pos[l] = GET_BEAT_POS(b);

		min_p_start = max_p_end = min_qrs_start = max_qrs_end = min_t_start = max_t_end = -1;

		noise = 0.0;
		for (i = 0; i < dr->num_ch; i++)
		{
			long flags, in_ch, pos;

			noise += GET_NOISE(b, i);

			in_ch = GET_BEAT_IN_CH(b, i);
			if (!in_ch)
				continue;

			flags = GET_MORPH_FLAGS(b, i);

			pos = GET_P_START(b, i);
			if ((flags & ECG_P_WAVE_OK) && ((min_p_start == -1) || ((pos * ch_max_scale[i]) < min_p_start)))
			{
				min_p_start = pos * ch_max_scale[i];
			}
			pos = GET_P_END(b, i);
			if ((flags & ECG_P_WAVE_OK) && ((pos * ch_max_scale[i]) > max_p_end))
			{
				max_p_end = pos * ch_max_scale[i];
			}
			pos = GET_QRS_START(b, i);
			if ((flags & ECG_QRS_OK) && ((min_qrs_start == -1) || ((pos * ch_max_scale[i]) < min_qrs_start)))
			{
				min_qrs_start = pos * ch_max_scale[i];
			}
			pos = GET_QRS_END(b, i);
			if ((flags & ECG_QRS_OK) && ((pos * ch_max_scale[i]) > max_qrs_end))
			{
				max_qrs_end = pos * ch_max_scale[i];
			}
			pos = GET_T_START(b, i);
			if ((flags & ECG_T_WAVE_OK) && ((min_t_start == -1) || ((pos * ch_max_scale[i]) < min_t_start)))
			{
				min_t_start = pos * ch_max_scale[i];
			}
			pos = GET_T_END(b, i);
			if ((flags & ECG_T_WAVE_OK) && ((pos * ch_max_scale[i]) > max_t_end))
			{
				max_t_end = pos * ch_max_scale[i];
			}
		}
		dr->noise[l] = noise / (double)(dr->num_ch);

		if ((min_p_start != -1) && (max_p_end != -1))
		{
			dr->p_start[l] = min_p_start;
			dr->p_end[l] = max_p_end;
			dr->p_width[l] = (max_p_end - min_p_start) * max_to_msec;
		}
		else
		{
			dr->p_start[l] = dr->p_end[l] = -1;
			dr->p_width[l] = -1;
		}

		if ((min_p_start != -1) && (min_qrs_start != 1))
			dr->pq[l] = (min_qrs_start - min_p_start) * max_to_msec;
		else
			dr->pq[l] = -1;

		if ((min_qrs_start != -1) && (max_qrs_end != -1))
		{
			dr->qrs_start[l] = min_qrs_start;
			dr->qrs_end[l] = max_qrs_end;
			dr->qrs_width[l] = (max_qrs_end - min_qrs_start) * max_to_msec;
		}
		else
		{
			dr->qrs_start[l] = dr->qrs_end[l] = -1;
			dr->qrs_width[l] = -1;
		}

		if ((min_t_start != -1) && (max_t_end != -1))
		{
			dr->t_start[l] = min_t_start;
			dr->t_end[l] = max_t_end;
			dr->t_width[l] = (max_t_end - min_t_start) * max_to_msec;
		}
		else
		{
			dr->t_start[l] = dr->t_end[l] = -1;
			dr->t_width[l] = -1;
		}

		if ((min_qrs_start != -1) && (max_t_end != -1))
		{
			dr->qt[l] = (max_t_end - min_qrs_start) * max_to_msec;
			if (l == 0)
				dr->qtc[l] = -1;
			else
			{
				dr->qtc[l] = dr->qt[l] / sqrt((dr->pos[l] - dr->pos[l-1])/max_samplerate);
			}
		}
		else
			dr->qt[l] = dr->qtc[l] = -1;

		b = b->next;
	}

	free(ch_max_scale);

	return 0;
} /* calc_ch_indep_values() */


int
classify_beats( struct detect_result *dr)
{
	long l;
	long num;
	double width_threshold;

	width_threshold = 0.0;
	num = 100;
	if (num > dr->num_beats)
		num = dr->num_beats;
	for (l = 0; l < num; l++)
		width_threshold += dr->qrs_width[l];
	width_threshold /= (double)num;
	width_threshold *= 1.1;

	/* width threshold is not really working; change in vector and different template class
	   should be more specific */
	for (l = 0; l < dr->num_beats; l++)
	{
		SET_CLASS(ECG_CLASS_SINUS, dr->annot[l]);
	}

	return 0;
} /* classifiy_beats() */


