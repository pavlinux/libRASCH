/**
 * \file load_xml_v0.c
 *
 * This file implements the functionality to read libRASCH evaluation files Version 0.
 *
 * \author Raphael Schneider (librasch@gmail.com)
 */

/*----------------------------------------------------------------------------
 *
 * Copyright (C) 2002-2010, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *
 *--------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <assert.h>
#include <ctype.h>

#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/parser.h>
#include "mime64.h"

#ifndef WIN32
#include <unistd.h>
#endif

#define _LIBRASCH_BUILD
/* #define _DEFINE_INFO_STRUCTS_PROP */
/* #undef _DEFINE_INFO_STRUCTS */
#include <ra_priv.h>
#include <pl_general.h>
#include <ra_linked_list.h>


struct ep_map_struct _event_class_map[] = {
  {"heartbeat", "", 1, 0, 0},
  {"rr-calibration", "", 1, 0, 0},
  {"comment", "annotation", 0, 0, 0},
  {"arrhythmia", "", 1, 0, 0},
  {"arrhythmia-original", "", 1, 0, 0},
  {"uterine-contraction", "", 1, 0, 0},
};				/* struct ep_map_struct _event_class_map[] */

int _num_ec_map_entries =
  sizeof (_event_class_map) / sizeof (_event_class_map[0]);


struct ep_map_struct _event_property_map[] = {
  {"heartbeat-flags", "", 0, 1, 0},
  {"rr-calibration-flags", "", 1, 0, 0},
  {"comment-flags", "", 0, 1, 0},
  {"arrhythmia-flags", "", 0, 1, 0},
  {"arrhythmia-original-flags", "", 0, 1, 0},
  {"uterine-contraction-flags", "", 0, 1, 0},

  {"qrs-template", "", 1, 0, 0},
  {"qrs-template-corr", "", 1, 0, 0},
  {"qrs-pos", "", 1, 0, 0},
  {"qrs-class", "qrs-annot", 0, 0, 0},
  {"qrs-ch", "", 1, 0, 0},
  {"qrs-temporal", "", 1, 0, 0},
  {"qrs-prsa-ma", "", 0, 1, 0},
  {"qrs-p-start", "", 1, 0, 0},
  {"qrs-p-max", "", 1, 0, 0},
  {"qrs-p-end", "", 1, 0, 0},
  {"qrs-q", "", 1, 0, 0},
  {"qrs-r", "", 1, 0, 0},
  {"qrs-s", "", 1, 0, 0},
  {"qrs-j", "", 1, 0, 0},
  {"qrs-t-start", "", 1, 0, 0},
  {"qrs-t-max", "", 1, 0, 0},
  {"qrs-t-end", "", 1, 0, 0},
  {"qrs-u-start", "", 1, 0, 0},
  {"qrs-u-max", "", 1, 0, 0},
  {"qrs-u-end", "", 1, 0, 0},
  {"rri", "", 1, 0, 0},
  {"rri-class", "rri-annot", 0, 0, 0},
  {"rri-refvalue", "", 1, 0, 0},
  {"rri-num-refvalue", "", 1, 0, 0},
  {"qt", "", 1, 0, 0},
  {"qta", "", 1, 0, 0},
  {"qt-rri", "", 1, 0, 0},
  {"resp-chest-mean-rri", "", 1, 0, 0},
  {"resp-chest-mean-ibi", "", 1, 0, 0},
  {"rr-template", "", 1, 0, 0},
  {"rr-systolic", "", 1, 0, 0},
  {"rr-systolic-pos", "", 1, 0, 0},
  {"rr-diastolic", "", 1, 0, 0},
  {"rr-diastolic-pos", "", 1, 0, 0},
  {"rr-mean", "", 1, 0, 0},
  {"rr-flags", "", 1, 0, 0},
  {"ibi", "", 1, 0, 0},
  {"rr-calibration-seq", "", 0, 0, 1},
  {"uc-max-pos", "", 1, 0, 0},
  {"uc-template", "", 1, 0, 0},
  {"uc-template-corr", "", 1, 0, 0},
  {"comments", "annotation", 0, 0, 0},
  {"comment-ch", "", 0, 1, 0},
  {"arr-type", "", 1, 0, 0},
  {"arr-hr", "", 1, 0, 0},
  {"arr-num-qrs", "", 1, 0, 0},
  {"arr_o-type", "", 1, 0, 0},
  {"arr_o-hr", "", 1, 0, 0},
  {"arr_o-num-qrs", "", 1, 0, 0}
};				/* struct ep_map_struct _event_property_map[] */

int _num_ep_map_entries =
  sizeof (_event_property_map) / sizeof (_event_property_map[0]);



/**
 * \brief load evalution in XML format
 * \param <mh> measurement handle
 * \param <file> name of the evaluation file (optional)
 *
 * This function loads the evaluation(s) that belongs to the measurement mh. If no filename file
 * is given, the default-filename will be used.
 */
int
eval_load_v0 (meas_handle mh, const char *file)
{
  struct ra_meas *mm = (struct ra_meas *) mh;
  char fn[MAX_PATH_RA];
  int ret;
  xmlDocPtr doc;
  FILE *fp;
  xmlChar *c;

  if (!mh)
    return -1;

  _ra_set_error (mh, RA_ERR_NONE, "libRASCH");

  if (file)
    strncpy (fn, file, MAX_PATH_RA);
  else
    get_eval_file_xml (mm, fn);

  /* first check if eval-file is available */
  fp = fopen (fn, "r");
  if (fp == NULL)
    return 0;
  fclose (fp);

  if ((doc = xmlParseFile (fn)) == NULL)
    {
      _ra_set_error (mh, RA_ERR_READ_EVAL, "libRASCH");
      return -1;
    }
  if (!doc->children
      || ((xmlStrcmp (doc->children->name, (xmlChar *) "libRASCH") != 0)
	  && (xmlStrcmp (doc->children->name, (xmlChar *) "libMTK") != 0)))
    {
      xmlFreeDoc (doc);
      _ra_set_error (mh, RA_ERR_EVAL_FILE_CORRUPT, "libRASCH");
      return -1;
    }

  c = xmlGetProp (doc->children, (xmlChar *) "version");
  if ((c == NULL) || (xmlStrcmp (c, (xmlChar *) "0") == 0))
    ret = read_evals_xml (mm, doc->children);
  else
    {
      _ra_set_error (mh, RA_ERR_EVAL_FILE_WRONG_VERSION, "libRASCH");
      ret = -1;
    }
  if (c)
    xmlFree (c);

  xmlFreeDoc (doc);

  return ret;
}				/* ra_eval_load_v0() */


