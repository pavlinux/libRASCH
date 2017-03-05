#ifndef OPTIONS_DLG_QT_H
#define OPTIONS_DLG_QT_H

#define _LIBRASCH_BUILD
#include <ra.h>
#include <ra_priv.h>
#include <pl_general.h>


#include "options_dlg_ui.h"


class options_dlg_qt : public QDialog, public Ui::options_dlg
{
      Q_OBJECT

public:
        options_dlg_qt(struct proc_info *pi, QWidget * parent = 0);
      
public slots:
	void opt_sel(int idx);
        void change();

protected:
	QString format_option_text(long idx);

	struct proc_info *_pi;
	struct ra_option_infos *_opts;
};  // class options_dlg_qt


#endif // OPTIONS_DLG_QT_H
