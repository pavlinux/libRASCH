/*----------------------------------------------------------------------------
 * ev_summary_view.c  -  view for event summaries
 *
 * Description:
 * The plugin shows event-summaries of an evaluation.
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002-2006, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/


#ifdef _AFXDLL
#include "stdafx.h"
#include "ev_summary_view_mfc.h"
#endif // _AFXDLL

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define _LIBRASCH_BUILD
#include <ra.h>
#include <ra_priv.h>
#include <pl_general.h>
#include <ra_pl_comm.h>

#include <ra_ev_sum_view.h>

#include "ev_summary_view_general.h"
#include "ev_summary_view.h"

#ifdef QT
#include "ev_summary_view_scroll_area.h"
#endif // QT


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

	ps->info.num_options = num_options;
	ps->info.opt = (ra_option_infos *)ra_alloc_mem(sizeof(options));
	memcpy(ps->info.opt, options, sizeof(options));

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
	struct plugin_struct *ps = (struct plugin_struct *)pl;

	struct view_info *vi = (struct view_info *)ra_alloc_mem(sizeof(struct view_info));
	if (!vi)
	{
		_ra_set_error(pl, 0, "plugin-ev-summary-view");
		return NULL;
	}

	memset(vi, 0, sizeof(struct view_info));
	vi->handle_id = RA_HANDLE_VIEW;
	vi->plugin = pl;

	/* init options */
	vi->options = calloc(1, sizeof(struct ra_ev_sum_view));
	set_option_offsets(ps->info.opt, (struct ra_ev_sum_view *)(vi->options));
	set_default_options((struct ra_ev_sum_view *)(vi->options));

	return vi;
} // pl_get_view_handle()


void
set_default_options(struct ra_ev_sum_view *opt)
{
	opt->sh = NULL;
} /* set_default_options() */


void
set_option_offsets(struct ra_option_infos *opt_inf, struct ra_ev_sum_view *opt)
{
	unsigned char *p = (unsigned char *)opt;
	int idx = 0;

	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->sh) - p);

	/* consistency check (useful when changing option structure) */
	if (idx != num_options)
	{
		fprintf(stderr, "critical error in ev_summary_view.c - set_option_offsets():\n"
			"  number of options(%ld) != idx(%d)\n", num_options, idx);
		exit(-1);
	}
} /* set_option_offsets() */


void
pl_free_view_handle(view_handle vih)
{
	struct view_info *vi = (struct view_info *)vih;
	
	if (vi->options)
	{
		struct ra_ev_sum_view *opt;

		opt = (struct ra_ev_sum_view *)vi->options;
		free(opt);
	}
	       
	ra_free_mem(vi);
} /* pl_free_view_handle() */


int
pl_create_view(view_handle vih)
{
	struct view_info *vi = (struct view_info *)vih;
	struct ra_ev_sum_view *opt = (struct ra_ev_sum_view *)vi->options;

	if (!vi || !vi->mh || !opt)
	{
		return -1;
	}

	if (opt->sh == NULL)
	{
		_ra_set_error(vih, RA_ERR_INFO_MISSING, "plugin-ev-summary");
		return -1;
	}

	vi->num_views = 1;
	vi->views = (void **) malloc(sizeof(void *));
#ifdef QT
	ev_summary_view_scroll_area *v = new ev_summary_view_scroll_area(vi->mh, opt->sh, (QWidget *)(vi->parent));
	vi->views[0] = v;
#endif // QT
#ifdef _AFXDLL
	ev_summary_view_mfc *v = new ev_summary_view_mfc();
	v->init(vi->mh, opt->sh);
	RECT rect;		// don't care, must be set by calling func
	v->Create(NULL, "ev-summary-view",
		  WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL, rect, (CWnd *) (vi->parent), 3);
	vi->views[0] = v;
#endif // _AFXDLL

	if (vi->views[0] == NULL)
	{
		free(vi->views);
		vi->num_views = 0;
		return -1;
	}

	// make signal/slot connections
	ra_comm_add(vi->mh, vi->plugin, vi->views[0], "eval-change");
	ra_comm_add(vi->mh, vi->plugin, vi->views[0], "x-resolution");
	ra_comm_add(vi->mh, vi->plugin, vi->views[0], "y-resolution");

	return 0;
}  /* pl_get_view() */


void
pl_handle_signal(meas_handle, dest_handle dh, const char *sig_name,
		 int num_para, struct comm_para *p)
{
#ifdef QT
	ev_summary_view_scroll_area *parent = (ev_summary_view_scroll_area *)dh;
	ev_summary_view_qt *v = (ev_summary_view_qt *)(parent->widget());
#endif // QT
#ifdef _AFXDLL
	ev_summary_view_mfc *v = (ev_summary_view_mfc *) dh;
#endif // _AFXDLL

	if (strcmp(sig_name, "eval-change") == 0)
		eval_change(v);
	else if (strcmp(sig_name, "x-resolution") == 0)
		x_res(v, num_para, p);
	else if (strcmp(sig_name, "y-resolution") == 0)
		y_res(v, num_para, p);
}  // pl_handle_signal()


void
eval_change(ev_summary_view_general * v)	//, void *eh)
{
	if (v->block_signals())
		return;

	v->init();

	v->update();
}  // eval_change()


void
x_res(ev_summary_view_general *v, long num_para, struct comm_para *p)
{
	if (v->block_signals())
		return;

	if (num_para < 1)
		return;

	v->set_x_res(p[0].v.d);
} // x_res()


void
y_res(ev_summary_view_general *v, long num_para, struct comm_para *p)
{
	if (v->block_signals())
		return;

	if (num_para < 2)
		return;

	v->set_ch_y_res(p[0].v.d, p[1].v.l);
} // y_res()

