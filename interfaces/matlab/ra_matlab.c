/*----------------------------------------------------------------------------
 * ra_matlab.c
 *
 * Implements interface of libRASCH for Matlab.
 *
 * Author(s): Raphael Schneider (librasch@gmail.com)
 *
 * Copyright (C) 2002-2009, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ra.h>
#include <mex.h>


#include "ra_matlab.h"
#include "../shared/infos.h"

/* prototypes of helper functions */
void *_get_handle(const mxArray *prhs[], int idx);
char *_get_string(const mxArray *prhs[], int idx);
short _get_short(const mxArray *prhs[], int idx);
long _get_long(const mxArray *prhs[], int idx);
double _get_double(const mxArray *prhs[], int idx);
short *_get_short_array(const mxArray *prhs[], int idx, long *num);
long *_get_long_array(const mxArray *prhs[], int idx, long *num);
double *_get_double_array(const mxArray *prhs[], int idx, long *num);
char **_get_string_array(const mxArray *prhs[], int idx, long *num);
void _set_handle(mxArray *plhs[], int idx, void *h);
void _set_long(mxArray *plhs[], int idx, long value);
void _set_dword(mxArray *plhs[], int idx, unsigned long value);
void _set_double(mxArray *plhs[], int idx, double value);
void _set_string(mxArray *plhs[], int idx, const char *d);
void _set_value(mxArray *plhs[], int idx, value_handle vh);
void _set_byte_array(mxArray *plhs[], int idx, unsigned char *buf, long num_data);
void _set_short_array(mxArray *plhs[], int idx, const short *buf, long num_data);
void _set_long_array(mxArray *plhs[], int idx, const long *buf, long num_data);
void _set_dword_array(mxArray *plhs[], int idx, const unsigned long *buf, long num_data);
void _set_double_array(mxArray *plhs[], int idx, const double *buf, long num_data);
void _set_string_array(mxArray *plhs[], int idx, const char **buf, long num_data);
void _set_handle_array(mxArray *plhs[], int idx, const void **buf, long num_data);
void _set_vh_array(mxArray *plhs[], int idx, const value_handle *buf, long num_data);
long _string_to_info_id(const char *name);
mxArray *_value_to_mxa(value_handle vh);
value_handle _mxa_to_value(const char *type, const mxArray *prhs[], long idx);
long _get_info_type(const char *name);


void *
_get_handle(const mxArray *prhs[], int idx)
{
	long *lp;
	void *h;
	
	lp = (long *)mxGetPr(prhs[idx]);
	h = (void *)*lp;
	
	return h;
}  /* _get_handle() */


char *
_get_string(const mxArray *prhs[], int idx)
{
	long len;
	char *s;
	int status;
	const mxArray *in;
	
	if (mxIsCell(prhs[idx]))
		in = mxGetCell(prhs[idx], 0);
	else
		in = prhs[idx];
	
	len = (mxGetM(in) * mxGetN(in)) + 1;
	s = mxCalloc(len, sizeof(char));
	status = mxGetString(in, s, len);
	
	return s;
}  /* _get_string() */


short
_get_short(const mxArray *prhs[], int idx)
{
	double temp;
	short v = 0;

	if (!mxIsNumeric(prhs[idx]))
		return v;

	temp = _get_double(prhs, idx);
	v = (short)temp;

	return v;
}  /*  _get_short() */


long
_get_long(const mxArray *prhs[], int idx)
{
	double temp;
	long v = 0;

	if (!mxIsNumeric(prhs[idx]))
		return v;

	temp = _get_double(prhs, idx);
	v = (long)temp;

	return v;
}  /*  _get_long() */


double
_get_double(const mxArray *prhs[], int idx)
{
	double *p;
	double v = 0.0;
	long temp = 0;

	if (!mxIsNumeric(prhs[idx]))
		return v;

	if (mxIsDouble(prhs[idx]))
	{
		p = mxGetPr(prhs[idx]);
		v = *p;
	}
	else
	{
		void *vp;
		mxClassID id;

		vp = mxGetData(prhs[idx]);
		id = mxGetClassID(prhs[idx]);

		switch (id)
		{
		case mxINT8_CLASS:
			v = (double)(*(char *)vp);
			break;
		case mxUINT8_CLASS:
			v = (double)(*(unsigned char *)vp);
			break;
		case mxINT16_CLASS:
			v = (double)(*(short *)vp);
			break;
		case mxUINT16_CLASS:
			v = (double)(*(unsigned short *)vp);
			break;
		case mxINT32_CLASS:
			v = (double)(*(long *)vp);
			break;
		case mxUINT32_CLASS:
			v = (double)(*(unsigned long *)vp);
			break;
		}
	}

	return v;
}  /*  _get_double() */


short *
_get_short_array(const mxArray *prhs[], int idx, long *num)
{
	long l;
	short *arr;
	double *m_data;

	*num = (mxGetM(prhs[idx]) * mxGetN(prhs[idx]));
	arr = mxCalloc(*num, sizeof(short));

	m_data = mxGetPr(prhs[idx]);

	for (l = 0; l < *num; l++)
		arr[l] = (short)(m_data[l]);

	return arr;
} /* _get_short_array() */


long *
_get_long_array(const mxArray *prhs[], int idx, long *num)
{
	long l;
	long *arr;
	double *m_data;

	*num = (mxGetM(prhs[idx]) * mxGetN(prhs[idx]));
	arr = mxCalloc(*num, sizeof(long));

	m_data = mxGetPr(prhs[idx]);

	for (l = 0; l < *num; l++)
		arr[l] = (long)(m_data[l]);

	return arr;
} /* _get_long_array() */


double *
_get_double_array(const mxArray *prhs[], int idx, long *num)
{
	long l;
	double *arr;
	double *m_data;

	*num = (mxGetM(prhs[idx]) * mxGetN(prhs[idx]));
	arr = mxCalloc(*num, sizeof(double));

	m_data = mxGetPr(prhs[idx]);

	for (l = 0; l < *num; l++)
		arr[l] = m_data[l];

	return arr;
} /* _get_double_array() */


char **
_get_string_array(const mxArray *prhs[], int idx, long *num)
{
	long l, len, status;
	char **arr;
	const mxArray *elem;

	*num = 0;
	arr = NULL;

	if (mxIsCell(prhs[idx]))
	{
		*num = (mxGetM(prhs[idx]) * mxGetN(prhs[idx]));
		arr = mxCalloc(*num, sizeof(char *));

		for (l = 0; l < *num; l++)
		{
			elem = mxGetCell(prhs[idx], l);
			len = (mxGetM(elem) * mxGetN(elem)) + 1;
			arr[l] = mxCalloc(len, sizeof(char));
			status = mxGetString(elem, arr[l], len);
		}
	}

	return arr;
} /* _get_double_array() */


void
_set_handle(mxArray *plhs[], int idx, void *h)
{
	long *yp;
	const mwSize dims[] = {1,1};

	/* Create a matrix for the return argument */ 
	plhs[idx] = mxCreateNumericArray(2, dims, mxUINT32_CLASS, mxREAL);    
	yp = (long *)mxGetPr(plhs[idx]);
	yp[0] = (long)h;
}  /* _set_handle() */


void
_set_long(mxArray *plhs[], int idx, long value)
{
	long *yp;
	const mwSize dims[] = {1,1};

	/* Create a matrix for the return argument */ 
	plhs[idx] = mxCreateNumericArray(2, dims, mxUINT32_CLASS, mxREAL);    
	yp = (long *)mxGetPr(plhs[idx]);
	yp[0] = value;
}  /* _set_long() */


void
_set_dword(mxArray *plhs[], int idx, unsigned long value)
{
	long *yp;
	const mwSize dims[] = {1,1};

	/* Create a matrix for the return argument */ 
	plhs[idx] = mxCreateNumericArray(2, dims, mxUINT32_CLASS, mxREAL);    
	yp = (long *)mxGetPr(plhs[idx]);
	yp[0] = value;
}  /* _set_dword() */


void
_set_double(mxArray *plhs[], int idx, double value)
{
	_set_double_array(plhs, idx, &value, 1);
}  /* _set_double() */


void
_set_string(mxArray *plhs[], int idx, const char *d)
{
	long *yp;

	/* Create a matrix for the return argument */ 
	plhs[idx] = mxCreateString(d);
	yp = (long *)mxGetPr(plhs[idx]);
}  /* _set_string() */


void
_set_value(mxArray *plhs[], int idx, value_handle vh)
{
	long l;
	double d;

	switch (ra_value_get_type(vh))  /* no breaks needed */
	{
	case RA_VALUE_TYPE_LONG:
		l = ra_value_get_long(vh);
		_set_long_array(plhs, idx, &l, 1);
		break;
	case RA_VALUE_TYPE_DOUBLE:
		d = ra_value_get_double(vh);
		_set_double_array(plhs, idx, &d, 1);
		break;
	case RA_VALUE_TYPE_CHAR:
		_set_string(plhs, idx, ra_value_get_string(vh));
		break;
	}
}  /* _set_value() */


void
_set_byte_array(mxArray *plhs[], int idx, unsigned char *buf, long num_data)
{
	double *d;
	long l;

	d = (double *)malloc(sizeof(double) * num_data);
	for (l = 0; l < num_data; l++)
		d[l] = buf[l];
	_set_double_array(plhs, idx, d, num_data);
	free(d);

	/* Because of problems with non-double values in Matlab, now
	   all values will be stored as double values. Keep the old
	   code in case it is needed in the future. */
/*	int dims[2];
	unsigned char *yp;
	long l;

	dims[0] = num_data;
	dims[1] = 1;
	plhs[idx] = mxCreateNumericArray(2, dims, mxUINT8_CLASS, mxREAL);    
	yp = (unsigned char *)mxGetPr(plhs[idx]);
	for (l = 0; l < num_data; l++)
		yp[l] = buf[l]; */
}  /* _set_byte_array() */


