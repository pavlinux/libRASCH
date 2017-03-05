/**
 * \file value.c
 *
 * This file provides the ra-value related libRASCH-API functions
 *
 * \author Raphael Schneider (rasch@med1.med.tum.de)
 */

/*--------------------------------------------------------------------------
 *
 * Copyright (C) 2004-2007, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *
 *--------------------------------------------------------------------------*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <locale.h>

#ifdef WIN32
#include <windows.h>
#endif

#define _LIBRASCH_BUILD
#include <ra.h>
#include <ra_priv.h>
#include <pl_general.h>


void free_ra_value_mem(struct ra_value *v);


/**
 * \brief allocate and initialize a value_handle
 *
 * Returns a initialzed value-handle.
 */
LIBRAAPI value_handle
ra_value_malloc()
{
	struct ra_value *v;

	v = malloc(sizeof(struct ra_value));
	memset(v, 0, sizeof(struct ra_value));

	v->handle_id = RA_HANDLE_VALUE;

	return (value_handle)v;
} /* ra_value_malloc() */


/**
 * \brief free value-handle
 * \param <vh> value-handle
 *
 * Frees the memory asocciated with vh.
 */
LIBRAAPI void
ra_value_free(value_handle vh)
{
	struct ra_value *v = (struct ra_value *)vh;

	if (!v || !valid_value_handle(vh, __LINE__))
		return;

	free_ra_value_mem(v);
	free(v);
} /* ra_value_free() */


/**
 * \brief checks the validity of a value-handle
 * \param <vh> value-handle
 *
 * Function checks if the handle given by 'vh' is really a value-handle. If it is
 * another handle, assert(0) is called which stops the program.
 */
int
valid_value_handle(value_handle vh, int line_num)
{
	int ok = 1;
	struct ra_value *v = (struct ra_value *)vh;
	if (!v)
		return ok;

	if (v->handle_id != RA_HANDLE_VALUE)
		ok = 0;

	if (!ok)		/* TODO: add condition to print only when LIBRASCH_DEBUG is set */
	{
		fprintf(stderr, "called from line %d\n", line_num);
		assert(0);
	}

	return ok;
} /* valid_value_handle() */


/**
 * \brief reset value-handle
 * \param <vh> value-handle
 *
 * Reset value-handle vh.
 */
LIBRAAPI void
ra_value_reset(value_handle vh)
{
	struct ra_value *v = (struct ra_value *)vh;
	if (!v || !valid_value_handle(vh, __LINE__))
		return;

	free_ra_value_mem(v);
} /* ra_value_reset() */


/**
 * \brief return value-type
 * \param <vh> value-handle
 *
 * Return the type of the value stored in vh. Check in ra_defines.h
 * for the meaning of the returned value (RA_VALUE_TYPE_*).
 */
LIBRAAPI long
ra_value_get_type(value_handle vh)
{
	struct ra_value *v = (struct ra_value *)vh;
	if (!v || !valid_value_handle(vh, __LINE__))
		return RA_VALUE_TYPE_NONE;

	return v->utype;
} /* ra_value_get_type() */


/**
 * \brief checks if value is valid
 * \param <vh> value-handle
 *
 * Returns '1' if value stored in vh is valid, '0' if not.
 */
LIBRAAPI int
ra_value_is_ok(value_handle vh)
{
	struct ra_value *v = (struct ra_value *)vh;
	if (!v || !valid_value_handle(vh, __LINE__))
		return 0;

	return v->value_is_valid;
} /* ra_value_is_ok() */


/**
 * \brief return number of elements of array values
 * \param <vh> value-handle
 *
 * When an array is stored in vh, the number of elements of the array
 * is returned.
 */
LIBRAAPI unsigned long
ra_value_get_num_elem(value_handle vh)
{
	struct ra_value *v = (struct ra_value *)vh;
	if (!v || !valid_value_handle(vh, __LINE__) || !v->value_is_valid)
		return 0;

	return v->num_values;
} /* ra_value_get_num_elem() */


/**
 * \brief returns id of info
 * \param <vh> value-handle
 *
 * Returns the id of the info set in vh. Check in ra_defines.h for
 * for the meaning of info-id (RA_INFO_*).
 */
LIBRAAPI long
ra_value_get_info(value_handle vh)
{
	struct ra_value *v = (struct ra_value *)vh;
	if (!v || !valid_value_handle(vh, __LINE__))
		return RA_INFO_NONE;

	return v->id;
} /* ra_value_get_info() */


/**
 * \brief sets the RA_INFO_* id of info
 * \param <vh> value-handle
 * \param <id> info-id
 *
 * Sets the id of the info in vh. Check in ra_defines.h for
 * for the meaning of info-id (RA_INFO_*).
 */
LIBRAAPI int
ra_value_set_info(value_handle vh, long id)
{
	struct ra_value *v = (struct ra_value *)vh;
	if (!v || !valid_value_handle(vh, __LINE__))
		return RA_ERR_WRONG_HANDLE;

	if (get_meta_info(id) == NULL)
		return RA_ERR_OUT_OF_RANGE;
	v->id = id;

	return RA_ERR_NONE;
} /* ra_value_set_info() */


