#ifndef _RA_ITEM_H
#define _RA_ITEM_H

#include <qlistview.h>

extern "C"
{
#include <ra.h>
}


class ra_item:public QListViewItem
{
      public:
	ra_item(QListView * parent, ra_handle h, char *fn);

	void setup();
	QString text(int c) const;

	QString filename();

      private:
	  ra_handle _ra;
	QString _fn;

	QString _name, _forename, _dob, _id, _rec_date, _eval_date, _sig_name, _sig_type;
};				// class dir

#endif // _RA_ITEM_H
