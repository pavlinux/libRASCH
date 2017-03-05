/*----------------------------------------------------------------------------
 * ra_view.cpp  -  general view for libRASCH, handling all views
 *
 * Description:
 * This plugin is a general view-plugin, which tries to use for every
 * supported type of signal (eg ecg, bloodpressure) the corresponding
 * view-plugin. For ever channels which is not handled by a specific
 * view-plugin, the default plugin is used (eg ts-view for time-series
 * signals).
 *
 * Remarks:
 * Win32 specific: This view needs as parent a CMDIFrameWnd. The plugin
 * creates a CMDIChildWnd and this will be returned. (The other view plugins
 * don't create the CMDIChildWnd by themself, these plugins need a
 * CMDIChildWnd as parent.)
 *
 * Author(s): Raphael Schneider (librasch@gmail.com)
 *
 * Copyright (C) 2002-2008, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifdef QT
#include "ra_view_qt.h"
#include "../shared/view_frame_qt.h"
#elif GTKMM
#include "ra_view_gtk.h"
#include "../shared/view_frame_gtk.h"
#elif _AFXDLL
#include "stdafx.h"
#include "ra_view_mfc.h"
#else
#error "no GUI-environment selected"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ra_view.h"


LIBRAAPI int
load_ra_plugin(struct plugin_struct *ps)
{
	strcpy(ps->info.name, PLUGIN_NAME);
	strcpy(ps->info.desc, PLUGIN_DESC);
	sprintf(ps->info.build_ts, "%s  %s", __DATE__, __TIME__);
	ps->info.type = PLUGIN_TYPE;
	ps->info.license = PLUGIN_LICENSE;
	sprintf(ps->info.version, PLUGIN_VERSION);

	ps->info.num_results = 0;
	ps->info.num_options = 0;

	ps->view.get_view_handle = pl_get_view_handle;
	ps->view.free_view_handle = pl_free_view_handle;
	ps->view.create_view = pl_create_view;

	ps->handle_signal = pl_handle_signal;

	return 0;
}  /* init_plugin() */


LIBRAAPI int
init_ra_plugin(struct ra *, struct plugin_struct *)
{
	return 0;
}  /* init_ra_plugin() */


LIBRAAPI int
fini_ra_plugin(void)
{
	/* clean up */

	return 0;
}  /* fini_plugin() */


view_handle
pl_get_view_handle(plugin_handle pl)
{
// 	struct plugin_struct *ps = (struct plugin_struct *)pl;

	struct view_info *vi = (struct view_info *)ra_alloc_mem(sizeof(struct view_info));
	if (!vi)
	{
		_ra_set_error(pl, 0, "plugin-ra-view");
		return NULL;
	}

	memset(vi, 0, sizeof(struct view_info));
	vi->handle_id = RA_HANDLE_VIEW;
	vi->plugin = pl;

	/* init options */
	vi->options = NULL;
// 	set_option_offsets(ps->info.opt, (struct ra_ev_sum_view *)(vi->options));
// 	set_default_options((struct ra_ev_sum_view *)(vi->options));

	return vi;
} // pl_get_view_handle()


// void
// set_default_options(struct ra_ev_sum_view *opt)
// {
// 	opt->sh = NULL;
// } /* set_default_options() */


// void
// set_option_offsets(struct ra_option_infos *opt_inf, struct ra_ev_sum_view *opt)
// {
// 	unsigned char *p = (unsigned char *)opt;
// 	int idx = 0;

// 	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->sh) - p);

// 	/* consistency check (useful when changing option structure) */
// 	if (idx != num_options)
// 	{
// 		fprintf(stderr, "critical error in ev_summary_view.c - set_option_offsets():\n"
// 			"  number of options(%ld) != idx(%d)\n", num_options, idx);
// 		exit(-1);
// 	}
// } /* set_option_offsets() */


void
pl_free_view_handle(view_handle vih)
{
	struct view_info *vi = (struct view_info *)vih;
	
// 	if (vi->options)
// 	{
// 		struct ra_ev_sum_view *opt;

// 		opt = (struct ra_ev_sum_view *)vi->options;
// 		free(opt);
// 	}
	       
	ra_free_mem(vi);
} /* pl_free_view_handle() */


