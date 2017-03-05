/*----------------------------------------------------------------------------
 * ra_plot_mfc.h
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

#ifndef RA_PLOT_MFC_H
#define RA_PLOT_MFC_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"


#include "ra_plot_general.h"


class ra_plot_mfc:public CWnd, public ra_plot_general
{
 public:
	ra_plot_mfc();
	~ra_plot_mfc();

	virtual void update();
	virtual void update_pos() { _plot_pos(); }

	virtual void init(meas_handle meas, plugin_handle pl, struct ra_plot_options *opt);

	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(ra_plot_mfc)
	public:
	virtual BOOL DestroyWindow();
	//}}AFX_VIRTUAL

 protected:
	plugin_handle _plugin;

	CDC *_save_dc;
	CBitmap *_save_bm;
	int _save_dc_width;
	int _save_dc_height;

	int _plot_frame(CDC * dc);
	void _plot_axis(CDC * dc);
	void _plot_x_axis(CDC * dc);
	void _plot_y_axis(CDC * dc);

	void plot_annot(CDC *dc);

	int _plot_data(CDC * dc);
	void _plot_line(CDC * dc, struct ra_plot_pair *pair, POINT * pt, int num);
	void _plot_symbol(CDC * dc, struct ra_plot_pair *pair, POINT * pt, int num);

	int _plot_legend(CDC * dc);

	int _plot_pos();

	// Generierte Message-Map-Funktionen
      protected:
	//{{AFX_MSG(ra_plot_mfc)
	  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
  DECLARE_MESSAGE_MAP()};	// class ra_plot_mfc


#endif // RA_PLOT_MFC_H
