#include <QtGui/QPainter>
#include <QtGui/QMouseEvent>
#include <QtGui/QKeyEvent>
#include <QtGui/QCursor>

#include <stdio.h>
#include <ctype.h>

#define _LIBRASCH_BUILD
#include <ra.h>
#include <ra_priv.h>

#include "annot_comment_view_qt.h"


annot_comment_view_qt::annot_comment_view_qt(meas_handle mh, eval_handle eh, QWidget *p/*=0*/)
	: QWidget(p),
	  annot_comment_view_general(mh, eh)
{
	setBackgroundRole(QPalette::Light);
	setAutoFillBackground(true);

	annot_comment_model *model = new annot_comment_model(mh, eh);
    tree = new annot_comment_tree_view(this);
    tree->setModel(model);
	tree->setRootIsDecorated(false);
} // constructor


annot_comment_view_qt::~annot_comment_view_qt()
{
} // destructor


void
annot_comment_view_qt::resizeEvent(QResizeEvent *ev)
{
	tree->resize(ev->size().width(), ev->size().height());
}  // resizeEvent()


void
annot_comment_view_qt::mouseDoubleClickEvent(QMouseEvent * /*ev*/)
{
	QModelIndex idx = tree->currentIndex();
	if (!idx.isValid())
		return;

	annot_comment_model *model = (annot_comment_model *)tree->model();
	model->send_event_signal(idx.row());
} // mouseDoubleClickEvent()


void
annot_comment_view_qt::keyPressEvent(QKeyEvent *ev)
{
	switch (ev->key())
	{
	case 'A':  // dummy
		break;
	default:
		break;
	}
} // keyPressEvent()


void
annot_comment_view_qt::unset_cursor()
{
	unsetCursor();
} // unset_cursor()


void
annot_comment_view_qt::set_busy_cursor()
{
	setCursor(Qt::WaitCursor);
} // set_busy_cursor()


void
annot_comment_view_qt::eval_change()
{
	annot_comment_model *model = (annot_comment_model *)tree->model();
	model->init();
	tree->reset();
} // eval_change()