void
_set_short_array(mxArray *plhs[], int idx, const short *buf, long num_data)
{
	double *d;
	long l;

	d = (double *)malloc(sizeof(double) * num_data);
	for (l = 0; l < num_data; l++)
		d[l] = buf[l];
	_set_double_array(plhs, idx, d, num_data);
	free(d);

	/* Because of problems with non-double values in Matlab, now
	   all values will be stored as double values. Keep the old
	   code in case it is needed in the future. */
/*	int dims[2];
	unsigned short *yp;
	long l;

	dims[0] = num_data;
	dims[1] = 1;
	plhs[idx] = mxCreateNumericArray(2, dims, mxINT16_CLASS, mxREAL);    
	yp = (unsigned short *)mxGetPr(plhs[idx]);
	for (l = 0; l < num_data; l++)
		yp[l] = buf[l]; */
}  /* _set_short_array() */


void
_set_long_array(mxArray *plhs[], int idx, const long *buf, long num_data)
{
	double *d;
	long l;

	d = (double *)malloc(sizeof(double) * num_data);
	for (l = 0; l < num_data; l++)
		d[l] = buf[l];
	_set_double_array(plhs, idx, d, num_data);
	free(d);

	/* Because of problems with non-double values in Matlab, now
	   all values will be stored as double values. Keep the old
	   code in case it is needed in the future. */
/*	int dims[2];
	unsigned long *yp;
	long l;
	
	dims[0] = num_data;
	dims[1] = 1;
	plhs[idx] = mxCreateNumericArray(2, dims, mxINT32_CLASS, mxREAL);    
	yp = (unsigned long *)mxGetPr(plhs[idx]);
	for (l = 0; l < num_data; l++)
		yp[l] = buf[l]; */
}  /* _set_long_array() */


void
_set_dword_array(mxArray *plhs[], int idx, const unsigned long *buf, long num_data)
{
	double *d;
	long l;

	d = (double *)malloc(sizeof(double) * num_data);
	for (l = 0; l < num_data; l++)
		d[l] = buf[l];
	_set_double_array(plhs, idx, d, num_data);
	free(d);

	/* Because of problems with non-double values in Matlab, now
	   all values will be stored as double values. Keep the old
	   code in case it is needed in the future. */
/*	int dims[2];
	unsigned long *yp;
	long l;
	
	dims[0] = num_data;
	dims[1] = 1;
	plhs[idx] = mxCreateNumericArray(2, dims, mxINT32_CLASS, mxREAL);    
	yp = (unsigned long *)mxGetPr(plhs[idx]);
	for (l = 0; l < num_data; l++)
		yp[l] = buf[l]; */
}  /* _set_dword_array() */


void
_set_double_array(mxArray *plhs[], int idx, const double *buf, long num_data)
{
	double *yp;
	long l;

	plhs[idx] = mxCreateDoubleMatrix(1, num_data, mxREAL);    
	yp = (double *)mxGetPr(plhs[idx]);
	for (l = 0; l < num_data; l++)
		yp[l] = buf[l];
}  /* _set_double_array() */


void
_set_string_array(mxArray *plhs[], int idx, const char **buf, long num_data)
{
	plhs[idx] = mxCreateCharMatrixFromStrings(num_data, buf);
}  /* _set_string_array() */


void
_set_handle_array(mxArray *plhs[], int idx, const void **buf, long num_data)
{
	long l;
	long *yp;
	mwSize dims[2];
	
	dims[0] = num_data;
	dims[1] = 1;

	/* Create a matrix for the return argument */ 
	plhs[idx] = mxCreateNumericArray(2, dims, mxUINT32_CLASS, mxREAL);    
	yp = (long *)mxGetPr(plhs[idx]);

	for (l = 0; l < num_data; l++)
		yp[l] = (long)(buf[l]);
}  /* _set_handle_array() */


void
_set_vh_array(mxArray *plhs[], int idx, const value_handle *buf, long num_data)
{
	long l;
	mxArray *val;

	plhs[idx] = mxCreateCellMatrix(num_data, 1);
	for (l = 0; l < num_data; l++)
	{
		val = _value_to_mxa(buf[l]);
		if (val)
			mxSetCell(plhs[idx], l, val);
	}
}  /* _set_vh_array() */


long
_string_to_info_id(const char *name)
{
	long ret = -1;
	long l;

	for (l = 0; l < (long)(sizeof(infos)/sizeof(infos[0])); l++)
	{
		if (infos[l].name == NULL)
			continue;

		if (strcmp(name, infos[l].name) == 0)
		{
			ret = infos[l].id;
			break;
		}
	}

	return ret;
}  /* _string_to_info_id() */


mxArray *
_value_to_mxa(value_handle vh)
{
	int ok;
	mxArray *arr;
	short s;
	long l;
	double d;
	
	if (!ra_value_is_ok(vh))
		return NULL;

	arr = malloc(sizeof(mxArray *));
	ok = 1;

	switch (ra_value_get_type(vh))
	{
	case RA_VALUE_TYPE_SHORT:
		s = ra_value_get_short(vh);
		_set_short_array(&arr, 0, &s, 1);
		break;
	case RA_VALUE_TYPE_SHORT_ARRAY:
		_set_short_array(&arr, 0, ra_value_get_short_array(vh), ra_value_get_num_elem(vh));
		break;
	case RA_VALUE_TYPE_LONG:
		l = ra_value_get_long(vh);
		_set_long_array(&arr, 0, &l, 1);
		break;
	case RA_VALUE_TYPE_LONG_ARRAY:
		_set_long_array(&arr, 0, ra_value_get_long_array(vh), ra_value_get_num_elem(vh));
		break;
	case RA_VALUE_TYPE_DOUBLE:
		d = ra_value_get_double(vh);
		_set_double_array(&arr, 0, &d, 1);
		break;
	case RA_VALUE_TYPE_DOUBLE_ARRAY:
		_set_double_array(&arr, 0, ra_value_get_double_array(vh), ra_value_get_num_elem(vh));
		break;
	case RA_VALUE_TYPE_CHAR:
		_set_string(&arr, 0, ra_value_get_string(vh));
		break;
    case RA_VALUE_TYPE_VOIDP:
        l = (long)ra_value_get_voidp(vh);
        _set_long_array(&arr, 0, &l, 1);
        break;
	case RA_VALUE_TYPE_VOIDP_ARRAY:
		_set_handle_array(&arr, 0, ra_value_get_voidp_array(vh), ra_value_get_num_elem(vh));
		break;
	case RA_VALUE_TYPE_VH_ARRAY:
		_set_vh_array(&arr, 0, ra_value_get_vh_array(vh), ra_value_get_num_elem(vh));
		break;
	default:
		ok = 0;
	}

	if (!ok)
	{
		free(arr);
		arr = NULL;
	}

	return arr;
}  /* _value_to_mxa() */


value_handle
_mxa_to_value(const char *type, const mxArray *prhs[], long idx)
{
	int ok;
	long len;
	short *s_arr, s_val;
	long *l_arr, l_val;
	double *d_arr, d_val;
	char *c_val;
    void *h_val;

	value_handle vh = ra_value_malloc();
	
	ok = 1;
	vh = ra_value_malloc();
	
	if (type[0] == 'p')  /* check if it's an array */
	{
		if (type[1] == 's')  /* short array? */
		{
			s_arr = _get_short_array(prhs, idx, &len);
			ra_value_set_short_array(vh, s_arr, len);
		}
		else if (type[1] == 'l')  /* long array? */
		{
			l_arr = _get_long_array(prhs, idx, &len);
			ra_value_set_long_array(vh, l_arr, len);
		}
		else if (type[1] == 'd')  /* or double array? */
		{
			d_arr = _get_double_array(prhs, idx, &len);
			ra_value_set_double_array(vh, d_arr, len);
		}
	}
	else if (type[0] == 's') /* is it a short? */
	{
		s_val = _get_short(prhs, idx);
		ra_value_set_short(vh, s_val);
	}
	else if (type[0] == 'l') /* is it a long? */
	{
		l_val = _get_long(prhs, idx);
		ra_value_set_long(vh, l_val);
	}
	else if (type[0] == 'd') /* is it a double? */
	{
		d_val = _get_double(prhs, idx);
		ra_value_set_double(vh, d_val);
	}
	else if (type[0] == 'c') /* is it a string? */
	{
		c_val = _get_string(prhs, idx);
		ra_value_set_string(vh, c_val);
		mxFree(c_val);
	}
	else if (type[0] == 'h') /* is it a handle? */
	{
		h_val = _get_handle(prhs, idx);
		ra_value_set_voidp(vh, h_val);
	}
	else
		ok = 0;
	
	if (!ok)
	{
		ra_value_free(vh);
		vh = NULL;
	}	

	return vh;
} /* _mxa_to_value() */


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
} /* _get_info_type() */


void
if_ra_lib_init(mxArray *plhs[], const mxArray *prhs[])
{
	ra_handle ra;

	if (prhs);

	ra = ra_lib_init();
	_set_handle(plhs, 0, ra);
}  /* if_ra_lib_init() */


void
if_ra_lib_close(mxArray *plhs[], const mxArray *prhs[])
{
	ra_handle ra;

	if (plhs);

	ra = _get_handle(prhs, 1);
	ra_lib_close(ra);
}  /* if_ra_lib_close() */
  

void
if_ra_lib_get_error(mxArray *plhs[], const mxArray *prhs[])
{
	char err_text[200];
	long err_num;
	ra_handle ra;
	
	ra = _get_handle(prhs, 1);

	err_num = ra_lib_get_error(ra, err_text, 200);

	_set_long_array(plhs, 0, &err_num, 1);
	_set_string(plhs, 1, err_text);
}  /* if_ra_lib_get_error() */


