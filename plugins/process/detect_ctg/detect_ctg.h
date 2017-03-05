/*----------------------------------------------------------------------------
 * detect_ctg.h
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2003-2006, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifndef _DETECT_CTG_H
#define _DETECT_CTG_H

#include "simple.h"


/* -------------------- defines -------------------- */
#define PLUGIN_VERSION  "0.2.0"
#define PLUGIN_NAME     "detect-ctg"
#define PLUGIN_DESC     gettext_noop("perform detections in cardiotocograms")
#define PLUGIN_TYPE     PLUGIN_PROCESS
#define PLUGIN_LICENSE  LICENSE_LGPL


enum result_idx
{
	NUM, POS
};

/* -------------------- structures -------------------- */

/* -------------------- globals -------------------- */
/* Up to now, results are not used just here so I do not forget it. */
static struct ra_result_infos results[] = {
	{"num", gettext_noop("number of found events"), RA_VALUE_TYPE_LONG, -1},
	{"pos", gettext_noop("positions of the found events in sampleunits"), RA_VALUE_TYPE_LONG_ARRAY, -1},
};
long num_results = sizeof(results) / sizeof(results[0]);

static struct ra_option_infos options[] = {
	{"num_ch", gettext_noop("number of channels set in 'ch'"), RA_VALUE_TYPE_LONG, -1, 0},
	{"ch", gettext_noop("list of channels used for the event detection"), RA_VALUE_TYPE_LONG_ARRAY, -1, -1},

	{"save_in_eval", gettext_noop("flag if results should be saved in an evaluation"), RA_VALUE_TYPE_SHORT, 0, 0},
	{"eh", gettext_noop("eval-handle used to store the results"), RA_VALUE_TYPE_VOIDP, 0, 0},
	{"clh", gettext_noop("event-class-handle used to store the results when region was selected"),
	 RA_VALUE_TYPE_VOIDP, 0, 0},
};
long num_options = sizeof(options) / sizeof(options[0]);

static struct ra_auto_create_class create_class[] = {
	{"uterine-contraction", POS, POS, "", "", "", NULL, 0, NULL},
};
long num_create_class = sizeof(create_class) / sizeof(create_class[0]);

static struct ra_auto_create_prop create_prop[] = {
	{"uc-max-pos", 0, -1, POS, -1, NULL, 1},
};
long num_create_prop = sizeof(create_prop) / sizeof(create_prop[0]);


/* -------------------- prototypes -------------------- */
int pl_call_gui(proc_handle proc);
proc_handle pl_get_proc_handle(plugin_handle pl);
void set_default_options(struct ra_detect_ctg *opt);
void set_option_offsets(struct ra_option_infos *opt_inf, struct ra_detect_ctg *opt);
void pl_free_proc_handle(proc_handle proc);
int pl_do_processing(proc_handle proc);

int get_uc_channels(rec_handle rh, long **ch, long *num_ch);
int store_results(struct proc_info *pi, struct detect_result *dr);


#endif /* _DETECT_CTG_H */
