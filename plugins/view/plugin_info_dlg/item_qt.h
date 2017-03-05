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

#ifndef _ITEM_QT_H
#define _ITEM_QT_H

#include <QtCore/QAbstractTableModel>

#include <ra.h>


class item_qt : public QAbstractTableModel
{
 public:
	item_qt(ra_handle ra);

	virtual int rowCount(const QModelIndex &/*parent = QModelIndex()*/) const { return _num_plugins; }
	virtual int columnCount(const QModelIndex &/*parent = QModelIndex()*/) const { return 3; }
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	plugin_handle plugin(int row);
	long plugin_index(int row);

 private:
	ra_handle _ra;
	long _num_plugins;

	QList<plugin_handle> _plugins;
	QList<long> _plugin_index;
	QList<QString> _names;
	QList<QString> _types;
	QList<QString> _licenses;
};  // class item_qt

#endif // _ITEM_QT_H
