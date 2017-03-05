/*----------------------------------------------------------------------------
 * ecg.h
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002-2007, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/


#ifndef _ECG_H
#define _ECG_H

/* -------------------- defines -------------------- */
#define PLUGIN_VERSION  "0.5.0"
#define PLUGIN_NAME     "ecg"
#define PLUGIN_DESC     gettext_noop("perform ecg specific actions after reading original evaluation or doing detect using libRASCH")
#define PLUGIN_TYPE     PLUGIN_PROCESS
#define PLUGIN_LICENSE  LICENSE_LGPL

#define INVALID_OFFSET_VALUE  32767


enum result_idx
{
	QRS_TEMPORAL, QRS_ANNOT, ECG_FLAGS, RRI, RRI_ANNOT, RRI_REFVALUE, RRI_NUM_REFVALUE, IDX_RRI,
	
	/* morphology values */
	P_WIDTH, QRS_WIDTH, T_WIDTH, PQ, QT, QTC, QTA,

	/* respiration related values */
	RESP_PHASE, RESP_CH,

	/* beats */
	NUM_ALL,

	NUM_UNKNOWN, IDX_UNKNOWN,

	NUM_SINUS, IDX_SINUS,
	NUM_NORMAL, IDX_NORMAL,
	NUM_SVPC, IDX_SVPC,
	NUM_SINUS_VPC, IDX_SINUS_VPC,
	NUM_PAUSE, IDX_PAUSE,

	NUM_VENT, IDX_VENT,
	NUM_VENT_SINGLE, IDX_VENT_SINGLE,
	NUM_VENT_SINGLE_PREM, IDX_VENT_SINGLE_PREM,
	NUM_VENT_SINGLE_PREM_COMP, IDX_VENT_SINGLE_PREM_COMP,
	NUM_INTERP, IDX_INTERP,
	NUM_ESCAPE, IDX_ESCAPE,

	NUM_PACED_BEATS, IDX_PACED_BEATS,

	NUM_ARTIFACT, IDX_ARTIFACT,
	NUM_ARTIFACT_TYPE1, IDX_ARTIFACT_TYPE1,
	NUM_ARTIFACT_TYPE2, IDX_ARTIFACT_TYPE2,
	NUM_ARTIFACT_TYPE3, IDX_ARTIFACT_TYPE3,

	/* arrhythmias */
	MIN_HR, IDX_MIN_HR,
	MAX_HR, IDX_MAX_HR,
	MEAN_HR,

	NUM_BRADY, IDX_BRADY, LEN_BRADY, HR_BRADY,
	NUM_TACHY, IDX_TACHY, LEN_TACHY, HR_TACHY,

	NUM_SSALVO, IDX_SSALVO, LEN_SSALVO, HR_SSALVO,
	NUM_SVT, IDX_SVT, LEN_SVT, HR_SVT,
	NUM_SSALVO_SVT, IDX_SSALVO_SVT, LEN_SSALVO_SVT, HR_SSALVO_SVT,

	NUM_IVR, IDX_IVR, LEN_IVR, HR_IVR,
	NUM_COUPLET, IDX_COUPLET,
	NUM_BIGEMINY, IDX_BIGEMINY, LEN_BIGEMINY,
	NUM_TRIGEMINY, IDX_TRIGEMINY, LEN_TRIGEMINY,

	NUM_SALVO, IDX_SALVO, LEN_SALVO, HR_SALVO,
	NUM_VT, IDX_VT, LEN_VT, HR_VT,
	NUM_SALVO_VT, IDX_SALVO_VT, LEN_SALVO_VT, HR_SALVO_VT
};


/* defines for arrhythmia calculations */
#define HR_LENGTH_SECONDS      60
#define MIN_BEATS_SSALVO        3
#define TACHY_HR              100
#define MIN_BEATS_SALVO         3
#define VT_HR                 100
#define MIN_LENGTH_IVR          3
#define MIN_LENGTH_BIGEMINY     2
#define MIN_LENGTH_TRIGEMINY    2



