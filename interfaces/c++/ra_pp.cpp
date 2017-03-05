/**
 * \file ra_pp.cpp
 *
 * The file contains the implementation for the C++ interface of libRASCH
 *
 * \author Raphael Schneider (rasch@med1.med.tum.de)
 */

/*----------------------------------------------------------------------------
 *
 * Copyright (C) 2004-2007, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *
 *--------------------------------------------------------------------------*/


#include <ra_priv.h>

#define _LIBRASCHPP_BUILD
#include "ra_pp.h"

#include <assert.h>
#include <locale>
#include <cstring>

////////////////////////////// class rasch_value //////////////////////////////


rasch_value::rasch_value()
{
	_vh = ra_value_malloc();
} // default constructor


rasch_value::rasch_value(const rasch_value &v)
{
	_vh = ra_value_malloc();
	ra_value_copy(_vh, v._vh);
}  // copy constructor


rasch_value::rasch_value(value_handle vh)
{
	_vh = ra_value_malloc();
	set_by_value_handle(vh);
}  // constructor value_handle


rasch_value::rasch_value(short value, const char *name/*=NULL*/, const char *desc/*=NULL*/, int id/*=RA_INFO_NONE*/)
{
	_vh = ra_value_malloc();
	ra_value_set_short(_vh, value);
	::set_meta_info(_vh, name, desc, id);
} // constructor short


rasch_value::rasch_value(long value, const char *name/*=NULL*/, const char *desc/*=NULL*/, int id/*=RA_INFO_NONE*/)
{
	_vh = ra_value_malloc();
	ra_value_set_long(_vh, value);
	::set_meta_info(_vh, name, desc, id);
} // constructor long


rasch_value::rasch_value(double value, const char *name/*=NULL*/, const char *desc/*=NULL*/, int id/*=RA_INFO_NONE*/)
{
	_vh = ra_value_malloc();
	ra_value_set_double(_vh, value);
	::set_meta_info(_vh, name, desc, id);
}  // constructor double


rasch_value::rasch_value(const string &value, const char *name/*=NULL*/, const char *desc/*=NULL*/, int id/*=RA_INFO_NONE*/)
{
	_vh = ra_value_malloc();
	ra_value_set_string(_vh, value.c_str());
	::set_meta_info(_vh, name, desc, id);
}  // constructor string


rasch_value::rasch_value(const char *value, const char *name/*=NULL*/, const char *desc/*=NULL*/, int id/*=RA_INFO_NONE*/)
{
	_vh = ra_value_malloc();
	ra_value_set_string(_vh, value);
	::set_meta_info(_vh, name, desc, id);
}  // constructor char pointer


rasch_value::rasch_value(void *value, const char *name/*=NULL*/, const char *desc/*=NULL*/, int id/*=RA_INFO_NONE*/)
{
	_vh = ra_value_malloc();
	ra_value_set_voidp(_vh, value);
	::set_meta_info(_vh, name, desc, id);
} // constructor void pointer


rasch_value::rasch_value(const vector<short> &value, const char *name/*=NULL*/, const char *desc/*=NULL*/, int id/*=RA_INFO_NONE*/)
{
	_vh = ra_value_malloc();

	short *v = new short[value.size()];
	for (size_t l = 0; l < value.size(); l++)
		v[l] = value[l];
	ra_value_set_short_array(_vh, v, (unsigned long)value.size());
	delete[] v;

	::set_meta_info(_vh, name, desc, id);
}  //  constructor short array


rasch_value::rasch_value(const short *value, long n_elem, const char *name/*=NULL*/, const char *desc/*=NULL*/, int id/*=RA_INFO_NONE*/)
{
	_vh = ra_value_malloc();
	ra_value_set_short_array(_vh, value, n_elem);
	::set_meta_info(_vh, name, desc, id);
}  //  constructor long array pointer


rasch_value::rasch_value(const vector<long> &value, const char *name/*=NULL*/, const char *desc/*=NULL*/, int id/*=RA_INFO_NONE*/)
{
	_vh = ra_value_malloc();

	long *v = new long[value.size()];
	for (size_t l = 0; l < value.size(); l++)
		v[l] = value[l];
	ra_value_set_long_array(_vh, v, (unsigned long)value.size());
	delete[] v;

	::set_meta_info(_vh, name, desc, id);
}  //  constructor long array


rasch_value::rasch_value(const long *value, long n_elem, const char *name/*=NULL*/, const char *desc/*=NULL*/, int id/*=RA_INFO_NONE*/)
{
	_vh = ra_value_malloc();
	ra_value_set_long_array(_vh, value, n_elem);
	::set_meta_info(_vh, name, desc, id);
}  //  constructor long array pointer


rasch_value::rasch_value(const vector<double> &value, const char *name/*=NULL*/, const char *desc/*=NULL*/, int id/*=RA_INFO_NONE*/)
{
	_vh = ra_value_malloc();

	double *v = new double[value.size()];
	for (size_t l = 0; l < value.size(); l++)
		v[l] = value[l];
	ra_value_set_double_array(_vh, v, (unsigned long)value.size());
	delete[] v;

	::set_meta_info(_vh, name, desc, id);
}  //  constructor double array


rasch_value::rasch_value(const double *value, long n_elem, const char *name/*=NULL*/, const char *desc/*=NULL*/, int id/*=RA_INFO_NONE*/)
{
	_vh = ra_value_malloc();
	ra_value_set_double_array(_vh, value, n_elem);
	::set_meta_info(_vh, name, desc, id);
}  //  constructor double array pointer


rasch_value::rasch_value(const vector<string> &value, const char *name/*=NULL*/, const char *desc/*=NULL*/, int id/*=RA_INFO_NONE*/)
{
	_vh = ra_value_malloc();

	char **v = new char *[value.size()];
	size_t l;
	for (l = 0; l < value.size(); l++)
	{
		string s = value[l];
		v[l] = new char[s.length()];
		strncpy(v[l], s.c_str(), s.length());
	}
	ra_value_set_string_array(_vh, (const char **)v, (unsigned long)value.size());
	for (l = 0; l < value.size(); l++)
		delete[] v[l];
	delete[] v;

	::set_meta_info(_vh, name, desc, id);
}  //  constructor string array


rasch_value::rasch_value(const char **value, long n_elem, const char *name/*=NULL*/, const char *desc/*=NULL*/, int id/*=RA_INFO_NONE*/)
{
	_vh = ra_value_malloc();
	ra_value_set_string_array(_vh, value, n_elem);
	::set_meta_info(_vh, name, desc, id);
}  //  constructor char array pointer


rasch_value::rasch_value(const vector<void *> &value, const char *name/*=NULL*/, const char *desc/*=NULL*/, int id/*=RA_INFO_NONE*/)
{
	_vh = ra_value_malloc();

	void **v = new void *[value.size()];
	for (size_t l = 0; l < value.size(); l++)
		v[l] = value[l];
	ra_value_set_voidp_array(_vh, (const void **)v, (unsigned long)value.size());
	delete[] v;

	::set_meta_info(_vh, name, desc, id);
}  //  constructor void pointer array


