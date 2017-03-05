#ifndef EV_SUMMARY_VIEW_QT_H
#define EV_SUMMARY_VIEW_QT_H

#include <QtGui/QWidget>
#include <QtGui/QMouseEvent>
#include <QtGui/QKeyEvent>

#include "ev_summary_view_general.h"


class ev_summary_view_qt : public QWidget, public ev_summary_view_general
{
	Q_OBJECT
public:
	ev_summary_view_qt(meas_handle h, sum_handle sh, QWidget *p = 0);
        ~ev_summary_view_qt();
 
        virtual void update();

 protected slots:
        virtual void paintEvent(QPaintEvent *ev);
	virtual void mouseDoubleClickEvent(QMouseEvent * ev);
	virtual void keyPressEvent(QKeyEvent * ev);

 protected:
//	void set_scroll_size();
	virtual void unset_cursor();
	virtual void set_busy_cursor();
 
	void plot_one_dim(QPainter *p, long x_start, long part_idx);
	void plot_ev_summary_1d(QPainter *p, long x_start, long part_idx);
	void plot_channel_1d(QPainter *p, long x_start, long part_idx, long ch);

    void show_selected_templ(QPainter * p);
 
	QPalette palette;
};  // class ev_summary_view_qt


#endif // EV_SUMMARY_VIEW_QT_H
