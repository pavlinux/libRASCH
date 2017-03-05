/*----------------------------------------------------------------------------
 * pair_opt_dlg_mfc.h
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

#if !defined(AFX_PAIR_OPT_DLG_MFC_H__38C680F8_3DD6_446A_B9D5_7FED6075E3EF__INCLUDED_)
#define AFX_PAIR_OPT_DLG_MFC_H__38C680F8_3DD6_446A_B9D5_7FED6075E3EF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// pair_opt_dlg_mfc.h : Header-Datei
//

#include "color_button.h"


/////////////////////////////////////////////////////////////////////////////
// Dialogfeld pair_opt_dlg_mfc 

class pair_opt_dlg_mfc:public CDialog
{
// Konstruktion
      public:
	// only empty constructor works
	pair_opt_dlg_mfc()
	{
	}
	void init(meas_handle meas, plugin_handle pl,
		  struct ra_plot_options *opt, int curr_pair = 0);

// Dialogfelddaten
	//{{AFX_DATA(pair_opt_dlg_mfc)
	enum { IDD = IDD_PAIR_OPT_DLG };
	CComboBox	_channel;
	color_button _symbol_color;
	CComboBox _log;
	CComboBox _time;
	CComboBox _symbol;
	CComboBox _pair;
	CComboBox _line;
	color_button _line_color;
	CComboBox _axis;
	BOOL _is_time;
	double _max;
	double _min;
	CString _name;
	BOOL _plot_line;
	BOOL _plot_log;
	BOOL _use_max;
	BOOL _use_min;
	BOOL _plot_symbol;
	CString	_txt_axis;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(pair_opt_dlg_mfc)
      protected:
	virtual void DoDataExchange(CDataExchange * pDX);	// DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
      protected:
	meas_handle _meas;
	plugin_handle _plugin;	// perhaps I need this later
	struct ra_plot_pair *_curr_pair;
	struct ra_plot_axis *_curr_axis;
	struct ra_plot_options *_opt;
	struct ra_plot_options *_opt_orig;

	int _init_pair_num;

	int _num_ch;
	long *_ch;

	void _save_pair_data();

	void _axis_init();
	void _save_axis_data();

	void _copy_opt_struct(struct ra_plot_options *dest, struct ra_plot_options *src);
	void _copy_pair_struct(struct ra_plot_pair **dest, struct ra_plot_pair *src, int n);
	void _copy_axis_struct(struct ra_plot_axis **dest, struct ra_plot_axis *src, int n);
	void _free_opt_struct(struct ra_plot_options *opt);

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(pair_opt_dlg_mfc)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangePair();
	afx_msg void OnPlotSymbols();
	afx_msg void OnSymColor();
	afx_msg void OnPlotLine();
	afx_msg void OnLineColor();
	afx_msg void OnSelchangeAxis();
	afx_msg void OnUseMin();
	afx_msg void OnUseMax();
	afx_msg void OnIsTime();
	afx_msg void OnPlotLog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnSelchangeChannel();
	//}}AFX_MSG
DECLARE_MESSAGE_MAP()};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_PAIR_OPT_DLG_MFC_H__38C680F8_3DD6_446A_B9D5_7FED6075E3EF__INCLUDED_
