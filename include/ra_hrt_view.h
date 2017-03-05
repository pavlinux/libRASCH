/*----------------------------------------------------------------------------
 * ra_hrt_view.h
 *
 *
 *
 * Author(s): Raphael Schneider (rasch@mytum.de)
 *
 * Copyright (C) 2007, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id: $
 *--------------------------------------------------------------------------*/

#ifndef RA_HRT_VIEW_H
#define RA_HRT_VIEW_H

#include <ra_hrt.h>


struct ra_hrt_view
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

	/* plot specific options */
	short show_grid;
	short show_legend;
	short show_single_segments;
};  /* struct ra_hrt_view */


#endif /* RA_HRT_VIEW_H */