/**
 * \brief returns evaluation filename
 * \param <mh> measurement handle
 * \param <f> will receive the filename of the evaluation
 *
 * If no evaluation file was given in ra_eval_loadxml, this function returns the
 * default filename used for evaluations (based on the measurement name).
 */
int
get_eval_file_xml (meas_handle mh, char *f)
{
  long dir = 0;
  value_handle vh;
  rec_handle rh;

  rh = ra_rec_get_first (mh, 0);
  vh = ra_value_malloc ();
  if (ra_info_get (rh, RA_INFO_REC_GEN_DIR_L, vh) != 0)
    {
      ra_value_free (vh);
      return -1;
    }
  dir = ra_value_get_long (vh);

  if (ra_info_get (rh, RA_INFO_REC_GEN_PATH_C, vh) != 0)
    {
      ra_value_free (vh);
      return -1;
    }

  if (dir)
    sprintf (f, "%s%ceval_ra.mte", ra_value_get_string (vh), SEPARATOR);
  else
    sprintf (f, "%s.mte", ra_value_get_string (vh));

  ra_value_free (vh);

  return 0;
}				/* get_eval_file_xml() */


/**
 * \brief reads the evaluations
 * \param <mh> measurement handle
 * \param <parent> pointer of the parent XML node
 *
 * The function reads all evaluations below the XML node 'parent'.
 */
int
read_evals_xml (meas_handle mh, xmlNodePtr parent)
{
  eval_handle eh;
  int ret = 0;
  xmlNodePtr node, child;
  xmlChar *c, *name, *desc;
  value_handle vh;

  vh = ra_value_malloc ();

  node = parent->children;
  while (node)
    {
      int original = 0;
      int def = 0;

      if (xmlStrcmp (node->name, (xmlChar *) "Evaluation") != 0)
	{
	  node = node->next;
	  continue;
	}

      /* get infos needed for ra_eval_add() function */
      name = xmlGetProp (node, (xmlChar *) "name");

      child = get_node (node, "comment");
      desc = xmlNodeGetContent (child);

      child = get_node (node, "Flags");
      if (child)
	{
	  if (xmlHasProp (child, (xmlChar *) "original") != NULL)
	    original = 1;
	  if (xmlHasProp (child, (xmlChar *) "default") != NULL)
	    def = 1;
	}

      eh =
	ra_eval_add (mh, (const char *) name, (const char *) desc, original);
      if (def == 1)
	ra_eval_set_default (eh);

      if (name)
	xmlFree (name);
      if (desc)
	xmlFree (desc);

      /* get attributes (if any) and add them to 'eh' */
      child = get_node (node, "add-ts");
      c = xmlNodeGetContent (child);
      if (c != NULL)
	{
	  fix_string ((char *) c);
	  ra_value_set_string (vh, (const char *) c);
	  ra_eval_attribute_set (eh, "add-timestamp", vh);
	  xmlFree (c);
	}
      child = get_node (node, "modify-ts");
      c = xmlNodeGetContent (child);
      if (c != NULL)
	{
	  fix_string ((char *) c);
	  ra_value_set_string (vh, (const char *) c);
	  ra_eval_attribute_set (eh, "modify-timestamp", vh);
	  xmlFree (c);
	}
      child = get_node (node, "user");
      c = xmlNodeGetContent (child);
      if (c != NULL)
	{
	  fix_string ((char *) c);
	  ra_value_set_string (vh, (const char *) c);
	  ra_eval_attribute_set (eh, "add-user", vh);
	  xmlFree (c);
	}
      child = get_node (node, "host");
      c = xmlNodeGetContent (child);
      if (c != NULL)
	{
	  fix_string ((char *) c);
	  ra_value_set_string (vh, (const char *) c);
	  ra_eval_attribute_set (eh, "add-host", vh);
	  xmlFree (c);
	}
      child = get_node (node, "program");
      c = xmlNodeGetContent (child);
      if (c != NULL)
	{
	  fix_string ((char *) c);
	  ra_value_set_string (vh, (const char *) c);
	  ra_eval_attribute_set (eh, "add-program", vh);
	  xmlFree (c);
	}

      if ((ret = read_sets_xml (eh, node)) != 0)
	return ret;

      node = node->next;
    }

  ra_value_free (vh);

  return ret;
}				/* read_evals_xml() */


void
fix_string (char *s)
{
  if (s == NULL)
    return;

  do
    {
      unsigned char c = *s;
      if (s == '\0')
	break;
      if (c == 0xCC)
	*s = '\0';
      else
	s++;
    }
  while (*s != '\0');
}				/* fix_string() */


/**
 * \brief returns the XML node named with 'name'
 * \param <parent> the childs of this XML node will be searched
 * \param <name> name of the wanted XML node
 *
 * The function returns the XML node named 'name' below the XML node 'parent'
 * (only the level directly below 'parent' is searched).
 */
xmlNodePtr
get_node (xmlNodePtr parent, char *name)
{
  xmlNodePtr child = parent->children;

  while (child)
    {
      if (xmlStrcmp (child->name, (xmlChar *) name) == 0)
	return child;
      child = child->next;
    }

  return NULL;
}				/* get_node() */


/**
 * \brief read event-sets
 * \param <e> pointer to an evaluation structure
 * \param <parent> pointer of the parent XML node
 *
 * The function reads all event-sets below the XML node 'parent'.
 */
