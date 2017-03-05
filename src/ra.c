/**
 * \file ra.c
 *
 * This file provides most part of the libRASCH-API (the evaluation-API can be
 * found in ./eval/eval.c).
 *
 * \author Raphael Schneider (rasch@med1.med.tum.de)
 */

/*--------------------------------------------------------------------------
 *
 * Copyright (C) 2002-2007, Raphael Schneider
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
#include <ra_priv.h>
#include <ra_version.h>
#include <ra.h>
#include <ra_error_txt.h>
#include <handle_plugin.h>
#include <pl_general.h>
#include <ra_file.h>
#include <ra_endian.h>

#include <libxml/tree.h>
#include <libxml/parser.h>

#ifdef WIN32
	/* Using fprintf() and printf() from libintl will crash on Win32.
	   The reason is maybe using different c-libs. */
#undef fprintf
#undef printf
#endif /* WIN32 */


/**
 * A global variable which controls debug-infos.
 */
int ra_print_debug_infos;

/**
 * A global variable which controls if already in error-mode (needed to avoid circular calling).
 */
int ra_in_error;

/**
 * A global variable which gives endian type on the running machine (set in ra_lib_init)
 */
int ra_endian_type;

/**
 * A global variable with the name of the current codeset
 */
char ra_i18n_codeset[MAX_I18N_LEN];

/**
 * A global variable holding a file pointer to a debug file
 * (test that it is not NULL before writing in it)
 */
FILE *ra_debug_file = NULL;

/**
 * \brief init libRASCH
 *
 * Init libRASCH, read config-file and init plugins; returns ra_handle.
 */
LIBRAAPI ra_handle
ra_lib_init (void)
{
  int ret = 0;
  struct librasch *ra;
  char *debug_fn;

  if (getenv ("LIBRASCH_DEBUG"))
    ra_print_debug_infos = atoi (getenv ("LIBRASCH_DEBUG"));
  else
    ra_print_debug_infos = 0;
  if (getenv ("LIBRASCH_DEBUG_FILE"))
    {
      debug_fn = getenv ("LIBRASCH_DEBUG_FILE");
      ra_debug_file = fopen (debug_fn, "w");
    }
  else
    ra_debug_file = NULL;
  ra_in_error = 0;

  set_endian_type ();		/* set global variable ra_endian_type */

  ra = malloc (sizeof (*ra));
  if (ra == NULL)
    return NULL;
  memset (ra, 0, sizeof (*ra));
  ra->handle_id = RA_HANDLE_LIB;
  _ra_set_error (ra, RA_ERR_NONE, "libRASCH");

  ra->hf.get_plugin = get_plugin_by_name;
  if ((ret = read_config (ra)) != 0)
    {
      if (ret == -1)
	_ra_set_error (ra, RA_WARN_NO_CONFIG_FILE, "libRASCH");
      else
	_ra_set_error (ra, RA_ERR_READ_CONFIG, "libRASCH");
      return (ra_handle) ra;
    }

  /* set global variable ra_i18n_codeset;
     do it after reading the config because on Win32, the
     place of the translation will be set when installing libRASCH */
  init_i18n (ra);

  close_plugins (ra->pl_head);
  if (read_plugins (ra) != 0)
    {
      _ra_set_error (ra, RA_ERR_LOAD_PLUGINS, "libRASCH");
      return (ra_handle) ra;
    }

  return (ra_handle) ra;
}				/* ra_lib_init() */


/**
 * \brief set endian of current system
 *
 * This function get the endianness of the current system on runtime and set the
 * global variable 'ra_endian_type' accordingly.
 */
void
set_endian_type ()
{
  short s;
  char *t;

  s = 'U' * 256 + 'N';

  t = (char *) &s;
  if (*t == 'N')
    ra_endian_type = RA_LITTLE_ENDIAN;
  else
    ra_endian_type = RA_BIG_ENDIAN;
}				/* set_endian_type() */


/**
 * \brief call functions and set variables needed for i18n
 * \param <ra> pointer to general libRASCH struct
 *
 * This function call functions for i18n handling and set global variables for
 * i18n. The following variable(s) are set:
 * 
 * - ra_i18n_codeset: the name of the current codeset ("" if locale is not working)
 */
void
init_i18n (struct librasch *ra)
{
  const char *l = NULL;

  /* when in debug mode, do not set language so messages are not translated */
  if (ra_print_debug_infos)
    return;

  l = setlocale (LC_ALL, "");
  if (l)
    {
      char *p = NULL;
      char *buf;

      buf = malloc (strlen (l) + 1);
      strcpy (buf, l);
      p = strrchr (buf, '.');
      if (p)
	{
	  /* when the charset startes with a number, then add 'CP' so iconv
	     understand the charset */
	  if ((*(p + 1) >= '0') && (*(p + 1) <= '9')
	      && (strlen (p + 1) < (MAX_I18N_LEN - 3)))
	    sprintf (ra_i18n_codeset, "CP%s", p + 1);
	  else
	    strncpy (ra_i18n_codeset, p + 1, MAX_I18N_LEN);
	}
      else
	ra_i18n_codeset[0] = '\0';
      free (buf);
    }
  bindtextdomain (PACKAGE, ra->config.po_dir);
  textdomain (PACKAGE);
}				/* init_i18n() */


/**
 * \brief returns endian type
 *
 * This function returns the endian type for the running system.
 */
LIBRAAPI int
get_endian_type ()
{
  return ra_endian_type;
}				/* get_endian_type() */


/**
 * \brief read libRASCH configuration
 * \param <ra> pointer to general libRASCH struct
 *
 * Read the configuration set for libRASCH. On Windows, first try to get
 * configuration from Registry. Than try to get configuration from
 * libRASCH config-file. (See find_config_file() in which locations is looked for.)
 * When no config-file was found, '-1' will be returned. When an error inside the
 * config-file was found, '-2' will be returned.
 */
int
read_config (struct librasch *ra)
{
  char fn[MAX_PATH_RA];
  xmlDocPtr doc;
  xmlNodePtr node;
  xmlChar *c;
  int config_found;

  /* set configuration to values which work when no config-file is found */
  init_config (&(ra->config));

  /* first look for configuration file */
  config_found = find_config_file (fn);

#ifdef WIN32
  /* If no config-file was found and we are running at a Win32 system,
     we look at the Registry for config information. */
  if ((config_found == 0) && read_win32_registry (ra) == 0)
    return 0;
#endif

  /* no config -> go back and use default settings (all further needed
     files are in the current working directory) */
  if (config_found == 0)
    return -1;

  if (ra_print_debug_infos)
    fprintf (stderr, gettext ("config-file: %s\n"), fn);
  if ((doc = xmlParseFile (fn)) == NULL)
    return -2;
  if (!doc->children
      || (xmlStrcmp (doc->children->name, (xmlChar *) "librasch-config") !=
	  0))
    {
      xmlFreeDoc (doc);
      return -2;
    }

  /* TODO: think about config structure */
  node = doc->children->children;
  while (node)
    {
#ifdef _DEBUG			/* only needed for Win32 to handle debug-build plugins */
      if (xmlStrcmp (node->name, (xmlChar *) "plugin-path-debug") == 0)
#else
      if (xmlStrcmp (node->name, (xmlChar *) "plugin-path") == 0)
#endif /* _DEBUG */
	{
	  c = xmlNodeGetContent (node);
	  if (c != NULL)
	    {
	      strncpy (ra->config.plugin_dir, (const char *) c,
		       EVAL_MAX_NAME);
	      if (ra_print_debug_infos)
		fprintf (stderr, gettext ("config plugin-dir: %s\n"), c);
	      xmlFree (c);
	    }
	}
      else if (xmlStrcmp (node->name, (xmlChar *) "po-path") == 0)
	{
	  c = xmlNodeGetContent (node);
	  if (c != NULL)
	    {
	      strncpy (ra->config.po_dir, (const char *) c, EVAL_MAX_NAME);
	      if (ra_print_debug_infos)
		fprintf (stderr, gettext ("po-dir: %s\n"), ra->config.po_dir);
	      xmlFree (c);
	    }
	}
      else if (xmlStrcmp (node->name, (xmlChar *) "ch-map") == 0)
	{
	  c = xmlNodeGetContent (node);
	  if (c != NULL)
	    {
	      int ret;

	      ret =
		find_ch_map_entry ((char *) c, &(ra->config.ch_map_system),
				   &(ra->config.num_ch_map_system));
	      if (ra_print_debug_infos)
		fprintf (stderr, gettext ("ch-map: %s -> %s\n"), c,
			 (ret == 0 ? "ok" : "not ok"));
	      xmlFree (c);
	    }
	}
      node = node->next;
    }

  xmlFreeDoc (doc);

  return 0;
}				/* read_config() */


/**
 * \brief set configuration to values which work when no config-file is found
 * \param <conf> pointer to the configuration infos
 *
 * The function set the configuration parameters to values which will work
 * when no config-file is found.
 */
void
init_config (struct config_info *conf)
{
  char curr_dir[MAX_PATH_RA];

  sprintf (curr_dir, ".%c", SEPARATOR);
  strcpy (conf->plugin_dir, curr_dir);

  strncpy (conf->po_dir, LOCALEDIR, MAX_PATH_RA);

  conf->num_ch_map_system = 0;
  conf->ch_map_system = NULL;
}				/* init_config() */


/**
 * \brief read libRASCH configuration from Registry (only on Win32)
 * \param <ra> pointer to general libRASCH struct
 *
 * Reads libRASCH configuration from Registry on Win32 systems. The Registry entry
 * \\HKEY_LOCAL_MACHINE\Software\libRASCH is used.
 */
#ifdef WIN32
int
read_win32_registry (struct librasch *ra)
{
  int ret = -1;
  HKEY key;
  DWORD size, type;
  char line[MAX_PATH_RA];
  char key_string[MAX_PATH_RA];
  int i, num_ch_map_lines;

  /* Check that libRASCH is installed */
  if (RegOpenKeyEx
      (HKEY_LOCAL_MACHINE, "Software\\libRASCH", 0, KEY_READ,
       &key) != ERROR_SUCCESS)
    return -1;

  size = MAX_PATH_RA;
  type = REG_SZ;
#ifdef _DEBUG
  if (RegQueryValueEx
      (key, "plugin_path_debug", 0, &type, (LPBYTE) (ra->config.plugin_dir),
       &size) == ERROR_SUCCESS)
    ret = 0;
#else
  if (RegQueryValueEx
      (key, "plugin_path", 0, &type, (LPBYTE) (ra->config.plugin_dir),
       &size) == ERROR_SUCCESS)
    ret = 0;
#endif /* _DEBUG */

  if (RegQueryValueEx
      (key, "po_path", 0, &type, (LPBYTE) (ra->config.po_dir),
       &size) == ERROR_SUCCESS)
    ret = 0;

  num_ch_map_lines = 0;
  if (RegQueryValueEx (key, "num_ch_map", 0, &type, (LPBYTE) line, &size) ==
      ERROR_SUCCESS)
    {
      num_ch_map_lines = atoi (line);
      ret = 0;
    }
  for (i = 0; i < num_ch_map_lines; i++)
    {
      sprintf (key_string, "ch_map_%d", i + 1);
      if (RegQueryValueEx (key, key_string, 0, &type, (LPBYTE) line, &size) ==
	  ERROR_SUCCESS)
	ret =
	  find_ch_map_entry (line, &(ra->config.ch_map_system),
			     &(ra->config.num_ch_map_system));

      /* set return value to '0' so a non-matching ch-map does not
         results in a failure to init libRASCH 
         (TODO: decide how to handle ch-map finding failure) */
      ret = 0;
    }

  RegCloseKey (key);

  return ret;
}				/* read_win32_registry() */
#endif /* WIN32 */


