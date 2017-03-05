/*----------------------------------------------------------------------------
 * ts_view_mfc.cpp
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
#include "ts_view_mfc.h"
#include "gen_opt_dlg_mfc.h"
#include "annot_dlg_mfc.h"
#include "ts_view_ch_opt_dlg_mfc.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ra_view_ts

//IMPLEMENT_DYNCREATE(ts_view_mfc, CWnd )
BEGIN_MESSAGE_MAP(ts_view_mfc, CWnd)
	//{{AFX_MSG_MAP(ts_view_mfc)
	ON_WM_KEYDOWN()
	ON_WM_PAINT()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(IDM_GENERAL_OPTIONS, _general_options)
	ON_COMMAND(IDM_CHANNEL_OPTIONS, _ch_options)
	ON_COMMAND(IDM_SORT_ANNOT, sort_annot)
	ON_COMMAND(IDM_FILTER_PLNOISE, filter_powerline)
	ON_COMMAND(IDM_FILTER_MEAN, filter_mean)
	ON_WM_MOUSEWHEEL()
	ON_WM_HSCROLL()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
	// Standard-Druckbefehle
	END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// ra_view_ts Konstruktion/Destruktion
ts_view_mfc::ts_view_mfc():ts_view_general(NULL)
{
	_popup_menu.CreatePopupMenu();
	_popup_menu.AppendMenu(MF_STRING, IDM_GENERAL_OPTIONS, gettext("&General Options..."));
	_popup_menu.AppendMenu(MF_STRING, IDM_CHANNEL_OPTIONS, gettext("&Channel Options..."));
	_popup_menu.AppendMenu(MF_STRING, IDM_SORT_ANNOT, gettext("&Sort Annotations"));

	_popup_menu_filter.CreatePopupMenu();
	_popup_menu_filter.AppendMenu(MF_STRING, IDM_FILTER_PLNOISE, gettext("Remove &Powerline Noise"));
	_popup_menu_filter.AppendMenu(MF_STRING, IDM_FILTER_MEAN, gettext("Remove Signal-&Mean"));

	_popup_menu.AppendMenu(MF_SEPARATOR, 0, "");
	_popup_menu.AppendMenu(MF_POPUP, (UINT)_popup_menu_filter.GetSafeHmenu(), gettext("Filter"));


	_popup_menu_filter.CheckMenuItem(IDM_FILTER_PLNOISE,
									 (_filter_powerline_noise ? MF_CHECKED : MF_UNCHECKED));
	_popup_menu_filter.CheckMenuItem(IDM_FILTER_MEAN,
									 (_remove_mean ? MF_CHECKED : MF_UNCHECKED));
}

ts_view_mfc::~ts_view_mfc()
{
	if (_font != NULL)
		DeleteObject(_font);
	if (_font_bold != NULL)
		DeleteObject(_font_bold);
	if (_ch)
		delete[] _ch;
}

BOOL
ts_view_mfc::PreCreateWindow(CREATESTRUCT & cs)
{
	// ZU ERLEDIGEN: Ändern Sie hier die Fensterklasse oder das Erscheinungsbild, indem Sie
	//  CREATESTRUCT cs modifizieren.

	return CWnd::PreCreateWindow(cs);
}


/////////////////////////////////////////////////////////////////////////////
// ra_view_ts Diagnose

#ifdef _DEBUG
void
ts_view_mfc::AssertValid() const
{
	CWnd::AssertValid();
}

void
ts_view_mfc::Dump(CDumpContext & dc) const
{
	CWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// ra_view_ts Nachrichten-Handler

void
ts_view_mfc::update()
{
	Invalidate();
} // update()


void
ts_view_mfc::init(meas_handle mh, plugin_handle pl, int num_ch, int *ch)
{
	_plugin = pl;
	if (num_ch > 0)
	{
		_num_ch = num_ch;
		_ch = new struct ch_info[num_ch];
		memset(_ch, 0, sizeof(struct ch_info) * num_ch);
		for (int i = 0; i < num_ch; i++)
		{
			_ch[i].fixed_top_win_pos = -1;
			_ch[i].ch_number = ch[i];
		}
	}

	ts_view_general::init(mh);

	_bk_color = RGB(254, 251, 197);
} // init()


void
ts_view_mfc::OnPaint()
{
	CPaintDC dc(this);	// device context for painting

	OnDraw(&dc);
} // OnPaint()



void
ts_view_mfc::OnDraw(CDC * pDC)
{
	pDC->SetBkColor(_bk_color);
	RECT rect;
	GetClientRect(&rect);
	pDC->FillRect(&rect, &_bk_brush);

	_screen_width = rect.right - rect.left;
	_screen_height = rect.bottom - rect.top;

	paint(_screen_width, _screen_height);

	plot_grid(pDC);

	plot_statusbar(pDC);

	int line = 0;
	for (int i = 0; i < _num_ch; i++)
	{
		if (_ch[i].show_it)
		{
			plot_channel(pDC, i, line);
			line++;
		}
	}

	print_comment_annotations(pDC);

	_highlight_start = -1;
	if (_area_marked || (_area_mark_step == MARKING_AREA))
		highlight_area();

	_cursor_pos_prev = -1;
	plot_cursor(pDC);
} // OnDraw()


void
ts_view_mfc::plot_grid(CDC * dc)
{
	if (!_ts_opt.plot_grid)
		return;

	// width and height in cm
	int h = _screen_height - _top_space - _bottom_space;
	int w = _screen_width - _left_space - _right_space;
	int n_lines = (int) ((double) h / (_pixmm_v * 10.0));
	int n_col = (int) ((double) w / (_pixmm_h * 10.0));

	CPen *save = (CPen *) dc->SelectObject(_grid_pen);
	// plot horizontal lines
	int i;
	for (i = 0; i <= n_lines; i++)
	{
		int y = (int) ((_pixmm_v * 10.0) * (double) i) + _top_space;
		dc->MoveTo(_left_space, y);
		dc->LineTo(_screen_width - _right_space, y);
	}
	// plot vertical lines
	for (i = 0; i <= n_col; i++)
	{
		int x = (int) ((_pixmm_h * 10.0) * (double) i) + _left_space;
		dc->MoveTo(x, _top_space);
		dc->LineTo(x, _screen_height - _bottom_space);
	}
} // plot_grid()


void
ts_view_mfc::plot_statusbar(CDC * dc)
{
	if (!_ts_opt.show_status)
		return;

	char *status_text = NULL;
	format_statusbar(&status_text);

	if (status_text)
	{
		RECT rect;
		rect.left = _left_space;
		rect.top = _screen_height - _bottom_space;
		rect.right = _screen_width - _right_space;
		rect.bottom = _screen_height;
		dc->FillRect(&rect, &_bk_brush);

		dc->SelectObject(_font);
		UINT ta_save = dc->SetTextAlign(TA_LEFT | TA_BASELINE);
		dc->TextOut(1 + _left_space, _screen_height - 5, status_text, strlen(status_text));
		dc->SetTextAlign(ta_save);

		delete[] status_text;
	}

	if (_num_sort)
	{
		char t[100];
		sprintf(t, "%ld / %ld", (_curr_sort + 1), _num_sort);
		UINT ta_save = dc->SetTextAlign(TA_RIGHT | TA_BASELINE);
		dc->TextOut(_screen_width - _right_space, _screen_height - 5, t, strlen(t));
		dc->SetTextAlign(ta_save);
	}
} // plot_statusbar()


void
ts_view_mfc::plot_channel(CDC * dc, int ch, int line)
{
	int *x, *y, *ignore, num;
	ch_draw_data(ch, line, &x, &y, &ignore, &num);

	dc->SelectObject(_ch_pen);
	dc->SelectObject(_font);

	if (num > 0)
	{
		CPoint *pts = new CPoint[num];
		for (int i = 0; i < num; i++)
		{
			pts[i].x = x[i];
			pts[i].y = y[i];
		}

		int start = 0;
		int num_draw = 0;
		int curr = 0;
		while (curr < num)
		{
			if (ignore[curr] == 1)
			{
				if (num_draw > 0)
					dc->Polyline(&(pts[start]), num_draw);
				num_draw = 0;
				start = curr + 1;
			}
			else
				num_draw++;
			curr++;
		}
		if (num_draw > 0)
			dc->Polyline(&(pts[start]), num_draw);

		delete[] x;
		delete[] y;
		delete[] ignore;
		delete[] pts;

		if (_ts_opt.show_ch_info)
		{
			// TODO: make font member; calc textposition
			if (_ch[ch].inverse)
			{
				CString s;
				s = "-";
				s += _ch[ch].name;
				dc->TextOut(_ch_info_offset, (line * _ts_opt.ch_height + _ts_opt.ch_height / 2) + 15, s);
			}
			else
			{
				dc->TextOut(_ch_info_offset, (line * _ts_opt.ch_height + _ts_opt.ch_height / 2) + 15,
					   _ch[ch].name);
			}

			char t[20];
			sprintf(t, "[%s]", _ch[ch].unit);
			dc->TextOut(_ch_info_offset, (line * _ts_opt.ch_height + _ts_opt.ch_height / 2) + 30, t, strlen(t));
		}
	}
} // plot_channel()


void
ts_view_mfc::print_comment_annotations(CDC * dc)
{
	struct curr_annot_info *info = NULL;
	int n_comments = 0;

	if (_aoi)
	{
		delete[] _aoi;
		_aoi = NULL;
		_num_aoi = 0;
	}

	get_comments(&info, &n_comments);
	
	place_comments(info, n_comments);

	if (n_comments > 0)
	{
		_num_aoi = n_comments;
		_aoi = new struct aoi[_num_aoi];

		for (int i = 0; i < n_comments; i++)
		{
			int x = info[i].text_x;
			int y = info[i].y;
			
			if (y != -1)
			{
				size_t len = strlen(info[i].text) * 2;
				char *buf = new char[len];
				utf8_to_local(info[i].text, buf, len);
				dc->TextOut(x, y, buf, strlen(buf));
				delete[] buf;

				_aoi[i].x = x;
				_aoi[i].y = y;
				_aoi[i].w = info[i].text_width;
				_aoi[i].h = info[i].text_height;
				_aoi[i].annot = info[i].annot_idx;
			}
			else
				_aoi[i].x = -1;

			int begin = info[i].begin;
			int end = info[i].end;

			// if begin is outside of shown area, plot arrow (instead of vertical line)
			if (info[i].begin_not_shown)
			{
				dc->MoveTo(_left_space, y+2);
				dc->LineTo(_left_space+3, y-1);
				dc->MoveTo(_left_space, y+2);
				dc->LineTo(_left_space+3, y+5);
				begin = _left_space;
			}
			else
			{
				dc->MoveTo(info[i].begin, info[i].y+4);
				dc->LineTo(info[i].begin, info[i].y);
			}
			if (info[i].end_not_shown)
			{
				long draw_end = _screen_width - _right_space;
				dc->MoveTo(draw_end, y+2);
				dc->LineTo(draw_end-3, y-1);
				dc->MoveTo(draw_end, y+2);
				dc->LineTo(draw_end-3, y+5);
				end = draw_end;
			}
			else
			{
				dc->MoveTo(info[i].end, info[i].y+4);
				dc->LineTo(info[i].end, info[i].y);
			}
			
			if (info[i].end > info[i].begin)
			{
				dc->MoveTo(begin, info[i].y+2);
				dc->LineTo(end, info[i].y+2);
			}
			else
			{
				dc->MoveTo(begin-4, info[i].y+2);
				dc->LineTo(end+4, info[i].y+2);
			}
		}
	}

	if (info)
	{
		for (int i = 0; i < n_comments; i++)
			delete[] info[i].text;

		free(info);
	}
} // print_comment_annotations()


void
ts_view_mfc::get_text_extent(char *text, int *w, int *h)
{
	CDC *dc = GetDC();
	CSize size = dc->GetTextExtent(text, strlen(text));
	ReleaseDC(dc);
	*w = size.cx;
	*h = size.cy;
} // get_text_extent()


// TODO: get rid of all these +-_left_space so it is easier to understand (at least for me)
void
ts_view_mfc::highlight_area()
{
	CDC *dc = GetDC();

	int h = _screen_height - _top_space - _bottom_space;

	// "remove" previously drawn area
	if (_highlight_start >= 0)
	{
		dc->PatBlt(_highlight_start, _top_space, _highlight_width, h, PATINVERT);
		_highlight_start = -1;
	}

	if (!_area_marked && (_area_start < 0))
	{
		ReleaseDC(dc);
		return;
	}

	CBrush brush(RGB(255,255,255));
	CBrush *brush_save = (CBrush *)dc->SelectObject(&brush);
	
	int start = su_to_screen(_area_start);
	int end = su_to_screen(_area_end);

	int w = _screen_width - _left_space - _right_space;
	if (((start < _left_space) && (end < _left_space)) ||
	    ((start > (w + _left_space)) && (end > (w + _left_space))))
	{
		dc->SelectObject(brush_save);
		ReleaseDC(dc);
		return;
	}
	
	if (start < _left_space)
		start = _left_space;
	if (end > (w + _left_space))
		end = (w + _left_space);
	int width = end - start;

	_highlight_start = start;
	if (_highlight_start < _left_space)
		_highlight_start = _left_space;

	_highlight_width = width;
	if (_highlight_width > (w - (_highlight_start - _left_space)))
		_highlight_width = w - (_highlight_start - _left_space);
	
	dc->PatBlt(_highlight_start, _top_space, _highlight_width, h, PATINVERT);

	dc->SelectObject(brush_save);
	ReleaseDC(dc);
} // highlight_area()


void
ts_view_mfc::plot_cursor(CDC * dc, int ch/* = -1*/)
{
	if (!_ts_handle_cursor)
		return;

	if (_cursor_pos_prev != -1)
	{
		if (_cursor_ch_prev == -1)
		{
			dc->PatBlt(_cursor_pos_prev, _top_space, 1,
						_screen_height - (_bottom_space+_top_space), PATINVERT);
		}
		else
		{
			dc->PatBlt(_cursor_pos_prev, _ch[_cursor_ch_prev].top_win_pos + _top_space,
				   1, _ts_opt.ch_height, PATINVERT);
		}

		_cursor_pos_prev = -1;
		_cursor_ch_prev = -1;
	}
	
	if ((_cursor_pos < _left_space) || (_cursor_pos > (_screen_width - _right_space)))
		return;

	if (ch == -1)
	{
		dc->PatBlt(_cursor_pos, _top_space, 1,
					_screen_height - (_bottom_space+_top_space), PATINVERT);
	}
	else
	{
		dc->PatBlt(_cursor_pos, _ch[ch].top_win_pos + _top_space, 1, _ts_opt.ch_height, PATINVERT);
	}

	_cursor_pos_prev = _cursor_pos;
	_cursor_ch_prev = ch;
} // plot_cursor()


