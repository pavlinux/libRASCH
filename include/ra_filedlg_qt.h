#include <qdialog.h>
#include <qlistview.h>

extern "C"
{
#include <ra.h>
}


class ra_filedlg:public QDialog
{
      Q_OBJECT public:
	  ra_filedlg(QWidget * parent, QString name, bool modal, ra_handle h);

	QString get_signal();

	public slots:void _search_dir(QListViewItem * i);
	void _handle_ok_btn(QListViewItem * i);

      protected:
	  QListView * _dirlist;
	QListView *_measlist;

	QPushButton *_ok;
	QPushButton *_cancel;

	ra_handle _ra;
};				// class ra_filedlg
