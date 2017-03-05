/**
 * \file pl_general.c
 *
 * This file provides functions which are useful for plugins.
 *
 * \author Raphael Schneider (rasch@med1.med.tum.de)
 */

/*----------------------------------------------------------------------------
 *
 * Copyright (C) 2002-2004, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Header: /home/cvs_repos/librasch/src/shared/pl_general.c,v 1.12 2004/07/06 08:19:50 rasch Exp $
 *
 *--------------------------------------------------------------------------*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define _LIBRASCH_BUILD
#define _DEFINE_INFO_STRUCTS
#include <pl_general.h>
#include <ra_event_props.h>

#ifdef WIN32
#include <windows.h>
#endif

/**
 * \brief helper function for get_meat_info()
 * \param <id> id number
 * \param <meta_inf> pointer to a meta-info struct
 *
 * Compare function for bsearch() performed in get_meta_info().
 */
int
comp_func (const void *id, const void *meta_inf)
{
  return (*((int *) id) - ((struct meta_info *) meta_inf)->id);
}				/* comp_func() */


/**
 * \brief return meta-info
 * \param <id> id for which meta-info is looked for
 *
 * Returns meta-info for info with the id 'id'.
 */
struct meta_info *
get_meta_info (int id)
{
  int num_inf = sizeof (_meta) / sizeof (_meta[0]);
  return bsearch (&id, _meta, num_inf, sizeof (_meta[0]), &comp_func);
}				/* get_meta_minfo() */


/**
 * \brief look for info by name
 * \param <vh> value-handle receiving the info meta-infos
 * \param <c> name of the info looked for
 *
 * The function search for a given info and set the info meta-info
 * in a given value-handle.
 */
int
find_ra_info_by_name (value_handle vh, const char *c)
{
  int num_inf, i;
  struct ra_value *v = (struct ra_value *) vh;

  if (!vh || !c)
    return -1;

  num_inf = sizeof (_meta) / sizeof (_meta[0]);
  /* TODO: think about faster way to find infos (perhaps hash-values ?) */
  for (i = 0; i < num_inf; i++)
    {
      if (strcmp (_meta[i].ascii_id, c) == 0)
	break;
    }

  if (i >= num_inf)
    return -1;

/*	ra_value_reset(vh);*/

  set_meta_info (vh, _meta[i].name, _meta[i].desc, _meta[i].id);

  v->utype = _meta[i].type;

  return 0;
}				/* find_ra_info_by_name() */


/**
 * \brief returns ASCII-id for numeric id
 * \param <id> id for which is looked for
 * \param <id_ascii> pointer to char buffer receiving the ASCII-id
 *
 * The function returns (in parameter 'id_ascii') the ASCII name for
 * a numeric id.
 */
int
get_ra_info_id_ascii_by_id (long id, char *id_ascii)
{
  int num_inf, i;

  if (!id_ascii)
    return -1;

  num_inf = sizeof (_meta) / sizeof (_meta[0]);
  for (i = 0; i < num_inf; i++)
    {
      if (_meta[i].id == id)
	break;
    }

  if (i >= num_inf)
    return -1;

  strcpy (id_ascii, _meta[i].ascii_id);

  return 0;
}				/* get_ra_info_name_by_id() */


/**
 * \brief splits path in parts
 * \param <file> complete filename which will be split
 * \param <sep> file seperator used
 * \param <dir> buffer receiving the directory part
 * \param <name> buffer receiving the file name
 * \param <ext> buffer receiving the file extension
 *
 * The function splits the elements of a file-path in: (1) directory,
 * (2) file name and (3) file extension. The fileseperator can be choosen.
 */
void
split_filename (const char *file, char sep, char *dir, char *name, char *ext)
{
  const char *dir_p, *name_p, *ext_p;
  size_t len;

  dir_p = name_p = ext_p = 0;

  strcpy (dir, ".");
  name[0] = '\0';
  ext[0] = '\0';

  /* look for directory */
  name_p = strrchr (file, sep);
  if (name_p)
    {
      len = name_p - file;
      strncpy (dir, file, len);
      dir[len] = '\0';
      name_p += 1;
    }
  else
    name_p = file;

  /* look for extension */
  ext_p = strrchr (name_p, '.');
  if (ext_p)
    {
      strcpy (ext, ext_p + 1);
      len = ext_p - name_p;
    }
  else
    len = strlen (name_p);

  strncpy (name, name_p, len);
  name[len] = '\0';
}				/* split_filename() */


