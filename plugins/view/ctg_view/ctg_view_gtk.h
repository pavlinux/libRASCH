/*----------------------------------------------------------------------------
 * ctg_view_gtk.h
 *
 *
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2007, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id: $
 *--------------------------------------------------------------------------*/

#ifndef CTG_VIEW_GTK_H
#define CTG_VIEW_GTK_H

#include "ctg_view_general.h"
#include "../ts_view/ts_view_gtk.h"


class ctg_view_gtk:public ts_view_gtk, public ctg_view_general
{
public:
	ctg_view_gtk(meas_handle h, int num_ch, int *ch);
	~ctg_view_gtk();

	virtual void do_update();

protected:
	virtual bool on_expose_event(GdkEventExpose *e);
	virtual void do_additional_drawing();
	virtual bool on_button_press_event(GdkEventButton *e);


//	virtual void resizeEvent(QResizeEvent *ev);

	void calc_scale();

	virtual void plot_grid();
	void plot_time();
	void plot_uc_events();
}; // class ctg_view_gtk


#endif // CTG_VIEW_GTK_H
