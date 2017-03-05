/**
 * \file ra_event_props.h
 *
 * The header file contains pre-defined event-sets and -properties.
 *
 * \author Raphael Schneider (librasch@gmail.com)
 */

/*----------------------------------------------------------------------------
 *
 * Copyright (C) 2002-2009, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *
 *--------------------------------------------------------------------------*/

#ifndef _RA_EVENT_PROPS_H
#define _RA_EVENT_PROPS_H

#include <ra_defines.h>
#include <ra_priv.h>

/* do gettext defines first */
#define _(String) gettext (String)
#define gettext_noop(String) String
#define N_(String) gettext_noop (String)

#ifdef __cplusplus
extern "C" {
#endif


/* -------------------- defines -------------------- */


/* -------------------- struct definitions -------------------- */
/**
 * \struct event_class_desc
 * \brief contains description for an event-class
 * \param <id> id of the event-class (see EVENT_CLASS_* in ra_event_props.h)
 * \param <id_ascii> id-string of the event-class (=name but will not be translated)
 * \param <name> name of the event-class
 * \param <desc> description of the event-class
 */
struct event_class_desc
{
	int id;
	char ascii_id[EVAL_MAX_NAME];

	char name[EVAL_MAX_NAME];
	char desc[EVAL_MAX_DESC];
}; /* struct event_class_desc */


/**
 * \struct event_prop_desc
 * \brief contains description for an event-property
 * \param <id> id of the event-property (see EVENT_PROP_* in ra_defines.h)
 * \param <event_set_id> id of the event-set which this property belongs to
 * \param <id_ascii> id-string of the event-property (=name but will not be translated)
 * \param <name> name of the event-property
 * \param <desc> description of the event-property
 * \param <unit> unit of the event-property
 * \param <val_type> type of the event-value
 * \param <num_values> number of values stored for the property (normally this is 1)
 * \param <use_minmax> flag if min- and max-values are valid (=1) or not (=0)
 * \param <min_value> minimum value of the event-property
 * \param <max_value> maximum value of the event-property
 * \param <has_ignore_value> flag if an ignore-value is available
 * \param <ignore_value> value which should be ignored (e.g. when plotting the values)
 */
struct event_prop_desc
{
	int id;

	int event_set_id;

	char ascii_id[EVAL_MAX_NAME];

	char name[EVAL_MAX_NAME];
	char desc[EVAL_MAX_DESC];
	char unit[EVAL_MAX_UNIT];
	long val_type;
	long num_values;

	/* default min- and max-values */
	int use_minmax;
	double min_value;
	double max_value;

	int has_ignore_value;
	double ignore_value;
}; /* struct event_prop_desc */


#ifdef _DEFINE_INFO_STRUCTS

/* -------------------- predefined infos for event-classes and event-properties -------------------- */
static struct event_class_desc ra_event_class[] = {
	{EVENT_CLASS_HEARTBEAT, "heartbeat", gettext_noop("heartbeat"),
	 gettext_noop("infos about a heart beat")},

	{EVENT_CLASS_RR_CALIBRATION, "rr-calibration", gettext_noop("rr-calibration"),
	 gettext_noop("infos about sequences of calibration in bloodpressure measurements")},

	{EVENT_CLASS_ANNOT, "annotation", gettext_noop("annotations"), gettext_noop("annotations/comments")},

	{EVENT_CLASS_ARRHYTHMIA, "arrhythmia", gettext_noop("arrhythmia"), gettext_noop("arrhythmic events")},

	{EVENT_CLASS_ARRHYTHMIA_ORIGINAL, "arrhythmia-original", gettext_noop("arrhythmia-original"),
	 gettext_noop("arrhythmic events from original evaluation")},

	{EVENT_CLASS_UTERINE_CONTRACTION, "uterine-contraction", gettext_noop("uterine-contraction"),
	 gettext_noop("infos about uterine contraction")},

	{EVENT_CLASS_RESPIRATION, "respiration", gettext_noop("respiration"),
	 gettext_noop("respiration related data")},

	{EVENT_CLASS_EGM_MARKER, "EGMmarker", gettext_noop("EGM-marker"),
	 gettext_noop("EGM-marker related data")},
}; /* ra_event_class */

#endif /* _DEFINE_INFO_STRUCTS */

#ifdef _DEFINE_INFO_STRUCTS_PROP
#define _DEFINE_INFO_STRUCTS
#endif /* _DEFINE_INFO_STRUCTS_PROP */

#ifdef _DEFINE_INFO_STRUCTS

static struct event_prop_desc ra_event_prop[] = {
	/* QRS complex infos */
	{EVENT_PROP_QRS_TEMPLATE, EVENT_CLASS_HEARTBEAT, "qrs-template", gettext_noop("qrs-template"),
	 gettext_noop("QRS template number"), "", RA_VALUE_TYPE_SHORT, 1, 0, 0, 0, 0, 0.0},

	{EVENT_PROP_QRS_TEMPLATE_CORR, EVENT_CLASS_HEARTBEAT, "qrs-template-corr", gettext_noop("qrs-template-corr"),
	 gettext_noop("correlation of qrs-complex with template"), "", RA_VALUE_TYPE_DOUBLE, 1, 0, 0, 0, 0, 0.0},

	{EVENT_PROP_QRS_POS, EVENT_CLASS_HEARTBEAT, "qrs-pos", gettext_noop("qrs-pos"),
	 gettext_noop("position of fiducial point of QRS-complex in sampleunits"),
	 "su", RA_VALUE_TYPE_LONG, 1, 0, 0, 0, 0, 0.0},

	{EVENT_PROP_QRS_ANNOT, EVENT_CLASS_HEARTBEAT, "qrs-annot", gettext_noop("qrs-annot"),
	 gettext_noop("annotation of QRS complex"), "", RA_VALUE_TYPE_LONG, 1, 0, 0, 0, 0, 0.0},

	{EVENT_PROP_QRS_CH, EVENT_CLASS_HEARTBEAT, "qrs-ch", gettext_noop("qrs-ch"),
	 gettext_noop("bitmask of channels where qrs was detected"),
	 "", RA_VALUE_TYPE_LONG, 1, 0, 0, 0, 0, 0.0},

	{EVENT_PROP_QRS_TEMPORAL, EVENT_CLASS_HEARTBEAT, "qrs-temporal", gettext_noop("qrs-temporal"),
	 gettext_noop("temporal setting of beat"), "", RA_VALUE_TYPE_SHORT, 1, 0, 0, 0, 0, 0.0},

	/* ECG related infos */
	{EVENT_PROP_ECG_FLAGS, EVENT_CLASS_HEARTBEAT, "ecg-flags", gettext_noop("ecg-flags"),
	 gettext_noop("ECG releated flags"), "", RA_VALUE_TYPE_LONG, 1, 0, 0, 0, 0, 0.0},

	/* heart beat morphology values */
	{EVENT_PROP_ECG_NOISE, EVENT_CLASS_HEARTBEAT, "ecg-noise", gettext_noop("ecg-noise"),
	 gettext_noop(""), "mV", RA_VALUE_TYPE_DOUBLE, 1, 0, 0, 0, 1, -1.0},

	{EVENT_PROP_ECG_P_TYPE, EVENT_CLASS_HEARTBEAT, "ecg-p-type", gettext_noop("ecg-p-type"),
	 gettext_noop(""), "", RA_VALUE_TYPE_LONG, 1, 0, 0, 0, 0, 0.0},

	{EVENT_PROP_ECG_QRS_TYPE, EVENT_CLASS_HEARTBEAT, "ecg-qrs-type", gettext_noop("ecg-qrs-type"),
	 gettext_noop(""), "", RA_VALUE_TYPE_LONG, 1, 0, 0, 0, 0, 0.0},

	{EVENT_PROP_ECG_T_TYPE, EVENT_CLASS_HEARTBEAT, "ecg-t-type", gettext_noop("ecg-t-type"),
	 gettext_noop(""), "", RA_VALUE_TYPE_LONG, 1, 0, 0, 0, 0, 0.0},

	{EVENT_PROP_ECG_U_TYPE, EVENT_CLASS_HEARTBEAT, "ecg-u-type", gettext_noop("ecg-u-type"),
	 gettext_noop(""), "", RA_VALUE_TYPE_LONG, 1, 0, 0, 0, 0, 0.0},

	{EVENT_PROP_ECG_MORPH_FLAGS, EVENT_CLASS_HEARTBEAT, "ecg-morph-flags", gettext_noop("ecg-morph-flags"),
	 gettext_noop("flags for the morphology values"),
	 "", RA_VALUE_TYPE_LONG, 1, 0, 0, 0, 0, 0.0},

	{EVENT_PROP_ECG_P_START_OFFSET, EVENT_CLASS_HEARTBEAT, "ecg-p-start", gettext_noop("ecg-p-start"),
	 gettext_noop("offset of p-wave begin from qrs-pos in sampleunits"),
	 "su", RA_VALUE_TYPE_SHORT, 1, 0, 0, 0, 0, 0.0},

	{EVENT_PROP_ECG_P_PEAK_1_OFFSET, EVENT_CLASS_HEARTBEAT, "ecg-p-peak-1", gettext_noop("ecg-p-peak-1"),
	 gettext_noop("offset of p-wave peak from qrs-pos in sampleunits"),
	 "su", RA_VALUE_TYPE_SHORT, 1, 0, 0, 0, 0, 0.0},

	{EVENT_PROP_ECG_P_PEAK_2_OFFSET, EVENT_CLASS_HEARTBEAT, "ecg-p-peak-2", gettext_noop("ecg-p-peak-2"),
	 gettext_noop("offset of 2nd p-wave peak (if biphasic) from qrs-pos in sampleunits"), 
	 "su", RA_VALUE_TYPE_SHORT, 1, 0, 0, 0, 0, 0.0},

	{EVENT_PROP_ECG_P_END_OFFSET, EVENT_CLASS_HEARTBEAT, "ecg-p-end", gettext_noop("ecg-p-end"),
	 gettext_noop("offset of p-wave end from qrs-pos in sampleunits"),
	 "su", RA_VALUE_TYPE_SHORT, 1, 0, 0, 0, 0, 0.0},

	{EVENT_PROP_ECG_QRS_START_OFFSET, EVENT_CLASS_HEARTBEAT, "ecg-qrs-start", gettext_noop("ecg-qrs-start"),
	 gettext_noop("offset of start of qrs complex from qrs-pos in sampleunits"),
	 "su", RA_VALUE_TYPE_SHORT, 1, 0, 0, 0, 0, 0.0},

	{EVENT_PROP_ECG_Q_PEAK_OFFSET, EVENT_CLASS_HEARTBEAT, "ecg-q-peak", gettext_noop("ecg-q-peak"),
	 gettext_noop("offset of q-wave from qrs-pos in sampleunits"),
	 "su", RA_VALUE_TYPE_SHORT, 1, 0, 0, 0, 0, 0.0},

	{EVENT_PROP_ECG_R_PEAK_OFFSET, EVENT_CLASS_HEARTBEAT, "ecg-r-peak", gettext_noop("ecg-r-peak"),
	 gettext_noop("offset of r-wave from qrs-pos in sampleunits"),
	 "su", RA_VALUE_TYPE_SHORT, 1, 0, 0, 0, 0, 0.0},

	{EVENT_PROP_ECG_S_PEAK_OFFSET, EVENT_CLASS_HEARTBEAT, "ecg-s-peak", gettext_noop("ecg-s-peak"),
	 gettext_noop("offset of s-wave from qrs-pos in sampleunits"),
	 "su", RA_VALUE_TYPE_SHORT, 1, 0, 0, 0, 0, 0.0},

	{EVENT_PROP_ECG_RS_PEAK_OFFSET, EVENT_CLASS_HEARTBEAT, "ecg-rs-peak", gettext_noop("ecg-rs-peak"),
	 gettext_noop("offset of r'-wave from qrs-pos in sampleunits"),
	 "su", RA_VALUE_TYPE_SHORT, 1, 0, 0, 0, 0, 0.0},

	{EVENT_PROP_ECG_QRS_END_OFFSET, EVENT_CLASS_HEARTBEAT, "ecg-qrs-end", gettext_noop("ecg-qrs-end"),
	 gettext_noop("offset of end of qrs complex from qrs-pos in sampleunits"),
	 "su", RA_VALUE_TYPE_SHORT, 1, 0, 0, 0, 0, 0.0},

	{EVENT_PROP_ECG_T_START_OFFSET, EVENT_CLASS_HEARTBEAT, "ecg-t-start", gettext_noop("ecg-t-start"),
	 gettext_noop("offset of t-wave begin from qrs-pos in sampleunits"),
	 "su", RA_VALUE_TYPE_SHORT, 1, 0, 0, 0, 0, 0.0},

	{EVENT_PROP_ECG_T_PEAK_1_OFFSET, EVENT_CLASS_HEARTBEAT, "ecg-t-peak-1", gettext_noop("ecg-t-peak-1"),
	 gettext_noop("offset of t-wave peak from qrs-pos in sampleunits"),
	 "su", RA_VALUE_TYPE_SHORT, 1, 0, 0, 0, 0, 0.0},

	{EVENT_PROP_ECG_T_PEAK_2_OFFSET, EVENT_CLASS_HEARTBEAT, "ecg-t-peak-2", gettext_noop("ecg-t-peak-2"),
	 gettext_noop("offset of 2nd t-wave peak (if biphasic) from qrs-pos in sampleunits"),
	 "su", RA_VALUE_TYPE_SHORT, 1, 0, 0, 0, 0, 0.0},

	{EVENT_PROP_ECG_T_END_OFFSET, EVENT_CLASS_HEARTBEAT, "ecg-t-end", gettext_noop("ecg-t-end"),
	 gettext_noop("offset of t-wave end from qrs-pos in sampleunits"),
	 "su", RA_VALUE_TYPE_SHORT, 1, 0, 0, 0, 0, 0.0},

	{EVENT_PROP_ECG_U_START_OFFSET, EVENT_CLASS_HEARTBEAT, "ecg-u-start", gettext_noop("ecg-u-start"),
	 gettext_noop("offset of u-wave begin from qrs-pos in sampleunits"),
	 "su", RA_VALUE_TYPE_SHORT, 1, 0, 0, 0, 0, 0.0},

	{EVENT_PROP_ECG_U_PEAK_1_OFFSET, EVENT_CLASS_HEARTBEAT, "ecg-u-peak-1", gettext_noop("ecg-u-peak-1"),
	 gettext_noop("offset of u-wave peak from qrs-pos in sampleunits"),
	 "su", RA_VALUE_TYPE_SHORT, 1, 0, 0, 0, 0, 0.0},

	{EVENT_PROP_ECG_U_PEAK_2_OFFSET, EVENT_CLASS_HEARTBEAT, "ecg-u-peak-2", gettext_noop("ecg-u-peak-2"),
	 gettext_noop("offset of 2nd u-wave peak (if biphasic) from qrs-pos in sampleunits"),
	 "su", RA_VALUE_TYPE_SHORT, 1, 0, 0, 0, 0, 0.0},

	{EVENT_PROP_ECG_U_END_OFFSET, EVENT_CLASS_HEARTBEAT, "ecg-u-end", gettext_noop("ecg-u-end"),
	 gettext_noop("offset of u-wave end from qrs-pos in sampleunits"),
	 "su", RA_VALUE_TYPE_SHORT, 1, 0, 0, 0, 0, 0.0},


	/* morphology values which should be used for further analysis (only one value for each beat) */
	{EVENT_PROP_ECG_P_WIDTH, EVENT_CLASS_HEARTBEAT, "ecg-p-width", gettext_noop("ecg-p-width"),
	 gettext_noop(""), "ms", RA_VALUE_TYPE_DOUBLE, 1, 1, 10, 110, 1, -1.0},

	{EVENT_PROP_ECG_QRS_WIDTH, EVENT_CLASS_HEARTBEAT, "ecg-qrs-width", gettext_noop("ecg-qrs-width"),
	 gettext_noop(""), "ms", RA_VALUE_TYPE_DOUBLE, 1, 1, 50, 250, 1, -1.0},

	{EVENT_PROP_ECG_T_WIDTH, EVENT_CLASS_HEARTBEAT, "ecg-t-width", gettext_noop("ecg-t-width"),
	 gettext_noop(""), "ms", RA_VALUE_TYPE_DOUBLE, 1, 1, 200, 600, 1, -1.0},

	{EVENT_PROP_ECG_PQ, EVENT_CLASS_HEARTBEAT, "ecg-pq", gettext_noop("ecg-pq"), gettext_noop("PQ interval"),
	 "ms", RA_VALUE_TYPE_DOUBLE, 1, 1, 50, 300, 1, -1.0},

	{EVENT_PROP_ECG_QT, EVENT_CLASS_HEARTBEAT, "ecg-qt", gettext_noop("ecg-qt"), gettext_noop("QT interval"),
	 "ms", RA_VALUE_TYPE_DOUBLE, 1, 1, 200, 600, 1, -1.0},

	{EVENT_PROP_ECG_QTC, EVENT_CLASS_HEARTBEAT, "ecg-qtc", gettext_noop("ecg-qtc"), gettext_noop("QTc interval"),
	 "ms", RA_VALUE_TYPE_DOUBLE, 1, 1, 200, 600, 1, -1.0},

	{EVENT_PROP_ECG_QTA, EVENT_CLASS_HEARTBEAT, "ecg-qta", gettext_noop("ecg-qta"), gettext_noop("QTa interval"),
	 "ms", RA_VALUE_TYPE_DOUBLE, 1, 1, 200, 600, 1, -1.0},

	/* RR interval infos */
	{EVENT_PROP_RRI, EVENT_CLASS_HEARTBEAT, "rri", gettext_noop("rri"), gettext_noop("RR interval"),
	 "ms", RA_VALUE_TYPE_DOUBLE, 1, 1, 200, 2000, 1, -1.0},

	{EVENT_PROP_RRI_ANNOT, EVENT_CLASS_HEARTBEAT, "rri-annot", gettext_noop("rri-annotation"),
	 gettext_noop("annoation of RR interval"), "", RA_VALUE_TYPE_LONG, 1, 0, 0, 0, 0, 0.0},

	{EVENT_PROP_RRI_REFVALUE, EVENT_CLASS_HEARTBEAT, "rri-refvalue", gettext_noop("rri-refvalue"),
	 gettext_noop("reference rri representing the current heart-rate"),
	 "ms", RA_VALUE_TYPE_DOUBLE, 1, 1, 200, 2000, 0, 0.0},

	{EVENT_PROP_RRI_NUM_REFVALUE, EVENT_CLASS_HEARTBEAT, "rri-num-refvalue", gettext_noop("rri-num-refvalue"),
	 gettext_noop("number of rri's used for calculation of reference value"),
	 "#", RA_VALUE_TYPE_SHORT, 1, 1, 0, 5, 0, 0.0},


	/* respiration values when heartbeat occurs */
	{EVENT_PROP_ECG_RESP_PHASE, EVENT_CLASS_HEARTBEAT, "ecg-resp-phase", gettext_noop("ecg-resp-phase"),
	 gettext_noop("respiration phase (-pi..+pi)"), "", RA_VALUE_TYPE_DOUBLE, 1, 0, 0, 0, 0, 0.0},


	/* values for respiration signals */
	{EVENT_PROP_RESP_CHEST_MEAN_RRI, EVENT_CLASS_HEARTBEAT, "resp-chest-mean-rri", gettext_noop("resp-chest-mean-rri"),
	 gettext_noop("mean chest measurement of the rr interval"),
	 "%", RA_VALUE_TYPE_DOUBLE, 1, 0, 0, 0, 0, 0.0},

	{EVENT_PROP_RESP_CHEST_MEAN_IBI, EVENT_CLASS_HEARTBEAT, "resp-chest-mean-ibi", gettext_noop("resp-chest-mean-ibi"),
	 gettext_noop("mean chest measurement of the inter beat interval"),
	 "%", RA_VALUE_TYPE_DOUBLE, 1, 0, 0, 0, 0, 0.0},


	/* bloodpressure infos */
	{EVENT_PROP_RR_TEMPLATE, EVENT_CLASS_HEARTBEAT, "rr-template", gettext_noop("rr-template"),
	 gettext_noop("bloodpressure template number"), "", RA_VALUE_TYPE_SHORT, 1, 0, 0, 0, 0, 0.0},

	{EVENT_PROP_RR_SYSTOLIC, EVENT_CLASS_HEARTBEAT, "rr-systolic", gettext_noop("rr-systolic"),
	 gettext_noop("systolic bloodpressure in mmHg"), "mmHg", RA_VALUE_TYPE_DOUBLE, 1, 1, 0, 200, 0, 0.0},

	{EVENT_PROP_RR_SYSTOLIC_POS, EVENT_CLASS_HEARTBEAT, "rr-systolic-pos", gettext_noop("rr-systolic-pos"),
	 gettext_noop("position of systolic bloodpressure in sampleunits"),
	 "su", RA_VALUE_TYPE_LONG, 1, 0, 0, 0, 0, 0.0},

	{EVENT_PROP_RR_DIASTOLIC, EVENT_CLASS_HEARTBEAT, "rr-diastolic", gettext_noop("rr-diastolic"),
	 gettext_noop("diastolic bloodpressure in mmHg"), "mmHg", RA_VALUE_TYPE_DOUBLE, 1, 1, 0, 200, 0, 0.0},

	{EVENT_PROP_RR_DIASTOLIC_POS, EVENT_CLASS_HEARTBEAT, "rr-diastolic-pos", gettext_noop("rr-diastolic-pos"),
	 gettext_noop("position of diastolic bloodpressure in sampleunits"),
	 "su", RA_VALUE_TYPE_LONG, 1, 0, 0, 0, 0, 0.0},

	{EVENT_PROP_RR_MEAN, EVENT_CLASS_HEARTBEAT, "rr-mean", gettext_noop("rr-mean"),
	 gettext_noop("mean bloodpressure in mmHg"), "mmHg", RA_VALUE_TYPE_DOUBLE, 1, 1, 0, 200, 0, 0.0},

	{EVENT_PROP_RR_FLAGS, EVENT_CLASS_HEARTBEAT, "rr-flags", gettext_noop("rr-flags"),
	 gettext_noop("flags for the rr-values"), "", RA_VALUE_TYPE_LONG, 1, 0, 0, 0, 0, 0.0},

	{EVENT_PROP_RR_IBI, EVENT_CLASS_HEARTBEAT, "ibi", gettext_noop("ibi"),
	 gettext_noop("inter-beat-interval"), "ms", RA_VALUE_TYPE_DOUBLE, 1, 0, 0, 0, 0, 0.0},

	{EVENT_PROP_RR_DPDT_MIN, EVENT_CLASS_HEARTBEAT, "rr-dpdt-min", gettext_noop("rr-dpdt-min"),
	 gettext_noop("dp/dt min in mmHg/s"), "mmHg/s", RA_VALUE_TYPE_DOUBLE, 1, 1, -2.0, 0.0, 0, 0.0},

	{EVENT_PROP_RR_DPDT_MAX, EVENT_CLASS_HEARTBEAT, "rr-dpdt-max", gettext_noop("rr-dpdt-max"),
	 gettext_noop("dp/dt max in mmHg/s"), "mmHg/s", RA_VALUE_TYPE_DOUBLE, 1, 1, 0.0, 2.0, 0, 0.0},

	{EVENT_PROP_RR_DPDT_MIN_POS, EVENT_CLASS_HEARTBEAT, "rr-dpdt-min-pos", gettext_noop("rr-dpdt-min-pos"),
	 gettext_noop("position of dp/dt min in sampleunits"),
	 "su", RA_VALUE_TYPE_LONG, 1, 0, 0, 0, 0, 0.0},

	{EVENT_PROP_RR_DPDT_MAX_POS, EVENT_CLASS_HEARTBEAT, "rr-dpdt-max-pos", gettext_noop("rr-dpdt-max-pos"),
	 gettext_noop("position of dp/dt max in sampleunits"),
	 "su", RA_VALUE_TYPE_LONG, 1, 0, 0, 0, 0, 0.0},

	{EVENT_PROP_RR_DPDT_MIN_PRESSURE, EVENT_CLASS_HEARTBEAT, "rr-dpdt-min-pressure", gettext_noop("rr-dpdt-min-pressure"),
	 gettext_noop("pressure at dp/dt min in mmHg"),
	 "su", RA_VALUE_TYPE_LONG, 1, 0, 0, 0, 0, 0.0},

	{EVENT_PROP_RR_DPDT_MAX_PRESSURE, EVENT_CLASS_HEARTBEAT, "rr-dpdt-max-pressure", gettext_noop("rr-dpdt-max-pressure"),
	 gettext_noop("pressure at dp/dt max in mmHg"),
	 "su", RA_VALUE_TYPE_LONG, 1, 0, 0, 0, 0, 0.0},



	/* rr-calibration infos */
	{EVENT_PROP_RR_CALIBRATION_INFO, EVENT_CLASS_RR_CALIBRATION, "rr-calibration-info", gettext_noop("rr-calibration-info"),
	 gettext_noop("infos about the RR calibration sequences"), "",
	 RA_VALUE_TYPE_LONG, 1, 0, 0, 0, 0, 0.0},


	/* uterine contraction infos */
	{EVENT_PROP_UC_MAX_POS, EVENT_CLASS_UTERINE_CONTRACTION, "uc-max-pos", gettext_noop("uc-max-pos"),
	 gettext_noop("position of uterine contraction (max. peak)"),
	 "su", RA_VALUE_TYPE_LONG, 1, 0, 0, 0, 0, 0.0},

	{EVENT_PROP_UC_TEMPLATE, EVENT_CLASS_UTERINE_CONTRACTION, "uc-template", gettext_noop("uc-template"),
	 gettext_noop("Uterine Contraction template number"), "", RA_VALUE_TYPE_SHORT, 1, 0, 0, 0, 0, 0.0},

	{EVENT_PROP_UC_TEMPLATE_CORR, EVENT_CLASS_UTERINE_CONTRACTION, "uc-template-corr", gettext_noop("uc-template-corr"),
	 gettext_noop("correlation of uterine contraction with template"),
	 "", RA_VALUE_TYPE_DOUBLE, 1, 0, 0, 0, 0, 0.0},


	/* values for comments/annotations */
	{EVENT_PROP_ANNOT, EVENT_CLASS_ANNOT, "annotation", gettext_noop("annotations"),
	 gettext_noop("annotations/comments"), "", RA_VALUE_TYPE_CHAR, 1, 0, 0, 0, 0, 0.0},


	/* arryhthmic events (set in the ecg libRASCH plugin) */
	{EVENT_PROP_ARR_TYPE, EVENT_CLASS_ARRHYTHMIA, "arr-type", gettext_noop("arr-type"),
	 gettext_noop("type of arrhythmic event"), "", RA_VALUE_TYPE_SHORT, 1, 0, 0, 0, 0, 0.0},

	{EVENT_PROP_ARR_HR, EVENT_CLASS_ARRHYTHMIA, "arr-hr", gettext_noop("arr-hr"),
	 gettext_noop("heart rate [bpm] of arrhythmic event"),
	 "", RA_VALUE_TYPE_DOUBLE, 1, 0, 0, 0, 0, 0.0},

	{EVENT_PROP_ARR_NUM_QRS, EVENT_CLASS_ARRHYTHMIA, "arr-num-qrs", gettext_noop("arr-num-qrs"),
	 gettext_noop("number of qrs-complexes in arryhthmic event"),
	 "", RA_VALUE_TYPE_LONG, 1, 0, 0, 0, 0, 0.0},


	/* arryhthmic events from original evaluation*/
	{EVENT_PROP_ARR_ORIG_TYPE, EVENT_CLASS_ARRHYTHMIA_ORIGINAL, "arr_o-type", gettext_noop("arr_o-type"),
	 gettext_noop("type of arrhythmic event (original eval)"),
	 "", RA_VALUE_TYPE_SHORT, 1, 0, 0, 0, 0, 0.0},

	{EVENT_PROP_ARR_ORIG_HR, EVENT_CLASS_ARRHYTHMIA_ORIGINAL, "arr_o-hr", gettext_noop("arr_o-hr"),
	 gettext_noop("heart rate [bpm] of arrhythmic event (original eval)"),
	 "", RA_VALUE_TYPE_DOUBLE, 1, 0, 0, 0, 0, 0.0},

	{EVENT_PROP_ARR_ORIG_NUM_QRS, EVENT_CLASS_ARRHYTHMIA_ORIGINAL, "arr_o-num-qrs", gettext_noop("arr_o-num-qrs"),
	 gettext_noop("number of qrs-complexes in arryhthmic event (original eval)"),
	 "", RA_VALUE_TYPE_LONG, 1, 0, 0, 0, 0, 0.0},


	/* respiration properties */
	{EVENT_PROP_RESP_POS, EVENT_CLASS_RESPIRATION, "resp-pos", gettext_noop("resp-pos"),
	 gettext_noop("reference point of the respiration phase [su]"),
	 "", RA_VALUE_TYPE_LONG, 1, 0, 0, 0, 0, 0.0},
	{EVENT_PROP_RESP_PHASE, EVENT_CLASS_RESPIRATION, "resp-phase", gettext_noop("resp-phase"),
	 gettext_noop("respiration phase (-pi..+pi)"),
	 "", RA_VALUE_TYPE_DOUBLE, 1, 0, 0, 0, 0, 0.0},

	 /* EGM marker properties */
	{EVENT_PROP_EGM_MARKER_POS, EVENT_CLASS_EGM_MARKER, "egm-marker-pos", gettext_noop("egm-marker-pos"),
	 gettext_noop("timing of an EGM marker [su]"),
	 "", RA_VALUE_TYPE_LONG, 1, 0, 0, 0, 0, 0.0},
	{EVENT_PROP_EGM_MARKER_TYPE, EVENT_CLASS_EGM_MARKER, "egm-marker-type", gettext_noop("egm-marker-type"),
	 gettext_noop("type of an EGM marker"),
	 "", RA_VALUE_TYPE_LONG, 1, 0, 0, 0, 0, 0.0},

}; /* ra_event_prop */

#endif /* _DEFINE_INFO_STRUCTS */

#ifdef __cplusplus
}
#endif

#endif /* _RA_EVENT_PROPS_H */
