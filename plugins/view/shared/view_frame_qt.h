/*----------------------------------------------------------------------------
 * view_frame_qt.h
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

#ifndef VIEW_FRAME_QT_H
#define VIEW_FRAME_QT_H

#include <QtGui/qwidget.h>
#include <QtGui/qscrollbar.h>
#include <QtGui/qlayout.h>
#include <Qt/qevent.h>
#include "view_frame_general.h"


class view_frame_qt:public QWidget, public virtual view_frame_general
{
	Q_OBJECT
public:
	view_frame_qt(meas_handle h, QWidget *p, char *name);
	~view_frame_qt();

	void init(QWidget *child);
	QWidget *view() { return _view_area; }

	virtual void init_scrollbar(long page_width, long num_samples, double max_xscale);
	virtual void set_scrollbar_pos(long pos);

protected slots:
	void scrollbar_value_changed(int value);

protected:
	virtual bool event(QEvent *ev);

	QString _name;
	QVBoxLayout *_main;
	QWidget *_view_area;
	QScrollBar *_scrollbar;
}; // class view_frame_qt


#endif // VIEW_FRAME_QT_H
