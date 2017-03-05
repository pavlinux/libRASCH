/**
 * \file handle_plugin_win32.c
 *
 * This file implements to loading/de-loading of libRASCH plugins for Windows.
 * (For Linux-implementation see handle_ra_plugin_linux.c)
 *
 * \author Raphael Schneider (ra@med1.med.tum.de)
 */

/*----------------------------------------------------------------------------
 *
 * Copyright (C) 2002, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *
 *--------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <windows.h>

#include <ra_priv.h>
#include <handle_plugin.h>
#include <pl_general.h>

/* file pointer to debug-file (global variable in ra.c) */
extern FILE *ra_debug_file;

#ifdef WIN32
	/* Using fprintf() and printf() from libintl will crash on Win32.
	   The reason is maybe using different c-libs. */
#undef fprintf
#undef printf
#endif /* WIN32 */

/**
 * \brief load plguins
 * \param <ra> pointer to general libRASCH struct
 *
 * The function loads all plugins found in the plugin-dir (given by the libRASCH configuration).
 * If a plugin loads successfully, the init-function of the plugin is called.
 */
int
read_plugins (struct librasch *ra)
{
  char mask[MAX_PATH_RA];
  WIN32_FIND_DATA find;
  HANDLE h;
  struct plugin_struct *phead = NULL;
  struct plugin_struct *pcurr = phead;

  sprintf (mask, "%s\\*.dll", ra->config.plugin_dir);
  h = FindFirstFile (mask, &find);
  if (h == INVALID_HANDLE_VALUE)
    return -1;

  do
    {
      char path[MAX_PATH_RA];
      struct plugin_struct *pnew;

      sprintf (path, "%s%c%s", ra->config.plugin_dir, SEPARATOR,
	       find.cFileName);

      /* check if d_name is not a dir */
      if (find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	continue;

      /* no -> try to load the plugin */
      pnew = dload_plugin (path);
      if (!pnew)
	{
	  if (ra_debug_file)
	    fprintf (ra_debug_file, "plugin %25s failed to load\n",
		     find.cFileName);
	  continue;
	}
      pnew->ra = ra;

      if (pcurr)
	{
	  pcurr->next = pnew;
	  pnew->prev = pcurr;
	  pcurr = pcurr->next;
	}
      else
	phead = pcurr = pnew;
    }
  while (FindNextFile (h, &find));
  FindClose (h);
  ra->pl_head = phead;

  /* now run init-functions for every plugin */
  pcurr = phead;
  while (pcurr)
    {
      int (*init) (struct librasch *, struct plugin_struct *);
      struct plugin_struct *p;

      init = (int (*)(struct librasch *, struct plugin_struct *))
	GetProcAddress (pcurr->dl, INIT_PLUGIN_FUNC);
      if (!init)
	{
	  LPVOID lpMsgBuf;
	  FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER |
			 FORMAT_MESSAGE_FROM_SYSTEM |
			 FORMAT_MESSAGE_IGNORE_INSERTS,
			 NULL, GetLastError (),
			 MAKELANGID (LANG_NEUTRAL,
				     SUBLANG_DEFAULT), (LPTSTR) & lpMsgBuf, 0,
			 NULL);

	  fprintf (stderr, "%s (%d): %s\n", __FILE__, __LINE__,
		   (LPCTSTR) lpMsgBuf);
	  LocalFree (lpMsgBuf);
	  FreeLibrary (pcurr->dl);
	  if (pcurr->prev)
	    pcurr->prev->next = pcurr->next;
	  if (pcurr == phead)
	    phead = pcurr->next;
	  p = pcurr;
	  pcurr = pcurr->next;
	  free (p);
	  continue;
	}

      if ((*init) (ra, pcurr) != 0)
	{
	  LPVOID lpMsgBuf;
	  FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER |
			 FORMAT_MESSAGE_FROM_SYSTEM |
			 FORMAT_MESSAGE_IGNORE_INSERTS,
			 NULL, GetLastError (),
			 MAKELANGID (LANG_NEUTRAL,
				     SUBLANG_DEFAULT), (LPTSTR) & lpMsgBuf, 0,
			 NULL);

	  fprintf (stderr, "%s (%d): %s\n", __FILE__, __LINE__,
		   (LPCTSTR) lpMsgBuf);
	  LocalFree (lpMsgBuf);
	  FreeLibrary (pcurr->dl);
	  if (pcurr->prev)
	    pcurr->prev->next = pcurr->next;
	  if (pcurr == phead)
	    phead = pcurr->next;
	  p = pcurr;
	  pcurr = pcurr->next;
	  free (p);
	  continue;
	}

      pcurr = pcurr->next;
    }

  ra->pl_head = phead;

  return 0;
}				/* read_plugins() */


/**
 * \brief do dynamic loading
 * \param <file> filename of a plugin
 *
 * Load a plugin and call 'load_ra_plugin' function in the loaded plugin.
 */
