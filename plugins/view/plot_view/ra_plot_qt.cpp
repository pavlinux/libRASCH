/*----------------------------------------------------------------------------
 * ra_plot_qt.cpp
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

#include <stdio.h>
#include <stdlib.h>

#include <QtGui/QBitmap>
#include <QtGui/QMouseEvent>
#include <QtGui/QKeyEvent>
#include <QtGui/QPaintEvent>

#include "ra_plot_qt.h"
#include "sel_data_dlg_qt.h"



ra_plot_qt::ra_plot_qt(meas_handle h, struct ra_plot_options *opt, QWidget *p/*=0*/)
	: ra_plot_general(h, opt),
	  QWidget(p)
{
	setFocusPolicy(Qt::StrongFocus);
	setBackgroundRole(QPalette::Light);
	setAutoFillBackground(true);
}


ra_plot_qt::~ra_plot_qt()
{
}


void
ra_plot_qt::update()
{
	if (_dont_response_ev_change)
		return;

	get_plot_data();

	QWidget::update();
} // update()


void
ra_plot_qt::update_pos()
{
	QWidget::update();
} // update_pos()


void
ra_plot_qt::keyPressEvent(QKeyEvent *ev)
{
	long curr = _curr_event;
	bool changed = false;
	switch (ev->key())
	{
	case Qt::Key_Right:
		changed = true;
		if (ev->modifiers() & Qt::ControlModifier)
			curr += 20;
		else
			curr++;
		break;
	case Qt::Key_Left:
		changed = true;
		if (ev->modifiers() & Qt::ControlModifier)
			curr -= 20;
		else
			curr--;
		break;
	case Qt::Key_Home:
		changed = true;
		curr = 0;
		break;
	case Qt::Key_End:
		changed = true;
		curr = _num_events[0]-1;
		break;
	}

	if (changed)
	{
		set_event(curr);
		repaint();
	}
} // keyPressEvent()


void
ra_plot_qt::paintEvent(QPaintEvent * /*ev*/)
{
	QPainter p(this);

	_plot_area_width = width() - (_plot_area_left + _plot_area_right);

	_plot_area_top = 0;
	plot_legend(p);
	if (_plot_area_top < 20)
		_plot_area_top = 20;
	_plot_area_top += 10;

	_plot_area_height = height() - (_plot_area_top + _plot_area_bottom);

	plot_frame(p);

	plot_annot(p);

	p.setClipping(true);
	p.setClipRect(_plot_area_left, _plot_area_top, _plot_area_width, _plot_area_height);
	plot_data(p);
	p.setClipping(false);

	plot_pos(p);
} // paintEvent()


int
ra_plot_qt::plot_frame(QPainter &p)
{
	plot_axis(p);

	QRect r(_plot_area_left, _plot_area_top, _plot_area_width, _plot_area_height);
	p.drawRect(r);

//    _plot_grid();

	return 0;
} // plot_frame()


void
ra_plot_qt::plot_axis(QPainter &p)
{
	plot_x_axis(p);
	plot_y_axis(p);
} // plot_axis()


void
ra_plot_qt::plot_x_axis(QPainter &p)
{
	int y = _plot_area_top + _plot_area_height;

	for (int i = 0; i < _num_x_axis; i++)
	{
		// draw axis-values
		double curr = _x_axis_info[i].start;
		double x_scale = (double)_plot_area_width / (_x_axis_info[i].end - _x_axis_info[i].start);
		double step = _x_axis_info[i].step;
		if (_plot_area_width < 200)
			step *= 2.0;

		int precision = get_tics_precision(step);

		QString s;
		while (curr <= _x_axis_info[i].end)
		{
			int x = (int) ((curr - _x_axis_info[i].start) * x_scale) + _plot_area_left;
			p.drawLine(x, y, x, y + 5);

			char t[20];
			switch (precision)
			{
			case 0:
				sprintf(t, "%.0f", curr);
				break;
			case 1:
				sprintf(t, "%.1f", curr);
				break;
			case 2:
				sprintf(t, "%.2f", curr);
				break;
			case 3:
				sprintf(t, "%.3f", curr);
				break;
			case 4:
				sprintf(t, "%.4f", curr);
				break;
			case 5:
				sprintf(t, "%.5f", curr);
				break;
			default:
				sprintf(t, "%f", curr);
				break;
			}
			s = t;

			p.drawText(x - 30, y + 7, 60, 20, Qt::AlignHCenter, s);

			curr += step;
		}

		// draw unit
		s = _opt->axis[_x_axis_info[i].idx].unit;
		p.drawText(_plot_area_width + _plot_area_left + 10, y + 7, _x_axis_space, 20, Qt::AlignLeft, s);
	}
} // plot_x_axis()


