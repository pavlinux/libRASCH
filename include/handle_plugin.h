/**
 * \file handle_plugin.h
 *
 * The header file contains the prototypes for the functions which
 * loads and unloads the plugins.
 *
 * \author Raphael Schneider (rasch@med1.med.tum.de)
 */

/*----------------------------------------------------------------------------
 *
 * Copyright (C) 2002-2006, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *
 *--------------------------------------------------------------------------*/

#ifndef _HANDLE_PLUGIN_H
#define _HANDLE_PLUGIN_H

#include <ra_defines.h>
#include <ra_priv.h>

#ifdef __cplusplus
extern "C" {
#endif


/* ------------------------------ prototypes ------------------------------ */
int read_plugins(struct librasch *ra);
struct plugin_struct *dload_plugin(char *file);
int is_plugin_ok(struct plugin_struct *p);
void close_plugins(struct plugin_struct *p);

#ifdef __cplusplus
}
#endif

#endif /* _HANDLE_PLUGIN_H */
