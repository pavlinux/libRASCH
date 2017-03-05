#include <QtGui/qcombobox.h>
#include <QtGui/qradiobutton.h>
#include <QtGui/qpushbutton.h>
#include <QtGui/qcheckbox.h>
#include <QtGui/qlabel.h>
#include <QtGui/qlineedit.h>

#include "ch_optdlg_qt.h"


ch_optdlg_qt::ch_optdlg_qt(struct ch_info **ch, int num, int curr_ch, QWidget *parent/*=0*/)
	: QDialog(parent)
{
	setupUi(this);

	_ch_save = ch;
	_num = num;

	if (num > 0)
	{
		_ch = new struct ch_info[num];
		memcpy(_ch, *_ch_save, sizeof(struct ch_info) * num);
	}
	else
		_ch = NULL;

	_prev_ch = -1;


	for (int i = 0; i < _num; i++)
		ch_list->addItem(_ch[i].name);

	if (_num > 0)
	{
		int idx = 0;

		if ((curr_ch >= 0) && (curr_ch <= num))
			idx = curr_ch;

		ch_list->setCurrentIndex(idx);
		_ch_select(idx);
	}

	QObject::connect(ch_list, SIGNAL(activated(int)), this, SLOT(_ch_select(int)));
	QObject::connect(pos_centered, SIGNAL(clicked()), this, SLOT(_centered_click()));
	QObject::connect(pos_minmax, SIGNAL(clicked()), this, SLOT(_minmax_click()));
	QObject::connect(cancel_btn, SIGNAL(clicked()), this, SLOT(reject()));
	QObject::connect(ok_btn, SIGNAL(clicked()), this, SLOT(_ok()));
	QObject::connect(use_res, SIGNAL(clicked()), this, SLOT(_use_res_click()));
	QObject::connect(invert, SIGNAL(clicked()), this, SLOT(_invert_click()));
} // constructor


ch_optdlg_qt::~ch_optdlg_qt()
{
	if (_ch)
		delete[]_ch;
} // destructor


void
ch_optdlg_qt::_ch_select(int index)
{
	if (index != _prev_ch)
	{
		_save_data(_prev_ch);
		_prev_ch = index;
	}

	invert->setChecked(_ch[index].inverse);
	QString s;
	max->setText(s.setNum(_ch[index].max_value));
	min->setText(s.setNum(_ch[index].min_value));
	percent->setText(s.setNum(_ch[index].center_pos));
	resolution->setText(s.setNum(_ch[index].mm_per_unit));
	show_ch->setChecked(_ch[index].show_it);
	use_res->setChecked(_ch[index].use_mm_per_unit);
	s = "mm/";
	s += _ch[index].unit;
	resolution_text->setText(s);

	s = "min/max [";
	s += _ch[index].unit;
	s += "]";
	pos_minmax->setText(s);

	_pos_sel();
} // _ch_select()


void
ch_optdlg_qt::_save_data(int ch)
{
	if (ch < 0)
		return;

	_ch[ch].inverse = invert->isChecked();
	_ch[ch].max_value = max->text().toDouble();
	_ch[ch].min_value = min->text().toDouble();
	_ch[ch].center_pos = percent->text().toInt();
	_ch[ch].mm_per_unit = resolution->text().toDouble();
	_ch[ch].show_it = show_ch->isChecked();
	_ch[ch].use_mm_per_unit = use_res->isChecked();
} // _save_data()


void
ch_optdlg_qt::_pos_sel()
{
	int curr = ch_list->currentIndex();
	if (curr < 0)
		return;

	if (_ch[curr].centered)
	{
		pos_centered->setChecked(true);
		pos_minmax->setChecked(false);
	}
	else
	{
		pos_centered->setChecked(false);
		pos_minmax->setChecked(true);
	}

	invert->setEnabled(true);
	if (_ch[curr].centered)
	{
		percent->setEnabled(true);
		percent_text->setEnabled(true);
		//invert->setEnabled(true);

		min_text->setEnabled(false);
		min->setEnabled(false);
		max_text->setEnabled(false);
		max->setEnabled(false);
		use_res->setEnabled(false);
	}
	else
	{
		percent->setEnabled(false);
		percent_text->setEnabled(false);
		//invert->setEnabled(false);

		min_text->setEnabled(true);
		min->setEnabled(true);

		use_res->setEnabled(true);
		_use_res_click();
	}
} // _pos_sel()


void
ch_optdlg_qt::_use_res_click()
{
	int curr = ch_list->currentIndex();
	if (curr < 0)
		return;

	_ch[curr].use_mm_per_unit = use_res->isChecked();

	if (_ch[curr].use_mm_per_unit)
	{
		max_text->setEnabled(false);
		max->setEnabled(false);
	}
	else
	{
		max_text->setEnabled(true);
		max->setEnabled(true);
	}
} // _use_res_click()


void
ch_optdlg_qt::_invert_click()
{
	int curr = ch_list->currentIndex();
	if (curr < 0)
		return;

	// swap min- and max-value
	QString temp = max->text();
	max->setText(min->text());
	min->setText(temp);
} // _invert_click()


void
ch_optdlg_qt::_minmax_click()
{
	int curr = ch_list->currentIndex();
	if (curr < 0)
		return;

	_ch[curr].centered = false;
	_pos_sel();
} // _minmax_click()


void
ch_optdlg_qt::_centered_click()
{
	int curr = ch_list->currentIndex();
	if (curr < 0)
		return;

	_ch[curr].centered = true;
	_pos_sel();
} // _centered_click()


void
ch_optdlg_qt::_ok()
{
	int curr = ch_list->currentIndex();
	if (curr >= 0)
		_save_data(curr);

	if (_num > 0)
		memcpy(*_ch_save, _ch, sizeof(struct ch_info) * _num);

	accept();
} // _ok()
