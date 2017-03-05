/*----------------------------------------------------------------------------
 * ra_view_mfc.h
 *
 * 
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002-2003, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Header: /home/cvs_repos.bak/librasch/plugins/view/ra_view/ra_view_mfc.h,v 1.3 2003/08/14 13:08:33 rasch Exp $
 *--------------------------------------------------------------------------*/

#ifndef RA_VIEW_MFC_H
#define RA_VIEW_MFC_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"

#include "ra_view_general.h"
#include "ra_splitter_mfc.h"

class ra_view_mfc:public CMDIChildWnd, public virtual ra_view_general
{
public:
	ra_view_mfc();

protected:		// Nur aus Serialisierung erzeugen
//      DECLARE_DYNCREATE(ra_view_mfc)

// Attribute
public:
// Operationen
public:
	virtual void init(rec_handle rec, plugin_handle pl);
	virtual void do_update();
	void get_child_dimensions();

	void init_scrollbar(long page_width, long num_samples, double max_xscale);
	void set_scrollbar_pos(long pos);

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(ra_view_mfc)
	public:
	  virtual BOOL PreCreateWindow(CREATESTRUCT & cs);
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_OVERLAPPEDWINDOW, const RECT& rect = rectDefault, CMDIFrameWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);
	protected:
	  virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext * pContext);
	//}}AFX_VIRTUAL

public:
	  virtual ~ ra_view_mfc()
	{;
	}
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext & dc) const;
#endif

protected:
	CSplitterWnd _split_wnd;
	plugin_handle _plugin;

	void show_focus(int prev_focus = -1);

	virtual void *create_main_view();
	virtual int get_view_dimensions(void *vh, long *width, long *height);
	virtual int pos_view(void *vh, long x, long y);

// Generierte Message-Map-Funktionen
protected:
	//{{AFX_MSG(ra_view_mfc)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
DECLARE_MESSAGE_MAP()};		// class ra_view_mfc


#endif // RA_VIEW_MFC_H
