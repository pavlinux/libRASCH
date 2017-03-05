/**
 * \file cont_ap_view_qt.h
 *
 * \author Raphael Schneider (librasch@gmail.com)
 */

/*--------------------------------------------------------------------------
 *
 * Copyright (C) 2002-2009, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *
 *--------------------------------------------------------------------------*/

#ifndef CONT_AP_VIEW_QT_H
#define CONT_AP_VIEW_QT_H

#include "cont_ap_view_general.h"
#include "../ts_view/ts_view_qt.h"
#include <QtGui/QMouseEvent>
#include <QtGui/QMenu>
#include <QtGui/QPaintEvent>
#include <QtCore/QSignalMapper>

#ifdef WIN32
#pragma warning(push)
#pragma warning(disable:4250)
#endif

class cont_ap_view_qt:public ts_view_qt, public cont_ap_view_general
{
      Q_OBJECT
 public:
      cont_ap_view_qt(meas_handle h, int num_ch, int *ch, QWidget * p =
		      0, char *name = 0, Qt::WFlags f = 0);
      ~cont_ap_view_qt();
      
      virtual void update();

 public slots:
      void sort_general(const char *evtype, double value);
      void sort_bp(int type);

      void sort_class(int classification);

 protected:
      virtual void paintEvent(QPaintEvent * ev);
	  virtual void mousePressEvent(QMouseEvent *ev);
      
      void plot_beats(QPainter *p);
      void mark_calib_seq(QPainter *p);
      
      void show_pos(QPainter *p, int ch, int x, long pos, bool draw_vert = false);
      
      QSignalMapper *sort_bp_mapper;
      QSignalMapper *sort_class_mapper;

      QMenu *popup_menu_sort;
      QMenu *popup_menu_bp;
}; // class cont_ap_view_qt

#ifdef WIN32
#pragma warning(pop)
#endif

#endif // CONT_AP_VIEW_QT_H
