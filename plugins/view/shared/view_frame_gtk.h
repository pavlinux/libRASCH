/*----------------------------------------------------------------------------
 * view_frame_gtk.h
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

#ifndef VIEW_FRAME_GTK_H
#define VIEW_FRAME_GTK_H

#include <gtkmm/widget.h>
#include <gtkmm/scrollbar.h>
#include <gtkmm/box.h>
#include "view_frame_general.h"


class view_frame_gtk : public Gtk::VBox, public virtual view_frame_general
{
public:
	view_frame_gtk(meas_handle h, Gtk::Widget *p, char *view_name);
	~view_frame_gtk();

	void init(Gtk::Widget *child);
	Gtk::Widget *view() { return view_area; }

	virtual void init_scrollbar(long page_width, long num_samples, double max_xscale);
	virtual void set_scrollbar_pos(long pos);
	void scrollbar_value_changed();

protected:
//	virtual bool event(QEvent *ev);

	Gtk::Scrollbar *scrollbar;
	Glib::ustring name;
	Gtk::Widget *view_area;
}; /* class view_frame_gtk */


#endif /* VIEW_FRAME_GTK_H */
