/*----------------------------------------------------------------------------
 * ra_plot_general.cpp
 *
 *
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002-2005, Raphael Schneider
 * See the file COYPING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifdef _AFXDLL			// so mfc-libs are linked before clib-libs
#include "stdafx.h"
#endif // _AFXDLL

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "ra_plot_general.h"


ra_plot_general::ra_plot_general(meas_handle h, struct ra_plot_options *opt)
{
	_meas = h;
	_block_signals = false;
	_opt = opt;
	_pos_idx = 0;

	_plot_area_top = 10;
	_plot_area_left = 10;
	_plot_area_right = 20;
	_plot_area_bottom = 10;

	_num_x_axis = 0;
	_x_axis_info = NULL;
	_num_y_axis = 0;
	_y_axis_info = NULL;

	_x_axis_space = 30;
	_y_axis_space = 50;

	_num_data = NULL;
	_num_pairs = 0;
	_x_data = _y_data = _z_data = NULL;
	_ignore_data = NULL;

	_x_scale = 1.0;
	_y_scale = 1.0;
	_curr_pos = 0;
	_curr_event = 0;

	_num_annot = 0;
	_annot_ev_id = _annot_start = _annot_end = NULL;

	_num_event_class = 0;
	_event_ids = NULL;
	_num_events = 0;

	_dont_response_ev_change = 0;

	if (h && opt)
		init();
} // constructor


ra_plot_general::~ra_plot_general()
{
	free_data();

	if (_annot_ev_id)
		delete[] _annot_ev_id;
	if (_annot_start)
		delete[] _annot_start;
	if (_annot_end)
		delete[] _annot_end;

	if (_x_axis_info)
		delete _x_axis_info;
	if (_y_axis_info)
		delete _y_axis_info;

	ra_comm_del(_meas, (void *) this, NULL);
} // destructor


void
ra_plot_general::free_data()
{
	for (int i = 0; i < _num_pairs; i++)
	{
		if (_x_data && _x_data[i])
			delete[] _x_data[i];
		if (_y_data && _y_data[i])
			delete[] _y_data[i];
		if (_z_data && _z_data[i])
			delete[] _z_data[i];
		if (_ignore_data && _ignore_data[i])
			delete[] _ignore_data[i];
	}

	if (_event_ids)
	{
		for (int i = 0; i < _num_event_class; i++)
			delete[] _event_ids[i];
		delete[] _event_ids;
	}
	if (_num_events)
		delete[] _num_events;

	if (_x_data)
		delete[] _x_data;
	if (_y_data)
		delete[] _y_data;
	if (_z_data)
		delete[] _z_data;
	if (_ignore_data)
		delete[] _ignore_data;

	_event_ids = NULL;
	_num_events = NULL;
	_num_data = NULL;
	_x_data = _y_data = _z_data = NULL;
	_ignore_data = NULL;
} // free_data()


void
ra_plot_general::update()
{
	// do nothing
} // update()


void
ra_plot_general::update_pos()
{
	// do nothing
} // update_pos()


void
ra_plot_general::init()
{
	get_plot_data();
	get_annot_data();
} // init()


void
ra_plot_general::set_event(long event_idx, bool from_signal /*=false*/ )
{
	if (_curr_event == event_idx)
		return;

	_curr_event = event_idx;
	check_curr_event();

	value_handle vh = ra_value_malloc();
	ra_prop_get_value(_opt->axis[_pos_idx].event_property, _event_ids[0][_curr_event], -1, vh);
	_curr_pos = ra_value_get_long(vh);
	ra_value_free(vh);

	double xs = _x_axis_info->start * _x_axis_info->scale;
	_curr_pos -= (long) xs;

	if (!from_signal)
		send_event();
} // set_event()


void
ra_plot_general::set_sig_event(class_handle clh, long event)
{
	bool ignore_it = true;
	for (int i = 0; i < _num_event_class; i++)
	{
		if (clh && (clh == _opt->clh[i]))
		{
			ignore_it = false;
			break;
		}
	}
	if (ignore_it)
		return;		// I don't care about this event-set

	long ev_idx = 0;
	for (long l = 0; l < _num_events[0]; l++)
	{
		if (_event_ids[0][l] == event)
		{
			ev_idx = l;
			break;
		}
	}

	set_event(ev_idx, true);
} // set_sig_event()


void
ra_plot_general::set_pos_event(long pos)
{
	if (!_x_axis_info->axis_is_pos)
		return;

	_curr_pos = pos - (long)(_x_axis_info->start * _x_axis_info->scale);
	update_pos();
} // set_pos_event()