rasch_value::rasch_value(const void **value, long n_elem, const char *name/*=NULL*/, const char *desc/*=NULL*/, int id/*=RA_INFO_NONE*/)
{
	_vh = ra_value_malloc();
	ra_value_set_voidp_array(_vh, (const void **)value, n_elem);
	::set_meta_info(_vh, name, desc, id);
}  //  constructor void pointer array pointer


rasch_value::rasch_value(const value_handle *value, long n_elem, const char *name/*=NULL*/,
			 const char *desc/*=NULL*/, int id/*=RA_INFO_NONE*/)
{
	_vh = ra_value_malloc();
	ra_value_set_vh_array(_vh, value, n_elem);
	::set_meta_info(_vh, name, desc, id);
}  //  constructor value_handle array pointer


rasch_value::~rasch_value()
{
	ra_value_free(_vh);
} // destructor


rasch_value &
rasch_value::operator=(const rasch_value &v)
{
	if (this != &v)
		ra_value_copy(_vh, v._vh);

	return *this;
}  // operator=


void
rasch_value::set_by_value_handle(value_handle vh)
{
	ra_value_copy(_vh, vh);
}  // set_by_value_handle()


void
rasch_value::fill_value_handle(value_handle vh)
{
	ra_value_copy(vh, _vh);
} // fill_value_handle()


bool
rasch_value::is_info_modifiable()
{
	int ret = ra_value_info_modifiable(_vh);

	if (ret == 1)
		return true;
	else
		return false;
} // is_info_modifiable()


void
rasch_value::set_info_modifiable(bool flag)
{
	int can_be_modified = 0;
	if (flag)
		can_be_modified = 1;

	ra_value_info_set_modifiable(_vh, can_be_modified);
} // set_info_modifiable()


long
rasch_value::get_num_elem()
{
	return ra_value_get_num_elem(_vh);
} // get_num_elem()


void
rasch_value::set_value(short value)
{
	ra_value_set_short(_vh, value);
} // set_value (short)


void
rasch_value::set_value(long value)
{
	ra_value_set_long(_vh, value);
} // set_value (long)


void
rasch_value::set_value(double value)
{
	ra_value_set_double(_vh, value);
} // set_value (double)


void
rasch_value::set_value(const string &value)
{
	ra_value_set_string(_vh, value.c_str());
} // set_value (string)


void
rasch_value::set_value(const char *value)
{
	ra_value_set_string(_vh, value);
} // set_value (char pointer)


void
rasch_value::set_value_utf8(const char *value)
{
	ra_value_set_string_utf8(_vh, value);
} // set_value_utf8 (char pointer)


void
rasch_value::set_value(void *value)
{
	ra_value_set_voidp(_vh, value);
} // set_value (void pointer)


void
rasch_value::set_value(const vector<short> &value)
{
	short *v = new short[value.size()];
	for (size_t l = 0; l < value.size(); l++)
		v[l] = value[l];
	ra_value_set_short_array(_vh, v, (unsigned long)value.size());
	delete[] v;
} // set_value (short vector)


void
rasch_value::set_value(const short *value, long n_elem)
{
	ra_value_set_short_array(_vh, value, n_elem);
} // set_value (short array)


void
rasch_value::set_value(const vector<long> &value)
{
	long *v = new long[value.size()];
	for (size_t l = 0; l < value.size(); l++)
		v[l] = value[l];
	ra_value_set_long_array(_vh, v, (unsigned long)value.size());
	delete[] v;
} // set_value (long vector)


void
rasch_value::set_value(const long *value, long n_elem)
{
	ra_value_set_long_array(_vh, value, n_elem);
} // set_value (long array)


void
rasch_value::set_value(const vector<double> &value)
{
	double *v = new double[value.size()];
	for (size_t l = 0; l < value.size(); l++)
		v[l] = value[l];
	ra_value_set_double_array(_vh, v, (unsigned long)value.size());
	delete[] v;	
} // set_value (double vector)


void
rasch_value::set_value(const double *value, long n_elem)
{
	ra_value_set_double_array(_vh, value, n_elem);
} // set_value (double array)


void
rasch_value::set_value(const vector<string> &value)
{
	char **v = new char *[value.size()];
	size_t l;
	for (l = 0; l < value.size(); l++)
	{
		string s = value[l];
		v[l] = new char[s.length()];
		strncpy(v[l], s.c_str(), s.length());
	}
	ra_value_set_string_array(_vh, (const char **)v, (unsigned long)value.size());
	for (l = 0; l < value.size(); l++)
		delete[] v[l];
	delete[] v;
} // set_value (string vector)


void
rasch_value::set_value(const char **value, long n_elem)
{
	ra_value_set_string_array(_vh, value, n_elem);
} // set_value (string array)


void
rasch_value::set_value_utf8(const char **value, long n_elem)
{
	ra_value_set_string_array_utf8(_vh, value, n_elem);
} // set_value_utf8 (string array)


void
rasch_value::set_value(const vector<void *> &value)
{
	void **v = new void *[value.size()];
	for (size_t l = 0; l < value.size(); l++)
		v[l] = value[l];
	ra_value_set_voidp_array(_vh, (const void **)v, (unsigned long)value.size());
	delete[] v;
} // set_value (void pointer vector)


void
rasch_value::set_value(const void **value, long n_elem)
{
	ra_value_set_voidp_array(_vh, value, n_elem);
} // set_value (void pointer array)


void
rasch_value::set_value(const value_handle *value, long n_elem)
{
	ra_value_set_vh_array(_vh, value, n_elem);
} // set_value (value_handle array)


void
rasch_value::set_meta_info(const char *name, const char *desc, int id, bool /*can_be_changed*/)
{
	::set_meta_info(_vh, name, desc, id);
} // set_meta_info()


string
rasch_value::get_value_c()
{
	string s = "";

	const char *c = ra_value_get_string(_vh);
	if (c != NULL)
		s = c;

	return s;
} // get_value_c()


string
rasch_value::get_value_c_utf8()
{
	string s = "";

	const char *c = ra_value_get_string_utf8(_vh);
	if (c != NULL)
		s = c;

	return s;
} // get_value_c_utf8()


vector<short>
rasch_value::get_value_sv()
{
	vector<short> sv;

	long n = get_num_elem();
	sv.resize(n);
	const short *sp = ra_value_get_short_array(_vh);
	for (long l = 0; l < n; l++)
		sv[l] = sp[l];

	return sv;
}  // get_value_sv()


vector<long>
rasch_value::get_value_lv()
{
	vector<long> lv;

	long n = get_num_elem();
	lv.resize(n);
	const long *lp = ra_value_get_long_array(_vh);
	for (long l = 0; l < n; l++)
		lv[l] = lp[l];

	return lv;
}  // get_value_lv()


vector<double>
rasch_value::get_value_dv()
{
	vector<double> dv;

	long n = get_num_elem();
	dv.resize(n);
	const double *dp = ra_value_get_double_array(_vh);
	for (long l = 0; l < n; l++)
		dv[l] = dp[l];

	return dv;
}  // get_value_dv()


