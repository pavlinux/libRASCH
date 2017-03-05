/*----------------------------------------------------------------------------
 * ecg_view_general.cpp
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

#ifdef _AFXDLL
#include "stdafx.h"
#endif // _AFXDLL

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <list>

#define _LIBRASCH_BUILD
#include <ra_ecg.h>
#include <ra_ap_morphology.h>
#include <ra_fiducial_point.h>
#include <pl_general.h>

#include "ecg_view_general.h"
#include "signal_processing.h"

using namespace std;


ecg_view_general::ecg_view_general(meas_handle h):ts_view_general(h)
{
	strncpy(_plugin_name, "ecg-view", RA_VALUE_NAME_MAX);

	_plugin_fiducial = NULL;
	_plugin_detect_ecg = NULL;

	_ev_idx_save = -2;

	_top_space = 30;

	_clh = NULL;
	_class_name[0] = '\0';

	_arr_clh = NULL;
	_arr_event_evprop = NULL;

	_prop_pos = NULL;
	_prop_ch = NULL;
	_prop_class = NULL;
	_prop_syst = NULL;
	_prop_dias = NULL;
	_prop_mean = NULL;
	_prop_syst_pos = NULL;
	_prop_dias_pos = NULL;
	_prop_rri = NULL;
	_prop_rri_class = NULL;
	_prop_qrs_temporal = NULL;
	_prop_rri_ref = NULL;
	_prop_rri_num_ref = NULL;
	_prop_ecg_flags = NULL;

	_prop_morph_flags = NULL;
	_prop_p_start = NULL;
	_prop_p_peak_1 = NULL;
	_prop_p_peak_2 = NULL;
	_prop_p_end = NULL;
	_prop_p_type = NULL;
	_prop_p_width = NULL;
	_prop_qrs_start = NULL;
	_prop_q = NULL;
	_prop_r = NULL;
	_prop_s = NULL;
	_prop_rs = NULL;
	_prop_qrs_end = NULL;
	_prop_qrs_type = NULL;
	_prop_qrs_width = NULL;
	_prop_t_start = NULL;
	_prop_t_peak_1 = NULL;
	_prop_t_peak_2 = NULL;
	_prop_t_end = NULL;
	_prop_t_type = NULL;
	_prop_t_width = NULL;

	_prop_pq = NULL;
	_prop_qt = NULL;
	_prop_qtc = NULL;

	_in_edit_mode = false;
	memset(&_edit_beat, 0, sizeof(struct edit_beat_data));
	_edit_beat.draw_area_top = 110;
	_edit_beat.draw_area_bottom = 70;

	_process_waiting = false;
	_show_morph_pos = true;
	_show_p_wave_pos = true;
	_show_qrs_pos = true;
	_show_t_wave_pos = true;

	_show_lowpass_edit_signal = false;
	_show_highpass_edit_signal = false;

	if (h != NULL)
		init();
} // constructor


ecg_view_general::~ecg_view_general()
{
	if (_process_waiting)
		post_process();

	ra_comm_del(_mh, (void *) this, NULL);
} // destructor


void
ecg_view_general::init()
{
	_plugin_fiducial = ra_plugin_get_by_name(ra_lib_handle_from_any_handle(_mh), "fiducial-point", 1);
	_plugin_detect_ecg = ra_plugin_get_by_name(ra_lib_handle_from_any_handle(_mh), "detect-ecg", 1);

	get_eventtypes();

	ra_value_set_number(_vh, _ch[0].ch_number);
	ra_info_get(_mh, RA_INFO_CH_XSCALE_D, _vh);
	_x_scale = ra_value_get_double(_vh);
} // init()


void
ecg_view_general::get_eventtypes()
{
	_num_events = 0;
	if (_event_ids)
	{
		delete[] _event_ids;
		_event_ids = NULL;
	}
	_ev_idx = -1;

	_cursor_pos = 0;
	_cursor_ch = 0;

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

	_prop_pos = ra_prop_get(_clh, "qrs-pos");
	if (_prop_pos == NULL)
	{
		// we need at least the 'qrs-pos' event property
		return;
	}

	_prop_ch = ra_prop_get(_clh, "qrs-ch");
	_prop_class = ra_prop_get(_clh, "qrs-annot");
	_prop_ecg_flags = ra_prop_get(_clh, "ecg-flags");

	// get number of events and store the event_id's
	ra_class_get_events(_clh, -1, -1, 0, 1, _vh);
	_num_events = ra_value_get_num_elem(_vh);
	_event_ids = new long[_num_events];
	memcpy(_event_ids, ra_value_get_long_array(_vh), sizeof(long) * _num_events);

	// if no 'qrs-annot' property available, add one and set all values to '0'
	if (_prop_class == NULL)
	{
		_prop_class = ra_prop_add_predef(_clh, "qrs-annot");
		if (_prop_class)
		{
			value_handle vh = ra_value_malloc();
			const long *ev_id = ra_value_get_long_array(_vh);
			for (long l = 0; l < _num_events; l++)
			{
				ra_value_set_long(vh, ECG_CLASS_SINUS);
				ra_prop_set_value(_prop_class, ev_id[l], -1, vh);
			}
			ra_value_free(vh);
		}
	}

	_prop_morph_flags = ra_prop_get(_clh, "ecg-morph-flags");
	_prop_p_start = ra_prop_get(_clh, "ecg-p-start");
	_prop_p_peak_1 = ra_prop_get(_clh, "ecg-p-peak-1");
	_prop_p_peak_2 = ra_prop_get(_clh, "ecg-p-peak-2");
	_prop_p_end = ra_prop_get(_clh, "ecg-p-end");
	_prop_p_type = ra_prop_get(_clh, "ecg-p-type");
	_prop_p_width = ra_prop_get(_clh, "ecg-p-width");
	_prop_qrs_start = ra_prop_get(_clh, "ecg-qrs-start");
	_prop_q = ra_prop_get(_clh, "ecg-q-peak");
	_prop_r = ra_prop_get(_clh, "ecg-r-peak");
	_prop_s = ra_prop_get(_clh, "ecg-s-peak");
	_prop_rs = ra_prop_get(_clh, "ecg-rs-peak");
	_prop_qrs_end = ra_prop_get(_clh, "ecg-qrs-end");
	_prop_qrs_type = ra_prop_get(_clh, "ecg-qrs-type");
	_prop_qrs_width = ra_prop_get(_clh, "ecg-qrs-width");
	_prop_t_start = ra_prop_get(_clh, "ecg-t-start");
	_prop_t_peak_1 = ra_prop_get(_clh, "ecg-t-peak-1");
	_prop_t_peak_2 = ra_prop_get(_clh, "ecg-t-peak-2");
	_prop_t_end = ra_prop_get(_clh, "ecg-t-end");
	_prop_t_type = ra_prop_get(_clh, "ecg-t-type");
	_prop_t_width = ra_prop_get(_clh, "ecg-t-width");

	_prop_pq = ra_prop_get(_clh, "ecg-pq");
	_prop_qt = ra_prop_get(_clh, "ecg-qt");
	_prop_qtc = ra_prop_get(_clh, "ecg-qtc");

	_prop_rri = ra_prop_get(_clh, "rri");
	_prop_rri_class = ra_prop_get(_clh, "rri-annot");

	_prop_qrs_temporal = ra_prop_get(_clh, "qrs-temporal");
	_prop_rri_ref = ra_prop_get(_clh, "rri-refvalue");
	_prop_rri_num_ref = ra_prop_get(_clh, "rri-num-refvalue");

	if (ra_info_get(_clh, RA_INFO_CLASS_NAME_C, _vh) == 0)
		strncpy(_class_name, ra_value_get_string(_vh), EVAL_MAX_NAME);

	get_template_infos(_clh);

	// get arrhythmic event 
	ra_class_get(_eh, "arrhythmia-original", _vh);
	if (ra_value_get_num_elem(_vh) == 0)
		return;

	if (ra_value_get_num_elem(_vh) > 1)
	{
		fprintf(stderr, "More than one 'arrhythmia-original' event-class available.\n" \
			"At the moment only ONE event-class is supported\n");
	}
	_arr_clh = (class_handle)(ra_value_get_voidp_array(_vh)[0]);

	_arr_event_evprop = ra_prop_get(_arr_clh, "arr_o-type");
	if (!_arr_event_evprop)
		return;

	return;
} // get_eventtypes()


void
ecg_view_general::get_beat_pos(int **x, long **pos, int *num)
{
	*x = NULL;
	*pos = NULL;
	*num = 0;

	if (_num_events <= 0)
		return;

	ra_class_get_events(_clh, _curr_pos, (_curr_pos + _page_width), 0, 1, _vh);
	*num = ra_value_get_num_elem(_vh);

	if (*num <= 0)
		return;

	const long *ev_id = ra_value_get_long_array(_vh);
	
	*x = new int[*num];
	*pos = new long[*num];
	value_handle vh_val = ra_value_malloc();
	for (int i = 0; i < *num; i++)
	{
		(*pos)[i] = ev_id[i];

		ra_prop_get_value(_prop_pos, ev_id[i], -1, vh_val);
		long p = ra_value_get_long(vh_val);
		(*x)[i] = (int) ((double)(p - _curr_pos) * _max_xscale) + _left_space;;
	}
	ra_value_free(vh_val);
} // get_beat_pos()


// get arrhythmic events for current shown data
void
ecg_view_general::get_arr_events(char ***arr_events, long **start, long **end, int *num)
{
	*arr_events = NULL;
	*start = NULL;
	*end = NULL;
	*num = 0;

	if (!_arr_event_evprop)
		return;

	ra_class_get_events(_arr_clh, _curr_pos, (_curr_pos + _page_width), 0, 1, _vh);
	*num = ra_value_get_num_elem(_vh);

	if (*num <= 0)
		return;
	
	const long *ev_id = ra_value_get_long_array(_vh);

	*arr_events = new char *[*num];
	*start = new long[*num];
	*end = new long[*num];

	for (int i = 0; i < *num; i++)
	{		
		ra_class_get_event_pos(_clh, ev_id[i], (*start)+i, (*end)+i);

		(*start)[i] = (long)((double)((*start)[i] - _curr_pos) * _max_xscale) + _left_space;
		(*end)[i] = (long)((double)((*end)[i] - _curr_pos) * _max_xscale) + _left_space;
		
		ra_prop_get_value(_arr_event_evprop, ev_id[i], -1, _vh);
		const char *desc = raecg_get_arr_desc(ra_value_get_long(_vh));
		if (desc != NULL)
		{
			int len = strlen(desc);
			(*arr_events)[i] = (char *)malloc(sizeof(char) * (len + 1));
			strncpy((*arr_events)[i], desc, len);
			(*arr_events)[i][len] = '\0';
		}
		else
		{
			char t[1000];
			sprintf(t, gettext("unknown arr-type (%ld)"), ra_value_get_long(_vh));
			int len = strlen(t);
			(*arr_events)[i] = (char *)malloc(sizeof(char) * (len + 1));
			strncpy((*arr_events)[i], t, len);
			(*arr_events)[i][len] = '\0';
		}
	}
} // get_arr_events()


int
ecg_view_general::mouse_press(int x, int y, bool shift, bool dbl_click)
{
	int ch = y_to_channel(y);

	if (shift && (_ev_idx != -1))
	{
		_cursor_pos = x;
		return ch;
	}

	long idx = get_sel_pos(x - _left_space);
	if (idx != -1)
	{
		// only set event when clicked inside the plot area
		if (x >= _left_space)
		{
			set_event(NULL, _event_ids[idx]);
			if (_in_edit_mode && dbl_click)
				fill_edit_beat_data();
		}
	}
	else
	{
		_ev_idx = -1;
		_cursor_pos = x;
	}

	if (_in_edit_mode)
	{
		int pos = get_inflection_point(x - _left_space, ch);
		if (pos >= 0)
			_cursor_pos = pos + _left_space;
	}

	return ch;
} // mouse_press()


long
ecg_view_general::get_sel_pos(int x)
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
	
	ra_prop_get_events(_prop_pos, vh_min, vh_max, -1, _vh);

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


int
ecg_view_general::get_inflection_point(int x, int ch)
{
	if (x < _left_space)
		return -1;

	long pos = _curr_pos + (long) ((double) x / _max_xscale);

	long region = (long)(15.0 / _max_xscale);// +-15 pixel
	long min = pos - region;
	long max = pos + region;

	size_t num = max - min + 1;
	double *data = new double[num];

	num = ra_raw_get_unit(_rh, ch, min, num, data);
	
	long l;
	double max_diff = 0.0;
	long max_pos = 0;
	for (l = 0; l < (long)num; l++)
	{
		double diff = fabs(data[l] - data[region]);
		if (diff > max_diff)
		{
			max_diff = diff;
			max_pos = l;
		}
	}

	// decide if we are looking for the beginning or the end of a wave
	bool get_end;
	if (max_pos < region)
		get_end = true;
	else
		get_end = false;

	// decide if the wave is negative or positive
	long th_pos = region;
	if (get_end)
	{
		for (l = region; l >= 0; l--)
		{
			double diff = fabs(data[l] - data[region]);
			if (diff > (0.25 * max_diff))
			{
				th_pos = l;
				break;
			}
		}
	}
	else
	{
		for (l = region; l < (long)num; l++)
		{
			double diff = fabs(data[l] - data[region]);
			if (diff > (0.25 * max_diff))
			{
				th_pos = l;
				break;
			}
		}
	}

	bool positive_peak;
	if (data[th_pos] > data[region])
		positive_peak = true;
	else
		positive_peak = false;

	// get peak
	double peak = data[0];
	long peak_pos = 0;
	for (l = 0; l < (long)num; l++)
	{
		if (positive_peak & (data[l] > peak))
		{
			peak = data[l];
			peak_pos = l;
		}
		if (!positive_peak & (data[l] < peak))
		{
			peak = data[l];
			peak_pos = l;
		}
	}

	/* calc distance between signal and straight line from peak to start- or end-pos */
	double step;
	if (get_end)
		step = (data[peak_pos] - data[num-1]) / (double)(num - peak_pos);
	else
		step = (data[peak_pos] - data[num-1]) / (double)(peak_pos);

	double max_dist = 0.0;
	long inflection_pos = peak_pos;
	double curr_line = data[peak_pos];

	if (get_end)
	{
		for (l = peak_pos; l < (long)num; l++)
		{
			double diff = curr_line - data[l];
			
			if (positive_peak && (diff > max_dist))
			{
				max_dist = diff;
				inflection_pos = l;
			}
			if (!positive_peak && (diff < max_dist))
			{
				max_dist = diff;
				inflection_pos = l;
			}

			curr_line -= step;
		}
	}
	else
	{
		for (l = peak_pos; l >= 0; l--)
		{
			double diff = curr_line - data[l];
			
			if (positive_peak && (diff > max_dist))
			{
				max_dist = diff;
				inflection_pos = l;
			}
			if (!positive_peak && (diff < max_dist))
			{
				max_dist = diff;
				inflection_pos = l;
			}

			curr_line -= step;
		}
	}

	inflection_pos = (long)((double)(inflection_pos - region) * _max_xscale) + x;

	return inflection_pos;
} // get_inflection_point()