void
ts_view_mfc::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	bool shift;
	if (GetKeyState(VK_SHIFT) < 0)
		shift = true;
	else
		shift = false;

	bool ctrl;
	if (GetKeyState(VK_CONTROL) < 0)
		ctrl = true;
	else
		ctrl = false;

	// get char for some key's
	char c = nChar;
	switch (nChar)
	{
	case 0xBB: c = '+'; break; 
	case 0xBC: c = ','; break;
	case 0xBD: c = '-'; break;
	case 0xBE: c = '.';break;
	}
	
	bool changed = false;
	switch (nChar)
	{
	case VK_RIGHT:
		changed = next_page(shift);
		break;
	case VK_LEFT:
		changed = prev_page(shift);
		break;
	case VK_PRIOR:
		changed = prev_page(true);
		break;
	case VK_NEXT:
		changed = next_page(true);
		break;
	case VK_HOME:
		changed = goto_begin();
		break;
	case VK_END:
		changed = goto_end();
		break;
	case VK_UP:
		if (_curr_session > 0)
			change_session(_curr_session - 1);
		break;
	case VK_DOWN:
		if (_curr_session < (_num_sessions - 1))
			change_session(_curr_session + 1);
		break;
	default:
		changed = key_press_char(c, shift, ctrl);
		break;
	}

	if (changed)
		Invalidate();
} // OnKeyDown()



