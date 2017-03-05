/*----------------------------------------------------------------------------
 * ap_morphology.c
 *
 * Author(s): Raphael Schneider (librasch@gmail.com)
 *
 * Copyright (C) 2002-2009, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifndef AP_MORPHOLOGY_H
#define AP_MORPHOLOGY_H

#include <ra.h>
#include <ra_ap_morphology.h>

/* -------------------- defines -------------------- */
#define PLUGIN_VERSION  "0.4.0"
#define PLUGIN_NAME     "ap-morphology"
#define PLUGIN_DESC     gettext_noop("get systolic and diastolyc values and positions of aterial pressure waves")
#define PLUGIN_TYPE     PLUGIN_PROCESS
#define PLUGIN_LICENSE  LICENSE_LGPL


#define RAW_BUF_SIZE  100000

enum result_idx
{
	EV_ID, CH, SYST, DIAS, MEAN, SYST_POS, DIAS_POS, FLAGS, IBI,
	DPDT_MIN, DPDT_MAX, DPDT_MIN_POS, DPDT_MAX_POS,
	CALIB_BEGIN, CALIB_END, CALIB_CH, CALIB_INFO
};


/* -------------------- structures -------------------- */
struct morph_struct
{
	/* Flag if positions should NOT be searched. Necessary when beats
	   are inserted then the surrounding positions should not be changed. */
	int dont_search_pos;

	long event_index;
	long event_id;
	long pos;

	double systolic;
	double diastolic;
	double mean;
	long systolic_pos;
	long diastolic_pos;

	double dpdt_min;
	double dpdt_max;
	long dpdt_min_pos;
	long dpdt_max_pos;

	long flags;

	double ibi;
}; /* struct morph_struct */


struct calibration
{
	long start;
	long end;
	long channel;  /* not really needed at the moment but maybe later so let it stay */

	long beat_before;
	long beat_before_idx;
	long pos_before;
	long syst_pos_before;
	long dias_pos_before;
	double syst_before;
	double dias_before;
	double mean_before;
	double ibi_before;
	double dpdt_min_before;
	double dpdt_max_before;
	long dpdt_min_pos_before;
	long dpdt_max_pos_before;

	long beat_after;
	long beat_after_idx;
	long pos_after;
	long syst_pos_after;
	long dias_pos_after;
	double syst_after;
	double dias_after;
	double mean_after;
	double ibi_after;
	double dpdt_min_after;
	double dpdt_max_after;
	long dpdt_min_pos_after;
	long dpdt_max_pos_after;
}; /* struct calibration */


struct calib_info
{
	int num;
	struct calibration *seq;	/* single calibration sequences */
}; /* struct calib_info */


struct ch_data
{
	long ch;

	long num_morph;
	struct morph_struct *morph;

	struct calib_info calib;
}; /* struct ch_data */


/* -------------------- globals -------------------- */
static struct ra_result_infos results[] = {
	{"EV_ID", gettext_noop("event-id's"), RA_VALUE_TYPE_LONG_ARRAY, 0},
	{"CH", gettext_noop("channel-number the tuple of data belongs to"), RA_VALUE_TYPE_LONG_ARRAY, 0},
	{"SYST", gettext_noop("systolic blood pressures"), RA_VALUE_TYPE_DOUBLE_ARRAY, 0},
	{"DIAS", gettext_noop("diastolic blood pressures"), RA_VALUE_TYPE_DOUBLE_ARRAY, 0},
	{"MEAN", gettext_noop("mean blood pressures"), RA_VALUE_TYPE_DOUBLE_ARRAY, 0},
	{"SYST_POS", gettext_noop("positions of systolic measurements"), RA_VALUE_TYPE_LONG_ARRAY, 0},
	{"DIAS_POS", gettext_noop("positions of diastolic measurements"), RA_VALUE_TYPE_LONG_ARRAY, 0},
	{"FLAGS", gettext_noop("flags"), RA_VALUE_TYPE_LONG_ARRAY, 0},
	{"IBI", gettext_noop("inter beat intervals"), RA_VALUE_TYPE_DOUBLE_ARRAY, 0},
	{"CALIB_BEGIN", gettext_noop("begin of the calibration sequences [su]"), RA_VALUE_TYPE_LONG_ARRAY, 0},
	{"CALIB_END", gettext_noop("end of the calibration sequences [su]"), RA_VALUE_TYPE_LONG_ARRAY, 0},
	{"CALIB_CH", gettext_noop("channel-number the calibration sequence belongs to"), RA_VALUE_TYPE_LONG_ARRAY, 0},
	{"CALIB_INFO", gettext_noop("infos about the calibration sequences"), RA_VALUE_TYPE_LONG_ARRAY, 0},
	{"DPDT_MIN", gettext_noop("dp/dt minimum"), RA_VALUE_TYPE_DOUBLE_ARRAY, 0},
	{"DPDT_MAX", gettext_noop("dp/dt maximum"), RA_VALUE_TYPE_DOUBLE_ARRAY, 0},
	{"DPDT_MIN_POS", gettext_noop("positions of dp/dt min"), RA_VALUE_TYPE_LONG_ARRAY, 0},
	{"DPDT_MAX_POS", gettext_noop("positions of dp/dt max"), RA_VALUE_TYPE_LONG_ARRAY, 0},	
};
long num_results = sizeof(results) / sizeof(results[0]);

