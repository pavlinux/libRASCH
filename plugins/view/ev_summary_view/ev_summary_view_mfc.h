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

#ifndef _EV_SUMMARY_VIEW_MFC_H
#define _EV_SUMMARY_VIEW_MFC_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"
#include "ev_summary_view_general.h"


class ev_summary_view_mfc:public CScrollView, public ev_summary_view_general
{
 public:
	ev_summary_view_mfc();
	~ev_summary_view_mfc();
	
	void init(meas_handle mh, sum_handle sh);
	virtual void update();
	
        // Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(ev_summary_view_mfc)
 public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName,
			    DWORD dwStyle, const RECT & rect,
			    CWnd * pParentWnd, UINT nID, CCreateContext * pContext = NULL);
 protected:
	virtual void OnDraw(CDC * pDC);
	//}}AFX_VIRTUAL
	
 protected:
	void plot_one_dim(CDC *dc, long x_start, long part_idx);
	void plot_ev_summary_1d(CDC *dc, long x_start, long part_idx);
	void plot_channel_1d(CDC *dc, long x_start, long part_idx, long ch);

	void show_selected_templ(CDC * dc);
	void set_scroll_size();
	
	CBrush _bk_brush;
		
        // Generierte Message-Map-Funktionen
 protected:
	//{{AFX_MSG(ev_summary_view_mfc)
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
}; // class ev_summary_view_mfc


#endif // _EV_SUMMARY_VIEW_MFC_H
