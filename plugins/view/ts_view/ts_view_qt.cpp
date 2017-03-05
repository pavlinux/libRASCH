/*----------------------------------------------------------------------------
 * ts_view_qt.cpp
 *
 *
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002-2007, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "ts_view_qt.h"
#include "ch_optdlg_qt.h"
#include "ts_view_optdlg_qt.h"
#include "annot_dlg_qt.h"

#include <Qt/qeventloop.h>
#include <Qt/qstring.h>
#include <QtGui/QCursor>
#include <QtGui/QWheelEvent>
#include <QtGui/QPixmap>
#include <QtGui/QPaintEvent>
#include <QtGui/QPolygon>
#include <QtGui/QKeyEvent>
#include <QtGui/QMenu>
#include <QtGui/QMouseEvent>
#include <QtCore/QTimer>
#include <QtGui/QMessageBox>


ts_view_qt::ts_view_qt(meas_handle h, int num_ch, int *ch, QWidget *p/*=0*/, const char *name/*NULL*/, Qt::WFlags f/*=0*/)
	: ts_view_general(h, num_ch, ch), QWidget(p, f)
{
	_name = name;

	// get resolution infos
	_pixmm_v = (double)height() / (double)heightMM();
	_pixmm_h = (double)width() / (double)widthMM();

	// resize widget if not all channels are shown by this plugin
	if (num_ch > 0)
	{
		value_handle vh = ra_value_malloc();
		if (ra_info_get(_rh, RA_INFO_REC_GEN_NUM_CHANNEL_L, vh) == 0)
		{
			long h = (long)((double)height() * ((double)num_ch / (double)ra_value_get_long(vh)));
			resize(width(), h);
		}
		ra_value_free(vh);
	}

	setAutoFillBackground(true);
	_palette.setColor(backgroundRole(), QColor(254, 251, 197));
	setPalette(_palette);

	_grid_pen.setColor(QColor(255, 170, 170));
	_grid_pen.setWidth(1);
	_grid_pen.setStyle(Qt::SolidLine);

	rubber_band = new QRubberBand(QRubberBand::Rectangle, this);

	for (int i = 0; i < _num_ch; i++)
		calc_scale(i);
	calc_max_scale_factors();

	// top level popup-menu entries
	_gen_opt_act = new QAction(QString(gettext("General Options...")), this);
	connect(_gen_opt_act, SIGNAL(triggered()), this, SLOT(general_options()));
	_ch_opt_act = new QAction(QString(gettext("Channel Options...")), this);
	connect(_ch_opt_act, SIGNAL(triggered()), this, SLOT(ch_options()));
	_sort_annot_act = new QAction(QString(gettext("Sort annotations")), this);
	connect(_sort_annot_act, SIGNAL(triggered()), this, SLOT(slot_sort_annot()));

	_popup_menu = new QMenu();
	_popup_menu->addAction(_gen_opt_act);
	_popup_menu->addAction(_ch_opt_act);
	_popup_menu->addAction(_sort_annot_act);

	// filter sub-menu entries
	_pw_filter_act = new QAction(QString(gettext("Remove Powerline Noise")), this);
	_pw_filter_act->setCheckable(true);
	connect(_pw_filter_act, SIGNAL(triggered()), this, SLOT(slot_powerline_filter()));

	_rm_mean_act = new QAction(QString(gettext("Remove Signal-Mean")), this);
	_rm_mean_act->setCheckable(true);
	connect(_rm_mean_act, SIGNAL(triggered()), this, SLOT(slot_remove_mean()));

	_popup_menu_filter = _popup_menu->addMenu(QString(gettext("Filter")));
	_popup_menu_filter->addAction(_pw_filter_act);
	_popup_menu_filter->addAction(_rm_mean_act);

	// marked area sub-menu entries
	_add_area_comment_act = new QAction(QString(gettext("Add comment...")), this);
	connect(_add_area_comment_act, SIGNAL(triggered()), this, SLOT(slot_insert_comment()));

	_del_events_in_area_act = new QAction(QString(gettext("Delete Events in Marked Area")), this);
	connect(_del_events_in_area_act, SIGNAL(triggered()), this, SLOT(slot_del_events_in_area()));

	_popup_menu_marked_area = _popup_menu->addMenu(QString(gettext("Marked Area")));
	_popup_menu_marked_area->addAction(_add_area_comment_act);

	// channel-settings save/load sub-menu entries
	_save_ch_settings_act = new QAction(QString(gettext("Save Channel Settings")), this);
	connect(_save_ch_settings_act, SIGNAL(triggered()), this, SLOT(slot_save_ch_settings()));
	_load_ch_settings_act = new QAction(QString(gettext("Load Channel Settings")), this);
	connect(_load_ch_settings_act, SIGNAL(triggered()), this, SLOT(slot_load_ch_settings()));
	
	_popup_menu_ch_save = _popup_menu->addMenu(QString(gettext("Save/Restore Channel Settings")));
	_popup_menu_ch_save->addAction(_save_ch_settings_act);
	_popup_menu_ch_save->addAction(_load_ch_settings_act);

	// sessions sub-menu entries
	if (_num_sessions > 1)
	{
		_popup_menu_sessions = _popup_menu->addMenu(QString(gettext("Sessions")));
		for (int i = 0; i < _num_sessions; i++)
		{
			char t[100];
			sprintf(t, gettext("session #%d"), i+1);
			QAction *a = _popup_menu_sessions->addAction(QString(t), this, "show_session");
			a->setData(i);
		}
	}
	
	// online specific things
	if (_is_online)
	{
		_online_timer = new QTimer(this);
		connect(_online_timer, SIGNAL(timeout()), this, SLOT(slot_online_update()));
		_online_timer->start(500);
	}
} // constructor