int
pl_create_view(view_handle vh)
{
	struct view_info *vi = (struct view_info *) vh;

	if (!vi)
		return -1;

	if (!vi->rh)
		vi->rh = ra_rec_get_first(vi->mh, 0);

	// to be save
	vi->views = NULL;
	vi->num_views = 0;

	// check if current recording has child's
	rec_handle h = ra_rec_get_first_child(vi->rh);
	if (h)
	{
		do
		{
			vi->num_views++;
			vi->views = (void **)realloc(vi->views, sizeof(void *) * vi->num_views);
			vi->real_views = (void **)realloc(vi->views, sizeof(void *) * vi->num_views);
			vi->views[vi->num_views - 1] = create_view(vi, h, &(vi->real_views[vi->num_views-1]));
		}
		while ((h = ra_rec_get_next(h)));
	}
	else
	{
		vi->num_views = 1;
		vi->views = (void **)malloc(sizeof(void *));
		vi->real_views = (void **)malloc(sizeof(void *));
		vi->views[0] = create_view(vi, vi->rh, &(vi->real_views[0]));
	}

	if (vi->views == NULL)
		return -1;

	// add slots
	ra_comm_add(vi->mh, vi->plugin, vi->views[0], "pos-change");
	ra_comm_add(vi->mh, vi->plugin, vi->views[0], "init-scrollbar");

	return 0;
}  /* pl_get_view() */


void *
create_view(struct view_info *vi, rec_handle rh, void **real_view)
{
	void *ret = NULL;

#ifdef QT
 	view_frame_qt *fr = new view_frame_qt(vi->mh, (QWidget *)(vi->parent), vi->name);
 	ra_view_qt *v = new ra_view_qt(rh, fr);
 	v->init();
 	fr->init(v);
 	ret = fr;
	*real_view = v;
#endif // QT

#ifdef GTKMM
	view_frame_gtk *fr = new view_frame_gtk(vi->mh, (Gtk::Widget *)(vi->parent), vi->name);
	ra_view_gtk *v = new ra_view_gtk(rh);
	fr->init(v);
	v->init();
	ret = fr;
	*real_view = v;
	fr->show();
	v->show();
#endif // GTKMM

#ifdef _AFXDLL
	ra_view_mfc *v = new ra_view_mfc();
	v->init(rh, vi->plugin);
	v->Create(NULL, "ra-view");
	ret = v;
#endif // _AFXDLL

	return ret;
} // create_view()


void
pl_handle_signal(meas_handle, dest_handle dh, const char *sig_name,
		 int num_para, struct comm_para *p)
{
#ifdef QT
	view_frame_qt *f = (view_frame_qt *)dh;
#endif // QT
#ifdef GTKMM
	view_frame_gtk *f = (view_frame_gtk *)dh;
#endif // GTKMM
#ifdef _AFXDLL
	ra_view_mfc *v = (ra_view_mfc *)dh;
	view_frame_general *f = (view_frame_general *)dh;
#endif // _AFXDLL

	if (strcmp(sig_name, "pos-change") == 0)
		pos_change(f, num_para, p);
	if (strcmp(sig_name, "init-scrollbar") == 0)
		init_scrollbar(f, num_para, p);
} // pl_handle_signal()


void
pos_change(view_frame_general *f, int n, struct comm_para *p)
{
	// TODO: find out why check for block-signals is in this function wrong
	if (!f || (n != 2))
		return;

#ifdef _AFXDLL
	ra_view_mfc *v = (ra_view_mfc *)f;
	v->set_scrollbar_pos(p[0].v.l);
#else
	f->set_scrollbar_pos(p[0].v.l);
#endif
} // pos_change()


void
init_scrollbar(view_frame_general *f, int n, struct comm_para *p)
{
#ifdef _AFXDLL
	ra_view_mfc *v = (ra_view_mfc *)f;
//	if (!v || v->block_signals() || (n != 3))
	if (!v || (n != 3))
		return;

	v->init_scrollbar(p[0].v.l, p[1].v.l, p[2].v.d);
#else
//	if (!f || f->block_signals() || (n != 3))
	if (!f || (n != 3))
		return;

	f->init_scrollbar(p[0].v.l, p[1].v.l, p[2].v.d);
#endif
}  // x_resolution()
