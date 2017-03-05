/*----------------------------------------------------------------------------
 * ra_plot_mfc.cpp
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

#include "ra_plot_mfc.h"
#include "sel_data_dlg_mfc.h"

#include <stdio.h>
#include <ctype.h>

#include <ra_priv.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ra_view_ts

//IMPLEMENT_DYNCREATE(ra_plot_mfc, CWnd )
BEGIN_MESSAGE_MAP(ra_plot_mfc, CWnd)
	//{{AFX_MSG_MAP(ra_plot_mfc)
	ON_WM_KEYDOWN()ON_WM_LBUTTONDOWN()ON_WM_PAINT()ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
	// Standard-Druckbefehle
END_MESSAGE_MAP()


ra_plot_mfc::ra_plot_mfc():CWnd(), ra_plot_general(NULL, NULL)
{
	_save_dc = NULL;
	_save_bm = NULL;
}


ra_plot_mfc::~ra_plot_mfc()
{
	if (_save_dc)
	{
		delete _save_dc;
		_save_dc = NULL;
	}
	if (_save_bm)
	{
		delete _save_bm;
		_save_bm = NULL;
	}
} // destructor


void
ra_plot_mfc::init(meas_handle meas, plugin_handle pl, struct ra_plot_options *opt)
{
	_meas = meas;
	_plugin = pl;
	_opt = opt;

	ra_plot_general::init();
} // init()


void
ra_plot_mfc::update()
{
	get_plot_data();
	Invalidate();
} // update()


void
ra_plot_mfc::OnPaint()
{
	PAINTSTRUCT ps;
	CDC *dc = BeginPaint(&ps);

	RECT r;
	GetClientRect(&r);

	dc->SetBkColor(RGB(255, 255, 255));
	dc->FillRect(&r, NULL);

	_plot_area_width = r.right - (_plot_area_left + _plot_area_right);

	_plot_area_top = 0;
	_plot_legend(dc);
	if (_plot_area_top < 20)
		_plot_area_top = 20;
	_plot_area_top += 10;

	_plot_area_height = r.bottom - (_plot_area_top + _plot_area_bottom);

	_plot_frame(dc);

	plot_annot(dc);

	CRgn rgn;
	rgn.CreateRectRgn(_plot_area_left, _plot_area_top,
			  (r.right - _plot_area_right), (r.bottom - _plot_area_bottom));
	dc->SelectClipRgn(&rgn);
	_plot_data(dc);
	dc->SelectClipRgn(NULL);

	if (_save_dc != NULL)
		delete _save_dc;
	
	_save_dc = new CDC;
	_save_dc->CreateCompatibleDC(dc);

	if (_save_bm != NULL)
		delete _save_bm;
	_save_bm = new CBitmap;

	_save_dc_height = r.bottom - r.top;
	_save_dc_width = r.right - r.left;
	_save_bm->CreateCompatibleBitmap(dc, _save_dc_width, _save_dc_height);
	_save_dc->SelectObject(_save_bm);
	_save_dc->BitBlt(0, 0, _save_dc_width, _save_dc_height, dc, 0, 0, SRCCOPY);

	EndPaint(&ps);

	_plot_pos();
} // OnPaint()


int
ra_plot_mfc::_plot_frame(CDC *dc)
{
	_plot_axis(dc);

	RECT r;
	r.left = _plot_area_left;
	r.top = _plot_area_top;
	r.right = r.left + _plot_area_width;
	r.bottom = r.top + _plot_area_height;
	dc->Rectangle(&r);

//    _plot_grid();

	return 0;
} // _plot_frame()


void
ra_plot_mfc::_plot_axis(CDC *dc)
{
	_plot_x_axis(dc);
	_plot_y_axis(dc);
} // _plot_axis()


void
ra_plot_mfc::_plot_x_axis(CDC *dc)
{
	int y = _plot_area_top + _plot_area_height;

	for (int i = 0; i < _num_x_axis; i++)
	{
		// draw axis-values
		double curr = _x_axis_info[i].start;
		double x_scale =
			(double) _plot_area_width / (_x_axis_info[i].end - _x_axis_info[i].start);
		double step = _x_axis_info[i].step;
		if (_plot_area_width < 200)
			step *= 2.0;

		int precision = get_tics_precision(step);

		CString s;
		while (curr <= _x_axis_info[i].end)
		{
			int x = (int) ((curr - _x_axis_info[i].start) * x_scale) + _plot_area_left;
			dc->MoveTo(x, y);
			dc->LineTo(x, y + 5);

			char t[20];
			switch (precision)
			{
			case 0:
				sprintf(t, "%.0f", curr);
				break;
			case 1:
				sprintf(t, "%.1f", curr);
				break;
			case 2:
				sprintf(t, "%.2f", curr);
				break;
			case 3:
				sprintf(t, "%.3f", curr);
				break;
			case 4:
				sprintf(t, "%.4f", curr);
				break;
			case 5:
				sprintf(t, "%.5f", curr);
				break;
			default:
				sprintf(t, "%f", curr);
				break;
			}

			s = t;

			UINT ta_save = dc->SetTextAlign(TA_CENTER | TA_TOP);
			dc->TextOut(x, y + 7, s);
			dc->SetTextAlign(ta_save);

			curr += step;
		}

		// draw unit
		s = _opt->axis[_x_axis_info[i].idx].unit;

		UINT ta_save = dc->SetTextAlign(TA_RIGHT | TA_TOP);
		dc->TextOut(_plot_area_width + _plot_area_left + _plot_area_right - 2, y + 7, s);
		dc->SetTextAlign(ta_save);
	}
} // _plot_x_axis()


void
ra_plot_mfc::_plot_y_axis(CDC *dc)
{
	for (int i = 0; i < _num_y_axis; i++)
	{
		if (_y_axis_info[i].dont_draw_axis)
			continue;

		int pos;
		if (_y_axis_info[i].opp_side)
		{
			pos = _y_axis_info[i].axis_num * _y_axis_space;
			pos += _plot_area_left + _plot_area_width;
		}
		else
			pos = (_y_axis_info[i].axis_num + 1) * _y_axis_space;

		// draw axis-line
		dc->MoveTo(pos, _plot_area_top);
		dc->LineTo(pos, _plot_area_top + _plot_area_height);

		// draw axis-values
		double curr = _y_axis_info[i].start;
		double y_scale =
			(double) _plot_area_height / (_y_axis_info[i].end - _y_axis_info[i].start);
		double step = _y_axis_info[i].step;
		if (_plot_area_height < 200)
			step *= 2.0;

		CString s;
		while (curr <= _y_axis_info[i].end)
		{
			int y = (int) ((_y_axis_info[i].end - curr) * y_scale) + _plot_area_top;
			dc->MoveTo(pos, y);
			if (_y_axis_info[i].opp_side)
				dc->LineTo(pos + 5, y);
			else
				dc->LineTo(pos - 5, y);

			char t[20];
			if (step >= 1)
				sprintf(t, "%.0f", curr);
			else
				sprintf(t, "%f", curr);
			s = t;

			if (_y_axis_info[i].opp_side)
			{
				UINT ta_save = dc->SetTextAlign(TA_LEFT | TA_BASELINE);
				dc->TextOut(pos + 7, y + 5, s);
				dc->SetTextAlign(ta_save);
			}
			else
			{
				UINT ta_save = dc->SetTextAlign(TA_RIGHT | TA_BASELINE);
				dc->TextOut(pos - 7, y + 5, s);
				dc->SetTextAlign(ta_save);
			}

			curr += step;
		}

		// draw unit
		s = _opt->axis[_y_axis_info[i].idx].unit;
		if (_y_axis_info[i].opp_side)
		{
			UINT ta_save = dc->SetTextAlign(TA_LEFT | TA_BASELINE);
			dc->TextOut(pos + 7, _plot_area_top - 15, s);
			dc->SetTextAlign(ta_save);
		}
		else
		{
			UINT ta_save = dc->SetTextAlign(TA_RIGHT | TA_BASELINE);
			dc->TextOut(pos - 7, _plot_area_top - 15, s);
			dc->SetTextAlign(ta_save);
		}
	}
} // _plot_y_axis()


void
ra_plot_mfc::plot_annot(CDC *dc)
{
	CPen pen(PS_SOLID, 1, RGB(100, 100, 100));
	CPen *pen_save = (CPen *)dc->SelectObject(&pen);

	CBrush br(RGB(220, 220, 220));
	CBrush *brush_save = (CBrush *)dc->SelectObject(&br);

	
	double xs = _x_axis_info->start * _x_axis_info->scale;
	double xe = _x_axis_info->end * _x_axis_info->scale;
	double xr = xe - xs;
	_x_scale = (double) (_plot_area_width) / xr;

	for (long l = 0; l < _num_annot; l++)
	{
		double x1 = _annot_start[l] - xs;
		double x2 = _annot_end[l] - xs;

		int x1_u = (int) (x1 * _x_scale) + _plot_area_left;
		int x2_u = (int) (x2 * _x_scale) + _plot_area_left;

		RECT r;
		r.left = x1_u;
		r.top = _plot_area_top+1;
		r.right = x2_u;
		r.bottom = r.top + _plot_area_height - 2;
		dc->Rectangle(&r);
	}

	dc->SelectObject(pen_save);
	dc->SelectObject(brush_save);
} // plot_annot()


int
ra_plot_mfc::_plot_data(CDC *dc)
{
	for (long l = 0; l < _opt->num_plot_pairs; l++)
	{
		POINT *pts = new POINT[_num_data[l]];

		double xs = _x_axis_info->start * _x_axis_info->scale;
		double xe = _x_axis_info->end * _x_axis_info->scale;
		double ys = _y_axis_info[l].start * _y_axis_info[l].scale;
		double ye = _y_axis_info[l].end * _y_axis_info[l].scale;

		double xr = xe - xs;
		_x_scale = (double) (_plot_area_width) / xr;
		double yr = ye - ys;
		_y_scale = (double) (_plot_area_height) / yr;

		long cnt = 0;
		for (long m = 0; m < _num_data[l]; m++)
		{
			if (_ignore_data[l][m])
			{
				if (cnt > 0)
				{
					_plot_line(dc, &(_opt->pair[l]), pts, cnt);
					_plot_symbol(dc, &(_opt->pair[l]), pts, cnt);
				}
				cnt = 0;
				continue;
			}

			double x = _x_data[l][m] - xs;
			double y = ye - _y_data[l][m];
			pts[cnt].x = (int) (x * _x_scale) + _plot_area_left;
			pts[cnt].y = (int) (y * _y_scale) + _plot_area_top;
			cnt++;
		}
		if (cnt > 0)
		{
			_plot_line(dc, &(_opt->pair[l]), pts, cnt);
			_plot_symbol(dc, &(_opt->pair[l]), pts, cnt);
		}

		delete[] pts;
	}

	return 0;
} // _plot_data()


void
ra_plot_mfc::_plot_line(CDC *dc, struct ra_plot_pair *pair, POINT *pts, int num)
{
	if (!pair->plot_line)
		return;

	CPen pen;

	switch (pair->line_type)
	{
	case RA_PLOT_LINE_SOLID:
		pen.CreatePen(PS_SOLID, 1, RGB(pair->line_r, pair->line_g, pair->line_b));
		break;
	case RA_PLOT_LINE_DASH:
		pen.CreatePen(PS_DASH, 1, RGB(pair->line_r, pair->line_g, pair->line_b));
		break;
	case RA_PLOT_LINE_DOT:
		pen.CreatePen(PS_DOT, 1, RGB(pair->line_r, pair->line_g, pair->line_b));
		break;
	case RA_PLOT_LINE_DASHDOT:
		pen.CreatePen(PS_DASHDOT, 1, RGB(pair->line_r, pair->line_g, pair->line_b));
		break;
	}

	CPen *pen_save = (CPen *) dc->SelectObject(&pen);

	dc->Polyline(pts, num);

	dc->SelectObject(pen_save);
} // _plot_line()


void
ra_plot_mfc::_plot_symbol(CDC * dc, struct ra_plot_pair *pair, POINT * pts, int num)
{
	if (!pair->plot_symbol)
		return;


	CDC dc_sym;
	dc_sym.CreateCompatibleDC(dc);

	CPen pen(PS_SOLID, 1, RGB(pair->sym_r, pair->sym_g, pair->sym_b));
	dc_sym.SelectObject(&pen);

	CBrush br(RGB(pair->sym_r, pair->sym_g, pair->sym_b));

	int size = 5;
	int size2 = size / 2;

	for (int i = 0; i < num; i++)
	{
		int x = pts[i].x - size2;
		int y = pts[i].y - size2;

		switch (pair->symbol_type)
		{
		case RA_PLOT_SYM_CIRCLE_FULL:
			dc->SelectObject(&br);
		case RA_PLOT_SYM_CIRCLE:
			dc->Ellipse(x, y, x + size, y + size);
			break;
		case RA_PLOT_SYM_RECT_FULL:
			dc->SelectObject(&br);
		case RA_PLOT_SYM_RECT:
			dc->Rectangle(x, y, x + size, y + size);
			break;
		case RA_PLOT_SYM_DIAMOND_FULL:
			dc->SelectObject(&br);
		case RA_PLOT_SYM_DIAMOND:
			dc->MoveTo(x + size2, y);
			dc->LineTo(x + size - 1, y + size2);
			dc->LineTo(x + size2, y + size - 1);
			dc->LineTo(x, y + size2);
			dc->LineTo(x + size2, y);
			break;
		case RA_PLOT_SYM_TRIANGLE_FULL:
			dc->SelectObject(&br);
		case RA_PLOT_SYM_TRIANGLE:
			dc->MoveTo(x, y + size - 1);
			dc->LineTo(x + size2, y);
			dc->LineTo(x + size - 1, y + size - 1);
			dc->LineTo(x, y + size - 1);
			break;
		}
	}

//    sym.setMask(sym.createHeuristicMask());
/*    for (int i = 0; i < num; i++)
    {
		int x = pts[i].x;
		int y = pts[i].y;

		dc->BitBlt(x-size2, y-size2, size, size, &dc_sym, 0, 0, SRCCOPY);
    }*/
} // _plot_symbol()


