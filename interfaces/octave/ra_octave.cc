/*----------------------------------------------------------------------------
 * ra_octave.cc
 *
 *
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002-2007, Raphael Schneider
 * See the file LICENSE for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#include <octave/config.h>

#include <iostream>

#include <octave/defun-dld.h>
#include <octave/error.h>
#include <octave/oct-obj.h>
#include <octave/pager.h>
#include <octave/symtab.h>
#include <octave/variables.h>

#include <ra.h>

#include "ra_octave.h"
#include "../shared/infos.h"


octave_value
_value_to_ov(value_handle vh)
{
	RowVector r(ra_value_get_num_elem(vh));
	long l, l_temp;
	octave_value ret = octave_value();
	if (!ra_value_is_ok(vh))
		return ret;

	const long *lp;
	const double *dp;
	const void **vp;

	switch (ra_value_get_type(vh))
	{
	case RA_VALUE_TYPE_SHORT:
		ret = octave_value((double)(ra_value_get_short(vh)));
		break;
	case RA_VALUE_TYPE_LONG:
		ret = octave_value((double)(ra_value_get_long(vh)));
		break;
	case RA_VALUE_TYPE_LONG_ARRAY:
		lp = ra_value_get_long_array(vh);
		for (l = 0; l < ra_value_get_num_elem(vh); l++)
			r(l) = (double)(lp[l]);
		ret = octave_value(r);
		break;
	case RA_VALUE_TYPE_DOUBLE:
		ret = octave_value(ra_value_get_double(vh));
		break;
	case RA_VALUE_TYPE_DOUBLE_ARRAY:
		dp = ra_value_get_double_array(vh);
		for (l = 0; l < ra_value_get_num_elem(vh); l++)
			r(l) = dp[l];
		ret = octave_value(r);
		break;
	case RA_VALUE_TYPE_CHAR:
		ret = octave_value(ra_value_get_string(vh));
		break;
	case RA_VALUE_TYPE_VOIDP:
		l_temp = (long)ra_value_get_voidp(vh);
		ret = octave_value((double)l_temp);
		break;
	case RA_VALUE_TYPE_VOIDP_ARRAY:
		vp = ra_value_get_voidp_array(vh);
		for (l = 0; l < ra_value_get_num_elem(vh); l++)
		{
			l_temp = (long)(vp[l]);
			r(l) = (double)l_temp;
		}
		ret = octave_value(r);
		break;
	}
	
	return ret;
}  // _value_to_ov()


value_handle
_ov_to_value(const char *type, octave_value ov)
{
	value_handle vh = ra_value_malloc();

	int ok = 1;	
	if (type[0] == 'p')  /* check if it's an array */
	{
		RowVector vec = ov.row_vector_value();
		long len = vec.length();
		
		if (type[1] == 's')  /* long array? */
		{
			short *all = new short[len];
			for (long l = 0; l < len; l++)
				all[l] = (short)(vec.elem(l));

			ra_value_set_short_array(vh, all, len);
			
			delete[] all;
		}
		else if (type[1] == 'l')  /* long array? */
		{
			long *all = new long[len];
			for (long l = 0; l < len; l++)
				all[l] = (long)(vec.elem(l));

			ra_value_set_long_array(vh, all, len);
			
			delete[] all;
		}
		else if (type[1] == 'd')  /* or double array? */
		{
			double *all = new double[len];
			for (long l = 0; l < len; l++)
				all[l] = vec.elem(l);

			ra_value_set_double_array(vh, all, len);
			
			delete[] all;
		}
	}
	else if (type[0] == 's') /* is it a short? */
	{
		short val = (short)(ov.double_value());		
		ra_value_set_short(vh, val);
	}
	else if (type[0] == 'l') /* is it a long? */
	{
		long val = (long)(ov.double_value());		
		ra_value_set_long(vh, val);
	}
	else if (type[0] == 'd') /* is it a double? */
	{
		double val = ov.double_value();		
		ra_value_set_double(vh, val);
	}
	else if (type[0] == 'c') /* is it a string? */
	{
		std::string val = ov.string_value();
		val += '\0';
		ra_value_set_string(vh, (const char *)val.data());
	}
	else
		ok = 0;
	
	if (!ok)
	{
		ra_value_free(vh);
		vh = NULL;
	}	

	return vh;
} // _ov_to_value()


octave_value_list
_info_get(const octave_value_list &args, value_handle vh, int inf_idx)
{
	long h = (long)(args(1).double_value());
	std::string inf_name = args(inf_idx).string_value();
	
	if (ra_info_get_by_name((any_handle)h, (const char *)inf_name.data(), vh) != 0)
		return octave_value();
	
	octave_value_list retval;
	retval(0) = _value_to_ov(vh);
	retval(1) = ra_value_get_name(vh);
	retval(2) = ra_value_get_desc(vh);
	retval(3) = ra_value_info_modifiable(vh);

	return retval;
} // _info_get()


long
_get_info_type(const char *name)
{
	long type = -1;

	if (strcmp(name, "meas") == 0)
		type = RA_INFO_OBJECT;
	else if (strcmp(name, "rec") == 0)
		type = RA_INFO_REC_GENERAL;
	else if (strcmp(name, "dev") == 0)
		type = RA_INFO_REC_DEVICE;
	else if (strcmp(name, "ch") == 0)
		type = RA_INFO_REC_CHANNEL;

	return type;
} // _get_info_type()


octave_value_list
if_ra_lib_init(const octave_value_list &args)
{
	octave_value_list retval(1, octave_value());
	
	long ra = (long)ra_lib_init();
	retval(0) = octave_value((double)ra);
	
	return retval;
}  // if_ra_lib_init()


octave_value_list
if_ra_lib_close(const octave_value_list &args)
{
	octave_value_list retval(0, octave_value());
	
	long ra = (long)(args(1).double_value());
	ra_lib_close((ra_handle)ra);
	
	return retval;
}  // if_ra_lib_close()


