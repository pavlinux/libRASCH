/**
 * \file ra_file.c
 *
 * This file provides support for handling files in libRASCH.
 *
 * \author Raphael Schneider (rasch@med1.med.tum.de)
 */

/*----------------------------------------------------------------------------
 *
 * Copyright (C) 2002-2007, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *
 *--------------------------------------------------------------------------*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef WIN32
#include <windows.h>
#endif

#ifdef LINUX
#include <dirent.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#endif

#define _LIBRASCH_BUILD
#include <ra.h>
#include <ra_priv.h>
#include <ra_file.h>
#include <pl_general.h>

/* private prototypes */
int find_files_win32 (const char *dir, const char *mask, char ***f, int *num,
		      long *size);
int find_files_linux (const char *dir, const char *mask, char ***f, int *num,
		      long *size);
FILE *_ra_fopen (const char *fn, const char *mode, int dir_based, char *fn_ok,
		 int buf_len);


/**
 * \brief return "case-corrected" filename
 * \param <fn> input filename
 * \param <buf> buffer for "case-corrected" filename
 * \param <buf_len> length of buffer 'buf'
 *
 * The function tries to find how the given filename is used on the current
 * system (case wise) and returns the filename case corrected.
 */
int
ra_get_filename (const char *fn, int dir_based, char *buf, int buf_len)
{
  int ret = 0;
  FILE *fp;
  char *fn_ok;

  fn_ok = malloc (sizeof (char) * MAX_PATH_RA);
  fp = _ra_fopen (fn, "rb", dir_based, fn_ok, MAX_PATH_RA);
  if (fp)
    {
      strncpy (buf, fn_ok, buf_len);
      fclose (fp);
    }
  else
    ret = -1;

  free (fn_ok);

  return ret;
}				/* ra_get_filename() */


/**
 * \brief opens a file
 * \param <fn> filename
 * \param <mode> the fopen file-open mode characters
 * \param <dir_based> flag if measurement-files are stored in a directory (=1) or not (!=1)
 *
 * The function tries different case combinations to open the file if
 * the given filename can not be found.
 */
FILE *
ra_fopen (const char *fn, const char *mode, int dir_based)
{
  return _ra_fopen (fn, mode, dir_based, NULL, 0);
}				/* ra_fopen() */

/**
 * \brief implementation for Linux version of ra_fopen()
 * \param <fn> filename
 * \param <mode> the fopen file-open mode characters
 * \param <dir_based> flag if measurement-files are stored in a directory (=1) or not (=0)
 * \param <fn_ok> "case-corrected" filename
 * \param <buf_len> length of buffer 'fn_ok'
 *
 * Explanation see ra_fopen() and ra_get_filename().
 */
