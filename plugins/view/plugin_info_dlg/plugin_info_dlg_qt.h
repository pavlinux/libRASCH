/*----------------------------------------------------------------------------
 * plugin_info_dlg_qt.h  -  
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

#ifndef PLUGIN_INFO_DLG_QT_H
#define PLUGIN_INFO_DLG_QT_H

#include "plugin_info_dlg_ui.h"

#define _LIBRASCH_BUILD
#include <ra.h>
#include <ra_priv.h>
#include <pl_general.h>

#include "item_qt.h"


class plugin_info_dlg_qt : public QDialog, public Ui::plugin_info_dlg
{
      Q_OBJECT

 public:
      plugin_info_dlg_qt(proc_handle proc, QWidget * parent = 0);

 public slots:
	void slot_plugin(const QModelIndex &index, const QModelIndex &prev);
        void slot_use_plugin(bool check);

 protected:
        ra_handle _ra;
	item_qt *_table_model;
	QItemSelectionModel *_sel_model;
}; // class plugin_info_dlg_qt

#endif  // PLUGIN_INFO_DLG_QT_H