void
ts_view_mfc::_general_options()
{
	struct plugin_struct *pl = (struct plugin_struct *) _plugin;
	HMODULE mod = LoadLibrary(pl->info.file);
	HRSRC res = FindResource(mod, (LPCTSTR) IDD_GENERAL_OPT_DLG, RT_DIALOG);
	HGLOBAL g = LoadResource(mod, res);
	LPVOID p = LockResource(g);

	double mm_sec_save = _ts_opt.mm_per_sec;

	gen_opt_dlg_mfc dlg;
	dlg.set_opt(&_ts_opt);
	dlg.InitModalIndirect(p);
	if (dlg.DoModal() == IDOK)
	{
		options_changed();
		if (mm_sec_save != _ts_opt.mm_per_sec)
			send_x_res((long)GetParent());
	}
	FreeResource(g);

	Invalidate();
} // _general_options()


void
ts_view_mfc::_ch_options()
{
	struct plugin_struct *pl = (struct plugin_struct *) _plugin;
	HMODULE mod = LoadLibrary(pl->info.file);
	HRSRC res = FindResource(mod, (LPCTSTR) IDD_CH_OPT_DLG, RT_DIALOG);
	HGLOBAL g = LoadResource(mod, res);
	LPVOID p = LockResource(g);

	ts_view_ch_opt_dlg_mfc dlg;
	dlg.init(&_ch, _num_ch, _curr_channel);
	dlg.InitModalIndirect(p);
	if (dlg.DoModal() == IDOK)
		options_changed();
	FreeResource(g);

	Invalidate();
} // _ch_options()


