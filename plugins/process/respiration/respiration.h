/*----------------------------------------------------------------------------
 * respiration.h
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2003-2007, Raphael Schneider
 * 
 *
 * $Id$
 *--------------------------------------------------------------------------*/


#ifndef RESPIRATION_H
#define RESPIRATION_H

#include <ra_defines.h>
#include <ra_respiration.h>


/* -------------------- defines -------------------- */
#define PLUGIN_VERSION   "0.2.0"
#define PLUGIN_NAME      "respiration"
#define PLUGIN_DESC      gettext_noop("calculates respiration parameters")
#define PLUGIN_TYPE      PLUGIN_PROCESS
#define PLUGIN_LICENSE   LICENSE_LGPL

#define INVALID_VALUE    1000


enum result_idx
{
	START_POS, END_POS, PHASE_POS, PHASE_VALUES, VALUE_CH
};


/* -------------------- structures -------------------- */
struct beat_infos
{
	long index;

	/* position and interval measures in sample units */
	/* values for the rr interval from the ecg */
	long beat_rri_pos;
	long interval_rri_prev;
	long interval_rri_post;
	/* values for the interbeat interval from arterial pressure */
	long beat_ibi_pos;
	long interval_ibi_prev;
	long interval_ibi_post;

	double mean_chest_rri;
	double mean_chest_ibi;
}; /* struct beat_infos */


struct single_phase_info
{
	long start;
	long end;

	long event_id;

	double *phase;
}; /* struct single_phase_info */


struct phase_infos
{
	long num_phase_events;
	struct single_phase_info *spi;

	long num_ch;
	double *pos_scale;
}; /* struct phase_infos*/


/* -------------------- globals -------------------- */
static struct ra_result_infos results[] = {
	{"start_pos", gettext_noop("start-positions [su] of the phase-values"), RA_VALUE_TYPE_LONG_ARRAY, 0},
	{"end_pos", gettext_noop("end-positions [su] of the phase-values"), RA_VALUE_TYPE_LONG_ARRAY, 0},
	{"phase_pos", gettext_noop("reference position for each phase value [su]"), RA_VALUE_TYPE_LONG_ARRAY, 0},
	{"phase_values", gettext_noop("phase-values [-pi..pi]"), RA_VALUE_TYPE_DOUBLE_ARRAY, 0},
	{"value_ch", gettext_noop("channel number for each phase value"), RA_VALUE_TYPE_LONG_ARRAY, 0}
};
long num_results = sizeof(results) / sizeof(results[0]);


static struct ra_option_infos options[] = {
	{"use_eval", gettext_noop("use values from evaluation"), RA_VALUE_TYPE_SHORT, -1, 0},
	{"use_start_end_pos", gettext_noop("use values between start_pos and end_pos"), RA_VALUE_TYPE_SHORT, -1, 0},
	{"start_pos", gettext_noop("start-pos in sample-units"), RA_VALUE_TYPE_LONG, -1, 0},
	{"end_pos", gettext_noop("end-pos in sample-units"), RA_VALUE_TYPE_LONG, -1, 0},

	{"rh", gettext_noop("recording handle"), RA_VALUE_TYPE_LONG, -1, 0},
	{"num_ch", gettext_noop("number of channels set in 'ch'"), RA_VALUE_TYPE_LONG, -1, 0},
	{"ch", gettext_noop("list of respiration channels "), RA_VALUE_TYPE_LONG_ARRAY, -1, -1},

	{"save_in_eval", gettext_noop("flag if results should be saved in an evaluation"), RA_VALUE_TYPE_SHORT, 0, 0},

	{"eh", gettext_noop("eval-handle used to store the results"), RA_VALUE_TYPE_VOIDP, 0, 0},
	{"num_clh", gettext_noop("number of event-class's set in 'clh'"), RA_VALUE_TYPE_LONG, -1, 0},
	{"clh", gettext_noop("event-class-handle(s) used to store the phase information"), RA_VALUE_TYPE_VOIDP_ARRAY, 0, -1},
};
long num_options = sizeof(options) / sizeof(options[0]);


/* infos to insert the results automagically */
static struct ra_auto_create_class create_class[] = {
	{"respiration", START_POS, END_POS, "", "", "", NULL, 0, NULL},
};
long num_create_class = sizeof(create_class) / sizeof(create_class[0]);

static struct ra_auto_create_prop create_prop[] = {
	{"resp-pos", 0, -1, PHASE_POS, -1, NULL, 1},
	{"resp-phase", 0, -1, PHASE_VALUES, VALUE_CH, NULL, 1},
};
long num_create_prop = sizeof(create_prop) / sizeof(create_prop[0]);


/* -------------------- prototypes -------------------- */
int pl_call_gui(proc_handle proc);
proc_handle pl_get_proc_handle(plugin_handle pl);
void pl_free_proc_handle(proc_handle proc);
int pl_do_processing(proc_handle proc);

void set_default_options(struct ra_respiration *opt);
void set_option_offsets(struct ra_option_infos *opt_inf, struct ra_respiration *p);

int find_resp_channel(struct ra_respiration *opt);

int get_phase(struct ra_respiration *opt, long ch_idx, struct phase_infos *phi);
int downsample_data(rec_handle rh, long ch_num, long dec_factor, long *num_data, double **data);
int do_hilbert_transformation(double *resp_data, long num_data, double **re, double **im);

/* int get_data(struct ra_respiration *opt, struct beat_infos **beats, long *num, int *rri_avail, int *ibi_avail); */
/* int get_beat_data(struct ra_respiration *opt, struct beat_infos **beats, long *num, int *rri_avail, int *ibi_avail); */

double get_samplerate(rec_handle rh, long ch);

/* int calc_resp(struct ra_respiration *opt, struct beat_infos *beats, long num, int rri_avail, int ibi_avail); */
/* int calc_beat_resp(struct ra_respiration *opt, struct beat_infos *beats, long num, int rri_avail, int ibi_avail); */
double mean(const double *data, long n);

int store_results(struct proc_info *pi, struct phase_infos *phi);

#endif /* RESPIRATION_H */
