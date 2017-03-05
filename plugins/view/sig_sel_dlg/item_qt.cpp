/*----------------------------------------------------------------------------
 * item_qt.cpp  -  
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

#include <Qt/qapplication.h>

#include "item_qt.h"

#include <ra_priv.h>


item_qt::item_qt(ra_handle h, QString dir) : QAbstractTableModel()
{
	ra = h;
	num_meas = 0;

	QApplication::setOverrideCursor(Qt::WaitCursor);

	use_dir(dir);

	QApplication::restoreOverrideCursor();
} // constructor


void
item_qt::use_dir(QString dir)
{
	fn.clear();

	if (num_meas > 0)
		beginRemoveRows(QModelIndex(), 0, num_meas - 1);

	name.clear();
	forename.clear();
	dob.clear();
	id.clear();
	rec_date.clear();
	eval_date.clear();
	sig_name.clear();
	sig_type.clear();

	if (num_meas > 0)
		endRemoveRows();
	num_meas = 0;
	qApp->processEvents();

	struct ra_find_struct find;
	char t[1000];
	strcpy(t, dir.toAscii().constData());
	int len_check = strlen(t)-1;
	if ((t[len_check] == '/') || (t[len_check] == '\\'))
		t[len_check] = '\0';
	ra_find_handle fh = ra_meas_find_first(ra, t, &find);
	num_meas = 0;
	if (fh)
	{
		do
		{
			beginInsertRows(QModelIndex(), num_meas, num_meas);
			add_measurement(find.name);
			endInsertRows();
			qApp->processEvents();
		}
		while (ra_meas_find_next(fh, &find));
		ra_meas_close_find(fh);
	}
	
	reset();
} // use_dir()


void
item_qt::add_measurement(const char *meas_fn)
{
	meas_handle mh = ra_meas_open(ra, meas_fn, NULL, 1);
	if (!mh)
		return;

	value_handle vh = ra_value_malloc();

	// name, vorname, geboren, id, rec.date, (eval.date), sig.name, sig.type
	// get sig.name first
	rec_handle rh = ra_rec_get_first(mh, 0);
	QString sig_name_temp;
	if (ra_info_get(rh, RA_INFO_REC_GEN_NAME_C, vh) == 0)
		sig_name_temp = ra_value_get_string(vh);

	// if no obj-name use sig.name
	QString name_temp;
	if (ra_info_get(mh, RA_INFO_OBJ_PERSON_NAME_C, vh) == 0)
		name_temp = ra_value_get_string(vh);
	else
		name_temp = sig_name_temp;
	if (name_temp.isEmpty())
		name_temp = sig_name_temp;

	QString forename_temp;
	if (ra_info_get(mh, RA_INFO_OBJ_PERSON_FORENAME_C, vh) == 0)
		forename_temp = ra_value_get_string(vh);
	QString dob_temp;
	if (ra_info_get(mh, RA_INFO_OBJ_PERSON_BIRTHDAY_C, vh) == 0)
		dob_temp = ra_value_get_string(vh);
	QString id_temp;
	if (ra_info_get(mh, RA_INFO_OBJ_PATIENT_ID_C, vh) == 0)
		id_temp = ra_value_get_string(vh);

	QString rec_date_temp;
	if (ra_info_get(rh, RA_INFO_REC_GEN_DATE_C, vh) == 0)
		rec_date_temp = ra_value_get_string(vh);
	QString sig_type_temp;
	if (ra_info_get(rh, RA_INFO_REC_GEN_DESC_C, vh) == 0)
		sig_type_temp = ra_value_get_string(vh);

	eval_handle eh = ra_eval_get_default(mh);
	QString eval_date_temp;
	if (eh)
	{
		if (ra_info_get(eh, RA_INFO_EVAL_ADD_TS_C, vh) == 0)
			eval_date_temp = ra_value_get_string(vh);
	}

	ra_meas_close(mh);
	ra_value_free(vh);

	fn.append(meas_fn);
	name.append(name_temp);
	forename.append(forename_temp);
	dob.append(dob_temp);
	id.append(id_temp);
	rec_date.append(rec_date_temp);
	eval_date.append(eval_date_temp);
	sig_name.append(sig_name_temp);
	sig_type.append(sig_type_temp);

	num_meas++;
} // add_measurement()


QVariant
item_qt::data(const QModelIndex &index, int role/* = Qt::DisplayRole*/) const
{
	if (role != Qt::DisplayRole)
            return QVariant();

	int row = index.row();
	int col = index.column();

	if (row >= num_meas)
		return QVariant();

	switch (col)
	{
	case 0:	return name.at(row);
	case 1:	return forename.at(row);
	case 2:	return dob.at(row);	
	case 3:	return id.at(row);	
	case 4:	return rec_date.at(row);	
	case 5:	return eval_date.at(row);	
	case 6:	return sig_name.at(row);	
	case 7:	return sig_type.at(row);	
	}

	return QVariant();
} // data()



QVariant
item_qt::headerData(int section, Qt::Orientation orientation, int role/* = Qt::DisplayRole*/) const
{
	if (role != Qt::DisplayRole)
            return QVariant();

	if (orientation != Qt::Horizontal)
		return QVariant();

	switch (section)
	{
	case 0:	return QString(gettext("Name"));
	case 1:	return QString(gettext("Forename"));
	case 2:	return QString(gettext("Birthday"));	
	case 3: return QString(gettext("ID"));
	case 4: return QString(gettext("Rec.Date"));
	case 5: return QString(gettext("Eval.Date"));
	case 6: return QString(gettext("Meas.Name"));
	case 7: return QString(gettext("System"));
	}

	return QVariant();
} // headerData()


QString
item_qt::filename(int row)
{
	if (row >= num_meas)
		return NULL;

	return fn.at(row);
} // plugin()
