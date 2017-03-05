/***************************************************************************
                          detect_progress.cpp  -  description
                             -------------------
    begin                : Sat Sep 29 2001
    copyright            : (C) 2001 by Raphael Schneider
    email                : rasch@med1.med.tu-muenchen.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "detect_progress_dlg.h"
#include <QtGui/qprogressbar.h>
#include <QtGui/qlabel.h>


detect_progress_dlg::detect_progress_dlg(QWidget *parent/*=0*/) : QDialog(parent)
{
	setupUi(this);
}

detect_progress_dlg::~detect_progress_dlg()
{
}

void
detect_progress_dlg::set_text(QString s)
{
	text->setText(QString::fromUtf8(s));
}  // set_text()


void
detect_progress_dlg::set_progress(int value)
{
	progress_bar->setValue(value);
}  // set_progress()