/* -------------------- structures -------------------- */
struct beat_infos
{
	long pos;
	long beat_class;
	long event_id;
	long event_pos;		/* THIS is the reference for OFFSET values */

	int rri;
	double rri_double;
	long rri_class;

	int rri_ref;		/* ref-value for this beat (-1: none) */
	int num_ref_rri;	/* number of rri for calc ref-value */
	int temporal_group;	/* see defines in ra_ecg.h */

	long *morph_flags;
	long *p_start, *p_end;
	long *qrs_start, *qrs_end;
	long *t_start, *t_end;

	/* morphology values for each single channel */
	double *p_width, *qrs_width, *t_width;
	double *pq, *qt, *qtc, *qta;
	
	/* morphology values independent of the channel; as the start-/end-position
	   the earliest/latest position in all channels is used */
	double p_width_all, qrs_width_all, t_width_all;
	double pq_all, qt_all, qtc_all, qta_all;

	long flags;	/* flag if beat was edited by user -> no automatic
			   classification changes allowed */
	
	int num_resp_ch;
	long *resp_ch;
	double *resp_phase;
}; /* struct beat_infos */


struct ref_value_infos
{
	long last_beatpos_ok;
	double *buf;
	int curr_buf_pos;
	int num_buffer_rri_ok;

	int last_ref_value;

	/* store pointer to opt here so function-call is simpler */
	struct ra_ecg_options *opt;
	double samplerate;
}; /* struct ref_value_infos */


struct arr_infos
{
	long start;
	long length;
	double hr;
}; /* struct arr_infos */


/* -------------------- globals -------------------- */
static struct ra_option_infos options[] = {
	{"eh", gettext_noop("evaluation used; if '= NULL' use default evaluation"), RA_VALUE_TYPE_VOIDP, -1, 0},
	{"rh", gettext_noop("recording handle"), RA_VALUE_TYPE_VOIDP, -1, 0},

	{"use_class", gettext_noop("use 'heartbeat' event-class stored in option 'clh'"), RA_VALUE_TYPE_SHORT, -1, 0},
	{"clh", gettext_noop("handle to the event-class holding the events which should be processed"), RA_VALUE_TYPE_VOIDP, -1, 0},

	{"use_start_end_pos", gettext_noop("use events between start_pos and end_pos"), RA_VALUE_TYPE_LONG, -1, 0},
	{"start_pos", gettext_noop("start-pos in sample-units"), RA_VALUE_TYPE_LONG, -1, 0},
	{"end_pos", gettext_noop("end-pos in sample-units"), RA_VALUE_TYPE_LONG, -1, 0},

	{"num_ignore_events", gettext_noop("number of events which should be ignored"), RA_VALUE_TYPE_LONG, -1, 0},
	{"ignore_events", gettext_noop("list of events to ignore"), RA_VALUE_TYPE_LONG_ARRAY, -1, -1},

	{"save_in_class", gettext_noop("flag if the results should be saved in the event-class"), RA_VALUE_TYPE_SHORT, -1, 0},

	{"num_data", gettext_noop("number of heartbeats"), RA_VALUE_TYPE_LONG, -1, 0},
	{"data", gettext_noop("positions of the heartbeats [su]"), RA_VALUE_TYPE_LONG_ARRAY, -1, -1},
	{"annot", gettext_noop("annotations of the heartbeats"), RA_VALUE_TYPE_LONG_ARRAY, -1, -1},
	{"flags", gettext_noop("ecg related flags"), RA_VALUE_TYPE_LONG_ARRAY, -1, -1},
	{"data_is_pos", gettext_noop("flag if values in 'data' are event-id's (=0) or sample positions (=1)"),
	 RA_VALUE_TYPE_LONG, -1, 0},

	{"min_beat_distance", gettext_noop("minimum distance of heartbeats [msec]; below this distance, beats will be combined"),
	 RA_VALUE_TYPE_LONG, -1, 0},