/**
 * \brief returns name of info
 * \param <vh> value-handle
 *
 * Returns a character pointer to the name of the info set in vh.
 * The pointer is valid until vh is free'd or a new info was retrived.
 */
LIBRAAPI const char *
ra_value_get_name(value_handle vh)
{
	struct ra_value *v = (struct ra_value *)vh;
	if (!v || !valid_value_handle(vh, __LINE__))
		return NULL;

	utf8_to_local(v->name, v->name_locale, RA_VALUE_NAME_MAX);

	return v->name_locale;
} /* ra_value_get_name() */


/**
 * \brief returns description of info
 * \param <vh> value-handle
 *
 * Returns a character pointer to the description of the info set in vh.
 * The pointer is valid until vh is free'd or a new info was retrived.
 */
LIBRAAPI const char *
ra_value_get_desc(value_handle vh)
{
	struct ra_value *v = (struct ra_value *)vh;
	if (!v || !valid_value_handle(vh, __LINE__))
		return NULL;

	utf8_to_local(v->desc, v->desc_locale, RA_VALUE_DESC_MAX);

	return v->desc_locale;
} /* ra_value_get_desc() */


/**
 * \brief returns flag if info in measurement file(s) can be modified
 * \param <vh> value-handle
 *
 * Returns a flag if the info currently handled by the value-handle,
 * can be modified in the measurement file(s).
 */
LIBRAAPI int
ra_value_info_modifiable(value_handle vh)
{
	struct ra_value *v = (struct ra_value *)vh;
	if (!v || !valid_value_handle(vh, __LINE__))
		return 0;

	return v->can_be_changed;
} /* ra_value_info_modifiable() */


/**
 * \brief set flag if info in measurement file(s) can be modified
 * \param <vh> value-handle
 * \param <can_be_modfied> flag
 *
 * Sets the flag if the info currently handled by the value-handle,
 * can be modified in the measurement file(s).
 */
LIBRAAPI void
ra_value_info_set_modifiable(value_handle vh, int can_be_modified)
{
	struct ra_value *v = (struct ra_value *)vh;
	if (!v || !valid_value_handle(vh, __LINE__))
		return;

	v->can_be_changed = can_be_modified;
} /* ra_value_info_set_modifiable() */


/**
 * \brief set a number in a value-handle
 * \param <vh> value-handle
 * \param <number> number
 *
 * Set the number number in the value-handle vh. This number is
 * used when infos about channels, recording devices and plugins. number
 * is also used when processing results are retrived.
 */
LIBRAAPI int
ra_value_set_number(value_handle vh, long number)
{
	struct ra_value *v = (struct ra_value *)vh;
	if (!v || !valid_value_handle(vh, __LINE__))
		return -1;

	v->number = number;

	return 0;
} /* ra_value_set_number() */


/**
 * \brief get a number set in a value-handle
 * \param <vh> value-handle
 *
 * Returns the number number set in the value-handle vh.
 */
LIBRAAPI long
ra_value_get_number(value_handle vh)
{
	struct ra_value *v = (struct ra_value *)vh;

	if (!v || !valid_value_handle(vh, __LINE__))
		return 0;

	return v->number;
} /* ra_value_get_number() */


/**
 * \brief set name and description in a value-handle
 * \param <vh> value-handle
 * \param <name> name of the value
 * \param <desc> a short description of the value
 * \param <id> id of the value (if it is an libRASCH info)
 *
 * When value-handle represents some information set inside the core or in a
 * plugin, most of the times a name and description is available.
 */
int
set_meta_info(value_handle vh, const char *name, const char *desc, int id)
{
	struct ra_value *v = (struct ra_value *)vh;
	if (!v || !valid_value_handle(vh, __LINE__))
		return -1;

	v->can_be_changed = 0;
	if (name && (name[0] != '\0'))
	{
		strncpy(v->name, gettext(name), RA_VALUE_NAME_MAX-1);
		v->name[RA_VALUE_NAME_MAX-1] = '\0';
	}
	else
		v->name[0] = '\0';

	if (desc && (desc[0] != '\0'))
	{
		strncpy(v->desc, gettext(desc), RA_VALUE_DESC_MAX-1);
		v->desc[RA_VALUE_DESC_MAX-1] = '\0';
	}
	else
		v->desc[0] = '\0';

	v->id = id;

	return 0;
} /* set_meta_info() */


/**
 * \brief set short value in value-handle
 * \param <vh> value-handle
 * \param <value> value to set in ra_info
 *
 * This function set the value in vh.
 */
LIBRAAPI void
ra_value_set_short(value_handle vh, const short value)
{
	struct ra_value *v = (struct ra_value *)vh;

	if (!v || !valid_value_handle(vh, __LINE__))
		return;

	free_ra_value_mem(v);
	v->value.s = value;
	v->utype = RA_VALUE_TYPE_SHORT;
	v->value_is_valid = 1;
	v->num_values = 1;
} /* ra_value_set_short() */


/**
 * \brief set long value in value-handle
 * \param <vh> value-handle
 * \param <value> value to set in ra_info
 *
 * This function set the value in vh.
 */