void
ecg_view_general::options_changed()
{
	ts_view_general::options_changed();

	if (_in_edit_mode)
		_left_space += EDIT_SPACE;
} // options_changed()


bool
ecg_view_general::key_press_char(char c, bool shift, bool ctrl)
{
	set_busy_cursor();

	long pos = _curr_pos + (long) ((double) (_cursor_pos - _left_space) / _max_xscale);

	bool cursor_visible = true;
	if ((_cursor_pos < _left_space) || (_cursor_pos > (_screen_width - _right_space)))
		cursor_visible = false;

	bool edit = false;
	bool redraw = true;
	bool handled = true;
	int cl;
	switch (c)
	{
	case 'X':		// artifact
		if (cursor_visible)
		{
			SET_CLASS(ECG_CLASS_ARTIFACT, cl);
			edit = do_beat_edit(_ev_idx, pos, cl, shift);
		}
		break;
	case 'N':		// beat
		if (cursor_visible)
		{
			SET_CLASS(ECG_CLASS_SINUS, cl);
			edit = do_beat_edit(_ev_idx, pos, cl, shift);
		}
		break;
	case 'S':		// atrial premature beat
		if (cursor_visible)
		{
			SET_CLASS(ECG_CLASS_SINUS, cl);
			SET_SUBCLASS(ECG_CLASS_SINUS_SVPC, cl);
			edit = do_beat_edit(_ev_idx, pos, cl, shift);
		}
		break;
	case 'V':		// VPC
		if (cursor_visible)
		{
			SET_CLASS(ECG_CLASS_VENT, cl);
			edit = do_beat_edit(_ev_idx, pos, cl, shift);
		}
		break;
	case 'P':
		if (cursor_visible)
		{
			SET_CLASS(ECG_CLASS_PACED, cl);
			edit = do_beat_edit(_ev_idx, pos, cl, shift);
		}
		break;
	case 'D':		//delete
		if (cursor_visible && (_ev_idx >= 0))
		{
			delete_beat();
			edit = true;
		}
		break;
	case 'H':		// delete all beats before current pos
//		delete_from_pos(0);
		edit = true;
		break;
	case 'J':		// delete all beats after current pos
//		delete_from_pos(1);
		edit = true;
		break;
	case 'L':
		sort_rri(false, shift);
		break;
	case 'K':
		sort_rri(true, shift);
		break;
	case 'C':
		sort_template_corr(shift);
		break;
	case '1':
		if (cursor_visible)
		{
			set_wave_delimiter(pos, _prop_p_start);
			redraw = true;
		}
		break;
	case '2':
		if (cursor_visible)
		{
			set_wave_delimiter(pos, _prop_p_end);
			redraw = true;
		}
		break;
	case '3':
		if (cursor_visible)
		{
			set_wave_delimiter(pos, _prop_qrs_start);
			redraw = true;
		}
		break;
	case '4':
		if (cursor_visible)
		{
			set_wave_delimiter(pos, _prop_qrs_end);
			redraw = true;
		}
		break;
	case '5':
		if (cursor_visible)
		{
			set_wave_delimiter(pos, _prop_t_start);
			redraw = true;
		}
		break;
	case '6':
		if (cursor_visible)
		{
			set_wave_delimiter(pos, _prop_t_end);
			redraw = true;
		}
		break;

// 	case '1':  // find pos where detected only in one channel
// 	case '2':  // in two channels
// 	case '3':  // in three channels
// 	case '4':  // ...
// 	case '5':
// 	case '6':
// 	case '7':
// 	case '8':
// 	case '9':
// 		sort_ch_found((c - '1') + 1);
// 		break;
	case ' ':		// show next sort position
		next_sort();
		break;
	case 'B':		// show prev. sort pos.
		prev_sort();
		break;
	case 'E':
		toggle_edit_mode();
		break;
	case 'M':
		_edit_beat.max *= 0.5;
		_edit_beat.yscale = ((double)_edit_beat.draw_area/2.0) / _edit_beat.max;
		redraw = true;
		break;
	default:
		redraw = false;
		handled = false;
		break;
	}

	if (edit)
	{
		_process_waiting = true;

		correct_neighbors();

		ra_info_get(_clh, RA_INFO_CLASS_EV_NUM_L, _vh);
		_num_events = ra_value_get_long(_vh); // TODO: why is this needed here????

		_block_signals = true;
		ra_comm_emit(_mh, "ecg-view", "eval-change", 0, NULL);
		_block_signals = false;
	}

	if (!handled)
		redraw = ts_view_general::key_press_char(c, shift, ctrl);

	unset_cursor();

	return redraw;
} // keyPressEvent()


