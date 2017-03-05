#include <QtGui/qapplication.h>

#include "annot_comment_tree_view.h"


annot_comment_tree_view::annot_comment_tree_view(QWidget *parent/* = 0*/)
	: QTreeView(parent)
{
}  // constructor


annot_comment_tree_view::~annot_comment_tree_view()
{
}  // destructor


void
annot_comment_tree_view::mouseDoubleClickEvent(QMouseEvent * ev)
{
	if (parent())
	{
		qApp->notify(parent(), (QEvent *)ev);		
	}
} // mouseDoubleClickEvent()


