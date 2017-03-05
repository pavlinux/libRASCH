/**
 * \file eval.c
 *
 * This file provides the part of the API to access evaluations.
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
#include <errno.h>
#include <time.h>
#include <assert.h>
#include <ctype.h>

#ifndef WIN32
#include <unistd.h>
#endif

#define _LIBRASCH_BUILD
#include <ra_eval.h>
#include <pl_general.h>
#include <ra_linked_list.h>
#include "eval.h"


LIBRAAPI int
ra_eval_edit_start (any_handle h)
{
  if (!h)
    return -1;
  else
    return 0;
}				/* ra_eval_edit_start() */


LIBRAAPI int
ra_eval_edit_complete (any_handle h)
{
  if (!h)
    return -1;
  else
    return 0;
}				/* ra_eval_edit_complete() */


LIBRAAPI int
ra_eval_edit_cancel (any_handle h)
{
  if (!h)
    return -1;
  else
    return 0;
}				/* ra_eval_edit_cancel() */


/* ------------------------------ attribute functions ------------------------------ */

/**
 * \brief get from 'h' a list of available attributes
 * \param <h> an eval-/event-class-/event-prop-handle
 * \param <vh> value-handle receiving the attribute list
 *
 * This function returns a list of all attributes associated to handle 'h'.
 */
LIBRAAPI int
ra_eval_attribute_list (any_handle h, value_handle vh)
{
  int ret = 0;
  int i, num;
  struct eval_head *head = (struct eval_head *) h;
  char **names;
  struct eval_attribute *curr;

  if (h == NULL)
    return -1;

  _ra_set_error (h, RA_ERR_NONE, "libRASCH");
  ra_value_reset (vh);

  num = 0;
  names = NULL;
  curr = head->meas->eval.attribute;
  while (curr)
    {
      if (curr->src_handle == h)
	{
	  num++;
	  names = realloc (names, sizeof (char *) * num);

	  names[num - 1] = (char *) malloc (sizeof (char) * MAX_ATTRIB_LEN);
	  strncpy (names[num - 1], curr->name, MAX_ATTRIB_LEN);
	}
      curr = curr->next;
    }

  if (num > 0)
    {
      ra_value_set_string_array (vh, (const char **) names, num);

      for (i = 0; i < num; i++)
	free (names[i]);
      free (names);
    }

  return ret;
}				/* ra_eval_attribute_list() */


/**
 * \brief get from 'h' the value of the attribute 'name'
 * \param <h> an eval-/event-class-/event-prop-handle
 * \param <id> ASCII-id of the attribute
 * \param <vh> value-handle receiving the value
 *
 * This function returns the value stored in the attribute 'name'.
 */
LIBRAAPI int
ra_eval_attribute_get (any_handle h, const char *id, value_handle vh)
{
  int ret = -1;
  struct eval_head *head = (struct eval_head *) h;
  struct eval_attribute *curr;

  if (h == NULL)
    return -1;

  _ra_set_error (h, RA_ERR_NONE, "libRASCH");
  ra_value_reset (vh);

  if ((curr = find_attribute (head, id)) == NULL)
    {
      char t[1000];
      sprintf (t, "libRASCH - ra_eval_attribute_get() '%s'", id);
      /* TODO: check error code */
      _ra_set_error (h, RA_ERR_ERROR, t);
/* 		assert(0); */
      return -1;
    }

  ret = ra_value_copy (vh, curr->value);

  return ret;
}				/* ra_eval_attribute_get() */


/**
 * \brief set the value of the attribute 'name' in 'h'
 * \param <h> an eval-/event-class-/event-prop-handle
 * \param <id> ASCII-id of the attribute
 * \param <vh> value-handle with the value
 *
 * This function sets the value of the attribute 'name'.
 */
LIBRAAPI int
ra_eval_attribute_set (any_handle h, const char *id, value_handle vh)
{
  int ret = -1;
  struct eval_head *head = (struct eval_head *) h;
  struct eval_attribute *curr;

  if (h == NULL)
    return -1;

  _ra_set_error (h, RA_ERR_NONE, "libRASCH");

  if ((curr = find_attribute (head, id)) == NULL)
    {
      curr =
	(struct eval_attribute *) calloc (1, sizeof (struct eval_attribute));
      ra_list_add ((void **) &(head->meas->eval.attribute), curr);

      curr->src_handle = h;
      strncpy (curr->name, id, MAX_ATTRIB_LEN - 1);
      curr->value = ra_value_malloc ();
    }

  ret = ra_value_copy (curr->value, vh);

  return ret;
}				/* ra_eval_attribute_set() */


/**
 * \brief removes the attribute 'name' in 'h'
 * \param <h> an eval-/event-class-/event-prop-handle
 * \param <id> ASCII-id of the attribute
 *
 * This function removes the attribute 'name'.
 */
LIBRAAPI int
ra_eval_attribute_unset (any_handle h, const char *id)
{
  int ret = -1;
  struct eval_head *head = (struct eval_head *) h;
  struct eval_attribute *curr;

  if (h == NULL)
    return -1;

  _ra_set_error (h, RA_ERR_NONE, "libRASCH");

  if ((curr = find_attribute (head, id)) == NULL)
    {
      /* TODO: check error code */
      _ra_set_error (h, RA_ERR_ERROR, "libRASCH");
      return -1;
    }

  ra_value_free (curr->value);
  ret = ra_list_del ((void **) &(head->meas->eval.attribute), curr);

  return ret;
}				/* ra_eval_attribute_unset() */


struct eval_attribute *
find_attribute (struct eval_head *h, const char *id)
{
  struct eval_attribute *curr;

  curr = h->meas->eval.attribute;
  while (curr)
    {
      if ((curr->src_handle == h)
	  && (strncmp (curr->name, id, MAX_ATTRIB_LEN) == 0))
	break;

      curr = curr->next;
    }

  return curr;
}				/* find_attribute() */


int
delete_attributes (struct eval_head *h)
{
  struct eval_attribute *a;

  a = h->meas->eval.attribute;
  while (a)
    {
      if (a->src_handle == h)
	{
	  ra_value_free (a->value);
	  ra_list_del ((void **) &(h->meas->eval.attribute), a);
	  free (a);
	  a = h->meas->eval.attribute;
	  continue;
	}

      a = a->next;
    }

  return 0;
}				/* delete_attributes() */


/* ------------------------------ evaluation functions ------------------------------ */


/**
 * \brief adds an evaluation
 * \param <mh> measurement-handle
 * \param <name> a short name of the evaluation
 * \param <desc> a short description of the evaluation
 * \param <original> flag if evaluation is the original evaluation
 *
 * This function adds an evaluation to a measurement.
 */
LIBRAAPI eval_handle
ra_eval_add (meas_handle mh, const char *name, const char *desc, int original)
{
  struct ra_meas *meas = (struct ra_meas *) mh;
  struct eval *e = NULL;
  char text[EVAL_MAX_DESC];
  struct tm *t;
  time_t tt;
  value_handle vh = NULL;

  if (!mh)
    return NULL;

  _ra_set_error (mh, RA_ERR_NONE, "libRASCH");

  if (meas->handle_id != RA_HANDLE_MEAS)
    {
      _ra_set_error (mh, RA_ERR_WRONG_HANDLE, "libRASCH");
      return NULL;
    }

  /* if the new eval should be the original check if not already an original one is available */
  if (original)
    {
      e = ra_eval_get_original (mh);
      if (e)
	{
	  _ra_set_error (mh, RA_ERR_EVAL_WRONG_TYPE, "libRASCH");
	  return NULL;
	}
    }

  e = (struct eval *) calloc (1, sizeof (struct eval));
  ra_list_add ((void **) &(meas->eval.evaluations), e);

  e->handle_id = RA_HANDLE_EVAL;
  e->meas = meas;
  e->original = original;

  time (&tt);
  t = gmtime (&tt);
  sprintf (text, "%02d.%02d.%4d  %02d:%02d:%02d", t->tm_mday,
	   t->tm_mon + 1, (t->tm_year + 1900), t->tm_hour, t->tm_min,
	   t->tm_sec);

  vh = ra_value_malloc ();
  ra_value_set_string (vh, text);
  if (ra_eval_attribute_set (e, "add-timestamp", vh) != 0)
    goto error;
  if (ra_eval_attribute_set (e, "modify-timestamp", vh) != 0)
    goto error;

  if (set_env (e) != 0)
    goto error;

  /* set the new evaluation as the default evaluation */
  if (ra_eval_set_default (e) != 0)
    goto error;

  if (name)
    {
      ra_value_set_string (vh, name);
      if (ra_eval_attribute_set (e, "name", vh) != 0)
	goto error;
    }
  if (desc)
    {
      ra_value_set_string (vh, desc);
      if (ra_eval_attribute_set (e, "description", vh) != 0)
	goto error;
    }
  ra_value_free (vh);

  return e;

error:
  if (e)
    ra_list_del ((void **) &(meas->eval.evaluations), e);
  if (vh)
    ra_value_free (vh);

  return NULL;
}				/* ra_eval_add() */



/**
 * \brief set environment informations in evaluation
 * \param <e> pointer to evaluation structure
 *
 * The function sets in an evaluation the following environment values:
 * hostname, user and program using the current libRASCH instance. There are
 * different implementations for Win32 and Linux (selected with #ifdef's).
 */
#ifdef LINUX
int
set_env (struct eval *e)
{
  char t[EVAL_MAX_DESC];
  char fn[MAX_PATH_RA];
  FILE *f;
  value_handle vh;

  vh = ra_value_malloc ();

  gethostname (t, EVAL_MAX_HOST);
  if (t[0] != '\0')
    {
      ra_value_set_string (vh, t);
      ra_eval_attribute_set (e, "add-host", vh);
    }

  if (getenv ("LOGNAME") != NULL)
    {
      ra_value_set_string (vh, getenv ("LOGNAME"));
      ra_eval_attribute_set (e, "add-user", vh);
    }

  sprintf (fn, "/proc/%d/status", getpid ());
  f = fopen (fn, "r");
  if (f != NULL)
    {
      char *p;

      fgets (t, 200, f);
      fclose (f);

      p = strchr (t, '\t');
      if ((p != NULL) && (p + 1 != '\0'))
	{
	  char *newline;

	  p++;
	  newline = strchr (p, '\n');
	  if (newline)
	    *newline = '\0';
	  ra_value_set_string (vh, p);
	  ra_eval_attribute_set (e, "add-program", vh);
	}
    }

  ra_value_free (vh);

  return 0;
}				/* set_env() */

#elif WIN32

int
set_env (struct eval *e)
{
  char t[EVAL_MAX_DESC];
  char full_path[MAX_PATH_RA];
  char *pos, *cmd;
  DWORD size;
  value_handle vh;

  vh = ra_value_malloc ();

  size = EVAL_MAX_DESC;
  GetComputerName (t, &size);
  ra_value_set_string (vh, t);
  ra_eval_attribute_set (e, "add-host", vh);
  size = EVAL_MAX_DESC;
  GetUserName (t, &size);
  ra_value_set_string (vh, t);
  ra_eval_attribute_set (e, "add-user", vh);

  memset (full_path, 0, MAX_PATH_RA);
  cmd = GetCommandLine ();
  /* first check if first argument is in qoutes */
  if (cmd[0] == '\"')
    {
      /* ok, now find "closing" quote */
      pos = strchr (cmd + 1, '\"');
      if (pos != NULL)
	strncpy (full_path, cmd + 1, pos - cmd - 1);
      else
	{
	  /* at the moment I do not know what to do if the
	     closing quote can not be found */
	  ;
	}
    }
  else
    {
      pos = strchr (cmd, ' ');
      if (pos == NULL)
	strncpy (full_path, cmd, MAX_PATH_RA);
      else
	strncpy (full_path, cmd, pos - cmd);
    }
  pos = strrchr (full_path, '\\');
  if (pos == NULL)
    pos = full_path;
  else
    pos++;
  strncpy (t, pos, EVAL_MAX_PRG);
  pos = strchr (t, '\"');
  if (pos != NULL)
    *pos = '\0';
  ra_value_set_string (vh, t);
  ra_eval_attribute_set (e, "add-program", vh);

  ra_value_free (vh);

  return 0;
}				/* set_env() */

#else /* no matching system found -> do nothing */

int
set_env (struct eval *e)
{
  return 0;
}				/* set_env() */
#endif


/**
 * \brief copy evaluation
 * \param <eh> source evaluation handle
 * \param <name> name of the copied evaluation 
 * \param <desc> description of the copied evaluation
 *
 * The function copies the evaluation given by 'eh', the handle of the
 * copy will be returned. If the parameter "name" is NULL or an empty string,
 * the name of the source evaluation will be used and the prefix 'copy-' will
 * be added.
 *
 */
LIBRAAPI eval_handle
ra_eval_copy (eval_handle eh, const char *name, const char *desc)
{
  unsigned long l;
  value_handle vh, vh2;
  struct eval *e_src = (struct eval *) eh;
  struct eval *e_dest = NULL;
  const char **attribs;

  if (!eh)
    return NULL;

  _ra_set_error (eh, RA_ERR_NONE, "libRASCH");

  if (e_src->handle_id != RA_HANDLE_EVAL)
    {
      _ra_set_error (eh, RA_ERR_WRONG_HANDLE, "libRASCH");
      return NULL;
    }


  e_dest = (struct eval *) calloc (1, sizeof (struct eval));
  ra_list_add ((void **) &(e_src->meas->eval.evaluations), e_dest);

  e_dest->handle_id = RA_HANDLE_EVAL;
  e_dest->meas = e_src->meas;

  /* copy attributes */
  vh = ra_value_malloc ();
  ra_eval_attribute_list (e_src, vh);
  vh2 = ra_value_malloc ();
  attribs = ra_value_get_string_array (vh);
  for (l = 0; l < ra_value_get_num_elem (vh); l++)
    {
      ra_eval_attribute_get (e_src, attribs[l], vh2);
      if (strcmp (attribs[l], "name") == 0)
	{
	  char *buf;
	  buf = malloc (strlen (ra_value_get_string (vh2)) + 5);
	  sprintf (buf, "copy-%s", ra_value_get_string (vh2));
	  ra_value_set_string (vh2, buf);
	  free (buf);
	}
      ra_eval_attribute_set (e_dest, attribs[l], vh2);
    }
  ra_value_free (vh2);

  /* set name and description if given */
  if (name)
    {
      ra_value_set_string (vh, name);
      if (ra_eval_attribute_set (e_dest, "name", vh) != 0)
	goto error;
    }
  if (desc)
    {
      ra_value_set_string (vh, desc);
      if (ra_eval_attribute_set (e_dest, "description", vh) != 0)
	goto error;
    }

  /* copy event-classes */

  /* copy event-summaries */

  ra_value_free (vh);

error:
  if (e_dest)
    {
      ra_list_del ((void **) &(e_src->meas->eval.evaluations), e_dest);
      e_dest = NULL;
    }

  return e_dest;
}				/* ra_eval_copy() */