/**
 * \brief look for libRASCH configuration file
 * \param <fn> contains path of config file if function ended successful
 *
 * This function looks for the libRASCH configuration file. For the config file is
 * first looked in the current directory. If there is none than it looks in '/etc'(Linux)
 * resp. in the system directory (Win32). If not found, on Linux the $HOME directory is
 * searched also.
 */
int
find_config_file (char *fn)
{
  FILE *fp = NULL;
  int found = 0;

  /* first check current-dir */
  sprintf (fn, ".%clibrasch.conf", SEPARATOR);

  fp = fopen (fn, "r");
#ifdef LINUX
  if (!fp)
    {
      /* if not found in the current directory then check on Linux first
         the HOME directory */
      sprintf (fn, "%s/librasch.conf", getenv ("HOME"));
      fp = fopen (fn, "r");

    }
#endif /* LINUX */
  if (!fp)
    {
      /* now check /etc under LINUX resp. system-dir under Windows */
#ifdef LINUX
      sprintf (fn, "/etc/librasch.conf");
#else /* WINDOWS */
      GetWindowsDirectory (fn, MAX_PATH_RA);
      strcat (fn, "\\librasch.conf");
#endif
      fp = fopen (fn, "r");
    }

  if (fp)
    {
      found = 1;
      fclose (fp);
    }

  return found;
}				/* find_config_file() */


/**
 * \brief close libRASCH
 * \param <h> handle to ra instance
 *
 * Close libRASCH, close plugins and frees all allocated memory.
 */
LIBRAAPI void
ra_lib_close (ra_handle h)
{
  struct librasch *ra = (struct librasch *) h;

  close_plugins (ra->pl_head);

  if (ra->error_from)
    {
      free (ra->error_from);
      ra->error_from = NULL;
    }

  if (ra->config.ch_map_system)
    free_ch_map (&(ra->config.ch_map_system), ra->config.num_ch_map_system);

  if (ra_debug_file)
    fclose (ra_debug_file);

  free (ra);
}				/* ra_lib_close() */


/**
 * \brief get last error
 * \param <h> handle to ra instance
 * \param <text> character buffer which receives error text
 * \param <len> size of character buffer text
 *
 * This function returns the last error occured in libRASCH (or in one of the
 * plugins). See 'ra_error.h' for meaning of returned number. If the number is negative,
 * the last error was an OS specific error and the error number comes from the OS (just
 * remove the minus sign). If text is not NULL, a short error description will be
 * returned in text.
 */
LIBRAAPI long
ra_lib_get_error (ra_handle h, char *text, size_t len)
{
  struct librasch *ra = (struct librasch *) h;

  if (!h || (ra->error_number == 0))
    return 0;

  if (text && len > 0)
    {
      char *t;
      size_t len_use;

      if (ra->error_from)
	len_use = len - strlen (ra->error_from) - 2;
      else
	len_use = len;

      if (len_use <= 0)
	{
	  strncpy (text, ra->error_from, len);
	  text[len - 1] = '\0';
	  return ra->error_number;
	}

      t = malloc (len_use);

      get_error_text (ra->error_number, t, len_use);

      if (ra->error_from)
	sprintf (text, "%s: %s", ra->error_from, t);
      else
	strcpy (text, t);

      free (t);

      utf8_to_local_inplace (text, len);
    }

  return ra->error_number;
}				/* ra_lib_get_error() */


/**
 * \brief returns error-string for OS specific errors
 * \param <err> OS specific error number
 * \param <text> array in which error-string will be set
 * \param <len> length of array 'text'
 *
 * Function gets error-string for OS specific error numbers.
 */
#ifdef WIN32
void
get_error_text (long err, char *text, size_t len)
{
  text[0] = '\0';

  if (err > 0)
    get_error_text_ra (err, text, len);
  else
    {
      LPVOID msg_buf;

      if (!FormatMessage
	  (FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
	   FORMAT_MESSAGE_IGNORE_INSERTS, NULL, -err,
	   MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) & msg_buf, 0,
	   NULL))
	{
	  return;
	}

      strncpy (text, (LPCTSTR) msg_buf, len);
      LocalFree (msg_buf);
    }
}				/* get_error_text() */
#endif /* WIN32 */


#ifdef LINUX
void
get_error_text (long err, char *text, size_t len)
{
  text[0] = '\0';

  if (err > 0)
    get_error_text_ra (err, text, len);
  else
    strerror_r (-err, text, len);
}				/* get_error_text() */
#endif /* LINUX
        */

/**
 * \brief returns error-string for libRASCH errors
 * \param <err> libRASCH error number
 * \param <text> array in which error-string will be set
 * \param <len> length of array 'text'
 *
 * Function gets error-string for libRASCH error numbers.
 */
void
get_error_text_ra (long err, char *text, size_t len)
{
  const char *c;

  if (err <= 0)
    return;

  if (err <= ra_num_error_text)
    {
      c = ra_error_text[err - 1];

      strncpy (text, (c[0] == '\0' ? "" : gettext (c)), len);
    }
  else if ((err > RA_WARNING) && ((err - RA_WARNING) <= ra_num_warning_text))
    {
      c = ra_warning_text[err - RA_WARNING - 1];

      strncpy (text, (c[0] == '\0' ? "" : gettext (c)), len);
    }
}				/* get_error_text_ra() */


/**
 * \brief set error number
 * \param <h> any libRASCH handle (except value_handle)
 * \param <error> error number
 * \param <error_from> source of error (short name)
 * \param <file> name of the source-file the function is called
 * \param <line> line in the source-file the function is called
 *
 * Function is called when an error occured and set the error number. Additionally,
 * the source of the error can be given. This function is called from the libRASCH
 * core and from the plugins. This function should _not_ be called from the user-space.
 */
LIBRAAPI void
_ra_set_error_int (any_handle h, long error, const char *error_from,
		   const char *file, int line)
{
  struct librasch *ra;

  if (ra_print_debug_infos >= 2)
    {
      fprintf (stderr, "set-error called from %s (%s:%d) for %p\n",
	       error_from, file, line, h);
    }

  if (ra_in_error)
    {
      fprintf (stderr,
	       gettext
	       ("circular set_error-calling, this should NOT happen\n"));
      fprintf (stderr, "called from %s (%s:%d)\n", error_from, file, line);
      ra_in_error = 0;
      return;
    }

  ra_in_error = 1;

  if (!h)
    return;

  /* get pointer to struct librasch */
  ra = (struct librasch *) ra_lib_handle_from_any_handle (h);
  if (!ra)
    {
      ra_in_error = 0;
      return;
    }

  if (ra->error_from)
    {
      free (ra->error_from);
      ra->error_from = NULL;
    }
  if (error_from || file)
    {
      ra->error_from = malloc (strlen (error_from) + strlen (file) + 4 + 10);
      ra->error_from[0] = '\0';
      if (error_from)
	strcat (ra->error_from, error_from);
      if (file)
	{
	  char t[20];

	  if (error_from)
	    strcat (ra->error_from, " (");
	  strcat (ra->error_from, file);

	  sprintf (t, ":%d", line);
	  strcat (ra->error_from, t);
	  if (error_from)
	    strcat (ra->error_from, ")");
	}
    }

  /* if error == 0 -> os error */
  if (error == 0)
    {
#ifdef LINUX
      ra->error_number = -errno;
#endif
#ifdef WIN32
      ra->error_number = GetLastError ();
      ra->error_number = -ra->error_number;
#endif
    }
  else
    ra->error_number = error;

  if (ra_print_debug_infos && (ra->error_number != RA_ERR_NONE))
    {
      char t[1000];
      long err_num;

      err_num = ra_lib_get_error (ra, t, 1000);
      fprintf (stderr, "%s:%d - %s (%ld)\n", file, line, t, err_num);
    }

  ra_in_error = 0;
}				/* _ra_set_error() */


/**
 * \brief retrive handle of libRASCH instance from any handle
 * \param <h> handle
 *
 * Returns handle of libRASCH instance from any handle.
 */
LIBRAAPI ra_handle
ra_lib_handle_from_any_handle (any_handle h)
{
  unsigned short type = *((unsigned short *) h);
  struct ra_meas *meas;
  ra_handle ra = NULL;

  switch (type)
    {
    case RA_HANDLE_LIB:
      ra = h;
      break;
    case RA_HANDLE_MEAS:
    case RA_HANDLE_REC:
    case RA_HANDLE_EVAL:
    case RA_HANDLE_EVAL_CLASS:
    case RA_HANDLE_EVAL_PROP:
    case RA_HANDLE_EVAL_SUMMARY:
      meas = ra_meas_handle_from_any_handle (h);
      if (meas)
	ra = meas->ra;
      break;
    case RA_HANDLE_PLUGIN:
      ra = ((struct plugin_struct *) h)->ra;
      break;
    case RA_HANDLE_PROC:
      ra = ((struct proc_info *) h)->ra;
      break;
    case RA_HANDLE_VIEW:
      ra = ((struct view_info *) h)->ra;
      break;
    case RA_HANDLE_FIND:
    default:
      break;
    }

  return ra;
}				/* ra_lib_handle_from_any_handle() */


/**
 * \brief retrive measurement-handle from any handle
 * \param <h> handle
 *
 * Returns measurement-handle.
 */
LIBRAAPI meas_handle
ra_meas_handle_from_any_handle (any_handle h)
{
  unsigned short type = *((unsigned short *) h);
  struct ra_rec *rec;
  struct eval *eval;
  struct eval_class *cl;
  struct eval_property *prop;
  struct eval_summary *sum;
  meas_handle mh = NULL;

  if (h == NULL)
    return NULL;

  switch (type)
    {
    case RA_HANDLE_MEAS:
      mh = h;
      break;
    case RA_HANDLE_REC:
      rec = (struct ra_rec *) h;
      mh = rec->meas;
      break;
    case RA_HANDLE_EVAL:
      eval = (struct eval *) h;
      mh = eval->meas;
      break;
    case RA_HANDLE_EVAL_CLASS:
      cl = (struct eval_class *) h;
      mh = cl->meas;
      break;
    case RA_HANDLE_EVAL_PROP:
      prop = (struct eval_property *) h;
      mh = prop->meas;
      break;
    case RA_HANDLE_EVAL_SUMMARY:
      sum = (struct eval_summary *) h;
      mh = sum->meas;
      break;
    default:
      _ra_set_error (h, RA_ERR_UNKNOWN_HANDLE, "libRASCH");
      break;
    }

  return mh;
}				/* ra_meas_handle_from_any_handle() */


/**
 * \brief set using a plugin (or not)
 * \param <h> handle to libRASCH
 * \param <plugin_index> index of plugin to change
 * \param <use_it> flag using plugin (1: use plugin, !1: do not use plugin)
 *
 * Use or do not use plugin.
 */
LIBRAAPI int
ra_lib_use_plugin (ra_handle h, int plugin_index, int use_it)
{
  struct librasch *ra = (struct librasch *) h;
  struct plugin_struct *p = get_plugin (ra, plugin_index);

  if (!p)
    {
      _ra_set_error (h, RA_ERR_UNKNOWN_PLUGIN, "libRASCH");
      return -1;
    }

  p->info.use_plugin = use_it;

  return 0;
}				/* ra_lib_use_plugin() */


/**
 * \brief returns plugin-handle
 * \param <ra> pointer to general libRASCH struct
 * \param <number> number of the plugin
 *
 * Function returns the plugin-handle for the plugin #number (zero based).
 */
struct plugin_struct *
get_plugin (struct librasch *ra, int number)
{
  struct plugin_struct *p = ra->pl_head;
  int cnt = number;

  while (p && cnt--)
    p = p->next;

  return p;
}				/* get_plugin() */


