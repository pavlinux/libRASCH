/*----------------------------------------------------------------------------
 * ra_view.h
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002-2006, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifndef _RA_VIEW_H
#define _RA_VIEW_H

#define _LIBRASCH_BUILD
#include <ra.h>
#include <ra_priv.h>
#include <pl_general.h>
#include <ra_pl_comm.h>

#include "ra_view_general.h"
#include "../shared/view_frame_general.h"


#define PLUGIN_VERSION  "0.3.0"
#define PLUGIN_NAME     "rasch-view"
#define PLUGIN_DESC     gettext_noop("general view for libRASCH, handling all views")
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

void *create_view(struct view_info *vi, rec_handle rh, void **real_view);
void pos_change(view_frame_general *f, int n, struct comm_para *p);
void init_scrollbar(view_frame_general *v, int n, struct comm_para *p);


#endif // _RA_VIEW_H