/**
 * \brief delete evaluation
 * \param <eh> evaluation handle
 *
 * The function deletes the evaluation given by 'eh'.
 */
LIBRAAPI int
ra_eval_delete (eval_handle eh)
{
  int ret;
  value_handle vh;
  struct eval *e = (struct eval *) eh;
  struct eval_info *ei;

  if (!eh)
    return -1;

  _ra_set_error (eh, RA_ERR_NONE, "libRASCH");

  if (e->handle_id != RA_HANDLE_EVAL)
    {
      _ra_set_error (eh, RA_ERR_WRONG_HANDLE, "libRASCH");
      return -1;
    }

  /* first delete all classes */
  vh = ra_value_malloc ();
  for (;;)
    {
      const void **p;
      long n;

      ret = ra_class_get (eh, NULL, vh);
      if (ret != 0)
	break;
      n = ra_value_get_num_elem (vh);
      if (n <= 0)
	break;
      p = ra_value_get_voidp_array (vh);
      if (!p)
	break;
      ra_class_delete ((class_handle) (p[0]));
    }
  ra_value_free (vh);
  if (ret != 0)
    return -1;

  /* now delete all attributes associated with this evaluation */
  delete_attributes ((struct eval_head *) e);

  /* and finally delete the evaluation entry */
  ei = &(e->meas->eval);
  ra_list_del ((void **) &(ei->evaluations), e);

  free (e);

  return 0;
}				/* ra_eval_delete() */


/**
 * \brief returns all evaluation-handles
 * \param <mh> measurement-handle
 * \param <vh> value-handle receiving the evaluation-handles
 *
 * This function returns all evaluation-handles in the evaluation file associated with mh.
 */
LIBRAAPI int
ra_eval_get_all (meas_handle mh, value_handle vh)
{
  struct ra_meas *meas = (struct ra_meas *) mh;
  void **p;
  int i, cnt;
  struct eval *curr;

  if ((!mh) || (!vh))
    {
      if (mh)
	_ra_set_error (mh, RA_ERR_INFO_MISSING, "libRASCH");
      return -1;
    }

  ra_value_reset (vh);
  _ra_set_error (mh, RA_ERR_NONE, "libRASCH");

  if (meas->eval.evaluations == NULL)
    return 0;

  cnt = ra_list_len (meas->eval.evaluations);

  p = (void **) calloc (cnt, sizeof (void *));

  curr = meas->eval.evaluations;
  for (i = 0; i < cnt; i++)
    {
      if (curr == NULL)
	break;
      p[i] = curr;
      curr = curr->next;
    }
  ra_value_set_voidp_array (vh, (const void **) p, cnt);

  free (p);

  return 0;
}				/* ra_eval_get_all() */


/**
 * \brief returns the original evaluation-handle
 * \param <mh> measurement-handle
 *
 * This function returns the original evaluation-handle.
 */
LIBRAAPI eval_handle
ra_eval_get_original (meas_handle mh)
{
  struct ra_meas *meas = (struct ra_meas *) mh;
  struct eval *e = NULL;

  if (!mh)
    return NULL;

  _ra_set_error (mh, RA_ERR_NONE, "libRASCH");

  e = meas->eval.evaluations;
  while (e)
    {
      if (e->original)
	break;
      e = e->next;
    }

  return e;
}				/* ra_eval_get_original() */


/**
 * \brief returns the default evaluation-handle
 * \param <mh> measurement-handle
 *
 * This function returns the default evaluation-handle.
 */
LIBRAAPI eval_handle
ra_eval_get_default (meas_handle mh)
{
  struct ra_meas *meas = (struct ra_meas *) mh;
  struct eval *e = NULL;

  if (!mh)
    return NULL;

  _ra_set_error (mh, RA_ERR_NONE, "libRASCH");

  e = meas->eval.evaluations;
  while (e)
    {
      if (e->def)
	break;
      e = e->next;
    }

  return e;
}				/* ra_eval_get_default() */


/**
 * \brief set an evaluation to the default one
 * \param <eh> evaluation handle
 *
 * The function sets the evaluation given by 'eh' to the default one.
 */
LIBRAAPI int
ra_eval_set_default (eval_handle eh)
{
  struct eval *e, *e_prev;

  if (!eh)
    return -1;

  _ra_set_error (eh, RA_ERR_NONE, "libRASCH");

  e = (struct eval *) eh;

  e_prev = ra_eval_get_default (ra_meas_handle_from_any_handle (eh));
  if (e_prev)
    e_prev->def = 0;

  e->def = 1;

  return 0;
}				/* ra_eval_set_default() */


/**
 * \brief returns the evaluation handle an event-class belongs to
 * \param <clh> event-class handle
 *
 * The function returns the evaluation handle which the evant-class
 * given by 'clh' belongs to.
 */
LIBRAAPI eval_handle
ra_eval_get_handle (class_handle clh)
{
  struct eval_class *c = (struct eval_class *) clh;

  if (!clh)
    return NULL;

  _ra_set_error (clh, RA_ERR_NONE, "libRASCH");

  if (c->handle_id != RA_HANDLE_EVAL_CLASS)
    {
      _ra_set_error (clh, RA_ERR_WRONG_HANDLE, "libRASCH");
      return NULL;
    }

  return (eval_handle) (c->eval);
}				/* ra_eval_get_handle() */



/* ------------------------------ event-class functions ------------------------------ */



/**
 * \brief adds an user-defined event-class to an evaluation
 * \param <eh> evaluation handle
 * \param <id> ASCII-id of the event-class
 * \param <name> a short name for the event-class
 * \param <desc> a short description of the event-class
 *
 * The function adds an event-class to the evaluation 'eh'. The parameter
 * 'id' is used to identifiy the event-class and must contain only ASCII
 * characters. The parameters 'name' and 'desc' are used to describe the 
 * event-class.
 * If you want to use a pre-defined event-class, use ra_class_add_predef().
 */
LIBRAAPI class_handle
ra_class_add (eval_handle eh, const char *id, const char *name,
	      const char *desc)
{
  struct eval *e = (struct eval *) eh;
  struct eval_class *c = NULL;
  value_handle vh = NULL;

  if (!eh)
    return NULL;

  if ((id == NULL) || (id[0] == '\0'))
    {
      _ra_set_error (eh, RA_ERR_INFO_MISSING, "libRASCH");
      return NULL;
    }

  _ra_set_error (eh, RA_ERR_NONE, "libRASCH");

  c = calloc (1, sizeof (struct eval_class));
  ra_list_add ((void **) &(e->cl), c);

  c->handle_id = RA_HANDLE_EVAL_CLASS;
  c->id = get_class_id (id);
  strncpy (c->ascii_id, id, MAX_ID_LEN);
  c->meas = e->meas;
  c->eval = e;

  vh = ra_value_malloc ();
  if (name)
    {
      ra_value_set_string (vh, name);
      if (ra_eval_attribute_set (c, "name", vh) != 0)
	goto error;
    }
  if (desc)
    {
      ra_value_set_string (vh, desc);
      if (ra_eval_attribute_set (c, "description", vh) != 0)
	goto error;
    }
  ra_value_free (vh);

  return c;

error:
  ra_class_delete (c);

  if (vh)
    ra_value_free (vh);

  return NULL;
}				/* ra_class_add() */


/**
 * \brief adds an pre-defined event-class to an evaluation
 * \param <eh> evaluation handle
 * \param <id> ASCII-id of the event-class
 *
 * The function adds the event-class 'id' to the evaluation 'eh'. 'id' is one
 * of the pre-defined event-classes in libRASCH.
 * If you want to add not a pre-defined event-class, use ra_class_add().
 */
LIBRAAPI class_handle
ra_class_add_predef (eval_handle eh, const char *id)
{
  class_handle clh = NULL;
  value_handle vh_name, vh_desc;

  if (!eh)
    return NULL;

  vh_name = ra_value_malloc ();
  vh_desc = ra_value_malloc ();

  if (fill_predef_class_info_ascii (id, vh_name, vh_desc) != 0)
    goto error;

  clh =
    ra_class_add (eh, id, ra_value_get_string (vh_name),
		  ra_value_get_string (vh_desc));

error:
  ra_value_free (vh_name);
  ra_value_free (vh_desc);

  return clh;
}				/* ra_class_add_predef() */


/**
 * \brief deletes an event-class
 * \param <clh> event-class handle
 *
 * The function deletes the event-class 'clh'.
 */
LIBRAAPI int
ra_class_delete (class_handle clh)
{
  struct eval_class *c = (struct eval_class *) clh;
  struct eval *e;
  struct eval_property *p;
  struct eval_summary *s;

  if (!clh)
    return -1;

  _ra_set_error (clh, RA_ERR_NONE, "libRASCH");

  if (c->handle_id != RA_HANDLE_EVAL_CLASS)
    {
      _ra_set_error (clh, RA_ERR_WRONG_HANDLE, "libRASCH");
      return -1;
    }

  /* delete all properties associated with this class */
  while ((p = c->prop) != NULL)
    ra_prop_delete (p);

  /* delete all event-summaries associated with this class */
  while ((s = c->summaries) != NULL)
    ra_sum_delete (s);

  /* then delete all events associated with this event class */
  if (c->ev)
    free (c->ev);

  /* now delete all attributes associated with this evaluation */
  delete_attributes ((struct eval_head *) c);

  /* and finally delete the class entry */
  e = c->eval;
  ra_list_del ((void **) &(e->cl), c);

  free (c);

  return 0;
}				/* ra_class_delete() */


/**
 * \brief return event-class 
 * \param <eh> evaluation handle
 * \param <id> event-class ASCII-id
 * \param <vh> value handle
 *
 * The function returns all event-classes with the id 'id'. If
 * 'id' is NULL (or is an empty string), all event-classes in
 * the evaluation 'eh' are given.
 */
LIBRAAPI int
ra_class_get (eval_handle eh, const char *id, value_handle vh)
{
  struct eval *e = (struct eval *) eh;
  struct eval_class *c;
  int num;
  void **vp;

  if (!eh)
    return -1;

  _ra_set_error (eh, RA_ERR_NONE, "libRASCH");
  ra_value_reset (vh);

  if (e->handle_id != RA_HANDLE_EVAL)
    {
      _ra_set_error (eh, RA_ERR_WRONG_HANDLE, "libRASCH");
      return -1;
    }
  if (!vh)
    {
      _ra_set_error (eh, RA_ERR_INFO_MISSING, "libRASCH");
      return -1;
    }

  c = e->cl;
  num = 0;
  vp = NULL;
  while (c)
    {
      int ok = 1;

      if (id && (id[0] != '\0') && (strcmp (c->ascii_id, id) != 0))
	ok = 0;

      if (ok)
	{
	  num++;
	  vp = (void **) realloc (vp, sizeof (void *) * num);
	  vp[num - 1] = c;
	}

      c = c->next;
    }

  if (num > 0)
    {
      ra_value_set_voidp_array (vh, (const void **) vp, num);
      free (vp);
    }

  return 0;
}				/* ra_class_get() */


/**
 * \brief add an event
 * \param <clh> event-class handle
 * \param <start> start position of the event in sample-units
 * \param <end> end position of the event in sample-units
 *
 * The function adds an event to the event-class 'clh'. The start and
 * end of the event are given by 'start' and 'end' respectively. If the
 * event is a point in time, use for 'end' the same value as 'start'.
 * A unique event-id is returned.
 */
LIBRAAPI long
ra_class_add_event (class_handle clh, long start, long end)
{
  struct eval_class *c = (struct eval_class *) clh;

  if (!clh)
    return -1;

  _ra_set_error (clh, RA_ERR_NONE, "libRASCH");

  if (c->handle_id != RA_HANDLE_EVAL_CLASS)
    {
      _ra_set_error (clh, RA_ERR_WRONG_HANDLE, "libRASCH");
      return -1;
    }

  c->num_event++;
  c->ev =
    (struct eval_event *) realloc (c->ev,
				   sizeof (struct eval_event) * c->num_event);
  c->last_id++;
  c->ev[c->num_event - 1].id = c->last_id;
  c->ev[c->num_event - 1].start = start;
  c->ev[c->num_event - 1].end = end;

  add_prop_mem (c);

  return c->last_id;
}				/* ra_class_add_event() */


/**
 * \brief add a list of events
 * \param <clh> event-class handle
 * \param <num_events> number of events to add
 * \param <start> array with the start positions of the event in sample-units
 * \param <end> array with the end positions of the event in sample-units
 * \param <ev_ids> array receiving the event-ids
 *
 * The function adds a list of events to the event-class 'clh'. The start and
 * end of the events are given by 'start' and 'end' respectively. The event-ids
 * are returned in the array 'ev_ids'.
 */
LIBRAAPI int
ra_class_add_event_mass (class_handle clh, unsigned long num_events,
			 const long *start, const long *end, long *ev_ids)
{
  int ret = 0;
  struct eval_class *c = (struct eval_class *) clh;
  size_t l;

  if (!clh)
    return -1;

  _ra_set_error (clh, RA_ERR_NONE, "libRASCH");

  if (c->handle_id != RA_HANDLE_EVAL_CLASS)
    {
      _ra_set_error (clh, RA_ERR_WRONG_HANDLE, "libRASCH");
      return -1;
    }

  c->ev =
    (struct eval_event *) realloc (c->ev,
				   sizeof (struct eval_event) *
				   (c->num_event + num_events));

  for (l = c->num_event; l < (c->num_event + num_events); l++)
    {
      c->last_id++;

      c->ev[l].id = c->last_id;
      c->ev[l].start = start[l - c->num_event];
      c->ev[l].end = end[l - c->num_event];

      add_prop_mem (c);

      ev_ids[l - c->num_event] = c->last_id;
    }
  c->num_event += num_events;

  return ret;
}				/* ra_class_add_event_mass() */