int
ra_plot_mfc::_plot_legend(CDC * dc)
{
	if (!(_opt->show_legend))
		return 0;

	int curr_x = _plot_area_left + 5;
	int curr_line = 1;
	int cnt = 0;		// no new line if this is the first

	UINT ta_save = dc->SetTextAlign(TA_BASELINE);
	for (long l = 0; l < _opt->num_plot_pairs; l++)
	{
		int y = curr_line * 20 - 5;

		CString s = _opt->pair[l].name;
		s += " [";
		s += _opt->axis[_opt->pair[l].y_idx].unit;
		s += "]";

		CSize size = dc->GetTextExtent(s);

		if (((curr_x + size.cx + 15) > (_plot_area_left + _plot_area_width)) && cnt)
		{
			curr_x = _plot_area_left + 5;
			curr_line++;
			y = curr_line * 20 - 10;
		}
		dc->TextOut(curr_x + 15, y + 5, s);

		POINT pts[2];
		pts[0].x = curr_x + 5;
		pts[0].y = y;
		_plot_symbol(dc, &(_opt->pair[l]), pts, 1);

		pts[0].x = curr_x;
		pts[0].y = y;
		pts[1].x = curr_x + 10;
		pts[1].y = y;
		_plot_line(dc, &(_opt->pair[l]), pts, 2);

		curr_x += (size.cx + 20);
		cnt++;
	}
	dc->SetTextAlign(ta_save);

	_plot_area_top = 20 * curr_line;

	return 0;
} // _plot_legend()


