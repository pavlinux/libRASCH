#ifndef CH_OPTDLG_QT_H
#define CH_OPTDLG_QT_H

#include "ch_optdlg_ui.h"
#include "ts_view_general.h"


class ch_optdlg_qt: public QDialog, public Ui::ch_optdlg_ui
{
	Q_OBJECT

 public:
	ch_optdlg_qt(struct ch_info **ch, int num, int curr_ch, QWidget * parent = 0);
	~ch_optdlg_qt();
	
 public slots:
	void _ch_select(int index);
	void _minmax_click();
	void _centered_click();
	void _invert_click();
	void _use_res_click();
	void _ok();
	
 protected:
	void _save_data(int ch);
	void _pos_sel();
	
	
	struct ch_info **_ch_save;
	struct ch_info *_ch;
	int _num;
	
	int _prev_ch;
}; // class ch_optdlg_qt

#endif //  CH_OPTDLG_QT_H
