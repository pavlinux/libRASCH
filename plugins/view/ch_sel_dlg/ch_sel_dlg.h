/**
 * \file ch_sel_dlg.h
 *
 * \author Raphael Schneider (rasch@med1.med.tum.de)
 */

/*--------------------------------------------------------------------------
 *
 * Copyright (C) 2002-2007, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *
 *--------------------------------------------------------------------------*/

#ifndef _CH_SEL_DLG_H
#define _CH_SEL_DLG_H

#define _LIBRASCH_BUILD
#include <ra.h>
#include <ra_priv.h>
#include <pl_general.h>
#include <ra_defines.h>


#define PLUGIN_VERSION  "0.3.0"
#define PLUGIN_NAME     "ch-select-dlg"
#define PLUGIN_DESC     gettext_noop("dialog for selecting channels which will be used for processing")
#define PLUGIN_TYPE     PLUGIN_GUI
#ifdef QT
#define PLUGIN_LICENSE  LICENSE_GPL
#else
#define PLUGIN_LICENSE  LICENSE_LGPL
#endif


enum result_idx
{
	CH
};

/* -------------------- structures -------------------- */


/* -------------------- globals -------------------- */
static struct ra_result_infos results[] = {
	{"CH", gettext_noop("list of selected channels"), RA_VALUE_TYPE_LONG_ARRAY, -1},
};
long num_results = sizeof(results) / sizeof(results[0]);

static struct ra_option_infos options[] = {
	{"num_ch", gettext_noop("number of channels given in 'ch_type_filter'"), RA_VALUE_TYPE_LONG, -1, 0},
	{"ch_type_filter", gettext_noop("list of channel types which have to be shown"), RA_VALUE_TYPE_LONG_ARRAY, -1, -1},
};
long num_options = sizeof(options) / sizeof(options[0]);


// -------------------- prototypes --------------------
extern "C"
{
	LIBRAAPI int load_ra_plugin(struct plugin_struct *ps);
	LIBRAAPI int init_ra_plugin(struct ra *, struct plugin_struct *);
	LIBRAAPI int fini_ra_plugin(void);
}


proc_handle pl_get_proc_handle(plugin_handle pl);
void set_default_options(struct ra_ch_sel *opt);
void set_option_offsets(struct ra_option_infos *opt_inf, struct ra_ch_sel *opt);

void pl_free_proc_handle(proc_handle proc);

int pl_call_gui(proc_handle proc);


#endif // _CH_SEL_DLG_H