octave_value_list
if_ra_lib_get_error(const octave_value_list &args)
{
	octave_value_list retval(0, octave_value());
	
	long ra = (long)(args(1).double_value());
	
	char err_text[200];
	long err_num = ra_lib_get_error((ra_handle)ra, err_text, 200);
	
	retval(0) = octave_value((double)err_num);
	retval(1) = err_text;
	
	return retval;
} // if_ra_lib_get_error()


octave_value_list
if_ra_lib_handle_from_any_handle(const octave_value_list &args)
{
	octave_value_list retval(1, octave_value());
	
	long meas = (long)(args(1).double_value());
	long ra = (long)ra_lib_handle_from_any_handle((meas_handle)meas);
	
	retval(0) = octave_value((double)ra);
	
	return retval;
}  // if_ra_lib_handle_from_any_handle()


octave_value_list
if_ra_lib_set_option(const octave_value_list &args)
{
	proc_handle pi = (proc_handle)(long)(args(1).double_value());
	std::string opt_name = args(2).string_value();
	std::string opt_type = args(3).string_value();

	value_handle vh = _ov_to_value((const char *)opt_type.data(), args(4));
	int ret = -1;
	if (vh)
	{
		ret = ra_lib_set_option(pi, (const char *)opt_name.data(), vh);
		ra_value_free(vh);
	}

	octave_value_list retval;
	retval(0) = octave_value((double)ret);

	return retval;
} // if_ra_lib_set_option()


octave_value_list
if_ra_lib_get_option(const octave_value_list &args)
{
	any_handle h = (proc_handle)(long)(args(1).double_value());
	std::string opt_name = args(2).string_value();

	octave_value_list retval;

	value_handle vh = ra_value_malloc();
	if (ra_lib_get_option(h, (const char *)opt_name.data(), vh) != 0)
	{
		ra_value_free(vh);
		return octave_value();
	}
	if (!ra_value_is_ok(vh))
	{
		ra_value_free(vh);
		return octave_value();
	}

	retval(0) = _value_to_ov(vh);

	ra_value_free(vh);

	return retval;
} // if_ra_lib_get_option()


octave_value_list
if_ra_lib_info_get(const octave_value_list &args)
{
	value_handle vh = ra_value_malloc();
	octave_value_list ret = _info_get(args, vh, 2);
	ra_value_free(vh);

	return ret;
}  // if_ra_lib_info_get()


octave_value_list
if_ra_plugin_info_get(const octave_value_list &args)
{
	long num = (long)(args(2).double_value());

	value_handle vh = ra_value_malloc();
	ra_value_set_number(vh, num);

	octave_value_list ret = _info_get(args, vh, 3);
	ra_value_free(vh);

	return ret;
}  // if_ra_plugin_info_get()


octave_value_list
if_ra_result_info_get(const octave_value_list &args)
{
	long res_num = (long)(args(2).double_value());

	value_handle vh = ra_value_malloc();
	ra_value_set_number(vh, res_num);

	octave_value_list ret = _info_get(args, vh, 3);
	ra_value_free(vh);

	return ret;
}  // if_ra_result_info_get()


octave_value_list
if_ra_meas_info_get(const octave_value_list &args)
{
	value_handle vh = ra_value_malloc();
	octave_value_list ret = _info_get(args, vh, 2);
	ra_value_free(vh);

	return ret;
}  // if_ra_meas_info_get()


octave_value_list
if_ra_obj_info_get(const octave_value_list &args)
{
	value_handle vh = ra_value_malloc();
	octave_value_list ret = _info_get(args, vh, 2);
	ra_value_free(vh);

	return ret;
}  // if_ra_obj_info_get()


octave_value_list
if_ra_rec_info_get(const octave_value_list &args)
{
	value_handle vh = ra_value_malloc();
	octave_value_list ret = _info_get(args, vh, 2);
	ra_value_free(vh);

	return ret;
}  // if_ra_rec_info_get()


octave_value_list
if_ra_dev_info_get(const octave_value_list &args)
{
	long dev_num = (long)(args(2).double_value());

	value_handle vh = ra_value_malloc();
	ra_value_set_number(vh, dev_num);

	octave_value_list ret = _info_get(args, vh, 3);
	ra_value_free(vh);

	return ret;
}  // if_ra_dev_info_get()


octave_value_list
if_ra_ch_info_get(const octave_value_list &args)
{
	long ch = (long)(args(2).double_value());

	value_handle vh = ra_value_malloc();
	ra_value_set_number(vh, ch);

	octave_value_list ret = _info_get(args, vh, 3);
	ra_value_free(vh);

	return ret;
}  // if_ra_ch_info_get()


octave_value_list
if_ra_eval_info_get(const octave_value_list &args)
{
	value_handle vh = ra_value_malloc();
	octave_value_list ret = _info_get(args, vh, 2);
	ra_value_free(vh);

	return ret;
}  // if_ra_eval_info_get()


octave_value_list
if_ra_class_info_get(const octave_value_list &args)
{
	value_handle vh = ra_value_malloc();
	octave_value_list ret = _info_get(args, vh, 2);
	ra_value_free(vh);

	return ret;
}  // if_ra_class_info_get()


octave_value_list
if_ra_prop_info_get(const octave_value_list &args)
{
	value_handle vh = ra_value_malloc();
	octave_value_list ret = _info_get(args, vh, 2);
	ra_value_free(vh);

	return ret;
}  // if_ra_prop_info_get()


octave_value_list
if_ra_summary_info_get(const octave_value_list &args)
{
	value_handle vh = ra_value_malloc();
	octave_value_list ret = _info_get(args, vh, 2);
	ra_value_free(vh);

	return ret;
} // if_ra_summary_info_get()


octave_value_list
if_ra_sum_dim_info_get(const octave_value_list &args)
{
	long dim_num = (long)(args(2).double_value());

	value_handle vh = ra_value_malloc();
	ra_value_set_number(vh, dim_num);

	octave_value_list ret = _info_get(args, vh, 3);
	ra_value_free(vh);

	return ret;
} // if_ra_sum_dim_info_get()


