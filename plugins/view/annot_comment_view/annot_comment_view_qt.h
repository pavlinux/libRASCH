#ifndef ANNOT_COMMENT_VIEW_QT_H
#define ANNOT_COMMENT_VIEW_QT_H

#include <QtGui/QWidget>
#include <QtGui/QMouseEvent>
#include <QtGui/QKeyEvent>


#include "annot_comment_view_general.h"
#include "annot_comment_tree_view.h"
#include "annot_comment_model.h"

class annot_comment_view_qt : public QWidget, public annot_comment_view_general
{
	Q_OBJECT
public:
	annot_comment_view_qt(meas_handle h, eval_handle eh, QWidget *p = 0);
    ~annot_comment_view_qt();
 
	virtual void eval_change();

 protected slots:
	virtual void resizeEvent(QResizeEvent *ev);
	virtual void mouseDoubleClickEvent(QMouseEvent * ev);
	virtual void keyPressEvent(QKeyEvent * ev);

 protected:
	virtual void unset_cursor();
	virtual void set_busy_cursor();
 
	annot_comment_tree_view *tree;
};  // class annot_comment_view_qt


#endif // ANNOT_COMMENT_VIEW_QT_H
