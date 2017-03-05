/*
 * signal_ra.cpp  - 
 *
 *
 * Copyright 2001-2006 by Raphael Schneider <librasch@gmail.com>
 *
 * $Id: $
 */

#include <stdio.h>
#include <string.h>

#include <ra_defines.h>
#include "signal_ra.h"


signal_ra::signal_ra(ra_handle h)
{
	ra = h;
	v_list = NULL;
	num_view = 0;
	num_plot_view = 0;
}  // constructor


signal_ra::~signal_ra()
{
	close();
}  // destructor


bool
signal_ra::open(const char *file)
{
	strncpy(fn, file, MAX_PATH_RA);
	
	char t[MAX_PATH_RA];
	strncpy(t, fn, MAX_PATH_RA);
	mh = ra_meas_open(ra, t, NULL, 0);
	if (!mh)
		return false;
	
	eh = ra_eval_get_default(mh);

	// create base title		
	value_handle vh = ra_value_malloc();
	title[0] = '\0';
	if ((ra_info_get(mh, RA_INFO_OBJ_PERSON_NAME_C, vh) == 0) &&
	    ra_value_get_string(vh))
	{
		strncpy(title, ra_value_get_string(vh), 200);
	}
	if ((ra_info_get(mh, RA_INFO_OBJ_PERSON_FORENAME_C, vh) == 0) &&
	    ra_value_get_string(vh))
	{
		if (!title[0] == '\0')
			strcat(title, ", ");
		strcat(title, ra_value_get_string(vh));
	}
	rec_handle rh = ra_rec_get_first(mh, 0);
	if (ra_info_get(rh, RA_INFO_REC_GEN_NAME_C, vh) == 0)
	{
		char *t = new char[strlen(ra_value_get_string(vh)) + 3];
		sprintf(t, "(%s)", ra_value_get_string(vh));
		strcat(title, t);
		delete[] t;
	}
	ra_value_free(vh);

	return true;
}  // open()


void
signal_ra::close()
{
	if (v_list == NULL)
		return;

	for (int i = 0; i < num_view; i++)
		close_view(i);
	
	delete[] v_list;
	v_list = NULL;
	delete[] v_handle;
	v_handle = NULL;
	delete[] v_type;
	v_type = NULL;

	num_view = 0;

	if (mh)
		ra_meas_close(mh);
	mh = NULL;
}  // close


void
signal_ra::save(int use_ascii)
{
	if (!mh)
		return;

	ra_eval_save(mh, NULL, use_ascii);
}  // save()


void
signal_ra::filename(char *buf, int buf_size)
{
	strncpy(buf, fn, buf_size);
}  // filename()


void
signal_ra::add_view(void *view, view_handle vih, int type)
{
	if (v_list == NULL)
	{
		v_list = new void *[1];
		v_list[0] = view;

		v_handle = new view_handle[1];
		v_handle[0] = vih;

		v_type = new int[1];
		v_type[0] = type;
	}
	else
	{
		void **t = new void *[num_view+1];
		memcpy(t, v_list, sizeof(void *)*num_view);
		delete[] v_list;
		t[num_view] = view;
		v_list = t;

		view_handle *v = new view_handle[num_view+1];
		memcpy(v, v_handle, sizeof(view_handle)*num_view);
		delete[] v_handle;
		v[num_view] = vih;
		v_handle = v;

		int *n = new int[num_view+1];
		memcpy(n, v_type, sizeof(int)*num_view);
		delete[] v_type;
		n[num_view] = type;
		v_type = n;
	}
	num_view++;

	if (type == PLOT_VIEW)
		num_plot_view++;
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
	ra_comm_del(mh, dh, NULL);

	if (destroy)
	{		
		close_view(idx);
		ra_view_free(v_handle[idx]);
	}
	
	if ((num_view > 1) && (idx < (num_view-1)))
	{
		memmove(&(v_list[idx]), &(v_list[idx+1]),
			sizeof(v_list[0])*(num_view - idx - 1));
		memmove(&(v_handle[idx]), &(v_handle[idx+1]),
			sizeof(v_handle[0])*(num_view - idx - 1));
		memmove(&(v_type[idx]), &(v_type[idx+1]),
			sizeof(v_type[0])*(num_view - idx - 1));
	}
	
	num_view--;
	void **t = NULL;
	view_handle *v = NULL;
	int *n = NULL;
	if (num_view > 0)
	{
		t = new void *[num_view];
		memcpy(t, v_list, sizeof(v_list[0]) * num_view);
		v = new view_handle[num_view];
		memcpy(v, v_handle, sizeof(v_handle[0]) * num_view);
		n = new int[num_view];
		memcpy(n, v_type, sizeof(v_type[0]) * num_view);
	}
	delete[] v_list;
	v_list = t;
	delete[] v_handle;
	v_handle = v;
	delete[] v_type;
	v_type = n;
}  // remove_view()