ts_view_qt::~ts_view_qt()
{
	delete rubber_band;
	delete _gen_opt_act;
	delete _ch_opt_act;
	delete _sort_annot_act;
	delete _pw_filter_act;
	delete _rm_mean_act;
	delete _popup_menu;

	if (_is_online)
	{
		_online_timer->stop();
		delete _online_timer;
	}
} // destructor


void
ts_view_qt::update()
{
	QWidget::update();
}  // update()


void
ts_view_qt::keyPressEvent(QKeyEvent * ev)
{
	bool changed = false;
	switch (ev->key())
	{
	case Qt::Key_Right:
		changed = next_page(ev->modifiers() & Qt::ShiftModifier);
		break;
	case Qt::Key_Left:
		changed = prev_page(ev->modifiers() & Qt::ShiftModifier);
		break;
	case Qt::Key_PageUp:
		changed = prev_page(true);
		break;
	case Qt::Key_PageDown:
		changed = next_page(true);
		break;
	case Qt::Key_Home:
		changed = goto_begin();
		break;
	case Qt::Key_End:
		changed = goto_end();
		break;
	case Qt::Key_Up:
		if (_curr_session > 0)
			change_session(_curr_session - 1);
		break;
	case Qt::Key_Down:
		if (_curr_session < (_num_sessions - 1))
			change_session(_curr_session + 1);
		break;
	default:		// handle alphanumeric keys
		if (!ev->text().isEmpty())
		{
			char c = ev->text().at(0).toAscii();
			changed = key_press_char((char)(toupper(c)), ev->modifiers() & Qt::ShiftModifier,
						 ev->modifiers() & Qt::ControlModifier);
		}
		break;
	}

	if (changed)
		update();
}  // keyPressEvent()


void
ts_view_qt::paintEvent(QPaintEvent * /*ev*/)
{
	if (_dont_paint)
		return;

	QPainter p(this);

	_screen_width = width();
	_screen_height = height();

	paint(width(), height());

	plot_grid(&p);

	plot_statusbar(&p);

	int line = 0;
	for (int i = 0; i < _num_ch; i++)
	{
		if (_ch[i].show_it)
		{
			plot_channel(&p, i, line);
			line++;
		}
	}

	if (_ts_opt.show_annot)
		print_comment_annotations(&p);

	if (_ts_handle_cursor)
		plot_cursor(&p);

	if (_area_marked || (_area_mark_step == MARKING_AREA))
		highlight_area();

 	if (_has_input_focus)
 		plot_focus(&p);
}  // paintEvent()


void
ts_view_qt::plot_grid(QPainter *p)
{
	if (!_ts_opt.plot_grid)
		return;

	// width and height in cm
	int h = height() - _top_space - _bottom_space;
	int w = width() - _left_space - _right_space;
	int n_lines = (int) ((double) h / (_pixmm_v * 10.0));
	int n_col = (int) ((double) w / (_pixmm_h * 10.0));

	p->save();

	p->setPen(_grid_pen);
	// plot horizontal lines
	for (int i = 0; i <= n_lines; i++)
	{
		int y = (int) ((_pixmm_v * 10.0) * (double) i) + _top_space;
		p->drawLine(_left_space, y, width() - _right_space, y);
	}

	// plot vertical lines
	for (int i = 0; i <= n_col; i++)
	{
		int x = (int) ((_pixmm_h * 10.0) * (double) i) + _left_space;
		p->drawLine(x, _top_space, x, height() - _bottom_space);
	}

	p->restore();
}  // plot_grid()