/**
 * \brief splits URL in parts
 * \param <url> complete URL which will be split
 * \param <protocol> buffer receiving the protocol part
 * \param <server> buffer receiving the server name
 * \param <file> buffer receiving the file part
 *
 * The function splits the elements of a URL in: (1) protocol,
 * (2) server name and (3) file name.
 */
void
split_url (const char *url, char *protocol, char *server, char *file)
{
  const char *curr, *next;
  size_t len;

  protocol[0] = '\0';
  server[0] = '\0';
  file[0] = '\0';

  curr = url;
  /* look for protocol */
  if ((next = strchr (curr, ':')) != NULL)
    {
      len = next - curr;
      strncpy (protocol, curr, len);
      curr = next + 1;
    }

  /* remove leading '/' before the server part */
  while (*curr == '/')
    curr++;

  /* look for server-name/file seperator */
  if ((next = strchr (curr, '/')) != NULL)
    strcpy (file, next + 1);
  else
    next = curr + strlen (curr);

  len = next - curr;
  strncpy (server, curr, len);
}				/* split_url() */


/** 
 * \brief set one session in a measurement (helper function)
 * \param <meas> pointer to a measuerment struct
 * \param <name> name of the session
 * \param <desc> description of the session
 *
 * Helper function to set one session with just the information needed by libRASCH.
 */
void
set_one_session (struct ra_meas *meas, const char *name, const char *desc)
{
  meas->sessions = ra_alloc_mem (sizeof (struct ra_session));
  memset (meas->sessions, 0, sizeof (struct ra_session));
  meas->num_sessions = 1;

  if (name)
    strncpy (meas->sessions[0].name, name, RA_VALUE_NAME_MAX);
  if (desc)
    strncpy (meas->sessions[0].desc, desc, RA_VALUE_DESC_MAX);

  meas->sessions[0].root_rec = ra_alloc_mem (sizeof (struct ra_rec));
  memset (meas->sessions[0].root_rec, 0, sizeof (struct ra_rec));
  meas->sessions[0].root_rec->meas = meas;
  meas->sessions[0].root_rec->handle_id = RA_HANDLE_REC;
}				/* set_one_session() */


/** 
 * \brief free recordings memory
 * \param <rh> pointer to the recording struct
 *
 * Function frees recursively the memory allocated for the recording structs.
 */
void
free_session_rec (struct ra_rec *rh)
{
  if (rh == NULL)
    return;

  if (rh->child != NULL)
    free_session_rec (rh->child);

  if (rh->next != NULL)
    free_session_rec (rh->next);

  ra_free_mem (rh);
}				/* free_session_rec */


int
fill_predef_class_info (long id, value_handle vh_id, value_handle vh_name,
			value_handle vh_desc)
{
  int n_class = sizeof (ra_event_class) / sizeof (struct event_class_desc);
  int i, idx;

  idx = -1;
  for (i = 0; i < n_class; i++)
    {
      if (ra_event_class[i].id == id)
	{
	  idx = i;
	  break;
	}
    }
  if (idx == -1)
    return -1;

  ra_value_set_string (vh_id, ra_event_class[idx].ascii_id);
  ra_value_set_string (vh_name, ra_event_class[idx].name);
  ra_value_set_string (vh_desc, ra_event_class[idx].desc);

  return 0;
}				/* fill_predef_class_info() */


int
fill_predef_class_info_ascii (const char *ascii_id, value_handle vh_name,
			      value_handle vh_desc)
{
  int n_class = sizeof (ra_event_class) / sizeof (struct event_class_desc);
  int i, idx;

  idx = -1;
  for (i = 0; i < n_class; i++)
    {
      if (strcmp (ascii_id, ra_event_class[i].ascii_id) == 0)
	{
	  idx = i;
	  break;
	}
    }
  if (idx == -1)
    return -1;

  ra_value_set_string (vh_name, ra_event_class[idx].name);
  ra_value_set_string (vh_desc, ra_event_class[idx].desc);

  return 0;
}				/* fill_predef_class_info_ascii() */