/**
 * \brief returns plugin-handle
 * \param <ra> pointer to general libRASCH struct
 * \param <name> name of plugin
 *
 * Function returns the plugin-handle for the plugin with the name 'name'.
 */
struct plugin_struct *
get_plugin_by_name (struct librasch *ra, const char *name)
{
  struct plugin_struct *p = ra->pl_head;
  while (p && (strcmp (p->info.name, name) != 0))
    p = p->next;
  return p;
}				/* get_plugin_by_name() */


/**
 * \brief get plugin-handle
 * \param <h> handle to libRASCH
 * \param <plugin_num> number of plugin
 * \param <search_all> flag if all plugins should be searched (or only those with the use-it-flag enabled)
 *
 * Returns the plugin-handle for plugin #plugin_num.
 */
LIBRAAPI plugin_handle
ra_plugin_get_by_num (ra_handle h, int plugin_num, int search_all)
{
  struct librasch *ra = (struct librasch *) h;
  struct plugin_struct *p = get_plugin (ra, plugin_num);

  if (p && !search_all && !p->info.use_plugin)
    {
      _ra_set_error (h, RA_ERR_UNKNOWN_PLUGIN, "libRASCH");
      p = NULL;
    }

  return p;
}				/* ra_plugin_get_by_num() */


/**
 * \brief get plugin-handle
 * \param <h> handle to libRASCH
 * \param <name> name of plugin
 * \param <search_all> flag if all plugins should be searched (or only those with the use-it-flag enabled)
 *
 * Returns the plugin-handle for plugin with name name.
 */
LIBRAAPI plugin_handle
ra_plugin_get_by_name (ra_handle h, const char *name, int search_all)
{
  struct librasch *ra = (struct librasch *) h;
  struct plugin_struct *p = NULL;
  char *name_utf8;

  if (!h)
    {
      _ra_set_error (h, RA_ERR_ERROR_INTERNAL, "libRASCH");
      return NULL;
    }

  name_utf8 = malloc (strlen (name) * 2);
  local_to_utf8 (name, name_utf8, strlen (name) * 2);
  p = get_plugin_by_name (ra, name_utf8);
  free (name_utf8);

  if (p && !search_all && !p->info.use_plugin)
    {
      _ra_set_error (h, RA_ERR_UNKNOWN_PLUGIN, "libRASCH");
      p = NULL;
    }

  return p;
}				/* ra_plugin_get_by_name() */


/**
 * \brief find libRASCH-handled measurements (return first one)
 * \param <h> handle to libRASCH
 * \param <dir> directory which should be scanned for measurements
 * \param <rfs> pointer to ra_find_struct which will held the infos for the found measurement.
 *
 * Search for libRASCH-handled measurements in dir. If at least on measurement was found a valid (!NULL)
 * ra_find_handle will be returned and the infos about the measurement is in mfs.
 */
LIBRAAPI ra_find_handle
ra_meas_find_first (ra_handle h, const char *dir, struct ra_find_struct * rfs)
{
  char *dir_utf8;
  struct plugin_struct *p = ((struct librasch *) h)->pl_head;
  struct find_meas *f = calloc (1, sizeof (struct find_meas));
  f->handle_id = RA_HANDLE_FIND;

  dir_utf8 = malloc (strlen (dir) * 2);
  local_to_utf8 (dir, dir_utf8, strlen (dir) * 2);

  while (p)
    {
      /* use plugin and (find_meas() is assigned) ? */
      if (!p->info.use_plugin || !p->access.find_meas)
	{
	  p = p->next;
	  continue;
	}

      /* look for signals handled by current plugin in dir */
      (*p->access.find_meas) (dir_utf8, f);

      /* next plugin */
      p = p->next;
    }

  free (dir_utf8);

  if (f->num <= 0)
    {
      free (f);
      f = NULL;
    }
  else
    {
      strcpy (rfs->name, f->names[f->curr]);
      utf8_to_local_inplace (rfs->name, MAX_PATH_RA);

      strcpy (rfs->plugin, f->plugins[f->curr]);
      utf8_to_local_inplace (rfs->plugin, RA_VALUE_NAME_MAX);

      f->curr++;
    }

  return (ra_find_handle) f;
}				/* ra_meas_find_first() */


/**
 * \brief find libRASCH-handled measurements (returns next)
 * \param <h> find-handle
 * \param <rfs> pointer to ra_find_struct which will held the infos for the found measurement.
 *
 * Returns 1 if another measurement was found (the information will be in mfs) or 0 if there
 * are no more measurements searched with ra_meas_find_first().
 */
LIBRAAPI int
ra_meas_find_next (ra_find_handle h, struct ra_find_struct *rfs)
{
  struct find_meas *f = (struct find_meas *) h;
  f->handle_id = RA_HANDLE_FIND;

  if (!f || (f->curr >= f->num))
    return 0;

  strcpy (rfs->name, f->names[f->curr]);
  utf8_to_local_inplace (rfs->name, MAX_PATH_RA);

  strcpy (rfs->plugin, f->plugins[f->curr]);
  utf8_to_local_inplace (rfs->plugin, RA_VALUE_NAME_MAX);

  f->curr++;

  return 1;
}				/* ra_meas_find_next() */


/**
 * \brief close the search for libRASCH-handled measurements
 * \param <h> find-handle
 *
 * End a search for libRASCH-handled measurements (frees some memory). Must be called if
 * the ra_find_handle h returned from ra_meas_find_first() will no be longer used.
 */
LIBRAAPI void
ra_meas_close_find (ra_find_handle h)
{
  int i;
  struct find_meas *f = (struct find_meas *) h;

  if (f == NULL)
    return;

  if (f->names != NULL)
    {
      for (i = 0; i < f->num; i++)
	{
	  if (f->names[i] != NULL)
	    ra_free_mem (f->names[i]);
	}
      ra_free_mem (f->names);
    }

  if (f->plugins != NULL)
    {
      for (i = 0; i < f->num; i++)
	{
	  if (f->plugins[i] != NULL)
	    ra_free_mem (f->plugins[i]);
	}
      ra_free_mem (f->plugins);
    }
  free (f);
}				/* ra_meas_close_find() */


/**
 * \brief open a measurement
 * \param <h> handle to libRASCH
 * \param <file> file-name of the measurement
 * \param <eval_file> file-name of the evaluation
 * \param <fast> flag if some init-code should be done (=0) or not (=1)
 *
 * Opens the measurement file and returns a handle to the measurement. If the fast-flag
 * is set, some initialisation-code (e.g. reading evaluation infos from the files) will not
 * be done. This can be useful if only information about the measurment-object is needed
 * but not about the evaluation. Getting the evaluation infos can be "very" time consuming.
 */
LIBRAAPI meas_handle
ra_meas_open (ra_handle h, const char *file, const char *eval_file, int fast)
{
  struct librasch *ra = (struct librasch *) h;
  struct plugin_struct *p;
  meas_handle mh = NULL;
  struct ra_meas *meas = NULL;
  char file_use[MAX_PATH_RA];
  char *file_utf8;
  char *eval_file_utf8 = NULL;
  size_t len;

  if (!ra)
    return NULL;

  p = ra->pl_head;

  len = strlen (file);
  if (len == 0)
    {
      _ra_set_error (ra, RA_ERR_INFO_MISSING, "libRASCH");
      return NULL;
    }

  strncpy (file_use, file, MAX_PATH_RA);
  if (file_use[len - 1] == SEPARATOR)
    file_use[len - 1] = '\0';
  file_utf8 = malloc (len * 2);
  local_to_utf8 (file_use, file_utf8, strlen (file) * 2);
  if ((eval_file != NULL) && (eval_file[0] != '\0'))
    {
      eval_file_utf8 = malloc (strlen (eval_file) * 2);
      local_to_utf8 (eval_file, eval_file_utf8, strlen (eval_file) * 2);
    }

  while (p)
    {
      /* use plugin and (open_meas() is assigned) ? */
      if (!p->info.use_plugin || !p->access.open_meas
	  || !p->access.check_meas)
	{
	  p = p->next;
	  continue;
	}

      /* check if current plugin supports file */
      if ((*p->access.check_meas) (file_utf8))
	{
	  /* yes, correct plugin -> open it */
	  mh = (*p->access.open_meas) (h, file_utf8, fast);
	  if (mh)
	    {
	      meas = (struct ra_meas *) mh;
	      meas->handle_id = RA_HANDLE_MEAS;

	      /* save pointer to overall ra struct */
	      meas->ra = h;
	      /* and save pointer to handling plugin */
	      meas->p = p;
	      /* get max. samplerate and calc channel scaling factors */
	      calc_x_scales (meas);
	      /* read evaluation */
	      /* store given evaluation filename */
	      if ((eval_file != NULL) && (eval_file[0] != '\0'))
		strncpy (meas->eval.filename, eval_file_utf8, MAX_PATH_RA);
	      else
		meas->eval.filename[0] = '\0';

	      if (!fast)
		{
		  load_eval (mh, eval_file_utf8);	/* first load eval (if avail.) */
		  read_evaluation (mh);	/* then read original (if not alread in eval) */
		}

	      break;
	    }
	}

      /* try next plugin */
      p = p->next;
    }
  free (file_utf8);
  free (eval_file_utf8);

  if (meas == NULL)
    _ra_set_error (ra, RA_ERR_UNSUP_FORMAT, "libRASCH");

  return meas;
}				/* ra_meas_open() */


/**
 * \brief do scaling calculations
 * \param <meas> pointer to a measurement struct
 *
 * Function gets maximum samplerate used in a measurement and calculate for each
 * recording channel the scaling factor to the max. samplerate.
 */
int
calc_x_scales (struct ra_meas *meas)
{
  value_handle vh;
  rec_handle rh;
  int i;

  vh = ra_value_malloc ();
  rh = ra_rec_get_first (meas, 0);

  if (ra_info_get (rh, RA_INFO_REC_GEN_NUM_CHANNEL_L, vh) == 0)
    meas->num_ch = ra_value_get_long (vh);

  if (meas->num_ch <= 0)
    {
      meas->max_samplerate = 1.0;
      ra_value_free (vh);

      return 0;
    }

  meas->x_scale = malloc (sizeof (double) * meas->num_ch);
  memset (meas->x_scale, 0, sizeof (double) * meas->num_ch);

  meas->max_samplerate = 0.0;
  for (i = 0; i < meas->num_ch; i++)
    {
      ra_value_set_number (vh, i);
      if (ra_info_get (rh, RA_INFO_REC_CH_SAMPLERATE_D, vh) != 0)
	continue;

      meas->x_scale[i] = ra_value_get_double (vh);
      if (meas->max_samplerate < meas->x_scale[i])
	meas->max_samplerate = meas->x_scale[i];
    }
  ra_value_free (vh);

  for (i = 0; i < meas->num_ch; i++)
    meas->x_scale[i] /= meas->max_samplerate;

  return 0;
}				/* calc_x_scales() */


/**
 * \brief creates a new measurement
 * \param <h> handle to libRASCH
 * \param <dir> directory where the measurement-files should be stored
 * \param <name> name of the measurement
 *
 * Creates a new measurement with the name name in the directory dir. The function returns
 * the handle to the measurement. The measurement will be stored using the libRASCH file-format.
 *
 * Not full implemented. !!!
 */
