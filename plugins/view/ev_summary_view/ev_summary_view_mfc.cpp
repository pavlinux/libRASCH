/*----------------------------------------------------------------------------
 * ev_summary_view_mfc.cpp
 *
 * 
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002-2006, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#include "stdafx.h"
#include "ev_summary_view_mfc.h"

#include <stdio.h>
//#include <ctype.h>

#define _LIBRASCH_BUILD
#include <ra.h>
#include <ra_priv.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//IMPLEMENT_DYNCREATE(ev_summary_view_mfc, CScrollView )
BEGIN_MESSAGE_MAP(ev_summary_view_mfc, CScrollView)
	//{{AFX_MSG_MAP(ev_summary_view_mfc)
	ON_WM_LBUTTONDBLCLK()ON_WM_KEYDOWN()ON_WM_SIZE()
	//}}AFX_MSG_MAP
	// Standard-Druckbefehle
END_MESSAGE_MAP()ev_summary_view_mfc::ev_summary_view_mfc():ev_summary_view_general(NULL, NULL)
{
//	_max_part_width = 50;
}  // constructor


ev_summary_view_mfc::~ev_summary_view_mfc()
{
}  // destructor


void
ev_summary_view_mfc::update()
{
	set_scroll_size();
	Invalidate();
}  // update()


void
ev_summary_view_mfc::init(meas_handle mh, sum_handle sh)
{
	_mh = mh;
	_sh = sh;

//	ev_summary_view_general::init();
} // init()


BOOL ev_summary_view_mfc::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName,
			       DWORD dwStyle, const RECT & rect,
			       CWnd * pParentWnd, UINT nID, CCreateContext * pContext)
{
	BOOL ok;
	ok = CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
	if (!ok)
		return ok;

	CDC *dc = GetDC();
	_pixmm_h = (double) dc->GetDeviceCaps(HORZRES) / (double) dc->GetDeviceCaps(HORZSIZE);
	_pixmm_v = (double) dc->GetDeviceCaps(VERTRES) / (double) dc->GetDeviceCaps(VERTSIZE);
	ReleaseDC(dc);

	// Brush for background
	LOGBRUSH
		LogBrush;
	LogBrush.lbStyle = BS_SOLID;
	LogBrush.lbColor = RGB(254, 251, 197);
	LogBrush.lbHatch = 0;
	_bk_brush.CreateBrushIndirect(&LogBrush);

	ev_summary_view_general::init();

	set_scroll_size();

	return ok;
}  // OnCreate()


void
ev_summary_view_mfc::OnDraw(CDC * pDC)
{
	int cont = 1;

	for (long l = 0; l < _num_sum_part; l++)
	{
		int x = X_OFFSET + (l * (EV_SUMMARY_DIST + _max_part_width));
		int x_next = X_OFFSET + ((l+1) * (EV_SUMMARY_DIST + _max_part_width));

		long part_idx = _part_order[l];

/*		if (x_next < clipx)
			continue;
		if (x > (clipx + clipw))
			break;
*/
		switch (_num_dim)
		{
		case 1:
			plot_one_dim(pDC, x, part_idx);
			break;
		case 2:
		case 3:
		default:
			_ra_set_error(_mh, RA_ERR_UNSUPPORTED, "plugin-ev-summary-view");
			cont = 0;
			break;
		}

		if (!cont)
			return;
	}
	
 	show_selected_templ(pDC);
} // OnDraw()


void
ev_summary_view_mfc::plot_one_dim(CDC *dc, long x_start, long part_idx)
{
	char t[100];
	UINT ta_save = dc->SetTextAlign(TA_CENTER | TA_BASELINE);
	sprintf(t, "%d", _sum_part[part_idx].part_id);
 	dc->TextOut(x_start + (_max_part_width/2), 15, t, strlen(t));
	sprintf(t, "%d", _sum_part[part_idx].num_events);
 	dc->TextOut(x_start + (_max_part_width/2), 30, t, strlen(t));
	dc->SetTextAlign(ta_save);
	
	plot_ev_summary_1d(dc, x_start, part_idx);
} // plot_one_dim()


void
ev_summary_view_mfc::plot_ev_summary_1d(CDC *dc, long x_start, long part_idx)
{
	for (long l = 0; l < _num_ch; l++)
		plot_channel_1d(dc, x_start, part_idx, l);
} // plot_ev_summary()


