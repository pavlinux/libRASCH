/*----------------------------------------------------------------------------
 * ts_view_qt.h
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

#ifndef _TS_VIEW_QT_H
#define _TS_VIEW_QT_H

#include <QtGui/qapplication.h>
#include <QtGui/qwidget.h>
#include <QtGui/qpen.h>
#include <QtGui/QMenu>
#include <QtGui/qpainter.h>
#include <QtGui/QPaintDevice>
#include <QtGui/qscrollbar.h>
#include <QtGui/qlayout.h>
#include <QtGui/QMouseEvent>
#include <QtGui/QPixmap>
#include <QtGui/QWheelEvent>
#include <QtGui/QKeyEvent>
#include <QtGui/QPaintEvent>
#include <QtGui/QRubberBand>

#include "../shared/progress_dlg_qt.h"

#include "ts_view_general.h"


class ts_view_qt:public QWidget, public virtual ts_view_general
{
	Q_OBJECT
public:
	ts_view_qt(meas_handle h, int num_ch=0, int *ch=NULL, QWidget * p=0, const char *name=NULL, Qt::WFlags f=0);
	~ts_view_qt();

	virtual void update();
	virtual void set_x_resolution(double x_res, void *parent);

protected slots:
	void general_options();
	void ch_options();
	void show_session(int session);
	void slot_sort_annot();
	void slot_powerline_filter();
	void slot_remove_mean();
	void slot_online_update();
	void slot_insert_comment();
    void slot_del_events_in_area();
	void slot_save_ch_settings();
	void slot_load_ch_settings();

protected:
	virtual void unset_cursor();
	virtual void set_busy_cursor();

	virtual void keyPressEvent(QKeyEvent * ev);
	virtual void paintEvent(QPaintEvent * ev);
	virtual void mousePressEvent(QMouseEvent * ev);
	virtual void mouseReleaseEvent(QMouseEvent * ev);
	virtual void mouseMoveEvent(QMouseEvent * ev);
	virtual void mouseDoubleClickEvent(QMouseEvent *ev);
	virtual void wheelEvent(QWheelEvent *e);

	virtual int annot_dlg(char **annot, bool *noise, bool *ignore, int *ch);

	virtual void plot_grid(QPainter *p);
	void plot_channel(QPainter *p, int ch, int line);
	void plot_statusbar(QPainter *p);
	void print_comment_annotations(QPainter *p);
	void plot_focus(QPainter *p);

	virtual void plot_cursor(void *p);

	virtual void get_text_extent(void *p, const char *text, int *w, int *h);

	virtual void highlight_area();

	QString _name;

	QMenu *_popup_menu;
	QMenu *_popup_menu_filter;
	QMenu *_popup_menu_sessions;
	QMenu *_popup_menu_marked_area;
	QMenu *_popup_menu_ch_save;

	QAction *_gen_opt_act;
	QAction *_ch_opt_act;
	QAction *_sort_annot_act;
	QAction *_pw_filter_act;
	QAction *_rm_mean_act;

	QAction *_add_area_comment_act;
    QAction *_del_events_in_area_act;

	QAction *_save_ch_settings_act;
	QAction *_load_ch_settings_act;

	// drawing (Qt specific)
	QPalette _palette;
	QPen _grid_pen;
	QPen _text_pen;

	QRubberBand *rubber_band;

	QPixmap _widget_save;

	// online signal specific stuff
	QTimer *_online_timer;	
}; // class ts_view_qt



#endif // _TS_VIEW_QT_H
