/*----------------------------------------------------------------------------
 * ts_view_general.h
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

#ifndef _TS_VIEW_GENERAL_H
#define _TS_VIEW_GENERAL_H

#include <stdlib.h>
#include <libxml/tree.h>

#define _LIBRASCH_BUILD
#include <ra.h>
#include <ra_pl_comm.h>
#include <ra_priv.h>


/* states when marking area */
#define NOT_MARKING_AREA     0	/* = not in marking state */
#define MARKING_AREA_START   1  /* marking started, but no area marked so far */
#define MARKING_AREA         2	/* in the middle of marking the area */
#define MARKING_AREA_MANUAL  3	/* marking the area by setting the start-/end-positions w/o mouse selection*/


//
// positions in channel-strip:
//   -----------------------------------  = top_win_pos
//     channel info (e.g. annotations)                    | = info_space
//   -----------------------------------  = highest pos
//                                                        |
//              channel-strip                             | = draw_space
//                                                        |
//   -----------------------------------  = lowest pos
//
struct ch_info
{
	int ch_number;		// which rec-channel is this channel

	bool show_it;

	bool highlight_channel;

	bool centered;
	// signal is centered around some value
	double center_value;
	int center_pos;		// where in chanel-area is zero (in percent: 0% lowest pos, 100% highest pos)
	bool inverse;

	// signal has min/max values (in samplevalues)
	double min_value;
	double max_value;
	bool use_mm_per_unit;	// true: lowest pos is min, highest pos depends on mm/unit
	                        // false: lowest pos is min, highest pos is max
	double mm_per_unit;

	char name[20];
	char unit[20];
	double samplerate;
	double amp_res;
	long num_samples;

	double xscale;
	double yscale;

	// navigation infos
	double pos_factor;	// factor to get signal-pos from overall view-pos

	// window-position of channel
	int fixed_top_win_pos;	/* set this value to the top-pos of the channel when no dynamic adaption should be used */
	int top_win_pos;
	int info_space;
	int draw_space;

	int type;

	/* some signals (e.g. CTG) needs that some values are not shown */
	int do_ignore_value;
	double ignore_value;

	double mean;		/* mean value of the last displayed data */

	/* info about scale */
	double scale_start;
	double scale_step;
	double scale_end;
}; // struct ch_info


struct ts_options
{
	double mm_per_sec;

	bool show_status;
	bool show_ch_info;
	bool show_ch_scale;
	bool plot_grid;
	bool show_annot;

	bool ch_height_dynamic;
	int ch_height;
}; // struct ts_options


struct annot_info
{
	long start;
	long end;

	char *text;
	bool noise_flag;
	bool ignore_flag;

	long event_id;
	int ch;
	int session;
}; /* struct annot_info */


struct curr_annot_info
{
	long annot_idx;
	
	char *text;
	int text_x;
	int text_width;
	int text_height;

	long begin;
	long end;
	int y;

	/* flags indicating if begin/end is not shown */
	bool begin_not_shown;
	bool end_not_shown;
}; /* struct curr_annot_info */


/* area of interest */
struct aoi
{
	int x, y, w, h;

	int annot;		/* index of annotation (-1 if no annotation) */
}; /* struct aoi */


/* template infos */
struct templ_info
{
	long part_id;

	long num_events;
	long *event_ids;
}; /* struct templ info */


class ts_view_general
{
public:
	ts_view_general(meas_handle h, int num_ch = 0, int *ch = NULL);
	virtual ~ts_view_general();

	virtual void update();
	virtual void set_x_resolution(double , void *) { };
	void init(meas_handle h);
	bool block_signals() { return _block_signals; }
	bool set_pos(long pos, double scale, bool from_signal = false);

	void in_gui_event(bool in_ev);

	virtual void init_scrollbar(long , long , double ) { };
	virtual void set_scrollbar_pos(long ) { };

	virtual void read_eval(); /* TODO: THINK how this function can be used in derived classes to have only one function */

	virtual void set_event(class_handle clh, long event, bool from_signal = false);

	void calc_max_scale_factors();

	bool has_input_focus() { return _has_input_focus; }
	void set_input_focus(bool flag) { _has_input_focus = flag; }

