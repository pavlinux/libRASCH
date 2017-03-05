/*----------------------------------------------------------------------------
 * plugin_info_dlg_qt.cpp  -  
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

#include <Qt/qvariant.h>
#include <QtGui/qcheckbox.h>
#include <QtGui/qgroupbox.h>
#include <QtGui/qlabel.h>
#include <QtGui/qtreeview.h>
#include <QtGui/qpushbutton.h>
#include <QtGui/qlayout.h>
#include <QtGui/qtooltip.h>
#include <QtGui/QHeaderView>

#include "plugin_info_dlg_qt.h"

plugin_info_dlg_qt::plugin_info_dlg_qt(proc_handle proc, QWidget * parent/*=0*/)
	: QDialog(parent)
{
	setupUi(this);

	_ra = ra_lib_handle_from_any_handle(proc);

	// init plugin-list
	_table_model = new item_qt(_ra);
	plugin_list->setModel(_table_model);
	plugin_list->setShowGrid(false);
	plugin_list->setSelectionBehavior(QAbstractItemView::SelectRows);
	plugin_list->setSelectionMode(QAbstractItemView::SingleSelection);

	_sel_model = new QItemSelectionModel(_table_model);
	plugin_list->setSelectionModel(_sel_model);

	connect(_sel_model, SIGNAL(currentRowChanged(const QModelIndex &, const QModelIndex &)),
		this, SLOT(slot_plugin(const QModelIndex &, const QModelIndex &)));
	connect(close_btn, SIGNAL(clicked()), this, SLOT(accept()));
	connect(use_it, SIGNAL(toggled(bool)), this, SLOT(slot_use_plugin(bool)));

	QModelIndex idx = plugin_list->indexAt(QPoint(1,1));
	plugin_list->setCurrentIndex(idx);

	plugin_list->verticalHeader()->hide();
	plugin_list->horizontalHeader()->resizeSections(QHeaderView::Stretch);
	plugin_list->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
}  // constructor


void
plugin_info_dlg_qt::slot_plugin(const QModelIndex &index, const QModelIndex &/*prev*/)
{
	plugin_handle pl = _table_model->plugin(index.row());
	value_handle vh = ra_value_malloc();

	ra_info_get(pl, RA_INFO_PL_NAME_C, vh);
	QString s = QString(ra_value_get_string_utf8(vh));
	ra_info_get(pl, RA_INFO_PL_VERSION_C, vh);
	s += " (Version ";
	s += QString(ra_value_get_string_utf8(vh));
	s += ")";
	plugin_info_box->setTitle(s);

	ra_info_get(pl, RA_INFO_PL_DESC_C, vh);
	desc->setText(QString(gettext(ra_value_get_string_utf8(vh))));

	ra_info_get(pl, RA_INFO_PL_USE_IT_L, vh);
	if (ra_value_get_long(vh))
		use_it->setChecked(true);
	else
		use_it->setChecked(false);

	ra_info_get(pl, RA_INFO_PL_FILE_C, vh);
	path->setText(QString(ra_value_get_string_utf8(vh)));

	ra_info_get(pl, RA_INFO_PL_BUILD_TS_C, vh);
	s = gettext("plugin-build: ");
	s += QString(ra_value_get_string_utf8(vh));
	build_ts->setText(s);
}  // slot_plugin()


void
plugin_info_dlg_qt::slot_use_plugin(bool check)
{
	QModelIndex curr = plugin_list->currentIndex();
	long pl_idx = _table_model->plugin_index(curr.row());
	ra_lib_use_plugin(_ra, pl_idx, (check ? 1 : 0));
} // slot_use_plugin()