LIBRAAPI void
ra_value_set_long(value_handle vh, const long value)
{
	struct ra_value *v = (struct ra_value *)vh;

	if (!v || !valid_value_handle(vh, __LINE__))
		return;

	free_ra_value_mem(v);
	v->value.l = value;
	v->utype = RA_VALUE_TYPE_LONG;
	v->value_is_valid = 1;
	v->num_values = 1;
} /* ra_value_set_long() */


/**
 * \brief set double value in value-handle
 * \param <vh> value-handle
 * \param <value> value to set in ra_info
 *
 * This function set the value in vh.
 */
LIBRAAPI void
ra_value_set_double(value_handle vh, const double value)
{
	struct ra_value *v = (struct ra_value *)vh;

	if (!v || !valid_value_handle(vh, __LINE__))
		return;

	free_ra_value_mem(v);
	v->value.d = value;
	v->utype = RA_VALUE_TYPE_DOUBLE;
	v->value_is_valid = 1;
	v->num_values = 1;
} /* ra_value_set_double() */


/**
 * \brief set a string in value-handle
 * \param <vh> value-handle
 * \param <string> string to set in ra_info
 *
 * This function set the string (`\\0`-ended char *) in vh.
 */
LIBRAAPI void
ra_value_set_string(value_handle vh, const char *string)
{
	struct ra_value *v = (struct ra_value *)vh;
	char *conv = NULL;
	size_t len;

	if (!string || !v || !valid_value_handle(vh, __LINE__))
		return;

	free_ra_value_mem(v);

	v->value_locale.c = ra_alloc_mem(strlen(string) + 1);
	memset(v->value_locale.c, '\0', strlen(string)+1);
	if (v->value_locale.c != NULL)
		strncpy(v->value_locale.c, string, strlen(string));

	/* now convert it to UTF-8 and store it */
	if (strlen(string) > 0)
	{
		len = strlen(string) * 2;
		conv = calloc(len, 1);
		local_to_utf8(string, conv, len);
		len = strlen(conv);
		v->value.c = ra_alloc_mem(len + 1);
		memset(v->value.c, '\0', len+1);
		if (v->value.c != NULL)
			strncpy(v->value.c, conv, len);
		free(conv);
	}
	else
	{
		v->value.c = ra_alloc_mem(1);
		v->value.c[0] = '\0';
	}

	v->utype = RA_VALUE_TYPE_CHAR;
	v->value_is_valid = 1;
	v->num_values = 1;
} /* ra_value_set_string() */


/**
 * \brief set a UTF-8 encoded string in value-handle
 * \param <vh> value-handle
 * \param <string> string to set in ra_info
 *
 * This function set the string (`\\0`-ended char *) in vh.
 */
LIBRAAPI void
ra_value_set_string_utf8(value_handle vh, const char *string)
{
	struct ra_value *v = (struct ra_value *)vh;
	char *conv = NULL;
	size_t len;

	if (!string || !v || !valid_value_handle(vh, __LINE__))
		return;

	free_ra_value_mem(v);

	v->value.c = ra_alloc_mem(strlen(string) + 1);
	memset(v->value.c, '\0', strlen(string)+1);
	if (v->value.c != NULL)
		strcpy(v->value.c, string);

	/* now convert it to locale setting and store it */
	if (strlen(string) > 0)
	{
		len = strlen(string) * 2;
		conv = calloc(len, 1);
		utf8_to_local(string, conv, len);
		len = strlen(conv);
		v->value_locale.c = ra_alloc_mem(len + 1);
		memset(v->value_locale.c, '\0', len+1);
		if (v->value_locale.c != NULL)
			strcpy(v->value_locale.c, conv);
		free(conv);
	}
	else
	{
		v->value_locale.c = ra_alloc_mem(1);
		v->value_locale.c[0] = '\0';
	}

	v->utype = RA_VALUE_TYPE_CHAR;
	v->value_is_valid = 1;
	v->num_values = 1;
} /* ra_value_set_string_utf8() */


/**
 * \brief set void-pointer value in value-handle
 * \param <vh> value-handle
 * \param <value> value to set in ra_info
 *
 * This function set the value in vh.
 */
LIBRAAPI void
ra_value_set_voidp(value_handle vh, const void *value)
{
	struct ra_value *v = (struct ra_value *)vh;

	if (!v || !valid_value_handle(vh, __LINE__))
		return;

	free_ra_value_mem(v);
	v->value.vp = (void *)value;
	v->utype = RA_VALUE_TYPE_VOIDP;
	v->value_is_valid = 1;
	v->num_values = 1;
} /* ra_value_set_voidp() */


/**
 * \brief set short-array values in value-handle
 * \param <vh> value-handle
 * \param <array> array to set in ra_info
 * \param <num> number of array elements
 *
 * This function set the short array array with num elements in vh.
 */
LIBRAAPI void
ra_value_set_short_array(value_handle vh, const short *array, unsigned long num)
{
	struct ra_value *v = (struct ra_value *)vh;
	size_t size;

	if (!v || !array || !valid_value_handle(vh, __LINE__))
		return;

	free_ra_value_mem(v);

	size = sizeof(short) * num;
	v->value.sp = ra_alloc_mem(size);
	if (v->value.sp)
	{
		memcpy(v->value.sp, array, size);
		v->utype = RA_VALUE_TYPE_SHORT_ARRAY;
		v->num_values = num;
	}
	v->value_is_valid = 1;
} /* ra_value_set_short_array() */