vector<string>
rasch_value::get_value_cv()
{
	vector<string> sv;

	long n = get_num_elem();
	sv.resize(n);
	const char **cp = ra_value_get_string_array(_vh);
	for (long l = 0; l < n; l++)
		sv[l] = cp[l];

	return sv;
}  // get_value_cv()


vector<string>
rasch_value::get_value_cv_utf8()
{
	vector<string> sv;

	long n = get_num_elem();
	sv.resize(n);
	const char **cp = ra_value_get_string_array_utf8(_vh);
	for (long l = 0; l < n; l++)
		sv[l] = cp[l];

	return sv;
}  // get_value_cv_utf8()


vector<void *>
rasch_value::get_value_vv()
{
	vector<void *> vv;

	long n = get_num_elem();
	vv.resize(n);
	const void **vp = ra_value_get_voidp_array(_vh);
	for (long l = 0; l < n; l++)
		vv[l] = (void **)(vp)[l];

	return vv;
}  // get_value_vv()


void
rasch_value::reset()
{
	ra_value_reset(_vh);
} // reset()


////////////////////////////// class rasch_attribute //////////////////////////////

rasch_attribute::rasch_attribute()
{
	_h = NULL;
} // default constructor


rasch_attribute::rasch_attribute(any_handle h, const char *name, rasch_value &val)
	: _name(name), _value(val)
{
	_h = h;

	// if everything is given, set the attribute
	if ((_h != NULL) && (_name.length() > 0) && _value.is_ok())
	{
		value_handle vh = ra_value_malloc();
		val.fill_value_handle(vh);
		ra_eval_attribute_set(_h, _name.c_str(), vh);
		ra_value_free(vh);
	}
	else if ((_h != NULL) && (_name.length() > 0))
	{
		// only handle and name are given, so try to get the value
		value_handle vh = ra_value_malloc();
		ra_eval_attribute_get(_h, _name.c_str(), vh);
		_value.set_by_value_handle(vh);
		ra_value_free(vh);
	}
} // constructor


int
rasch_attribute::init(any_handle h, const char *name)
{
	_h = h;
	_name = name;

	value_handle vh = ra_value_malloc();
	int ret;
	if ((ret = ra_eval_attribute_get(_h, _name.c_str(), vh)) == 0)
		_value.set_by_value_handle(vh);
	ra_value_free(vh);

	return ret;
} // init()


int
rasch_attribute::set_value(rasch_value &val)
{
	if ((_h == NULL) || (_name.length() > 0))
		return -1;

	_value = val;

	value_handle vh = ra_value_malloc();
	val.fill_value_handle(vh);

	int ret = ra_eval_attribute_set(_h, _name.c_str(), vh);

	ra_value_free(vh);

	return ret;
} // set_value()


int
rasch_attribute::unset()
{
	return ra_eval_attribute_unset(_h, _name.c_str());
} // unset()


vector<rasch_attribute>
_get_all_attributes(any_handle h)
{
	vector<rasch_attribute> att;

	value_handle vh = ra_value_malloc();
	if (ra_eval_attribute_list(h, vh) != 0)
	{
		ra_value_free(vh);
		return att;
	}

	long num = ra_value_get_num_elem(vh);
	att.resize(num);
	const char **att_names = ra_value_get_string_array(vh);
	for (long l = 0; l < num; l++)
		att[l].init(h, att_names[l]);

	ra_value_free(vh);
	
	return att;
} // _get_all_attributes()



////////////////////////////// class rasch //////////////////////////////


rasch::rasch()
{
	_ra = ra_lib_init();
} // constructor


rasch::~rasch()
{
	if (_ra)
		ra_lib_close(_ra);
} // destructor


long
rasch::get_error(string &err_string)
{
	if (_ra == NULL)
		return -1;

	char error[200];
	long err = ra_lib_get_error(_ra, error, 200);

	err_string = error;

	return err;
}  // get_error()


int
rasch::use_plugin(int plugin_index, bool use_it)
{
	if (_ra == NULL)
		return -1;

	return ra_lib_use_plugin(_ra, plugin_index, (use_it ? 1 : 0));
}  // use_plugin()


rasch_plugin
rasch::get_plugin(int index, bool search_all/*=false*/)
{
	plugin_handle pl = ra_plugin_get_by_num(_ra, index, (search_all ? 1 : 0));
	return rasch_plugin(_ra, pl);
} // get_plugin (int)


rasch_plugin
rasch::get_plugin(const char *name, bool search_all/*=false*/)
{
	plugin_handle pl = ra_plugin_get_by_name(_ra, name, (search_all ? 1 : 0));
	return rasch_plugin(_ra, pl);
} // get_plugin (const char *)


vector<rasch_find>
rasch::find_meas(const char *dir)
{
	vector<rasch_find> meas;

	if (_ra == NULL)
		return meas;

	struct ra_find_struct rfs;
	ra_find_handle fh = ra_meas_find_first(_ra, dir, &rfs);
	if (!fh)
		return meas;

	do
	{
		rasch_find f(rfs.name, rfs.plugin);
		meas.push_back(f);
	} while (ra_meas_find_next(fh, &rfs));

	ra_meas_close_find(fh);

	return meas;
}  // find_meas()


rasch_value
rasch::get_info(int id)
{
	rasch_value v;

	if (_ra == NULL)
		return v;

	value_handle vh = ra_value_malloc();
	if (ra_info_get(_ra, id, vh) == 0)
		v.set_by_value_handle(vh);

	ra_value_free(vh);

	return v;
}  // get_info(int)


rasch_value
rasch::get_info(const char *name)
{
	rasch_value v;

	if (_ra == NULL)
		return v;

	value_handle vh = ra_value_malloc();
	if (ra_info_get_by_name(_ra, name, vh) == 0)
		v.set_by_value_handle(vh);

	ra_value_free(vh);

	return v;	
}  // get_info(const char *)


rasch_value
rasch::get_info_by_idx(int info_type, int index)
{
	rasch_value v;

	if (_ra == NULL)
		return v;

	value_handle vh = ra_value_malloc();
	if (ra_info_get_by_idx(_ra, info_type, index, vh) == 0)
		v.set_by_value_handle(vh);

	ra_value_free(vh);

	return v;
}  // get_info_by_idx()

/*
int
rasch::set_info(int id, rasch_value &value)
{
	if (_ra == NULL)
		return -1;

	value_handle vh = ra_value_malloc();
	value.fill_value_handle(vh);

	int ret = ra_info_set(_ra, id, vh);

	ra_value_free(vh);

	return ret;
}  // set_info()
*/


////////////////////////////// class rasch_meas //////////////////////////////


rasch_meas::rasch_meas(ra_handle ra, const char *fn, const char *eval_fn/*=NULL*/,
		       bool fast/*=false*/, bool new_meas/*=false*/)
{
	_meas = NULL;

	if (new_meas)
		return;  // not implemented yet

	_ra = ra;
	_meas = ra_meas_open(_ra, fn, eval_fn, (fast ? 1 : 0));
}  // constructor


rasch_meas::~rasch_meas()
{
	if (_meas)
		ra_meas_close(_meas);
	_meas = NULL;
}  // desctructor


