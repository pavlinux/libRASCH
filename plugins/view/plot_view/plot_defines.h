/*----------------------------------------------------------------------------
 * plot_defines.h
 *
 * 
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002-2003, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Header: /home/cvs_repos.bak/librasch/plugins/view/plot_view/plot_defines.h,v 1.3 2003/08/06 15:05:15 rasch Exp $
 *--------------------------------------------------------------------------*/

#ifndef PLOT_DEFINES_H
#define PLOT_DEFINES_H


struct ra_plot_object
{
	long id;
	char name[PLOT_OBJECT_LEN];
}; /* struct ra_plot_object */


static struct ra_plot_object ra_plot_symbol[] = {
	{RA_PLOT_SYM_CIRCLE, "circle"},
	{RA_PLOT_SYM_CIRCLE_FULL, "circle full"},
	{RA_PLOT_SYM_RECT, "rectangle"},
	{RA_PLOT_SYM_RECT_FULL, "rectangle full"},
	{RA_PLOT_SYM_DIAMOND, "diamond"},
	{RA_PLOT_SYM_DIAMOND_FULL, "diamond full"},
	{RA_PLOT_SYM_TRIANGLE, "triangle"},
	{RA_PLOT_SYM_TRIANGLE_FULL, "triangle full"},
}; /* ra_plot_symbol */


static struct ra_plot_object ra_plot_line[] = {
	{RA_PLOT_LINE_SOLID, "solid"},
	{RA_PLOT_LINE_DASH, "dashed"},
	{RA_PLOT_LINE_DOT, "dotted"},
	{RA_PLOT_LINE_DASHDOT, "dash-dotted"},
}; /* ra_plot_line */


static struct ra_plot_object ra_plot_time_format[] = {
	{RA_PLOT_TIME_MSEC, "milliseconds"},
	{RA_PLOT_TIME_SEC, "seconds"},
	{RA_PLOT_TIME_MIN, "minutes"},
	{RA_PLOT_TIME_HOUR, "hours"},
	{RA_PLOT_TIME_DAY, "days"},
	{RA_PLOT_TIME_WEEK, "weeks"},
	{RA_PLOT_TIME_MONTH, "months"},
	{RA_PLOT_TIME_YEAR, "years"},
}; /* ra_plot_time_format */


static struct ra_plot_object ra_plot_log_type[] = {
	{RA_PLOT_LOG_10, "log10"},
	{RA_PLOT_LOG_2, "log2"},
	{RA_PLOT_LOG_LN, "ln"},
}; /* ra_plot_log_type */


#endif /* PLOT_DEFINES */
