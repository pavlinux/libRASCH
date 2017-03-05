/*----------------------------------------------------------------------------
 * gen_opt_dlg_mfc.h
 *
 * 
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002-2003, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Header: /home/cvs_repos.bak/librasch/plugins/view/ts_view/gen_opt_dlg_mfc.h,v 1.2 2003/03/21 09:43:10 rasch Exp $
 *--------------------------------------------------------------------------*/

#if !defined(AFX_GEN_OPT_DLG_MFC_H__FD5E537E_46FC_49ED_AFB4_493B1F5C2D34__INCLUDED_)
#define AFX_GEN_OPT_DLG_MFC_H__FD5E537E_46FC_49ED_AFB4_493B1F5C2D34__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// gen_opt_dlg_mfc.h : Header-Datei
//

#include "resource.h"
#include "ts_view_general.h"

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld gen_opt_dlg_mfc 

class gen_opt_dlg_mfc:public CDialog
{
// Konstruktion
      public:
	gen_opt_dlg_mfc()
	{
	}
	gen_opt_dlg_mfc(struct ts_options *ts_opt, CWnd * pParent = NULL);	// Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(gen_opt_dlg_mfc)
	enum
	{ IDD = IDD_GENERAL_OPT_DLG };
	int _ch_height;
	BOOL _show_ch_info;
	BOOL _show_grid;
	BOOL _show_status;
	double _speed;
	BOOL _use_fix_ch_height;
	//}}AFX_DATA

	void set_opt(struct ts_options *ts_opt);

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(gen_opt_dlg_mfc)
      protected:
	virtual void DoDataExchange(CDataExchange * pDX);	// DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
      protected:
	struct ts_options *_ts_opt_save;
	struct ts_options _ts_opt;

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(gen_opt_dlg_mfc)
	afx_msg void OnUseFixChHeight();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
DECLARE_MESSAGE_MAP()};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_GEN_OPT_DLG_MFC_H__FD5E537E_46FC_49ED_AFB4_493B1F5C2D34__INCLUDED_