LIBRAAPI meas_handle
ra_meas_new (ra_handle h, const char *dir, const char *name)
{
  struct librasch *ra = (struct librasch *) h;
  plugin_handle pl;
  struct plugin_struct *p;
  meas_handle mh = NULL;
  struct ra_meas *meas = NULL;
  char *dir_utf8;
  char *name_utf8;

  /* measurement will be handled by rasch-file plugin */
  pl = ra_plugin_get_by_name (h, "rasch-file", 0);
  if (!pl)
    return NULL;
  p = (struct plugin_struct *) pl;

  /* to be sure, check that new_meas() function exists in ra-raw plugin */
  if (!p->access.new_meas)
    {
      _ra_set_error (h, RA_ERR_PL_API_NOT_AVAIL, "libRASCH");
      return NULL;
    }

  dir_utf8 = malloc (strlen (dir) * 2);
  local_to_utf8 (dir, dir_utf8, strlen (dir) * 2);
  name_utf8 = malloc (strlen (name) * 2);
  local_to_utf8 (name, name_utf8, strlen (name) * 2);

  mh = (*p->access.new_meas) (dir_utf8, name_utf8);
  if (!mh)
    goto error;

  /* create measurement and set needed info */
  meas = (struct ra_meas *) mh;
  meas->handle_id = RA_HANDLE_MEAS;
  /* save pointer to overall lib struct */
  meas->ra = ra;
  /* and save pointer to handling plugin */
  meas->p = p;
  /* get max. samplerate and calc channel scaling factors */
  calc_x_scales (meas);

error:
  free (dir_utf8);
  free (name_utf8);

  return meas;
}				/* ra_meas_new() */



/**
 * \brief saves a measurement
 * \param <mh> handle to measurement
 *
 * Saves the changes to a measurement (NOT to the evaluation). Plugin must support this.
 *
 * Not full implemented. !!!
 */
LIBRAAPI int
ra_meas_save (meas_handle mh)
{
  /* (TODO: think about how to implement one save function (for measurement and evaluation)) */
  int ret = -1;
  unsigned short type = *((unsigned short *) mh);

  if (type == RA_HANDLE_MEAS)	/* to be sure that correct handle is used */
    {
      struct plugin_struct *p = ((struct ra_meas *) mh)->p;

      if (p->access.save_meas)
	ret = (*p->access.save_meas) (mh);
      else
	_ra_set_error (mh, RA_ERR_PL_API_NOT_AVAIL, "libRASCH");
    }
  else
    _ra_set_error (mh, RA_ERR_WRONG_HANDLE, "libRASCH");

  return ret;
}				/* ra_meas_save() */


/**
 * \brief close a measurement
 * \param <mh> handle to measurement
 *
 * Close a measurement.
 */
LIBRAAPI void
ra_meas_close (meas_handle mh)
{
  long l;
  struct ra_meas *meas = (struct ra_meas *) mh;
  struct plugin_struct *p = NULL;

  if (!mh)
    return;
  p = meas->p;

  free_eval_infos (&(meas->eval));

  /* is close_meas() assigned ? */
  if (p->access.close_meas)
    (*p->access.close_meas) (mh);

  if (meas->sessions)
    {
      for (l = 0; l < meas->num_sessions; l++)
	free_session_rec (meas->sessions[l].root_rec);
      ra_free_mem (meas->sessions);
      meas->num_sessions = 0;
    }

  if (meas->x_scale)
    free (meas->x_scale);

  ra_free_mem (meas);
}				/* ra_meas_close() */


/**
 * \brief get infos
 * \param <h> handle
 * \param <id> id of information wanted
 * \param <vh> value_handle receiving the info
 *
 * Returns 0 if the wanted information was found and != 0 if not. The information will be
 * in vh. For usage of the function please see user-manual.
 */
LIBRAAPI int
ra_info_get (any_handle h, int id, value_handle vh)
{
  unsigned short type;
  struct plugin_struct *p;

  /* in-validate value so in case of error 'vh' has no value */
  ra_value_reset (vh);

  if (!h || (id < 0) || !vh)
    return -1;

  if (check_handle_type (h, id) != 0)
    return -1;

  type = *((unsigned short *) h);

  if (type == RA_HANDLE_LIB)
    return get_lib_info ((ra_handle) h, id, vh);

  if (type == RA_HANDLE_PLUGIN)
    return get_plugin_info ((plugin_handle) h, id, vh);

  if ((type == RA_HANDLE_MEAS) && (id > RA_INFO_MEASUREMENT_START)
      && (id < RA_INFO_MEASUREMENT_END))
    return get_meas_general_info ((meas_handle) h, id, vh);

  if ((id > RA_INFO_EVALUATION) && (id < RA_INFO_EVALUATION_END))
    return eval_get_info (h, id, vh);

  if ((id > RA_INFO_SESSION_START) && (id < RA_INFO_SESSION_END))
    return get_session_info (h, id, vh);

  if ((id > RA_INFO_PROCESSING_START) && (id < RA_INFO_PROCESSING_END))
    return get_proc_info (h, id, vh);

  p = get_plugin_from_handle (h);
  if (!p)
    return -1;

  if (p->access.get_info_id)
    return (*p->access.get_info_id) (h, id, vh);
  else
    _ra_set_error (h, RA_ERR_PL_API_NOT_AVAIL, "libRASCH");

  return -1;
}				/* ra_info_get() */


/**
 * \brief check handle type
 * \param <h> any libRASCH handle
 * \param <id> name of plugin
 *
 * Function checks if the type handle 'h' is correct for the info-id 'id'.
 */
int
check_handle_type (any_handle h, int id)
{
  int ret = -1;
  unsigned short type = *((unsigned short *) h);

  switch (type)
    {
    case RA_HANDLE_LIB:
      if ((id > RA_INFO_LIB_START) && (id < RA_INFO_LIB_END))
	ret = 0;
      break;
    case RA_HANDLE_MEAS:
      if ((id > RA_INFO_MEASUREMENT_START) && (id < RA_INFO_OBJECT_END))
	ret = 0;
      break;
    case RA_HANDLE_REC:
      if ((id > RA_INFO_RECORDING_START) && (id < RA_INFO_RECORDING_END))
	ret = 0;
      break;
    case RA_HANDLE_FIND:
      if ((id > RA_INFO_MEASUREMENT_START) && (id < RA_INFO_OBJECT_END))
	ret = 0;
      break;
    case RA_HANDLE_EVAL:
      if ((id > RA_INFO_EVAL_START) && (id < RA_INFO_EVAL_END))
	ret = 0;
      break;
    case RA_HANDLE_EVAL_CLASS:
      if ((id > RA_INFO_CLASS_START) && (id < RA_INFO_CLASS_END))
	ret = 0;
      break;
    case RA_HANDLE_EVAL_PROP:
      if ((id > RA_INFO_PROP_START) && (id < RA_INFO_PROP_END))
	ret = 0;
      break;
    case RA_HANDLE_EVAL_SUMMARY:
      if ((id > RA_INFO_SUM_START) && (id < RA_INFO_SUM_END))
	ret = 0;
      break;
    case RA_HANDLE_PLUGIN:
      if ((id > RA_INFO_PLUGIN_START) && (id < RA_INFO_PLUGIN_END))
	ret = 0;
      break;
    case RA_HANDLE_PROC:
      if ((id > RA_INFO_PROCESSING_START) && (id < RA_INFO_PROCESSING_END))
	ret = 0;
      break;
    case RA_HANDLE_VIEW:
      _ra_set_error (h, RA_ERR_UNSUPPORTED, "libRASCH");
      break;
    default:
      _ra_set_error (h, RA_ERR_UNKNOWN_HANDLE, "libRASCH");
      break;
    }

  return ret;
}				/* check_handle_type() */


/**
 * \brief returns informations about the core
 * \param <ra> handle to library instance
 * \param <id> info id
 * \param <vh> value-handle (will receive info)
 *
 * Function returns informations about the library core.
 */
int
get_lib_info (ra_handle ra, int id, value_handle vh)
{
  int ret = 0;
  long cnt;
  struct plugin_struct *p;
  struct meta_info *meta_inf;
  char t[1000];

  /* get infos about asked measurement info */
  meta_inf = get_meta_info (id);

  if (!ra || !meta_inf || (id <= RA_INFO_LIB_START)
      || (id >= RA_INFO_LIB_END))
    {
      _ra_set_error (ra, RA_ERR_UNKNOWN_INFO, "libRASCH");
      return -1;
    }
  set_meta_info (vh, meta_inf->name, meta_inf->desc, meta_inf->id);

  switch (id)
    {
    case RA_INFO_NUM_PLUGINS_L:
      p = ((struct librasch *) ra)->pl_head;
      cnt = 0;
      while (p)
	{
	  cnt++;
	  p = p->next;
	}
      ra_value_set_long (vh, cnt);
      break;
    case RA_INFO_VERSION_C:
      sprintf (t, "%d.%d.%d%s", LIBRASCH_MAJ_VERSION, LIBRASCH_MIN_VERSION,
	       LIBRASCH_PATCH_LEVEL, LIBRASCH_EXTRA_VERSION);
      ra_value_set_string_utf8 (vh, t);
      break;
    default:
      _ra_set_error (ra, RA_ERR_UNKNOWN_INFO, "libRASCH");
      ret = -1;
      break;
    }

  return ret;
}				/* get_lib_info() */


/**
 * \brief returns informations about plugins
 * \param <pl> handle to plugin
 * \param <id> info id
 * \param <vh> value-handle (will receive info)
 *
 * Function returns informations about library plugins.
 */
int
get_plugin_info (plugin_handle pl, int id, value_handle vh)
{
  int ret = 0;
  struct ra_value *v = (struct ra_value *) vh;
  struct plugin_struct *p = (struct plugin_struct *) pl;
  struct meta_info *meta_inf;
  const char *c;

  /* get infos about asked measurement info */
  meta_inf = get_meta_info (id);

  if (!pl || !meta_inf || (id <= RA_INFO_PLUGIN_START)
      || (id >= RA_INFO_PLUGIN_END))
    {
      _ra_set_error (pl, RA_ERR_UNKNOWN_INFO, "libRASCH");
      return -1;
    }

  if (((id >= RA_INFO_PL_RES_NAME_C) && (id <= RA_INFO_PL_RES_DEFAULT_L))
      && ((v->number < 0) || (v->number >= p->info.num_results)))
    {
      _ra_set_error (pl, RA_ERR_UNKNOWN_INFO, "libRASCH");
      return -1;
    }

  if (((id >= RA_INFO_PL_OPT_NAME_C) && (id <= RA_INFO_PL_OPT_TYPE_L))
      && ((v->number < 0) || (v->number >= p->info.num_options)))
    {
      _ra_set_error (pl, RA_ERR_UNKNOWN_INFO, "libRASCH");
      return -1;
    }

  /* internal checks */
  if (((id >= RA_INFO_PL_RES_NAME_C) && (id <= RA_INFO_PL_RES_DEFAULT_L))
      && (p->info.res == NULL))
    {
      fprintf (stderr,
	       gettext ("critical error - get_plugin_info() in ra.c:\n"
			"tried to get result-infos but mem was not set\nplugin used: %s\n"),
	       p->info.name);
      exit (-1);
/*		_ra_set_error(pl, RA_ERR_INTERNAL_PLUGIN_ERROR, "libRASCH");
		return -1; */
    }
  if (((id >= RA_INFO_PL_OPT_NAME_C) && (id <= RA_INFO_PL_OPT_TYPE_L))
      && (p->info.opt == NULL))
    {
      fprintf (stderr,
	       gettext ("critical error - get_plugin_info() in ra.c:\n"
			"tried to get option-infos but mem was not set\nplugin used: %s\n"),
	       p->info.name);
      exit (-1);
/*		_ra_set_error(pl, RA_ERR_INTERNAL_PLUGIN_ERROR, "libRASCH");
		return -1; */
    }

  set_meta_info (vh, meta_inf->name, meta_inf->desc, meta_inf->id);

  switch (id)
    {
    case RA_INFO_PL_NAME_C:
      ra_value_set_string_utf8 (vh, p->info.name);
      break;
    case RA_INFO_PL_DESC_C:
      ra_value_set_string_utf8 (vh, p->info.desc);
      break;
    case RA_INFO_PL_FILE_C:
      ra_value_set_string_utf8 (vh, p->info.file);
      break;
    case RA_INFO_PL_USE_IT_L:
      ra_value_set_long (vh, p->info.use_plugin);
      break;
    case RA_INFO_PL_TYPE_L:
      ra_value_set_long (vh, p->info.type);
      break;
    case RA_INFO_PL_VERSION_C:
      ra_value_set_string_utf8 (vh, p->info.version);
      break;
    case RA_INFO_PL_BUILD_TS_C:
      ra_value_set_string_utf8 (vh, p->info.build_ts);
      break;
    case RA_INFO_PL_LICENSE_L:
      ra_value_set_long (vh, p->info.license);
      break;
    case RA_INFO_PL_NUM_OPTIONS_L:
      ra_value_set_long (vh, p->info.num_options);
      break;
    case RA_INFO_PL_OPT_NAME_C:
      c = p->info.opt[v->number].name;
      ra_value_set_string_utf8 (vh, (c[0] == '\0' ? "" : gettext (c)));
      break;
    case RA_INFO_PL_OPT_DESC_C:
      c = p->info.opt[v->number].desc;
      ra_value_set_string_utf8 (vh, (c[0] == '\0' ? "" : gettext (c)));
      break;
    case RA_INFO_PL_OPT_TYPE_L:
      ra_value_set_long (vh, p->info.opt[v->number].type);
      break;
    case RA_INFO_PL_NUM_RESULTS_L:
      ra_value_set_long (vh, p->info.num_results);
      break;
    case RA_INFO_PL_RES_NAME_C:
      c = p->info.res[v->number].name;
      ra_value_set_string_utf8 (vh, (c[0] == '\0' ? "" : gettext (c)));
      break;
    case RA_INFO_PL_RES_DESC_C:
      c = p->info.res[v->number].desc;
      ra_value_set_string_utf8 (vh, (c[0] == '\0' ? "" : gettext (c)));
      break;
    case RA_INFO_PL_RES_TYPE_L:
      ra_value_set_long (vh, p->info.res[v->number].type);
      break;
    case RA_INFO_PL_RES_DEFAULT_L:
      ra_value_set_long (vh, p->info.res[v->number].def);
      break;
    default:
      _ra_set_error (pl, RA_ERR_UNKNOWN_INFO, "libRASCH");
      ret = -1;
      break;
    }

  return ret;
}				/* get_plugin_info() */


