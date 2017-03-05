/*----------------------------------------------------------------------------
 * ts_view_gtk.cpp
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

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <gtkmm/main.h>
#include <gdk/gdkkeysyms.h>

#include "annot_dlg_gtk.h"
#include "ts_view_gtk.h"
#include "gen_opt_dlg_gtk.h"
#include "ch_opt_dlg_gtk.h"



ts_view_gtk::ts_view_gtk(meas_handle h, int num_ch/*0*/, int *ch/*NULL*/, const char *win_name/*NULL*/)
 	: ts_view_general(h, num_ch, ch)
{
	if (win_name)
		name = win_name;

	_highlight_start = -1;
	_highlight_width = -1;

	Glib::RefPtr<Gdk::Colormap> colormap = get_default_colormap();

	bg_color = Gdk::Color("white");
 	bg_color.set_rgb(65278, 64507, 50629);
	draw_color = Gdk::Color("black");
	grid_color = Gdk::Color();
	grid_color.set_rgb(65535, 43690, 43690);
	cursor_color = Gdk::Color("black");
	
	colormap->alloc_color(bg_color);
	colormap->alloc_color(draw_color);
	colormap->alloc_color(grid_color);
	colormap->alloc_color(cursor_color);

	popup_group = Gtk::ActionGroup::create();

	popup_group->add( Gtk::Action::create("GeneralOptions", "General Options..."),
			  sigc::mem_fun(*this, &ts_view_gtk::on_action_general_options) );
	popup_group->add( Gtk::Action::create("ChannelOptions", "Channel Options..."),
			  sigc::mem_fun(*this, &ts_view_gtk::on_action_channel_options) );

	act_start_mark_area = Gtk::Action::create("StartAreaMark", "Start Marking Area - ','");
	popup_group->add(act_start_mark_area, sigc::mem_fun(*this, &ts_view_gtk::on_action_start_mark_area));
	act_end_mark_area = Gtk::Action::create("EndAreaMark", "End Marking Area - ','");
	popup_group->add(act_end_mark_area, sigc::mem_fun(*this, &ts_view_gtk::on_action_end_mark_area));
	popup_group->add( Gtk::Action::create("InsertComment", "Insert Comment..."),
			  sigc::mem_fun(*this, &ts_view_gtk::on_action_insert_comment) );

	popup_filter_group = Gtk::ActionGroup::create();
	popup_filter_group->add( Gtk::Action::create("MenuFilter", "_Filter") );
	act_powerline_filter = Gtk::ToggleAction::create("RemovePowerline", "Remove Powerline Noise");
	popup_filter_group->add(act_powerline_filter, sigc::mem_fun(*this, &ts_view_gtk::on_action_powerline_filter));
	act_remove_mean = Gtk::ToggleAction::create("RemoveMean", "Remove Signal Mean");
	popup_filter_group->add(act_remove_mean, sigc::mem_fun(*this, &ts_view_gtk::on_action_remove_mean) );

	ui_manager = Gtk::UIManager::create();
	ui_manager->insert_action_group(popup_group);
	ui_manager->insert_action_group(popup_filter_group);

	//Layout the actions in a menubar and toolbar:
	try
	{
		Glib::ustring ui_info = 
			"<ui>"
			"  <popup name='PopupMenu'>"
			"    <menuitem action='GeneralOptions'/>"
			"    <menuitem action='ChannelOptions'/>"
			"    <menu action='MenuFilter'>"
			"      <menuitem action='RemovePowerline'/>"
			"      <menuitem action='RemoveMean'/>"
			"    </menu>"
			"    <menuitem action='StartAreaMark'/>"
			"    <menuitem action='EndAreaMark'/>"
			"    <menuitem action='InsertComment'/>"
			"  </popup>"
			"</ui>";
		
		ui_manager->add_ui_from_string(ui_info);
	}
	catch(const Glib::Error& ex)
	{
// 		std::cerr << "building menus failed: " <<  ex.what();
	}

	//Get the menu:
	popup_menu = dynamic_cast<Gtk::Menu*>( ui_manager->get_widget("/PopupMenu") ); 
	if(!popup_menu)
		g_warning("menu not found");


	glade_xml = Gnome::Glade::Xml::create("ts_view_ui.glade");

//	add_accel_group(ui_manager->get_accel_group());

//	add_events(Gdk::ALL_EVENTS_MASK);
	add_events(Gdk::BUTTON1_MOTION_MASK
		   | Gdk::BUTTON2_MOTION_MASK
		   | Gdk::BUTTON3_MOTION_MASK
		   | Gdk::BUTTON_PRESS_MASK
		   | Gdk::BUTTON_RELEASE_MASK
		   | Gdk::SCROLL_MASK);

} // constructor


