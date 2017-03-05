#include "progress_dlg_qt.h"
#include <QtGui/qprogressbar.h>
#include <QtGui/qlabel.h>


progress_dlg_qt::progress_dlg_qt(QWidget *parent/*=0*/)
	: QDialog(parent)
{
	setupUi(this);
}

progress_dlg_qt::~progress_dlg_qt()
{
}

void
progress_dlg_qt::set_text(QString s)
{
	text->setText(s);
}  // set_text()


void
progress_dlg_qt::set_progress(int value)
{
	progress_bar->setValue(value);
}  // set_progress()