	virtual void save_settings(const char *file);
	virtual void load_settings(const char *file);

	virtual void options_changed();

protected:
	void get_ch_info(int ch);
	void calc_scale(int ch);
	void get_template_infos(class_handle clh);

	bool next_page(bool whole_page);
	bool prev_page(bool whole_page);
	bool goto_begin();
	bool goto_end();
	bool goto_pos(long new_pos, bool center_pos);

	void paint(long width, long height);
	void format_statusbar(char **status_text);
	void format_sec(long sec, char **text);
	void format_msec(long msec, char **text);
	void ch_draw_data(int ch, int line, int **xa, int **ya, int **ignore, int *num);

	void check_curr_pos();
	void send_pos();
	void send_init_scrollbar();
	void send_eval_change();

	void send_x_res(long parent);
	void send_y_res(long ch);

	void send_input_focus_change();

	virtual void highlight_area() { };
	void start_area_mark(int x);
	void change_area_mark(int x);
	void end_area_mark(int x);

	virtual void plot_cursor() { }

	// helper functions for drawing routine
	virtual double draw_calc_max_value(int ch);
	virtual inline int draw_calc_amplitude(double max, double val, int ch);

	int y_to_channel(int y);
	int su_to_screen(long pos);  /* convert sample unit to screen pos */
	int check_x_pos(int x);	/* check that x is inside the visible page */
	int get_ch_index(long ch_number);

	virtual void auto_adjust_y_axis(int y);

	/* key functions */
	virtual bool key_press_char(char c, bool shift, bool ctrl);
	virtual void unset_cursor() { ; }
	virtual void set_busy_cursor() { ; }

	bool insert_comment();
	virtual int annot_dlg(char **, bool *, bool *, int *) { return -1; }

	void init_comment_data();
	bool search_flag(char *buf, const char *flag);
	void get_comments(void *p, struct curr_annot_info **info, int *n_comments);
	int get_annot_height(long ch);
	virtual void get_text_extent(void * /*paint_handle*/, const char * /*text*/, int *w, int *h) { *w = *h = 0; }
	void place_comments(struct curr_annot_info *info, int n_comments);
	void update_comment(int idx);
	void delete_comment(int idx);

	int find_aoi(int x, int y);

	void change_session(long num);

	long get_event_index(long id, long last_idx = 0);

	void sort_annotations();
	void next_sort();
	void prev_sort();
	void correct_sort(long idx, long offset);

	void send_event();
	void send_add_event(long event);

    void remove_events_in_area();

	virtual int save_general_settings(xmlNodePtr parent);
	virtual int save_single_ch_settings(int ch, xmlNodePtr parent);
	virtual int load_general_settings(xmlNodePtr parent);
	virtual int load_single_ch_settings(xmlNodePtr parent);
	
	// helper functions for load_single_ch_settings()
	void read_xml_prop_bool(xmlNodePtr node, const char *prop, bool &value);
	void read_xml_prop_int(xmlNodePtr node, const char *prop, int &value);
	void read_xml_prop_double(xmlNodePtr node, const char *prop, double &value);

	int get_ch_settings_file(char *buf, int buf_size);

	// functions to get the scales for each channel
	int get_ch_axis_div();
	double get_step(double range);
	double raise(double x, int y);
	int get_tics_precision(double step);

	// name of the plugin needed for inter-plugin-communications
	char _plugin_name[RA_VALUE_NAME_MAX];

	// value-handle which can be used inside ONE function
	// (to avoid multiple ra_value_malloc() and ra_value_free() calls)
	value_handle _vh;

	// lib handles
	meas_handle _mh;
	rec_handle _rh;
	// event related vars
	eval_handle _eh;
	// if only one event-class is handled, ts-view provides the functionality
	// (if more than one has to be handled, write your own set_event() function)
	class_handle _clh;
	prop_handle _prop_pos;
	// annotations related vars
	class_handle _annot_class;
	prop_handle _annot_prop;
	long _num_annot;
	struct annot_info *_annot;	

	int _num_annot_predefs;
	char **_annot_predefs;