octave_value_list
if_ra_sum_ch_info_get(const octave_value_list &args)
{
	long ch = (long)(args(2).double_value());

	value_handle vh = ra_value_malloc();
	ra_value_set_number(vh, ch);

	octave_value_list ret = _info_get(args, vh, 3);
	ra_value_free(vh);

	return ret;
} // if_ra_sum_ch_info_get()


octave_value_list
if_ra_info_get_by_idx(const octave_value_list &args)
{
	long any = (long)(args(1).double_value());
	std::string type_name = args(2).string_value();
	long idx = (long)(args(3).double_value());

	long type = _get_info_type((char *)type_name.data());
	if (type < 0)
		return octave_value();

	value_handle vh = ra_value_malloc();
	if (ra_info_get_by_idx((any_handle)any, type, idx, vh) != 0)
		return octave_value();
	
	octave_value_list retval;
	retval(0) = _value_to_ov(vh);
	retval(1) = ra_value_get_name(vh);
	retval(2) = ra_value_get_desc(vh);
	
	ra_value_free(vh);
	
	return retval;
}  // if_ra_info_get_by_idx()


octave_value_list
if_ra_info_set(const octave_value_list &args)
{
	octave_value_list retval(0, octave_value());
	return retval;
}  // if_ra_info_set()


octave_value_list
if_ra_meas_handle_from_any_handle(const octave_value_list &args)
{
	octave_value_list retval(1, octave_value());
	
	long rec = (long)(args(1).double_value());
	long meas = (long)ra_meas_handle_from_any_handle((any_handle)rec);
	
	retval(0) = octave_value((double)meas);
	
	return retval;
}  // if_ra_meas_handle_from_any_handle()


octave_value_list
if_ra_meas_find(const octave_value_list &args)
{
	long ra = (long)(args(1).double_value());
	std::string dir = args(2).string_value();
	
	// get all signal-names
	char **sigs_found = NULL;
	char **plugins = NULL;
	long n_sigs = 0;
	struct ra_find_struct rfs;
	ra_find_handle mfh = ra_meas_find_first((ra_handle)ra, (const char *)dir.data(), &rfs);
	if (mfh)
	{
		do
		{
			++n_sigs;
			sigs_found = (char **)realloc(sigs_found, sizeof(char *) * n_sigs);
			sigs_found[n_sigs-1] = new char[strlen(rfs.name)+1];
			strcpy(sigs_found[n_sigs-1], rfs.name);

			plugins = (char **)realloc(plugins, sizeof(char *) * n_sigs);
			plugins[n_sigs-1] = new char[strlen(rfs.plugin)+1];
			strcpy(plugins[n_sigs-1], rfs.plugin);
		} while (ra_meas_find_next(mfh, &rfs));
		ra_meas_close_find(mfh);
	}
	
	string_vector sv_fn(n_sigs);
	string_vector sv_pl(n_sigs);
	for (long l = 0; l < n_sigs; l++)
	{
		std::string s = sigs_found[l];
		sv_fn(l) = s;
		delete[] sigs_found[l];

		s = plugins[l];
		sv_pl(l) = s;
		delete[] plugins[l];
	}
	free(sigs_found);
	free(plugins);
	
	octave_value_list retval;
	retval(0) = octave_value(sv_fn);
	retval(1) = octave_value(sv_pl);
	
	return retval;
}  // if_ra_meas_find()


octave_value_list
if_ra_meas_open(const octave_value_list &args)
{
	long ra = (long)(args(1).double_value());
	std::string fn = args(2).string_value();
	std::string eval = args(3).string_value();
	long fast = (long)(args(4).double_value());
	
	long meas = (long)ra_meas_open((ra_handle)ra, (const char *)fn.data(), (const char *)eval.data(), fast);
	
	octave_value_list retval;
	retval(0) = octave_value((double)meas);
	
	return retval;
}  // if_ra_meas_open()


octave_value_list
if_ra_meas_new(const octave_value_list &args)
{
	long ra = (long)(args(1).double_value());
	std::string dir = args(2).string_value();
	std::string name = args(3).string_value();
	
	long meas =(long) ra_meas_new((ra_handle)ra, (const char *)dir.data(), (const char *)name.data());
	
	octave_value_list retval;
	retval(0) = octave_value((double)meas);
	
	return retval;
}  // if_ra_meas_new()


octave_value_list
if_ra_meas_save(const octave_value_list &args)
{
	long meas = (long)(args(1).double_value());
	ra_meas_save((meas_handle)meas);
	
	octave_value_list retval(0, octave_value());
	
	return retval;
}  // if_ra_meas_save()


octave_value_list
if_ra_meas_close(const octave_value_list &args)
{
	long meas = (long)(args(1).double_value());
	ra_meas_close((meas_handle)meas);
	
	octave_value_list retval(0, octave_value());
	
	return retval;
}  // if_ra_meas_close()


octave_value_list
if_ra_meas_copy(const octave_value_list &args)
{
	long mh = (long)(args(1).double_value());
	std::string dir = args(2).string_value();
	
	int ret = ra_meas_copy((meas_handle)mh, (const char *)dir.data());
	
	octave_value_list retval(1, octave_value());
	retval(0) = octave_value((double)ret);
	
	return retval;
}  // if_ra_meas_copy()


octave_value_list
if_ra_meas_move(const octave_value_list &args)
{
	long mh = (long)(args(1).double_value());
	std::string dir = args(2).string_value();
	
	mh = (long)ra_meas_move((meas_handle)mh, (const char *)dir.data());
	
	octave_value_list retval(1, octave_value());
	retval(0) = octave_value((double)mh);
	
	return retval;
}  // if_ra_meas_move()


