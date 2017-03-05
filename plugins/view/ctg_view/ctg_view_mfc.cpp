/*----------------------------------------------------------------------------
 * ctg_view_mfc.cpp
 *
 * 
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002-2003, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Header: /home/cvs_repos.bak/librasch/plugins/view/ctg_view/ctg_view_mfc.cpp,v 1.5 2003/12/11 13:20:39 rasch Exp $
 *--------------------------------------------------------------------------*/

#include <stdio.h>
#include <ctype.h>

#include "ctg_view_mfc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ra_view_ts

//IMPLEMENT_DYNCREATE(ctg_view_mfc, ts_view_mfc )
BEGIN_MESSAGE_MAP(ctg_view_mfc, ts_view_mfc)
	//{{AFX_MSG_MAP(ctg_view_mfc)
	ON_WM_KEYDOWN()ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	// Standard-Druckbefehle
END_MESSAGE_MAP()

ctg_view_mfc::ctg_view_mfc():ts_view_mfc(), ctg_view_general(NULL), ts_view_general(NULL)
{
}


ctg_view_mfc::~ctg_view_mfc()
{
}				// destructor


void
ctg_view_mfc::update()
{
	Invalidate();
}				// update()


void
ctg_view_mfc::init(meas_handle mh, plugin_handle pl, int num_ch, int *ch)
{
	ts_view_mfc::init(mh, pl, num_ch, ch);
	ctg_view_general::init();

	_grid_pen1.CreatePen(PS_SOLID, 1, RGB(0, 255, 155));
	_grid_pen2.CreatePen(PS_SOLID, 2, RGB(0, 255, 155));
	_grid_pen3.CreatePen(PS_SOLID, 3, RGB(0, 255, 155));

	_uc_event_pen.CreatePen(PS_SOLID, 3, RGB(0, 0, 255));

	_bk_color = RGB(255, 255, 255);
}				// init()



void
ctg_view_mfc::OnPaint()
{
	ts_view_mfc::OnPaint();

	CDC *dc = GetDC();
	dc->SelectObject(_font);
	plot_time(dc);
	plot_uc_events(dc);

	// status bar
	if (_ts_opt.show_status && _num_sort)
	{
		char t[100];
		sprintf(t, "%ld / %ld", (_curr_sort + 1), _num_sort);
		UINT ta_save = dc->SetTextAlign(TA_RIGHT | TA_BASELINE);
		dc->TextOut(_screen_width - 4, _screen_height - 5, t, strlen(t));
		dc->SetTextAlign(ta_save);
	}

	if (_cursor_pos >= 0)
		plot_cursor(dc, _cursor_ch);
}				// OnPaint()