bool
ecg_view_general::do_beat_edit(long ev_idx, long pos, long classification, bool shift)
{
	bool edit = true;

	if (ev_idx == -1)
	{
		insert_beat(pos, classification, shift);
	}
	else
	{
		if (shift)
			change_templ_class(_event_ids[ev_idx], classification);
		else
			change_class(_event_ids[ev_idx], classification);
	}

	return edit;
} // do_beat_edit()


void
ecg_view_general::insert_beat(long pos, long classification, bool pos_is_fixed/*=false*/)
{
 	// insert beat in evaluation
 	eval_handle eh = ra_eval_get_default(_mh);
 	if (!eh || !_plugin_detect_ecg)
 		return;

	struct proc_info *pi = (struct proc_info *)ra_proc_get(_mh, _plugin_detect_ecg, NULL);

	ra_value_set_short(_vh, 1);
	ra_lib_set_option(pi, "save_in_eval", _vh);
	ra_value_set_voidp(_vh, eh);
	ra_lib_set_option(pi, "eh", _vh);
	ra_value_set_voidp(_vh, _clh);
	ra_lib_set_option(pi, "clh", _vh);
	ra_value_set_short(_vh, 1);
	ra_lib_set_option(pi, "do_interpolation", _vh);
	ra_value_set_short(_vh, (_filter_powerline_noise ? 1 : 0));
	ra_lib_set_option(pi, "filter_powerline_noise", _vh);
	ra_value_set_short(_vh, (_remove_mean ? 1 : 0));
	ra_lib_set_option(pi, "filter_baseline_wander", _vh);

	ra_value_set_short(_vh, 1);
	ra_lib_set_option(pi, "use_region", _vh);
	ra_value_set_long(_vh, pos);
	ra_lib_set_option(pi, "region_start", _vh);
	ra_value_set_long(_vh, pos+0);
	ra_lib_set_option(pi, "region_end", _vh);

	if (ra_proc_do(pi) != 0)
	{
		ra_proc_free(pi);
		return;
	}

	// get found beats
	ra_proc_get_result(pi, 0, 0, _vh);
	long num_beats = ra_value_get_long(_vh);

	ra_proc_get_result(pi, 1, 0, _vh);
	long *beat_pos = new long[num_beats];
	memcpy(beat_pos, ra_value_get_long_array(_vh), sizeof(long) * num_beats);

	// clean up
	ra_proc_free(pi);

	long ev_id = -1;
	for (long l = 0; l < num_beats; l++)
	{
		if (ra_class_get_events(_clh, beat_pos[l], beat_pos[l], 0, 1, _vh) != 0)
			continue;
		ev_id = ra_value_get_long_array(_vh)[0];

		get_fiducial_point(ev_id);
		get_template(ev_id);

		change_class(ev_id, classification);
 		send_add_event(ev_id);
		correct_sort(ev_id, 1);
	}
	delete[] beat_pos;

 	if (ev_id != -1)
 	{
 		get_eventtypes();
 		set_event(NULL, ev_id);
 	}

	if (pos_is_fixed && (num_beats == 1))
	{
		ra_value_set_long(_vh, pos);
		ra_prop_set_value(_prop_pos, ev_id, -1, _vh);
	}
	
	post_process();
	send_eval_change();
	get_eventtypes();
} // insert_beat()


void
ecg_view_general::get_fiducial_point(long ev_id)
{
	plugin_handle pl = ra_plugin_get_by_name(ra_lib_handle_from_any_handle(_mh), "fiducial-point", 1);
	if (!pl)
		return;

	proc_handle proc = ra_proc_get(_mh, pl, NULL);
		
	ra_value_set_short(_vh, 0);
	ra_lib_set_option(proc, "use_class", _vh);
	ra_value_set_voidp(_vh, _clh);
	ra_lib_set_option(proc, "clh", _vh);
	ra_value_set_string(_vh, "qrs-pos");
	ra_lib_set_option(proc, "pos_prop", _vh);
	ra_value_set_short(_vh, 1);
	ra_lib_set_option(proc, "save_in_class", _vh);
	ra_value_set_long(_vh, _ch[_curr_channel].ch_number);
	ra_lib_set_option(proc, "ch", _vh);
	ra_value_set_double(_vh, 0.03);
	ra_lib_set_option(proc, "corr_len", _vh);
	ra_value_set_long_array(_vh, &ev_id, 1);
	ra_lib_set_option(proc, "data", _vh);

	ra_proc_do(proc);
	ra_proc_free(proc);
} // get_fiducial_point()


