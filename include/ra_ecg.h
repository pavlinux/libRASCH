/*----------------------------------------------------------------------------
 * ra_ecg.h
 *
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002-2007, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifndef _RA_ECG_H
#define _RA_ECG_H

#include "ra_defines.h"

#define _(String) gettext (String)
#define gettext_noop(String) String
#define N_(String) gettext_noop (String)

#ifdef __cplusplus
extern "C" {
#endif


/* ---------- defines ---------- */
/* base beat/rri classifications */
#define ECG_CLASS_UNKNOWN     0x0000	/* unknown classification */
#define ECG_CLASS_SINUS       0x0001	/* sinus beats */
#define ECG_CLASS_VENT        0x0002	/* ventricular beats */
#define ECG_CLASS_PACED       0x0004	/* paced beats */
#define ECG_CLASS_ARTIFACT    0x0008	/* artifacts */
/* sub-classification of ECG_CLASS_NORMAL */
#define ECG_CLASS_SINUS_NORMAL     0x0001	/* normal sinus-beat */
#define ECG_CLASS_SINUS_SVPC       0x0002	/* premature sinus-beat -> SVPC */
#define ECG_CLASS_SINUS_VPC        0x0004	/* premature sinus-beat but following
						   beat comes late (perhaps VPC?) */
#define ECG_CLASS_SINUS_PAUSE      0x0008       /* pause */
/* sub-classification of ECG_CLASS_VENT */
#define ECG_CLASS_VENT_SINGLE            0x0001	/* single VPC */
#define ECG_CLASS_VENT_SINGLE_PREM       0x0002	/* single premature VPC */
#define ECG_CLASS_VENT_SINGLE_PREM_COMP  0x0004	/* single premature VPC with compensatory pause */
#define ECG_CLASS_VENT_INTERP            0x0008	/* interpolated VPC */
#define ECG_CLASS_VENT_ESCAPE            0x0010	/* escape beat */
/* sub-classifications of ECG_CLASS_PACED */
#define ECG_CLASS_PACED_ATRIUM       0x0001	/* pace event in the atrium */
#define ECG_CLASS_PACED_VENTRICLE    0x0002	/* pace event in the ventricle */
/* sub-classifications of ECG_CLASS_ARTIFACT */
#define ECG_CLASS_ARTIFACT_TYPE1  0x0001	/* rri lower than minimal allowed rri */
#define ECG_CLASS_ARTIFACT_TYPE2  0x0002	/* artifact between 2 beats (eg. T-wave detection) */
#define ECG_CLASS_ARTIFACT_TYPE3  0x0004	/* rri after VPC is greater than 150% of 2*ref-rri */

/* macros accessing classifications */
#define GET_CLASS(x)       ((long)x & 0x0000ffff)
#define GET_SUBCLASS(x)    (((long)x & 0xffff0000) >> 16)

#define SET_CLASS(x,y)       y = ((long)x & 0x0000ffff)
#define SET_SUBCLASS(x,y)    y |= (((long)x << 16) & 0xffff0000)


#define IS_UNKNOWN(x)   (GET_CLASS(x) == ECG_CLASS_UNKNOWN)
#define IS_SINUS(x)     (GET_CLASS(x) == ECG_CLASS_SINUS)
#define IS_VENT(x)      (GET_CLASS(x) == ECG_CLASS_VENT)
#define IS_PACED(x)     (GET_CLASS(x) == ECG_CLASS_PACED)
#define IS_ARTIFACT(x)  (GET_CLASS(x) == ECG_CLASS_ARTIFACT)

#define IS_NORMAL(x)       (IS_SINUS(x) && (GET_SUBCLASS(x) & ECG_CLASS_SINUS_NORMAL))
#define IS_SVPC(x)         (IS_SINUS(x) && (GET_SUBCLASS(x) & ECG_CLASS_SINUS_SVPC))
#define IS_NORM_VPC(x)     (IS_SINUS(x) && (GET_SUBCLASS(x) & ECG_CLASS_SINUS_VPC))
#define IS_PAUSE(x)        (IS_SINUS(x) && (GET_SUBCLASS(x) & ECG_CLASS_SINUS_PAUSE))

