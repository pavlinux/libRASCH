/***************************************************************************
                          signal_ra.h  -  description
                             -------------------
    begin                : Mon Nov 22 2001
    copyright            : (C) 2001-2004 by Raphael Schneider
    email                : rasch@med1.med.tum.de

    $Id$
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SIGNAL_RA_H
#define SIGNAL_RA_H


//#include <list>
//#include <algorithm>

//using namespace std;


extern "C" {
#include <ra.h>
#include <ra_pl_comm.h>
}

#define SIGNAL_VIEW    0
#define TEMPLATE_VIEW  1
#define PLOT_VIEW      2
#define UNKNOWN_VIEW  99


class signal_ra
{
public:
	signal_ra(ra_handle m);
	virtual ~signal_ra();

	bool open(const char *fn);
	virtual void close();
	void save(int save_eval_ascii);
	void file_name(char *buf, int buf_size);
	
	void add_view(void *view, view_handle vih, int type);
	virtual void remove_view(void *view, bool destroy=true, void *dest=NULL);
	virtual void update_all_views(void *sender);
	void *get_view(void *prev);
	int find_view(void *view);
	int num_views();
	int num_plot_views() { return _num_plot_view; }
			
	meas_handle get_meas_handle() { return _mh; }
	eval_handle get_eval_handle() { return _eh; }
	void set_eval_handle(eval_handle eh) { _eh = eh; }

	void get_title(char *buf, int buf_size) { strncpy(buf, _title, buf_size); }

	void set_view_type(void *view, int type);
	int get_view_type(void *view);


protected:
	ra_handle _ra;
	meas_handle _mh;
	eval_handle _eh;
	
	int _num_view;
	int _num_plot_view;
	void **_v_list;
	view_handle *_v_handle;
	int *_v_type;

	char _fn[MAX_PATH_RA];
	char _title[200];

		
	virtual void close_view(int index);
	virtual dest_handle get_dest_handle(int index);
	virtual void update_view(int index);	
};  // class signal_ra


#endif  // SIGNAL_RA_H