	{"num_ref_rri", gettext_noop("number rri used for calculation of reference-value"), RA_VALUE_TYPE_LONG, -1, 0},
	{"ref_normal_percent", gettext_noop("percent within the previous rri will be uses as reference-value"),
	 RA_VALUE_TYPE_LONG, -1, 0},
	{"normal_percent", gettext_noop("max. tolerance for being a normal rri in percent"), RA_VALUE_TYPE_LONG, -1, 0},
	{"normal_pre_percent", "", RA_VALUE_TYPE_LONG, -1, 0},
	{"normal_late_percent", "", RA_VALUE_TYPE_LONG, -1, 0},
	{"max_abs_difference", "", RA_VALUE_TYPE_LONG, -1, 0},
	{"vpc_pre_percent", gettext_noop("max. percent of ref-value to be premature vpc"), RA_VALUE_TYPE_LONG, -1, 0},
	{"vpc_late_percent", gettext_noop("min. percent of ref-value to be a 'real' compensatory pause"), RA_VALUE_TYPE_LONG, -1, 0},
	{"min_interp_vpc_percent", "", RA_VALUE_TYPE_LONG, -1, 0},
	{"max_interp_vpc_percent", "", RA_VALUE_TYPE_LONG, -1, 0},
	{"time2get_new_ref_value", gettext_noop("value in sec; if in the last time2get.. sec no normal"
	 "rri show up, a new ref-value must be calculated"), RA_VALUE_TYPE_LONG, -1, 0},
	{"min_rri", gettext_noop("minimum value of a valid rr-interval"), RA_VALUE_TYPE_LONG, -1, 0},
	{"max_rri", gettext_noop("maximum value of a valid rr-interval (only used for heart rate calculations)"),
	 RA_VALUE_TYPE_LONG, -1, 0},
	{"pause", gettext_noop("rri at which a pause begins"), RA_VALUE_TYPE_LONG, -1, 0},
	{"do_summarize", gettext_noop("flag if a summary of beats and arrhythmias should be created and stored in the results"),
	 RA_VALUE_TYPE_SHORT, -1, 0},
	{"get_respiration_values", gettext_noop("flag if respiration related values have to be assessed"),
	 RA_VALUE_TYPE_SHORT, -1, 0},
	{"skip_artifact_detection", gettext_noop("flag if own artifact detection should be skipped"),
	 RA_VALUE_TYPE_SHORT, -1, 0},
};
long num_options = sizeof(options) / sizeof(options[0]);


static struct ra_result_infos results[] = {
	{"QRS_TEMPORAL", gettext_noop("temporal setting of beat"), RA_VALUE_TYPE_SHORT_ARRAY, 0},
	{"QRS_ANNOT", gettext_noop("annotation of QRS complex"), RA_VALUE_TYPE_LONG_ARRAY, 0},
	{"ECG_FLAGS", gettext_noop("ecg flags"), RA_VALUE_TYPE_LONG_ARRAY, 0},
	{"RRI", gettext_noop("RR interval"), RA_VALUE_TYPE_DOUBLE_ARRAY, 0},
	{"RRI_ANNOT", gettext_noop("annoation of RR interval"), RA_VALUE_TYPE_LONG_ARRAY, 0},
	{"RRI_REFVALUE", gettext_noop("reference rri representing the current heart-rate"), RA_VALUE_TYPE_DOUBLE_ARRAY, 0},
	{"RRI_NUM_REFVALUE", gettext_noop("number of rri's used for calculation of reference value"), RA_VALUE_TYPE_SHORT_ARRAY, 0},
	{"IDX_RRI", gettext_noop("event indeces the RR intervals belongs to"), RA_VALUE_TYPE_LONG_ARRAY, 0},

	/* overall morphology values which should be used for further analysis
	   (one value for each event) 
	   TODO: think about if this is really needed and if the current scheme makes sense */
	{"p_width", gettext_noop(""), RA_VALUE_TYPE_DOUBLE_ARRAY, 0},
	{"qrs_width", gettext_noop(""), RA_VALUE_TYPE_DOUBLE_ARRAY, 0},
	{"t_width", gettext_noop(""), RA_VALUE_TYPE_DOUBLE_ARRAY, 0},
	{"pq", gettext_noop(""), RA_VALUE_TYPE_DOUBLE_ARRAY, 0},
	{"qt", gettext_noop(""), RA_VALUE_TYPE_DOUBLE_ARRAY, 0},
	{"qtc", gettext_noop(""), RA_VALUE_TYPE_DOUBLE_ARRAY, 0},
	{"qta", gettext_noop(""), RA_VALUE_TYPE_DOUBLE_ARRAY, 0},