ts_view_gtk::~ts_view_gtk()
{
// 	if (_is_online)
// 	{
// 		_online_timer->stop();
// 		delete _online_timer;
// 	}
} // destructor


void
ts_view_gtk::on_realize()
{
	// We need to call the base on_realize()
	Gtk::DrawingArea::on_realize();

	// Now we can allocate any additional resources we need
	Glib::RefPtr<Gdk::Window> window = get_window();
	gc = Gdk::GC::create(window);
	window->set_background(bg_color);
	window->clear();
	gc->set_foreground(draw_color);

 	Gtk::WidgetFlags f = get_flags();
 	f |= Gtk::CAN_FOCUS;
	set_flags(f);

 	// get resolution infos
	Glib::RefPtr<Gdk::Screen> screen = get_screen();
 	_pixmm_v = (double)screen->get_height() / (double)screen->get_height_mm();
  	_pixmm_h = (double)screen->get_width() / (double)screen->get_width_mm();

// 	// resize widget if not all channels are shown by this plugin
// 	if (num_ch > 0)
// 	{
// 		value_handle vh = ra_value_malloc();
// 		if (ra_info_get(_rh, RA_INFO_REC_GEN_NUM_CHANNEL_L, vh) == 0)
// 		{
// 			long h = (long)((double)height() * ((double)num_ch / (double)ra_value_get_long(vh)));
// 			resize(width(), h);
// 		}
// 		ra_value_free(vh);
// 	}

 	for (int i = 0; i < _num_ch; i++)
 		calc_scale(i);
 	calc_max_scale_factors();


// 	if (_num_sessions > 1)
// 	{
// 		_popup_menu_sessions = _popup_menu->addMenu(QString(gettext("Sessions")));
// 		for (int i = 0; i < _num_sessions; i++)
// 		{
// 			char t[100];
// 			sprintf(t, gettext("session #%d"), i+1);
// 			QAction *a = _popup_menu_sessions->addAction(QString(t), this, "show_session");
// 			a->setData(i);
// 		}
// 	}
	
// 	if (_is_online)
// 	{
// 		_online_timer = new QTimer(this);
// 		connect(_online_timer, SIGNAL(timeout()), this, SLOT(slot_online_update()));
// 		_online_timer->start(500);
// 	}
} /* on_realize() */


bool
ts_view_gtk::on_expose_event(GdkEventExpose *e)
{
 	if (_dont_paint)
 		return true;

	_cursor_pos_prev = -1;
 	_screen_width = get_width();
 	_screen_height = get_height();

	Glib::RefPtr<Gdk::Window> window = get_window();
	pixmap = Gdk::Pixmap::create(window, _screen_width, _screen_height);
	gc->set_foreground(bg_color);
	pixmap->draw_rectangle(gc, true, 0, 0, _screen_width, _screen_height);

 	paint(_screen_width, _screen_height);

 	plot_grid();

 	plot_statusbar();

	int line = 0;
	for (int i = 0; i < _num_ch; i++)
	{
		if (_ch[i].show_it)
		{
			plot_channel(i, line);
			line++;
		}
	}

	print_comment_annotations();

	do_additional_drawing();

	window->draw_drawable(gc, pixmap, e->area.x, e->area.y, e->area.x, 
			      e->area.y, e->area.width, e->area.height);

	// either mark an area or plot cursor
	if (_area_marked || (_area_mark_step == MARKING_AREA))
		highlight_area();
	else if (_ts_handle_cursor)
		plot_cursor();

//  	if (_has_input_focus)
//  		plot_focus(&p);

	return true;
}  /* on_expose_event() */


