/*----------------------------------------------------------------------------
 * ra_plot.cpp  -  plot evaluation data
 *
 * Description:
 * The plugin plot evaluation data in a window and allows to control which data
 * and in which format the data is plotted.
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002-2006, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifdef QT
#include "ra_plot_qt.h"
#include "sel_data_dlg_qt.h"
#endif // QT

#ifdef _AFXDLL
#include "stdafx.h"
#include "ra_plot_mfc.h"
#include "sel_data_dlg_mfc.h"
#endif // _AFXDLL

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ra_plot.h"


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

	ps->call_gui = pl_call_gui;

	ps->view.get_view_handle = pl_get_view_handle;
	ps->view.free_view_handle = pl_free_view_handle;
	ps->view.create_view = pl_create_view;

	ps->handle_signal = pl_handle_signal;

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


int
pl_call_gui(any_handle h)
{
	if (!h)
		return -1;

	struct view_info *v = (struct view_info *)h;
	if (!v->mh)
		return -1;

	struct ra_plot_options *opt = (struct ra_plot_options *) v->options;
	if (!opt)
 	{
 		opt = (struct ra_plot_options *)ra_alloc_mem(sizeof(struct ra_plot_options));
 		v->options = (void *)opt;
 		memset(opt, 0, sizeof(struct ra_plot_options));
 		opt->eval = ra_eval_get_default(v->mh);
 	}

	int ret = 0;
#ifdef QT
	sel_data_dlg_qt dlg(v->mh, opt);
	if (dlg.exec() == QDialog::Rejected)
		ret = -1;
#endif // QT

#ifdef _AFXDLL
	struct plugin_struct *pl = (struct plugin_struct *) v->plugin;
	HMODULE mod = LoadLibrary(pl->info.file);
	HRSRC res = FindResource(mod, (LPCTSTR) IDD_SELECT_DATA_DLG, RT_DIALOG);
	HGLOBAL g = LoadResource(mod, res);
	LPVOID p = LockResource(g);

	sel_data_dlg_mfc dlg;
	dlg.init(v->mh, v->plugin, opt);
	dlg.InitModalIndirect(p, NULL);

	if (dlg.DoModal() != IDOK)
		ret = -1;
	FreeLibrary(mod);
#endif // _AFXDLL

	return ret;
} /* pl_call_gui */


view_handle
pl_get_view_handle(plugin_handle pl)
{
// 	struct plugin_struct *ps = (struct plugin_struct *)pl;

	struct view_info *vi = (struct view_info *)ra_alloc_mem(sizeof(struct view_info));
	if (!vi)
	{
		_ra_set_error(pl, 0, "plugin-plot-view");
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

	vi->views = (void **) malloc(sizeof(void *));
	vi->num_views = 1;

	struct ra_plot_options *opt = (struct ra_plot_options *)vi->options;

#ifdef QT
	vi->views[0] = new ra_plot_qt(vi->mh, opt, (QWidget *)(vi->parent));
#endif // QT

#ifdef _AFXDLL
	ra_plot_mfc *v = new ra_plot_mfc();
	v->init(vi->mh, vi->plugin, opt);
	RECT rect;		// don't care, must be set in calling function
	v->Create(NULL, vi->name, WS_CHILD | WS_VISIBLE, rect, (CWnd *) (vi->parent), 1);
	vi->views[0] = v;
#endif // _AFXDLL

	if (vi->views[0] == NULL)
	{
		vi->num_views = 0;
		free(vi->views);
		return -1;
	}

	// add slots
	ra_comm_add(vi->mh, vi->plugin, vi->views[0], "highlight-event");
	ra_comm_add(vi->mh, vi->plugin, vi->views[0], "eval-change");
	ra_comm_add(vi->mh, vi->plugin, vi->views[0], "event-change");
	ra_comm_add(vi->mh, vi->plugin, vi->views[0], "start-event-change");
	ra_comm_add(vi->mh, vi->plugin, vi->views[0], "end-event-change");
	ra_comm_add(vi->mh, vi->plugin, vi->views[0], "pos-change");

	return 0;
} /* pl_get_view() */


void
pl_handle_signal(meas_handle, dest_handle dh, const char *sig_name,
		 int num_para, struct comm_para *p)
{
#ifdef QT
	ra_plot_qt *v = (ra_plot_qt *) dh;
#endif // QT
#ifdef _AFXDLL
	ra_plot_mfc *v = (ra_plot_mfc *) dh;
#endif // _AFXDLL

	if (v->block_signals())
		return;

	if (strcmp(sig_name, "highlight-event") == 0)
		highlight_event(v, num_para, p);
	else if (strcmp(sig_name, "eval-change") == 0)
		eval_change(v);
	else if (strcmp(sig_name, "event-change") == 0)
		eval_change(v);	// TODO: handle event-change and not call only eval_change()
	else if (strcmp(sig_name, "start-event-change") == 0)
		v->collect_event_changes(1);
	else if (strcmp(sig_name, "end-event-change") == 0)
	{
		v->collect_event_changes(0);
		eval_change(v);
	}
	else if (strcmp(sig_name, "pos-change") == 0)
		pos_change(v, num_para, p);
} // pl_handle_signal()


void
highlight_event(ra_plot_general * v, int n, struct comm_para *p)
{
	if ((n < 2) || (p[0].type != PARA_VOID_POINTER) || (p[1].type != PARA_LONG))
		return;

	v->set_sig_event(p[0].v.pv, p[1].v.l);
	v->update_pos();
} // highligth_event()


void
eval_change(ra_plot_general * v)
{
	v->get_plot_data();
	v->get_annot_data();
	v->update();
} // eval_change()


void
pos_change(ra_plot_general *v, int n, struct comm_para *p)
{
	if (n != 2)
		return;

	if (!v->block_signals())
		v->set_pos_event(p[0].v.l);
} // pos_change()
