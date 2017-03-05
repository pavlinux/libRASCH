/*----------------------------------------------------------------------------
 * ev_summary_view_general.h
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

#ifndef _EV_SUMMARY_VIEW_GENERAL_H
#define _EV_SUMMARY_VIEW_GENERAL_H

#define _LIBRASCH_BUILD
#include <ra_defines.h>

// space for event-summary in pixel
#define EV_SUMMARY_DIST     30	// distance between event-summaries
#define EV_SUMMARY_HEIGHT   50	// per channel

// pos of first event-summary
#define X_OFFSET  20
#define Y_OFFSET  40


struct summary_part_data
{
	long part_id;		// id of this summary-part

	long num_events;	// number of events used for this summary-part
	long *ev_ids;		// event-id's this summary-part is based on

	long **num_data;
	double ***raw_orig;	// the event-summary-raw-data saved for later use
	double ***raw;		// the event-summary-raw-data scaled for display
}; // struct summary_part_data


struct ch_info
{
	bool centered;
	// signal is centered around some value
	double center_value;	// most zero
	double sample_offset;	// which samplevalue is zero
	int center_pos;		// where in chanel-area is zero (in percent: 0% lowest pos, 100% highest pos)
	bool inverse;

	// signal has min/max values (in samplevalues)
	double min_value;
	double max_value;
	bool use_mm_per_unit;	// true: lowest pos is min, highest pos depends on mm/unit
	// false: lowest pos is min, highest pos is max

	double mm_per_unit;
	double samplerate;
	double amp_res;

	double xscale;
	double yscale;

	int type;
}; // struct ch_info


class ev_summary_view_general
{
 public:
	ev_summary_view_general(meas_handle mh, sum_handle sh);
	virtual ~ev_summary_view_general();

	bool init();
	bool get_summary_infos();
	void delete_part(long idx, bool del_in_eval, bool del_events);

	virtual void update();

	bool block_signals() { return _block_signals; }

	void set_x_res(double x_res);
	void set_ch_y_res(double mm_per_unit, long ch);

	void get_area_to_be_visible(int *x_start, int *x_end);

  	virtual void unset_cursor() { ; }
	virtual void set_busy_cursor() { ; }

 protected:
	meas_handle _mh;
	sum_handle _sh;

	bool _block_signals;

	long _num_dim;
	char **_dim_name;
	char **_dim_unit;

	long _num_ch;
	struct ch_info *_ch;
	long *_ch_number;
	long *_fiducial_offset;

	long _num_sum_part;
	struct summary_part_data *_sum_part;
	long *_part_order;

	int _curr_sum_part_idx;
	int _curr_sum_part_id;

	double _mm_per_sec;
	long _max_part_width;
	long _max_part_width_su;

	double _pixmm_v;
	double _pixmm_h;

	bool get_summary_part(long part_id, struct summary_part_data *sum_part);

	long get_summary_part_index(long part_id);

	void sort_ev_summaries();
	void scaling_info();
	void get_ch_info(rec_handle rh, int ch);
	void calc_scale(int ch);
	void scale_raw_data();
	double max_value(int ch);
	inline double scale_amplitude(double max, double val, int ch);
	double mean(double *v, long len);
	void get_max_part_width();

	void ev_summary_select();
	bool key_press(char c);
}; // class ev_summary_view_general


#endif // _EV_SUMMARY_VIEW_GENERAL_H