rasch_value
rasch_meas::get_info(int id)
{
	rasch_value v;

	if (_meas == NULL)
		return v;

	value_handle vh = ra_value_malloc();
	if (ra_info_get(_meas, id, vh) == 0)
		v.set_by_value_handle(vh);

	ra_value_free(vh);

	return v;
}  // get_info(int)


rasch_value
rasch_meas::get_info(const char *name)
{
	rasch_value v;

	if (_meas == NULL)
		return v;

	value_handle vh = ra_value_malloc();
	if (ra_info_get_by_name(_meas, name, vh) == 0)
		v.set_by_value_handle(vh);

	ra_value_free(vh);

	return v;	
}  // get_info(const char *)


rasch_value
rasch_meas::get_info_by_idx(int info_type, int index)
{
	rasch_value v;

	if (_meas == NULL)
		return v;

	value_handle vh = ra_value_malloc();
	if (ra_info_get_by_idx(_meas, info_type, index, vh) == 0)
		v.set_by_value_handle(vh);

	ra_value_free(vh);

	return v;
}  // get_info_by_idx()


int
rasch_meas::set_info(int id, rasch_value &value)
{
	if (_meas == NULL)
		return -1;

	value_handle vh = ra_value_malloc();
	value.fill_value_handle(vh);

	int ret = ra_info_set(_meas, id, vh);

	ra_value_free(vh);

	return ret;
}  // set_info(int)


int
rasch_meas::set_info(const char *name, rasch_value &value)
{
	if (_meas == NULL)
		return -1;

	value_handle vh = ra_value_malloc();
	value.fill_value_handle(vh);

	int ret = ra_info_set_by_name(_meas, name, vh);

	ra_value_free(vh);

	return ret;
}  // set_info(char)


int
rasch_meas::move(const char *dest_dir)
{
	if (_meas == NULL)
		return -1;

	_meas = ra_meas_move(_meas, dest_dir);
	if (_meas == NULL)
		return -1;
	
	return 0;
}  // move()


int
rasch_meas::copy(const char *dest_dir)
{
	if (_meas == NULL)
		return -1;

	return ra_meas_copy(_meas, dest_dir);
}  // copy()


int
rasch_meas::del()
{
	if (_meas == NULL)
		return -1;

	return ra_meas_delete(_meas);
}  // del()


int
rasch_meas::save()
{
	if (_meas == NULL)
		return -1;

	return ra_meas_save(_meas);
}  // save()


int
rasch_meas::load_eval(const char * /*fn / *=NULL*/)
{
	if (_meas == NULL)
		return -1;

//	return ra_eval_load(_meas, fn);
	return -1;		// TODO: libRASCH function not implemented yet
}  // load_eval()


rasch_eval
rasch_meas::add_eval(const char *name, const char *desc, bool original)
{
	eval_handle eh = ra_eval_add(_meas, name, desc, (original ? 1 : 0));
	return rasch_eval(_meas, eh);
} // add_eval()


rasch_eval
rasch_meas::get_default_eval()
{
	eval_handle eh = ra_eval_get_default(_meas);
	return rasch_eval(_meas, eh);
} // get_default_eval()


rasch_eval
rasch_meas::get_original_eval()
{
	eval_handle eh = ra_eval_get_original(_meas);
	return rasch_eval(_meas, eh);
} // get_original_eval()


vector<rasch_eval>
rasch_meas::get_eval_all()
{
	vector<rasch_eval> all;

	if (_meas == NULL)
		return all;

	value_handle vh = ra_value_malloc();
	if (ra_eval_get_all(_meas, vh) != 0)
	{
		ra_value_free(vh);
		return all;
	}
	
	long n = ra_value_get_num_elem(vh);
	all.resize(n);
	const void **vp = ra_value_get_voidp_array(vh);
	for (long l = 0; l < n; l++)
		all[l].init(_meas, (void *)(vp[l]));

	ra_value_free(vh);

	return all;
} // get_eval_all()


////////////////////////////// class rasch_rec //////////////////////////////


rasch_rec::rasch_rec(meas_handle meas, long session)
{
	_meas = meas;
	_rec = ra_rec_get_first(_meas, session);
}  // constructor


rasch_rec::rasch_rec(rec_handle rec)
{
	_rec = rec;
	_meas = ra_meas_handle_from_any_handle(rec);
}  // constructor (rec_handle)


rasch_rec::~rasch_rec()
{
	// do nothing
}  // destructor


rasch_rec
rasch_rec::get_next()
{
	rec_handle next = ra_rec_get_next(_rec);
	return rasch_rec(next);
}  // get_next()


rasch_rec
rasch_rec::get_first_child()
{
	rec_handle child = ra_rec_get_first_child(_rec);
	return rasch_rec(child);
}  // get_first_child()


rasch_value
rasch_rec::get_info(int id)
{
	rasch_value v;

	if (_meas == NULL)
		return v;

	value_handle vh = ra_value_malloc();
	if (ra_info_get(_rec, id, vh) == 0)
		v.set_by_value_handle(vh);

	ra_value_free(vh);

	return v;
}  // get_info(int)


rasch_value
rasch_rec::get_info(const char *name)
{
	rasch_value v;

	if (_meas == NULL)
		return v;

	value_handle vh = ra_value_malloc();
	if (ra_info_get_by_name(_rec, name, vh) == 0)
		v.set_by_value_handle(vh);

	ra_value_free(vh);

	return v;	
}  // get_info(const char *)


rasch_value
rasch_rec::get_info_by_idx(int index)
{
	rasch_value v;

	if (_meas == NULL)
		return v;

	value_handle vh = ra_value_malloc();
	if (ra_info_get_by_idx(_rec, RA_INFO_REC_GENERAL, index, vh) == 0)
		v.set_by_value_handle(vh);

	ra_value_free(vh);

	return v;
}  // get_info_by_idx()


int
rasch_rec::set_info(int id, rasch_value &value)
{
	if (_meas == NULL)
		return -1;

	value_handle vh = ra_value_malloc();
	value.fill_value_handle(vh);

	int ret = ra_info_set(_rec, id, vh);

	ra_value_free(vh);

	return ret;
}  // set_info(int)


int
rasch_rec::set_info(const char *name, rasch_value &value)
{
	if (_meas == NULL)
		return -1;

	value_handle vh = ra_value_malloc();
	value.fill_value_handle(vh);

	int ret = ra_info_set_by_name(_rec, name, vh);

	ra_value_free(vh);

	return ret;
}  // set_info(char)



////////////////////////////// class rasch_device //////////////////////////////


rasch_device::rasch_device(rec_handle rec, int dev_number)
{
	_rec = rec;
	_device = dev_number;
}  // constructor


rasch_device::~rasch_device()
{
	// do nothing
}  // destructor


rasch_value
rasch_device::get_info(int id)
{
	rasch_value v;

	if (_rec == NULL)
		return v;

	value_handle vh = ra_value_malloc();
	ra_value_set_number(vh, _device);
	if (ra_info_get(_rec, id, vh) == 0)
		v.set_by_value_handle(vh);

	ra_value_free(vh);

	return v;
}  // get_info(int)


