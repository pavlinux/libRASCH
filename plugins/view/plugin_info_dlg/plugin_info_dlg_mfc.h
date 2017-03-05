/*----------------------------------------------------------------------------
 * plugin_info_dlg_mfc.h
 *
 * 
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2003-2004, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Header: /home/cvs_repos.bak/librasch/plugins/view/plugin_info_dlg/plugin_info_dlg_mfc.h,v 1.3 2004/06/30 16:44:26 rasch Exp $
 *--------------------------------------------------------------------------*/

#if !defined(AFX_PLUGIN_INFO_DLG_MFC_H__1B7F4273_54F9_40FB_95B7_E288BB3FC7CB__INCLUDED_)
#define AFX_PLUGIN_INFO_DLG_MFC_H__1B7F4273_54F9_40FB_95B7_E288BB3FC7CB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// plugin_info_dlg_mfc.h : Header-Datei
//

#include "Resource.h"

#include <afxcmn.h>


#define _LIBRASCH_BUILD
#include <ra.h>
#include <ra_priv.h>
#include <pl_general.h>


/////////////////////////////////////////////////////////////////////////////
// Dialogfeld plugin_info_dlg_mfc 

class plugin_info_dlg_mfc:public CDialog
{
// Konstruktion
public:
	plugin_info_dlg_mfc() { }
	plugin_info_dlg_mfc(struct proc_info *pi, CWnd * pParent = NULL);	// Standardkonstruktor

	void set_pi(struct proc_info *pi) { _pi = pi; }

// Dialogfelddaten
	//{{AFX_DATA(plugin_info_dlg_mfc)
	enum { IDD = IDD_PLUGIN_INFO_DLG };
	CListCtrl	_plugin_list;
	CString	_build_time_stamp;
	CString	_plugin_desc;
	CString	_plugin_path;
	BOOL	_use_plugin;
	CString	_txt_loaded_plugins;
	CString	_txt_path;
	CString	_txt_description;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(plugin_info_dlg_mfc)
protected:
	virtual void DoDataExchange(CDataExchange * pDX);	// DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	struct proc_info *_pi;
	ra_handle _ra;

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(plugin_info_dlg_mfc)
	virtual BOOL OnInitDialog();
	afx_msg void use_plugin();
	afx_msg void Itemchanged_plugin_list(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_PLUGIN_INFO_DLG_MFC_H__1B7F4273_54F9_40FB_95B7_E288BB3FC7CB__INCLUDED_
