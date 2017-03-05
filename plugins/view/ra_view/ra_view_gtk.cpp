#include <stdio.h>
#include <ctype.h>

#include "ra_view_gtk.h"

#include <ra_pl_comm.h>


ra_view_gtk::ra_view_gtk(rec_handle rh)
	: ra_view_general(rh)
{
	curr_pane = 0;
	num_sub_panes = 0;
	sub_panes = NULL;
} // constructor


ra_view_gtk::~ra_view_gtk()
{
} // destructor


void
ra_view_gtk::do_update()
{
} // do_update()


// bool ra_view_gtk::event(QEvent *ev)
// {
// 	int focus_save = -1;

// 	bool ret = QWidget::event(ev);

// 	if (ev->type() == QEvent::KeyPress)
// 	{
// 		// The keyPressEvent must be set to accepted. Without this, the
// 		// event is not deleted from the event queue because the default event handler
// 		// was already called (see above). Without calling the default event handler,
// 		// there would be problems with the initial size of ra-view.
// 		// TODO: Check if this is ok or how to do it more correctly. It works now, but
// 		// I am not sure if this is the correct way and what side effects this has.
// 		((QKeyEvent *)ev)->accept();
// 		if (_view_with_focus >= 0)
// 		{
// 			QWidget *v = (QWidget *)(_view_info[_view_with_focus].view_handle);
// 			qApp->notify(v, ev);
// 		}
// 	}

// 	if (ev->type() == QEvent::Resize)
// 	{
// 		_split_main->resize(width(), height());
// 		get_child_dimensions();
// 	}

// 	if (ev->type() == QEvent::MouseButtonPress)
// 	{
// 		// Because there are not the correct values available after startup,
// 		// get the values here also (additional to the resize event);
// 		// I think that getting the values costs only very little time
// 		get_child_dimensions();

// 		QPoint pt_gl;
// 		pt_gl.setX(((QMouseEvent *) ev)->globalX());
// 		pt_gl.setY(((QMouseEvent *) ev)->globalY());

// 		QPoint pt = mapFromGlobal(pt_gl);

// 		focus_save = _view_with_focus;
// 		_view_with_focus = 0;
// 		for (int i = 0; i < _num_view; i++)
// 		{
// 			if ((pt.x() >= _view_info[i].x)
// 			    && (pt.x() <= (_view_info[i].x + _view_info[i].width))
// 			    && (pt.y() >= _view_info[i].y)
// 			    && (pt.y() <= (_view_info[i].y + _view_info[i].height)))
// 			{
// 				_view_with_focus = i;
// 				break;
// 			}
// 		}
// 	}

// //	show_focus(focus_save);

// 	return ret;
// } // event()


void *
ra_view_gtk::create_main_view()
{
// 	if (_split_main == NULL)
// 		_split_main = new ra_view_splitter(this);
// 	_split_main->setOrientation(Qt::Vertical);

// 	connect(_split_main, SIGNAL(sig_move_sizer()), this, SLOT(get_child_dimensions()));

// 	return _split_main;
	return this;
} // create_main_view()


int
ra_view_gtk::add_single_view(void *view)
{
// 	if (curr_pane == 0)
// 		pack1(*(Gtk::Widget *)view);
// 	else if (curr_pane == 1)
// 		pack2(*(Gtk::Widget *)view);
// 	((Gtk::Widget *)view)->show();
// 	curr_pane++;

	Gtk::VPaned *pane2use = this;

	if (curr_pane > 0)
	{
		num_sub_panes++;
		sub_panes = (Gtk::VPaned **)realloc(sub_panes, sizeof(Gtk::VPaned *) * num_sub_panes);
		sub_panes[num_sub_panes-1] = new Gtk::VPaned;
		pane2use = sub_panes[num_sub_panes-1];
		pane2use->show();

		if (curr_pane == 1)
			pack2(*pane2use);
		else
			sub_panes[num_sub_panes-2]->pack2(*pane2use);
	}
	pane2use->pack1(*(Gtk::Widget *)view);
	((Gtk::Widget *)view)->show();
	curr_pane++;

	return 0;
} // add_single_view()


int
ra_view_gtk::get_view_dimensions(void *vh, long *width, long *height)
{
	Gtk::Widget *v = (Gtk::Widget *)vh;
	*width = v->get_width();
	*height = v->get_height();

	return 0;
} // get_view_dimensions()


int
ra_view_gtk::pos_view(void *vh, long x, long y)
{
//	Gtk::Widget *v = (Gtk::Widget *)vh;
// 	v->move(x, y);

	return 0;
} // pos_view()


void
ra_view_gtk::show_focus(int prev_focus/*=-1*/)
{
	if (_num_view <= 1)
		return;		// no need to show focus if we have only one view

	if (prev_focus == _view_with_focus)
		return;

	if (prev_focus != -1)
	{
//		Gtk::Widget *v = (Gtk::Widget *)(_view_info[prev_focus].view_handle);
// 		v->repaint();
	}

	Gtk::Widget *v = (Gtk::Widget *)(_view_info[_view_with_focus].view_handle);
	if (!v)
		return;

//	long w = v->get_width();
//	long h = v->get_height();

// 	QPen pen;
// 	pen.setWidth(3);
// //	pen.setColor();

// 	QPainter p;
// 	p.begin(v);

// 	p.setPen(pen);

// 	p.drawRect(0, 0, w, h);

//	p.flush();
} // show_focus()


void
ra_view_gtk::get_child_dimensions()
{
	long curr_y_pos = 0;
	for (int i = 0; i < _num_view; i++)
	{
		Gtk::Widget *v = (Gtk::Widget *)(_view_info[i].view_handle);
		_view_info[i].width = v->get_width();
		_view_info[i].height = v->get_height();
		_view_info[i].y = curr_y_pos;

		curr_y_pos += _view_info[i].height;
	}
} // get_child_dimensions()


