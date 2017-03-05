/*----------------------------------------------------------------------------
 * ecg_view_general.h
 *
 *
 *
 * Author(s): Raphael Schneider (librasch@gmail.com)
 *
 * Copyright (C) 2002-2010, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifndef _ECG_VIEW_GENERAL_H
#define _ECG_VIEW_GENERAL_H

#include "../ts_view/ts_view_general.h"

#define EDIT_SPACE         500
#define EDIT_CURVE_LEN     (EDIT_SPACE - 20)


#define MORPH_TYPE_S       12
#define MORPH_TYPE_R       21
#define MORPH_TYPE_QR     121
#define MORPH_TYPE_RS     212
#define MORPH_TYPE_QRS   1212
#define MORPH_TYPE_RSR   2121

#ifdef WIN32
#define IDX_CORR 0
#else
#define IDX_CORR 0
#endif

#define MORPH_TYPE_S_IDX   1 - IDX_CORR
#define MORPH_TYPE_R_IDX   2 - IDX_CORR
#define MORPH_TYPE_QR_IDX  3 - IDX_CORR
#define MORPH_TYPE_RS_IDX  4 - IDX_CORR
#define MORPH_TYPE_QRS_IDX 5 - IDX_CORR
#define MORPH_TYPE_RSR_IDX 6 - IDX_CORR


#define MORPH_TYPE_NEG          12
#define MORPH_TYPE_POS          21
#define MORPH_TYPE_BI_NEG_POS  121
#define MORPH_TYPE_BI_POS_NEG  212

#define MORPH_TYPE_NEG_IDX         1 - IDX_CORR
#define MORPH_TYPE_POS_IDX         2 - IDX_CORR
#define MORPH_TYPE_BI_NEG_POS_IDX  3 - IDX_CORR
#define MORPH_TYPE_BI_POS_NEG_IDX  4 - IDX_CORR


struct edit_beat_data
{
	long event_id;
	long event_idx;
	long ch;
	long ch_idx;
	long annot;
	long template_num;
	long num_template_beats;
	long pos;
	long fiducial_pos;

	long num_data;
	double *data;
	double *data_filt;
	double *data_filt2;
	double max;
	double mean;
	long start;
	long end;

	long draw_area_top;
	long draw_area_bottom;
	long draw_area;
	double xscale;
	double yscale;

	long interp_factor;
	long filter_len;
	double *filter_coef;
	long filter_len2;
	double *filter_coef2;

	long morph_flags;

	bool p_valid;
	short p_start, p_end;
	short p_start_save, p_end_save;
	long p_type, p_type_save;

	bool qrs_valid;
	short qrs_start, qrs_end;
	short qrs_start_save, qrs_end_save;
	long qrs_type, qrs_type_save;

	bool t_valid;
	short t_start, t_end;
	short t_start_save, t_end_save;
	long t_type, t_type_save;
};  // struct edit_beat_data


class ecg_view_general:virtual public ts_view_general
{
public:
	ecg_view_general(meas_handle h);
	~ecg_view_general();

	void init();
	void get_eventtypes();
	virtual void sort_general(prop_handle ph, double value);
	virtual void select_events(class_handle clh, long num, long *ev_ids);

	virtual void options_changed();

	virtual void save_settings(const char *file);
	virtual void load_settings(const char *file);

protected:
	virtual bool key_press_char(char c, bool shift, bool ctrl);

	void get_beat_pos(int **x, long **pos, int *num);
	void get_arr_events(char ***arr_events, long **start, long **end, int *n_arr_events);

	int mouse_press(int x, int y, bool shift, bool dbl_click);
	long get_sel_pos(int x);
	int get_inflection_point(int x, int ch);

	bool do_beat_edit(long pos_id, long pos, long classification, bool shift);
	void insert_beat(long pos, long classification, bool pos_is_fixed=false);
	void get_fiducial_point(long ev_id);
	void get_template(long ev_id);
	void change_class(long index, long classification);
	void change_templ_class(long index, long classification);
	void calc_single_rri(int index);

	void delete_beat();
	void do_delete_beat(long ev_id);

	void correct_neighbors(long ev_id=-1);
	void post_process();

	int set_wave_delimiter(long pos, prop_handle prop);

	void sort_ch_found(int num);
	int num_bits(long v);

	void sort_rri(bool short_to_long, bool ignore_artifacts);
	void sort_class(int classification);
	void sort_template(int templ_num);
	void sort_template_corr(bool highest_first);

	void remove_ch_morph_flags(long clean_mask, long curr_ch);
	void remove_ev_morph_flags(long clean_mask, long ev_id);
	void remove_single_morph_flags(long clean_mask, long ev_id, long ch);

	void toggle_edit_mode();
	int fill_edit_beat_data();
	int prepare_edit_filter();
	int filter_data();
	void redo_wave_boundaries(long p_type, long qrs_type, long t_type, bool save, bool apply_all,
				  void (*callback) (const char *, int ));
	int get_template_events(long event_id, long *num_ev, long **ev_ids);
	int get_same_annot_events(long event_id, long *num_ev, long **ev_ids);

	void mouse_press_edit_area(int x, int y);

	void set_area_to_artifacts();
	void set_area_to_normals();
	void set_area_to_pacs();
	void set_area_to_pvcs();
	void set_area_to_paced();
	void set_area_beats(int beat_class, int beat_sub_class);
	void remove_beats_in_area();

	plugin_handle _plugin_rr;
	plugin_handle _plugin_fiducial;
	plugin_handle _plugin_detect_ecg;

	char _class_name[EVAL_MAX_NAME];

	prop_handle _prop_ch;
	prop_handle _prop_class;
	prop_handle _prop_syst;
	prop_handle _prop_dias;
	prop_handle _prop_mean;
	prop_handle _prop_syst_pos;
	prop_handle _prop_dias_pos;
	prop_handle _prop_rri;
	prop_handle _prop_rri_class;
	prop_handle _prop_qrs_temporal;
	prop_handle _prop_rri_ref;
	prop_handle _prop_rri_num_ref;
	prop_handle _prop_ecg_flags;

	prop_handle _prop_morph_flags;
	prop_handle _prop_p_start;
	prop_handle _prop_p_peak_1;
	prop_handle _prop_p_peak_2;
	prop_handle _prop_p_end;
	prop_handle _prop_p_type;
	prop_handle _prop_p_width;
	prop_handle _prop_qrs_start;
	prop_handle _prop_q;
	prop_handle _prop_r;
	prop_handle _prop_s;
	prop_handle _prop_rs;
	prop_handle _prop_qrs_end;
	prop_handle _prop_qrs_type;
	prop_handle _prop_qrs_width;
	prop_handle _prop_t_start;
	prop_handle _prop_t_peak_1;
	prop_handle _prop_t_peak_2;
	prop_handle _prop_t_end;
	prop_handle _prop_t_type;
	prop_handle _prop_t_width;

	prop_handle _prop_pq;
	prop_handle _prop_qt;
	prop_handle _prop_qtc;

	class_handle _arr_clh;
	prop_handle _arr_event_evprop;

	bool _in_edit_mode;
	struct edit_beat_data _edit_beat;

	bool _process_waiting;
	bool _show_morph_pos;
	bool _show_p_wave_pos;
	bool _show_qrs_pos;
	bool _show_t_wave_pos;

	bool _show_lowpass_edit_signal;
	bool _show_highpass_edit_signal;
}; // class ecg_view_general


#endif // _ECG_VIEW_GENERAL_H