/**
 * \brief returns informations about a measurement
 * \param <mh> handle to a measurement
 * \param <id> info id
 * \param <vh> value-handle (will receive info)
 *
 * Function returns informations about a measurement.
 */
int
get_meas_general_info (meas_handle mh, int id, value_handle vh)
{
  int ret = 0;
  struct ra_meas *meas = (struct ra_meas *) mh;
  struct ra_value *v = (struct ra_value *) vh;
  struct meta_info *meta_inf;

  /* get infos about asked measurement info */
  meta_inf = get_meta_info (id);

  if (!meas || !meta_inf || (id <= RA_INFO_MEASUREMENT_START)
      || (id >= RA_INFO_MEASUREMENT_END))
    {
      _ra_set_error (mh, RA_ERR_UNKNOWN_INFO, "libRASCH");
      return -1;
    }

  set_meta_info (vh, meta_inf->name, meta_inf->desc, meta_inf->id);

  if (id == RA_INFO_NUM_SESSIONS_L)
    ra_value_set_long (vh, meas->num_sessions);
  else if (id == RA_INFO_MAX_SAMPLERATE_D)
    ra_value_set_double (vh, meas->max_samplerate);
  else if (id == RA_INFO_CH_XSCALE_D)
    {
      if (v->number >= meas->num_ch)
	{
	  _ra_set_error (mh, RA_ERR_OUT_OF_RANGE, "libRASCH");
	  return -1;
	}

      ra_value_set_double (vh, meas->x_scale[v->number]);
    }
  else
    {
      struct plugin_struct *p = get_plugin_from_handle (mh);
      if (!p)
	return -1;

      if (p->access.get_info_id)
	ret = (*p->access.get_info_id) (mh, id, vh);
      else
	_ra_set_error (mh, RA_ERR_PL_API_NOT_AVAIL, "libRASCH");
    }

  return ret;
}				/* get_meas_general_info() */


/**
 * \brief returns informations about a session
 * \param <h> any handle belonging to a measurement
 * \param <id> info id
 * \param <vh> value-handle (will receive info)
 *
 * Function returns informations about a measurement session.
 */
int
get_session_info (any_handle h, int id, value_handle vh)
{
  int ret = 0;
  struct ra_meas *meas;
  struct ra_value *v = (struct ra_value *) vh;

  if ((id <= RA_INFO_SESSION_START) || (id >= RA_INFO_SESSION_END))
    return -1;

  meas = (struct ra_meas *) ra_meas_handle_from_any_handle (h);
  if (!meas)
    return -1;

  if (v->number >= meas->num_sessions)
    return -1;

  switch (id)
    {
    case RA_INFO_SES_NAME_C:
      ra_value_set_string_utf8 (vh, meas->sessions[v->number].name);
      break;
    case RA_INFO_SES_DESC_C:
      ra_value_set_string_utf8 (vh, meas->sessions[v->number].desc);
      break;
    default:
      ret = -1;
      break;
    }

  return ret;
}				/* get_session_info() */


/**
 * \brief returns informations about processing results
 * \param <h> any handle belonging to a processing handle
 * \param <id> info id
 * \param <vh> value-handle (will receive info)
 *
 * Function returns informations about processing results
 */
int
get_proc_info (any_handle h, int id, value_handle vh)
{
  int ret = 0;
  struct proc_info *proc;

  if ((id <= RA_INFO_PROCESSING_START) || (id >= RA_INFO_PROCESSING_END))
    return -1;

  proc = (struct proc_info *) h;
  if (!proc)
    return -1;

  switch (id)
    {
    case RA_INFO_PROC_NUM_RES_SETS_L:
      ra_value_set_long (vh, proc->num_result_sets);
      break;
    case RA_INFO_PROC_NUM_RES_L:
      ra_value_set_long (vh, proc->num_results);
      break;
    default:
      ret = -1;
      break;
    }

  return ret;
}				/* get_proc_info() */


/**
 * \brief returns plugin-handle
 * \param <h> handle which belongs to a plugin
 *
 * Function returns plugin-handle which is associated with the handle 'h'.
 */
struct plugin_struct *
get_plugin_from_handle (any_handle h)
{
  struct plugin_struct *p = NULL;
  unsigned short type = *((unsigned short *) h);

  switch (type)
    {
    case RA_HANDLE_MEAS:
      p = ((struct ra_meas *) h)->p;
      break;
    case RA_HANDLE_REC:
      p = ((struct ra_rec *) h)->meas->p;
      break;
    case RA_HANDLE_EVAL:
      p = ((struct eval *) h)->meas->p;
      break;
    case RA_HANDLE_EVAL_CLASS:
      p = ((struct eval_class *) h)->meas->p;
      break;
    case RA_HANDLE_EVAL_PROP:
      p = ((struct eval_property *) h)->meas->p;
      break;
    case RA_HANDLE_EVAL_SUMMARY:
      p = ((struct eval_summary *) h)->meas->p;
      break;
    }

  if (!p)
    _ra_set_error (h, RA_ERR_ERROR, "libRASCH");

  return p;
}				/* get_plugin_from_handle() */


/**
 * \brief get infos
 * \param <h> handle
 * \param <name> name (text string) of information wanted
 * \param <vh> value_handle receiving the info
 *
 * Returns 0 if the wanted information was found and != 0 if not. The information will be
 * in vh. For usage of the function please see user-manual.
 */
LIBRAAPI int
ra_info_get_by_name (any_handle h, const char *name, value_handle vh)
{
  char *name_utf8;

  name_utf8 = malloc (strlen (name) * 2);
  local_to_utf8 (name, name_utf8, strlen (name) * 2);

  if (find_ra_info_by_name (vh, name_utf8) != 0)
    {
      _ra_set_error (h, RA_ERR_UNKNOWN_INFO, "libRASCH");
      return -1;
    }

  free (name_utf8);

  return ra_info_get (h, ra_value_get_info (vh), vh);
}				/* ra_info_get_by_name() */


/**
 * \brief get infos
 * \param <h> measurement-handle
 * \param <info_type> type of information wanted
 * \param <idx> index of information wanted
 * \param <vh> value_handle receiving the info
 *
 * Returns 0 if the wanted information was found and != 0 if not. The information will be
 * in vh. For usage of the function please see user-manual.
 */
LIBRAAPI int
ra_info_get_by_idx (any_handle h, int info_type, int idx, value_handle vh)
{
  struct plugin_struct *p;

  /* in-validate value so in case of error 'vh' has no value */
  ra_value_reset (vh);

  if ((h == NULL) || (idx < 0) || (vh == NULL))
    {
      _ra_set_error (h, RA_ERR_UNKNOWN_INFO, "libRASCH");
      return -1;
    }

  p = get_plugin_from_handle (h);
  if (!p)
    return -1;

  if (p->access.get_info_idx)
    return (*p->access.get_info_idx) (h, info_type, idx, vh);
  else
    _ra_set_error (h, RA_ERR_PL_API_NOT_AVAIL, "libRASCH");

  return -1;
}				/* ra_info_get_by_idx() */


/**
 * \brief set info
 * \param <h> handle
 * \param <id> info id
 * \param <vh> value_handle with the info
 *
 * Set info 'id' with the data found in 'vh'. At the moment only some measurement
 * and recording infos can be set. In the future, other infos (e.g. eval-name,
 * eval-desc etc.) can be set with this function also.
 */
LIBRAAPI int
ra_info_set (any_handle h, int id, value_handle vh)
{
  unsigned short type;
  struct plugin_struct *p;
  int id_use;
  struct meta_info *meta_inf;

  if (!h || !vh)
    return -1;
  if (id == -1)
    id_use = ra_value_get_info (vh);
  else
    id_use = id;

  if ((id_use < 0) || (check_handle_type (h, id_use) != 0))
    {
      _ra_set_error (h, RA_ERR_OUT_OF_RANGE, "libRASCH");
      return -1;
    }

  type = *((unsigned short *) h);
  if ((type != RA_HANDLE_MEAS) && (type != RA_HANDLE_REC))
    {
      _ra_set_error (h, RA_ERR_UNSUPPORTED, "libRASCH");
      return -1;
    }

  p = get_plugin_from_handle (h);
  if (!p)
    {
      _ra_set_error (h, RA_ERR_UNKNOWN_PLUGIN, "libRASCH");
      return -1;
    }

  /* get infos about asked measurement info */
  meta_inf = get_meta_info (id_use);
  if (meta_inf == NULL)
    {
      _ra_set_error (h, RA_ERR_UNKNOWN_INFO, "libRASCH");
      return -1;
    }

  set_meta_info (vh, meta_inf->name, meta_inf->desc, meta_inf->id);

  if (p->access.set_info)
    return (*p->access.set_info) (h, id, vh);
  else
    _ra_set_error (h, RA_ERR_PL_API_NOT_AVAIL, "libRASCH");

  return -1;
}				/* ra_info_set() */


LIBRAAPI int
ra_info_set_by_name (any_handle h, const char *name, value_handle vh)
{
  char *name_utf8;

  name_utf8 = malloc (strlen (name) * 2);
  local_to_utf8 (name, name_utf8, strlen (name) * 2);

  if (find_ra_info_by_name (vh, name_utf8) != 0)
    {
      _ra_set_error (h, RA_ERR_UNKNOWN_INFO, "libRASCH");
      return -1;
    }

  return ra_info_set (h, ra_value_get_info (vh), vh);
}				/* ra_info_set_by_name() */


