#include "ra_view_splitter_qt.h"
#include <QtGui/QPaintEvent>

ra_view_splitter::ra_view_splitter(QWidget *parent/*=0*/)
	: QSplitter(parent)
{
	
} // constructor


void
ra_view_splitter::paintEvent(QPaintEvent * ev)
{
	QSplitter::paintEvent(ev);
	emit sig_move_sizer();
} // paintEvent()