void
ts_view_mfc::sort_annot()
{
	sort_annotations();
	Invalidate();
}  // sort_annot()


void
ts_view_mfc::OnRButtonDown(UINT nFlags, CPoint point)
{
	POINT p;
	p.x = point.x;
	p.y = point.y;
	if (_ts_opt.ch_height > 0)
		_curr_channel = p.y / _ts_opt.ch_height;
	else
		_curr_channel = 0;

	ClientToScreen(&p);
	_popup_menu.
		TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, p.x, p.y, this);

	CWnd::OnRButtonDown(nFlags, point);
} // OnRButtonDown()


BOOL
ts_view_mfc::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName,
		    DWORD dwStyle, const RECT & rect, CWnd * pParentWnd,
		    UINT nID, CCreateContext * pContext)
{	
	BOOL ok;
	ok = CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
	if (!ok)
		return ok;

	if (dwStyle & WS_HSCROLL)
		_use_scrollbar = true;

	// get resolution infos
	CDC *dc = GetDC();
	_pixmm_h = (double) dc->GetDeviceCaps(HORZRES) / (double) dc->GetDeviceCaps(HORZSIZE);
	_pixmm_v = (double) dc->GetDeviceCaps(VERTRES) / (double) dc->GetDeviceCaps(VERTSIZE);
	ReleaseDC(dc);

	// Brush for background
	LOGBRUSH LogBrush;
	LogBrush.lbStyle = BS_SOLID;
	LogBrush.lbColor = _bk_color;
	LogBrush.lbHatch = 0;
	_bk_brush.CreateBrushIndirect(&LogBrush);

	_plot_grid = true;
	_grid_pen.CreatePen(PS_SOLID, 1, RGB(255, 170, 170));
	_ch_pen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));

	LOGFONT logFont;
	memset(&logFont, 0, sizeof(LOGFONT));
	// 10pt-Font erzeugen
	logFont.lfHeight = -(int) (10.0 * (25.4 / 72.0) * _pixmm_h);
	logFont.lfWeight = FW_NORMAL;
	logFont.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
	if (_font != NULL)
		DeleteObject(_font);
	_font = CreateFontIndirect(&logFont);

	memset(&logFont, 0, sizeof(LOGFONT));
	// 10pt-Font erzeugen
	logFont.lfHeight = -(int) (10.0 * (25.4 / 72.0) * _pixmm_h);
	logFont.lfWeight = FW_BOLD;
	logFont.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
	if (_font_bold != NULL)
		DeleteObject(_font_bold);
	_font_bold = CreateFontIndirect(&logFont);

	for (int i = 0; i < _num_ch; i++)
		calc_scale(i);
	calc_max_scale_factors();

	return ok;
} // Create()


