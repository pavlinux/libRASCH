/*----------------------------------------------------------------------------
 * ts_view_general.cpp
 *
 *
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002-2008, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifdef _AFXDLL			// so mfc-libs are linked before c-lib libs
//#include "stdafx.h"
#endif // _AFXDLL

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ts_view_general.h"

#define _LIBRASCH_BUILD
#include <ra_priv.h>

#ifdef WIN32
	/* Using fprintf() and printf() from libintl will crash on Win32.
	   The reason is maybe using different c-libs. */
	#undef fprintf
	#undef printf
	#undef sprintf
	#undef snprintf
#endif  /* WIN32 */

//#include <algorithm>

ts_view_general::ts_view_general(meas_handle h, int num_ch, int *ch)
{
	strncpy(_plugin_name, "ts-view", RA_VALUE_NAME_MAX);

	_mh = h;
	_block_signals = false;
	_max_samplerate = 0.0;
	_max_xscale = 1.0;
	_max_num_samples = 0;
	_top_space = _bottom_space = _left_space = _right_space = 0;
	_curr_pos = 0;
	_cursor_pos = 0;
	_cursor_ch = 0;
	_update_cursor_pos = true;
	_cursor_ch_prev = -1;
	_cursor_pos_prev = -1;

	_ev_idx = -1;
	_num_events = 0;
	_event_ids = NULL;

	_num_sort = _curr_sort = 0;
	_sort = NULL;
	_sort_is_pos = 0;

	_ts_handle_cursor = true;

	_already_changed = false;

	_use_scrollbar = false;

	_ts_opt.ch_height_dynamic = true;
	_ts_opt.ch_height = 100;
	_ts_opt.show_ch_info = true;
	_ts_opt.show_ch_scale = true;
	_ts_opt.show_status = true;
	_ts_opt.plot_grid = true;
	_ts_opt.show_annot = true;

	_page_width = 0;
	_screen_width = 0;
	_screen_height = 0;
	_height_scale = 1.0;

	_area_marked = false;
	_area_mark_step = NOT_MARKING_AREA;
	_area_start = -1;

	_clh = NULL;
	_prop_pos = NULL;
	_eh = NULL;
	_annot_class = NULL;
	_annot_prop = NULL;
	_num_annot = 0;
	_annot = NULL;

	_num_annot_predefs = 0;
	_annot_predefs = NULL;

	_num_aoi = 0;
	_aoi = NULL;

	_num_sessions = 1;
	_curr_session = 0;

	_curr_channel = -1;

	_vh = ra_value_malloc();

	_remove_mean = false;
	_filter_powerline_noise = false;

	_dont_paint = false;
	_has_input_focus = false;

	_num_templates = 0;
	_templates = NULL;

	if (num_ch > 0)
	{
		_num_ch = num_ch;
		_ch = new struct ch_info[num_ch];
		memset(_ch, 0, sizeof(struct ch_info) * num_ch);
		for (int i = 0; i < num_ch; i++)
		{
			_ch[i].ch_number = ch[i];
			_ch[i].fixed_top_win_pos = -1;
		}
	}
	else
	{
		_num_ch = 0;
		_ch = NULL;
	}

	if (_ts_opt.show_ch_info)
		_left_space += 40;
	if (_ts_opt.show_ch_scale)
		_left_space += 50;

	_ch_info_offset = 3;

	if (_ts_opt.show_status)
		_bottom_space = 20;

	if (h == NULL)
		return;

	init(h);
} // constructor


ts_view_general::~ts_view_general()
{
	ra_comm_del(_mh, (void *)this, NULL);

	if (_ch)
		delete[] _ch;
	if (_sort)
		delete[] _sort;
	if (_annot)
	{
		for (int i = 0; i < _num_annot; i++)
		{
			if (_annot[i].text)
				delete[] _annot[i].text;
		}
		delete[] _annot;
	}
	if (_annot_predefs)
	{
		for (int i = 0; i < _num_annot_predefs; i++)
		{
			if (_annot_predefs[i])
				delete[] _annot_predefs[i];
		}
		delete[] _annot_predefs;
	}
	if (_aoi)
		delete[] _aoi;

	if (_event_ids)
		delete[] _event_ids;

	if (_templates)
	{
		for (long l = 0; l < _num_templates; l++)
		{
			if (_templates[l].event_ids)
				free(_templates[l].event_ids);
		}
		free(_templates);
	}

	ra_value_free(_vh);
} // destructor


void
ts_view_general::update()
{
	// do nothing
} // update()


void
ts_view_general::init(meas_handle h)
{
	_mh = h;

	if (ra_info_get(_mh, RA_INFO_NUM_SESSIONS_L, _vh) == 0)
		_num_sessions = ra_value_get_long(_vh);

	_rh = ra_rec_get_first(_mh, _curr_session);
	if (_num_ch == 0)
	{
		if (ra_info_get(_rh, RA_INFO_REC_GEN_NUM_CHANNEL_L, _vh) == 0)
			_num_ch = ra_value_get_long(_vh);
		else
			_num_ch = 0;
	}

	if (ra_info_get(_rh, RA_INFO_REC_GEN_MM_SEC_D, _vh) == 0)
		_ts_opt.mm_per_sec = ra_value_get_double(_vh);
	else
		_ts_opt.mm_per_sec = 1.0;

	if (_num_ch > 0)
	{
		int i;		// Visual C++
		if (_ch == NULL)
		{
			_ch = new struct ch_info[_num_ch];
			memset(_ch, 0, sizeof(struct ch_info) * _num_ch);
			for (i = 0; i < _num_ch; i++)
			{
				_ch[i].ch_number = i;
				_ch[i].fixed_top_win_pos = -1;
			}
		}
		for (i = 0; i < _num_ch; i++)
			get_ch_info(i);

 		calc_max_scale_factors();

		// calc pos_factor for each channel
		for (i = 0; i < _num_ch; i++)
			_ch[i].pos_factor = _ch[i].samplerate / _max_samplerate;
	}
	else
		_ch = NULL;

	if (ra_info_get(_rh, RA_INFO_REC_GEN_TIME_C, _vh) == 0)
	{
		char t[10];
		strncpy(t, ra_value_get_string(_vh), 9);
		t[2] = '\0';
		t[5] = '\0';
		_start_sec = atoi(t) * 3600 + atoi(t + 3) * 60 + atoi(t + 6);
	}
	else
		_start_sec = 0;
	// save start of first recording session (needed later for multi-session recordings)
	// TODO: think about better handling of multi-session recordings re. annotations
	_start_meas_sec = _start_sec;

	// check if an online signal is shown
	_is_online = false;
	value_handle vh = ra_value_malloc();
	if (ra_info_get(_rh, RA_INFO_REC_GEN_TYPE_L, vh) == 0)
	{
		if (ra_value_get_long(vh) == RA_REC_TYPE_DAQ_TIMESERIES)
			_is_online = true;
	}
	ra_value_free(vh);

	read_eval();
} // init()


void
ts_view_general::calc_max_scale_factors()
{
	_max_samplerate = -10000.0;

	for (int i = 0; i < _num_ch; i++)
	{
		if (_max_samplerate < _ch[i].samplerate)
		{
			_max_samplerate = _ch[i].samplerate;
			_max_xscale = _ch[i].xscale;
			_max_num_samples = _ch[i].num_samples;
		}
	}

	value_handle vh = ra_value_malloc();
	ra_info_get(_mh, RA_INFO_MAX_SAMPLERATE_D, vh);
	double sr = ra_value_get_double(vh);
	if (sr != _max_samplerate)
	{
		_max_xscale = (_max_samplerate / sr) * _max_xscale;
		_max_num_samples = (long)((sr / _max_samplerate) * (double)_max_num_samples);
		_max_samplerate = sr;		
	}
	ra_value_free(vh);
} // calc_max_scale_factors()


