/*----------------------------------------------------------------------------
 * detect_ecg.h
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2005-2006, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifndef _DETECT_ECG_H
#define _DETECT_ECG_H

#include <ra_defines.h>
#include "main_detect.h"


/* -------------------- defines -------------------- */
#define PLUGIN_VERSION  "0.1.0"
#define PLUGIN_NAME     "detect-ecg"
#define PLUGIN_DESC     gettext_noop("performs ECG beat-detection and morphology measures")
#define PLUGIN_TYPE     PLUGIN_PROCESS
#define PLUGIN_LICENSE  LICENSE_LGPL



enum result_idx
{
	NUM, POS, CH, ANNOT,

	NOISE, CH_NOISE,
	MORPH_FLAGS, CH_MORPH,
	P_TYPE, CH_P_TYPE,
	QRS_TYPE, CH_QRS_TYPE,
	T_TYPE, CH_T_TYPE,

	P_START, P_END, P_WIDTH, CH_P,
	P_PEAK_1, CH_P_PEAK_1,
	P_PEAK_2, CH_P_PEAK_2,

	QRS_START, QRS_END, QRS_WIDTH, CH_QRS,
	Q_PEAK, CH_Q_PEAK,
	R_PEAK, CH_R_PEAK,
	S_PEAK, CH_S_PEAK,
	RS_PEAK, CH_RS_PEAK,

	T_START, T_END, T_WIDTH, CH_T,
	T_PEAK_1, CH_T_PEAK_1,
	T_PEAK_2, CH_T_PEAK_2,
	
	PQ, CH_PQ,
	QT, QTC, QTA, CH_QT
};

/* -------------------- structures -------------------- */


/* -------------------- globals -------------------- */
static struct ra_result_infos results[] = {
	{"num", gettext_noop("number of found events"), RA_VALUE_TYPE_LONG},
	{"pos", gettext_noop("positions of the found events in sampleunits"), RA_VALUE_TYPE_LONG_ARRAY},
	{"ch", gettext_noop("bit-mask indicating the channels in which the event was found"), RA_VALUE_TYPE_LONG_ARRAY},
	{"annot", gettext_noop("annotations to the found events"), RA_VALUE_TYPE_LONG_ARRAY},

	/* morphology informations (layout: ALL values (= 'num' values) for 1st channel,
	   ALL values for 2nd channel ...) */	
	{"noise", gettext_noop(""), RA_VALUE_TYPE_DOUBLE_ARRAY},
	{"ch_noise", gettext_noop(""), RA_VALUE_TYPE_LONG_ARRAY},
	{"morph_flags", gettext_noop(""), RA_VALUE_TYPE_LONG_ARRAY},
	{"ch_morph", gettext_noop(""), RA_VALUE_TYPE_LONG_ARRAY},
	{"p_type", gettext_noop(""), RA_VALUE_TYPE_LONG_ARRAY},
	{"ch_p_type", gettext_noop(""), RA_VALUE_TYPE_LONG_ARRAY},
	{"qrs_type", gettext_noop(""), RA_VALUE_TYPE_LONG_ARRAY},
	{"ch_qrs_type", gettext_noop(""), RA_VALUE_TYPE_LONG_ARRAY},
	{"t_type", gettext_noop(""), RA_VALUE_TYPE_LONG_ARRAY},
	{"ch_t_type", gettext_noop(""), RA_VALUE_TYPE_LONG_ARRAY},

	/* values are offsets to the position stored in 'pos';
	   a value of INVALID_OFFSET_VALUE (=32767) means no value available */
	{"p_start", gettext_noop(""), RA_VALUE_TYPE_SHORT_ARRAY},
	{"p_end", gettext_noop(""), RA_VALUE_TYPE_SHORT_ARRAY},
	{"p_width", gettext_noop(""), RA_VALUE_TYPE_DOUBLE},
	{"ch_p", gettext_noop(""), RA_VALUE_TYPE_LONG_ARRAY},
	{"p_peak_1", gettext_noop(""), RA_VALUE_TYPE_SHORT_ARRAY},
	{"ch_p_peak_1", gettext_noop(""), RA_VALUE_TYPE_LONG_ARRAY},
	{"p_peak_2", gettext_noop(""), RA_VALUE_TYPE_SHORT_ARRAY},
	{"ch_p_peak_2", gettext_noop(""), RA_VALUE_TYPE_LONG_ARRAY},

