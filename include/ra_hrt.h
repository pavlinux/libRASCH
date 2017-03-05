/*----------------------------------------------------------------------------
 * ra_hrt.h
 *
 *
 * Author(s): Raphael Schneider (ra@med1.med.tum.de)
 *
 * Copyright (C) 2003-2006, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifndef RA_HRT_H
#define RA_HRT_H

/* possible trigger types */
#define RA_HRT_TRIGGER_DEFAULT                  0  /* = RA_HRT_TRIGGER_VENT_SINGLE_PREM_COMP */
#define RA_HRT_TRIGGER_SINUS                    1
#define RA_HRT_TRIGGER_VENT                     2
#define RA_HRT_TRIGGER_PACED                    3
#define RA_HRT_TRIGGER_ARTIFACT                 4
/* sinus sub-types */
#define RA_HRT_TRIGGER_NORMAL                  10
#define RA_HRT_TRIGGER_SVPC                    11 
#define RA_HRT_TRIGGER_NORM_VENT               12
#define RA_HRT_TRIGGER_PAUSE                   13
/* ventricular sub-types */
#define RA_HRT_TRIGGER_VENT_SINGLE             30 /* single VPC */
#define RA_HRT_TRIGGER_VENT_SINGLE_PREM        31 /* single premature VPC */
#define RA_HRT_TRIGGER_VENT_SINGLE_PREM_COMP   32 /* single premature VPC with compensatory pause */
#define RA_HRT_TRIGGER_VENT_INTERP             33 /* interponated VPC */
#define RA_HRT_TRIGGER_VENT_ESCAPE             34 /* ventricular escape beat */
/* artifact sub-types */
#define RA_HRT_TRIGGER_ARTIFACT_T1             90 /* artifact type 1 */
#define RA_HRT_TRIGGER_ARTIFACT_T2             91 /* artifact type 2 */
#define RA_HRT_TRIGGER_ARTIFACT_T3             92 /* artifact type 3 */
/* arrhythmia trigger types */
#define RA_HRT_TRIGGER_CPL                    100 /* 2 contiguous VPCs, 1st premature, 2nd with comp. pause */
#define RA_HRT_TRIGGER_SALVO                  101 /* at least 3 contiguous VPCs, 1st premature,
						     last with compensatory pause */

/* defines for the reason that "stopped" the search for normal rr-intervals */
#define RA_HRT_REASON_NONE               0
#define RA_HRT_REASON_NO_NORMAL_BEAT     1 /* rr-interval is no normal-to-normal interval */
#define RA_HRT_REASON_TEMPORAL           2 /* rr-interval is to much premature or late */
#define RA_HRT_REASON_RRI_BELOW_MIN      3 /* rr-interval is below allowed minimal value (300msec) */
#define RA_HRT_REASON_RRI_ABOVE_MAX      4 /* rr-interval is above allowed maximal value (2000msec) */

struct ra_hrt
{
	eval_handle eh; /* evaluation used; if '= NULL' use default evaluation */

	short use_class; /* use data from the event-class given in 'clh' */
	class_handle clh; /* event class with the data used for the calculations */
	long ch; /* channel being the source of the values which should be used for the calculations */
	char *prop_value; /* event property holding the values used for the calculations */
	short use_event_pos; /* flag if positions should be used from the events (=1) or from 'prop_pos' (=0) */
	char *prop_pos; /* event property holding the positions of the values */
	char *prop_annot; /* event property with the value annotation */
	char *prop_trigger_annot; /* event property with the trigger (beat) annotation */
	char *prop_temporal; /* event property with the temporal behaviour of the beats */

	short use_start_end_pos; /* use events between start_pos and end_pos */
	long start_pos; /* start-pos in sample-units */
	long end_pos; /* end-pos in sample-units */

	long num_ignore_events; /* number of events which should be ignored */
	long *ignore_events; /* list of events to ignore */

	/* type of trigger */
	long trigger;		/* see RA_HRT_TRIGGER_* above */

	/* normal beats necessary for complete tachograms */
	long rri_normal_before;
	long rri_normal_after;
	short use_complete;

	long num_trigger_to_use;
	long *trigger_to_use;

	/* Turbulence Onset (TO) */
	short to_use_single_tacho;
	long to_before_start;
	long to_before_num;
	long to_after_start;
	long to_after_num;

	/* Turbulence Slope (TS) */
	short ts_use_single_tacho;
	short ts_use_pos;
	long ts_slope_pos;
	long ts_before_start;
	long ts_before_num;
	long ts_after_start;
	long ts_after_num;
	long ts_regression_len;

	/* Correlation Coefficient (KS) */
	short ks_use_single_tacho;
	long ks_before_start;
	long ks_before_num;
	long ks_after_start;
	long ks_after_num;
	long ks_regression_len;

	short save_in_eval; /* save results in eval-file (not supported yet) */
};  /* struct ra_hrt */


#endif /* RA_HRT_H */
