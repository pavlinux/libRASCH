/**
 * \file cont_ap_view_mfc.cpp
 *
 * MFC specific file for arterial-blood-pressure view plugin.
 *
 * \author Raphael Schneider (rasch@med1.med.tum.de)
 */

/*--------------------------------------------------------------------------
 *
 * Copyright (C) 2002-2006, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *
 *--------------------------------------------------------------------------*/

#include <stdio.h>
#include <ctype.h>

#include "cont_ap_view_mfc.h"

#include <ra_ap_morphology.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// ra_view_ts

//IMPLEMENT_DYNCREATE(cont_ap_view_mfc, ts_view_mfc )
BEGIN_MESSAGE_MAP(cont_ap_view_mfc, ts_view_mfc)
	//{{AFX_MSG_MAP(cont_ap_view_mfc)
	ON_WM_KEYDOWN()ON_WM_LBUTTONDOWN()ON_WM_PAINT()
	//}}AFX_MSG_MAP
	// Standard-Druckbefehle
END_MESSAGE_MAP()cont_ap_view_mfc::cont_ap_view_mfc():ts_view_mfc(), cont_ap_view_general(NULL),
ts_view_general(NULL)
{
/*	_popup_menu_sort = new QPopupMenu();
	for (int i = 1; i <= _num_ch; i++)
	{
		char t[100];
		sprintf(t, "beats found in %d channel%s", i, (i > 1 ? "s" : ""));
		int id = _popup_menu_sort->insertItem(t, this, SLOT(sort_ch_found(int)));
		_popup_menu_sort->setItemParameter(id, i-1);
	}
	int id = _popup_menu_sort->insertItem("shortest rr-interval first", this, SLOT(sort_rri(int)));
	_popup_menu_sort->setItemParameter(id, 1);
	id = _popup_menu_sort->insertItem("longest rr-interval first", this, SLOT(sort_rri(int)));
	_popup_menu_sort->setItemParameter(id, 0);

	id = _popup_menu_sort->insertItem("normal beats", this, SLOT(sort_class(int)));
	_popup_menu_sort->setItemParameter(id, ECG_NORMAL_BEAT);
	id = _popup_menu_sort->insertItem("VPCs", this, SLOT(sort_class(int)));
	_popup_menu_sort->setItemParameter(id, ECG_VES_BEAT);
	id = _popup_menu_sort->insertItem("paced beats", this, SLOT(sort_class(int)));
	_popup_menu_sort->setItemParameter(id, ECG_PACED_BEAT);
	id = _popup_menu_sort->insertItem("Artifacts", this, SLOT(sort_class(int)));
	_popup_menu_sort->setItemParameter(id, ECG_ARTIFACT_BEAT);

	if (_num_templates > 0)
	{
		_popup_menu_template = new QPopupMenu();
		for (int i = 0; i < _num_templates; i++)
		{
			char t[100];
			sprintf(t, "beats in template %d", i);
			int id = _popup_menu_template->insertItem(t, this, SLOT(sort_template(int)));
			_popup_menu_template->setItemParameter(id, i);
		}
	}

	_popup_menu_ecg = new QPopupMenu();
	_popup_menu_ecg->insertItem("sort", _popup_menu_sort);
	if (_num_templates > 0)
		_popup_menu_ecg->insertItem("templates", _popup_menu_template);

	_popup_menu->insertItem("ecg", _popup_menu_ecg);*/
}


cont_ap_view_mfc::~cont_ap_view_mfc()
{
//      delete _popup_menu_sort;
}				// destructor


void
cont_ap_view_mfc::update()
{
	Invalidate();
}				// update()


void
cont_ap_view_mfc::init(meas_handle mh, plugin_handle pl, int num_ch, int *ch)
{
	ts_view_mfc::init(mh, pl, num_ch, ch);
	cont_ap_view_general::init();
} // init()


void
cont_ap_view_mfc::do_update()
{
	get_eventtypes();
}				// do_update()


void
cont_ap_view_mfc::OnPaint()
{
//      CPaintDC dc(this); // device context for painting

	ts_view_mfc::OnPaint();

	CDC *dc = GetDC();
	dc->SelectObject(_font);

	plot_beats(dc);

	// mark calibration sequences (if any)
	mark_calib_seq(dc);

	plot_cursor(dc);
} // OnPaint()