BOOL
ts_view_mfc::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	bool shift = false;
	if (nFlags & MK_SHIFT)
		shift = true;

	bool changed = false;
	if (zDelta > 0)  // go back
		changed = prev_page(shift);
	if (zDelta < 0)
		changed = next_page(shift);

	if (changed)
		Invalidate();

	return TRUE;
} // OnMouseWheel()


void
ts_view_mfc::set_x_resolution(double x_res, void *parent)
{
	if ((CWnd *)parent != GetParent())
		return;

	_ts_opt.mm_per_sec = x_res;
	options_changed();

	update();
} // set_x_resolution


void
ts_view_mfc::init_scrollbar(long page_width, long num_samples, double max_xscale)
{
	if (!_use_scrollbar)
		return;

	double num_steps = ((double)(num_samples - page_width) / (double)page_width) * 10.0;
	if (((double)((long)num_steps) - num_steps) > 0.0)
		num_steps++;

	if (num_steps <= 0)
		return;

	SetScrollRange(SB_HORZ, 0, (int)num_steps - 10);
} // init_scrollbar()


void
ts_view_mfc::set_scrollbar_pos(long pos)
{
	if (!_use_scrollbar)
		return;

	_block_signals = true;

	SetScrollPos(SB_HORZ, (int)(((double)pos / (double)_page_width) * 10.0));

	_block_signals = false;
} // set_scrollbar_pos()