octave_value_list
if_ra_meas_delete(const octave_value_list &args)
{
	long mh = (long)(args(1).double_value());

	int ret = ra_meas_delete((meas_handle)mh);
	
	octave_value_list retval(1, octave_value());
	retval(0) = octave_value((double)ret);
	
	return retval;
}  // if_ra_meas_delete()


octave_value_list
if_ra_rec_get_first(const octave_value_list &args)
{
	long meas = (long)(args(1).double_value());
	long session = (long)(args(2).double_value());
	
	long rec =(long) ra_rec_get_first((meas_handle)meas, session);
	
	octave_value_list retval;
	retval(0) = octave_value((double)rec);
	
	return retval;
}  // if_ra_rec_get_first()


octave_value_list
if_ra_rec_get_next(const octave_value_list &args)
{
	long rec = (long)(args(1).double_value());
	
	long rec_next =(long) ra_rec_get_next((rec_handle)rec);
	
	octave_value_list retval;
	retval(0) = octave_value((double)rec_next);
	
	return retval;
}  // if_ra_rec_get_next()


octave_value_list
if_ra_rec_get_first_child(const octave_value_list &args)
{
	long rec = (long)(args(1).double_value());
	
	long rec_child =(long) ra_rec_get_first_child((rec_handle)rec);
	
	octave_value_list retval;
	retval(0) = octave_value((double)rec_child);
	
	return retval;
}  // if_ra_rec_get_first_child()


octave_value_list
if_ra_raw_get(const octave_value_list &args)
{
	long rec = (long)(args(1).double_value());
	unsigned int ch = (unsigned int)(args(2).double_value());
	size_t start = (size_t)(args(3).double_value());
	size_t num_data = (size_t)(args(4).double_value());
	
	DWORD *buf = new DWORD[num_data];
	num_data = ra_raw_get((rec_handle)rec, ch, start, num_data, buf, NULL);
	
	RowVector rv(num_data);
	for (size_t l = 0; l < num_data; l++)
		rv(l) = buf[l];
	delete[] buf;
	
	octave_value_list retval;
	retval(0) = octave_value(rv);
	
	return retval;
}  // if_ra_raw_get()


octave_value_list
if_ra_raw_get_unit(const octave_value_list &args)
{
	long rec = (long)(args(1).double_value());
	unsigned int ch = (unsigned int)(args(2).double_value());
	size_t start = (size_t)(args(3).double_value());
	size_t num_data = (size_t)(args(4).double_value());
	
	double *buf = new double[num_data];
	num_data = ra_raw_get_unit((rec_handle)rec, ch, start, num_data, buf);
	
	RowVector rv(num_data);
	for (size_t l = 0; l < num_data; l++)
		rv(l) = buf[l];
	delete[] buf;
	
	octave_value_list retval;
	retval(0) = octave_value(rv);
	
	return retval;
}  // if_ra_raw_get_unit()


octave_value_list
if_ra_lib_use_plugin(const octave_value_list &args)
{
	octave_value_list retval(0, octave_value());
	return retval;
}  // if_ra_lib_use_plugin()


octave_value_list
if_ra_plugin_get_by_num(const octave_value_list &args)
{
	long ra = (long)(args(1).double_value());
	int num = (int)(args(2).double_value());
	int search_all = (int)(args(3).double_value());
	
	long ph = (long)ra_plugin_get_by_num((ra_handle)ra, num, search_all);
	
	octave_value_list retval(1, octave_value());
	retval(0) = octave_value((double)ph);
	
	return retval;
}  // if_ra_plugin_by_idx()


octave_value_list
if_ra_plugin_get_by_name(const octave_value_list &args)
{
	long ra = (long)(args(1).double_value());
	std::string pl_name = args(2).string_value();
	int search_all = (int)(args(3).double_value());

	long ph = (long)ra_plugin_get_by_name((ra_handle)ra, (const char *)pl_name.data(), search_all);
	
	octave_value_list retval(1, octave_value());
	retval(0) = octave_value((double)ph);
	
	return retval;
}  // if_ra_plugin_get_by_name()


octave_value_list
if_ra_proc_get(const octave_value_list &args)
{
	meas_handle meas = (meas_handle)(long)(args(1).double_value());
	plugin_handle pl = (plugin_handle)(long)(args(2).double_value());
	
	proc_handle proc = ra_proc_get(meas, pl, NULL);

	octave_value_list retval;
	retval(0) = octave_value((double)((long)proc));

	return retval;
} // if_ra_proc_get()


octave_value_list
if_ra_proc_do(const octave_value_list &args)
{
	proc_handle pi = (proc_handle)(long)(args(1).double_value());

	int res = ra_proc_do(pi);

	octave_value_list retval;
	retval(0) = octave_value((double)res);

	return retval;
}  // if_ra_proc_do()


octave_value_list
if_ra_proc_get_result_idx(const octave_value_list &args)
{
	proc_handle pi = (proc_handle)(long)(args(1).double_value());
	std::string res_ascii_id = args(2).string_value();
	
	long idx = ra_proc_get_result_idx(pi, (const char *)res_ascii_id.data());

	/* arrays are one-based and not zero-based */
	if (idx != -1)
		idx++;

	octave_value_list retval;
	retval(0) = octave_value((double)idx);

	return retval;
}  // if_ra_proc_get_result_idx()