static struct ra_option_infos options[] = {
	{"eh", gettext_noop("evaluation used; if '= NULL' use default evaluation"), RA_VALUE_TYPE_VOIDP, -1, 0},
	{"use_class", gettext_noop("use 'heartbeat' event-class stored in option 'clh'"), RA_VALUE_TYPE_SHORT, -1, 0},
	{"clh", gettext_noop("handle to the event-class holding the events which should be processed"), RA_VALUE_TYPE_VOIDP, -1, 0},
	{"clh_calib", gettext_noop("handle to the event-class holding the calibration sequences"), RA_VALUE_TYPE_VOIDP, -1, 0},
	{"save_in_class", gettext_noop("flag if the results should be saved in the event-class"), RA_VALUE_TYPE_SHORT, -1, 0},

	{"num_ch", gettext_noop("number of channels to process"), RA_VALUE_TYPE_LONG, -1, -1},
	{"ch", gettext_noop("channel(s) of the aterial pressure signal; if none was given, "
			    "all arterial pressure channels will be processed"), RA_VALUE_TYPE_LONG_ARRAY, -1, -1},

	{"num_pos", gettext_noop("number of positions where rr values and positions should be get"),
	 RA_VALUE_TYPE_LONG, -1, 0},
	{"pos", gettext_noop("positions"), RA_VALUE_TYPE_LONG_ARRAY, -1, -1},

	{"pos_are_region", gettext_noop("flag if positions are start and end of a region; if set, num_pos must be 2"),
	 RA_VALUE_TYPE_SHORT, -1, 0},
	{"pos_are_event_id", gettext_noop("flag if positions are id's of events in eh"),
	 RA_VALUE_TYPE_SHORT, -1, 0},
	{"rr_pos", gettext_noop("positions are bloodpressure peaks (= 1) or qrs-complexes (= 0); "
				"don't be used if postions are start-/end-region"), RA_VALUE_TYPE_SHORT, -1, 0},

	{"dont_search_calibration", gettext_noop("if set (=1) -> don't look for sequences of calibration "
						 "in bloodpressure measurements"), RA_VALUE_TYPE_SHORT, -1, 0},

	{"dont_search_positions", gettext_noop("if set (=1) -> do not search the positions of systolic- and diastolic "
	 "blood pressure but use the previously selected positions (needed to re-calc the ap related measures)"),
	 RA_VALUE_TYPE_SHORT, -1, 0},
};
long num_options = sizeof(options) / sizeof(options[0]);


static struct ra_auto_create_class create_class[] = {
	{"heartbeat", DIAS_POS, DIAS_POS, "", "", "", NULL, 0, NULL},
	{"rr-calibration", CALIB_BEGIN, CALIB_END, "", "", "", NULL, 0, NULL},
};
long num_create_class = sizeof(create_class) / sizeof(create_class[0]);

