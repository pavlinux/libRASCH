/**
 * \file cont_ap_view_mfc.h
 *
 * \author Raphael Schneider (rasch@med1.med.tum.de)
 */

/*--------------------------------------------------------------------------
 *
 * Copyright (C) 2002-2005, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *
 *--------------------------------------------------------------------------*/

#ifndef CONT_AP_VIEW_MFC_H
#define CONT_AP_VIEW_MFC_H


#include "../ts_view/ts_view_mfc.h"
#include "cont_ap_view_general.h"


class cont_ap_view_mfc:public ts_view_mfc, public cont_ap_view_general
{
public:
	cont_ap_view_mfc();
	~cont_ap_view_mfc();

	virtual void update();
	virtual void init(meas_handle mh, plugin_handle pl, int num_ch, int *ch);
	virtual void do_update();

	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(cont_ap_view_mfc)
	//}}AFX_VIRTUAL

	virtual void sort_general(const char *evtype, double value);
	void sort_bp(int type);
	void sort_class(int classification);

protected:
	void plot_beats(CDC * dc);
	void mark_calib_seq(CDC * dc);

	void show_pos(CDC * dc, int ch, int x, long pos);

	CMenu *_popup_menu_sort;

	// Generierte Message-Map-Funktionen
protected:
	//{{AFX_MSG(cont_ap_view_mfc)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	//}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};	// class cont_ap_view_mfc


#endif // CONT_AP_VIEW_MFC_H