void
ev_summary_view_mfc::plot_channel_1d(CDC *dc, long x_start, long part_idx, long ch)
{
	long x_offset = (_max_part_width_su / 2) - _fiducial_offset[ch];

	POINT *pts = new POINT[_sum_part[part_idx].num_data[ch][0]];
	for (long l = 0; l < _sum_part[part_idx].num_data[ch][0]; l++)
	{
		int x = (int)((double) l * _ch[ch].xscale);
		int y = (int)(_sum_part[part_idx].raw[ch][0][l]);

		// move to position
		x += x_start;
		y += (EV_SUMMARY_HEIGHT * ch);

		pts[l].x = x;
		pts[l].y = y + Y_OFFSET;
	}
	dc->Polyline(pts, _sum_part[part_idx].num_data[ch][0]);

	if (pts != NULL)
		delete[]pts;
} // plot_channel()


void
ev_summary_view_mfc::show_selected_templ(CDC *dc)
{
	if (_curr_sum_part_idx == -1)
		return;

	int width = _max_part_width + EV_SUMMARY_DIST;


	RECT rect;
	int t = _curr_sum_part_idx * width + X_OFFSET;
	rect.left = t;
	rect.top = Y_OFFSET;
	rect.right = t + _max_part_width;
	rect.bottom = Y_OFFSET + (EV_SUMMARY_HEIGHT * _num_ch);

	dc->MoveTo(rect.left, rect.top);
	dc->LineTo(rect.right, rect.top);
	dc->LineTo(rect.right, rect.bottom);
	dc->LineTo(rect.left, rect.bottom);
	dc->LineTo(rect.left, rect.top);
} // show_selected_templ()


void
ev_summary_view_mfc::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	int x, y;
	x = point.x;
	y = point.y;

	CPoint pt = GetScrollPosition();
	x += pt.x;

	int idx = ((x - X_OFFSET) / (_max_part_width + EV_SUMMARY_DIST));
	if (idx < _num_sum_part)
	{
		_curr_sum_part_idx = idx;
		_curr_sum_part_id = _sum_part[_part_order[_curr_sum_part_idx]].part_id;;
	}
	else
	{
		_curr_sum_part_idx = -1;
		_curr_sum_part_id = -1;
	}

	ev_summary_select();

	Invalidate();
}  // OnLButtonDblClk()


void
ev_summary_view_mfc::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	char c = (char) (toupper(nChar));

	bool new_select = false;
	bool do_repaint = false;
	switch (nChar)
	{
	case VK_RIGHT:
		if (_curr_sum_part_idx < (_num_sum_part-1))
		{
			_curr_sum_part_idx++;
			new_select = true;
		}
		break;
	case VK_LEFT:
		if (_curr_sum_part_idx > 0)
		{
			_curr_sum_part_idx--;
			new_select = true;
		}
		break;
	case VK_HOME:
		_curr_sum_part_idx = 0;
		new_select = true;
		break;
	case VK_END:
		_curr_sum_part_idx = _num_sum_part - 1;
		new_select = true;
		break;
	default:
		do_repaint = key_press(c);
		break;
	}

	if (new_select)
	{
		_curr_sum_part_id = _sum_part[_part_order[_curr_sum_part_idx]].part_id;

		int part_width = _max_part_width + EV_SUMMARY_DIST;
		int x = _curr_sum_part_idx * part_width + X_OFFSET;

		RECT r;
		GetClientRect(&r);

		CPoint new_pos = GetScrollPosition();
		if (x < new_pos.x)
		{
			new_pos.x = x - X_OFFSET;
			ScrollToPosition(new_pos);
		}
		if (x > (new_pos.x + (r.right-r.left) - part_width))
		{
			new_pos.x = x - (r.right-r.left) + part_width - X_OFFSET;
			ScrollToPosition(new_pos);
		}

		update();
		ev_summary_select();
	}
	
	if (do_repaint)
		update();
}  // OnKeyDown()


void
ev_summary_view_mfc::set_scroll_size()
{
	// set overall size
	SIZE sz;
	sz.cx = X_OFFSET + (_num_sum_part * (EV_SUMMARY_DIST + _max_part_width));
	sz.cy = Y_OFFSET + (_num_ch * EV_SUMMARY_HEIGHT);

	// set page size
	RECT r;
	GetClientRect(&r);
	int w = r.right - r.left;
	int h = r.bottom - r.top;
	SIZE page_sz;
	page_sz.cx = w - (w % (EV_SUMMARY_DIST + _max_part_width));
	page_sz.cy = h - (h % EV_SUMMARY_HEIGHT);

	SIZE line_sz;
	line_sz.cx = EV_SUMMARY_DIST + _max_part_width;
	line_sz.cy = EV_SUMMARY_HEIGHT;

	SetScrollSizes(MM_TEXT, sz, page_sz, line_sz);
}  // set_scroll_size()


void
ev_summary_view_mfc::OnSize(UINT nType, int cx, int cy)
{
	CScrollView::OnSize(nType, cx, cy);

	set_scroll_size();
}  // OnSize()
