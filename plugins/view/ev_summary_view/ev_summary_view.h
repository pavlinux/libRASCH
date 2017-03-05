/*----------------------------------------------------------------------------
 * ev_summary_view.h
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002-2006, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifndef _EV_SUMMARY_VIEW_H
#define _EV_SUMMARY_VIEW_H

#define PLUGIN_VERSION  "0.2.0"
#define PLUGIN_NAME     "ev-summary-view"
#define PLUGIN_DESC     gettext_noop("view for event summaries")
#define PLUGIN_TYPE     PLUGIN_VIEW
#ifdef QT
#define PLUGIN_LICENSE  LICENSE_GPL
#else
#define PLUGIN_LICENSE  LICENSE_LGPL
#endif

static struct ra_option_infos options[] = {
	{"sh", gettext_noop("event-summary-handle"), RA_VALUE_TYPE_VOIDP, 0, 0},
};
long num_options = sizeof(options) / sizeof(options[0]);


// -------------------- prototypes --------------------
extern "C"
{
	LIBRAAPI int load_ra_plugin(struct plugin_struct *ps);
	LIBRAAPI int init_ra_plugin(struct ra *m, struct plugin_struct *ps);
	LIBRAAPI int fini_ra_plugin(void);
}

view_handle pl_get_view_handle(plugin_handle pl);
void set_default_options(struct ra_ev_sum_view *opt);
void set_option_offsets(struct ra_option_infos *opt_inf, struct ra_ev_sum_view *opt);
void pl_free_view_handle(view_handle vih);
int pl_create_view(view_handle vih);

void pl_handle_signal(meas_handle, dest_handle dh, const char *sig_name,
		      int num_para, struct comm_para *p);

// Slots
void eval_change(ev_summary_view_general * v);
void x_res(ev_summary_view_general *v, long num_para, struct comm_para *p);
void y_res(ev_summary_view_general *v, long num_para, struct comm_para *p);


#endif // _EV_SUMMARY_VIEW_H
