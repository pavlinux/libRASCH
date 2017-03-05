#ifndef TS_VIEW_OPTDLG_QT_H
#define TS_VIEW_OPTDLG_QT_H

#include "ts_view_optdlg_ui.h"
#include "ts_view_general.h"


class ts_view_optdlg_qt: public QDialog, public Ui::ts_view_optdlg_ui
{
	Q_OBJECT

 public:
	ts_view_optdlg_qt(struct ts_options *ts_opt, QWidget * parent = 0);

 protected slots:
	void _ok();
	void _ch_height_click();

 protected:
	struct ts_options *_ts_opt_save;
	struct ts_options _ts_opt;
};  // class ts_view_optdlg_qt


#endif // TS_VIEW_OPTDLG_QT_H
