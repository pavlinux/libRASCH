/**
 * \file ra_template.h
 *
 * The header file contains settings when performing a template match
 * using the template plugin.
 *
 * \author Raphael Schneider (rasch@med1.med.tum.de)
 */

/*----------------------------------------------------------------------------
 *
 * Copyright (C) 2002-2007, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *
 *--------------------------------------------------------------------------*/

#ifndef _RA_TEMPLATE_H
#define _RA_TEMPLATE_H

#include <ra_eval.h>
#include <ra_ch_list.h>


struct ra_template
{
	rec_handle rh;       /* recording handle of the data which should be processed */
	sum_handle sh;       /* event summary handle of already available template event-class */

	short use_class;      /* align ALL events stored in option 'clh' */
	class_handle clh;    /* handle to the event-class holding the events which should be processed */
	char *pos_prop;      /* ASCII-id of the event-property holding the positions (optional) */
	long pos_ch;         /* channel-number 'pos_prop' belongs to (only needed when 'pos_prop' is used) */
	short save_in_class;  /* flag if the fiducial-points should be saved in the event-class */

	long num_data;       /* number of event-id's/sample-positions */
	long *data;          /* event-id's/sample-positions */
	long data_is_pos;    /* flag if values in 'data' are event-id's (=0) or sample positions (=1) */

	char *templ_name;    /* name of event-type which holds template numbers */
	char *templ_corr;    /* name of event-type which holds template correlation */

	/* size of the window around a single event which is used for the correlation
	   with the templates (in seconds) */
	double corr_win_before;
	double corr_win_after;
	double corr_step;    /* step used for moving the correlation window over the template (in seconds);
			        when '-1', one sampleunit is used as step-size, which is highly recommended
				because larger steps will blur the template */
	
	double accept;	     /* treshold which correlation values are accepted */
	/* slope of regression-line must be between the next tresholds */
	double slope_accept_low;
	double slope_accept_high;

	/* size of template-window in seconds */
	double template_win_before;
	double template_win_after;
	/* channels for which the template should be created */
	long num_ch;
	long *ch;

	short align_events;  /* flag if events should be aligned on the corresponding template */
}; /* struct ra_template */


#endif /* _RA_TEMPLATE_H */
