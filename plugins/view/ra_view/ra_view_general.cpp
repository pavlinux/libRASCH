/*----------------------------------------------------------------------------
 * ra_view_general.cpp
 *
 * 
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002-2004, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Header: /home/cvs_repos.bak/librasch/plugins/view/ra_view/ra_view_general.cpp,v 1.8 2004/08/03 14:34:31 rasch Exp $
 *--------------------------------------------------------------------------*/

#ifdef _AFXDLL
#include "stdafx.h"
#endif // _AFXDLL

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <list>

#define _LIBRASCH_BUILD
#include <ra.h>
#include <ra_priv.h>
#include <pl_general.h>

#include "ra_view_general.h"

using namespace std;


/* 
 * view-plugin must be able to check if it can handle channels from current
 measurement
 check-function must return following infos:
 - which channels can be handled
 - height and width for every channel

 * view-plugin must be able to check if a plugin-name is a "low-level" plugin
 used by this plugin (e.g. ecg-plugin is using timeseries-plugin)

 * init function will do
 - check all view-plugins and store for every channel the infos returned
 from view-plugin if it can handle the channel, the plugin-handle and
 the plugin-name
 - if a channel is already assigned to a plugin, check if it is a "high-level"
 plugin (e.g. the ecg-plugin is "higher" than the timeseries-plugin)
 (up to now, it is not allowed to handle a channel in their own way if there is
 already a plugin handling this type of channel)
 - after all channels are checked assign every channel a place on the whole
 view and tell plugins the position of the left/upper position

 * every mouse-click will be forwarded to the plugin in which channel the click was done

 * every key-press will be forwarded to the plugin in which channel the cursor is

 * a repaint-signal from a plugin will be forwarded to the remaining plugins
 */


ra_view_general::ra_view_general(rec_handle h)
{
	_ra = _meas = _rec = _pl = NULL;
	_num_ch = _num_view = 0;
	_ch_info = NULL;
	_view_info = NULL;
	_view_with_focus = -1;
	_block_signals = false;

	if (h != NULL)
	{
		_rec = h;
		_meas = ra_meas_handle_from_any_handle(_rec);
		_ra = ra_lib_handle_from_any_handle(_meas);
	}
}				// constructor

ra_view_general::~ra_view_general()
{
	if (_ch_info)
		delete[]_ch_info;
	if (_view_info)
	{
		for (int i = 0; i < _num_view; i++)
		{
			if (_view_info[i].ch)
				delete[]_view_info[i].ch;
		}
		delete[]_view_info;
	}
}				// destructor


void
ra_view_general::init()
{
	int ret;

	if ((ret = init_general()) != 0)
		return;		// FIXME: error handling

	if ((ret = check_modules()) != 0)
		return;		// FIXME: error handling

#ifndef _AFXDLL			// routines must be called in OnCreateClient()-function
	if ((ret = init_views()) != 0)
		return;		// FIXME: error handling
#endif // _AFXDLL
}				// init()


int
ra_view_general::init_general()
{
	// get number of channels
	value_handle vh = ra_value_malloc();
	if (ra_info_get(_rec, RA_INFO_REC_GEN_NUM_CHANNEL_L, vh) != 0)
	{
		ra_value_free(vh);
		return -1;
	}
	_num_ch = ra_value_get_long(vh);

	// get general-type of recording
	if (ra_info_get(_rec, RA_INFO_REC_GEN_TYPE_L, vh) != 0)
	{
		ra_value_free(vh);
		return -1;
	}
	_rec_type = ra_value_get_long(vh);

	ra_value_free(vh);

	return 0;
} // init_general()


int
ra_view_general::check_modules()
{
	if (_ch_info)
	{
		delete[]_ch_info;
		_ch_info = NULL;
	}
	_ch_info = new struct ch_info[_num_ch];
	memset(_ch_info, 0, sizeof(struct ch_info) * _num_ch);

	// check every viewing-module if it can handle some (or all) of the recording channels
	value_handle vh = ra_value_malloc();
	ra_info_get(_ra, RA_INFO_NUM_PLUGINS_L, vh);
	long n = ra_value_get_long(vh);
	for (long i = 0; i < n; i++)
	{
		struct plugin_struct *pl = (struct plugin_struct *) ra_plugin_get_by_num(_ra, i, 0);
		if (!(pl->view.check_channels) || !(pl->view.check_plugin_usage))
			continue;

		ra_info_get(pl, RA_INFO_PL_TYPE_L, vh);
		if (!(ra_value_get_long(vh) & PLUGIN_VIEW))
			continue;

		ra_info_get(pl, RA_INFO_PL_USE_IT_L, vh);
		if (!(ra_value_get_long(vh)))
			continue;

		for (int j = 0; j < _num_ch; j++)
		{
			if (pl->view.check_channels(_rec, j) != 0)
				continue;

			if (_ch_info[j].pl != NULL)
			{
				if (pl->view.check_plugin_usage(_ch_info[j].plugin_name) != 0)
					continue;
			}

			ra_info_get(pl, RA_INFO_PL_NAME_C, vh);
			strncpy(_ch_info[j].plugin_name, ra_value_get_string(vh), RA_VALUE_NAME_MAX);
			_ch_info[j].pl = pl;
		}
	}
	ra_value_free(vh);

	return 0;
} // check_modules()


