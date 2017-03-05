/*----------------------------------------------------------------------------
 * ra_plot_qt.h
 *
 *
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002-2007, Raphael Schneider
 * See the file COYPING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifndef RA_PLOT_QT_H
#define RA_PLOT_QT_H

#include <QtGui/QApplication>
#include <QtGui/QWidget>
#include <QtGui/QPen>
#include <QtGui/QMenu>
#include <QtGui/QPainter>
#include <QtGui/QMouseEvent>
#include <QtGui/QKeyEvent>
#include <QtGui/QPaintEvent>

#include "ra_plot_general.h"

#include <ra_plot_structs.h>

class ra_plot_qt : public QWidget, public virtual ra_plot_general
{
	Q_OBJECT
public:
	ra_plot_qt(meas_handle h, struct ra_plot_options *opt, QWidget * p = 0);
	~ra_plot_qt();

	virtual void update();
	virtual void update_pos();

protected slots:

protected:
        virtual void paintEvent(QPaintEvent * ev);
	virtual void keyPressEvent(QKeyEvent * ev);
	virtual void mousePressEvent(QMouseEvent * ev);

	int plot_frame(QPainter &p);
	void plot_axis(QPainter &p);
	void plot_x_axis(QPainter &p);
	void plot_y_axis(QPainter &p);

	void plot_annot(QPainter &p);

	int plot_data(QPainter &p);
	void plot_line(QPainter &p, struct ra_plot_pair *pair, QPoint *a, int num_point);
	void plot_symbol(QPainter &p, struct ra_plot_pair *pair, QPoint *a, int num_point);

	int plot_legend(QPainter &p);

	int plot_pos(QPainter &p);
}; // class ra_plot_qt


#endif // RA_PLOT_QT_H
