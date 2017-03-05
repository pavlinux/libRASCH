/*----------------------------------------------------------------------------
 * ctg_view_qt.h
 *
 *
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2003-2006, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifndef CTG_VIEW_QT_H
#define CTG_VIEW_QT_H

#include "ctg_view_general.h"
#include "../ts_view/ts_view_qt.h"
#include <QtGui/QMouseEvent>
#include <QtGui/QPaintEvent>

#ifdef WIN32
#pragma warning(push)
#pragma warning(disable:4250)
#endif

class ctg_view_qt:public ts_view_qt, public ctg_view_general
{
	Q_OBJECT
public:
	ctg_view_qt(meas_handle h, int num_ch, int *ch, QWidget * p = 0);
	~ctg_view_qt();

	virtual void do_update();

public slots:

protected:
	virtual void paintEvent(QPaintEvent * ev);
	virtual void mousePressEvent(QMouseEvent * ev);
	virtual void resizeEvent(QResizeEvent *ev);

	void calc_scale();
	virtual void plot_grid(QPainter *p);

	void plot_time(QPainter *p);
	void plot_uc_events(QPainter *p);
}; // class ctg_view_qt

#ifdef WIN32
#pragma warning(pop)
#endif

#endif // CTG_VIEW_QT_H