void
ecg_view_general::get_template(long ev_id)
{
	plugin_handle pl = ra_plugin_get_by_name(ra_lib_handle_from_any_handle(_mh), "template", 1);
	if (!pl)
		return;

	
	ra_sum_get(_clh, "template", _vh);
	if (ra_value_get_num_elem(_vh) <= 0)
		return;
	sum_handle sh = (sum_handle)(ra_value_get_voidp_array(_vh)[0]);

	proc_handle proc = ra_proc_get(_mh, pl, NULL);

	ra_value_set_short(_vh, 0);
	ra_lib_set_option(proc, "use_class", _vh);
	ra_value_set_voidp(_vh, _clh);
	ra_lib_set_option(proc, "clh", _vh);
	ra_value_set_voidp(_vh, sh);
	ra_lib_set_option(proc, "sh", _vh);
	ra_value_set_short(_vh, 1);
	ra_lib_set_option(proc, "save_in_class", _vh);
	ra_value_set_string(_vh, "qrs-pos");
	ra_lib_set_option(proc, "pos_prop", _vh);
	ra_value_set_string(_vh, "qrs-template");
	ra_lib_set_option(proc, "templ_name", _vh);
	ra_value_set_string(_vh, "qrs-template-corr");
	ra_lib_set_option(proc, "templ_corr", _vh);
	ra_value_set_double(_vh, 0.10);
	ra_lib_set_option(proc, "corr_win_before", _vh);
	ra_value_set_double(_vh, 0.10);
	ra_lib_set_option(proc, "corr_win_after", _vh);
	ra_value_set_double(_vh, 0.85);
	ra_lib_set_option(proc, "accept", _vh);
	ra_value_set_double(_vh, 0.7);
	ra_lib_set_option(proc, "slope_accept_low", _vh);
	ra_value_set_double(_vh, 1.3);
	ra_lib_set_option(proc, "slope_accept_high", _vh);
	ra_value_set_double(_vh, 0.20);
	ra_lib_set_option(proc, "template_win_before", _vh);
	ra_lib_set_option(proc, "template_win_after", _vh);
	ra_value_set_short(_vh, 1);
	ra_lib_set_option(proc, "align_events", _vh);
	ra_value_set_long_array(_vh, &ev_id, 1);
	ra_lib_set_option(proc, "data", _vh);
	
	ra_proc_do(proc);
	ra_proc_free(proc);
} // get_template()


void
ecg_view_general::change_class(long ev_id, long classification)
{
	if (ev_id < 0)
		return;

	if (!_prop_class)
		return;
	
	ra_value_set_long(_vh, classification);
	ra_prop_set_value(_prop_class, ev_id, -1, _vh);

	if (_prop_ch)
	{
		ra_value_set_long(_vh, 0);
		ra_prop_set_value(_prop_ch, ev_id, -1, _vh);
	}
	
	if (_prop_ecg_flags)
	{
		ra_prop_get_value(_prop_ecg_flags, ev_id, -1, _vh);
		long flag = ra_value_get_long(_vh);
		flag |= ECG_FLAG_USER_EDIT;
		ra_value_set_long(_vh, flag);
		ra_prop_set_value(_prop_ecg_flags, ev_id, -1, _vh);
	}
} // change_class()


void
ecg_view_general::change_templ_class(long ev_id, long classification)
{
	if (ev_id < 0)
		return;

	if (!_prop_class)
		return;

	long num_ev = 0;
	long *ev_ids = NULL;
		
	int ret = get_template_events(ev_id, &num_ev, &ev_ids);
	if ((ret != 0) || (num_ev <= 0))
		return;

	for (long l = 0; l < num_ev; l++)
	{
		change_class(ev_ids[l], classification);
		correct_neighbors(ev_ids[l]);
	}

	delete[] ev_ids;
} // change_templ_class()


void
ecg_view_general::delete_beat()
{
	// set position and actual event to next event
	long idx_save = _ev_idx;

	do_delete_beat(_event_ids[_ev_idx]);
	correct_sort(_ev_idx, -1);

	if (idx_save > (_num_events - 1))
		idx_save = _num_events - 1;

	set_event(NULL, _event_ids[idx_save]);
} // delete_beat()


void
ecg_view_general::do_delete_beat(long ev_id)
{
	struct comm_para *para = new struct comm_para[4];
	para[0].type = PARA_VOID_POINTER;
	para[0].v.pv = _clh;
	para[1].type = PARA_LONG;
	para[1].v.l = ev_id;
	para[2].type = PARA_LONG;
	para[2].v.l = ra_class_get_prev_event(_clh, ev_id);
	para[3].type = PARA_LONG;
	para[3].v.l = ra_class_get_next_event(_clh, ev_id);

	_block_signals = true;
	ra_comm_emit(_mh, "ecg-view", "del-event-begin", 4, para);
	_block_signals = false;

	ra_class_del_event(_clh, ev_id);

	_block_signals = true;
	ra_comm_emit(_mh, "ecg-view", "del-event-end", 4, para);
	ra_comm_emit(_mh, "ecg-view", "eval-change", 0, NULL);
	_block_signals = false;

	delete[]para;

	get_eventtypes();
} // delete_beat()


void
ecg_view_general::correct_neighbors(long ev_id/*=-1*/)
{
	if ((_ev_idx < 0) && (ev_id < 0))
		return;

	plugin_handle p = ra_plugin_get_by_name(ra_lib_handle_from_any_handle(_mh), "ecg", 1);
	if (!p)
		return;

	set_busy_cursor();

	// check if the current event-id should be used or the given one
	// (the given one has higher priority)
	long ev_id_use = _event_ids[_ev_idx];
	if (ev_id >= 0)
		ev_id_use = ev_id;

	// get event-id's which should be processed
	long num = 1;
	long ids[3];
	ids[0] = ra_class_get_prev_event(_clh, ev_id_use);
	if (ids[0] >= 0)
		num++;
	ids[num-1] = ev_id_use;
	ids[num] = ra_class_get_next_event(_clh, ev_id_use);
	if (ids[num] >= 0)
		num++;

	// process the events
	struct proc_info *pi = (struct proc_info *)ra_proc_get(_mh, p, NULL);

	ra_value_set_short(_vh, 0);
	int ret = ra_lib_set_option(pi, "use_class", _vh);
	ra_value_set_voidp(_vh, _clh);
	ret = ra_lib_set_option(pi, "clh", _vh);
	ra_value_set_short(_vh, 1);
	ret = ra_lib_set_option(pi, "save_in_class", _vh);
	ra_value_set_long(_vh, num);
	ret = ra_lib_set_option(pi, "num_data", _vh);
	ra_value_set_long_array(_vh, ids, num);
	ret = ra_lib_set_option(pi, "data", _vh);
	ra_value_set_short(_vh, 0);
	ret = ra_lib_set_option(pi, "data_is_pos", _vh);

	ra_proc_do(pi);
	ra_proc_free(pi);

	unset_cursor();
} // correct_neighbors()


void
ecg_view_general::post_process()
{
	_process_waiting = false;

	plugin_handle p = ra_plugin_get_by_name(ra_lib_handle_from_any_handle(_mh), "ecg", 1);
	if (!p)
		return;

	set_busy_cursor();

	struct proc_info *pi = (struct proc_info *)ra_proc_get(_mh, p, NULL);

	ra_value_set_short(_vh, 1);
	ra_lib_set_option(pi, "use_class", _vh);
	ra_value_set_voidp(_vh, _clh);
	ra_lib_set_option(pi, "clh", _vh);
	ra_value_set_short(_vh, 1);
	ra_lib_set_option(pi, "save_in_class", _vh);

	ra_proc_do(pi);
	ra_proc_free(pi);

	unset_cursor();
} // post_process()


void
ecg_view_general::sort_general(prop_handle ph, double value)
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
			_sort[_num_sort++] = ev_id[l];
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
ecg_view_general::select_events(class_handle clh, long num, long *ev_ids)
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

	if (!clh || (clh != _clh) || (num < 0) || (ev_ids == NULL))
		return;

	_sort = new long[num];
	_num_sort = num;
	for (long l = 0; l < num; l++)
		_sort[l] = ev_ids[l];

	_curr_sort = -1;
	next_sort();
} // select_events()