int
add_prop_mem (struct eval_class *c)
{
  struct eval_property *p = c->prop;
  unsigned long total, added;

  while (p)
    {
      /* check if memory needs to be allocated */
      if (p->allocated_events >= c->num_event)
	{
	  p = p->next;
	  continue;
	}

      /* memory is needed -> prepare variables */
      added = (long) ((double) p->allocated_events * EVENT_MEM_ADD);
      if (added < 1)
	added = 1;

      total = p->allocated_events + added;
      if (total < c->num_event)
	{
	  total = c->num_event;
	  added = total - p->allocated_events;
	}

      switch (p->value_type)
	{
	case RA_VALUE_TYPE_SHORT:
	  p->value.s = realloc (p->value.s, p->entry_size * total);
	  memset (p->value.s +
		  (p->allocated_events * (p->entry_size / sizeof (short))),
		  0xff, p->entry_size * added);
	  break;
	case RA_VALUE_TYPE_LONG:
	  p->value.l = realloc (p->value.l, p->entry_size * total);
	  memset (p->value.l +
		  (p->allocated_events * (p->entry_size / sizeof (long))),
		  0xff, p->entry_size * added);
	  break;
	case RA_VALUE_TYPE_DOUBLE:
	  p->value.d = realloc (p->value.d, p->entry_size * total);
	  memset (p->value.d +
		  (p->allocated_events * (p->entry_size / sizeof (double))),
		  0xff, p->entry_size * added);
	  break;
	case RA_VALUE_TYPE_CHAR:
	  p->value.c = realloc (p->value.c, p->entry_size * total);
	  memset (p->value.c +
		  (p->allocated_events * (p->entry_size / sizeof (char *))),
		  0, p->entry_size * added);
	  break;
	case RA_VALUE_TYPE_SHORT_ARRAY:
	  p->value.sa = realloc (p->value.sa, p->entry_size * total);
	  memset (p->value.sa +
		  (p->allocated_events * (p->entry_size / sizeof (short *))),
		  0, p->entry_size * added);
	  break;
	case RA_VALUE_TYPE_LONG_ARRAY:
	  p->value.la = realloc (p->value.la, p->entry_size * total);
	  memset (p->value.la +
		  (p->allocated_events * (p->entry_size / sizeof (long *))),
		  0, p->entry_size * added);
	  break;
	case RA_VALUE_TYPE_DOUBLE_ARRAY:
	  p->value.da = realloc (p->value.da, p->entry_size * total);
	  memset (p->value.da +
		  (p->allocated_events * (p->entry_size / sizeof (double *))),
		  0, p->entry_size * added);
	  break;
	case RA_VALUE_TYPE_CHAR_ARRAY:
	  p->value.ca = realloc (p->value.ca, p->entry_size * total);
	  memset (p->value.ca +
		  (p->allocated_events * (p->entry_size / sizeof (char **))),
		  0, p->entry_size * added);
	  break;
	}

      switch (p->num_type)
	{
	case RA_PROP_NUM_ELEM_TYPE_BYTE:
	  p->num_elements.b =
	    realloc (p->num_elements.b,
		     sizeof (unsigned char) * total * p->num_ch_values);
	  memset (p->num_elements.b +
		  (p->allocated_events * p->num_ch_values), 0,
		  sizeof (unsigned char) * added * p->num_ch_values);
	  break;
	case RA_PROP_NUM_ELEM_TYPE_LONG:
	  p->num_elements.l =
	    realloc (p->num_elements.b,
		     sizeof (long) * total * p->num_ch_values);
	  memset (p->num_elements.l +
		  (p->allocated_events * p->num_ch_values), 0,
		  sizeof (long) * added * p->num_ch_values);
	  break;
	}

      /* the value of the allocated events has to be set at the end
         because the old value is needed above */
      p->allocated_events = total;

      p = p->next;
    }

  return 0;
}				/* add_prop_mem() */


/**
 * \brief delete an event
 * \param <clh> event-class handle
 * \param <event_id> event-id of the event
 *
 * The function deletes the event 'event_id' from the event-class 'clh'.
 */
LIBRAAPI int
ra_class_del_event (class_handle clh, long event_id)
{
  struct eval_class *c = (struct eval_class *) clh;
  unsigned long idx;
  static long last_event_idx = -1;

  if (!clh)
    return -1;

  _ra_set_error (clh, RA_ERR_NONE, "libRASCH");

  if (c->handle_id != RA_HANDLE_EVAL_CLASS)
    {
      _ra_set_error (clh, RA_ERR_WRONG_HANDLE, "libRASCH");
      return -1;
    }
  if ((event_id < 0) || (event_id > c->last_id))
    {
      _ra_set_error (clh, RA_ERR_OUT_OF_RANGE, "libRASCH");
      return -1;
    }

  idx = get_event_idx (c, event_id, last_event_idx);
  if (idx < 0)
    {
      _ra_set_error (clh, RA_ERR_WRONG_INPUT, "libRASCH");
      return -1;
    }
  last_event_idx = idx;

  del_prop_values (c, idx);

  if (idx == 0)
    memmove (c->ev, c->ev + 1,
	     sizeof (struct eval_event) * (c->num_event - 1));
  else if (idx < (c->num_event - 1))
    memmove (c->ev + idx, c->ev + idx + 1,
	     sizeof (struct eval_event) * (c->num_event - idx - 1));

  c->num_event--;

  c->ev =
    (struct eval_event *) realloc (c->ev,
				   sizeof (struct eval_event) * c->num_event);

  del_ev_summary (c, event_id);

  return 0;
}				/* ra_class_del_event() */


long
get_event_idx (struct eval_class *c, long event_id, long last_idx)
{
  static long event_id_save, idx_save;
  static class_handle cl_save;
  long idx = -1;
  long start = 0;
  long l;

  if (((class_handle) c == cl_save) && (event_id == event_id_save))
    return idx_save;

  if (last_idx >= 0)
    start = last_idx + 1;

  for (l = start; l < (long) c->num_event; l++)
    {
      if (c->ev[l].id == event_id)
	{
	  idx = l;
	  break;
	}
    }

  if ((idx == -1) && (last_idx >= 0))
    {
      if (last_idx >= (long) c->num_event)
	last_idx = (long) c->num_event - 1;

      for (l = 0; l <= last_idx; l++)
	{
	  if (c->ev[l].id == event_id)
	    {
	      idx = l;
	      break;
	    }
	}
    }

  cl_save = (class_handle) c;
  event_id_save = event_id;
  idx_save = idx;

  return idx;
}				/* get_event_idx() */


int
del_prop_values (struct eval_class *c, unsigned long idx)
{
  unsigned long l, m, offset, num;
  struct eval_property *p = c->prop;

  while (p)
    {
      offset = idx * p->num_ch_values;

      switch (p->value_type)
	{
	case RA_VALUE_TYPE_SHORT:
	  if (idx < (c->num_event - 1))
	    {
	      memmove (p->value.s + offset,
		       p->value.s + (offset + p->num_ch_values),
		       sizeof (short) * p->num_ch_values * (c->num_event -
							    idx - 1));
	    }
	  break;
	case RA_VALUE_TYPE_LONG:
	  if (idx < (c->num_event - 1))
	    {
	      memmove (p->value.l + offset,
		       p->value.l + (offset + p->num_ch_values),
		       sizeof (long) * p->num_ch_values * (c->num_event -
							   idx - 1));
	    }
	  break;
	case RA_VALUE_TYPE_DOUBLE:
	  if (idx < (c->num_event - 1))
	    {
	      memmove (p->value.d + offset,
		       p->value.d + (offset + p->num_ch_values),
		       sizeof (double) * p->num_ch_values * (c->num_event -
							     idx - 1));
	    }
	  break;
	case RA_VALUE_TYPE_CHAR:
	  for (l = 0; l < p->num_ch_values; l++)
	    free (p->value.c[offset + l]);
	  if (idx < (c->num_event - 1))
	    {
	      memmove (p->value.c + offset,
		       p->value.c + (offset + p->num_ch_values),
		       sizeof (char *) * p->num_ch_values * (c->num_event -
							     idx - 1));
	    }
	  for (l = 0; l < p->num_ch_values; l++)
	    p->value.c[((c->num_event - 1) * p->num_ch_values) + l] = NULL;
	  break;
	case RA_VALUE_TYPE_SHORT_ARRAY:
	  for (l = 0; l < p->num_ch_values; l++)
	    free (p->value.sa[offset + l]);
	  if (idx < (c->num_event - 1))
	    {
	      memmove (p->value.sa + offset,
		       p->value.sa + (offset + p->num_ch_values),
		       sizeof (short *) * p->num_ch_values * (c->num_event -
							      idx - 1));
	    }
	  for (l = 0; l < p->num_ch_values; l++)
	    p->value.sa[((c->num_event - 1) * p->num_ch_values) + l] = NULL;
	  break;
	case RA_VALUE_TYPE_LONG_ARRAY:
	  for (l = 0; l < p->num_ch_values; l++)
	    free (p->value.la[offset + l]);
	  if (idx < (c->num_event - 1))
	    {
	      memmove (p->value.la + offset,
		       p->value.la + (offset + p->num_ch_values),
		       sizeof (long *) * p->num_ch_values * (c->num_event -
							     idx - 1));
	    }
	  for (l = 0; l < p->num_ch_values; l++)
	    p->value.la[((c->num_event - 1) * p->num_ch_values) + l] = NULL;
	  break;
	case RA_VALUE_TYPE_DOUBLE_ARRAY:
	  for (l = 0; l < p->num_ch_values; l++)
	    free (p->value.da[offset + l]);
	  if (idx < (c->num_event - 1))
	    {
	      memmove (p->value.da + offset,
		       p->value.da + (offset + p->num_ch_values),
		       sizeof (double *) * p->num_ch_values * (c->num_event -
							       idx - 1));
	    }
	  for (l = 0; l < p->num_ch_values; l++)
	    p->value.da[((c->num_event - 1) * p->num_ch_values) + l] = NULL;
	  break;
	case RA_VALUE_TYPE_CHAR_ARRAY:
	  for (l = 0; l < p->num_ch_values; l++)
	    {
	      num = 0;
	      switch (p->num_type)
		{
		case RA_PROP_NUM_ELEM_TYPE_BYTE:
		  num = p->num_elements.b[offset + l];
		  break;
		case RA_PROP_NUM_ELEM_TYPE_LONG:
		  num = p->num_elements.l[offset + l];
		  break;
		}
	      for (m = 0; m < num; m++)
		free (p->value.ca[offset + l][m]);
	      free (p->value.ca[offset + l]);
	    }
	  if (idx < (c->num_event - 1))
	    {
	      memmove (p->value.ca + offset,
		       p->value.ca + (offset + p->num_ch_values),
		       sizeof (char **) * p->num_ch_values * (c->num_event -
							      idx - 1));
	    }
	  for (l = 0; l < p->num_ch_values; l++)
	    p->value.ca[((c->num_event - 1) * p->num_ch_values) + l] = NULL;
	  break;
	}

      if (p->num_type == RA_PROP_NUM_ELEM_TYPE_BYTE)
	{
	  memmove (p->num_elements.b + offset,
		   p->num_elements.b + (offset + p->num_ch_values),
		   sizeof (unsigned char) * p->num_ch_values * (c->num_event -
								idx - 1));
	  for (l = 0; l < p->num_ch_values; l++)
	    p->num_elements.b[((c->num_event - 1) * p->num_ch_values) + l] =
	      0;
	}
      else
	{
	  memmove (p->num_elements.l + offset,
		   p->num_elements.l + (offset + p->num_ch_values),
		   sizeof (long) * p->num_ch_values * (c->num_event - idx -
						       1));
	  for (l = 0; l < p->num_ch_values; l++)
	    p->num_elements.l[((c->num_event - 1) * p->num_ch_values) + l] =
	      0;
	}

      p = p->next;
    }

  return 0;
}				/* del_prop_values() */


int
del_ev_summary (class_handle clh, long event_id)
{
  struct eval_class *c;
  struct eval_summary *sum;
  struct eval_sum_data *sd, *sd_next;
  unsigned long l, cnt;
  long *new_id;

  c = (struct eval_class *) clh;
  sum = c->summaries;

  while (sum)
    {
      sd = sum->sum;
      while (sd)
	{
	  if (sd->num_events <= 0)
	    {
	      sd = sd->next;
	      continue;
	    }

	  new_id = malloc (sizeof (long) * sd->num_events);
	  cnt = 0;
	  for (l = 0; l < sd->num_events; l++)
	    {
	      if (sd->event_ids[l] == event_id)
		continue;

	      new_id[cnt++] = sd->event_ids[l];
	    }
	  sd_next = sd->next;

	  if (sd->event_ids)
	    {
	      sd->num_events = 0;
	      free (sd->event_ids);
	      sd->event_ids = NULL;
	    }
	  if (cnt > 0)
	    {
	      sd->num_events = cnt;
	      sd->event_ids = new_id;
	    }
	  else
	    {
	      free (new_id);
	      ra_sum_del_part (sum, sd->id);
	    }

	  sd = sd_next;
	}

      sum = sum->next;
    }

  return 0;
}				/* del_ev_summary() */


/**
 * \brief get the start and end position of an event
 * \param <clh> event-class handle
 * \param <event_id> event-id of the event
 * \param <start> here the start position will be returned
 * \param <end> here the end position will be returned
 *
 * The function returns in 'start' and 'end' the position of the evvent 'event_id'
 * from the event-class 'clh'. The positions are in sample-units.
 */
