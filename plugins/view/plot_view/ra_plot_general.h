/*----------------------------------------------------------------------------
 * ra_plot_general.h
 *
 *
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002-2005, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifndef RA_PLOT_GENERAL_H
#define RA_PLOT_GENERAL_H

#define _LIBRASCH_BUILD
#include <ra.h>
#include <ra_pl_comm.h>
#include <ra_plot_structs.h>


struct axis_info
{
	bool dont_draw_axis;

	int idx;
	double start, end;
	double step;

	bool is_time;
	int time_format;
	bool add_starttime;	// at the moment not used (TODO: extend dialog and use it)

	// axis is position in measurement (if unit is 'su' (sample-units))
	bool axis_is_pos;

	// if time-axis, values are shown differently (e.g. data is in sampleunits but it is
	// shown in minutes) and after the calculation of the begin and end of the axis (get_axis_div()),
	// the min- and max-value in ra_plot_axis must be corrected using scale
	double scale;

	bool opp_side;		// flag if axis is opposide than "normal" side
	int axis_num;		// number of axis on the side of the axis (e.g. there are
	// two axis on the left side, than this can be 1 or 2)

	char unit[20];
};				// struct axis_info


struct sort
{
	double x, y, z;
	int ign;
}; // struct sort



class ra_plot_general
{
 public:
	ra_plot_general(meas_handle h, struct ra_plot_options *opt);
	virtual ~ ra_plot_general();
	
	virtual void update();
	virtual void update_pos();
	void init();
	bool block_signals() { return _block_signals; }
	
	void set_sig_event(class_handle clh, long event);
	void set_pos_event(long pos);
	void collect_event_changes(int flag);
	int get_annot_data();
	int get_plot_data();
	
 protected:
	void free_data();
	int get_data(struct ra_plot_axis *a, double **data, long *len, int **ignore);
	void get_min_max(double *data, int *ignore_data, long n, double *min, double *max);
	int sort_data(double *x, double *y, double *z, long num, int *ignore);

	int set_axis_variables();
	int get_tics_precision(double step);
	int get_x_axis_info();
	int get_y_axis_info();
	int get_axis_div(struct axis_info *inf, int num);
	double get_step(double range);
	double raise(double x, int y);

	void set_time_infos(struct axis_info *inf, int num);
	double get_samplerate();

	void set_event(long event, bool from_signal = false);
	void check_curr_event();
	void send_event();

	// ra handles
	meas_handle _meas;

	// ra-signal related variables
	bool _block_signals;
	// flag showing if change was already done (_already_changed) while in
	// gui-event-loop (_in_gui_event)
	// (e.g. if key-event to show next page is send than it is possible that
	// position change was already done by handling a signal; in this cases
	// I must decide if change done by signal is enough (-> ignore event) or
	// if some additional things must be done)
	bool _in_gui_event;
	bool _already_changed;

	// axis specific infos
	int _num_x_axis;
	struct axis_info *_x_axis_info;
	int _num_y_axis;
	struct axis_info *_y_axis_info;

	int _x_axis_space;
	int _y_axis_space;

	// plot specific infos
	struct ra_plot_options *_opt;

	int _dont_response_ev_change;

	int _plot_area_top;
	int _plot_area_left;
	int _plot_area_right;
	int _plot_area_bottom;
	int _plot_area_width;	// will be set in gui specific paint-routine
	int _plot_area_height;	// dito

	long _num_pairs;	// value in _opt-struct can be changed by calling opt-dialog
	long *_num_data;
	long **_data_ev_id;
	double **_x_data;
	double **_y_data;
	double **_z_data;
	int **_ignore_data;

	double _x_scale;
	double _y_scale;

	int _pos_idx;
	long _curr_pos;
	long _curr_event;

	long _num_event_class;
	long *_num_events;
	long **_event_ids;

	long _num_annot;
	long *_annot_ev_id, *_annot_start, *_annot_end;
}; // class ra_plot_general


#endif // RA_PLOT_GENERAL_H