void
ra_plot_qt::plot_y_axis(QPainter &p)
{
	for (int i = 0; i < _num_y_axis; i++)
	{
		if (_y_axis_info[i].dont_draw_axis)
			continue;

		int pos;
		if (_y_axis_info[i].opp_side)
		{
			pos = _y_axis_info[i].axis_num * _y_axis_space;
			pos += _plot_area_left + _plot_area_width;
		}
		else
			pos = (_y_axis_info[i].axis_num + 1) * _y_axis_space;

		// draw axis-line
		p.drawLine(pos, _plot_area_top, pos, _plot_area_top + _plot_area_height);

		// draw axis-values
		double curr = _y_axis_info[i].start;
		double y_scale =
			(double) _plot_area_height / (_y_axis_info[i].end - _y_axis_info[i].start);
		double step = _y_axis_info[i].step;
		if (_plot_area_height < 200)
			step *= 2.0;

		QString s;
		while (curr <= _y_axis_info[i].end)
		{
			int y = (int) ((_y_axis_info[i].end - curr) * y_scale) + _plot_area_top;
			if (_y_axis_info[i].opp_side)
				p.drawLine(pos, y, pos + 5, y);
			else
				p.drawLine(pos, y, pos - 5, y);

			char t[20];
			if (step >= 1)
				sprintf(t, "%.0f", curr);
			else
				sprintf(t, "%f", curr);
			s = t;

			if (_y_axis_info[i].opp_side)
				p.drawText(pos + 7, y - 10, _y_axis_space, 20, Qt::AlignVCenter, s);
			else
				p.drawText(pos - 7 - _y_axis_space, y - 10,
					   _y_axis_space, 20, Qt::AlignRight | Qt::AlignVCenter, s);

			curr += step;
		}

		// draw unit
		s = _opt->axis[_y_axis_info[i].idx].unit;
		if (_y_axis_info[i].opp_side)
			p.drawText(pos + 7, _plot_area_top - 30, _y_axis_space, 20, Qt::AlignVCenter, s);
		else
			p.drawText(pos - 7 - _y_axis_space, _plot_area_top - 30,  _y_axis_space, 20, Qt::AlignRight | Qt::AlignVCenter, s);
	}
} // plot_y_axis()


void
ra_plot_qt::plot_annot(QPainter &p)
{
	p.save();

	QColor col(100, 100, 100);
	QPen pen;
	pen.setColor(col);
	pen.setStyle(Qt::SolidLine);
	p.setPen(pen);

	QBrush br;
	QColor col2(220, 220, 220);
	br.setColor(col2);
	br.setStyle(Qt::SolidPattern);
	p.setBrush(br);

	double xs = _x_axis_info->start * _x_axis_info->scale;
	double xe = _x_axis_info->end * _x_axis_info->scale;
	double xr = xe - xs;
	_x_scale = (double) (_plot_area_width) / xr;

	for (long l = 0; l < _num_annot; l++)
	{
		double x1 = _annot_start[l] - xs;
		double x2 = _annot_end[l] - xs;

		int x1_u = (int) (x1 * _x_scale) + _plot_area_left;
		int x2_u = (int) (x2 * _x_scale) + _plot_area_left;

		p.drawRect(x1_u, _plot_area_top+1, (x2_u - x1_u), _plot_area_height-2);
	}

	p.restore();
} // plot_annot()