rasch_value
rasch_device::get_info(const char *name)
{
	rasch_value v;

	if (_rec == NULL)
		return v;

	value_handle vh = ra_value_malloc();
	ra_value_set_number(vh, _device);
	if (ra_info_get_by_name(_rec, name, vh) == 0)
		v.set_by_value_handle(vh);

	ra_value_free(vh);

	return v;	
}  // get_info(const char *)


rasch_value
rasch_device::get_info_by_idx(int index)
{
	rasch_value v;

	if (_rec == NULL)
		return v;

	value_handle vh = ra_value_malloc();
	ra_value_set_number(vh, _device);
	if (ra_info_get_by_idx(_rec, RA_INFO_REC_DEVICE, index, vh) == 0)
		v.set_by_value_handle(vh);

	ra_value_free(vh);

	return v;
}  // get_info_by_idx()



////////////////////////////// class rasch_channel //////////////////////////////


rasch_channel::rasch_channel(rec_handle rec, int channel)
{
	_rec = rec;
	_ch = channel;
}  // constructor


rasch_channel::~rasch_channel()
{
	// do nothing
}  // destructor


rasch_value
rasch_channel::get_info(int id)
{
	rasch_value v;

	if (_rec == NULL)
		return v;

	value_handle vh = ra_value_malloc();
	ra_value_set_number(vh, _ch);
	if (ra_info_get(_rec, id, vh) == 0)
		v.set_by_value_handle(vh);

	ra_value_free(vh);

	return v;
}  // get_info(int)


rasch_value
rasch_channel::get_info(const char *name)
{
	rasch_value v;

	if (_rec == NULL)
		return v;

	value_handle vh = ra_value_malloc();
	ra_value_set_number(vh, _ch);
	if (ra_info_get_by_name(_rec, name, vh) == 0)
		v.set_by_value_handle(vh);

	ra_value_free(vh);

	return v;	
}  // get_info(const char *)


rasch_value
rasch_channel::get_info_by_idx(int index)
{
	rasch_value v;

	if (_rec == NULL)
		return v;

	value_handle vh = ra_value_malloc();
	ra_value_set_number(vh, _ch);
	if (ra_info_get_by_idx(_rec, RA_INFO_REC_CHANNEL, index, vh) == 0)
		v.set_by_value_handle(vh);

	ra_value_free(vh);

	return v;
}  // get_info_by_idx()


vector<unsigned long>
rasch_channel::get_raw(size_t start, size_t num_data)
{
	vector<unsigned long> val;

	if ((_rec == NULL) || (num_data <= 0))
		return val;

	unsigned long *buf = new unsigned long[num_data];
	size_t num = ra_raw_get(_rec, _ch, start, num_data, buf, NULL);
	val.resize(num);
	for (size_t i = 0; i < num; i++)
		val[i] = buf[i];
	delete[] buf;

	return val;
}  // get_raw()


vector<double>
rasch_channel::get_raw_unit(size_t start, size_t num_data)
{
	vector<double> val;

	if ((_rec == NULL) || (num_data <= 0))
		return val;

	double *buf = new double[num_data];
	size_t num = ra_raw_get_unit(_rec, _ch, start, num_data, buf);
	val.resize(num);
	for (size_t i = 0; i < num; i++)
		val[i] = buf[i];
	delete[] buf;

	return val;
}  // get_raw_unit()



////////////////////////////// class rasch_plugin //////////////////////////////


rasch_plugin::rasch_plugin(ra_handle ra, plugin_handle pl)
{
	_ra = ra;
	_pl = pl;
}  // constructor


rasch_plugin::~rasch_plugin()
{
	// do nothing
}  // destructor


rasch_value
rasch_plugin::get_info(int id, long num/*=0*/)
{
	rasch_value v;

	if (_pl == NULL)
		return v;

	value_handle vh = ra_value_malloc();
	ra_value_set_number(vh, num);
	if (ra_info_get(_pl, id, vh) == 0)
		v.set_by_value_handle(vh);

	ra_value_free(vh);

	return v;
}  // get_info(int)


rasch_value
rasch_plugin::get_info(const char *name, long num/*=0*/)
{
	rasch_value v;

	if (_pl == NULL)
		return v;

	value_handle vh = ra_value_malloc();
	ra_value_set_number(vh, num);
	if (ra_info_get_by_name(_pl, name, vh) == 0)
		v.set_by_value_handle(vh);

	ra_value_free(vh);

	return v;	
}  // get_info(const char *)


rasch_value
rasch_plugin::get_info_by_idx(int index, long num/*=0*/)
{
	rasch_value v;

	if (_pl == NULL)
		return v;

	value_handle vh = ra_value_malloc();
	ra_value_set_number(vh, num);
	if (ra_info_get_by_idx(_pl, RA_INFO_PLUGIN_START, index, vh) == 0)
		v.set_by_value_handle(vh);

	ra_value_free(vh);

	return v;
}  // get_info_by_idx()



////////////////////////////// class rasch_proc //////////////////////////////


rasch_proc::rasch_proc(plugin_handle pl, meas_handle mh, void (*callback) (const char *, int ))
{
	_pl = pl;
	_meas = mh;
	_proc = ra_proc_get(mh, pl, callback);
}  // constructor


rasch_proc::~rasch_proc()
{
	if (_proc)
		ra_proc_free(_proc);
}  // destructor


long
rasch_proc::get_result_idx(const char *res_ascii_id)
{
	if (_proc == NULL)
		return -1;

	return ra_proc_get_result_idx(_proc, res_ascii_id);
}  // get_result_idx()


int
rasch_proc::set_option(const char *name, rasch_value &value)
{
	if (_proc == NULL)
		return -1;

	value_handle vh = ra_value_malloc();
	value.fill_value_handle(vh);
	int ret = ra_lib_set_option(_proc, name, vh);
	ra_value_free(vh);

	return ret;
}  // set_option()


rasch_value
rasch_proc::get_option(const char *name)
{
	rasch_value val;

	if (_proc == NULL)
		return val;

	value_handle vh = ra_value_malloc();
	if (ra_lib_get_option(_proc, name, vh) == 0)
		val.set_by_value_handle(vh);
	ra_value_free(vh);

	return val;
}  // get_option()


vector<rasch_value>
rasch_proc::do_processing(long &num_sets, long &num_res)
{
	vector<rasch_value> results;
	
	if (_proc == NULL)
		return results;

	results.resize(0);
	int ret = ra_proc_do(_proc);
	if (ret != 0)
		return results;

	value_handle vh = ra_value_malloc();
	if (ra_info_get(_proc, RA_INFO_PROC_NUM_RES_SETS_L, vh) != 0)
	{
		ra_value_free(vh);
		return results;
	}
	num_sets = ra_value_get_long(vh);
	if (ra_info_get(_proc, RA_INFO_PROC_NUM_RES_L, vh) != 0)
	{
		ra_value_free(vh);
		return results;
	}
	num_res = ra_value_get_long(vh);
	results.resize(num_res * num_sets);
	long cnt = 0;
	for (long l = 0; l < num_sets; l++)
	{
		for (long m = 0; m < num_res; m++)
		{
			if (ra_proc_get_result(_proc, m, l, vh) == 0)
				results[cnt].set_by_value_handle(vh);
			cnt++;
		}
	}

	ra_value_free(vh);

	return results;
}  // do_processing()


