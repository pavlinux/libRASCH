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


item_qt::item_qt(ra_handle ra) : QAbstractTableModel()
{
	_ra = ra;

	value_handle vh = ra_value_malloc();
	ra_info_get(_ra, RA_INFO_NUM_PLUGINS_L, vh);
	_num_plugins = ra_value_get_long(vh);
	
	for (long l = 0; l < _num_plugins; l++)
	{
		_plugin_index.append(l);

		plugin_handle pl = ra_plugin_get_by_num(_ra, l, 1);
		if (ra_info_get(pl, RA_INFO_PL_NAME_C, vh) != 0)
			continue;
		_plugins.append(pl);
		_names.append(ra_value_get_string(vh));

		ra_info_get(pl, RA_INFO_PL_TYPE_L, vh);
		QString s;
		switch (ra_value_get_long(vh))
		{
		case PLUGIN_ACCESS:
			s += "access ";
			break;
		case PLUGIN_PROCESS:
			s += "process ";
			break;
		case PLUGIN_GUI:
			s += "GUI ";
			break;
		case PLUGIN_VIEW:
			s += "view ";
			break;
		default:
			s = "???";
			break;
		}
		_types.append(s);
	
		ra_info_get(pl, RA_INFO_PL_LICENSE_L, vh);
		switch (ra_value_get_long(vh))
		{
		case LICENSE_GPL:
			s = "GPL";
			break;
		case LICENSE_LGPL:
			s = "LGPL";
			break;
		case LICENSE_PROPRIETARY:
			s = "proprietary";
			break;
		case LICENSE_NOT_CHOOSEN:
			s = "not choosen";
			break;
		default:
			s = "???";
			break;
		}
		_licenses.append(s);
	}

	ra_value_free(vh);
}

QVariant
item_qt::data(const QModelIndex &index, int role/* = Qt::DisplayRole*/) const
{
	if (role != Qt::DisplayRole)
            return QVariant();

	int row = index.row();
	int col = index.column();

	if (row >= _num_plugins)
		return QVariant();

	switch (col)
	{
	case 0:	return _names.at(row);
	case 1:	return _types.at(row);
	case 2:	return _licenses.at(row);	
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
	case 0:	return QString("Name");
	case 1:	return QString("Type");
	case 2:	return QString("License");	
	}

	return QVariant();
} // headerData()


plugin_handle
item_qt::plugin(int row)
{
	if (row >= _num_plugins)
		return NULL;

	return _plugins.at(row);
} // plugin()


long
item_qt::plugin_index(int row)
{
	if (row >= _num_plugins)
		return -1;

	return _plugin_index.at(row);
} // plugin_index()