void
ts_view_qt::plot_statusbar(QPainter *p)
{
	if (!_ts_opt.show_status)
		return;

	char *status_text = NULL; // memory will be allocated in format_statusbar()
	format_statusbar(&status_text);

	p->save();

	if (status_text)
	{
		QBrush b(_palette.color(backgroundRole()));
		p->setBrush(b);
		QPen pen(_palette.color(backgroundRole()));
		p->setPen(pen);
		// clear background (-3 is needed so the input-focus is not "cleared")
		p->drawRect(_left_space, height() - _bottom_space, width()-_left_space-_right_space-3, _bottom_space - 3);
		
		pen.setColor(QColor(0,0,0));
		p->setPen(pen);
		p->drawText(1 + _left_space, height() - 5, status_text);
		delete[] status_text;
	}

	if (_num_sort)
	{
		char t[100];
		sprintf(t, "%ld / %ld", (_curr_sort + 1), _num_sort);
		p->drawText(_left_space, this->height() - 19, width() - _left_space - _right_space - 4,
			    15, Qt::AlignRight, t);
	}

	p->restore();
}  // plot_statusbar()


void
ts_view_qt::plot_channel(QPainter *p, int ch, int line)
{
	p->save();

	QPen pen;
	pen.setWidth(1);
	pen.setStyle(Qt::SolidLine);
	if (_ch[ch].highlight_channel)
		pen.setColor(QColor(255, 0, 0));
	else
		pen.setColor(QColor(0, 0, 0));
	p->setPen(pen);

	int *x, *y, *ignore, num;
	ch_draw_data(ch, line, &x, &y, &ignore, &num);

	if (num > 0)
	{
		QPoint *pts = new QPoint[num];
		for (int i = 0; i < num; i++)
		{
			pts[i].setX(x[i]);
			pts[i].setY(y[i]);
		}

		// draw line seperating channels
		int line_y_pos = line*_ts_opt.ch_height + _top_space;
		pen.setStyle(Qt::DotLine);
		p->setPen(pen);
		p->drawLine(0, line_y_pos, width(), line_y_pos);
		pen.setStyle(Qt::SolidLine);
		p->setPen(pen);

		int start = 0;
		int num_draw = 0;
		int curr = 0;
		while (curr < num)
		{
			if (ignore[curr] == 1)
			{
				if (num_draw > 0)
					p->drawPolyline(pts+start, num_draw);
				num_draw = 0;
				start = curr + 1;
			}
			else
				num_draw++;
			curr++;
		}
		if (num_draw > 0)
			p->drawPolyline(pts+start, num_draw);

		delete[] x;
		delete[] y;
		delete[] pts;
		delete[] ignore;

		if (_ts_opt.show_ch_info)
		{
			QFont f;
			if (ch == _curr_channel)
			{
				f.setBold(true);
				p->setFont(f);
			}

			int w, h;
			get_text_extent(p, "X", &w, &h);

			if (_ch[ch].inverse)
			{
				QString s;
				s = "-";
				s += _ch[ch].name;
				p->drawText(_ch_info_offset, (line_y_pos + _ts_opt.ch_height / 2) + 0, s);
			}
			else
			{
				p->drawText(_ch_info_offset, (line_y_pos + _ts_opt.ch_height / 2) + 0,
					    _ch[ch].name);
			}

			QString s = "[";
			s += _ch[ch].unit;
			s += "]";
			p->drawText(_ch_info_offset, (line_y_pos + _ts_opt.ch_height / 2) + h, s);
		}

		if (_ts_opt.show_ch_scale)
		{
			int y_e = line_y_pos;
			int y_s = y_e + _ts_opt.ch_height;

			int w, h;
			char t[20];
			sprintf(t, "%.1f", _ch[ch].scale_end);
			get_text_extent(p, t, &w, &h);
			p->drawText(_left_space - w, y_e + h, t);

			sprintf(t, "%.1f", _ch[ch].scale_start);
			get_text_extent(p, t, &w, &h);
			p->drawText(_left_space - w, y_s-2, t);
		}
	}

	p->restore();
} // plot_channel()


