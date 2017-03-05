/*----------------------------------------------------------------------------
 * evprop_dlg_qt.cpp  -  
 *
 * Description:
 *
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002-2006, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#include <QtGui/QGroupBox>
#include <QtGui/QLabel>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>

#include "evprop_dlg_qt.h"


evprop_dlg_qt::evprop_dlg_qt(eval_handle eh, QWidget *parent/*=0*/)
	: QDialog(parent)
{
	setupUi(this);

	_eh = eh;
	_mh = (meas_handle)(((struct eval *)eh)->meas);

	_class_all = NULL;
	_num_class = 0;
	_curr_class = -1;

	_prop_all = NULL;
	_num_prop = 0;
	_curr_prop = -1;

	init();

	QObject::connect(class_list, SIGNAL(currentRowChanged(int)), this, SLOT(slot_class(int)));
	QObject::connect(prop_list, SIGNAL(currentRowChanged(int)), this, SLOT(slot_prop(int)));
	QObject::connect(del_class_btn, SIGNAL(clicked()), this, SLOT(slot_del_class()));
	QObject::connect(del_prop_btn, SIGNAL(clicked()), this, SLOT(slot_del_prop()));
	QObject::connect(exit_btn, SIGNAL(clicked()), this, SLOT(accept()));
} // constructor


void
evprop_dlg_qt::init()
{
	class_list->clear();
	prop_list->clear();

	group->setWindowTitle("");
	desc_field->setText("");
	val_field->setText("");
	occur_field->setText("");

	if (_class_all)
	{
		delete[] _class_all;
		_class_all = NULL;
		_num_class = 0;
		_curr_class = -1;
	}
	if (_prop_all)
	{		
		delete[] _prop_all;
		_prop_all = NULL;
		_num_prop = 0;
		_curr_prop = -1;
	}

	value_handle vh = ra_value_malloc();
	if (ra_class_get(_eh, NULL, vh) != 0)
		return;
	_num_class = ra_value_get_num_elem(vh);
	if (_num_class <= 0)
	{
		ra_value_free(vh);
		return;
	}

	_class_all = new class_handle[_num_class];
	const void **p = ra_value_get_voidp_array(vh);
	int i;
	value_handle vh2 = ra_value_malloc();
	for (i = 0; i < _num_class; i++)
	{
		_class_all[i] = (class_handle)(p[i]);
		ra_info_get(_class_all[i], RA_INFO_CLASS_NAME_C, vh2);
		class_list->addItem(QString::fromUtf8(ra_value_get_string_utf8(vh2)));
	}

	class_list->setCurrentRow(0);
	slot_class(0);
	class_list->setFocus();

	ra_value_free(vh);
	ra_value_free(vh2);
} // init()


void
evprop_dlg_qt::slot_class(int idx)
{
	_curr_class = -1;

	prop_list->clear();
	group->setWindowTitle("");
	desc_field->setText("");
	val_field->setText("");
	occur_field->setText("");

	if (_prop_all)
	{		
		delete[] _prop_all;
		_prop_all = NULL;
		_num_prop = 0;
		_curr_prop = -1;
	}

	if (idx < 0)
		return;

	_curr_class = idx;

	value_handle vh = ra_value_malloc();
	if (ra_prop_get_all(_class_all[idx], vh) != 0)
	{
		ra_value_free(vh);
		return;
	}

	_num_prop = ra_value_get_num_elem(vh);
	if (_num_prop <= 0)
	{
		ra_value_free(vh);
		return;
	}

	_prop_all = new prop_handle[_num_prop];
	const void **p = ra_value_get_voidp_array(vh);
	int i;
	value_handle vh2 = ra_value_malloc();
	for (i = 0; i < _num_prop; i++)
	{
		_prop_all[i] = (prop_handle)(p[i]);
		ra_info_get(_prop_all[i], RA_INFO_PROP_NAME_C, vh2);
		prop_list->addItem(QString::fromUtf8(ra_value_get_string_utf8(vh2)));
	}

	prop_list->setCurrentRow(0);
	slot_prop(0);

	ra_value_free(vh);
	ra_value_free(vh2);
}  // slot_class()


void
evprop_dlg_qt::slot_prop(int idx)
{
	_curr_prop = -1;

	group->setWindowTitle("");
	desc_field->setText("");
	val_field->setText("");
	occur_field->setText("");

	if (idx < 0)
		return;

	_curr_prop = idx;

	value_handle vh = ra_value_malloc();
	ra_info_get(_class_all[_curr_class], RA_INFO_CLASS_EV_NUM_L, vh);
	long num_events = ra_value_get_long(vh);

	ra_info_get(_prop_all[idx], RA_INFO_PROP_NAME_C, vh);
	char t[100];
	sprintf(t, gettext("%s (%ld events)"), ra_value_get_string_utf8(vh), num_events);
	group->setTitle(QString(t));

	ra_info_get(_prop_all[idx], RA_INFO_PROP_DESC_C, vh);
	desc_field->setText(QString::fromUtf8(ra_value_get_string_utf8(vh)));
	ra_info_get(_prop_all[idx], RA_INFO_PROP_VALTYPE_L, vh);
	switch (ra_value_get_long(vh))
	{
	case RA_VALUE_TYPE_SHORT:
		val_field->setText("short");
		break;
	case RA_VALUE_TYPE_LONG:
		val_field->setText("long");
		break;
	case RA_VALUE_TYPE_DOUBLE:
		val_field->setText("double");
		break;
	case RA_VALUE_TYPE_CHAR:
		val_field->setText("string");
		break;
	case RA_VALUE_TYPE_VOIDP:
		val_field->setText("handle");
		break;
	case RA_VALUE_TYPE_SHORT_ARRAY:
		val_field->setText("short array");
		break;
	case RA_VALUE_TYPE_LONG_ARRAY:
		val_field->setText("long array");
		break;
	case RA_VALUE_TYPE_DOUBLE_ARRAY:
		val_field->setText("double array");
		break;
	case RA_VALUE_TYPE_CHAR_ARRAY:
		val_field->setText("string array");
		break;
	case RA_VALUE_TYPE_VOIDP_ARRAY:
		val_field->setText("handle array");
		break;
	case RA_VALUE_TYPE_VH_ARRAY:
		val_field->setText("libRASCH-values array");
		break;
	default:
		val_field->setText("---");
		break;
	}

	ra_value_free(vh);
} // slot_prop()


void
evprop_dlg_qt::slot_del_class()
{
	if (_curr_class == -1)
		return;

	ra_class_delete(_class_all[_curr_class]);

	init();
}  // slot_del_class()


void
evprop_dlg_qt::slot_del_prop()
{
	if (_curr_prop == -1)
		return;

	ra_prop_delete(_prop_all[_curr_prop]);

	init();
} // slot_del_prop()
