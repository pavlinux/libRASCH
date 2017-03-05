/*----------------------------------------------------------------------------
 * ts_view_mfc.h
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

#if !defined(AFX_RA_VIEW_TS_MFC_H__AE58343C_2689_418C_AEBD_62165065FE79__INCLUDED_)
#define AFX_RA_VIEW_TS_MFC_H__AE58343C_2689_418C_AEBD_62165065FE79__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"
#include "ts_view_general.h"

#define _LIBRASCH_BUILD
#include <ra.h>
#include <ra_priv.h>
#include <ra_pl_comm.h>

// menu defines
#define IDM_GENERAL_OPTIONS  10

#define IDM_CHANNEL_OPTIONS  20

#define IDM_SORT_ANNOT       30

#define IDM_FILTER           40
#define IDM_FILTER_PLNOISE   41
#define IDM_FILTER_MEAN      42


class ts_view_mfc:public CWnd, virtual public ts_view_general
{
public:
	ts_view_mfc();

protected:		// Nur aus Serialisierung erzeugen
//      DECLARE_DYNCREATE(ts_view_mfc)

// Attribute
public:

// Operationen
public:
	virtual void init(meas_handle mh, plugin_handle pl, int num_ch, int *ch);
	virtual void update();
	virtual void set_x_resolution(double x_res, void *parent);

	virtual void init_scrollbar(long page_width, long num_samples, double max_xscale);
	virtual void set_scrollbar_pos(long pos);

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(ts_view_mfc)
      public:
	  virtual void OnDraw(CDC * pDC);	// überladen zum Zeichnen dieser Ansicht
	virtual BOOL PreCreateWindow(CREATESTRUCT & cs);
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT & rect, CWnd * pParentWnd,UINT nID, CCreateContext * pContext = NULL);
      protected:
	//}}AFX_VIRTUAL

// Implementierung
public:
	  virtual ~ ts_view_mfc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext & dc) const;
#endif

protected:
	virtual void plot_grid(CDC * dc);
	void plot_statusbar(CDC * dc);
	void plot_channel(CDC * dc, int ch, int line);
	void print_comment_annotations(CDC * dc);
	void highlight_area();
	virtual void plot_cursor(CDC *dc, int ch = -1);

	virtual void get_text_extent(char *text, int *w, int *h);

	void process_msgs();

	void ch_dlg();
	virtual int annot_dlg(char **, bool *, bool *, int *);

	plugin_handle _plugin;

	CMenu _popup_menu;
	CMenu _popup_menu_filter;

	COLORREF _bk_color;
	bool _plot_grid;
	CPen _grid_pen;
	CPen _ch_pen;
	CPen _text_pen;
	CBrush _bk_brush;
	HFONT _font;
	HFONT _font_bold;

// Generierte Message-Map-Funktionen
      protected:
	//{{AFX_MSG(ts_view_mfc)
	  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnPaint();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void _general_options();
	afx_msg void _ch_options();
	afx_msg void sort_annot();
	afx_msg void filter_powerline();
	afx_msg void filter_mean();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
  DECLARE_MESSAGE_MAP()};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_RA_VIEW_TS_MFC_H__AE58343C_2689_418C_AEBD_62165065FE79__INCLUDED_)