void
ts_view_qt::plot_focus(QPainter *p)
{
	p->save();

	QPen pen;
	pen.setWidth(2);
//	pen.setColor();

	p->setPen(pen);

	p->drawRect(1, 1, width()-2, height()-2);

	p->restore();
} // plot_focus()


void
ts_view_qt::plot_cursor(void *p)
{
	if (!p)
	{
		repaint();
		return;
	}

	if ((_area_mark_step != NOT_MARKING_AREA) || (_area_marked))
		return;

	if ((_cursor_pos < _left_space) || (_cursor_pos > (width() - _right_space)))
	{
		rubber_band->hide();
		return;
	}

	rubber_band->setGeometry(_cursor_pos, _top_space+4, 2, height() - _top_space - _bottom_space);
	rubber_band->show();
	plot_statusbar((QPainter *)p);
} // plot_cursor()


void
ts_view_qt::print_comment_annotations(QPainter *p)
{
	struct curr_annot_info *info = NULL;
	int n_comments = 0;

	if (_aoi)
	{
		delete[] _aoi;
		_aoi = NULL;
		_num_aoi = 0;
	}

	get_comments(p, &info, &n_comments);	
	place_comments(info, n_comments);
	if (n_comments > 0)
	{
		_num_aoi = n_comments;
		_aoi = new struct aoi[_num_aoi];

		for (int i = 0; i < n_comments; i++)
		{
			int x = info[i].text_x;
			int y = info[i].y;
			
			if (y != -1)
			{
				QString s = QString::fromUtf8(info[i].text);
				p->drawText(x, y, s);

				_aoi[i].x = x;
				_aoi[i].y = y;
				_aoi[i].w = info[i].text_width;
				_aoi[i].h = info[i].text_height;
				_aoi[i].annot = info[i].annot_idx;
			}
			else
				_aoi[i].x = -1;

			int begin = info[i].begin;
			int end = info[i].end;

			// if begin is outside of shown area, plot arrow (instead of vertical line)
			if (info[i].begin_not_shown)
			{
				p->drawLine(_left_space, y+2, _left_space+3, y-1);
				p->drawLine(_left_space, y+2, _left_space+3, y+5);
				begin = _left_space;
			}
			else
				p->drawLine(info[i].begin, y+4, info[i].begin, y);
			if (info[i].end_not_shown)
			{
				long draw_end = _screen_width - _right_space;
				p->drawLine(draw_end, y+2, draw_end-3, y-1);
				p->drawLine(draw_end, y+2, draw_end-3, y+5);
				end = draw_end;
			}
			else
				p->drawLine(info[i].end, y+4, info[i].end, y);

			if (end > begin)
				p->drawLine(begin, y+2, end, y+2);
			else
				p->drawLine(begin-4, y+2, end+4, y+2);
		}
	}

	if (info)
	{
		for (int i = 0; i < n_comments; i++)
			delete[] info[i].text;

		free(info);
	}
} // print_comment_annotations()


void
ts_view_qt::get_text_extent(void *p, const char *text, int *w, int *h)
{
	QPainter *p_use = (QPainter *)p;
	if (!p)
		p_use = new QPainter(this);

	QFontMetrics f = p_use->fontMetrics();
	QRect rect = f.boundingRect(text);
	*w = rect.width();
	*h = rect.height();

	if (!p)
		delete p_use;
} // get_text_extent()


// TODO: get rid of all these +-_left_space so it is easier to understand (at least for me)
void
ts_view_qt::highlight_area()
{
	if (!_area_marked && (_area_start < 0))
	{
		rubber_band->hide();
		return;
	}

	int h = height() - _top_space - _bottom_space;

	int start = su_to_screen(_area_start);
	int end = su_to_screen(_area_end);

	int w = this->width() - _left_space - _right_space;
	if (((start < _left_space) && (end < _left_space)) ||
	    ((start > (w + _left_space)) && (end > (w + _left_space))))
	{
		rubber_band->hide();
		return;
	}
	
	rubber_band->show();
	if (start <= end)
		rubber_band->setGeometry(start, _top_space+4, end - start, h);
	else
		rubber_band->setGeometry(end, _top_space+4, start - end, h);
} // highlight_area()