LIBRAAPI int
ra_class_get_event_pos (class_handle clh, long event_id, long *start,
			long *end)
{
  static long last_event_idx = -1;
  struct eval_class *c = (struct eval_class *) clh;
  long idx;

  if (!clh)
    return -1;

  _ra_set_error (clh, RA_ERR_NONE, "libRASCH");

  if (c->handle_id != RA_HANDLE_EVAL_CLASS)
    {
      _ra_set_error (clh, RA_ERR_WRONG_HANDLE, "libRASCH");
      return -1;
    }
  if ((event_id < 0) || (event_id > c->last_id))
    {
      _ra_set_error (clh, RA_ERR_OUT_OF_RANGE, "libRASCH");
      return -1;
    }

  idx = get_event_idx (c, event_id, last_event_idx);
  if (idx < 0)
    {
      _ra_set_error (clh, RA_ERR_WRONG_INPUT, "libRASCH");
      return -1;
    }
  last_event_idx = idx;

  if (start)
    *start = c->ev[idx].start;
  if (end)
    *end = c->ev[idx].end;

  return 0;
}				/* ra_class_get_event_pos() */


/**
 * \brief set the start and end position of an event
 * \param <clh> event-class handle
 * \param <event_id> event-id of the event
 * \param <start> the start position of the event
 * \param <end> the end position of the event
 *
 * The function sets the start and end position of the event 'event_id' in the
 * event-class 'clh' to the values 'start' and 'end'.
 */
LIBRAAPI int
ra_class_set_event_pos (class_handle clh, long event_id, long start, long end)
{
  static long last_event_idx = -1;
  struct eval_class *c = (struct eval_class *) clh;
  long idx;

  if (!clh)
    return -1;

  _ra_set_error (clh, RA_ERR_NONE, "libRASCH");

  if (c->handle_id != RA_HANDLE_EVAL_CLASS)
    {
      _ra_set_error (clh, RA_ERR_WRONG_HANDLE, "libRASCH");
      return -1;
    }
  if ((event_id < 0) || (event_id > c->last_id))
    {
      _ra_set_error (clh, RA_ERR_OUT_OF_RANGE, "libRASCH");
      return -1;
    }

  idx = get_event_idx (c, event_id, last_event_idx);
  if (idx < 0)
    {
      _ra_set_error (clh, RA_ERR_WRONG_INPUT, "libRASCH");
      return -1;
    }
  last_event_idx = idx;

  c->ev[idx].start = start;
  c->ev[idx].end = end;

  return 0;
}				/* ra_class_set_event_pos() */


int
comp_pos (const void *arg1, const void *arg2)
{
  struct sort_pos *b1 = (struct sort_pos *) arg1;
  struct sort_pos *b2 = (struct sort_pos *) arg2;

  return (b1->pos - b2->pos);
}				/* comp_pos() */



/**
 * \brief returns the events in a specific region
 * \param <clh> event-class handle
 * \param <start> start of the region of interest
 * \param <end> end of the region of interest
 * \param <complete> flag if the events have to be complete in the region of interest
 * \param <sort> flag if the found events should be sorted according their start position
 * \param <vh> value handle receiving the event-id's asked for
 *
 * The function returns in 'vh' the events which are inside a specific region.
 * The region of interest (ROI)is given by 'start' and 'end'. If the 'complete' flag
 * is set, the events have to be complete inside the ROI. If it is not set, all events
 * are returned which starts or ends inside the ROI.
 */
LIBRAAPI int
ra_class_get_events (class_handle clh, long start, long end, int complete,
		     int sort, value_handle vh)
{
  struct eval_class *c = (struct eval_class *) clh;
  struct sort_pos *sp = NULL;
  long *ids;
  unsigned long l, num;

  if (!clh)
    return -1;

  _ra_set_error (clh, RA_ERR_NONE, "libRASCH");
  ra_value_reset (vh);

  if (c->handle_id != RA_HANDLE_EVAL_CLASS)
    {
      _ra_set_error (clh, RA_ERR_WRONG_HANDLE, "libRASCH");
      return -1;
    }
  if (!vh)
    {
      _ra_set_error (clh, RA_ERR_INFO_MISSING, "libRASCH");
      return -1;
    }

  if ((start <= 0) && (end < 0))
    {
      num = c->num_event;
      sp = (struct sort_pos *) malloc (sizeof (struct sort_pos) * num);
      for (l = 0; l < num; l++)
	{
	  sp[l].id = c->ev[l].id;
	  sp[l].pos = c->ev[l].start;
	}
    }
  else
    {
      num = 0;
      for (l = 0; l < c->num_event; l++)
	{
	  if ((c->ev[l].start < start) && complete)
	    continue;
	  if ((end != -1) && ((c->ev[l].end > end) && complete))
	    continue;
	  if ((end != -1) && (c->ev[l].end < start))
	    continue;
	  if (c->ev[l].start > end)
	    continue;

	  num++;
	  sp =
	    (struct sort_pos *) realloc (sp, sizeof (struct sort_pos) * num);
	  sp[num - 1].id = c->ev[l].id;
	  sp[num - 1].pos = c->ev[l].start;
	}
    }

  if ((num > 0) && sort)
    qsort (sp, num, sizeof (struct sort_pos), comp_pos);

  if (num > 0)
    {
      ids = (long *) malloc (sizeof (long) * num);
      for (l = 0; l < num; l++)
	ids[l] = sp[l].id;

      ra_value_set_long_array (vh, ids, num);

      free (ids);
      free (sp);
    }

  return 0;
}				/* ra_class_get_events() */


/**
 * \brief returns the event-id preceeding the given one
 * \param <clh> event-class handle
 * \param <event_id> event-id
 *
 * The function returns the id of the event preceeding the one given in 'event_id'.
 * If the given event is the first one '-1' is returned.
 */
LIBRAAPI long
ra_class_get_prev_event (class_handle clh, long event_id)
{
  value_handle vh;
  long l, num, id_prev;
  const long *id;

  if (!clh)
    return -1;

  vh = ra_value_malloc ();

  ra_class_get_events (clh, -1, -1, 0, 1, vh);
  id = ra_value_get_long_array (vh);
  num = ra_value_get_num_elem (vh);

  id_prev = -1;
  for (l = 0; l < num; l++)
    {
      if (id[l] == event_id)
	{
	  if (l > 0)
	    id_prev = id[l - 1];
	  break;
	}
    }

  return id_prev;
}				/* ra_class_get_prev_event() */


/**
 * \brief returns the event-id following the given one
 * \param <clh> event-class handle
 * \param <event_id> event-id
 *
 * The function returns the id of the event following the one given in 'event_id'.
 * If the given event is the first one '-1' is returned.
 */
LIBRAAPI long
ra_class_get_next_event (class_handle clh, long event_id)
{
  value_handle vh;
  long l, num, id_next;
  const long *id;

  if (!clh)
    return -1;

  vh = ra_value_malloc ();

  ra_class_get_events (clh, -1, -1, 0, 1, vh);
  id = ra_value_get_long_array (vh);
  num = ra_value_get_num_elem (vh);

  id_next = -1;
  for (l = 0; l < num; l++)
    {
      if (id[l] == event_id)
	{
	  if (l < (num - 1))
	    id_next = id[l + 1];
	  break;
	}
    }

  return id_next;
}				/* ra_class_get_next_event() */


/**
 * \brief returns the event-class handle the event-property belongs to
 * \param <ph> event-property handle
 *
 * The function returns the event-class handle the event-property 'ph' belongs to.
 */
LIBRAAPI class_handle
ra_class_get_handle (any_handle h)
{
  struct eval_property *p;
  struct eval_summary *s;
  unsigned short type;
  class_handle clh = NULL;

  if (!h)
    return NULL;

  _ra_set_error (h, RA_ERR_NONE, "libRASCH");

  type = *((unsigned short *) h);
  if (type == RA_HANDLE_EVAL_PROP)
    {
      p = (struct eval_property *) h;
      clh = (class_handle) (p->evclass);
    }
  else if (type == RA_HANDLE_EVAL_SUMMARY)
    {
      s = (struct eval_summary *) h;
      clh = (class_handle) (s->evclass);
    }
  else
    _ra_set_error (h, RA_ERR_WRONG_HANDLE, "libRASCH");

  return clh;
}				/* ra_class_get_handle() */


/* ------------------------------ event-property functions ------------------------------ */


/**
 * \brief adds an user-defined event-property to an event-class
 * \param <clh> event-class handle
 * \param <id> ASCII-id of the event-property
 * \param <value_type> RA_VALUE_TYPE_* of the values stored in the event-property
 * \param <name> a short name for the event-property
 * \param <desc> a short description of the event-property
 * \param <unit> unit of the values stored in the event-property
 * \param <use_minmax> flag if min-/max-values are valid
 * \param <min> minimum value
 * \param <max> maximum value
 * \param <has_ignore_value> flag if 'ignore_value' is valid
 * \param <ignore_value> a value which is interpreted as non-valid and can be ignored
 *
 * The function adds an event-property to the event-class 'clh'. The parameter
 * 'id' is used to identifiy the event-property and must contain only ASCII
 * characters. The parameters 'name' and 'desc' are used to describe the 
 * event-class.
 * If you want to use a pre-defined event-property, use ra_prop_add_predef().
 */
LIBRAAPI prop_handle
ra_prop_add (class_handle clh, const char *id, long value_type,
	     const char *name, const char *desc, const char *unit,
	     int use_minmax, double min, double max, int has_ignore_value,
	     double ignore_value)
{
  struct eval_class *c = (struct eval_class *) clh;
  struct eval_property *p = NULL;
  value_handle vh = NULL;
  unsigned long l, num_ch;
  long *ch = NULL;

  if (!clh)
    return NULL;

  _ra_set_error (clh, RA_ERR_NONE, "libRASCH");

  if (c->handle_id != RA_HANDLE_EVAL_CLASS)
    {
      _ra_set_error (clh, RA_ERR_WRONG_HANDLE, "libRASCH");
      return NULL;
    }
  if (id == NULL)
    {
      _ra_set_error (clh, RA_ERR_INFO_MISSING, "libRASCH");
      return NULL;
    }

  p = calloc (1, sizeof (struct eval_property));
  ra_list_add ((void **) &(c->prop), p);

  p->handle_id = RA_HANDLE_EVAL_PROP;
  p->id = get_prop_id (id);
  strncpy (p->ascii_id, id, MAX_ID_LEN);
  p->meas = c->meas;
  p->evclass = c;
  p->value_type = value_type;

  vh = ra_value_malloc ();
  if (name)
    {
      ra_value_set_string (vh, name);
      if (ra_eval_attribute_set (p, "name", vh) != 0)
	goto error;
    }
  if (desc)
    {
      ra_value_set_string (vh, desc);
      if (ra_eval_attribute_set (p, "description", vh) != 0)
	goto error;
    }
  if (unit)
    {
      ra_value_set_string (vh, unit);
      if (ra_eval_attribute_set (p, "unit", vh) != 0)
	goto error;
    }
  if (use_minmax)
    {
      ra_value_set_long (vh, 1);
      if (ra_eval_attribute_set (p, "use-minmax", vh) != 0)
	goto error;
      ra_value_set_double (vh, min);
      if (ra_eval_attribute_set (p, "min", vh) != 0)
	goto error;
      ra_value_set_double (vh, max);
      if (ra_eval_attribute_set (p, "max", vh) != 0)
	goto error;
    }
  if (has_ignore_value)
    {
      ra_value_set_long (vh, 1);
      if (ra_eval_attribute_set (p, "has-ignore-value", vh) != 0)
	goto error;
      ra_value_set_double (vh, ignore_value);
      if (ra_eval_attribute_set (p, "ignore-value", vh) != 0)
	goto error;
    }

  if (get_channels (clh, &num_ch, &ch) == 0)
    {
      p->num_ch_values = num_ch + 1;
      p->ch_map = malloc (sizeof (unsigned short) * p->num_ch_values);
      p->ch_map[0] = -1;
      for (l = 1; l < p->num_ch_values; l++)
	p->ch_map[l] = (short) (ch[l - 1]);
    }
  else
    goto error;

  if (alloc_prop_mem (p) != 0)
    goto error;

  ra_value_free (vh);
  free (ch);

  return p;

error:
  ra_prop_delete (p);

  if (vh)
    ra_value_free (vh);
  if (ch)
    free (ch);

  return NULL;
}				/* ra_prop_add() */


int
get_channels (class_handle clh, unsigned long *num_ch, long **ch)
{
  meas_handle meas;
  rec_handle rec;
  value_handle vh;
  unsigned long l, num;
  struct eval_class *cl = (struct eval_class *) clh;

  *num_ch = 0;
  *ch = NULL;

  meas = ra_meas_handle_from_any_handle (clh);
  rec = ra_rec_get_first (meas, 0);
  vh = ra_value_malloc ();
  if (ra_info_get (rec, RA_INFO_REC_GEN_NUM_CHANNEL_L, vh) != 0)
    {
      ra_value_free (vh);
      return 0;
    }

  num = ra_value_get_long (vh);
  *ch = malloc (sizeof (long) * num);
  for (l = 0; l < num; l++)
    {
      int skip_ch = 0;
      long ch_type = -1;

      /* check if it is a real channel and not a fake one to needed to
         get the samplerate */
      ra_value_set_number (vh, l);
      if (ra_info_get (rec, RA_INFO_REC_CH_NAME_C, vh) != 0)
	continue;

      if (ra_info_get (rec, RA_INFO_REC_CH_TYPE_L, vh) == 0)
	ch_type = ra_value_get_long (vh);

      /* check if channel fits to current event-class */
      switch (cl->id)
	{
	case EVENT_CLASS_HEARTBEAT:
	  if ((ch_type != RA_CH_TYPE_ECG) && (ch_type != RA_CH_TYPE_RR)
	      && (ch_type != RA_CH_TYPE_RESP))
	    skip_ch = 1;
	  break;
	case EVENT_CLASS_RR_CALIBRATION:
	  if (ch_type != RA_CH_TYPE_RR)
	    skip_ch = 1;
	  break;
	case EVENT_CLASS_ARRHYTHMIA:
	case EVENT_CLASS_ARRHYTHMIA_ORIGINAL:
	  skip_ch = 1;
	  break;
	case EVENT_CLASS_UTERINE_CONTRACTION:
	  if (ch_type != RA_CH_TYPE_CTG_UC)
	    skip_ch = 1;
	  break;
	case EVENT_CLASS_RESPIRATION:
	  if (ch_type != RA_CH_TYPE_RESP)
	    skip_ch = 1;
	  break;
	case EVENT_CLASS_ANNOT:
	default:
	  break;
	}

      if (skip_ch)
	continue;

      (*ch)[*num_ch] = l;
      (*num_ch)++;
    }
  ra_value_free (vh);

  if (*num_ch == 0)
    {
      free (*ch);
      *ch = NULL;
    }

  return 0;
}				/* get_channels() */


