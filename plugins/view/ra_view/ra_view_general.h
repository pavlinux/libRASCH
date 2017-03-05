/*----------------------------------------------------------------------------
 * ra_view_general.cpp
 *
 * 
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002-2006, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifndef _RA_VIEW_GENERAL_H
#define _RA_VIEW_GENERAL_H

#define _LIBRASCH_BUILD
#include <ra_defines.h>

struct ch_info
{
	plugin_handle pl;
	char plugin_name[RA_VALUE_NAME_MAX];
}; // struct ch_info


struct general_view_info
{
	plugin_handle pl;

	int num_ch;
	int *ch;

	// space needed for view
	int width;
	int height;
	// space in percent (resize is easier)
	double width_percent;
	double height_percent;

	// position of left/upper corner
	int x;
	int y;

	void *view_handle;
}; // struct general_view_info


class ra_view_general
{
 public:
	ra_view_general(rec_handle rh);
	virtual ~ra_view_general();

	void init();

	bool block_signals() { return _block_signals; }

	int save_settings(const char *file);
	int load_settings(const char *file);

 protected:
	int init_general();
	int check_modules();
	int init_views();
	int combine_channels();
	int create_single_view(void *parent, int view);

	int mouse_press(int x, int y);

	virtual void *create_main_view();
	virtual int add_single_view(void *view);
	virtual int get_view_dimensions(void *vh, long *width, long *height);
	virtual int pos_view(void *vh, long x, long y);

	ra_handle _ra;
	meas_handle _meas;
	rec_handle _rec;
	plugin_handle _pl;

	bool _block_signals;

	// infos handling channels
	long _rec_type;		// RA_REC_TYPE_* value (e.g. RA_REC_TYPE_TIMESERIES)
	int _num_ch;
	struct ch_info *_ch_info;
	int _num_view;
	struct general_view_info *_view_info;

	int _view_with_focus;

	// variables needed for scrollbar
	long _page_width;
	long _num_samples;
	double _max_xscale;
	int _scroll_pos;
};				// class ra_view_general


#endif // _RA_VIEW_GENERAL_H
