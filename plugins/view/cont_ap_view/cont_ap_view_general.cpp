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

#ifdef _AFXDLL
#include "stdafx.h"
#endif // _AFXDLL

#include <stdio.h>
#include <string.h>
#include <list>

#define _LIBRASCH_BUILD
#include <pl_general.h>
#include <ra_ap_morphology.h>
#include <ra_fiducial_point.h>

#include "cont_ap_view_general.h"


using namespace std;


cont_ap_view_general::cont_ap_view_general(meas_handle h):ts_view_general(h)
{
	_plugin_ap = NULL;

	_first_pos_idx = _last_pos_idx = -1;
	_num_sort = _curr_sort = 0;
	_sort = NULL;

	_update_cursor_pos = false;

	_num_templates = 0;

	_clh = NULL;
	_clh_calib = NULL;

	_prop_class = NULL;
	_prop_qrs_pos = NULL;
	_prop_syst = NULL;
	_prop_dias = NULL;
	_prop_mean = NULL;
	_prop_syst_pos = NULL;
	_prop_dias_pos = NULL;
	_prop_flags = NULL;
	_prop_ibi = NULL;
	_prop_dpdt_min = NULL;
	_prop_dpdt_max = NULL;
	_prop_dpdt_min_pos = NULL;
	_prop_dpdt_max_pos = NULL;
	_calib_info = NULL;

	_remove_mean = false;

	if (h != NULL)
		init();
} // constructor

cont_ap_view_general::~cont_ap_view_general()
{
	ra_comm_del(_mh, (void *) this, NULL);
} // destructor


void
cont_ap_view_general::init()
{
	// look for ap_morphology plugin
	_plugin_ap = ra_plugin_get_by_name(ra_lib_handle_from_any_handle(_mh), "ap-morphology", 1);
	get_eventtypes();

	value_handle vh = ra_value_malloc();
	ra_value_set_number(vh, _ch[0].ch_number);
	ra_info_get(_mh, RA_INFO_CH_XSCALE_D, vh);
	_x_scale = ra_value_get_double(vh);
	ra_value_free(vh);

	// need for each channel a line to print out the RR values
	_top_space = 20 * _num_ch;
} // init()


void
cont_ap_view_general::get_eventtypes()
{
	_num_events = 0;
	if (_event_ids)
	{
		delete[] _event_ids;
		_event_ids = NULL;
	}
	_ev_idx = -1;

	_eh = ra_eval_get_default(_mh);
	if (!_eh)
		return;

	ra_class_get(_eh, "heartbeat", _vh);
	if (ra_value_get_num_elem(_vh) == 0)
	{
		// we need at least the 'heartbeat' event-class
		return;
	}
	if (ra_value_get_num_elem(_vh) > 1)
	{
		fprintf(stderr, "More than one 'heartbeat' event-class available.\n" \
			"At the moment only ONE event-class is supported\n");
	}
	_clh = (class_handle)(ra_value_get_voidp_array(_vh)[0]);

	// get number of events and store the event_id's
	ra_class_get_events(_clh, -1, -1, 0, 1, _vh);
	_num_events = ra_value_get_num_elem(_vh);
	_event_ids = new long[_num_events];
	// TODO: find out why the memcpy does not work (maybe some memory
	//       alignment problems) and fix it to get rid of the for loop
	//	memcpy(_event_ids, ra_value_get_long_array(_vh), sizeof(long) * _num_events);
	const long *temp_event_ids = ra_value_get_long_array(_vh);
	for (long l = 0; l < _num_events; l++)
		_event_ids[l] = temp_event_ids[l];

	_prop_qrs_pos = ra_prop_get(_clh, "qrs-pos");
 	_prop_pos = _prop_qrs_pos;
	_prop_class = ra_prop_get(_clh, "qrs-class");
	_prop_syst = ra_prop_get(_clh, "rr-systolic");
	_prop_dias = ra_prop_get(_clh, "rr-diastolic");
	_prop_mean = ra_prop_get(_clh, "rr-mean");
	_prop_syst_pos = ra_prop_get(_clh, "rr-systolic-pos");
	_prop_dias_pos = ra_prop_get(_clh, "rr-diastolic-pos");
	_prop_flags = ra_prop_get(_clh, "rr-flags");
	_prop_ibi = ra_prop_get(_clh, "ibi");
	_prop_dpdt_min = ra_prop_get(_clh, "rr-dpdt-min");
	_prop_dpdt_max = ra_prop_get(_clh, "rr-dpdt-max");
	_prop_dpdt_min_pos = ra_prop_get(_clh, "rr-dpdt-min-pos");
	_prop_dpdt_max_pos = ra_prop_get(_clh, "rr-dpdt-max-pos");

	ra_class_get(_eh, "rr-calibration", _vh);
	if (ra_value_get_num_elem(_vh) == 1)
	{
		_clh_calib = (class_handle)(ra_value_get_voidp_array(_vh)[0]);
		_calib_info = ra_prop_get(_clh_calib, "rr-calibration-info");
	}
} // get_eventtypes()