/**
 * \brief get root-recording
 * \param <mh> measurement-handle
 * \param <session> session number
 *
 * Returns the first recording-handle of session session. 
 */
LIBRAAPI rec_handle
ra_rec_get_first (meas_handle mh, long session)
{
  struct ra_meas *meas = (struct ra_meas *) mh;

  if (!meas || (session < 0) || (session > meas->num_sessions))
    {
      _ra_set_error (mh, RA_ERR_OUT_OF_RANGE, "libRASCH");
      return NULL;
    }

  return meas->sessions[session].root_rec;
}				/* ra_rec_get_first() */


/**
 * \brief get next recording
 * \param <rh> recording-handle
 *
 * Returns the next recording which follows recording rh.
 */
LIBRAAPI rec_handle
ra_rec_get_next (rec_handle rh)
{
  /* TODO: think about if "follows" is correct and insert link to user-manual (if written) */
  return ((struct ra_rec *) rh)->next;
}				/* ra_rec_get_next() */


/**
 * \brief get first sub-recording
 * \param <rh> recording-handle
 *
 * Returns the first child-recording of recording rh.
 */
LIBRAAPI rec_handle
ra_rec_get_first_child (rec_handle rh)
{
  return ((struct ra_rec *) rh)->child;
}				/* ra_rec_get_first_child() */


/**
 * \brief add a recording
 * \param <mh> handle of the measurement
 * \param <parent> handle of the parent recording (or NULL)
 *
 * Adds a recording to a measurement. The recording will be a child-recording
 * of the parent-recording parent. The recording-handle will be returned.
 *
 * Not implemented yet. !!!
 */
LIBRAAPI rec_handle
ra_rec_add (meas_handle mh, rec_handle parent)
{
  struct ra_meas *meas;
  struct plugin_struct *p;

  meas = (struct ra_meas *) mh;
  if (!meas)
    return 0;
  p = meas->p;
  if (!p)
    {
      _ra_set_error (mh, RA_ERR_ERROR, "libRASCH");
      return NULL;
    }

  if (p->access.rec_add)
    return (*p->access.rec_add) (meas, parent);
  else
    _ra_set_error (mh, RA_ERR_PL_API_NOT_AVAIL, "libRASCH");

  return NULL;
}				/* ra_rec_add() */


/**
 * \brief add a recording-device
 * \param <rh> handle of the recording
 *
 * Adds a device to the recording rh. The number of the device will be returned.
 *
 * Not implemented yet. !!!
 */
LIBRAAPI int
ra_dev_add (rec_handle rh)
{
  struct ra_meas *meas;
  struct plugin_struct *p;

  meas = (struct ra_meas *) ra_meas_handle_from_any_handle (rh);
  if (!meas)
    return 0;
  p = meas->p;
  if (!p)
    {
      _ra_set_error (rh, RA_ERR_ERROR, "libRASCH");
      return -1;
    }

  if (p->access.dev_add)
    return (*p->access.dev_add) (rh);
  else
    _ra_set_error (rh, RA_ERR_PL_API_NOT_AVAIL, "libRASCH");

  return -1;
}				/* ra_dev_add() */


/**
 * \brief add a recording-channel
 * \param <rh> handle of the recording
 *
 * Adds a channel to the recording rh. The number of the channel will be returned.
 *
 * Not implemented yet. !!!
 */
LIBRAAPI int
ra_ch_add (rec_handle rh)
{
  struct ra_meas *meas;
  struct plugin_struct *p;

  meas = (struct ra_meas *) ra_meas_handle_from_any_handle (rh);
  if (!meas)
    return 0;
  p = meas->p;
  if (!p)
    {
      _ra_set_error (rh, RA_ERR_ERROR, "libRASCH");
      return -1;
    }

  if (p->access.ch_add)
    return (*p->access.ch_add) (rh);
  else
    _ra_set_error (rh, RA_ERR_PL_API_NOT_AVAIL, "libRASCH");

  return -1;
}				/* ra_ch_add() */


/**
 * \brief start a new recording session
 * \param <mh> handle of the measurement
 *
 * Starts a new recording session and close the previous one. The function returns
 * the new session number.
 *
 * Not implemented yet. !!!
 */
LIBRAAPI int
ra_session_new (meas_handle mh)
{
  struct ra_meas *meas;
  struct plugin_struct *p;

  meas = (struct ra_meas *) mh;
  if (!meas)
    return 0;
  p = meas->p;
  if (!p)
    {
      _ra_set_error (mh, RA_ERR_ERROR, "libRASCH");
      return -1;
    }

  if (p->access.session_new)
    return (*p->access.session_new) (mh);
  else
    _ra_set_error (mh, RA_ERR_PL_API_NOT_AVAIL, "libRASCH");

  return -1;
}				/* ra_session_new() */


/**
 * \brief add raw data
 * \param <mh> handle of the measurement
 * \param <ch> channel where data should be added
 * \param <vh> data to be added
 *
 * Adds raw data to a measurement. The data is added to the current session.
 *
 * Not implemented yet. !!!
 */
LIBRAAPI size_t
ra_raw_add (meas_handle mh, unsigned int ch, value_handle vh)
{
  struct ra_meas *meas;
  struct plugin_struct *p;

  meas = (struct ra_meas *) mh;
  if (!meas)
    return 0;
  p = meas->p;
  if (!p)
    {
      _ra_set_error (mh, RA_ERR_ERROR, "libRASCH");
      return 0;
    }

  if (p->access.add_raw)
    return (*p->access.add_raw) (mh, ch, vh);
  else
    _ra_set_error (mh, RA_ERR_PL_API_NOT_AVAIL, "libRASCH");

  return 0;
}				/* ra_raw_add() */


/**
 * \brief get raw-signal data 
 * \param <rh> recording-handle
 * \param <ch> channel
 * \param <start> first sample
 * \param <num_data> number of samples
 * \param <data> buffer for samples
 * \param <data_high> buffer for higher 4 bytes of samples (if size of one sample > 4 bytes)
 *
 * This function returns the sample's (raw-signal data) from the measurement mh
 * and the recording rh. The first sample in data will be the sample number start
 * (in sampleunits) and max. num_data are in data. The memory for buffer data must 
 * be allocated in calling function. The samples returned in this function are the
 * samples as stored in the file. If the size of one sample is greater than 4 bytes, the upper
 * part of the samples are stored in the data_high buffer. If the size is not greater than
 * 4 bytes, the buffer is not needed.
 */
LIBRAAPI size_t
ra_raw_get (rec_handle rh, unsigned int ch, size_t start, size_t num_data,
	    DWORD * data, DWORD * data_high)
{
  struct ra_meas *meas;
  struct plugin_struct *p;

  meas = (struct ra_meas *) ra_meas_handle_from_any_handle (rh);
  if (!meas)
    return 0;
  p = meas->p;
  if (!p)
    {
      _ra_set_error (rh, RA_ERR_ERROR, "libRASCH");
      return 0;
    }

  if (p->access.get_raw)
    return (*p->access.get_raw) (meas, rh, ch, start, num_data, data,
				 data_high);
  else
    _ra_set_error (rh, RA_ERR_PL_API_NOT_AVAIL, "libRASCH");

  return -0;
}				/* ra_raw_get() */



/**
 * \brief get raw-signal data scaled to 'unit'-values
 * \param <rh> recording-handle
 * \param <ch> channel
 * \param <start> first sample
 * \param <num_data> number of samples
 * \param <data> buffer for samples
 *
 * This function returns the sample's (raw-signal data) from the measurement mh
 * and the recording rh. The first sample in data will be the sample number start
 * (in sampleunits) and max. num_data are in data. The memory for buffer data must 
 * be allocated in calling function. The values are scaled to the unit of the 
 * channel.
 */
LIBRAAPI size_t
ra_raw_get_unit (rec_handle rh, unsigned int ch, size_t start,
		 size_t num_data, double *data)
{
  struct ra_meas *meas;
  struct plugin_struct *p;

  meas = (struct ra_meas *) ra_meas_handle_from_any_handle (rh);
  if (!meas)
    return 0;
  p = meas->p;
  if (!p)
    {
      _ra_set_error (rh, RA_ERR_ERROR, "libRASCH");
      return 0;
    }

  if (p->access.get_raw_unit)
    return (*p->access.get_raw_unit) (meas, rh, ch, start, num_data, data);
  else
    _ra_set_error (rh, RA_ERR_PL_API_NOT_AVAIL, "libRASCH");

  return 0;
}				/* ra_raw_get_unit() */


/**
 * \brief shows GUI-element of a plugin (if avail.)
 * \param <h> processing-handle or view-handle
 * \param <pl> plugin-handle
 *
 * If the plugin pl provides a GUI-element, this function shows this GUI-element
 * and transfer control to it. The h variable contains some information, needed
 * by the plugin. For more information if a plugin provides a GUI-element and
 * what type/information is needed, please see the plugin specific documentation.
 */
LIBRAAPI int
ra_gui_call (any_handle h, plugin_handle pl)
{
  struct plugin_struct *p = pl;

  if (!p)
    {
      _ra_set_error (h, RA_ERR_ERROR, "libRASCH");
      return -1;
    }

  if (p->call_gui)
    return (*p->call_gui) (h);
  else
    _ra_set_error (h, RA_ERR_PL_API_NOT_AVAIL, "libRASCH");

  return -1;
}				/* ra_gui_call() */


/**
 * \brief get a processing handle
 * \param <pl> plugin-handle
 *
 * This function returns a proc_handle for the plugin pl. The proc_handle will be
 * returned initialized.
 */
LIBRAAPI proc_handle
ra_proc_get (meas_handle mh, plugin_handle pl,
	     void (*callback) (const char *, int))
{
  struct plugin_struct *p = pl;

  if (!p)
    return NULL;

  if (p->proc.get_proc_handle)
    {
      struct proc_info *proc = (*p->proc.get_proc_handle) (pl);
      if (proc)
	{
	  proc->ra = p->ra;
	  proc->mh = mh;
	  proc->callback = callback;
	}
      return (proc_handle) proc;
    }
  else
    _ra_set_error (pl, RA_ERR_PL_API_NOT_AVAIL, "libRASCH");

  return NULL;
}				/* ra_proc_get() */


/**
 * \brief free a processing handle
 * \param <proc> processing-handle
 *
 * This function frees a processing-handle and all associated memory.
 */
LIBRAAPI void
ra_proc_free (proc_handle proc)
{
  struct proc_info *pi = proc;
  struct plugin_struct *p;

  if (!pi || !pi->plugin)
    {
      _ra_set_error (proc, RA_ERR_ERROR, "libRASCH");
      return;
    }

  p = pi->plugin;
  if (!p)
    return;

  if (p->proc.free_proc_handle)
    (*p->proc.free_proc_handle) (proc);
  else
    _ra_set_error (proc, RA_ERR_PL_API_NOT_AVAIL, "libRASCH");
}				/* ra_proc_free() */


/**
 * \brief process the signal
 * \param <proc> processing-handle
 *
 * This function calls the "processing" function of a process-plugin. For more information
 * what information in proc is needed, please see the plugin specific documentation.
 */
LIBRAAPI int
ra_proc_do (proc_handle proc)
{
  struct proc_info *pi = proc;
  struct plugin_struct *p;

  if (!pi || !pi->mh || !pi->plugin)
    {
      _ra_set_error (proc, RA_ERR_ERROR, "libRASCH");
      return -1;
    }

  p = pi->plugin;

  if (p->proc.do_processing)
    return (*p->proc.do_processing) (proc);
  else
    _ra_set_error (proc, RA_ERR_PL_API_NOT_AVAIL, "libRASCH");

  return -1;
}				/* ra_proc_do() */