/**
 * \brief set long-array values in value-handle
 * \param <vh> value-handle
 * \param <array> array to set in ra_info
 * \param <num> number of array elements
 *
 * This function set the long array array with num elements in vh.
 */
LIBRAAPI void
ra_value_set_long_array(value_handle vh, const long *array, unsigned long num)
{
	struct ra_value *v = (struct ra_value *)vh;
	size_t size;

	if (!v || !array || !valid_value_handle(vh, __LINE__))
		return;

	free_ra_value_mem(v);

	size = sizeof(long) * num;
	v->value.lp = ra_alloc_mem(size);
	if (v->value.lp)
	{
		memcpy(v->value.lp, array, size);
		v->utype = RA_VALUE_TYPE_LONG_ARRAY;
		v->num_values = num;
	}
	v->value_is_valid = 1;
} /* ra_value_set_long_array() */


/**
 * \brief set double-array values in value-handle
 * \param <vh> value-handle
 * \param <array> array to set in ra_info
 * \param <num> number of array elements
 *
 * This function set the double array array with num elements in vh.
 */
LIBRAAPI void
ra_value_set_double_array(value_handle vh, const double *array, unsigned long num)
{
	struct ra_value *v = (struct ra_value *)vh;
	size_t size;

	if (!v || !array || !valid_value_handle(vh, __LINE__))
		return;

	free_ra_value_mem(v);

	size = sizeof(double) * num;
	v->value.dp = ra_alloc_mem(size);
	if (v->value.dp)
	{
		memcpy(v->value.dp, array, size);
		v->utype = RA_VALUE_TYPE_DOUBLE_ARRAY;
		v->num_values = num;
	}
	v->value_is_valid = 1;
} /* ra_value_set_double_array() */


/**
 * \brief set a string-array in value-handle
 * \param <vh> value-handle
 * \param <array> array to set in ra_info
 * \param <num> number of array elements
 *
 * This function set the string-array array with num elements in vh. 
 */
LIBRAAPI void
ra_value_set_string_array(value_handle vh, const char **array, unsigned long num)
{
	struct ra_value *v = (struct ra_value *)vh;
	size_t size;
	unsigned long l;

	if (!v || !array || !valid_value_handle(vh, __LINE__))
		return;

	free_ra_value_mem(v);

	size = sizeof(char *) * num;
	v->value_locale.cp = ra_alloc_mem(size);
	if (v->value_locale.cp)
	{
		for (l = 0; l < num; l++)
		{
			v->value_locale.cp[l] = ra_alloc_mem(strlen(array[l]) + 1);
			memset(v->value_locale.cp[l], '\0', strlen(array[l])+1);
			if (v->value_locale.cp[l])
				memcpy(v->value_locale.cp[l], array[l], strlen(array[l]) + 1);
		}
	}

	/* convert all strings to UTF-8 encoding and store them */
	v->value.cp = ra_alloc_mem(size);
	if (v->value.cp)
	{
		size_t len;
		char *conv = NULL;

		for (l = 0; l < num; l++)
		{
			if (strlen(array[l]) > 0)
			{
				len = strlen(array[l]) * 2;
				conv = calloc(len, 1);
				local_to_utf8(array[l], conv, len);
				len = strlen(conv);
				v->value.cp[l] = ra_alloc_mem(len + 1);
				memset(v->value.cp[l], '\0', len+1);
				if (v->value.cp[l])
					memcpy(v->value.cp[l], conv, len + 1);
				free(conv);
			}
			else
			{
				v->value.cp[l] = ra_alloc_mem(1);
				v->value.cp[l][0] = '\0';
			}
		}
	}

	v->utype = RA_VALUE_TYPE_CHAR_ARRAY;
	v->num_values = num;
	v->value_is_valid = 1;
} /* ra_value_set_string_array() */


/**
 * \brief set a string-array (UTF-8 encoded) in value-handle
 * \param <vh> value-handle
 * \param <array> array to set in ra_info
 * \param <num> number of array elements
 *
 * This function set the string-array 'array' with 'num' elements in vh. The
 * strings in 'array' are expected to be UTF-8 encoded.
 */