int
ra_plot_qt::plot_data(QPainter &p)
{
	for (long l = 0; l < _opt->num_plot_pairs; l++)
	{
		QPoint *a = new QPoint[_num_data[l]];

		double xs = _x_axis_info->start * _x_axis_info->scale;
		double xe = _x_axis_info->end * _x_axis_info->scale;
		double ys = _y_axis_info[l].start * _y_axis_info[l].scale;
		double ye = _y_axis_info[l].end * _y_axis_info[l].scale;

		double xr = xe - xs;
		_x_scale = (double) (_plot_area_width) / xr;
		double yr = ye - ys;
		_y_scale = (double) (_plot_area_height) / yr;

		long cnt = 0;
		for (long m = 0; m < _num_data[l]; m++)
		{
			if (_ignore_data[l][m])
			{
				if (cnt > 0)
				{
					plot_line(p, &(_opt->pair[l]), a, cnt);
					plot_symbol(p, &(_opt->pair[l]), a, cnt);
				}
				cnt = 0;
				continue;
			}

			int x = (int)(((_x_data[l][m] - xs) * _x_scale) + _plot_area_left);
			int y = (int)(((ye - _y_data[l][m]) * _y_scale) + _plot_area_top);
			a[cnt].setX(x);
			a[cnt].setY(y);
			cnt++;
		}
		if (cnt > 0)
		{
			plot_line(p, &(_opt->pair[l]), a, cnt);
			plot_symbol(p, &(_opt->pair[l]), a, cnt);
		}
		delete[] a;
	}

	return 0;
} // plot_data()


void
ra_plot_qt::plot_line(QPainter &p, struct ra_plot_pair *pair, QPoint *a, int num_point)
{
	if (!pair->plot_line)
		return;

	p.save();

	QColor col(pair->line_r, pair->line_g, pair->line_b);
	QPen pen;
	pen.setColor(col);
	switch (pair->line_type)
	{
	case RA_PLOT_LINE_SOLID:
		pen.setStyle(Qt::SolidLine);
		break;
	case RA_PLOT_LINE_DASH:
		pen.setStyle(Qt::DashLine);
		break;
	case RA_PLOT_LINE_DOT:
		pen.setStyle(Qt::DotLine);
		break;
	case RA_PLOT_LINE_DASHDOT:
		pen.setStyle(Qt::DashDotLine);
		break;
	}

	p.setPen(pen);
	p.drawPolyline(a, num_point);

	p.restore();
} // plot_line()


void
ra_plot_qt::plot_symbol(QPainter & p, struct ra_plot_pair *pair, QPoint * /*a*/, int /*num_point*/)
{
	if (!pair->plot_symbol)
		return;

	p.save();

	QColor col(pair->sym_r, pair->sym_g, pair->sym_b);
	QPen pen;
	pen.setColor(col);

	int size = 7;
//	int size2 = size / 2;
	QPixmap sym(size, size);
//	sym.fill(paletteBackgroundColor());
	QPainter p_pix(&sym);
	QBrush br;
	br.setColor(col);
	br.setStyle(Qt::NoBrush);
	p_pix.setPen(pen);
	switch (pair->symbol_type)
	{
	case RA_PLOT_SYM_CIRCLE_FULL:
		br.setStyle(Qt::SolidPattern);
	case RA_PLOT_SYM_CIRCLE:
		p_pix.setBrush(br);
		p_pix.drawEllipse(0, 0, size, size);
		break;
	case RA_PLOT_SYM_RECT_FULL:
		br.setStyle(Qt::SolidPattern);
	case RA_PLOT_SYM_RECT:
		p_pix.setBrush(br);
		p_pix.drawRect(0, 0, size, size);
		break;
	case RA_PLOT_SYM_DIAMOND_FULL:
		br.setStyle(Qt::SolidPattern);
	case RA_PLOT_SYM_DIAMOND:
		p_pix.setBrush(br);
/*		p_pix.moveTo(size2, 0);
		p_pix.lineTo(size - 1, size2);
		p_pix.lineTo(size2, size - 1);
		p_pix.lineTo(0, size2);
		p_pix.lineTo(size2, 0);*/
		break;
	case RA_PLOT_SYM_TRIANGLE_FULL:
		br.setStyle(Qt::SolidPattern);
	case RA_PLOT_SYM_TRIANGLE:
		p_pix.setBrush(br);
/*		p_pix.moveTo(0, size - 1);
		p_pix.lineTo(size2, 0);
		p_pix.lineTo(size - 1, size - 1);
		p_pix.lineTo(0, size - 1);*/
		break;
	}
/*
	sym.setMask(sym.createHeuristicMask());
	for (unsigned int n = 0; n < a.count(); n++)
	{
		int x, y;
		a.point(n, &x, &y);
		bitBlt(this, x - size2, y - size2, &sym, 0, 0, size, size, CopyROP);
	}
*/
	p.restore();
} // plot_symbol()