void
ctg_view_mfc::plot_grid(CDC *dc)
{
	double mm_5v = 5.0 * _pixmm_v;
	double mm_4v = 4.0 * _pixmm_v;
	double mm_5h = 5.0 * _pixmm_h;
	double mm_4h = 4.0 * _pixmm_h;

	int h = _screen_height - _top_space - _bottom_space;
	int w = _screen_width - _left_space - _right_space;

	// init device context
	dc->SelectObject(_font);

	CPen *pen_save = (CPen *)dc->SelectObject(&_grid_pen1);
	COLORREF color_save = dc->SetTextColor(RGB(0, 255, 155));

	// FHR area
	// first vertical lines
	int fhr_height = (int)(((210 - 50) / 10) * mm_5v);
	int n_col = (int)((double)w / mm_5h);
	int i;
	for (i = 0; i <= n_col; i++)
	{
		int x = (int) (mm_5h * (double)i) + _left_space;
		dc->MoveTo(x, _top_space);
		dc->LineTo(x, fhr_height + _top_space);
	}
	// and now the horizontal lines of the FHR area
	double y = -mm_5v;
	for (i = 210; i >= 50; i -= 10)
	{
		y += mm_5v;

		if ((i == 160) || (i == 120))
			dc->SelectObject(&_grid_pen3);
		else if ((i % 20) == 0)
			dc->SelectObject(&_grid_pen2);
		else
			dc->SelectObject(&_grid_pen1);

		int y_use = (int)y + _top_space;
		dc->MoveTo(_left_space, y_use);
		dc->LineTo(_screen_width - _right_space, y_use);
	}
	// and now the axis
	y = mm_5v;
	CBrush brush(RGB(255,255,255));
	CBrush *brush_save = (CBrush *)dc->SelectObject(&brush);
	for (i = 200; i >= 60; i -= 20)
	{
		
		int y_use = (int)y + _top_space;

		int wr = (int)(2*mm_4h);
		int hr = (int)(1.5*mm_4v);
		int xr = (int)(2*mm_5h) + _left_space - wr/2;
		int yr = y_use - hr/2;

		char t[4];
		sprintf(t, "%d", i);
		while (xr < (w - _right_space))
		{
			RECT r;
			r.left = xr;
			r.top = yr;
			r.right = r.left + wr;
			r.bottom = r.top + hr;
			dc->Rectangle(&r);
			dc->DrawText(t, strlen(t), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			xr += (int)(20 * mm_5h);
		}

		y += (2.0 * mm_5v);
	}

	// tokogram area
	y = fhr_height + (2 * mm_5v);	// time line
	int top_toko = (int)y + _top_space;
	y -= mm_4v;
	for (i = 100; i >= 0; i -= 10)
	{
		y += mm_4v;

		if (i == 20)
			dc->SelectObject(&_grid_pen3);
		else if ((i % 20) == 0)
			dc->SelectObject(&_grid_pen2);
		else
			dc->SelectObject(&_grid_pen1);

		int y_use = (int)y + _top_space;
		dc->MoveTo(_left_space, y_use);
		dc->LineTo(_screen_width - _right_space, y_use);
	}
	// and now the vertical lines of the Tokogram area
	// width and height in cm
	dc->SelectObject(&_grid_pen1);
	for (i = 0; i <= n_col; i++)
	{
		int x = (int) (mm_5h * (double) i) + _left_space;
		dc->MoveTo(x, top_toko);
		dc->LineTo(x, (int)y + _top_space);	// use y of horizontal lines
	}
	// and now the axis
	y = fhr_height + (2 * mm_5v) + (2 * mm_4v);
	for (i = 80; i >= 20; i -= 20)
	{
		
		int y_use = (int)y + _top_space;

		int wr = (int)(2*mm_4h);
		int hr = (int)(1.5*mm_4v);
		int xr = (int)(2*mm_5h) + _left_space - wr/2;
		int yr = y_use - hr/2;

		char t[4];
		sprintf(t, "%d", i);
		while (xr < (w - _right_space))
		{
			RECT r;
			r.left = xr;
			r.top = yr;
			r.right = r.left + wr;
			r.bottom = r.top + hr;
			dc->Rectangle(&r);
			dc->DrawText(t, strlen(t), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			xr += (int)(20 * mm_5h);
		}

		y += (2.0 * mm_4v);
	}
	dc->SelectObject(pen_save);
	dc->SelectObject(brush_save);
	dc->SetTextColor(color_save);
} // plot_grid()


void
ctg_view_mfc::plot_time(CDC *dc)
{
	double mm_5v = 5.0 * _pixmm_v;
	double mm_5h = 5.0 * _pixmm_v;
	int fhr_height = (int)(((210 - 50) / 10) * mm_5v);
	int y = fhr_height + (int)mm_5v;  // center of time-line

	long curr_sec = _start_sec + (long)((double)_curr_pos / _max_samplerate);
	int min_10_sec = 10 * 60;
	double min_10_pix = 10 * 60 * _max_samplerate * _max_xscale;
	long first_10_sec = min_10_sec - (curr_sec % min_10_sec);
	double first_10_pix = (double)first_10_sec * _max_samplerate * _max_xscale;

	int w = _screen_width - _left_space - _right_space;
	char *sec_txt = new char[10];
	double x = first_10_pix + _left_space;
	long sec = curr_sec + first_10_sec;
	while (x < (w + _left_space))
	{
		format_sec(sec, &sec_txt);			
		char t[100];
		sprintf(t, "%s   (1cm/min)", sec_txt);
		RECT r;
		r.left = (int)x + _left_space - (int)(10 * mm_5h);
		r.top = y + _top_space - 50;
		r.right = r.left + (int)(20 * mm_5h);
		r.bottom = r.top + 100;
		dc->DrawText(t, strlen(t),  &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		
		x += min_10_pix;
		sec += min_10_sec;
	}
	delete[] sec_txt;
} // plot_time()


void
ctg_view_mfc::plot_uc_events(CDC *dc)
{
	if (_num_events == 0)
		return;

	// calc position of markers (below tocogram area)
	double mm_5v = 5.0 * _pixmm_v;
	double mm_5h = 5.0 * _pixmm_v;
	double mm_4v = 4.0 * _pixmm_v;
	int fhr_height = (int)(((210 - 50) / 10) * mm_5v);
	double y = fhr_height + (2 * mm_5v);
	y += ((100 / 10) * mm_4v);

	int *x, num;
	long *pos;
	get_uc_pos(&x, &pos, &num);
	CPen *pen_save = (CPen *)dc->SelectObject(&_uc_event_pen);
	for (int i = 0; i < num; i++)
	{
		dc->MoveTo(x[i], (int)y);
		dc->LineTo(x[i], (int)(y - mm_5v));
	}
	dc->SelectObject(pen_save);

	if (x)
		delete[]x;
	if (pos)
		delete[]pos;
} // plot_uc_events()


void
ctg_view_mfc::OnLButtonDown(UINT nFlags, CPoint point)
{
	CDC *dc = GetDC();

	mouse_press(point.x);
//	_cursor_pos_prev = _cursor_pos;
//	_cursor_pos = point.x;
	plot_cursor(dc);
	ReleaseDC(dc);

	ts_view_mfc::OnLButtonDown(nFlags, point);
} // OnLButtonDown()


