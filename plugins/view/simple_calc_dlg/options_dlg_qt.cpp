/*----------------------------------------------------------------------------
 * options_dlg_qt.cpp
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2003-2007, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#include <QtGui/QTextBrowser>
#include <QtGui/QComboBox>
#include <QtGui/QPushButton>
#include <QtGui/QListWidget>
#include <QtGui/QLineEdit>
#include <QtGui/QLabel>

#include "options_dlg_qt.h"

options_dlg_qt::options_dlg_qt(struct proc_info *pi, QWidget * parent/*=0*/)
	: QDialog(parent)
{
	setupUi(this);

	_pi = pi;

	if (_pi->ra == NULL)
		_pi->ra = ra_lib_handle_from_any_handle(_pi->mh);

	struct plugin_struct *ps = (struct plugin_struct *)pi->plugin;
	if (ps == NULL)
		return;

	_opts = (struct ra_option_infos *)ps->info.opt;
	if (_opts == NULL)
		return;

	for (long l = 0; l < ps->info.num_options; l++)
	{
		QString s = format_option_text(l);
		opt_list->addItem(s);
		QListWidgetItem *item = opt_list->item(l);
		item->setToolTip(_opts[l].desc);
	}

	QObject::connect(opt_list, SIGNAL(currentRowChanged(int)), this, SLOT(opt_sel(int)));
	QObject::connect(change_btn, SIGNAL(clicked()), this, SLOT(change()));
	QObject::connect(cancel_btn, SIGNAL(clicked()), this, SLOT(reject()));
	QObject::connect(ok_btn, SIGNAL(clicked()), this, SLOT(accept()));

	opt_list->setCurrentRow(0);
	opt_sel(0);
}  // constructor


QString
options_dlg_qt::format_option_text(long idx)
{
	QString s = _opts[idx].name;

	value_handle vh = ra_value_malloc();
	if (ra_lib_get_option(_pi, _opts[idx].name, vh) == 0)
	{
		s += " = ";

		QString t;
		switch (_opts[idx].type)
		{
		case RA_VALUE_TYPE_SHORT:
			t.setNum(ra_value_get_short(vh));
			s += t;
			break;
		case RA_VALUE_TYPE_LONG:
			t.setNum(ra_value_get_long(vh));
			s += t;
			break;
		case RA_VALUE_TYPE_DOUBLE:
			t.setNum(ra_value_get_double(vh));
			s += t;
			break;
		case RA_VALUE_TYPE_CHAR:
			s += QString::fromUtf8(ra_value_get_string_utf8(vh));
			break;
		}
	}

	return s;
} // format_option_text()


void
options_dlg_qt::opt_sel(int idx)
{
	opt_value->setEnabled(false);
	change_btn->setEnabled(false);

	QString s = _opts[idx].name;
	s += " (";
	int ok = 1;
	QString val;
	value_handle vh = ra_value_malloc();
	ra_lib_get_option(_pi, _opts[idx].name, vh);
	
	switch (_opts[idx].type)
	{
	case RA_VALUE_TYPE_SHORT:
		s += "short integer";
		val.setNum(ra_value_get_short(vh));
		break;
	case RA_VALUE_TYPE_LONG:
		s += "long integer";
		val.setNum(ra_value_get_long(vh));
		break;
	case RA_VALUE_TYPE_DOUBLE:
		s += "double";
		val.setNum(ra_value_get_double(vh));
		break;
	case RA_VALUE_TYPE_CHAR:
		s += "string";
		if (ra_value_get_string(vh))
			val = ra_value_get_string(vh);
		break;
	default:
		s += "not supported";
		ok = 0;
		break;
	}
	s += ")";

	ra_value_free(vh);

	opt_name->setText(s);
	opt_value->setText(val);

 	if (!ok)
 		return;

	opt_value->setEnabled(true);
	change_btn->setEnabled(true);
} // opt_sel()


void
options_dlg_qt::change()
{
	int idx = opt_list->currentRow();
	value_handle vh = ra_value_malloc();
	if (ra_lib_get_option(_pi, _opts[idx].name, vh) != 0)
	{
		ra_value_free(vh);
		return;
	}

	int ok = 1;
	switch (_opts[idx].type)
	{
	case RA_VALUE_TYPE_SHORT:
		ra_value_set_short(vh, opt_value->text().toShort());
		break;
	case RA_VALUE_TYPE_LONG:
		ra_value_set_long(vh, opt_value->text().toLong());
		break;
	case RA_VALUE_TYPE_DOUBLE:
		ra_value_set_double(vh, opt_value->text().toDouble());
		break;
	case RA_VALUE_TYPE_CHAR:
		ra_value_set_string_utf8(vh, opt_value->text().toUtf8().constData());
		break;
	default:
		ok = 0;
		break;
	}
	if (ok)
	{
		ra_lib_set_option(_pi, _opts[idx].name, vh);
		QString s = format_option_text(idx);
		QListWidgetItem *item = opt_list->takeItem(idx);
		delete item;
		opt_list->insertItem(idx, s);
		opt_list->setCurrentRow(idx);
	}

	ra_value_free(vh);
} // change()

