#include <qdir.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qapplication.h>

#include "dir_qt.h"


dir_qt::dir_qt(Q3ListView * parent)
:Q3ListViewItem(parent), f("/")
{
	p = 0;
	readable = true;
}


dir_qt::dir_qt(dir_qt * parent, const char *fn)
:Q3ListViewItem(parent), f(fn)
{
	p = parent;
}

QString dir_qt::text(int c) const
{
	switch (c)
	{
	case 0:
		return f.name();
	default:
		return "";
	}
}				// text()


QString dir_qt::fullName()
{
	QString
		s;
	if (p)
	{
		s = p->fullName();
		s.append(f.name());
		s.append("/");
	}
	else
		s = "/";

	// be portable (TODO: test with win32)
	QDir
		d;
	s = d.convertSeparators(s);

	return s;
}				// fullName()


void
dir_qt::setOpen(bool o)
{
	QApplication::setOverrideCursor(Qt::waitCursor);

	if (o && !childCount())
	{
		QString s(fullName());
		QDir thisDir(s);
		setText(0, f.name());
/*		if (!readable)
		{
			QApplication::restoreOverrideCursor();
			return;
		}
*/
		const QFileInfoList *files = thisDir.entryInfoList();
		if (files)
		{
			QFileInfoListIterator it(*files);
			QFileInfo *f;
			while ((f = it.current()) != 0)
			{
				++it;
				if ((f->fileName() == ".") || (f->fileName() == ".."))
					;
				else if (f->isDir())
					new dir_qt(this, f->fileName());
			}
		}
	}
	Q3ListViewItem::setOpen(o);

	QApplication::restoreOverrideCursor();
}				// setOpen()


void
dir_qt::setup()
{
	QString s(fullName());
	QFileInfo fi(s);
	setExpandable(readable = fi.isReadable());
	Q3ListViewItem::setup();
}				// setup()
