/*----------------------------------------------------------------------------
 * ra_plot.h
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002-2006, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifndef RA_PLOT_H
#define RA_PLOT_H


#define _LIBRASCH_BUILD
#include <ra.h>
#include <ra_priv.h>
#include <pl_general.h>
#include <ra_pl_comm.h>

#include "ra_plot_general.h"

#define PLUGIN_VERSION  "0.2.0"
#define PLUGIN_NAME     "plot-view"
#define PLUGIN_DESC     gettext_noop("plot evaluation data")
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

int pl_call_gui(any_handle h);

int pl_create_view(view_handle vih);

void pl_handle_signal(meas_handle mh, dest_handle dh, const char *sig_name,
		      int num_para, struct comm_para *p);
void highlight_event(ra_plot_general * v, int n, struct comm_para *p);
void eval_change(ra_plot_general * v);
void pos_change(ra_plot_general *v, int n, struct comm_para *p);

#endif // RA_PLOT_H