long
get_class_id (const char *ascii_id)
{
  int n = sizeof (ra_event_class) / sizeof (struct event_class_desc);
  int i, idx;

  idx = -1;
  for (i = 0; i < n; i++)
    {
      if (strcmp (ascii_id, ra_event_class[i].ascii_id) == 0)
	{
	  idx = i;
	  break;
	}
    }
  if (idx == -1)
    return -1;

  return ra_event_class[idx].id;
}				/* get_class_id() */


/** 
 * \brief get infos for pre-defined event-properties
 * \param <prop> id of pre-defined event-property
 * \param <set_id> receive id-string of event-set
 * \param <set_name> receive name of event-set
 * \param <set_desc> receive description of event-set
 * \param <event_type> receive type of event
 * \param <prop_id> receive id-string of event-property
 * \param <prop_name> receive name of event-property
 * \param <prop_desc> receive description of event-property
 * \param <val_type> receive type of event-property value
 * \param <use_minmax> receive flag if min and max receive valid values
 * \param <min> receive minimum value of event-property
 * \param <max> receive maximum value of event-property
 * \param <unit> receive unit of event-property
 *
 * Function returns (by setting values in function arguments) infos for pre-defined
 * event-properties.
 */
int
fill_predef_prop_info (int prop, value_handle vh_id, value_handle vh_type,
		       value_handle vh_len, value_handle vh_name,
		       value_handle vh_desc, value_handle vh_unit,
		       value_handle vh_use_minmax, value_handle vh_min,
		       value_handle vh_max, value_handle vh_has_ign_val,
		       value_handle vh_ign_val)
{
  int n_prop = sizeof (ra_event_prop) / sizeof (struct event_prop_desc);
  int i, idx;

  idx = -1;
  for (i = 0; i < n_prop; i++)
    {
      if (ra_event_prop[i].id == prop)
	{
	  idx = i;
	  break;
	}
    }
  if (idx == -1)
    return -1;

  ra_value_set_string (vh_id, ra_event_prop[idx].ascii_id);
  ra_value_set_long (vh_type, ra_event_prop[idx].val_type);
  ra_value_set_long (vh_len, ra_event_prop[idx].num_values);
  ra_value_set_string (vh_name, ra_event_prop[idx].name);
  ra_value_set_string (vh_desc, ra_event_prop[idx].desc);
  ra_value_set_string (vh_unit, ra_event_prop[idx].unit);
  ra_value_set_long (vh_use_minmax, ra_event_prop[idx].use_minmax);
  ra_value_set_double (vh_min, ra_event_prop[idx].min_value);
  ra_value_set_double (vh_max, ra_event_prop[idx].max_value);
  ra_value_set_long (vh_has_ign_val, ra_event_prop[idx].has_ignore_value);
  ra_value_set_double (vh_ign_val, ra_event_prop[idx].ignore_value);

  return 0;
}				/* fill_predef_prop_info() */


int
fill_predef_prop_info_ascii (const char *ascii_id, value_handle vh_type,
			     value_handle vh_len, value_handle vh_name,
			     value_handle vh_desc, value_handle vh_unit,
			     value_handle vh_use_minmax, value_handle vh_min,
			     value_handle vh_max, value_handle vh_has_ign_val,
			     value_handle vh_ign_val)
{
  int n_prop = sizeof (ra_event_prop) / sizeof (struct event_prop_desc);
  int i, idx;

  idx = -1;
  for (i = 0; i < n_prop; i++)
    {
      if (strcmp (ascii_id, ra_event_prop[i].ascii_id) == 0)
	{
	  idx = i;
	  break;
	}
    }
  if (idx == -1)
    return -1;

  ra_value_set_long (vh_type, ra_event_prop[idx].val_type);
  ra_value_set_long (vh_len, ra_event_prop[idx].num_values);
  ra_value_set_string (vh_name, ra_event_prop[idx].name);
  ra_value_set_string (vh_desc, ra_event_prop[idx].desc);
  ra_value_set_string (vh_unit, ra_event_prop[idx].unit);
  ra_value_set_long (vh_use_minmax, ra_event_prop[idx].use_minmax);
  ra_value_set_double (vh_min, ra_event_prop[idx].min_value);
  ra_value_set_double (vh_max, ra_event_prop[idx].max_value);
  ra_value_set_long (vh_has_ign_val, ra_event_prop[idx].has_ignore_value);
  ra_value_set_double (vh_ign_val, ra_event_prop[idx].ignore_value);

  return 0;
}				/* fill_predef_prop_info_ascii() */


