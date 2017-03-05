/*----------------------------------------------------------------------------
 * ctg_view_qt.cpp
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

#include <QtGui/QPainter>
//Added by qt3to4:
#include <QtGui/QMouseEvent>
#include <QtGui/QPaintEvent>
#include <stdio.h>
#include <ctype.h>


#include "ctg_view_qt.h"

#define SHOW_BEAT_DEBUG_INFO  0


ctg_view_qt::ctg_view_qt(meas_handle h, int num_ch, int *ch, QWidget *p/*=0*/)
	: ts_view_general(h, num_ch, ch),
	  ts_view_qt(h, num_ch, ch, p),
	  ctg_view_general(h)
{
	setAutoFillBackground(true);
	_palette.setColor(backgroundRole(), QColor(255, 255, 255));
	setPalette(_palette);

	_grid_pen.setColor(QColor(0, 255, 155));
	_grid_pen.setWidth(1);
	_grid_pen.setStyle(Qt::SolidLine);
} // constructor


ctg_view_qt::~ctg_view_qt()
{
} // destructor


void
ctg_view_qt::do_update()
{
} // do_update()


void
ctg_view_qt::paintEvent(QPaintEvent *ev)
{
	if (!_fixed_height)
		calc_scale();

	ts_view_qt::paintEvent(ev);

	QPainter p(this);

	plot_time(&p);
	plot_uc_events(&p);

	// status bar
	if (_ts_opt.show_status && _num_sort)
	{
		char t[100];
		sprintf(t, "%ld / %ld", (_curr_sort + 1), _num_sort);
		p.drawText(_left_space, this->height() - 19,
			   this->width() - _left_space - _right_space - 4, 15, Qt::AlignRight, t);
	}
} // paintEvent()


void
ctg_view_qt::calc_scale()
{
	double mm_5v = 5.0 * _pixmm_v;
	int h = _screen_height - _top_space - _bottom_space;
 	int toco_height = (int)(((100- 0) / 10) * mm_5v);
	int fhr_height = (int)(((210 - 50) / 10) * mm_5v);

	_height_scale = 1.0;
	if (!_fixed_height)
		_height_scale = (double)h / (double)(fhr_height + toco_height + mm_5v);
	mm_5v *= _height_scale;
	fhr_height = (int)(((210 - 50) / 10) * mm_5v);

	// "correct" channel draw settings
	for (int i = 0; i < _num_ch; i++)
	{
		_ch[i].use_mm_per_unit = 1;
		if (_ch[i].type == RA_CH_TYPE_CTG_FHR)
		{
			_ch[i].fixed_top_win_pos = 0;
			_ch[i].info_space = 0;
			_ch[i].draw_space = fhr_height;
			_ch[i].min_value = 50;
		}
		if (_ch[i].type == RA_CH_TYPE_CTG_UC)
		{
			_ch[i].fixed_top_win_pos = fhr_height + (int)(2 * mm_5v);
			_ch[i].info_space = 0;
			_ch[i].draw_space = (int)(8 * mm_5v);
			_ch[i].min_value = 0;
		}

		ts_view_qt::calc_scale(i);
	}
} // calc_scale()