int
ra_view_general::init_views()
{
	int ret = 0;

	// combine channels which use the same plugin
	if ((ret = combine_channels()) != 0)
		return -1;

	// create "container"-view (call gui-specific virtual function)
	void *v = create_main_view();
	if (v == NULL)
		return -1;
	// for every single channel call corresponding plugin and create view
	for (int i = 0; i < _num_view; i++)
		create_single_view(v, i);

	if (_num_view == 1)
		_view_with_focus = 0;

	return 0;
} // init_views()


int
ra_view_general::combine_channels()
{
	// create _num_ch view_infos so memory handling is easier and
	// unused memory is not so much
	_view_info = new struct general_view_info[_num_ch];
	memset(_view_info, 0, sizeof(struct general_view_info) * _num_ch);
	for (int i = 0; i < _num_ch; i++)
	{
		bool found = false;
		int pl_idx = -1;
		for (int j = 0; j < _num_view; j++)
		{
			if (_view_info[j].pl == _ch_info[i].pl)
			{
				found = true;
				pl_idx = j;
			}
		}
		if (!found)
		{
			pl_idx = _num_view;
			_num_view++;
			_view_info[pl_idx].pl = _ch_info[i].pl;

		}
		if (pl_idx == -1)
			assert(0);

		if (_view_info[pl_idx].ch == NULL)
			_view_info[pl_idx].ch = new int[_num_ch];
		_view_info[pl_idx].num_ch++;
		_view_info[pl_idx].ch[_view_info[pl_idx].num_ch - 1] = i;
	}

	return 0;
} // combine_channels()


int
ra_view_general::create_single_view(void *parent, int view)
{
	struct view_info *vi = new struct view_info;
	memset(vi, 0, sizeof(struct view_info));

	vi->handle_id = RA_HANDLE_VIEW;
	vi->parent = parent;
	vi->ra = _ra;
	vi->mh = _meas;
	vi->rh = _rec;
	vi->plugin = _view_info[view].pl;
	vi->num_ch = _view_info[view].num_ch;
	strcpy(vi->name, "ra-sub-view");
	if (vi->num_ch > 0)
	{
		vi->ch = new int[vi->num_ch];
		for (int i = 0; i < vi->num_ch; i++)
			vi->ch[i] = _view_info[view].ch[i];
	}

	if (ra_view_create(vi) == 0)
	{
		if (vi->real_views && (vi->real_views[0]))
			_view_info[view].view_handle = vi->real_views[0];
		else
			_view_info[view].view_handle = vi->views[0];
		add_single_view(_view_info[view].view_handle);
	}
	else
	{
		if (vi->views)
			free(vi->views);
		if (vi->ch)
			delete[] vi->ch;
		delete vi;
		return -1;
	}

	long width_main, height_main;
	get_view_dimensions(parent, &width_main, &height_main);

	long width, height;
	get_view_dimensions(_view_info[view].view_handle, &width, &height);
	_view_info[view].width = width;
	_view_info[view].height = height;
	_view_info[view].width_percent = (double) width / (double) width_main;
	_view_info[view].height_percent = (double) height / (double) height_main;
	_view_info[view].x = 0;	// up to now no handling of channels with different width
	_view_info[view].y = 0;

	pos_view(_view_info[view].view_handle, _view_info[view].x, _view_info[view].y);

	if (vi->views)
		free(vi->views);
	if (vi->ch)
		delete[] vi->ch;
	delete vi;

	return 0;
} // create_single_view()


void *
ra_view_general::create_main_view()
{
	return NULL;		// gui-specific function must be called
} // create_main_view()


int
ra_view_general::add_single_view(void *)
{
	return -1;		// gui-specific function must be called
} // create_main_view()


int
ra_view_general::get_view_dimensions(void * /*vh */ , long * /*width */ ,
				      long * /*height */ )
{
	return -1;		// gui-specific function must be called
} // get_view_dimensions()


int
ra_view_general::pos_view(void * /*vh */ , long /*x */ , long /*y */ )
{
	return -1;		// gui-specific function must be called
} // get_view_dimensions()


int
ra_view_general::mouse_press(int /*x */ , int y)
{
	int view = 0;
	for (int i = 0; i < _num_view; i++)
	{
		if ((y > _view_info[i].y) && (y <= (_view_info[i].y + _view_info[i].height)))
		{
			view = i;
			break;
		}
	}

	return view;
} // mouse_press()
