/*----------------------------------------------------------------------------
 * annot_comment_view.c  -  view listing annotations/comments
 *
 * Description:
 * The plugin list the annotations/comments saved in an evaluation.
 *
 * Author(s): Raphael Schneider (librasch@gmail.com)
 *
 * Copyright (C) 2008-2009, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define _LIBRASCH_BUILD
#include <ra.h>
#include <ra_priv.h>
#include <pl_general.h>
#include <ra_pl_comm.h>

#include <ra_annot_comment_view.h>

#include "annot_comment_view_general.h"
#include "annot_comment_view.h"

#ifdef QT
#include "annot_comment_view_qt.h"
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
		_ra_set_error(pl, 0, "plugin-annot-comment-view");
		return NULL;
	}

	memset(vi, 0, sizeof(struct view_info));
	vi->handle_id = RA_HANDLE_VIEW;
	vi->plugin = pl;

	/* init options */
	vi->options = calloc(1, sizeof(struct ra_annot_comment_view));
	set_option_offsets(ps->info.opt, (struct ra_annot_comment_view *)(vi->options));
	set_default_options((struct ra_annot_comment_view *)(vi->options));

	return vi;
} // pl_get_view_handle()


void
set_default_options(struct ra_annot_comment_view *opt)
{
	opt->eh = NULL;
} /* set_default_options() */


void
set_option_offsets(struct ra_option_infos *opt_inf, struct ra_annot_comment_view *opt)
{
	unsigned char *p = (unsigned char *)opt;
	int idx = 0;

	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->eh) - p);

	/* consistency check (useful when changing option structure) */
	if (idx != num_options)
	{
		fprintf(stderr, "critical error in annot_comment_view.c - set_option_offsets():\n"
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
		struct ra_annot_comment_view *opt;

		opt = (struct ra_annot_comment_view *)vi->options;
		free(opt);
	}
	       
	ra_free_mem(vi);
} /* pl_free_view_handle() */


int
pl_create_view(view_handle vih)
{
	struct view_info *vi = (struct view_info *)vih;
	struct ra_annot_comment_view *opt = (struct ra_annot_comment_view *)vi->options;

	if (!vi || !vi->mh || !opt)
	{
		return -1;
	}

	if (opt->eh == NULL)
	{
		_ra_set_error(vih, RA_ERR_INFO_MISSING, "plugin-annot-comment-view");
		return -1;
	}

	vi->num_views = 1;
	vi->views = (void **) malloc(sizeof(void *));
#ifdef QT
	vi->views[0] = new annot_comment_view_qt(vi->mh, opt->eh, (QWidget *)(vi->parent));
#endif // QT

	if (vi->views[0] == NULL)
	{
		free(vi->views);
		vi->num_views = 0;
		return -1;
	}

	// make signal/slot connections
	ra_comm_add(vi->mh, vi->plugin, vi->views[0], "eval-change");

	return 0;
}  /* pl_get_view() */


void
pl_handle_signal(meas_handle, dest_handle dh, const char *sig_name,
		 int /*num_para*/, struct comm_para * /*p*/)
{
#ifdef QT
	annot_comment_view_qt *v = (annot_comment_view_qt *)dh;
#endif // QT

	if (strcmp(sig_name, "eval-change") == 0)
		eval_change(v);
}  // pl_handle_signal()


void
eval_change(annot_comment_view_general * v)	//, void *eh)
{
	if (v->block_signals())
		return;

	v->eval_change();
}  // eval_change()
