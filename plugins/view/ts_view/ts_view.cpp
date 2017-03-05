/*----------------------------------------------------------------------------
 * ts_view.cpp  -  view for time-series
 *
 * Description:
 * The plugin shows the raw-data for time-series signals. This plugin is the
 * default-plugin for all time-series signals.
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002-2006, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifdef QT
#include "ts_view_qt.h"
#include "../shared/view_frame_qt.h"
#endif // QT

#ifdef _AFXDLL
#include "stdafx.h"
#include "ts_view_mfc.h"
#endif // _AFXDLL

#ifdef GTKMM
#include "ts_view_gtk.h"
#include "../shared/view_frame_gtk.h"
#endif  // GTKMM

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ts_view.h"


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
init_ra_plugin(struct librasch *, struct plugin_struct *)
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
		_ra_set_error(pl, 0, "plugin-ts-view");
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
	vi->views = (void **)malloc(sizeof(void *));
	vi->real_views = (void **)malloc(sizeof(void *));
#ifdef QT
	view_frame_qt *fr = new view_frame_qt(vi->mh, (QWidget *)(vi->parent), vi->name);
	ts_view_qt *v = new ts_view_qt(vi->mh, vi->num_ch, vi->ch, fr, vi->name);
	fr->init(v);
	vi->views[0] = fr;
	vi->real_views[0] = v;
#endif // QT

#ifdef GTKMM
	view_frame_gtk *fr = new view_frame_gtk(vi->mh, (Gtk::Widget *)(vi->parent), vi->name);
	ts_view_gtk *v = new ts_view_gtk(vi->mh, vi->num_ch, vi->ch, vi->name);
	fr->init(v);
	vi->views[0] = fr;
	vi->real_views[0] = v;
	v->show();
#endif // GTKMM

#ifdef _AFXDLL
	ts_view_mfc *v = new ts_view_mfc();
	v->init(vi->mh, vi->plugin, vi->num_ch, vi->ch);
	RECT rect;		// don't care, must be set in calling function
	DWORD style = WS_CHILD | WS_VISIBLE;
	if (strncmp(vi->name, "ra-sub-view", 11) != 0)
		style |= WS_HSCROLL;
	v->Create(NULL, vi->name, style, rect, (CWnd *) (vi->parent), 1);
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

	// add slots
	ra_comm_add(vi->mh, vi->plugin, vi->views[0], "pos-change");
 	ra_comm_add(vi->mh, vi->plugin, vi->views[0], "gui-event-start");
 	ra_comm_add(vi->mh, vi->plugin, vi->views[0], "gui-event-end");
 	ra_comm_add(vi->mh, vi->plugin, vi->views[0], "x-resolution");
	ra_comm_add(vi->mh, vi->plugin, vi->views[0], "init-scrollbar");
	ra_comm_add(vi->mh, vi->plugin, vi->views[0], "eval-change");
	ra_comm_add(vi->mh, vi->plugin, vi->views[0], "input-focus-change");
	ra_comm_add(vi->mh, vi->plugin, vi->views[0], "save-settings");
	ra_comm_add(vi->mh, vi->plugin, vi->views[0], "load-settings");

	return 0;
} /* pl_create_view() */


void
pl_handle_signal(meas_handle, dest_handle dh, const char *sig_name,
		 int num_para, struct comm_para *p)
{
#ifdef QT
	view_frame_qt *f = (view_frame_qt *)dh;
	ts_view_qt *v = (ts_view_qt *)(f->view());
#endif // QT
#ifdef GTKMM
	view_frame_gtk *f = (view_frame_gtk *)dh;
	ts_view_gtk *v = (ts_view_gtk *)(f->view());
#endif // GTKMM
#ifdef _AFXDLL
	ts_view_mfc *v = (ts_view_mfc *) dh;
	view_frame_general *f = (view_frame_general *)dh;
#endif // _AFXDLL

	if (strcmp(sig_name, "pos-change") == 0)
		pos_change(v, f, num_para, p);
	if (strcmp(sig_name, "gui-event-start") == 0)
		gui_event(v, true);
	if (strcmp(sig_name, "gui-event-end") == 0)
		gui_event(v, false);
	if (strcmp(sig_name, "x-resolution") == 0)
		x_resolution(v, num_para, p);
	if (strcmp(sig_name, "init-scrollbar") == 0)
		init_scrollbar(f, num_para, p);
	if (strcmp(sig_name, "eval-change") == 0)
		eval_change(v);
	if (strcmp(sig_name, "input-focus-change") == 0)
		input_focus_change(v, num_para, p);
	if (strcmp(sig_name, "save-settings") == 0)
		save_settings(v, num_para, p);
	if (strcmp(sig_name, "load-settings") == 0)
		load_settings(v, num_para, p);
} // pl_handle_signal()


void
pos_change(ts_view_general *v, view_frame_general *f, int n, struct comm_para *p)
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
gui_event(ts_view_general * v, bool start)
{
	if (v->block_signals())
		return;

	v->in_gui_event(start);
} // gui_event()


void
x_resolution(ts_view_general *v, int n, struct comm_para *p)
{
	if (v->block_signals())
		return;

	if (n != 2)
		return;

	v->set_x_resolution(p[0].v.d, (void *)(p[1].v.l));
}  // x_resolution()


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
eval_change(ts_view_general * v)
{
	if (v->block_signals())
		return;

	// re-read eval and redraw
	v->read_eval();
	v->update();
} // eval_change()


void
input_focus_change(ts_view_general * v, int /*n*/, struct comm_para * /*p*/)
{
	if (v->block_signals())
		return;

	if (v->has_input_focus())
	{
		// because change was send from the view which received the input focus,
		// we have to clear it here
		v->set_input_focus(false);
		v->update();
	}
} // input_focus_change()


void
save_settings(ts_view_general * v, int n, struct comm_para *p)
{
	if (v->block_signals() || (n != 1))
		return;

	v->save_settings(p->v.pc);
} // save_settings()


void
load_settings(ts_view_general * v, int n, struct comm_para *p)
{
	if (v->block_signals() || (n != 1))
		return;

	v->load_settings(p->v.pc);
} // load_settings()


int
pl_check_channels(rec_handle rh, long /*ch */ )
{
	int ret = -1;

	/* I am a view-plugin handling timeseries so I only need to check if
	   the recording is a timeseries. If it is a timeseries, I can handle all
	   channels */
	value_handle vh = ra_value_malloc();
	if (ra_info_get(rh, RA_INFO_REC_GEN_TYPE_L, vh) != 0)
	{
		ra_value_free(vh);
		return ret;
	}

	if ((ra_value_get_long(vh) == RA_REC_TYPE_TIMESERIES)
	    || (ra_value_get_long(vh) == RA_REC_TYPE_DAQ_TIMESERIES))
	{
		ret = 0;
	}

	ra_value_free(vh);

	return ret;
} /* pl_check_channels() */


/*
  check if pl_name is used by this plugin (0: yes, !0: no)
 */
int
pl_check_plugin_usage(const char * /*pl_name */ )
{
	/* I am not using another plugin to show recordings */
	return -1;
} /* pl_check_plugin_usage() */
