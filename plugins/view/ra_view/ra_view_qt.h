#ifndef RA_VIEW_QT_H
#define RA_VIEW_QT_H

#include <QtGui/qwidget.h>
#include <QtGui/qscrollbar.h>
#include <QtGui/qlayout.h>
#include <QtCore/QEvent>
#include <QtGui/QSplitter>

#include "ra_view_general.h"
#include "ra_view_splitter_qt.h"

class ra_view_qt : public QWidget, public virtual ra_view_general
{
      Q_OBJECT

 public:
      ra_view_qt(meas_handle h, QWidget *p = 0);
      ~ra_view_qt();

      virtual void do_update();

 protected slots:
      void get_child_dimensions();
     
 protected:
      virtual bool event(QEvent * ev);
      
      void show_focus(int prev_focus = -1);
      
      virtual void *create_main_view();
      virtual int add_single_view(void *view);
      virtual int get_view_dimensions(void *vh, long *width, long *height);
      virtual int pos_view(void *vh, long x, long y);
      

      ra_view_splitter *_split_main;
}; // class ra_view_qt


#endif // RA_VIEW_QT_H