LIBRAAPI void
ra_value_set_string_array_utf8(value_handle vh, const char **array, unsigned long num)
{
	struct ra_value *v = (struct ra_value *)vh;
	size_t size;
	unsigned long l;

	if (!v || !array || !valid_value_handle(vh, __LINE__))
		return;

	free_ra_value_mem(v);

	size = sizeof(char *) * num;
	v->value.cp = ra_alloc_mem(size);
	if (v->value.cp)
	{
		for (l = 0; l < num; l++)
		{
			v->value.cp[l] = ra_alloc_mem(strlen(array[l]) + 1);
			memset(v->value.cp[l], '\0', strlen(array[l])+1);
			if (v->value.cp[l])
				memcpy(v->value.cp[l], array[l], strlen(array[l]) + 1);
		}
	}

	/* convert all strings to current used encoding scheme and store them */
	v->value_locale.cp = ra_alloc_mem(size);
	if (v->value_locale.cp)
	{
		size_t len;
		char *conv = NULL;

		for (l = 0; l < num; l++)
		{
			if (strlen(array[l]) > 0)
			{
				len = strlen(array[l]) * 2;
				conv = calloc(len, 1);
				utf8_to_local(array[l], conv, len);
				len = strlen(conv);
				v->value_locale.cp[l] = ra_alloc_mem(len + 1);
				memset(v->value_locale.cp[l], '\0', len+1);
				if (v->value_locale.cp[l])
					memcpy(v->value_locale.cp[l], conv, len + 1);
				free(conv);
			}
			else
			{
				v->value_locale.cp[l] = ra_alloc_mem(1);
				v->value_locale.cp[l][0] = '\0';
			}
		}
	}

	v->utype = RA_VALUE_TYPE_CHAR_ARRAY;
	v->num_values = num;
	v->value_is_valid = 1;
} /* ra_value_set_string_array_utf8() */


/**
 * \brief set void-pointer-array values in value-handle
 * \param <vh> value-handle
 * \param <array> array to set in ra_info
 * \param <num> number of array elements
 *
 * This function set the long array array with num elements in vh.
 */
LIBRAAPI void
ra_value_set_voidp_array(value_handle vh, const void **array, unsigned long num)
{
	struct ra_value *v = (struct ra_value *)vh;
	size_t size;

	if (!v || !array || !valid_value_handle(vh, __LINE__))
		return;

	free_ra_value_mem(v);

	size = sizeof(void *) * num;
	v->value.vpp = ra_alloc_mem(size);
	if (v->value.vpp)
	{
		memcpy(v->value.vpp, array, size);
		v->utype = RA_VALUE_TYPE_VOIDP_ARRAY;
		v->num_values = num;
	}
	v->value_is_valid = 1;
} /* ra_value_set_voidp_array() */


/**
 * \brief set value-handle-array values in value-handle
 * \param <vh> value-handle
 * \param <array> array to set in ra_value
 * \param <num> number of array elements
 *
 * This function set the value-handle array 'array' with num elements in vh.
 */
LIBRAAPI void
ra_value_set_vh_array(value_handle vh, const value_handle *array, unsigned long num)
{
	struct ra_value *v = (struct ra_value *)vh;
	unsigned long l;

	if (!v || !array || !valid_value_handle(vh, __LINE__))
		return;

	free_ra_value_mem(v);

	v->value.vhp = ra_alloc_mem(sizeof(value_handle) * num);
	if (v->value.vhp)
	{
		for (l = 0; l < num; l++)
		{
			v->value.vhp[l] = ra_value_malloc();
			ra_value_copy(v->value.vhp[l], array[l]);
		}
		v->utype = RA_VALUE_TYPE_VH_ARRAY;
		v->num_values = num;
	}
	v->value_is_valid = 1;
} /* ra_value_set_vh_array() */


/**
 * \brief return short value from value-handle
 * \param <vh> value-handle
 *
 * This function returns the short value set in vh.
 */
LIBRAAPI short
ra_value_get_short(value_handle vh)
{
	short ret = 0;
	struct ra_value *v = (struct ra_value *)vh;

	if (!v || !(v->value_is_valid) || !valid_value_handle(vh, __LINE__))
		return ret;

	switch (v->utype)
	{
	case RA_VALUE_TYPE_SHORT:
		ret = v->value.s;
		break;
	case RA_VALUE_TYPE_LONG:
		ret = (short)(v->value.l);
		break;
	case RA_VALUE_TYPE_DOUBLE:
		ret = (short)(v->value.d);
		break;
	}

	return ret;
} /* ra_value_get_short() */


/**
 * \brief return long value from value-handle
 * \param <vh> value-handle
 *
 * This function returns the long value set in vh.
 */
LIBRAAPI long
ra_value_get_long(value_handle vh)
{
	long ret = 0;
	struct ra_value *v = (struct ra_value *)vh;

	if (!v || !(v->value_is_valid) || !valid_value_handle(vh, __LINE__))
		return ret;

	switch (v->utype)
	{
	case RA_VALUE_TYPE_SHORT:
		ret = (long)(v->value.s);
		break;
	case RA_VALUE_TYPE_LONG:
		ret = v->value.l;
		break;
	case RA_VALUE_TYPE_DOUBLE:
		ret = (long)(v->value.d);
		break;
	}

	return ret;
} /* ra_value_get_long() */


/**
 * \brief return double value from value-handle
 * \param <vh> value-handle
 *
 * This function returns the double value set in vh.
 */
LIBRAAPI double
ra_value_get_double(value_handle vh)
{
	double ret = 0.0;
	struct ra_value *v = (struct ra_value *)vh;

	if (!v || !(v->value_is_valid) || !valid_value_handle(vh, __LINE__))
		return ret;

	switch (v->utype)
	{
	case RA_VALUE_TYPE_SHORT:
		ret = (double)(v->value.s);
		break;
	case RA_VALUE_TYPE_LONG:
		ret = (double)(v->value.l);
		break;
	case RA_VALUE_TYPE_DOUBLE:
		ret = v->value.d;
		break;
	}

	return ret;
} /* ra_value_get_double() */


