#ifndef RA_VIEW_SPLITTER_QT_H
#define RA_VIEW_SPLITTER_QT_H

#include <QtGui/qsplitter.h>
#include <QtGui/QPaintEvent>


class ra_view_splitter:public QSplitter
{
      Q_OBJECT 
 public:
      ra_view_splitter(QWidget * parent = 0);

 protected:
      virtual void paintEvent(QPaintEvent * ev);
      
 signals:
      void sig_move_sizer(void);
}; // class ra_View_splitter

#endif // RA_VIEW_SPLITTER_QT_H
