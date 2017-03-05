/*----------------------------------------------------------------------------
 * eval_dlg_qt.h
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002-2006, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#include <QtGui/QPushButton>
#include <QtGui/QListWidget>
#include <QtGui/QLabel>
#include <QtGui/QCheckBox>
#include <QtGui/QGroupBox>
#include <QtGui/QMessageBox>

#include "eval_dlg_qt.h"
#include "evprop_dlg_qt.h"


eval_dlg_qt::eval_dlg_qt(meas_handle mh, QWidget *parent/*=0*/)
	: QDialog(parent)
{
	setupUi(this);

	_modified = false;
	_curr_eval = -1;

	_mh = mh;

	init();

	QObject::connect(eval_list, SIGNAL(currentRowChanged(int)), this, SLOT(slot_eval(int)));
	QObject::connect(del_btn, SIGNAL(clicked()), this, SLOT(slot_del()));

	QObject::connect(cancel_btn, SIGNAL(clicked()), this, SLOT(slot_cancel()));
	QObject::connect(apply_btn, SIGNAL(clicked()), this, SLOT(slot_apply()));
	QObject::connect(ok_btn, SIGNAL(clicked()), this, SLOT(slot_ok()));

	QObject::connect(prop_btn, SIGNAL(clicked()), this, SLOT(slot_show_prop()));
//	QObject::connect(desc_field, SIGNAL(textChanged()), this, SLOT(slot_modified()));
	QObject::connect(def_eval, SIGNAL(clicked()), this, SLOT(slot_default()));
} // constructor


void
eval_dlg_qt::init()
{
	group->setWindowTitle("");
	desc_field->setText("");
	orig_field->setText("");
	def_eval->setChecked(false);
	created_field->setText("");
	modified_field->setText("");
	user_field->setText("");
	host_field->setText("");
	program_field->setText("");

	del_btn->setEnabled(false);
	prop_btn->setEnabled(false);
	_curr_eval = -1;

	value_handle vh = ra_value_malloc();
	ra_eval_get_all(_mh, vh);
	_num_eval = ra_value_get_num_elem(vh);
	_all_eval = new eval_handle[_num_eval];
	long l;
	for (l = 0; l < _num_eval; l++)
		_all_eval[l] = (eval_handle *)(ra_value_get_voidp_array(vh))[l];
	
	eval_list->clear();
	for (l = 0; l < _num_eval; l++)
	{
		ra_info_get(_all_eval[l], RA_INFO_EVAL_NAME_C, vh);
		eval_list->addItem(QString::fromUtf8(ra_value_get_string_utf8(vh)));
	}
	ra_value_free(vh);

	if (_num_eval > 0)
	{
		eval_list->setCurrentRow(0);
		slot_eval(0);
	}
} // init()


