#include <Qt/qapplication.h>
#include <QtGui/qpainter.h>
#include <Qt/qevent.h>
#include <Qt/qeventloop.h>
#include <QtGui/QMouseEvent>
#include <QtGui/QKeyEvent>
#include <stdio.h>
#include <ctype.h>

#include "ra_view_qt.h"

#include <ra_pl_comm.h>


ra_view_qt::ra_view_qt(rec_handle rh, QWidget *p/*=0*/)
	: ra_view_general(rh), QWidget(p) 
{
	_split_main = NULL;
} // constructor


ra_view_qt::~ra_view_qt()
{
} // destructor


void
ra_view_qt::do_update()
{
} // do_update()


bool ra_view_qt::event(QEvent *ev)
{
	int focus_save = -1;

	bool ret = QWidget::event(ev);

	if (ev->type() == QEvent::KeyPress)
	{
		// The keyPressEvent must be set to accepted. Without this, the
		// event is not deleted from the event queue because the default event handler
		// was already called (see above). Without calling the default event handler,
		// there would be problems with the initial size of ra-view.
		// TODO: Check if this is ok or how to do it more correctly. It works now, but
		// I am not sure if this is the correct way and what side effects this has.
		((QKeyEvent *)ev)->accept();
		if (_view_with_focus >= 0)
		{
			QWidget *v = (QWidget *)(_view_info[_view_with_focus].view_handle);
			qApp->notify(v, ev);
		}
	}

	if (ev->type() == QEvent::Resize)
	{
		_split_main->resize(width(), height());
		get_child_dimensions();
	}

	if (ev->type() == QEvent::MouseButtonPress)
	{
		// Because there are not the correct values available after startup,
		// get the values here also (additional to the resize event);
		// I think that getting the values costs only very little time
		get_child_dimensions();

		QPoint pt_gl;
		pt_gl.setX(((QMouseEvent *) ev)->globalX());
		pt_gl.setY(((QMouseEvent *) ev)->globalY());

		QPoint pt = mapFromGlobal(pt_gl);

		focus_save = _view_with_focus;
		_view_with_focus = 0;
		for (int i = 0; i < _num_view; i++)
		{
			if ((pt.x() >= _view_info[i].x)
			    && (pt.x() <= (_view_info[i].x + _view_info[i].width))
			    && (pt.y() >= _view_info[i].y)
			    && (pt.y() <= (_view_info[i].y + _view_info[i].height)))
			{
				_view_with_focus = i;
				QWidget *v = (QWidget *)(_view_info[_view_with_focus].view_handle);
				v->setFocus(Qt::MouseFocusReason);
				break;
			}
		}
	}

//	show_focus(focus_save);

	return ret;
} // event()


void *
ra_view_qt::create_main_view()
{
	if (_split_main == NULL)
		_split_main = new ra_view_splitter(this);
	_split_main->setOrientation(Qt::Vertical);

	connect(_split_main, SIGNAL(sig_move_sizer()), this, SLOT(get_child_dimensions()));

	return _split_main;
} // create_main_view()


int
ra_view_qt::add_single_view(void * /*view*/)
{
	// The functionality of this function is not needed in the
	// Qt version (I'm not sure why I added it initially), therefore
	// just go out (but leave the code, maybe it will be needed later)
	return 0;

// 	if (!_split_main)
// 		create_main_view();

// 	_split_main->addWidget((QWidget *)view);

// 	return 0;
} // add_single_view()


int
ra_view_qt::get_view_dimensions(void *vh, long *width, long *height)
{
	QWidget *v = (QWidget *) vh;
	*width = v->width();
	*height = v->height();

	return 0;
} // get_view_dimensions()


int
ra_view_qt::pos_view(void *vh, long x, long y)
{
	QWidget *v = (QWidget *)vh;
	v->move(x, y);

	return 0;
} // pos_view()


void
ra_view_qt::show_focus(int prev_focus/*=-1*/)
{
	if (_num_view <= 1)
		return;		// no need to show focus if we have only one view

	if (prev_focus == _view_with_focus)
		return;

	if (prev_focus != -1)
	{
		QWidget *v = (QWidget *)(_view_info[prev_focus].view_handle);
		v->repaint();
	}

	QWidget *v = (QWidget *)(_view_info[_view_with_focus].view_handle);
	if (!v)
		return;

	long w = v->width();
	long h = v->height();

	QPen pen;
	pen.setWidth(3);
//	pen.setColor();

	QPainter p;
	p.begin(v);

	p.setPen(pen);

	p.drawRect(0, 0, w, h);

//	p.flush();
} // show_focus()


void
ra_view_qt::get_child_dimensions()
{
	long curr_y_pos = 0;
	for (int i = 0; i < _num_view; i++)
	{
		QWidget *v = (QWidget *)(_view_info[i].view_handle);
		_view_info[i].width = v->width();
		_view_info[i].height = v->height();
		_view_info[i].y = curr_y_pos;

		curr_y_pos += _view_info[i].height;
	}
} // get_child_dimensions()


