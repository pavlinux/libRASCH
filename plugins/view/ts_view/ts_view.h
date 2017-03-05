/*----------------------------------------------------------------------------
 * ts_view.h
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002-2006, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifndef _TS_VIEW_H
#define _TS_VIEW_H


#define _LIBRASCH_BUILD
#include <ra.h>
#include <ra_priv.h>
#include <pl_general.h>

#include "../shared/view_frame_general.h"

#define PLUGIN_VERSION  "0.5.0"
#define PLUGIN_NAME     "ts-view"
#define PLUGIN_DESC     gettext_noop("view for time-series")
#define PLUGIN_TYPE     PLUGIN_VIEW
#ifdef QT
#define PLUGIN_LICENSE  LICENSE_GPL
#else
#define PLUGIN_LICENSE  LICENSE_LGPL
#endif


// -------------------- prototypes --------------------
extern "C"
{
	LIBRAAPI int load_ra_plugin(struct plugin_struct *ps);
	LIBRAAPI int init_ra_plugin(struct librasch *ra, struct plugin_struct *ps);
	LIBRAAPI int fini_ra_plugin(void);
}


view_handle pl_get_view_handle(plugin_handle pl);
/* void set_default_options(struct ra_ev_sum_view *opt); */
/* void set_option_offsets(struct ra_option_infos *opt_inf, struct ra_ev_sum_view *opt); */
void pl_free_view_handle(view_handle vih);
int pl_create_view(view_handle vih);

int pl_check_channels(rec_handle rh, long ch);
int pl_check_plugin_usage(const char *pl_name);

void pl_handle_signal(meas_handle mh, dest_handle dh, const char *sig_name,
		      int num_para, struct comm_para *p);
void pos_change(ts_view_general *v, view_frame_general *f, int n, struct comm_para *p);
void gui_event(ts_view_general *v, bool start);
void x_resolution(ts_view_general *v, int n, struct comm_para *p);
void init_scrollbar(view_frame_general *f, int n, struct comm_para *p);
void eval_change(ts_view_general * v);
void input_focus_change(ts_view_general *v, int n, struct comm_para *p);
void save_settings(ts_view_general *v, int n, struct comm_para *p);
void load_settings(ts_view_general *v, int n, struct comm_para *p);

#endif // _TS_VIEW_H