////////////////////////////// class rasch_view //////////////////////////////


rasch_view::rasch_view(plugin_handle pl, meas_handle mh, void *parent)
{
	_pl = pl;
	_meas = mh;
	_view = ra_view_get(mh, pl, parent);
}  // constructor


rasch_view::~rasch_view()
{
	if (_view)
		ra_view_free(_view);
}  // destructor


////////////////////////////// class rasch_dialog //////////////////////////////


rasch_dialog::rasch_dialog(plugin_handle pl, meas_handle mh)
{
	_pl = pl;
	_meas = mh;
	_dlg = ra_proc_get(mh, pl, NULL);
}  // constructor


rasch_dialog::~rasch_dialog()
{
	if (_dlg)
		ra_proc_free(_dlg);
}  // destructor


long
rasch_dialog::get_result_idx(const char *res_ascii_id)
{
	if (_dlg == NULL)
		return -1;

	return ra_proc_get_result_idx(_dlg, res_ascii_id);
}  // get_result_idx()


int
rasch_dialog::set_option(const char *name, rasch_value &value)
{
	if (_dlg == NULL)
		return -1;

	value_handle vh = ra_value_malloc();
	value.fill_value_handle(vh);
	int ret = ra_lib_set_option(_dlg, name, vh);
	ra_value_free(vh);

	return ret;
}  // set_option()


rasch_value
rasch_dialog::get_option(const char *name)
{
	rasch_value val;

	if (_dlg == NULL)
		return val;

	value_handle vh = ra_value_malloc();
	if (ra_lib_get_option(_dlg, name, vh) == 0)
		val.set_by_value_handle(vh);
	ra_value_free(vh);

	return val;
}  // get_option()


int
rasch_dialog::exec(vector<rasch_value> &results)
{
	results.resize(0);
	
	if (_dlg == NULL)
		return -1;

	results.resize(0);
	int ret = ra_gui_call(_dlg, _pl);

	value_handle vh = ra_value_malloc();
	if (ra_info_get(_dlg, RA_INFO_PROC_NUM_RES_SETS_L, vh) != 0)
	{
		ra_value_free(vh);
		return ret;
	}
	long num_sets = ra_value_get_long(vh);
	if (ra_info_get(_dlg, RA_INFO_PROC_NUM_RES_L, vh) != 0)
	{
		ra_value_free(vh);
		return ret;
	}
	long num_res = ra_value_get_long(vh);
	results.resize(num_res * num_sets);
	long cnt = 0;
	for (long l = 0; l < num_sets; l++)
	{
		for (long m = 0; m < num_res; m++)
		{
			if (ra_proc_get_result(_dlg, m, l, vh) == 0)
				results[cnt].set_by_value_handle(vh);
			cnt++;
		}
	}

	ra_value_free(vh);

	return ret;
}  // exec()


////////////////////////////// class rasch_eval //////////////////////////////


rasch_eval::rasch_eval()
{
	_meas = NULL;
	_eval = NULL;
}  // constructor


rasch_eval::rasch_eval(meas_handle meas, eval_handle eh)
{
	_meas = meas;
	_eval = eh;
}  // constructor


rasch_eval::~rasch_eval()
{
}  // destructor


vector<rasch_attribute>
rasch_eval::get_attributes()
{
	return _get_all_attributes(_eval);
} // get_attributes()


rasch_value
rasch_eval::get_info(int id)
{
	rasch_value v;

	if (_eval == NULL)
		return v;

	value_handle vh = ra_value_malloc();
	if (ra_info_get(_eval, id, vh) == 0)
		v.set_by_value_handle(vh);

	ra_value_free(vh);

	return v;
}  // get_info(int)


rasch_value
rasch_eval::get_info(const char *name)
{
	rasch_value v;

	if (_eval == NULL)
		return v;

	value_handle vh = ra_value_malloc();
	if (ra_info_get_by_name(_eval, name, vh) == 0)
		v.set_by_value_handle(vh);

	ra_value_free(vh);

	return v;	
}  // get_info(const char *)


rasch_value
rasch_eval::get_info_by_idx(int index)
{
	rasch_value v;

	if (_eval == NULL)
		return v;

	value_handle vh = ra_value_malloc();
	if (ra_info_get_by_idx(_eval, RA_INFO_EVAL_START, index, vh) == 0)
		v.set_by_value_handle(vh);

	ra_value_free(vh);

	return v;
}  // get_info_by_idx()


int
rasch_eval::del()
{
	if (_eval == NULL)
		return -1;

	return ra_eval_delete(_eval);
}  // del()


int
rasch_eval::set_default()
{
	if (_eval == NULL)
		return -1;

	return ra_eval_set_default(_eval);
}  // set_default()


rasch_class
rasch_eval::add_class(const char *id, const char *name, const char *desc)
{
	class_handle h = ra_class_add(_eval, id, name, desc);
	return rasch_class(_eval, h);
} // add_class()


rasch_class
rasch_eval::add_class_predef(const char *id)
{
	class_handle h = ra_class_add_predef(_eval, id);
	return rasch_class(_eval, h);
} // add_class_predef()


vector<rasch_class>
rasch_eval::get_class(const char *id)
{
	vector<rasch_class> cl;

	value_handle vh = ra_value_malloc();
	if (ra_class_get(_eval, id, vh) != 0)
	{
		ra_value_free(vh);
		return cl;
	}

	long num = ra_value_get_num_elem(vh);
	cl.resize(num);
	const void **h = ra_value_get_voidp_array(vh);
	for (long l = 0; l < num; l++)
		cl[l].init(_eval, (void *)(h[l]));

	ra_value_free(vh);

	return cl;
} // get_class()


int
rasch_eval::save(const char *fn/*=NULL*/, bool use_ascii/*=false*/)
{
	if (_eval == NULL)
		return -1;

	return ra_eval_save(ra_meas_handle_from_any_handle(_eval), fn, (use_ascii ? 1 : 0));
}  // save()


////////////////////////////// class rasch_class //////////////////////////////

rasch_class::rasch_class()
{
	_eval = NULL;
	_class = NULL;
} // constructor


rasch_class::rasch_class(eval_handle eh, class_handle clh)
{
	_eval = eh;
	_class = clh;
} // contstructor


rasch_class::~rasch_class()
{
	// do nothing
}  // destructor


vector<rasch_attribute>
rasch_class::get_attributes()
{
	return _get_all_attributes(_class);
} // get_attributes()


rasch_value
rasch_class::get_info(int id)
{
	rasch_value v;

	if (_class == NULL)
		return v;

	value_handle vh = ra_value_malloc();
	if (ra_info_get(_class, id, vh) == 0)
		v.set_by_value_handle(vh);

	ra_value_free(vh);

	return v;
}  // get_info(int)


rasch_value
rasch_class::get_info(const char *name)
{
	rasch_value v;

	if (_class == NULL)
		return v;

	value_handle vh = ra_value_malloc();
	if (ra_info_get_by_name(_class, name, vh) == 0)
		v.set_by_value_handle(vh);

	ra_value_free(vh);

	return v;	
}  // get_info(const char *)


