/*----------------------------------------------------------------------------
 * hrv.h
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002-2006, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/


#ifndef HRV_H
#define HRV_H

#include <ra_defines.h>
#include <ra_hrv.h>


/* -------------------- defines -------------------- */
#define PLUGIN_VERSION   "0.4.0"
#define PLUGIN_NAME      "hrv"
#define PLUGIN_DESC      gettext_noop("calculate heart rate variability (HRV) parameters")
#define PLUGIN_TYPE      PLUGIN_PROCESS
#define PLUGIN_LICENSE   LICENSE_LGPL

#define MIN_RRI    300
#define MAX_RRI   2000


enum result_idx
{ SDNN, HRVTI, SDANN, RMSSD, PNN50,
  TP, ULF, VLF, LF, LF_NORM, HF, HF_NORM, LF_HF_RATIO, POWER_LAW,
  TACHO_INDEX, USER_BAND,
  SD1, SD2,
  DFA, DFA_OFFSET, DFA1, DFA1_OFFSET, DFA2, DFA2_OFFSET, DFA_USER, DFA_USER_OFFSET, DFA_X, DFA_Y
};


#define TOTAL_POWER_START  0.0000
#define TOTAL_POWER_END    0.4000
#define ULF_START          0.0000
#define ULF_END            0.0030
#define VLF_START          0.0030
#define VLF_END            0.0400
#define LF_START           0.0400
#define LF_END             0.1500
#define HF_START           0.1500
#define HF_END             0.4000
#define POWER_LAW_START    0.0001
#define POWER_LAW_END      0.0100

#define EQUIDIST_MS   300

#define DFA_DEFAULT_SEGMENT     8000
#define DFA_ALPHA1_ALPHA2_CUT     11


/* -------------------- structures -------------------- */
struct beats
{
	long idx;
	long ev_id;
	double pos;
	double rri;
	long rri_class;
	long qrs_temporal;
};				/* beats */


/* -------------------- globals -------------------- */
static struct ra_result_infos results[] = {
	{"SDNN", gettext_noop("standard deviation of normal-to-normal intervals"), RA_VALUE_TYPE_DOUBLE, 1},
	{"HRVI", gettext_noop("HRV-Index"), RA_VALUE_TYPE_DOUBLE, 1},
	{"SDANN", gettext_noop("standard deviation of averaged normal-to-normal intervals"), RA_VALUE_TYPE_DOUBLE, 1},
	{"rmssd", gettext_noop("root mean of squared sucsessive differences"), RA_VALUE_TYPE_DOUBLE, 1},
	{"pNN50", "", RA_VALUE_TYPE_DOUBLE, 1},
	{"TP", gettext_noop("total power"), RA_VALUE_TYPE_DOUBLE, 1},
	{"ULF", gettext_noop("ultra low frequency power"), RA_VALUE_TYPE_DOUBLE, 1},
	{"VLF", gettext_noop("very low frequency power of short-term recordings"), RA_VALUE_TYPE_DOUBLE, 1},
	{"LF", gettext_noop("low frequency power"), RA_VALUE_TYPE_DOUBLE, 1},
	{"LF_NORM", gettext_noop("normalised low frequency power"), RA_VALUE_TYPE_DOUBLE, 1},
	{"HF", gettext_noop("high freuqency power"), RA_VALUE_TYPE_DOUBLE, 1},
	{"HF_NORM", gettext_noop("normalised high frequency power"), RA_VALUE_TYPE_DOUBLE, 1},
	{"LF_HF_RATIO", gettext_noop("LF/HF ratio"), RA_VALUE_TYPE_DOUBLE, 1},
	{"POWER_LAW", gettext_noop("power law behavior"), RA_VALUE_TYPE_DOUBLE, 1},
 	{"TACHO_INDEX", gettext_noop("Event numbers used for HRV calculations"), RA_VALUE_TYPE_LONG_ARRAY, 0},
 	{"USER_BAND", gettext_noop("frequency power in a user-selected frequency band"), RA_VALUE_TYPE_DOUBLE, 0},
 	{"SD1", gettext_noop("SD1 of the Poincare Plot"), RA_VALUE_TYPE_DOUBLE, 1},
 	{"SD2", gettext_noop("SD2 of the Poincare Plot"), RA_VALUE_TYPE_DOUBLE, 1},
 	{"DFA", gettext_noop("overall DFA Alpha"), RA_VALUE_TYPE_DOUBLE, 1},
 	{"DFA_OFFSET", gettext_noop("offset of the overall DFA Alpha slope"), RA_VALUE_TYPE_DOUBLE, 1},
 	{"DFA1", gettext_noop("DFA Alpha1"), RA_VALUE_TYPE_DOUBLE, 1},
 	{"DFA1_OFFSET", gettext_noop("offset of the DFA Alpha-1 slope"), RA_VALUE_TYPE_DOUBLE, 1},
 	{"DFA2", gettext_noop("DFA Alpha2"), RA_VALUE_TYPE_DOUBLE, 1},
 	{"DFA2_OFFSET", gettext_noop("offset of the DFA Alpha-2 slope"), RA_VALUE_TYPE_DOUBLE, 1},
 	{"DFA_USER", gettext_noop("DFA Alpha of user-defined range"), RA_VALUE_TYPE_DOUBLE, 1},
 	{"DFA_USER_OFFSET", gettext_noop("offset of the user-defined DFA Alpha slope"), RA_VALUE_TYPE_DOUBLE, 1},
 	{"DFA_X", gettext_noop("x-axis for DFA plot"), RA_VALUE_TYPE_DOUBLE_ARRAY, 0},
 	{"DFA_Y", gettext_noop("y-axis for DFA plot"), RA_VALUE_TYPE_DOUBLE_ARRAY, 0},
};
long num_results = sizeof(results) / sizeof(results[0]);

