/*----------------------------------------------------------------------------
 * ra_view_mfc.cpp
 *
 * 
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002-2004, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Header: /home/cvs_repos.bak/librasch/plugins/view/ra_view/ra_view_mfc.cpp,v 1.8 2004/06/02 13:36:35 rasch Exp $
 *--------------------------------------------------------------------------*/

#include "stdafx.h"

#include "ra_view_mfc.h"

#include <ra_pl_comm.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ra_view_ts

//IMPLEMENT_DYNCREATE(ra_view_mfc, CMDIChildWnd )
BEGIN_MESSAGE_MAP(ra_view_mfc, CMDIChildWnd)
	//{{AFX_MSG_MAP(ra_view_mfc)
	ON_WM_SIZE()
	ON_WM_HSCROLL()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
	// Standard-Druckbefehle
	END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// ra_view_mfc Konstruktion/Destruktion
ra_view_mfc::ra_view_mfc():CMDIChildWnd(), ra_view_general(NULL)
{
	_scroll_pos = 0;
}				// constructor


BOOL
ra_view_mfc::PreCreateWindow(CREATESTRUCT & cs)
{
	// ZU ERLEDIGEN: Ändern Sie hier die Fensterklasse oder das Erscheinungsbild, indem Sie
	//  CREATESTRUCT cs modifizieren.

	return CMDIChildWnd::PreCreateWindow(cs);
}


/////////////////////////////////////////////////////////////////////////////
// ra_view_mfc Diagnose

