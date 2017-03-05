/*----------------------------------------------------------------------------
 * view_frame_general.h
 *
 *
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2003-2004, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Header: /home/cvs_repos.bak/librasch/plugins/view/shared/view_frame_general.h,v 1.2 2004/01/06 12:46:35 rasch Exp $
 *--------------------------------------------------------------------------*/

#ifndef VIEW_FRAME_GENERAL_H
#define VIEW_FRAME_GENERAL_H

#include <stdlib.h>

#define _LIBRASCH_BUILD
#include <ra_defines.h>
#include <ra_pl_comm.h>

class view_frame_general
{
public:
	view_frame_general(meas_handle meas);
	virtual ~view_frame_general();

	virtual void init_scrollbar(long , long , double ) { ; }
	virtual void set_scrollbar_pos(long ) { ; }

	bool block_signals() { return _block_signals; }

protected:
	bool _block_signals;
	meas_handle _meas;

	long _page_width;
	double _max_xscale;
}; // class view_frame_general


#endif // VIEW_FRAME_GENERAL_H