void
cont_ap_view_mfc::plot_beats(CDC * dc)
{
	if (_num_events == 0)
		return;

	int w, h;
	get_text_extent("120/99 (99)", &w, &h);

	UINT ta_save = dc->SetTextAlign(TA_CENTER | TA_TOP);
	dc->SetBkMode(TRANSPARENT);

	value_handle vh = ra_value_malloc();
	for (int j = 0; j < _num_ch; j++)
	{
		if (!_ch[j].show_it)
			continue;
		
		if (_ch[j].type != RA_CH_TYPE_RR)
			continue;

		int *x = NULL;
		int num = 0;
		long *events = NULL;
		get_beat_pos(_prop_dias_pos, _ch[j].ch_number, &x, &events, &num);

		// get mean difference between 5 beats (better behaviour when SVPCs occure)
		int n = num >= 5 ? 5 : num;
		int d = 0;
		int i;  // because of VC++
		for (i = 1; i < n; i++)
			d += (x[i] - x[i-1]);
		if (n > 1)
			d /= (n-1);
		else
			d = 100;
		// check if speed is to high to show values proper
		if (d < w)
		{
			if (x)
				delete[]x;
			if (events)
				delete[]events;
			return;
		}

		int y_value_line = 2 + (j * 20);
		// print channel name before RR values
		CString s;
		if (_ch[j].inverse)
		{
			s = "-";
			s += _ch[j].name;
		}
		else
			s = _ch[j].name;
		s += ":";
		UINT ta_save2 = dc->SetTextAlign(TA_LEFT | TA_TOP);
		dc->TextOut(_ch_info_offset, y_value_line, s);
		dc->SetTextAlign(ta_save2);

		for (i = 0; i < num; i++)
		{
			ra_prop_get_value(_prop_flags, events[i], _ch[j].ch_number, vh);
			long flags = (ra_value_is_ok(vh) ? ra_value_get_long(vh) : -1);
			ra_prop_get_value(_prop_syst, events[i], _ch[j].ch_number, vh);
			long syst = (ra_value_is_ok(vh) ? ra_value_get_long(vh) : -1);
			ra_prop_get_value(_prop_dias, events[i], _ch[j].ch_number, vh);
			long dias = (ra_value_is_ok(vh) ? ra_value_get_long(vh) : -1);
			ra_prop_get_value(_prop_mean, events[i], _ch[j].ch_number, vh);
			long mean = (ra_value_is_ok(vh) ? ra_value_get_long(vh) : -1);

			char t[50];
			sprintf(t, "%s%s%ld/%ld (%ld)", ((flags & AP_VALUE_NOK) ? "X-" : ""), 
				((flags & AP_VALUE_INTERPOLATED) ? "I-" : ""), syst, dias, mean);
			dc->TextOut(x[i], y_value_line, t, strlen(t));

			// show ibi values
			if (_prop_ibi && (i < (num-1)) && (d > (int)(1.5 * (double)w)))
			{
				ra_prop_get_value(_prop_ibi, events[i], _ch[j].ch_number, vh);
				long ibi = (ra_value_is_ok(vh) ? ra_value_get_long(vh) : -1);
				sprintf(t, "%ld", ibi);
				
				int x_ibi = x[i] + (x[i+1] - x[i]) / 2;
				dc->TextOut(x_ibi, y_value_line, t, strlen(t));
			}

			// show positions
			if (_prop_syst)	// bloodpressure values available?
			{
				ra_prop_get_value(_prop_syst_pos, events[i], _ch[j].ch_number, vh);				
				long syst_pos = (long)(ra_value_get_double(vh) * _x_scale);

				ra_prop_get_value(_prop_dias_pos, events[i], _ch[j].ch_number, vh);				
				long dias_pos = (long)(ra_value_get_double(vh) * _x_scale);
				
				int pos_x = (int)((double)(syst_pos - _curr_pos) * _max_xscale) + _left_space;
				show_pos(dc, j, pos_x, syst_pos);
				pos_x = (int)((double)(dias_pos - _curr_pos) * _max_xscale) + _left_space;
				show_pos(dc, j, pos_x, dias_pos);
			}
		}
		if (x)
			delete[]x;
		if (events)
			delete[]events;
	}
	dc->SetTextAlign(ta_save);

	ra_value_free(vh);
}  // plot_beats()


void
cont_ap_view_mfc::show_pos(CDC *dc, int ch, int x, long pos)
{
	double max = draw_calc_max_value(ch);
	double val;
	if (ra_raw_get_unit(_rh, _ch[ch].ch_number, pos, 1, &val) != 1)
		return;

	int amp = draw_calc_amplitude(max, val, ch);
	int y = amp + _top_space + _ch[ch].top_win_pos;

	dc->MoveTo(x, y + 10);
	dc->LineTo(x, y - 10);
}				// show_pos()


void
cont_ap_view_mfc::mark_calib_seq(CDC * dc)
{
	int num_seq = 0;
	long *start_pos = NULL;
	long *end_pos = NULL;
	long *ch = NULL;
	long *index = NULL;

	get_calib_seq(&num_seq, &start_pos, &end_pos, &ch, &index);

	CBrush brush;
	brush.CreateHatchBrush(HS_BDIAGONAL, RGB(0, 0, 0));
	CBrush *brush_save = (CBrush *) dc->SelectObject(&brush);
	dc->SetBkMode(TRANSPARENT);

	for (int i = 0; i < num_seq; i++)
	{
		int start = (int) ((double) (start_pos[i] - _curr_pos) * _max_xscale) + _left_space;
		if (start < _left_space)
			start = _left_space;
		int end = (int) ((double) (end_pos[i] - _curr_pos) * _max_xscale) + _left_space;
		if (end > (_screen_width - _right_space))
			end = _screen_width - _right_space;

		int ch_idx = get_ch_index(ch[i]);
		dc->Rectangle(start, _top_space + _ch[ch_idx].top_win_pos,
				end, _top_space + _ch[ch_idx].top_win_pos + _ts_opt.ch_height);
	}
	dc->SelectObject(brush_save);

	if (start_pos)
		delete[] start_pos;
	if (end_pos)
		delete[] end_pos;
	if (ch)
		delete[] ch;
	if (index)
		delete[] index;
} // mark_calib_seq()


void
cont_ap_view_mfc::OnLButtonDown(UINT nFlags, CPoint point)
{
	int ch = mouse_press(point.x, point.y);
	_cursor_ch = ch;

	ts_view_mfc::OnLButtonDown(nFlags, point);
} // OnLButtonDown()


void
cont_ap_view_mfc::sort_general(const char *evtype, double value)
{
	cont_ap_view_general::sort_general(evtype, value);
	Invalidate();
}				// sort_general()


void
cont_ap_view_mfc::sort_bp(int type)
{
	cont_ap_view_general::sort_bp(type);
	Invalidate();
}				// sort_bp()


void
cont_ap_view_mfc::sort_class(int classification)
{
	cont_ap_view_general::sort_class(classification);
	Invalidate();
}				// sort_class
