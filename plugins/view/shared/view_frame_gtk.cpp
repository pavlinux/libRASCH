/*----------------------------------------------------------------------------
 * view_frame_gtk.cpp
 *
 *
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2006, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id: $
 *--------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>

#include <gtkmm/adjustment.h>
#include "view_frame_gtk.h"


view_frame_gtk::view_frame_gtk(meas_handle h, Gtk::Widget *p, char *view_name)
	: view_frame_general(h)
{
	view_area = NULL;
	name = view_name;
	scrollbar = NULL;
}


view_frame_gtk::~view_frame_gtk()
{
}


void
view_frame_gtk::init(Gtk::Widget *child)
{
	view_area = child;

	if (strncmp(name.c_str(), "ra-sub-view", 11) != 0)
	{		
		// this view is not a ra-sub-view -> add scrollbar		
		scrollbar = new Gtk::HScrollbar();
		pack_start(*view_area);
		pack_end(*scrollbar, Gtk::PACK_SHRINK);

		Gtk::Adjustment *a = scrollbar->get_adjustment();
		a->signal_value_changed().connect(sigc::mem_fun(*this, &view_frame_gtk::scrollbar_value_changed));
	}
} // init()


// bool
// view_frame_gtk::event(QEvent *ev)
// {
// 	if (ev->type() == QEvent::Resize)
// 	{
// 		if (_scrollbar)
// 			_view_area->resize(width(), height() - _scrollbar->height());
// 		else
// 			_view_area->resize(width(), height());
// 	}

// 	if (ev->type() == QEvent::KeyPress)
// 		qApp->notify(_view_area, ev);

// 	return QWidget::event(ev);
// } // event()


void
view_frame_gtk::init_scrollbar(long page_width, long num_samples, double max_xscale)
{
	if (!scrollbar)
		return;

	_page_width = page_width;
	_max_xscale = max_xscale;

	double num_pages = num_samples / page_width;
	if (((double)((long)num_pages) - num_pages) > 0.0)
		num_pages++;

	double curr_value = scrollbar->get_value();
	
	if ((num_pages <= 0) || (num_samples <= page_width))
	{
		scrollbar->hide();
		return;
	}
	
	if (num_samples > page_width)
	{
		scrollbar->show();
		scrollbar->set_range(0, num_samples-page_width);
		scrollbar->set_increments(page_width/10, page_width);
	}

	scrollbar->set_value(curr_value);
} // init_scrollbar()


void
view_frame_gtk::scrollbar_value_changed()
{
	if (!scrollbar || _block_signals)
		return;

	_block_signals = true;

	double value = scrollbar->get_value();

	struct comm_para *para = new struct comm_para[2];
	para[0].type = PARA_LONG;
	para[0].v.l = (long)value;
	para[1].type = PARA_DOUBLE;
	para[1].v.d = _max_xscale;
	ra_comm_emit(_meas, "view-frame-gtk", "pos-change", 2, para);
	delete[] para;

	_block_signals = false;
} // scrollbar_value_changed()


void
view_frame_gtk::set_scrollbar_pos(long pos)
{
	if (!scrollbar || _block_signals)
		return;

	_block_signals = true;

	scrollbar->set_value(pos);

	_block_signals = false;
} // set_scrollbar_pos()