void
signal_ra::update_all_views(void *sender)
{
	for (int i = 0; i < num_view; i++)
	{
		if (v_list[i] == sender)
			continue;
		
		update_view(i);
	}
}  // update_all_views()


void *
signal_ra::get_view(void *prev)
{
	if (v_list == NULL)
		return NULL;

	void *ret = NULL;
	if (prev == NULL)
		ret = v_list[0];
	else
	{
		bool use_next = false;
		for (int i = 0; i < num_view; i++)
		{
			if (v_list[i] == prev)
			{
				use_next = true;
				continue;
			}
			if (use_next)
			{
				ret = v_list[i];
				break;
			}
		}
	}

	return ret;
}  // get_view()


int
signal_ra::find_view(void *view)
{
	if (v_list == NULL)
		return -1;

	int index = -1;
	for (int i = 0; i < num_view; i++)
	{
		if (v_list[i] == view)
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
	if (v_list == NULL)
		return 0;

	return num_view;
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
	
	v_type[idx] = type;
}  // set_view_type()


int
signal_ra::get_view_type(void *view)
{
	int idx = find_view(view);
	if (idx == -1)
		return UNKNOWN_VIEW;

	return v_type[idx];
}  // get_view_type()


void
signal_ra::load_default_settings()
{
	struct comm_para *para = new struct comm_para[1];
	para[0].type = PARA_CHAR_POINTER;
	para[0].v.pc = new char[MAX_PATH_RA];
	get_ch_settings_file(para[0].v.pc, MAX_PATH_RA);

	ra_comm_emit(mh, "signal-ra", "load-settings", 1, para);

	delete[] para[0].v.pc;
	delete[] para;
}  // load_default_settings()


/**
 * \brief returns filename used for storage of channel settings
 * \param <buf> will receive the filename
 * \param <buf_size> size of parameter 'buf'
 *
 * This function returns the default filename used for storing
 * channel settings; it is based on the measurement name.
 */
int
signal_ra::get_ch_settings_file(char *buf, int buf_size)
{
	rec_handle rh = ra_rec_get_first(mh, 0);
	value_handle vh = ra_value_malloc();
	if (ra_info_get(rh, RA_INFO_REC_GEN_DIR_L, vh) != 0)
	{
		ra_value_free(vh);
		return -1;
	}
	long dir = ra_value_get_long(vh);

	if (ra_info_get(rh, RA_INFO_REC_GEN_PATH_C, vh) != 0)
	{
		ra_value_free(vh);
		return -1;
	}

	if (dir)
	{
#ifdef WIN32
		_snprintf(buf, buf_size, "%s%cch_settings.raset", ra_value_get_string(vh), SEPARATOR);
#else
		snprintf(buf, buf_size, "%s%cch_settings.raset", ra_value_get_string(vh), SEPARATOR);
#endif
	}
	else
	{
#ifdef WIN32
		_snprintf(buf, buf_size, "%s.raset", ra_value_get_string(vh));
#else
		snprintf(buf, buf_size, "%s.raset", ra_value_get_string(vh));
#endif
	}

	ra_value_free(vh);

	return 0;
} /* get_ch_settings_file() */