void
ra_plot_general::check_curr_event()
{
	if (_curr_event < 0)
		_curr_event = 0;
	else if (_curr_event >= _num_events[0])
		_curr_event = _num_events[0] - 1;
} // check_curr_event()


void
ra_plot_general::send_event()
{
// 	for (int i = 0; i < _num_event_class; i++)
// 	{
// 		struct comm_para *para = new struct comm_para[2];
// 		para[0].type = PARA_VOID_POINTER;
// 		para[0].v.pv = _opt->clh[i];
// 		para[1].type = PARA_LONG;
// 		para[1].v.l = _event_ids[i][_curr_event[i]];
		
// 		_block_signals = true;
// 		ra_comm_emit(_meas, "plot-view", "highlight-event", 2, para);
// 		_block_signals = false;
		
// 		delete[]para;
// 	}

	struct comm_para *para = new struct comm_para[2];
	para[0].type = PARA_VOID_POINTER;
	para[0].v.pv = _opt->clh[0];
	para[1].type = PARA_LONG;
	para[1].v.l = _event_ids[0][_curr_event];
		
	_block_signals = true;
	ra_comm_emit(_meas, "plot-view", "highlight-event", 2, para);
	_block_signals = false;
	
	delete[]para;
} // send_event()


int
ra_plot_general::get_plot_data()
{
	free_data();

	// have to use a local number of event-class because when
	// calling the set-data dialog, the number in the option
	// struct can change (and then freeing the memory fails)
	_num_event_class = _opt->num_event_class;

	value_handle vh_ev = ra_value_malloc();
	_num_events = new long[_num_event_class];
	_event_ids = new long *[_num_event_class];
	for (int i = 0; i < _num_event_class; i++)
	{
		ra_class_get_events(_opt->clh[i], 0, -1, 0, 1, vh_ev);
		_num_events[i] = ra_value_get_num_elem(vh_ev);
		_event_ids[i] = new long[_num_events[i]];
		memcpy(_event_ids[i], ra_value_get_long_array(vh_ev), sizeof(long) * _num_events[i]);
	}
	ra_value_free(vh_ev);

	_num_pairs = _opt->num_plot_pairs;
	_num_data = new long[_num_pairs];

	_data_ev_id = new long *[_num_pairs];
	_x_data = new double *[_num_pairs];
	memset(_x_data, 0, sizeof(double *) * _num_pairs);
	_y_data = new double *[_num_pairs];
	memset(_y_data, 0, sizeof(double *) * _num_pairs);
 	_z_data = new double *[_num_pairs];
 	memset(_z_data, 0, sizeof(double *) * _num_pairs);
	_ignore_data = new int *[_num_pairs];
	memset(_ignore_data, 0, sizeof(int *) * _num_pairs);

	for (int i = 0; i < _num_pairs; i++)
	{
		// FIXME: up to now I need that the x-axis will in all pairs the same. Perhaps
		// this will be forced in the selection dialog or I find a better way to handle
		// pairs comming from different event-sets and displayed in the same plot-window
		_pos_idx = _opt->pair[i].x_idx;

		get_data(&(_opt->axis[_opt->pair[i].x_idx]), //&(_data_ev_id[i]),
			 &(_x_data[i]), &(_num_data[i]), &(_ignore_data[i]));
		get_data(&(_opt->axis[_opt->pair[i].y_idx]), //&(_data_ev_id[i]),
			 &(_y_data[i]), &(_num_data[i]), &(_ignore_data[i]));
		if (0)		//_opt->use_z_axis)
		{
			get_data(&(_opt->axis[_opt->pair[i].z_idx]), //&(_data_ev_id[i]),
				 &(_z_data[i]), &(_num_data[i]), &(_ignore_data[i]));
		}

 		sort_data(_x_data[i], _y_data[i], _z_data[i], _num_data[i], _ignore_data[i]);
	}

	// get how many axes, the axis range and "binning" and set the variables for the plot-area
	// space needed
	set_axis_variables();

	return 0;
} // get_plot_data()