void
if_ra_lib_handle_from_any_handle(mxArray *plhs[], const mxArray *prhs[])
{
	meas_handle meas;
	ra_handle ra;

	meas = _get_handle(prhs, 1);
	ra = ra_lib_handle_from_any_handle(meas);

	_set_handle(plhs, 0, ra);
}  /* if_ra_lib_handle_from_any_handle() */


void
if_ra_lib_set_option(mxArray *plhs[], const mxArray *prhs[])
{
	proc_handle pi;
	char *opt_name;
	char *opt_type;
	value_handle vh;
	int ret = -1;

	pi = _get_handle(prhs, 1);
	opt_name = _get_string(prhs, 2);
	opt_type = _get_string(prhs, 3);

	vh = _mxa_to_value(opt_type, prhs, 4);

	if (vh)
	{
		ret = ra_lib_set_option(pi, opt_name, vh);
		ra_value_free(vh);
	}

	_set_long(plhs, 0, (long)ret);
}  /* if_ra_lib_set_option() */


void
if_ra_lib_get_option(mxArray *plhs[], const mxArray *prhs[])
{
	any_handle h;
	char *opt_name;
	value_handle vh;
	mxArray *val;

	h = _get_handle(prhs, 1);
	opt_name = _get_string(prhs, 2);

	vh = ra_value_malloc();
	if (ra_lib_get_option(h, opt_name, vh) != 0)
	{
		ra_value_free(vh);
		_set_double(plhs, 0, mxGetNaN());
		return;
	}
	if (!ra_value_is_ok(vh))
	{
		ra_value_free(vh);
		_set_double(plhs, 0, mxGetNaN());
		return;
	}
	
	val = _value_to_mxa(vh);
	plhs[0] = val;
	
	ra_value_free(vh);
} /* if_ra_lib_get_option() */


void
if_ra_lib_info_get(mxArray *plhs[], const mxArray *prhs[])
{
	ra_handle ra;
	char *inf_name = NULL;
	long inf_id = -1;
	value_handle vh;

	ra = _get_handle(prhs, 1);
	inf_name = _get_string(prhs, 2);
	inf_id = _string_to_info_id(inf_name);
	if ((inf_id == -1) ||
		(inf_id <= RA_INFO_LIB_START) || (inf_id >= RA_INFO_LIB_END))
	{
		_set_double(plhs, 0, mxGetNaN());
		_set_string(plhs, 1, "");
		_set_string(plhs, 2, "");
		_set_long(plhs, 3, 0);
		return;
	}

	vh = ra_value_malloc();
	if (ra_info_get(ra, inf_id, vh) != 0)
	{
		ra_value_free(vh);

		_set_double(plhs, 0, mxGetNaN());
		_set_string(plhs, 1, "");
		_set_string(plhs, 2, "");
		_set_long(plhs, 3, 0);
		return;
	}

	_set_value(plhs, 0, vh);
	_set_string(plhs, 1, ra_value_get_name(vh));
	_set_string(plhs, 2, ra_value_get_desc(vh));
	_set_long(plhs, 3, ra_value_info_modifiable(vh));

	ra_value_free(vh);
}  /* if_ra_lib_info_get() */


void
if_ra_plugin_info_get(mxArray *plhs[], const mxArray *prhs[])
{
	plugin_handle pl;
	char *inf_name = NULL;
	long inf_id = -1;
	value_handle vh;
	long num;

	pl = _get_handle(prhs, 1);
	num = _get_long(prhs, 2);
	inf_name = _get_string(prhs, 3);
	inf_id = _string_to_info_id(inf_name);

	if ((inf_id == -1) ||
		(inf_id <= RA_INFO_PLUGIN_START) || (inf_id >= RA_INFO_PLUGIN_END))
	{
		_set_double(plhs, 0, mxGetNaN());
		_set_string(plhs, 1, "");
		_set_string(plhs, 2, "");
		_set_long(plhs, 3, 0);
		return;
	}

	vh = ra_value_malloc();
	ra_value_set_number(vh, num);
	if (ra_info_get(pl, inf_id, vh) != 0)
	{
		ra_value_free(vh);

		_set_double(plhs, 0, mxGetNaN());
		_set_string(plhs, 1, "");
		_set_string(plhs, 2, "");
		_set_long(plhs, 3, 0);
		return;
	}

	_set_value(plhs, 0, vh);
	_set_string(plhs, 1, ra_value_get_name(vh));
	_set_string(plhs, 2, ra_value_get_desc(vh));
	_set_long(plhs, 3, ra_value_info_modifiable(vh));

	ra_value_free(vh);
}  /* if_ra_plugin_info_get() */


void
if_ra_result_info_get(mxArray *plhs[], const mxArray *prhs[])
{
	plugin_handle pl;
	char *inf_name = NULL;
	long res_num, inf_id = -1;
	value_handle vh;

	pl = _get_handle(prhs, 1);
	res_num = _get_long(prhs, 2);
	inf_name = _get_string(prhs, 3);
	inf_id = _string_to_info_id(inf_name);

	if ((inf_id == -1) ||
		(inf_id <= RA_INFO_PLUGIN_START) || (inf_id >= RA_INFO_PLUGIN_END))
	{
		_set_double(plhs, 0, mxGetNaN());
		_set_string(plhs, 1, "");
		_set_string(plhs, 2, "");
		_set_long(plhs, 3, 0);
		return;
	}

	vh = ra_value_malloc();
	ra_value_set_number(vh, res_num);
	if (ra_info_get(pl, inf_id, vh) != 0)
	{
		ra_value_free(vh);

		_set_double(plhs, 0, mxGetNaN());
		_set_string(plhs, 1, "");
		_set_string(plhs, 2, "");
		_set_long(plhs, 3, 0);
		return;
	}

	_set_value(plhs, 0, vh);
	_set_string(plhs, 1, ra_value_get_name(vh));
	_set_string(plhs, 2, ra_value_get_desc(vh));
	_set_long(plhs, 3, ra_value_info_modifiable(vh));

	ra_value_free(vh);
}  /* if_ra_result_info_get() */


void
if_ra_meas_info_get(mxArray *plhs[], const mxArray *prhs[])
{
	meas_handle meas;
	char *inf_name = NULL;
	long inf_id = -1;
	value_handle vh;

	meas = _get_handle(prhs, 1);
	inf_name = _get_string(prhs, 2);
	inf_id = _string_to_info_id(inf_name);

	if ((inf_id == -1) ||
		(inf_id <= RA_INFO_MEASUREMENT_START) || (inf_id >= RA_INFO_MEASUREMENT_END))
	{
		_set_double(plhs, 0, mxGetNaN());
		_set_string(plhs, 1, "");
		_set_string(plhs, 2, "");
		_set_long(plhs, 3, 0);
		return;
	}

	vh = ra_value_malloc();
	if (ra_info_get(meas, inf_id, vh) != 0)
	{
		ra_value_free(vh);

		_set_double(plhs, 0, mxGetNaN());
		_set_string(plhs, 1, "");
		_set_string(plhs, 2, "");
		_set_long(plhs, 3, 0);
		return;
	}

	_set_value(plhs, 0, vh);
	_set_string(plhs, 1, ra_value_get_name(vh));
	_set_string(plhs, 2, ra_value_get_desc(vh));
	_set_long(plhs, 3, ra_value_info_modifiable(vh));

	ra_value_free(vh);
}  /* if_ra_meas_info_get() */


void
if_ra_obj_info_get(mxArray *plhs[], const mxArray *prhs[])
{
	meas_handle meas;
	char *inf_name = NULL;
	long inf_id = -1;
	value_handle vh;

	meas = _get_handle(prhs, 1);
	inf_name = _get_string(prhs, 2);
	inf_id = _string_to_info_id(inf_name);

	if ((inf_id == -1) ||
		(inf_id <= RA_INFO_OBJECT_START) || (inf_id >= RA_INFO_OBJECT_END))
	{
		_set_double(plhs, 0, mxGetNaN());
		_set_string(plhs, 1, "");
		_set_string(plhs, 2, "");
		_set_long(plhs, 3, 0);
		return;
	}

	vh = ra_value_malloc();
	if (ra_info_get(meas, inf_id, vh) != 0)
	{
		ra_value_free(vh);

		_set_double(plhs, 0, mxGetNaN());
		_set_string(plhs, 1, "");
		_set_string(plhs, 2, "");
		_set_long(plhs, 3, 0);
		return;
	}

	_set_value(plhs, 0, vh);
	_set_string(plhs, 1, ra_value_get_name(vh));
	_set_string(plhs, 2, ra_value_get_desc(vh));
	_set_long(plhs, 3, ra_value_info_modifiable(vh));

	ra_value_free(vh);
}  /* if_ra_obj_info_get() */


void
if_ra_rec_info_get(mxArray *plhs[], const mxArray *prhs[])
{
	rec_handle rec;
	char *inf_name = NULL;
	long inf_id = -1;
	value_handle vh;

	rec = _get_handle(prhs, 1);
	inf_name = _get_string(prhs, 2);
	inf_id = _string_to_info_id(inf_name);

	if ((inf_id == -1) ||
		(inf_id <= RA_INFO_REC_GENERAL_START) || (inf_id >= RA_INFO_REC_GENERAL_END))
	{
		_set_double(plhs, 0, mxGetNaN());
		_set_string(plhs, 1, "");
		_set_string(plhs, 2, "");
		_set_long(plhs, 3, 0);
		return;
	}

	vh = ra_value_malloc();
	if (ra_info_get(rec, inf_id, vh) != 0)
	{
		ra_value_free(vh);

		_set_double(plhs, 0, mxGetNaN());
		_set_string(plhs, 1, "");
		_set_string(plhs, 2, "");
		_set_long(plhs, 3, 0);
		return;
	}

	_set_value(plhs, 0, vh);
	_set_string(plhs, 1, ra_value_get_name(vh));
	_set_string(plhs, 2, ra_value_get_desc(vh));
	_set_long(plhs, 3, ra_value_info_modifiable(vh));

	ra_value_free(vh);
}  /* if_ra_rec_info_get() */