int
read_sets_xml (eval_handle eh, xmlNodePtr parent)
{
  int ret = 0;
  class_handle clh;
  xmlNodePtr child, node = parent->children;
  xmlChar *c;
  int use_ascii = 0;
  unsigned long l, num_events;
  int type_cont = 0;
  struct event_info *evi;
  struct ep_map_struct *class_info;
  char v0_name_save[EVAL_MAX_NAME];
  const char *name_use;

  while (node)
    {
      if (xmlStrcmp (node->name, (xmlChar *) "Event-Set") != 0)
	{
	  node = node->next;
	  continue;
	}

      c = xmlGetProp (node, (xmlChar *) "name");
      if (!c)
	{
	  continue;
	  /* TODO: think if an error should be returned */
	}

      class_info =
	get_ep_map_info (_event_class_map, _num_ec_map_entries,
			 (const char *) c);
      if (class_info == NULL)
	{
	  fprintf (stderr, "unknown class %s\n", (const char *) c);
	  xmlFree (c);
	  node = node->next;
	  continue;
	}
      if (class_info->not_needed)
	{
	  xmlFree (c);
	  node = node->next;
	  continue;
	}
      if (class_info->not_avail)
	{
	  fprintf (stderr, "class %s is not supported\n", (const char *) c);
	  xmlFree (c);
	  node = node->next;
	  continue;
	}
      strncpy (v0_name_save, (const char *) c, EVAL_MAX_NAME);
      if (class_info->same)
	name_use = (const char *) c;
      else
	name_use = class_info->v1_prop_name;
      clh = ra_class_add_predef (eh, name_use);
      if (clh == NULL)
	{
	  fprintf (stderr, "class %s (%s) can not be created\n",
		   (const char *) c, name_use);
	  exit (-1);
	}
      xmlFree (c);

      child = get_node (node, "number-events");
      c = xmlNodeGetContent (child);
      if (c)
	{
	  num_events = atol ((const char *) c);
	  xmlFree (c);
	}
      else
	{
	  ra_class_delete (clh);
	  continue;
	  /* TODO: think if an error should be returned */
	}

      c = xmlGetProp (node, (xmlChar *) "event-encoding");
      if (c && (xmlStrcmp (c, (xmlChar *) "ascii") == 0))
	use_ascii = 1;

      c = xmlGetProp (node, (xmlChar *) "type");
      if (c)
	{
	  if (xmlStrcmp (c, (xmlChar *) "continues") == 0)
	    type_cont = 1;
	  xmlFree (c);
	}

      evi =
	(struct event_info *) calloc (num_events, sizeof (struct event_info));
      if (type_cont)
	ret = read_events_start_end_xml (node, evi, num_events);
      else
	{
	  for (l = 0; l < num_events; l++)
	    evi[l].start = evi[l].end = -1;
	}

      if (ret != 0)
	{
	  _ra_set_error (eh, RA_ERR_EVAL_FILE_CORRUPT, "libRASCH");
	  break;
	}

      for (l = 0; l < num_events; l++)
	evi[l].id = ra_class_add_event (clh, evi[l].start, evi[l].end);

      if ((ret = read_props_xml (clh, node, use_ascii, num_events)) != 0)
	{
	  free (evi);
	  break;
	}

/* 		if (read_templates_xml(clh, node) != 0) */
/* 		{ */
/* 			free(evi); */
/* 			return -1; */
/* 		} */

      free (evi);

      node = node->next;
    }

  return ret;
}				/* read_sets_xml() */


/**
 * \brief read start- and end-time for event-sets
 * \param <set> pointer to an event-set structure
 * \param <parent> pointer of the parent XML node
 *
 * The function reads all start- and end-times for the event-set 'set'
 * below the XML node 'parent'.
 */
int
read_events_start_end_xml (xmlNodePtr parent, struct event_info *evi,
			   unsigned long num_events)
{
  xmlChar *c;
  void *t;
  size_t size;
  xmlNodePtr curr;
  size_t l, data_size;

  data_size = num_events * sizeof (long);

  curr = get_node (parent, "event-start");
  c = xmlNodeGetContent (curr);
  if (!c)
    return -1;

  decode_base64 (c, strlen ((const char *) c), &t, &size, data_size);
  size /= sizeof (long);
  for (l = 0; l < size; l++)
    {
      if (l >= num_events)
	break;
      evi[l].start = ((long *) t)[l];
    }
  xmlFree (c);
  free (t);
  if (size != num_events)
    return -1;

  curr = get_node (parent, "event-end");
  c = xmlNodeGetContent (curr);
  if (!c)
    return -1;

  decode_base64 (c, strlen ((const char *) c), &t, &size, data_size);
  size /= sizeof (long);
  for (l = 0; l < size; l++)
    {
      if (l >= num_events)
	break;
      evi[l].end = ((long *) t)[l];
    }
  xmlFree (c);
  free (t);

  if (size != num_events)
    return -1;

  return 0;
}				/* read_events_start_end_xml() */


/**
 * \brief read event-properties
 * \param <set> pointer to an event-set structure
 * \param <parent> pointer of the parent XML node
 * \param <use_ascii> flag if event-values are stored as ASCII-text (=1) or as
 * binary MIME64 encoded data (=0)
 *
 * The function reads all event-properties below the XML node 'parent'.
 */
