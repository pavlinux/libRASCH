#include <QtGui/QScrollArea>
#include "ev_summary_view_qt.h"


class ev_summary_view_scroll_area : public QScrollArea
{
	Q_OBJECT

 public:
	ev_summary_view_scroll_area(meas_handle mh, sum_handle sh, QWidget *parent);
	~ev_summary_view_scroll_area();

 protected slots:
	virtual void keyPressEvent(QKeyEvent *ev);

 private:
        ev_summary_view_qt *child;
}; // class ev_summary_view_scroll_area