	/* area's of interest of the current part shown */
	int _num_aoi;
	struct aoi *_aoi;

	bool _dont_paint;
	bool _block_signals;
	bool _ts_handle_cursor;
	bool _dbl_left_click_handled;
	bool _has_input_focus;

	// flag showing if change was already done (_already_changed) while in
	// gui-event-loop (_in_gui_event)
	// (e.g. if key-event to show next page is send than it is possible that
	// position change was already done by handling a signal; in this cases
	// I must decide if change done by signal is enough (-> ignore event) or
	// if some additional things must be done)
	bool _in_gui_event;
	bool _already_changed;

	struct ts_options _ts_opt;

	// navigation
	long _curr_pos;		// current pos in signal (in sample units of the channel with
	                        // the highest samplerate)
	int _curr_channel;	// last channel clicked
	long _page_width;	// width of viewable area (in sample units, see above)
	int _screen_width;	// width of viewable area (in pixel)
	int _screen_height;	// height of viewable area (in pixel)
	double _max_samplerate;
	double _max_xscale;
	long _max_num_samples;

	/* TODO: check if this is still used, because different samplerates are supported */
	double _x_scale;  // factor to scale positions from evaluation to positons in the 
	                  // channel (for measurements with different samplerates);
	                  // up to now this assumes all channels in the current view have the
                          // same samplerate, it will be used at the moment only for derivative
	                  // classes (like ecg-view, cont-rr-view); TODO: think how to handle
	                  // ch with different samplerates in the same view

	long _start_sec;
	long _start_meas_sec;

	// screen layout (areas around the channel drawing area, values must be 0 if space is not used)
	int _top_space;
	int _bottom_space;
	int _left_space;
	int _right_space;

	// channels
	int _num_ch;
	struct ch_info *_ch;
	int _ch_info_offset;	/* x-pos in the window where ch-info starts */

	// resolution
	double _pixmm_v;
	double _pixmm_h;
	bool _fixed_height;
	double _height_scale;	/* scaling factor which is needed when a view has
				   specific areas which should fill the whole view
				   (e.g. ctg-view)*/

	// positions (used by derived views)
	int _cursor_pos_prev;
	int _cursor_ch_prev;
	int _cursor_pos;
	int _cursor_ch;
	bool _update_cursor_pos; /* flag if cursor position should be updated */

	long _num_events;
	long *_event_ids;
	long _ev_idx;
	// last event-index set or send or received
	long _ev_idx_save;

	// area selection
	bool _area_marked;
	int _area_mark_step;  // 1: mouse button pressed;  2: mouse moved (while mouse button pressed)
	long _area_start;
	long _area_end;

	// sorting
	long _num_sort;
	long _curr_sort;
	int _sort_is_pos;
	long *_sort;

	// flag if scrollbar is used (at the moment only for the MFC version)
	bool _use_scrollbar;

	// infos about sessions
	long _num_sessions;
	long _curr_session;

	// filter options
	bool _filter_powerline_noise;
	bool _remove_mean;

	/* template stuff */
	sum_handle _template_sh;
	long _num_templates;
	struct templ_info *_templates;

	// online signal specific stuff
	bool _is_online;
};  // class ts_view_general


inline int
ts_view_general::draw_calc_amplitude(double max, double val, int ch)
{
	int amp = 0;

	if (_ch[ch].centered)
	{
		if (_ch[ch].inverse)
			amp = (int) ((max - (-(val - _ch[ch].center_value))) * _ch[ch].yscale);
		else
			amp = (int) ((max - (val - _ch[ch].center_value)) * _ch[ch].yscale);
	}
	else
	{
		if (_ch[ch].use_mm_per_unit)
			amp = (int) ((max - (val - _ch[ch].min_value)) * _ch[ch].yscale);
		else
		{
			double range = _ch[ch].max_value - _ch[ch].min_value;
			double scale = (double) _ts_opt.ch_height / range;
			double v = _ch[ch].max_value - val;  // amp-res no longer needed because of ra_raw_get_UNIT
			amp = (int)(v * scale);
		}
	}

	return amp;
} // draw_calc_amplitude (inline)


#endif // _TS_VIEW_GENERAL_H