static struct ra_option_infos options[] = {
	{"eh", gettext_noop("evaluation used; if '= NULL' use default evaluation"), RA_VALUE_TYPE_VOIDP, -1, 0},

	{"use_class", gettext_noop("use data from the event-class given in 'clh'"), RA_VALUE_TYPE_SHORT, -1, 0},
	{"clh", gettext_noop("event class with the data used for the calculations"), RA_VALUE_TYPE_VOIDP, -1, 0},
	{"ch", gettext_noop("channel being the source of the values which should be used for the calculations"),
	 RA_VALUE_TYPE_LONG, -1, 0},
	{"prop_value", gettext_noop("event property holding the values used for the calculations"), RA_VALUE_TYPE_CHAR, -1, 0},
	{"use_event_pos", gettext_noop("flag if positions should be used from the events (=1) or from 'prop_pos' (=0)"),
	 RA_VALUE_TYPE_SHORT, -1, 0},
	{"prop_pos", gettext_noop("event property holding the positions of the values (needed for freq. analysis)"),
	 RA_VALUE_TYPE_CHAR, -1, 0},
	{"prop_annot", gettext_noop("event property with the beat annotation"), RA_VALUE_TYPE_CHAR, -1, 0},
	{"prop_temporal", gettext_noop("event property with the temporal behaviour of the beats"),
	 RA_VALUE_TYPE_CHAR, -1, 0},
	{"ignore_annot", gettext_noop("ignore beat annotations and use all beats"), RA_VALUE_TYPE_SHORT, -1, 0},
	
	{"use_start_end_pos", gettext_noop("use events between start_pos and end_pos"), RA_VALUE_TYPE_SHORT, -1, 0},
	{"start_pos", gettext_noop("start-pos in sample-units"), RA_VALUE_TYPE_LONG, -1, 0},
	{"end_pos", gettext_noop("end-pos in sample-units"), RA_VALUE_TYPE_LONG, -1, 0},

	{"num_events", gettext_noop("use the events listed in events"), RA_VALUE_TYPE_LONG, -1, 0},
	{"events", gettext_noop("list of events to use"), RA_VALUE_TYPE_LONG_ARRAY, -1, -1},

	{"num_ignore_events", gettext_noop("number of events which should be ignored"), RA_VALUE_TYPE_LONG, -1, 0},
	{"ignore_events", gettext_noop("list of events to ignore"), RA_VALUE_TYPE_LONG_ARRAY, -1, -1},

	{"num_values", gettext_noop(""), RA_VALUE_TYPE_LONG, -1, 0},
	{"values", gettext_noop(""), RA_VALUE_TYPE_DOUBLE_ARRAY, -1, -1},
	{"pos", gettext_noop(""), RA_VALUE_TYPE_DOUBLE_ARRAY, -1, -1},
	{"samplerate", gettext_noop(""), RA_VALUE_TYPE_DOUBLE, -1, -1},

	{"use_min_max", gettext_noop("check if values are between 'min_value' and 'max_value'"), RA_VALUE_TYPE_SHORT, -1, 0},
	{"min_value", gettext_noop("minimum allowed value"), RA_VALUE_TYPE_DOUBLE, -1, 0},
	{"max_value", gettext_noop("maximum allowed value"), RA_VALUE_TYPE_DOUBLE, -1, 0},

	{"use_user_freq_band", gettext_noop("calculate also the power in the user-selected frequency band"),
	 RA_VALUE_TYPE_SHORT, -1, 0},
	{"user_freq_start", gettext_noop("start of user-selected frequency band [Hz]"), RA_VALUE_TYPE_DOUBLE, -1, 0},
	{"user_freq_end", gettext_noop("end of user-selected frequency band [Hz]"), RA_VALUE_TYPE_DOUBLE, -1, 0},

	{"poincare_lag", gettext_noop("lag used for Poincare plots"), RA_VALUE_TYPE_LONG, -1, 0},