int
ra_plot_general::get_data(struct ra_plot_axis *a, double **data, long *len, int **ignore_data)
{
	// TODO: after del of templates evprop-handles changes; why?
	a->event_property = ra_prop_get(_opt->clh[a->event_class_idx], a->prop_name);

	value_handle vh = ra_value_malloc();
	ra_info_get(a->event_property, RA_INFO_PROP_IGNORE_VALUE_D, vh);

	double ign_value = 0.0;
	int use_ign = 0;
	if (ra_value_is_ok(vh))
	{
		use_ign = 1;
		ign_value = ra_value_get_double(vh);
	}

	value_handle vh_id = ra_value_malloc();
	value_handle vh_ch = ra_value_malloc();
	ra_prop_get_value_all(a->event_property, vh_id, vh_ch, vh);

	long n = ra_value_get_num_elem(vh);
	if (n <= 0)
	{
		// if not use min/max values set them to zero to have sane values
		if (!a->use_min)
			a->min = 0;
		if (!a->use_max)
			a->max = 0;

		ra_value_free(vh_id);
		ra_value_free(vh_ch);
		ra_value_free(vh);

		return -1;
	}

	*data = new double[n];
	*ignore_data = new int[n];
	memset((*ignore_data), 0, sizeof(int) * n);

//	const long *ev_id = ra_value_get_long_array(vh_id);
	const long *ev_ch = ra_value_get_long_array(vh_ch);

	long *idx = new long[n];
	long num_valid = 0;
	long l;
	for (l = 0; l < n; l++)
	{
  		if (ev_ch[l] != a->ch)
 			continue;

		idx[num_valid++] = l;
	}
	if (len)
		*len = num_valid;

	const short *sbuf;
	const long *lbuf;
	const double *dbuf;
	switch (ra_value_get_type(vh))
	{
	case RA_VALUE_TYPE_SHORT_ARRAY:
		sbuf = ra_value_get_short_array(vh);
		for (l = 0; l < num_valid; l++)
			(*data)[l] = (double)(sbuf[idx[l]]);
		break;
	case RA_VALUE_TYPE_LONG_ARRAY:
		lbuf = ra_value_get_long_array(vh);
		for (l = 0; l < num_valid; l++)
			(*data)[l] = (double)(lbuf[idx[l]]);
		break;
	case RA_VALUE_TYPE_DOUBLE_ARRAY:
		dbuf = ra_value_get_double_array(vh);
		for (l = 0; l < num_valid; l++)
			(*data)[l] = dbuf[idx[l]];
		break;
	default:
		delete[] *data;
		delete[] idx;
		ra_value_free(vh_id);
		ra_value_free(vh_ch);
		ra_value_free(vh);
		return -1;
		break;
	}

	delete[] idx;

	ra_value_free(vh_id);
	ra_value_free(vh_ch);
	ra_value_free(vh);

	if (use_ign)
	{
		for (l = 0; l < num_valid; l++)
		{
			if ((*data)[l] == ign_value)
				(*ignore_data)[l] = 1;
		}
	}

	double min, max;
	get_min_max(*data, *ignore_data, num_valid, &min, &max);
	if (!a->use_min)
		a->min = min;
	if (!a->use_max)
		a->max = max;

	return 0;
} // get_data()


void
ra_plot_general::get_min_max(double *data, int *ignore_data, long n, double *min, double *max)
{
	if (!data || (n <= 0))
		return;

	long start = 0;
	while (start < n)
	{
		if (ignore_data[start] == 0)
			break;
		start++;
	}

	*min = *max = data[start];
	for (long l = start+1; l < n; l++)
	{
		if (ignore_data[l])
			continue;

		if (data[l] < *min)
			*min = data[l];
		if (data[l] > *max)
			*max = data[l];
	}
} // get_min_max()


int
comp_pos(const void* arg1, const void* arg2)
{
	struct sort *b1 = (struct sort *)arg1;
	struct sort *b2 = (struct sort *)arg2;

	if (b1->x > b2->x)
		return 1;
	else if (b1->x < b2->x)
		return -1;

	return 0;
}  /* comp_pos() */


int
ra_plot_general::sort_data(double *x, double *y, double *z, long num, int *ignore)
{
	if ((x == NULL) || (num <= 1))
		return 0;	// no need to sort

	struct sort *s = new struct sort[num];
	long l;
	for (l = 0; l < num; l++)
	{
		if (x)
			s[l].x = x[l];
		if (y)
			s[l].y = y[l];
		if (z)
			s[l].z = z[l];
		s[l].ign = ignore[l];
	}

	qsort(s, num, sizeof(struct sort), comp_pos);

	for (l = 0; l < num; l++)
	{
		if (x)
			x[l] = s[l].x;
		if (y)
			y[l] = s[l].y;
		if (z)
			z[l] = s[l].z;
		ignore[l] = s[l].ign;
	}

	delete[] s;

	return 0;
} // sort_data