/**
 * \brief return character pointer from value-handle
 * \param <vh> value-handle
 *
 * This function returns a character pointer to the string set in vh.
 * The pointer is valid until vh is used in another function or vh
 * is freed.
 */
LIBRAAPI const char *
ra_value_get_string(value_handle vh)
{
	const char *ret = NULL;
	struct ra_value *v = (struct ra_value *)vh;

	if (!v || !(v->value_is_valid) || !valid_value_handle(vh, __LINE__))
		return ret;

	switch (v->utype)
	{
	case RA_VALUE_TYPE_CHAR:
		ret = v->value_locale.c;
		break;
	}

	return ret;
} /* ra_value_get_string() */


/**
 * \brief return character pointer from value-handle (UTF-8 encoded)
 * \param <vh> value-handle
 *
 * This function returns a character pointer to the string set in vh.
 * The string is UTF-8 encoded. The pointer is valid until vh is used
 * in another function or vh is freed.
 */
LIBRAAPI const char *
ra_value_get_string_utf8(value_handle vh)
{
	const char *ret = NULL;
	struct ra_value *v = (struct ra_value *)vh;

	if (!v || !(v->value_is_valid) || !valid_value_handle(vh, __LINE__))
		return ret;

	switch (v->utype)
	{
	case RA_VALUE_TYPE_CHAR:
		ret = v->value.c;
		break;
	}

	return ret;
} /* ra_value_get_string_utf8() */


/**
 * \brief return void-pointer from value-handle
 * \param <vh> value-handle
 *
 * This function returns the void-pointer set in vh.
 */
LIBRAAPI const void *
ra_value_get_voidp(value_handle vh)
{
	void *ret = NULL;
	struct ra_value *v = (struct ra_value *)vh;

	if (!v || !(v->value_is_valid) || !valid_value_handle(vh, __LINE__))
		return ret;

	switch (v->utype)
	{
	case RA_VALUE_TYPE_VOIDP:
		ret = v->value.vp;
		break;
	}

	return ret;
} /* ra_value_get_voidp() */


/**
 * \brief return pointer to short array from value-handle
 * \param <vh> value-handle
 *
 * This function returns a pointer to the short array stored in vh.
 * The pointer is valid until vh is used in another function or vh
 * is freed. The number of elements of the array can be get by calling
 * ra_value_get_num_elem().
 */
LIBRAAPI const short *
ra_value_get_short_array(value_handle vh)
{
	const short *ret = NULL;
	struct ra_value *v = (struct ra_value *)vh;

	if (!v || !(v->value_is_valid) || !valid_value_handle(vh, __LINE__))
		return ret;

	switch (v->utype)
	{
	case RA_VALUE_TYPE_SHORT_ARRAY:
		ret = v->value.sp;
		break;
	}

	return ret;
} /* ra_value_get_short_array() */


/**
 * \brief return pointer to long array from value-handle
 * \param <vh> value-handle
 *
 * This function returns a pointer to the long array stored in vh.
 * The pointer is valid until vh is used in another function or vh
 * is freed. The number of elements of the array can be get by calling
 * ra_value_get_num_elem().
 */
LIBRAAPI const long *
ra_value_get_long_array(value_handle vh)
{
	const long *ret = NULL;
	struct ra_value *v = (struct ra_value *)vh;

	if (!v || !(v->value_is_valid) || !valid_value_handle(vh, __LINE__))
		return ret;

	switch (v->utype)
	{
	case RA_VALUE_TYPE_LONG_ARRAY:
		ret = v->value.lp;
		break;
	}

	return ret;
} /* ra_value_get_long_array() */


/**
 * \brief return pointer to double array from value-handle
 * \param <vh> value-handle
 *
 * This function returns a pointer to the double array stored in vh.
 * The pointer is valid until vh is used in another function or vh
 * is freed. The number of elements of the array can be get by calling
 * ra_value_get_num_elem().
 */
LIBRAAPI const double *
ra_value_get_double_array(value_handle vh)
{
	const double *ret = NULL;
	struct ra_value *v = (struct ra_value *)vh;

	if (!v || !(v->value_is_valid) || !valid_value_handle(vh, __LINE__))
		return ret;

	switch (v->utype)
	{
	case RA_VALUE_TYPE_DOUBLE_ARRAY:
		ret = v->value.dp;
		break;
	}

	return ret;
} /* ra_value_get_double_array() */


/**
 * \brief return pointer to character pointer array from value-handle
 * \param <vh> value-handle
 *
 * This function returns a pointer to the character pointer array stored in vh.
 * The pointer is valid until vh is used in another function or vh
 * is freed. The number of elements of the array can be get by calling
 * ra_value_get_num_elem().
 */
LIBRAAPI const char **
ra_value_get_string_array(value_handle vh)
{
	const char **ret = NULL;
	struct ra_value *v = (struct ra_value *)vh;

	if (!v || !(v->value_is_valid) || !valid_value_handle(vh, __LINE__))
		return ret;

	switch (v->utype)
	{
	case RA_VALUE_TYPE_CHAR_ARRAY:
		ret = (const char **)(v->value_locale.cp);
		break;
	}

	return ret;
} /* ra_value_get_string_array() */