void
ts_view_mfc::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if (!_use_scrollbar || _block_signals)
		return;

	_block_signals = true;

	long pos_save = _curr_pos;
	bool changed = false;
	switch (nSBCode)
	{
	case SB_LEFT:
		changed = goto_begin();
		break;
	case SB_RIGHT:
		changed = goto_end();
		break;
	case SB_LINELEFT:
		changed = prev_page(false);
		break;
	case SB_LINERIGHT:
		changed = next_page(false);
		break;
	case SB_PAGELEFT:
		changed = prev_page(true);
		break;
	case SB_PAGERIGHT:
		changed = next_page(true);
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		_curr_pos = (long)((double)((int)nPos) / 10.0) * _page_width;
		check_curr_pos();
		break;
	case SB_ENDSCROLL:
	default:
		_block_signals = false;
		return;
	}

	if (_curr_pos != pos_save)
		changed = true;

	SetScrollPos(SB_HORZ, (int)(((double)_curr_pos / _page_width) * 10.0));

	if (changed)
	{
		_cursor_pos -= (_curr_pos - pos_save);
		Invalidate();
	}

	struct comm_para *para = new struct comm_para[2];
	para[0].type = PARA_LONG;
	para[0].v.l = _curr_pos;
	para[1].type = PARA_DOUBLE;
	para[1].v.d = _max_xscale;
	ra_comm_emit(_mh, "ts-view-mfc", "pos-change", 2, para);
	delete[] para;

	_block_signals = false;

	CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
} // OnHScroll()


void
ts_view_mfc::OnLButtonDown(UINT nFlags, CPoint point) 
{
	_cursor_pos = point.x;

	CDC *dc = GetDC();
	plot_cursor(dc);
	ReleaseDC(dc);

	SetCapture();
	y_to_channel(point.y);
	if (_area_mark_step != MARKING_AREA_MANUAL)
		start_area_mark(point.x);
	highlight_area();
	
	CWnd::OnLButtonDown(nFlags, point);
} // OnLButtonDown()


void
ts_view_mfc::OnLButtonUp(UINT nFlags, CPoint point) 
{
	ReleaseCapture();
	if (_area_mark_step == MARKING_AREA_MANUAL)
		change_area_mark(point.x);
	else
		end_area_mark(point.x);
	
	CWnd::OnLButtonUp(nFlags, point);
} // OnLButtonUp()


void
ts_view_mfc::OnMouseMove(UINT nFlags, CPoint point) 
{
	static bool x_outside = false;
	static bool in_change = false;

	if ((_area_mark_step != MARKING_AREA_START) && (_area_mark_step != MARKING_AREA))
	{
		x_outside = in_change = false;
		CWnd::OnMouseMove(nFlags, point);
		return;
	}

	if ((point.x > (_screen_width - _right_space)) || (point.x < _left_space))
		x_outside = true;
	else
		x_outside = false;

	if (in_change)
	{
		CWnd::OnMouseMove(nFlags, point);
		return;
	}

	in_change = true;
	long as, ae;
	do
	{
		as = _area_start;
		ae = _area_end;

		change_area_mark(point.x);
		process_msgs();

		if ((as == _area_start) && (ae == _area_end))
			break;
	} while (x_outside && (_area_mark_step == MARKING_AREA));

	in_change = false;

	CWnd::OnMouseMove(nFlags, point);
} // OnMouseMove()