int
alloc_prop_mem (struct eval_property *p)
{
  struct eval_class *c = p->evclass;
  size_t size;

  p->allocated_events = c->num_event;

  switch (p->value_type)
    {
    case RA_VALUE_TYPE_SHORT:
      p->entry_size = sizeof (short) * p->num_ch_values;
      if (c->num_event > 0)
	{
	  size = c->num_event * p->entry_size;
	  p->value.s = malloc (size);
	  memset (p->value.s, 0, size);
	}
      break;
    case RA_VALUE_TYPE_LONG:
      p->entry_size = sizeof (long) * p->num_ch_values;
      if (c->num_event > 0)
	{
	  size = c->num_event * p->entry_size;
	  p->value.l = malloc (size);
	  memset (p->value.l, 0, size);
	}
      break;
    case RA_VALUE_TYPE_DOUBLE:
      p->entry_size = sizeof (double) * p->num_ch_values;
      if (c->num_event > 0)
	{
	  size = c->num_event * p->entry_size;
	  p->value.d = malloc (size);
	  memset (p->value.d, 0, size);
	}
      break;
    case RA_VALUE_TYPE_CHAR:
      p->entry_size = sizeof (char *) * p->num_ch_values;
      if (c->num_event > 0)
	{
	  size = c->num_event * p->entry_size;
	  p->value.c = malloc (size);
	  memset (p->value.c, 0, size);
	}
      break;
    case RA_VALUE_TYPE_SHORT_ARRAY:
      p->entry_size = sizeof (short *) * p->num_ch_values;
      if (c->num_event > 0)
	{
	  size = c->num_event * p->entry_size;
	  p->value.sa = malloc (size);
	  memset (p->value.sa, 0, size);
	}
      break;
    case RA_VALUE_TYPE_LONG_ARRAY:
      p->entry_size = sizeof (long *) * p->num_ch_values;
      if (c->num_event > 0)
	{
	  size = c->num_event * p->entry_size;
	  p->value.la = malloc (size);
	  memset (p->value.la, 0, size);
	}
      break;
    case RA_VALUE_TYPE_DOUBLE_ARRAY:
      p->entry_size = sizeof (double *) * p->num_ch_values;
      if (c->num_event > 0)
	{
	  size = c->num_event * p->entry_size;
	  p->value.da = malloc (size);
	  memset (p->value.da, 0, size);
	}
      break;
    case RA_VALUE_TYPE_CHAR_ARRAY:
      p->entry_size = sizeof (char **) * p->num_ch_values;
      if (c->num_event > 0)
	{
	  size = c->num_event * p->entry_size;
	  p->value.ca = malloc (size);
	  memset (p->value.ca, 0, size);
	}
      break;
    }

  /* initially we start with the byte array for holding the number
     of elements because most of the times only one value will be
     stored */
  p->num_type = RA_PROP_NUM_ELEM_TYPE_BYTE;
  p->num_elements.b = calloc (c->num_event * p->num_ch_values, 1);

  return 0;
}				/* alloc_prop_mem() */


/**
 * \brief adds an pre-defined event-property to an event-class
 * \param <clh> event-class handle
 * \param <id> ASCII-id of the event-property
 *
 * The function adds the event-property 'id' to the event-class 'clh'. 'id' is one
 * of the pre-defined event-properties in libRASCH.
 * If you want to add not a pre-defined event-property, use ra_prop_add().
 */
LIBRAAPI prop_handle
ra_prop_add_predef (class_handle clh, const char *id)
{
  prop_handle ph = NULL;
  value_handle vh_type, vh_len, vh_name, vh_desc, vh_unit;
  value_handle vh_use_minmax, vh_min, vh_max;
  value_handle vh_has_ign_val, vh_ign_val;

  if (!clh)
    return NULL;

  vh_type = ra_value_malloc ();
  vh_len = ra_value_malloc ();
  vh_name = ra_value_malloc ();
  vh_desc = ra_value_malloc ();
  vh_unit = ra_value_malloc ();
  vh_use_minmax = ra_value_malloc ();
  vh_min = ra_value_malloc ();
  vh_max = ra_value_malloc ();
  vh_has_ign_val = ra_value_malloc ();
  vh_ign_val = ra_value_malloc ();

  if (fill_predef_prop_info_ascii
      (id, vh_type, vh_len, vh_name, vh_desc, vh_unit, vh_use_minmax, vh_min,
       vh_max, vh_has_ign_val, vh_ign_val) != 0)
    {
      goto error;
    }

  /* add property */
  ph = ra_prop_add (clh, id, ra_value_get_long (vh_type),
		    ra_value_get_string (vh_name),
		    ra_value_get_string (vh_desc),
		    ra_value_get_string (vh_unit),
		    ra_value_get_long (vh_use_minmax),
		    ra_value_get_double (vh_min),
		    ra_value_get_double (vh_max),
		    ra_value_get_long (vh_has_ign_val),
		    ra_value_get_double (vh_ign_val));

error:
  ra_value_free (vh_type);
  ra_value_free (vh_len);
  ra_value_free (vh_name);
  ra_value_free (vh_desc);
  ra_value_free (vh_unit);
  ra_value_free (vh_use_minmax);
  ra_value_free (vh_min);
  ra_value_free (vh_max);
  ra_value_free (vh_has_ign_val);
  ra_value_free (vh_ign_val);

  return ph;
}				/* ra_prop_add_predef() */


/**
 * \brief deletes an event-property
 * \param <ph> event-property handle
 *
 * The function deletes the event-property 'ph'.
 */
LIBRAAPI int
ra_prop_delete (prop_handle ph)
{
  struct eval_property *p = (struct eval_property *) ph;
  struct eval_class *c;

  if (!ph)
    return -1;

  _ra_set_error (ph, RA_ERR_NONE, "libRASCH");

  if (p->handle_id != RA_HANDLE_EVAL_PROP)
    {
      _ra_set_error (ph, RA_ERR_WRONG_HANDLE, "libRASCH");
      return -1;
    }


  /* delete allocated memory */
  free_prop_values (p);

  if (p->ch_map)
    free (p->ch_map);
  switch (p->num_type)
    {
    case RA_PROP_NUM_ELEM_TYPE_BYTE:
      free (p->num_elements.b);
      break;
    case RA_PROP_NUM_ELEM_TYPE_LONG:
      free (p->num_elements.l);
      break;
    }

  /* now delete all attributes associated with this event property */
  delete_attributes (ph);

  /* delete event-property entry */
  c = p->evclass;
  ra_list_del ((void **) &(c->prop), p);

  free (p);

  return 0;
}				/* ra_prop_delete() */


int
free_prop_values (struct eval_property *p)
{
  unsigned long m, n, num;
  struct eval_class *c;

  c = p->evclass;

  switch (p->value_type)
    {
    case RA_VALUE_TYPE_SHORT:
      free (p->value.s);
      break;
    case RA_VALUE_TYPE_LONG:
      free (p->value.l);
      break;
    case RA_VALUE_TYPE_DOUBLE:
      free (p->value.d);
      break;
    case RA_VALUE_TYPE_CHAR:
      if (p->num_type == RA_PROP_NUM_ELEM_TYPE_BYTE)
	{
	  for (m = 0; m < (p->num_ch_values * c->num_event); m++)
	    {
	      if (p->num_elements.b[m] > 0)
		free (p->value.c[m]);
	    }
	}
      else
	{
	  for (m = 0; m < (p->num_ch_values * c->num_event); m++)
	    {
	      if (p->num_elements.l[m] > 0)
		free (p->value.c[m]);
	    }
	}
      free (p->value.c);
      break;
    case RA_VALUE_TYPE_SHORT_ARRAY:
      if (p->num_type == RA_PROP_NUM_ELEM_TYPE_BYTE)
	{
	  for (m = 0; m < (p->num_ch_values * c->num_event); m++)
	    {
	      if (p->num_elements.b[m] > 0)
		free (p->value.sa[m]);
	    }
	}
      else
	{
	  for (m = 0; m < (p->num_ch_values * c->num_event); m++)
	    {
	      if (p->num_elements.l[m] > 0)
		free (p->value.sa[m]);
	    }
	}
      free (p->value.sa);
      break;
    case RA_VALUE_TYPE_LONG_ARRAY:
      if (p->num_type == RA_PROP_NUM_ELEM_TYPE_BYTE)
	{
	  for (m = 0; m < (p->num_ch_values * c->num_event); m++)
	    {
	      if (p->num_elements.b[m] > 0)
		free (p->value.la[m]);
	    }
	}
      else
	{
	  for (m = 0; m < (p->num_ch_values * c->num_event); m++)
	    {
	      if (p->num_elements.l[m] > 0)
		free (p->value.la[m]);
	    }
	}
      free (p->value.la);
      break;
    case RA_VALUE_TYPE_DOUBLE_ARRAY:
      if (p->num_type == RA_PROP_NUM_ELEM_TYPE_BYTE)
	{
	  for (m = 0; m < (p->num_ch_values * c->num_event); m++)
	    {
	      if (p->num_elements.b[m] > 0)
		free (p->value.da[m]);
	    }
	}
      else
	{
	  for (m = 0; m < (p->num_ch_values * c->num_event); m++)
	    {
	      if (p->num_elements.l[m] > 0)
		free (p->value.da[m]);
	    }
	}
      free (p->value.da);
      break;
    case RA_VALUE_TYPE_CHAR_ARRAY:
      for (m = 0; m < (p->num_ch_values * c->num_event); m++)
	{
	  num = 0;
	  switch (p->num_type)
	    {
	    case RA_PROP_NUM_ELEM_TYPE_BYTE:
	      num = p->num_elements.b[m];
	      break;
	    case RA_PROP_NUM_ELEM_TYPE_LONG:
	      num = p->num_elements.l[m];
	      break;
	    }
	  for (n = 0; n < num; n++)
	    free (p->value.ca[m][n]);
	  free (p->value.ca[m]);
	}
      free (p->value.ca);
      break;
    default:
      _ra_set_error (p, RA_ERR_ERROR, "libRASCH");
      break;
    }

  return 0;
}				/* free_prop_values() */


/**
 * \brief returns all event-properties of an event-class
 * \param <ph> event-class handle
 * \param <vh> ra-value handle receiving the event-property handles
 *
 * The function returns in 'vh' all event-properties available in event-class 'clh'.
 */
LIBRAAPI int
ra_prop_get_all (class_handle clh, value_handle vh)
{
  struct eval_class *c = (struct eval_class *) clh;
  void **vp;
  long num, cnt;
  struct eval_property *p;

  if (!clh)
    return -1;

  _ra_set_error (clh, RA_ERR_NONE, "libRASCH");
  ra_value_reset (vh);

  if (c->handle_id != RA_HANDLE_EVAL_CLASS)
    {
      _ra_set_error (clh, RA_ERR_WRONG_HANDLE, "libRASCH");
      return -1;
    }
  if (!vh)
    {
      _ra_set_error (clh, RA_ERR_INFO_MISSING, "libRASCH");
      return -1;
    }

  num = ra_list_len (c->prop);
  if (num <= 0)
    return 0;

  vp = malloc (sizeof (void *) * num);

  p = c->prop;
  cnt = 0;
  while (p)
    {
      if (cnt >= num)
	break;

      vp[cnt] = p;
      cnt++;
      p = p->next;
    }

  if (cnt > 0)
    ra_value_set_voidp_array (vh, (const void **) vp, cnt);

  free (vp);

  return 0;
}				/* ra_prop_get_all() */


/**
 * \brief returns event-property 'id'
 * \param <clh> event-class handle
 * \param <id> ASCII id of the wanted event-property
 *
 * The function returns a handle to the event-property 'id' from event-class 'clh'.
 * If the event-property is not available 'NULL' is returned.
 */
LIBRAAPI prop_handle
ra_prop_get (class_handle clh, const char *id)
{
  struct eval_class *c = (struct eval_class *) clh;
  struct eval_property *p;

  if (!clh)
    return NULL;

  _ra_set_error (clh, RA_ERR_NONE, "libRASCH");

  if (c->handle_id != RA_HANDLE_EVAL_CLASS)
    {
      _ra_set_error (clh, RA_ERR_WRONG_HANDLE, "libRASCH");
      return NULL;
    }
  if (!id || (id[0] == '\0'))
    {
      _ra_set_error (clh, RA_ERR_INFO_MISSING, "libRASCH");
      return NULL;
    }

  p = c->prop;
  while (p)
    {
      if (strcmp (p->ascii_id, id) == 0)
	break;
      p = p->next;
    }

  return p;
}				/* ra_prop_get() */


/**
 * \brief set value in event-property
 * \param <ph> event-property handle
 * \param <event_id> event-id
 * \param <ch> channel number
 * \param <vh> contains value which should be set
 *
 * The function set the value stored in 'vh' in event-property 'ph' for
 * event-id 'event_id' and channel 'ch'. If the value is channel independent
 * use '-1' for the channel number.
 */