int
ecg_view_general::set_wave_delimiter(long pos, prop_handle prop)
{
	int ret = 0;

	long start, end;
	if ((ret = ra_class_get_event_pos(_clh, _event_ids[_ev_idx], &start, &end)) != 0)
		return ret;

	short offset = (short)(pos - start);
	ra_value_set_short(_vh, offset);
	ret = ra_prop_set_value(prop, _event_ids[_ev_idx], _ch[_curr_channel].ch_number, _vh);

	correct_neighbors();
	send_eval_change();
	get_eventtypes();

	return ret;
} // set_wave_delimiter()


void
ecg_view_general::sort_ch_found(int num)
{
	if (_prop_ch == NULL)
		return;

	_num_sort = 0;
	_curr_sort = 0;
	if (_sort)
		delete[] _sort;
	_sort = new long[_num_events];

	int n_use = num;
	if (n_use > _num_ch)
		n_use = _num_ch;

	for (long l = 0; l < _num_events; l++)
	{
		ra_prop_get_value(_prop_ch, _event_ids[l], -1, _vh);
		long ch_detect = ra_value_get_long(_vh);
		if (num_bits(ch_detect) == n_use)
			_sort[_num_sort++] = _event_ids[l];
	}

	if (_num_sort > 0)
	{
		_curr_sort = -1;
		next_sort();
	}
} // sort_ch_found()


int
ecg_view_general::num_bits(long v)
{
	int n = 0;
	for (int i = 0; i < 32; i++)
	{
		if (v & (1 << i))
			n++;
	}

	return n;
} // num_bits()


struct sort_struct_ecg
{
	long idx;
	long id;
	double rri;
	double corr;
};
bool _sort_short_first;

int
sort_rri_comp(const void *p1, const void *p2)
{
	struct sort_struct_ecg *s1 = (struct sort_struct_ecg *) p1;
	struct sort_struct_ecg *s2 = (struct sort_struct_ecg *) p2;

	if (_sort_short_first)
	{
		if (s1->rri < s2->rri)
			return -1;
		else if (s1->rri > s2->rri)
			return 1;
		else
			return 0;
	}
	else
	{
		if (s1->rri < s2->rri)
			return 1;
		else if (s1->rri > s2->rri)
			return -1;
		else
			return 0;
	}
} // sort_rri_comp()


int
sort_corr_comp(const void *p1, const void *p2)
{
	int ret = 1;

	struct sort_struct_ecg *s1 = (struct sort_struct_ecg *) p1;
	struct sort_struct_ecg *s2 = (struct sort_struct_ecg *) p2;
	
	if (s1->corr == s2->corr)
		return 0;

	if (_sort_short_first)
	{
		if (s1->corr < s2->corr)
			ret = -1;
	}
	else
	{
		if (s1->corr > s2->corr)
			ret = -1;
	}

	return ret;
} // sort_corr_comp()

void
ecg_view_general::sort_rri(bool short_to_long, bool ignore_artifacts)
{	
	if ((_num_events <= 0) || !_prop_rri || !_event_ids)
		return;

	value_handle vh = ra_value_malloc();
	value_handle vh_id = ra_value_malloc();
	value_handle vh_ch = ra_value_malloc();
	if (ra_prop_get_value_all(_prop_rri, vh_id, vh_ch, vh) != 0)
	{
		ra_value_free(vh);
		return;
	}

	value_handle vh_annot = ra_value_malloc();
	if (ra_prop_get_value_all(_prop_rri_class, NULL, NULL, vh_annot) != 0)
	{
		ra_value_free(vh);
		ra_value_free(vh_annot);
		return;
	}

	long num = ra_value_get_num_elem(vh);
	const long *ev_id = ra_value_get_long_array(vh_id);
	const double *rri = ra_value_get_double_array(vh);
	const long *ch = ra_value_get_long_array(vh_ch);
	const long *rri_annot = ra_value_get_long_array(vh_annot);

	struct sort_struct_ecg *t = new struct sort_struct_ecg[_num_events];
	_num_sort = 0;
	long l;		// Visual-C++
	for (l = 0; l < num; l++)
	{
		if (ch[l] != -1)
			continue;

		if ((rri[l] <= 0) || (ignore_artifacts && IS_ARTIFACT(rri_annot[l])))
			continue;

		t[_num_sort].id = ev_id[l];
		t[_num_sort].rri = rri[l];

		_num_sort++;
	}

	_sort_short_first = short_to_long;
	qsort(t, _num_sort, sizeof(struct sort_struct_ecg), sort_rri_comp);

	if (_sort)
		delete[] _sort;
	_sort = new long[_num_sort];

	for (l = 0; l < _num_sort; l++)
		_sort[l] = t[l].id;
	delete[]t;

	if (_num_sort > 0)
	{
		_curr_sort = -1;
		next_sort();
	}

	ra_value_free(vh);
	ra_value_free(vh_id);
	ra_value_free(vh_annot);
} // sort_rri()


void
ecg_view_general::sort_class(int classification)
{
	_num_sort = 0;
	_curr_sort = 0;
	if (_sort)
		delete[] _sort;
	_sort = new long[_num_events];

	value_handle vh_id = ra_value_malloc();
	value_handle vh_annot = ra_value_malloc();
	if (ra_prop_get_value_all(_prop_class, vh_id, NULL, vh_annot) != 0)
	{
		ra_value_free(vh_annot);
		return;
	}

	long num = ra_value_get_num_elem(vh_annot);
	const long *ev_id = ra_value_get_long_array(vh_id);
	const long *annot = ra_value_get_long_array(vh_annot);

	for (long l = 0; l < num; l++)
	{
		if ((GET_CLASS(annot[l]) == (classification & 0x0000ffff))
			&& (((classification & 0xffff0000) == 0)
			    || ((int)GET_SUBCLASS(annot[l]) == (classification >> 16))))
		{
			_sort[_num_sort++] = ev_id[l];
		}
	}

	if (_num_sort > 0)
	{
		_curr_sort = -1;
		next_sort();
	}

	ra_value_free(vh_id);
	ra_value_free(vh_annot);
} // sort_class()


void
ecg_view_general::sort_template(int /*templ_num*/)
{
	if (_num_templates <= 0)
		return;

// 	_num_sort = 0;
// 	_curr_sort = 0;
// 	if (_sort)
// 		delete[] _sort;
// 	_sort = new long[_num_events];

// 	for (long l = 0; l < _num_events; l++)
// 	{
// 		if ((long) ra_ev_get_value(_prop_template, l) == templ_num)
// 			_sort[_num_sort++] = l;
// 	}

// 	if (_num_sort > 0)
// 	{
// 		_curr_sort = -1;
// 		next_sort();
// 	}
} // sort_template()


void
ecg_view_general::sort_template_corr(bool /*highest_first*/)
{
	if ((_num_events <= 0) || _num_templates <= 0)
		return;

// 	long templ_num = (long)ra_ev_get_value(_prop_template, _pos_sel);
// 	if ((templ_num < 0) || (templ_num >= _num_templates))
// 		return;

// 	struct sort_struct *t = new struct sort_struct[_num_events];
// 	_num_sort = 0;
// 	long l;			// Visual-C++
// 	for (l = 0; l < _num_events; l++)
// 	{
// 		if ((long)ra_ev_get_value(_prop_template, l) != templ_num)
// 			continue;

// 		t[_num_sort].idx = l;
// 		t[_num_sort].corr = ra_ev_get_value(_prop_template_corr, l);
// 		_num_sort++;
// 	}

// 	_sort_short_first = !highest_first;
// 	qsort(t, _num_sort, sizeof(struct sort_struct), sort_corr_comp);

// 	if (_sort)
// 		delete[] _sort;
// 	_sort = new long[_num_events];

// 	for (l = 0; l < _num_sort; l++)
// 		_sort[l] = t[l].idx;
// 	delete[]t;

// 	if (_num_sort > 0)
// 	{
// 		_curr_sort = -1;
// 		next_sort();
// 	}
} // sort_template_corr()