void
ts_view_mfc::process_msgs()
{
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}  // process_msgs()


void
ts_view_mfc::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	int idx = find_aoi(point.x, point.y);
	if (idx < 0)
		return;

	if (_aoi[idx].annot != -1)
	{
		struct annot_info *curr = &(_annot[_aoi[idx].annot]);

		struct plugin_struct *pl = (struct plugin_struct *) _plugin;
		HMODULE mod = LoadLibrary(pl->info.file);
		HRSRC res = FindResource(mod, (LPCTSTR)IDD_ANNOT_DLG, RT_DIALOG);
		HGLOBAL g = LoadResource(mod, res);
		LPVOID p = LockResource(g);

		annot_dlg_mfc dlg;
		dlg.set_opt(_ch, _num_ch, _curr_channel, _annot_predefs, _num_annot_predefs, curr->text,
					curr->noise_flag, curr->ignore_flag, curr->ch);
		dlg.InitModalIndirect(p);
		int ret = 0;
		if (dlg.DoModal() == IDCANCEL)
		{
			FreeResource(g);
			return;
		}
		else if (dlg.del_annot())
		{
			delete_comment(_aoi[idx].annot);
			FreeResource(g);
			update();
			return;
		}
		else		// user pressed 'OK'
		{
			CString s = dlg.annot();
			if (curr->text)
			{
				delete[] curr->text;
				curr->text = NULL;
			}
			size_t len = s.GetLength() * 2;
			char *buf = new char[len];
			strcpy(buf, (const char *)s);
			local_to_utf8_inplace(buf, len);
			curr->text = new char[strlen(buf) + 1];
			strcpy(curr->text, buf);
			
			curr->noise_flag = (dlg.is_noise() ? 1 : 0);
			curr->ignore_flag = (dlg.do_ignore() ? 1 : 0);
			curr->ch = dlg.channel();
			
			FreeResource(g);

			update_comment(_aoi[idx].annot);
			update();
		}
	}
} // OnLButtonDblClk()


int
ts_view_mfc::annot_dlg(char **annot, bool *noise, bool *ignore, int *ch)
{
	struct plugin_struct *pl = (struct plugin_struct *) _plugin;
	HMODULE mod = LoadLibrary(pl->info.file);
	HRSRC res = FindResource(mod, (LPCTSTR)IDD_ANNOT_DLG, RT_DIALOG);
	HGLOBAL g = LoadResource(mod, res);
	LPVOID p = LockResource(g);

	annot_dlg_mfc dlg;
	dlg.set_opt(_ch, _num_ch, _curr_channel, _annot_predefs, _num_annot_predefs, NULL, false, false, -1);
	dlg.InitModalIndirect(p);
	int ret = 0;
	if (dlg.DoModal() == IDOK)
	{
		CString s = dlg.annot();
		*annot = new char[s.GetLength() + 1];
		strcpy(*annot, (const char *)s);
		(*annot)[s.GetLength()] = '\0';

		*noise = (dlg.is_noise() ? 1 : 0);
		*ignore = (dlg.do_ignore() ? 1 : 0);
		*ch = dlg.channel();
	}
	else
		ret = -1;

	FreeResource(g);

	return ret;
} // annot_dlg


void
ts_view_mfc::filter_powerline()
{
	_filter_powerline_noise = (_filter_powerline_noise ? false : true);
	_popup_menu_filter.CheckMenuItem(IDM_FILTER_PLNOISE,
									 (_filter_powerline_noise ? MF_CHECKED : MF_UNCHECKED));

	update();
} // slot_powerline_filter()


void
ts_view_mfc::filter_mean()
{
	_remove_mean = (_remove_mean ? false : true);
	_popup_menu_filter.CheckMenuItem(IDM_FILTER_MEAN,
									 (_remove_mean ? MF_CHECKED : MF_UNCHECKED));
	update();
} // slot_remove_mean()