octave_value_list
if_ra_proc_get_result(const octave_value_list &args)
{
	proc_handle pi = (proc_handle)(long)(args(1).double_value());
	plugin_handle pl = (plugin_handle)(long)(args(2).double_value());

	value_handle vh = ra_value_malloc();

	long n_res = 0;
	if (ra_info_get(pl, RA_INFO_PL_NUM_RESULTS_L, vh) == 0)
		n_res = ra_value_get_long(vh);

	octave_value_list values;
	string_vector names(n_res);
	string_vector descriptions(n_res);
	for (int i = 0; i < n_res; i++)
	{
		// TODO: handle result-sets
		if (ra_proc_get_result(pi, i, 0, vh) == 0)
		{
			if (ra_value_is_ok(vh))
				values(i) = _value_to_ov(vh);
			else
				values(i) = octave_value();
		}
		else
			values(i) = octave_value();

		if (ra_value_get_name(vh))
			names(i) = ra_value_get_name(vh);
		else
			names(i) = "--";

		if (ra_value_get_desc(vh))
			descriptions(i) = ra_value_get_desc(vh);
		else
			descriptions(i) = "--";
	}

	ra_value_free(vh);
	octave_value_list retval;
	retval(0) = octave_value(values);
	retval(1) = octave_value(names);
	retval(2) = octave_value(descriptions);

	return retval;
}  // if_ra_proc_get_result()


octave_value_list
if_ra_proc_free(const octave_value_list &args)
{
	proc_handle pi = (proc_handle)(long)(args(1).double_value());

	if (pi)
		ra_proc_free(pi);

	return octave_value();
} // if_ra_proc_free()


octave_value_list
if_ra_eval_save(const octave_value_list &args)
{
	long meas = (long)(args(1).double_value());
	std::string fn = args(2).string_value();
	int use_ascii = (int)(args(3).double_value());
	
	int ret = ra_eval_save((meas_handle)meas, (const char *)fn.data(), use_ascii);
	
	octave_value_list retval;
	retval(0) = octave_value((double)ret);
	
	return retval;
}  // if_ra_eval_save()


octave_value_list
if_ra_eval_attribute_list(const octave_value_list &args)
{
	long h = (long)(args(1).double_value());
	value_handle vh = ra_value_malloc();
	
	if (ra_eval_attribute_list((any_handle)h, vh) != 0)
	{
		ra_value_free(vh);
		return octave_value();
	}
	
	octave_value_list retval = _value_to_ov(vh);
	
	ra_value_free(vh);
	
	return retval;
} // if_ra_eval_attribute_list()


octave_value_list
if_ra_eval_attribute_get(const octave_value_list &args)
{
	long h = (long)(args(1).double_value());
	std::string attr = args(2).string_value();
	
	value_handle vh = ra_value_malloc();
	
	if (ra_eval_attribute_get((any_handle)h, (const char *)attr.data(), vh) != 0)
	{
		ra_value_free(vh);
		return octave_value();
	}
	
	octave_value_list retval = _value_to_ov(vh);
	
	ra_value_free(vh);
	
	return retval;
} // if_ra_eval_attribute_get()


octave_value_list
if_ra_eval_attribute_set(const octave_value_list &args)
{
	long h = (long)(args(1).double_value());
	std::string attr = args(2).string_value();
	std::string opt_type = args(3).string_value();

	value_handle vh = _ov_to_value((const char *)opt_type.data(), args(4));
	if (vh)
	{
		ra_eval_attribute_set((any_handle)h, (const char *)attr.data(), vh);
		ra_value_free(vh);
	}

	return octave_value();
} // if_ra_eval_attribute_set()


octave_value_list
if_ra_eval_attribute_unset(const octave_value_list &args)
{
	long h = (long)(args(1).double_value());
	std::string attr = args(2).string_value();
	
	ra_eval_attribute_unset((any_handle)h, (const char *)attr.data());
	
	return octave_value();
} // if_ra_eval_attribute_unset()


octave_value_list
if_ra_eval_add(const octave_value_list &args)
{
	long h = (long)(args(1).double_value());
	std::string name = args(2).string_value();
	std::string desc = args(3).string_value();
	int original = (int)(args(4).double_value());

	long eh = (long)ra_eval_add((meas_handle)h, (const char *)name.data(),
				    (const char *)desc.data(), original);

	octave_value_list retval;
	retval(0) = octave_value((double)eh);

	return retval;
}  // if_ra_eval_add()


octave_value_list
if_ra_eval_delete(const octave_value_list &args)
{
	long h = (long)(args(1).double_value());

	ra_eval_delete((eval_handle)h);

	return octave_value();
} // if_ra_eval_delete()


octave_value_list
if_ra_eval_get_all(const octave_value_list &args)
{
	long meas = (long)(args(1).double_value());
	
	value_handle vh = ra_value_malloc();
	if (ra_eval_get_all((meas_handle)meas, vh) != 0)
	{
		ra_value_free(vh);
		return octave_value();
	}
	
	octave_value_list retval;
	retval(0) = _value_to_ov(vh);
	
	ra_value_free(vh);
	
	return retval;
}  // if_ra_eval_get_all()


octave_value_list
if_ra_eval_get_original(const octave_value_list &args)
{
	long meas = (long)(args(1).double_value());

	long eval = (long)ra_eval_get_original((meas_handle)meas);

	octave_value_list retval;
	retval(0) = octave_value((double)eval);
	
	return retval;
}  // if_ra_eval_get_original()


octave_value_list
if_ra_eval_get_default(const octave_value_list &args)
{
	long meas = (long)(args(1).double_value());

	long eval = (long)ra_eval_get_default((meas_handle)meas);

	octave_value_list retval;
	retval(0) = octave_value((double)eval);
	
	return retval;
}  // if_ra_eval_get_default()


octave_value_list
if_ra_eval_set_default(const octave_value_list &args)
{
	long h = (long)(args(1).double_value());

	ra_eval_set_default((eval_handle)h);

	return octave_value();
}  // if_ra_eval_set_default()


octave_value_list
if_ra_eval_get_handle(const octave_value_list &args)
{
	long h = (long)(args(1).double_value());

	long eval = (long)ra_eval_get_handle((class_handle)h);

	octave_value_list retval;
	retval(0) = octave_value((double)eval);
	
	return retval;	
} // if_ra_eval_get_handle()


octave_value_list
if_ra_class_add(const octave_value_list &args)
{
	long h = (long)(args(1).double_value());
	std::string id = args(2).string_value();
	std::string name = args(3).string_value();
	std::string desc = args(4).string_value();

	long clh = (long)ra_class_add((eval_handle)h, (const char *)id.data(), (const char *)name.data(),
				      (const char *)desc.data());

	octave_value_list retval;
	retval(0) = octave_value((double)clh);

	return retval;
} // if_ra_class_add()