void
ts_view_general::get_ch_info(int ch)
{
	_ch[ch].show_it = true;

	ra_value_set_number(_vh, _ch[ch].ch_number);
	if (ra_info_get(_rh, RA_INFO_REC_CH_CENTER_VALUE_D, _vh) == 0)
	{
		_ch[ch].centered = true;
		_ch[ch].center_value = ra_value_get_double(_vh);
		_ch[ch].center_pos = 50;
		_ch[ch].inverse = false;
	}
	else
	{
		_ch[ch].centered = false;
		ra_info_get(_rh, RA_INFO_REC_CH_MIN_UNIT_D, _vh);
		_ch[ch].min_value = ra_value_get_double(_vh);
		if (ra_info_get(_rh, RA_INFO_REC_CH_MAX_UNIT_D, _vh) == 0)
		{
			_ch[ch].max_value = ra_value_get_double(_vh);
			_ch[ch].use_mm_per_unit = false;
		}
		else
		{
			// no max value, than use mm/unit (got from recording in
			// next ra_info_get() call below)
			_ch[ch].use_mm_per_unit = true;
		}
	}

	if (ra_info_get(_rh, RA_INFO_REC_CH_MM_UNIT_D, _vh) == 0)
	{
		_ch[ch].mm_per_unit = ra_value_get_double(_vh);
		// if the access plugin returns a mm/unit value, changes are higher
		// that this should be used instead to scale the values between min and max
		_ch[ch].use_mm_per_unit = true;
	}
	else
		_ch[ch].mm_per_unit = 1.0;

	if (ra_info_get(_rh, RA_INFO_REC_CH_NAME_C, _vh) == 0)
		strncpy(_ch[ch].name, ra_value_get_string_utf8(_vh), 20);
	if (ra_info_get(_rh, RA_INFO_REC_CH_UNIT_C, _vh) == 0)
		strncpy(_ch[ch].unit, ra_value_get_string_utf8(_vh), 20);
	if (ra_info_get(_rh, RA_INFO_REC_CH_SAMPLERATE_D, _vh) == 0)
		_ch[ch].samplerate = ra_value_get_double(_vh);
	if (ra_info_get(_rh, RA_INFO_REC_CH_AMP_RESOLUTION_D, _vh) == 0)
		_ch[ch].amp_res = ra_value_get_double(_vh);
	if (ra_info_get(_rh, RA_INFO_REC_CH_NUM_SAMPLES_L, _vh) == 0)
		_ch[ch].num_samples = ra_value_get_long(_vh);
	if (ra_info_get(_rh, RA_INFO_REC_CH_TYPE_L, _vh) == 0)
		_ch[ch].type = ra_value_get_long(_vh);
	if (ra_info_get(_rh, RA_INFO_REC_CH_DO_IGNORE_VALUE_L, _vh) == 0)
		_ch[ch].do_ignore_value = ra_value_get_long(_vh);
	if ((_ch[ch].do_ignore_value == 1) && (ra_info_get(_rh, RA_INFO_REC_CH_IGNORE_VALUE_D, _vh) == 0))
		_ch[ch].ignore_value = ra_value_get_double(_vh);

	calc_scale(ch);
} // get_ch_info()


void
ts_view_general::calc_scale(int ch)
{
	// calculation of x-scaling-factor
	// 1. speed[mm/sec] / samplerate[sample/sec] => foo[mm/sample]
	// 2. foo[mm/sample] * res[pix/mm] => scale[pix/sample]
	if (_ch[ch].samplerate == 0)
		_ch[ch].xscale = 1.0;
	else
	{
		double foo = _ts_opt.mm_per_sec / _ch[ch].samplerate;
		_ch[ch].xscale = foo * _pixmm_h;
	}

	// similar for y-axis
	if (_ch[ch].amp_res == 0)
		_ch[ch].yscale = 1;
	else
	{
		double foo = _ch[ch].mm_per_unit;  // '/ _ch[ch].amp_res' no longer needed because of ra_raw_get_unit()
		_ch[ch].yscale = foo * _pixmm_v;
	}
	_ch[ch].yscale *= _height_scale;
} // calc_scale()


int
ts_view_general_cmp_long(const void *p1, const void *p2)
{
	return ((*(long *)p1) - (*(long *)p2));
} /* ts_view_general_cmp_long() */


void
ts_view_general::get_template_infos(class_handle clh)
{
	_num_templates = 0;
	if (_templates)
	{
		for (long l = 0; l < _num_templates; l++)
		{
			if (_templates[l].event_ids)
				free(_templates[l].event_ids);
		}
		free(_templates);
		_templates = NULL;
	}

	ra_sum_get(clh, "template", _vh);
	if (ra_value_get_num_elem(_vh) > 0)
	{
		// use the first event summary (TODO: decide how to handle more than one event summaries)
		_template_sh = (sum_handle)(ra_value_get_voidp_array(_vh)[0]);
		ra_sum_get_part_all(_template_sh, _vh);
		_num_templates = ra_value_get_num_elem(_vh);
		if (_num_templates > 0)
		{
			_templates = (struct templ_info *)calloc(_num_templates, sizeof(struct templ_info));
			for (long l = 0; l < _num_templates; l++)
				_templates[l].part_id = ra_value_get_long_array(_vh)[l];
		}
		for (long l = 0; l < _num_templates; l++)
		{
			ra_sum_get_part_events(_template_sh, _templates[l].part_id, _vh);
			_templates[l].num_events = ra_value_get_num_elem(_vh);
			if (_templates[l].num_events > 0)
			{
				_templates[l].event_ids = (long *)malloc(sizeof(long) * _templates[l].num_events);
				for (long m = 0; m < _templates[l].num_events; m++)
					_templates[l].event_ids[m] = ra_value_get_long_array(_vh)[m];

				qsort(_templates[l].event_ids, _templates[l].num_events, sizeof(long),
				      ts_view_general_cmp_long);
			}
			
		}
	}
} // get_template_infos()


void
ts_view_general::read_eval()
{
	// up to now handle only textual comments
	init_comment_data();
} // read_eval()


void
ts_view_general::init_comment_data()
{
	_eh = ra_eval_get_default(_mh);
	if (!_eh)
		return;

	// free previously allocated mem for the annotations
	if (_annot)
	{
		for (int i = 0; i < _num_annot; i++)
		{
			if (_annot[i].text)
				delete[] _annot[i].text;
		}
		delete[] _annot;
		_annot = NULL;
	}

	// get the annotation event-class and -property
	if (ra_class_get(_eh, "annotation", _vh) != 0)
		return;
	if (ra_value_get_num_elem(_vh) <= 0)
		return;

	if (ra_value_get_num_elem(_vh) != 1)
	{
		// TODO: think how to handle more than one annotation event-class
		fprintf(stderr, "more than one annotation class !!!");
		return;
	}
	_annot_class = (class_handle)(ra_value_get_voidp_array(_vh)[0]);

	_annot_prop = ra_prop_get(_annot_class, "annotation");
	if (_annot_prop == NULL)
		return;		

	// allocate mem for the available annotations
	value_handle vh_events = ra_value_malloc();
	if (ra_class_get_events(_annot_class, -1, -1, 0, 1, vh_events) != 0)
	{
		ra_value_free(vh_events);
		return;
	}
	const long *event_ids = ra_value_get_long_array(vh_events);

	_num_annot = ra_value_get_num_elem(vh_events);
	_annot = new struct annot_info[_num_annot];
	memset(_annot, 0, sizeof(struct annot_info) * _num_annot);

	// read annotations
	value_handle vh_value = ra_value_malloc();
	value_handle vh_ch = ra_value_malloc();
	for (long l = 0; l < _num_annot; l++)
	{
		ra_class_get_event_pos(_annot_class, event_ids[l], &(_annot[l].start), &(_annot[l].end));
		_annot[l].event_id = event_ids[l];

		ra_prop_get_ch(_annot_prop, event_ids[l], vh_ch);
		long n = ra_value_get_num_elem(vh_ch);
		if (n <= 0)
			continue;
		if (n > 1)
		{
			// TODO: there should be only ONE 'channel' for each annotation;
			//       think about it that this assumption is true and if there
			//       is a better way to handle/code/implement it!!!
			n = 1;
		}

		const short *ch = ra_value_get_short_array(vh_ch);
		for (long m = 0; m < n; m++)
		{
			ra_prop_get_value(_annot_prop, event_ids[l], ch[m], vh_value);

			char *buf = new char[strlen(ra_value_get_string_utf8(vh_value)) + 1];
			char *buf_save = buf;
			strcpy(buf, ra_value_get_string_utf8(vh_value));
			
			// look for flags
			if (search_flag(buf, "#NOISE#"))
				_annot[l].noise_flag = true;
			if (search_flag(buf, "#IGNORE#"))
				_annot[l].ignore_flag = true;
			
			_annot[l].text = new char[strlen(buf) + 1];
			strcpy(_annot[l].text, buf);
			delete[] buf_save;
			
			_annot[l].ch = ch[m];
		}
	}
	ra_value_free(vh_value);
	ra_value_free(vh_ch);
	ra_value_free(vh_events);
} // init_comment_data()


