/**
 * \file cont_ap_view_qt.cpp
 *
 * Qt specific file for arterial-blood-pressure view plugin.
 *
 * \author Raphael Schneider (librasch@gmail.com)
 */

/*--------------------------------------------------------------------------
 *
 * Copyright (C) 2002-2009, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *
 *--------------------------------------------------------------------------*/

#include <QtGui/QPainter>
#include <QtGui/QMouseEvent>
#include <QtGui/QMenu>
#include <QtGui/QPaintEvent>
#include <stdio.h>
#include <ctype.h>

#include "cont_ap_view_qt.h"

#include <ra_ap_morphology.h>


cont_ap_view_qt::cont_ap_view_qt(meas_handle h, int num_ch, int *ch, QWidget *p/*=0*/, char *name/*=0*/, Qt::WFlags f/*=0*/)
	: ts_view_general(h, num_ch, ch),
	  ts_view_qt(h, num_ch, ch, p, name, f),
	  cont_ap_view_general(h)
{
	setFocusPolicy(Qt::StrongFocus);

	popup_menu_bp = _popup_menu->addMenu(QString(gettext("bloodpressure")));

	popup_menu_sort = popup_menu_bp->addMenu(QString(gettext("sort")));

	QAction *act;

	sort_bp_mapper = new QSignalMapper(this);
	act = popup_menu_sort->addAction(QString(gettext("lowest diastolic value first")));
	connect(act, SIGNAL(triggered()), sort_bp_mapper, SLOT(map()));
	sort_bp_mapper->setMapping(act, 0);
	act = popup_menu_sort->addAction(QString(gettext("highest diastolic value first")));
	connect(act, SIGNAL(triggered()), sort_bp_mapper, SLOT(map()));
	sort_bp_mapper->setMapping(act, 1);
	act = popup_menu_sort->addAction(QString(gettext("lowest systolic value first")));
	connect(act, SIGNAL(triggered()), sort_bp_mapper, SLOT(map()));
	sort_bp_mapper->setMapping(act, 2);
	act = popup_menu_sort->addAction(QString(gettext("highest systolic value first")));
	connect(act, SIGNAL(triggered()), sort_bp_mapper, SLOT(map()));
	sort_bp_mapper->setMapping(act, 3);
	connect(sort_bp_mapper, SIGNAL(mapped(int)), this, SLOT(sort_bp(int)));

	popup_menu_sort->addSeparator();

	sort_class_mapper = new QSignalMapper(this);
	act = popup_menu_sort->addAction(QString(gettext("normal_beats")));
	connect(act, SIGNAL(triggered()), sort_class_mapper, SLOT(map()));
	sort_class_mapper->setMapping(act, ECG_CLASS_SINUS);
	act = popup_menu_sort->addAction(QString(gettext("VPCs")));
	connect(act, SIGNAL(triggered()), sort_class_mapper, SLOT(map()));
	sort_class_mapper->setMapping(act, ECG_CLASS_VENT);
	act = popup_menu_sort->addAction(QString(gettext("paced beats")));
	connect(act, SIGNAL(triggered()), sort_class_mapper, SLOT(map()));
	sort_class_mapper->setMapping(act, ECG_CLASS_PACED);
	act = popup_menu_sort->addAction(QString(gettext("Artifacts")));
	connect(act, SIGNAL(triggered()), sort_class_mapper, SLOT(map()));
	sort_class_mapper->setMapping(act, ECG_CLASS_ARTIFACT);
	connect(sort_class_mapper, SIGNAL(mapped(int)), this, SLOT(sort_class(int)));
} // constructor


cont_ap_view_qt::~cont_ap_view_qt()
{
	delete sort_bp_mapper;
	delete sort_class_mapper;

	delete popup_menu_sort;
	delete popup_menu_bp;
} // destructor


void
cont_ap_view_qt::update()
{
	ts_view_qt::update();
}  // update()