#ifdef _DEBUG
void
ra_view_mfc::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void
ra_view_mfc::Dump(CDumpContext & dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// ra_view_mfc Nachrichten-Handler

void
ra_view_mfc::init(rec_handle rec, plugin_handle pl)
{
	_rec = rec;
	_meas = ra_meas_handle_from_any_handle(_rec);
	_ra = ra_lib_handle_from_any_handle(_meas);

	_plugin = pl;

	ra_view_general::init();
}				// set_rec_handle()


void
ra_view_mfc::do_update()
{
}				// do_update()


void *
ra_view_mfc::create_main_view()
{
	Create(NULL, "ra-view");

	return this;
}				// create_main_view()


int
ra_view_mfc::get_view_dimensions(void *vh, long *width, long *height)
{
	CWnd *v = (CWnd *) vh;
	RECT r;
	v->GetWindowRect(&r);

	*width = r.right - r.left;
	*height = r.bottom - r.top;

	return 0;
}				// get_view_dimensions()


int
ra_view_mfc::pos_view(void *vh, long x, long y)
{
	CWnd *v = (CWnd *) vh;
	RECT r;
	v->GetWindowRect(&r);

	r.left = x;
	r.top = y;

	v->MoveWindow(&r);

	return 0;
}				// pos_view()


void
ra_view_mfc::show_focus(int prev_focus /*=-1*/ )
{
	if (_num_view < 2)
		return;

	if (prev_focus == _view_with_focus)
		return;

	CWnd *v;
	if ((prev_focus != -1) && (v = (CWnd *)(_view_info[prev_focus].view_handle)))
	{
/*		RECT r;
		v->GetClientRect(&r);
		int screen_width = r.right - r.left;
		int screen_height = r.bottom - r.top;

		r.bottom -= 3;
		r.right -= 3;
		
		CDC *dc = v->GetDC();

		dc->PatBlt(r.left, r.top, screen_width, 3, PATINVERT);
		dc->PatBlt(r.left, r.top+3, 3, screen_height-3, PATINVERT);
		dc->PatBlt(r.left+3, r.bottom, screen_width-3, 3, PATINVERT);
		dc->PatBlt(r.right, r.top+3, 3, screen_height-6, PATINVERT);

		v->ReleaseDC(dc);
*/		
		CWnd *v = (CWnd *) (_view_info[prev_focus].view_handle);
		v->Invalidate();
	}

	if (_view_with_focus < 0)
		return;

	v = (CWnd *) (_view_info[_view_with_focus].view_handle);
	if (!v)
		return;

	CDC *dc = v->GetDC();
	RECT r;
	v->GetClientRect(&r);
	int screen_width = r.right - r.left;
	int screen_height = r.bottom - r.top;

	r.bottom -= 3;
	r.right -= 3;

/*	dc->PatBlt(r.left, r.top, screen_width, 3, PATINVERT);
	dc->PatBlt(r.left, r.top+3, 3, screen_height-3, PATINVERT);
	dc->PatBlt(r.left+3, r.bottom, screen_width-3, 3, PATINVERT);
	dc->PatBlt(r.right, r.top+3, 3, screen_height-6, PATINVERT);
*/
	CPen pen;
	pen.CreatePen(PS_SOLID, 3, RGB(0, 0, 0));

	CPen *pen_save = (CPen *) dc->SelectObject(&pen);

//      int rop_save = dc->SetROP2(R2_NOT);

	dc->MoveTo(r.left, r.top);
	dc->LineTo(r.right, r.top);
	dc->LineTo(r.right, r.bottom);
	dc->LineTo(r.left, r.bottom);
	dc->LineTo(r.left, r.top);

//      dc->SetROP2(rop_save);
	dc->SelectObject(pen_save);
	v->ReleaseDC(dc);
}				// show_focus()


void
ra_view_mfc::get_child_dimensions()
{
	long curr_y_pos = 0;
	for (int i = 0; i < _num_view; i++)
	{
		CWnd *v = (CWnd *) (_view_info[i].view_handle);
		RECT r;
		v->GetWindowRect(&r);
		_view_info[i].width = r.right - r.left;
		_view_info[i].height = r.bottom - r.top;
		_view_info[i].y = curr_y_pos;

		curr_y_pos += _view_info[i].height;
	}
}				// get_child_dimensions()



BOOL ra_view_mfc::PreTranslateMessage(MSG * msg)
{
	int focus_save = -1;

	if (msg->message == WM_KEYDOWN)
	{
		// send keydown message to pane with focus
		CWnd *v = (CWnd *) (_view_info[_view_with_focus].view_handle);
		v->SendMessage(msg->message, msg->wParam, msg->lParam);

		// done with the message
		return TRUE;
	}

	if (msg->message == WM_MOUSEWHEEL)
	{
		CWnd *v = (CWnd *) (_view_info[_view_with_focus].view_handle);
		v->SendMessage(msg->message, msg->wParam, msg->lParam);

		// done with the message
		return TRUE;
	}

	if ((msg->message == WM_LBUTTONDOWN) || (msg->message == WM_RBUTTONDOWN))
	{
		// getting size-info of panes must be done here because
		// ra-view seems not to receive a WM_SIZE message.
		// (Perhaps I have overlooked something)
		for (int j = 0; j < _num_view; j++)
		{
			CWnd *v = (CWnd *)_view_info[j].view_handle;
			RECT r;
			v->GetClientRect(&r);
			v->MapWindowPoints(this, &r);
			_view_info[j].x = r.left;
			_view_info[j].y = r.top;
			_view_info[j].width = r.right - r.left;
			_view_info[j].height = r.bottom - r.top;
		}

		// the coordinates of the mouse-button-press event
		// are in pane-coordinates -> need to convert them in
		// ra-view coordinates
		POINT pt;
		pt.x = ((int) (short) LOWORD(msg->lParam));
		pt.y = ((int) (short) HIWORD(msg->lParam));

		CWnd *temp = CWnd::FromHandle(msg->hwnd);
		temp->MapWindowPoints(this, &pt, 1);

		focus_save = _view_with_focus;
		_view_with_focus = -1;
		for (int i = 0; i < _num_view; i++)
		{
			if ((pt.x >= _view_info[i].x)
			    && (pt.x <= (_view_info[i].x + _view_info[i].width))
			    && (pt.y >= _view_info[i].y)
			    && (pt.y <= (_view_info[i].y + _view_info[i].height)))
			{
				_view_with_focus = i;
				break;
			}
		}
	}

	show_focus(focus_save);

	return CMDIChildWnd::PreTranslateMessage(msg);
}


BOOL
ra_view_mfc::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext * context)
{
	combine_channels();

	// if only one view, it will not work with the splitter-wnd
	// (the splitter-wnd needs at least 2 panes)
	if (_num_view == 1)
	{
		long y_pos = 0;
//		create_single_view(this, 0, &y_pos);
		create_single_view(this, 0);
		return TRUE;
	}

	_split_wnd.CreateStatic(this, _num_view, 1);

	long curr_y_pos = 0;
	for (int i = 0; i < _num_view; i++)
	{
//		create_single_view(&_split_wnd, i, &curr_y_pos);
		create_single_view(&_split_wnd, i);
		CWnd *wnd = (CWnd *) (_view_info[i].view_handle);

		// every single pane in a splitter-window is assigned a fixed id;
		// if you want to use your own wnd (which is already created) it is
		// enough to assign the id to your already created wnd and it will
		// be displayed in the corresponding splitter pane
		//
		// (what a brain damaged way to implement it this way; I needed a whole
		// day to get this working; with QT I needed onle half a day to
		// implement the whole idea to use splitter windows and write the
		// *_general-files also)
		wnd->SetDlgCtrlID(AFX_IDW_PANE_FIRST + i * 16 + 0);  // row i, column 0

		// no percentage for width so try to set a not so much insane
		// value for the ideal-height of the pane
		_split_wnd.SetRowInfo(i, 70 * _view_info[i].num_ch, 30);
	}

	return (TRUE);
}


