/*----------------------------------------------------------------------------
 * dawes_redman.h
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2003-2006, Raphael Schneider
 * 
 *
 * $Id$
 *--------------------------------------------------------------------------*/


#ifndef DAWES_REDMAN_H
#define DAWES_REDMAN_H

#include <ra_defines.h>
#include <ra_dawes_redman.h>


/* -------------------- defines -------------------- */
#define PLUGIN_VERSION   "0.2.0"
#define PLUGIN_NAME      "dawes-redman"
#define PLUGIN_DESC      gettext_noop("calculate FHR variations using the Dawes/Redman criteria")
#define PLUGIN_TYPE      PLUGIN_PROCESS
#define PLUGIN_LICENSE   LICENSE_LGPL

#define EPOCH_LEN  3.75  /* in seconds */
#define EPOCHS_MIN   16  /* number of epochs in one minute */

#define MIN_DIFF_MATERNAL_PULSE  20 /* minimum difference (in bpm) between succeeding values to identify maternal pulse */

#define MIN_FHR   50
#define MAX_FHR  200


/* defines for artifact-check at the beginning */
#define CHECK_BEGIN_MINUTES    5
#define CHECK_BEGIN_MIN_FHR   95
#define CHECK_BEGIN_MAX_FHR  180

/* Defines for digital filter designed by mkfilter/mkshape/gencode   A.J. Fisher
   Command line: /www/usr/fisher/helpers/mkfilter -Bu -Lp -o 4 -a 6.3742032246e-03 0.0000000000e+00 -l */
#define FILT_NZEROS 4
#define FILT_NPOLES 4
#define FILT_GAIN   6.549261075e+06


enum result_idx
{
	FHR_EPOCHS,		/* epochs of the fetal heart rate (averages over 3.75 seconds) */
	FHR_BASELINE,		/* baseline of the fetal heart rate */
	SIGNAL_LOST,
	NUM_VALID_EPOCHS,
	BASAL_FHR,
	ACCEL_10,
	POS_ACCEL_10,
	ACCEL_15,
	POS_ACCEL_15,
	LOST_BEATS_20,
	POS_LOST_BEATS_20,
	LOST_BEATS_21_100,
	POS_LOST_BEATS_21_100,
	LOST_BEATS_101,
	POS_LOST_BEATS_101,	
	MINUTE_RANGE,
	MINUTE_RANGE_MS,
	HIGH_VARIATIONS,
	MIN_HIGH_VARIATIONS,
	LOW_VARIATIONS,
	MIN_LOW_VARIATIONS,
	LONG_TERM_VARIATIONS_MS,
	LONG_TERM_VARIATIONS_BPM,
	LONG_TERM_VARIATIONS_HIGH_MS,
	LONG_TERM_VARIATIONS_HIGH_BPM,
	SHORT_TERM_VARIATIONS
};


/* -------------------- structures -------------------- */
struct epoch
{
	double percent_noise;

	/* epoch values coming from the original data */
	double epoch_raw_ms;
	double epoch_raw_bpm;
	
	/* epoch values after some checking/interpolation */
	double epoch_ms;
	double epoch_bpm;
	int interpolated;	/* flag if epoch value was interpolated */

	/* epoch values after low-pass filter */
	double filt_ms;
	double filt_bpm;

	/* baseline */
	double baseline_ms;
	double baseline_bpm;

	/* fhr variations */
	int in_accel;
	int in_decel;
	int in_low_variation;
	int in_high_variation;
}; /* struct epoch */


struct event_infos
{
	long index;
	double pos;
	double value_1;
	double value_2;

	int dont_use_event;
}; /* struct event_infos */


/* -------------------- globals -------------------- */
static struct ra_result_infos results[] = {
	{"FHR_EPOCHS", gettext_noop("epochs of the fetal heart rate (averages over 3.75 seconds)"), RA_VALUE_TYPE_DOUBLE_ARRAY, 1},
	{"FHR_BASELINE", gettext_noop("baseline of the fetal heart rate"), RA_VALUE_TYPE_DOUBLE_ARRAY, 1},
	{"SIGNAL_LOST", "signal lost in percent", RA_VALUE_TYPE_DOUBLE, 1},
	{"NUM_VALID_EPOCHS", "number of valid epochs", RA_VALUE_TYPE_LONG, 1},
	{"BASAL_FHR", "basal heart rate of the fetus", RA_VALUE_TYPE_DOUBLE, 1},
	{"ACCEL_10", "number of accelerations > 10bpm and <= 15bpm", RA_VALUE_TYPE_LONG, 1},
	{"POS_ACCEL_10", "positions of the accelerations between 10 and 15bpm", RA_VALUE_TYPE_LONG_ARRAY, 1},
	{"ACCEL_15", "number of accelerations > 15bpm", RA_VALUE_TYPE_LONG, 1},
	{"POS_ACCEL_15", "positions of the accelerations > 15bpm", RA_VALUE_TYPE_LONG_ARRAY, 1},
	{"LOST_BEATS_20", "decelerations < 20 lost beats", RA_VALUE_TYPE_LONG, 1},
	{"POS_LOST_BEATS_20", "positions of the decelerations < 20", RA_VALUE_TYPE_LONG_ARRAY, 1},
	{"LOST_BEATS_21_100", "decelerations between 20 and 100 lost beats", RA_VALUE_TYPE_LONG, 1},
	{"POS_LOST_BEATS_21_100", "postions of the decelerations between 20 and 100", RA_VALUE_TYPE_LONG_ARRAY, 1},
	{"LOST_BEATS_101", "decelerations above 100 lost beats", RA_VALUE_TYPE_LONG, 1},
	{"POS_LOST_BEATS_101", "postions of the decelerations above 100", RA_VALUE_TYPE_LONG_ARRAY, 1},
	{"MINUTE_RANGE", "minute range for each minute in bpm", RA_VALUE_TYPE_DOUBLE_ARRAY, 1},
	{"MINUTE_RANGE_MS", "miniute range for each minute in msec", RA_VALUE_TYPE_DOUBLE_ARRAY, 1},
	{"HIGH_VARIATIONS", "number of minutes with high variations", RA_VALUE_TYPE_LONG, 1},
	{"MIN_HIGH_VARIATIONS", "the minutes with the high variations", RA_VALUE_TYPE_LONG_ARRAY, 1},
	{"LOW_VARIATIONS", "number of minutes with low variations", RA_VALUE_TYPE_LONG, 1},
	{"MIN_LOW_VARIATIONS", "the minutes with low variations", RA_VALUE_TYPE_LONG_ARRAY, 1},
	{"LONG_TERM_VARIATIONS_MS", "long term variations for all minutes in msec", RA_VALUE_TYPE_DOUBLE, 1},
	{"LONG_TERM_VARIATIONS_BPM", "long term variations for all minutes in bpm", RA_VALUE_TYPE_DOUBLE, 1},
	{"LONG_TERM_VARIATIONS_HIGH_MS", "long term variations for minutes with high variations in msec", RA_VALUE_TYPE_DOUBLE, 1},
	{"LONG_TERM_VARIATIONS_HIGH_BPM", "long term variations for minutes with high variations in bpm", RA_VALUE_TYPE_DOUBLE, 1},
	{"SHORT_TERM_VARIATIONS", "short term variations", RA_VALUE_TYPE_DOUBLE, 1}
};
long num_results = sizeof(results) / sizeof(results[0]);