FILE *
_ra_fopen (const char *fn, const char *mode, int dir_based, char *fn_ok,
	   int buf_len)
{
  FILE *fp = NULL;
  char *pos, *pos_save;
  int is_lower = -1;
  char *pos_sep = NULL;
  char fn_locale[MAX_PATH_RA];

  /* IIUIC toupper() and tolower() respect locale setting (at least on Linux),
     therfore do everything using the "locale"-string */
  utf8_to_local (fn, fn_locale, MAX_PATH_RA);

  /* i18n: do not care about the mode string (I think it will be always ASCII char's) */

  fp = fopen (fn_locale, mode);
  if (fp)
    goto found;

  /* fopen failed, perhaps the notation (upper-/lower-case) of the filename is different */
  /* when NOT dir-based check if a reverse of the character case helps */
  if (!dir_based)
    {
      pos = strrchr (fn_locale, SEPARATOR);
      if (pos == NULL)
	pos = fn_locale;

      pos_save = pos;
      while (*pos)
	{
	  if (islower (*pos))
	    *pos = (char) toupper (*pos);
	  else
	    *pos = (char) tolower (*pos);
	  pos++;
	}
      fp = fopen (fn_locale, mode);
      if (fp)
	goto found;

      /* not found, undo changes */
      pos = pos_save;
      while (*pos)
	{
	  if (islower (*pos))
	    *pos = (char) toupper (*pos);
	  else
	    *pos = (char) tolower (*pos);
	  pos++;
	}
    }

  /* fopen failed, perhaps the file-extension has some character case differences */
  pos = strrchr (fn_locale, '.');

  /* if measurement-files are in a directory, search for extension of directory */
  if (dir_based && pos)
    {
      pos_sep = strrchr (fn_locale, SEPARATOR);
      if (pos_sep && (pos > pos_sep))
	{
	  *pos_sep = '\0';
	  pos = strrchr (fn_locale, '.');
	  *pos_sep = SEPARATOR;
	}
    }

  if (pos == NULL)
    return NULL;

  /* reverse character case (for dir-based meas. reverse char-case for file also) */
  pos++;			/* '.' don't need to be handled */
  pos_save = pos;
  while (*pos)
    {
      if (is_lower == -1)
	is_lower = islower (*pos);

      if (islower (*pos))
	*pos = (char) toupper (*pos);
      else
	*pos = (char) tolower (*pos);
      pos++;
    }

  /* and now try again to open file */
  fp = fopen (fn_locale, mode);
  if (!fp && dir_based && pos_sep)
    {
      /* change char-case of extension back but leave the file-name reversed */
      pos = pos_save;
      while (*pos && (pos != pos_sep))
	{
	  if (is_lower)		/* was reversed before */
	    *pos = (char) tolower (*pos);
	  else
	    *pos = (char) toupper (*pos);
	  pos++;
	}
      fp = fopen (fn_locale, mode);
      if (!fp)
	{
	  /* if still not worked, try 4th possibility */
	  pos = pos_save;
	  while (*pos)
	    {
	      if (islower (*pos))
		*pos = (char) toupper (*pos);
	      else
		*pos = (char) tolower (*pos);
	      pos++;
	    }
	  fp = fopen (fn_locale, mode);
	}
    }

found:
  if ((buf_len > 0) && (fn_ok != NULL) && fp)
    strncpy (fn_ok, fn_locale, buf_len);

  return fp;
}				/* _ra_fopen() */


/**
 * \brief search in a directory for specific file format
 * \param <dir> directory to look in
 * \param <mask> file-mask used to select specific files
 * \param <plugin_name> name of the access plugin handling the file
 * \param <f> pointer to a find-meas struct receiving the found measurements
 * \param <check_meas> pointer to the function checking if the found file's are supported measurements
 *
 * The function search in a directory for measurements which are handled by the calling
 * plugin. The function calls 'check_meas' (provided by the plugin) to check if the found
 * file's are supported measurements.
 */
int
find_meas (const char *dir, const char *mask, const char *plugin_name,
	   struct find_meas *f, int (*check_meas) (const char *))
{
  int n, i;
  char **files;

  n = 0;
  files = NULL;

  if (find_files (dir, mask, &files, &n, NULL) != 0)
    return -1;

#ifdef WIN32
  if (f->names == NULL)
    {
      f->names = GlobalAlloc (GMEM_FIXED, sizeof (char *) * (f->num + n));
      f->plugins = GlobalAlloc (GMEM_FIXED, sizeof (char *) * (f->num + n));
    }
  else
    {
      char **t = GlobalAlloc (GMEM_FIXED, sizeof (char *) * (f->num + n));
      memcpy (t, f->names, sizeof (char *) * f->num);
      GlobalFree (f->names);
      f->names = t;

      t = GlobalAlloc (GMEM_FIXED, sizeof (char *) * (f->num + n));
      memcpy (t, f->plugins, sizeof (char *) * f->num);
      GlobalFree (f->plugins);
      f->plugins = t;
    }
#else
  f->names = realloc (f->names, sizeof (char *) * (f->num + n));
  f->plugins = realloc (f->plugins, sizeof (char *) * (f->num + n));
#endif
  for (i = 0; i < n; i++)
    {
      if ((*check_meas) (files[i]))
	{
	  f->names[f->num] =
	    ra_alloc_mem (sizeof (char) * strlen (files[i]) + 1);
	  strcpy (f->names[f->num], files[i]);

	  f->plugins[f->num] =
	    ra_alloc_mem (sizeof (char) * strlen (plugin_name) + 1);
	  strcpy (f->plugins[f->num], plugin_name);

	  f->num++;
	}

      ra_free_mem (files[i]);
    }
  ra_free_mem (files);

  return 0;
}				/* find_meas() */