int
read_props_xml (class_handle clh, xmlNodePtr parent, int use_ascii,
		unsigned long num_events)
{
  xmlNodePtr node = parent->children;
  xmlChar *c;
  prop_handle prop;
  prop_handle prop_comment = NULL;
  unsigned long l;
  long src_value_type;
  struct ep_map_struct *prop_info;
  const char *name_use;
  char v0_name_save[EVAL_MAX_NAME];
  int comment_found = 0;
  value_handle vh, vh_comment, vh_com_ch;

  vh = ra_value_malloc ();
  vh_comment = ra_value_malloc ();
  vh_com_ch = ra_value_malloc ();

  while (node)
    {
      if (xmlStrcmp (node->name, (xmlChar *) "Event-Property") != 0)
	{
	  node = node->next;
	  continue;
	}

      c = xmlGetProp (node, (xmlChar *) "name");
      if (!c)
	{
	  node = node->next;
	  continue;
	}
      if (strcmp ((const char *) c, "comments") == 0)
	{
	  if (get_comment_data (node, vh_comment, num_events) != 0)
	    {
	      _ra_set_error (clh, RA_ERR_EVAL_FILE_CORRUPT, "libRASCH");
	      xmlFree (c);
	      node = node->next;
	      continue;
	    }
	  comment_found = 1;

	  prop_comment = ra_prop_add_predef (clh, "annotation");

	  xmlFree (c);
	  node = node->next;
	  continue;
	}
      if (strcmp ((const char *) c, "comment-ch") == 0)
	{
	  if (get_comment_ch_data (node, vh_com_ch, num_events) != 0)
	    _ra_set_error (clh, RA_ERR_EVAL_FILE_CORRUPT, "libRASCH");

	  xmlFree (c);
	  node = node->next;
	  continue;
	}

      prop_info =
	get_ep_map_info (_event_property_map, _num_ep_map_entries,
			 (const char *) c);
      if (prop_info == NULL)
	{
	  fprintf (stderr, "unknown property %s\n", (const char *) c);
	  xmlFree (c);
	  node = node->next;
	  continue;
	}
      if (prop_info->not_needed)
	{
	  xmlFree (c);
	  node = node->next;
	  continue;
	}
      if (prop_info->not_avail)
	{
/*			fprintf(stderr, "property %s is not supported\n", (const char *)c);*/
	  xmlFree (c);
	  node = node->next;
	  continue;
	}

      strncpy (v0_name_save, (const char *) c, EVAL_MAX_NAME);
      if (prop_info->same)
	name_use = (const char *) c;
      else
	name_use = prop_info->v1_prop_name;
      prop = ra_prop_add_predef (clh, name_use);
      if (prop == NULL)
	{
	  fprintf (stderr, "property %s (%s) can not be created\n",
		   (const char *) c, name_use);
	  exit (-1);
	}
      xmlFree (c);
      c = xmlGetProp (node, (xmlChar *) "val-type");
      src_value_type = RA_VALUE_TYPE_NONE;
      if (c)
	{
	  if (xmlStrcmp (c, (xmlChar *) "short") == 0)
	    src_value_type = RA_VALUE_TYPE_SHORT;
	  else if (xmlStrcmp (c, (xmlChar *) "long") == 0)
	    src_value_type = RA_VALUE_TYPE_LONG;
	  else if (xmlStrcmp (c, (xmlChar *) "double") == 0)
	    src_value_type = RA_VALUE_TYPE_DOUBLE;
	  else if (xmlStrcmp (c, (xmlChar *) "char") == 0)
	    src_value_type = RA_VALUE_TYPE_CHAR;
	  else if (xmlStrcmp (c, (xmlChar *) "short-array") == 0)
	    src_value_type = RA_VALUE_TYPE_SHORT_ARRAY;
	  else if (xmlStrcmp (c, (xmlChar *) "long-array") == 0)
	    src_value_type = RA_VALUE_TYPE_LONG_ARRAY;
	  else if (xmlStrcmp (c, (xmlChar *) "double-array") == 0)
	    src_value_type = RA_VALUE_TYPE_DOUBLE_ARRAY;
	  else
	    {
	      _ra_set_error (clh, RA_ERR_EVAL_FILE_CORRUPT, "libRASCH");
	      return -1;	/* file corrupt */
	    }

	  xmlFree (c);
	}

      read_events_xml (prop, src_value_type, node, use_ascii);

      /*
       * Special handling of 2 position properties.
       * 
       * Because the concept that all events have a start- and end
       * position was introduced in eval-file format 1, we have to
       * set the event-positions from values found in event-properties.
       * The following event-properties have position informations:
       * 'qrs-pos' -> heartbeats
       * 'uc-max-pos' -> 'uterine-contraction'
       */
      if ((strcmp (v0_name_save, "qrs-pos") == 0)
	  || (strcmp (v0_name_save, "uc-max-pos") == 0))
	{
	  for (l = 0; l < num_events; l++)
	    {
	      ra_prop_get_value (prop, l + 1, -1, vh);
	      ra_class_set_event_pos (clh, l + 1, ra_value_get_long (vh),
				      ra_value_get_long (vh));
	    }
	}

      node = node->next;
    }

  if (comment_found && prop_comment)
    {
      const long *annot_ch = ra_value_get_long_array (vh_com_ch);

      for (l = 0; l < num_events; l++)
	{
	  long ch_use = -1;
	  if (annot_ch)
	    ch_use = annot_ch[l];

	  ra_value_get_single_elem (vh, vh_comment, l);

	  ra_prop_set_value (prop_comment, l + 1, ch_use, vh);
	}
    }

  ra_value_free (vh);
  ra_value_free (vh_comment);
  ra_value_free (vh_com_ch);

  return 0;
}				/* read_props_xml() */


struct ep_map_struct *
get_ep_map_info (struct ep_map_struct *map, long num_entries,
		 const char *v0_prop_name)
{
  int i;
  struct ep_map_struct *curr = NULL;

  for (i = 0; i < num_entries; i++)
    {
      if (strcmp (v0_prop_name, map[i].v0_prop_name) == 0)
	{
	  curr = map + i;
	  break;
	}
    }

  return curr;
}				/* get_ep_map_info() */


int
get_comment_data (xmlNodePtr parent, value_handle vh,
		  unsigned long num_events)
{
  xmlNodePtr curr;
  xmlChar *c;
  int ret = 0;
  char *p_curr, *p_next;
  unsigned long cnt;
  char **all;

  curr = get_node (parent, "events");
  c = xmlNodeGetContent (curr);
  if (!c)
    {
      return -1;
    }

  all = malloc (sizeof (char *) * num_events);

  cnt = 0;
  p_curr = (char *) c;
  p_next = find_next_string (p_curr);
  while (p_curr)
    {
      all[cnt] = malloc (sizeof (char) * (strlen (p_curr) + 1));
      strcpy (all[cnt++], p_curr);

      if (!p_next)
	break;
      if (cnt >= num_events)
	break;

      p_curr = p_next;
      p_next = find_next_string (p_curr);
    }

  ra_value_set_string_array (vh, (const char **) all, num_events);

  return ret;
}				/* get_comment_data() */


