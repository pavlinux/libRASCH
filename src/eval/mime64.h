/*----------------------------------------------------------------------------
 * mime64.h
 *
 * Header-file for mim-base64 encoding/decoding.
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Header: /home/cvs_repos/librasch/src/eval/mime64.h,v 1.3 2004/07/13 20:16:26 rasch Exp $
 *--------------------------------------------------------------------------*/

#ifndef MIME64_H
#define MIME64_H


int encode_base64 (const void *in, size_t isize, char **out, size_t * osize);
int decode_base64 (const unsigned char *in, size_t isize, void **out,
		   size_t * osize, size_t max_size);


#endif /* MIME64_H */
