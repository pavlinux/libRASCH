/*----------------------------------------------------------------------------
 * freq_analysis.h
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2003-2005, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/


#ifndef FREQ_ANALYSIS_H
#define FREQ_ANALYSIS_H

#include <ra_defines.h>
#include <ra_freq_analysis.h>


/* -------------------- defines -------------------- */
#define PLUGIN_VERSION   "0.3.0"
#define PLUGIN_NAME      "freq-analysis"
#define PLUGIN_DESC      gettext_noop("perform frequency analysis")
#define PLUGIN_TYPE      PLUGIN_PROCESS
#define PLUGIN_LICENSE   LICENSE_LGPL


enum result_idx
{
	FREQ_AXIS,
	PSD,
	REAL_PART,
	IMG_PART
};


/* -------------------- structures -------------------- */
struct event_infos
{
	long id;
	double pos;
	double value;

	int dont_use_event;
}; /* struct event_infos */


/* -------------------- globals -------------------- */
static struct ra_result_infos results[] = {
	{"FREQ_AXIS", gettext_noop("frequency values (the x-axis)"), RA_VALUE_TYPE_DOUBLE_ARRAY, 0},
	{"PSD", gettext_noop("Power Spectrum Density"), RA_VALUE_TYPE_DOUBLE_ARRAY, 0},
	{"REAL_PART", gettext_noop("real part of the frequency spectrum"), RA_VALUE_TYPE_DOUBLE_ARRAY, 0},
	{"IMG_PART", gettext_noop("imaginary part of the frequency spectrum"), RA_VALUE_TYPE_DOUBLE_ARRAY, 0},
};
long num_results = sizeof(results) / sizeof(results[0]);

static struct ra_option_infos options[] = {	
	/* options when event-properties should be processed */
	{"use_events", gettext_noop("!=0: use events, ==0: use signal raw data"), RA_VALUE_TYPE_SHORT, -1, 0},
	{"clh", gettext_noop("event class of the events"), RA_VALUE_TYPE_VOIDP, -1, 0},
	{"prop_value", gettext_noop("use these event-property for frequency analysis"), RA_VALUE_TYPE_CHAR, -1, 0},
	{"use_event_pos", gettext_noop("flag if positions should be used from the events (=1) or from 'prop_pos' (=0)"),
	 RA_VALUE_TYPE_SHORT, -1, 0},
	{"prop_pos", gettext_noop("when using events, the position of the events is needed "
					 "for time based analysis"), RA_VALUE_TYPE_CHAR, -1, 0},
	{"use_time", gettext_noop("flag if freq. analysis should be done based on time(!=0) "
				  "or based on the event number (=0) (e.g. RR-interval)"), RA_VALUE_TYPE_SHORT, -1, 0},

	{"num_events", gettext_noop("use the events listed in events"), RA_VALUE_TYPE_LONG, -1, 0},
	{"events", gettext_noop("list of events to use"), RA_VALUE_TYPE_LONG_ARRAY, -1, -1},

	{"num_ignore_events", gettext_noop("number of events which should be ignored"), RA_VALUE_TYPE_LONG, -1, 0},
	{"ignore_events", gettext_noop("list of events to ignore"), RA_VALUE_TYPE_LONG_ARRAY, -1, -1},

	/* options when raw signal data should be processed */
	{"rh", gettext_noop("recording handle"), RA_VALUE_TYPE_VOIDP, -1, 0},
	{"ch", gettext_noop("channel which should be used"), RA_VALUE_TYPE_LONG, -1, 0},
	{"use_ignore_value", gettext_noop("flag if there are values which should not "
					  "be used for freq. analysis"), RA_VALUE_TYPE_SHORT, -1, 0},
	{"ignore_value", gettext_noop("this value will be not used for freq. analysis "
				      "(if use_ignore_value_1 is != 0)"), RA_VALUE_TYPE_DOUBLE, -1, 0},

