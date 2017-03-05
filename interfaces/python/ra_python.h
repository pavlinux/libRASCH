#ifndef RA_PYTHON_H
#define RA_PYTHON_H


#include "../shared/infos.h"


long
get_info_id(char *name)
{
	long id, l;
	
	id = -1;
	for (l = 0; l < (long)(sizeof(infos)/sizeof(infos[0])); l++)
	{
		if (infos[l].name == NULL)
			continue;
		if (strcmp(name, infos[l].name) == 0)
		{
			id = infos[l].id;
			break;
		}
	}

	return id;
} /* get_info_id() */


long
get_info_type(char *name)
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
} /* get_info_type() */


PyObject *
value_to_py_list(value_handle vh)
{
	PyObject *list;
	int is_string = 0;

	if (ra_value_get_type(vh) == RA_VALUE_TYPE_CHAR)
		is_string = 1;

	if (is_string)
		list = PyList_New(5);
	else
		list = PyList_New(4);

	switch (ra_value_get_type(vh))
	{
	case RA_VALUE_TYPE_LONG:
		PyList_SetItem(list, 0, Py_BuildValue("l", ra_value_get_long(vh)));
		break;
	case RA_VALUE_TYPE_DOUBLE:
		PyList_SetItem(list, 0, Py_BuildValue("d", ra_value_get_double(vh)));
		break;
	case RA_VALUE_TYPE_CHAR:
		PyList_SetItem(list, 0, Py_BuildValue("s", ra_value_get_string(vh)));
		PyList_SetItem(list, 4, Py_BuildValue("s", ra_value_get_string_utf8(vh)));
		break;
	default:
		PyList_SetItem(list, 0, Py_BuildValue(""));
		break;
	}

	PyList_SetItem(list, 1, Py_BuildValue("s", ra_value_get_name(vh)));
	PyList_SetItem(list, 2, Py_BuildValue("s", ra_value_get_desc(vh)));
	PyList_SetItem(list, 3, Py_BuildValue("l", ra_value_info_modifiable(vh)));

	return list;
} /* value_to_py_list() */


PyObject *
value_to_py_object(value_handle vh)
{
	PyObject *ret;
	long l, n;
	const short *buf_s;
	const long *buf_l;
	const double *buf_d;
	const char **buf_c;
	const void **buf_v;

	n = ra_value_get_num_elem(vh);

	switch (ra_value_get_type(vh))
	{
	case RA_VALUE_TYPE_SHORT:
		ret = Py_BuildValue("h", ra_value_get_short(vh));
		break;
	case RA_VALUE_TYPE_LONG:
		ret = Py_BuildValue("l", ra_value_get_long(vh));
		break;
	case RA_VALUE_TYPE_DOUBLE:
		ret = Py_BuildValue("d", ra_value_get_double(vh));
		break;
	case RA_VALUE_TYPE_CHAR:
		ret = Py_BuildValue("s", ra_value_get_string(vh));
		break;
	case RA_VALUE_TYPE_VOIDP:
		ret = Py_BuildValue("l", (long)ra_value_get_voidp(vh));
		break;
	case RA_VALUE_TYPE_SHORT_ARRAY:
		ret = PyList_New(n);
		buf_s = ra_value_get_short_array(vh);
		for(l = 0; l < n; l++)
			PyList_SetItem(ret, l, Py_BuildValue("h", buf_s[l]));
		break;
	case RA_VALUE_TYPE_LONG_ARRAY:
		ret = PyList_New(n);
		buf_l = ra_value_get_long_array(vh);
		for(l = 0; l < n; l++)
			PyList_SetItem(ret, l, Py_BuildValue("l", buf_l[l]));
		break;
	case RA_VALUE_TYPE_DOUBLE_ARRAY:
		ret = PyList_New(n);
		buf_d = ra_value_get_double_array(vh);
		for(l = 0; l < n; l++)
			PyList_SetItem(ret, l, Py_BuildValue("d", buf_d[l]));
		break;
	case RA_VALUE_TYPE_CHAR_ARRAY:
		ret = PyList_New(n);
		buf_c = ra_value_get_string_array(vh);
		for(l = 0; l < n; l++)
			PyList_SetItem(ret, l, Py_BuildValue("s", buf_c[l]));
		break;
	case RA_VALUE_TYPE_VOIDP_ARRAY:
		ret = PyList_New(n);
		buf_v = ra_value_get_voidp_array(vh);
		for(l = 0; l < n; l++)
		{
			long t = (long)(buf_v[l]);
			PyList_SetItem(ret, l, Py_BuildValue("l", t));
		}
		break;
	default:
		ret = Py_BuildValue("");
		break;
	}

	return ret;
} /* value_to_py_object() */


int
py_object_to_value(PyObject *value, char *type, value_handle vh)
{
	int ret = 0;

	if (type[0] == 'p')  /* check if it's an array */
	{
		int len, i;
		PyObject *item;
		
		if (!PyList_Check(value))
			return -1;
		
		len = PyList_Size(value);
		
		if (type[1] == 'l')  /* long array? */
		{
			long *all = malloc(sizeof(long) * len);
			for (i = 0; i < len; i++)
			{
				item = PyList_GetItem(value, i);
				all[i] = PyLong_AsLong(item);
			}
			ra_value_set_long_array(vh, all, len);
			
			free(all);
		}
		else if (type[1] == 'd')  /* or double array? */
		{
			double *all = malloc(sizeof(double) * len);
			for (i = 0; i < len; i++)
			{
				item = PyList_GetItem(value, i);
				all[i] = PyFloat_AsDouble(item);
			}
			ra_value_set_double_array(vh, all, len);
			
			free(all);
		}
		else if (type[1] == 'c')  /* or string array? */
		{
			char *str;
			long str_len;
			
			char **all = calloc(len, sizeof(char *));
				
			for (i = 0; i < len; i++)
			{
				item = PyList_GetItem(value, i);
				str = PyString_AsString(item);
				str_len = strlen(str);
				all[i] = malloc(sizeof(char) * (str_len+1));
				strncpy(all[i], str, len);
			}
			if (type[2] == 'u')
				ra_value_set_string_array_utf8(vh, (const char **)all, len);
			else
				ra_value_set_string_array(vh, (const char **)all, len);
			
			for (i = 0; i < len; i++)
				free(all[i]);
			free(all);
		}
	}
	else if (type[0] == 'l') /* is it a long? */
	{
		ra_value_set_long(vh, PyLong_AsLong(value));
	}
	else if (type[0] == 'd') /* is it a double? */
	{
		ra_value_set_double(vh, PyFloat_AsDouble(value));
	}
	else if (type[0] == 'c') /* is it a string? */
	{
		if (type[1] == 'u')
			ra_value_set_string_utf8(vh, PyString_AsString(value));
		else
			ra_value_set_string(vh, PyString_AsString(value));
	}
	else
		ret = -1;

	return ret;
} /* py_object_to_value() */


#endif /* RA_PYTHON_H */
