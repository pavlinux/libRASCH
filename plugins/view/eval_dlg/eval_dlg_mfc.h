/*----------------------------------------------------------------------------
 * eval_dlg_mfc.h
 *
 * 
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002-2004, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Header: /home/cvs_repos.bak/librasch/plugins/view/eval_dlg/eval_dlg_mfc.h,v 1.6 2004/06/30 16:44:26 rasch Exp $
 *--------------------------------------------------------------------------*/

#if !defined(AFX_EVAL_DLG_MFC_H__C2B40EB2_EE4E_4CAA_9D50_23EBAEC7B00B__INCLUDED_)
#define AFX_EVAL_DLG_MFC_H__C2B40EB2_EE4E_4CAA_9D50_23EBAEC7B00B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// eval_dlg_mfc.h : Header-Datei
//


#define _LIBRASCH_BUILD
#include <ra.h>
#include <ra_priv.h>
#include <pl_general.h>


/////////////////////////////////////////////////////////////////////////////
// Dialogfeld eval_dlg_mfc 

class eval_dlg_mfc:public CDialog
{
// Konstruktion
public:
	eval_dlg_mfc() { }
	eval_dlg_mfc(struct proc_info *pi, CWnd * pParent = NULL);	// Standardkonstruktor

	void set_pi(struct proc_info *pi)
	{
		_pi = pi;
	}


// Dialogfelddaten
	//{{AFX_DATA(eval_dlg_mfc)
	enum { IDD = IDD_EVAL_DLG };
	CButton	_group;
	CListBox _eval_list;
	CString _created;
	BOOL _default_eval;
	CString _description;
	CString _host;
	CString _original;
	CString _program;
	CString _user;
	CString	_modified;
	CString	_txt_description;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(eval_dlg_mfc)
      protected:
	virtual void DoDataExchange(CDataExchange * pDX);	// DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	struct proc_info *_pi;

	eval_handle *_all_eval;
	int _num_eval;
	int _curr_eval;

	bool _changed;
	
	void init();

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(eval_dlg_mfc)
	virtual BOOL OnInitDialog();
	afx_msg void OnDelete();
	afx_msg void OnDefault();
	virtual void OnOK();
	afx_msg void OnSelchangeEvalList();
	afx_msg void OnEventTypes();
	virtual void OnCancel();
	//}}AFX_MSG
DECLARE_MESSAGE_MAP()};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_EVAL_DLG_MFC_H__C2B40EB2_EE4E_4CAA_9D50_23EBAEC7B00B__INCLUDED_