/*
 * Search in 'buf' for flag 'flag'. If found, remove 'flag' from buf and
 * return true otherwise return false.
 */
bool
ts_view_general::search_flag(char *buf, const char *flag)
{
	char *p = strstr(buf, flag);
	if (p == NULL)
		return false;	// not found

	// found, now remove flag
	size_t len = strlen(buf);
	size_t len_flag = strlen(flag);

	strncpy(p, p + len_flag, len - (p-buf) - len_flag);

	buf[len - len_flag] = '\0';

	return true;
} // search_flag()


bool
ts_view_general::next_page(bool whole_page)
{
	long pos = _curr_pos;

	if (whole_page)
		pos += _page_width;
	else
		pos += (_page_width / 10);

	return set_pos(pos, _max_xscale);
} // next_page()


bool
ts_view_general::prev_page(bool whole_page)
{
	long pos = _curr_pos;

	if (whole_page)
		pos -= _page_width;
	else
		pos -= (_page_width / 10);

	return set_pos(pos, _max_xscale);
} // prev_page()


bool
ts_view_general::goto_begin()
{
	return set_pos(0, _max_xscale);
} // goto_begin()


bool
ts_view_general::goto_end()
{
	return set_pos(_max_num_samples, _max_xscale);
} // goto_end()


void
ts_view_general::paint(long width, long height)
{	
	// set some variables (which depends on draw area)
	int w = width - _left_space - _right_space;
	long pw_save = _page_width;
	_page_width = (long) ((double) w / _max_xscale);

	// check if page-width has changed
	if (_page_width != pw_save)
	{
		// yes -> set scrollbar according to the new page width
		// the MFC version handle the scrollbar directly -> call virtual function
		init_scrollbar(_page_width, _max_num_samples, _max_xscale);

		send_init_scrollbar();
	}

	if (_ts_opt.ch_height_dynamic)
	{
		int n = 0;
		for (int i = 0; i < _num_ch; i++)
			if (_ch[i].show_it)
				n++;
		int h = height - _top_space - _bottom_space;
		_ts_opt.ch_height = h / n;
	}
} // paint()


void
ts_view_general::format_statusbar(char **status_text)
{
	if (!_ts_opt.show_status)
		return;

	long sec = (long) ((double) _start_sec + ((double) _curr_pos / _max_samplerate));
	char *sec_txt1 = new char[10];
	format_sec(sec, &sec_txt1);

	sec = (long) ((double) _curr_pos / _max_samplerate);
	char *sec_txt2 = new char[10];
	format_sec(sec, &sec_txt2);

	*status_text = new char[RA_VALUE_NAME_MAX + 50];
	sprintf((*status_text), gettext("time: %s (rel: %s)"), sec_txt1, sec_txt2);
	if (_num_sessions > 1)
	{
		char t[RA_VALUE_NAME_MAX + 10];
		const char *p;

		ra_value_set_number(_vh, _curr_session);
		ra_info_get(_mh, RA_INFO_SES_NAME_C, _vh);
		p = ra_value_get_string_utf8(_vh);
		if ((ra_value_is_ok(_vh)) && (p[0] != '\0'))
			sprintf(t, gettext("  --  session %s"), p);
		else
			sprintf(t, gettext("  --  session #%ld"), _curr_session+1);
		strcat((*status_text), t);
	}

	if ((_cursor_pos >= _left_space) && (_cursor_pos <= (_screen_width - _right_space))
		&& (_curr_channel >= 0))
	{
		double s = (double)(_cursor_pos - _left_space) / _max_xscale;
		s += ((double)(_start_sec - _start_meas_sec) * _max_samplerate) + _curr_pos;
		
		long msec = (long)((s*1000.0) / _max_samplerate);
		char *sec_cursor = new char[20];
		format_msec(msec, &sec_cursor);

		double val = 0.0;
		ra_raw_get_unit(_rh, _ch[_curr_channel].ch_number, (long)s, 1, &val);

		char t[100];
		sprintf(t, "  --  cursor at ch %s: %f %s (%s)", _ch[_curr_channel].name, val, _ch[_curr_channel].unit, sec_cursor);
		strcat((*status_text), t);

		delete[] sec_cursor;
	}

	delete[]sec_txt1;
	delete[]sec_txt2;
} // plot_statusbar()


void
ts_view_general::format_sec(long sec, char **text)
{
	double t = (double) sec;
	int h = (int) (t / 3600.0);
	t -= (h * 3600.0);
	h %= 24;
	int m = (int) (t / 60.0);
	t -= (m * 60.0);
	int s = (int) t;
	sprintf((*text), "%02d:%02d:%02d", h, m, s);
} // format_sec()


void
ts_view_general::format_msec(long msec, char **text)
{
	double t = (double)msec;
	int h = (int)(t / 3600000.0);
	t -= (h * 3600000.0);
	h %= 24;
	int m = (int)(t / 60000.0);
	t -= (m * 60000.0);
	int s = (int)(t / 1000.0);
	t -= (s * 1000.0);
	int ms = (int)t;
	sprintf((*text), "%02d:%02d:%02d.%03d", h, m, s, ms);
} // format_msec()


void
ts_view_general::ch_draw_data(int ch, int line, int **xa, int **ya, int **ignore, int *num)
{
	*xa = NULL;
	*ya = NULL;
	*num = 0;

	if (_ch[ch].fixed_top_win_pos != -1)
		_ch[ch].top_win_pos = _ch[ch].fixed_top_win_pos;
	else
		_ch[ch].top_win_pos = line * _ts_opt.ch_height;

	long n = (long) ((double) _page_width * _ch[ch].pos_factor);
	long pos = (long) ((double) _curr_pos * _ch[ch].pos_factor);

	if (n <= 0)
		return;

	double *data = new double[n];
	n = ra_raw_get_unit(_rh, _ch[ch].ch_number, pos, n, data);
	if (n <= 0)
	{
		delete[] data;
		return;
	}
	*num = n;

	_ch[ch].mean = 0.0;
	if (_remove_mean)
	{
		long l;

		for (l = 0; l < n; l++)
			_ch[ch].mean += data[l];
		_ch[ch].mean /= (double)n;
		for (l = 0; l < n; l++)
			data[l] -= _ch[ch].mean;
	}

	if (_filter_powerline_noise)
		ra_raw_process_unit(RA_RAW_PROC_RM_POWERLINE_NOISE, NULL, n, data, _rh, _ch[ch].ch_number);

	*num = n;

	if (n > 0)
	{
		*xa = new int[n];
		*ya = new int[n];
		*ignore = new int[n];

		double max_value = draw_calc_max_value(ch);
		for (int i = 0; i < n; i++)
		{
			int x = (int) ((double) i * _ch[ch].xscale) + _left_space;
			int y = draw_calc_amplitude(max_value, data[i], ch);
			y += _top_space;
			(*xa)[i] = x;
			(*ya)[i] = y + _ch[ch].top_win_pos;
			if (_ch[ch].do_ignore_value && (data[i] == _ch[ch].ignore_value))
				(*ignore)[i] = 1;
			else
				(*ignore)[i] = 0;
		}
	}

	delete[]data;
} // ch_draw_data()