	/* respiration related values (respiration channel specific) */
	{"resp_phase", gettext_noop(""), RA_VALUE_TYPE_DOUBLE_ARRAY, 0},
	{"resp_ch", gettext_noop(""), RA_VALUE_TYPE_LONG_ARRAY, 0},

	/* beats */
	{"NUM_ALL", gettext_noop("number of all QRS complexes"), RA_VALUE_TYPE_LONG, 1},

	{"NUM_UNKNOWN", gettext_noop("number of un-classified QRS complexes"), RA_VALUE_TYPE_LONG, 1},
	{"IDX_UNKNOWN", gettext_noop("event indeces of un-classified QRS complexes"), RA_VALUE_TYPE_LONG_ARRAY, 0},

	{"NUM_SINUS", gettext_noop("number of sinus beats"), RA_VALUE_TYPE_LONG, 1},
	{"IDX_SINUS", gettext_noop("event indeces of sinus beats"), RA_VALUE_TYPE_LONG_ARRAY, 0},
	{"NUM_NORMAL", gettext_noop("number of normal sinus-beats"), RA_VALUE_TYPE_LONG, 1},
	{"IDX_NORMAL", gettext_noop("event indeces of normal sinus-beats"), RA_VALUE_TYPE_LONG_ARRAY, 0},
	{"NUM_SVPC", gettext_noop("number of SVPCs"), RA_VALUE_TYPE_LONG, 1},
	{"IDX_SVPC", gettext_noop("event indeces of SVPCs"), RA_VALUE_TYPE_LONG_ARRAY, 0},
	{"NUM_SINUS_VPC", gettext_noop("number of premature sinus-beats which following beats come late (perhaps VENT?)"),
	 RA_VALUE_TYPE_LONG, 1},
	{"IDX_SINUS_VPC", gettext_noop("event indeces of premature sinus-beats which following beats come late (perhaps VENT?)"),
	 RA_VALUE_TYPE_LONG_ARRAY, 0},
	{"NUM_PAUSE", gettext_noop("number of pause"), RA_VALUE_TYPE_LONG, 1},
	{"IDX_PAUSE", gettext_noop("event indeces of pause"), RA_VALUE_TYPE_LONG_ARRAY, 0},

	{"NUM_VENT", gettext_noop("number of VPCs"), RA_VALUE_TYPE_LONG, 1},
	{"IDX_VENT", gettext_noop("event indeces of VPCs"), RA_VALUE_TYPE_LONG_ARRAY, 0},
	{"NUM_VENT_SINGLE", gettext_noop("number of single VPCs"), RA_VALUE_TYPE_LONG, 1},
	{"IDX_VENT_SINGLE", gettext_noop("event indeces of single VPCs"), RA_VALUE_TYPE_LONG_ARRAY, 0},
	{"NUM_VENT_SINGLE_PREM", gettext_noop("number of VPCs which are premature"), RA_VALUE_TYPE_LONG, 1},
	{"IDX_VENT_SINGLE_PREM", gettext_noop("event indeces of VPCs which are premature"), RA_VALUE_TYPE_LONG_ARRAY, 0},
	{"NUM_VENT_SINGLE_PREM_COMP", gettext_noop("number of VPCs which are premature and have a compensatory pause"),
	 RA_VALUE_TYPE_LONG, 1},
	{"IDX_VENT_SINGLE_PREM_COMP", gettext_noop("event indeces of VPCs which are premature and have a compensatory pause"),
	 RA_VALUE_TYPE_LONG_ARRAY, 0},
	{"NUM_INTERP", gettext_noop("number of interpolated VPCs"), RA_VALUE_TYPE_LONG, 1},
	{"IDX_INTERP", gettext_noop("event indeces of interpolated VPCs"), RA_VALUE_TYPE_LONG_ARRAY, 0},
	{"NUM_ESACPE", gettext_noop("number of escape beats"), RA_VALUE_TYPE_LONG, 1},
	{"IDX_ESCAPE", gettext_noop("event indeces of escape beats"), RA_VALUE_TYPE_LONG_ARRAY, 0},

