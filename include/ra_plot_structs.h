/*----------------------------------------------------------------------------
 * ra_plot_structs.h
 *
 *
 *
 * Author(s): Raphael Schneider (ra@med1.med.tum.de)
 *
 * Copyright (C) 2002-2007, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifndef RA_PLOT_STRUCTS_H
#define RA_PLOT_STRUCTS_H

#include <ra.h>

/* TODO: use continues values (for this, in Qt a combo-box with item-value property is needed) */
#define RA_PLOT_SYM_CIRCLE           0
#define RA_PLOT_SYM_CIRCLE_FULL      1
#define RA_PLOT_SYM_RECT             2
#define RA_PLOT_SYM_RECT_FULL        3
#define RA_PLOT_SYM_DIAMOND          4
#define RA_PLOT_SYM_DIAMOND_FULL     5
#define RA_PLOT_SYM_TRIANGLE         6
#define RA_PLOT_SYM_TRIANGLE_FULL    7

#define RA_PLOT_LINE_SOLID     0
#define RA_PLOT_LINE_DASH      1
#define RA_PLOT_LINE_DOT       2
#define RA_PLOT_LINE_DASHDOT   3

#define RA_PLOT_TIME_MSEC   0
#define RA_PLOT_TIME_SEC    1
#define RA_PLOT_TIME_MIN    2
#define RA_PLOT_TIME_HOUR   3
#define RA_PLOT_TIME_DAY    4
#define RA_PLOT_TIME_WEEK   5
#define RA_PLOT_TIME_MONTH  6
#define RA_PLOT_TIME_YEAR   7

#define RA_PLOT_LOG_10  0
#define RA_PLOT_LOG_2   1
#define RA_PLOT_LOG_LN  2


#define PLOT_OBJECT_LEN  50


struct ra_plot_axis
{
	int event_class_idx;
	char prop_name[EVAL_MAX_NAME];
	prop_handle event_property;

	char unit[EVAL_MAX_UNIT];

	int ch;

	int use_min, use_max;
	double min, max;

	int is_time;
	int time_format;
	int plot_log;
	int log_type;
}; /* struct ra_plot_axis */


struct ra_plot_pair
{
	/* index for ra_plot_options.axis array */
	int x_idx;		/* x-axis */
	int y_idx;		/* y-axis */
	int use_z_axis;
	int z_idx;		/* z-axis */

	int plot_symbol;
	int symbol_type;
	int sym_r, sym_g, sym_b;
	int plot_line;
	int line_type;
	int line_r, line_g, line_b;

	char name[PLOT_OBJECT_LEN * 3];

	/* TODO: 
	   - data specific for 3d-plot
	   - handle additional analyses (e.g. regression line)
	 */
}; /* struct ra_plot_pair */

struct ra_plot_options
{
	eval_handle eval;
	int num_event_class;
	class_handle *clh;
	char **class_name;

	int num_plot_pairs;
	struct ra_plot_pair *pair;

	int num_axis;
	struct ra_plot_axis *axis;

	int show_grid;
	int frame_type;
	int show_legend;
}; /* struct ra_plot_options */


#endif /* RA_PLOT_STRUCTS_H */
