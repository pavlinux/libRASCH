/*----------------------------------------------------------------------------
 * simple_calc_dlg_mfc.h
 *
 * 
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2003-2006, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#if !defined(AFX_SIMPLE_CALC_DLG_MFC_H__1B7F4273_54F9_40FB_95B7_E288BB3FC7CB__INCLUDED_)
#define AFX_SIMPLE_CALC_DLG_MFC_H__1B7F4273_54F9_40FB_95B7_E288BB3FC7CB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// simple_calc_dlg_mfc.h : Header-Datei
//

#define _LIBRASCH_BUILD
#include <ra.h>
#include <ra_priv.h>
#include <pl_general.h>


/////////////////////////////////////////////////////////////////////////////
// Dialogfeld simple_calc_dlg_mfc 

class simple_calc_dlg_mfc:public CDialog
{
// Konstruktion
      public:
	simple_calc_dlg_mfc()
	{
	}
	simple_calc_dlg_mfc(struct proc_info *pi, CWnd * pParent = NULL);	// Standardkonstruktor

	void set_pi(struct proc_info *pi)
	{
		_pi = pi;
	}


// Dialogfelddaten
	//{{AFX_DATA(simple_calc_dlg_mfc)
	enum { IDD = IDD_SIMPLE_CALC_DLG };
	CComboBox	_pl_list;
	CString	_results;
	CString	_txt_plugins;
	CString	_txt_results;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(simple_calc_dlg_mfc)
      protected:
	virtual void DoDataExchange(CDataExchange * pDX);	// DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
      protected:
	struct proc_info *_pi;

	int _num_proc_pl;
	struct proc_info **_pi_for_pl;

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(simple_calc_dlg_mfc)
	virtual BOOL OnInitDialog();
	afx_msg void OnCalc();
	afx_msg void OnExit();
	afx_msg void OnCopyClipboard();
	//}}AFX_MSG
DECLARE_MESSAGE_MAP()};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_SIMPLE_CALC_DLG_MFC_H__1B7F4273_54F9_40FB_95B7_E288BB3FC7CB__INCLUDED_
