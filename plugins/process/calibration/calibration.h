/*----------------------------------------------------------------------------
 * calibration.h
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2004-2005, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <ra_defines.h>
#include <ra_calibration.h>


/* -------------------- defines -------------------- */
#define PLUGIN_VERSION  "0.3.0"
#define PLUGIN_NAME     "calibration"
#define PLUGIN_DESC     gettext_noop("measure calibration signals in a recording")
#define PLUGIN_TYPE     PLUGIN_PROCESS
#define PLUGIN_LICENSE  LICENSE_LGPL

enum result_idx
{
	CALIB_HEIGHT, CALIB_HIGH_VALUE, CALIB_LOW_VALUE, CALIB_WIDTH,
	CALIB_ACCURACY, CALIB_OUT_OF_RANGE,
	CALIB_TYPE, CALIB_DATA, CALIB_CYCLE_POS
};

/* defines for the state machine */
#define ST_LEVEL_LOW       1
#define ST_LEVEL_HIGH      2
#define ST_IN_CHANGE_UP    3
#define ST_IN_CHANGE_DOWN  4
#define ST_ERROR           5
/* defines for the autocorrelation */
#define AUTOCORR_DIFF_LAG   5
/* defines for the level in the calibration pulses */
#define CALIB_LEVEL_NONE    0
#define CALIB_LEVEL_HIGH    1
#define CALIB_LEVEL_LOW     2


/* -------------------- structures -------------------- */

struct cycle_info
{
	long start;
	long end;

	int num_high_level;
	long start_high;
	long end_high;

	int num_low_level;
	long start_low;
	long end_low;

	int valid;
};


/* -------------------- globals -------------------- */
static struct ra_result_infos results[] = {
	{"calib_height", gettext_noop("calibration value"), RA_VALUE_TYPE_DOUBLE, -1},
	{"calib_high_value", gettext_noop("high-level value of the calibration signal"), RA_VALUE_TYPE_DOUBLE, -1},
	{"calib_low_value", gettext_noop("low-level value of the calibration signal"), RA_VALUE_TYPE_DOUBLE, -1},
	{"calib_width", gettext_noop("width of the calibration signal (measured using 'calib_data')"), RA_VALUE_TYPE_DOUBLE, -1},
	{"calib_type", gettext_noop("type of the calibration signal"), RA_VALUE_TYPE_LONG, -1},
	{"calib_accuracy", gettext_noop("accurcy of the calibration height [percent]"), RA_VALUE_TYPE_DOUBLE, -1},
	{"calib_out_of_range", gettext_noop("flag if calibration cycles reached upper or lower value range"), RA_VALUE_TYPE_LONG, -1},
	{"calib_data", gettext_noop("data used to measure calibration (all valid segments combined)"), RA_VALUE_TYPE_DOUBLE_ARRAY, -1},
	{"calib_cycle_pos", gettext_noop("start- and end-position of valid calibration cycles"), RA_VALUE_TYPE_LONG_ARRAY, -1},
};
long num_results = sizeof(results) / sizeof(results[0]);

static struct ra_option_infos options[] = {
	{"rh", gettext_noop("recording handle"), RA_VALUE_TYPE_LONG, -1, 0},
	{"ch", gettext_noop("channel of the recording used to measure the calibration signal"), RA_VALUE_TYPE_LONG, -1, 0},
	{"start_pos", gettext_noop("signal-position to start search for calibration signal [su]"), RA_VALUE_TYPE_LONG, -1, 0},
	{"end_pos", gettext_noop("signal-position to stop search for calibration signal [su]"), RA_VALUE_TYPE_LONG, -1, 0},

	{"use_data", gettext_noop("use the data given in 'data'"), RA_VALUE_TYPE_SHORT, -1, 0},
	{"num_data", gettext_noop("number of the data given in 'data'"), RA_VALUE_TYPE_LONG, -1, 0},
	{"data", gettext_noop("perform calibration meausure using this data"), RA_VALUE_TYPE_DOUBLE_ARRAY, -1, -1},
	{"samplerate", gettext_noop("samplerate used for the values in 'data'"), RA_VALUE_TYPE_DOUBLE, -1, -1},

	{"type", gettext_noop("type of calibration signal (see CALIB_TYPE_* in ra_calibration.h)"), RA_VALUE_TYPE_LONG, -1, 0},
	{"difference_lag", gettext_noop("gap between samples used for difference signal [su]"), RA_VALUE_TYPE_LONG, -1, 0},
	{"min_calib_amp", gettext_noop("percentage of maximum difference used as minimal calibration amplitude"),
	 RA_VALUE_TYPE_DOUBLE, -1, 0},
	{"segment_length", gettext_noop("grid-width used to search for continous calibration signals [seconds]"),
	 RA_VALUE_TYPE_DOUBLE, -1, 0},
	{"min_level_length", gettext_noop("minimum duration staying on the same level [seconds]"), RA_VALUE_TYPE_DOUBLE, -1, 0},
	{"calib_cycle_length", gettext_noop("length of one calibration cycle [seconds]"), RA_VALUE_TYPE_DOUBLE, -1, 0},
	{"get_cycle_length", gettext_noop("flag if the calibration cycle length should be measured"), RA_VALUE_TYPE_SHORT, -1, 0},
	{"min_cycles", gettext_noop("required minimum number of contiguous calibration cycles"), RA_VALUE_TYPE_LONG, -1, 0},
};
long num_options = sizeof(options) / sizeof(options[0]);


/* -------------------- prototypes -------------------- */
int pl_call_gui(proc_handle proc);
proc_handle pl_get_proc_handle(plugin_handle pl);
void set_default_options(struct ra_calibration *opt);
void set_option_offsets(struct ra_option_infos *opt_inf, struct ra_calibration *opt);
void pl_free_proc_handle(proc_handle proc);
int pl_do_processing(proc_handle proc);

int get_data(struct ra_calibration *opt, double **values, long *num);
int get_raw_data(struct ra_calibration *opt, double **values, long *num);

int get_calibration(struct ra_calibration *opt, double *values, long num, value_handle *res);

int get_cycles(struct ra_calibration *opt, double *values, long num, struct cycle_info **ci, long *num_cycles);
int get_change_flags(struct ra_calibration *opt, double *data, long num, int *flags);
int get_max_diff(double *data, long n, long diff_lag, double *max_pos, double *max_neg);
int select_cycles(int *flags, long num, struct cycle_info **ci, long *num_cycles);

int check_cycles_step1(struct ra_calibration *opt, struct cycle_info *ci, long num_cycles, double *data, long num);
int check_cycles_step2(struct ra_calibration *opt, struct cycle_info *ci, long num_cycles);

int get_calib_width(struct ra_calibration *opt, double *data, long num, struct cycle_info *ci, long num_cycles);
int acorf(const double *data, long num, double *corr, long max_lag);
int calc_correlation(const double* x, const double* y, long len, double *corr);
int get_periodogram(struct ra_calibration *opt, double *data, long num, double **spec, double **freq, long *n_spec);

int get_results(struct ra_calibration *opt, double *data, long num, struct cycle_info *ci, long num_cycles, value_handle *res);
int select_levels(struct cycle_info *ci, long num_cycles, int *level);
int get_min_max_values(struct ra_calibration *opt, double *min_value, double *max_value);
int qsort_double_compare(const void *v1, const void *v2);

#endif /* CALIBRATION_H */
