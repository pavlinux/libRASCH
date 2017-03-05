/*----------------------------------------------------------------------------
 * annot_comment_view.h
 *
 * Author(s): Raphael Schneider (librasch@gmail.com)
 *
 * Copyright (C) 2008-2009, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifndef ANNOT_COMMENT_VIEW_H
#define ANNOT_COMMENT_VIEW_H

#define PLUGIN_VERSION  "0.1.0"
#define PLUGIN_NAME     "annot-comment-view"
#define PLUGIN_DESC     gettext_noop("view listing annotations/comments")
#define PLUGIN_TYPE     PLUGIN_VIEW
#ifdef QT
#define PLUGIN_LICENSE  LICENSE_GPL
#else
#define PLUGIN_LICENSE  LICENSE_LGPL
#endif

static struct ra_option_infos options[] = {
	{"eh", gettext_noop("eval-handle"), RA_VALUE_TYPE_VOIDP, 0, 0},
};
long num_options = sizeof(options) / sizeof(options[0]);


// -------------------- prototypes --------------------
extern "C"
{
	LIBRAAPI int load_ra_plugin(struct plugin_struct *ps);
	LIBRAAPI int init_ra_plugin(struct ra *m, struct plugin_struct *ps);
	LIBRAAPI int fini_ra_plugin(void);
}

view_handle pl_get_view_handle(plugin_handle pl);
void set_default_options(struct ra_annot_comment_view *opt);
void set_option_offsets(struct ra_option_infos *opt_inf, struct ra_annot_comment_view *opt);
void pl_free_view_handle(view_handle vih);
int pl_create_view(view_handle vih);

void pl_handle_signal(meas_handle, dest_handle dh, const char *sig_name,
		      int num_para, struct comm_para *p);

// Slots
void eval_change(annot_comment_view_general * v);


#endif // ANNOT_COMMENT_VIEW_H