int
ra_plot_general::set_axis_variables()
{
	get_x_axis_info();
	get_y_axis_info();

	set_time_infos(_x_axis_info, _num_x_axis);
	set_time_infos(_y_axis_info, _num_y_axis);

	get_axis_div(_x_axis_info, _num_x_axis);
	get_axis_div(_y_axis_info, _num_y_axis);

	return 0;
} // set_axis_variables()


int
ra_plot_general::get_x_axis_info()
{
	if (_opt->num_plot_pairs <= 0)
		return 0;

	if (_x_axis_info)
		delete _x_axis_info;

	// up to now only one x-axis is supported
	_num_x_axis = 1;
	_x_axis_info = new struct axis_info;

	_plot_area_bottom = _x_axis_space;

	_x_axis_info->idx = _opt->pair[0].x_idx;

	_x_axis_info->start = _opt->axis[_opt->pair[0].x_idx].min;
	_x_axis_info->end = _opt->axis[_opt->pair[0].x_idx].max;

	// will be later decided if unit is 'su' (sample-units)
	if (_opt->axis[_opt->pair[0].x_idx].is_time)
		_x_axis_info->axis_is_pos = true;
	else
		_x_axis_info->axis_is_pos = false;

	return 0;
} // get_x_axis_info()


int
ra_plot_general::get_y_axis_info()
{
	if (_y_axis_info)
		delete[]_y_axis_info;
	_y_axis_info = new struct axis_info[_opt->num_plot_pairs];
	memset(_y_axis_info, 0, sizeof(struct axis_info) * _opt->num_plot_pairs);

	_num_y_axis = 0;
	int cnt_all, cnt_left, cnt_right;
	cnt_all = cnt_left = cnt_right = 0;
	for (int i = 0; i < _opt->num_plot_pairs; i++)
	{
		bool already_count = false;
		for (int j = 0; j < i; j++)
		{
			if (_opt->pair[i].y_idx == _opt->pair[j].y_idx)
			{
				already_count = true;
				break;
			}
			if ((_opt->axis[_opt->pair[i].y_idx].unit[0] != '\0') &&
			    (strcmp
			     (_opt->axis[_opt->pair[i].y_idx].unit,
			      _opt->axis[_opt->pair[j].y_idx].unit) == 0))
			{
				already_count = true;
				break;
			}

		}
		if (already_count)
			_y_axis_info[_num_y_axis].dont_draw_axis = true;

		_y_axis_info[_num_y_axis].start = _opt->axis[_opt->pair[i].y_idx].min;
		_y_axis_info[_num_y_axis].end = _opt->axis[_opt->pair[i].y_idx].max;

		_y_axis_info[_num_y_axis].idx = _opt->pair[i].y_idx;
		if (!already_count)
		{
			if ((cnt_all % 2) == 0)
			{
				_y_axis_info[_num_y_axis].opp_side = false;
				_y_axis_info[_num_y_axis].axis_num = cnt_left;
				cnt_left++;
			}
			else
			{
				_y_axis_info[_num_y_axis].opp_side = true;
				_y_axis_info[_num_y_axis].axis_num = cnt_right;
				cnt_right++;
			}
		}

		_num_y_axis++;
		cnt_all++;
	}

	_plot_area_left = cnt_left * _y_axis_space;
	_plot_area_right = cnt_right * _y_axis_space;
	if (_plot_area_right < 20)
		_plot_area_right = 20;

	return 0;
} // get_y_axis_info()


int
ra_plot_general::get_axis_div(struct axis_info *inf, int num)
{
	for (int i = 0; i < num; i++)
	{
		if (_opt->axis[inf[i].idx].use_min == 0)
			inf[i].start = _opt->axis[inf[i].idx].min / inf[i].scale;
		if (_opt->axis[inf[i].idx].use_max == 0)
			inf[i].end = _opt->axis[inf[i].idx].max / inf[i].scale;

		double range = inf[i].end - inf[i].start;
		inf[i].step = get_step(range);
		inf[i].start = floor(inf[i].start / inf[i].step) * inf[i].step;
		inf[i].end = ceil(inf[i].end / inf[i].step) * inf[i].step;
	}

	return 0;
} // get_axis_div()


double
ra_plot_general::get_step(double range)
{
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

	double n = abs_range / step;
	while (n > 10)
	{
		step *= 2;
		n = abs_range / step;
	}

	return step;
} // get_step()


