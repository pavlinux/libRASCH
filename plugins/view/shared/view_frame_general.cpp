/*----------------------------------------------------------------------------
 * view_frame_general.cpp
 *
 *
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2003, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Header: /home/cvs_repos.bak/librasch/plugins/view/shared/view_frame_general.cpp,v 1.1 2003/07/28 12:13:25 rasch Exp $
 *--------------------------------------------------------------------------*/

#ifdef _AFXDLL			// so mfc-libs are linked before c-lib libs
#include "stdafx.h"
#endif // _AFXDLL

#include "view_frame_general.h"


view_frame_general::view_frame_general(meas_handle meas)
{
	_meas = meas;
	_block_signals = false;
} // constructor


view_frame_general::~view_frame_general()
{
} // destructor

