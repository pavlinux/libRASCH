/*----------------------------------------------------------------------------
 * ecg_view_mfc.h
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

#ifndef ECG_VIEW_MFC_H
#define ECG_VIEW_MFC_H


#include "../ts_view/ts_view_mfc.h"
#include "ecg_view_general.h"

// popup-menu id's
#define IDM_SHORT_RRI_OPTIONS   1000
#define IDM_LONG_RRI_OPTIONS    1001
#define IDM_TOGGLE_EDIT_MODE    1002
#define ECG_NORMAL_BEAT    1010
#define ECG_VPC_BEAT       1011
#define ECG_PACED_BEAT     1012
#define ECG_ARTIFACT_BEAT  1013

#define ECG_VIEW_CH_START          1100
#define ECG_VIEW_CH_MAX            1299
#define ECG_VIEW_TEMPLATE_START    1300
#define ECG_VIEW_TEMPLATE_MAX      1499

#define ID_P_TYPE_SEL     2000
#define ID_QRS_TYPE_SEL   2001
#define ID_T_TYPE_SEL     2002

#define ID_APPLY_MORPH_CHANGE       2100
#define ID_APPLY_ALL_MORPH_CHANGE   2101
#define ID_EDIT_CLOSE               2102

class ecg_view_mfc:public ts_view_mfc, public ecg_view_general
{
public:
	ecg_view_mfc();
	~ecg_view_mfc();

	virtual void update();
	virtual void init(meas_handle mh, plugin_handle pl, int num_ch, int *ch);
	virtual void do_update();

	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(ecg_view_mfc)
	//}}AFX_VIRTUAL

	virtual void sort_general(prop_handle ph, double value);
	void sort_template(int templ_num);
	void sort_ch_found(int ch);
	void sort_rri(int short_first);
	void sort_class(int classification);

protected:
	void init_menu();
	void init_edit();

	void plot_beats(CDC * dc);
	void show_beat_class(CDC *dc, long beat, int x);
	void show_beat_template(CDC *dc, long beat, int x);

	void show_beat_position(CDC *dc, long beat, int x);
	void show_p_wave_info(CDC *dc, long ev_id, int ch, int qrs_pos, long morph_flags, bool in_edit_area);
	void show_qrs_info(CDC *dc, long ev_id, int ch, int qrs_pos, long morph_flags, bool in_edit_area);
	void show_t_wave_info(CDC *dc, long ev_id, int ch, int qrs_pos, long morph_flags, bool in_edit_area);

	void show_peak(CDC *dc, int ch, int x, long pos, bool below, COLORREF col = RGB(0, 0, 0));
	void show_peak_edit(CDC *dc, int ch, int x, long pos, bool below, COLORREF col = RGB(0, 0, 0));

	void show_beat_rri(CDC *dc, long beat, int x_m1, int x);
	void show_beat_info(CDC *dc, long beat, long x);
	void show_beat_index(CDC *dc, long beat, long x);

	virtual void plot_cursor(CDC * dc, int ch = -1);

	void show_pos(CDC * dc, int ch, int x, long pos, COLORREF col = RGB(0,0,0));
	void show_pos_edit(CDC *dc, int ch, int x, long pos, COLORREF col = RGB(0,0,0), char *type = NULL);

	void plot_edit_area(CDC *dc);
	void print_edit_beat_info(CDC *dc);

	plugin_handle _plugin;

	CMenu _popup_menu_sort;
	CMenu _popup_menu_ecg;
	CMenu _popup_menu_template;

	CComboBox _p_type_select;
	CComboBox _qrs_type_select;
	CComboBox _t_type_select;

	CButton _edit_apply;
	CButton _edit_apply_all;
	CButton _edit_close;

	// Generierte Message-Map-Funktionen
      protected:
	//{{AFX_MSG(ecg_view_mfc)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void _sort_short_rri();
	afx_msg void _sort_long_rri();
	afx_msg void _sort_normal();
	afx_msg void _sort_vpc();
	afx_msg void _sort_paced();
	afx_msg void _sort_artifact();
	afx_msg void _sort_ch_use(UINT id);
	afx_msg void _sort_template(UINT id);
	afx_msg void _toggle_edit_mode_menu();
	afx_msg void _p_type_changed();
	afx_msg void _qrs_type_changed();
	afx_msg void _t_type_changed();
	afx_msg void _edit_apply_pressed();
	afx_msg void _edit_apply_all_pressed();
	afx_msg void _edit_close_pressed();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};	// class ecg_view_mfc

#endif // ECG_VIEW_MFC_H
