/***************************************************************************
                          signal_ra.cpp  -  description
                             -------------------
    begin                : Mon Nov 22 2001
    copyright            : (C) 2001-2004 by Raphael Schneider
    email                : rasch@med1.med.tum.de

    $Header: /home/cvs_repos.bak/librasch/raschlab/general/signal_ra.cpp,v 1.4 2004/04/26 20:00:02 rasch Exp $
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <stdio.h>
#include <string.h>

#include "signal_ra.h"


signal_ra::signal_ra(ra_handle m)
{
	_ra = m;
	_v_list = NULL;
	_num_view = 0;
	_num_plot_view = 0;
}  // constructor


signal_ra::~signal_ra()
{
	close();
}  // destructor


bool
signal_ra::open(const char *fn)
{
	strncpy(_fn, fn, MAX_PATH_RA);
	
	char t[MAX_PATH_RA];
	strncpy(t, fn, MAX_PATH_RA);
	_mh = ra_meas_open(_ra, t, NULL, 0);
	if (!_mh)
		return false;
	
	_eh = ra_eval_get_default(_mh);

	// create base title		
	value_handle vh = ra_value_malloc();
	_title[0] = '\0';
	if (ra_info_get(_mh, RA_INFO_OBJ_PERSON_NAME_C, vh) == 0)
		strncpy(_title, ra_value_get_string(vh), 200);
	if (ra_info_get(_mh, RA_INFO_OBJ_PERSON_FORENAME_C, vh) == 0)
	{
		if (!_title[0] == '\0')
			strcat(_title, ", ");
		strcat(_title, ra_value_get_string(vh));
	}
	rec_handle rh = ra_rec_get_first(_mh, 0);
	if (ra_info_get(rh, RA_INFO_REC_GEN_NAME_C, vh) == 0)
	{
		char *t = new char[strlen(ra_value_get_string(vh)) + 3];
		sprintf(t, "(%s)", ra_value_get_string(vh));
		strcat(_title, t);
		delete[] t;
	}
	ra_value_free(vh);
	
	return true;
}  // open()


void
signal_ra::close()
{
	if (_v_list == NULL)
		return;

	for (int i = 0; i < _num_view; i++)
		close_view(i);
	
	delete[] _v_list;
	_v_list = NULL;
	delete[] _v_handle;
	_v_handle = NULL;
	delete[] _v_type;
	_v_type = NULL;

	_num_view = 0;

	ra_meas_close(_mh);
}  // close


void
signal_ra::save(int use_ascii)
{
	if (!_mh)
		return;

	ra_eval_save(_mh, NULL, use_ascii);
}  // save()


void
signal_ra::file_name(char *buf, int buf_size)
{
	strncpy(buf, _fn, buf_size);
}  // file_name()


void
signal_ra::add_view(void *view, view_handle vih, int type)
{
	if (_v_list == NULL)
	{
		_v_list = new void *[1];
		_v_list[0] = view;

		_v_handle = new view_handle[1];
		_v_handle[0] = vih;

		_v_type = new int[1];
		_v_type[0] = type;
	}
	else
	{
		void **t = new void *[_num_view+1];
		memcpy(t, _v_list, sizeof(void *)*_num_view);
		delete[] _v_list;
		t[_num_view] = view;
		_v_list = t;

		view_handle *v = new view_handle[_num_view+1];
		memcpy(v, _v_handle, sizeof(view_handle)*_num_view);
		delete[] _v_handle;
		v[_num_view] = vih;
		_v_handle = v;

		int *n = new int[_num_view+1];
		memcpy(n, _v_type, sizeof(int)*_num_view);
		delete[] _v_type;
		n[_num_view] = type;
		_v_type = n;
	}
	_num_view++;

	if (type == PLOT_VIEW)
		_num_plot_view++;
}  // add_view()


void
signal_ra::remove_view(void *view, bool destroy/*=true*/, void *dest/*=NULL*/)
{
	int idx;
	if ((idx = find_view(view)) < 0)
		return;
		
	dest_handle dh;
	if (dest)
		dh = dest;
	else
		dh = get_dest_handle(idx);
	ra_comm_del(_mh, dh, NULL);

	if (destroy)
	{		
		close_view(idx);
		ra_view_free(_v_handle[idx]);
	}
	
	if ((_num_view > 1) && (idx < (_num_view-1)))
	{
		memmove(&(_v_list[idx]), &(_v_list[idx+1]),
			sizeof(_v_list[0])*(_num_view - idx - 1));
		memmove(&(_v_handle[idx]), &(_v_handle[idx+1]),
			sizeof(_v_handle[0])*(_num_view - idx - 1));
		memmove(&(_v_type[idx]), &(_v_type[idx+1]),
			sizeof(_v_type[0])*(_num_view - idx - 1));
	}
	
	_num_view--;
	void **t = NULL;
	view_handle *v = NULL;
	int *n = NULL;
	if (_num_view > 0)
	{
		t = new void *[_num_view];
		memcpy(t, _v_list, sizeof(_v_list[0]) * _num_view);
		v = new view_handle[_num_view];
		memcpy(v, _v_handle, sizeof(_v_handle[0]) * _num_view);
		n = new int[_num_view];
		memcpy(n, _v_type, sizeof(_v_type[0]) * _num_view);
	}
	delete[] _v_list;
	_v_list = t;
	delete[] _v_handle;
	_v_handle = v;
	delete[] _v_type;
	_v_type = n;
}  // remove_view()


