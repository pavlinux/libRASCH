/*----------------------------------------------------------------------------
 * ra_splitter_mfc.h
 *
 * 
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002-2003, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Header: /home/cvs_repos.bak/librasch/plugins/view/ra_view/ra_splitter_mfc.h,v 1.1 2003/04/15 09:20:28 rasch Exp $
 *--------------------------------------------------------------------------*/

#if !defined(AFX_RA_SPLITTER_MFC_H__D1A99E96_FEBD_44BE_BF42_FD81AADF867A__INCLUDED_)
#define AFX_RA_SPLITTER_MFC_H__D1A99E96_FEBD_44BE_BF42_FD81AADF867A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ra_splitter_mfc.h : Header-Datei
//

#define SPLIT_VERTICAL    0
#define SPLIT_HORIZONTAL  1

#define SPLIT_BAR_WIDTH  3


struct single_view_info
{
	CWnd *view;

	// relative size of view
	double rel_size;

	int size_hint;
};  // struct single_view_info


/////////////////////////////////////////////////////////////////////////////
// Fenster ra_splitter_mfc 

class ra_splitter_mfc : public CWnd
{
// Konstruktion
public:
	ra_splitter_mfc();

// Attribute
public:

// Operationen
public:
	int orientation() { return _orientation; }
	void set_orientation(int type) { _orientation = type; }

	bool add_view(CWnd *v);

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(ra_splitter_mfc)
	//}}AFX_VIRTUAL

// Implementierung
public:
	virtual ~ra_splitter_mfc();

	// Generierte Nachrichtenzuordnungsfunktionen
protected:
	int _orientation;
	bool _mouse_down;

	int _num_views;
	struct single_view_info *_info;

	int get_size_hint(CWnd *v);
	void recalc_relative_size();
	
	//{{AFX_MSG(ra_splitter_mfc)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_RA_SPLITTER_MFC_H__D1A99E96_FEBD_44BE_BF42_FD81AADF867A__INCLUDED_
