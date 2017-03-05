/*----------------------------------------------------------------------------
 * ra_splitter_mfc.cpp
 *
 * 
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002-2003, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Header: /home/cvs_repos.bak/librasch/plugins/view/ra_view/ra_splitter_mfc.cpp,v 1.1 2003/04/15 09:20:28 rasch Exp $
 *--------------------------------------------------------------------------*/

// mtk_splitter_mfc.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "ra_view.h"
#include "ra_splitter_mfc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ra_splitter_mfc

ra_splitter_mfc::ra_splitter_mfc()
{
	_orientation = SPLIT_HORIZONTAL;
	_mouse_down = false;

	_num_views = 0;
	_info = NULL;
}

ra_splitter_mfc::~ra_splitter_mfc()
{
	if (_info)
		delete[] _info;
}


BEGIN_MESSAGE_MAP(ra_splitter_mfc, CWnd)
	//{{AFX_MSG_MAP(ra_splitter_mfc)
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten ra_splitter_mfc 

bool
ra_splitter_mfc::add_view(CWnd *v)
{
	_num_views++;
	struct single_view_info *t = new struct single_view_info[_num_views];
	if (_info)
	{
		memcpy(t, _info, sizeof(struct single_view_info) * (_num_views-1));
		delete[] _info;
	}
	_info = t;
	_info[_num_views-1].view = v;
	_info[_num_views-1].size_hint = get_size_hint(v);

	recalc_relative_size();

	return true;
}  // add_view()


int
ra_splitter_mfc::get_size_hint(CWnd *v)
{
	RECT r;
	v->GetWindowRect(&r);

	if (_orientation == SPLIT_VERTICAL)
		return (r.right - r.left);
	else
		return (r.bottom - r.top);
}  // get_size_hint()


void
ra_splitter_mfc::recalc_relative_size()
{
	double all = 0.0;
	for (int i = 0; i < _num_views; i++)
		all += (double)_info[i].size_hint;
	for (i = 0; i < _num_views; i++)
		_info[i].rel_size = (double)_info[i].size_hint / all;
}  // recalc_relative_size()


void
ra_splitter_mfc::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	RECT r;
	GetClientRect(&r);
	int height = r.bottom - r.top;
	int width = r.right - r.left;

	if (_orientation == SPLIT_VERTICAL)
		width -= ((_num_views-1) * SPLIT_BAR_WIDTH);
	else
		height -= ((_num_views-1) * SPLIT_BAR_WIDTH);

	int x_curr, y_curr;
	x_curr = y_curr = 0;
	for (int i = 0; i < _num_views; i++)
	{
		int x, y, w, h;
		if (_orientation == SPLIT_VERTICAL)
		{
			x = x_curr;
			y = 0;
			w = (int)((double)width * _info[i].rel_size);
			h = height;

			x_curr += w;
		}
		else
		{
			x = 0;
			y = y_curr;
			w = width;
			h = (int)((double)height * _info[i].rel_size);

			y_curr += h;
		}

		_info[i].view->MoveWindow(x, y, w, h);
	}
}


void
ra_splitter_mfc::OnLButtonDown(UINT nFlags, CPoint point) 
{
	_mouse_down = true;
	
	CWnd::OnLButtonDown(nFlags, point);
}

void
ra_splitter_mfc::OnLButtonUp(UINT nFlags, CPoint point) 
{
	_mouse_down = false;
	
	CWnd::OnLButtonUp(nFlags, point);
}

void
ra_splitter_mfc::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (_mouse_down)
	{
		;
	}

	CWnd::OnMouseMove(nFlags, point);
}


void
ra_splitter_mfc::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	for (int i = 0; i < _num_views; i++)
		_info[i].view->Invalidate();
}
