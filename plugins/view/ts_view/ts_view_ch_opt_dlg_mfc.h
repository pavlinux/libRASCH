/*----------------------------------------------------------------------------
 * ts_view_ch_opt_dlg_mfc.h
 *
 * 
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002-2003, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Header: /home/cvs_repos.bak/librasch/plugins/view/ts_view/ts_view_ch_opt_dlg_mfc.h,v 1.2 2003/03/21 09:43:10 rasch Exp $
 *--------------------------------------------------------------------------*/

#if !defined(AFX_TS_VIEW_CH_OPT_DLG_MFC_H__69C8E9F5_A67C_4B64_B50E_0FCC45FDB35B__INCLUDED_)
#define AFX_TS_VIEW_CH_OPT_DLG_MFC_H__69C8E9F5_A67C_4B64_B50E_0FCC45FDB35B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ts_view_ch_opt_dlg_mfc.h : Header-Datei
//

#include "ts_view_general.h"


/////////////////////////////////////////////////////////////////////////////
// Dialogfeld ts_view_ch_opt_dlg_mfc 

class ts_view_ch_opt_dlg_mfc:public CDialog
{
// Konstruktion
      public:
	ts_view_ch_opt_dlg_mfc()
	{
	}
//      ts_view_ch_opt_dlg_mfc(CWnd* pParent = NULL);   // Standardkonstruktor

	void init(struct ch_info **ch, int num, int curr_ch);

// Dialogfelddaten
	//{{AFX_DATA(ts_view_ch_opt_dlg_mfc)
	enum
	{ IDD = IDD_CH_OPT_DLG };
	CComboBox _ch_list;
	BOOL _invert;
	double _max;
	int _pos_percent;
	double _res;
	CString _res_unit;
	BOOL _show_ch;
	BOOL _use_res;
	double _min;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(ts_view_ch_opt_dlg_mfc)
      protected:
	virtual void DoDataExchange(CDataExchange * pDX);	// DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
      protected:
	struct ch_info **_ch_save;
	struct ch_info *_ch;
	int _num;

	int _curr_ch;
	int _prev_ch;


	void _save_data(int ch);
	void _pos_sel();

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(ts_view_ch_opt_dlg_mfc)
	afx_msg void OnUseRes();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeChList();
	virtual void OnOK();
	afx_msg void OnPosCenter();
	afx_msg void OnPosMinmax();
	//}}AFX_MSG
DECLARE_MESSAGE_MAP()};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_TS_VIEW_CH_OPT_DLG_MFC_H__69C8E9F5_A67C_4B64_B50E_0FCC45FDB35B__INCLUDED_
