#include <QtGui/QApplication>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QSplitter>
#include <QtGui/QPushButton>
#include <QtGui/QHeaderView>

//#include "dir_qt.h"
//#include "item_qt.h"
#include "sig_sel_dlg_qt.h"
#include <ra_qt_gettext.h>


sig_sel_dlg_qt::sig_sel_dlg_qt(ra_handle h, const char *path, QWidget *parent)
	:QDialog(parent)
{
	QApplication::setOverrideCursor(Qt::WaitCursor);

	ra = h;

	if (path)
		curr_path = path;

	// set dlg title
	this->setWindowTitle(QString(gettext("Select signal")));

	// on left side show directory, on the right side the measurements and
	// "connect" both with a splitter
	QSplitter *sp = new QSplitter(Qt::Horizontal, this);

	// prepare directory list
	dir_model = new QDirModel;
	dir_model->setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);
	dir_list = new QTreeView(sp);
	dir_list->setModel(dir_model);

	dir_list->setColumnHidden(1, true);
	dir_list->setColumnHidden(2, true);
	dir_list->setColumnHidden(3, true);
	QHeaderView *hdr = dir_list->header();
	hdr->resizeSections(QHeaderView::Stretch);

	sel_dir_model = new QItemSelectionModel(dir_model);
	dir_list->setSelectionModel(sel_dir_model);

	QModelIndex curr_dir = dir_model->index(curr_path);
	dir_list->setCurrentIndex(curr_dir);

	connect(sel_dir_model, SIGNAL(currentRowChanged(const QModelIndex &, const QModelIndex &)),
		this, SLOT(search_dir(const QModelIndex &, const QModelIndex &)));

	// prepare measurement list
	meas_list = new QTreeView(sp);
	meas_model = new item_qt(ra, curr_path);
	meas_list->setModel(meas_model);
	meas_list->setRootIsDecorated(false);
	meas_list->setAlternatingRowColors(true);
//	meas_list->header()->setSortIndicatorShown(true);
	meas_list->header()->setStretchLastSection(false);  // so last section is not "compressed"

	sel_meas_model = new QItemSelectionModel(meas_model);
	meas_list->setSelectionModel(sel_meas_model);
	meas_list->setSelectionBehavior(QAbstractItemView::SelectRows);
	meas_list->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(sel_meas_model, SIGNAL(currentRowChanged(const QModelIndex &, const QModelIndex &)),
		this, SLOT(meas_selected(const QModelIndex &, const QModelIndex &)));

	// make the buttons
	cancel = new QPushButton("Cancel", this);
	QSize s = cancel->size();
	cancel->setMaximumSize(s);
	ok = new QPushButton("OK", this);
	s = ok->size();
	ok->setMaximumSize(s);

	// do layout things
	QHBoxLayout *btns = new QHBoxLayout();
	btns->setMargin(0);
	btns->addStretch(1);	// buttons have to be on the right side
	btns->addWidget(cancel);
	btns->addWidget(ok);

	QVBoxLayout *top = new QVBoxLayout(this);
	top->setMargin(0);
	top->addWidget(sp);
	top->addLayout(btns);

	// set exit connections
	connect(meas_list, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(accept()));
	connect(ok, SIGNAL(clicked()), this, SLOT(accept()));
	connect(cancel, SIGNAL(clicked()), this, SLOT(reject()));

	// set sizes
	sp->setStretchFactor(1, 2);
	sp->show();
	this->resize(750, 300);

	// set initial states
	ok->setEnabled(false);

	QApplication::restoreOverrideCursor();
}  // constructor


void
sig_sel_dlg_qt::search_dir(const QModelIndex &curr, const QModelIndex &/*prev*/)
{
	QApplication::setOverrideCursor(Qt::WaitCursor);

	ok->setEnabled(false);
	curr_path = "";
	curr_meas_fn = "";

	if (!dir_model->isDir(curr))
	{
		QApplication::restoreOverrideCursor();
		return;
	}

	QString fn = dir_model->filePath(curr);
	curr_path = fn;

	meas_model->use_dir(fn);

	QApplication::restoreOverrideCursor();
} // search_dir()


void
sig_sel_dlg_qt::meas_selected(const QModelIndex &/*curr*/, const QModelIndex &/*prev*/)
{
	ok->setEnabled(true);
} // meas_selected()


void
sig_sel_dlg_qt::accept()
{
	QModelIndex curr = sel_meas_model->currentIndex();
	curr_meas_fn = meas_model->filename(curr.row());

	QDialog::accept();
} // accept()