LIBRAAPI int
ra_prop_set_value (prop_handle ph, long event_id, long ch, value_handle vh)
{
  struct eval_property *p = (struct eval_property *) ph;
  unsigned long offset, num_elem;
  long l, col, idx;
  const char **ca;
  static long last_event_idx = -1;

  if (!ph)
    return -1;

  _ra_set_error (ph, RA_ERR_NONE, "libRASCH");

  if (p->handle_id != RA_HANDLE_EVAL_PROP)
    {
      _ra_set_error (ph, RA_ERR_WRONG_HANDLE, "libRASCH");
      return -1;
    }
  if (!vh || !ra_value_is_ok (vh))
    {
      _ra_set_error (ph, RA_ERR_INFO_MISSING, "libRASCH");
      return -1;
    }
  /* TODO: think how to handle "compatible" value types */
/*  	if (p->value_type != ra_value_get_type(vh)) */
/*  	{ */
/*  		_ra_set_error(ph, RA_ERR_EVAL_WRONG_TYPE, "libRASCH"); */
/*  		return -1; */
/*  	} */

  idx = get_event_idx (p->evclass, event_id, last_event_idx);
  if (idx < 0)
    {
      _ra_set_error (ph, RA_ERR_OUT_OF_RANGE, "libRASCH");
      return -1;
    }
  last_event_idx = idx;

  col = -1;
  for (l = 0; l < (long) p->num_ch_values; l++)
    {
      if (p->ch_map[l] == ch)
	{
	  col = l;
	  break;
	}
    }
  if (col == -1)
    {
      _ra_set_error (ph, RA_ERR_OUT_OF_RANGE, "libRASCH");
      return -1;
    }

  offset = (idx * p->num_ch_values) + col;
  num_elem = ra_value_get_num_elem (vh);
  if (num_elem < 256)
    {
      switch (p->num_type)
	{
	case RA_PROP_NUM_ELEM_TYPE_BYTE:
	  p->num_elements.b[offset] = (unsigned char) num_elem;
	  break;
	case RA_PROP_NUM_ELEM_TYPE_LONG:
	  p->num_elements.l[offset] = num_elem;
	  break;
	}
    }
  else
    {
      /* do we have the long-type array already */
      if (p->num_type != RA_PROP_NUM_ELEM_TYPE_LONG)
	{
	  /* switch to the long-type array */
	  unsigned long *num_elem_new =
	    malloc (p->evclass->num_event * p->entry_size);
	  for (l = 0; l < (long) (p->evclass->num_event * p->num_ch_values);
	       l++)
	    num_elem_new[l] = p->num_elements.b[l];
	  free (p->num_elements.b);
	  p->num_elements.l = num_elem_new;
	  p->num_type = RA_PROP_NUM_ELEM_TYPE_LONG;
	}
      p->num_elements.l[offset] = num_elem;
    }

  switch (p->value_type)
    {
    case RA_VALUE_TYPE_SHORT:
      p->value.s[offset] = ra_value_get_short (vh);
      break;
    case RA_VALUE_TYPE_LONG:
      p->value.l[offset] = ra_value_get_long (vh);
      break;
    case RA_VALUE_TYPE_DOUBLE:
      p->value.d[offset] = ra_value_get_double (vh);
      break;
    case RA_VALUE_TYPE_CHAR:
      p->value.c[offset] =
	(char *) calloc ((strlen (ra_value_get_string_utf8 (vh)) + 1),
			 sizeof (char));
      strcpy (p->value.c[offset], ra_value_get_string_utf8 (vh));
      break;
    case RA_VALUE_TYPE_SHORT_ARRAY:
      p->value.sa[offset] = (short *) malloc (sizeof (short) * num_elem);
      memcpy (p->value.sa[offset], ra_value_get_short_array (vh),
	      sizeof (short) * num_elem);
      break;
    case RA_VALUE_TYPE_LONG_ARRAY:
      p->value.la[offset] = (long *) malloc (sizeof (long) * num_elem);
      memcpy (p->value.la[offset], ra_value_get_long_array (vh),
	      sizeof (long) * num_elem);
      break;
    case RA_VALUE_TYPE_DOUBLE_ARRAY:
      p->value.da[offset] = (double *) malloc (sizeof (double) * num_elem);
      memcpy (p->value.da[offset], ra_value_get_double_array (vh),
	      sizeof (double) * num_elem);
      break;
    case RA_VALUE_TYPE_CHAR_ARRAY:
      p->value.ca[offset] = (char **) malloc (sizeof (char *) * num_elem);
      ca = ra_value_get_string_array_utf8 (vh);
      for (l = 0; l < (long) num_elem; l++)
	{
	  p->value.ca[offset][l] =
	    (char *) calloc (strlen (ca[l]) + 1, sizeof (char));
	  strcpy (p->value.ca[offset][l], ca[l]);
	}
      break;
    default:
      _ra_set_error (ph, RA_ERR_ERROR, "libRASCH");
      return -1;
    }


  return 0;
}				/* ra_prop_set_value() */


/**
 * \brief set multiple values in event-property
 * \param <ph> event-property handle
 * \param <event_id> array containing event-id's
 * \param <ch> array containing channel number's
 * \param <vh> contains values to be set
 *
 * The function set the values stored in 'vh' in event-property 'ph' for
 * the event-id's 'event_id' and channel's 'ch'. If the value is channel independent
 * use '-1' for the channel numbers. The length of 'event_id' and 'ch' has to be
 * the same. And the number of values in 'vh' has to be the same as in 'event_id'/'ch'.
 */
LIBRAAPI int
ra_prop_set_value_mass (prop_handle ph, const long *event_id, const long *ch,
			value_handle vh)
{
  int ret;
  struct eval_property *p = (struct eval_property *) ph;
  long num, l, m, *idx, *col, last_idx;

  if (!ph)
    return -1;

  _ra_set_error (ph, RA_ERR_NONE, "libRASCH");

  if (p->handle_id != RA_HANDLE_EVAL_PROP)
    {
      _ra_set_error (ph, RA_ERR_WRONG_HANDLE, "libRASCH");
      return -1;
    }
  if (!vh || !ra_value_is_ok (vh))
    {
      _ra_set_error (ph, RA_ERR_INFO_MISSING, "libRASCH");
      return -1;
    }
  if ((p->value_type != RA_VALUE_TYPE_SHORT) &&
      (p->value_type != RA_VALUE_TYPE_LONG) &&
      (p->value_type != RA_VALUE_TYPE_DOUBLE))
    {
      _ra_set_error (ph, RA_ERR_UNSUPPORTED, "libRASCH");
      return -1;
    }
  if (((p->value_type == RA_VALUE_TYPE_SHORT)
       && (ra_value_get_type (vh) != RA_VALUE_TYPE_SHORT_ARRAY))
      || ((p->value_type == RA_VALUE_TYPE_LONG)
	  && (ra_value_get_type (vh) != RA_VALUE_TYPE_LONG_ARRAY))
      || ((p->value_type == RA_VALUE_TYPE_DOUBLE)
	  && (ra_value_get_type (vh) != RA_VALUE_TYPE_DOUBLE_ARRAY)))
    {
      _ra_set_error (ph, RA_ERR_WRONG_INPUT, "libRASCH");
      return -1;
    }

  num = ra_value_get_num_elem (vh);
  idx = malloc (sizeof (long) * num);
  col = malloc (sizeof (long) * num);
  last_idx = -1;
  for (l = 0; l < num; l++)
    {
      idx[l] = get_event_idx (p->evclass, event_id[l], last_idx);
      last_idx = idx[l];

      col[l] = -1;
      for (m = 0; m < (long) p->num_ch_values; m++)
	{
	  if (p->ch_map[m] == ch[l])
	    {
	      col[l] = m;
	      break;
	    }
	}
      if (col[l] == -1)
	{
	  fprintf (stderr,
		   "eval.c: given channel (ch=%ld) is not a valid one\n",
		   ch[l]);
	  free (idx);
	  free (col);
	  _ra_set_error (ph, RA_ERR_WRONG_INPUT, "libRASCH");
	  return -1;
	}
    }

  switch (p->value_type)
    {
    case RA_VALUE_TYPE_SHORT:
      ret = set_short_events_mass (p, idx, col, vh, num);
      break;
    case RA_VALUE_TYPE_LONG:
      ret = set_long_events_mass (p, idx, col, vh, num);
      break;
    case RA_VALUE_TYPE_DOUBLE:
      ret = set_double_events_mass (p, idx, col, vh, num);
      break;
    default:
      ret = -1;
      _ra_set_error (ph, RA_ERR_ERROR_INTERNAL, "libRASCH");
      break;
    }

  free (idx);
  free (col);

  return ret;
}				/* ra_prop_set_value_mass() */


int
set_short_events_mass (struct eval_property *p, const long *idx,
		       const long *col, value_handle vh, long num)
{
  const short *v;
  long l, offset;

  v = ra_value_get_short_array (vh);

  for (l = 0; l < num; l++)
    {
      long idx_c = idx[l];
      long col_c = col[l];

      offset = (idx_c * p->num_ch_values) + col_c;
      p->value.s[offset] = v[l];
      p->num_elements.b[offset] = 1;	/* only single-values are allowed in mass functions */
    }

  return 0;
}				/* set_short_events_mass() */


int
set_long_events_mass (struct eval_property *p, const long *idx,
		      const long *col, value_handle vh, long num)
{
  const long *v;
  long l, offset;

  v = ra_value_get_long_array (vh);

  for (l = 0; l < num; l++)
    {
      long idx_c = idx[l];
      long col_c = col[l];

      offset = (idx_c * p->num_ch_values) + col_c;
      p->value.l[offset] = v[l];
      p->num_elements.b[offset] = 1;	/* only single-values are allowed in mass functions */
    }

  return 0;
}				/* set_long_events_mass() */


int
set_double_events_mass (struct eval_property *p, const long *idx,
			const long *col, value_handle vh, long num)
{
  const double *v;
  long l, offset;

  v = ra_value_get_double_array (vh);

  for (l = 0; l < num; l++)
    {
      long idx_c = idx[l];
      long col_c = col[l];

      offset = (idx_c * p->num_ch_values) + col_c;
      p->value.d[offset] = v[l];
      p->num_elements.b[offset] = 1;	/* only single-values are allowed in mass functions */
    }

  return 0;
}				/* set_double_events_mass() */


/**
 * \brief returns the channels for which data is available
 * \param <ph> event-property handle
 * \param <event_id> event-id
 * \param <vh> ra-value receiving the channel-list
 *
 * The function return in 'vh' the list of channels where values are available
 * for event-id 'event_id' and event-property 'ph'.
 */
LIBRAAPI int
ra_prop_get_ch (prop_handle ph, long event_id, value_handle vh)
{
  struct eval_property *p = (struct eval_property *) ph;
  short *ch = NULL;
  unsigned long l, num_valid, offset;
  long idx;
  static long last_event_idx;

  if (!ph)
    return -1;

  _ra_set_error (ph, RA_ERR_NONE, "libRASCH");

  if (p->handle_id != RA_HANDLE_EVAL_PROP)
    {
      _ra_set_error (ph, RA_ERR_WRONG_HANDLE, "libRASCH");
      return -1;
    }
  idx = get_event_idx (p->evclass, event_id, last_event_idx);
  if (idx < 0)
    {
      _ra_set_error (ph, RA_ERR_OUT_OF_RANGE, "libRASCH");
      return -1;
    }
  last_event_idx = idx;

  num_valid = 0;
  ch = malloc (sizeof (short) * p->num_ch_values);
  offset = idx * p->num_ch_values;
  switch (p->value_type)
    {
    case RA_VALUE_TYPE_SHORT:
      for (l = 0; l < p->num_ch_values; l++)
	{
	  if (p->num_elements.b[offset + l] == 0)
	    continue;
	  ch[num_valid] = p->ch_map[l];
	  num_valid++;
	}
      break;
    case RA_VALUE_TYPE_LONG:
      for (l = 0; l < p->num_ch_values; l++)
	{
	  if (p->num_elements.b[offset + l] == 0)
	    continue;
	  ch[num_valid] = p->ch_map[l];
	  num_valid++;
	}
      break;
    case RA_VALUE_TYPE_DOUBLE:
      for (l = 0; l < p->num_ch_values; l++)
	{
	  if (p->num_elements.b[offset + l] == 0)
	    continue;
	  ch[num_valid] = p->ch_map[l];
	  num_valid++;
	}
      break;
    case RA_VALUE_TYPE_CHAR:
      for (l = 0; l < p->num_ch_values; l++)
	{
	  if (p->value.c[offset + l] == NULL)
	    continue;
	  ch[num_valid] = p->ch_map[l];
	  num_valid++;
	}
      break;
    case RA_VALUE_TYPE_SHORT_ARRAY:
      for (l = 0; l < p->num_ch_values; l++)
	{
	  if (p->value.sa[offset + l] == NULL)
	    continue;
	  ch[num_valid] = p->ch_map[l];
	  num_valid++;
	}
      break;
    case RA_VALUE_TYPE_LONG_ARRAY:
      for (l = 0; l < p->num_ch_values; l++)
	{
	  if (p->value.la[offset + l] == NULL)
	    continue;
	  ch[num_valid] = p->ch_map[l];
	  num_valid++;
	}
      break;
    case RA_VALUE_TYPE_DOUBLE_ARRAY:
      for (l = 0; l < p->num_ch_values; l++)
	{
	  if (p->value.da[offset + l] == NULL)
	    continue;
	  ch[num_valid] = p->ch_map[l];
	  num_valid++;
	}
      break;
    case RA_VALUE_TYPE_CHAR_ARRAY:
      for (l = 0; l < p->num_ch_values; l++)
	{
	  if (p->value.ca[offset + l] == NULL)
	    continue;
	  ch[num_valid] = p->ch_map[l];
	  num_valid++;
	}
      break;
    default:
      _ra_set_error (ph, RA_ERR_ERROR, "libRASCH");
      free (ch);
      return -1;
    }

  ra_value_set_short_array (vh, ch, num_valid);

  free (ch);

  return 0;
}				/* ra_prop_get_ch() */


/**
 * \brief get value from event-property
 * \param <ph> event-property handle
 * \param <event_id> event-id
 * \param <ch> channel number
 * \param <vh> receives value
 *
 * The function returns a value in 'vh' from event-property 'ph' for
 * event-id 'event_id' and channel 'ch'. If you interested in the channel
 * independent value use '-1' for the channel number.
 */