int
get_comment_ch_data (xmlNodePtr parent, value_handle vh,
		     unsigned long num_events)
{
  xmlNodePtr curr;
  xmlChar *c;
  int ret = 0;
  long *ch;
  void *t = NULL;
  size_t size, data_size;
  unsigned long l;

  curr = get_node (parent, "events");
  c = xmlNodeGetContent (curr);
  if (!c)
    {
      return -1;
    }

  data_size = num_events * sizeof (long);
  decode_base64 (c, strlen ((const char *) c), &t, &size, data_size);
  ch = malloc (data_size);
  for (l = 0; l < num_events; l++)
    ch[l] = ((long *) t)[l];

  ra_value_set_long_array (vh, ch, num_events);

  if (t)
    free (t);

  return ret;
}				/* get_comment_ch_data() */


/**
 * \brief read event values
 * \param <prop> pointer to an event-property structure
 * \param <parent> pointer of the parent XML node
 * \param <use_ascii> flag if event-values are stored as ASCII-text (=1) or as
 * binary MIME64 encoded data (=0)
 *
 * The function reads all event-values below the XML node 'parent'.
 */
int
read_events_xml (prop_handle prop, long src_value_type, xmlNodePtr parent,
		 int use_ascii)
{
  int ret = 0;
  xmlNodePtr curr;
  xmlChar *c;
  struct eval_property *p = prop;

  curr = get_node (parent, "events");

  /* If events stored in ASCII, read them and leave function */
  if (use_ascii)
    return handle_value_events_ascii (prop, src_value_type, curr);

  /* get the event string */
  c = xmlNodeGetContent (curr);
  if (!c)
    {
      _ra_set_error (prop, RA_ERR_EVAL_FILE_CORRUPT, "libRASCH");
      return -1;
    }

  /* if not a char-event */
  if (p->value_type == RA_VALUE_TYPE_CHAR)
    ret = handle_char_events (prop, c);
  else
    ret = handle_value_events (prop, src_value_type, c);

  xmlFree (c);

  return ret;
}				/* read_events_xml() */


/**
 * \brief read event values stored as ASCII-text
 * \param <prop> pointer to an event-property structure
 * \param <parent> pointer of the parent XML node
 *
 * The function reads the event values for the event-property 'prop' below the XML node 'parent'.
 * The function expects that the values are stored as ASCII text in there own XML tag.
 */
int
handle_value_events_ascii (prop_handle prop, long src_value_type,
			   xmlNodePtr parent)
{
  xmlNodePtr node;
  xmlChar *c;
  struct eval_class *ec;
  value_handle vh;

  ec = ra_class_get_handle (prop);

  vh = ra_value_malloc ();

  node = parent->children;
  while (node)
    {
      unsigned long idx;
      c = xmlGetProp (node, (xmlChar *) "index");
      idx = atol ((const char *) c);
      xmlFree (c);
      if (idx >= ec->num_event)
	{
	  ra_value_free (vh);
	  return -1;		/* FIXME: better error handling */
	}

      switch (src_value_type)
	{
	case RA_VALUE_TYPE_SHORT:
	  read_short (node, prop, idx, vh);
	  break;
	case RA_VALUE_TYPE_LONG:
	  read_long (node, prop, idx, vh);
	  break;
	case RA_VALUE_TYPE_DOUBLE:
	  read_double (node, prop, idx, vh);
	  break;
	case RA_VALUE_TYPE_CHAR:
	  read_char (node, prop, idx, vh);
	  break;
	case RA_VALUE_TYPE_SHORT_ARRAY:
	  fprintf (stderr,
		   "critical internal error: load_xml_v0.c - handle_value_events_ascii - short array\n");
	  exit (-1);
/* 			read_short_array(node, prop, idx, vh); */
/* 			break; */
	case RA_VALUE_TYPE_LONG_ARRAY:
	  fprintf (stderr,
		   "critical internal error: load_xml_v0.c - handle_value_events_ascii - long array\n");
	  exit (-1);
/* 			read_long_array(node, prop, idx, vh); */
/* 			break; */
	case RA_VALUE_TYPE_DOUBLE_ARRAY:
	  fprintf (stderr,
		   "critical internal error: load_xml_v0.c - handle_value_events_ascii - double array\n");
	  exit (-1);
/* 			read_double_array(node, prop, idx, vh); */
/* 			break; */
	default:
	  return -1;		/* FIXME: better error handling */
	  break;
	}

      /* no need to check that node is "event" */
      node = node->next;
    }

  ra_value_free (vh);

  return 0;
}				/* handle_value_events_ascii() */


/**
 * \brief read short event value
 * \param <node> node containing the value as ASCII text
 * \param <prop> event-property
 * \param <idx> event number for which the value will be set
 *
 * The function reads a short event value and set the value in the array for
 * the property 'prop' at position 'idx'.
 */
int
read_short (xmlNodePtr node, prop_handle prop, long idx, value_handle vh)
{
  xmlNodePtr content;
  xmlChar *c;

  content = node->children;
  if (!content)
    return -1;

  c = xmlNodeGetContent (content);
  ra_value_set_short (vh, (short) atoi ((const char *) c));
  xmlFree (c);

  ra_prop_set_value (prop, idx + 1, -1, vh);

  return 0;
}				/* read_short() */


/**
 * \brief read long event value
 * \param <node> node containing the value as ASCII text
 * \param <prop> event-property
 * \param <idx> event number for which the value will be set
 *
 * The function reads a long event value and set the value in the array for
 * the property 'prop' at position 'idx'.
 */
int
read_long (xmlNodePtr node, prop_handle prop, long idx, value_handle vh)
{
  xmlNodePtr content;
  xmlChar *c;

  content = node->children;
  if (!content)
    return -1;

  c = xmlNodeGetContent (content);
  ra_value_set_long (vh, (long) atol ((const char *) c));
  xmlFree (c);

  ra_prop_set_value (prop, idx + 1, -1, vh);

  return 0;
}				/* read_long() */