static struct ra_auto_create_prop create_prop[] = {
	{"rr-systolic", 0, EV_ID, SYST, CH, NULL, 1},
	{"rr-diastolic", 0, EV_ID, DIAS, CH, NULL, 1},
	{"rr-mean", 0, EV_ID, MEAN, CH, NULL, 1},
	{"rr-systolic-pos", 0, EV_ID, SYST_POS, CH, NULL, 1},
	{"rr-diastolic-pos", 0, EV_ID, DIAS_POS, CH, NULL, 1},
	{"rr-flags", 0, EV_ID, FLAGS, CH, NULL, 1},
	{"ibi", 0, EV_ID, IBI, CH, NULL, 1},
	{"rr-calibration-info", 1, -1, CALIB_INFO, CALIB_CH, NULL, 1},
	{"rr-dpdt-min", 0, EV_ID, DPDT_MIN, CH, NULL, 1},
	{"rr-dpdt-max", 0, EV_ID, DPDT_MAX, CH, NULL, 1},
	{"rr-dpdt-min-pos", 0, EV_ID, DPDT_MIN_POS, CH, NULL, 1},
	{"rr-dpdt-max-pos", 0, EV_ID, DPDT_MAX_POS, CH, NULL, 1},
};
long num_create_prop = sizeof(create_prop) / sizeof(create_prop[0]);


/* -------------------- prototypes -------------------- */
int pl_call_gui(proc_handle proc);
proc_handle pl_get_proc_handle(plugin_handle pl);
void set_default_options(struct ra_ap_morphology *opt);
void set_option_offsets(struct ra_option_infos *opt_inf, struct ra_ap_morphology *opt);
void pl_free_proc_handle(proc_handle proc);
int pl_do_processing(proc_handle proc);

long init_ch_data(struct ra_ap_morphology *opt, rec_handle rh, struct ch_data **ch);
class_handle get_event_class(struct proc_info *pi);
long get_data(struct ra_ap_morphology *opt, long ch, struct morph_struct **morph);
long get_all_data_from_eval(class_handle clh, long ch, struct morph_struct **morph);
long get_specific_data_from_eval(class_handle clh, long ch, struct morph_struct **morph,
				 long *pos, long num_pos);
void get_beat_data(class_handle clh, long ch, struct morph_struct *morph, long index);
long get_beats_in_region(class_handle clh, long ch, struct morph_struct **morph, long *event_pos, long num_pos);
long add_beat(class_handle clh, long ch, struct morph_struct **morph, long num, long curr_event);

int morphology_rr(rec_handle rh, long rr_channel, long num, struct morph_struct *morph,
		  int rr_pos, int dont_search_pos, void (*callback) (const char *, int));

prop_handle get_pos_property(class_handle clh, int *rr_pos);
double get_samplerate(rec_handle rh, int ch);
void process_beat(rec_handle rh, struct morph_struct *morph,
			 long num, long curr, long pos_offset, int ch, double samplerate);
int get_rr_channel(rec_handle rh, int prev_rr_ch);


long find_diastole(double *buf, unsigned long size, double *min_value, double samplerate,
				  long curr_ibi);
long find_systole(double *buf, unsigned long size, double *max_value);
double calc_mean_pressure(rec_handle rec, int ch, long start, long end);

int find_dpdt_values(double *buf, unsigned long size, double samplerate,
					 double *min_value, double *max_value, long *min_pos, long *max_pos);

int find_calibration(rec_handle rh, long rr_channel, struct calib_info *calib,
		     void (*callback) (const char *, int));
int add_calibration(struct calib_info *calib, long start, long end, long channel, double samplerate);

int complete_calib_infos(struct calib_info *calib, struct morph_struct *morph, long num);
long find_beat(struct morph_struct *morph, long num, long last_index, long pos, int after_pos);

int get_bp_values_calibration(rec_handle rh, int rr_channel, struct calib_info *calib,
			      struct morph_struct *morph, long num, void (*callback) (const char *, int));
long get_calib_seq(long pos, struct calib_info *calib, long last_idx);

int read_calibration(struct ra_ap_morphology *opt, class_handle clh, long ch, struct calib_info *calib);
long find_beat_eval(prop_handle syst_pos_prop, prop_handle beat_pos_prop, long ch, const long *ev_ids,
		    long num, long last_index, long pos, int after_pos);

int save_in_res(struct proc_info *pi, struct ch_data *ch, long num_ch, double x_scale);
int save_single(struct proc_info *pi, struct ch_data *ch, long num_ch, double x_scale);

#endif /* AP_MORPHOLOGY_H */
