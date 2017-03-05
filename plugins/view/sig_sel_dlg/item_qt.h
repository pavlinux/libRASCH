/*----------------------------------------------------------------------------
 * item_qt.h  -  
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

#ifndef ITEM_QT_H
#define ITEM_QT_H

#include <QtCore/QAbstractTableModel>

#include <ra.h>


class item_qt : public QAbstractTableModel
{
 public:
	item_qt(ra_handle h, QString dir);

	virtual int rowCount(const QModelIndex &/*parent = QModelIndex()*/) const { return num_meas; }
	virtual int columnCount(const QModelIndex &/*parent = QModelIndex()*/) const { return 8; }
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	void use_dir(QString dir);

	QString filename(int row);

 private:
	void add_measurement(const char *meas_fn);


	ra_handle ra;
	long num_meas;

	QList<QString> fn;

	QList<QString> name;
	QList<QString> forename;
	QList<QString> dob;
	QList<QString> id;
	QList<QString> rec_date;
	QList<QString> eval_date;
	QList<QString> sig_name;
	QList<QString> sig_type;
};  // class item_qt

#endif // ITEM_QT_H