void
ts_view_gtk::plot_grid()
{
	if (!_ts_opt.plot_grid)
		return;

	// width and height in cm
	int h = get_height() - _top_space - _bottom_space;
	int w = get_width() - _left_space - _right_space;
	int n_lines = (int)((double)h / (_pixmm_v * 10.0));
	int n_col = (int)((double)w / (_pixmm_h * 10.0));

	gc->set_line_attributes(1, Gdk::LINE_SOLID, Gdk::CAP_ROUND, Gdk::JOIN_ROUND);
	gc->set_foreground(grid_color);
	
	// plot horizontal lines
	for (int i = 0; i <= n_lines; i++)
	{
		int y = (int)((_pixmm_v * 10.0) * (double) i) + _top_space;
		pixmap->draw_line(gc, _left_space, y, _screen_width - _right_space, y);
	}

	// plot vertical lines
	for (int i = 0; i <= n_col; i++)
	{
		int x = (int)((_pixmm_h * 10.0) * (double) i) + _left_space;
		pixmap->draw_line(gc, x, _top_space, x, _screen_height - _bottom_space);
	}
}  // plot_grid()


void
ts_view_gtk::plot_statusbar()
{
	if (!_ts_opt.show_status)
		return;

	char *status_text = NULL; // memory will be allocated in format_statusbar()
	format_statusbar(&status_text);

	gc->set_foreground(draw_color);

	if (status_text)
	{
		Glib::RefPtr<Pango::Layout> layout = create_pango_layout(status_text);
		pixmap->draw_layout(gc, 1 + _left_space, _screen_height - _bottom_space, layout);
		
		delete[] status_text;
	}

	if (_num_sort)
	{
		char t[100];
		sprintf(t, "%ld / %ld", (_curr_sort + 1), _num_sort);
		Glib::RefPtr<Pango::Layout> layout = create_pango_layout(t);
		int w, h;
		layout->get_pixel_size(w, h);
		pixmap->draw_layout(gc, _screen_width - _right_space - w - 1, _screen_height - _bottom_space, layout);
	}
}  // plot_statusbar()


void
ts_view_gtk::plot_channel(int ch, int line)
{
	gc->set_line_attributes(1, Gdk::LINE_SOLID, Gdk::CAP_ROUND, Gdk::JOIN_ROUND);
	gc->set_foreground(draw_color);

	int *x, *y, *ignore, num;
	ch_draw_data(ch, line, &x, &y, &ignore, &num);

	if (num > 0)
	{
		std::vector<Gdk::Point> pts;
		pts.resize(0);

		int curr = 0;
		while (curr < num)
		{
			if (ignore[curr] == 1)
			{
				if (pts.size() > 0)
					pixmap->draw_lines(gc, pts);
				pts.resize(0);
			}
			else
				pts.push_back(Gdk::Point(x[curr], y[curr]));

			curr++;
		}
		if (pts.size() > 0)
			pixmap->draw_lines(gc, pts);

		delete[] x;
		delete[] y;
		delete[] ignore;

		if (_ts_opt.show_ch_info)
		{
			// TODO: make font member; calc textposition
//			QFont f;
//			f.defaultFont();
//			f.setPointSize(10);
//			p.setFont(f);
// 			QFont f;
// 			if (ch == _curr_channel)
// 			{
// //				f.defaultFont();
// 				f.setBold(true);
// 				p->setFont(f);
// 			}

			Glib::ustring s = _ch[ch].name;
			if (_ch[ch].inverse)
				s.insert(1, 1, '-');

			Glib::RefPtr<Pango::Layout> layout = create_pango_layout(s);
			int w, h;
			layout->get_pixel_size(w, h);
			pixmap->draw_layout(gc, _ch_info_offset, (line * _ts_opt.ch_height + _ts_opt.ch_height / 2) - h, layout);

/*			char t[20];
			sprintf(t, "%dmm/", (int) _ch[ch].mm_per_unit);
			p.drawText(_ch_info_offset, (line * _ts_opt.ch_height + _ts_opt.ch_height / 2) + 30, t);
			QString s = "[";
			s += _ch[ch].unit;
			s += "]";
			QString s_utf8 = QString::fromUtf8(_ch[ch].unit);
			p.drawText(_ch_info_offset, (line * _ts_opt.ch_height + _ts_opt.ch_height / 2) + 45, s_utf8);
*/
			s = "[";
			s += _ch[ch].unit;
			s += "]";
			layout->set_text(s);
			pixmap->draw_layout(gc, _ch_info_offset, (line * _ts_opt.ch_height + _ts_opt.ch_height / 2), layout);
		}
	}
} /* plot_channel() */