#define IS_SINGLE_VPC(x)           (IS_VENT(x) && (GET_SUBCLASS(x) & ECG_CLASS_VENT_SINGLE))
#define IS_SINGLE_PREM_VPC(x)      (IS_VENT(x) && (GET_SUBCLASS(x) & ECG_CLASS_VENT_SINGLE_PREM))
#define IS_SINGLE_PREM_COMP_VPC(x) (IS_VENT(x) && (GET_SUBCLASS(x) & ECG_CLASS_VENT_SINGLE_PREM_COMP))
#define IS_INTERP(x)               (IS_VENT(x) && (GET_SUBCLASS(x) & ECG_CLASS_VENT_INTERP))
#define IS_ESCAPE(x)               (IS_VENT(x) && (GET_SUBCLASS(x) & ECG_CLASS_VENT_ESCAPE))

#define IS_ATRIAL_PACED(x)     (IS_PACED(x) && (GET_SUBCLASS(x) & ECG_CLASS_PACED_ATRIUM))
#define IS_VENTRICLE_PACED(x)  (IS_PACED(x) && (GET_SUBCLASS(x) & ECG_CLASS_PACED_VENTRICLE))

#define IS_TYPE1_ARTIFACT(x)  (IS_ARTIFACT(x) && (GET_SUBCLASS(x) & ECG_CLASS_ARTIFACT_TYPE1))
#define IS_TYPE2_ARTIFACT(x)  (IS_ARTIFACT(x) && (GET_SUBCLASS(x) & ECG_CLASS_ARTIFACT_TYPE2))
#define IS_TYPE3_ARTIFACT(x)  (IS_ARTIFACT(x) && (GET_SUBCLASS(x) & ECG_CLASS_ARTIFACT_TYPE3))


/* temporal setting of beats */
/* relative to reference-value */
#define ECG_TEMPORAL_UNKNOWN  0x0000
#define ECG_TEMPORAL_NORMAL   0x0001
#define ECG_TEMPORAL_PRE      0x0002
#define ECG_TEMPORAL_LATE     0x0004
/* absolute to previous rr-interval */
#define ECG_ABS_PRE           0x0010
#define ECG_ABS_LATE          0x0020

/* ecg flags */
#define ECG_FLAG_USER_EDIT          0x00000001 /* beat was edited by the user */
#define ECG_FLAG_NO_AV_CONDUCTION   0x00000002 /* an atrial event was sensed/paced but no ventricular event follows
						  (mainly used for intracardiac ECGs; beat-annotation is set to artifact) */


/* Types of arryhthmic events. These types are used in the EVENT_PROP_ARR_TYPE event property */
#define ECG_ARR_MAXHR          1
#define ECG_ARR_MINHR          2
#define ECG_ARR_BRADY         10
#define ECG_ARR_SSALVO        11
#define ECG_ARR_SVT           12
#define ECG_ARR_SSALVO_SVT    13
#define ECG_ARR_AFIB          20
#define ECG_ARR_IRREG         21
#define ECG_ARR_COUPLET      100
#define ECG_ARR_TRIPLET      101
#define ECG_ARR_BIGEMINUS    120
#define ECG_ARR_TRIGEMINUS   121
#define ECG_ARR_IVRS         140
#define ECG_ARR_SALVO        141
#define ECG_ARR_VT           142
#define ECG_ARR_SALVO_VT     143


/* -------------------- structures -------------------- */
struct ra_ecg_options
{
	eval_handle eh; /* evaluation used; if '= NULL' use default evaluation */
	rec_handle rh;  /* recording handle; if '=NULL' use root recording */

	short use_class;      /* use 'heartbeat' event-class stored in option 'clh' */
	class_handle clh;    /* handle to the event-class holding the events which should be processed */

	short use_start_end_pos; /* use events between start_pos and end_pos */
	long start_pos; /* start-pos in sample-units */
	long end_pos; /* end-pos in sample-units */

	long num_ignore_events; /* number of events which should be ignored */
	long *ignore_events; /* list of events to ignore */