double
ra_plot_general::raise(double x, int y)
{
	double val = 1.0;
	for (int i = 0; i < abs(y); i++)
		val *= x;
	if (y < 0)
		return (1.0 / val);
	else
		return val;
} // raise()


int
ra_plot_general::get_tics_precision(double step)
{
	if (step >= 1)
		return 0;

	int l = (int) (log10(step) - 1);
	l = abs(l);

	return l;
} // get_tics_precision()


void
ra_plot_general::set_time_infos(struct axis_info *inf, int num)
{
	for (int i = 0; i < num; i++)
	{
		inf[i].is_time = (_opt->axis[inf[i].idx].is_time ? true : false);
		inf[i].time_format = _opt->axis[inf[i].idx].time_format;

		inf[i].scale = 1.0;

		if (inf[i].is_time)
		{
			switch (inf[i].time_format)
			{
			case RA_PLOT_TIME_MSEC:
				inf[i].scale = 1.0 / 1000.0;
				break;
			case RA_PLOT_TIME_MIN:
				inf[i].scale = 60.0;
				break;
			case RA_PLOT_TIME_HOUR:
				inf[i].scale = 60.0 * 60.0;
				break;
			case RA_PLOT_TIME_DAY:
				inf[i].scale = 60.0 * 60.0 * 24.0;
				break;
			case RA_PLOT_TIME_WEEK:
				inf[i].scale = 60.0 * 60.0 * 24.0 * 7.0;
				break;
			case RA_PLOT_TIME_MONTH:	// FIXME: not every month has 30 days
				inf[i].scale = 60.0 * 60.0 * 24.0 * 30.0;
				break;
			case RA_PLOT_TIME_YEAR:	// FIXME: not every year has 365 days
				inf[i].scale = 60.0 * 60.0 * 24.0 * 365.0;
				break;
			}
			inf[i].scale *= get_samplerate();
		}
	}
} // set_time_infos()


double
ra_plot_general::get_samplerate()
{
	double sr = 1.0;

	value_handle vh = ra_value_malloc();
	if (ra_info_get(_meas, RA_INFO_MAX_SAMPLERATE_D, vh) == 0)
		sr = ra_value_get_double(vh);
	ra_value_free(vh);

	return sr;
} // get_samplerate()


int
ra_plot_general::get_annot_data()
{
	_num_annot = 0;
	if (_annot_ev_id)
	{
		delete[] _annot_ev_id;
		_annot_ev_id = NULL;
	}
	if (_annot_start)
	{
		delete[] _annot_start;
		_annot_start = NULL;
	}
	if (_annot_end)
	{
		delete[] _annot_end;
		_annot_end = NULL;
	}

	if (!_x_axis_info->axis_is_pos)
		return 0;

	// there should be at least one event-class, and here it it not
	// important which one is used
	eval_handle eh = ra_eval_get_handle(_opt->clh[0]);
	if (eh == NULL)
		return 0;

	value_handle vh = ra_value_malloc();

	if (ra_class_get(eh, "annotation", vh) != 0)
	{
		ra_value_free(vh);
		return 0;
	}
	if (ra_value_get_num_elem(vh) <= 0)
	{
		ra_value_free(vh);
		return 0;
	}

	if (ra_value_get_num_elem(vh) != 1)
	{
		// TODO: think how to handle more than one annotation event-class
		fprintf(stderr, "more than one annotation class !!!");
		ra_value_free(vh);
		return 0;
	}
	class_handle clh = (class_handle)(ra_value_get_voidp_array(vh)[0]);

	prop_handle annot_prop = ra_prop_get(clh, "annotation");
	if (annot_prop == NULL)
	{
		ra_value_free(vh);
		return 0;		
	}

	// allocate mem for the available annotations
	if (ra_class_get_events(clh, -1, -1, 0, 1, vh) != 0)
	{
		ra_value_free(vh);
		return 0;
	}
	_num_annot = ra_value_get_num_elem(vh);
	_annot_ev_id = new long[_num_annot];
	_annot_start = new long[_num_annot];
	_annot_end = new long[_num_annot];

	const long *ev_ids = ra_value_get_long_array(vh);
	for (long l = 0; l < _num_annot; l++)
	{
		_annot_ev_id[l] = ev_ids[l];
		ra_class_get_event_pos(clh, ev_ids[l], _annot_start+l, _annot_end+l);
	}

	ra_value_free(vh);

	return 0;
}  // get_annot_data()


void
ra_plot_general::collect_event_changes(int flag)
{
	_dont_response_ev_change = flag;
} // collect_event_changes()