double
ts_view_general::draw_calc_max_value(int ch)
{
	double max;

	if (_ch[ch].centered)
		max = ((double)_ts_opt.ch_height * ((double)(100 - _ch[ch].center_pos) / 100.0)) / _ch[ch].yscale;
	else
	{
		if (_ch[ch].use_mm_per_unit)
			max = (double)_ts_opt.ch_height / _ch[ch].yscale;
		else
			max = _ch[ch].max_value;
	}

	return max;
} // draw_calc_max_value()


bool
ts_view_general::goto_pos(long new_pos, bool center_pos)
{
	long pos = _curr_pos;

	if (center_pos)
		pos = new_pos - (_page_width / 2);
	else
		pos = new_pos;

	return set_pos(pos, _max_xscale);
} // goto_pos()


bool
ts_view_general::key_press_char(char c, bool /*shift*/, bool /*ctrl*/)
{
	_dont_paint = true;

	bool redraw = false;
	switch (c)
	{
	case 'a':
		break;
	case '.':
		redraw = insert_comment();
		break;
	case ',':
		start_area_mark(_cursor_pos);
		_area_mark_step = MARKING_AREA_MANUAL;
		break;
	case '-':
		end_area_mark(_cursor_pos);
		break;
	case 'A':
		sort_annotations();
		redraw = true;
		break;
	case ' ':
		next_sort();
		break;
	case 'B':
		prev_sort();
		break;
	default:
		break;
	}

	_dont_paint = false;

	return redraw;
} // key_press_char()


bool
ts_view_general::insert_comment()
{
	// TODO: if no evaluation -> create one
	if (!_eh)
	{
		_eh = ra_eval_add(_mh, "libRASCH", "evaluation created inside of libRASCH", 0);
		if (!_eh)
		{
			// TODO: make error-code for 'create eval failed'
			_ra_set_error(_mh, RA_ERR_ERROR, "plugin-ts-view");
			return false;
		}
	}


	// get annotation
	char *annot = NULL;
	bool noise = false;
	bool ignore = false;;
	int ch = -1;
	if ((annot_dlg(&annot, &noise, &ignore, &ch) != 0) || (annot == NULL))
		return false;	// user canceled operation, no redraw necessary

	long s = (long)((double)(_cursor_pos - _left_space) / _max_xscale);
	s += (long)((double)(_start_sec - _start_meas_sec) * _max_samplerate) + _curr_pos;
	long e = s;
	if (_area_marked)
	{
		s = _area_start;
		e = _area_end;
	}

	if (!_annot_class)
	{
		if ((_annot_class = ra_class_add_predef(_eh, "annotation")) == NULL)
			return false;
	}
	if (!_annot_prop)
	{
		if ((_annot_prop = ra_prop_add_predef(_annot_class, "annotation")) == NULL)
			return false;
	}

	long ev = ra_class_add_event(_annot_class, s, e);

	size_t len = strlen(annot) + 7 + 8 + 1;	// space for #NOISE#, #IGNORE# and ending '\0'
	char *t = new char[len];
	sprintf(t, "%s%s%s", (noise ? "#NOISE#" : ""), (ignore ? "#IGNORE#" : ""), annot);
	ra_value_set_string_utf8(_vh, t);
	ra_prop_set_value(_annot_prop, ev, ch, _vh);

	delete[] t;
	delete[] annot;

	init_comment_data();
	send_eval_change();

	// if area was marked, remove it
	_area_marked = false;

	return true;
} // insert_comment()


void
ts_view_general::update_comment(int idx)
{
	size_t len = strlen(_annot[idx].text) + 7 + 8 + 1;	// space for #NOISE#, #IGNORE# and ending '\0'
	char *t = new char[len];
	sprintf(t, "%s%s%s", (_annot[idx].noise_flag ? "#NOISE#" : ""),
		(_annot[idx].ignore_flag ? "#IGNORE#" : ""),
		_annot[idx].text);

	ra_value_set_string_utf8(_vh, t);
	ra_prop_set_value(_annot_prop, _annot[idx].event_id, _annot[idx].ch, _vh);

	init_comment_data();	
	send_eval_change();
} // update_comment();


void
ts_view_general::delete_comment(int idx)
{
	ra_class_del_event(_annot_class, _annot[idx].event_id);
	init_comment_data();	
	send_eval_change();
} // delete_comment()


// get comment-events for current shown data
void
ts_view_general::get_comments(void *p, struct curr_annot_info **info, int *n_comments)
{
	*info = NULL;
	*n_comments = 0;

	if (!_annot_class || !_annot_prop)
		return;

	for (long l = 0; l < _num_annot; l++)
	{
		// first check if channel for channel specific annot is shown here
		if (_annot[l].ch != -1)
		{
			bool found = false;
			for (long m = 0; m < _num_ch; m++)
			{
				if (_annot[l].ch == _ch[m].ch_number)
				{
					found = true;
					break;
				}
			}
			if (!found)
				continue;
		}

		long s = (long)((double)(_start_sec - _start_meas_sec) * _max_samplerate) + _curr_pos;
		if (((_annot[l].start >= s) && (_annot[l].start < (s + _page_width))) ||
		    ((_annot[l].end > s) && (_annot[l].end <= (s + _page_width))) ||
			((_annot[l].start < s) && (_annot[l].end > (s + _page_width))))
		{
			(*n_comments)++;

			*info = (struct curr_annot_info *)realloc(*info, sizeof(struct curr_annot_info) * (*n_comments));

			struct curr_annot_info *inf = &((*info)[(*n_comments)-1]);

			inf->annot_idx = l;
			inf->begin = (long)((double)(_annot[l].start - s) * _max_xscale) + _left_space;
			if (inf->begin < _left_space)
				inf->begin_not_shown = true;
			else
				inf->begin_not_shown = false;
			inf->end = (long)((double)(_annot[l].end - s) * _max_xscale) + _left_space;
			int draw_end = _screen_width - _right_space;
			if (inf->end > draw_end)
				inf->end_not_shown = true;
			else
				inf->end_not_shown = false;

			inf->text = new char[strlen(_annot[l].text) + 8];
			strcpy(inf->text, _annot[l].text);
			if (_annot[l].noise_flag || _annot[l].ignore_flag)
			{
				strcat(inf->text, " (");
				bool first = true;
				if (_annot[l].noise_flag)
				{
					if (!first)
						strcat(inf->text, ", ");
					strcat(inf->text, "N");
					first = false;
				}
				if (_annot[l].ignore_flag)
				{
					if (!first)
						strcat(inf->text, ", ");
					strcat(inf->text, "I");
					first = false;
				}
				strcat(inf->text, ")");
			}
			if (strlen(inf->text) == 0)
				strcpy(inf->text, "???"); // inf->text is at least 8 char long

			get_text_extent(p, inf->text, &inf->text_width, &inf->text_height);

			/* set position of text */
			inf->text_x = inf->begin + 2;
			if ((inf->text_x + inf->text_width) > draw_end)
				inf->text_x = draw_end - inf->text_width - 4;

			if (inf->text_x < _left_space)
				inf->text_x = _left_space + 4;

			if (_annot[l].ch != -1)
				inf->y = get_annot_height(_annot[l].ch);
			else
				inf->y = _screen_height - _top_space - _bottom_space - 5;		
		}
	}
} // get_comments()