/**
 * \brief file-search function for libRASCH plugins (OS independent)
 * \param <dir> directory in which files are searched for
 * \param <mask> file-mask
 * \param <f> pointer to the buffer receiving the found file-names
 * \param <num> pointer to the buffer-size
 * \param <size> receive the sum of the size for the found files
 *
 * The function search in a directory for files using the given file-mask. The function
 * provides a OS independent file-search.
 */
int
find_files (const char *dir, const char *mask, char ***f, int *num,
	    long *size)
{
  int ret = -1;
  long s = 0;

#ifdef WIN32
  ret = find_files_win32 (dir, mask, f, num, &s);
#endif

#ifdef LINUX
  ret = find_files_linux (dir, mask, f, num, &s);
#endif

  if (size)
    *size = s;

  return ret;
}				/* find_files() */


/**
 * \brief Win32 implementation for find_meas()
 * \param <dir> directory in which files are searched for
 * \param <mask> file-mask which will be used for searching
 * \param <f> pointer to the buffer receiving the found file-names
 * \param <num> pointer to the buffer-size
 * \param <size> receive the sum of the size for the found files
 *
 * Explanation see file_search() in this file.
 */
#ifdef WIN32
int
find_files_win32 (const char *dir, const char *mask, char ***f, int *num,
		  long *size)
{
  char fm[MAX_PATH_RA];
  WIN32_FIND_DATA find;
  HANDLE h;
  char dir_locale[MAX_PATH_RA];
  char **f_temp = NULL;
  long l, num_new = 0;

  if (dir == NULL)
    sprintf (fm, "%s", mask);
  else
    {
      sprintf (fm, "%s\\%s", dir, mask);
      /* will be needed later */
      utf8_to_local (dir, dir_locale, MAX_PATH_RA);
    }
  utf8_to_local_inplace (fm, MAX_PATH_RA);

  h = FindFirstFile (fm, &find);
  if (h == INVALID_HANDLE_VALUE)
    return -1;

  *size = 0;
  do
    {
      char t[MAX_PATH_RA];

      if ((strcmp (find.cFileName, ".") == 0)
	  || (strcmp (find.cFileName, "..") == 0))
	continue;

      num_new++;
      f_temp = realloc (f_temp, sizeof (char *) * num_new);
      if (dir)
	sprintf (t, "%s%c%s", dir_locale, SEPARATOR, find.cFileName);
      else
	sprintf (t, "%s", find.cFileName);

      local_to_utf8_inplace (t, MAX_PATH_RA);

      f_temp[num_new - 1] = malloc (strlen (t) + 1);
      strcpy (f_temp[num_new - 1], t);

      *size += find.nFileSizeLow;	/* TODO: think about how to handle .nFileSizeHigh */
    }
  while (FindNextFile (h, &find));
  FindClose (h);

  if (num_new > 0)
    {
      char **f_new;

      f_new = ra_alloc_mem (sizeof (char *) * (*num + num_new));

      /* if there are already some files in 'f', copy these first */
      if (*num > 0)
	{
	  for (l = 0; l < *num; l++)
	    {
	      f_new[l] =
		ra_alloc_mem (sizeof (char) * (strlen ((*f)[l]) + 1));
	      strcpy (f_new[l], (*f)[l]);
	      ra_free_mem ((*f)[l]);
	    }
	  ra_free_mem (*f);
	}

      /* now copy the new found files */
      for (l = *num; l < (*num + num_new); l++)
	{
	  f_new[l] =
	    ra_alloc_mem (sizeof (char) * (strlen (f_temp[l - (*num)]) + 1));
	  strcpy (f_new[l], f_temp[l - (*num)]);
	  free (f_temp[l - (*num)]);
	}
      free (f_temp);
      *f = f_new;
      *num += num_new;
    }

  return 0;
}				/* find_files_win32() */

#endif /* WIN32 */

/**
 * \brief Linux implementation for find_meas()
 * \param <dir> directory in which files are searched for
 * \param <mask> file-mask which will be used for searching
 * \param <f> pointer to the buffer receiving the found file-names
 * \param <num> pointer to the buffer-size
 * \param <size> receive the sum of the size for the found files
 *
 * Explanation see file_search() in this file.
 */
