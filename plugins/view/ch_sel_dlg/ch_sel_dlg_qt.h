/**
 * \file ch_sel_dlg_qt.h
 *
 * \author Raphael Schneider (librasch@gmail.com)
 */

/*--------------------------------------------------------------------------
 *
 * Copyright (C) 2002-2007, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *
 *--------------------------------------------------------------------------*/

#ifndef CH_SEL_DLG_QT_H
#define CH_SEL_DLG_QT_H

#define _LIBRASCH_BUILD
#include <ra.h>
#include <ra_priv.h>
#include <pl_general.h>

#include <ra_ch_sel.h>

#include "ch_sel_dlg_ui.h"


class my_list_item : public QListWidgetItem
{
 public:
	my_list_item(QString s, long ch) : QListWidgetItem(s) { ch_val = ch; }
	int ch() { return ch_val; }
 protected:
	int ch_val;
}; // class my_list_item


class ch_sel_dlg_qt : public QDialog, public Ui::ch_sel_dlg
{
      Q_OBJECT
 public:
      ch_sel_dlg_qt(struct proc_info *pi, QWidget * parent = 0);

 public slots:
      void ch_use(QListWidgetItem *item);
      void ch_use();
      void ch_dont_use(QListWidgetItem * item);
      void ch_dont_use();

      void all_ch_sel(int idx);
      void use_ch_sel(int idx);
      
      void ch_up();
      void ch_down();
      
      void ok();
      
 protected:
      struct proc_info *_pi;
      struct ra_ch_sel *_opt;
      int _curr_sel_use_ch;
      int _curr_sel_all_ch;
}; // class ch_sel_dlg_qt


#endif // CH_SEL_DLG_QT_H
