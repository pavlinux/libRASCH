/*----------------------------------------------------------------------------
 * ra_prsa.h
 *
 *
 * Author(s): Raphael Schneider (librasch@gmail.com)
 *
 * Copyright (C) 2003-2007, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifndef RA_PRSA_H
#define RA_PRSA_H

#pragma pack(1)

struct ra_prsa
{
	/* selection of input-data */
	/* prsa can be performed on events (e.g. rr-intervals) or on the signal raw data */
	eval_handle eh; /* evaluation used; if '= NULL' use default evaluation */

	short use_class;  /* use data from the event-class given in 'clh' (!=0)
			     or use signal raw data (==0) */
	class_handle clh; /* event class with the data used for the calculations */

	short link_two_data_streams; /* prsa can be used to 'link' two data-streams
				       !=0: 'sort' first data-stream according the second data-stream */

	short use_event_pos; /* flag if positions should be used from the events (=1) or from 'prop_pos' (=0) */
	char *prop_pos; /* when using events, the position of the events is needed */
	long pos_ch;

	/* use events (if use_events != 0) which are between start-pos-sample and end-pos-sample
	   or use the signal raw data (if use_events == 0) between start-pos-sample and end-pos-sample */
	short use_start_end_pos;
	long start_pos;
	long end_pos;

	/* when using events, the following options will be used */
	/* name of event-properties */
	char *prop_name_1;
	char *prop_name_2; /* this will only be used if link of two data-streams
				   should be calculated */

	/* use events between start-event and end-event (including both) */
	short use_start_end_event;
	long start_event;
	long end_event;
	/* use events listed */
	long num_events;
	long *events;

	/* infos if some regions and/or events should be ignored */
	short ignore_marked_regions;  /* ignore regions which are marked in the annotations with the IGNORE flag */
	short ignore_noise_regions;   /* ignore regions which are marked in the annotations with the NOISE flag */
	/* events to ignore */
	long num_ignore_events;
	long *ignore_events;
	/* flag if ignore-regions/-values are cut-out(=0) or interpolated(=1) */
	short interpolate_ignore_values;

	/* when using signal raw data, the following options will be used */
	rec_handle rh;
	long ch_num_1;
	long ch_num_2;
	short use_ignore_value_1;
	double ignore_value_1;	/* this value will be not used for PRSA (if use_ignore_value is != 0) */
	short use_ignore_value_2;
	double ignore_value_2;

	/* if no selection criteria is used, all events/the complete signal raw data will be used */

	/* options for an optional filter applied on the data directly before PRSA */
	short apply_boxcar_filter; /* flag if a boxcar-filter should be applied on the data directly before PRSA */
	long boxcar_filter_width; /* width of the boxcar-filter in data-units */

	/* options regarding calculation (independent of used input-data) */
	short skip_freq; /* flag if calculation of PRS should be skiped */
	short use_complete_only; /* use only segments when all values are valid */
	short use_time;		/* flag if PRA is time (!= 0) or event (e.g. rr-interval) (= 0) based */
	double pra_samplerate;	/* samplerate used for PRA */
	double pra_filter_width;	/* filter width used for PRA (time[sec] or #events) */
	double pra_window_width;	/* window width of one side used for PRA (time[sec] or #events) */

	short pra_filter_min_value_percent; /* flag if minimal filter value is percentage (=1) or msec (=0) value */
	short pra_filter_max_value_percent; /* flag if maximal filter value is percentage (=1) or msec (=0) value */
	double pra_filter_min;  /* minimal value of filter acceptance */
	double pra_filter_max;  /* maximal value of filter acceptance */
	int pra_filter_dir_pos; /* If pra_filter_min==ra_filter_max than only the "direction"
	                           of the change is used. This parameter shows which directions
	                           should be used as trigger (!=0: positive-  ==0: negative-changes) */
	long prs_length;  /* if more than length of PRA: length of PRS */

	short save_in_eval; /* save results in eval-file (not supported yet) */
}; /* struct ra_prsa */

#pragma pack()

#endif /* RA_PRSA_H */
