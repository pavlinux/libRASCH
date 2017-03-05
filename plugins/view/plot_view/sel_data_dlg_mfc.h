/*----------------------------------------------------------------------------
 * sel_data_dlg_mfc.cpp
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

#if !defined(AFX_SEL_DATA_DLG_MFC_H__C2D9ECB0_BD65_41A5_AFDE_83B405B04E4D__INCLUDED_)
#define AFX_SEL_DATA_DLG_MFC_H__C2D9ECB0_BD65_41A5_AFDE_83B405B04E4D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// sel_data_dlg_mfc.h : Header-Datei
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld sel_data_dlg_mfc 

class sel_data_dlg_mfc:public CDialog
{
// Konstruktion
      public:
	// need to use an empty standard constructor
	sel_data_dlg_mfc()
	{
	}
	void init(meas_handle meas, plugin_handle pl, struct ra_plot_options *opt);


// Dialogfelddaten
	//{{AFX_DATA(sel_data_dlg_mfc)
	enum { IDD = IDD_SELECT_DATA_DLG };
	CComboBox _z_axis;
	CComboBox _y_axis;
	CComboBox _x_axis;
	CListBox _pair_list;
	CComboBox _data_set;
	BOOL _show_grid;
	BOOL _show_legend;
	BOOL _use_z_axis;
	CString	_txt_data_set;
	CString	_txt_x_axis;
	CString	_txt_y_axis;
	CString	_txt_z_axis;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(sel_data_dlg_mfc)
      protected:
	virtual void DoDataExchange(CDataExchange * pDX);	// DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
      protected:
	meas_handle _meas;
	eval_handle _eval;
	plugin_handle _plugin;
	struct ra_plot_options *_opt;
	struct ra_plot_options *_opt_orig;

	void _copy_opt_struct(struct ra_plot_options *dest, struct ra_plot_options *src);
	void _copy_pair_struct(struct ra_plot_pair **dest, struct ra_plot_pair *src, int n);
	void _copy_axis_struct(struct ra_plot_axis **dest, struct ra_plot_axis *src, int n);
	void _free_opt_struct(struct ra_plot_options *opt);

	void _init_axis();
	void _init_pair_list();
	void format_pair(int idx, CString &entry);
	void get_ch_name(long ch, CString &name);

	void _set_axis(int *idx, CString &s);

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(sel_data_dlg_mfc)
	afx_msg void OnAdd();
	afx_msg void OnSelchangeDataSet();
	afx_msg void OnDelete();
	afx_msg void OnOptions();
	afx_msg void OnDblclkPairList();
	afx_msg void OnSelchangePairList();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	//}}AFX_MSG
DECLARE_MESSAGE_MAP()};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_SEL_DATA_DLG_MFC_H__C2D9ECB0_BD65_41A5_AFDE_83B405B04E4D__INCLUDED_