void
ts_view_gtk::update()
{
	Glib::RefPtr<Gdk::Window> window = get_window();
	if (!window)
		return;
	Gdk::Rectangle r(0, 0, _screen_width, _screen_height);
	window->invalidate_rect(r, true);
} /* update() */


void
ts_view_gtk::print_comment_annotations()
{
	struct curr_annot_info *info = NULL;
	int n_comments = 0;

	if (_aoi)
	{
		delete[] _aoi;
		_aoi = NULL;
		_num_aoi = 0;
	}

	get_comments(NULL, &info, &n_comments);	
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
				Glib::RefPtr<Pango::Layout> layout = create_pango_layout(info[i].text);
				layout->get_pixel_size(info[i].text_width, info[i].text_height);
				pixmap->draw_layout(gc, x, y-info[i].text_height, layout);

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
				pixmap->draw_line(gc, _left_space, y+2, _left_space+3, y-1);
				pixmap->draw_line(gc, _left_space, y+2, _left_space+3, y+5);
				begin = _left_space;
			}
			else
				pixmap->draw_line(gc, info[i].begin, y+4, info[i].begin, y);
			if (info[i].end_not_shown)
			{
				long draw_end = _screen_width - _right_space;
				pixmap->draw_line(gc, draw_end, y+2, draw_end-3, y-1);
				pixmap->draw_line(gc, draw_end, y+2, draw_end-3, y+5);
				end = draw_end;
			}
			else
				pixmap->draw_line(gc, info[i].end, y+4, info[i].end, y);

			if (end > begin)
				pixmap->draw_line(gc, begin, y+2, end, y+2);
			else
				pixmap->draw_line(gc, begin-4, y+2, end+4, y+2);
		}
	}

	if (info)
	{
		for (int i = 0; i < n_comments; i++)
			delete[] info[i].text;

		free(info);
	}
} /* print_comment_annotations() */


void
ts_view_gtk::get_text_extent(char *text, int *w, int *h)
{
	Glib::RefPtr<Pango::Layout> layout = create_pango_layout(text);
	layout->get_pixel_size(*w, *h);
} /* get_text_extent() */



void
ts_view_gtk::plot_cursor()
{
	Glib::RefPtr<Gdk::Window> w = get_window();
	w->draw_drawable(gc, pixmap, 0, 0, 0, 0, _screen_width, _screen_height);

	if (((_area_mark_step != NOT_MARKING_AREA) && (_area_mark_step != MARKING_AREA_START))
	    || (_area_marked))
	{
		return;
	}

	if ((_cursor_pos < _left_space) || (_cursor_pos > (_screen_width - _right_space)))
		return;

	gc->set_line_attributes(2, Gdk::LINE_SOLID, Gdk::CAP_ROUND, Gdk::JOIN_ROUND);
	gc->set_foreground(cursor_color);
 	gc->set_function(Gdk::INVERT);

	w->draw_line(gc, _cursor_pos, _top_space, _cursor_pos, _screen_height - _bottom_space);
	_cursor_pos_prev = _cursor_pos;

	gc->set_function(Gdk::COPY);
} /* plot_cursor() */