	{"NUM_PACED", gettext_noop("number of paced beats"), RA_VALUE_TYPE_LONG, 1},
	{"IDX_PACED", gettext_noop("event indeces of paced beats"), RA_VALUE_TYPE_LONG_ARRAY, 0},

	{"NUM_ARTIFACT", gettext_noop("number of artifacts"), RA_VALUE_TYPE_LONG, 1},
	{"IDX_ARTIFACT", gettext_noop("event indeces of artifacts"), RA_VALUE_TYPE_LONG_ARRAY, 0},
	{"NUM_ARTIFACT_TYPE1", gettext_noop("number of automatic detected artifacts: "
					    "rri is lower than the minimal allowed rri"), RA_VALUE_TYPE_LONG, 1},
	{"IDX_ARTIFACT_TYPE1", gettext_noop("event indeces of automatic detected artifacts: "
					    "rri is lower than the minimal allowed rri"), RA_VALUE_TYPE_LONG_ARRAY, 0},
	{"NUM_ARTIFACT_TYPE2", gettext_noop("number of automatic detected artifacts: perhaps detected T-wave"),
	 RA_VALUE_TYPE_LONG, 1},
	{"IDX_ARTIFACT_TYPE2", gettext_noop("event indeces of automatic detected artifacts: perhaps detected T-wave"),
	 RA_VALUE_TYPE_LONG_ARRAY, 0},
	{"NUM_ARTIFACT_TYPE3", gettext_noop("number of automatic detected artifacts: perhaps overlooked beat after a VPC"),
	 RA_VALUE_TYPE_LONG, 1},
	{"IDX_ARTIFACT_TYPE3", gettext_noop("event indeces of automatic detected artifacts: perhaps overlooked beat after a VPC"),
	 RA_VALUE_TYPE_LONG_ARRAY, 0},

	/* arrhythmias */
	{"MIN_HR", gettext_noop("min. heart rate"), RA_VALUE_TYPE_DOUBLE, 1},
	{"IDX_MIN_HR", gettext_noop("start event index of min. heart rate"), RA_VALUE_TYPE_LONG, 0},
	{"MAX_HR", gettext_noop("max. heart rate"), RA_VALUE_TYPE_DOUBLE, 1},
	{"IDX_MAX_HR", gettext_noop("start event index of max. heart rate"), RA_VALUE_TYPE_LONG, 0},

	{"MEAN_HR", gettext_noop("mean heart rate"), RA_VALUE_TYPE_DOUBLE, 1},

	{"NUM_BRADY", gettext_noop("number of bradycardia"), RA_VALUE_TYPE_LONG, 1},
	{"IDX_BRADY", gettext_noop("start event indeces of the bradicardia"), RA_VALUE_TYPE_LONG_ARRAY, 0},
	{"LEN_BRADY", gettext_noop("length (#rri's) of the bradicardia"), RA_VALUE_TYPE_LONG_ARRAY, 0},
	{"HR_BRADY", gettext_noop("heart rate of the bradicardia"), RA_VALUE_TYPE_DOUBLE_ARRAY, 0},

	{"NUM_TACHY", gettext_noop("number of tachycardia"), RA_VALUE_TYPE_LONG, 1},
	{"IDX_TACHY", gettext_noop("start event indeces of the tachycardia"), RA_VALUE_TYPE_LONG_ARRAY, 0},
	{"LEN_TACHY", gettext_noop("length (#rri's) of the tachycardia"), RA_VALUE_TYPE_LONG_ARRAY, 0},
	{"HR_TACHY", gettext_noop("heart rate of the tachycardia"), RA_VALUE_TYPE_DOUBLE_ARRAY, 0},

