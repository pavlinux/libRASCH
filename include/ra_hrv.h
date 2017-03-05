/*----------------------------------------------------------------------------
 * ra_hrv.h
 *
 *
 * Author(s): Raphael Schneider (ra@med1.med.tum.de)
 *
 * Copyright (C) 2003-2006, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifndef RA_HRV_H
#define RA_HRV_H


struct ra_hrv
{
	eval_handle eh;	/* evaluation used; if '= NULL' use default evaluation */
	
	short use_class; /* use data from the event-class given in 'clh' */
	class_handle clh; /* event class with the data used for the calculations */
	long ch; /* channel being the source of the values which should be used for the calculations */
	char *prop_value; /* event property holding the values used for the calculations */
	short use_event_pos; /* flag if positions should be used from the events (=1) or from 'prop_pos' (=0) */
	char *prop_pos; /* event property holding the positions of the values (needed for freq. analysis) */
	char *prop_annot; /* event property with the beat annotation */
	char *prop_temporal; /* event property with the temporal behaviour of the beats */
	short ignore_annot; /* ignore beat annotations and use all beats */
	
	short use_start_end_pos; /* use events between start_pos and end_pos */
	long start_pos; /* start-pos in sample-units */
	long end_pos; /* end-pos in sample-units */

	long num_events; /* use the events listed in events */
	long *events; /* list of events to use */

	long num_ignore_events; /* number of events which should be ignored */
	long *ignore_events; /* list of events to ignore */

	long num_values; /*  */
	double *values; /*  */
	double *pos; /*  */
	double samplerate; /*  */

	short use_min_max; /* check if values are between 'min_value' and 'max_value' */
	double min_value; /* minimum allowed value */
	double max_value; /* maximum allowed value */

	short use_user_freq_band; /* calculate also the power in the user-selected frequency band */
	double user_freq_start; /* start of user-selected frequency band [Hz] */
	double user_freq_end; /* end of user-selected frequency band [Hz] */

	/* options for Poincare analysis */
	long poincare_lag;	/* lag used for Poincare plots */

	/* options for Detrended Fluctuation analysis */
	short dfa_use_only_nn;	/* use only normal-to-normal intervals for DFA */
	long dfa_segment_size;	/* segment size used for DFA (= 0: use complete signal) */
	long dfa_alpha1_alpha2_cut; /* box-size used for calculation of DFA alpha-1 and -2
				       (value is upper limit used for alpha-1, incl. the value) */

	short dfa_use_user_range; /* flag if dfa-alpha should be calculated between a user-defined range */
	long dfa_user_lower_limit; /* lower limit of the DFA user-defined range (incl. the limit value) */
	long dfa_user_upper_limit; /* upper limit of the DFA user-defined ragne (incl. the limit value) */

	short skip_time_domain; /* do not calculate HRV in the time domain */
	short skip_freq_domain; /* do not calculate HRV in the frequency domain */
	short skip_nld_analysis;	/* do not calculate non-linear dynamic HRV parameters */

	short save_in_eval; /* save results in eval-file (not supported yet) */
}; /* struct ra_hrv */


#endif /* RA_HRV_H */