	{"qrs_start", gettext_noop(""), RA_VALUE_TYPE_SHORT_ARRAY},
	{"qrs_end", gettext_noop(""), RA_VALUE_TYPE_SHORT_ARRAY},
	{"qrs_width", gettext_noop(""), RA_VALUE_TYPE_DOUBLE},
	{"ch_qrs", gettext_noop(""), RA_VALUE_TYPE_LONG_ARRAY},
	{"q_peak", gettext_noop(""), RA_VALUE_TYPE_SHORT_ARRAY},
	{"ch_q_peak", gettext_noop(""), RA_VALUE_TYPE_LONG_ARRAY},
	{"r_peak", gettext_noop(""), RA_VALUE_TYPE_SHORT_ARRAY},
	{"ch_r_peak", gettext_noop(""), RA_VALUE_TYPE_LONG_ARRAY},
	{"s_peak", gettext_noop(""), RA_VALUE_TYPE_SHORT_ARRAY},
	{"ch_s_peak", gettext_noop(""), RA_VALUE_TYPE_LONG_ARRAY},
	{"rs_peak", gettext_noop(""), RA_VALUE_TYPE_SHORT_ARRAY},
	{"ch_rs_peak", gettext_noop(""), RA_VALUE_TYPE_LONG_ARRAY},

	{"t_start", gettext_noop(""), RA_VALUE_TYPE_SHORT_ARRAY},
	{"t_end", gettext_noop(""), RA_VALUE_TYPE_SHORT_ARRAY},
	{"t_width", gettext_noop(""), RA_VALUE_TYPE_DOUBLE},
	{"ch_t", gettext_noop(""), RA_VALUE_TYPE_LONG_ARRAY},
	{"t_peak_1", gettext_noop(""), RA_VALUE_TYPE_SHORT_ARRAY},
	{"ch_t_peak_1", gettext_noop(""), RA_VALUE_TYPE_LONG_ARRAY},
	{"t_peak_2", gettext_noop(""), RA_VALUE_TYPE_SHORT_ARRAY},
	{"ch_t_peak_2", gettext_noop(""), RA_VALUE_TYPE_LONG_ARRAY},

	/* overall morphology values which should be used for further analysis
	   (one value for each event) */
	{"pq", gettext_noop(""), RA_VALUE_TYPE_DOUBLE},
	{"ch_pq", gettext_noop(""), RA_VALUE_TYPE_LONG_ARRAY},
	{"qt", gettext_noop(""), RA_VALUE_TYPE_DOUBLE},
	{"qtc", gettext_noop(""), RA_VALUE_TYPE_DOUBLE},
	{"qta", gettext_noop(""), RA_VALUE_TYPE_DOUBLE},
	{"ch_qt", gettext_noop(""), RA_VALUE_TYPE_LONG_ARRAY},
};
long num_results = sizeof(results) / sizeof(results[0]);

static struct ra_option_infos options[] = {
	{"num_ch", gettext_noop("number of channels set in 'ch'"), RA_VALUE_TYPE_LONG, -1, 0},
	{"ch", gettext_noop("list of channels used for the event detection"), RA_VALUE_TYPE_LONG_ARRAY, -1, -1},
	{"combine_beats", gettext_noop("flag if nearby beats (distance below 'min_beat_distance') should be combined"),
	 RA_VALUE_TYPE_SHORT, 0, 0},
	{"min_beat_distance", gettext_noop("minimum allowed distance between beats [seconds]"), RA_VALUE_TYPE_DOUBLE, 0, 0},
	{"save_in_eval", gettext_noop("flag if results should be saved in an evaluation"), RA_VALUE_TYPE_SHORT, 0, 0},
	{"eh", gettext_noop("eval-handle used to store the results"), RA_VALUE_TYPE_VOIDP, 0, 0},
	{"clh", gettext_noop("event-class-handle used to store the results when region was selected"),
	 RA_VALUE_TYPE_VOIDP, 0, 0},