#ifdef LINUX
int
find_files_linux (const char *dir, const char *mask, char ***f, int *num,
		  long *size)
{
  char dir_locale[MAX_PATH_RA];
  DIR *d;
  struct dirent *de;
  struct stat s;
  int mlen, mlen_use;
  char mask_use[MAX_PATH_RA];

  /* handle wildchar's (i.e. delete them here but chek for them later, see below)  */
  mlen = strlen (mask);
  if (mlen > 0)
    {
      memset (mask_use, 0, MAX_PATH_RA);
      if (mask[0] == '*')
	strcpy (mask_use, mask + 1);
      else if (mask[mlen - 1] == '*')
	strncpy (mask_use, mask, mlen - 1);
    }
  utf8_to_local_inplace (mask_use, MAX_PATH_RA);
  mlen_use = strlen (mask_use);

  memset (dir_locale, 0, MAX_PATH_RA);
  utf8_to_local (dir, dir_locale, MAX_PATH_RA);

  d = opendir (dir_locale);
  if (d == NULL)
    return 0;			/* TODO: think if an error (dir not exist) should be set */

  while ((de = readdir (d)))
    {
      char t[MAX_PATH_RA];
      char name_utf8[MAX_PATH_RA];
      int len = strlen (de->d_name);

      /* skip current and parent directory */
      if ((strcmp (de->d_name, ".") == 0) || (strcmp (de->d_name, "..") == 0))
	continue;

      /* if no mask or mask == '*' -> get all files in directory; if not check
         if name match with mask */
      if ((mlen > 0) && !((mask[0] == '*') && (mlen == 1)))
	{
	  /* if the current filename is shorter than mask, try the next one */
	  if (len < (mlen - 1))
	    continue;

	  /* up to now only handle '*' as wildcard character (and only at the beginning
	     or at the end of the mask) */
	  memset (t, 0, MAX_PATH_RA);
	  if (mask[0] == '*')
	    strcpy (t, &(de->d_name[len - (mlen - 1)]));
	  else if (mask[mlen - 1] == '*')
	    strncpy (t, de->d_name, mlen - 1);
	  /* use case-insensitive compare to allow extensions with different case */
	  if (RA_STRICMP (t, mask_use) != 0)
	    continue;
	}

      (*num)++;
      *f = ra_realloc_mem (*f, sizeof (char *) * (*num));

      local_to_utf8 (de->d_name, name_utf8, MAX_PATH_RA);
      sprintf (t, "%s%c%s", dir, SEPARATOR, name_utf8);

      (*f)[(*num) - 1] = ra_alloc_mem (strlen (t) + 1);
      strcpy ((*f)[(*num) - 1], t);
      stat (t, &s);
      *size += s.st_size;
    }
  closedir (d);

  return 0;
}				/* find_files_linux() */


#endif /* LINUX */


/**
 * \brief create a directory (OS independent)
 * \param <dir> directory which should be created
 *
 * Function creates the given directory.
 */
int
make_dir (const char *dir)
{
  int ret = 0;
  char dir_locale[MAX_PATH_RA];

  utf8_to_local (dir, dir_locale, MAX_PATH_RA);

#ifdef WIN32
  if (CreateDirectory (dir_locale, NULL) == FALSE)
    ret = -1;
#endif /* WIN32 */

#ifdef LINUX
  ret = mkdir (dir_locale, ~0);
  if (ret != 0)
    ret = errno;
#endif /* LINUX */

  return ret;
}				/* make_dir() */


/**
 * \brief copy or move a file (OS independent)
 * \param <file> source file
 * \param <dest_dir> destination directory
 * \param <copy> flag if file should be copied (=1) or moved (=0)
 *
 * Function copy or move a file in the destination directory.
 */