void
cont_ap_view_qt::paintEvent(QPaintEvent * ev)
{
	ts_view_qt::paintEvent(ev);

	QPainter p(this);

	plot_beats(&p);

	// mark calibration sequences (if any)
	mark_calib_seq(&p);

	plot_cursor((void *)&p);
} // paintEvent()


void
cont_ap_view_qt::plot_beats(QPainter *p)
{
	if (_num_events == 0)
		return;

	int w, h;
	get_text_extent(p, "120/99 (99)", &w, &h);

	value_handle vh = ra_value_malloc();
	for (int j = 0; j < _num_ch; j++)
	{
		if (!_ch[j].show_it)
			continue;
		
		if (_ch[j].type != RA_CH_TYPE_RR)
			continue;

		int *x = NULL;
		int num = 0;
		long *events = NULL;
		get_beat_pos(_prop_dias_pos, _ch[j].ch_number, &x, &events, &num);
		
		// get mean difference between 5 beats (better behaviour when SVPCs occure)
		int n = num >= 5 ? 5 : num;
		int d = 0;
		int i;  // because of VC++
		for (i = 1; i < n; i++)
			d += (x[i] - x[i-1]);
		if (n > 1)
			d /= (n-1);
		else
			d = 100;
		// check if speed is to high to show values proper
		if (d < w)
		{
			if (x)
				delete[] x;
			if (events)
				delete[] events;
			continue;
		}

		int y_value_line = 2 + (j * 20);
		// print channel name before RR values
		QString s;
		if (_ch[j].inverse)
		{
			s = "-";
			s += _ch[j].name;
		}
		else
			s = _ch[j].name;
		s += ":";
		p->drawText(_ch_info_offset, y_value_line, 50, 16, Qt::AlignLeft, s);

		for (int i = 0; i < num; i++)
		{
			ra_prop_get_value(_prop_flags, events[i], _ch[j].ch_number, vh);
			long flags = (ra_value_is_ok(vh) ? ra_value_get_long(vh) : -1);
			ra_prop_get_value(_prop_syst, events[i], _ch[j].ch_number, vh);
			long syst = (ra_value_is_ok(vh) ? ra_value_get_long(vh) : -1);
			ra_prop_get_value(_prop_dias, events[i], _ch[j].ch_number, vh);
			long dias = (ra_value_is_ok(vh) ? ra_value_get_long(vh) : -1);
			ra_prop_get_value(_prop_mean, events[i], _ch[j].ch_number, vh);
			long mean = (ra_value_is_ok(vh) ? ra_value_get_long(vh) : -1);

			char t[50];
			sprintf(t, "%s%s%ld/%ld (%ld)", ((flags & AP_VALUE_NOK) ? "X-" : ""), 
				((flags & AP_VALUE_INTERPOLATED) ? "I-" : ""), syst, dias, mean);
			p->drawText(x[i] - 100, y_value_line, 200, 16, Qt::AlignHCenter, t);

			// show ibi values
			if (_prop_ibi && (i < (num-1)) && (d > (int)(1.5 * (double)w)))
			{
				ra_prop_get_value(_prop_ibi, events[i], _ch[j].ch_number, vh);
				long ibi = (ra_value_is_ok(vh) ? ra_value_get_long(vh) : -1);
				sprintf(t, "%ld", ibi);
				
				int x_ibi = x[i] + (x[i+1] - x[i]) / 2;
				p->drawText(x_ibi - 100, y_value_line, 200, 16, Qt::AlignHCenter, t);
			}

			// show positions
			if (_prop_syst)	// bloodpressure values available?
			{
				ra_prop_get_value(_prop_syst_pos, events[i], _ch[j].ch_number, vh);				
				long syst_pos = (long)(ra_value_get_double(vh) * _x_scale);

				ra_prop_get_value(_prop_dias_pos, events[i], _ch[j].ch_number, vh);				
				long dias_pos = (long)(ra_value_get_double(vh) * _x_scale);
				
				int pos_x = (int)((double)(syst_pos - _curr_pos) * _max_xscale) + _left_space;
				show_pos(p, j, pos_x, syst_pos);
				pos_x = (int)((double)(dias_pos - _curr_pos) * _max_xscale) + _left_space;
				show_pos(p, j, pos_x, dias_pos);
			}
			// dpdt values available?
			if (_prop_dpdt_min_pos)
			{
				ra_prop_get_value(_prop_dpdt_min_pos, events[i], _ch[j].ch_number, vh);				
				long dpdt_min_pos = (long)(ra_value_get_double(vh) * _x_scale);
				ra_prop_get_value(_prop_dpdt_max_pos, events[i], _ch[j].ch_number, vh);				
				long dpdt_max_pos = (long)(ra_value_get_double(vh) * _x_scale);

				int pos_x = (int)((double)(dpdt_min_pos - _curr_pos) * _max_xscale) + _left_space;
				show_pos(p, j, pos_x, dpdt_min_pos, true);
				pos_x = (int)((double)(dpdt_max_pos - _curr_pos) * _max_xscale) + _left_space;
				show_pos(p, j, pos_x, dpdt_max_pos, true);
			}
		}
		if (x)
			delete[]x;
		if (events)
			delete[]events;
	}
	ra_value_free(vh);
} // plot_beats()