	{"use_region", gettext_noop("search beats in a given area"), RA_VALUE_TYPE_SHORT, 0, 0},
	{"region_start", gettext_noop("start position of the search area"), RA_VALUE_TYPE_LONG, 0, 0},
	{"region_end", gettext_noop("end position of the search area"), RA_VALUE_TYPE_LONG, 0, 0},
	{"region_start_is_beatpos", gettext_noop("'region_start' is the beat position"), RA_VALUE_TYPE_SHORT, 0, 0},

	{"num_events", gettext_noop("number of events set in 'events' option"), RA_VALUE_TYPE_LONG, 0, 0},
	{"events", gettext_noop("heartbeat events for which the wave-boundary detection should be re-run"),
	 RA_VALUE_TYPE_LONG_ARRAY, 0, -1},

	{"filter_powerline_noise", gettext_noop("flag if a power-line noise filter should be applied"),
	 RA_VALUE_TYPE_SHORT, 0, 0},
	{"filter_baseline_wander", gettext_noop("flag if a baseline-wander filter should be applied"),
	 RA_VALUE_TYPE_SHORT, 0, 0},

	{"check_for_calibration", gettext_noop("flag if for a calibration signal should be searched"),
	 RA_VALUE_TYPE_SHORT, 0, 0},

	{"do_interpolation", gettext_noop("flag if no interpolation should be performed for signals "
					  "with lower sampling rates (below 500Hz)"),
	 RA_VALUE_TYPE_SHORT, 0, 0},
	
	{"t_wave_algorithm", gettext_noop("selects which T-wave-end algortithm should be used:\n"
					  "0: default (line method)\n"
					  "1: tangent method\n"
					  "2: area method\n"
					  "3: line method"),
	 RA_VALUE_TYPE_LONG, 0, 0},

	/* wave-form morphology types which should be the P-/QRS-/T-Wave
	   forced to (used when editing an ecg) */
	{"force_p_type", gettext_noop(""),
	 RA_VALUE_TYPE_LONG, 0, 0},
	{"force_qrs_type", gettext_noop(""),
	 RA_VALUE_TYPE_LONG, 0, 0},
	{"force_t_type", gettext_noop(""),
	 RA_VALUE_TYPE_LONG, 0, 0},

	/* options when the wave limits should be derived around a specific position */
	{"check_wave_at_cursor", gettext_noop(""), RA_VALUE_TYPE_SHORT, 0, 0},
	{"edit_cursor_pos", gettext_noop(""), RA_VALUE_TYPE_LONG, 0, 0},
	{"wave_to_check", gettext_noop("selects which 'wave'-limits should be checked:\n"
		                       "0: P-wave\n"
		                       "1: QRS-complex\n"
		                       "2: T-wave"), RA_VALUE_TYPE_LONG, 0, 0},

	/* percentage of threshold levels used in wave detection (1=100%) */
	{"thresh_p", gettext_noop(""), RA_VALUE_TYPE_DOUBLE, 0, 0},
	{"thresh_qrs", gettext_noop(""), RA_VALUE_TYPE_DOUBLE, 0, 0},
	{"thresh_t", gettext_noop(""), RA_VALUE_TYPE_DOUBLE, 0, 0},
};
long num_options = sizeof(options) / sizeof(options[0]);

/* infos to do processing of signals automagically */
/*static struct ra_autoprocess_prop_needed prop_needed[] = {
};
long num_prop_needed = sizeof(prop_needed) / sizeof(prop_needed[0]);*/
/* long num_prop_needed = 0; */

/* static struct ra_autoprocess_sig_needed sigs_needed[] = { */
/* 	{RA_CH_TYPE_ECG, "ch", RA_VALUE_TYPE_LONG_ARRAY}, */
/* }; */
/* long num_sigs_needed = sizeof(sigs_needed) / sizeof(sigs_needed[0]); */

static struct ra_auto_create_class create_class[] = {
	{"heartbeat", POS, POS, "", "", "", NULL, 0, NULL},
};
long num_create_class = sizeof(create_class) / sizeof(create_class[0]);