LIBRAAPI long
ra_proc_get_result_idx (proc_handle proc, const char *res_ascii_id)
{
  long l, res_idx;
  struct proc_info *pi = proc;
  struct plugin_struct *pl;

  if (!res_ascii_id || (res_ascii_id[0] == '\0') || !pi || !pi->plugin)
    {
      _ra_set_error (proc, RA_ERR_ERROR, "libRASCH");
      return -1;
    }

  pl = (struct plugin_struct *) pi->plugin;

  res_idx = -1;
  for (l = 0; l < pl->info.num_results; l++)
    {
      if (RA_STRICMP (pl->info.res[l].name, res_ascii_id) == 0)
	{
	  res_idx = l;
	  break;
	}
    }

  return res_idx;
}				/* ra_proc_get_result_idx() */


/**
 * \brief get the processing results
 * \param <proc> processing-handle
 * \param <res_num> number (zero-based index) of the wanted result
 * \param <res_set> number (zero-based index) of the wanted result-set
 * \param <vh> value-handle
 *
 * This function returns in vh one result from a processing perfomed with ra_proc_do.
 */
LIBRAAPI int
ra_proc_get_result (proc_handle proc, long res_num, long res_set,
		    value_handle vh)
{
  int ret;
  struct proc_info *pi = proc;
  struct plugin_struct *pl;
  value_handle *res;

  /* in-validate value so in case of error 'vh' has no value */
  ra_value_reset (vh);

  if (!pi || !pi->plugin)
    {
      _ra_set_error (proc, RA_ERR_ERROR, "libRASCH");
      return -1;
    }

  pl = (struct plugin_struct *) pi->plugin;

  if ((res_num < 0) || (res_num >= pi->num_results) || (pi->results == NULL)
      || (res_set < 0) || (res_set >= pi->num_result_sets))
    {
      char t[RA_VALUE_NAME_MAX];
#ifdef WIN32
      _snprintf (t, RA_VALUE_NAME_MAX - 1, "%s-%s", gettext ("plugin"),
		 pl->info.name);
#else
      snprintf (t, RA_VALUE_NAME_MAX - 1, "%s-%s", gettext ("plugin"),
		pl->info.name);
#endif /* WIN32 */
      _ra_set_error (proc, RA_ERR_OUT_OF_RANGE, t);
      return -1;
    }

  res = pi->results;
  ret = ra_value_copy (vh, res[res_num + (res_set * pi->num_results)]);

  return ret;
}				/* ra_proc_get_result() */


LIBRAAPI view_handle
ra_view_get (meas_handle mh, plugin_handle pl, void *parent)
{
  struct plugin_struct *p = pl;

  if (!p)
    return NULL;

  if (p->view.get_view_handle)
    {
      struct view_info *vi = (*p->view.get_view_handle) (pl);
      if (vi)
	{
	  vi->ra = p->ra;
	  vi->mh = mh;
	  vi->parent = parent;
	}
      return (view_handle) vi;
    }
  else
    _ra_set_error (pl, RA_ERR_PL_API_NOT_AVAIL, "libRASCH");

  return NULL;
}				/* ra_view_get() */


LIBRAAPI void
ra_view_free (view_handle vih)
{
  struct view_info *vi = vih;
  struct plugin_struct *p;

  if (!vi || !vi->plugin)
    {
      _ra_set_error (vih, RA_ERR_ERROR, "libRASCH");
      return;
    }

  p = vi->plugin;
  if (!p)
    return;

  if (p->view.free_view_handle)
    (*p->view.free_view_handle) (vih);
  else
    _ra_set_error (vih, RA_ERR_PL_API_NOT_AVAIL, "libRASCH");
}				/* ra_view_free() */


LIBRAAPI int
ra_view_create (view_handle vih)
{
  struct view_info *vi = vih;
  struct plugin_struct *p;

  if (!vi || !vi->plugin)
    {
      _ra_set_error (vih, RA_ERR_ERROR, "libRASCH");
      return -1;
    }

  p = vi->plugin;
  vi->ra = p->ra;

  if (p->view.create_view)
    return (*p->view.create_view) (vih);
  else
    _ra_set_error (vih, RA_ERR_PL_API_NOT_AVAIL, "libRASCH");

  return -1;
}				/* ra_view_create() */


LIBRAAPI void *
ra_view_get_window (view_handle vih, int win_number)
{
  struct view_info *vi = vih;

  if (!vi)
    {
      _ra_set_error (vih, RA_ERR_ERROR, "libRASCH");
      return NULL;
    }

  if (win_number >= vi->num_views)
    {
      _ra_set_error (vih, RA_ERR_OUT_OF_RANGE, "libRASCH");
      return NULL;
    }

  return vi->views[win_number];
}				/* ra_view_get_window() */


/**
 * \brief returns an option value
 * \param <h> return option from this object
 * \param <opt_name> name of the option
 * \param <vh> after function-call, contains option value
 *
 * The function tries to return the option opt_name from the object h.
 * At the moment only options for process-plugins (h needs to be a
 * process-handle) are supported.
 */
LIBRAAPI int
ra_lib_get_option (any_handle h, const char *opt_name, value_handle vh)
{
  int ret = -1;
  unsigned short type = *((unsigned short *) h);
  char *opt_name_utf8;

  /* in-validate value so in case of error 'vh' has no value */
  ra_value_reset (vh);

  if (!h)
    return ret;

  opt_name_utf8 = malloc (strlen (opt_name) * 2);
  local_to_utf8 (opt_name, opt_name_utf8, strlen (opt_name) * 2);

  /* up to now only options for process-plugins are supported */
  if (type == RA_HANDLE_PROC)
    {
      struct proc_info *pi;
      struct plugin_struct *pl;
      struct ra_option_infos *opt_infos;
      int opt_idx;
      long num_offset = 0;

      pi = (struct proc_info *) h;
      pl = (struct plugin_struct *) pi->plugin;

      opt_infos = (struct ra_option_infos *) pl->info.opt;
      opt_idx = find_option (opt_name_utf8, opt_infos, pl->info.num_options);
      if (opt_idx < 0)
	{
	  _ra_set_error (h, RA_ERR_OPTION_UNKNOWN, "libRASCH");
	  free (opt_name_utf8);
	  return ret;
	}

      if (opt_infos[opt_idx].num_offset != 0)
	num_offset =
	  opt_infos[opt_idx + opt_infos[opt_idx].num_offset].offset;

      if ((opt_infos[opt_idx].offset < 0) || (num_offset < 0))
	{
	  _ra_set_error (h, RA_ERR_OPTION_NO_OFFSET, "libRASCH");
	  free (opt_name_utf8);
	  return ret;
	}

      ret = get_option (pi->options, &(opt_infos[opt_idx]), vh, num_offset);
      if (ret != 0)
	_ra_set_error (h, RA_ERR_ERROR, "libRASCH");
    }
  else
    _ra_set_error (h, RA_ERR_UNSUPPORTED, "libRASCH");

  free (opt_name_utf8);

  return ret;
}				/* ra_lib_get_option() */


/**
 * \brief returns index of an option
 * \param <name> name of the option looked for
 * \param <infos> array of available options
 * \param <num_opt> number of the available options
 *
 * Functions looks in the array 'infos' for the option named 'name'.
 */
int
find_option (const char *name, struct ra_option_infos *infos, int num_opt)
{
  int i, idx = -1;

  for (i = 0; i < num_opt; i++)
    {
      if (strcmp (name, infos[i].name) == 0)
	{
	  idx = i;
	  break;
	}
    }

  return idx;
}				/* find_option() */


/**
 * \brief returns option value
 * \param <options> pointer to an option struct
 * \param <opt_info> option looked for
 * \param <vh> value-handle (will receive option value)
 * \param <num_offset> offset where number of array elements is stored (only for array values needed)
 *
 * Function returns the value for the option in 'vh'.
 */
int
get_option (void *options, struct ra_option_infos *opt_info, value_handle vh,
	    long num_offset)
{
  int ret = 0;
  unsigned char *struct_pos, *struct_pos_num;

  set_meta_info (vh, opt_info->name, opt_info->desc, RA_INFO_NONE);

  struct_pos = (unsigned char *) options + opt_info->offset;
  struct_pos_num = (unsigned char *) options + num_offset;

  switch (opt_info->type)
    {
    case RA_VALUE_TYPE_SHORT:
      ra_value_set_short (vh, *((short *) struct_pos));
      break;
    case RA_VALUE_TYPE_LONG:
      ra_value_set_long (vh, *((long *) struct_pos));
      break;
    case RA_VALUE_TYPE_DOUBLE:
      ra_value_set_double (vh, *((double *) struct_pos));
      break;
    case RA_VALUE_TYPE_CHAR:
      ra_value_set_string_utf8 (vh, *((char **) struct_pos));
      break;
    case RA_VALUE_TYPE_SHORT_ARRAY:
      ra_value_set_short_array (vh, (short *) struct_pos,
				*((short *) struct_pos_num));
      break;
    case RA_VALUE_TYPE_LONG_ARRAY:
      ra_value_set_long_array (vh, (long *) struct_pos,
			       *((long *) struct_pos_num));
      break;
    case RA_VALUE_TYPE_DOUBLE_ARRAY:
      ra_value_set_double_array (vh, (double *) struct_pos,
				 *((long *) struct_pos_num));
      break;
    case RA_VALUE_TYPE_CHAR_ARRAY:
      /* TODO: add functionality */
      break;
    default:
      ret = -1;
      break;
    }

  return ret;
}				/* get_option() */


/**
 * \brief set an option
 * \param <h> option will be set in this object
 * \param <opt_name> name of the option
 * \param <vh> option value
 *
 * The function set the option opt_name in object h.
 * At the moment only options for process-plugins (h needs to be a
 * process-handle) are supported.
 */
LIBRAAPI int
ra_lib_set_option (any_handle h, const char *opt_name, value_handle vh)
{
  int ret = -1;
  unsigned short type = *((unsigned short *) h);
  char *opt_name_utf8;

  opt_name_utf8 = malloc (strlen (opt_name) * 2);
  local_to_utf8 (opt_name, opt_name_utf8, strlen (opt_name) * 2);

  /* up to now only options for process- and view-plugins are supported */
  if (type == RA_HANDLE_PROC)
    {
      struct proc_info *pi;
      struct plugin_struct *pl;
      struct ra_option_infos *opt_infos;
      int opt_idx;
      long num_offset = 0;

      pi = (struct proc_info *) h;
      pl = (struct plugin_struct *) pi->plugin;

      opt_infos = (struct ra_option_infos *) pl->info.opt;
      opt_idx = find_option (opt_name_utf8, opt_infos, pl->info.num_options);
      if (opt_idx < 0)
	{
	  _ra_set_error (h, RA_ERR_OPTION_UNKNOWN, "libRASCH");
	  free (opt_name_utf8);
	  return ret;
	}

      if (opt_infos[opt_idx].num_offset != 0)
	num_offset =
	  opt_infos[opt_idx + opt_infos[opt_idx].num_offset].offset;

      if ((opt_infos[opt_idx].offset < 0) || (num_offset < 0))
	{
	  _ra_set_error (h, RA_ERR_OPTION_NO_OFFSET, "libRASCH");
	  free (opt_name_utf8);
	  return ret;
	}

      ret = set_option (pi->options, &(opt_infos[opt_idx]), vh, num_offset);
      if (ret != 0)
	_ra_set_error (h, RA_ERR_ERROR, "libRASCH");
    }
  else if (type == RA_HANDLE_VIEW)
    {
      struct view_info *vi;
      struct plugin_struct *pl;
      struct ra_option_infos *opt_infos;
      int opt_idx;
      long num_offset = 0;

      vi = (struct view_info *) h;
      pl = (struct plugin_struct *) vi->plugin;

      opt_infos = (struct ra_option_infos *) pl->info.opt;
      opt_idx = find_option (opt_name_utf8, opt_infos, pl->info.num_options);
      if (opt_idx < 0)
	{
	  _ra_set_error (h, RA_ERR_OPTION_UNKNOWN, "libRASCH");
	  free (opt_name_utf8);
	  return ret;
	}

      if (opt_infos[opt_idx].num_offset != 0)
	num_offset =
	  opt_infos[opt_idx + opt_infos[opt_idx].num_offset].offset;

      if ((opt_infos[opt_idx].offset < 0) || (num_offset < 0))
	{
	  _ra_set_error (h, RA_ERR_OPTION_NO_OFFSET, "libRASCH");
	  free (opt_name_utf8);
	  return ret;
	}

      ret = set_option (vi->options, &(opt_infos[opt_idx]), vh, num_offset);
      if (ret != 0)
	_ra_set_error (h, RA_ERR_ERROR, "libRASCH");
    }
  else
    _ra_set_error (h, RA_ERR_UNSUPPORTED, "libRASCH");

  free (opt_name_utf8);

  return ret;
}				/* ra_lib_set_option() */