// void
// ts_view_qt::plot_focus(QPainter *p)
// {
// 	p->save();

// 	QPen pen;
// 	pen.setWidth(2);
// //	pen.setColor();

// 	p->setPen(pen);

// 	p->drawRect(1, 1, width()-2, height()-2);

// 	p->restore();
// } // plot_focus()


// TODO: get rid of all these +-_left_space so it is easier to understand (at least for me)
void
ts_view_gtk::highlight_area()
{
	int h = _screen_height - _top_space - _bottom_space;

	Glib::RefPtr<Gdk::Window> win = get_window();
	if (_highlight_start >= 0)
	{
		win->draw_drawable(gc, pixmap, _highlight_start, 0, _highlight_start, 0, _highlight_width, _screen_height);
		_highlight_start = -1;
	}

	if (_area_start < 0)
		return;

	int w = _screen_width - _left_space - _right_space;

	int start = su_to_screen(_area_start);
	int end = su_to_screen(_area_end);
	if (end < start)
	{
		int t = start;
		start = end;
		end = t;
	}

	if (((start < _left_space) && (end < _left_space)) ||
	    ((start > (w + _left_space)) && (end > (w + _left_space))))
	{
		return;
	}

	if (start < _left_space)
		start = _left_space;
	if (end > (w + _left_space))
		end = (w + _left_space);

	
	_highlight_start = start;
	if (_highlight_start < _left_space)
		_highlight_start = _left_space;

	int width = end - start;
	_highlight_width = width;
	if (_highlight_width > (w - (_highlight_start - _left_space)))
		_highlight_width = w - (_highlight_start - _left_space);
	
	gc->set_foreground(cursor_color);
 	gc->set_function(Gdk::INVERT);
	win->draw_rectangle(gc, true, _highlight_start, _top_space, _highlight_width, h);
 	gc->set_function(Gdk::COPY);
} /* highlight_area() */


bool
ts_view_gtk::on_key_press_event(GdkEventKey *e)
{
	bool changed = false;
	switch (e->keyval)
	{
	case GDK_Right:
		changed = next_page(e->state & GDK_SHIFT_MASK);
		break;
	case GDK_Left:
		changed = prev_page(e->state & GDK_SHIFT_MASK);
		break;
	case GDK_Page_Up:
		changed = prev_page(true);
		break;
	case GDK_Page_Down:
		changed = next_page(true);
		break;
	case GDK_Home:
		changed = goto_begin();
		break;
	case GDK_End:
		changed = goto_end();
		break;
	case GDK_Up:
		if (_curr_session > 0)
			change_session(_curr_session - 1);
		break;
	case GDK_Down:
		if (_curr_session < (_num_sessions - 1))
			change_session(_curr_session + 1);
		break;
	default:		// handle alphanumeric keys
		if (e->length > 0)
		{
			char c = e->string[0];
			changed = key_press_char((char)(toupper(c)), e->state & GDK_SHIFT_MASK,
						 e->state & GDK_CONTROL_MASK);
		}
		break;
	}

	if (changed)
		update();

	return true;
}  /* on_key_press_event() */