void
ra_view_mfc::OnSize(UINT nType, int cx, int cy) 
{
	CMDIChildWnd::OnSize(nType, cx, cy);
	
	// if only one view resize child (is not necesary if the splitter-wnd
	// is used)
	CWnd *v = (CWnd *) (_view_info[0].view_handle);
	if ((_num_view == 1) && v)
	{
		RECT rect;
		GetClientRect(&rect);
		v->MoveWindow(0, 0, rect.right-rect.left, rect.bottom-rect.top);
	}
} // OnSize()


BOOL
ra_view_mfc::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CMDIFrameWnd* pParentWnd, CCreateContext* pContext) 
{
	DWORD style_use = dwStyle | WS_HSCROLL;
		
	return CMDIChildWnd::Create(lpszClassName, lpszWindowName, style_use, rect, pParentWnd, pContext);
} // Create()


void
ra_view_mfc::init_scrollbar(long page_width, long num_samples, double max_xscale)
{
	_page_width = page_width;
	_num_samples = num_samples;
	_max_xscale = max_xscale;

	if (page_width <= 0)
		return;

	double num_steps = ((double)(num_samples - page_width) / (double)page_width) * 10.0;
	if (((double)((long)num_steps) - num_steps) > 0.0)
		num_steps++;

	if (num_steps <= 0)
		return;

	SetScrollRange(SB_HORZ, 0, (int)num_steps);
} // init_scrollbar()


void
ra_view_mfc::set_scrollbar_pos(long pos)
{
	if ((_page_width <= 0) || _block_signals)
		return;

	_block_signals = true;

	double t = ((double)pos / (double)_page_width) * 10.0;
	if (((double)((long)t) - t) > 0.0)
		t += 1.0;
	_scroll_pos = (int)t;
	
	SetScrollPos(SB_HORZ, _scroll_pos);

	_block_signals = false;
} // set_scrollbar_pos()


void
ra_view_mfc::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if (_block_signals)
		return;

	_block_signals = true;

	int pos_save = _scroll_pos;
	switch (nSBCode)
	{
	case SB_LEFT:
	case SB_LINELEFT:
		_scroll_pos--;
		break;
	case SB_RIGHT:
	case SB_LINERIGHT:
		_scroll_pos++;
		break;
	case SB_PAGELEFT:
		_scroll_pos -= 10;
		break;
	case SB_PAGERIGHT:
		_scroll_pos += 10;
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		_scroll_pos = (int)nPos;
		break;
	case SB_ENDSCROLL:
	default:
		_block_signals = false;
		return;
	}

	if (_scroll_pos < 0)
		_scroll_pos = 0;

	int max_pos = GetScrollLimit(SB_HORZ);
	if (_scroll_pos > max_pos)
		_scroll_pos = max_pos;

	if (_scroll_pos == pos_save)
	{
		_block_signals = false;
		return;
	}

	SetScrollPos(SB_HORZ, _scroll_pos);

	struct comm_para *para = new struct comm_para[2];
	para[0].type = PARA_LONG;
	para[0].v.l = (long)((double)(_scroll_pos * _page_width) / 10.0);
	para[1].type = PARA_DOUBLE;
	para[1].v.d = _max_xscale;
	ra_comm_emit(_meas, "ra-view-mfc", "pos-change", 2, para);
	delete[] para;

	_block_signals = false;

//MDIChildWnd::OnHScroll(nSBCode, nPos, pScrollBar);
} // OnHScroll()


void ra_view_mfc::OnSetFocus(CWnd* pOldWnd) 
{
	CMDIChildWnd::OnSetFocus(pOldWnd);
	
	if (_view_with_focus < 0)
		_view_with_focus = 0;
}  // OnSetFocus()
