/*----------------------------------------------------------------------------
 * template.h
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002-2007, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifndef _TEMPLATE_H
#define _TEMPLATE_H

#include <ra.h>
#include <ra_template.h>

/* -------------------- defines -------------------- */
#define PLUGIN_VERSION  "0.4.0"
#define PLUGIN_NAME     "template"
#define PLUGIN_DESC     gettext_noop("combine events in templates")
#define PLUGIN_TYPE     PLUGIN_PROCESS
#define PLUGIN_LICENSE  LICENSE_LGPL


/* -------------------- structures -------------------- */
struct template_single
{
	double **data;
	double **data_sum;

	int fiducial_offset;	/* pos. of fiducial value in template (-1 if none) */
	int type;		/* e.g. classification when using ecg */

	long num_events;	/* number of events with this template */
	long *ev_ids;
	long buf_size;  	/* size of 'ev_ids' (to avoid mem-alloc for each new event) */

	int new_template;       /* when re-assessing templates or processing single beats,
				   this flag shows if a new template was found and that the
				   new one has to be saved in the evaluation (if wanted) */
	long part_id;           /* part-id of the event-summary (valid when no new_template) */
}; /* struct template_single */


struct info
{
	int n_temp;
	struct template_single *ts;

	class_handle clh;
	prop_handle posh;
	sum_handle sh;

	/* some temporary variables for speedup */
	double *samplerate;	/* for each channel */
	long *templ_win_before;
	long *templ_win_after;
	long *templ_win;
	long *corr_win_before;
	long *corr_win_after;
	long *corr_win;
	long *corr_step;

	long *corr_offset;
	double **corr_buf;
}; /* struct info */


struct event_templ_info
{
	long event_id;
	long pos;
	long pos_orig;

	int template;
	double correlation;
}; /* struct event_templ_info */


/* -------------------- globals -------------------- */
static struct ra_option_infos options[] = {
	{"rh", gettext_noop("recording handle of the data which should be processed"), RA_VALUE_TYPE_VOIDP, -1, 0},
	{"sh", gettext_noop("event summary handle of already available template event-class"), RA_VALUE_TYPE_VOIDP, -1, 0},
	
	{"use_class", gettext_noop("align events stored in option 'clh'"), RA_VALUE_TYPE_SHORT, -1, 0},
	{"clh", gettext_noop("handle to the event-class holding the events which should be processed"),
	 RA_VALUE_TYPE_VOIDP, -1, 0},
	{"pos_prop", gettext_noop("ASCII-id of the event-property holding the positions (optional)"),
	 RA_VALUE_TYPE_CHAR, -1, 0},
	{"pos_ch", gettext_noop("channel-number 'pos_prop' belongs to (only needed when 'pos_prop' is used)"),
	 RA_VALUE_TYPE_LONG, -1, 0},
	{"save_in_class", gettext_noop("flag if the fiducial-points should be saved in the event-class"),
	 RA_VALUE_TYPE_SHORT, -1, 0},
	{"num_data", gettext_noop("number of event-id's/sample-positions"), RA_VALUE_TYPE_LONG, -1, 0},
	{"data", gettext_noop("event-id's/sample-positions"), RA_VALUE_TYPE_LONG_ARRAY, -1, -1},
	{"data_is_pos", gettext_noop("flag if values in 'data' are event-id's (=0) or sample positions (=1)"),
	 RA_VALUE_TYPE_SHORT, -1, 0},

	{"templ_name", gettext_noop("name of event-type which holds template numbers"), RA_VALUE_TYPE_CHAR, -1, 0},
	{"templ_corr", gettext_noop("name of event-type which holds template correlation"), RA_VALUE_TYPE_CHAR, -1, 0},

	/* size of the window around a single event which is used for the correlation
	   with the templates (in seconds) */
	{"corr_win_before", gettext_noop("correlation window size before the reference position [sec]"),
	 RA_VALUE_TYPE_DOUBLE, -1, 0},
	{"corr_win_after", gettext_noop("correlation window size after the reference position [sec]"),
	 RA_VALUE_TYPE_DOUBLE, -1, 0},
	{"corr_step", gettext_noop("step used for moving the correlation window over the template (in seconds);\n"
				   "when '-1', one sampleunit is used as step-size, which is highly recommended\n"
				   "because larger steps will blur the template"), RA_VALUE_TYPE_DOUBLE, -1, 0},

	{"accept", gettext_noop("treshold which correlation values are accepted"), RA_VALUE_TYPE_DOUBLE, -1, 0},
	{"slope_accept_low", gettext_noop("min-value of slope of regression line"), RA_VALUE_TYPE_DOUBLE, -1, 0},
	{"slope_accept_high", gettext_noop("max-value of slope of regression line"), RA_VALUE_TYPE_DOUBLE, -1, 0},

	{"template_win_before", gettext_noop("template window size before postion (in seconds)"), RA_VALUE_TYPE_DOUBLE, -1, 0},
	{"template_win_after", gettext_noop("template window size after position (in seconds)"), RA_VALUE_TYPE_DOUBLE, -1, 0},

	{"num_ch", gettext_noop("number of channels used for template creation"), RA_VALUE_TYPE_LONG, -1, 0},
	{"ch", gettext_noop("list of channels for which the template should be created"), RA_VALUE_TYPE_LONG_ARRAY, -1, -1},

	{"align_events", gettext_noop("flag if events should be aligned on the corresponding template"),
	 RA_VALUE_TYPE_SHORT, -1, 0},
};
long num_options = sizeof(options) / sizeof(options[0]);


/* -------------------- prototypes -------------------- */
int pl_call_gui(proc_handle proc);
proc_handle pl_get_proc_handle(plugin_handle pl);
void set_default_options(struct ra_template *opt);
void set_option_offsets(struct ra_option_infos *opt_inf, struct ra_template *opt);
void pl_free_proc_handle(proc_handle proc);
int pl_do_processing(proc_handle proc);
void _prepare_eval(eval_handle eh, struct ra_template *opt);

int get_summary_channels(struct ra_template *opt);

int get_data(struct proc_info *pi, struct event_templ_info **evi, long *num);
int get_data_from_class(struct ra_template *opt, struct event_templ_info **evi, long *num);
int get_all_events(struct ra_template *opt, prop_handle pos_ph, struct event_templ_info **evi, long *num);

int read_templates_from_eval(struct proc_info *pi, struct info *inf);

int get_templates(struct proc_info *pi, struct info *inf, struct event_templ_info *evi, long num);
int assign_template(struct proc_info *pi, struct info *inf, struct event_templ_info *evi);
int find_template(struct proc_info *pi, struct info *inf, long pos, int *templ_num, double *corr, long *offset);

double corr_template(struct info *inf, struct ra_template *opt, int templ, int ch, long *offset);
int calc_correlation(double *x, double *y, long len, double *corr);
int calc_regression(double *x, double *y, long len, double *offset, double *slope);
long get_median(long *values, unsigned long num_values);

int update_template(struct info *inf, struct ra_template *opt, long pos, int templ_num);
int new_template(struct info *inf, struct ra_template *opt, long pos);

int align_events(struct ra_template *opt, struct event_templ_info *evi, long num);

int save_templates(struct proc_info *pi, struct info *inf, struct event_templ_info *evi, long num_ev);


#endif /* _TEMPLATE_H */
