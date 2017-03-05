/*----------------------------------------------------------------------------
 * annot_comment_view_general.h
 *
 *
 *
 * Author(s): Raphael Schneider (librasch@gmail.com)
 *
 * Copyright (C) 2008-2009, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifndef ANNOT_COMMENT_VIEW_GENERAL_H
#define ANNOT_COMMENT_VIEW_GENERAL_H

#define _LIBRASCH_BUILD
#include <ra_defines.h>


class annot_comment_view_general
{
 public:
	annot_comment_view_general(meas_handle mh, eval_handle eh);
	virtual ~annot_comment_view_general();

	bool block_signals() { return _block_signals; }

	virtual void eval_change() { ; }

  	virtual void unset_cursor() { ; }
	virtual void set_busy_cursor() { ; }

 protected:
	meas_handle _mh;
	eval_handle _eh;

	bool _block_signals;


	bool key_press(char c);
}; // class annot_comment_view_general


#endif // ANNOT_COMMENT_VIEW_GENERAL_H
