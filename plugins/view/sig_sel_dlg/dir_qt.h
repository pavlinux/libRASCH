#ifndef _DIR_QT_H
#define _DIR_QT_H

#include <q3listview.h>
#include <qstring.h>
#include <qfile.h>

class dir_qt:public Q3ListViewItem
{
      public:
	dir_qt(Q3ListView * parent);
	dir_qt(dir_qt * parent, const char *fn);

	QString text(int c) const;
	QString fullName();

	void setOpen(bool o);
	void setup();

      private:
	  QFile f;
	dir_qt *p;
	bool readable;
};				// class dir_qt

#endif // _DIR_QT_H
