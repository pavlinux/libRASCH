/***************************************************************************
                          signal_ra_qt.cpp  -  description
                             -------------------
    begin                : Thu Jan 10 2002
    copyright            : (C) 2002 by Raphael Schneider
    email                : rasch@med1.med.tum.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/qwidget.h>

#include "signal_ra_qt.h"


void
signal_ra_qt::close_view(int index)
{
	if (!_v_list || (index < 0) || (index >= _num_view))
		return;
		
	QWidget *w = (QWidget *)_v_list[index];
	w->close();
}  // close_view()


dest_handle
signal_ra_qt::get_dest_handle(int index)
{
	if (!_v_list || (index < 0) || (index >= _num_view))
		return NULL;
		
	return (dest_handle)_v_list[index];
}  // get_dest_handle()


void
signal_ra_qt::update_view(int index)
{
	if (!_v_list || (index < 0) || (index >= _num_view))
		return;
	
	QWidget *w = (QWidget *)_v_list[index];
	w->repaint();
}  // update_view()
