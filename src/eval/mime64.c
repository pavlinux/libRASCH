/**
 * \file mime64.c
 *
 * This file implements mime-base64 encoding/decoding.
 *
 * \author Raphael Schneider (ra@med1.med.tum.de)
 */

/*----------------------------------------------------------------------------
 *
 * Copyright (C) 2002, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Header: /home/cvs_repos/librasch/src/eval/mime64.c,v 1.5 2004/07/13 20:16:26 rasch Exp $
 *
 *--------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>


unsigned char base64_alph[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";


/* to be sure of hex-values

0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F
0000  0001  0010  0011  0100  0101  0110  0111  1000  1001  1010  1011  1100  1101  1110  1111

*/

/**
 * \brief encode a string using MIME64
 * \param <in> input stream
 * \param <isize> size of input stream
 * \param <out> receive output stream
 * \param <osize> receive size of output stream
 *
 * The function encodes the input stream 'in' using MIME-base64.
 */
int
encode_base64(unsigned char *in, size_t isize, unsigned char **out, size_t *osize)
{
	size_t l, o_pos = 0, len;
	unsigned char t;
	size_t line_size = 0;

	len = (size_t) ((double) isize * 1.33) + 4;
	len += ((len / 76) * 2) + 2;
	*out = malloc(len);
	if (*out == NULL)
		return -1;

	for (l = 0; l < isize; l += 3)
	{
		if (l < (isize - 2))
		{
			t = (in[l] & 0xfc) >> 2;
			(*out)[o_pos++] = base64_alph[t];
			t = ((in[l] & 0x03) << 4) | ((in[l + 1] & 0xf0) >> 4);
			(*out)[o_pos++] = base64_alph[t];
			t = ((in[l + 1] & 0x0f) << 2) | ((in[l + 2] & 0xc0) >> 6);
			(*out)[o_pos++] = base64_alph[t];
			t = in[l + 2] & 0x3f;
			(*out)[o_pos++] = base64_alph[t];
		}
		else if (l < (isize - 1))
		{
			t = (in[l] & 0xfc) >> 2;
			(*out)[o_pos++] = base64_alph[t];
			t = ((in[l] & 0x03) << 4) | ((in[l + 1] & 0xf0) >> 4);
			(*out)[o_pos++] = base64_alph[t];
			t = (in[l + 1] & 0x0f) << 2;
			(*out)[o_pos++] = base64_alph[t];
			(*out)[o_pos++] = '=';
		}
		else
		{
			t = (in[l] & 0xfc) >> 2;
			(*out)[o_pos++] = base64_alph[t];
			t = (in[l] & 0x03) << 4;
			(*out)[o_pos++] = base64_alph[t];
			(*out)[o_pos++] = '=';
			(*out)[o_pos++] = '=';
		}
		line_size += 4;

		if (line_size >= 76)
		{
			(*out)[o_pos++] = '\n';	/* libxml2 has problems with CRLF */
			line_size = 0;
		}
	}
	(*out)[o_pos] = '\0';	/* being save if osize will be ignored */
	*osize = o_pos;
	*out = realloc(*out, *osize + 1);

	return 0;
} /* encode_base64() */


/**
 * \brief decode a string using MIME64
 * \param <in> input stream
 * \param <isize> size of input stream
 * \param <out> receive output stream
 * \param <osize> receive size of output stream
 * \param <max_size> maximum size of output stream
 *
 * The function decodes the MIME64 encoded input stream 'in'. If 'max_size' is not -1
 * than this will be the maximal number of values decoded.
 */
int
decode_base64(unsigned char *in, size_t isize, unsigned char **out, size_t *osize, size_t max_size)
{
	size_t l;
	size_t o_pos = 0;

	unsigned char decode_alph[255];

	for (l = 0; l < 64; l++)
		decode_alph[base64_alph[l]] = (unsigned char)l;

	*out = malloc(isize);

	l = 0;
	for (;;)
	{
		if ((max_size >= 0) && (o_pos >= max_size))
			break;  /* we have all needed data */

		if ((l + 3) >= isize)
			break;

		if ((in[l] == 0x0A) || (in[l] == 0x0D) || (in[l] == ' '))
		{
			l++;
			continue;
		}

		(*out)[o_pos++] =
			(decode_alph[in[l]] << 2) | ((decode_alph[in[l + 1]] & 0xf0) >> 4);
		if (in[l + 2] != '=')
			(*out)[o_pos++] =
				((decode_alph[in[l + 1]] & 0x0f) << 4) |
				((decode_alph[in[l + 2]] & 0xfc) >> 2);

		if (in[l + 3] != '=')
			(*out)[o_pos++] =
				((decode_alph[in[l + 2]] & 0x03) << 6) | decode_alph[in[l + 3]];

		l += 4;
	}

	*osize = o_pos;
	*out = realloc(*out, *osize);

	return 0;
} /* decode_base64() */
