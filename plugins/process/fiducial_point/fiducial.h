/*----------------------------------------------------------------------------
 * fiducial.h
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002-2005, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifndef _FIDUCIAL_H
#define _FIDUCIAL_H

#include <ra.h>
#include <ra_fiducial_point.h>

/* -------------------- defines -------------------- */
#define PLUGIN_VERSION  "0.3.0"
#define PLUGIN_NAME     "fiducial-point"
#define PLUGIN_DESC     gettext_noop("finds fiducial points")
#define PLUGIN_TYPE     PLUGIN_PROCESS
#define PLUGIN_LICENSE  LICENSE_LGPL


enum result_idx
{
	POS
};

/* -------------------- structures -------------------- */
struct info
{
	double x_scale;

	long num;
	long *pos;
	long *index;
	
	prop_handle ph_pos;
}; /* struct infos */


/* -------------------- globals -------------------- */
static struct ra_option_infos options[] = {
	{"use_class", gettext_noop("align events stored in option 'clh'"), RA_VALUE_TYPE_SHORT, -1, 0},
	{"clh", gettext_noop("handle to the event-class holding the events which should be processed"),
	 RA_VALUE_TYPE_VOIDP, -1, 0},
	{"pos_prop", gettext_noop("ASCII-id of the event-property holding the positions (optional)"),
	 RA_VALUE_TYPE_CHAR, -1, 0},
	{"save_in_class", gettext_noop("flag if the fiducial-points should be saved in the event-class"),
	 RA_VALUE_TYPE_SHORT, -1, 0},
	{"num_data", gettext_noop("number of event-id's/sample-positions"), RA_VALUE_TYPE_LONG, -1, 0},
	{"data", gettext_noop("event-id's/sample-positions"), RA_VALUE_TYPE_LONG_ARRAY, -1, -1},
	{"data_is_pos", gettext_noop("flag if values in 'data' are event-id's (=0) or sample positions (=1)"),
	 RA_VALUE_TYPE_SHORT, -1, 0},
	{"ch", gettext_noop("channel where fiducial point should be searched"), RA_VALUE_TYPE_LONG, -1, 0},
	{"win_len", gettext_noop("area (+-'win_len') where the fiducial-point will be searched [sec]"),
	 RA_VALUE_TYPE_DOUBLE, -1, 0},
	{"corr_len", gettext_noop("length of correlation [sec]"), RA_VALUE_TYPE_DOUBLE, -1, 0}
};
long num_options = sizeof(options) / sizeof(options[0]);


static struct ra_result_infos results[] = {
	{"pos", gettext_noop("positions after the alignment"), RA_VALUE_TYPE_LONG_ARRAY, -1},
};
long num_results = sizeof(results) / sizeof(results[0]);


/* -------------------- prototypes -------------------- */
int pl_call_gui(proc_handle proc);
proc_handle pl_get_proc_handle(plugin_handle pl);
void set_default_options(struct ra_fiducial_point *opt);
void set_option_offsets(struct ra_option_infos *opt_inf, struct ra_fiducial_point *opt);
void pl_free_proc_handle(proc_handle proc);
int pl_do_processing(proc_handle proc);

int get_data(struct proc_info *pi, struct info *i);
int get_data_from_eval(struct proc_info *pi, struct info *i);
int get_data_from_ev_prop(struct info *i);
int get_data_from_ev_class(struct proc_info *pi, struct info *i);
int get_data_from_opt(struct proc_info *pi, struct info *i);

int find_fiducial(struct proc_info *pi, long num, long *pos);
int get_orientation(double *buf, int size, int *minmax_pos);

int save_results(struct proc_info *pi, struct info *i);


#endif /* _FIDUCIAL_H */