int
ra_plot_mfc::_plot_pos()
{
	RECT r;
	GetClientRect(&r);

	CDC *dc = GetDC();

	dc->BitBlt(0, 0, _save_dc_width, _save_dc_height, _save_dc, 0, 0, SRCCOPY);
	
	int pos = (int) ((double) _curr_pos * _x_scale);
	pos += _plot_area_left;

	CPen pen;
	pen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	CPen *pen_save = (CPen *)dc->SelectObject(&pen);

	dc->MoveTo(pos, _plot_area_top);
	dc->LineTo(pos, r.bottom - _plot_area_bottom);
	dc->SelectObject(pen_save);

	ReleaseDC(dc);

	return 0;
} // _plot_pos()


void
ra_plot_mfc::OnLButtonDown(UINT nFlags, CPoint point)
{
	long pos = (long) ((double) (point.x - _plot_area_left) / _x_scale);
	double xs = _x_axis_info->start * _x_axis_info->scale;
	pos += (long) xs;
	long event = 0;
	for (int i = 0; i < _num_data[0]; i++)
	{
		if (_x_data[0][i] >= pos)
		{
			event = i;
			break;
		}
	}
	set_event(event);
	_plot_pos();
} // OnLButtonDown()


void
ra_plot_mfc::OnRButtonDown(UINT nFlags, CPoint point)
{
	struct plugin_struct *pl = (struct plugin_struct *) _plugin;
	HMODULE mod = LoadLibrary(pl->info.file);
	HRSRC res = FindResource(mod, (LPCTSTR) IDD_SELECT_DATA_DLG, RT_DIALOG);
	HGLOBAL g = LoadResource(mod, res);
	LPVOID p = LockResource(g);

	sel_data_dlg_mfc dlg;
	dlg.init(_meas, _plugin, _opt);
	dlg.InitModalIndirect(p, this);
	if (dlg.DoModal() == IDOK)
		update();
	FreeLibrary(mod);
} // OnRButtonDown()


void
ra_plot_mfc::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	bool ctrl;
	if (GetKeyState(VK_CONTROL) < 0)
		ctrl = true;
	else
		ctrl = false;

	long curr = _curr_event;
	bool changed = false;
	switch (nChar)
	{
	case VK_RIGHT:
		changed = true;
		if (ctrl)
			curr += 20;
		else
			curr++;
		break;
	case VK_LEFT:
		changed = true;
		if (ctrl)
			curr -= 20;
		else
			curr--;
		break;
	case VK_HOME:
		changed = true;
		curr = 0;
		break;
	case VK_END:
		changed = true;
		curr = _num_data[0];
		break;
	}

	if (changed)
	{
		set_event(curr);
		_plot_pos();
	}
} // OnKeyDown()


BOOL
ra_plot_mfc::DestroyWindow() 
{
	if (_save_dc)
	{
		delete _save_dc;
		_save_dc = NULL;
	}
	if (_save_bm)
	{
		delete _save_bm;
		_save_bm = NULL;
	}
	
	return CWnd::DestroyWindow();
}  // DestroyWindow()
