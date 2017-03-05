/*----------------------------------------------------------------------------
 * ecg_view.h
 *
 * Author(s): Raphael Schneider (librasch@gmail.com)
 *
 * Copyright (C) 2002-2008, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifndef _ECG_VIEW_H
#define _ECG_VIEW_H

#define _LIBRASCH_BUILD
#include <ra.h>
#include <ra_priv.h>
#include <pl_general.h>
#include <ra_pl_comm.h>

#include "ecg_view_general.h"
#include "../shared/view_frame_general.h"

#define PLUGIN_VERSION  "0.2.0"
#define PLUGIN_NAME     "ecg-view"
#define PLUGIN_DESC     gettext_noop("view for ecg's")
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
	LIBRAAPI int init_ra_plugin(struct ra *m, struct plugin_struct *ps);
	LIBRAAPI int fini_ra_plugin(void);
}

view_handle pl_get_view_handle(plugin_handle pl);
/* void set_default_options(struct ra_ev_sum_view *opt); */
/* void set_option_offsets(struct ra_option_infos *opt_inf, struct ra_ev_sum_view *opt); */
void pl_free_view_handle(view_handle vih);
int pl_create_view(view_handle vih);

void pl_handle_signal(meas_handle mh, dest_handle dh, const char *sig_name,
		      int num_para, struct comm_para *p);
int pl_check_channels(rec_handle rh, long ch);
int pl_check_plugin_usage(const char *pl_name);

// slots
void call_sort(ecg_view_general *v, int n, struct comm_para *p);
void eval_change(meas_handle mh, ecg_view_general *v);
void event_change(ecg_view_general *v, int n, struct comm_para *p);
void pos_change(ecg_view_general *v, view_frame_general *f, int n, struct comm_para *p);
void init_scrollbar(view_frame_general *f, int n, struct comm_para *p);
void gui_event(ts_view_general *v, bool start);
void highlight_event(ecg_view_general *v, int n, struct comm_para *p);
void x_resolution(ecg_view_general *v, int n, struct comm_para *p);
void input_focus_change(ecg_view_general *v, int n, struct comm_para *p);
void save_settings(ecg_view_general *v, int n, struct comm_para *p);
void load_settings(ecg_view_general *v, int n, struct comm_para *p);


#endif // _ECG_VIEW_H