	/* options for event-based and raw-based data */
	{"use_start_end_pos", gettext_noop("use raw-data/events between start_pos and end_pos"), RA_VALUE_TYPE_SHORT, -1, 0},
	{"start_pos", gettext_noop("start-pos in sample-units"), RA_VALUE_TYPE_LONG, -1, 0},
	{"end_pos", gettext_noop("end-pos in sample-units"), RA_VALUE_TYPE_LONG, -1, 0},
	
	/* options when data is given directly to the plugin (has precedence of the other options (events or raw-data) */
	{"num_values", gettext_noop("number of values given in 'value'"), RA_VALUE_TYPE_LONG, -1, 0},
	{"values", gettext_noop("perform frequency analysis using this data"), RA_VALUE_TYPE_DOUBLE_ARRAY, -1, -1},
	{"pos", gettext_noop("positions of the values given in 'values'"), RA_VALUE_TYPE_DOUBLE_ARRAY, -1, -2},
	{"pos_samplerate", gettext_noop("samplerate of the position values"), RA_VALUE_TYPE_DOUBLE, -1, 0},

	/* options independent of source type (event/raw/data) */
	{"samplerate", gettext_noop("samplerate to use when freq.analysis should be done time based"), RA_VALUE_TYPE_DOUBLE, -1, 0},

	/* pre-processing options */
	{"smooth_data", gettext_noop("flag if input-data should be smoothed"), RA_VALUE_TYPE_SHORT, -1, 0},
	{"smooth_method", gettext_noop("at the moment only a boxcar-filter is available"), RA_VALUE_TYPE_CHAR, -1, 0},
	{"smooth_width", gettext_noop("width of the smooth-filter"), RA_VALUE_TYPE_DOUBLE, -1, 0},

	/* frequency analysis specifc options */
	{"window", gettext_noop("window function used"), RA_VALUE_TYPE_CHAR, -1, 0},
	{"remove_mean", gettext_noop("remove the mean value before the frequency analysis"), RA_VALUE_TYPE_SHORT, -1, 0},
	{"remove_trend", gettext_noop("remove the trend (first order) before the frequency analysis"), RA_VALUE_TYPE_SHORT, -1, 0},
	{"method", gettext_noop("method used for the frequency analysis"), RA_VALUE_TYPE_CHAR, -1, 0},
	{"num_freq", gettext_noop("number of frequency components"), RA_VALUE_TYPE_LONG, -1, 0},

	/* post-processing options */
	{"smooth_spec", gettext_noop("flag if spectrum should be smoothed"), RA_VALUE_TYPE_SHORT, -1, 0},

	{"save_in_eval", gettext_noop("save results in the evaluation file (not supported yet)"), RA_VALUE_TYPE_SHORT, -1, 0}
};
long num_options = sizeof(options) / sizeof(options[0]);


/* -------------------- prototypes -------------------- */
int pl_call_gui(proc_handle proc);
proc_handle pl_get_proc_handle(plugin_handle pl);
void pl_free_proc_handle(proc_handle proc);
int pl_do_processing(proc_handle proc);

void set_default_options(struct ra_freq_analysis *opt);
void set_option_offsets(struct ra_option_infos *opt_inf, struct ra_freq_analysis *opt);

double get_samplerate(any_handle h);

int get_data(meas_handle mh, struct ra_freq_analysis *opt, double **values, double **pos, long *num);
int get_event_data(struct ra_freq_analysis *opt, double **values, double **pos, long *num);
int get_events(struct ra_freq_analysis *opt, struct event_infos **ev, long *num);
int get_event_list(struct ra_freq_analysis *opt, long **list, long *num);
int get_raw_data(struct ra_freq_analysis *opt, double **values, double **pos, long *num);
int interpolate_values(double **value, double **pos, long *num, double samplerate);

int pre_process_data(struct ra_freq_analysis *opt, double **values, long *num);

int do_freq_analysis(struct ra_freq_analysis *opt, double *values, long num, value_handle *res);

int post_process_spec(struct ra_freq_analysis *opt, value_handle *res);

int save(proc_handle proc, eval_handle eh);

#endif /* FREQ_ANALYSIS_H */
