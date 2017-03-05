/**
 * \file ch_sel_dlg_qt.cpp
 *
 * Qt specific code for the channel select dialog.
 *
 * \author Raphael Schneider (librasch@gmail.com)
 */

/*--------------------------------------------------------------------------
 *
 * Copyright (C) 2002-2007, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *
 *--------------------------------------------------------------------------*/

#include <QtGui/QLabel>
#include <QtGui/QCheckBox>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QListWidgetItem>

#include <ra_ch_list.h>

#include "ch_sel_dlg_qt.h"

ch_sel_dlg_qt::ch_sel_dlg_qt(struct proc_info *pi, QWidget * parent/*=0*/)
	: QDialog(parent)
{
	setupUi(this);

	_opt = (struct ra_ch_sel *)pi->options;

	_pi = pi;

	value_handle vh = ra_value_malloc();
	if (ra_info_get(pi->rh, RA_INFO_REC_GEN_NUM_CHANNEL_L, vh) != 0)
	{
		ra_value_free(vh);
		return;
	}

	long num_ch = ra_value_get_long(vh);
	for (long l = 0; l < num_ch; l++)
	{
		ra_value_set_number(vh, l);
		if (ra_info_get(pi->rh, RA_INFO_REC_CH_TYPE_L, vh) != 0)
			continue;
		long ch_type = ra_value_get_long(vh);

		if (ra_info_get(pi->rh, RA_INFO_REC_CH_NAME_C, vh) != 0)
			continue;

		bool skip = (_opt->num_ch > 0 ? true : false);
		for (long m = 0; m < _opt->num_ch; m++)
		{
			if (_opt->ch_type_filter[m] == ch_type)
			{
				skip = false;
				break;
			}
		}
		if (skip)
			continue;
		
		QString s = QString::fromUtf8(ra_value_get_string_utf8(vh));
		my_list_item *i = new my_list_item(s, l);
		all_ch_list->addItem(i);
	}
	ra_value_free(vh);

	all_ch_sel(-1);
	use_ch_sel(-1);

	QObject::connect(all_ch_list, SIGNAL(itemDoubleClicked(QListWidgetItem *)),
			 this, SLOT(ch_use(QListWidgetItem *)));
	QObject::connect(use_btn, SIGNAL(clicked()), this, SLOT(ch_use()));
	QObject::connect(use_ch_list, SIGNAL(itemDoubleClicked(QListWidgetItem *)),
			 this, SLOT(ch_dont_use(QListWidgetItem *)));
	QObject::connect(dont_use_btn, SIGNAL(clicked()), this, SLOT(ch_dont_use()));

	QObject::connect(all_ch_list, SIGNAL(currentRowChanged(int)), this, SLOT(all_ch_sel(int)));
	QObject::connect(use_ch_list, SIGNAL(currentRowChanged(int)), this, SLOT(use_ch_sel(int)));

	QObject::connect(ch_up_btn, SIGNAL(clicked()), this, SLOT(ch_up()));
	QObject::connect(ch_down_btn, SIGNAL(clicked()), this, SLOT(ch_down()));
	QObject::connect(cancel_btn, SIGNAL(clicked()), this, SLOT(reject()));
	QObject::connect(ok_btn, SIGNAL(clicked()), this, SLOT(ok()));
} // constructor


void
ch_sel_dlg_qt::ch_use(QListWidgetItem * /*item*/)
{
	use_ch_list->addItem(all_ch_list->takeItem(all_ch_list->currentRow()));
	all_ch_list->clearSelection();
} // ch_use(QListBoxItem *)


void
ch_sel_dlg_qt::ch_use()
{
	if (_curr_sel_all_ch < 0)
		return;

	ch_use(all_ch_list->item(_curr_sel_all_ch));
} // ch_use()


void
ch_sel_dlg_qt::ch_dont_use(QListWidgetItem *item)
{
	my_list_item *item_use = (my_list_item *)item;

	int idx = (int)all_ch_list->count();
	for (int i = 0; i < (int)all_ch_list->count(); i++)
	{
		my_list_item *curr = (my_list_item *)all_ch_list->item(i);
		if (curr->ch() > item_use->ch())
		{
			idx = i;
			break;
		}
	}

	all_ch_list->insertItem(idx, use_ch_list->takeItem(use_ch_list->currentRow()));

	use_ch_list->clearSelection();
} // ch_dont_use(QListBoxItem *)


void
ch_sel_dlg_qt::ch_dont_use()
{
	if (_curr_sel_use_ch < 0)
		return;

	ch_dont_use(use_ch_list->item(_curr_sel_use_ch));
} // ch_dont_use()


void
ch_sel_dlg_qt::all_ch_sel(int idx)
{
	if (idx < 0)
		use_btn->setEnabled(false);
	else
		use_btn->setEnabled(true);

	_curr_sel_all_ch = idx;
} // all_ch_sel()


void
ch_sel_dlg_qt::use_ch_sel(int idx)
{
	if (idx < 0)
	{
		ch_up_btn->setEnabled(false);
		ch_down_btn->setEnabled(false);

		dont_use_btn->setEnabled(false);
	}
	else
	{
		if (idx == 0)
			ch_up_btn->setEnabled(false);
		else
			ch_up_btn->setEnabled(true);

		if (idx == ((int) use_ch_list->count() - 1))
			ch_down_btn->setEnabled(false);
		else
			ch_down_btn->setEnabled(true);

		dont_use_btn->setEnabled(true);
	}

	_curr_sel_use_ch = idx;
} // use_ch_sel()


void
ch_sel_dlg_qt::ch_up()
{
	if ((_curr_sel_use_ch < 0) || (_curr_sel_use_ch == 0))
		return;
	
	int row = use_ch_list->currentRow();
	use_ch_list->insertItem(row-1, use_ch_list->takeItem(row));
	use_ch_list->setCurrentRow(row-1);
} // ch_up()


void
ch_sel_dlg_qt::ch_down()
{
	if ((_curr_sel_use_ch < 0) || (_curr_sel_use_ch == ((int)use_ch_list->count() - 1)))
		return;

	int row = use_ch_list->currentRow();
	use_ch_list->insertItem(row+1, use_ch_list->takeItem(row));
	use_ch_list->setCurrentRow(row+1);
} // ch_down()


void
ch_sel_dlg_qt::ok()
{
	_pi->num_results = 1;
	if (use_ch_list->count() <= 0)
	{
		accept();
		return;
	}

	long *ch = new long[use_ch_list->count()];
	for (int i = 0; i < use_ch_list->count(); i++)
	{
		my_list_item *item = (my_list_item *)use_ch_list->item(i);
		ch[i] = item->ch();
	}

	value_handle *res = (value_handle *)_pi->results;
	ra_value_set_long_array(res[0], ch, use_ch_list->count());

	delete[] ch;

	accept();
} // ok()
