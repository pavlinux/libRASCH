/**
 * \file i18n.c
 *
 * This file provides functions for internationalization (i18n) needed in libRASCH
 *
 * \author Raphael Schneider (rasch@med1.med.tum.de)
 */

/*----------------------------------------------------------------------------
 *
 * Copyright (C) 2004, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Header: /home/cvs_repos.bak/librasch/src/shared/i18n.c,v 1.4 2004/07/27 08:49:35 rasch Exp $
 *
 *--------------------------------------------------------------------------*/


#include <string.h>

#define _LIBRASCH_BUILD
#include <ra_priv.h>
#include <locale.h>
#include <iconv.h>


/* prototypes needed only here */
int do_i18n_conversion_inplace(char *string, size_t dest_size, const char *from, const char *to,
			       int force);
int do_i18n_conversion(const char *src, char *dest, size_t dest_size, const char *from, const char *to,
		       int force);
int local_is_utf8();


int
local_to_utf8(const char *src, char *dest, size_t dest_size)
{
	return do_i18n_conversion(src, dest, dest_size, ra_i18n_codeset, "UTF-8", 0);
} /* local_to_utf8() */


int
local_to_utf8_inplace(char *string, size_t buf_size)
{
	return do_i18n_conversion_inplace(string, buf_size, ra_i18n_codeset, "UTF-8", 0);
} /* local_to_utf8_inplace() */


int
to_utf8(const char *src, char *dest, size_t dest_size, const char *src_encoding)
{
	return do_i18n_conversion(src, dest, dest_size, src_encoding, "UTF-8", 1);
} /* to_utf8() */


int
to_utf8_inplace(char *string, size_t buf_size, const char *src_encoding)
{
	return do_i18n_conversion_inplace(string, buf_size, src_encoding, "UTF-8", 1);
} /* to_utf8_inplace() */


int
utf8_to_local(const char *src, char *dest, size_t dest_size)
{
	return do_i18n_conversion(src, dest, dest_size, "UTF-8", ra_i18n_codeset, 0);
} /* utf8_to_local() */


int
utf8_to_local_inplace(char *string, size_t buf_size)
{
	return do_i18n_conversion_inplace(string, buf_size, "UTF-8", ra_i18n_codeset, 0);
} /* local_to_utf8_inplace() */


int
from_utf8(const char *src, char *dest, size_t dest_size, const char *dest_encoding)
{
	return do_i18n_conversion(src, dest, dest_size, "UTF-8", dest_encoding, 1);
} /* from_utf8() */


int
from_utf8_inplace(char *string, size_t buf_size, const char *dest_encoding)
{
	return do_i18n_conversion_inplace(string, buf_size, "UTF-8", dest_encoding, 1);
} /* from_utf8_inplace() */


int
do_i18n_conversion_inplace(char *string, size_t dest_size, const char *from, const char *to,
			   int force)
{
	char *conv = NULL;
	size_t len;
	int ret;

	if (string == NULL)
		return -1;
	if (string[0] == '\0')
		return 0;

	len = strlen(string) * 2;
	conv = malloc(len);
	ret = do_i18n_conversion(string, conv, len, from, to, force);
	strncpy(string, conv, dest_size);
	free(conv);

	return ret;
} /* do_i18n_conversion_inplace() */


int
do_i18n_conversion(const char *src, char *dest, size_t dest_size, const char *from, const char *to,
		   int force)
{
	int ret = 0;
	iconv_t cd;
	size_t in_bytes, out_bytes;
	char *in, *out;

	if ((src == NULL) || (dest == NULL) || (dest_size <= 0))
		return -1;

	if (src[0] == '\0')
	{
		dest[0] = '\0';
		return 0;
	}

	/* if no i18n support or locale is UTF-8 then do no conversion */
	if ((force == 0) && ((ra_i18n_codeset[0] == '\0') || local_is_utf8()))
	{
		strncpy(dest, src, dest_size);
		return -0;
	}

	cd = iconv_open(to, from);
	/* if iconv can not be opened, just copy string but return error */
	if (cd == (iconv_t)-1)
	{
		strncpy(dest, src, dest_size);
		return -1;
	}

	/* convert string, do no special error handling */
	in_bytes = strlen(src);
	out_bytes = dest_size;
	in = (char *)src;
	out = dest;
	if (iconv(cd, &in, &in_bytes, &out, &out_bytes) == (size_t)-1)
		ret = -2;
	
	dest[dest_size-out_bytes] = '\0';

	iconv_close(cd);

	return ret;
}  /* do_i18n_conversion() */


int
local_is_utf8()
{
	if (strcmp(ra_i18n_codeset, "UTF-8") == 0)
		return 1;

	return 0;
}  /* local_is_utf8() */
