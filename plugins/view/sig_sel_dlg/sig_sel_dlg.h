/*----------------------------------------------------------------------------
 * sig_sel_dlg.h
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002-2005, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifndef _SIG_SEL_DLG_H
#define _SIG_SEL_DLG_H


#define _LIBRASCH_BUILD
#include <ra.h>
#include <ra_priv.h>
#include <pl_general.h>

#define PLUGIN_VERSION  "0.3.0"
#ifdef _AFXDLL
#define PLUGIN_NAME     "sig-sel-dlg-mfc"
#define PLUGIN_DESC     gettext_noop("dialog to choose signales which can be handled with libRASCH (MFC version)")
#else
#define PLUGIN_NAME     "sig-sel-dlg"
#define PLUGIN_DESC     gettext_noop("dialog to choose signales which can be handled with libRASCH")
#endif /* _AFXDLL */
#define PLUGIN_TYPE     PLUGIN_GUI
#ifdef QT
#define PLUGIN_LICENSE  LICENSE_GPL
#else
#define PLUGIN_LICENSE  LICENSE_LGPL
#endif

enum result_idx
{
	SEL_FILE
};


/* -------------------- structures -------------------- */


/* -------------------- globals -------------------- */
static struct ra_result_infos results[] = {
	{"SEL_FILE", gettext_noop("selected file (incl. path)"), RA_VALUE_TYPE_CHAR, 1},
};
long num_results = sizeof(results) / sizeof(results[0]);

static struct ra_option_infos options[] = {
	{"initial_path", gettext_noop("open dialog showing this path"), RA_VALUE_TYPE_CHAR, -1, 0},
};
long num_options = sizeof(options) / sizeof(options[0]);

// -------------------- prototypes --------------------
extern "C"
{
	LIBRAAPI int load_ra_plugin(struct plugin_struct *ps);
	LIBRAAPI int init_ra_plugin(struct ra *m, struct plugin_struct *ps);
	LIBRAAPI int fini_ra_plugin(void);
}

proc_handle pl_get_proc_handle(plugin_handle pl);
void set_default_options(struct ra_sig_sel *opt);
void set_option_offsets(struct ra_option_infos *opt_inf, struct ra_sig_sel *opt);
void pl_free_proc_handle(proc_handle proc);
int pl_call_gui(proc_handle proc);

#endif /* _SIG_SEL_DLG_H */