	{"NUM_SSALVO", gettext_noop("number of supraventricular salvos"), RA_VALUE_TYPE_LONG, 1},
	{"IDX_SSALVO", gettext_noop("start event indeces of supraventricular salvos"), RA_VALUE_TYPE_LONG_ARRAY, 0},
	{"LEN_SSALVO", gettext_noop("length of supraventricular salvos"), RA_VALUE_TYPE_LONG, 0},
	{"HR_SSALVO", gettext_noop("heart rate of supraventricular salvos"), RA_VALUE_TYPE_DOUBLE, 0},

	{"NUM_SVT", gettext_noop("number of supraventricular tachycardia"), RA_VALUE_TYPE_LONG, 1},
	{"IDX_SVT", gettext_noop("start event indeces of supraventricular tachycardia"), RA_VALUE_TYPE_LONG_ARRAY, 0},
	{"LEN_SVT", gettext_noop("length of supraventricular tachycardia"), RA_VALUE_TYPE_LONG, 0},
	{"HR_SVT", gettext_noop("heart rate of supraventricular tachycardia"), RA_VALUE_TYPE_DOUBLE, 0},

	{"NUM_SSALVO_SVT", gettext_noop("number of supraventricular salvos/SVT's"), RA_VALUE_TYPE_LONG, 1},
	{"IDX_SSALVO_SVT", gettext_noop("start event indeces of supraventricular salvos/SVT's"), RA_VALUE_TYPE_LONG_ARRAY, 0},
	{"LEN_SSALVO_SVT", gettext_noop("length of supraventricular salvos/SVT's"), RA_VALUE_TYPE_LONG, 0},
	{"HR_SSALVO_SVT", gettext_noop("heart rate of supraventricular salvos/SVT's"), RA_VALUE_TYPE_DOUBLE, 0},

	{"NUM_IVR", gettext_noop("number of ideoventricular rhythms"), RA_VALUE_TYPE_LONG, 1},
	{"IDX_IVR", gettext_noop("start event indeces of ideoventricular rhythms"), RA_VALUE_TYPE_LONG_ARRAY, 0},
	{"LEN_IVR", gettext_noop("length (#rri's) of ideoventricular rhythms"), RA_VALUE_TYPE_LONG_ARRAY, 0},
	{"HR_IVR", gettext_noop("heart rate of ideoventricular rhythms"), RA_VALUE_TYPE_DOUBLE_ARRAY, 0},

	{"NUM_COUPLET", gettext_noop("number of couplets"), RA_VALUE_TYPE_LONG, 1},
	{"IDX_COUPLET", gettext_noop("event indeces of couplets"), RA_VALUE_TYPE_LONG_ARRAY, 0},

	{"NUM_BIGEMINY", gettext_noop("number of bigeminy"), RA_VALUE_TYPE_LONG, 1},
	{"IDX_BIGEMINY", gettext_noop("start event indeces of the bigeminy"), RA_VALUE_TYPE_LONG_ARRAY, 0},
	{"LEN_BIGEMINY", gettext_noop("length (#rri's) of the bigeminy"), RA_VALUE_TYPE_LONG_ARRAY, 0},

	{"NUM_TRIGEMINY", gettext_noop("number of trigeminy"), RA_VALUE_TYPE_LONG, 1},
	{"IDX_TRIGEMINY", gettext_noop("start event indeces of the trigeminy"), RA_VALUE_TYPE_LONG_ARRAY, 0},
	{"LEN_TRIGEMINY", gettext_noop("length (#rri's) of the trigeminy"), RA_VALUE_TYPE_LONG_ARRAY, 0},

	{"NUM_SALVO", gettext_noop("number of salvos"), RA_VALUE_TYPE_LONG, 1},
	{"IDX_SALVO", gettext_noop("start event indeces of the salvos"), RA_VALUE_TYPE_LONG_ARRAY, 0},
	{"LEN_SALVO", gettext_noop("length (#rri's) of the salvos"), RA_VALUE_TYPE_LONG, 0},
	{"HR_SALVO", gettext_noop("heart rate of the salvos"), RA_VALUE_TYPE_DOUBLE, 0},

