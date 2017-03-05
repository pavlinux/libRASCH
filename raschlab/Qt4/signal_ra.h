/*
 * signal_ra.h  - 
 *
 * Copyright 2001-2006 by Raphael Schneider <librasch@gmail.com>
 *
 * $Id: $
 */

#ifndef SIGNAL_RA_H
#define SIGNAL_RA_H


//#include <list>
//#include <algorithm>

//using namespace std;


extern "C" {
#include <ra.h>
#include <ra_pl_comm.h>
}

#define SIGNAL_VIEW          0
#define TEMPLATE_VIEW        1
#define PLOT_VIEW            2
#define HRT_VIEW             3
#define ANNOT_COMMENT_VIEW   4
#define UNKNOWN_VIEW        99


class signal_ra
{
public:
	signal_ra(ra_handle m);
	virtual ~signal_ra();

	bool open(const char *fn);
	virtual void close();
	void save(int save_eval_ascii);
	void filename(char *buf, int buf_size);

	void add_view(void *view, view_handle vih, int type);
	virtual void remove_view(void *view, bool destroy=true, void *dest=NULL);
	virtual void update_all_views(void *sender);
	void *get_view(void *prev);
	int find_view(void *view);
	int num_views();
	int num_plot_views() { return num_plot_view; }
			
	meas_handle get_meas_handle() { return mh; }
	eval_handle get_eval_handle() { return eh; }
	void set_eval_handle(eval_handle eh) { eh = eh; }

	void get_title(char *buf, int buf_size) { strncpy(buf, title, buf_size); }

	void set_view_type(void *view, int type);
	int get_view_type(void *view);

	void load_default_settings();

protected:
	virtual void close_view(int index);
	virtual dest_handle get_dest_handle(int index);
	virtual void update_view(int index);	

	int get_ch_settings_file(char *buf, int buf_size);

	ra_handle ra;
	meas_handle mh;
	eval_handle eh;
	
	int num_view;
	int num_plot_view;
	void **v_list;
	view_handle *v_handle;
	int *v_type;

	char fn[MAX_PATH_RA];
	char title[200];
};  // class signal_ra


#endif  // SIGNAL_RA_H