rasch_value
rasch_class::get_info_by_idx(int index)
{
	rasch_value v;

	if (_class == NULL)
		return v;

	value_handle vh = ra_value_malloc();
	if (ra_info_get_by_idx(_class, RA_INFO_CLASS_START, index, vh) == 0)
		v.set_by_value_handle(vh);

	ra_value_free(vh);

	return v;
}  // get_info_by_idx()


long
rasch_class::add_event(long start, long end)
{
	long ev_id = ra_class_add_event(_class, start, end);
	return ev_id;
} // add_event()


int
rasch_class::del_event(long ev_id)
{
	return ra_class_del_event(_class, ev_id);
} // del_event()


int
rasch_class::set_event_pos(long ev_id, long start, long end)
{
	return ra_class_set_event_pos(_class, ev_id, start, end);
} // set_event_pos()


int
rasch_class::get_event_pos(long ev_id, long &start, long &end)
{
	long s, e;
	int ret = ra_class_get_event_pos(_class, ev_id, &s, &e);
	if (ret == 0)
	{
		start = s;
		end = e;
	}

	return ret;
} // get_event_pos()


vector<long>
rasch_class::get_events(long start/*=0*/, long end/*=-1*/, bool complete/*=false*/, bool sort/*=true*/)
{
	vector<long> lv;

	value_handle vh = ra_value_malloc();
	if (ra_class_get_events(_class, start, end, (complete ? 1 : 0), (sort ? 1 : 0), vh) != 0)
	{
		ra_value_free(vh);
		return lv;
	}

	long num = ra_value_get_num_elem(vh);
	lv.resize(num);
	const long *ev_ids = ra_value_get_long_array(vh);
	for (long l = 0; l < num; l++)
		lv[l] = ev_ids[l];

	ra_value_free(vh);

	return lv;
} // get_events()


long
rasch_class::get_prev_event(long ev_id)
{
	return ra_class_get_prev_event(_class, ev_id);
} // get_prev_event()


long
rasch_class::get_next_event(long ev_id)
{
	return ra_class_get_next_event(_class, ev_id);
} // get_next_event()


rasch_prop
rasch_class::add_prop(const char *id, long val_type, const char *name,
		      const char *desc, const char *unit, bool use_minmax,
		      double min, double max, bool has_ignore_value, double ignore_value)
{
	prop_handle ph = ra_prop_add(_class, id, val_type, name, desc, unit,
				     (use_minmax ? 1 : 0), min, max,
				     (has_ignore_value ? 1 : 0), ignore_value);
	return rasch_prop(_class, ph);
} // add_prop()


rasch_prop
rasch_class::add_prop_predef(const char *id)
{
	prop_handle ph = ra_prop_add_predef(_class, id);
	return rasch_prop(_class, ph);
} // add_prop_predef()


rasch_prop
rasch_class::get_prop(const char *id)
{
	prop_handle ph = ra_prop_get(_class, id);
	return rasch_prop(_class, ph);
} // get_prop()


vector<rasch_prop>
rasch_class::get_prop_all()
{
	vector<rasch_prop> p;

	value_handle vh = ra_value_malloc();
	if (ra_prop_get_all(_class, vh) != 0)
	{
		ra_value_free(vh);
		return p;
	}

	long num = ra_value_get_num_elem(vh);
	p.resize(num);
	const void **h = ra_value_get_voidp_array(vh);
	for (long l = 0; l < num; l++)
		p[l].init(_class, (void *)(h[l]));

	ra_value_free(vh);

	return p;
} // get_prop_all()


rasch_summary
rasch_class::add_sum(const char *id, const char *name, const char *desc,
		     long num_dim, const char **dim_name, const char **dim_unit)
{
	sum_handle sh = ra_sum_add(_class, id, name, desc, num_dim, dim_name, dim_unit);
	return rasch_summary(_class, sh);
} // add_sum()


vector<rasch_summary>
rasch_class::get_sum(const char *id)
{
	vector<rasch_summary> s;

	value_handle vh = ra_value_malloc();
	if (ra_sum_get(_class, id, vh) != 0)
	{
		ra_value_free(vh);
		return s;
	}

	long num = ra_value_get_num_elem(vh);
	s.resize(num);
	const void **h = ra_value_get_voidp_array(vh);
	for (long l = 0; l < num; l++)
		s[l].init(_class, (void *)(h[l]));

	ra_value_free(vh);

	return s;
} // get_sum()



////////////////////////////// class rasch_prop //////////////////////////////



rasch_prop::rasch_prop()
{
	_eval = NULL;
	_prop = NULL;
}  // constructor


rasch_prop::rasch_prop(eval_handle eval, prop_handle prop)
{
	_eval = eval;
	_prop = prop;
}  // constructor (prop_handle)


rasch_prop::~rasch_prop()
{
	// do nothing
}  // destructor


vector<rasch_attribute>
rasch_prop::get_attributes()
{
	return _get_all_attributes(_prop);
} // get_attributes()


rasch_value
rasch_prop::get_info(int id)
{
	rasch_value v;

	if (_prop == NULL)
		return v;

	value_handle vh = ra_value_malloc();
	if (ra_info_get(_prop, id, vh) == 0)
		v.set_by_value_handle(vh);

	ra_value_free(vh);

	return v;
}  // get_info(int)


rasch_value
rasch_prop::get_info(const char *name)
{
	rasch_value v;

	if (_prop == NULL)
		return v;

	value_handle vh = ra_value_malloc();
	if (ra_info_get_by_name(_prop, name, vh) == 0)
		v.set_by_value_handle(vh);

	ra_value_free(vh);

	return v;	
}  // get_info(const char *)


rasch_value
rasch_prop::get_info_by_idx(int index)
{
	rasch_value v;

	if (_prop == NULL)
		return v;

	value_handle vh = ra_value_malloc();
	if (ra_info_get_by_idx(_prop, RA_INFO_PROP_START, index, vh) == 0)
		v.set_by_value_handle(vh);

	ra_value_free(vh);

	return v;
}  // get_info_by_idx()


int
rasch_prop::del()
{
	if (_prop == NULL)
		return -1;

	int ret = ra_prop_delete(_prop);
	if (ret == 0)
		_prop = NULL;

	return ret;
}  // del()


int
rasch_prop::set_value(long ev_id, long ch, rasch_value value)
{
	value_handle vh = ra_value_malloc();
	value.fill_value_handle(vh);
	int ret = ra_prop_set_value(_prop, ev_id, ch, vh);

	ra_value_free(vh);

	return ret;
} // set_value()


int
rasch_prop::set_value_mass(const long *ev_ids, const long *chs, rasch_value value)
{
	value_handle vh = ra_value_malloc();
	value.fill_value_handle(vh);
	int ret = ra_prop_set_value_mass(_prop, ev_ids, chs, vh);

	ra_value_free(vh);

	return ret;	
} // set_value_mass()