/**
 * \brief read double event value
 * \param <node> node containing the value as ASCII text
 * \param <prop> event-property
 * \param <idx> event number for which the value will be set
 *
 * The function reads a double event value and set the value in the array for
 * the property 'prop' at position 'idx'.
 */
int
read_double (xmlNodePtr node, prop_handle prop, long idx, value_handle vh)
{
  xmlNodePtr content;
  xmlChar *c;

  content = node->children;
  if (!content)
    return -1;

  c = xmlNodeGetContent (content);
  ra_value_set_double (vh, xmlXPathCastStringToNumber (c));
  xmlFree (c);

  ra_prop_set_value (prop, idx + 1, -1, vh);

  return 0;
}				/* read_double() */


/**
 * \brief read string event value
 * \param <node> node containing the value as ASCII text
 * \param <prop> event-property
 * \param <idx> event number for which the value will be set
 *
 * The function reads a string event value and set the value in the array for
 * the property 'prop' at position 'idx'.
 */
int
read_char (xmlNodePtr node, prop_handle prop, long idx, value_handle vh)
{
  xmlNodePtr content;
  xmlChar *c;

  content = node->children;
  if (!content)
    return -1;

  c = xmlNodeGetContent (content);
  ra_value_set_string (vh, (const char *) c);
  xmlFree (c);

  ra_prop_set_value (prop, idx + 1, -1, vh);

  return 0;
}				/* read_char() */


/* /\** */
/*  * \brief read short array event value */
/*  * \param <node> node containing the value as ASCII text */
/*  * \param <prop> event-property */
/*  * \param <idx> event number for which the value will be set */
/*  * */
/*  * The function reads a short array event value and set the value in the array for */
/*  * the property 'prop' at position 'idx'. */
/*  *\/ */
/* int */
/* read_short_array(xmlNodePtr node, prop_handle prop, long idx, value_handle vh) */
/* { */
/* 	xmlNodePtr content; */
/* 	struct event_set *set = prop->set; */
/* 	long n_elem = 0; */
/* 	short *buf = NULL; */
/* 	short *ch_buf = NULL; */
/* 	xmlChar *c; */

/* 	content = node->children; */
/* 	if (!content) */
/* 		return -1; */

/* 	while (content) */
/* 	{ */
/* 		long idx2; */
/* 		c = xmlGetProp(content, (xmlChar *)"index"); */
/* 		idx2 = atol((const char *)c); */
/* 		xmlFree(c); */
/* 		if ((idx2+1) > n_elem) */
/* 		{ */
/* 			n_elem = idx2 + 1; */
/* 			buf = realloc(buf, sizeof(short) * n_elem); */
/* 			ch_buf = realloc(ch_buf, sizeof(short) * n_elem); */
/* 		} */

/* 		c = xmlGetProp(content, (xmlChar *)"ch"); */
/* 		ch_buf[idx2] = (short)atol((const char *)c); */
/* 		xmlFree(c); */

/* 		c = xmlNodeGetContent(content); */
/* 		buf[idx2] = (short)atoi((const char *)c); */
/* 		xmlFree(c); */

/* 		content = content->next; */
/* 	} */
/* 	if (buf) */
/* 	{ */
/* 		ra_value_set_short_array(vh,  */
/* 		set->sa[prop->idx][idx] = buf; */
/* 		set->sa_ch[prop->idx][idx] = ch_buf; */
/* 		set->sa_num_elem[prop->idx][idx] = n_elem; */
/* 	} */

/* 	return 0; */
/* } /\* read_short_array() *\/ */


/* /\** */
/*  * \brief read long array event value */
/*  * \param <node> node containing the value as ASCII text */
/*  * \param <prop> event-property */
/*  * \param <idx> event number for which the value will be set */
/*  * */
/*  * The function reads a long array event value and set the value in the array for */
/*  * the property 'prop' at position 'idx'. */
/*  *\/ */
/* int */
/* read_long_array(xmlNodePtr node, prop_handle prop, long idx, value_handle vh) */
/* { */
/* 	xmlNodePtr content; */
/* 	struct event_set *set = prop->set; */
/* 	long n_elem = 0; */
/* 	long *buf = NULL; */
/* 	short *ch_buf = NULL; */
/* 	xmlChar *c; */

/* 	content = node->children; */
/* 	if (!content) */
/* 		return -1; */

/* 	while (content) */
/* 	{ */
/* 		long idx2; */
/* 		c = xmlGetProp(content, (xmlChar *)"index"); */
/* 		idx2 = atol((const char *)c); */
/* 		xmlFree(c); */
/* 		if ((idx2+1) > n_elem) */
/* 		{ */
/* 			n_elem = idx2 + 1; */
/* 			buf = realloc(buf, sizeof(long) * n_elem); */
/* 		} */

/* 		c = xmlGetProp(content, (xmlChar *)"ch"); */
/* 		ch_buf[idx2] = (short)atol((const char *)c); */
/* 		xmlFree(c); */

/* 		c = xmlNodeGetContent(content); */
/* 		buf[idx2] = (long)atol((const char *)c); */
/* 		xmlFree(c); */

/* 		content = content->next; */
/* 	} */
/* 	if (buf) */
/* 	{ */
/* 		set->la[prop->idx][idx] = buf; */
/* 		set->la_ch[prop->idx][idx] = ch_buf; */
/* 		set->la_num_elem[prop->idx][idx] = n_elem; */
/* 	} */

/* 	return 0; */
/* } /\* read_long_array() *\/ */


/* /\** */
/*  * \brief read double array event value */
/*  * \param <node> node containing the value as ASCII text */
/*  * \param <prop> event-property */
/*  * \param <idx> event number for which the value will be set */
/*  * */
/*  * The function reads a double array event value and set the value in the array for */
/*  * the property 'prop' at position 'idx'. */
/*  *\/ */
/* int */
/* read_double_array(xmlNodePtr node, prop_handle prop, long idx, value_handle vh) */
/* { */
/* 	xmlNodePtr content; */
/* 	struct event_set *set = prop->set; */
/* 	long n_elem = 0; */
/* 	double *buf = NULL; */
/* 	short *ch_buf = NULL; */
/* 	xmlChar *c; */

