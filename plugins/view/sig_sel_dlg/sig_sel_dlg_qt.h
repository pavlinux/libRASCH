#ifndef SIG_SEL_DLG_QT_H
#define SIG_SEL_DLG_QT_H

#include <QtGui/QDialog>
#include <QtGui/QTreeView>
#include <QtGui/QListView>
#include <QtGui/QDirModel>
#include <QtGui/QItemSelectionModel>

#include "item_qt.h"


class sig_sel_dlg_qt : public QDialog
{
	Q_OBJECT

 public:
	sig_sel_dlg_qt(ra_handle h, const char *path, QWidget *parent);

	QString get_signal() { return curr_meas_fn; }

public slots:
	void search_dir(const QModelIndex &curr, const QModelIndex &prev);
	void meas_selected(const QModelIndex &curr, const QModelIndex &prev);
	void accept();

protected:
	QDirModel *dir_model;
	QItemSelectionModel *sel_dir_model;
	QTreeView *dir_list;

	item_qt *meas_model;
	QItemSelectionModel *sel_meas_model;
	QTreeView *meas_list;

	QString curr_path;
	QString curr_meas_fn;

	QPushButton *ok;
	QPushButton *cancel;

	ra_handle ra;
}; // class sig_sel_dlg_qt


#endif // SIG_SEL_DLG_QT_H
