/*----------------------------------------------------------------------------
 * annot_comment_view_general.cpp
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

#ifdef _AFXDLL
#include "stdafx.h"
#endif // _AFXDLL

#include <stdio.h>
#include <string.h>

#include <ra_defines.h>
#include <ra_pl_comm.h>

#include "annot_comment_view_general.h"


annot_comment_view_general::annot_comment_view_general(meas_handle mh, eval_handle eh)
{
	_block_signals = false;


	_mh = mh;
	_eh = eh;
} // constructor


annot_comment_view_general::~annot_comment_view_general()
{
} // destructor


bool
annot_comment_view_general::key_press(char c)
{
	bool do_repaint = false;
	switch (c)
	{
	case 'A':  // dummy
		break;
	default:
		break;
	}

	return do_repaint;
} // key_press()