/* 	content = node->children; */
/* 	if (!content) */
/* 		return -1; */

/* 	while (content) */
/* 	{ */
/* 		long idx2; */
/* 		c = xmlGetProp(content, (xmlChar *)"index"); */
/* 		idx2 = atol((const char *)c); */
/* 		xmlFree(c); */
/* 		if ((idx2+1) > n_elem) */
/* 		{ */
/* 			n_elem = idx2 + 1; */
/* 			buf = realloc(buf, sizeof(double) * n_elem); */
/* 		} */

/* 		c = xmlGetProp(content, (xmlChar *)"ch"); */
/* 		ch_buf[idx2] = (short)atol((const char *)c); */
/* 		xmlFree(c); */

/* 		c = xmlNodeGetContent(content); */
/* 		buf[idx2] = xmlXPathCastStringToNumber(c); */
/* 		xmlFree(c); */

/* 		content = content->next; */
/* 	} */
/* 	if (buf) */
/* 	{ */
/* 		set->da[prop->idx][idx] = buf; */
/* 		set->da_ch[prop->idx][idx] = ch_buf; */
/* 		set->da_num_elem[prop->idx][idx] = n_elem; */
/* 	} */

/* 	return 0; */
/* } /\* read_double_array() *\/ */


/**
 * \brief read string event values stored in one XML tag
 * \param <prop> pointer to an event-property structure
 * \param <c> string containing the value
 *
 * The function "reads" the string event values for the event-property 'prop' stored in 'c'.
 * The single strings are seperated with 0x10 + '.' + 0x10 .
 */
int
handle_char_events (prop_handle prop, xmlChar * c)
{
  int ret = 0;
  char *p_curr, *p_next;
  long cnt;
  value_handle vh;

  p_curr = (char *) c;
  p_next = find_next_string (p_curr);
  vh = ra_value_malloc ();
  cnt = 0;
  while (p_curr)
    {
      ra_value_set_string (vh, p_curr);
      ra_prop_set_value (prop, cnt + 1, -1, vh);

      if (!p_next)
	break;

      p_curr = p_next;
      p_next = find_next_string (p_curr);
    }
  ra_value_free (vh);

  return ret;
}				/* handle_char_events() */


/**
 * \brief find next occurence of string seperator
 * \param <s> string which will be searched
 *
 * The function returns the position of the next string seperator (0x10 + '.' + 0x10).
 */
char *
find_next_string (char *s)
{
  char *p = s;
  size_t len = strlen (s);

  while ((p = strchr (p, 10)) != NULL)
    {
      if ((!p) || ((size_t) (p - s) >= (len - 2)))
	{
	  if (p)
	    *p = '\0';
	  return NULL;
	}

      if ((p[1] == '.') && (p[2] == 10))
	{
	  *p = '\0';
	  p += 3;
	  break;
	}

      p++;
    }

  if (p >= (s + len))
    return NULL;

  return p;
}				/* find_next_string() */


/**
 * \brief read event values MIME64 encoded
 * \param <prop> pointer to an event-property structure
 * \param <c> string containing the values
 *
 * The function "reads" the event values for the event-property 'prop' from the string 'c'.
 * The string 'c' contains the values MIME64 encoded.
 */
int
handle_value_events (prop_handle prop, long src_value_type, xmlChar * c)
{
  int ret = 0;
  void *t = NULL;
  size_t size, data_size;
  unsigned long l;
  value_handle vh;
  struct eval_class *cl = ra_class_get_handle (prop);

  vh = ra_value_malloc ();

  /* decode and put values as double in an array */
  switch (src_value_type)
    {
    case RA_VALUE_TYPE_SHORT:
      data_size = cl->num_event * sizeof (short);
      decode_base64 (c, strlen ((const char *) c), &t, &size, data_size);
      for (l = 0; l < cl->num_event; l++)
	{
	  ra_value_set_short (vh, ((short *) t)[l]);
	  ra_prop_set_value (prop, l + 1, -1, vh);
	}
      break;
    case RA_VALUE_TYPE_LONG:
      data_size = cl->num_event * sizeof (long);
      decode_base64 (c, strlen ((const char *) c), &t, &size, data_size);
      for (l = 0; l < cl->num_event; l++)
	{
	  ra_value_set_long (vh, ((long *) t)[l]);
	  ra_prop_set_value (prop, l + 1, -1, vh);
	}
      break;
    case RA_VALUE_TYPE_DOUBLE:
      data_size = cl->num_event * sizeof (double);
      decode_base64 (c, strlen ((const char *) c), &t, &size, data_size);
      for (l = 0; l < cl->num_event; l++)
	{
	  ra_value_set_double (vh, ((double *) t)[l]);
	  ra_prop_set_value (prop, l + 1, -1, vh);
	}
      break;
    }

  if (t)
    free (t);
  ra_value_free (vh);

  return ret;
}				/* handle_value_events() */


/**
 * \brief read templates
 * \param <set> pointer to an event-set structure
 * \param <parent> pointer of the parent XML node
 *
 * The function reads all template informations below the XML node 'parent'.
 */
/* int */
/* read_templates_xml(class_handle clh, xmlNodePtr parent) */
/* { */
/* 	xmlNodePtr child, node = parent->children; */
/* 	xmlChar *c; */
/* 	sum_handle sh; */

/* 	while (node) */
/* 	{ */
/* 		long *ch_size = NULL; */
/* 		long *ch_num = NULL; */
/* 		long num_ch = 0; */

/* 		if (xmlStrcmp(node->name, (xmlChar *)"Template") != 0) */
/* 		{ */
/* 			node = node->next; */
/* 			continue; */
/* 		} */

/* 		sh = ra_sum_add(clh, "template", "template", "template of events"); */

/* 		child = node->children; */
/* 		while (child) */
/* 		{ */
/* 			if (xmlStrcmp(child->name, (xmlChar *)"Channel") != 0) */
/* 			{ */
/* 				child = child->next; */
/* 				continue; */
/* 			} */

