/**
 * \file ch_sel_dlg_mfc.h
 *
 * \author Raphael Schneider (rasch@med1.med.tum.de)
 */

/*--------------------------------------------------------------------------
 *
 * Copyright (C) 2002-2004, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Header: /home/cvs_repos.bak/librasch/plugins/view/ch_sel_dlg/ch_sel_dlg_mfc.h,v 1.6 2004/06/30 16:44:26 rasch Exp $
 *
 *--------------------------------------------------------------------------*/

#if !defined(AFX_CH_SEL_DLG_MFC_H__1B7F4273_54F9_40FB_95B7_E288BB3FC7CB__INCLUDED_)
#define AFX_CH_SEL_DLG_MFC_H__1B7F4273_54F9_40FB_95B7_E288BB3FC7CB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ch_sel_dlg_mfc.h : Header-Datei
//

#define _LIBRASCH_BUILD
#include <ra.h>
#include <ra_priv.h>
#include <pl_general.h>


/////////////////////////////////////////////////////////////////////////////
// Dialogfeld ch_sel_dlg_mfc 

class ch_sel_dlg_mfc:public CDialog
{
// Konstruktion
      public:
	ch_sel_dlg_mfc()
	{
	}
	ch_sel_dlg_mfc(struct proc_info *pi, CWnd * pParent = NULL);	// Standardkonstruktor

	void set_pi(struct proc_info *pi)
	{
		_pi = pi;
	}


// Dialogfelddaten
	//{{AFX_DATA(ch_sel_dlg_mfc)
	enum { IDD = IDD_CH_SEL_DLG };
	CListBox _use_ch_list;
	CButton _use_btn;
	CButton _up_btn;
	CButton _not_use_btn;
	CButton _down_btn;
	CListBox _ch_list;
	CString	_txt_ch_use;
	CString	_txt_rec_ch;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(ch_sel_dlg_mfc)
      protected:
	virtual void DoDataExchange(CDataExchange * pDX);	// DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
      protected:
	struct proc_info *_pi;
	int _curr_sel_use_ch;
	int _curr_sel_all_ch;

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(ch_sel_dlg_mfc)
	virtual BOOL OnInitDialog();
	afx_msg void all_ch_sel();
	afx_msg void ch_use();
	afx_msg void use_ch_sel();
	afx_msg void ch_dont_use();
	afx_msg void ch_up();
	afx_msg void ch_down();
	afx_msg void ok();
	//}}AFX_MSG
DECLARE_MESSAGE_MAP()};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_CH_SEL_DLG_MFC_H__1B7F4273_54F9_40FB_95B7_E288BB3FC7CB__INCLUDED_