void
if_ra_dev_info_get(mxArray *plhs[], const mxArray *prhs[])
{
	rec_handle rec;
	long dev;
	char *inf_name = NULL;
	long inf_id = -1;
	value_handle vh;

	rec = _get_handle(prhs, 1);
	dev = _get_long(prhs, 2);
	inf_name = _get_string(prhs, 3);
	inf_id = _string_to_info_id(inf_name);

	if ((inf_id == -1) ||
		(inf_id <= RA_INFO_REC_DEVICE_START) || (inf_id >= RA_INFO_REC_DEVICE_END))
	{
		_set_double(plhs, 0, mxGetNaN());
		_set_string(plhs, 1, "");
		_set_string(plhs, 2, "");
		_set_long(plhs, 3, 0);
		return;
	}

	vh = ra_value_malloc();
	ra_value_set_number(vh, dev);
	if (ra_info_get(rec, inf_id, vh) != 0)
	{
		ra_value_free(vh);

		_set_double(plhs, 0, mxGetNaN());
		_set_string(plhs, 1, "");
		_set_string(plhs, 2, "");
		_set_long(plhs, 3, 0);
		return;
	}

	_set_value(plhs, 0, vh);
	_set_string(plhs, 1, ra_value_get_name(vh));
	_set_string(plhs, 2, ra_value_get_desc(vh));
	_set_long(plhs, 3, ra_value_info_modifiable(vh));

	ra_value_free(vh);
}  /* if_ra_dev_info_get() */


void
if_ra_ch_info_get(mxArray *plhs[], const mxArray *prhs[])
{
	rec_handle rec;
	char *inf_name = NULL;
	long ch;
	long inf_id = -1;
	value_handle vh;

	rec = _get_handle(prhs, 1);
	ch = _get_long(prhs, 2);
	inf_name = _get_string(prhs, 3);
	inf_id = _string_to_info_id(inf_name);

	if ((inf_id == -1) ||
		(inf_id <= RA_INFO_REC_CHANNEL_START) || (inf_id >= RA_INFO_REC_CHANNEL_END))
	{
		_set_double(plhs, 0, mxGetNaN());
		_set_string(plhs, 1, "");
		_set_string(plhs, 2, "");
		_set_long(plhs, 3, 0);
		return;
	}

	vh = ra_value_malloc();
	ra_value_set_number(vh, ch);
	if (ra_info_get(rec, inf_id, vh) != 0)
	{
		ra_value_free(vh);

		_set_double(plhs, 0, mxGetNaN());
		_set_string(plhs, 1, "");
		_set_string(plhs, 2, "");
		_set_long(plhs, 3, 0);
		return;
	}

	_set_value(plhs, 0, vh);
	_set_string(plhs, 1, ra_value_get_name(vh));
	_set_string(plhs, 2, ra_value_get_desc(vh));
	_set_long(plhs, 3, ra_value_info_modifiable(vh));

	ra_value_free(vh);
}  /* if_ra_ch_info_get() */


void
if_ra_eval_info_get(mxArray *plhs[], const mxArray *prhs[])
{
	eval_handle eval;
	char *inf_name = NULL;
	long inf_id = -1;
	value_handle vh;

	eval = _get_handle(prhs, 1);
	inf_name = _get_string(prhs, 2);
	inf_id = _string_to_info_id(inf_name);

	if ((inf_id == -1) ||
		(inf_id <= RA_INFO_EVAL_START) || (inf_id >= RA_INFO_EVAL_END))
	{
		_set_double(plhs, 0, mxGetNaN());
		_set_string(plhs, 1, "");
		_set_string(plhs, 2, "");
		_set_long(plhs, 3, 0);
		return;
	}

	vh = ra_value_malloc();
	if (ra_info_get(eval, inf_id, vh) != 0)
	{
		ra_value_free(vh);

		_set_double(plhs, 0, mxGetNaN());
		_set_string(plhs, 1, "");
		_set_string(plhs, 2, "");
		_set_long(plhs, 3, 0);
		return;
	}

	_set_value(plhs, 0, vh);
	_set_string(plhs, 1, ra_value_get_name(vh));
	_set_string(plhs, 2, ra_value_get_desc(vh));
	_set_long(plhs, 3, ra_value_info_modifiable(vh));

	ra_value_free(vh);
}  /* if_ra_eval_info_get() */


void
if_ra_class_info_get(mxArray *plhs[], const mxArray *prhs[])
{
	class_handle clh;
	char *inf_name = NULL;
	long inf_id = -1;
	value_handle vh;

	clh = _get_handle(prhs, 1);
	inf_name = _get_string(prhs, 2);
	inf_id = _string_to_info_id(inf_name);

	if ((inf_id == -1) ||
		(inf_id <= RA_INFO_CLASS_START) || (inf_id >= RA_INFO_CLASS_END))
	{
		_set_double(plhs, 0, mxGetNaN());
		_set_string(plhs, 1, "");
		_set_string(plhs, 2, "");
		_set_long(plhs, 3, 0);
		return;
	}

	vh = ra_value_malloc();
	if (ra_info_get(clh, inf_id, vh) != 0)
	{
		ra_value_free(vh);

		_set_double(plhs, 0, mxGetNaN());
		_set_string(plhs, 1, "");
		_set_string(plhs, 2, "");
		_set_long(plhs, 3, 0);
		return;
	}

	_set_value(plhs, 0, vh);
	_set_string(plhs, 1, ra_value_get_name(vh));
	_set_string(plhs, 2, ra_value_get_desc(vh));
	_set_long(plhs, 3, ra_value_info_modifiable(vh));

	ra_value_free(vh);
}  /* if_ra_class_info_get() */


void
if_ra_prop_info_get(mxArray *plhs[], const mxArray *prhs[])
{
	prop_handle prop;
	char *inf_name = NULL;
	long inf_id = -1;
	value_handle vh;

	prop = _get_handle(prhs, 1);
	inf_name = _get_string(prhs, 2);
	inf_id = _string_to_info_id(inf_name);

	if ((inf_id == -1) ||
		(inf_id <= RA_INFO_PROP_START) || (inf_id >= RA_INFO_PROP_END))
	{
		_set_double(plhs, 0, mxGetNaN());
		_set_string(plhs, 1, "");
		_set_string(plhs, 2, "");
		_set_long(plhs, 3, 0);
		return;
	}

	vh = ra_value_malloc();
	if (ra_info_get(prop, inf_id, vh) != 0)
	{
		ra_value_free(vh);

		_set_double(plhs, 0, mxGetNaN());
		_set_string(plhs, 1, "");
		_set_string(plhs, 2, "");
		_set_long(plhs, 3, 0);
		return;
	}

	_set_value(plhs, 0, vh);
	_set_string(plhs, 1, ra_value_get_name(vh));
	_set_string(plhs, 2, ra_value_get_desc(vh));
	_set_long(plhs, 3, ra_value_info_modifiable(vh));

	ra_value_free(vh);
}  /* if_ra_prop_info_get() */


void
if_ra_summary_info_get(mxArray *plhs[], const mxArray *prhs[])
{
	sum_handle sh;
	char *inf_name = NULL;
	long inf_id = -1;
	value_handle vh;

	sh = _get_handle(prhs, 1);
	inf_name = _get_string(prhs, 2);
	inf_id = _string_to_info_id(inf_name);

	if ((inf_id == -1) ||
		(inf_id <= RA_INFO_SUM_START) || (inf_id >= RA_INFO_SUM_END))
	{
		_set_double(plhs, 0, mxGetNaN());
		_set_string(plhs, 1, "");
		_set_string(plhs, 2, "");
		_set_long(plhs, 3, 0);
		return;
	}

	vh = ra_value_malloc();
	if (ra_info_get(sh, inf_id, vh) != 0)
	{
		ra_value_free(vh);

		_set_double(plhs, 0, mxGetNaN());
		_set_string(plhs, 1, "");
		_set_string(plhs, 2, "");
		_set_long(plhs, 3, 0);
		return;
	}

	_set_value(plhs, 0, vh);
	_set_string(plhs, 1, ra_value_get_name(vh));
	_set_string(plhs, 2, ra_value_get_desc(vh));
	_set_long(plhs, 3, ra_value_info_modifiable(vh));

	ra_value_free(vh);
}  /* if_ra_summary_info_get() */



void
if_ra_sum_dim_info_get(mxArray *plhs[], const mxArray *prhs[])
{
	sum_handle sh;
	char *inf_name = NULL;
	long dim_num, inf_id = -1;
	value_handle vh;

	sh = _get_handle(prhs, 1);
	dim_num = _get_long(prhs, 2);
	inf_name = _get_string(prhs, 3);
	inf_id = _string_to_info_id(inf_name);

	if ((inf_id == -1) ||
		(inf_id <= RA_INFO_SUM_START) || (inf_id >= RA_INFO_SUM_END))
	{
		_set_double(plhs, 0, mxGetNaN());
		_set_string(plhs, 1, "");
		_set_string(plhs, 2, "");
		_set_long(plhs, 3, 0);
		return;
	}

	vh = ra_value_malloc();
	ra_value_set_number(vh, dim_num);
	if (ra_info_get(sh, inf_id, vh) != 0)
	{
		ra_value_free(vh);

		_set_double(plhs, 0, mxGetNaN());
		_set_string(plhs, 1, "");
		_set_string(plhs, 2, "");
		_set_long(plhs, 3, 0);
		return;
	}

	_set_value(plhs, 0, vh);
	_set_string(plhs, 1, ra_value_get_name(vh));
	_set_string(plhs, 2, ra_value_get_desc(vh));
	_set_long(plhs, 3, ra_value_info_modifiable(vh));

	ra_value_free(vh);
}  /* if_ra_sum_dim_info_get() */