bool
ts_view_gtk::on_button_press_event(GdkEventButton *e)
{
	if ((e->type == GDK_2BUTTON_PRESS) || (e->state & GDK_CONTROL_MASK))
	{
		button_press_double_event(e);
		return true;
	}

	// if this is subview of ra_view tell ra_view that this view has the input-focus
// 	if (name.startsWith("ra-sub-view"))
// 	{
// 		send_input_focus_change();
// // 		QWidget *parent = parentWidget();
// // 		qApp->notify(parent, ev);
// 	}

	y_to_channel((int)e->y);

	if ((e->type == GDK_BUTTON_PRESS) && (e->button == 3))
	{
		_cursor_pos = (int)e->x;
		plot_cursor();

		act_powerline_filter->set_active(_filter_powerline_noise);
		act_remove_mean->set_active(_remove_mean);
		if (_area_mark_step != MARKING_AREA_MANUAL)
		{
			act_start_mark_area->set_sensitive(true);
			act_end_mark_area->set_sensitive(false);
		}
		else
		{
			act_start_mark_area->set_sensitive(false);
			act_end_mark_area->set_sensitive(true);
		}

		if (popup_menu)
			popup_menu->popup(e->button, e->time);

		return true;
	}

	if (e->button == 1)
	{
		_cursor_pos = (int)e->x;

		if (_highlight_start >= 0)
		{
			Glib::RefPtr<Gdk::Window> win = get_window();
			win->draw_drawable(gc, pixmap, _highlight_start, 0, _highlight_start, 0, _highlight_width, _screen_height);
			_highlight_start = -1;
		}

		if (_area_mark_step != MARKING_AREA_MANUAL)
			start_area_mark((int)e->x);

		plot_cursor();
	}

	return true;
} /* on_button_press_event() */


void
ts_view_gtk::button_press_double_event(GdkEventButton *e)
{
	int idx = find_aoi((int)(e->x), (int)(e->y));
	if (idx < 0)
		return;

	if (_aoi[idx].annot != -1)
	{
		_dbl_left_click_handled = true;

		struct annot_info *curr = &(_annot[_aoi[idx].annot]);
		Glib::ustring s = curr->text;

		annot_dlg_gtk *dlg = NULL;
		glade_xml->get_widget_derived("annotation_dlg", dlg);
		dlg->init(_ch, _num_ch, curr->ch, _annot_predefs, _num_annot_predefs,
			  s,curr->noise_flag, curr->ignore_flag);
	
		int ret = dlg->run();
		if (ret == Gtk::RESPONSE_CANCEL)
		{
			return;
		}
		else if (ret == 1)  // 'Delete' button was pressed
		{
			delete_comment(_aoi[idx].annot);
			update();
			return;
		}
		else		// user pressed 'OK'
		{
			s = dlg->annot();
			if (curr->text)
			{
				delete[] curr->text;
				curr->text = NULL;
			}
			curr->text = new char[s.length() + 1];
			strcpy(curr->text, s.c_str());
			
			curr->noise_flag = dlg->is_noise();
			curr->ignore_flag = dlg->do_ignore();
			curr->ch = dlg->channel();
			
			update_comment(_aoi[idx].annot);

			update();
		}
	}
} /* button_press_double_event() */


bool
ts_view_gtk::on_button_release_event(GdkEventButton *e)
{
	if (e->button == 1)
	{
		if (_area_mark_step == MARKING_AREA_MANUAL)
			change_area_mark((int)e->x);
		else
			end_area_mark((int)e->x);
	}

	return true;
} /* on_button_release_event() */


bool
ts_view_gtk::on_motion_notify_event(GdkEventMotion *e)
{
	static bool x_outside = false;
	static bool in_change = false;

	if ((_area_mark_step != MARKING_AREA_START) && (_area_mark_step != MARKING_AREA))
	{
		x_outside = in_change = false;
		return true;
	}

	if ((e->x > (_screen_width - _right_space)) || (e->x < _left_space))
		x_outside = true;
	else
		x_outside = false;

	if (in_change)
		return true;

	in_change = true;

	long as, ae;
	do
	{
		as = _area_start;
		ae = _area_end;

 		change_area_mark((int)(e->x));
		while( Gtk::Main::events_pending() )
			Gtk::Main::iteration();

		if ((as == _area_start) && (ae == _area_end))
			break;
	} while (x_outside && (_area_mark_step == MARKING_AREA));

	in_change = false;

	return false;
} /* on_motion_notify_event() */


