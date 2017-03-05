#ifndef ANNOT_COMMENT_TREE_VIEW
#define ANNOT_COMMENT_TREE_VIEW

#include <QtGui/QTreeView>


class annot_comment_tree_view : public QTreeView
{
	Q_OBJECT

public:
	annot_comment_tree_view(QWidget * parent = 0);
	~annot_comment_tree_view();

public slots:
	virtual void mouseDoubleClickEvent(QMouseEvent * ev);

};  // class annot_comment_tree_view


#endif  /* ANNOT_COMMENT_TREE_VIEW */