void
if_ra_sum_ch_info_get(mxArray *plhs[], const mxArray *prhs[])
{
	sum_handle sh;
	char *inf_name = NULL;
	long ch, inf_id = -1;
	value_handle vh;

	sh = _get_handle(prhs, 1);
	ch = _get_long(prhs, 2);
	inf_name = _get_string(prhs, 3);
	inf_id = _string_to_info_id(inf_name);

	if ((inf_id == -1) ||
		(inf_id <= RA_INFO_SUM_START) || (inf_id >= RA_INFO_SUM_END))
	{
		_set_double(plhs, 0, mxGetNaN());
		_set_string(plhs, 1, "");
		_set_string(plhs, 2, "");
		_set_long(plhs, 3, 0);
		return;
	}

	vh = ra_value_malloc();
	ra_value_set_number(vh, ch);
	if (ra_info_get(sh, inf_id, vh) != 0)
	{
		ra_value_free(vh);

		_set_double(plhs, 0, mxGetNaN());
		_set_string(plhs, 1, "");
		_set_string(plhs, 2, "");
		_set_long(plhs, 3, 0);
		return;
	}

	_set_value(plhs, 0, vh);
	_set_string(plhs, 1, ra_value_get_name(vh));
	_set_string(plhs, 2, ra_value_get_desc(vh));
	_set_long(plhs, 3, ra_value_info_modifiable(vh));

	ra_value_free(vh);
}  /* if_ra_sum_ch_info_get() */


void
if_ra_info_get_by_idx(mxArray *plhs[], const mxArray *prhs[])
{
	any_handle h;
	char *type_s;
	long type, idx;
	value_handle vh;

	h = _get_handle(prhs, 1);
	type_s = _get_string(prhs, 2);
	idx = _get_long(prhs, 3);

	type = _get_info_type(type_s);
	if (type < 0)
	{
		_set_double(plhs, 0, mxGetNaN());
		_set_string(plhs, 1, "");
		_set_string(plhs, 2, "");
		_set_long(plhs, 3, 0);
		return;
	}

	vh = ra_value_malloc();
	if (ra_info_get_by_idx(h, type, idx, vh) != 0)
	{
		ra_value_free(vh);

		_set_double(plhs, 0, mxGetNaN());
		_set_string(plhs, 1, "");
		_set_string(plhs, 2, "");
		_set_long(plhs, 3, 0);
		return;
	}

	_set_value(plhs, 0, vh);
	_set_string(plhs, 1, ra_value_get_name(vh));
	_set_string(plhs, 2, ra_value_get_desc(vh));	
	_set_long(plhs, 3, ra_value_info_modifiable(vh));
	
	ra_value_free(vh);
}  /* ra_info_get_by_idx() */


void
if_ra_info_set(mxArray *plhs[], const mxArray *prhs[])
{
	if (plhs || prhs);

	/* TODO: add functionality */
}  /* ra_info_set() */


void
if_ra_meas_handle_from_any_handle(mxArray *plhs[], const mxArray *prhs[])
{
	any_handle any = _get_handle(prhs, 1);

	meas_handle meas = ra_meas_handle_from_any_handle(any);

	_set_handle(plhs, 0, meas);
}  /* if_ra_meas_handle_from_any_handle() */


void
if_ra_meas_find(mxArray *plhs[], const mxArray *prhs[])
{
	char *dir;
	ra_handle ra;
	struct ra_find_struct rfs;
	ra_find_handle mfh;
	char **sigs_found = NULL;
	char **plugins = NULL;
	long l, n_sigs = 0;

	ra = _get_handle(prhs, 1);
	dir = _get_string(prhs, 2);

	/* get all signal-names */
	mfh = ra_meas_find_first(ra, dir, &rfs);
	if (mfh)
	{
		do
		{
			++n_sigs;
			sigs_found = (char **)realloc(sigs_found, sizeof(char *) * n_sigs);
			sigs_found[n_sigs-1] = (char *)malloc(sizeof(char) * (strlen(rfs.name)+1));
			strcpy(sigs_found[n_sigs-1], rfs.name);

			plugins = (char **)realloc(plugins, sizeof(char *) * n_sigs);
			plugins[n_sigs-1] = (char *)malloc(sizeof(char) * (strlen(rfs.plugin)+1));
			strcpy(plugins[n_sigs-1], rfs.plugin);
		} while (ra_meas_find_next(mfh, &rfs));
		ra_meas_close_find(mfh);
	}

	/* assign all signal-names to Matlab-Array */
	plhs[0] = mxCreateCharMatrixFromStrings(n_sigs, (const char **)sigs_found);
	plhs[1] = mxCreateCharMatrixFromStrings(n_sigs, (const char **)plugins);

	/* clean up */
	for (l = 0; l < n_sigs; l++)
	{
		free(sigs_found[l]);
		free(plugins[l]);
	}
	free(sigs_found);
	free(plugins);

	return;
}  /* if_ra_meas_find() */


void
if_ra_meas_open(mxArray *plhs[], const mxArray *prhs[])
{
	ra_handle ra;
	char *dir, *eval;
	int fast;
	meas_handle meas;

	ra = _get_handle(prhs, 1);
	dir = _get_string(prhs, 2);
	eval = _get_string(prhs, 3);
	fast = (int)_get_long(prhs, 4);

	meas = ra_meas_open(ra, dir, eval, fast);

	_set_handle(plhs, 0, meas);

	return;
}  /* if_ra_meas_open() */


void
if_ra_meas_new(mxArray *plhs[], const mxArray *prhs[])
{
	ra_handle ra;
	char *dir, *name;
	meas_handle meas;

	ra = _get_handle(prhs, 1);
	dir = _get_string(prhs, 2);
	name = _get_string(prhs, 3);

	meas = ra_meas_new(ra, dir, name);

	_set_handle(plhs, 0, meas);

	return;
}  /* if_ra_meas_new() */


void
if_ra_meas_save(mxArray *plhs[], const mxArray *prhs[])
{
	meas_handle meas;

	if (plhs);

	meas = _get_handle(prhs, 1);
	ra_meas_save(meas);
}  /* if_ra_meas_save() */


void
if_ra_meas_close(mxArray *plhs[], const mxArray *prhs[])
{
	meas_handle meas;

	if (plhs);

	meas = _get_handle(prhs, 1);
	ra_meas_close(meas);
}  /* if_ra_meas_close() */


void
if_ra_rec_get_first(mxArray *plhs[], const mxArray *prhs[])
{
	meas_handle meas;
	long session;
	rec_handle rec;

	meas = _get_handle(prhs, 1);
	session = _get_long(prhs, 2);

	rec = ra_rec_get_first(meas, session);

	_set_handle(plhs, 0, rec);
}  /* if_ra_rec_get_first() */


void
if_ra_rec_get_next(mxArray *plhs[], const mxArray *prhs[])
{
	rec_handle rec, rec_next;

	rec = _get_handle(prhs, 1);

	rec_next = ra_rec_get_next(rec);

	_set_handle(plhs, 0, rec_next);
}  /* if_ra_rec_get_next() */


void
if_ra_rec_get_first_child(mxArray *plhs[], const mxArray *prhs[])
{
	rec_handle rec, rec_child;

	rec = _get_handle(prhs, 1);

	rec_child = ra_rec_get_first_child(rec);

	_set_handle(plhs, 0, rec_child);
}  /* if_ra_rec_get_first_child() */


void
if_ra_raw_get(mxArray *plhs[], const mxArray *prhs[])
{
	rec_handle rec;
	unsigned int ch;
	size_t start, num_data, num_out, l;
	DWORD *buf = NULL;
	unsigned long *l_buf = NULL;

	rec = _get_handle(prhs, 1);
	ch = (unsigned int)_get_long(prhs, 2);
	start = (size_t)_get_long(prhs, 3);
	num_data = (size_t)_get_long(prhs, 4);

	buf = malloc(sizeof(DWORD) * num_data);
	num_out = ra_raw_get(rec, ch, start, num_data, buf, NULL);

	/* TODO: think if a DWORD value-type is necessary in "value-handle" */
	l_buf = malloc(sizeof(unsigned long) * num_data);
	for (l = 0; l < num_out; l++)
		l_buf[l] = buf[l];
	/* _set_long_array(plhs, 0, l_buf, (long)num_out); */
	/* keep values as unsigned long when assining it to double */
	_set_dword_array(plhs, 0, l_buf, (long)num_out);

	free(buf);
	free(l_buf);
}  /* if_ra_raw_get() */


void
if_ra_raw_get_unit(mxArray *plhs[], const mxArray *prhs[])
{
	rec_handle rec;
	unsigned int ch;
	size_t start, num_data, num_out;
	double *buf = NULL;

	rec = _get_handle(prhs, 1);
	ch = (unsigned int)_get_long(prhs, 2);
	start = (size_t)_get_long(prhs, 3);
	num_data = (size_t)_get_long(prhs, 4);

	buf = malloc(sizeof(double) * num_data);
	num_out = ra_raw_get_unit(rec, ch, start, num_data, buf);

	_set_double_array(plhs, 0, buf, (long)num_out);

	free(buf);
}  /* if_ra_raw_get_unit() */


void
if_ra_meas_copy(mxArray *plhs[], const mxArray *prhs[])
{
	meas_handle mh;
	char *dir;
	long ret;

	mh = _get_handle(prhs, 1);
	dir = _get_string(prhs, 2);

	ret = ra_meas_copy(mh, dir);
	_set_long_array(plhs, 0, &ret, 1);
}  /* if_ra_meas_copy() */


void
if_ra_meas_move(mxArray *plhs[], const mxArray *prhs[])
{
	meas_handle mh = _get_handle(prhs, 1);
	char *dir = _get_string(prhs, 2);

	mh = ra_meas_move(mh, dir);

	_set_handle(plhs, 0, mh);
}  /* if_ra_meas_move() */


void
if_ra_meas_delete(mxArray *plhs[], const mxArray *prhs[])
{
	meas_handle mh;
	long ret;

	mh = _get_handle(prhs, 1);

	ret = ra_meas_delete(mh);
	_set_long_array(plhs, 0, &ret, 1);
}  /* if_ra_meas_delete() */


