/*----------------------------------------------------------------------------
 * view_frame_qt.cpp
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

#include <stdio.h>
#include <stdlib.h>

#include <QtGui/qapplication.h>
#include <QtGui/QVBoxLayout>
#include <QtGui/qevent.h>

#include "view_frame_qt.h"


view_frame_qt::view_frame_qt(meas_handle h, QWidget *p, char *name)
	: view_frame_general(h), QWidget(p)
{
	_scrollbar = NULL;
	_main = NULL;
	_view_area = NULL;
	_name = name;
}


view_frame_qt::~view_frame_qt()
{
}


void
view_frame_qt::init(QWidget *child)
{
	_view_area = child;

	if (strncmp(_name.toAscii().constData(), "ra-sub-view", 11) != 0)
	{
		// this view is not a ra-sub-view -> add scrollbar
		_main = new QVBoxLayout(this);
		_main->setMargin(0);

		_scrollbar = new QScrollBar(Qt::Horizontal, this);
	
		_main->addWidget(_view_area);
		_main->addWidget(_scrollbar);

		connect(_scrollbar, SIGNAL(valueChanged(int)), this, SLOT(scrollbar_value_changed(int)));
		connect(_scrollbar, SIGNAL(sliderMoved(int)), this, SLOT(scrollbar_value_changed(int)));
	}
} // init()


bool
view_frame_qt::event(QEvent *ev)
{
	if (ev->type() == QEvent::Resize)
	{
		if (_scrollbar)
			_view_area->resize(width(), height() - _scrollbar->height());
		else
			_view_area->resize(width(), height());
	}

	if (ev->type() == QEvent::KeyPress)
		qApp->notify(_view_area, ev);

	return QWidget::event(ev);
} // event()


void
view_frame_qt::init_scrollbar(long page_width, long num_samples, double max_xscale)
{
	if (!_scrollbar)
		return;

	_page_width = page_width;
	_max_xscale = max_xscale;

	double num_pages = num_samples / page_width;
	if (((double)((long)num_pages) - num_pages) > 0.0)
		num_pages++;

	if (num_pages <= 0)
	{
		_scrollbar->hide();
		return;
	}
	
	if (num_samples > page_width)
	{
		_scrollbar->show();
		_scrollbar->setMinimum(0);
		_scrollbar->setMaximum((num_samples-page_width));
		_scrollbar->setPageStep(page_width);
		_scrollbar->setSingleStep(page_width/10);
	}
	else
		_scrollbar->hide();
} // init_scrollbar()


void
view_frame_qt::scrollbar_value_changed(int value)
{
	if (!_scrollbar || _block_signals)
		return;

	_block_signals = true;

	// this call fixed the problems with emitting the signal twice when
	// the drawing take too much time
	qApp->processEvents();

	struct comm_para *para = new struct comm_para[2];
	para[0].type = PARA_LONG;
	para[0].v.l = (long)value;
	para[1].type = PARA_DOUBLE;
	para[1].v.d = _max_xscale;
	ra_comm_emit(_meas, "view-frame-qt", "pos-change", 2, para);
	delete[] para;

	update();

	_block_signals = false;
} // scrollbar_value_changed()


void
view_frame_qt::set_scrollbar_pos(long pos)
{
	if (!_scrollbar || _block_signals)
		return;

	_block_signals = true;

	_scrollbar->setValue(pos);

	_block_signals = false;
} // set_scrollbar_pos()
