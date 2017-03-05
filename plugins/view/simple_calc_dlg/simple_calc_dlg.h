/*----------------------------------------------------------------------------
 * simple_calc_dlg.h
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002-2005, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifndef _SIMPLE_CALC_DLG_H
#define _SIMPLE_CALC_DLG_H

#define _LIBRASCH_BUILD
#include <ra.h>
#include <ra_priv.h>
#include <pl_general.h>

#define PLUGIN_VERSION  "0.2.0"
#define PLUGIN_NAME     "simple-calc-dlg"
#define PLUGIN_DESC     gettext_noop("dialog to perform calculations using process plugins")
#define PLUGIN_TYPE     PLUGIN_GUI
#ifdef QT
#define PLUGIN_LICENSE  LICENSE_GPL
#else
#define PLUGIN_LICENSE  LICENSE_LGPL
#endif


// -------------------- prototypes --------------------
extern "C"
{
	LIBRAAPI int load_ra_plugin(struct plugin_struct *ps);
	LIBRAAPI int init_ra_plugin(struct ra *, struct plugin_struct *);
	LIBRAAPI int fini_ra_plugin(void);
}

proc_handle pl_get_proc_handle(plugin_handle pl);
void pl_free_proc_handle(proc_handle proc);
int pl_call_gui(proc_handle proc);


#endif // _SIMPLE_CALC_DLG_H