void
if_ra_lib_use_plugin(mxArray *plhs[], const mxArray *prhs[])
{
	ra_handle ra;
	int pl_index;
	int use_it;
	long ret;

	ra = _get_handle(prhs, 1);
	pl_index = (int)_get_long(prhs, 2);
	use_it = (int)_get_long(prhs, 3);

	ret = (long)ra_lib_use_plugin(ra, pl_index, use_it);

	_set_long_array(plhs, 0, &ret, 1);
}  /* if_ra_lib_use_plugin() */


void
if_ra_plugin_get_by_num(mxArray *plhs[], const mxArray *prhs[])
{
	ra_handle ra = _get_handle(prhs, 1);
	int idx = (int)_get_long(prhs, 2);
	int search_all = (int)_get_long(prhs, 3);

	plugin_handle ph = ra_plugin_get_by_num(ra, idx, search_all);

	_set_handle(plhs, 0, ph);
}  /* if_ra_plugin_get_by_num() */


void
if_ra_plugin_get_by_name(mxArray *plhs[], const mxArray *prhs[])
{
	ra_handle ra = _get_handle(prhs, 1);
	char *name = _get_string(prhs, 2);
	int search_all = (int)_get_long(prhs, 3);

	plugin_handle ph = ra_plugin_get_by_name(ra, name, search_all);

	_set_handle(plhs, 0, ph);
}  /* if_ra_plugin_get_by_name() */


void
if_ra_proc_get(mxArray *plhs[], const mxArray *prhs[])
{
	meas_handle mh;
	plugin_handle pl;
	proc_handle proc;

	mh = _get_handle(prhs, 1);
	pl = _get_handle(prhs, 2);

	proc = ra_proc_get(mh, pl, NULL);
	_set_handle(plhs, 0, proc);
} /* if_ra_proc_get() */


void
if_ra_proc_do(mxArray *plhs[], const mxArray *prhs[])
{
	proc_handle pi;
	long res;

	pi = _get_handle(prhs, 1);

	res = ra_proc_do(pi);

	_set_long_array(plhs, 0, &res, 1);
}  /* if_ra_proc_do() */


void
if_ra_proc_get_result_idx(mxArray *plhs[], const mxArray *prhs[])
{
	proc_handle pi;
	char *res_ascii_id;
	long idx;

	pi = _get_handle(prhs, 1);
	res_ascii_id = _get_string(prhs, 2);

	idx = ra_proc_get_result_idx(pi, res_ascii_id);

	/* arrays are one-based and not zero-based */
	if (idx != -1)
		idx++;

	_set_long_array(plhs, 0, &idx, 1);
}  /* if_ra_proc_get_result_idx() */


void
if_ra_proc_get_result(mxArray *plhs[], const mxArray *prhs[])
{
	proc_handle pi;
	plugin_handle pl;
	value_handle vh;
	long n_res = 0;
	mxArray *values, *val;
	char **names;
	char **desc;
	int i;

	pi = _get_handle(prhs, 1);
	pl = _get_handle(prhs, 2);

	vh = ra_value_malloc();

	if (ra_info_get(pl, RA_INFO_PL_NUM_RESULTS_L, vh) == 0)
		n_res = ra_value_get_long(vh);

	values = mxCreateCellMatrix(n_res, 1);
	names = calloc(1, sizeof(char *) * n_res);
	desc = calloc(1, sizeof(char *) * n_res);

	for (i = 0; i < n_res; i++)
	{
		/* TODO: handle result-sets */
		if (ra_proc_get_result(pi, i, 0, vh) == 0)
		{
			val = _value_to_mxa(vh);
			if (val)
				mxSetCell(values, i, val);
		}
		if (ra_value_get_name(vh))
		{
			names[i] = calloc(1, sizeof(char) * (strlen(ra_value_get_name(vh))+1));
			strcpy(names[i], ra_value_get_name(vh));
		}
		else
			names[i] = calloc(1, sizeof(char));

		if (ra_value_get_desc(vh))
		{
			desc[i] = calloc(1, sizeof(char) * (strlen(ra_value_get_desc(vh))+1));
			strcpy(desc[i], ra_value_get_desc(vh));
		}
		else
			desc[i] = calloc(1, sizeof(char));
	}

	ra_value_free(vh);

	plhs[0] = values;
	_set_string_array(plhs, 1, (const char **)names, n_res);
	_set_string_array(plhs, 2, (const char **)desc, n_res);

	for (i = 0; i < n_res; i++)
	{
		if (names[i])
			free(names[i]);
		if (desc[i])
			free(desc[i]);
	}
        /* free(values); TODO: check if values can be freed or not */
	free(names);
	free(desc);
}  /* if_ra_result_get() */


void
if_ra_proc_free(mxArray *plhs[], const mxArray *prhs[])
{
	proc_handle pi;

	if (plhs);

	pi = _get_handle(prhs, 1);
	ra_proc_free(pi);
}  /* if_ra_proc_free() */


void
if_ra_eval_save(mxArray *plhs[], const mxArray *prhs[])
{
	meas_handle meas;
	char *fn;
	int use_ascii;
	long ret;

	meas = _get_handle(prhs, 1);
	fn = _get_string(prhs, 2);
	use_ascii = (int)_get_long(prhs, 3);
	
	ret = (long)ra_eval_save(meas, fn, use_ascii);
	_set_long_array(plhs, 0, &ret, 1);
}  /* if_ra_eval_save() */


void
if_ra_eval_attribute_list(mxArray *plhs[], const mxArray *prhs[])
{
	any_handle h;
	value_handle vh;

	h = _get_handle(prhs, 1);
	vh = ra_value_malloc();

	if (ra_eval_attribute_list(h, vh) != 0)
	{
		ra_value_free(vh);
		_set_double(plhs, 0, mxGetNaN());
		return;
	}

	plhs[0] = _value_to_mxa(vh);

	ra_value_free(vh);
} /* if_ra_eval_attribute_list() */


void
if_ra_eval_attribute_get(mxArray *plhs[], const mxArray *prhs[])
{
	any_handle h;
	char *attr;
	value_handle vh;

	h = _get_handle(prhs, 1);
	attr = _get_string(prhs, 2);

	vh = ra_value_malloc();

	if (ra_eval_attribute_get(h, attr, vh) != 0)
	{
		ra_value_free(vh);
		_set_double(plhs, 0, mxGetNaN());
		return;
	}

	plhs[0] = _value_to_mxa(vh);

	ra_value_free(vh);
} /* if_ra_eval_attribute_get() */


void
if_ra_eval_attribute_set(mxArray *plhs[], const mxArray *prhs[])
{
	any_handle h;
	char *attr, *type;
	value_handle vh;

	if (plhs);

	h = _get_handle(prhs, 1);
	attr = _get_string(prhs, 2);
	type = _get_string(prhs, 3);

	vh = _mxa_to_value(type, prhs, 4);
	if (vh)
	{
		ra_eval_attribute_set(h, attr, vh);
		ra_value_free(vh);
	}
} /* if_ra_eval_attribute_set() */


void
if_ra_eval_attribute_unset(mxArray *plhs[], const mxArray *prhs[])
{
	any_handle h;
	char *attr;

	if (plhs);

	h = _get_handle(prhs, 1);
	attr = _get_string(prhs, 2);

	ra_eval_attribute_unset(h, attr);
} /* if_ra_eval_attribute_unset() */


void
if_ra_eval_add(mxArray *plhs[], const mxArray *prhs[])
{
	meas_handle mh;
	char *name, *desc;
	long original;
	eval_handle eh;

	mh = _get_handle(prhs, 1);
	name = _get_string(prhs, 2);
	desc = _get_string(prhs, 3);
	original = _get_long(prhs, 4);

	eh = ra_eval_add(mh, name, desc, original);

	_set_handle(plhs, 0, eh);
} /* if_ra_eval_add() */


void
if_ra_eval_delete(mxArray *plhs[], const mxArray *prhs[])
{
	eval_handle eh;
	int ret;

	eh = _get_handle(prhs, 1);
	ret = ra_eval_delete(eh);

	_set_long(plhs, 0, ret);
} /* if_ra_eval_delete() */


void
if_ra_eval_get_all(mxArray *plhs[], const mxArray *prhs[])
{
	int ret;
	meas_handle mh;
	value_handle vh;

	mh = _get_handle(prhs, 1);
	
	vh = ra_value_malloc();
	if ((ret = ra_eval_get_all(mh, vh)) != 0)
	{
		ra_value_free(vh);
		_set_double(plhs, 0, mxGetNaN());
		return;
	}
	
	plhs[0] = _value_to_mxa(vh);

	ra_value_free(vh);
} /* if_ra_eval_get_all() */


void
if_ra_eval_get_original(mxArray *plhs[], const mxArray *prhs[])
{
	meas_handle mh;
	eval_handle eh;

	mh = _get_handle(prhs, 1);
	eh = ra_eval_get_original(mh);

	_set_handle(plhs, 0, eh);
} /* if_ra_eval_get_original() */


void
if_ra_eval_get_default(mxArray *plhs[], const mxArray *prhs[])
{
	meas_handle mh;
	eval_handle eh;

	mh = _get_handle(prhs, 1);
	eh = ra_eval_get_default(mh);

	_set_handle(plhs, 0, eh);
} /* if_ra_eval_get_default() */


void
if_ra_eval_set_default(mxArray *plhs[], const mxArray *prhs[])
{
	int ret;
	eval_handle eh;

	eh = _get_handle(prhs, 1);
	ret = ra_eval_set_default(eh);

	_set_long(plhs, 0, ret);
} /* if_ra_eval_set_default() */