/**
 * \brief set option value
 * \param <options> pointer to an option struct
 * \param <opt_info> used option
 * \param <vh> value-handle (has value which should be set)
 * \param <num_offset> offset where number of array elements will be set (only for array values needed)
 *
 * Function set the value for the option given in 'vh'.
 */
int
set_option (void *options, struct ra_option_infos *opt_info, value_handle vh,
	    long num_offset)
{
  int ret = 0;
  unsigned char *struct_pos, *struct_pos_num;
  long n_elem;
  char **c_arr;
  const char *c_temp;
  short **s_arr;
  const short *s_temp;
  long **l_arr;
  const long *l_temp;
  double **d_arr;
  const double *d_temp;
  void **vp_arr;
  const void *vp_temp;

  struct_pos = (unsigned char *) options + opt_info->offset;
  struct_pos_num = (unsigned char *) options + num_offset;

  n_elem = ra_value_get_num_elem (vh);
  switch (opt_info->type)
    {
    case RA_VALUE_TYPE_SHORT:
      *((short *) struct_pos) = ra_value_get_short (vh);
      break;
    case RA_VALUE_TYPE_LONG:
      *((long *) struct_pos) = ra_value_get_long (vh);
      break;
    case RA_VALUE_TYPE_DOUBLE:
      *((double *) struct_pos) = ra_value_get_double (vh);
      break;
    case RA_VALUE_TYPE_CHAR:
      c_arr = (char **) struct_pos;
      if (*c_arr)
	ra_free_mem (*c_arr);
      c_temp = ra_value_get_string_utf8 (vh);
      *c_arr = ra_alloc_mem (strlen (c_temp) + 1);
      strcpy (*c_arr, c_temp);
      break;
    case RA_VALUE_TYPE_VOIDP:
      *((void **) struct_pos) = (void *) ra_value_get_voidp (vh);
      break;
    case RA_VALUE_TYPE_SHORT_ARRAY:
      s_arr = (short **) struct_pos;
      if (*s_arr)
	ra_free_mem (*s_arr);
      *s_arr = ra_alloc_mem (sizeof (short) * n_elem);
      s_temp = ra_value_get_short_array (vh);
      memcpy (*s_arr, s_temp, sizeof (short) * n_elem);
      *((short *) struct_pos_num) = (short) n_elem;
      break;
    case RA_VALUE_TYPE_LONG_ARRAY:
      l_arr = (long **) struct_pos;
      if (*l_arr)
	ra_free_mem (*l_arr);
      *l_arr = ra_alloc_mem (sizeof (long) * n_elem);
      l_temp = ra_value_get_long_array (vh);
      memcpy (*l_arr, l_temp, sizeof (long) * n_elem);
      *((long *) struct_pos_num) = n_elem;
      break;
    case RA_VALUE_TYPE_DOUBLE_ARRAY:
      d_arr = (double **) struct_pos;
      if (*d_arr)
	ra_free_mem (*d_arr);
      *d_arr = ra_alloc_mem (sizeof (double) * n_elem);
      d_temp = ra_value_get_double_array (vh);
      memcpy (*d_arr, d_temp, sizeof (double) * n_elem);
      *((long *) struct_pos_num) = n_elem;
      break;
    case RA_VALUE_TYPE_CHAR_ARRAY:
      /* TODO: implement functionality */
      break;
    case RA_VALUE_TYPE_VOIDP_ARRAY:
      vp_arr = (void **) struct_pos;
      if (*vp_arr)
	ra_free_mem (*vp_arr);
      *vp_arr = ra_alloc_mem (sizeof (void *) * n_elem);
      vp_temp = ra_value_get_voidp_array (vh);
      memcpy (*vp_arr, vp_temp, sizeof (void *) * n_elem);
      *((long *) struct_pos_num) = n_elem;
      break;
    default:
      ret = -1;
      break;
    }

  return ret;
}				/* set_option() */


/**
 * \brief copy measurement
 * \param <mh> handle of measurement which should be copied
 * \param <dest_dir> destination directory
 *
 * The function copy the files of measurement mh to directory dest_dir.
 *
 * IMPORTANT!!!
 * Function _not_ completely tested. Use at your own risk.
 */
LIBRAAPI int
ra_meas_copy (meas_handle mh, const char *dest_dir)
{
  char dir_utf8[MAX_PATH_RA];
  local_to_utf8 (dest_dir, dir_utf8, MAX_PATH_RA);

  return copy_move_meas (mh, dir_utf8, 1);
}				/* ra_meas_copy() */


/**
 * \brief move measurement
 * \param <mh> handle of measurement which should be moved
 * \param <dest_dir> destination directory
 *
 * The function move the files of measurement mh to directory dest_dir. After the move
 * the measurement will be reopend (to be able to handle the new positions of the files)
 * and the functions returns the new measurement-handle.
 *
 * IMPORTANT!!!
 * Function _not_ completely tested. Use at your own risk.
 */
LIBRAAPI meas_handle
ra_meas_move (meas_handle mh, const char *dest_dir)
{
  int ret;
  struct ra_meas *meas = (struct ra_meas *) mh;
  meas_handle mh_moved = mh;
  ra_handle ra;
  value_handle vh;
  const char *fn;
  char moved_meas[MAX_PATH_RA];
  char eval_filename[MAX_PATH_RA];
  char *p;
  char dir_utf8[MAX_PATH_RA];

  /* save name of evaluation file */
  strncpy (eval_filename, meas->eval.filename, MAX_PATH_RA);

  /* get infos so I'm able to open moved measuerment */
  ra = ra_lib_handle_from_any_handle (mh);
  vh = ra_value_malloc ();
  ra_info_get (mh, RA_INFO_PATH_C, vh);
  fn = ra_value_get_string (vh);

  p = strrchr (fn, SEPARATOR);
  if (p)
    sprintf (moved_meas, "%s%c%s", dest_dir, SEPARATOR, p + 1);
  else
    sprintf (moved_meas, "%s%c%s", dest_dir, SEPARATOR, fn);

  ra_value_free (vh);

  local_to_utf8 (dest_dir, dir_utf8, MAX_PATH_RA);

  /* move measurement */
  ret = copy_move_meas (mh, dir_utf8, 0);

  /* open moved measurement */
  if (ret == 0)
    {
      ra_meas_close (mh);
      mh_moved = ra_meas_open (ra, moved_meas, eval_filename, 0);
    }

  return mh_moved;
}				/* ra_meas_move() */


/**
 * \brief do a copy or move of a measurement
 * \param <mh> measurement-handle
 * \param <dest_dir> destination directory
 * \param <copy> flag if a copy (!= 0) or a move (== 0) should be done
 *
 * Function copy or move a measurement.
 */
int
copy_move_meas (meas_handle mh, const char *dest_dir, int copy)
{
  int ret = 0;
  value_handle vh;
  const char *fn, **files;
  char dir[MAX_PATH_RA], dir_save[MAX_PATH_RA];
  long l, n_elem, is_dir;

  vh = ra_value_malloc ();

  dir_save[0] = '\0';

  /* first check if dir */
  if (ra_info_get (mh, RA_INFO_DIR_L, vh) != 0)
    {
      ra_value_free (vh);
      return -1;
    }
  is_dir = ra_value_get_long (vh);
  if (is_dir)
    {
      /* yes, measurement is in a directory -> create dir */
      char *p = NULL;
      char temp[MAX_PATH_RA];

      /* build directory name */
      if (ra_info_get (mh, RA_INFO_PATH_C, vh) != 0)
	{
	  ra_value_free (vh);
	  return -1;
	}
      fn = ra_value_get_string (vh);
      strncpy (dir_save, fn, MAX_PATH_RA);
      strncpy (temp, fn, MAX_PATH_RA);
      p = strrchr (temp, SEPARATOR);
      if (p)
	{
	  *p = '\0';
	  p++;
	}
      else
	p = temp;

      sprintf (dir, "%s%c%s", dest_dir, SEPARATOR, p);

      /* make it */
      ret = make_dir (dir);
      if (ret != 0)
	{
	  ra_value_free (vh);
	  return ret;
	}
    }
  else
    strncpy (dir, dest_dir, MAX_PATH_RA);

  if (ra_info_get (mh, RA_INFO_FILES_CA, vh) != 0)
    {
      ra_value_free (vh);
      return -1;
    }

  files = ra_value_get_string_array (vh);
  if (!files)
    {
      ra_value_free (vh);
      return -1;
    }

  n_elem = ra_value_get_num_elem (vh);
  for (l = 0; l < n_elem; l++)
    {
      ret = copy_move_file (files[l], dir, copy);
      if (ret != 0)
	break;
    }

  ra_value_free (vh);

  if (dir_save[0] && !copy)
    delete_file (dir_save);

  return ret;
}				/* copy_move_meas() */


/**
 * \brief delete measurement
 * \param <mh> handle of measurement which should be deleted
 *
 * The function deletes the files of measurement mh. The measurement mh
 * will be closed after the deletion. Therefore mh is no longer a valid
 * measurement-handle.
 *
 * IMPORTANT!!!
 * Function _not_ completely tested. Use at your own risk.
 */
LIBRAAPI int
ra_meas_delete (meas_handle mh)
{
  value_handle vh;
  int ret = 0;
  char dir[MAX_PATH_RA];
  long l, n_elem;
  const char **files;

  vh = ra_value_malloc ();

  /* check if dir */
  dir[0] = '\0';
  if (ra_info_get (mh, RA_INFO_DIR_L, vh) != 0)
    {
      ra_value_free (vh);
      return -1;
    }
  if (ra_value_get_long (vh))
    {
      /* yes, measurement is in a directory -> save dir-name */
      if (ra_info_get (mh, RA_INFO_PATH_C, vh) != 0)
	{
	  ra_value_free (vh);
	  return -1;


	}
      strncpy (dir, ra_value_get_string (vh), MAX_PATH_RA);
    }

  /* get files of measurement */
  if (ra_info_get (mh, RA_INFO_FILES_CA, vh) != 0)
    {
      ra_value_free (vh);
      return -1;
    }

  /* close measurement */
  ra_meas_close (mh);

  /* delete files */
  files = ra_value_get_string_array (vh);
  if (!files)
    {
      ra_value_free (vh);
      return -1;
    }

  n_elem = ra_value_get_num_elem (vh);
  for (l = 0; l < n_elem; l++)
    {
      ret = delete_file (files[l]);
      if (ret != 0)
	break;
    }
  ra_value_free (vh);

  /* if measurement in dir -> remove dir */
  if (dir[0])
    delete_file (dir);

  return ret;
}				/* ra_meas_delete() */
