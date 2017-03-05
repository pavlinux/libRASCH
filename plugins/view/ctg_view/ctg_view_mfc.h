/*----------------------------------------------------------------------------
 * ctg_view_mfc.h
 *
 * 
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002-2003, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Header: /home/cvs_repos.bak/librasch/plugins/view/ctg_view/ctg_view_mfc.h,v 1.5 2003/11/27 12:09:30 rasch Exp $
 *--------------------------------------------------------------------------*/

#ifndef CTG_VIEW_MFC_H
#define CTG_VIEW_MFC_H


#include "../ts_view/ts_view_mfc.h"
#include "ctg_view_general.h"

class ctg_view_mfc:public ts_view_mfc, public ctg_view_general
{
public:
	ctg_view_mfc();
	~ctg_view_mfc();

	virtual void update();
	virtual void init(meas_handle mh, plugin_handle pl, int num_ch, int *ch);

	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(ctg_view_mfc)
	//}}AFX_VIRTUAL

protected:
	virtual void plot_grid(CDC *dc);
	void plot_time(CDC *dc);
	void plot_uc_events(CDC *dc);

	plugin_handle _plugin;

	CPen _grid_pen1;
	CPen _grid_pen2;
	CPen _grid_pen3;
	CPen _uc_event_pen;

	// Generierte Message-Map-Funktionen
      protected:
	//{{AFX_MSG(ctg_view_mfc)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	//}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};	// class ctg_view_mfc

#endif // CTG_VIEW_MFC_H