vector<long>
rasch_prop::get_ch(long ev_id)
{
	vector<long> ch;
	
	value_handle vh = ra_value_malloc();
	if (ra_prop_get_ch(_prop, ev_id, vh) != 0)
	{
		ra_value_free(vh);
		return ch;
	}

	long num = ra_value_get_num_elem(vh);
	ch.resize(num);
	const short *chs = ra_value_get_short_array(vh);
	for (long l = 0; l < num; l++)
		ch[l] = chs[l];
	
	ra_value_free(vh);

	return ch;
} // get_ch()


rasch_value
rasch_prop::get_value(long ev_id, long ch)
{
	if (_prop == NULL)
		return rasch_value();

	value_handle vh = ra_value_malloc();
	if (ra_prop_get_value(_prop, ev_id, ch, vh) != 0)
	{
		ra_value_free(vh);
		return rasch_value();
	}

	rasch_value v;
	v.set_by_value_handle(vh);

	ra_value_free(vh);

	return v;
} // get_value()


int
rasch_prop::get_value_all(rasch_value &ev_ids, rasch_value &chs, rasch_value &values)
{
	if (_prop == NULL)
		return -1;

	value_handle id = ra_value_malloc();
	value_handle ch = ra_value_malloc();
	value_handle v = ra_value_malloc();
	int ret = ra_prop_get_value_all(_prop, id, ch, v);
	if (ret == 0)
	{
		ev_ids.set_by_value_handle(id);
		chs.set_by_value_handle(ch);
		values.set_by_value_handle(v);
	}

	ra_value_free(id);
	ra_value_free(ch);
	ra_value_free(v);

	return ret;
} // get_value_all()


vector<long>
rasch_prop::get_events(rasch_value min, rasch_value max, long ch)
{
	vector<long> ev_ids;
	
	value_handle vh_min = ra_value_malloc();
	min.fill_value_handle(vh_min);
	value_handle vh_max = ra_value_malloc();
	max.fill_value_handle(vh_max);
	
	value_handle vh = ra_value_malloc();
	if (ra_prop_get_events(_prop, vh_min, vh_max, ch, vh) != 0)
	{
		ra_value_free(vh);
		ra_value_free(vh_min);
		ra_value_free(vh_max);
		return ev_ids;
	}

	long num = ra_value_get_num_elem(vh);
	ev_ids.resize(num);
	const long *ids = ra_value_get_long_array(vh);
	for (long l = 0; l < num; l++)
		ev_ids[l] = ids[l];
	
	ra_value_free(vh_min);
	ra_value_free(vh_max);
	ra_value_free(vh);

	return ev_ids;
} // get_events()


////////////////////////////// class rasch_summary //////////////////////////////


rasch_summary::rasch_summary()
{
	_class = NULL;
	_summary = NULL;
}  // constructor


rasch_summary::rasch_summary(class_handle clh, sum_handle sh)
{
	_class = clh;
	_summary = sh;
}  // constructor


rasch_summary::~rasch_summary()
{
	// do nothing
}  // destructor


vector<rasch_attribute>
rasch_summary::get_attributes()
{
	return _get_all_attributes(_summary);
} // get_attributes()


rasch_value
rasch_summary::get_info(int id)
{
	rasch_value v;

	if (_summary == NULL)
		return v;

	value_handle vh = ra_value_malloc();
	if (ra_info_get(_summary, id, vh) == 0)
		v.set_by_value_handle(vh);

	ra_value_free(vh);

	return v;
}  // get_info(int)


rasch_value
rasch_summary::get_info(const char *name)
{
	rasch_value v;

	if (_summary == NULL)
		return v;

	value_handle vh = ra_value_malloc();
	if (ra_info_get_by_name(_summary, name, vh) == 0)
		v.set_by_value_handle(vh);

	ra_value_free(vh);

	return v;	
}  // get_info(const char *)


rasch_value
rasch_summary::get_info_by_idx(int index)
{
	rasch_value v;

	if (_summary == NULL)
		return v;

	value_handle vh = ra_value_malloc();
	if (ra_info_get_by_idx(_summary, RA_INFO_SUM_START, index, vh) == 0)
		v.set_by_value_handle(vh);

	ra_value_free(vh);

	return v;
}  // get_info_by_idx()


int
rasch_summary::del()
{
	if (_summary == NULL)
		return -1;

	int ret = ra_sum_delete(_summary);
	if (ret == 0)
		_summary = NULL;

	return ret;
}  // del()


int
rasch_summary::add_ch(long ch, long fiducial_offset/*=0*/)
{
	return ra_sum_add_ch(_summary, ch, fiducial_offset);
} // add_ch()


long
rasch_summary::add_part(vector<long> event_ids)
{
	if (_summary == NULL)
		return -1;

	size_t num = event_ids.size();
	long *ids = new long[num];
	for (size_t l = 0; l < num; l++)
		ids[l] = event_ids[l];

	long part_id = ra_sum_add_part(_summary, (long)num, ids);

	delete[] ids;

	return part_id;
} // add_part()


int
rasch_summary::del_part(long part_id)
{
	return ra_sum_del_part(_summary, part_id);
} // del_part()


vector<long>
rasch_summary::get_part_all()
{
	vector<long> all;

	value_handle vh = ra_value_malloc();
	if (ra_sum_get_part_all(_summary, vh) != 0)
	{
		ra_value_free(vh);
		return all;
	}

	long num = ra_value_get_num_elem(vh);
	all.resize(num);
	const long *part_ids = ra_value_get_long_array(vh);
	for (long l = 0; l < num; l++)
		all[l] = part_ids[l];

	ra_value_free(vh);

	return all;
} // get_part_all()


int
rasch_summary::set_part_data(long part_id, long ch_idx, long dim, rasch_value value)
{
	value_handle vh = ra_value_malloc();
	value.fill_value_handle(vh);
	int ret = ra_sum_set_part_data(_summary, part_id, ch_idx, dim, vh);

	ra_value_free(vh);

	return ret;
} // set_part_data()


int
rasch_summary::get_part_data(long part_id, long ch_idx, long dim, rasch_value &value)
{
	value_handle vh = ra_value_malloc();
	int ret = ra_sum_set_part_data(_summary, part_id, ch_idx, dim, vh);
	if (ret)
		value.set_by_value_handle(vh);
	ra_value_free(vh);

	return ret;
} // get_part_data()


vector<long>
rasch_summary::get_part_events(long part_id)
{
	vector<long> events;

	value_handle vh = ra_value_malloc();
	if (ra_sum_get_part_events(_summary, part_id, vh) != 0)
	{
		ra_value_free(vh);
		return events;
	}

	long num = ra_value_get_num_elem(vh);
	events.resize(num);
	const long *ev = ra_value_get_long_array(vh);
	for (long l = 0; l < num; l++)
		events[l] = ev[l];

	ra_value_free(vh);

	return events;
} // get_part_events()


int
rasch_summary::set_part_events(long part_id, vector<long> events)
{
	value_handle vh = ra_value_malloc();
	rasch_value v(events);
	v.fill_value_handle(vh);

	int ret = ra_sum_set_part_events(_summary, part_id, vh);

	ra_value_free(vh);

	return ret;
} // set_part_events()

