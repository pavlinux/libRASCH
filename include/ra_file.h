/**
 * \file ra_file.h
 *
 * The header file contains prototypes for libRASCH file functions. The
 * functions should be used in libRASCH to provide an OS independend
 * file access. 
 *
 * \author Raphael Schneider (rasch@med1.med.tum.de)
 */

/*----------------------------------------------------------------------------
 *
 * Copyright (C) 2002-2006, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *
 *--------------------------------------------------------------------------*/

#ifndef RA_FILE_H
#define RA_FILE_H

#include <stdio.h>		/* for FILE */

#ifdef __cplusplus
extern "C" {
#endif


int find_meas(const char *dir, const char *mask, const char *plugin_name, struct find_meas *f, int (*check_meas) (const char *));
int find_files(const char *dir, const char *mask, char ***f, int *num, long *size);

int ra_get_filename(const char *fn, int dir_based, char *buf, int buf_len);
FILE *ra_fopen(const char *fn, const char *mode, int dir_based);

int make_dir(const char *dir);
int copy_move_file(const char *file, const char *dest_dir, int copy);
int copy_file(const char *src, const char *dest);
int move_file(const char *src, const char *dest);
int delete_file(const char *file);

#ifdef __cplusplus
}
#endif

#endif /* RA_FILE_H */