	short save_in_class;  /* flag if the results should be saved in the event-class */
	long num_data;       /* number of heartbeats */
	long *data;          /* positions of the heartbeats [su] */
	long *annot;         /* annotations of the heartbeats */
	long *flags;         /* ecg related flags */
	long data_is_pos;    /* flag if values in 'data' are event-id's (=0) or sample positions (=1) */

	long min_beat_distance;  /* minimum distance of heartbeats [msec]; below this distance, beats will be combined */

	/* options for calc of temporal setting of rri */
	long num_ref_rri;	      /* number rri used for calculation of reference-value */
	long ref_normal_percent;      /* percent within the previous rri will be uses as reference-value */
	long normal_percent;	      /* max. tolerance for being a normal rri in percent */
	long normal_pre_percent;
	long normal_late_percent;
	long max_abs_difference;
	long vpc_pre_percent;	      /* max. percent of ref-value to be premature vpc */
	long vpc_late_percent;	      /* min. percent of ref-value to be a "real"
					 compensatory pause */
	long min_interp_vpc_percent;
	long max_interp_vpc_percent;
	long time2get_new_ref_value;  /* value in sec; if in the last time2get.. sec
					 no normal rri show up, a new ref-value must
					 be calculated */
	long min_rri;		      /* minimum value of a valid rr-interval */
	long max_rri;		      /* maximum value of a valid rr-interval (only used for heart rate calculations) */
	long pause;		      /* rri at which a pause begins */

	short do_summarize;	      /* flag if a summary of beats and arrhythmias should be created and stored in the results */

	short get_respiration_values; /* flag if respiration related values have to be assessed */
	short skip_artifact_detection; /* flag if own artifact detection should be skipped */
}; /* struct ra_ecg_options */


/* -------------------- globals -------------------- */
/* ---------- beat classification texts ---------- */
static const char *ra_ecg_baseclass2string[] = {
	"N",
	"V",
	"P",
	"X",
	"?",
}; /* ra_ecg_baseclass2string */

static const char *ra_ecg_class2string[] = {
	"N",
	"S",
	"V?",
	"N",
	"sV",
	"pV",
	"fV",
	"iV",
	"eV",
	"X1",
	"X2",
	"X3",
	"aP",
	"vP",
	"avP",
	"?",
}; /* ra_ecg_class2string */

#ifdef WIN32
_inline
#else
inline
#endif
const char *
raecg_get_baseclass_string(long c)
{
        if (IS_SINUS(c))
                return ra_ecg_baseclass2string[0];
	else if (IS_VENT(c))
                return ra_ecg_baseclass2string[1];
	else if (IS_PACED(c))
                return ra_ecg_baseclass2string[2];
	else if (IS_ARTIFACT(c))
                return ra_ecg_baseclass2string[3];
	else if (IS_ARTIFACT(c))
                return ra_ecg_baseclass2string[3];
	else
                return ra_ecg_baseclass2string[4];
} /* raecg_get_baseclass_string() */


#ifdef WIN32
_inline
#else
inline
#endif
const char*
raecg_get_class_string(long c)
{
	if (GET_SUBCLASS(c) == 0)
                return raecg_get_baseclass_string(c);
        else if (IS_NORMAL(c))
                return ra_ecg_class2string[0];
        else if (IS_SVPC(c))
                return ra_ecg_class2string[1];
        else if (IS_NORM_VPC(c))
                return ra_ecg_class2string[2];
        else if (IS_PAUSE(c))
                return ra_ecg_class2string[3];
        else if (IS_SINGLE_VPC(c))
                return ra_ecg_class2string[4];
        else if (IS_SINGLE_PREM_VPC(c))
                return ra_ecg_class2string[5];
        else if (IS_SINGLE_PREM_COMP_VPC(c))
                return ra_ecg_class2string[6];
        else if (IS_INTERP(c))
                return ra_ecg_class2string[7];
        else if (IS_ESCAPE(c))
                return ra_ecg_class2string[8];
        else if (IS_TYPE1_ARTIFACT(c))
                return ra_ecg_class2string[9];
        else if (IS_TYPE2_ARTIFACT(c))
                return ra_ecg_class2string[10];
        else if (IS_TYPE3_ARTIFACT(c))
                return  ra_ecg_class2string[11];
        else if (IS_ATRIAL_PACED(c) && !IS_VENTRICLE_PACED(c))
                return  ra_ecg_class2string[12];
        else if (IS_VENTRICLE_PACED(c) && !IS_ATRIAL_PACED(c))
                return  ra_ecg_class2string[13];
        else if (IS_ATRIAL_PACED(c) && IS_VENTRICLE_PACED(c))
                return  ra_ecg_class2string[14];
        else
                return ra_ecg_class2string[15];
} /* raecg_get_class_string() */