static struct ra_auto_create_prop create_prop[] = {
	{"qrs-pos", 0, -1, POS, -1, NULL, 1},
/* 	{"qrs-ch", 0, -1, CH, -1, NULL, 1}, */
	{"qrs-annot", 0, -1, ANNOT, -1, NULL, 1},
	{"ecg-noise", 0, -1, NOISE, CH_NOISE, NULL, 1},
	{"ecg-morph-flags", 0, -1, MORPH_FLAGS, CH_MORPH, NULL, 1},
	{"ecg-p-type", 0, -1, P_TYPE, CH_P_TYPE, NULL, 1},
	{"ecg-qrs-type", 0, -1, QRS_TYPE, CH_QRS_TYPE, NULL, 1},
	{"ecg-t-type", 0, -1, T_TYPE, CH_T_TYPE, NULL, 1},
	{"ecg-p-start", 0, -1, P_START, CH_P, NULL, 1},
	{"ecg-p-end", 0, -1, P_END, CH_P, NULL, 1},
	{"ecg-p-width", 0, -1, P_WIDTH, CH_P, NULL, 1},
 	{"ecg-p-peak-1", 0, -1, P_PEAK_1, CH_P_PEAK_1, NULL, 1},
 	{"ecg-p-peak-2", 0, -1, P_PEAK_2, CH_P_PEAK_2, NULL, 1},
	{"ecg-qrs-start", 0, -1, QRS_START, CH_QRS, NULL, 1},
	{"ecg-qrs-end", 0, -1, QRS_END, CH_QRS, NULL, 1},
	{"ecg-qrs-width", 0, -1, QRS_WIDTH, CH_QRS, NULL, 1},
	{"ecg-q-peak", 0, -1, Q_PEAK, CH_Q_PEAK, NULL, 1},
	{"ecg-r-peak", 0, -1, R_PEAK, CH_R_PEAK, NULL, 1},
	{"ecg-s-peak", 0, -1, S_PEAK, CH_S_PEAK, NULL, 1},
	{"ecg-rs-peak", 0, -1, RS_PEAK, CH_RS_PEAK, NULL, 1},
	{"ecg-t-start", 0, -1, T_START, CH_T, NULL, 1},
	{"ecg-t-end", 0, -1, T_END, CH_T, NULL, 1},
	{"ecg-t-width", 0, -1, T_WIDTH, CH_T, NULL, 1},
 	{"ecg-t-peak-1", 0, -1, T_PEAK_1, CH_T_PEAK_1, NULL, 1},
 	{"ecg-t-peak-2", 0, -1, T_PEAK_2, CH_T_PEAK_2, NULL, 1},
	{"ecg-pq", 0, -1, PQ, CH_PQ, NULL, 1},
	{"ecg-qt", 0, -1, QT, CH_QT, NULL, 1},
	{"ecg-qtc", 0, -1, QTC, CH_QT, NULL, 1},
};
long num_create_prop = sizeof(create_prop) / sizeof(create_prop[0]);


struct beat_props
{
	prop_handle pos, noise, rri, flags;
	prop_handle p_type, p_start, p_peak_1, p_peak_2, p_end, p_width;
	prop_handle qrs_type, qrs_start, q, r, s, rs, qrs_end, qrs_width;
	prop_handle t_type, t_start, t_peak_1, t_peak_2, t_end, t_width;
	prop_handle pq, qt, qtc;
}; /* struct beat_props */


/* -------------------- prototypes -------------------- */
int pl_call_gui(proc_handle proc);
proc_handle pl_get_proc_handle(plugin_handle pl);
void set_default_options(struct ra_detect *opt);
void set_option_offsets(struct ra_option_infos *opt_inf, struct ra_detect *opt);
void pl_free_proc_handle(proc_handle proc);
int pl_do_processing(proc_handle proc);

int get_ecg_channels(rec_handle rh, long **ch, long *num_ch);

int get_beat_infos(long num_events, long *events, long num_ch, long *ch, class_handle clh, struct detect_result *dr);
int get_beat_props(class_handle clh, struct beat_props *p);
int fill_beat_ch_infos(struct beat_props *p, long ch, long ch_idx, long interp_factor, struct beats2 *b);

int store_results(struct proc_info *pi, struct detect_result *dr);
int store_single_value(struct ra_detect *opt, struct beats2_mem *beats_mem, int *ch_in_sig, long value_idx,
		       double *ch_indep_value_d, int calc_offset, int type,
		       short *sbuf, long *lbuf, double *dbuf, long *ch, long num_values,
		       long res_ch_idx, long res_idx, long res_number, value_handle *res);


#endif /* _DETECT_ECG_H */