long
get_prop_id (const char *ascii_id)
{
  int n = sizeof (ra_event_prop) / sizeof (struct event_prop_desc);
  int i, idx;

  idx = -1;
  for (i = 0; i < n; i++)
    {
      if (strcmp (ascii_id, ra_event_prop[i].ascii_id) == 0)
	{
	  idx = i;
	  break;
	}
    }
  if (idx == -1)
    return -1;

  return ra_event_prop[idx].id;
}				/* get_prop_id() */


struct intersect_long
{
  long idx;
  long value;
};				/* struct intersect_long */

int
sort_intersection_long (const void *v1, const void *v2)
{
  struct intersect_long *s1, *s2;

  s1 = (struct intersect_long *) v1;
  s2 = (struct intersect_long *) v2;

  return (int) (s1->value - s2->value);
}				/* sort_intersection() */


int
sort_long_pl_general (const void *v1, const void *v2)
{
  long l1, l2;
  l1 = *((long *) v1);
  l2 = *((long *) v2);

  return (int) (l1 - l2);
}				/* sort_long_pl_general() */


int
get_intersection_long (const long *v1, long num1, const long *v2, long num2,
		       long **idx_v2_in_v1, long *num_idx)
{
  struct intersect_long *s1, *s2;
  long l, *tmp, curr_v1_idx;

  if ((v1 == NULL) || (num1 <= 0) || (v2 == NULL) || (num2 <= 0)
      || (idx_v2_in_v1 == NULL) || (num_idx == 0))
    {
      return -1;
    }
  *idx_v2_in_v1 = NULL;
  *num_idx = 0;

  /* prepare data: store values with the index given in the original array
     and sort the values */
  s1 = malloc (sizeof (struct intersect_long) * num1);
  for (l = 0; l < num1; l++)
    {
      s1[l].idx = l;
      s1[l].value = v1[l];
    }
  qsort (s1, num1, sizeof (struct intersect_long), sort_intersection_long);

  s2 = malloc (sizeof (struct intersect_long) * num2);
  for (l = 0; l < num2; l++)
    {
      s2[l].idx = l;
      s2[l].value = v2[l];
    }
  qsort (s2, num2, sizeof (struct intersect_long), sort_intersection_long);

  tmp = malloc (sizeof (long) * num1);
  curr_v1_idx = 0;
  for (l = 0; l < num2; l++)
    {
      /* when values in v2 given more than once, use only the first one */
      if ((l > 0) && (s2[l].value == s2[l - 1].value))
	continue;

      /* go through v1 until the value is equal or lager the current value in v2 */
      while ((curr_v1_idx < num1) && (s1[curr_v1_idx].value < s2[l].value))
	curr_v1_idx++;
      /* if we are at the end of v1, stop searching */
      if (curr_v1_idx >= num1)
	break;

      if (s1[curr_v1_idx].value == s2[l].value)
	{
	  /* found a value, store index in the sorted "array" */
	  tmp[*num_idx] = curr_v1_idx;
	  (*num_idx)++;
	}
    }

  /* get index in v1 */
  *idx_v2_in_v1 = ra_alloc_mem (sizeof (long) * (*num_idx));
  for (l = 0; l < (*num_idx); l++)
    (*idx_v2_in_v1)[l] = s1[tmp[l]].idx;

  /* sort found indeces */
  qsort (*idx_v2_in_v1, *num_idx, sizeof (long), sort_long_pl_general);

  /* clean up */
  free (s1);
  free (s2);
  free (tmp);

  return 0;
}				/* get_intersection_long() */
