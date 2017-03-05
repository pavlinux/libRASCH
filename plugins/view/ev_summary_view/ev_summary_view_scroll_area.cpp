#include <QtGui/QApplication>
#include <QtGui/QScrollBar>
#include "ev_summary_view_scroll_area.h"


ev_summary_view_scroll_area::ev_summary_view_scroll_area(meas_handle mh, sum_handle sh, QWidget *parent)
	: QScrollArea(parent)
{
	child = new ev_summary_view_qt(mh, sh, NULL);
	setWidget(child);
} // constructor


ev_summary_view_scroll_area::~ev_summary_view_scroll_area()
{
} // destructor


void
ev_summary_view_scroll_area::keyPressEvent(QKeyEvent *ev)
{
	qApp->notify(child, ev);

	if (!ev->isAccepted())
		QScrollArea::keyPressEvent(ev);
	else
	{
		int xs, xe;
		// TODO: add support for y-axis if 2d summaries are supported
		child->get_area_to_be_visible(&xs, &xe);
		// check for sane values in xs and xe
		if ((xs > xe) || (xs == xe))
			return;
		
		QScrollBar *sb = horizontalScrollBar();
		if (sb->value() > xs)
			sb->setSliderPosition(xs);
		else
		{
			int curr_pos = sb->value();
			while (((curr_pos + sb->pageStep()) < xe) && (curr_pos < sb->maximum()))
			{
				curr_pos += (xe-xs);
				sb->setSliderPosition(curr_pos);
			}
		}
	}
} // keyPressEvent()