/* ---------- arrhyhthmic texts ---------- */
struct _ecg_arr_info
{
	long type;
	char name[EVAL_MAX_NAME];
	char desc[EVAL_MAX_DESC];
}; /* struct _ecg_arr_info */


static struct _ecg_arr_info ra_ecg_arr_info[] =
{
	{ ECG_ARR_MAXHR, gettext_noop("min-hr"), gettext_noop("maximal heart rate") },
	{ ECG_ARR_MINHR, gettext_noop("arr_min.hr"), gettext_noop("minimum heart rate") },
	{ ECG_ARR_BRADY, gettext_noop("arr_brady"), gettext_noop("Bradycardia") },
	{ ECG_ARR_SSALVO, gettext_noop("arr_ssalvo"), gettext_noop("supra ventricular salvo (hr < 100bpm)") },
	{ ECG_ARR_SVT, gettext_noop("arr_svt"), gettext_noop("Tachycardia (Supra Ventricular Tachycardia)") },
	{ ECG_ARR_SSALVO_SVT, gettext_noop("arr_ssalvo+svt"), gettext_noop("supra ventricular salvos AND supra ventricular tachycardias") },
	{ ECG_ARR_AFIB, gettext_noop("arr_afib"), gettext_noop("atrial fibrilation") },
	{ ECG_ARR_IRREG, gettext_noop("arr_irregular"), gettext_noop("irregular rhyhtm") },
	{ ECG_ARR_COUPLET, gettext_noop("arr_couplet"), gettext_noop("Couplets") },
	{ ECG_ARR_TRIPLET, gettext_noop("arr_triplet"), gettext_noop("Triplets") },
	{ ECG_ARR_BIGEMINUS, gettext_noop("arr_bigeminus"), gettext_noop("Bigeminus") },
	{ ECG_ARR_TRIGEMINUS, gettext_noop("arr_trigeminus"), gettext_noop("Trigeminus") },
	{ ECG_ARR_IVRS, gettext_noop("arr_ivrs"), gettext_noop("ideo ventricular rhythm") },
	{ ECG_ARR_SALVO, gettext_noop("arr_salvo"), gettext_noop("ventricular salvo (hr < 100bpm)") },
	{ ECG_ARR_VT, gettext_noop("arr_vt"), gettext_noop("ventricular tachycardia") },
	{ ECG_ARR_SALVO_VT, gettext_noop("arr_salvo+vt"), gettext_noop("ventricular salvos AND ventricular tachycardias") }
}; /* ra_ecg_arr_info */


#ifdef WIN32
_inline
#else
inline
#endif
const char *
raecg_get_arr_name(long type)
{
	const char *name = NULL;
	long l;
	long n = sizeof(ra_ecg_arr_info) / sizeof(ra_ecg_arr_info[0]);

	for (l = 0; l < n; l++)
	{
		if (type == ra_ecg_arr_info[l].type)
		{
			name = ra_ecg_arr_info[l].name;
			break;
		}
	}

	return name;
} /* raecg_get_arr_name() */


#ifdef WIN32
_inline
#else
inline
#endif
const char *
raecg_get_arr_desc(long type)
{
	const char *desc = NULL;
	long l;
	long n = sizeof(ra_ecg_arr_info) / sizeof(ra_ecg_arr_info[0]);

	for (l = 0; l < n; l++)
	{
		if (type == ra_ecg_arr_info[l].type)
		{
			desc = ra_ecg_arr_info[l].desc;
			break;
		}
	}

	return desc;
} /* raecg_get_arr_desc() */


#ifdef __cplusplus
}
#endif

#endif /* _RA_ECG_H */
