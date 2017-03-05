/*----------------------------------------------------------------------------
 * ts_view_gtk.h
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

#ifndef TS_VIEW_GTK_H
#define TS_VIEW_GTK_H

#include <gtkmm/drawingarea.h>
#include <gtkmm/uimanager.h>
#include <gtkmm/actiongroup.h>
#include <gtkmm/toggleaction.h>
#include <gtkmm/menu.h>

#include <gdkmm/colormap.h>
#include <gdkmm/window.h>

#include <libglademm/xml.h>

//#include "../shared/progress_dlg_qt.h"

#include "ts_view_general.h"


class ts_view_gtk : public Gtk::DrawingArea, public virtual ts_view_general
{
public:
	ts_view_gtk(meas_handle h, int num_ch = 0, int *ch = NULL, const char *win_name = NULL);
	~ts_view_gtk();

	virtual void update();
/* 	virtual void set_x_resolution(double x_res, void *parent); */

protected:
	virtual void on_realize();
	virtual bool on_expose_event(GdkEventExpose *e);
	virtual bool on_key_press_event(GdkEventKey *e);
	virtual bool on_button_press_event(GdkEventButton *e);
	virtual bool on_button_release_event(GdkEventButton *e);
	virtual bool on_motion_notify_event(GdkEventMotion *e);
	virtual bool on_scroll_event(GdkEventScroll *e);

	void button_press_double_event(GdkEventButton *e);

	virtual void plot_grid();
 	void plot_statusbar();
	void plot_channel(int ch, int line);
	void print_comment_annotations();
	virtual void get_text_extent(char *text, int *w, int *h);
	virtual void plot_cursor();
	virtual void highlight_area();
/* 	void plot_focus(QPainter *p); */
	virtual void do_additional_drawing() { /* do nothing here */ }

	virtual int annot_dlg(char **annot, bool *noise, bool *ignore, int *ch);

	Glib::RefPtr<Gdk::GC> gc;
	Gdk::Color bg_color, draw_color, grid_color, cursor_color;

	/* popup menu actions */
	void on_action_general_options();
	void on_action_channel_options();

	void on_action_sort_annot();

	void on_action_powerline_filter();
	void on_action_remove_mean();

	void on_action_start_mark_area();
	void on_action_end_mark_area();

	void on_action_insert_comment();

/* 	void show_session(int session); */
/* 	void slot_sort_annot(); */
/* 	void slot_online_update(); */

/* 	virtual void unset_cursor(); */
/* 	virtual void set_busy_cursor(); */


	Glib::ustring name;

	Glib::RefPtr<Gnome::Glade::Xml> glade_xml;

	Glib::RefPtr<Gtk::UIManager> ui_manager;
	Glib::RefPtr<Gtk::ActionGroup> popup_group;
	Glib::RefPtr<Gtk::ActionGroup> popup_filter_group;
	Glib::RefPtr<Gtk::ToggleAction> act_powerline_filter;
	Glib::RefPtr<Gtk::ToggleAction> act_remove_mean;
	Glib::RefPtr<Gtk::Action> act_start_mark_area;
	Glib::RefPtr<Gtk::Action> act_end_mark_area;

	Gtk::Menu *popup_menu;

/* 	QMenu *_popup_menu; */
/* 	QMenu *_popup_menu_filter; */
/* 	QMenu *_popup_menu_sessions; */

/* 	QAction *_gen_opt_act; */
/* 	QAction *_ch_opt_act; */
/* 	QAction *_sort_annot_act; */
/* 	QAction *_pw_filter_act; */
/* 	QAction *_rm_mean_act; */

	int _highlight_start;
	int _highlight_width;

	Glib::RefPtr<Gdk::Pixmap> pixmap;

/* 	// online signal specific stuff */
/* 	QTimer *_online_timer;	 */
}; // class ts_view_gtk



#endif // TS_VIEW_GTK_H
