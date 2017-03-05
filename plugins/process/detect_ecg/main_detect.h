/*----------------------------------------------------------------------------
 * main_detect.h
 *
 * 
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2005-2006, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifndef MAIN_DETECT_H
#define MAIN_DETECT_H

#include <ra.h>
#include <ra_priv.h>
#include <ra_detect.h>

#include "beat_mem.h"

struct detect_info
{
	meas_handle mh;
	rec_handle rh;
}; /* struct detect_info */


struct beats
{
	struct beats *next;
	struct beats *prev;

	long pos;		/* position of beat */
	long ch_found;
	double hr;

	int *beat_in_ch;
	double *noise;
	long *morph_flags;

	int *p_type;
	long *p_start;
	long *p_peak_1;
	long *p_peak_2;
	long *p_end;
	double *p_width;

	int *qrs_type;
	long *qrs_start;
	long *q;
	long *r;
	long *s;
	long *rs;
	long *qrs_end;
	double *qrs_width;

	int *t_type;
	long *t_start;
	long *t_peak_1;
	long *t_peak_2;
	long *t_end;
	double *t_width;

	double *pq;
	double *qt;
	double *qtc;

	long event_id;
}; /* struct beats */



struct detect_result
{
	int num_ch;		/* number of channels where we searched for beats */
	int *ch_in_sig;		/* channel-numbers in the recording  */

	long num_beats;		/* num of detected beats */
	struct beats2_mem b;	/* detected beats */

	double *samplerate;	/* samplerate used in each channel */
	long *interp_factor;	/* interpolation factor used for each channel */
	long *calib_offset;	/* end of calibration sequence (= -1 if no calibration sequence was found) */

	long *ch_found;		/* bit-mask showing in which channel the beats were found
				   (at the moment a maximum of 31 channels is supported) */

	/* beat values independend of the channel (in msec) */
	long *pos;
	long *annot;

	double *p_start;
	double *p_end;
	double *p_width;

	double *qrs_start;
	double *qrs_end;
	double *qrs_width;

	double *t_start;
	double *t_end;
	double *t_width;

	double *pq;
	double *qt;
	double *qtc;
	double *noise;
}; /* struct detect_result */


int main_detect(struct proc_info *pi, struct detect_result *dr);
long search_beat(rec_handle rh, int ch, long pos, double samplerate, int len);


/* ------------------------------ internal ------------------------------ */
#define BUFFER_SIZE  100000
#define HR_BUF_SIZE       5

#define LIMIT_P_START     30
#define LIMIT_P_END       30
#define LIMIT_QRS_START   15
#define LIMIT_QRS_END     50
#define LIMIT_T_START    100
#define LIMIT_T_END       50

#define MIN_AMP_P_WAVE_SCALE  0.02
#define MIN_AMP_T_WAVE_SCALE  0.05

#define MIN_AMP_STEPS   5

#define INVALID_OFFSET_VALUE  32767


struct data_buffer
{
	long num_data;
	long data_offset;

	long filter_baseline_len;
	double *filter_baseline;
	long filter_len1;
	double *filter1;
	long filter_len2;
	double *filter2;
	/* filter-3 used for wave-boundaries of P- and T-waves */
	long filter_len3;
	double *filter3;

	double *orig;
	double *orig_deriv;
	double *f0;
	double *s0;
	double *f1;
	double *s1;
	double *f2;
	double *s2;
}; /* struct data_buffer */


struct ch_info
{
	double samplerate;
	double samplerate_orig;

	double amp_res;

	long num_samples;
	long interp_factor;

	long s5;
	long ms100;
	long ms50;
	long ms30;
	long ms10;
}; /* struct ch_info */


struct peak_buffer
{
	double amp;
	long idx;
	long start;
	long end;
}; /* struct peak_buffer */

struct stdev_struct
{
	double value;
	long start, end;
}; /* struct stdev_struct */


#ifdef WIN32
#define inline  __inline
#endif /* WIN32 */


int check_calib(struct proc_info *pi, int ch, long *calib_offset);
int ch_detect(struct proc_info *pi, int ch, int ch_idx, long *n, struct beats2_mem *bmem,
	      double *samplerate, long *interp_factor, long pos_offset);
int ch_recalc(struct proc_info *pi, int ch, int ch_idx, struct beats2_mem *b, double *samplerate, long *interp_factor);

int get_ch_infos(rec_handle rh, long ch, int do_interpolation, struct ch_info *ci);
int prepare_data_access(struct ch_info *ci, struct data_buffer *buf);

long get_data(rec_handle rh, long ch, long ch_pos, struct data_buffer *buf, long num_to_read,
	      long interp_factor, int filter_pl_noise, int filter_baseline);
long look_for_next_beat(double *data, long num_data, long pos, double threshold, long blanktime, long ms30);

struct beats2 *beat_malloc(struct beats2_mem *mem, int ch_idx, long pos);
struct beats2 *find_beat(struct beats2 *last_beat, long curr_pos, long beat_range, struct beats2 **prev_beat);

int get_morph_values(struct data_buffer *buf, struct beats2 *curr_beat, int ch_idx, double threshold_s0,
		     double threshold_s2, double curr_hr, struct ra_detect *opt, struct ch_info *ci, long max_t_wave_end);

int get_noise_level(struct data_buffer *buf, struct beats2 *curr_beat, int ch_idx, long ms30);
double get_stddev(double *buf, long len, long pos, int before, long width, long num_steps,
		  long num_use);

int get_qrs_complex(struct data_buffer *buf, struct beats2 *curr_beat, int ch_idx,
		    long ms100, long ms50, long ms30, long force_type);
int get_p_wave(struct data_buffer *buf, struct beats2 *curr_beat, int ch_idx, double min_amp,
	       long ms100, long ms30, long force_type);
int get_t_wave(struct data_buffer *buf, struct beats2 *curr_beat, int ch_idx, double min_amp,
	       long ms100, long ms30, double hr, long force_type, long end_wave_algo, long end_pos_force);

int t_wave_end_tangent(struct data_buffer *buf, struct beats2 *curr_beat, int ch_idx,
		       struct peak_buffer *peaks, long num_peaks, long ms30);
int t_wave_end_area_div(struct data_buffer *buf, struct beats2 *curr_beat, int ch_idx, long ms100, long ms30);
int t_wave_end_area_diff(struct data_buffer *buf, struct beats2 *curr_beat, int ch_idx, long ms100, long ms30);
int t_wave_end_area(struct data_buffer *buf, struct beats2 *curr_beat, int ch_idx, long ms100, long ms30);
int t_wave_end_distance(struct data_buffer *buf, struct beats2 *curr_beat, int ch_idx, long ms100);

long get_inflection_point(double *data, long start_pos, long end_pos, int positive_peak, int get_end);

int calc_beat_values(struct beats2 *curr_beat, long ch_idx, double samplerate);


int ch_post_process(struct proc_info *pi, int ch_idx, struct beats2 *bhead, double samplerate);
int check_q_wave(struct proc_info *pi, int ch_idx, struct beats2 *bhead, long ms50, long ms30);
int check_rs_wave(struct proc_info *pi, int ch_idx, struct beats2 *bhead, long ms50, long ms30);

int post_process(struct detect_result *dr);
int check_limits(struct beats2 *b, long start_idx, long end_idx, double *ch_scale, long num_ch,
		 double limit_start, double limit_end, long clean_mask);

int calc_ch_indep_values(struct detect_result *dr);
int classify_beats( struct detect_result *dr);

int sort_beats(const void *p1, const void *p2);
int sort_func(const void *p1, const void *p2);


#endif /* MAIN_DETECT_H */