octave_value_list
if_ra_class_add_predef(const octave_value_list &args)
{
	long h = (long)(args(1).double_value());
	std::string id = args(2).string_value();

	long clh = (long)ra_class_add_predef((eval_handle)h, (const char *)id.data());

	octave_value_list retval;
	retval(0) = octave_value((double)clh);

	return retval;
} // if_ra_class_add_predef()


octave_value_list
if_ra_class_delete(const octave_value_list &args)
{
	long h = (long)(args(1).double_value());

	ra_eval_delete((class_handle)h);

	return octave_value();	
} // if_ra_class_delete()


octave_value_list
if_ra_class_get(const octave_value_list &args)
{
	long eval = (long)(args(1).double_value());
	std::string id = args(2).string_value();

	value_handle vh = ra_value_malloc();
	if (ra_class_get((eval_handle)eval, (const char *)id.data(), vh) != 0)
	{
		ra_value_free(vh);
		return octave_value();
	}

	octave_value_list retval;
	retval(0) = _value_to_ov(vh);

	ra_value_free(vh);

	return retval;
} // if_ra_class_get()


octave_value_list
if_ra_class_add_event(const octave_value_list &args)
{
	long h = (long)(args(1).double_value());
	long start = (long)(args(2).double_value());
	long end = (long)(args(3).double_value());

	long ev_id = ra_class_add_event((class_handle)h, start, end);

	octave_value_list retval;
	retval(0) = octave_value((double)ev_id);

	return retval;
} // if_ra_class_add_event()


octave_value_list
if_ra_class_del_event(const octave_value_list &args)
{
	long h = (long)(args(1).double_value());
	long ev_id = (long)(args(2).double_value());

	ra_class_del_event((class_handle)h, ev_id);

	return octave_value();
} // if_ra_class_del_event()


octave_value_list
if_ra_class_get_event_pos(const octave_value_list &args)
{
	long h = (long)(args(1).double_value());	
	long ev_id = (long)(args(2).double_value());
	long start, end;

	if (ra_class_get_event_pos((class_handle)h, ev_id, &start, &end) != 0)
		return octave_value();

	octave_value_list retval;
	retval(0) = octave_value((double)start);
	retval(1) = octave_value((double)end);

	return retval;
} // if_ra_class_get_event_pos()


octave_value_list
if_ra_class_set_event_pos(const octave_value_list &args)
{
	long h = (long)(args(1).double_value());	
	long ev_id = (long)(args(2).double_value());
	long start = (long)(args(3).double_value());
	long end = (long)(args(4).double_value());

	ra_class_set_event_pos((class_handle)h, ev_id, start, end);

	return octave_value();
} // if_ra_class_set_event_pos()


octave_value_list
if_ra_class_get_events(const octave_value_list &args)
{
	long h = (long)(args(1).double_value());	
	long start = (long)(args(2).double_value());
	long end = (long)(args(3).double_value());
	int complete = (int)(args(4).double_value());
	int sort = (int)(args(5).double_value());

	value_handle vh = ra_value_malloc();
	if (ra_class_get_events((class_handle)h, start, end, complete, sort, vh) != 0)
	{
		ra_value_free(vh);
		return octave_value();
	}

	octave_value_list retval;
	retval(0) = _value_to_ov(vh);

	ra_value_free(vh);

	return retval;
} // if_ra_class_get_events()


octave_value_list
if_ra_class_get_prev_event(const octave_value_list &args)
{
	long h = (long)(args(1).double_value());	
	long ev_id = (long)(args(2).double_value());

	long prev_ev = ra_class_get_prev_event((class_handle)h, ev_id);

	octave_value_list retval;
	retval(0) = octave_value((double)prev_ev);

	return retval;
} // if_ra_class_get_prev_event()


octave_value_list
if_ra_class_get_next_event(const octave_value_list &args)
{
	long h = (long)(args(1).double_value());	
	long ev_id = (long)(args(2).double_value());

	long next_ev = ra_class_get_next_event((class_handle)h, ev_id);

	octave_value_list retval;
	retval(0) = octave_value((double)next_ev);

	return retval;
} // if_ra_class_get_next_event()


octave_value_list
if_ra_class_get_handle(const octave_value_list &args)
{
	long h = (long)(args(1).double_value());

	long clh = (long)ra_class_get_handle((prop_handle)h);

	octave_value_list retval;
	retval(0) = octave_value((double)clh);
	
	return retval;	
} // if_ra_class_get_handle()


octave_value_list
if_ra_prop_add(const octave_value_list &args)
{
	long h = (long)(args(1).double_value());
	std::string id = args(2).string_value();
	long value_type = (long)(args(3).double_value());
	std::string name = args(4).string_value();
	std::string desc = args(5).string_value();
	std::string unit = args(6).string_value();
	int use_minmax = (int)(args(7).double_value());
	double min = args(8).double_value();
	double max = args(9).double_value();
	int has_ignore_value = (int)(args(10).double_value());
	double ignore_value = args(11).double_value();

	long prop = (long)ra_prop_add((class_handle)h, (const char *)id.data(), value_type,
				      (const char *)name.data(), (const char *)desc.data(),
				      (const char *)unit.data(), use_minmax, min, max,
				      has_ignore_value, ignore_value);

	octave_value_list retval;
	retval(0) = octave_value((double)prop);

	return retval;
} // if_ra_prop_add()


octave_value_list
if_ra_prop_add_predef(const octave_value_list &args)
{
	long h = (long)(args(1).double_value());
	std::string id = args(2).string_value();

	long prop = (long)ra_prop_add_predef((class_handle)h, (const char *)id.data());

	octave_value_list retval;
	retval(0) = octave_value((double)prop);

	return retval;
} // if_ra_prop_add_predef