void
cont_ap_view_qt::show_pos(QPainter *p, int ch, int x, long pos, bool draw_vert/* = false */)
{
	if ((p == NULL) || (ch < 0) || (x < 0) || (pos < 0))
		return;

	double max = draw_calc_max_value(ch);
	double val;
	if (ra_raw_get_unit(_rh, _ch[ch].ch_number, pos, 1, &val) != 1)
		return;

	int amp = draw_calc_amplitude(max, val, ch);
	int y = amp + _top_space + _ch[ch].top_win_pos;

	if (draw_vert)
		p->drawLine(x-10, y, x+10, y);
	else
		p->drawLine(x, y + 10, x, y - 10);
} // show_pos()


void
cont_ap_view_qt::mark_calib_seq(QPainter *p)
{
	int num_seq = 0;
	long *start_pos = NULL;
	long *end_pos = NULL;
	long *ch = NULL;
	long *index = NULL;

	get_calib_seq(&num_seq, &start_pos, &end_pos, &ch, &index);

	p->save();

	QBrush b(Qt::BDiagPattern);
	p->setBrush(b);

	for (int i = 0; i < num_seq; i++)
	{
		int start = (int) ((double) (start_pos[i] - _curr_pos) * _max_xscale) + _left_space;
		if (start < _left_space)
			start = _left_space;
		int end = (int) ((double) (end_pos[i] - _curr_pos) * _max_xscale) + _left_space;
		if (end > (_screen_width - _right_space))
			end = _screen_width - _right_space;

		int ch_idx = get_ch_index(ch[i]);
		p->drawRect(start, _top_space + _ch[ch_idx].top_win_pos, (end - start), _ts_opt.ch_height);
	}

	p->restore();

	if (start_pos)
		delete[] start_pos;
	if (end_pos)
		delete[] end_pos;
	if (ch)
		delete[] ch;
	if (index)
		delete[] index;
} // mark_calib_seq()


void
cont_ap_view_qt::sort_general(const char *evtype, double value)
{
	cont_ap_view_general::sort_general(evtype, value);
	update();
} // sort_general()


void
cont_ap_view_qt::sort_bp(int type)
{
	cont_ap_view_general::sort_bp(type);
	update();
} // sort_bp()


void
cont_ap_view_qt::sort_class(int classification)
{
	cont_ap_view_general::sort_class(classification);
	update();
} // sort_class


void
cont_ap_view_qt::mousePressEvent(QMouseEvent * ev)
{
	if (ev->modifiers() & Qt::ControlModifier)
	{
		mouseDoubleClickEvent(ev);
		return;
	}

	if (ev->button() == Qt::LeftButton)
	{
		mouse_press(ev->x(), ev->y());
	}

	ts_view_qt::mousePressEvent(ev);
} // mousePressEvent()