/* 			num_ch++; */
/* 			ch_size = realloc(ch_size, sizeof(long) * num_ch); */
/* 			ch_num = realloc(ch_num, sizeof(long) * num_ch); */

/* 			c = xmlGetProp(child, (xmlChar *)"number"); */
/* 			if (c) */
/* 			{ */
/* 				ch_num[num_ch-1] = atoi((const char *)c); */
/* 				xmlFree(c); */
/* 			} */
/* 			else */
/* 			{ */
/* 				free(ch_size); */
/* 				free(ch_num); */
/* 				_ra_set_error(set, RA_ERR_EVAL_FILE_CORRUPT, "libRASCH"); */
/* 				return -1; */
/* 			} */
/* 			c = xmlGetProp(child, (xmlChar *)"size"); */
/* 			if (c) */
/* 			{ */
/* 				ch_size[num_ch-1] = atoi((const char *)c); */
/* 				xmlFree(c); */
/* 			} */
/* 			else */
/* 			{ */
/* 				free(ch_size); */
/* 				free(ch_num); */
/* 				_ra_set_error(set, RA_ERR_EVAL_FILE_CORRUPT, "libRASCH"); */
/* 				return -1; */
/* 			} */

/* 			child = child->next; */
/* 		} */

/* 		if (read_single_template_xml(sh, ch_size, ch_num, num_ch, node) != 0) */
/* 		{ */
/* 			free(ch_size); */
/* 			free(ch_num); */
/* 			_ra_set_error(set, RA_ERR_EVAL_FILE_CORRUPT, "libRASCH"); */
/* 			return -1; */
/* 		} */

/* 		node = node->next; */

/* 		free(ch_size); */
/* 		free(ch_num); */
/* 	} */

/* 	return 0; */
/* } /\* read_templates_xml() *\/ */


/* /\** */
/*  * \brief return a specific event-property */
/*  * \param <e> pointer to an evaluation structure */
/*  * \param <name> name of the event-property looked for */
/*  * */
/*  * The function returns the event-property named 'name'. */
/*  *\/ */
/* /\* prop_handle *\/ */
/* /\* find_prop(struct eval *e, const char *name) *\/ */
/* /\* { *\/ */
/* /\* 	struct event_property *prop = e->prop; *\/ */

/* /\* 	while (prop) *\/ */
/* /\* 	{ *\/ */
/* /\* 		if (strcmp(prop->id_ascii, name) == 0) *\/ */
/* /\* 			return prop; *\/ */

/* /\* 		prop = prop->next; *\/ */
/* /\* 	} *\/ */

/* /\* 	return NULL; *\/ */
/* /\* } /\\* find_prop() *\\/ *\/ */


/* /\** */
/*  * \brief read a single template */
/*  * \param <templ> pointer to an template structure */
/*  * \param <parent> pointer of the parent XML node */
/*  * */
/*  * The function reads a single template information below the XML node 'parent'. */
/*  *\/ */
/* int */
/* read_single_template_xml(sum_handle sh, long *ch_size, long *ch_num, long num_ch, xmlNodePtr parent) */
/* { */
/* 	void *t; */
/* 	long size; */
/* 	xmlChar *c; */
/* 	xmlNodePtr curr, node = parent->children; */

/* 	while (node) */
/* 	{ */
/* 		if (xmlStrcmp(node->name, (xmlChar *)"single") != 0) */
/* 		{ */
/* 			node = node->next; */
/* 			continue; */
/* 		} */

/* 		templ->num_templates++; */
/* 		templ->st = realloc(templ->st, sizeof(struct single_template) * templ->num_templates); */

/* 		c = xmlGetProp(node, (xmlChar *)"number"); */
/* 		if (c) */
/* 		{ */
/* 			templ->st[templ->num_templates - 1].num = atol((const char *)c); */
/* 			xmlFree(c); */
/* 		} */
/* 		c = xmlGetProp(node, (xmlChar *)"fiducial-offset"); */
/* 		if (c) */
/* 		{ */
/* 			templ->st[templ->num_templates - 1].fiducial_offset = atoi((const char *)c); */
/* 			xmlFree(c); */
/* 		} */
/* 		c = xmlGetProp(node, (xmlChar *)"type"); */
/* 		if (c) */
/* 		{ */
/* 			templ->st[templ->num_templates - 1].type = atol((const char *)c); */
/* 			xmlFree(c); */
/* 		} */

/* 			ch_idx = ra_sum_add_ch(sh, ch, long fiducial_offset, long num_dim, const char **dim_unit, */
/* 			   const char **dim_name); */


/* 		templ->st[templ->num_templates - 1].data = malloc(sizeof(double *) * templ->num_ch); */
/* 		curr = node->children; */
/* 		while (curr) */
/* 		{ */
/* 			int idx; */

/* 			if (xmlStrcmp(curr->name, (xmlChar *)"data") != 0) */
/* 			{ */
/* 				curr = curr->next; */
/* 				continue; */
/* 			} */

/* 			c = xmlGetProp(curr, (xmlChar *)"ch-index"); */
/* 			if (!c) */
/* 				return -1;	/\* file corrupt *\/ */
/* 			idx = atoi((const char *)c); */
/* 			xmlFree(c); */
/* 			if ((idx >= templ->num_ch) || (idx < 0)) */
/* 				return -1;	/\* file corrupt *\/ */
/* 			c = xmlNodeGetContent(curr); */
/* 			if (!c) */
/* 				return -1; */
/* 			decode_base64(c, strlen((const char *)c), &t, &size, -1); */
/* 			templ->st[templ->num_templates - 1].data[idx] = malloc(size); */
/* 			memcpy(templ->st[templ->num_templates - 1].data[idx], t, size); */
/* 			xmlFree(c); */
/* 			free(t); */

/* 			curr = curr->next; */
/* 		} */

/* 		node = node->next; */
/* 	} */

/* 	return 0; */
/* } /\* read_single_tempalate_xml() *\/ */
