#ifndef PROGRESS_DLG_QT_H
#define PROGRESS_DLG_QT_H


#include "progress_dlg_qt_ui.h"

class progress_dlg_qt : public QDialog, public Ui::progress_dlg_qt
{
	Q_OBJECT

public:
	progress_dlg_qt(QWidget* parent = 0);
	~progress_dlg_qt();

	void set_text(QString s);
	void set_progress(int value);
};  // class progress_dlg_qt


#endif  // PROGRESS_DLG_QT_H