	{"dfa_use_only_nn", gettext_noop("use only normal-to-normal intervals for DFA"), RA_VALUE_TYPE_SHORT, -1, 0},
	{"dfa_segment_size", gettext_noop("segment size used for DFA (= 0: use complete signal)"), RA_VALUE_TYPE_LONG, -1, 0},
	{"dfa_alpha1_alpha2_cut", gettext_noop("box-size used for calculation of DFA alpha-1 and -2 (value "
					       "is upper limit used for alpha-1, incl. the value)"),
	 RA_VALUE_TYPE_LONG, -1, 0},
	{"dfa_use_user_range", gettext_noop("flag if dfa-alpha should be calculated between a user-defined range"),
	 RA_VALUE_TYPE_SHORT, -1, 0},
	{"dfa_user_lower_limit", gettext_noop("lower limit of the DFA user-defined range (incl. the limit value)"),
	 RA_VALUE_TYPE_LONG, -1, 0},
	{"dfa_user_upper_limit", gettext_noop("upper limit of the DFA user-defined ragne (incl. the limit value)"),
	 RA_VALUE_TYPE_LONG, -1, 0},

	{"skip_time_domain", gettext_noop("do not calculate HRV in the time domain"), RA_VALUE_TYPE_SHORT, -1, 0},
	{"skip_freq_domain", gettext_noop("do not calculate HRV in the frequency domain"), RA_VALUE_TYPE_SHORT, -1, 0},
	{"skip_nld_analysis", gettext_noop("do not calculate non-linear dynamic HRV parameters"), RA_VALUE_TYPE_SHORT, -1, 0},

	{"save_in_eval", gettext_noop("save results in eval-file (not supported yet)"), RA_VALUE_TYPE_SHORT, -1, 0},
};
long num_options = sizeof(options) / sizeof(options[0]);


/* -------------------- prototypes -------------------- */
int pl_call_gui(proc_handle proc);
proc_handle pl_get_proc_handle(plugin_handle pl);
void pl_free_proc_handle(proc_handle proc);
int pl_do_processing(proc_handle proc);

void set_default_options(struct ra_hrv *opt);
void set_option_offsets(struct ra_option_infos *opt_inf, struct ra_hrv *opt);

int get_number_runs(struct ra_hrv *opt);
double get_samplerate(eval_handle eh);
int get_values(struct ra_hrv *opt, long run, struct beats **b, long *num);
int get_given_values(struct ra_hrv *opt, struct beats **b, long *num);
int get_event_list(struct ra_hrv *opt, class_handle clh, prop_handle pos, long **list, long *num);
int remove_ignored_events(struct ra_hrv *opt, struct beats **b, long *num);
int filter_values(struct ra_hrv *opt, struct beats **b, long *num,
		  struct beats **b_no_art, long *num_no_art);

int calc_hrv_timedomain(struct beats *b, long num, value_handle *res, long run_num);
void calc_sdnn(struct beats *b, long num, value_handle res);
void calc_hrvi(struct beats *b, long num, value_handle res);
void calc_sdann(struct beats *b, long num, value_handle res);
void calc_rmssd(struct beats *b, long num, value_handle res);
void calc_pnn50(struct beats *b, long num, value_handle res);

int calc_hrv_freqdomain(struct proc_info *pi, struct beats *b, long num, char *window, value_handle *res, long run_num);
int get_periodogram(struct proc_info *pi, struct beats *b, long num, char *window, int smooth_spec,
		    double **spec, long *n_spec);

/* int calc_spec(double *equi, long n, double samplefreq, double **spec, long *n_spec, char *window); */
int get_hrv_freqvalues(struct ra_hrv *opt, double *spec, long n, value_handle *res, long run_num);
double calc_power(double start, double end, double sample_dist_ms, double *spec, long n);
long calc_idx_from_freq(double samplefreq, long n_sample, double freq);
int calc_total_power(double *spec, long n, value_handle res);
int calc_ulf_power(double *spec, long n, value_handle res);
int calc_vlf_power(double *spec, long n, value_handle res);
int calc_lf_power(double *spec, long n, value_handle res);
int calc_hf_power(double *spec, long n, value_handle res);
int calc_power_law(double *spec, long n, double samplefreq, value_handle *res, long n_spec_orig);

int calc_nld(struct ra_hrv *opt, struct beats *b, long num, struct beats *b_no_art, long num_no_art, value_handle *res);
int calc_poincare(struct ra_hrv *opt, struct beats *b, long num, value_handle *res);
int calc_dfa(struct ra_hrv *opt, struct beats *b, long num, struct beats *b_no_art, long num_no_art, value_handle *res);
int calc_dfa_scales(struct beats *b, long len, double *scales, long *num_scale_values);

/* saving results not implemented yet */
int save(proc_handle proc, eval_handle eh);

#endif /* HRV_H */
