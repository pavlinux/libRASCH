/*----------------------------------------------------------------------------
 * evprop_dlg_qt.h
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002-2006, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifndef EVPROP_DLG_QT_H
#define EVPROP_DLG_QT_H

#include "evprop_dlg_ui.h"

#define _LIBRASCH_BUILD
#include <ra.h>
#include <ra_priv.h>
#include <pl_general.h>

class evprop_dlg_qt : public QDialog, public Ui::evprop_dlg_ui
{
      Q_OBJECT
public:
      evprop_dlg_qt(eval_handle eh, QWidget * parent = 0);

protected slots:
	void slot_class(int);
	void slot_prop(int);
        void slot_del_class();
        void slot_del_prop();

protected:
	void init();

	meas_handle _mh;
	eval_handle _eh;

	int _num_class;
	class_handle *_class_all;
	int _curr_class;

	int _num_prop;
	prop_handle *_prop_all;
	int _curr_prop;
};  // class evprop_dlg_qt


#endif // EVPROP_DLG_QT_H