int
ts_view_general::get_annot_height(long ch)
{
	// find screen-channel
	int screen_ch = -1;
	for (int i = 0; i < _num_ch; i++)
	{
//		if (!_ch[i].show_it)
//			continue;

		if (_ch[i].ch_number == ch)
		{
			screen_ch = i;
			break;
		}
	}
	if (screen_ch == -1)
		return -1;

	int y = _ch[screen_ch].top_win_pos + _top_space + _ts_opt.ch_height; // _ch[screen_ch].draw_space;

	return y;
} // get_annot_height()


// TODO: describe the algorithm how the row is selected
void
ts_view_general::place_comments(struct curr_annot_info *info, int n_comments)
{
	if (n_comments <= 0)
		return;

	// first get maximum text height so we have a fixed height for all annotations
	// (text height can be different when using different text encodings)
	int max_text_height = 0;
	for (int i = 0; i < n_comments; i++)
	{
		if (info[i].text_height > max_text_height)
			max_text_height = info[i].text_height;
	}

	bool again;
	do
	{
		again = false;
		for (int i = 0; i < n_comments; i++)
		{
			int s_fix = (info[i].begin < info[i].text_x ? info[i].begin : info[i].text_x);
			if (s_fix < _left_space)
				s_fix = _left_space;
			int e_fix = info[i].end;
			if (e_fix < (info[i].text_x + info[i].text_width))
				e_fix = info[i].text_x + info[i].text_width;

			for (int j = i+1; j < n_comments; j++)
			{
				if ((info[j].y > (info[i].y + max_text_height)) ||
				    (info[i].y > (info[j].y + max_text_height)))
				{
					continue;
				}

				int s_test = (info[j].begin < info[j].text_x ? info[j].begin : info[j].text_x);
				if (s_test < _left_space)
					s_test = _left_space;
				int e_test = info[j].end;
				if (e_test < (info[j].text_x + info[j].text_width))
					e_test = info[j].text_x + info[j].text_width;

				if (((s_test >= s_fix) && (s_test <= e_fix)) ||
					((e_test >= s_fix) && (e_test <= e_fix)) ||
					((s_test < s_fix) && (e_test > e_fix)))
				{
					info[j].y -= max_text_height + 4;
					again = true;
					break;
				}

			}
			if (again)
				break;
		}
	} while (again);
} // place_comments()


void
ts_view_general::options_changed()
{
	for (int i = 0; i < _num_ch; i++)
	{
		calc_scale(i);
		send_y_res(i);
	}

 	calc_max_scale_factors();

	get_ch_axis_div();

	_left_space = 0;
	if (_ts_opt.show_ch_info)
		_left_space += 40;
	if (_ts_opt.show_ch_scale)
		_left_space += 50;

	if (_ts_opt.show_status)
		_bottom_space = 20;
	else
		_bottom_space = 0;
} // options_changed()


void
ts_view_general::send_x_res(long parent)
{
	_block_signals = true;
	
	struct comm_para *para = new struct comm_para[2];
	para[0].type = PARA_DOUBLE;
	para[0].v.d = _ts_opt.mm_per_sec;
	para[1].type = PARA_LONG;
	para[1].v.l = parent;
	ra_comm_emit(_mh, "ts-view", "x-resolution", 2, para);
	delete[] para;
	
	_block_signals = false;
} // send_x_res()


void
ts_view_general::send_y_res(long ch)
{
	_block_signals = true;

	struct comm_para *para = new struct comm_para[2];
	para[0].type = PARA_DOUBLE;
	para[0].v.d = _ch[ch].mm_per_unit;
	para[1].type = PARA_LONG;
	para[1].v.l = _ch[ch].ch_number;
	ra_comm_emit(_mh, "ts-view", "y-resolution", 2, para);
	delete[] para;

	_block_signals = false;
	
} // send_y_res()


void
ts_view_general::send_input_focus_change()
{
	_block_signals = true;

	struct comm_para *para = new struct comm_para[1];
	para[0].type = PARA_LONG;
	para[0].v.l = (long)this;
	ra_comm_emit(_mh, "ts-view", "input-focus-change", 1, para);
	delete[] para;

	_block_signals = false;

	_has_input_focus = true;
} // send_input_focus_change()


int
ts_view_general::y_to_channel(int y)
{
	if (y >= (_screen_height - _bottom_space))
		return -1;

	y -= _top_space;
	_curr_channel = -1;
	for (int i = 0; i < _num_ch; i++)
	{
		if (!_ch[i].show_it)
			continue;
		
		if ((y > _ch[i].top_win_pos) && (y <= (_ch[i].top_win_pos + _ts_opt.ch_height)))
		{
			_curr_channel = i;
			break;
		}
	}

	return _curr_channel;
} // y_to_channel()


bool
ts_view_general::set_pos(long pos, double scale, bool from_signal /*=false*/ )
{
	long pos_save = _curr_pos;

	if (_curr_pos == pos)
		return false;

	if ((scale != 0.0) && (scale != _max_xscale))
	{
		double t = (double)pos * scale;
		t /= _max_xscale;
		_curr_pos = (long)t;
	}
	else
		_curr_pos = pos;

	check_curr_pos();

	// send position only if pos-change doesn't come from a signal
	if (!from_signal && (_curr_pos != pos_save))
		send_pos();

	if (pos_save != _curr_pos)
		_cursor_pos -= (su_to_screen(_curr_pos) - su_to_screen(pos_save));

	return (pos_save != _curr_pos);
} // set_pos()


void
ts_view_general::check_curr_pos()
{
	if (_curr_pos < 0)
		_curr_pos = 0;
	else if ((_curr_pos + _page_width) > _max_num_samples)
		_curr_pos = _max_num_samples - _page_width;

	if (_curr_pos < 0)
		_curr_pos = 0;
} // check_curr_pos()


void
ts_view_general::send_pos()
{
	set_scrollbar_pos(_curr_pos);
	
	_block_signals = true;

	struct comm_para *para = new struct comm_para[2];
	para[0].type = PARA_LONG;
	para[0].v.l = _curr_pos;
	para[1].type = PARA_DOUBLE;
	para[1].v.d = _max_xscale;
	ra_comm_emit(_mh, "ts-view", "pos-change", 2, para);
	delete[] para;

	_block_signals = false;
} // send_pos()


void
ts_view_general::send_init_scrollbar()
{
	_block_signals = true;

	struct comm_para *para = new struct comm_para[3];
	para[0].type = PARA_LONG;
	para[0].v.l = _page_width;
	para[1].type = PARA_LONG;
	para[1].v.l = _max_num_samples;
	para[2].type = PARA_DOUBLE;
	para[2].v.d = _max_xscale;
	ra_comm_emit(_mh, "ts-view", "init-scrollbar", 3, para);
	delete[] para;

	_block_signals = false;
} // send_init_scrollbar()


void
ts_view_general::send_eval_change()
{
	_block_signals = true;

	ra_comm_emit(_mh, "ts-view", "eval-change", 0, NULL);

	_block_signals = false;
} // send_eval_change()


void
ts_view_general::in_gui_event(bool in_ev)
{
	_in_gui_event = in_ev;

	if (in_ev)
		_already_changed = false;
} // in_gui_event()


int
ts_view_general::su_to_screen(long pos)
{
	long s = (long)((double)(_start_sec - _start_meas_sec) * _max_samplerate) + _curr_pos;

	int s_pos = (int)((double)(pos - s) * _max_xscale) + _left_space;
	
	return s_pos;
} // su_to_screen()


// check that x is inside the visible screen; if outside than return
// the x-value of the border; if inside than return just the x value
int
ts_view_general::check_x_pos(int x)
{
	int x_use = x - _left_space;
	if (x_use < 0)
		x_use = 0;
	if (x_use > (_screen_width - (_left_space + _right_space)))
		x_use = _screen_width - (_left_space + _right_space);
	
	return x_use;
} // check_x_pos()


int
ts_view_general::get_ch_index(long ch_number)
{
	int ch_idx = -1;

	for (int j = 0; j < _num_ch; j++)
	{
		if (_ch[j].ch_number == ch_number)
		{
			ch_idx = j;
			break;
		}
	}

	return ch_idx;
}  // get_ch_index()