void
eval_dlg_qt::slot_eval(int idx)
{
	if (idx < 0)
		return;

	if (_modified)
	{
		QString s = QString(gettext("Settings were modified, do you want to discard changes?"));
		QMessageBox mb(QString(gettext("libRASCH - Evaluation")), s,
			       QMessageBox::Information, QMessageBox::No | QMessageBox::Default,
			       QMessageBox::Yes | QMessageBox::Escape, 0);
		if (mb.exec() == QMessageBox::No)
		{
			eval_list->setCurrentRow(_curr_eval);
			return;
		}
	}

	_curr_eval = idx;

	value_handle vh = ra_value_malloc();
	ra_info_get(_all_eval[idx], RA_INFO_EVAL_NAME_C, vh);
	group->setWindowTitle(QString::fromUtf8(ra_value_get_string_utf8(vh)));
	ra_info_get(_all_eval[idx], RA_INFO_EVAL_DESC_C, vh);
	desc_field->setText(QString::fromUtf8(ra_value_get_string_utf8(vh)));

	ra_info_get(_all_eval[idx], RA_INFO_EVAL_ORIGINAL_L, vh);
	if (ra_value_get_long(vh) == 0)
		orig_field->setText(QString(gettext("no")));
	else
		orig_field->setText(QString(gettext("yes")));

	ra_info_get(_all_eval[idx], RA_INFO_EVAL_DEFAULT_L, vh);
	if (ra_value_get_long(vh) == 0)
		def_eval->setChecked(false);
	else
		def_eval->setChecked(true);

	ra_info_get(_all_eval[idx], RA_INFO_EVAL_ADD_TS_C, vh);
	created_field->setText(ra_value_get_string(vh));
	ra_info_get(_all_eval[idx], RA_INFO_EVAL_MODIFY_TS_C, vh);
	modified_field->setText(ra_value_get_string(vh));
	ra_info_get(_all_eval[idx], RA_INFO_EVAL_USER_C, vh);
	user_field->setText(QString::fromUtf8(ra_value_get_string_utf8(vh)));
	ra_info_get(_all_eval[idx], RA_INFO_EVAL_HOST_C, vh);
	host_field->setText(QString::fromUtf8(ra_value_get_string_utf8(vh)));
	ra_info_get(_all_eval[idx], RA_INFO_EVAL_PROG_C, vh);
	program_field->setText(QString::fromUtf8(ra_value_get_string_utf8(vh)));

	ra_value_free(vh);

	_modified = false;
	del_btn->setEnabled(true);
	prop_btn->setEnabled(true);
} // slot_eval()


void
eval_dlg_qt::slot_del()
{
	if (_curr_eval == -1)
		return;

	// TODO: think about how to delete the default evaluation (hint: set youngest eval as default eval)
	if (def_eval->isChecked())
	{
		QMessageBox::information(this,QString(gettext("libRASCH - Evaluation")),
					 QString(gettext("You can not delete the default evaluation.")));
		return;
	}

	QMessageBox mb(QString(gettext("libRASCH - Evaluation")),
		       QString(gettext("Do you really want to delete the evaluation?")),
		       QMessageBox::Information,
		       QMessageBox::No | QMessageBox::Default,
		       QMessageBox::Yes | QMessageBox::Escape, 0);
	if (mb.exec() == QMessageBox::No)
		return;

	if (ra_eval_delete(_all_eval[_curr_eval]) != 0)
		fprintf(stderr, "deletion of the evaluation was not sucessful\n");
	init();
} // slot_del()


void
eval_dlg_qt::slot_cancel()
{
	if (_modified)
	{
		QMessageBox mb(QString(gettext("libRASCH - Evaluation")),
			       QString(gettext("Settings were modified, do you want to save changes?")),
			       QMessageBox::Information,
			       QMessageBox::No | QMessageBox::Escape,
			       QMessageBox::Yes | QMessageBox::Default, 0);
		if (mb.exec() == QMessageBox::Yes)
			slot_ok();
		else
			reject();
	}
	else
		reject();
} // slot_cancel()


void
eval_dlg_qt::slot_apply()
{
} // slot_apply()


void
eval_dlg_qt::slot_ok()
{
	accept();
} // slot_ok()


void
eval_dlg_qt::slot_show_prop()
{
	if (_curr_eval < 0)
		return;

	evprop_dlg_qt dlg(_all_eval[_curr_eval], this);
	dlg.exec();
} // slot_show_prop()


void
eval_dlg_qt::slot_modified()
{
	_modified = true;
	apply_btn->setEnabled(true);
} // slot_modified()


void
eval_dlg_qt::slot_default()
{
	if (_curr_eval < 0)
		return;

	// is checkbox NOW (after user click) checked?
	if (def_eval->isChecked())
		ra_eval_set_default(_all_eval[_curr_eval]);
	else
	{
		// no; this means it was checked before -> this is the default evaluation
		QMessageBox::information(this,QString(gettext("libRASCH - Evaluations")),
					 QString(gettext("You can't deselect default evalution.\n"
								   "Please choose evaluation which you want to be the\n"
								   "default-evalution and check there the default-box."))); 
		def_eval->setChecked(true); // set it back to checked
	}
} // slot_default()
