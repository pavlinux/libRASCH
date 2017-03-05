/*----------------------------------------------------------------------------
 * ctg_view_gtk.cpp
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

#include <stdio.h>
#include <ctype.h>


#include "ctg_view_gtk.h"

#define SHOW_BEAT_DEBUG_INFO  0


ctg_view_gtk::ctg_view_gtk(meas_handle h, int num_ch, int *ch)
	: ts_view_general(h, num_ch, ch),
	  ts_view_gtk(h, num_ch, ch),
	  ctg_view_general(h)
{
 	bg_color.set_rgb(65535, 65535, 65535);
	grid_color.set_rgb(0, 65535, 39835);

	Glib::RefPtr<Gdk::Colormap> colormap = get_default_colormap();
	colormap->alloc_color(bg_color);
	colormap->alloc_color(grid_color);
} // constructor


ctg_view_gtk::~ctg_view_gtk()
{
} // destructor


void
ctg_view_gtk::do_update()
{
} // do_update()


bool
ctg_view_gtk::on_expose_event(GdkEventExpose *e)
{
	if (!_fixed_height)
		calc_scale();

	ts_view_gtk::on_expose_event(e);

	// status bar
// 	if (_ts_opt.show_status && _num_sort)
// 	{
// 		char t[100];
// 		sprintf(t, "%ld / %ld", (_curr_sort + 1), _num_sort);
// 		Glib::RefPtr<Pango::Layout> layout = create_pango_layout(t);
// 		int w, h;
// 		layout->get_pixel_size(w, h);
// 		pixmap->draw_layout(gc, _left_spacescreen_width - _right_space - w - 1, _screen_height - _bottom_space, layout);
// 		p.drawText(_left_space, this->height() - 19,
// 			   this->width() - _left_space - _right_space - 4, 15, Qt::AlignRight, t);
// 	}

	return true;
} // paintEvent()


void 
ctg_view_gtk::do_additional_drawing()
{
	plot_time();
	plot_uc_events();
} /* do_additional_drawing() */


void
ctg_view_gtk::calc_scale()
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

		ts_view_gtk::calc_scale(i);
	}
} // calc_scale()


void
ctg_view_gtk::plot_grid()
{
	double mm_5v = (5.0 * _pixmm_v) * _height_scale;
	double mm_4v = (4.0 * _pixmm_v) * _height_scale;
	double mm_5h = 5.0 * _pixmm_h;
	double mm_4h = 4.0 * _pixmm_h;

//	int h = height() - _top_space - _bottom_space;
	int w = get_width() - _left_space - _right_space;

// 	QFont f;
// 	int font_size = (int)(12 * _height_scale);
// 	if (font_size > 12)
// 		font_size = 12;
// 	if (font_size < 6)
// 		font_size = 6;
// 	f.setPointSize(font_size);
// 	p->setFont(f);

	// FHR area
	gc->set_line_attributes(1, Gdk::LINE_SOLID, Gdk::CAP_ROUND, Gdk::JOIN_ROUND);
	gc->set_foreground(grid_color);
	// first vertical lines
	int fhr_height = (int)(((210 - 50) / 10) * mm_5v);
	int n_col = (int) ((double) w / mm_5h);
	for (int i = 0; i <= n_col; i++)
	{
		int x = (int) (mm_5h * (double) i) + _left_space;
		pixmap->draw_line(gc, x, _top_space, x, fhr_height + _top_space);
	}
	// and now the horizontal lines of the FHR area
	double y = -mm_5v;
	for (int i = 210; i >= 50; i -= 10)
	{
		y += mm_5v;

		if ((i == 160) || (i == 120))
			gc->set_line_attributes(3, Gdk::LINE_SOLID, Gdk::CAP_ROUND, Gdk::JOIN_ROUND);
		else if ((i % 20) == 0)
			gc->set_line_attributes(2, Gdk::LINE_SOLID, Gdk::CAP_ROUND, Gdk::JOIN_ROUND);
		else
			gc->set_line_attributes(1, Gdk::LINE_SOLID, Gdk::CAP_ROUND, Gdk::JOIN_ROUND);

		int y_use = (int)y + _top_space;
		pixmap->draw_line(gc, _left_space, y_use, get_width() - _right_space, y_use);
	}
	// and now the axis
	y = mm_5v;
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
			gc->set_foreground(bg_color);
			pixmap->draw_rectangle(gc, true, xr, yr, wr, hr);
			gc->set_foreground(grid_color);
			pixmap->draw_rectangle(gc, false, xr, yr, wr, hr);

			Glib::RefPtr<Pango::Layout> layout = create_pango_layout(t);
			int w, h;
			layout->get_pixel_size(w, h);
			pixmap->draw_layout(gc, xr + wr/2 - w/2, yr + hr/2 - h/2, layout);

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
			gc->set_line_attributes(3, Gdk::LINE_SOLID, Gdk::CAP_ROUND, Gdk::JOIN_ROUND);
		else if ((i % 20) == 0)
			gc->set_line_attributes(2, Gdk::LINE_SOLID, Gdk::CAP_ROUND, Gdk::JOIN_ROUND);
		else
			gc->set_line_attributes(1, Gdk::LINE_SOLID, Gdk::CAP_ROUND, Gdk::JOIN_ROUND);

		int y_use = (int)y + _top_space;
		pixmap->draw_line(gc, _left_space, y_use, get_width() - _right_space, y_use);
	}
	// and now the vertical lines of the Tokogram area
	// width and height in cm
	gc->set_line_attributes(1, Gdk::LINE_SOLID, Gdk::CAP_ROUND, Gdk::JOIN_ROUND);
	for (int i = 0; i <= n_col; i++)
	{
		int x = (int) (mm_5h * (double) i) + _left_space;
		pixmap->draw_line(gc, x, top_toko, x, (int)y + _top_space);	// use y of horizontal lines
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
			gc->set_foreground(bg_color);
			pixmap->draw_rectangle(gc, true, xr, yr, wr, hr);
			gc->set_foreground(grid_color);
			pixmap->draw_rectangle(gc, false, xr, yr, wr, hr);

			Glib::RefPtr<Pango::Layout> layout = create_pango_layout(t);
			int w, h;
			layout->get_pixel_size(w, h);
			pixmap->draw_layout(gc, xr + wr/2 - w/2, yr + hr/2 - h/2, layout);

			xr += (int)(20 * mm_5h);
		}

		y += (2.0 * mm_4v);
	}
} // plot_grid()