bool
ts_view_gtk::on_scroll_event(GdkEventScroll *e)
{
	if (e->direction == GDK_SCROLL_UP)
		prev_page(e->state & GDK_SHIFT_MASK);
	if (e->direction == GDK_SCROLL_DOWN)
		next_page(e->state & GDK_SHIFT_MASK);

	update();

	return true;
} /* on_scroll_event() */


void
ts_view_gtk::on_action_general_options()
{
// 	double mm_sec_save = _ts_opt.mm_per_sec;

	gen_opt_dlg *dlg = 0;
	glade_xml->get_widget_derived("general_options", dlg);
	dlg->init(&_ts_opt);

	int ret = dlg->run();
	if (ret == Gtk::RESPONSE_OK)
	{
		options_changed();
// 		if (mm_sec_save != _ts_opt.mm_per_sec)
// 			send_x_res((long)(parentWidget()->parentWidget())); // need parent of parent because of view_frame
	}

	update();
} /* on_action_general_options() */


void
ts_view_gtk::on_action_channel_options()
{
	ch_opt_dlg *dlg = 0;
	glade_xml->get_widget_derived("ch_draw_options", dlg);
	dlg->init(_ch, _num_ch, _curr_channel);

	int ret = dlg->run();
	if (ret == Gtk::RESPONSE_OK)
		options_changed();

	update();
} /* on_action_channel_options() */


int
ts_view_gtk::annot_dlg(char **annot, bool *noise, bool *ignore, int *ch)
{
	annot_dlg_gtk *dlg = NULL;
 	glade_xml->get_widget_derived("annotation_dlg", dlg);
	Glib::ustring s = "";
 	dlg->init(_ch, _num_ch, _curr_channel, _annot_predefs, _num_annot_predefs, s, false, false);
	
	int ret = dlg->run();
	if (ret != Gtk::RESPONSE_OK)
		return -1;

	s = dlg->annot();
	*annot = new char[s.length()+1];
	strcpy(*annot, s.c_str());

	*noise = dlg->is_noise();
	*ignore = dlg->do_ignore();
	*ch = dlg->channel();

 	return 0;
} /* annot_dlg() */


// void
// ts_view_qt::set_x_resolution(double x_res, void *parent)
// {
//         // need parent of parent because of view_frame
// 	QWidget *p = parentWidget();
// 	if (((QWidget *)parent != p) && ((QWidget *)parent != p->parentWidget()))
// 		return;

// 	_ts_opt.mm_per_sec = x_res;
// 	options_changed();
	
// 	update();
// }  // set_x_resolution()


// void
// ts_view_qt::show_session(int session)
// {
// 	change_session(session);
// } // show_session()


void
ts_view_gtk::on_action_sort_annot()
{
	sort_annotations();
} /* on_action_sort_annot() */


// void
// ts_view_qt::unset_cursor()
// {
// 	unsetCursor();
// } // set_cursor_shape()


// void
// ts_view_qt::set_busy_cursor()
// {
// 	setCursor(Qt::WaitCursor);
// } // set_busy_cursor()

void
ts_view_gtk::on_action_powerline_filter()
{
	_filter_powerline_noise = (_filter_powerline_noise ? false : true);
	update();
} /* on_action_powerline_filter() */


void
ts_view_gtk::on_action_remove_mean()
{
	_remove_mean = (_remove_mean ? false : true);
	update();
} /* on_action_remove_mean() */


void
ts_view_gtk::on_action_start_mark_area()
{
	start_area_mark(_cursor_pos);
	_area_mark_step = MARKING_AREA_MANUAL;	
} /* on_action_start_mark_area() */


void
ts_view_gtk::on_action_end_mark_area()
{
	end_area_mark(_cursor_pos);
	bool redraw = insert_comment();
	if (redraw)
		update();
} /* on_action_end_mark_area() */


void
ts_view_gtk::on_action_insert_comment()
{
	bool redraw = insert_comment();
	if (redraw)
		update();
} /* on_action_insert_comment() */


// void
// ts_view_qt::slot_online_update()
// {
// 	update();
// } // slot_online_update()