void
if_ra_eval_get_handle(mxArray *plhs[], const mxArray *prhs[])
{
	class_handle clh;
	eval_handle eh;

	clh = _get_handle(prhs, 1);
	eh = ra_eval_get_handle(clh);

	_set_handle(plhs, 0, eh);
} /* if_ra_eval_get_handle() */


void
if_ra_class_add(mxArray *plhs[], const mxArray *prhs[])
{
	eval_handle eh;
	char *id, *name, *desc;
	class_handle clh;

	eh = _get_handle(prhs, 1);
	id = _get_string(prhs, 2);
	name = _get_string(prhs, 3);
	desc = _get_string(prhs, 4);

	clh = ra_class_add(eh, id, name, desc);

	_set_handle(plhs, 0, clh);
} /* if_ra_class_add() */


void
if_ra_class_add_predef(mxArray *plhs[], const mxArray *prhs[])
{
	eval_handle eh;
	char *id;
	class_handle clh;

	eh = _get_handle(prhs, 1);
	id = _get_string(prhs, 2);

	clh = ra_class_add_predef(eh, id);

	_set_handle(plhs, 0, clh);
} /* if_ra_class_add_predef() */


void
if_ra_class_delete(mxArray *plhs[], const mxArray *prhs[])
{
	int ret;
	class_handle clh;

	clh = _get_handle(prhs, 1);
	ret = ra_class_delete(clh);

	_set_long(plhs, 0, ret);
} /* if_ra_class_delete() */


void
if_ra_class_get(mxArray *plhs[], const mxArray *prhs[])
{
	eval_handle eh;
	char *id;
	value_handle vh;

	eh = _get_handle(prhs, 1);
	id = _get_string(prhs, 2);

	vh = ra_value_malloc();
	if ((ra_class_get(eh, id, vh) != 0) ||
		(ra_value_get_num_elem(vh) <= 0))
	{
		ra_value_free(vh);
		_set_double(plhs, 0, mxGetNaN());
		return;
	}
	
	plhs[0] = _value_to_mxa(vh);

	ra_value_free(vh);
} /* if_ra_class_get() */


void
if_ra_class_add_event(mxArray *plhs[], const mxArray *prhs[])
{
	class_handle clh;
	long start, end, ev_id;
	
	clh = _get_handle(prhs, 1);
	start = _get_long(prhs, 2);
	end = _get_long(prhs, 3);

	ev_id = ra_class_add_event(clh, start, end);

	_set_long(plhs, 0, ev_id);
} /* if_ra_class_add_event() */


void
if_ra_class_del_event(mxArray *plhs[], const mxArray *prhs[])
{
	int ret;
	class_handle clh;
	long ev_id;
	
	clh = _get_handle(prhs, 1);
	ev_id = _get_long(prhs, 2);

	ret = ra_class_del_event(clh, ev_id);

	_set_long(plhs, 0, ret);
} /* if_ra_class_del_event() */


void
if_ra_class_get_event_pos(mxArray *plhs[], const mxArray *prhs[])
{
	class_handle clh;
	long ev_id, pos[2];

	clh = _get_handle(prhs, 1);
	ev_id = _get_long(prhs, 2);

	if (ra_class_get_event_pos(clh, ev_id, pos, pos+1) != 0)
	{
		_set_double(plhs, 0, mxGetNaN());
		return;
	}

	_set_long_array(plhs, 0, pos, 2);
} /* if_ra_class_get_event_pos() */


void
if_ra_class_set_event_pos(mxArray *plhs[], const mxArray *prhs[])
{
	int ret;
	class_handle clh;
	long ev_id, start, end;
	
	clh = _get_handle(prhs, 1);
	ev_id = _get_long(prhs, 2);
	start = _get_long(prhs, 3);
	end = _get_long(prhs, 4);

	ret = ra_class_set_event_pos(clh, ev_id, start, end);

	_set_long(plhs, 0, ret);
} /* if_ra_class_set_event_pos() */


void
if_ra_class_get_events(mxArray *plhs[], const mxArray *prhs[])
{
	class_handle clh;
	long start, end, complete, sort;
	value_handle vh;

	clh = _get_handle(prhs, 1);
	start = _get_long(prhs, 2);
	end = _get_long(prhs, 3);
	complete = _get_long(prhs, 4);
	sort = _get_long(prhs, 5);

	vh = ra_value_malloc();
	if (ra_class_get_events(clh, start, end, complete, sort, vh) != 0)
	{
		ra_value_free(vh);
		_set_double(plhs, 0, mxGetNaN());
		return;
	}

	plhs[0] = _value_to_mxa(vh);

	ra_value_free(vh);
} /* if_ra_class_get_events() */


void
if_ra_class_get_prev_event(mxArray *plhs[], const mxArray *prhs[])
{
	class_handle clh;
	long ev_id, ev_id_prev;
	
	clh = _get_handle(prhs, 1);
	ev_id = _get_long(prhs, 2);

	ev_id_prev = ra_class_get_prev_event(clh, ev_id);

	_set_long(plhs, 0, ev_id_prev);
} /* if_ra_class_get_prev_event() */


void
if_ra_class_get_next_event(mxArray *plhs[], const mxArray *prhs[])
{
	class_handle clh;
	long ev_id, ev_id_next;
	
	clh = _get_handle(prhs, 1);
	ev_id = _get_long(prhs, 2);

	ev_id_next = ra_class_get_next_event(clh, ev_id);

	_set_long(plhs, 0, ev_id_next);
} /* if_ra_class_get_next_event() */


void
if_ra_class_get_handle(mxArray *plhs[], const mxArray *prhs[])
{
	prop_handle ph;
	class_handle clh;

	ph = _get_handle(prhs, 1);
	clh = ra_class_get_handle(ph);

	_set_handle(plhs, 0, clh);
} /* if_ra_class_get_handle() */


void
if_ra_prop_add(mxArray *plhs[], const mxArray *prhs[])
{
	class_handle clh;
	char *id;
	long value_type;
	char *name, *desc, *unit;
	long use_minmax;
	double min, max;
	long has_ignore_value;
	double ignore_value;
	prop_handle ph;

	clh = _get_handle(prhs, 1);
	id = _get_string(prhs, 2);
	value_type = _get_long(prhs, 3);
	name = _get_string(prhs, 4);
	desc = _get_string(prhs, 5);
	unit = _get_string(prhs, 6);
	use_minmax = _get_long(prhs, 7);
	min = _get_double(prhs, 8);
	max = _get_double(prhs, 9);
	has_ignore_value = _get_long(prhs, 10);
	ignore_value = _get_double(prhs, 11);

	ph = ra_prop_add(clh, id, value_type, name, desc, unit,
			 use_minmax, min, max, has_ignore_value, ignore_value);

	_set_handle(plhs, 0, ph);
} /* if_ra_prop_add() */


void
if_ra_prop_add_predef(mxArray *plhs[], const mxArray *prhs[])
{
	class_handle clh;
	char *id;
	prop_handle ph;

	clh = _get_handle(prhs, 1);
	id = _get_string(prhs, 2);

	ph = ra_prop_add_predef(clh, id);

	_set_handle(plhs, 0, ph);
} /* if_ra_prop_add_predef() */


void
if_ra_prop_delete(mxArray *plhs[], const mxArray *prhs[])
{
	int ret;
	prop_handle ph;

	ph = _get_handle(prhs, 1);
	ret = ra_prop_delete(ph);

	_set_long(plhs, 0, ret);
} /* if_ra_prop_delete() */


void
if_ra_prop_get(mxArray *plhs[], const mxArray *prhs[])
{
	class_handle clh;
	char *id;
	prop_handle ph;

	clh = _get_handle(prhs, 1);
	id = _get_string(prhs, 2);

	ph = ra_prop_get(clh, id);

	_set_handle(plhs, 0, ph);
} /* if_ra_prop_get() */


void
if_ra_prop_get_all(mxArray *plhs[], const mxArray *prhs[])
{
	class_handle clh;
	value_handle vh;

	clh = _get_handle(prhs, 1);

	vh = ra_value_malloc();
	if (ra_prop_get_all(clh, vh) != 0)
	{
		ra_value_free(vh);
		_set_double(plhs, 0, mxGetNaN());
		return;
	}

	plhs[0] = _value_to_mxa(vh);

	ra_value_free(vh);
} /* if_ra_prop_get_all() */


void
if_ra_prop_set_value(mxArray *plhs[], const mxArray *prhs[])
{
	int ret = -1;
	prop_handle ph;
	long ev_id, ch;
	char *val_type;
	value_handle vh;
	
	ph = _get_handle(prhs, 1);
	ev_id = _get_long(prhs, 2);
	ch = _get_long(prhs, 3);
	val_type = _get_string(prhs, 4);

	vh = _mxa_to_value(val_type, prhs, 5);
	if (vh)
	{
		ret = ra_prop_set_value(ph, ev_id, ch, vh);
		ra_value_free(vh);
	}
	
	_set_long(plhs, 0, ret);
} /* if_ra_prop_set_value() */


void
if_ra_prop_set_value_mass(mxArray *plhs[], const mxArray *prhs[])
{
	if (plhs || prhs);

	/* TODO: think if this is really necessary and if how to
	   be sure that the id and ch vectors are correct */
} /* if_ra_prop_set_value_mass() */


void
if_ra_prop_get_ch(mxArray *plhs[], const mxArray *prhs[])
{
	prop_handle ph;
	long ev_id;
	value_handle vh;

	ph = _get_handle(prhs, 1);
	ev_id = _get_long(prhs, 2);

	vh = ra_value_malloc();
	if (ra_prop_get_ch(ph, ev_id, vh) != 0)
	{
		ra_value_free(vh);
		_set_double(plhs, 0, mxGetNaN());
		return;
	}

	plhs[0] = _value_to_mxa(vh);

	ra_value_free(vh);
} /* if_ra_prop_get_ch() */


