#ifndef ANNOT_DLG_QT_H
#define ANNOT_DLG_QT_H

#include "annot_dlg_ui.h"
#include "ts_view_general.h"


class annot_dlg_qt: public QDialog, public Ui::annot_dlg_ui
{
	Q_OBJECT

public:
	annot_dlg_qt(struct ch_info *ch, int num_ch, int curr_ch, char **predefs, int num_predefs,
		     QString text, bool noise = false, bool ignore = false, int ch_sel = -1,
		     QWidget * parent = 0);
	~annot_dlg_qt();
	
	QString annot();
	long channel();
	bool is_noise();
	bool do_ignore();
	bool delete_annot();

public slots:
	void sel_annot(const QString &annot);
        void del_pressed();

protected:
         struct ch_info *_ch ;
	bool _del;
	int _curr_ch;
}; // class annot_dlg_qt

#endif //  ANNOT_DLG_QT_H
