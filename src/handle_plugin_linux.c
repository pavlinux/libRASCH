/**
 * \file handle_plugin_linux.c
 *
 * This file implements to loading/de-loading of libRASCH plugins for Linux.
 * (For Win32-implementation see handle_plugin_win32.c)
 *
 * \author Raphael Schneider (ra@med1.med.tum.de)
 */

/*----------------------------------------------------------------------------
 *
 * Copyright (C) 2002-2007, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <dlfcn.h>
#include <dirent.h>
#include <sys/stat.h>

#define _LIBRASCH_BUILD
#include <ra_priv.h>
#include <handle_plugin.h>
#include <pl_general.h>


/**
 * \brief load plguins
 * \param <ra> pointer to general libRASCH struct
 *
 * The function loads all plugins found in the plugin-dir (given by the libRASCH configuration).
 * If a plugin loads successfully, the init-function of the plugin is called. The implementation
 * for Linux can be found in the file 'handle_plugin_linux.c' and for Win32 systems in the file
 * 'handle_plugin_win32.c'.
 */
int
read_plugins (struct librasch *ra)
{
  DIR *dir;
  struct dirent *dir_info;
  struct plugin_struct *phead = NULL;
  struct plugin_struct *pcurr = phead;
  char dir_locale[MAX_PATH_RA];

  utf8_to_local (ra->config.plugin_dir, dir_locale, MAX_PATH_RA);

  if ((dir = opendir (dir_locale)) == NULL)
    return -1;

  while ((dir_info = readdir (dir)) != NULL)
    {
      char path[MAX_PATH_RA];
      struct stat s;
      struct plugin_struct *pnew;

      sprintf (path, "%s%c%s", dir_locale, SEPARATOR, dir_info->d_name);

      if (stat (path, &s) == -1)
	continue;		/* TODO: error handling */

      /* check if d_name is not a dir */
      if (S_ISDIR (s.st_mode))
	continue;

      /* no -> try to load the plugin */
      pnew = dload_plugin (path);
      if (!pnew)
	{
	  if (ra_print_debug_infos)
	    fprintf (stderr, gettext ("can't load %s: %s\n"), path,
		     dlerror ());
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
  closedir (dir);
  ra->pl_head = phead;

  /* now run init-functions for every plugin */
  pcurr = phead;
  while (pcurr)
    {
      struct plugin_struct *p;
      int (*init) (struct librasch *, struct plugin_struct *);

/* 		*(void **) (&cosine)init = (int(*)(struct librasch *, struct plugin_struct *))dlsym(pcurr->dl, INIT_PLUGIN_FUNC); */
      dlerror ();
      *(void **) (&init) = dlsym (pcurr->dl, INIT_PLUGIN_FUNC);
      if (!init)
	{
	  if (ra_print_debug_infos)
	    fprintf (stderr, gettext ("error load plugin %s: %s\n"),
		     pcurr->info.name, dlerror ());

	  dlclose (pcurr->dl);
	  if (pcurr->prev)
	    pcurr->prev->next = pcurr->next;
	  p = pcurr;
	  free (p);
	  pcurr = pcurr->next;
	  continue;
	}
      if ((*init) (ra, pcurr) != 0)
	{
	  if (ra_print_debug_infos)
	    fprintf (stderr, gettext ("error init plugin %s: %s\n"),
		     pcurr->info.name, dlerror ());

	  dlclose (pcurr->dl);
	  if (pcurr->prev)
	    pcurr->prev->next = pcurr->next;
	  p = pcurr;
	  free (p);
	  pcurr = pcurr->next;
	  continue;
	}
      pcurr = pcurr->next;
    }

  return 0;
}				/* read_plugins() */



/**
 * \brief do dynamic loading
 * \param <file> filename of a plugin (locale encoding)
 *
 * Load a plugin and call 'load_ra_plugin' function in the loaded plugin.
 */
struct plugin_struct *
dload_plugin (char *file)
{
  int ret = 0;
  struct plugin_struct *p;
  int (*load) (struct plugin_struct *);

  p = malloc (sizeof (*p));
  if (!p)
    return NULL;
  memset (p, 0, sizeof (*p));
  p->handle_id = RA_HANDLE_PLUGIN;

  p->dl = dlopen (file, RTLD_NOW);
  if (!p->dl)
    {
      if (ra_print_debug_infos)
	fprintf (stderr, gettext ("dlopen() failed\n"));

      free (p);
      return NULL;
    }

  dlerror ();
  *(void **) (&load) = dlsym (p->dl, LOAD_PLUGIN_FUNC);
  if (!load)
    {
      if (ra_print_debug_infos)
	fprintf (stderr, gettext ("dlsym() failed\n"));

      dlclose (p->dl);
      free (p);
      return NULL;
    }
  if ((ret = (*load) (p)) != 0)
    {
      dlclose (p->dl);
      free (p);
      return NULL;
    }

  if (!is_plugin_ok (p))
    {
      dlclose (p->dl);
      free (p);
      return NULL;
    }


  strncpy (p->info.file, file, MAX_PATH_RA);
  local_to_utf8_inplace (p->info.file, MAX_PATH_RA);

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
  /* check if minimal settings are done */
  return 1;
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

      ra_free_mem (p->info.create_class);
      ra_free_mem (p->info.create_prop);


/* 		close_func = (void(*)())dlsym(p->dl, FINI_PLUGIN_FUNC); */
      dlerror ();
      *(void **) (&close_func) = dlsym (p->dl, FINI_PLUGIN_FUNC);
      if (close_func)
	(*close_func) ();
      dlclose (p->dl);
      next = p->next;
      free (p);
      p = next;
    }
}				/* close_plugins() */