LIBRAAPI int
ra_prop_get_value (prop_handle ph, long event_id, long ch, value_handle vh)
{
  int ret = 0;
  static long last_event_idx = -1;
  struct eval_property *p = (struct eval_property *) ph;
  long l, idx, col, col_def, col_use, offset, num_elem;
  int val_ok, def_ok;

  if (!ph)
    return -1;

  _ra_set_error (ph, RA_ERR_NONE, "libRASCH");

  if (p->handle_id != RA_HANDLE_EVAL_PROP)
    {
      _ra_set_error (ph, RA_ERR_WRONG_HANDLE, "libRASCH");
      return -1;
    }
  idx = get_event_idx (p->evclass, event_id, last_event_idx);
  if (idx < 0)
    {
      _ra_set_error (ph, RA_ERR_OUT_OF_RANGE, "libRASCH");
      return -1;
    }
  last_event_idx = idx;

  col = -1;
  col_def = 0;
  for (l = 0; l < (long) p->num_ch_values; l++)
    {
      if (p->ch_map[l] == ch)
	{
	  col = l;
	  break;
	}
    }

  /* check if column was found; if not then check if overall-value is available and then use this value */

  val_ok = def_ok = 0;
  offset = idx * p->num_ch_values;
  switch (p->value_type)
    {
    case RA_VALUE_TYPE_SHORT:
      if (p->num_elements.b[offset] > 0)
	def_ok = 1;
      if ((col >= 0) && (p->num_elements.b[offset + col] > 0))
	val_ok = 1;
      break;
    case RA_VALUE_TYPE_LONG:
      if (p->num_elements.b[offset] > 0)
	def_ok = 1;
      if ((col >= 0) && (p->num_elements.b[offset + col] > 0))
	val_ok = 1;
      break;
    case RA_VALUE_TYPE_DOUBLE:
      if (p->num_elements.b[offset] > 0)
	def_ok = 1;
      if ((col >= 0) && (p->num_elements.b[offset + col] > 0))
	val_ok = 1;
      break;
    case RA_VALUE_TYPE_CHAR:
      if (p->value.c[offset] != NULL)
	def_ok = 1;
      if ((col >= 0) && (p->value.c[offset + col] != NULL))
	val_ok = 1;
      break;
    case RA_VALUE_TYPE_SHORT_ARRAY:
      if (p->value.sa[offset] != NULL)
	def_ok = 1;
      if ((col >= 0) && (p->value.sa[offset + col] != NULL))
	val_ok = 1;
      break;
    case RA_VALUE_TYPE_LONG_ARRAY:
      if (p->value.la[offset] != NULL)
	def_ok = 1;
      if ((col >= 0) && (p->value.la[offset + col] != NULL))
	val_ok = 1;
      break;
    case RA_VALUE_TYPE_DOUBLE_ARRAY:
      if (p->value.da[offset] != NULL)
	def_ok = 1;
      if ((col >= 0) && (p->value.da[offset + col] != NULL))
	val_ok = 1;
      break;
    case RA_VALUE_TYPE_CHAR_ARRAY:
      if (p->value.ca[offset] != NULL)
	def_ok = 1;
      if ((col >= 0) && (p->value.ca[offset + col] != NULL))
	val_ok = 1;
      break;
    default:
      _ra_set_error (ph, RA_ERR_ERROR, "libRASCH");
      return -1;
    }

  if (!def_ok && !val_ok)
    {
      _ra_set_error (ph, RA_ERR_OUT_OF_RANGE, "libRASCH");
      return -1;
    }

  col_use = col;
  if (!val_ok)
    {
      col_use = col_def;
      ret = RA_WARN_CH_INDEP_PROP_VALUE;
    }

  /* get value */
  offset = (idx * p->num_ch_values) + col_use;
  num_elem = 0;
  switch (p->num_type)
    {
    case RA_PROP_NUM_ELEM_TYPE_BYTE:
      num_elem = p->num_elements.b[offset];
      break;
    case RA_PROP_NUM_ELEM_TYPE_LONG:
      num_elem = p->num_elements.l[offset];
      break;
    }
  switch (p->value_type)
    {
    case RA_VALUE_TYPE_SHORT:
      ra_value_set_short (vh, p->value.s[offset]);
      break;
    case RA_VALUE_TYPE_LONG:
      ra_value_set_long (vh, p->value.l[offset]);
      break;
    case RA_VALUE_TYPE_DOUBLE:
      ra_value_set_double (vh, p->value.d[offset]);
      break;
    case RA_VALUE_TYPE_CHAR:
      ra_value_set_string_utf8 (vh, p->value.c[offset]);
      break;
    case RA_VALUE_TYPE_SHORT_ARRAY:
      ra_value_set_short_array (vh, p->value.sa[offset], num_elem);
      break;
    case RA_VALUE_TYPE_LONG_ARRAY:
      ra_value_set_long_array (vh, p->value.la[offset], num_elem);
      break;
    case RA_VALUE_TYPE_DOUBLE_ARRAY:
      ra_value_set_double_array (vh, p->value.da[offset], num_elem);
      break;
    case RA_VALUE_TYPE_CHAR_ARRAY:
      ra_value_set_string_array_utf8 (vh, (const char **) p->value.ca[offset],
				      num_elem);
      break;
    default:
      _ra_set_error (ph, RA_ERR_ERROR, "libRASCH");
      return -1;
    }

  return ret;
}				/* ra_prop_get_value() */


LIBRAAPI int
ra_prop_get_value_some (prop_handle ph, long *event_ids, long num_events,
			long ch, short *ok, value_handle vh)
{
  int ret = 0;
  struct eval_property *p = (struct eval_property *) ph;
  long l, m, idx, last_idx, col, offset;
  short *s_values = NULL;
  long *l_values = NULL;
  double *d_values = NULL;
  char **strings = NULL;

  if (!ph)
    return -1;

  _ra_set_error (ph, RA_ERR_NONE, "libRASCH");

  if (p->handle_id != RA_HANDLE_EVAL_PROP)
    {
      _ra_set_error (ph, RA_ERR_WRONG_HANDLE, "libRASCH");
      return -1;
    }

  switch (p->value_type)
    {
    case RA_VALUE_TYPE_SHORT:
      s_values = malloc (sizeof (short) * num_events);
      break;
    case RA_VALUE_TYPE_LONG:
      l_values = malloc (sizeof (long) * num_events);
      break;
    case RA_VALUE_TYPE_DOUBLE:
      d_values = malloc (sizeof (double) * num_events);
      break;
    case RA_VALUE_TYPE_CHAR:
      strings = calloc (num_events, sizeof (char **));
      break;
    default:
      ret = -1;
      _ra_set_error (ph, RA_ERR_UNSUPPORTED, "libRASCH");
      return -1;
    }

  col = -1;
  for (m = 0; m < (long) p->num_ch_values; m++)
    {
      if (p->ch_map[m] == ch)
	{
	  col = m;
	  break;
	}
    }
  if (col == -1)
    {
      _ra_set_error (ph, RA_ERR_OUT_OF_RANGE, "libRASCH");
      return -1;
    }

  last_idx = -1;
  for (l = 0; l < num_events; l++)
    {
      idx = get_event_idx (p->evclass, event_ids[l], last_idx);
      if (idx < 0)
	{
	  _ra_set_error (ph, RA_ERR_OUT_OF_RANGE, "libRASCH");
	  return -1;
	}
      last_idx = idx;

      offset = idx * p->num_ch_values + col;
      switch (p->value_type)
	{
	case RA_VALUE_TYPE_SHORT:
	  if (p->num_elements.b[offset] > 0)
	    {
	      s_values[l] = p->value.s[offset];
	      ok[l] = 1;
	    }
	  break;
	case RA_VALUE_TYPE_LONG:
	  if (p->num_elements.b[offset] > 0)
	    {
	      l_values[l] = p->value.l[offset];
	      ok[l] = 1;
	    }
	  break;
	case RA_VALUE_TYPE_DOUBLE:
	  if (p->num_elements.b[offset] > 0)
	    {
	      d_values[l] = p->value.d[offset];
	      ok[l] = 1;
	    }
	  break;
	case RA_VALUE_TYPE_CHAR:
	  if (p->value.c[offset] != NULL)
	    {
	      strings[l] =
		malloc (sizeof (char) * (strlen (p->value.c[offset]) + 1));
	      strcpy (strings[l], p->value.c[offset]);
	      ok[l] = 1;
	    }
	  break;
	}
    }

  switch (p->value_type)
    {
    case RA_VALUE_TYPE_SHORT:
      ra_value_set_short_array (vh, s_values, num_events);
      free (s_values);
      break;
    case RA_VALUE_TYPE_LONG:
      ra_value_set_long_array (vh, l_values, num_events);
      free (l_values);
      break;
    case RA_VALUE_TYPE_DOUBLE:
      ra_value_set_double_array (vh, d_values, num_events);
      free (d_values);
      break;
    case RA_VALUE_TYPE_CHAR:
      ra_value_set_string_array_utf8 (vh, (const char **) strings,
				      num_events);
      for (l = 0; l < num_events; l++)
	{
	  if (strings[l] != NULL)
	    free (strings[l]);
	}
      free (strings);
      break;
    }

  return ret;
}				/* ra_prop_get_value_some() */


LIBRAAPI int
ra_prop_get_value_all (prop_handle ph, value_handle id, value_handle ch,
		       value_handle value)
{
  int ret;
  struct eval_property *p = (struct eval_property *) ph;
  long num_complete;
  long *event_id = NULL;
  long *ch_num = NULL;

  if (!ph)
    return -1;

  _ra_set_error (ph, RA_ERR_NONE, "libRASCH");

  if (!value)
    {
      _ra_set_error (ph, RA_ERR_INFO_MISSING, "libRASCH");
      return -1;
    }

  if (p->handle_id != RA_HANDLE_EVAL_PROP)
    {
      _ra_set_error (ph, RA_ERR_WRONG_HANDLE, "libRASCH");
      return -1;
    }
  if ((p->value_type != RA_VALUE_TYPE_SHORT) &&
      (p->value_type != RA_VALUE_TYPE_LONG) &&
      (p->value_type != RA_VALUE_TYPE_DOUBLE))
    {
      _ra_set_error (ph, RA_ERR_UNSUPPORTED, "libRASCH");
      return -1;
    }

  num_complete = p->evclass->num_event * p->num_ch_values;

  /* if no id AND ch values are wanted, skip the step to get them */
  if ((id != NULL) || (ch != NULL))
    {
      event_id = malloc (sizeof (long) * num_complete);
      ch_num = malloc (sizeof (long) * num_complete);
    }
  else
    event_id = ch_num = NULL;

  switch (p->value_type)
    {
    case RA_VALUE_TYPE_SHORT:
      ret = get_short_events_all (p, value, event_id, ch_num, num_complete);
      break;
    case RA_VALUE_TYPE_LONG:
      ret = get_long_events_all (p, value, event_id, ch_num, num_complete);
      break;
    case RA_VALUE_TYPE_DOUBLE:
      ret = get_double_events_all (p, value, event_id, ch_num, num_complete);
      break;
    default:
      ret = -1;
      _ra_set_error (ph, RA_ERR_ERROR_INTERNAL, "libRASCH");
      return -1;
    }
  if (id && event_id)
    ra_value_set_long_array (id, event_id, ra_value_get_num_elem (value));
  free (event_id);

  if (ch && ch_num)
    ra_value_set_long_array (ch, ch_num, ra_value_get_num_elem (value));
  free (ch_num);

  return ret;
}				/* ra_prop_get_value_all() */


int
get_short_events_all (struct eval_property *p, value_handle value,
		      long *event_id, long *ch_num, long num)
{
  short *v;
  unsigned long l, m, curr, offset;

  v = malloc (sizeof (short) * num);

  curr = 0;
  for (l = 0; l < p->evclass->num_event; l++)
    {
      offset = l * p->num_ch_values;
      for (m = 0; m < p->num_ch_values; m++)
	{
	  if (p->num_elements.b[offset + m] == 0)
	    continue;

	  v[curr] = p->value.s[offset + m];
	  if (event_id)
	    event_id[curr] = p->evclass->ev[l].id;
	  if (ch_num)
	    ch_num[curr] = p->ch_map[m];
	  curr++;
	}
    }
  ra_value_set_short_array (value, v, curr);

  free (v);

  return 0;
}				/* get_short_events_all() */


int
get_long_events_all (struct eval_property *p, value_handle value,
		     long *event_id, long *ch_num, long num)
{
  long *v;
  unsigned long l, m, curr, offset;

  v = malloc (sizeof (long) * num);

  curr = 0;
  for (l = 0; l < p->evclass->num_event; l++)
    {
      offset = l * p->num_ch_values;
      for (m = 0; m < p->num_ch_values; m++)
	{
	  if (p->num_elements.b[offset + m] == 0)
	    continue;

	  v[curr] = p->value.l[offset + m];
	  if (event_id)
	    event_id[curr] = p->evclass->ev[l].id;
	  if (ch_num)
	    ch_num[curr] = p->ch_map[m];
	  curr++;
	}
    }
  ra_value_set_long_array (value, v, curr);

  free (v);

  return 0;
}				/* get_long_events_all() */


int
get_double_events_all (struct eval_property *p, value_handle value,
		       long *event_id, long *ch_num, long num)
{
  double *v;
  unsigned long l, m, curr, offset;

  v = malloc (sizeof (double) * num);

  curr = 0;
  for (l = 0; l < p->evclass->num_event; l++)
    {
      offset = l * p->num_ch_values;
      for (m = 0; m < p->num_ch_values; m++)
	{
	  if (p->num_elements.b[offset + m] == 0)
	    continue;

	  v[curr] = p->value.d[offset + m];
	  if (event_id)
	    event_id[curr] = p->evclass->ev[l].id;
	  if (ch_num)
	    ch_num[curr] = p->ch_map[m];
	  curr++;
	}
    }
  ra_value_set_double_array (value, v, curr);

  free (v);

  return 0;
}				/* get_double_events_all() */


