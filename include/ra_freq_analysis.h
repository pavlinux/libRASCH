/*----------------------------------------------------------------------------
 * ra_freq_analysis.h
 *
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2003-2005, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifndef RA_FREQ_ANALYSIS_H
#define RA_FREQ_ANALYSIS_H

struct ra_freq_analysis
{
	/* selection of input-data */
	/* options when event-properties should be processed */
	short use_events; /* !=0: use events, ==0: use signal raw data */
	class_handle clh; /* event class of the events */
	char *prop_value; /* use these event-property for frequency analysis */
	short use_event_pos; /* flag if positions should be used from the events (=1) or from 'prop_pos' (=0) */
	char *prop_pos; /* when using events, the position of the events is needed for time based analysis */
	short use_time; /* flag if freq. analysis should be done based on time(!=0)
			   or based on the event number (=0) (e.g. RR-interval) */

	long num_events; /* use the events listed in events */
	long *events; /* list of events to use */

	long num_ignore_events; /* number of events which should be ignored */
	long *ignore_events; /* list of events to ignore */

	/* options when raw signal data should be processed */
	rec_handle rh;  /* recording handle */
	long ch; /* channel which should be used */
	short use_ignore_value; /* flag if there are values which should not be used for freq. analysis */
	double ignore_value; /* this value will be not used for freq. analysis (if use_ignore_value_1 is != 0) */

	/* options for event-based and raw-based data */
	short use_start_end_pos; /* use raw-data/events between start_pos and end_pos */
	long start_pos; /* start-pos in sample-units */
	long end_pos; /* end-pos in sample-units */
	
	/* options when data is given directly to the plugin (has precedence of the other options (events or raw-data) */
	long num_values; /* number of the data given in 'data' */
	double *values; /* perform frequency analysis using this data */
	double *pos; /* perform frequency analysis using this data */
	double pos_samplerate; /* samplerate of the position values */

	/* options independent of source type (event/raw/data) */
	double samplerate;  /* samplerate to use when freq.analysis should be done time based */

	/* pre-processing options */
	short smooth_data;	/* flag if input-data should be smoothed */
	char *smooth_method;	/* at the moment only a boxcar-filter is available */
	double smooth_width;	/* width of the smooth-filter (TODO: think how parameters can be handled for different methods) */

	/* frequency analysis specifc options */
	char *window;  /* window function used */
	short remove_mean;  /* remove the mean value before the frequency analysis */
	short remove_trend;  /* remove the trend (first order) before the frequency analysis */
	char *method;  /* method used for the frequency analysis */
	long num_freq;  /* number of frequency components: if lower than number of input values
	                   it is ignored; if higher, zeros are added */

	/* post-processing options */
	short smooth_spec;  /* flag if spectrum should be smoothed (TODO: think about possible post-processing methods) */

	short save_in_eval; /* save results in the evaluation file (not supported yet) */
}; /* struct ra_freq_analysis */

#endif /* RA_FREQ_ANALYSIS_H */
