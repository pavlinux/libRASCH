/*----------------------------------------------------------------------------
 * ctg_view.cpp  -  view for ecg's
 *
 * Description:
 * The plugin allows to view ecg-stripes. The plugin use the ts-view plugin for
 * showing the raw-data.
 *
 * Author(s): Raphael Schneider (librasch@gmail.com)
 *
 * Copyright (C) 2003-2008, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifdef _AFXDLL
#include "stdafx.h"
#include "ctg_view_mfc.h"
#endif // _AFXDLL

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ctg_view.h"

#ifdef QT
#include "../shared/view_frame_qt.h"
#include "ctg_view_qt.h"
#endif // QT

#ifdef GTKMM
#include "../shared/view_frame_gtk.h"
#include "ctg_view_gtk.h"
#endif // GTKMM


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

	ps->view.check_channels = pl_check_channels;
	ps->view.check_plugin_usage = pl_check_plugin_usage;

	return 0;
} /* init_plugin() */


LIBRAAPI int
init_ra_plugin(struct ra *, struct plugin_struct *)
{
	return 0;
} /* init_ra_plugin() */


LIBRAAPI int
fini_ra_plugin(void)
{
	/* clean up */

	return 0;
} /* fini_plugin() */


view_handle
pl_get_view_handle(plugin_handle pl)
{
// 	struct plugin_struct *ps = (struct plugin_struct *)pl;

	struct view_info *vi = (struct view_info *)ra_alloc_mem(sizeof(struct view_info));
	if (!vi)
	{
		_ra_set_error(pl, 0, "plugin-ctg-view");
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

	if (!vi || !vi->mh)
		return -1;

	vi->num_views = 1;
	vi->views = (void **) malloc(sizeof(void *));
	vi->real_views = (void **)malloc(sizeof(void *));

#ifdef QT
	view_frame_qt *fr = new view_frame_qt(vi->mh, (QWidget *)(vi->parent), vi->name);
	ctg_view_qt *v = new ctg_view_qt(vi->mh, vi->num_ch, vi->ch, fr);
	v->init();
	fr->init(v);
	vi->views[0] = fr;
	vi->real_views[0] = v;
#endif // QT

#ifdef GTKMM
	view_frame_gtk *fr = new view_frame_gtk(vi->mh, (Gtk::Widget *)(vi->parent), vi->name);
	ctg_view_gtk *v = new ctg_view_gtk(vi->mh, vi->num_ch, vi->ch);
	fr->init(v);
	vi->views[0] = fr;
	vi->real_views[0] = v;
	v->show();
#endif // GTKMM

#ifdef _AFXDLL
	ctg_view_mfc *v = new ctg_view_mfc();
	v->init(vi->mh, vi->plugin, vi->num_ch, vi->ch);
	RECT rect;		// don't care, must be set in calling function
	DWORD style = WS_CHILD | WS_VISIBLE;
	if (strncmp(vi->name, "ra-sub-view", 11) != 0)
		style |= WS_HSCROLL;
	v->Create(NULL, vi->name, style, rect, (CWnd *) (vi->parent), 2);
	((ctg_view_general *)v)->init();
	v->ShowWindow(SW_SHOW);
	vi->views[0] = v;
	vi->real_views[0] = v;
#endif // _AFXDLL

	if (vi->views[0] == NULL)
	{
		vi->num_views = 0;
		free(vi->views);
		free(vi->real_views);
		return -1;
	}

	// make signal/slot connections
	ra_comm_add(vi->mh, vi->plugin, vi->views[0], "pos-change");
	ra_comm_add(vi->mh, vi->plugin, vi->views[0], "init-scrollbar");
	ra_comm_add(vi->mh, vi->plugin, vi->views[0], "x-resolution");
	ra_comm_add(vi->mh, vi->plugin, vi->views[0], "select-event");
	ra_comm_add(vi->mh, vi->plugin, vi->views[0], "save-settings");
	ra_comm_add(vi->mh, vi->plugin, vi->views[0], "load-settings");

	return 0;
} /* pl_get_view() */


void
pl_handle_signal(meas_handle , dest_handle dh, const char *sig_name, int num_para, struct comm_para *p)
{
#ifdef QT
	view_frame_qt *f = (view_frame_qt *)dh;
	ctg_view_qt *v = (ctg_view_qt *)(f->view());
#endif // QT
#ifdef GTKMM
	view_frame_gtk *f = (view_frame_gtk *)dh;
	ctg_view_gtk *v = (ctg_view_gtk *)(f->view());
#endif // GTKMM
#ifdef _AFXDLL
	ctg_view_mfc *v = (ctg_view_mfc *)dh;
	view_frame_general *f = (view_frame_general *)dh;
#endif // _AFXDLL

	if (strcmp(sig_name, "pos-change") == 0)
		pos_change(v, f, num_para, p);
	if (strcmp(sig_name, "init-scrollbar") == 0)
		init_scrollbar(f, num_para, p);
	if (strcmp(sig_name, "x-resolution") == 0)
		x_resolution(v, num_para, p);
	if (strcmp(sig_name, "select-event") == 0)
		call_sort(v, num_para, p);
	if (strcmp(sig_name, "save-settings") == 0)
		save_settings(v, num_para, p);
	if (strcmp(sig_name, "load-settings") == 0)
		load_settings(v, num_para, p);
} // pl_handle_signal()


void
pos_change(ctg_view_general *v, view_frame_general *f, int n, struct comm_para *p)
{
	if (n != 2)
		return;

#ifdef _AFXDLL
	if (v && !v->block_signals())
		v->set_scrollbar_pos(p[0].v.l);
#else
	if (f && !f->block_signals())
		f->set_scrollbar_pos(p[0].v.l);
#endif

	if (!v->block_signals())
	{
		v->set_pos(p[0].v.l, p[1].v.d, true);
		v->update();
	}
} // pos_change()


void
init_scrollbar(view_frame_general *f, int n, struct comm_para *p)
{
#ifdef _AFXDLL
	ts_view_mfc *v = (ts_view_mfc *)f;
	if (v->block_signals() || (n != 3))
		return;

	v->init_scrollbar(p[0].v.l, p[1].v.l, p[2].v.d);
#else
	if (f->block_signals() || (n != 3))
		return;

	f->init_scrollbar(p[0].v.l, p[1].v.l, p[2].v.d);
#endif
}  // init_scrollbar()


void
x_resolution(ctg_view_general * v, int n, struct comm_para *p)
{
	if (v->block_signals())
		return;

	if (n != 2)
		return;

	v->set_x_resolution(p[0].v.d, (void *)(p[1].v.l));
}  // x_resolution()


void
call_sort(ctg_view_general * v, int n, struct comm_para *p)
{
	if (v->block_signals())
		return;

	if (n < 2)
		return;

	v->sort_general(p[0].v.pc, p[1].v.d);
	v->update();
} // call_sort()

void
save_settings(ctg_view_general * v, int n, struct comm_para *p)
{
	if (v->block_signals() || (n != 1))
		return;

	v->save_settings(p->v.pc);
} // save_settings()


void
load_settings(ctg_view_general * v, int n, struct comm_para *p)
{
	if (v->block_signals() || (n != 1))
		return;

	v->load_settings(p->v.pc);
} // load_settings()


int
pl_check_channels(rec_handle rh, long ch)
{
	int ret = -1;

	value_handle vh = ra_value_malloc();
	ra_value_set_number(vh, ch);
	if (ra_info_get(rh, RA_INFO_REC_CH_TYPE_L, vh) != 0)
	{
		ra_value_free(vh);
		return ret;
	}

	if ((ra_value_get_long(vh) == RA_CH_TYPE_CTG_FHR) || (ra_value_get_long(vh) == RA_CH_TYPE_CTG_UC))
		ret = 0;

	ra_value_free(vh);

	return ret;
} /* pl_check_channels() */


/*
  check if pl_name is used by this plugin (0: yes, !0: no)
*/
int
pl_check_plugin_usage(const char *pl_name)
{
	if (strcmp(pl_name, "ts-view") == 0)
		return 0;

	return -1;
} /* pl_check_plugin_usage() */
