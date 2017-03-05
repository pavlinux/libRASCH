#ifndef _EVAL_DLG_QT_H
#define _EVAL_DLG_QT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define _LIBRASCH_BUILD
#include <ra.h>
#include <ra_priv.h>
#include <pl_general.h>

#include "eval_dlg_ui.h"

class eval_dlg_qt : public QDialog, public Ui::eval_dlg_ui
{
        Q_OBJECT
public:
	eval_dlg_qt(meas_handle mh, QWidget * parent = 0);

protected slots:
	void slot_eval(int idx);
	void slot_del();
	void slot_cancel();
	void slot_apply();
	void slot_ok();
	void slot_show_prop();
	void slot_modified();
	void slot_default();

protected:
	void init();

	meas_handle _mh;
	eval_handle *_all_eval;
	int _num_eval;
	int _curr_eval;
	bool _modified;
}; // class eval_dlg_qt


#endif /* _EVAL_DLG_QT_H */