void
ts_view_general::start_area_mark(int x)
{
	if (_area_mark_step == MARKING_AREA_MANUAL)
		return;

	_area_marked = false;
	_area_mark_step = MARKING_AREA_START;

	int x_use = check_x_pos(x);
	_area_start = _curr_pos + (long)((double)x_use / _max_xscale);
	_area_start += (long)((double)(_start_sec - _start_meas_sec) * _max_samplerate);

	_area_end = _area_start;
} // start_area_mark()


void
ts_view_general::change_area_mark(int x)
{
	if (_area_mark_step == NOT_MARKING_AREA)
		return;
	
	if (_area_mark_step != MARKING_AREA_MANUAL)
		_area_mark_step = MARKING_AREA;
	
	if (x > (_screen_width - _right_space))
	{
		next_page(false);
		update();
	}
	if (x < _left_space)
	{
		prev_page(false);
		update();
	}

	int x_use = check_x_pos(x);
	_area_end = _curr_pos + (long)((double)x_use / _max_xscale);
	if (_area_end > _max_num_samples)
		_area_end = _max_num_samples;
	// "correct" area-end here, because _max_num_samples are given for a session
	_area_end += (long)((double)(_start_sec - _start_meas_sec) * _max_samplerate);

	highlight_area();
} // change_area_mark()


void
ts_view_general::end_area_mark(int x)
{
	int x_use = check_x_pos(x);

	if (_area_mark_step == MARKING_AREA_START)
	{
		_area_marked = false;
		_area_mark_step = NOT_MARKING_AREA;
		_area_start = -1;
	}

	if ((_area_mark_step == MARKING_AREA) || (_area_mark_step == MARKING_AREA_MANUAL))
	{
		_area_mark_step = NOT_MARKING_AREA;
		_area_end = _curr_pos + (long)((double)x_use / _max_xscale);
		if (_area_end > _max_num_samples)
			_area_end = _max_num_samples;
		// "correct" area-end here, because _max_num_samples are given for a session
		_area_end += (long)((double)(_start_sec - _start_meas_sec) * _max_samplerate);
		_area_marked = true;

		if (_area_start > _area_end)
		{
			long t = _area_start;
			_area_start = _area_end + 1;
			_area_end = t + 1;
		}

		highlight_area();
	}
} // end_area_mark()


int
ts_view_general::find_aoi(int x, int y)
{
	if ((_aoi == NULL) || (_num_aoi == 0))
		return -1;

	int ret = -1;
	for (int i = 0; i < _num_aoi; i++)
	{
		if (_aoi[i].x == -1)
			continue;

		if ((x > _aoi[i].x) && (x < (_aoi[i].x + _aoi[i].w)) && (y < _aoi[i].y) && (y > (_aoi[i].y - _aoi[i].h)))
		{
			ret = i;
			break;
		}
	}

	return ret;
} // find_aoi()


void
ts_view_general::change_session(long num)
{
	if (num >= _num_sessions)
		return;

	_curr_session = num;
	_rh = ra_rec_get_first(_mh, _curr_session);

	if (ra_info_get(_rh, RA_INFO_REC_GEN_TIME_C, _vh) == 0)
	{
		char t[10];
		strncpy(t, ra_value_get_string(_vh), 9);
		t[2] = '\0';
		t[5] = '\0';
		_start_sec = atoi(t) * 3600 + atoi(t + 3) * 60 + atoi(t + 6);
	}
	else
		_start_sec = 0;

	update();
} // change_session()


long
ts_view_general::get_event_index(long id, long last_idx/* = 0*/)
{
	if (_event_ids == NULL)
		return -1;

	long idx = -1;
	for (long l = last_idx; l < _num_events; l++)
	{
		if (_event_ids[l] == id)
		{
			idx = l;
			break;
		}
	}

	return idx;
} // get_event_index()


void
ts_view_general::next_sort()
{
	if ((_num_sort <= 0) || !_sort || (_curr_sort >= (_num_sort - 1)))
		return;

	_curr_sort++;
	if (_sort_is_pos)
		set_pos(_sort[_curr_sort], _max_xscale);
	else
	{
		bool save = _update_cursor_pos;

		_update_cursor_pos = true;
		set_event(NULL, _sort[_curr_sort]);
		_update_cursor_pos = save;
	}
} // next_sort()


void
ts_view_general::prev_sort()
{
	if ((_num_sort <= 0) || !_sort || (_curr_sort <= 0))
		return;

	_curr_sort--;
	if (_sort_is_pos)
		set_pos(_sort[_curr_sort], _max_xscale);
	else
	{
		bool save = _update_cursor_pos;

		_update_cursor_pos = true;
		set_event(NULL, _sort[_curr_sort]);
		_update_cursor_pos = save;
	}
} // prev_sort()


void
ts_view_general::correct_sort(long idx, long offset)
{
	if ((_num_sort == 0) || ((offset != 1) && (offset != -1)) || (_sort_is_pos))
		return;

	/* If offset == -1 than a beat was deleted. If this beat is in
	   the sorting than remove beat from sorting.

	   If offset == 1 than a beat was inserted. The beat will be not
	   inserted in the sorting-array, only the indeces will be adjusted.
	   
	   (TODO: think about if, when inserting a beat, this beat can also
	   be inserted in the sorting-array.)
	*/
	long sort_change = 0;
	if (offset < 0)
	{
		for (long l = 0; l < _num_sort; l++)
		{
			if (_sort[l] == idx)
			{
				sort_change = -1;
				continue;
			}
			_sort[l] = _sort[l - sort_change];
		}
		if (sort_change != 0)
			_num_sort += sort_change;

		for (long l = 0; l < _num_sort; l++)
		{
			if (_sort[l] > idx)
				_sort[l] += offset;
		}
	}
	if (offset > 0)
	{
		for (long l = (_num_sort-1); l >= 0; l--)
		{
			if (_sort[l] >= idx)
				_sort[l] = _sort[l] + offset;
		}
	}

	if (_curr_sort >= _num_sort)
		_curr_sort = _num_sort - 1;
} // correct_sort()


void
ts_view_general::set_event(class_handle clh, long event, bool from_signal /* = false */ )
{
	long dummy, pos;

	if (clh && (clh != _clh))
		return;  // do nothing if current event is set to nothing

	pos = 0;
	if (_prop_pos)
	{
		ra_prop_get_value(_prop_pos, event, -1, _vh);
		pos = ra_value_get_long(_vh);
		_ev_idx = get_event_index(event);
	}
	else
	{
		// TODO: check for what this code is needed
		if ((event == -1) || (_clh == NULL))
			return;

		if (event == _event_ids[_ev_idx])
			return;

		_ev_idx = get_event_index(event);

		ra_class_get_event_pos(_clh, event, &pos, &dummy);
	}

	if ((pos < _curr_pos) || (pos > (_curr_pos + _page_width)))
		set_pos((pos - (_page_width / 2)), _max_xscale, from_signal);

	if (_update_cursor_pos)
		_cursor_pos = (int)((double)(pos - _curr_pos) * _max_xscale) + _left_space;

	if (!from_signal)
		send_event();
} // set_event()


void
ts_view_general::send_event()
{
	if (_ev_idx == _ev_idx_save)
		return;  // position already handled

	_ev_idx_save = _ev_idx;

	struct comm_para *para = new struct comm_para[2];
	para[0].type = PARA_VOID_POINTER;
	para[0].v.pv = _clh;
	para[1].type = PARA_LONG;
	para[1].v.l = _event_ids[_ev_idx];

	_block_signals = true;
	ra_comm_emit(_mh, _plugin_name, "highlight-event", 2, para);
	_block_signals = false;

	delete[]para;
} // send_event()