/**
 * \brief return pointer to character pointer array from value-handle (UTF-8 encoded)
 * \param <vh> value-handle
 *
 * This function returns a pointer to the character pointer array stored in vh.
 * The strings are UTF-8 encoded. The pointer is valid until vh is used in another
 * function or vh is freed. The number of elements of the array can be get by calling
 * ra_value_get_num_elem().
 */
LIBRAAPI const char **
ra_value_get_string_array_utf8(value_handle vh)
{
	const char **ret = NULL;
	struct ra_value *v = (struct ra_value *)vh;

	if (!v || !(v->value_is_valid) || !valid_value_handle(vh, __LINE__))
		return ret;

	switch (v->utype)
	{
	case RA_VALUE_TYPE_CHAR_ARRAY:
		ret = (const char **)(v->value.cp);
		break;
	}

	return ret;
} /* ra_value_get_string_array_utf8() */


/**
 * \brief return pointer to void-pointer array from value-handle
 * \param <vh> value-handle
 *
 * This function returns a pointer to the void-pointer array stored in vh.
 * The pointer is valid until vh is used in another function or vh
 * is freed. The number of elements of the array can be get by calling
 * ra_value_get_num_elem().
 */
LIBRAAPI const void **
ra_value_get_voidp_array(value_handle vh)
{
	const void **ret = NULL;
	struct ra_value *v = (struct ra_value *)vh;

	if (!v || !(v->value_is_valid) || !valid_value_handle(vh, __LINE__))
		return ret;

	switch (v->utype)
	{
	case RA_VALUE_TYPE_VOIDP_ARRAY:
		ret = (const void **)(v->value.vpp);
		break;
	}

	return ret;
} /* ra_value_get_voidp_array() */


/**
 * \brief return pointer to long array from value-handle
 * \param <vh> value-handle
 *
 * This function returns a pointer to the value-handle array stored in vh.
 * The pointer is valid until vh is used in another function or vh
 * is freed. The number of elements of the array can be get by calling
 * ra_value_get_num_elem().
 */
LIBRAAPI const value_handle *
ra_value_get_vh_array(value_handle vh)
{
	const value_handle *ret = NULL;
	struct ra_value *v = (struct ra_value *)vh;

	if (!v || !(v->value_is_valid) || !valid_value_handle(vh, __LINE__))
		return ret;

	switch (v->utype)
	{
	case RA_VALUE_TYPE_VH_ARRAY:
		ret = v->value.vhp;
		break;
	}

	return ret;
} /* ra_value_get_vh_array() */


/**
 * \brief copy one value_handle to another value_handle
 * \param <dest> destination value-handle
 * \param <src> source value-handle
 *
 * This function copies the values from one value-handle to another value-handle
 * and the name and description. The remaining fields are not copied.
 */
LIBRAAPI int
ra_value_copy(value_handle dest, value_handle src)
{
	struct ra_value *d = (struct ra_value *)dest;
	struct ra_value *s = (struct ra_value *)src;
	int ret = 0;

	if (!d || !s || !valid_value_handle(d, __LINE__) || !valid_value_handle(s, __LINE__))
		return -1;

	free_ra_value_mem(d);

	strncpy(d->name, s->name, RA_VALUE_NAME_MAX);
	strncpy(d->desc, s->desc, RA_VALUE_DESC_MAX);
	d->id = s->id;
	d->can_be_changed = 1;
	
	if (!s->value_is_valid)
		return -1;

	switch (s->utype)
	{
	case RA_VALUE_TYPE_SHORT:
		ra_value_set_short(dest, s->value.s);
		break;
	case RA_VALUE_TYPE_LONG:
		ra_value_set_long(dest, s->value.l);
		break;
	case RA_VALUE_TYPE_DOUBLE:
		ra_value_set_double(dest, s->value.d);
		break;
	case RA_VALUE_TYPE_CHAR:
		ra_value_set_string(dest, s->value.c);
		break;
	case RA_VALUE_TYPE_VOIDP:
		ra_value_set_voidp(dest, s->value.vp);
		break;
	case RA_VALUE_TYPE_SHORT_ARRAY:
		ra_value_set_short_array(dest, s->value.sp, s->num_values);
		break;
	case RA_VALUE_TYPE_LONG_ARRAY:
		ra_value_set_long_array(dest, s->value.lp, s->num_values);
		break;
	case RA_VALUE_TYPE_DOUBLE_ARRAY:
		ra_value_set_double_array(dest, s->value.dp, s->num_values);
		break;
	case RA_VALUE_TYPE_CHAR_ARRAY:
		ra_value_set_string_array(dest, (const char **)(s->value.cp), s->num_values);
		break;
	case RA_VALUE_TYPE_VOIDP_ARRAY:
		ra_value_set_voidp_array(dest, (const void **)(s->value.vpp), s->num_values);
		break;
	case RA_VALUE_TYPE_VH_ARRAY:
		ra_value_set_vh_array(dest, s->value.vhp, s->num_values);
		break;
	default:
		ret = -1;
		break;
	}

	return ret;
} /* ra_value_copy() */


