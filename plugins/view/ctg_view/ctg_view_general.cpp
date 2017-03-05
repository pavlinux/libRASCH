/*----------------------------------------------------------------------------
 * ctg_view_general.cpp
 *
 *
 *
 * Author(s): Raphael Schneider (librasch@gmail.com)
 *
 * Copyright (C) 2002-2008, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifdef _AFXDLL
#include "stdafx.h"
#endif // _AFXDLL

#include <stdio.h>
#include <string.h>
#include <list>

#define _LIBRASCH_BUILD
#include <pl_general.h>

#define _CTG_ANNOTS
#include "ctg_view_general.h"

using namespace std;


ctg_view_general::ctg_view_general(meas_handle h):ts_view_general(h)
{
	strncpy(_plugin_name, "ctg-view", RA_VALUE_NAME_MAX);

	_remove_mean = false;
	_filter_powerline_noise = false;

	_prop_uc_templ = NULL;

	_first_pos_idx = _last_pos_idx = -1;
	_cursor_pos = 0;
	_ev_idx_save = -2;

	_fixed_height = false;

// 	if (h != NULL)
// 		init();

	_num_annot_predefs = sizeof(ctg_annot) / sizeof(char *);
	_annot_predefs = new char *[_num_annot_predefs];
	for (int i = 0; i < _num_annot_predefs; i++)
	{
		_annot_predefs[i] = new char[strlen(ctg_annot[i]) + 1];
		strcpy(_annot_predefs[i], ctg_annot[i]);
	}
} // constructor


ctg_view_general::~ctg_view_general()
{
	ra_comm_del(_mh, (void *)this, NULL);
} // destructor


void
ctg_view_general::init()
{
	_ts_opt.ch_height_dynamic = 0;
	_ts_opt.show_ch_info = 0;
	_left_space = 0;

	double mm_5 = 5.0 * _pixmm_v;
	int fhr_height = (int)(((210 - 50) / 10) * mm_5);
	int toco_height = (int)(((100- 0) / 10) * mm_5);
	_height_scale = 1.0;
	if (!_fixed_height)
	{
		int h = _screen_height - _top_space - _bottom_space;
		_height_scale = (double)h / (double)(fhr_height + toco_height + mm_5);
		if (_height_scale < 0.5)
			_height_scale = 0.5;

		mm_5 *= _height_scale;

		fhr_height = (int)(((210 - 50) / 10) * mm_5);
	}

	// "correct" channel draw settings
	for (int i = 0; i < _num_ch; i++)
	{
		_ch[i].use_mm_per_unit = 1;
		if (_ch[i].type == RA_CH_TYPE_CTG_FHR)
		{
			_ch[i].fixed_top_win_pos = 0;
			_ch[i].info_space = 0;
			_ch[i].draw_space = fhr_height;
			_ch[i].min_value = 50;
		}
		if (_ch[i].type == RA_CH_TYPE_CTG_UC)
		{
			_ch[i].fixed_top_win_pos = fhr_height + (int)(2 * mm_5);
			_ch[i].info_space = 0;
			_ch[i].draw_space = (int)(8 * mm_5);
			_ch[i].min_value = 0;

			value_handle vh = ra_value_malloc();
			ra_value_set_number(vh, _ch[i].ch_number);
			ra_info_get(_mh, RA_INFO_CH_XSCALE_D, vh);
			_x_scale = ra_value_get_double(vh);
			ra_value_free(vh);
		}

		ts_view_general::calc_scale(i);
	}

	get_event_infos();
} // init()


void
ctg_view_general::get_event_infos()
{
	_first_pos_idx = _last_pos_idx = -1;
	_ev_idx = -1;
	_cursor_pos = 0;

	eval_handle eh = ra_eval_get_default(_mh);
	if (!eh)
		return;
 
	if ((ra_class_get(eh, "uterine-contraction", _vh) != 0) ||
		(ra_value_get_num_elem(_vh) <= 0))
	{
		return;
	}
	const void **clh_s = ra_value_get_voidp_array(_vh);
	if (clh_s == NULL)
		return;
	// use first event-class (TODO: think how to handle additional event-class)
	_clh = (class_handle)(clh_s[0]);

	ra_class_get_events(_clh, -1, -1, 0, 1, _vh);
	_num_events = ra_value_get_num_elem(_vh);
	_event_ids = new long[_num_events];
	memcpy(_event_ids, ra_value_get_long_array(_vh), sizeof(long) * _num_events);

	_prop_pos = ra_prop_get(_clh, "uc-max-pos");
	if (!_prop_pos)
		return;

	_prop_uc_templ = ra_prop_get(_clh, "uc-template");

	return;
} // get_event_infos()


double
ctg_view_general::draw_calc_max_value(int ch)
{
	double max;
	if (_ch[ch].type == RA_CH_TYPE_CTG_FHR)
		max = 210;
	if (_ch[ch].type == RA_CH_TYPE_CTG_UC)
		max = 100;

	return max;
} // draw_calc_max_value()


bool
ctg_view_general::key_press_char(char c, bool shift, bool ctrl)
{
	long pos = _curr_pos + (long)((double)(_cursor_pos - _left_space) / _max_xscale);

	bool edit = false;
	bool redraw = true;
	bool handled = true;
	switch (c)
	{
	case 'M': // maximum uterine contraction
		if (_ev_idx == -1)
			edit = insert_max_uc_edit(pos);
		break;
	case 'D':		//delete
		if (_ev_idx >= 0)
		{
			delete_event();
			edit = true;
		}
		break;
	case ' ':		// show next sort position
		next_sort();
		break;
	case 'B':		// show prev. sort pos.
		prev_sort();
		break;
	default:
		redraw = false;
		handled = false;
		break;
	}

	if (edit)
	{
		get_event_infos();
		_block_signals = true;
		ra_comm_emit(_mh, "ctg-view", "eval-change", 0, NULL);
		_block_signals = false;
	}

	if (!handled)
		redraw = ts_view_general::key_press_char(c, shift, ctrl);

	return redraw;
} // key_press_char()


bool
ctg_view_general::insert_max_uc_edit(long pos)
{
	bool edit = true;

	if (_prop_pos == NULL)
		create_eval();

	long ev_id = ra_class_add_event(_clh, pos, pos);
	ra_value_set_long(_vh, pos);
	ra_prop_set_value(_prop_pos, ev_id, -1, _vh);
	ra_value_set_long(_vh, -1);
	ra_prop_set_value(_prop_uc_templ, ev_id, -1, _vh);

	send_add_event(ev_id);

	return edit;
} // insert_max_uc_edit()


void
ctg_view_general::create_eval()
{
	if (!_eh)
	{
		_eh = ra_eval_add(_mh, "libRASCH", "evaluation created inside of libRASCH (ctg-view plugin)", 0);
		if (!_eh)
		{
			// TODO: make error-code for 'create eval failed'
			_ra_set_error(_mh, RA_ERR_ERROR, "ctg-view");
			return;
		}
	}

	if (!_clh)
	{
		_clh = ra_class_add_predef(_eh, "uterine-contraction");
	}

	if (!_prop_pos && _clh)
	{
		_prop_pos = ra_prop_add_predef(_clh, "uc-max-pos");
	}
} // create_eval();


void
ctg_view_general::delete_event()
{
	struct comm_para *para = new struct comm_para[2];
	para[0].type = PARA_VOID_POINTER;
	para[0].v.pv = _clh;
	para[1].type = PARA_LONG;
	para[1].v.l = _event_ids[_ev_idx];
	para[2].type = PARA_LONG;
	para[2].v.l = ra_class_get_prev_event(_clh, _event_ids[_ev_idx]);
	para[3].type = PARA_LONG;
	para[3].v.l = ra_class_get_next_event(_clh, _event_ids[_ev_idx]);

	_block_signals = true;
	ra_comm_emit(_mh, "ctg-view", "del-event-begin", 2, para);
	_block_signals = false;

	ra_class_del_event(_clh, _event_ids[_ev_idx]);

	_block_signals = true;
	ra_comm_emit(_mh, "ctg-view", "del-event-end", 2, para);
	_block_signals = false;

	delete[]para;

	// set position and actual event to next event
	long idx_save = _ev_idx;

	get_event_infos();

	if (idx_save > (_num_events - 1))
		idx_save = _num_events - 1;

	set_event(NULL, _event_ids[idx_save]);
} // delete_event()


void
ctg_view_general::get_uc_pos(int **x, long **pos, int *num)
{
	if (_num_events <= 0)
		return;

	_first_pos_idx = _last_pos_idx = -1;

	list < int >data_x;
	list < long >data_pos;

	*x = NULL;
	*pos = NULL;

	*num = 0;

	for (long l = 0; l < _num_events; l++)
	{
		if (ra_prop_get_value(_prop_pos, _event_ids[l], -1, _vh) != 0)
			break;
		long p = ra_value_get_long(_vh);

		if (p < _curr_pos)
			continue;
		if (p > (_curr_pos + _page_width))
			break;

		if (_first_pos_idx == -1)
			_first_pos_idx = _last_pos_idx = l;
		if (l < _first_pos_idx)
			_first_pos_idx = l;
		if (l > _last_pos_idx)
			_last_pos_idx = l;

		int x = (int) ((double) (p - _curr_pos) * _max_xscale) + _left_space;
		data_x.push_back(x);
		data_pos.push_back(l);
		(*num)++;
	}

	if (*num > 0)
	{
		*x = new int[*num];
		*pos = new long[*num];
		for (int i = 0; i < *num; i++)
		{
			(*x)[i] = data_x.front();
			data_x.pop_front();
			(*pos)[i] = data_pos.front();
			data_pos.pop_front();
		}
	}
} // get_uc_pos()


int
ctg_view_general::mouse_press(int x)
{
	long ev = get_sel_pos(x - _left_space);
	if (ev != -1)
	{
		if (ra_prop_get_value(_prop_pos, _event_ids[ev], -1, _vh) == 0)
		{
			long pos = ra_value_get_long(_vh);
			_cursor_pos = (int) ((double) (pos - _curr_pos) * _max_xscale) + _left_space;
			_ev_idx = ev;
		}
		else
		{
			_ev_idx = -1;
			_cursor_pos = x;
		}
	}
	else
	{
		_ev_idx = -1;
		_cursor_pos = x;
	}

	return 0;
} // mouse_press()


long
ctg_view_general::get_sel_pos(int x)
{
	if (_num_events <= 0)
		return -1;
	
	long pos = _curr_pos + (long) ((double) x / _max_xscale);

	long min = pos - (long) (10.0 / _max_xscale);	// +-10 pixel
	long max = pos + (long) (10.0 / _max_xscale);

	value_handle vh_min = ra_value_malloc();
	ra_value_set_long(vh_min, min);
	value_handle vh_max = ra_value_malloc();
	ra_value_set_long(vh_max, max);
	
	ra_prop_get_events(_prop_pos, vh_min, vh_max, _curr_channel, _vh);

	ra_value_free(vh_min);
	ra_value_free(vh_max);

	long idx = -1;
	if (ra_value_get_num_elem(_vh) > 0)
	{
		long id = ra_value_get_long_array(_vh)[0]; // get only first one
		idx = get_event_index(id);
	}

	return idx;
} // get_sel_pos()


void
ctg_view_general::sort_general(prop_handle ph, double value)
{
	_num_sort = 0;
	_curr_sort = 0;
	if (_num_events <= 0)
		return;
	if (_sort)
	{
		delete[] _sort;
		_sort = NULL;
	}

	if (!ph)
		return;
	class_handle clh = ra_class_get_handle(ph);

	value_handle vh_id = ra_value_malloc();
	ra_class_get_events(clh, -1, -1, 0, 1, vh_id);
	
	value_handle vh = ra_value_malloc();
	long n = ra_value_get_num_elem(vh_id);
	_sort = new long[n];
	const long *ev_id = ra_value_get_long_array(vh_id);
	for (long l = 0; l < n; l++)
	{
		ra_prop_get_value(ph, ev_id[l], _curr_channel, vh);
		double v = ra_value_get_double(vh);
		if (v == value)
			_sort[_num_sort++] = l;
	}

	ra_value_free(vh);
	ra_value_free(vh_id);

	if (_num_sort > 0)
	{
		_curr_sort = -1;
		next_sort();
	}
} // sort_general()


void
ctg_view_general::use_fixed_height(bool flag)
{
	if (flag == _fixed_height)
		return;

	_fixed_height = flag;
	init();
	update();
} // use_fixed_height()


void
ctg_view_general::save_settings(const char *file)
{
	ts_view_general::save_settings(file);
} // save_settings()


void
ctg_view_general::load_settings(const char *file)
{
	ts_view_general::load_settings(file);
} // load_settings()

