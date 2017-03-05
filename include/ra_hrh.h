/*----------------------------------------------------------------------------
 * ra_hrh.h
 *
 *
 * Author(s): Raphael Schneider (librasch@gmail.com)
 *
 * Copyright (C) 2003-2009, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifndef RA_HRH_H
#define RA_HRH_H

#pragma pack(1)

struct ra_hrh
{
	eval_handle eh;	/* evaluation used; if '= NULL' use default evaluation */

	/* selection of input-data */
	short use_class; /* use data from the event-class given in 'clh' */
	class_handle clh; /* event class with the data used for the calculations */

	/* use events which are between start-pos-sample and end-pos-sample */
	short use_start_end_pos;
	long start_pos;
	long end_pos;

	long num_ignore_events; /* number of events which should be ignored */
	long *ignore_events; /* list of events to ignore */

	/* flag if only normal RR intervals should be used */
	short use_only_normal_rri;
	/*  flag if VPC related RR intervals should be removed */
	short dont_use_vent_rri;
	/* flag if PAC related RR intervals should be removed */
	short dont_use_pac_rri;

	/* options regarding calculation (independent of used input-data) */
	short skip_freq; /* flag if calculations of freq-domain parameters should be skipped */
	short use_complete_only; /* use only segments when all values are valid */
	short use_time;		/* flag if PRA is time (!= 0) or event (e.g. rr-interval) (= 0) based */
	double pra_samplerate;	/* samplerate used for PRA */
	double pra_filter_width;	/* filter width used for PRA (time[sec] or #events) */
	double pra_window_width;	/* window width of one side used for PRA (time[sec] or #events) */

	short pra_use_min_max_values;   /* flag if filter min/max values should be used (=1) or 'anchor_filter_width' */
	short pra_filter_min_value_percent; /* flag if minimal filter value is percentage (=1) or msec (=0) value */
	short pra_filter_max_value_percent; /* flag if maximal filter value is percentage (=1) or msec (=0) value */
	double pra_filter_min;  /* minimal value of filter acceptance */
	double pra_filter_max;  /* maximal value of filter acceptance */

	double anchor_filter_width; /* width of anchor-selection filter for DC and AC calculation */
	double capacity_width;  /* width of Haar wavelet used for capacity calculations (time[sec] or #events) */

	short save_in_eval;	/* save results in eval-file (not supported yet) */
}; /* struct ra_hrh */

#pragma pack()

#endif /* RA_HRH_H */