void
cont_ap_view_general::get_beat_pos(prop_handle prop, long ch, int **x, long **ev_id, int *num)
{
	*x = NULL;
	*ev_id = NULL;
	*num = 0;

	if ((_num_events <= 0) || (_clh == NULL) || (prop == NULL))
		return;

	ra_class_get_events(_clh, _curr_pos, (_curr_pos + _page_width), 0, 1, _vh);
	*num = ra_value_get_num_elem(_vh);

	if (*num <= 0)
		return;

	*ev_id = new long[*num];
	const long *ids = ra_value_get_long_array(_vh);
	int i;
	for (i = 0; i < *num; i++)
		(*ev_id)[i] = ids[i];
	
	*x = new int[*num];
	for (i = 0; i < *num; i++)
	{
		ra_prop_get_value(prop, (*ev_id)[i], ch, _vh);
		long p = ra_value_get_long(_vh);
		(*x)[i] = (int) ((double)(p - _curr_pos) * _max_xscale) + _left_space;;
	}
} // get_beat_pos()


int
cont_ap_view_general::mouse_press(int x, int y)
{
	int ch = y_to_channel(y);

	if (x < _left_space)
		return ch;

	long idx = get_sel_pos(x - _left_space);
	if ((idx != -1) && _clh)
		set_event(NULL, _event_ids[idx]);
	else
		_ev_idx = -1;
	
	_cursor_pos = x;
	_cursor_ch = ch;

	return ch;
} // mouse_press()


