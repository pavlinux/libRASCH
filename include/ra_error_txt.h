/**
 * \file ra_error_txt.h
 *
 * The header file contains short descritions for libRASCH errors.
 *
 * \author Raphael Schneider (rasch@med1.med.tum.de)
 */

/*----------------------------------------------------------------------------
 *
 * Copyright (C) 2003-2007, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *
 *--------------------------------------------------------------------------*/

#ifndef RA_ERROR_TXT_H
#define RA_ERROR_TXT_H


static char *ra_error_text[] = {
	gettext_noop("no error"),		/* 1 */
	gettext_noop("unspecific error (maybe internal)"), /* 2 */
	gettext_noop("error reading config-file"), /* 3 */
	gettext_noop("error loading plugins"), /* 4 */
	gettext_noop("not supported format"),	/* 5 */
	gettext_noop("unknown plugin"),	/* 6 */
	gettext_noop("needed plugin function is not available"), /* 7 */
	gettext_noop("unknown handle"),	/* 8 */
	gettext_noop("not handled/not supported info"), /* 9 */
	gettext_noop("a input parameter is out of range"), /* 10 */
	gettext_noop("functionality not supported (planned for future versions)"), /* 11 */
	gettext_noop("wrong handle was used"), /* 12 */
	gettext_noop("error reading evaluation"), /* 13 */
	gettext_noop("info for used event-type not available"), /* 14 */
	gettext_noop("event property is not known/out of range"), /* 15 */
	gettext_noop("event set is not known/out of range"), /* 16 */
	gettext_noop("wrong event-property was used"), /* 17 */
	gettext_noop("template number was not found"), /* 18 */
	gettext_noop("structure of evaluation file is corrupt"), /* 19 */
	gettext_noop("a needed plugin is not available"), /* 20 */
	gettext_noop("one of the measurement files is missing or corrupt"), /* 21 */
	gettext_noop("a needed information is missing"), /* 22 */
	gettext_noop("at least one needed event-property is not available"), /* 23 */
	gettext_noop("the option is unknown"), /* 24 */
	gettext_noop("the needed signal is not available in the measurement"), /* 25 */
	gettext_noop("serious internal error: an offset for an option was not set"), /* 26 */
	gettext_noop("internal error"), /* 27 */
	gettext_noop("internal error in plugin"), /* 28 */
	gettext_noop("function argument(s) wrong"), /* 29 */
	gettext_noop("data cause to a state in the algorithm which should not happen"), /* 30 */
	gettext_noop("saving data to file failed"), /* 31 */
	gettext_noop("a needed information is missing in the measurement file(s)"), /* 32 */
	gettext_noop("the type of the evaluation is wrong"), /* 33 */
	gettext_noop("wrong version of the evaluation file"), /* 34 */
	gettext_noop("data is not available because measurement was opened with fast-flag"), /* 35 */
};


static int ra_num_error_text = sizeof(ra_error_text) / sizeof(ra_error_text[0]);


static char *ra_warning_text[] = {
	gettext_noop("no data/events available for processing"), /* 1000 */
	gettext_noop("no config-file was found"), /* 1001 */
	gettext_noop("no default evaluation available"), /* 1002 */
	gettext_noop("not all ch/events could be processed (maybe lack of suitable data)"), /* 1003 */
	gettext_noop("the property-value returned is the channel independent value"), /* 1004 */
};

static int ra_num_warning_text = sizeof(ra_warning_text) / sizeof(ra_warning_text[0]);

#endif /* RA_ERROR_TXT_H */