void
ts_view_general::send_add_event(long event)
{
	struct comm_para *para = new struct comm_para[2];
	para[0].type = PARA_VOID_POINTER;
	para[0].v.pv = _clh;
	para[1].type = PARA_LONG;
	para[1].v.l = event;

	_block_signals = true;
	ra_comm_emit(_mh, _plugin_name, "add-event", 2, para);
	_block_signals = false;

	delete[]para;
} // send_add_event()


void
ts_view_general::sort_annotations()
{
	if (_num_annot <= 0)
		return;
	
	if (_sort)
		delete[] _sort;
	_sort = new long[_num_sort];
	_num_sort = _num_annot;
	
	for (long l = 0; l < _num_annot; l++)
		_sort[l] = _annot[l].start;
	
	if (_num_sort > 0)
	{
		_curr_sort = -1;
		next_sort();
	}
} // sort_annotations()


void
ts_view_general::remove_events_in_area()
{
	if (!_area_marked || !_clh || !_prop_pos)
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

	for (long l = 0; l < n; l++)
        ra_class_del_event(_clh, ev[l]);
	delete[] ev;

	// tell others that something was changed
	_block_signals = true;
	ra_comm_emit(_mh, "ts-view", "eval-change", 0, NULL);
	_block_signals = false;

	unset_cursor();

	update();
} // remove_events_in_area()


void
ts_view_general::save_settings(const char *file)
{
	/* first check if settings-file is available */
	bool settings_file_avail = false;
	FILE *fp = fopen(file, "r");
	if (fp != NULL)
	{
		settings_file_avail = true;
		fclose(fp);
	}

	char ch_node_id[200];
	sprintf(ch_node_id, "ts-view-settings_for_%d_channels_1st_channel_is_%s",
		_num_ch, _ch[0].name);

	xmlDocPtr doc;
	xmlNodePtr prev_settings = NULL;
	xmlNodePtr node;
	if (settings_file_avail)
	{
		if ((doc = xmlParseFile(file)) == NULL)
		{
			_ra_set_error(_mh, RA_ERR_READ_CONFIG, "ts-view-plugin");
			return;
		}

		node = doc->children;
		while (node)
		{
			if (strcmp((const char *)node->name, "libRASCH-channel-settings") == 0)
				break;
			node = node->next;
		}

		if (node)
		{
			prev_settings = node->children;
			while (prev_settings)
			{		
				if (strcmp((const char *)prev_settings->name, ch_node_id) == 0)
					break;
				prev_settings = prev_settings->next;
			}
		}
		else
		{
			node = xmlNewDocNode(doc, NULL, (xmlChar *)"libRASCH-channel-settings", NULL);
		}
	}
	else
	{
		doc = xmlNewDoc((xmlChar *)"1.0");
		doc->children = xmlNewDocNode(doc, NULL, (xmlChar *)"libRASCH-channel-settings", NULL);
		node = doc->children;
	}

	xmlNodePtr new_settings = xmlNewChild(node, NULL, (xmlChar *)ch_node_id, NULL);
	save_general_settings(new_settings);

	for (int i = 0; i < _num_ch; i++)
		save_single_ch_settings(i, new_settings);

	if (prev_settings)
		xmlReplaceNode(prev_settings, new_settings);

	if (xmlSaveFileEnc(file, doc, "UTF-8") == -1)
		_ra_set_error(_mh, RA_ERR_SAVE_FILE, "ts-view-plugin");

	xmlFreeDoc(doc);
} // save_settings()


int
ts_view_general::save_general_settings(xmlNodePtr parent)
{
	xmlNodePtr node = xmlNewChild(parent, NULL, (xmlChar *)"general-settings", NULL);

	char s[50];
	sprintf(s, "%f", _ts_opt.mm_per_sec);
	xmlSetProp(node, (xmlChar *)"mm-sec", (xmlChar *)s);
	xmlSetProp(node, (xmlChar *)"show-status", (xmlChar *)(_ts_opt.show_status ? "1" : "0"));
	xmlSetProp(node, (xmlChar *)"show-ch-info", (xmlChar *)(_ts_opt.show_ch_info ? "1" : "0"));
	xmlSetProp(node, (xmlChar *)"show-ch-scale", (xmlChar *)(_ts_opt.show_ch_scale ? "1" : "0"));
	xmlSetProp(node, (xmlChar *)"plot-grid", (xmlChar *)(_ts_opt.plot_grid ? "1" : "0"));
	xmlSetProp(node, (xmlChar *)"show-annot", (xmlChar *)(_ts_opt.show_annot ? "1" : "0"));
	xmlSetProp(node, (xmlChar *)"ch-height-dynamic", (xmlChar *)(_ts_opt.ch_height_dynamic ? "1" : "0"));
	sprintf(s, "%d", _ts_opt.ch_height);
	xmlSetProp(node, (xmlChar *)"ch-height", (xmlChar *)s);

	return 0;
}  // save_general_settings()


int
ts_view_general::save_single_ch_settings(int ch, xmlNodePtr parent)
{
	xmlNodePtr node = xmlNewChild(parent, NULL, (xmlChar *)"channel-settings", NULL);

	xmlSetProp(node, (xmlChar *)"name", (xmlChar *)_ch[ch].name);
	xmlSetProp(node, (xmlChar *)"show-it", (xmlChar *)(_ch[ch].show_it ? "1" : "0"));
	xmlSetProp(node, (xmlChar *)"centered", (xmlChar *)(_ch[ch].centered ? "1" : "0"));
	char s[50];
	sprintf(s, "%d", _ch[ch].center_pos);
	xmlSetProp(node, (xmlChar *)"center-pos", (xmlChar *)s);
	xmlSetProp(node, (xmlChar *)"inverse", (xmlChar *)(_ch[ch].inverse ? "1" : "0"));
	sprintf(s, "%f", _ch[ch].min_value);
	xmlSetProp(node, (xmlChar *)"min-value", (xmlChar *)s);
	sprintf(s, "%f", _ch[ch].max_value);
	xmlSetProp(node, (xmlChar *)"max-value", (xmlChar *)s);
	xmlSetProp(node, (xmlChar *)"use-mm-per-unit", (xmlChar *)(_ch[ch].use_mm_per_unit ? "1" : "0"));
	sprintf(s, "%f", _ch[ch].mm_per_unit);
	xmlSetProp(node, (xmlChar *)"mm-per-unit", (xmlChar *)s);

	return 0;
} // save_single_ch_settings()


void
ts_view_general::load_settings(const char *file)
{
	/* first check if settings-file is available */
	FILE *fp = fopen(file, "r");
	if (fp == NULL)
		return;
	fclose(fp);

	xmlDocPtr doc;
	if ((doc = xmlParseFile(file)) == NULL)
	{
		_ra_set_error(_mh, RA_ERR_READ_CONFIG, "ts-view-plugin");
		return;
	}

	xmlNodePtr node = doc->children;
	while (node)
	{
		if (strcmp((const char *)node->name, "libRASCH-channel-settings") == 0)
			break;
		node = node->next;
	}
	if (!node)
	{
		xmlFreeDoc(doc);
		return;
	}

	char ch_node_id[200];
	sprintf(ch_node_id, "ts-view-settings_for_%d_channels_1st_channel_is_%s",
		_num_ch, _ch[0].name);

	xmlNodePtr settings = node->children;
	while (settings)
	{		
		if (strcmp((const char *)settings->name, ch_node_id) == 0)
			break;
		settings = settings->next;
	}
	if (!settings)
		return;

	xmlNodePtr child = settings->children;
	while (child)
	{
		if (strcmp((const char *)child->name, "general-settings") == 0)
			load_general_settings(child);
		else
			load_single_ch_settings(child);

		child = child->next;
	}

	xmlFreeDoc(doc);

	options_changed();

	update();
} // load_settings()


