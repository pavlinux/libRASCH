#include <QtGui/qpushbutton.h>
#include <QtGui/qcheckbox.h>
#include <QtGui/qlineedit.h>
#include <QtGui/qlabel.h>

#include "ts_view_optdlg_qt.h"


ts_view_optdlg_qt::ts_view_optdlg_qt(struct ts_options * ts_opt, QWidget *parent/*=0*/)
	: QDialog(parent)
{
	setupUi(this);

	if (ts_opt)
		memcpy(&_ts_opt, ts_opt, sizeof(struct ts_options));
	_ts_opt_save = ts_opt;

	show_ch_info->setChecked(_ts_opt.show_ch_info);
	show_ch_scale->setChecked(_ts_opt.show_ch_scale);
	ch_height_fixed->setChecked(!_ts_opt.ch_height_dynamic);
	QString s;
	ch_height->setText(s.setNum(_ts_opt.ch_height));
	show_status->setChecked(_ts_opt.show_status);
	mm_sec->setText(s.setNum(_ts_opt.mm_per_sec));
	show_grid->setChecked(_ts_opt.plot_grid);
	show_annotations->setChecked(_ts_opt.show_annot);

	QObject::connect(ok_btn, SIGNAL(clicked()), this, SLOT(_ok()));
	QObject::connect(cancel_btn, SIGNAL(clicked()), this, SLOT(reject()));
	QObject::connect(ch_height_fixed, SIGNAL(clicked()), this, SLOT(_ch_height_click()));
}  // constructor


void
ts_view_optdlg_qt::_ch_height_click()
{
	if (ch_height_fixed->isChecked())
	{
		ch_height->setEnabled(true);
		ch_height_text->setEnabled(true);
	}
	else
	{
		ch_height->setEnabled(false);
		ch_height_text->setEnabled(false);
	}
}  // _ch_height_click()


void
ts_view_optdlg_qt::_ok()
{
	_ts_opt.show_ch_info = show_ch_info->isChecked();
	_ts_opt.show_ch_scale = show_ch_scale->isChecked();
	_ts_opt.ch_height_dynamic = !ch_height_fixed->isChecked();
	_ts_opt.ch_height = ch_height->text().toInt();
	_ts_opt.show_status = show_status->isChecked();
	_ts_opt.mm_per_sec = mm_sec->text().toDouble();
	_ts_opt.plot_grid = show_grid->isChecked();
	_ts_opt.show_annot = show_annotations->isChecked();

	memcpy(_ts_opt_save, &_ts_opt, sizeof(struct ts_options));

	accept();
}  // _ok()