void
if_ra_prop_get_value(mxArray *plhs[], const mxArray *prhs[])
{
	prop_handle ph;
	long ev_id, ch;
	value_handle vh;

	ph = _get_handle(prhs, 1);
	ev_id = _get_long(prhs, 2);
	ch = _get_long(prhs, 3);

	vh = ra_value_malloc();
	if (ra_prop_get_value(ph, ev_id, ch, vh) != 0)
	{
		ra_value_free(vh);
		_set_double(plhs, 0, mxGetNaN());
		return;
	}

	plhs[0] = _value_to_mxa(vh);

	ra_value_free(vh);
} /* if_ra_prop_get_value() */


void
if_ra_prop_get_value_all(mxArray *plhs[], const mxArray *prhs[])
{
	prop_handle ph;
	value_handle event_id, ch, value;

	ph = _get_handle(prhs, 1);

	event_id = ra_value_malloc();
	ch = ra_value_malloc();
	value = ra_value_malloc();

	if (ra_prop_get_value_all(ph, event_id, ch, value) != 0)
	{
		ra_value_free(event_id);
		ra_value_free(ch);
		ra_value_free(value);

		_set_double(plhs, 0, mxGetNaN());
		_set_double(plhs, 1, mxGetNaN());
		_set_double(plhs, 2, mxGetNaN());
		return;
	}

	plhs[0] = _value_to_mxa(event_id);
	plhs[1] = _value_to_mxa(ch);
	plhs[2] = _value_to_mxa(value);

	ra_value_free(event_id);
	ra_value_free(ch);
	ra_value_free(value);
} /* if_ra_prop_get_value_all() */


void
if_ra_prop_get_events(mxArray *plhs[], const mxArray *prhs[])
{
	prop_handle ph;
	char *minmax_type;
	value_handle min, max;
	long ch;
	value_handle vh;

	ph = _get_handle(prhs, 1);
	minmax_type = _get_string(prhs, 2);
	min = _mxa_to_value(minmax_type, prhs, 3);
	max = _mxa_to_value(minmax_type, prhs, 4);
	ch = _get_long(prhs, 5);

	vh = ra_value_malloc();
	if (ra_prop_get_events(ph, min, max, ch, vh) != 0)
	{
		ra_value_free(min);
		ra_value_free(max);
		_set_double(plhs, 0, mxGetNaN());
		return;
	}

	plhs[0] = _value_to_mxa(vh);

	ra_value_free(vh);
} /* if_ra_prop_get_events() */


void
if_ra_sum_add(mxArray *plhs[], const mxArray *prhs[])
{
	class_handle clh;
	char *id, *name, *desc;
	long num_dim;
	char **dim_name, **dim_unit;
	long num_dim_got;
	sum_handle sh;
		
	clh = _get_handle(prhs, 1);
	id = _get_string(prhs, 2);
	name = _get_string(prhs, 3);
	desc = _get_string(prhs, 4);
	num_dim = _get_long(prhs, 5);
	dim_name = _get_string_array(prhs, 6, &num_dim_got);
	if (num_dim != num_dim_got)
	{
		_set_double(plhs, 0, mxGetNaN());
		return;
	}
	dim_unit = _get_string_array(prhs, 7, &num_dim_got);
	if (num_dim != num_dim_got)
	{
		_set_double(plhs, 0, mxGetNaN());
		return;
	}

	sh = ra_sum_add(clh, id, name, desc, num_dim, (const char **)dim_name, (const char **)dim_unit);

	_set_handle(plhs, 0, sh);
} /* if_ra_sum_add() */


void
if_ra_sum_add_ch(mxArray *plhs[], const mxArray *prhs[])
{
	int ret;
	sum_handle sh;
	long ch, fid_off;

	sh = _get_handle(prhs, 1);
	ch = _get_long(prhs, 2);
	fid_off = _get_long(prhs, 3);

	ret = ra_sum_add_ch(sh, ch, fid_off);

	_set_long(plhs, 0, ret);
} /* if_ra_sum_add_ch() */


void
if_ra_sum_delete(mxArray *plhs[], const mxArray *prhs[])
{
	int ret;
	sum_handle sh;

	sh = _get_handle(prhs, 1);
	ret = ra_sum_delete(sh);

	_set_long(plhs, 0, ret);
} /* if_ra_sum_delete() */


void
if_ra_sum_get(mxArray *plhs[], const mxArray *prhs[])
{
	class_handle clh;
	char *id;
	value_handle vh;
	
	clh = _get_handle(prhs, 1);
	id = _get_string(prhs, 2);
	
	vh = ra_value_malloc();
	if (ra_sum_get(clh, id, vh) != 0)
	{
		ra_value_free(vh);
		_set_double(plhs, 0, mxGetNaN());
		return;
	}

	plhs[0] = _value_to_mxa(vh);

	ra_value_free(vh);
} /* if_ra_sum_get() */


void
if_ra_sum_add_part(mxArray *plhs[], const mxArray *prhs[])
{
	sum_handle sh;
	long num_events, *ev_ids, num_got, part_id;

	sh = _get_handle(prhs, 1);
	num_events = _get_long(prhs, 2);
	ev_ids = _get_long_array(prhs, 3, &num_got);
	if (num_events != num_got)
		return;

	part_id = ra_sum_add_part(sh, num_events, ev_ids);

	_set_long(plhs, 0, part_id);
} /* if_ra_sum_add_part() */


void
if_ra_sum_del_part(mxArray *plhs[], const mxArray *prhs[])
{
	int ret;
	sum_handle sh;
	long part_id;

	sh = _get_handle(prhs, 1);
	part_id = _get_long(prhs, 2);

	ret = ra_sum_del_part(sh, part_id);

	_set_long(plhs, 0, ret);
} /* if_ra_sum_del_part() */


void
if_ra_sum_get_part_all(mxArray *plhs[], const mxArray *prhs[])
{
	sum_handle sh;
	value_handle vh;

	sh = _get_handle(prhs, 1);
	
	vh = ra_value_malloc();
	if (ra_sum_get_part_all(sh, vh) != 0)
	{
		ra_value_free(vh);
		_set_double(plhs, 0, mxGetNaN());
		return;
	}

	plhs[0] = _value_to_mxa(vh);

	ra_value_free(vh);
} /* if_ra_sum_get_part_all() */


void
if_ra_sum_set_part_data(mxArray *plhs[], const mxArray *prhs[])
{
	int ret = -1;
	sum_handle sh;
	long part_id, ch_idx, dim;
	char *val_type;
	value_handle vh;

	sh = _get_handle(prhs, 1);
	part_id = _get_long(prhs, 2);
	ch_idx = _get_long(prhs, 3);
	dim = _get_long(prhs, 4);
	val_type = _get_string(prhs, 5);
	vh = _mxa_to_value(val_type, prhs, 6);

	if (vh)
	{
		ret = ra_sum_set_part_data(sh, part_id, ch_idx, dim, vh);
		ra_value_free(vh);
	}

	_set_long(plhs, 0, ret);
} /* if_ra_sum_set_part_data() */


void
if_ra_sum_get_part_data(mxArray *plhs[], const mxArray *prhs[])
{
	sum_handle sh;
	long part_id, ch_idx, dim;
	value_handle vh;

	sh = _get_handle(prhs, 1);
	part_id = _get_long(prhs, 2);
	ch_idx = _get_long(prhs, 3);
	dim = _get_long(prhs, 4);

	vh = ra_value_malloc();
	if (ra_sum_get_part_data(sh, part_id, ch_idx, dim, vh) != 0)
	{
		ra_value_free(vh);
		_set_double(plhs, 0, mxGetNaN());
		return;
	}

	plhs[0] = _value_to_mxa(vh);

	ra_value_free(vh);
} /* if_ra_sum_get_part_data() */


void
if_ra_sum_get_part_events(mxArray *plhs[], const mxArray *prhs[])
{
	sum_handle sh;
	long part_id;
	value_handle vh;

	sh = _get_handle(prhs, 1);
	part_id = _get_long(prhs, 2);

	vh = ra_value_malloc();
	if (ra_sum_get_part_events(sh, part_id, vh) != 0)
	{
		ra_value_free(vh);
		_set_double(plhs, 0, mxGetNaN());
		return;
	}

	plhs[0] = _value_to_mxa(vh);

	ra_value_free(vh);
} /* if_ra_sum_get_part_events() */


void
if_ra_sum_set_part_events(mxArray *plhs[], const mxArray *prhs[])
{
	sum_handle sh;
	long part_id;
	value_handle vh;
	int ret;

	sh = _get_handle(prhs, 1);
	part_id = _get_long(prhs, 2);
	vh = _mxa_to_value("l", prhs, 3);

	ret = ra_sum_set_part_events(sh, part_id, vh);

	ra_value_free(vh);

	_set_long(plhs, 0, ret);
} /* if_ra_sum_set_part_events() */


void
mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	int func_idx;
	void (*func)(mxArray **, const mxArray **);
	char *func_name;
	int i;

	/* Check for proper number of arguments */    
	if (nrhs <= 0)
		mexErrMsgTxt("libRASCH-Matlab is corrupted. No func-name");

	func_name = _get_string(prhs, 0);
	func_idx = -1;
	for (i = 0; i < (int)(sizeof(f) / sizeof(f[0])); i++)
	{
		if (f[i].func_name == NULL)
			continue;
		if (strcmp(func_name, f[i].func_name) == 0)
		{
			func_idx = i;
			break;
		}
	}

	if (func_idx == -1)
		mexErrMsgTxt("libRASCH-Matlab is corrupted: did not found func-name");

	if (nrhs != (f[func_idx].n_args_in + 1))
	{
		char s[1000];
		sprintf(s, "func: %d  #in=%d  #out=%d", func_idx, nrhs, nlhs);
		mexErrMsgTxt(s);
	}
	if (nlhs != f[func_idx].n_args_out)
		mexErrMsgTxt("libRASCH-Matlab is corrupted: wrong #output-args");

	func = f[func_idx].func;

	(*func)(plhs, prhs);

	return;    
} /* mexFucntion() */