octave_value_list
if_ra_prop_delete(const octave_value_list &args)
{
	long h = (long)(args(1).double_value());

	ra_eval_delete((prop_handle)h);

	return octave_value();	
} // if_ra_prop_delete()


octave_value_list
if_ra_prop_get(const octave_value_list &args)
{
	long h = (long)(args(1).double_value());
	std::string name = args(2).string_value();

	long prop = (long)ra_prop_get((class_handle)h, (const char *)name.data());

	octave_value_list retval;
	retval(0) = octave_value((double)prop);
	
	return retval;
} // if_ra_prop_get()


octave_value_list
if_ra_prop_get_all(const octave_value_list &args)
{
	long h = (long)(args(1).double_value());

	value_handle vh = ra_value_malloc();
	if (ra_prop_get_all((class_handle)h, vh) != 0)
	{
		ra_value_free(vh);
		return octave_value();
	}

	octave_value_list retval;
	retval(0) = _value_to_ov(vh);

	ra_value_free(vh);
	
	return retval;
} // if_ra_prop_get_all()


octave_value_list
if_ra_prop_set_value(const octave_value_list &args)
{
	long h = (long)(args(1).double_value());
	long ev_id = (long)(args(2).double_value());
	long ch = (long)(args(3).double_value());
	std::string opt_type = args(4).string_value();
	
	value_handle vh = _ov_to_value((const char *)opt_type.data(), args(5));
	if (vh)
	{
		ra_prop_set_value((prop_handle)h, ev_id, ch, vh);
		ra_value_free(vh);
	}

	return octave_value();
} // if_ra_prop_set_value()


octave_value_list
if_ra_prop_set_value_mass(const octave_value_list &args)
{
	// TODO: think if this is really necessary and if how to
	// be sure that the id and ch vectors are correct
	return octave_value();
} // if_ra_prop_set_value_mass()


octave_value_list
if_ra_prop_get_ch(const octave_value_list &args)
{
	long h = (long)(args(1).double_value());	
	long ev_id = (long)(args(2).double_value());

	value_handle vh = ra_value_malloc();
	if (ra_prop_get_ch((prop_handle)h, ev_id, vh) != 0)
	{
		ra_value_free(vh);
		return octave_value();
	}

	octave_value_list retval;
	retval(0) = _value_to_ov(vh);

	ra_value_free(vh);

	return retval;
} // if_ra_prop_get_ch()


octave_value_list
if_ra_prop_get_value(const octave_value_list &args)
{
	long h = (long)(args(1).double_value());	
	long ev_id = (long)(args(2).double_value());
	long ch = (long)(args(3).double_value());

	value_handle vh = ra_value_malloc();
	if (ra_prop_get_value((prop_handle)h, ev_id, ch, vh) != 0)
	{
		ra_value_free(vh);
		return octave_value();
	}

	octave_value_list retval;
	retval(0) = _value_to_ov(vh);

	ra_value_free(vh);

	return retval;
} // if_ra_prop_get_value()


octave_value_list
if_ra_prop_get_value_all(const octave_value_list &args)
{
	long h = (long)(args(1).double_value());

	value_handle event_id = ra_value_malloc();
	value_handle ch = ra_value_malloc();
	value_handle value = ra_value_malloc();

	if (ra_prop_get_value_all((prop_handle)h, event_id, ch, value) != 0)
	{
		ra_value_free(event_id);
		ra_value_free(ch);
		ra_value_free(value);

		return octave_value();
	}

	octave_value_list retval;
	retval(0) = _value_to_ov(event_id);
	retval(1) = _value_to_ov(ch);
	retval(2) = _value_to_ov(value);

	ra_value_free(event_id);
	ra_value_free(ch);
	ra_value_free(value);

	return retval;
} // if_ra_prop_get_value_all()


octave_value_list
if_ra_prop_get_events(const octave_value_list &args)
{
	long h = (long)(args(1).double_value());	
	std::string minmax_type = args(2).string_value();
	value_handle min = _ov_to_value((const char *)minmax_type.data(), args(3));
	value_handle max = _ov_to_value((const char *)minmax_type.data(), args(4));
	long ch = (long)(args(5).double_value());

	value_handle vh = ra_value_malloc();
	if (ra_prop_get_events((prop_handle)h, min, max, ch, vh) != 0)
	{
		ra_value_free(min);
		ra_value_free(max);
		return octave_value();
	}

	octave_value_list retval;
	retval(0) = _value_to_ov(vh);

	ra_value_free(vh);

	return retval;	
} // if_ra_prop_get_events()


octave_value_list
if_ra_sum_add(const octave_value_list &args)
{
	long h = (long)(args(1).double_value());
	std::string id = args(2).string_value();
	std::string name = args(3).string_value();
	std::string desc = args(4).string_value();
	long num_dim = (long)(args(5).double_value());
	string_vector dim_name = args(6).all_strings();
	string_vector dim_unit = args(7).all_strings();

	long sum = (long)ra_sum_add((class_handle)h, (const char *)id.data(), (const char *)name.data(),
				    (const char *)desc.data(), num_dim, (const char **)dim_name.c_str_vec(),
				    (const char **)dim_unit.c_str_vec());

	octave_value_list retval;
	retval(0) = octave_value((double)sum);

	return retval;
} // if_ra_sum_add()


octave_value_list
if_ra_sum_add_ch(const octave_value_list &args)
{
	long h = (long)(args(1).double_value());
	long ch = (long)(args(2).double_value());
	long fid_off = (long)(args(3).double_value());

	long ret = (long)ra_sum_add_ch((sum_handle)h, ch, fid_off);

	octave_value_list retval;
	retval(0) = octave_value((double)ret);

	return retval;
} // if_ra_sum_add_ch()