/**
 * \brief copy one element from a value-handle in another value-handle
 * \param <dest> destination value-handle
 * \param <src> source value-handle
 * \param <elem_num> zero-based index of the value which has to be copied
 *
 * This function copies one element from value-handle to another value-handle
 * and the name and description. The remaining fields are not copied. The
 * function is helpfull when a single value from an array, stored in a
 * value-handle, is needed for another function expecting a single value in a
 * value-handle.
 */
LIBRAAPI int
ra_value_get_single_elem(value_handle dest, value_handle src, size_t elem_num)
{
	struct ra_value *d = (struct ra_value *)dest;
	struct ra_value *s = (struct ra_value *)src;
	int ret = 0;

	if (!d || !s || !valid_value_handle(d, __LINE__) || !valid_value_handle(s, __LINE__) || (elem_num < 0))
		return -1;

	if (elem_num >= s->num_values)
		return -1;

	if (!s->value_is_valid)
		return -1;

	free_ra_value_mem(d);

	strncpy(d->name, s->name, RA_VALUE_NAME_MAX);
	strncpy(d->desc, s->desc, RA_VALUE_DESC_MAX);
	d->id = s->id;
	d->can_be_changed = 1;
	
	switch (s->utype)
	{
	case RA_VALUE_TYPE_SHORT:
		ra_value_set_short(dest, s->value.s);
		break;
	case RA_VALUE_TYPE_LONG:
		ra_value_set_long(dest, s->value.l);
		break;
	case RA_VALUE_TYPE_DOUBLE:
		ra_value_set_double(dest, s->value.d);
		break;
	case RA_VALUE_TYPE_CHAR:
		ra_value_set_string(dest, s->value.c);
		break;
	case RA_VALUE_TYPE_VOIDP:
		ra_value_set_voidp(dest, s->value.vp);
		break;
	case RA_VALUE_TYPE_SHORT_ARRAY:
		ra_value_set_short(dest, s->value.sp[elem_num]);
		break;
	case RA_VALUE_TYPE_LONG_ARRAY:
		ra_value_set_long(dest, s->value.lp[elem_num]);
		break;
	case RA_VALUE_TYPE_DOUBLE_ARRAY:
		ra_value_set_double(dest, s->value.dp[elem_num]);
		break;
	case RA_VALUE_TYPE_CHAR_ARRAY:
		ra_value_set_string(dest, s->value.cp[elem_num]);
		break;
	case RA_VALUE_TYPE_VOIDP_ARRAY:
		ra_value_set_voidp(dest, s->value.vpp[elem_num]);
		break;
	case RA_VALUE_TYPE_VH_ARRAY:
		ret = ra_value_copy(dest, s->value.vhp[elem_num]);
		break;
	default:
		ret = -1;
		break;
	}

	return ret;
} /* ra_value_get_single_elem() */


/**
 * \brief frees value-handle memory
 * \param <v> pointer to a value-handle struct
 *
 * Function frees memory allocated for a value-handle.
 */
void
free_ra_value_mem(struct ra_value *v)
{
	switch (v->utype)
	{
	case RA_VALUE_TYPE_CHAR:
		if (v->value.c)
			ra_free_mem(v->value.c);
		v->value.c = NULL;
		if (v->value_locale.c)
			ra_free_mem(v->value_locale.c);
		v->value_locale.c = NULL;
		break;
	case RA_VALUE_TYPE_SHORT_ARRAY:
		if (v->value.sp)
			ra_free_mem(v->value.sp);
		v->value.sp = NULL;
		break;
	case RA_VALUE_TYPE_LONG_ARRAY:
		if (v->value.lp)
			ra_free_mem(v->value.lp);
		v->value.lp = NULL;
		break;
	case RA_VALUE_TYPE_DOUBLE_ARRAY:
		if (v->value.dp)
			ra_free_mem(v->value.dp);
		v->value.dp = NULL;
		break;
	case RA_VALUE_TYPE_CHAR_ARRAY:
		if (v->value.cp)
		{
			size_t l;
			for (l = 0; l < v->num_values; l++)
			{
				if (v->value.cp[l])
					ra_free_mem(v->value.cp[l]);
			}
			ra_free_mem(v->value.cp);
		}
		v->value.cp = NULL;
		if (v->value_locale.cp)
		{
			size_t l;
			for (l = 0; l < v->num_values; l++)
			{
				if (v->value_locale.cp[l])
					ra_free_mem(v->value_locale.cp[l]);
			}
			ra_free_mem(v->value_locale.cp);
		}
		v->value_locale.cp = NULL;
		break;
	case RA_VALUE_TYPE_VOIDP_ARRAY:
		if (v->value.vpp)
			ra_free_mem(v->value.vpp);
		v->value.vpp = NULL;
		break;
	case RA_VALUE_TYPE_VH_ARRAY:
		if (v->value.vhp)
		{
			size_t l;
			for (l = 0; l < v->num_values; l++)
				ra_value_free(v->value.vhp[l]);
			ra_free_mem(v->value.vhp);
		}
		v->value.vhp = NULL;
		break;
	}

	v->value_is_valid = 0;
	v->num_values = 0;
	v->utype = RA_VALUE_TYPE_NONE;
} /* free_ra_value_mem() */


