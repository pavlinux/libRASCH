#ifndef _DIR_H
#define _DIR_H

#include <qlistview.h>
#include <qstring.h>
#include <qfile.h>

class dir:public QListViewItem
{
      public:
	dir(QListView * parent);
	dir(dir * parent, const char *fn);

	QString text(int c) const;
	QString fullName();

	void setOpen(bool o);
	void setup();

      private:
	  QFile f;
	dir *p;
	bool readable;
};				// class dir

#endif
