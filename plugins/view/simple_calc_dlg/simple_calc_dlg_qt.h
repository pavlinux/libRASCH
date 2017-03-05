#ifndef SIMPLE_CALC_DLG_QT_H
#define SIMPLE_CALC_DLG_QT_H

#define _LIBRASCH_BUILD
#include <ra.h>
#include <ra_priv.h>
#include <pl_general.h>


#include "simple_calc_dlg_ui.h"


class simple_calc_dlg_qt : public QDialog, public Ui::simple_calc_dlg
{
      Q_OBJECT

public:
      simple_calc_dlg_qt(struct proc_info *pi, QWidget * parent = 0);
      ~simple_calc_dlg_qt();

public slots:
	void calc();
        void set_options();
	void copy_results_to_clipboard();

protected:
	struct proc_info *_pi;
	int _num_proc_pl;
	struct proc_info **_pi_for_pl;
	QString results_text;
};  // class simple_calc_dlg_qt


#endif // SIMPLE_CALC_DLG_QT_H