int
copy_move_file (const char *file, const char *dest_dir, int copy)
{
  int ret = 0;
  char new_file[MAX_PATH_RA];
  char *p;
  char file_locale[MAX_PATH_RA];
  char new_file_locale[MAX_PATH_RA];

  p = strrchr (file, SEPARATOR);
  if (p == NULL)
    sprintf (new_file, "%s%c%s", dest_dir, SEPARATOR, file);
  else
    sprintf (new_file, "%s%c%s", dest_dir, SEPARATOR, p + 1);

  utf8_to_local (file, file_locale, MAX_PATH_RA);
  utf8_to_local (new_file, new_file_locale, MAX_PATH_RA);

#ifdef WIN32
  if (copy)
    {
      /* overwrite file if new_file exists */
      if (CopyFile (file_locale, new_file_locale, 0) == FALSE)
	ret = -1;
    }
  else
    {
      if (MoveFileEx
	  (file_locale, new_file_locale,
	   MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED) == FALSE)
	ret = -1;
    }
#endif /* WIN32 */

#ifdef LINUX
  if (copy)
    {
      /* it seems that it exist no copy function in libc */
      FILE *fp_in, *fp_out;
      unsigned char buf[1024];
      size_t len;

      fp_in = fopen (file_locale, "rb");
      fp_out = fopen (new_file_locale, "wb");
      if (!fp_in || !fp_out)
	return -1;

      while ((len = fread (buf, 1, 1024, fp_in)))
	fwrite (buf, 1, len, fp_out);
      fclose (fp_in);
      fclose (fp_out);
    }
  else
    {
      ret = rename (file_locale, new_file_locale);
      if ((ret != 0) && (errno == EXDEV))
	{
	  ret = copy_move_file (file, dest_dir, 1);
	  if (ret == 0)
	    ret = delete_file (file);
	}
    }

  if (ret != 0)
    ret = errno;
#endif /* LINUX */

  return ret;
}				/* copy_move_file() */


int
copy_file (const char *src, const char *dest)
{
  int ret = 0;
  char src_locale[MAX_PATH_RA];
  char dest_locale[MAX_PATH_RA];
#ifdef LINUX
  FILE *fp_in, *fp_out;
  unsigned char buf[1024];
  size_t len;
#endif /* LINUX */

  utf8_to_local (src, src_locale, MAX_PATH_RA);
  utf8_to_local (dest, dest_locale, MAX_PATH_RA);

#ifdef WIN32
  /* overwrite file if new_file exists */
  if (CopyFile (src_locale, dest_locale, 0) == FALSE)
    ret = -1;
#endif /* WIN32 */

#ifdef LINUX
  /* it seems that it exist no copy function in libc */
  fp_in = fopen (src_locale, "rb");
  fp_out = fopen (dest_locale, "wb");
  if (!fp_in || !fp_out)
    return -1;

  while ((len = fread (buf, 1, 1024, fp_in)))
    fwrite (buf, 1, len, fp_out);
  fclose (fp_in);
  fclose (fp_out);

  if (ret != 0)
    ret = errno;
#endif /* LINUX */

  return ret;
}				/* copy_file() */


int
move_file (const char *src, const char *dest)
{
  int ret = 0;
  char src_locale[MAX_PATH_RA];
  char dest_locale[MAX_PATH_RA];

  utf8_to_local (src, src_locale, MAX_PATH_RA);
  utf8_to_local (dest, dest_locale, MAX_PATH_RA);

#ifdef WIN32
  if (MoveFileEx
      (src_locale, dest_locale,
       MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED) == FALSE)
    ret = -1;
#endif /* WIN32 */

#ifdef LINUX
  ret = rename (src_locale, dest_locale);
  if ((ret != 0) && (errno == EXDEV))
    {
      ret = copy_file (src, dest);
      if (ret == 0)
	ret = delete_file (src);
    }

  if (ret != 0)
    ret = errno;
#endif /* LINUX */

  return ret;
}				/* move_file() */


/**
 * \brief delete a file (OS independent)
 * \param <file> file which should be deleted
 *
 * Function deletes a file.
 */
int
delete_file (const char *file)
{
  int ret = 0;
  char file_locale[MAX_PATH_RA];

#ifdef WIN32
  DWORD attrib;
#endif
#ifdef LINUX
  struct stat s;
#endif

  utf8_to_local (file, file_locale, MAX_PATH_RA);

#ifdef WIN32
  attrib = GetFileAttributes (file);
  if (attrib & FILE_ATTRIBUTE_DIRECTORY)
    {
      if (RemoveDirectory (file_locale) == FALSE)
	ret = -1;
    }
  else
    {
      if (DeleteFile (file_locale) == FALSE)
	ret = -1;
    }
#endif /* WIN32 */

#ifdef LINUX
  stat (file_locale, &s);
  if (S_ISDIR (s.st_mode))
    ret = rmdir (file_locale);
  else
    ret = unlink (file_locale);

  if (ret != 0)
    ret = errno;
#endif /* LINUX */

  return ret;
}				/* delete_file() */