	{"NUM_VT", gettext_noop("number of VTs"), RA_VALUE_TYPE_LONG, 1},
	{"IDX_VT", gettext_noop("start event indeces of VTs"), RA_VALUE_TYPE_LONG_ARRAY, 0},
	{"LEN_VT", gettext_noop("length (#rri's) of the VTs"), RA_VALUE_TYPE_LONG, 0},
	{"HR_VT", gettext_noop("heart rate of the VTs"), RA_VALUE_TYPE_DOUBLE, 0},

	{"NUM_SALVO_VT", gettext_noop("number of salvos/VTs"), RA_VALUE_TYPE_LONG, 1},
	{"IDX_SALVO_VT", gettext_noop("start event indeces of the salvos/VTs"), RA_VALUE_TYPE_LONG_ARRAY, 0},
	{"LEN_SALVO_VT", gettext_noop("length (#rri's) of the salvos/VTs"), RA_VALUE_TYPE_LONG, 0},
	{"HR_SALVO_VT", gettext_noop("heart rate of the salvos/VTs"), RA_VALUE_TYPE_DOUBLE, 0},
};
long num_results = sizeof(results) / sizeof(results[0]);


/* static struct ra_auto_create_class create_class[] = { */
/* }; */
/* long num_create_class = sizeof(create_class) / sizeof(create_class[0]); */
static struct ra_auto_create_class *create_class = NULL;
long num_create_class = 0;

static struct ra_auto_create_prop create_prop[] = {
	{"qrs-temporal", -1, IDX_RRI, QRS_TEMPORAL, -1, NULL, 1},
	{"qrs-annot", -1, IDX_RRI, QRS_ANNOT, -1, NULL, 1},
	{"ecg-flags", -1, IDX_RRI, ECG_FLAGS, -1, NULL, 1},
	{"rri", -1, IDX_RRI, RRI, -1, NULL, 1},
	{"rri-annot", -1, IDX_RRI, RRI_ANNOT, -1, NULL, 1},
	{"rri-refvalue", -1, IDX_RRI, RRI_REFVALUE, -1, NULL, 1},
	{"rri-num-refvalue", -1, IDX_RRI, RRI_NUM_REFVALUE, -1, NULL, 1},
	{"ecg-p-width", 0, IDX_RRI, P_WIDTH, -1, NULL, 1},
	{"ecg-qrs-width", 0, IDX_RRI, QRS_WIDTH, -1, NULL, 1},
	{"ecg-t-width", 0, IDX_RRI, T_WIDTH, -1, NULL, 1},
	{"ecg-pq", 0, IDX_RRI, PQ, -1, NULL, 1},
	{"ecg-qt", 0, IDX_RRI, QT, -1, NULL, 1},
	{"ecg-qtc", 0, IDX_RRI, QTC, -1, NULL, 1},
};
long num_create_prop = sizeof(create_prop) / sizeof(create_prop[0]);


/* -------------------- prototypes -------------------- */
int pl_call_gui(proc_handle proc);
proc_handle pl_get_proc_handle(plugin_handle pl);
void set_default_options(struct ra_ecg_options *opt);
void set_option_offsets(struct ra_option_infos *opt_inf, struct ra_ecg_options *opt);
void pl_free_proc_handle(proc_handle proc);
int pl_do_processing(proc_handle proc);
int pl_get_result(proc_handle proc, value_handle vh);

int get_number_runs(struct ra_ecg_options *opt);
double get_samplerate(meas_handle mh);
void free_beats_mem(struct beat_infos *beats, long num);

#ifdef WIN32
__inline void free_one_beat_mem(struct beat_infos *b);
#else
inline void free_one_beat_mem(struct beat_infos *b);
#endif  /* WIN32 */

int get_ecg_channels(rec_handle rh, long **ch, long *num_ch, double **ch_samplerate);

int get_eval_values(struct proc_info *pi, long run, struct beat_infos **beats, long *num,
		    long *ch, long num_ch, class_handle *clh_used);