int
ra_plot_qt::plot_legend(QPainter &p)
{
	if (!(_opt->show_legend))
		return 0;

	int curr_x = _plot_area_left + 5;
	int curr_line = 1;
	int cnt = 0;		// no new line if this is the first
	for (long l = 0; l < _opt->num_plot_pairs; l++)
	{
		int y = curr_line * 20 - 10;

		QString s = _opt->pair[l].name;
		s += " [";
		s += _opt->axis[_opt->pair[l].y_idx].unit;
		s += "]";

		QRect r = p.boundingRect(curr_x + 15, y - 10, 200, 20,
					 Qt::AlignVCenter, s);
		if (((curr_x + r.width() + 15) > (_plot_area_left + _plot_area_width)) && cnt)
		{
			curr_x = _plot_area_left + 5;
			curr_line++;
			y = curr_line * 20 - 10;
		}
		p.drawText(curr_x + 15, y - 10, 200, 20, Qt::AlignVCenter, s);

		QPoint *a = new QPoint[2];
		a[0].setX(curr_x+5);
		a[0].setY(y);
		plot_symbol(p, &(_opt->pair[l]), a, 1);

		a[0].setX(curr_x);
		a[0].setY(y);
		a[1].setX(curr_x+10);
		a[1].setY(y);

		plot_line(p, &(_opt->pair[l]), a, 2);

		curr_x += r.width() + 20;
		cnt++;
	}

	_plot_area_top = 20 * curr_line;

	return 0;
} // plot_legend()


int
ra_plot_qt::plot_pos(QPainter &p)
{
	p.save();

	QPen pen;
	QColor col(0, 0, 0);
	pen.setColor(col);
	p.setPen(pen);

	int pos = (int) ((double) _curr_pos * _x_scale);
	pos += _plot_area_left;
	p.drawLine(pos, _plot_area_top, pos, height() - _plot_area_bottom);

	p.restore();

	return 0;
} // plot_pos()


// TODO: handle ignored values/event-positions correctly
void
ra_plot_qt::mousePressEvent(QMouseEvent * ev)
{
	if (ev->button() == Qt::LeftButton)
	{
		long pos = (long) ((double) (ev->x() - _plot_area_left) / _x_scale);
		long event = 0;
		for (int i = 0; i < _num_data[0]; i++)
		{
 			if (_x_data[0][i] >= ((double)pos + (_x_axis_info[0].start * _x_axis_info[0].scale)))
			{
				event = i;
				break;
			}
		}
		set_event(event);
		repaint();
	}
	else if (ev->button() == Qt::RightButton)
	{
		sel_data_dlg_qt dlg(_meas, _opt);
		if (dlg.exec() == QDialog::Accepted)
		{
			get_plot_data();
			repaint();
		}
	}
} // mousePressEvent()
