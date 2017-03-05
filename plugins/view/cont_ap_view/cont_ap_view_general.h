/**
 * \file cont_ap_general.cpp
 *
 * GUI independent source.
 *
 * \author Raphael Schneider (librasch@gmail.com)
 */

/*--------------------------------------------------------------------------
 *
 * Copyright (C) 2002-2009, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *
 *--------------------------------------------------------------------------*/

#ifndef CONT_AP_VIEW_GENERAL_H
#define CONT_AP_VIEW_GENERAL_H

#include "../ts_view/ts_view_general.h"
#include <ra_ecg.h>

class cont_ap_view_general:virtual public ts_view_general
{
 public:
	cont_ap_view_general(meas_handle h);
	~cont_ap_view_general();

	void init();
	void get_eventtypes();
	virtual void sort_general(const char *evtype, double value);
	virtual void sort_bp(int type);
	virtual void sort_class(int classification);
	void sort_calib_seq();

	void add_event(class_handle clh, long event);
	void recalc_value(class_handle clh, long event);

	virtual void save_settings(const char *file);
	virtual void load_settings(const char *file);

 protected:
	void get_beat_pos(prop_handle prop, long ch, int **x, long **pos, int *num);
	void get_calib_seq(int *num_seq, long **start_pos, long **end_pos, long **ch, long **index);
	long find_beat_eval(long last_index, long pos, int after_pos);
	int mouse_press(int x, int y);
	long get_sel_pos(int x);
	virtual bool key_press_char(char c, bool shift, bool ctrl);

	void set_bp_value(prop_handle prop_amp, prop_handle prop_pos);

	void change_calib_seq(bool begin);
	void insert_calib_seq();
	void remove_calib_seq();

	void toggle_value_ok();

	void send_event_change(prop_handle ev_prop, long event_num);
	void handle_region_change_event(long start, long end, long ch);

	plugin_handle _plugin_ap;

	class_handle _clh_calib;

	prop_handle _prop_class;
	prop_handle _prop_qrs_pos;
	prop_handle _prop_syst;
	prop_handle _prop_dias;
	prop_handle _prop_mean;
	prop_handle _prop_syst_pos;
	prop_handle _prop_dias_pos;
	prop_handle _prop_flags;
	prop_handle _prop_ibi;
	prop_handle _prop_dpdt_min;
	prop_handle _prop_dpdt_max;
	prop_handle _prop_dpdt_min_pos;
	prop_handle _prop_dpdt_max_pos;
	prop_handle _calib_info;

	long _first_pos_idx;
	long _last_pos_idx;

	long _num_templates;
}; // class cont_ap_view_general


#endif // CONT_AP_VIEW_GENERAL_H
