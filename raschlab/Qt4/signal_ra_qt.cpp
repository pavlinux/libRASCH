/*
 * signal_ra_qt.cpp  - 
 *
 *
 * Copyright 2001-2007 by Raphael Schneider <librasch@gmail.com>
 *
 * $Id: $
 */

#include <QtGui/qwidget.h>

#include "signal_ra_qt.h"


signal_ra_qt::~signal_ra_qt()
{
	close();
}  // destructor


void
signal_ra_qt::close_view(int index)
{
	if (!v_list || (index < 0) || (index >= num_view))
		return;
		
	QWidget *w = (QWidget *)v_list[index];
	w->setParent(0); // remove window from workspace
	w->close();
}  // close_view()


dest_handle
signal_ra_qt::get_dest_handle(int index)
{
	if (!v_list || (index < 0) || (index >= num_view))
		return NULL;
		
	return (dest_handle)v_list[index];
}  // get_dest_handle()


void
signal_ra_qt::update_view(int index)
{
	if (!v_list || (index < 0) || (index >= num_view))
		return;
	
	QWidget *w = (QWidget *)v_list[index];
	w->repaint();
}  // update_view()