long
cont_ap_view_general::get_sel_pos(int x)
{
	if ((_num_events <= 0) || (_curr_channel < 0))
		return -1;
	
	long pos = _curr_pos + (long) ((double) x / _max_xscale);

	// pos-100 pixel ... pos+100 pixel
	long min = pos - (long)(100.0 / _max_xscale);
	long max = pos + (long)(100.0 / _max_xscale);

	value_handle vh_min = ra_value_malloc();
	ra_value_set_long(vh_min, min);
	value_handle vh_max = ra_value_malloc();
	ra_value_set_long(vh_max, max);
	
	// TODO: implement possibility to get pulse positions instead of QRS positions if no
	// ecg is available
	// QRS position is channel independent -> use '-1' (TODO: think about if it should be
	// possible to get events from the independent channel also)
	ra_prop_get_events(_prop_qrs_pos, vh_min, vh_max, -1, _vh);

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


bool cont_ap_view_general::key_press_char(char c, bool shift, bool ctrl)
{
	set_busy_cursor();

	bool redraw = true;
	bool handled = true;
	switch (c)
	{
	case '1':		// pos of diastole
		if (_ev_idx >= 0)
			set_bp_value(_prop_dias, _prop_dias_pos);
		break;
	case '2':		// pos of systole
		if (_ev_idx >= 0)
			set_bp_value(_prop_syst, _prop_syst_pos);
		break;
	case '4':		// begin of calibration sequence
		change_calib_seq(true);
		break;
	case '5':		// end of calibration sequence
		change_calib_seq(false);
		break;
	case 'I':  // insert calibration sequence
		insert_calib_seq();
		break;
	case 'R':  // remove calibration sequence
		remove_calib_seq();
		break;
	case 'X':  // toggle status of bloodpressure value (OK/not OK)
		if (_ev_idx >= 0)
			toggle_value_ok();
		break;
	case ' ':		// show next sort position
		next_sort();
		break;
	case 'B':		// show prev. sort pos.
		prev_sort();
		break;
	case 'C':
		redraw = true;
		sort_calib_seq();
		break;
	default:
		redraw = false;
		handled = false;
		break;
	}

	if (!handled)
		redraw = ts_view_general::key_press_char(c, shift, ctrl);

	unset_cursor();

	return redraw;
} // key_press_char()


void
cont_ap_view_general::set_bp_value(prop_handle prop_amp, prop_handle prop_pos)
{
	if (_ev_idx == -1)
		return;

	long pos = _curr_pos + (long) ((double) (_cursor_pos - _left_space) / _max_xscale);

	// TODO:
	// 1. look for local min/max
	double val;
	if (ra_raw_get_unit(_rh, _ch[_cursor_ch].ch_number, pos, 1, &val) != 1)
		return;

	ra_value_set_double(_vh, val);
	ra_prop_set_value(prop_amp, _event_ids[_ev_idx], _ch[_cursor_ch].ch_number, _vh);

	ra_value_set_double(_vh, (double)(pos / _x_scale));
	ra_prop_set_value(prop_pos, _event_ids[_ev_idx], _ch[_cursor_ch].ch_number, _vh);

	recalc_value(_clh, _event_ids[_ev_idx]);

	send_event_change(prop_amp, _event_ids[_ev_idx]);
	send_event_change(prop_pos, _event_ids[_ev_idx]);
} // set_bp_value()


void
cont_ap_view_general::change_calib_seq(bool begin)
{
	if (!_clh_calib)
		return;

	// find nearest calibration sequence
	int n;
	long *start, *end, *ch, *index;
	get_calib_seq(&n, &start, &end, &ch, &index);

	int curr = -1;
	long pos = _curr_pos + (long) ((double) (_cursor_pos - _left_space) / _max_xscale);
	int last_diff = -1;
	for (int i = 0; i < n; i++)
	{
		int ch_idx = get_ch_index(ch[i]);
		if (ch_idx < 0)
			continue;
		if (ch_idx != _cursor_ch)
			continue;

		long seq_pos = (start[i] + end[i]) / 2;
		long diff = abs(pos - seq_pos);
		if (diff < last_diff || (last_diff == -1))
			curr = i;
		last_diff = diff;
	}

	if (curr == -1)
	{
		if (start)
			delete[] start;
		if (end)
			delete[] end;
		if (index)
			delete[] index;

		return;
	}

	// change start/end
	long s = start[curr];
	long e = end[curr];
	if (begin)
		s = pos;
	else
		e = pos;

	// save in evaluation
	ra_class_set_event_pos(_clh_calib, index[curr], (long)((double)s / _x_scale), (long)((double)e / _x_scale));

	handle_region_change_event(s, e, _ch[_cursor_ch].ch_number);

	if (start)
		delete[] start;
	if (end)
		delete[] end;
	if (ch)
		delete[] ch;
	if (index)
		delete[] index;
} // change_calib_seq()


void
cont_ap_view_general::insert_calib_seq()
{
	if (!_clh_calib)
		return;
	if (_cursor_ch < 0)
		return;

	// TODO: add code to add rr-calibration event-class

	// if an area was marked and the marked area is completely in the current window
	// than use this area for the calibration sequence
	long s, e;
	long pos = _curr_pos + (long) ((double) (_cursor_pos - _left_space) / _max_xscale);
	if (_area_marked && ((_area_start > _curr_pos) && ((_area_end - _curr_pos) * _max_xscale < _screen_width)))
	{
		s = _area_start;
		e = _area_end;
	}
	else
	{
		// if no area was marked, use a default width, which needs to be
		// corrected by the user
		s = pos - (long) (100.0 / _max_xscale);
		e = pos + (long) (100.0 / _max_xscale);
	}
	long ev_id = ra_class_add_event(_clh_calib, (long)((double)s / _x_scale), (long)((double)e / _x_scale));
	ra_value_set_long(_vh, AP_CALIB_PHYSIOCAL);
	ra_prop_set_value(_calib_info, ev_id, _ch[_cursor_ch].ch_number, _vh);

	handle_region_change_event(s, e, _ch[_cursor_ch].ch_number);
} // insert_calib_seq()


void
cont_ap_view_general::remove_calib_seq()
{
	if (!_clh_calib)
		return;

	long pos = _curr_pos + (long) ((double) (_cursor_pos - _left_space) / _max_xscale);

	// find calibration sequence with cursor inside
	ra_class_get_events(_clh_calib, -1, -1, 0, 1, _vh);
	long n = ra_value_get_num_elem(_vh);
	const long *ev_id = ra_value_get_long_array(_vh);
	long curr = -1;
	long s, e;
	value_handle vh = ra_value_malloc();
	for (long l = 0; l < n; l++)
	{
		if (ra_class_get_event_pos(_clh_calib, ev_id[l], &s, &e) != 0)
			continue;

		if (ra_prop_get_ch(_calib_info, ev_id[l], vh) != 0)
			continue;

		bool found = false;
		for (unsigned long m = 0; m < ra_value_get_num_elem(vh); m++)
		{
			int ch_idx = get_ch_index(ra_value_get_short_array(vh)[m]);
			if (ch_idx < 0)
				break;
			if (_cursor_ch == ch_idx)
			{
				found = true;
				break;
			}
		}
		if (!found)
			continue;

		s = (long)((double)s * _x_scale);
		e = (long)((double)e * _x_scale);
		if ((s <= pos) && (pos <= e))
		{
			curr = ev_id[l];
			break;
		}
	}

	if (curr != -1)
	{
		ra_class_del_event(_clh_calib, curr);
		handle_region_change_event(s, e, _ch[_cursor_ch].ch_number);
	}
} // remove_calib_seq()


void
cont_ap_view_general::handle_region_change_event(long start, long end, long ch)
{
	struct proc_info *pi = (struct proc_info *)ra_proc_get(_mh, _plugin_ap, NULL);
	if (!pi)
		return;
	pi->rh = _rh;

	ra_value_set_short(_vh, 1);
	ra_lib_set_option(pi, "save_in_class", _vh);
	ra_value_set_short(_vh, 0);
	ra_lib_set_option(pi, "use_class", _vh);
	ra_lib_set_option(pi, "rr_pos", _vh);
	ra_value_set_long(_vh, 2);
	ra_lib_set_option(pi, "num_pos", _vh);
	ra_value_set_short(_vh, 1);
	ra_lib_set_option(pi, "pos_are_region", _vh);
	ra_lib_set_option(pi, "dont_search_calibration", _vh);
	ra_value_set_voidp(_vh, _clh);
	ra_lib_set_option(pi, "clh", _vh);
	ra_value_set_voidp(_vh, _clh_calib);
	ra_lib_set_option(pi, "clh_calib", _vh);
	long *pos = new long[2];
	pos[0] = (long)((double)start / _x_scale);
	pos[1] = (long)((double)end / _x_scale);
	ra_value_set_long_array(_vh, pos, 2);
	delete[] pos;
	ra_lib_set_option(pi, "pos", _vh);
	ra_value_set_long_array(_vh, &ch, 1);
	ra_lib_set_option(pi, "ch", _vh);
	
	ra_proc_do(pi);

	// send signals that something has changed
	_block_signals = true;
	ra_comm_emit(_mh, "cont-ap-view", "start-event-change", 0, NULL);
	_block_signals = false;
	send_event_change(_prop_syst, -1);
	send_event_change(_prop_dias, -1);
	send_event_change(_prop_mean, -1);
	send_event_change(_prop_syst_pos, -1);
	send_event_change(_prop_dias_pos, -1);
	send_event_change(_prop_dpdt_min, -1);
	send_event_change(_prop_dpdt_max, -1);
	send_event_change(_prop_dpdt_min_pos, -1);
	send_event_change(_prop_dpdt_max_pos, -1);
	send_event_change(_prop_flags, -1);
	send_event_change(_calib_info, -1);
	_block_signals = true;
	ra_comm_emit(_mh, "cont-ap-view", "end-event-change", 0, NULL);
	_block_signals = false;

	// clean up
	ra_proc_free(pi);

	update();
}  // handle_region_change_event()


void
cont_ap_view_general::toggle_value_ok()
{
	if ((_ev_idx == -1) || !_prop_flags)
		return;

	ra_prop_get_value(_prop_flags, _event_ids[_ev_idx], _ch[_cursor_ch].ch_number, _vh);
	long flags = ra_value_get_long(_vh);
	flags ^= AP_VALUE_NOK;
	ra_value_set_long(_vh, flags);
	ra_prop_set_value(_prop_flags, _event_ids[_ev_idx], _ch[_cursor_ch].ch_number, _vh);
 	send_event_change(_prop_flags, _event_ids[_ev_idx]);
}  // toggle_value_ok()


void
cont_ap_view_general::sort_general(const char *evtype, double value)
{
	_num_sort = 0;
	_curr_sort = 0;
	if ((_num_events <= 0) || (_clh == NULL))
		return;
	if (_sort)
	{
		delete[] _sort;
		_sort = NULL;
	}

	prop_handle ph = ra_prop_get(_clh, evtype);
	if (!ph)
		return;

	value_handle vh_id = ra_value_malloc();
	ra_class_get_events(_clh, -1, -1, 0, 1, vh_id);
	long n = ra_value_get_num_elem(vh_id);
	const long *ev_id = ra_value_get_long_array(vh_id);

	_sort = new long[n];
	for (long l = 0; l < n; l++)
	{
		ra_prop_get_value(ph, ev_id[l], _ch[_curr_channel].ch_number, _vh);
		double v = ra_value_get_double(_vh);
		if (v == value)
			_sort[_num_sort++] = ra_value_get_long(vh_id);
	}

	ra_value_free(vh_id);

	if (_num_sort > 0)
	{
		_curr_sort = -1;
		next_sort();
	}
} // sort_general()


struct sort_struct
{
	long idx;
	int val;
};
bool _sort_short_first;

int
sort_comp(const void *p1, const void *p2)
{
	struct sort_struct *s1 = (struct sort_struct *) p1;
	struct sort_struct *s2 = (struct sort_struct *) p2;

	if (_sort_short_first)
		return (s1->val - s2->val);
	else
		return -(s1->val - s2->val);
} // sort_comp()


void
cont_ap_view_general::sort_bp(int type)
{
	_num_sort = 0;
	_curr_sort = 0;
	_sort_is_pos = 0;
	if (_sort)
	{
		delete[] _sort;
		_sort = NULL;
	}

	if (_num_events <= 0)
		return;

	_sort = new long[_num_events];

	prop_handle prop = NULL;
	switch (type)
	{
	case 0:
		prop = _prop_dias;
		_sort_short_first = true;
		break;
	case 1:
		prop = _prop_dias;
		_sort_short_first = false;
		break;
	case 2:
		prop = _prop_syst;
		_sort_short_first = true;
		break;
	case 3:
		prop = _prop_syst;
		_sort_short_first = false;
		break;
	default:
		return;
	}

	struct sort_struct *t = new struct sort_struct[_num_events];
	long l;			// Visual-C++
	for (l = 0; l < _num_events; l++)
	{
		if (ra_prop_get_value(prop, _event_ids[l], _ch[_curr_channel].ch_number, _vh) != 0)
			continue;
		long val = ra_value_get_long(_vh);

		t[_num_sort].idx = l;// _event_ids[l];
		t[_num_sort].val = val;

		_num_sort++;
	}

	qsort(t, _num_sort, sizeof(struct sort_struct), sort_comp);

	if (!_sort)
		_sort = new long[_num_events];
	for (l = 0; l < _num_sort; l++)
		_sort[l] = t[l].idx;
	delete[]t;

	if (_num_sort > 0)
	{
		_curr_sort = -1;
		next_sort();
	}
} // sort_bp()


void
cont_ap_view_general::sort_calib_seq()
{
	_num_sort = 0;
	_curr_sort = 0;
	_sort_is_pos = 0;
	if (_sort)
	{
		delete[] _sort;
		_sort = NULL;
	}

	if (!_clh_calib)
		return;

	ra_class_get_events(_clh_calib, -1, -1, 0, 1, _vh);
	_num_sort = ra_value_get_num_elem(_vh);
	if (_num_sort <= 0)
		return;

	_sort = new long[_num_sort];
	const long *ev_id = ra_value_get_long_array(_vh);
	long offset = _page_width / 2;
	long s, e;
	for (long l = 0; l < _num_sort; l++)
	{
		if (ra_class_get_event_pos(_clh_calib, ev_id[l], &s, &e) != 0)
			continue;

		s = (long)((double)s * _x_scale);
		e = (long)((double)e * _x_scale);

		_sort[l] = ((s + e) / 2) - offset;
	}

	_sort_is_pos = 1;
	_curr_sort = -1;
	next_sort();
}  // sort_calib_seq()


void
cont_ap_view_general::sort_class(int classification)
{
	if (!_prop_class)
		return;

	_num_sort = 0;
	_curr_sort = 0;
	_sort_is_pos = 0;
	if (_sort)
	{
		delete[] _sort;
		_sort = NULL;
	}

	_sort = new long[_num_events];

	for (long l = 0; l < _num_events; l++)
	{
		if (ra_prop_get_value(_prop_class, _event_ids[l], _ch[_curr_channel].ch_number, _vh) != 0)
			continue;
		long cl = ra_value_get_long(_vh);
		if (GET_CLASS(cl) == classification)
			_sort[_num_sort++] = l;
	}

	if (_num_sort > 0)
	{
		_curr_sort = -1;
		next_sort();
	}
} // sort_class()


void
cont_ap_view_general::get_calib_seq(int *num_seq, long **start_pos, long **end_pos, long **ch, long **index)
{
	*num_seq = 0;
	*start_pos = *end_pos = *ch = NULL;

	if (!_clh_calib)
		return;

	ra_class_get_events(_clh_calib, -1, -1, 0, 1, _vh);
	long n = ra_value_get_num_elem(_vh);
	const long *ev_id = ra_value_get_long_array(_vh);

	value_handle vh = ra_value_malloc();
	*start_pos = new long[n];
	*end_pos = new long[n];
	*ch = new long[n];
	*index = new long[n];
	for (long l = 0; l < n; l++)
	{
		long s, e;
		if (ra_class_get_event_pos(_clh_calib, ev_id[l], &s, &e) != 0)
			continue;

		s = (long)((double)s * _x_scale);
		e = (long)((double)e * _x_scale);

		if ((_curr_pos + _page_width) < s)
			continue;
		if (_curr_pos > e)
			continue;

		if (ra_prop_get_ch(_calib_info, ev_id[l], vh) != 0)
			continue;

		const short *chs = ra_value_get_short_array(vh);
		for (unsigned long m = 0; m < ra_value_get_num_elem(vh); m++)
		{
			(*start_pos)[*num_seq] = s;
			(*end_pos)[*num_seq] = e;
			(*ch)[*num_seq] = chs[m];
			(*index)[*num_seq] = ev_id[l];
			(*num_seq)++;
		}
	}
	ra_value_free(vh);
} // get_calib_seq()


long
cont_ap_view_general::find_beat_eval(long last_index, long pos, int after_pos)
{
	long l;
	long idx = -1;

	for (l = last_index; l < _num_events; l++)
	{
		long syst_pos, beat_pos;

		ra_prop_get_value(_prop_syst_pos, _event_ids[l], _ch[_curr_channel].ch_number, _vh);		
		syst_pos = (long)(ra_value_get_double(_vh) * _x_scale);

		ra_prop_get_value(_prop_qrs_pos, _event_ids[l], _ch[_curr_channel].ch_number, _vh);		
		beat_pos = (long)(ra_value_get_double(_vh) * _x_scale);

		if (!after_pos && (syst_pos > pos))
		{
			idx--;
			break;
		}
		if (after_pos && (beat_pos > pos))
		{
			idx = l;
			break;
		}

		idx = l;
	}

	return idx;
} /* find_beat_eval() */


void
cont_ap_view_general::send_event_change(prop_handle ev_prop, long event_num)
{
	_block_signals = true;

	struct comm_para *para = new struct comm_para[2];
	para[0].type = PARA_VOID_POINTER;
	para[0].v.pv = ev_prop;
	para[1].type = PARA_LONG;
	para[1].v.l = event_num;
	ra_comm_emit(_mh, "cont-ap-view", "event-change", 2, para);
	delete[]para;

	_block_signals = false;
} // send_event_change()


void
cont_ap_view_general::add_event(class_handle clh, long event)
{
	if (clh != _clh)
		return;

	struct proc_info *pi = (struct proc_info *)ra_proc_get(_mh, _plugin_ap, NULL);
	if (!pi)
		return;
	pi->rh = _rh;

	ra_value_set_short(_vh, 1);
	ra_lib_set_option(pi, "save_in_class", _vh);
	ra_value_set_short(_vh, 0);
	ra_lib_set_option(pi, "use_class", _vh);
	ra_lib_set_option(pi, "rr_pos", _vh);
	ra_value_set_long(_vh, 1);
	ra_lib_set_option(pi, "num_pos", _vh);
	ra_value_set_short(_vh, 1);
	ra_lib_set_option(pi, "pos_are_event_id", _vh);
	ra_lib_set_option(pi, "dont_search_calibration", _vh);
	ra_value_set_voidp(_vh, _clh);
	ra_lib_set_option(pi, "clh", _vh);
	ra_value_set_voidp(_vh, _clh_calib);
	ra_lib_set_option(pi, "clh_calib", _vh);

 	long *pos = new long[1];
 	pos[0] = event;
 	ra_value_set_long_array(_vh, pos, 1);
	delete[] pos;
	ra_lib_set_option(pi, "pos", _vh);

	ra_proc_do(pi);

	// clean up
	ra_proc_free(pi);
} // add_event()


void
cont_ap_view_general::recalc_value(class_handle clh, long event)
{
	if (clh != _clh)
		return;

	if (event < 0)
		return;

	struct proc_info *pi = (struct proc_info *)ra_proc_get(_mh, _plugin_ap, NULL);
	if (!pi)
		return;
	pi->rh = _rh;

	ra_value_set_short(_vh, 1);
	ra_lib_set_option(pi, "save_in_class", _vh);
	ra_value_set_short(_vh, 0);
	ra_lib_set_option(pi, "use_class", _vh);
	ra_lib_set_option(pi, "rr_pos", _vh);
	ra_value_set_long(_vh, 1);
	ra_lib_set_option(pi, "num_pos", _vh);
	ra_value_set_short(_vh, 1);
	ra_lib_set_option(pi, "pos_are_event_id", _vh);
	ra_lib_set_option(pi, "dont_search_calibration", _vh);
	ra_lib_set_option(pi, "dont_search_positions", _vh);
	ra_value_set_voidp(_vh, _clh);
	ra_lib_set_option(pi, "clh", _vh);
	ra_value_set_voidp(_vh, _clh_calib);
	ra_lib_set_option(pi, "clh_calib", _vh);

 	long *pos = new long[1];
 	pos[0] = event;
 	ra_value_set_long_array(_vh, pos, 1);
	delete[] pos;
	ra_lib_set_option(pi, "pos", _vh);

	ra_proc_do(pi);

	// clean up
	ra_proc_free(pi);
}  // recalc_value()


void
cont_ap_view_general::save_settings(const char *file)
{
	ts_view_general::save_settings(file);
} // save_settings()


void
cont_ap_view_general::load_settings(const char *file)
{
	ts_view_general::load_settings(file);
} // load_settings()