LIBRAAPI int
ra_prop_get_events (prop_handle ph, value_handle min, value_handle max,
		    long ch, value_handle vh)
{
  int ret = -1;
  struct eval_property *p = (struct eval_property *) ph;
  long l, m, col, offset;
  int use_min, use_max;
  struct eval_class *cl = p->evclass;
  long *events, num_events;
  short min_sval, max_sval;
  long min_val, max_val;
  double min_dval, max_dval;

  if (!ph)
    return -1;

  min_sval = max_sval = 0;
  min_val = max_val = 0;
  min_dval = max_dval = 0.0;
  events = NULL;

  _ra_set_error (ph, RA_ERR_NONE, "libRASCH");
  ra_value_reset (vh);

  if (p->handle_id != RA_HANDLE_EVAL_PROP)
    {
      _ra_set_error (ph, RA_ERR_WRONG_HANDLE, "libRASCH");
      return -1;
    }

  use_min = use_max = 0;
  if (min && ra_value_is_ok (min))
    use_min = 1;
  if (max && ra_value_is_ok (max))
    use_max = 1;

  if ((p->value_type != RA_VALUE_TYPE_LONG)
      && (p->value_type != RA_VALUE_TYPE_DOUBLE)
      && (p->value_type != RA_VALUE_TYPE_SHORT))
    {
      _ra_set_error (ph, RA_ERR_UNSUPPORTED, "libRASCH");
      return -1;
    }

  if (p->value_type == RA_VALUE_TYPE_SHORT)
    {
      if (use_min)
	min_sval = ra_value_get_short (min);
      if (use_max)
	max_sval = ra_value_get_short (max);
    }
  else if (p->value_type == RA_VALUE_TYPE_LONG)
    {
      if (use_min)
	min_val = ra_value_get_long (min);
      if (use_max)
	max_val = ra_value_get_long (max);
    }
  else
    {
      if (use_min)
	min_dval = ra_value_get_double (min);
      if (use_max)
	max_dval = ra_value_get_double (max);
    }

  col = -1;
  for (m = 0; m < (long) p->num_ch_values; m++)
    {
      if (p->ch_map[m] == ch)
	{
	  col = m;
	  break;
	}
    }
  if (col == -1)
    {
      _ra_set_error (ph, RA_ERR_OUT_OF_RANGE, "libRASCH");
      goto error;
    }

  events = malloc (sizeof (long) * (cl->num_event * p->num_ch_values));
  num_events = 0;
  for (l = 0; l < (long) cl->num_event; l++)
    {
      offset = (l * p->num_ch_values) + col;

      if (p->value_type == RA_VALUE_TYPE_SHORT)
	{
	  if (p->num_elements.b[offset] == 0)
	    continue;
	  if (use_min && (p->value.s[offset] < min_sval))
	    continue;
	  if (use_max && (p->value.s[offset] > max_sval))
	    continue;
	}
      else if (p->value_type == RA_VALUE_TYPE_LONG)
	{
	  if (p->num_elements.b[offset] == 0)
	    continue;
	  if (use_min && (p->value.l[offset] < min_val))
	    continue;
	  if (use_max && (p->value.l[offset] > max_val))
	    continue;
	}
      else
	{
	  if (p->num_elements.b[offset] == 0)
	    continue;
	  if (use_min && (p->value.d[offset] < min_dval))
	    continue;
	  if (use_max && (p->value.d[offset] > max_dval))
	    continue;
	}
      num_events++;
      events[num_events - 1] = cl->ev[l].id;
    }

  ra_value_set_long_array (vh, events, num_events);

  ret = 0;

error:
  if (events)
    free (events);

  return ret;
}				/* ra_prop_get_events() */


/* ------------------------------ event-summary functions ------------------------------ */


LIBRAAPI sum_handle
ra_sum_add (class_handle clh, const char *id, const char *name,
	    const char *desc, long num_dim, const char **dim_name,
	    const char **dim_unit)
{
  struct eval_class *c = (struct eval_class *) clh;
  value_handle vh = NULL;
  struct eval_summary *s = NULL;
  long l;

  if (!clh)
    return NULL;

  _ra_set_error (clh, RA_ERR_NONE, "libRASCH");

  s = calloc (1, sizeof (struct eval_summary));
  ra_list_add ((void **) &(c->summaries), s);

  s->handle_id = RA_HANDLE_EVAL_SUMMARY;
  strncpy (s->ascii_id, id, MAX_ID_LEN);
  s->meas = c->meas;
  s->evclass = c;

  vh = ra_value_malloc ();

  if (name)
    {
      ra_value_set_string (vh, name);
      if (ra_eval_attribute_set (s, "name", vh) != 0)
	goto error;
    }
  if (desc)
    {
      ra_value_set_string (vh, desc);
      if (ra_eval_attribute_set (s, "description", vh) != 0)
	goto error;
    }

  ra_value_free (vh);

  s->num_dim = num_dim;
  if (dim_unit)
    s->dim_unit = (char **) calloc (num_dim, sizeof (char *));
  if (dim_name)
    s->dim_name = (char **) calloc (num_dim, sizeof (char *));

  for (l = 0; l < num_dim; l++)
    {
      if (dim_unit)
	{
	  s->dim_unit[l] =
	    (char *) calloc (strlen (dim_unit[l]) + 1, sizeof (char));
	  strcpy (s->dim_unit[l], dim_unit[l]);
	}
      if (dim_name)
	{
	  s->dim_name[l] =
	    (char *) calloc (strlen (dim_name[l]) + 1, sizeof (char));
	  strcpy (s->dim_name[l], dim_name[l]);
	}
    }

  return s;

error:
  ra_sum_delete (s);

  if (vh)
    ra_value_free (vh);

  return NULL;
}				/* ra_sum_add() */


LIBRAAPI int
ra_sum_add_ch (sum_handle sh, long ch, long fiducial_offset)
{
  struct eval_summary *s = (struct eval_summary *) sh;
  struct eval_sum_ch_desc *cd;

  if (!sh)
    return -1;

  _ra_set_error (sh, RA_ERR_NONE, "libRASCH");

  s->num_ch++;
  s->ch_desc =
    (struct eval_sum_ch_desc *) realloc (s->ch_desc,
					 sizeof (struct eval_sum_ch_desc) *
					 s->num_ch);
  cd = &(s->ch_desc[s->num_ch - 1]);

  cd->ch = ch;
  cd->fiducial_offset = fiducial_offset;

  s->num_data_elements = s->num_ch * s->num_dim;

  return 0;
}				/* ra_sum_add_ch() */


LIBRAAPI int
ra_sum_delete (sum_handle sh)
{
  unsigned long l;
  struct eval_summary *s = (struct eval_summary *) sh;
  struct eval_sum_data *d = s->sum;

  if (!sh)
    return -1;

  _ra_set_error (sh, RA_ERR_NONE, "libRASCH");

  while (d)
    {
      ra_sum_del_part (sh, d->id);
      d = s->sum;
    }

  if (s->ch_desc)
    free (s->ch_desc);

  for (l = 0; l < s->num_dim; l++)
    {
      if (s->dim_name && s->dim_name[l])
	free (s->dim_name[l]);
      if (s->dim_unit && s->dim_unit[l])
	free (s->dim_unit[l]);
    }
  if (s->dim_name)
    free (s->dim_name);
  if (s->dim_unit)
    free (s->dim_unit);

  ra_list_del ((void **) &(s->evclass->summaries), s);
  free (s);

  return 0;
}				/* ra_sum_delete() */


LIBRAAPI int
ra_sum_get (class_handle clh, const char *id, value_handle vh)
{
  struct eval_class *c = (struct eval_class *) clh;
  struct eval_summary *s;
  long len, len_use, l;
  void **vp = NULL;

  if (!clh)
    return -1;

  _ra_set_error (clh, RA_ERR_NONE, "libRASCH");
  ra_value_reset (vh);

  s = c->summaries;

  if (s == NULL)
    return 0;

  len = ra_list_len (c->summaries);
  if (len == 0)
    return 0;

  vp = (void **) calloc (len, sizeof (void *));
  if ((id == NULL) || (id[0] == '\0'))
    {
      for (l = 0; l < len; l++)
	{
	  vp[l] = (void *) s;
	  s = s->next;
	}
      len_use = len;
    }
  else
    {
      len_use = 0;
      for (l = 0; l < len; l++)
	{
	  if (strcmp (s->ascii_id, id) == 0)
	    {
	      vp[len_use] = (void *) s;
	      len_use++;
	    }
	}
    }

  ra_value_set_voidp_array (vh, (const void **) vp, len_use);

  free (vp);

  return 0;
}				/* ra_sum_get() */


LIBRAAPI long
ra_sum_add_part (sum_handle sh, long num_events, const long *events_based_on)
{
  unsigned long l;
  struct eval_summary *s = (struct eval_summary *) sh;
  struct eval_sum_data *d;

  if (!sh)
    return -1;

  _ra_set_error (sh, RA_ERR_NONE, "libRASCH");

  d = (struct eval_sum_data *) malloc (sizeof (struct eval_sum_data));
  ra_list_add ((void **) &(s->sum), d);

  d->id = s->last_data_id + 1;
  s->last_data_id = d->id;

  d->num_events = num_events;
  d->event_ids = (long *) malloc (sizeof (long) * num_events);
  memcpy (d->event_ids, events_based_on, sizeof (long) * num_events);

  d->data =
    (value_handle *) calloc (s->num_data_elements, sizeof (value_handle));
  for (l = 0; l < s->num_data_elements; l++)
    d->data[l] = ra_value_malloc ();

  return d->id;
}				/* ra_sum_add_part() */


LIBRAAPI int
ra_sum_del_part (sum_handle sh, long part_id)
{
  unsigned long l;
  struct eval_summary *s = (struct eval_summary *) sh;
  struct eval_sum_data *d = s->sum;

  if (!sh)
    return -1;

  _ra_set_error (sh, RA_ERR_NONE, "libRASCH");

  while (d)
    {
      if (d->id == part_id)
	break;
      d = d->next;
    }
  if (d == NULL)
    {
      _ra_set_error (sh, RA_ERR_OUT_OF_RANGE, "libRASCH");
      return -1;
    }

  if (d->event_ids)
    free (d->event_ids);
  d->event_ids = NULL;

  if (d->data)
    {
      for (l = 0; l < s->num_data_elements; l++)
	ra_value_free (d->data[l]);

      free (d->data);
      d->data = NULL;
    }

  ra_list_del ((void **) &(s->sum), d);
  free (d);

  return 0;
}				/* ra_sum_del_part() */


LIBRAAPI int
ra_sum_get_part_all (sum_handle sh, value_handle vh)
{
  long *part_id = NULL;
  unsigned long num, cnt;
  struct eval_summary *s = (struct eval_summary *) sh;
  struct eval_sum_data *d = s->sum;

  if (!sh)
    return -1;

  ra_value_reset (vh);
  _ra_set_error (sh, RA_ERR_NONE, "libRASCH");

  num = ra_list_len (d);
  if (num <= 0)
    return 0;

  part_id = malloc (sizeof (long) * num);

  cnt = 0;
  while (d)
    {
      if (cnt >= num)
	break;

      part_id[cnt++] = d->id;

      d = d->next;
    }

  ra_value_set_long_array (vh, part_id, num);

  free (part_id);

  return 0;
}				/* ra_sum_get_part_all() */


LIBRAAPI int
ra_sum_set_part_data (sum_handle sh, long part_id, long channel, long dim,
		      value_handle vh)
{
  int ret;
  long data_idx = -1;
  struct eval_summary *s = (struct eval_summary *) sh;
  struct eval_sum_data *d = s->sum;

  if (!sh)
    return -1;

  _ra_set_error (sh, RA_ERR_NONE, "libRASCH");

  data_idx = channel * s->num_dim + dim;
  if ((data_idx < 0) || (data_idx >= (long) s->num_data_elements))
    {
      _ra_set_error (sh, RA_ERR_OUT_OF_RANGE, "libRASCH");
      return -1;
    }

  while (d)
    {
      if (d->id == part_id)
	break;
      d = d->next;
    }
  if (d == NULL)
    {
      _ra_set_error (sh, RA_ERR_OUT_OF_RANGE, "libRASCH");
      return -1;
    }

  ret = ra_value_copy (d->data[data_idx], vh);

  return ret;
}				/* ra_sum_set_part_data() */


LIBRAAPI int
ra_sum_get_part_data (sum_handle sh, long part_id, long channel, long dim,
		      value_handle vh)
{
  int ret;
  long data_idx = -1;
  struct eval_summary *s = (struct eval_summary *) sh;
  struct eval_sum_data *d = s->sum;

  if (!sh)
    return -1;

  _ra_set_error (sh, RA_ERR_NONE, "libRASCH");

  data_idx = channel * s->num_dim + dim;
  if ((data_idx < 0) || (data_idx >= (long) s->num_data_elements))
    {
      _ra_set_error (sh, RA_ERR_OUT_OF_RANGE, "libRASCH");
      return -1;
    }

  while (d)
    {
      if (d->id == part_id)
	break;
      d = d->next;
    }
  if (d == NULL)
    {
      _ra_set_error (sh, RA_ERR_OUT_OF_RANGE, "libRASCH");
      return -1;
    }

  ret = ra_value_copy (vh, d->data[data_idx]);

  return ret;
}				/* ra_sum_get_part_data() */


LIBRAAPI int
ra_sum_get_part_events (sum_handle sh, long part_id, value_handle vh)
{
  struct eval_summary *s = (struct eval_summary *) sh;
  struct eval_sum_data *d = s->sum;

  if (!sh)
    return -1;

  _ra_set_error (sh, RA_ERR_NONE, "libRASCH");

  while (d)
    {
      if (d->id == part_id)
	break;
      d = d->next;
    }
  if (d == NULL)
    {
      _ra_set_error (sh, RA_ERR_OUT_OF_RANGE, "libRASCH");
      return -1;
    }

  ra_value_set_long_array (vh, d->event_ids, d->num_events);

  return 0;
}				/* ra_sum_get_part_events() */


LIBRAAPI int
ra_sum_set_part_events (sum_handle sh, long part_id, value_handle vh)
{
  struct eval_summary *s = (struct eval_summary *) sh;
  struct eval_sum_data *d = s->sum;

  if (!sh)
    return -1;

  _ra_set_error (sh, RA_ERR_NONE, "libRASCH");

  if (!ra_value_is_ok (vh))
    {
      _ra_set_error (sh, RA_ERR_INFO_MISSING, "libRASCH");
      return -1;
    }

  while (d)
    {
      if (d->id == part_id)
	break;
      d = d->next;
    }
  if (d == NULL)
    {
      _ra_set_error (sh, RA_ERR_OUT_OF_RANGE, "libRASCH");
      return -1;
    }

  if (d->event_ids)
    {
      free (d->event_ids);
      d->event_ids = NULL;
    }

  d->num_events = ra_value_get_num_elem (vh);
  if (d->num_events > 0)
    {
      d->event_ids = malloc (sizeof (long) * d->num_events);
      memcpy (d->event_ids, ra_value_get_long_array (vh),
	      sizeof (long) * d->num_events);
    }

  return 0;
}				/* ra_sum_set_part_events() */