void
ctg_view_gtk::plot_time()
{
	double mm_5 = _height_scale * (5.0 * _pixmm_v);
	int fhr_height = (int)(((210 - 50) / 10) * mm_5);
	int y = fhr_height + (int)mm_5;  // center of time-line

	long curr_sec = _start_sec + (long)((double)_curr_pos / _max_samplerate);
	int min_10_sec = 10 * 60;
	double min_10_pix = 10 * 60 * _max_samplerate * _max_xscale;
	long first_10_sec = min_10_sec - (curr_sec % min_10_sec);
	double first_10_pix = (double)first_10_sec * _max_samplerate * _max_xscale;

	int width = get_width() - _left_space - _right_space;
	char *sec_txt = new char[10];
	double x = first_10_pix + _left_space;
	long sec = curr_sec + first_10_sec;

	gc->set_foreground(draw_color);
	while (x < (width + _left_space))
	{
		format_sec(sec, &sec_txt);			
		char t[100];
		sprintf(t, "%s   (1cm/min)", sec_txt);

		Glib::RefPtr<Pango::Layout> layout = create_pango_layout(t);
		int w, h;
		layout->get_pixel_size(w, h);
		pixmap->draw_layout(gc, (int)x, y+_top_space - h/2, layout);

		x += min_10_pix;
		sec += min_10_sec;
	}
	delete[] sec_txt;
} // plot_time()


void
ctg_view_gtk::plot_uc_events()
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

	gc->set_line_attributes(3, Gdk::LINE_SOLID, Gdk::CAP_ROUND, Gdk::JOIN_ROUND);
	gc->set_foreground(Gdk::Color("blue"));

	for (int i = 0; i < num; i++)
	{
		pixmap->draw_line(gc, x[i], (int)y, x[i], (int)(y - mm_5v));
	}

	if (x)
		delete[]x;
	if (pos)
		delete[]pos;
} // plot_uc_events()


bool
ctg_view_gtk::on_button_press_event(GdkEventButton *e)
{
	if (e->button == 1)
	{
		mouse_press(e->x);

//		long pos = _curr_pos + (long) ((double)(ev->x() - _left_space) / _max_xscale);
//		_cursor_pos = (int) ((double) (pos - _curr_pos) * _max_xscale) + _left_space;
		
//		plot_cursor(NULL);
	}

	return ts_view_gtk::on_button_press_event(e);
} // mousePressEvent()


// void
// ctg_view_gtk::resizeEvent(QResizeEvent *ev)
// {
// 	calc_scale();
// 	ts_view_qt::resizeEvent(ev);
// } // resizeEvent()