static struct ra_option_infos options[] = {
	{"eh", gettext_noop("evaluation used; if '= NULL' use default evaluation"), RA_VALUE_TYPE_VOIDP, -1, 0},

	{"use_start_end_pos", gettext_noop("use values between start_pos and end_pos"), RA_VALUE_TYPE_SHORT, -1, 0},
	{"start_pos", gettext_noop("start-pos in sample-units"), RA_VALUE_TYPE_LONG, -1, 0},
	{"end_pos", gettext_noop("end-pos in sample-units"), RA_VALUE_TYPE_LONG, -1, 0},

	{"rh", gettext_noop("recording handle"), RA_VALUE_TYPE_LONG, -1, 0},
	{"ch_num", gettext_noop("channel with the FHR"), RA_VALUE_TYPE_LONG, -1, 0},
	{"use_ignore_value", gettext_noop("flag if there are values which are not valid"), RA_VALUE_TYPE_SHORT, -1, 0},
	{"ignore_value", gettext_noop("value which indicate not valid values"), RA_VALUE_TYPE_DOUBLE, -1, 0},
	{"filter_maternal_pulse", gettext_noop("run filter to find maternal pulse and remove these values"),
	 RA_VALUE_TYPE_SHORT, -1, 0},
	{"ignore_marked_regions", gettext_noop("ignore regions which are marked in the annotations with the IGNORE flag"),
	 RA_VALUE_TYPE_SHORT, -1, 0},
	{"ignore_noise_regions", gettext_noop("ignore regions which are marked in the annotations with the NOISE flag"),
	 RA_VALUE_TYPE_SHORT, -1, 0},
};
long num_options = sizeof(options) / sizeof(options[0]);


/* -------------------- prototypes -------------------- */
int pl_call_gui(proc_handle proc);
proc_handle pl_get_proc_handle(plugin_handle pl);
void pl_free_proc_handle(proc_handle proc);
int pl_do_processing(proc_handle proc);

void set_default_options(struct ra_dawes_redman *opt);
void set_option_offsets(struct ra_option_infos *opt_inf, struct ra_dawes_redman *p);

int get_data(struct ra_dawes_redman *opt, struct epoch **epochs, long *num);
int identify_invalid_values(struct ra_dawes_redman *opt, double *data, long n);
double get_samplerate(any_handle h, long ch_num);
void handle_ignore_regions(struct ra_dawes_redman *opt, double *data, long n, double scale);
int calc_epochs(double *raw, long n_raw, struct epoch **epochs, long *n_epochs, double samplerate);
int interpolate_epochs(struct epoch *epochs, long num);

long get_mode(struct epoch *epochs, long n, long *mode_cnt);
int get_histogram(struct epoch *epochs, long n, long **hist, long *n_hist, long *min, long *max);

int filter_data(struct epoch *epochs, long n);

int estimate_baseline(struct epoch *epochs, long n);
long get_mod_mode(struct epoch *epochs, long n);

int get_fhr_variations(struct epoch *epochs, long n, value_handle *vh);
int calc_signal_lost(struct epoch *epochs, long n, double *sig_lost);
int find_accelerations(struct epoch *epochs, long n, long *n_accel_10, long **pos_accel_10, long *n_accel_15, long **pos_accel_15);
int find_decelerations(struct epoch *epochs, long n, long *n_lost_beats_20, long *n_lost_beats_21_100, long *n_lost_beats_101);
int find_variations(struct epoch *epochs, long n, double *ltv_bpm, double *ltv_ms, double *ltv_high_bpm, double *ltv_high_ms, 
		    long *n_high_variations, long *n_low_variations, double **minute_range, double **minute_range_ms, long *n_min);
int calc_short_term_variations(struct epoch *epochs, long n, double *svt);
int calc_basal_heart_rate(struct epoch *epochs, long n, double *bhr);

/* int save(proc_handle proc, eval_handle eh); */

#endif /* DAWES_REDMAN_H */