octave_value_list
if_ra_sum_delete(const octave_value_list &args)
{
	long h = (long)(args(1).double_value());

	long ret = (long)ra_sum_delete((sum_handle)h);

	octave_value_list retval;
	retval(0) = octave_value((double)ret);

	return retval;
} // if_ra_sum_delete()


octave_value_list
if_ra_sum_get(const octave_value_list &args)
{
	long h = (long)(args(1).double_value());
	std::string id = args(2).string_value();

	value_handle vh = ra_value_malloc();
	if (ra_sum_get((class_handle)h, (const char *)id.data(), vh) != 0)
	{
		ra_value_free(vh);
		return octave_value();
	}

	octave_value_list retval;
	retval(0) = _value_to_ov(vh);

	ra_value_free(vh);

	return retval;
} // if_ra_sum_get()


octave_value_list
if_ra_sum_add_part(const octave_value_list &args)
{
	long h = (long)(args(1).double_value());
	long num_events = (long)(args(2).double_value());
	Array<int> ev_ids_ov = args(3).int_vector_value();

	long *ev_ids = new long[num_events];
	for (long l = 0; l < num_events; l++)
		ev_ids[l] = (long)(ev_ids_ov.elem(l));

	long part_id = ra_sum_add_part((sum_handle)h, num_events, ev_ids);
	delete[] ev_ids;

	octave_value_list retval;
	retval(0) = octave_value((double)part_id);

	return retval;
} // if_ra_sum_add_part()


octave_value_list
if_ra_sum_del_part(const octave_value_list &args)
{
	long h = (long)(args(1).double_value());
	long part_id = (long)(args(2).double_value());

	long ret = (long)ra_sum_del_part((sum_handle)h, part_id);

	octave_value_list retval;
	retval(0) = octave_value((double)ret);

	return retval;
} // if_ra_sum_del_part()


octave_value_list
if_ra_sum_get_part_all(const octave_value_list &args)
{
	long h = (long)(args(1).double_value());

	value_handle vh = ra_value_malloc();
	if (ra_sum_get_part_all((class_handle)h, vh) != 0)
	{
		ra_value_free(vh);
		return octave_value();
	}

	octave_value_list retval;
	retval(0) = _value_to_ov(vh);

	ra_value_free(vh);

	return retval;
} // if_ra_sum_get_part_all()


octave_value_list
if_ra_sum_set_part_data(const octave_value_list &args)
{
	long h = (long)(args(1).double_value());
	long part_id = (long)(args(2).double_value());
	long ch_idx = (long)(args(3).double_value());
	long dim = (long)(args(4).double_value());
	std::string val_type = args(5).string_value();

	value_handle vh = _ov_to_value((const char *)val_type.data(), args(6));
	long ret = -1;
	if (vh)
	{
		ret = (long)ra_sum_set_part_data((sum_handle)h, part_id, ch_idx, dim, vh);
		ra_value_free(vh);
	}

	octave_value_list retval;
	retval(0) = octave_value((double)ret);

	return retval;
} // if_ra_sum_set_part_data()


octave_value_list
if_ra_sum_get_part_data(const octave_value_list &args)
{
	long h = (long)(args(1).double_value());
	long part_id = (long)(args(2).double_value());
	long ch_idx = (long)(args(3).double_value());
	long dim = (long)(args(4).double_value());

	value_handle vh = ra_value_malloc();
	if (ra_sum_get_part_data((sum_handle)h, part_id, ch_idx, dim, vh) != 0)
	{
		ra_value_free(vh);
		return octave_value();
	}

	octave_value_list retval;
	retval(0) = _value_to_ov(vh);

	ra_value_free(vh);

	return retval;
} // if_ra_sum_get_part_data()


octave_value_list
if_ra_sum_get_part_events(const octave_value_list &args)
{
	long h = (long)(args(1).double_value());
	long part_id = (long)(args(2).double_value());

	value_handle vh = ra_value_malloc();
	if (ra_sum_get_part_events((sum_handle)h, part_id, vh) != 0)
	{
		ra_value_free(vh);
		return octave_value();
	}

	octave_value_list retval;
	retval(0) = _value_to_ov(vh);

	ra_value_free(vh);

	return retval;
} // if_ra_sum_get_part_events()


octave_value_list
if_ra_sum_set_part_events(const octave_value_list &args)
{
	long h = (long)(args(1).double_value());
	long part_id = (long)(args(2).double_value());
	value_handle vh = _ov_to_value("l", args(3));

	int ret = ra_sum_set_part_events((sum_handle)h, part_id, vh);

	ra_value_free(vh);

	octave_value_list retval;
	retval(0) = octave_value((double)ret);

	return retval;
} // if_ra_sum_set_part_events()



DEFUN_DLD (ra_octave, args, ,
"[...] = ra_octave (...)\n\
\n\
Call functions from libRASCH.")
{
	/* Check for proper number of arguments */    
	int nargin = args.length();
	if (nargin <= 0)
	{
		octave_stdout << "libRASCH-Octave is corrupted. No func-name";
		return octave_value();
	}
	
	std::string func_name = args(0).string_value();
	func_name += '\0';
	
	int func_idx = -1;
	for (int i = 0; i < (int)(sizeof(f) / sizeof(f[0])); i++)
	{
		if (f[i].func_name == NULL)
			continue;
		if (strcmp(func_name.data(), f[i].func_name) == 0)
		{
			func_idx = i;
			break;
		}
		
	}
	if (func_idx == -1)
	{
		octave_stdout << "libRASCH-Octave is corrupted: func-name " << func_name <<
			" not found" << "\n";
		return octave_value();
	}
	
	if (nargin != (f[func_idx].n_args_in + 1))
	{
		octave_stdout << "func: " << func_name << "#in=" << nargin <<
			" (expected #in=" << (f[func_idx].n_args_in + 1) << ")\n";
		return octave_value();
	}
	
	octave_value_list (*func)(const octave_value_list &args);
	func = f[func_idx].func;
	octave_value_list retval = (*func)(args);
	
	return retval;
}  // ra_octave()