void
ts_view_qt::mousePressEvent(QMouseEvent *ev)
{
	if (ev->modifiers() & Qt::ControlModifier)
	{
		mouseDoubleClickEvent(ev);
		return;
	}

	static int in_event = 0;

	if (in_event)
		return;

	in_event = 1;

	// if this is subview of ra_view tell ra_view that this view has the input-focus
	if (_name.startsWith(QString("ra-sub-view")))
	{
		send_input_focus_change();
		QWidget *parent = parentWidget();
		qApp->notify(parent, ev);
	}

	y_to_channel(ev->y());

	if (ev->button() == Qt::RightButton)
	{
		_pw_filter_act->setChecked(_filter_powerline_noise);
		_rm_mean_act->setChecked(_remove_mean);

		_popup_menu_marked_area->setEnabled(_area_marked);

		_popup_menu->exec(mapToGlobal(ev->pos()));
	}

	if (ev->button() == Qt::LeftButton)
	{
		if (ev->x() >= _left_space)
		{
			_cursor_pos = ev->x();

			if (_area_mark_step != MARKING_AREA_MANUAL)
				start_area_mark(ev->x());

 			rubber_band->setGeometry(ev->x(), _top_space+4, 2, height() - _top_space - _bottom_space);
 			rubber_band->show();
			plot_cursor(NULL);
		}
		else
		{
			_ch[_curr_channel].highlight_channel = true;
			update();
		}
	}

	in_event = 0;
}  // mousePressEvent()


void
ts_view_qt::mouseReleaseEvent(QMouseEvent *ev)
{
	if (ev->button() == Qt::LeftButton)
	{
		// do not longer highlight current channel
		if (_ch[_curr_channel].highlight_channel)
		{
			_ch[_curr_channel].highlight_channel = false;
			update();
		}

		if (_area_mark_step == MARKING_AREA_MANUAL)
			change_area_mark(ev->x());
		else
			end_area_mark(ev->x());
	}
}  // mouseReleaseEvent()


void
ts_view_qt::mouseMoveEvent(QMouseEvent *ev)
{
	static bool x_outside = false;
	static bool in_change = false;

	if ((_area_mark_step != MARKING_AREA_START) && (_area_mark_step != MARKING_AREA))
	{
		x_outside = in_change = false;
		return;
	}

	if ((ev->x() > (_screen_width - _right_space)) || (ev->x() < _left_space))
		x_outside = true;
	else
		x_outside = false;

	if (in_change)
		return;

	in_change = true;

	long as, ae;
	do
	{
		as = _area_start;
		ae = _area_end;

		change_area_mark(ev->x());
		qApp->processEvents();

		if ((as == _area_start) && (ae == _area_end))
			break;
	} while (x_outside && (_area_mark_step == MARKING_AREA));

	in_change = false;
} // mouseMoveEvent()


void
ts_view_qt::mouseDoubleClickEvent(QMouseEvent *ev)
{
	if (ev->x() < _left_space)
	{
		auto_adjust_y_axis(ev->y());
		return;
	}

	int idx = find_aoi(ev->x(), ev->y());
	if (idx < 0)
		return;

	if (_aoi[idx].annot != -1)
	{
		_dbl_left_click_handled = true;

		struct annot_info *curr = &(_annot[_aoi[idx].annot]);
		QString s = QString::fromUtf8(curr->text);
		annot_dlg_qt dlg(_ch, _num_ch, _curr_channel, _annot_predefs, _num_annot_predefs, 
				 s, curr->noise_flag, curr->ignore_flag, curr->ch, this);
		int ret = dlg.exec();
		if (ret == QDialog::Rejected)
		{
			return;
		}
		else if (dlg.delete_annot())
		{
			delete_comment(_aoi[idx].annot);
			update();
			return;
		}
		else		// user pressed 'OK'
		{
			s = dlg.annot();
			QByteArray ba = s.toUtf8();
			if (curr->text)
			{
				delete[] curr->text;
				curr->text = NULL;
			}
			curr->text = new char[ba.size() + 1];
			strcpy(curr->text, ba.data());
			
			curr->noise_flag = dlg.is_noise();
			curr->ignore_flag = dlg.do_ignore();
			curr->ch = dlg.channel();
			
			update_comment(_aoi[idx].annot);

			update();
		}
	}
} // mouseDoubleClickEvent()


