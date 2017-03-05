/*----------------------------------------------------------------------------
 * ev_summary_view_general.cpp
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

#include <ra_defines.h>
#include <ra_pl_comm.h>

#include "ev_summary_view_general.h"


ev_summary_view_general::ev_summary_view_general(meas_handle mh, sum_handle sh)
{
	_block_signals = false;

	_num_dim = 0;
	_dim_name = NULL;
	_dim_unit = NULL;

	_num_ch = 0;
	_ch = NULL;
	_ch_number = NULL;
	_fiducial_offset = NULL;

	_num_sum_part = 0;
	_sum_part = 0;
	_part_order = NULL;

	_curr_sum_part_idx = -1;
	_curr_sum_part_id = -1;

	_mm_per_sec = 1.0;
	_max_part_width = -1;

	_pixmm_v = 0.0;
	_pixmm_h = 0.0;

	_mh = mh;
	_sh = sh;
} // constructor


ev_summary_view_general::~ev_summary_view_general()
{
	while (_num_sum_part > 0)
		delete_part(_sum_part[0].part_id, false, false);

	for (long l = 0; l < _num_dim; l++)
	{
		if (_dim_name && _dim_name[l])
			delete[] _dim_name[l];
		if (_dim_unit && _dim_unit[l])
			delete[] _dim_unit[l];
	}
	if (_dim_name)
		delete[] _dim_name;
	_dim_name = NULL;
	if (_dim_unit)
		delete[] _dim_unit;
	_dim_unit = NULL;

	if (_ch_number)
		delete[] _ch_number;
	_ch_number = NULL;
	if (_fiducial_offset)
		delete[] _fiducial_offset;
	_fiducial_offset = NULL;
} // destructor


void
ev_summary_view_general::delete_part(long part_id, bool del_in_eval, bool del_events)
{
	long l;

	if (_num_sum_part <= 0)
		return;

	long idx = get_summary_part_index(part_id);
	if (idx < 0)
		return;

	if (del_events)
	{
		class_handle clh = ra_class_get_handle(_sh);
		if (clh == NULL)
			return;

		for (l = 0; l < _sum_part[idx].num_events; l++)
			ra_class_del_event(clh, _sum_part[idx].ev_ids[l]);

	}

	if (del_in_eval)
		ra_sum_del_part(_sh, part_id);

	for (l = 0; l < _num_ch; l++)
	{
		for (long m = 0; m < _num_dim; m++)
		{
			delete[] _sum_part[idx].raw_orig[l][m];
			delete[] _sum_part[idx].raw[l][m];
		}
		delete[] _sum_part[idx].num_data[l];
		delete[] _sum_part[idx].raw_orig[l];
		delete[] _sum_part[idx].raw[l];
	}
	delete[] _sum_part[idx].num_data;
	delete[] _sum_part[idx].raw_orig;
	delete[] _sum_part[idx].raw;

	delete[] _sum_part[idx].ev_ids;

	if (_num_sum_part == 1)
	{
		_num_sum_part = 0;
		delete[] _sum_part;
		delete[] _part_order;
		return;
	}

	long curr = 0;
	struct summary_part_data *t = new struct summary_part_data[_num_sum_part-1];
	for (l = 0; l < _num_sum_part; l++)
	{
		if (l == idx)
			continue;

		t[curr++] = _sum_part[l];
	}
	delete[] _sum_part;
	_sum_part = t;
	_num_sum_part--;

	sort_ev_summaries();

	if (del_events || del_in_eval)
	{
		_block_signals = true;
		ra_comm_emit(_mh, "ev-summary-view", "eval-change", 0, NULL);
		_block_signals = false;
	}

} // delete_part()


void
ev_summary_view_general::update()
{
	// do nothing
} // update()


bool
ev_summary_view_general::init()
{
	bool ok;
	if (!(ok = get_summary_infos()))
		return ok;

	sort_ev_summaries();

	scaling_info();
	scale_raw_data();

	get_max_part_width();

	return ok;
} // init


bool
ev_summary_view_general::get_summary_infos()
{
	long l;

	value_handle vh = ra_value_malloc();

	if (ra_info_get(_sh, RA_INFO_SUM_NUM_DIM_L, vh) != 0)
	{
		ra_value_free(vh);
		return false;
	}
	_num_dim = ra_value_get_long(vh);
	if (_num_dim <= 0)
	{
		ra_value_free(vh);
		return false;
	}
	_dim_name = new char *[_num_dim];
	_dim_unit = new char *[_num_dim];

	for (l = 0; l < _num_dim; l++)
	{
		ra_value_set_number(vh, l);

		if (ra_info_get(_sh, RA_INFO_SUM_DIM_NAME_C, vh) != 0)
		{
			ra_value_free(vh);
			return false;
		}
		_dim_name[l] = new char[strlen(ra_value_get_string(vh)) + 1];
		strcpy(_dim_name[l], ra_value_get_string(vh));

		if (ra_info_get(_sh, RA_INFO_SUM_DIM_UNIT_C, vh) != 0)
		{
			ra_value_free(vh);
			return false;
		}
		_dim_unit[l] = new char[strlen(ra_value_get_string(vh)) + 1];
		strcpy(_dim_unit[l], ra_value_get_string(vh));
	}

	if (ra_info_get(_sh, RA_INFO_SUM_NUM_CH_L, vh) != 0)
	{
		ra_value_free(vh);
		return false;
	}
	_num_ch = ra_value_get_long(vh);
	if (_num_ch <= 0)
	{
		ra_value_free(vh);
		return false;
	}
	_ch_number = new long[_num_ch];
	_fiducial_offset = new long[_num_ch];

	for (l = 0; l < _num_ch; l++)
	{
		ra_value_set_number(vh, l);

		if (ra_info_get(_sh, RA_INFO_SUM_CH_NUM_L, vh) != 0)
		{
			ra_value_free(vh);
			return false;
		}
		_ch_number[l] = ra_value_get_long(vh);

		if (ra_info_get(_sh, RA_INFO_SUM_CH_FIDUCIAL_L, vh) != 0)
		{
			ra_value_free(vh);
			return false;
		}
		_fiducial_offset[l] = ra_value_get_long(vh);
	}

	if (ra_sum_get_part_all(_sh, vh) != 0)
	{
		ra_value_free(vh);
		return false;
	}
	_num_sum_part = ra_value_get_num_elem(vh);
	if (_num_sum_part <= 0)
	{
		ra_value_free(vh);
		return false;
	}
	_sum_part = new struct summary_part_data[_num_sum_part];
	const long *part_id = ra_value_get_long_array(vh);

	for (l = 0; l < _num_sum_part; l++)
		get_summary_part(part_id[l], _sum_part + l);

	return true;
} // get_summary_infos()


bool
ev_summary_view_general::get_summary_part(long part_id, struct summary_part_data *part_data)
{
	long l;
	value_handle vh;
	if (!(vh = ra_value_malloc()))
		return false;

	part_data->part_id = part_id;

	if (ra_sum_get_part_events(_sh, part_id, vh) != 0)
	{
		ra_value_free(vh);
		return false;
	}
	part_data->num_events = ra_value_get_num_elem(vh);
	if (part_data->num_events <= 0)
	{
		ra_value_free(vh);
		return false;
	}
	
	part_data->ev_ids = new long[part_data->num_events];
	const long *ev_ids = ra_value_get_long_array(vh);
	for (l = 0; l < part_data->num_events; l++)
		part_data->ev_ids[l] = ev_ids[l];
	
	part_data->num_data = new long *[_num_ch];
	part_data->raw_orig = new double **[_num_ch];
	part_data->raw = new double **[_num_ch];
	for (l = 0; l < _num_ch; l++)
	{
		part_data->num_data[l] = new long[_num_dim];
		part_data->raw_orig[l] = new double *[_num_dim];
		part_data->raw[l] = new double *[_num_dim];

		for (long m = 0; m < _num_dim; m++)
		{
			ra_sum_get_part_data(_sh, part_id, l, m, vh);
			long num = ra_value_get_num_elem(vh);
			part_data->num_data[l][m] = num;
			part_data->raw_orig[l][m] = new double[num];
			part_data->raw[l][m] = new double[num];

			const double *data = ra_value_get_double_array(vh);
			for (long n = 0; n < num; n++)
			{
				part_data->raw_orig[l][m][n] = data[n];
				part_data->raw[l][m][n] = data[n];
			}
		}
	}
	
	return true;
} // get_summary_part()


long
ev_summary_view_general::get_summary_part_index(long part_id)
{
	long idx = -1;

	for (long l = 0; l < _num_sum_part; l++)
	{
		if (_sum_part[l].part_id == part_id)
		{
			idx = l;
			break;
		}
	}

	return idx;
} // get_summary_part_index()


void
ev_summary_view_general::sort_ev_summaries()
{
	if (_part_order)
	{
		delete[] _part_order;
		_part_order = NULL;
	}

	if ((_num_sum_part <= 0) || (_sum_part == NULL))
		return;

	_part_order = new long[_num_sum_part];
	int *used = new int[_num_sum_part];
	memset(used, 0, sizeof(int) * _num_sum_part);

	for (long l = 0; l < _num_sum_part; l++)
	{
		long max = -1;
		long idx = -1;
		for (long m = 0; m < _num_sum_part; m++)
		{
			if (used[m])
				continue;
			if (max < _sum_part[m].num_events)
			{
				max = _sum_part[m].num_events;
				idx = m;
			}
		}
		used[idx] = 1;
		_part_order[l] = idx;
	}
	delete[] used;
} // sort_ev_summaries()


void
ev_summary_view_general::scaling_info()
{
	// if mm/sec is valid value and ch-info is not NULL, do not get new values
	// but use old values
	if ((_mm_per_sec != -1) && (_ch != NULL))
		return;

	rec_handle rh = ra_rec_get_first(_mh, 0);
	value_handle vh = ra_value_malloc();
	if (ra_info_get(rh, RA_INFO_REC_GEN_MM_SEC_D, vh) == 0)
		_mm_per_sec = ra_value_get_double(vh);
	else
		_mm_per_sec = 1.0;
	ra_value_free(vh);

	if (_ch != NULL)
	{
		delete[]_ch;
		_ch = NULL;
	}
	if (_num_ch > 0)
	{
		_ch = new struct ch_info[_num_ch];
		memset(_ch, 0, sizeof(struct ch_info) * _num_ch);
		for (long l = 0; l < _num_ch; l++)
			get_ch_info(rh, l);
	}
} // scaling_info()


void
ev_summary_view_general::get_ch_info(rec_handle rh, int ch)
{
	value_handle vh = ra_value_malloc();
	ra_value_set_number(vh, _ch_number[ch]);
	if (ra_info_get(rh, RA_INFO_REC_CH_CENTER_VALUE_D, vh) == 0)
	{
		_ch[ch].centered = true;
		_ch[ch].center_value = ra_value_get_double(vh);
		ra_info_get(rh, RA_INFO_REC_CH_CENTER_SAMPLE_D, vh);
		_ch[ch].sample_offset = ra_value_get_double(vh);
		_ch[ch].center_pos = 50;
		_ch[ch].inverse = false;
	}
	else
	{
		_ch[ch].centered = false;
		ra_info_get(rh, RA_INFO_REC_CH_MIN_UNIT_D, vh);
		_ch[ch].min_value = ra_value_get_double(vh);
		ra_info_get(rh, RA_INFO_REC_CH_MAX_UNIT_D, vh);
		_ch[ch].max_value = ra_value_get_double(vh);
		_ch[ch].use_mm_per_unit = true;
	}

	if (ra_info_get(rh, RA_INFO_REC_CH_MM_UNIT_D, vh) == 0)
		_ch[ch].mm_per_unit = ra_value_get_double(vh);
	else
		_ch[ch].mm_per_unit = 1.0;

	ra_info_get(rh, RA_INFO_REC_CH_SAMPLERATE_D, vh);
	_ch[ch].samplerate = ra_value_get_double(vh);
	ra_info_get(rh, RA_INFO_REC_CH_AMP_RESOLUTION_D, vh);
	_ch[ch].amp_res = ra_value_get_double(vh);
	ra_info_get(rh, RA_INFO_REC_CH_TYPE_L, vh);
	_ch[ch].type = ra_value_get_long(vh);

	ra_value_free(vh);

	calc_scale(ch);
} // get_ch_info()


void
ev_summary_view_general::calc_scale(int ch)
{
	// calculation of x-scaling-factor
	// 1. speed[mm/sec] / samplerate[sample/sec] = foo[mm/sample]
	// 2. foo[mm/sample] * res[pix/mm] = scale[pix/sample]
	if (_ch[ch].samplerate == 0)
		_ch[ch].xscale = 1.0;
	else
	{
		double foo = _mm_per_sec / _ch[ch].samplerate;
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
} // calc_scale()

void
ev_summary_view_general::scale_raw_data()
{
	for (long l = 0; l < _num_sum_part; l++)
	{
		for (long m = 0; m < _num_ch; m++)
		{
			for (long n = 0; n < _num_dim; n++)
			{
				double min, max, range, scale;
				min = max = _sum_part[l].raw_orig[m][n][0];
				for (long i = 1; i < _sum_part[l].num_data[m][n]; i++)
				{
					if (_sum_part[l].raw_orig[m][n][i] < min)
						min = _sum_part[l].raw_orig[m][n][i];
					if (_sum_part[l].raw_orig[m][n][i] > max)
						max = _sum_part[l].raw_orig[m][n][i];
				}
				range = max - min;
				scale = (double)EV_SUMMARY_HEIGHT / range;

				for (long i = 0; i < _sum_part[l].num_data[m][n]; i++)
				{
					_sum_part[l].raw[m][n][i] = (max  - _sum_part[l].raw_orig[m][n][i]) * scale;

					// scaling of values by using ch-settings from signal-type 
					//double v = scale_amplitude(max, _sum_part[l].raw_orig[m][n][i], n);
					//_sum_part[l].raw[m][n][i] = v;
				}
			}
		}
	}
} // scale_raw_data()


double
ev_summary_view_general::max_value(int ch)
{
	double max;

	if (_ch[ch].centered)
		max = ((double) EV_SUMMARY_HEIGHT * (double) (100 - _ch[ch].center_pos) / 100) / _ch[ch].yscale;
	else
		max = (double) EV_SUMMARY_HEIGHT / _ch[ch].yscale;

	return max;
} // draw_calc_max_value()


inline double
ev_summary_view_general::scale_amplitude(double max, double val, int ch)
{
	double amp = 0.0;

	if (_ch[ch].centered)
	{
		if (_ch[ch].inverse)
			amp = (max - (-(val - _ch[ch].center_value))) * _ch[ch].yscale;
		else
			amp = (max - (val - _ch[ch].center_value)) * _ch[ch].yscale;
	}
	else
	{
		if (_ch[ch].use_mm_per_unit)
			amp = (max - (val - _ch[ch].min_value)) * _ch[ch].yscale;
		else
		{
			double range = _ch[ch].max_value - _ch[ch].min_value;
			double scale = (double) EV_SUMMARY_HEIGHT / range;
			double v = _ch[ch].max_value - val;  // * _ch[ch].amp_res);
			amp = v * scale;
		}
	}

	return amp;
} // scale_amplitude (inline)


double
ev_summary_view_general::mean(double *v, long len)
{
	double m = 0.0;
	for (long l = 0; l < len; l++)
		m += v[l];
	m /= (double)len;

	return m;
} // mean()


void
ev_summary_view_general::get_max_part_width()
{
	// get max ev_summary-width
	_max_part_width = 0;

	for (long l = 0; l < _num_sum_part; l++)
	{
		for (long m = 0; m < _num_ch; m++)
		{
			for (long n = 0; n < _num_dim; n++)
			{
				long w = (long)((double)(_sum_part[l].num_data[m][n]) * _ch[m].xscale);
				if (w > _max_part_width)
				{
					_max_part_width = w;
					_max_part_width_su = _sum_part[l].num_data[m][n];
				}
			}
		}
	}
} // get_max_part_width()


void
ev_summary_view_general::ev_summary_select()
{
	long n;
	struct comm_para *para = new struct comm_para[3];

	para[0].type = PARA_VOID_POINTER;
	para[0].v.pv = ra_class_get_handle(_sh);
	
	para[1].type = PARA_LONG;
	n = _sum_part[_part_order[_curr_sum_part_idx]].num_events;
	para[1].v.l = n;

	para[2].type = PARA_LONG_POINTER;
	para[2].v.pl = new long[n];
	memcpy(para[2].v.pl, _sum_part[_part_order[_curr_sum_part_idx]].ev_ids, sizeof(long) * n);

	_block_signals = true;
	ra_comm_emit(_mh, "ev-summary-view", "select-events", 3, para);
	_block_signals = false;

	delete[]para[2].v.pl;
	delete[]para;
} // ev_summary_select()


bool
ev_summary_view_general::key_press(char c)
{
	bool do_repaint = false;
	switch (c)
	{
	case 'D':
		if (_curr_sum_part_id >= 0)
		{
        	set_busy_cursor();

			do_repaint = true;
			delete_part(_curr_sum_part_id, true, true);
			if (_curr_sum_part_idx >= _num_sum_part)
				_curr_sum_part_idx--;
			if (_curr_sum_part_idx >= 0)
			{
				// it may happen that after the delete the running number of
				// deleted one and the next one is the same which results
				// in a no-change of the views; therefore "unset" it first
				_curr_sum_part_id = -1;
				ev_summary_select();
				_curr_sum_part_id = _sum_part[_part_order[_curr_sum_part_idx]].part_id;
				ev_summary_select();
			}

            unset_cursor();
		}
		break;
	}

	return do_repaint;
} // key_press()


void
ev_summary_view_general::set_x_res(double x_res)
{
	if (x_res == _mm_per_sec)
		return;

	_mm_per_sec = x_res;
	for (long l = 0; l < _num_ch; l++)
		calc_scale(l);

	get_max_part_width();
	update();
} // set_x_res()


void
ev_summary_view_general::set_ch_y_res(double mm_per_unit, long ch)
{
	long ch_use = -1;
	for (long l = 0; l < _num_ch; l++)
	{
		if (_ch_number[l] == ch)
		{
			ch_use = l;
			break;
		}
	}
	if (ch_use == -1)
		return;

	if (_ch[ch_use].mm_per_unit == mm_per_unit)
		return;

	_ch[ch_use].mm_per_unit = mm_per_unit;
	calc_scale(ch_use);
	scale_raw_data();

	update();
} // set_ch_y_res()


void
ev_summary_view_general::get_area_to_be_visible(int *x_start, int *x_end)
{
	_curr_sum_part_id = _sum_part[_part_order[_curr_sum_part_idx]].part_id;

	int part_width = _max_part_width + EV_SUMMARY_DIST;
	int x = _curr_sum_part_idx * part_width + X_OFFSET;

	*x_start = x - X_OFFSET;
	*x_end = *x_start + part_width;
} // get_area_to_be_visible()