void
ctg_view_qt::plot_grid(QPainter *p)
{
	double mm_5v = (5.0 * _pixmm_v) * _height_scale;
	double mm_4v = (4.0 * _pixmm_v) * _height_scale;
	double mm_5h = 5.0 * _pixmm_h;
	double mm_4h = 4.0 * _pixmm_h;

//	int h = height() - _top_space - _bottom_space;
	int w = width() - _left_space - _right_space;

	p->save();

	QFont f;
//	f.defaultFont();
	int font_size = (int)(12 * _height_scale);
	if (font_size > 12)
		font_size = 12;
	if (font_size < 6)
		font_size = 6;
	f.setPointSize(font_size);
	p->setFont(f);

	// FHR area
	_grid_pen.setWidth(1);
	p->setPen(_grid_pen);
	// first vertical lines
	int fhr_height = (int)(((210 - 50) / 10) * mm_5v);
	int n_col = (int) ((double) w / mm_5h);
	for (int i = 0; i <= n_col; i++)
	{
		int x = (int) (mm_5h * (double) i) + _left_space;
		p->drawLine(x, _top_space, x, fhr_height + _top_space);
	}
	// and now the horizontal lines of the FHR area
	double y = -mm_5v;
	for (int i = 210; i >= 50; i -= 10)
	{
		y += mm_5v;

		if ((i == 160) || (i == 120))
			_grid_pen.setWidth(3);
		else if ((i % 20) == 0)
			_grid_pen.setWidth(2);
		else
			_grid_pen.setWidth(1);
		p->setPen(_grid_pen);

		int y_use = (int)y + _top_space;
		p->drawLine(_left_space, y_use, width() - _right_space, y_use);
	}
	// and now the axis
	y = mm_5v;
	QBrush brush(Qt::white);  
	p->setBrush(brush);
	for (int i = 200; i >= 60; i -= 20)
	{
		
		int y_use = (int)y + _top_space;

		int wr = (int)(2*mm_4h);
		int hr = (int)(1.5*mm_4v);
		int xr = (int)(2*mm_5h) + _left_space - wr/2;
		int yr = y_use - hr/2;

		char t[4];
		sprintf(t, "%d", i);
		while (xr < (w - _right_space))
		{
			p->drawRect(xr, yr, wr, hr);
			p->drawText(xr, yr, wr, hr, Qt::AlignCenter, t);
			xr += (int)(20 * mm_5h);
		}

		y += (2.0 * mm_5v);
	}

	// tokogram area
	y = fhr_height + (2 * mm_5v);	// time line
	int top_toko = (int)y + _top_space;
	y -= mm_4v;
	for (int i = 100; i >= 0; i -= 10)
	{
		y += mm_4v;

		if (i == 20)
			_grid_pen.setWidth(3);
		else if ((i % 20) == 0)
			_grid_pen.setWidth(2);
		else
			_grid_pen.setWidth(1);
		p->setPen(_grid_pen);

		int y_use = (int)y + _top_space;
		p->drawLine(_left_space, y_use, width() - _right_space, y_use);
	}
	// and now the vertical lines of the Tokogram area
	// width and height in cm
	_grid_pen.setWidth(1);
	p->setPen(_grid_pen);
	for (int i = 0; i <= n_col; i++)
	{
		int x = (int) (mm_5h * (double) i) + _left_space;
		p->drawLine(x, top_toko, x, (int)y + _top_space);	// use y of horizontal lines
	}
	// and now the axis
	y = fhr_height + (2 * mm_5v) + (2 * mm_4v);
	for (int i = 80; i >= 20; i -= 20)
	{
		
		int y_use = (int)y + _top_space;

		int wr = (int)(2*mm_4h);
		int hr = (int)(1.5*mm_4v);
		int xr = (int)(2*mm_5h) + _left_space - wr/2;
		int yr = y_use - hr/2;

		char t[4];
		sprintf(t, "%d", i);
		while (xr < (w - _right_space))
		{
			p->drawRect(xr, yr, wr, hr);
			p->drawText(xr, yr, wr, hr, Qt::AlignCenter, t);
			xr += (int)(20 * mm_5h);
		}

		y += (2.0 * mm_4v);
	}

	p->restore();
} // plot_grid()


void
ctg_view_qt::plot_time(QPainter *p)
{
	double mm_5 = _height_scale * (5.0 * _pixmm_v);
	int fhr_height = (int)(((210 - 50) / 10) * mm_5);
	int y = fhr_height + (int)mm_5;  // center of time-line

	long curr_sec = _start_sec + (long)((double)_curr_pos / _max_samplerate);
	int min_10_sec = 10 * 60;
	double min_10_pix = 10 * 60 * _max_samplerate * _max_xscale;
	long first_10_sec = min_10_sec - (curr_sec % min_10_sec);
	double first_10_pix = (double)first_10_sec * _max_samplerate * _max_xscale;

	int w = this->width() - _left_space - _right_space;
	char *sec_txt = new char[10];
	double x = first_10_pix + _left_space;
	long sec = curr_sec + first_10_sec;
	while (x < (w + _left_space))
	{
		format_sec(sec, &sec_txt);			
		char t[100];
		sprintf(t, "%s   (1cm/min)", sec_txt);
		p->drawText((int)x + _left_space, y + _top_space - 50, (int)(20 * mm_5), 100, Qt::AlignVCenter, t);
		
		x += min_10_pix;
		sec += min_10_sec;
	}
	delete[] sec_txt;
} // plot_time()


void
ctg_view_qt::plot_uc_events(QPainter *p)
{
	if (_num_events == 0)
		return;

	// calc position of markers (below tocogram area)
	double mm_5v = (5.0 * _pixmm_v) * _height_scale;
	double mm_4v = (4.0 * _pixmm_v) * _height_scale;

	int fhr_height = (int)(((210 - 50) / 10) * mm_5v);
	double y = fhr_height + (2 * mm_5v);
	y += ((100 / 10) * mm_4v);

	int *x, num;
	long *pos;
	get_uc_pos(&x, &pos, &num);

	p->save();

	QPen pen;
	pen.setColor(QColor(0, 0, 255));
	pen.setWidth(3);
	pen.setStyle(Qt::SolidLine);
	p->setPen(pen);

	for (int i = 0; i < num; i++)
	{
		p->drawLine(x[i], (int)y, x[i], (int)(y - mm_5v));
	}

	if (x)
		delete[]x;
	if (pos)
		delete[]pos;

	p->restore();
} // plot_uc_events()


void
ctg_view_qt::mousePressEvent(QMouseEvent * ev)
{
	if (ev->button() == Qt::LeftButton)
	{
		mouse_press(ev->x());

//		long pos = _curr_pos + (long) ((double)(ev->x() - _left_space) / _max_xscale);
//		_cursor_pos = (int) ((double) (pos - _curr_pos) * _max_xscale) + _left_space;
		
//		plot_cursor(NULL);
	}

	ts_view_qt::mousePressEvent(ev);
} // mousePressEvent()


void
ctg_view_qt::resizeEvent(QResizeEvent *ev)
{
	calc_scale();
	ts_view_qt::resizeEvent(ev);
} // resizeEvent()

