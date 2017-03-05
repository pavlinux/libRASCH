/*----------------------------------------------------------------------------
 * ra_plot.h
 *
 *
 *
 * Author(s): Raphael Schneider (ra@med1.med.tum.de)
 *
 * Copyright (C) 2002-2005, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifndef RA_PLOT_H
#define RA_PLOT_H

#include <ra.h>


struct ra_plot_axis
{
	char event_name[EVAL_MAX_NAME];
	prop_handle event;

	int use_min, use_max;
	double min, max;

	int is_time;
	int time_format;
	int plot_log;
	int log_format;
};  /* struct ra_plot_data */


struct ra_plot_pair
{
	struct ra_plot_data x_axis;
	struct ra_plot_data y_axis;
	int use_z_axis;
	struct ra_plot_data z_axis;

	int plot_symbol;
	int symbol_type;
	long symbol_color;
	int plot_line;
	int line_type;
	long line_color;

	char *name;

	/* TODO: 
	   - data specific for 3d-plot
	   - handle analyses (e.g. regression line)
	 */
};  /* struct ra_plot_pair */


struct ra_plot_options
{
	eval_handle eval;
	class_handle clh;

	int num_plot_pairs;
	struct ra_plot_pair *pair;

	int show_grid;
	int frame_type;
	int show_legend;
};  /* struct ra_plot_options */


#endif /* RA_PLOT_H */