void
ecg_view_general::remove_ch_morph_flags(long clean_mask, long curr_ch)
{
	if (!_prop_morph_flags)
		return;

	value_handle ev_id = ra_value_malloc();
	value_handle ch = ra_value_malloc();
	value_handle flags = ra_value_malloc();

	int ret;
	if ((ret = ra_prop_get_value_all(_prop_morph_flags, ev_id, ch, flags)) != 0)
	{
		ra_value_free(flags);
		ra_value_free(ch);
		ra_value_free(ev_id);
		
		return;
	}

	long num = ra_value_get_num_elem(flags);
	const long *flags_orig = ra_value_get_long_array(flags);
	const long *ch_num = ra_value_get_long_array(ch);
	long *flags_new = new long[num];
	for (long l = 0; l < num; l++)
	{
		if (ch_num[l] == curr_ch)
			flags_new[l] = flags_orig[l] & clean_mask;
		else
			flags_new[l] = flags_orig[l];
	}

	ra_value_set_long_array(flags, flags_new, num);

	ret = ra_prop_set_value_mass(_prop_morph_flags, ra_value_get_long_array(ev_id),
				     ra_value_get_long_array(ch), flags);

	delete[] flags_new;

	ra_value_free(flags);
	ra_value_free(ch);
	ra_value_free(ev_id);

	post_process();
	send_eval_change();
} // remove_ch_morph_flags()


void
ecg_view_general::remove_ev_morph_flags(long clean_mask, long ev_id)
{
	if (!_prop_morph_flags)
		return;

	value_handle ch = ra_value_malloc();
	int ret;
	if ((ret = ra_prop_get_ch(_prop_morph_flags, ev_id, ch)) != 0)
	{
		ra_value_free(ch);
		return;
	}

	const short *ch_num = ra_value_get_short_array(ch);
	for (unsigned long l = 0; l < ra_value_get_num_elem(ch); l++)
	{
		ra_prop_get_value(_prop_morph_flags, ev_id, ch_num[l], _vh);
		long flag = ra_value_get_long(_vh);
		flag &= clean_mask;
		ra_value_set_long(_vh, flag);
		ra_prop_set_value(_prop_morph_flags, ev_id, ch_num[l], _vh);
	}

	ra_value_free(ch);

	correct_neighbors();
	send_eval_change();
} // remove_ev_morph_flags()


void
ecg_view_general::remove_single_morph_flags(long clean_mask, long ev_id, long ch)
{
	if (!_prop_morph_flags)
		return;

	ra_prop_get_value(_prop_morph_flags, ev_id, ch, _vh);
	long flag = ra_value_get_long(_vh);
	flag &= clean_mask;
	ra_value_set_long(_vh, flag);
	ra_prop_set_value(_prop_morph_flags, ev_id, ch, _vh);

	correct_neighbors();
	send_eval_change();
} // remove_single_morph_flags()


int
ecg_view_general::fill_edit_beat_data()
{
	if (_edit_beat.data)
	{
		delete[] _edit_beat.data;
		_edit_beat.data = NULL;
	}
	if (_edit_beat.data_filt)
	{
		delete[] _edit_beat.data_filt;
		_edit_beat.data_filt = NULL;
	}
	if (_edit_beat.data_filt2)
	{
		delete[] _edit_beat.data_filt2;
		_edit_beat.data_filt2 = NULL;
	}

	if (_ev_idx < 0)
		return -1;

	prepare_edit_filter();

	_edit_beat.event_id = _event_ids[_ev_idx];
	_edit_beat.event_idx = _ev_idx;
	_edit_beat.ch = _ch[_curr_channel].ch_number;
	_edit_beat.ch_idx = _curr_channel;

	long start, end;
	ra_class_get_event_pos(_clh, _edit_beat.event_id, &start, &end);
	_edit_beat.pos = start;

	ra_prop_get_value(_prop_pos, _edit_beat.event_id, -1, _vh);
	_edit_beat.fiducial_pos = ra_value_get_long(_vh);

	_edit_beat.annot = -1;
	if (_prop_class)
	{
		ra_prop_get_value(_prop_class, _edit_beat.event_id, -1, _vh);
		_edit_beat.annot = ra_value_get_long(_vh);
	}

	double ms100 = 0.1 * _ch[_curr_channel].samplerate;

	_edit_beat.start = _edit_beat.pos - (long)(3.0 * ms100);
	if (_edit_beat.start < 0)
		_edit_beat.start = 0;

	_edit_beat.end = _edit_beat.pos + (long)(6.0 * ms100);
	_edit_beat.num_data = _edit_beat.end - _edit_beat.start + 1;
	if (_edit_beat.num_data <= 0)
		return -1;

	_edit_beat.data = new double[_edit_beat.num_data];
	_edit_beat.num_data = ra_raw_get_unit(_rh, _edit_beat.ch, _edit_beat.start, _edit_beat.num_data, _edit_beat.data);
	if (_edit_beat.num_data <= 0)
	{
		delete[] _edit_beat.data;
		_edit_beat.data = NULL;
		return -1;
	}

	int i;
	_edit_beat.mean = 0.0;
 	for (i = 0; i < _edit_beat.num_data; i++)
		_edit_beat.mean += _edit_beat.data[i];
 	_edit_beat.mean /= (double)(_edit_beat.num_data);
	for (i = 0; i < _edit_beat.num_data; i++)
		_edit_beat.data[i] -= _edit_beat.mean;

	_edit_beat.max = 0;
	for (i = 0; i < _edit_beat.num_data; i++)
	{
		if (_edit_beat.max < fabs(_edit_beat.data[i]))
			_edit_beat.max = fabs(_edit_beat.data[i]);
	}

	_edit_beat.xscale = EDIT_CURVE_LEN / (double)_edit_beat.num_data;
	_edit_beat.yscale = ((double)_edit_beat.draw_area/2.0) / _edit_beat.max;

	filter_data();
	
	// get beat infos
	// P-wave info
	_edit_beat.p_type = _edit_beat.p_type_save = -1;
	if (ra_prop_get_value(_prop_p_type, _edit_beat.event_id, _edit_beat.ch, _vh) == 0)
		_edit_beat.p_type = _edit_beat.p_type_save = ra_value_get_long(_vh);

	_edit_beat.p_start = _edit_beat.p_end = _edit_beat.p_start_save = _edit_beat.p_end_save = 32767;
	if (ra_prop_get_value(_prop_p_start, _edit_beat.event_id, _edit_beat.ch, _vh) == 0)
		_edit_beat.p_start = _edit_beat.p_start_save = ra_value_get_short(_vh);
	if (ra_prop_get_value(_prop_p_end, _edit_beat.event_id, _edit_beat.ch, _vh) == 0)
		_edit_beat.p_end = _edit_beat.p_end_save = ra_value_get_short(_vh);

	// QRS info
	_edit_beat.qrs_type = _edit_beat.qrs_type_save = -1;
	if (ra_prop_get_value(_prop_qrs_type, _edit_beat.event_id, _edit_beat.ch, _vh) == 0)
		_edit_beat.qrs_type = _edit_beat.qrs_type_save = ra_value_get_long(_vh);

	_edit_beat.qrs_start = _edit_beat.qrs_end = _edit_beat.qrs_start_save = _edit_beat.qrs_end_save = 32767;
	if (ra_prop_get_value(_prop_qrs_start, _edit_beat.event_id, _edit_beat.ch, _vh) == 0)
		_edit_beat.qrs_start = _edit_beat.qrs_start_save = ra_value_get_short(_vh);
	if (ra_prop_get_value(_prop_qrs_end, _edit_beat.event_id, _edit_beat.ch, _vh) == 0)
		_edit_beat.qrs_end = _edit_beat.qrs_end_save = ra_value_get_short(_vh);

	// T-wave info
	_edit_beat.t_type = _edit_beat.t_type_save = -1;
	if (ra_prop_get_value(_prop_t_type, _edit_beat.event_id, _edit_beat.ch, _vh) == 0)
		_edit_beat.t_type = _edit_beat.t_type_save = ra_value_get_long(_vh);

	_edit_beat.t_start = _edit_beat.t_end = _edit_beat.t_start_save = _edit_beat.t_end_save = 32767;
	if (ra_prop_get_value(_prop_t_start, _edit_beat.event_id, _edit_beat.ch, _vh) == 0)
		_edit_beat.t_start = _edit_beat.t_start_save = ra_value_get_short(_vh);
	if (ra_prop_get_value(_prop_t_end, _edit_beat.event_id, _edit_beat.ch, _vh) == 0)
		_edit_beat.t_end = _edit_beat.t_end_save = ra_value_get_short(_vh);

	return 0;
} // fill_edit_beat_data()