void
ts_view_qt::wheelEvent(QWheelEvent *e)
{
	int d = e->delta();

	if (d > 0)
		prev_page(e->modifiers() & Qt::ShiftModifier);
	if (d < 0)
		next_page(e->modifiers() & Qt::ShiftModifier);

	update();
}  // wheelEvent()


void
ts_view_qt::general_options()
{
	double mm_sec_save = _ts_opt.mm_per_sec;

	ts_view_optdlg_qt dlg(&_ts_opt, this);

	int ret = dlg.exec();
	if (ret == QDialog::Accepted)
	{
		options_changed();
		if (mm_sec_save != _ts_opt.mm_per_sec)
			send_x_res((long)(parentWidget()->parentWidget())); // need parent of parent because of view_frame
	}

	update();
} // general_options()


void
ts_view_qt::ch_options()
{
	ch_optdlg_qt dlg(&_ch, _num_ch, _curr_channel, this);

	int ret = dlg.exec();
	if (ret == QDialog::Accepted)
		options_changed();

	update();
} // ch_options()


void
ts_view_qt::set_x_resolution(double x_res, void *parent)
{
        // need parent of parent because of view_frame
	QWidget *p = parentWidget();
	if (((QWidget *)parent != p) && ((QWidget *)parent != p->parentWidget()))
		return;

	_ts_opt.mm_per_sec = x_res;
	options_changed();
	
	update();
}  // set_x_resolution()


int
ts_view_qt::annot_dlg(char **annot, bool *noise, bool *ignore, int *ch)
{
	annot_dlg_qt dlg(_ch, _num_ch, _curr_channel, _annot_predefs,
			 _num_annot_predefs, NULL, false, false, -1,
			 this);
	int ret = dlg.exec();
	if (ret == QDialog::Rejected)
		return -1;

	QString s = dlg.annot();
	QByteArray ba = s.toUtf8();
	*annot = new char[ba.size() + 1];
	strcpy(*annot, ba.data());
	(*annot)[ba.size()] = '\0';

	*noise = dlg.is_noise();
	*ignore = dlg.do_ignore();
	*ch = dlg.channel();

	return 0;
} // annot_dlg


void
ts_view_qt::show_session(int session)
{
	change_session(session);
} // show_session()


void
ts_view_qt::slot_sort_annot()
{
	sort_annotations();
} // slot_sort_annot()


void
ts_view_qt::unset_cursor()
{
	unsetCursor();
} // unset_cursor()


void
ts_view_qt::set_busy_cursor()
{
	setCursor(Qt::WaitCursor);
} // set_busy_cursor()

void
ts_view_qt::slot_powerline_filter()
{
	_filter_powerline_noise = (_filter_powerline_noise ? false : true);
	update();
} // slot_powerline_filter()


void
ts_view_qt::slot_remove_mean()
{
	_remove_mean = (_remove_mean ? false : true);
	update();
} // slot_remove_mean()


void
ts_view_qt::slot_online_update()
{
	update();
} // slot_online_update()


void
ts_view_qt::slot_insert_comment()
{
	insert_comment();
} // slot_insert_comment()


void
ts_view_qt::slot_del_events_in_area()
{
    int ret = QMessageBox::warning(this, tr("Raschlab"),
                   tr("Do you really want to delete the Events\n"
                      "in the marked area?"),
                   QMessageBox::Yes | QMessageBox::No,
                   QMessageBox::No);
    if (ret == QMessageBox::No)
        return;

    remove_events_in_area();
} // slot_del_events_in_area()


void
ts_view_qt::slot_save_ch_settings()
{
	struct comm_para *para = new struct comm_para[1];
	para[0].type = PARA_CHAR_POINTER;
	para[0].v.pc = new char[MAX_PATH_RA];
	get_ch_settings_file(para[0].v.pc, MAX_PATH_RA);

	ra_comm_emit(_mh, "ts-view", "save-settings", 1, para);

	delete[] para[0].v.pc;
	delete[] para;
} // slot_save_ch_settings()


void
ts_view_qt::slot_load_ch_settings()
{
	struct comm_para *para = new struct comm_para[1];
	para[0].type = PARA_CHAR_POINTER;
	para[0].v.pc = new char[MAX_PATH_RA];
	get_ch_settings_file(para[0].v.pc, MAX_PATH_RA);

	ra_comm_emit(_mh, "ts-view", "load-settings", 1, para);

	delete[] para[0].v.pc;
	delete[] para;
} // slot_load_ch_settings()