struct plugin_struct *
dload_plugin (char *file)
{
  int ret = 0;
  struct plugin_struct *p;
  int (*load) (struct plugin_struct *);

  p = malloc (sizeof (struct plugin_struct));
  if (!p)
    return NULL;
  memset (p, 0, sizeof (struct plugin_struct));
  p->handle_id = RA_HANDLE_PLUGIN;

  p->dl = LoadLibrary (file);
  if (!p->dl)
    {
      LPVOID lpMsgBuf;
      FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER |
		     FORMAT_MESSAGE_FROM_SYSTEM |
		     FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
		     GetLastError (), MAKELANGID (LANG_NEUTRAL,
						  SUBLANG_DEFAULT),
		     (LPTSTR) & lpMsgBuf, 0, NULL);

      fprintf (stderr, "%s (%d): %s\n", __FILE__, __LINE__,
	       (LPCTSTR) lpMsgBuf);
      LocalFree (lpMsgBuf);
      free (p);
      return NULL;
    }

  load =
    (int (*)(struct plugin_struct *)) GetProcAddress (p->dl,
						      LOAD_PLUGIN_FUNC);
  if (!load)
    {
      LPVOID lpMsgBuf;
      FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER |
		     FORMAT_MESSAGE_FROM_SYSTEM |
		     FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
		     GetLastError (), MAKELANGID (LANG_NEUTRAL,
						  SUBLANG_DEFAULT),
		     (LPTSTR) & lpMsgBuf, 0, NULL);
      fprintf (stderr, "%s (%d): %s\n", __FILE__, __LINE__,
	       (LPCTSTR) lpMsgBuf);
      LocalFree (lpMsgBuf);
      FreeLibrary (p->dl);
      free (p);
      return NULL;
    }

  if ((ret = (*load) (p)) != 0)
    {
      LPVOID lpMsgBuf;
      FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER |
		     FORMAT_MESSAGE_FROM_SYSTEM |
		     FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
		     GetLastError (), MAKELANGID (LANG_NEUTRAL,
						  SUBLANG_DEFAULT),
		     (LPTSTR) & lpMsgBuf, 0, NULL);
      fprintf (stderr, "%s (%d): %s\n", __FILE__, __LINE__,
	       (LPCTSTR) lpMsgBuf);
      LocalFree (lpMsgBuf);
      FreeLibrary (p->dl);
      free (p);
      return NULL;
    }

  if (!is_plugin_ok (p))
    {
      LPVOID lpMsgBuf;
      FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER |
		     FORMAT_MESSAGE_FROM_SYSTEM |
		     FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
		     GetLastError (), MAKELANGID (LANG_NEUTRAL,
						  SUBLANG_DEFAULT),
		     (LPTSTR) & lpMsgBuf, 0, NULL);
      fprintf (stderr, "%s (%d): %s\n", __FILE__, __LINE__,
	       (LPCTSTR) lpMsgBuf);
      LocalFree (lpMsgBuf);
      FreeLibrary (p->dl);
      free (p);
      return NULL;
    }

  strncpy (p->info.file, file, MAX_PATH_RA);
  p->info.use_plugin = 1;

  return p;
}				/* dload_plugin() */


/**
 * \brief checks if plugin is ok
 * \param <p> pointer to a pluign struct
 *
 * The function checks if a plugin is ok. (Nothing special yet.)
 */
int
is_plugin_ok (struct plugin_struct *p)
{
  if (p != NULL)
    return 1;
  else
    return 0;
}				/* is_plugin_ok() */


/**
 * \brief close a plugin
 * \param <p> pointer to a plugin struct
 *
 * The function calls the closing functions of a plugin (so plugin can free allocated memory)
 * and unloads the plugin.
 */
void
close_plugins (struct plugin_struct *p)
{
  struct plugin_struct *next;
  void (*close_func) ();

  while (p)
    {
      if (p->info.res)
	ra_free_mem (p->info.res);
      if (p->info.opt)
	ra_free_mem (p->info.opt);

      close_func = (void (*)()) GetProcAddress (p->dl, FINI_PLUGIN_FUNC);
      if (close_func)
	(*close_func) ();
      if (FreeLibrary (p->dl) == 0)
	{
	  LPVOID lpMsgBuf;
	  FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER |
			 FORMAT_MESSAGE_FROM_SYSTEM |
			 FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
			 GetLastError (), MAKELANGID (LANG_NEUTRAL,
						      SUBLANG_DEFAULT),
			 (LPTSTR) & lpMsgBuf, 0, NULL);
	  fprintf (stderr, "%s (%d): %s\n", __FILE__, __LINE__,
		   (LPCTSTR) lpMsgBuf);
	  LocalFree (lpMsgBuf);
	}

      next = p->next;
      free (p);
      p = next;
    }
}				/* close_plugins() */