int
ecg_view_general::prepare_edit_filter()
{	
	_edit_beat.interp_factor = 1;
	if (_ch[_curr_channel].samplerate < 500)
	{
		double temp = _ch[_curr_channel].samplerate / 10;
		if (((_ch[_curr_channel].samplerate/10.0)-(double)temp) != 0)
			_edit_beat.interp_factor = (long)(512.0 / _ch[_curr_channel].samplerate);
		else
			_edit_beat.interp_factor = (long)(500 / _ch[_curr_channel].samplerate);
	}

	double samplerate_use = _ch[_curr_channel].samplerate * (double)_edit_beat.interp_factor;

	if (_edit_beat.filter_coef)
	{
		delete[] _edit_beat.filter_coef;
		_edit_beat.filter_coef = NULL;
	}
 	_edit_beat.filter_len = 41;
	fir_filter_design(_edit_beat.filter_len, (0.05 / samplerate_use),
			  (40.0/ samplerate_use), &(_edit_beat.filter_coef), &(_edit_beat.filter_len));

	if (_edit_beat.filter_coef2)
	{
		delete[] _edit_beat.filter_coef2;
		_edit_beat.filter_coef2 = NULL;
	}
  	_edit_beat.filter_len2 = 21;
 	fir_filter_design(_edit_beat.filter_len2, (42.5 / samplerate_use),
 			  (105.0/ samplerate_use), &(_edit_beat.filter_coef2), &(_edit_beat.filter_len2));

	return 0;
} // prepare_edit_filter()


int
ecg_view_general::filter_data()
{
	if (_filter_powerline_noise)
	{
		ra_raw_process_unit(RA_RAW_PROC_RM_POWERLINE_NOISE, NULL, _edit_beat.num_data, _edit_beat.data,
				    _rh, _edit_beat.ch);
	}

	long start = _edit_beat.start - (2 * _edit_beat.filter_len);
	if (start < 0)
		start = 0;
	long end = _edit_beat.end + (2 * _edit_beat.filter_len);
	long num = end - start + 1;
	double *buf = new double[num * (_edit_beat.interp_factor > 1 ? _edit_beat.interp_factor : 1)];
	num = ra_raw_get_unit(_rh, _edit_beat.ch, start, num, buf);

	long l;
	for (l = 0; l < num; l++)
		buf[l] -= _edit_beat.mean;

	if (_filter_powerline_noise)
	{
		ra_raw_process_unit(RA_RAW_PROC_RM_POWERLINE_NOISE, NULL, num, buf,
				    _rh, _edit_beat.ch);
	}

	if (_edit_beat.interp_factor > 1)
	{
		double *buf_temp;

		buf_temp = new double[num];
		memcpy(buf_temp, buf, sizeof(double) * num);
		interp(buf_temp, buf, num, _edit_beat.interp_factor);

		num *= _edit_beat.interp_factor;
		delete[] buf_temp;
	}

	double *buf_filt = new double[num];

	memcpy(buf_filt, buf, sizeof(double) * num);
	filtfilt(_edit_beat.filter_coef, NULL, _edit_beat.filter_len, buf_filt, num);
	if (_edit_beat.interp_factor > 1)
	{
		long cnt = 0;
		for (l = 0; l < num; l += _edit_beat.interp_factor)
			buf_filt[cnt++] = buf[l];
	}

	_edit_beat.data_filt = new double[_edit_beat.num_data];
	memcpy(_edit_beat.data_filt, &(buf_filt[_edit_beat.start - start]), sizeof(double) * _edit_beat.num_data);

	memcpy(buf_filt, buf, sizeof(double) * num);
	filtfilt(_edit_beat.filter_coef2, NULL, _edit_beat.filter_len2, buf_filt, num);
	if (_edit_beat.interp_factor > 1)
	{
		long cnt = 0;
		for (l = 0; l < num; l += _edit_beat.interp_factor)
			buf_filt[cnt++] = buf[l];
	}

	_edit_beat.data_filt2 = new double[_edit_beat.num_data];
	memcpy(_edit_beat.data_filt2, &(buf_filt[_edit_beat.start - start]), sizeof(double) * _edit_beat.num_data);

	delete[] buf;
	delete[] buf_filt;

	return 0;
} // filter_data()


void
ecg_view_general::mouse_press_edit_area(int /*x*/, int /*y*/)
{
} // mouse_press_edit_area()



	/* filter DC component */
/* 	filter_len = 501; */
/* 	filter = calloc(filter_len+1, sizeof(double)); */
/* 	fir_filter_design(filter_len, 0, (0.5 / i->samplerate), &filter, &filter_len); */
/* 	buf = malloc(sizeof(double) * i->num_samples); */
/* 	for (j = 0; j < NUM_CH; j++) */
/* 	{ */
/* 		for (l = 0; l < i->num_samples; l++) */
/* 			buf[l] = i->samples_scaled[l*NUM_CH + j]; */

/* 		filtfilt(filter, NULL, filter_len, buf, i->num_samples); */
/* 		for (l = 0; l < i->num_samples; l++) */
/* 			i->samples_scaled[l*NUM_CH + j] -= buf[l]; */
/* 	} */
/* 	free(buf); */
/* 	free(filter); */
	


void
ecg_view_general::redo_wave_boundaries(long p_type, long qrs_type, long t_type, bool save, bool apply_all,
				       void (*callback) (const char *, int ))
{
 	eval_handle eh = ra_eval_get_default(_mh);
 	if (!eh || !_plugin_detect_ecg)
 		return;

	set_busy_cursor();

	struct proc_info *pi = (struct proc_info *)ra_proc_get(_mh, _plugin_detect_ecg, callback);

 	ra_value_set_short(_vh, (save ? 1 : 0));
	ra_lib_set_option(pi, "save_in_eval", _vh);
	ra_value_set_voidp(_vh, eh);
	ra_lib_set_option(pi, "eh", _vh);
	ra_value_set_voidp(_vh, _clh);
	ra_lib_set_option(pi, "clh", _vh);

	long ch = _edit_beat.ch;
	ra_value_set_long_array(_vh, &ch, 1);
	ra_lib_set_option(pi, "ch", _vh);

	ra_value_set_long(_vh, p_type);
	ra_lib_set_option(pi, "force_p_type", _vh);
	ra_value_set_long(_vh, qrs_type);
	ra_lib_set_option(pi, "force_qrs_type", _vh);
	ra_value_set_long(_vh, t_type);
	ra_lib_set_option(pi, "force_t_type", _vh);

	ra_value_set_short(_vh, 1);
	ra_lib_set_option(pi, "do_interpolation", _vh);
	ra_value_set_short(_vh, (_filter_powerline_noise ? 1 : 0));
	ra_lib_set_option(pi, "filter_powerline_noise", _vh);
	ra_value_set_short(_vh, (_remove_mean ? 1 : 0));
	ra_lib_set_option(pi, "filter_baseline_wander", _vh);

	if (apply_all)
	{
		long num_ev = 0;
		long *ev_ids = NULL;
		
		// int ret = get_template_events(_edit_beat.event_id, &num_ev, &ev_ids);
		int ret = get_same_annot_events(_edit_beat.event_id, &num_ev, &ev_ids);
		if ((ret != 0) || (num_ev <= 0))
			return;

		ra_value_set_long_array(_vh, ev_ids, num_ev);
		ra_lib_set_option(pi, "events", _vh);

		delete[] ev_ids;
	}
	else
	{
		ra_value_set_long_array(_vh, &(_edit_beat.event_id), 1);
		ra_lib_set_option(pi, "events", _vh);
	}

	if (ra_proc_do(pi) != 0)
	{
		ra_proc_free(pi);
		unset_cursor();
		return;
	}

	if (apply_all)
	{
		send_eval_change();
		ra_proc_free(pi);
		unset_cursor();
		return;
	}

	int ret = ra_proc_get_result(pi, 0, 0, _vh);
	if (ret != 0)
	{
		ra_proc_free(pi);
		unset_cursor();
		return;
	}

	long num_beats = ra_value_get_long(_vh);
	if (num_beats != 1)
	{
		ra_proc_free(pi);
		unset_cursor();
		return;
	}

	if (ra_proc_get_result(pi, 8, 0, _vh) == 0)
		_edit_beat.p_type = ra_value_get_long_array(_vh)[0];
	if (ra_proc_get_result(pi, 14, 0, _vh) == 0)
		_edit_beat.p_start = ra_value_get_short_array(_vh)[0];
	if (ra_proc_get_result(pi, 15, 0, _vh) == 0)
		_edit_beat.p_end = ra_value_get_short_array(_vh)[0];

	if (ra_proc_get_result(pi, 10, 0, _vh) == 0)
		_edit_beat.qrs_type = ra_value_get_long_array(_vh)[0];
	if (ra_proc_get_result(pi, 22, 0, _vh) == 0)
		_edit_beat.qrs_start = ra_value_get_short_array(_vh)[0];
	if (ra_proc_get_result(pi, 23, 0, _vh) == 0)
		_edit_beat.qrs_end = ra_value_get_short_array(_vh)[0];

	if (ra_proc_get_result(pi, 12, 0, _vh) == 0)
		_edit_beat.t_type = ra_value_get_long_array(_vh)[0];
	if (ra_proc_get_result(pi, 34, 0, _vh) == 0)
		_edit_beat.t_start = ra_value_get_short_array(_vh)[0];
	if (ra_proc_get_result(pi, 35, 0, _vh) == 0)
		_edit_beat.t_end = ra_value_get_short_array(_vh)[0];

	if (save)
	{
		correct_neighbors();
		send_eval_change();
	}

	unset_cursor();

	ra_proc_free(pi);
} // redo_wave_boundaries()


