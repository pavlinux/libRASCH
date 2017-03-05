/**
 * \file ra_qt_gettext.h
 *
 * The header file contains defines that Qt GUI elements created with designer
 * are using the gettext functions.
 *
 * \author Raphael Schneider (rasch@med1.med.tum.de)
 */

/*----------------------------------------------------------------------------
 *
 * Copyright (C) 2004, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Header: /home/cvs_repos.bak/librasch/include/ra_qt_gettext.h,v 1.1 2004/08/11 10:44:32 rasch Exp $
 *
 *--------------------------------------------------------------------------*/

#ifndef RA_QT_GETTEXT_H
#define RA_QT_GETTEXT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <libintl.h>
#define tr(string) QString::fromUtf8(gettext(string))

#ifdef __cplusplus
}
#endif


#endif  /* RA_QT_GETTEXT_H */