int
ts_view_general::load_general_settings(xmlNodePtr node)
{
	read_xml_prop_double(node, "mm-sec", _ts_opt.mm_per_sec);
	read_xml_prop_bool(node, "show-status", _ts_opt.show_status);
	read_xml_prop_bool(node, "show-ch-info", _ts_opt.show_ch_info);
	read_xml_prop_bool(node, "show-ch-scale", _ts_opt.show_ch_scale);
	read_xml_prop_bool(node, "plot-grid", _ts_opt.plot_grid);
	read_xml_prop_bool(node, "show-annot", _ts_opt.show_annot);
	read_xml_prop_bool(node, "ch-height-dynamic", _ts_opt.ch_height_dynamic);
	read_xml_prop_int(node, "ch-height", _ts_opt.ch_height);

	return 0;
} // load_general_settings()


int
ts_view_general::load_single_ch_settings(xmlNodePtr node)
{
	xmlChar *c = xmlGetProp(node, (const xmlChar *)"name");
	if (!c)
		return -1;

	for (int i = 0; i < _num_ch; i++)
	{
		if (strcmp(_ch[i].name, (const char *)c) == 0)
		{
			read_xml_prop_bool(node, "show-it", _ch[i].show_it);
			read_xml_prop_bool(node, "centered", _ch[i].centered);
			read_xml_prop_int(node, "center-pos", _ch[i].center_pos);
			read_xml_prop_bool(node, "inverse", _ch[i].inverse);
			read_xml_prop_double(node, "min-value", _ch[i].min_value);
			read_xml_prop_double(node, "max-value", _ch[i].max_value);
			read_xml_prop_bool(node, "use-mm-per-unit", _ch[i].use_mm_per_unit);
			read_xml_prop_double(node, "mm-per-unit", _ch[i].mm_per_unit);

			break;
		}
	}
	xmlFree(c);

	return 0;
} // load_single_ch_settings()


void
ts_view_general::read_xml_prop_bool(xmlNodePtr node, const char *prop, bool &value)
{
	xmlChar *c = xmlGetProp(node, (const xmlChar *)prop);
	if (c)
	{
		if (strcmp((const char *)c, "1") == 0)
			value = true;
		else
			value = false;
		xmlFree(c);
	}
} // read_xml_prop_bool()


void
ts_view_general::read_xml_prop_int(xmlNodePtr node, const char *prop, int &value)
{
	xmlChar *c = xmlGetProp(node, (const xmlChar *)prop);
	if (c)
	{
		value = atoi((const char *)c);
		xmlFree(c);
	}
} // read_xml_prop_int()


void
ts_view_general::read_xml_prop_double(xmlNodePtr node, const char *prop, double &value)
{
	xmlChar *c = xmlGetProp(node, (const xmlChar *)prop);
	if (c)
	{
		value = atof((const char *)c);
		xmlFree(c);
	}
} // read_xml_prop_double()


/**
 * \brief returns filename used for storage of channel settings
 * \param <buf> will receive the filename
 * \param <buf_size> size of parameter 'buf'
 *
 * This function returns the default filename used for storing
 * channel settings; it is based on the measurement name.
 */
int
ts_view_general::get_ch_settings_file(char *buf, int buf_size)
{
	long dir = 0;
	value_handle vh;
	rec_handle rh;

	rh = ra_rec_get_first(_mh, 0);
	vh = ra_value_malloc();
	if (ra_info_get(rh, RA_INFO_REC_GEN_DIR_L, vh) != 0)
	{
		ra_value_free(vh);
		return -1;
	}
	dir = ra_value_get_long(vh);

	if (ra_info_get(rh, RA_INFO_REC_GEN_PATH_C, vh) != 0)
	{
		ra_value_free(vh);
		return -1;
	}

	if (dir)
	{
#ifdef WIN32
		_snprintf(buf, buf_size, "%s%cch_settings.raset", ra_value_get_string(vh), SEPARATOR);
#else
		snprintf(buf, buf_size, "%s%cch_settings.raset", ra_value_get_string(vh), SEPARATOR);
#endif
	}
	else
	{
#ifdef WIN32
		_snprintf(buf, buf_size, "%s.raset", ra_value_get_string(vh));
#else
		snprintf(buf, buf_size, "%s.raset", ra_value_get_string(vh));
#endif
	}

	ra_value_free(vh);

	return 0;
} /* get_ch_settings_file() */


int
ts_view_general::get_ch_axis_div()
{
	for (int i = 0; i < _num_ch; i++)
	{
		if (_ch[i].centered)
		{
			_ch[i].scale_start = ((double)_ts_opt.ch_height * ((double)(-_ch[i].center_pos) / 100.0)) / _ch[i].yscale;

			_ch[i].scale_end = draw_calc_max_value(i);
		}
		else
		{
			_ch[i].scale_start = _ch[i].min_value;

			if (!_ch[i].use_mm_per_unit)
				_ch[i].scale_end = _ch[i].max_value;
			else
			{
				_ch[i].scale_end = draw_calc_max_value(i);
				_ch[i].scale_end += _ch[i].scale_start;
			}
		}

		double range = _ch[i].scale_end - _ch[i].scale_start;
		_ch[i].scale_step = get_step(range);
		//_ch[i].scale_start = floor(_ch[i].scale_start / _ch[i].scale_step) * _ch[i].scale_step;
		//_ch[i].scale_end = ceil(_ch[i].scale_end / _ch[i].scale_step) * _ch[i].scale_step;
	}

	return 0;
} // get_ch_axis_div()


double
ts_view_general::get_step(double range)
{
	if (range <= 0.0)
		return range;

	double abs_range = fabs(range);
	double log_range = log10(abs_range);
	double norm = pow(10.0, log_range - (log_range >= 0.0 ? log_range : (log_range - 1.0)));
	double div;
	if (norm <= 2.0)
		div = 0.2;
	else if (norm <= 5.0)
		div = 0.5;
	else
		div = 1.0;

	double step = div * raise(10.0, (int) (log_range >= 0.0 ? log_range : (log_range - 1.0)));
	if (step <= 1.0)
		return range;

	double n = abs_range / step;
	while (n > 10)
	{
		step *= 2;
		n = abs_range / step;
	}

	return step;
} // get_step()


double
ts_view_general::raise(double x, int y)
{
	if (y <= 0)
		return 0;

	double val = 1.0;
	for (int i = 0; i < abs(y); i++)
		val *= x;
	if (y < 0)
		return (1.0 / val);
	else
		return val;
} // raise()


int
ts_view_general::get_tics_precision(double step)
{
	if (step >= 1)
		return 0;

	int l = (int) (log10(step) - 1);
	l = abs(l);

	return l;
} // get_tics_precision()


void
ts_view_general::auto_adjust_y_axis(int y)
{
	int ch = y_to_channel(y);
	if (ch < 0)
		return;

	long n = (long) ((double) _page_width * _ch[ch].pos_factor);
	long pos = (long) ((double) _curr_pos * _ch[ch].pos_factor);

	if (n <= 0)
		return;

	double *data = new double[n];
	n = ra_raw_get_unit(_rh, _ch[ch].ch_number, pos, n, data);
	if (n <= 0)
	{
		delete[] data;
		return;
	}

	double min = data[0];
	double max = data[0];
	for (int i = 1; i < n; i++)
	{
		if (data[i] < min)
			min = data[i];
		if (data[i] > max)
			max = data[i];
	}
	delete[] data;

	double range = max - min;
	_ch[ch].scale_step = get_step(range);
	_ch[ch].scale_start = floor(min / _ch[ch].scale_step) * _ch[ch].scale_step;
	_ch[ch].scale_end = ceil(max / _ch[ch].scale_step) * _ch[ch].scale_step;

	_ch[ch].centered = false;
	_ch[ch].use_mm_per_unit = false;
	// if channel is inverse swap min- and max-values
	if (_ch[ch].inverse)
	{
		_ch[ch].max_value = _ch[ch].scale_start;
		_ch[ch].min_value = _ch[ch].scale_end;
	}
	else
	{
		_ch[ch].max_value = _ch[ch].scale_end;
		_ch[ch].min_value = _ch[ch].scale_start;
	}

	options_changed();
	update();
}  // auto_adjust_y_axis()