int
ecg_view_general::get_template_events(long event_id, long *num_ev, long **ev_ids)
{
	int ret;

	if ((ret = ra_sum_get(_clh, "template", _vh)) != 0)
		return ret;

	long num = ra_value_get_num_elem(_vh);
	if (num <= 0)
		return -1;

	sum_handle *sh = new sum_handle[num];
	memcpy(sh, ra_value_get_voidp_array(_vh), sizeof(sum_handle) * num);

	bool found = false;
	for (long l = 0; l < num; l++)
	{
		if ((ret = ra_sum_get_part_all(sh[l], _vh)) != 0)
			continue;

		long num_p = ra_value_get_num_elem(_vh);
		long *id = new long[num_p];
		memcpy(id, ra_value_get_long_array(_vh), sizeof(long) * num_p);

		for (long m = 0; m < num_p; m++)
		{
			if ((ret = ra_sum_get_part_events(sh[l], id[m], _vh)) != 0)
				continue;

			const long *ev = ra_value_get_long_array(_vh);
			for (unsigned long n = 0; n < ra_value_get_num_elem(_vh); n++)
			{
				if (ev[n] == event_id)
				{
					*num_ev = ra_value_get_num_elem(_vh);
					*ev_ids = new long[*num_ev];
					memcpy(*ev_ids, ev, sizeof(long) * (*num_ev));

					found = true;
					break;
				}
			}
			if (found)
				break;
		}
		delete[] id;

		if (found)
			break;
	}
	delete[] sh;

	return 0;
} // get_template_events()


int
ecg_view_general::get_same_annot_events(long event_id, long *num_ev, long **ev_ids)
{
	*num_ev = 0;
	*ev_ids = NULL;

	if (_prop_class == NULL)
		return 0;

	value_handle vh = ra_value_malloc();
	int ret;
	if ((ret = ra_prop_get_value(_prop_class, event_id, -1, vh)) != 0)
	{
		ra_value_free(vh);
		return ret;
	}
	long annot = GET_CLASS(ra_value_get_long(vh));

	value_handle vh_ev_id = ra_value_malloc();
	value_handle vh_ch = ra_value_malloc();
	if ((ret = ra_prop_get_value_all(_prop_class, vh_ev_id, vh_ch, vh)) != 0)
	{
		ra_value_free(vh);
		ra_value_free(vh_ev_id);
		ra_value_free(vh_ch);
		return ret;
	}
	long num = ra_value_get_num_elem(vh);
	const long *ids = ra_value_get_long_array(vh_ev_id);
	const long *all_annots = ra_value_get_long_array(vh);

	*ev_ids = new long[num];
	for (long l = 0; l < num; l++)
	{
		if (GET_CLASS(all_annots[l]) == annot)
		{
			(*ev_ids)[*num_ev] = ids[l];
			(*num_ev)++;
		}
	}

	ra_value_free(vh);
	ra_value_free(vh_ev_id);
	ra_value_free(vh_ch);

	return 0;	
} // get_same_annot_events()


void
ecg_view_general::toggle_edit_mode()
{
	// before we change in edit-mode check if an event was selected
	if (!_in_edit_mode && (_ev_idx < 0))
		return;  // no -> do nothing
	
	_in_edit_mode = (_in_edit_mode ? false : true);
	if (_in_edit_mode)
	{
		_left_space += EDIT_SPACE;
		_ch_info_offset +=EDIT_SPACE;
		fill_edit_beat_data();
	}
	else
	{
		_left_space -= EDIT_SPACE;
		_ch_info_offset -= EDIT_SPACE;
	}
}  // toggle_edit_mode()


void
ecg_view_general::set_area_to_artifacts()
{
	set_area_beats(ECG_CLASS_ARTIFACT, 0);
} // set_area_to_artifacts()


void
ecg_view_general::set_area_to_normals()
{
	set_area_beats(ECG_CLASS_SINUS, ECG_CLASS_SINUS_NORMAL);
} // set_area_to_normals()


void
ecg_view_general::set_area_to_pacs()
{
	set_area_beats(ECG_CLASS_SINUS, ECG_CLASS_SINUS_SVPC);
} // set_area_to_pacs()


void
ecg_view_general::set_area_to_pvcs()
{
	set_area_beats(ECG_CLASS_VENT, 0);
} // set_area_to_pvcs()


void
ecg_view_general::set_area_to_paced()
{
	set_area_beats(ECG_CLASS_PACED, 0);
} // set_area_to_paced()


void
ecg_view_general::set_area_beats(int beat_class, int beat_sub_class)
{
	if (!_area_marked || !_clh || !_prop_class)
		return;

	if (ra_class_get_events(_clh, _area_start, _area_end, 0, 0, _vh) != 0)
		return;

	long n = ra_value_get_num_elem(_vh);
	if (n <= 0)
		return;

	set_busy_cursor();

	// set found events to artifacts
	long *ev = new long[n];
	const long *ev_arr = ra_value_get_long_array(_vh);
	for (long l = 0; l < n; l++)
		ev[l] = ev_arr[l];

	long cl = ECG_CLASS_UNKNOWN;
	if (beat_class)
		SET_CLASS(beat_class, cl);
	if (beat_sub_class)
		SET_SUBCLASS(beat_sub_class, cl);

	for (long l = 0; l < n; l++)
	{
		change_class(ev[l], cl);
		correct_neighbors(ev[l]);
	}
	delete[] ev;

	// tell others that something was changed
	_block_signals = true;
	ra_comm_emit(_mh, "ecg-view", "eval-change", 0, NULL);
	_block_signals = false;

	unset_cursor();

	update();
} // set_area_beats()


void
ecg_view_general::remove_beats_in_area()
{
	if (!_area_marked || !_clh || !_prop_class)
		return;

	if (ra_class_get_events(_clh, _area_start, _area_end, 0, 0, _vh) != 0)
		return;

	long n = ra_value_get_num_elem(_vh);
	if (n <= 0)
		return;

	set_busy_cursor();

	long *ev = new long[n];
	const long *ev_arr = ra_value_get_long_array(_vh);
	for (long l = 0; l < n; l++)
		ev[l] = ev_arr[l];

	// get beat after the marked area to correct RR interval of it
	long event_after_area = ra_class_get_next_event(_clh, ev[n-1]);

	// remove found events
	for (long l = 0; l < n; l++)
	{
		do_delete_beat(ev[l]);
	}
	delete[] ev;

	// correct RRI of event after marked area
	correct_neighbors(event_after_area);

	unset_cursor();

	update();
} // remove_beats_in_area()


void
ecg_view_general::save_settings(const char *file)
{
	ts_view_general::save_settings(file);
} // save_settings()


void
ecg_view_general::load_settings(const char *file)
{
	ts_view_general::load_settings(file);
} // load_settings()