void
signal_ra::update_all_views(void *sender)
{
	for (int i = 0; i < _num_view; i++)
	{
		if (_v_list[i] == sender)
			continue;
		
		update_view(i);
	}
}  // update_all_views()


void *
signal_ra::get_view(void *prev)
{
	if (_v_list == NULL)
		return NULL;

	void *ret = NULL;
	if (prev == NULL)
		ret = _v_list[0];
	else
	{
		bool use_next = false;
		for (int i = 0; i < _num_view; i++)
		{
			if (_v_list[i] == prev)
			{
				use_next = true;
				continue;
			}
			if (use_next)
			{
				ret = _v_list[i];
				break;
			}
		}
	}

	return ret;
}  // get_view()


int
signal_ra::find_view(void *view)
{
	if (_v_list == NULL)
		return -1;

	int index = -1;
	for (int i = 0; i < _num_view; i++)
	{
		if (_v_list[i] == view)
		{
			index = i;
			break;
		}
	}

	return index;
}  // find_view()


int
signal_ra::num_views()
{
	if (_v_list == NULL)
		return 0;

	return _num_view;
}  // num_views()


void
signal_ra::close_view(int )
{
	// do nothing; virtual function implemented in GUI specific class
}  // close_view()


dest_handle
signal_ra::get_dest_handle(int )
{
	// do nothing; virtual function implemented in GUI specific class
	return NULL;
}  // get_dest_handle()


void
signal_ra::update_view(int )
{
	// do nothing; virtual function implemented in GUI specific class
}  // update_view()



void
signal_ra::set_view_type(void *view, int type)
{
	int idx = find_view(view);
	if (idx == -1)
		return;
	
	_v_type[idx] = type;
}  // set_view_type()


int
signal_ra::get_view_type(void *view)
{
	int idx = find_view(view);
	if (idx == -1)
		return UNKNOWN_VIEW;

	return _v_type[idx];
}  // get_view_type()



/* win32 includes
#include "stdafx.h"
#include "ChildFrm.h"
*/

/* close_view for win32			
			CChildFrame *v = (CChildFrame *)(_v_list[i]);
			v->DestroyWindow();
*/

/* update_view for win32
		CChildFrame *f = (CChildFrame *)_v_list[i];
		f->Invalidate();
*/
/* get_dest_handle for win32
	CChildFrame *v = (CChildFrame *)(_v_list[0]);
*/
