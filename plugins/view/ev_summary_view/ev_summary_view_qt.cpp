#include <QtGui/QPainter>
#include <QtGui/QMouseEvent>
#include <QtGui/QKeyEvent>
#include <QtGui/QCursor>

#include <stdio.h>
#include <ctype.h>

#define _LIBRASCH_BUILD
#include <ra.h>
#include <ra_priv.h>

#include "ev_summary_view_qt.h"


ev_summary_view_qt::ev_summary_view_qt(meas_handle mh, sum_handle sh, QWidget *p/*=0*/)
	: QWidget(p),
	  ev_summary_view_general(mh, sh)
{
	setBackgroundRole(QPalette::Light);
	setAutoFillBackground(true);

	// get resolution infos
	_pixmm_v = (double)height() / (double)heightMM();
	_pixmm_h = (double)width() / (double)widthMM();
	
	ev_summary_view_general::init();

	int w = X_OFFSET + (_num_sum_part * (EV_SUMMARY_DIST + _max_part_width));
	int h = Y_OFFSET + (_num_ch * EV_SUMMARY_HEIGHT) + 10;
	resize(w, h);
} // constructor


ev_summary_view_qt::~ev_summary_view_qt()
{
} // destructor


void
ev_summary_view_qt::update()
{
	repaint();
} // update()


void
ev_summary_view_qt::paintEvent(QPaintEvent *ev)
{
	int cont = 1;

	int w = X_OFFSET + (_num_sum_part * (EV_SUMMARY_DIST + _max_part_width));
	int h = Y_OFFSET + (_num_ch * EV_SUMMARY_HEIGHT) + 10;
	resize(w, h);

	QRect r = ev->rect();

	QPainter p(this);

	for (long l = 0; l < _num_sum_part; l++)
	{
		int x = X_OFFSET + (l * (EV_SUMMARY_DIST + _max_part_width));
//		int x_next = X_OFFSET + ((l+1) * (EV_SUMMARY_DIST + _max_part_width));

		long part_idx = _part_order[l];
		switch (_num_dim)
		{
		case 1:
			plot_one_dim(&p, x, part_idx);
			break;
		case 2:
		case 3:
		default:
			_ra_set_error(_mh, RA_ERR_UNSUPPORTED, "plugin-ev-summary-view");
			cont = 0;
			break;
		}

		if (!cont)
			return;
	}
	
 	show_selected_templ(&p);
} // drawContents()


void
ev_summary_view_qt::plot_one_dim(QPainter *p, long x_start, long part_idx)
{
 	QString s, t;
 	s = t.setNum(_sum_part[part_idx].part_id);
 	p->drawText(x_start, 5, _max_part_width, 20, Qt::AlignHCenter, s);
 	s = t.setNum(_sum_part[part_idx].num_events);
 	p->drawText(x_start, 25, _max_part_width, 20, Qt::AlignHCenter, s);
	
	plot_ev_summary_1d(p, x_start, part_idx);
} // plot_one_dim()


void
ev_summary_view_qt::plot_ev_summary_1d(QPainter *p, long x_start, long part_idx)
{
	for (long l = 0; l < _num_ch; l++)
		plot_channel_1d(p, x_start, part_idx, l);
} // plot_ev_summary()


void
ev_summary_view_qt::plot_channel_1d(QPainter *p, long x_start, long part_idx, long ch)
{
	long x_offset = (_max_part_width_su / 2) - _fiducial_offset[ch];

	long num = _sum_part[part_idx].num_data[ch][0];
	QPoint *pts = new QPoint[num];
	for (long l = 0; l < num; l++)
	{
		int x = (int)((double)(l + x_offset) * _ch[ch].xscale);
		int y = (int)(_sum_part[part_idx].raw[ch][0][l]);

		// move to position
		x += x_start;
		y += (EV_SUMMARY_HEIGHT * ch);

		pts[l].setX(x);
		pts[l].setY(y + Y_OFFSET);
	}
	p->drawPolyline(pts, num);

	delete[] pts;
} // plot_channel()


void
ev_summary_view_qt::show_selected_templ(QPainter * p)
{
	if (_curr_sum_part_idx == -1)
		return;

	int width = _max_part_width + EV_SUMMARY_DIST;

	QRect rect;
	int t = _curr_sum_part_idx * width + X_OFFSET;
	rect.setLeft(t);
	rect.setTop(Y_OFFSET);
	rect.setRight(t + _max_part_width);
	rect.setBottom(Y_OFFSET + (EV_SUMMARY_HEIGHT * _num_ch));
	
 	p->drawRect(rect);
} // show_selected_templ()


void
ev_summary_view_qt::mouseDoubleClickEvent(QMouseEvent * ev)
{
	int x, y;
//	viewportToContents(ev->x(), ev->y(), x, y);
	x = ev->x();
	y = ev->y();

	_curr_sum_part_idx = ((x - X_OFFSET) / (_max_part_width + EV_SUMMARY_DIST));
	_curr_sum_part_id = _sum_part[_part_order[_curr_sum_part_idx]].part_id;

 	ev_summary_select();

	repaint();
} // mouseDoubleClickEvent()


void
ev_summary_view_qt::keyPressEvent(QKeyEvent * ev)
{
	bool do_repaint = false;
	bool new_select = false;
	switch (ev->key())
	{
	case Qt::Key_Right:
		if (_curr_sum_part_idx < (_num_sum_part-1))
		{
			_curr_sum_part_idx++;
			new_select = true;
		}
		ev->accept();
		break;
	case Qt::Key_Left:
		if (_curr_sum_part_idx > 0)
		{
			_curr_sum_part_idx--;
			new_select = true;
		}
		ev->accept();
		break;
	case Qt::Key_Home:
		_curr_sum_part_idx = 0;
		new_select = true;
		ev->accept();
		break;
	case Qt::Key_End:
		_curr_sum_part_idx = _num_sum_part - 1;
		new_select = true;
		ev->accept();
		break;
	default:
		if (!ev->text().isEmpty())
		{
			char c = ev->text().at(0).toAscii();
			do_repaint = key_press((char)(toupper(c)));
		}
		break;
	}

	if (new_select)
	{
		update();
		ev_summary_select();
	}

	if (do_repaint)
		update();
} // keyPressEvent()


void
ev_summary_view_qt::unset_cursor()
{
	unsetCursor();
} // unset_cursor()


void
ev_summary_view_qt::set_busy_cursor()
{
	setCursor(Qt::WaitCursor);
} // set_busy_cursor()