int get_single_values(struct proc_info *pi, struct beat_infos *beats, long num,
		      long *ch, long num_ch, class_handle *clh_used);
int get_data_from_class(struct proc_info *pi, long run, struct beat_infos **beats, long *num,
			long *ch, long num_ch, class_handle *clh_used);
class_handle get_event_class(struct proc_info *pi, long run);

int calc_rri(struct beat_infos *beats, long num, double samplerate);
int check_beats_on_same_pos(class_handle clh, struct beat_infos *beats, long *num, long min_beat_distance, long change_eval);
int calc_temporal_setting(struct ra_ecg_options *opt, struct beat_infos *beats, long num, double samplerate);
int temporal_normal(struct ra_ecg_options *opt, struct beat_infos *beats, long curr_beat, double rri_ref);
int temporal_vpc(struct ra_ecg_options *opt, struct beat_infos *beats, long curr_beat, double rri_ref);
int init_calc_ref(struct ra_ecg_options *opt, struct ref_value_infos *ref_info, double samplerate);
int calc_ref_value(struct ref_value_infos *inf, struct beat_infos *beat, double *rri_ref, int *num_ref_rri);
int calc_ref_value_new(struct ref_value_infos *inf, struct beat_infos *beat, double *rri_ref, int *num_ref_rri);

int subclass_beats(struct ra_ecg_options *opt, struct beat_infos *beats, long num);
int subclass_normals(struct ra_ecg_options *opt, struct beat_infos *beats, long num);
int subclass_vpcs(struct ra_ecg_options *opt, struct beat_infos *beats, long num);
void subclass_single_vpc(struct ra_ecg_options *opt, struct beat_infos *beats, long num, long curr);
int filter_artifacts(struct ra_ecg_options *opt, struct beat_infos *beats, long num);
int make_rri_classifications(struct beat_infos *beats, long num);

int calc_morphology_values(struct beat_infos *beats, long num_beats, long num_ch, double *ch_samplerate);

int make_summary(struct ra_ecg_options *opt, long run, struct beat_infos *beats, long num, value_handle *res);
int make_beat_summary(long run, struct beat_infos *beats, long num, value_handle *res);
int get_beat_count(long base_cl, long sub_cl, struct beat_infos *beats, long num, value_handle *res,
		   long res_idx_num, long res_idx_idx);

int make_arrhythmia_summary(struct ra_ecg_options *opt, long run, struct beat_infos *beats, long num, value_handle *res);
int get_hr_values(struct ra_ecg_options *opt, long run, struct beat_infos *beats, long num, value_handle *res);

int get_sinus_related_values(long run, struct beat_infos *beats, long num, value_handle *res);
int get_brady_tachy(long run, struct beat_infos *beats, long num, value_handle *res);
int get_sinus_salvos(long run, struct beat_infos *beats, long num, value_handle *res);

int get_ventricular_related_values(long run, struct beat_infos *beats, long num, value_handle *res);
int get_salvos(long run, struct beat_infos *beats, long num, value_handle *res);
int get_ivr(long run, struct beat_infos *beats, long num, value_handle *res);
int get_couplet(long run, struct beat_infos *beats, long num, value_handle *res);
int get_bigeminy(long run, struct beat_infos *beats, long num, value_handle *res);
int get_trigeminy(long run, struct beat_infos *beats, long num, value_handle *res);
int save_arr_info(long num, struct arr_infos *inf, value_handle *res, long num_idx, long idx_idx, long len_idx, long hr_idx);

int save_in_res(long run, struct proc_info *pi, struct beat_infos *beats, long num);
int save_single(class_handle clh, struct beat_infos *beats, long num);
prop_handle get_prop(class_handle clh, const char *prop);

int get_resp_phase(struct ra_ecg_options *opt, long run, struct beat_infos *beats, long num,
		   value_handle *res, class_handle clh_beats);
int find_resp_channel(rec_handle rh, long *num_ch, long **ch);
int save_resp_values(struct ra_ecg_options *opt, long run, struct beat_infos *beats, long num,
		     value_handle *res, class_handle clh_beats, long num_ch_resp);

#endif /* _ECG_H */
