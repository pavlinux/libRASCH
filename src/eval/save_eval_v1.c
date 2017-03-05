/**
 * \file save_eval_v1.c
 *
 * This file implements the functionality to save libRASCH evaluation files Version 1.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <assert.h>

#include <libxml/tree.h>
#include <libxml/xpath.h>
#include "mime64.h"

#ifndef WIN32
#include <unistd.h>
#endif

#define _LIBRASCH_BUILD
#include <ra_eval.h>
#include <pl_general.h>


/**
 * \brief save evalution
 * \param <mh> measurement handle
 * \param <file> name of the evaluation file (optional)
 * \param <use_ascii> flag if event-values should be stored as ASCII-text (=1) or as
 * binary MIME64 encoded data (=0)
 *
 * This function saves the evaluation(s) that belongs to the measurement mh. If no filename file
 * is given, the default-filename will be used. If use_ascii is !=0 than the event values
 * are stored as ASCII text.
 */
LIBRAAPI int
ra_eval_save (meas_handle mh, const char *file, int use_ascii)
{
  char fn[MAX_PATH_RA];
  int ret;
  xmlDocPtr doc = NULL;

  if (!mh)
    return -1;

  if (file && (file[0] != '\0'))
    strncpy (fn, file, MAX_PATH_RA);
  else
    get_eval_file (mh, fn);	/* function is in eval_internal.c */

  doc = xmlNewDoc ((xmlChar *) "1.0");
  doc->children = xmlNewDocNode (doc, NULL, (xmlChar *) "libRASCH", NULL);
  xmlSetProp (doc->children, (xmlChar *) "format-version", (xmlChar *) "1.0");

  ret = write_evals (mh, doc->children, use_ascii);
  if (ret != 0)
    goto clean_up;

  /* before saving the file, rename the previous file (if available) to have a
     backup of at least the last version; for the "backup"-filename the '~' will
     be added */
  ret = keep_current_eval_file (fn);
  if (ret != 0)
    {
      _ra_set_error (mh, -errno, "libRASCH");
      goto clean_up;
    }

  if ((ret = xmlSaveFileEnc (fn, doc, "UTF-8")) == -1)
    _ra_set_error (mh, RA_ERR_SAVE_FILE, "libRASCH");
  else
    ret = 0;			/* no error (xmlSaveFile returns #Bytes written) */

clean_up:
  if (doc)
    xmlFreeDoc (doc);

  return ret;
}				/* ra_eval_save() */


/**
 * \brief writes all evaluations
 * \param <mh> measurement handle
 * \param <parent> pointer of the parent XML node
 * \param <use_ascii> flag if event-values should be stored as ASCII-text (=1) or as
 * binary MIME64 encoded data (=0)
 *
 * The function writes all evaluations below the XML node 'parent'.
 */
int
write_evals (meas_handle mh, xmlNodePtr parent, int use_ascii)
{
  struct ra_meas *mm = (struct ra_meas *) mh;
  struct eval *e;
  int ret = 0;
  xmlNodePtr node;
  value_handle vh;
  char max_samplerate_s[100];
  char modify_timestamp[30];

  vh = ra_value_malloc ();
  ra_info_get (mh, RA_INFO_MAX_SAMPLERATE_D, vh);
  sprintf (max_samplerate_s, "%f", ra_value_get_double (vh));
  ra_value_free (vh);

  e = mm->eval.evaluations;
  while (e)
    {
      node = xmlNewChild (parent, NULL, (xmlChar *) "Evaluation", NULL);
      if (e->original)
	xmlSetProp (node, (xmlChar *) "original", (xmlChar *) "1");
      else
	xmlSetProp (node, (xmlChar *) "original", (xmlChar *) "0");
      if (e->def)
	xmlSetProp (node, (xmlChar *) "default", (xmlChar *) "1");
      else
	xmlSetProp (node, (xmlChar *) "default", (xmlChar *) "0");

      if (e->modified)
	{
	  time_t tt;
	  struct tm *t;
	  time (&tt);
	  t = gmtime (&tt);

	  sprintf (modify_timestamp,
		   "%02d.%02d.%4d  %02d:%02d:%02d", t->tm_mday,
		   t->tm_mon + 1, (t->tm_year + 1900), t->tm_hour,
		   t->tm_min, t->tm_sec);
	}
      else
	modify_timestamp[0] = '\0';

      write_attributes (e, node, (e->modified ? modify_timestamp : NULL));

      if ((ret = write_class (e, node, use_ascii)) != 0)
	return ret;

      e = e->next;
    }

  return ret;
}				/* write_evals() */


/**
 * \brief write event-classes
 * \param <e> pointer to an evaluation structure
 * \param <parent> pointer of the parent XML node
 * \param <use_ascii> flag if event-values should be stored as ASCII-text (=1) or as
 * binary MIME64 encoded data (=0)
 *
 * The function write all event-sets below the XML node 'parent'.
 */
int
write_class (struct eval *e, xmlNodePtr parent, int use_ascii)
{
  int ret = 0;
  char s[100];
  struct eval_class *cl = e->cl;

  while (cl)
    {
      xmlNodePtr node;

      if (cl->num_event <= 0)
	{
	  cl = cl->next;
	  continue;
	}

      node = xmlNewChild (parent, NULL, (xmlChar *) "Event-Class", NULL);
      xmlSetProp (node, (xmlChar *) "id", (xmlChar *) cl->ascii_id);
      sprintf (s, "%ld", cl->num_event);
      xmlSetProp (node, (xmlChar *) "num-events", (xmlChar *) s);

      write_attributes (cl, node, NULL);

      if (use_ascii)
	xmlSetProp (node, (xmlChar *) "event-encoding", (xmlChar *) "ascii");

      write_event_infos (cl, node);

      if ((ret = write_props (cl, node, use_ascii)) != 0)
	return ret;

      if ((ret = write_summaries (cl, node)) != 0)
	return ret;

      cl = cl->next;
    }

  return ret;
}				/* write_class() */


/**
 * \brief write general event infos
 * \param <cl> pointer to an eval-class structure
 * \param <parent> pointer of the parent XML node
 *
 * The function writes general event-infos: start- and end-position
 * and the id's for each event.
 */
int
write_event_infos (struct eval_class *cl, xmlNodePtr node)
{
  long *id, *start, *end;
  unsigned long l;
  char *t = NULL;
  size_t size;

  id = malloc (sizeof (long) * cl->num_event);
  start = malloc (sizeof (long) * cl->num_event);
  end = malloc (sizeof (long) * cl->num_event);
  for (l = 0; l < cl->num_event; l++)
    {
      id[l] = cl->ev[l].id;
      start[l] = cl->ev[l].start;
      end[l] = cl->ev[l].end;
    }

  encode_base64 (id, sizeof (long) * cl->num_event, &t, &size);
  t = realloc (t, size + 1);
  t[size] = '\0';
  xmlNewTextChild (node, NULL, (xmlChar *) "event-id", (xmlChar *) t);
  free (t);

  encode_base64 (start, sizeof (long) * cl->num_event, &t, &size);
  t = realloc (t, size + 1);
  t[size] = '\0';
  xmlNewTextChild (node, NULL, (xmlChar *) "start-pos", (xmlChar *) t);
  free (t);

  encode_base64 (end, sizeof (long) * cl->num_event, &t, &size);
  t = realloc (t, size + 1);
  t[size] = '\0';
  xmlNewTextChild (node, NULL, (xmlChar *) "end-pos", (xmlChar *) t);
  free (t);

  free (id);
  free (start);
  free (end);

  return 0;
}				/* write_event_infos() */


/**
 * \brief write event-properties
 * \param <set> pointer to an event-set structure
 * \param <parent> pointer of the parent XML node
 * \param <use_ascii> flag if event-values should be stored as ASCII-text (=1) or as
 * binary MIME64 encoded data (=0)
 *
 * The function writes all event-properties below the XML node 'parent'.
 */
int
write_props (struct eval_class *cl, xmlNodePtr parent, int use_ascii)
{
  struct eval_property *prop = cl->prop;

  while (prop)
    {
      xmlNodePtr node;

      node = xmlNewChild (parent, NULL, (xmlChar *) "Event-Property", NULL);

      xmlSetProp (node, (xmlChar *) "id", (xmlChar *) prop->ascii_id);
      switch (prop->value_type)
	{
	case RA_VALUE_TYPE_SHORT:
	  xmlSetProp (node, (xmlChar *) "type", (xmlChar *) "short");
	  break;
	case RA_VALUE_TYPE_LONG:
	  xmlSetProp (node, (xmlChar *) "type", (xmlChar *) "long");
	  break;
	case RA_VALUE_TYPE_DOUBLE:
	  xmlSetProp (node, (xmlChar *) "type", (xmlChar *) "double");
	  break;
	case RA_VALUE_TYPE_CHAR:
	  xmlSetProp (node, (xmlChar *) "type", (xmlChar *) "char");
	  break;
	case RA_VALUE_TYPE_SHORT_ARRAY:
	  xmlSetProp (node, (xmlChar *) "type", (xmlChar *) "short-array");
	  break;
	case RA_VALUE_TYPE_LONG_ARRAY:
	  xmlSetProp (node, (xmlChar *) "type", (xmlChar *) "long-array");
	  break;
	case RA_VALUE_TYPE_DOUBLE_ARRAY:
	  xmlSetProp (node, (xmlChar *) "type", (xmlChar *) "double-array");
	  break;
	case RA_VALUE_TYPE_CHAR_ARRAY:
	  xmlSetProp (node, (xmlChar *) "type", (xmlChar *) "char-array");
	  break;
	default:
	  /* TODO: set error code */
	  return -1;
	  break;
	}

      write_attributes (prop, node, NULL);

      if (use_ascii)		/* TODO: add code to write data in ASCII */
	;			/* write_events_ascii(prop, node); */
      else
	write_events (prop, node);

      prop = prop->next;
    }

  return 0;
}				/* write_props() */


/**
 * \brief write event values using MIME64 encoding scheme
 * \param <prop> pointer to an event-property structure
 * \param <parent> pointer of the parent XML node
 *
 * The function writes the event values for event-property 'prop' below the XML node 'parent' using
 * a MIME64 encoding scheme for binary data.
 */
int
write_events (struct eval_property *prop, xmlNodePtr node)
{
  int ret = 0;
  char *t = NULL;
  size_t size, l;
  long num_data_complete;
  char s[100];
  xmlNodePtr child;

  sprintf (s, "%lu", prop->num_ch_values);
  xmlSetProp (node, (xmlChar *) "num-ch-values", (xmlChar *) s);

  encode_base64 (prop->ch_map, sizeof (unsigned short) * prop->num_ch_values,
		 &t, &size);
  t = realloc (t, size + 1);
  t[size] = '\0';
  child = xmlNewChild (node, NULL, (xmlChar *) "ch-map", NULL);
  xmlNodeAddContent (child, (xmlChar *) t);
  free (t);

  sprintf (s, "%d", prop->num_type);
  xmlSetProp (node, (xmlChar *) "num-elem-type", (xmlChar *) s);

  num_data_complete = 0;
  if (prop->num_type == RA_PROP_NUM_ELEM_TYPE_BYTE)
    {
      encode_base64 (prop->num_elements.b,
		     sizeof (unsigned char) * prop->num_ch_values *
		     prop->evclass->num_event, &t, &size);
      for (l = 0;
	   l < (size_t) (prop->num_ch_values * prop->evclass->num_event); l++)
	num_data_complete += prop->num_elements.b[l];
    }
  else
    {
      encode_base64 (prop->num_elements.l,
		     sizeof (long) * prop->num_ch_values *
		     prop->evclass->num_event, &t, &size);
      for (l = 0;
	   l < (size_t) (prop->num_ch_values * prop->evclass->num_event); l++)
	num_data_complete += prop->num_elements.l[l];
    }
  t = realloc (t, size + 1);
  t[size] = '\0';
  child = xmlNewChild (node, NULL, (xmlChar *) "num-elements", NULL);
  xmlNodeAddContent (child, (xmlChar *) t);
  free (t);


  switch (prop->value_type)
    {
    case RA_VALUE_TYPE_SHORT:
      ret = write_short_events (prop, node);
      break;
    case RA_VALUE_TYPE_LONG:
      ret = write_long_events (prop, node);
      break;
    case RA_VALUE_TYPE_DOUBLE:
      ret = write_double_events (prop, node);
      break;
    case RA_VALUE_TYPE_CHAR:
      ret = write_char_events (prop, node, num_data_complete);
      break;
    case RA_VALUE_TYPE_SHORT_ARRAY:
      ret = write_short_array_events (prop, node, num_data_complete);
      break;
    case RA_VALUE_TYPE_LONG_ARRAY:
      ret = write_long_array_events (prop, node, num_data_complete);
      break;
    case RA_VALUE_TYPE_DOUBLE_ARRAY:
      ret = write_double_array_events (prop, node, num_data_complete);
      break;
    case RA_VALUE_TYPE_CHAR_ARRAY:
      ret = write_char_array_events (prop, node, num_data_complete);
      break;
    default:
      /* TODO: set error code */
      return -1;
      break;
    }

  return ret;
}				/* write_events() */


int
write_short_events (struct eval_property *prop, xmlNodePtr node)
{
  long num;
  value_handle vh;

  num = prop->num_ch_values * prop->evclass->num_event;
  vh = ra_value_malloc ();
  ra_value_set_short_array (vh, prop->value.s, num);
  write_value (vh, -1, node);

  ra_value_free (vh);

  return 0;
}				/* write_short_events() */


int
write_long_events (struct eval_property *prop, xmlNodePtr node)
{
  long num;
  value_handle vh;

  num = prop->num_ch_values * prop->evclass->num_event;
  vh = ra_value_malloc ();
  ra_value_set_long_array (vh, prop->value.l, num);
  write_value (vh, -1, node);

  ra_value_free (vh);

  return 0;
}				/* write_long_events() */


int
write_double_events (struct eval_property *prop, xmlNodePtr node)
{
  long num;
  value_handle vh;

  num = prop->num_ch_values * prop->evclass->num_event;
  vh = ra_value_malloc ();
  ra_value_set_double_array (vh, prop->value.d, num);
  write_value (vh, -1, node);

  ra_value_free (vh);

  return 0;
}				/* write_double_events() */


int
write_char_events (struct eval_property *prop, xmlNodePtr node,
		   long num_data_complete)
{
  char **values = NULL;
  unsigned long l, m, curr, offset;
  value_handle vh;

  values = malloc (sizeof (char *) * num_data_complete);
  curr = 0;
  for (l = 0; l < prop->evclass->num_event; l++)
    {
      for (m = 0; m < prop->num_ch_values; m++)
	{
	  offset = l * prop->num_ch_values + m;

	  if (prop->num_elements.b[offset] == 0)
	    continue;

	  values[curr] = malloc (strlen (prop->value.c[offset]) + 1);
	  strcpy (values[curr], prop->value.c[offset]);
	  curr++;
	}
    }

  vh = ra_value_malloc ();
  ra_value_set_string_array_utf8 (vh, (const char **) values, curr);

  for (l = 0; l < curr; l++)
    free (values[l]);
  free (values);

  write_value (vh, -1, node);

  ra_value_free (vh);

  return 0;
}				/* write_char_events() */


int
write_short_array_events (struct eval_property *prop, xmlNodePtr node,
			  long num_data_complete)
{
  short *values = NULL;
  unsigned long l, m, n, curr, num, offset;
  value_handle vh;

  values = malloc (sizeof (short) * num_data_complete);
  curr = 0;
  for (l = 0; l < prop->evclass->num_event; l++)
    {
      for (m = 0; m < prop->num_ch_values; m++)
	{
	  offset = l * prop->num_ch_values + m;
	  num = 0;
	  switch (prop->num_type)
	    {
	    case RA_PROP_NUM_ELEM_TYPE_BYTE:
	      num = prop->num_elements.b[offset];
	      break;
	    case RA_PROP_NUM_ELEM_TYPE_LONG:
	      num = prop->num_elements.l[offset];
	      break;
	    }
	  for (n = 0; n < num; n++)
	    values[curr++] = prop->value.sa[offset][n];
	}
    }

  vh = ra_value_malloc ();
  ra_value_set_short_array (vh, values, curr);
  free (values);

  write_value (vh, -1, node);

  ra_value_free (vh);

  return 0;
}				/* write_short_events() */


int
write_long_array_events (struct eval_property *prop, xmlNodePtr node,
			 long num_data_complete)
{
  long *values = NULL;
  unsigned long l, m, n, curr, num, offset;
  value_handle vh;

  values = malloc (sizeof (short) * num_data_complete);
  curr = 0;
  for (l = 0; l < prop->evclass->num_event; l++)
    {
      for (m = 0; m < prop->num_ch_values; m++)
	{
	  offset = l * prop->num_ch_values + m;
	  num = 0;
	  switch (prop->num_type)
	    {
	    case RA_PROP_NUM_ELEM_TYPE_BYTE:
	      num = prop->num_elements.b[offset];
	      break;
	    case RA_PROP_NUM_ELEM_TYPE_LONG:
	      num = prop->num_elements.l[offset];
	      break;
	    }
	  for (n = 0; n < num; n++)
	    values[curr++] = prop->value.la[offset][n];
	}
    }

  vh = ra_value_malloc ();
  ra_value_set_long_array (vh, values, curr);

  free (values);

  write_value (vh, -1, node);

  ra_value_free (vh);

  return 0;
}				/* write_long_array_events() */


int
write_double_array_events (struct eval_property *prop, xmlNodePtr node,
			   long num_data_complete)
{
  double *values = NULL;
  unsigned long l, m, n, curr, num, offset;
  value_handle vh;

  values = malloc (sizeof (short) * num_data_complete);
  curr = 0;
  for (l = 0; l < prop->evclass->num_event; l++)
    {
      for (m = 0; m < prop->num_ch_values; m++)
	{
	  offset = l * prop->num_ch_values + m;
	  num = 0;
	  switch (prop->num_type)
	    {
	    case RA_PROP_NUM_ELEM_TYPE_BYTE:
	      num = prop->num_elements.b[offset];
	      break;
	    case RA_PROP_NUM_ELEM_TYPE_LONG:
	      num = prop->num_elements.l[offset];
	      break;
	    }
	  for (n = 0; n < num; n++)
	    values[curr++] = prop->value.da[offset][n];
	}
    }

  vh = ra_value_malloc ();
  ra_value_set_double_array (vh, values, curr);
  free (values);

  write_value (vh, -1, node);

  ra_value_free (vh);

  return 0;
}				/* write_double_array_events() */


int
write_char_array_events (struct eval_property *prop, xmlNodePtr node,
			 long num_data_complete)
{
  char **values = NULL;
  unsigned long l, m, n, curr, num, offset;
  value_handle vh;

  values = malloc (sizeof (char *) * num_data_complete);
  curr = 0;
  for (l = 0; l < prop->evclass->num_event; l++)
    {
      for (m = 0; m < prop->num_ch_values; m++)
	{
	  offset = l * prop->num_ch_values + m;
	  num = 0;
	  switch (prop->num_type)
	    {
	    case RA_PROP_NUM_ELEM_TYPE_BYTE:
	      num = prop->num_elements.b[offset];
	      break;
	    case RA_PROP_NUM_ELEM_TYPE_LONG:
	      num = prop->num_elements.l[offset];
	      break;
	    }
	  for (n = 0; n < num; n++)
	    {
	      values[curr] = malloc (strlen (prop->value.ca[offset][n]) + 1);
	      strcpy (values[curr++], prop->value.ca[offset][n]);
	    }
	}
    }

  vh = ra_value_malloc ();
  ra_value_set_string_array_utf8 (vh, (const char **) values, curr);

  for (l = 0; l < curr; l++)
    free (values[curr]);
  free (values);

  write_value (vh, -1, node);

  ra_value_free (vh);

  return 0;
}				/* write_char_array_events() */


/**
 * \brief write event summaries
 * \param <clh> pointer to an event-class structure
 * \param <parent> pointer of the parent XML node
 *
 * The function writes all template informations below the XML node 'parent'.
 */
int
write_summaries (struct eval_class *cl, xmlNodePtr parent)
{
  int ret;
  struct eval_summary *s = cl->summaries;
  xmlNodePtr node;
  unsigned long l;
  char st[100];

  while (s)
    {
      node = xmlNewChild (parent, NULL, (xmlChar *) "Summary", NULL);
      xmlSetProp (node, (xmlChar *) "id", (xmlChar *) s->ascii_id);
      write_attributes (cl, node, NULL);

      sprintf (st, "%ld", s->num_data_elements);
      xmlSetProp (node, (xmlChar *) "num-data-elements", (xmlChar *) st);

      sprintf (st, "%ld", s->num_dim);
      xmlSetProp (node, (xmlChar *) "num-dimensions", (xmlChar *) st);

      for (l = 0; l < s->num_dim; l++)
	{
	  xmlNodePtr dim_node;

	  dim_node = xmlNewChild (node, NULL, (xmlChar *) "dimension", NULL);

	  sprintf (st, "%ld", l);
	  xmlSetProp (dim_node, (xmlChar *) "index", (xmlChar *) st);

	  xmlSetProp (dim_node, (xmlChar *) "name",
		      (xmlChar *) (s->dim_name[l]));
	  xmlSetProp (dim_node, (xmlChar *) "unit",
		      (xmlChar *) (s->dim_unit[l]));
	}

      for (l = 0; l < s->num_ch; l++)
	{
	  ret = write_summary_ch_desc (s->ch_desc + l, node);
	  if (ret != 0)
	    return ret;
	}

      if ((ret = write_summary_data_element (s, node)) != 0)
	return ret;

      s = s->next;
    }

  return 0;
}				/* write_templates() */


int
write_summary_ch_desc (struct eval_sum_ch_desc *desc, xmlNodePtr parent)
{
  xmlNodePtr node;
  char st[100];

  node = xmlNewChild (parent, NULL, (xmlChar *) "channel-description", NULL);
  sprintf (st, "%ld", desc->ch);
  xmlSetProp (node, (xmlChar *) "ch-number", (xmlChar *) st);
  sprintf (st, "%ld", desc->fiducial_offset);
  xmlSetProp (node, (xmlChar *) "fiducial-offset", (xmlChar *) st);

  return 0;
}				/* write_summary_ch_desc() */


int
write_summary_data_element (struct eval_summary *s, xmlNodePtr parent)
{
  int ret;
  unsigned long l;
  struct eval_sum_data *d;
  xmlNodePtr node, node_id;
  char st[100];
  value_handle vh;

  d = s->sum;

  vh = ra_value_malloc ();
  while (d)
    {
      node = xmlNewChild (parent, NULL, (xmlChar *) "data", NULL);
      sprintf (st, "%d", d->id);
      xmlSetProp (node, (xmlChar *) "id", (xmlChar *) st);

      node_id = xmlNewChild (node, NULL, (xmlChar *) "event-ids", NULL);
      ra_value_set_long_array (vh, d->event_ids, d->num_events);
      write_value (vh, -1, node_id);

      for (l = 0; l < s->num_data_elements; l++)
	{
	  if ((ret = write_value (d->data[l], l, node)) != 0)
	    return ret;
	}

      d = d->next;
    }
  ra_value_free (vh);

  return 0;
}				/* write_summary_data_element() */


int
write_attributes (any_handle h, xmlNodePtr parent,
		  const char *modify_timestamp)
{
  int ret = 0;
  value_handle vh = NULL;
  value_handle vh_attrib = NULL;
  size_t l, num;
  const char **c;
  xmlNodePtr node;

  vh_attrib = ra_value_malloc ();
  ra_eval_attribute_list (h, vh_attrib);
  num = ra_value_get_num_elem (vh_attrib);
  if (num == 0)
    goto clean;

  vh = ra_value_malloc ();
  c = ra_value_get_string_array (vh_attrib);
  for (l = 0; l < num; l++)
    {
      if (ra_eval_attribute_get (h, c[l], vh) != 0)
	{
	  /* TODO: set error code */
	  ret = -1;
	  goto clean;
	}
      node = xmlNewChild (parent, NULL, (xmlChar *) "attribute", NULL);
      if (node == NULL)
	{
	  /* TODO: set error code */
	  ret = -1;
	  goto clean;
	}
      xmlSetProp (node, (xmlChar *) "name", (xmlChar *) c[l]);
      if (modify_timestamp && (strcmp (c[l], "modify-timestamp") == 0))
	ra_value_set_string (vh, modify_timestamp);
      ret = write_value (vh, -1, node);
      if (ret != 0)
	goto clean;
    }

clean:
  if (vh_attrib)
    ra_value_free (vh_attrib);
  if (vh)
    ra_value_free (vh);

  return ret;
}				/* write_attributes() */


int
write_value (value_handle vh, long idx, xmlNodePtr parent)
{
  int ret = 0;
  xmlNodePtr node;
  char st[200];
  short s;
  size_t l, num;
  double d;
  const void *v;
  const char **c;
  char *t = NULL;
  size_t size = 0;

  node = xmlNewChild (parent, NULL, (xmlChar *) "value", NULL);
  if (idx >= 0)
    {
      sprintf (st, "%ld", idx);
      xmlSetProp (node, (xmlChar *) "index", (xmlChar *) st);
    }

  num = ra_value_get_num_elem (vh);
  sprintf (st, "%u", num);
  xmlSetProp (node, (xmlChar *) "size", (xmlChar *) st);

  switch (ra_value_get_type (vh))
    {
    case RA_VALUE_TYPE_SHORT:
      strcpy (st, "short");
      break;
    case RA_VALUE_TYPE_LONG:
      strcpy (st, "long");
      break;
    case RA_VALUE_TYPE_DOUBLE:
      strcpy (st, "double");
      break;
    case RA_VALUE_TYPE_CHAR:
      strcpy (st, "char");
      break;
    case RA_VALUE_TYPE_VOIDP:
      strcpy (st, "voidp");
      break;
    case RA_VALUE_TYPE_SHORT_ARRAY:
      strcpy (st, "short-array");
      break;
    case RA_VALUE_TYPE_LONG_ARRAY:
      strcpy (st, "long-array");
      break;
    case RA_VALUE_TYPE_DOUBLE_ARRAY:
      strcpy (st, "double-array");
      break;
    case RA_VALUE_TYPE_CHAR_ARRAY:
      strcpy (st, "char-array");
      break;
    case RA_VALUE_TYPE_VOIDP_ARRAY:
      strcpy (st, "voidp-array");
      break;
    default:
      /* TODO: set error code */
      return -1;
      break;
    }
  xmlSetProp (node, (xmlChar *) "type", (xmlChar *) st);

  switch (ra_value_get_type (vh))
    {
    case RA_VALUE_TYPE_SHORT:
      s = ra_value_get_short (vh);
      encode_base64 (&s, sizeof (short), &t, &size);
      break;
    case RA_VALUE_TYPE_LONG:
      l = ra_value_get_long (vh);
      encode_base64 (&l, sizeof (long), &t, &size);
      break;
    case RA_VALUE_TYPE_DOUBLE:
      d = ra_value_get_double (vh);
      encode_base64 (&d, sizeof (double), &t, &size);
      break;
    case RA_VALUE_TYPE_CHAR:
      size = strlen (ra_value_get_string_utf8 (vh));
      t = realloc (t, size + 1);
      strcpy (t, ra_value_get_string_utf8 (vh));
      break;
    case RA_VALUE_TYPE_VOIDP:
      v = ra_value_get_voidp (vh);
      encode_base64 (&v, sizeof (void *), &t, &size);
      break;
    case RA_VALUE_TYPE_SHORT_ARRAY:
      encode_base64 (ra_value_get_short_array (vh), (sizeof (short) * num),
		     &t, &size);
      break;
    case RA_VALUE_TYPE_LONG_ARRAY:
      encode_base64 (ra_value_get_long_array (vh), (sizeof (long) * num), &t,
		     &size);
      break;
    case RA_VALUE_TYPE_DOUBLE_ARRAY:
      encode_base64 (ra_value_get_double_array (vh), (sizeof (double) * num),
		     &t, &size);
      break;
    case RA_VALUE_TYPE_CHAR_ARRAY:
      size = 0;
      c = ra_value_get_string_array_utf8 (vh);
      for (l = 0; l < num; l++)
	{
	  size_t len;

	  len = strlen (c[l]);
	  size += len + 3;
	  t = realloc (t, size);
	  strncpy (&(t[size - len - 3]), c[l], len);
	  t[size - 3] = 10;
	  t[size - 2] = '.';
	  t[size - 1] = 10;
	}
      break;
    case RA_VALUE_TYPE_VOIDP_ARRAY:
      encode_base64 (ra_value_get_voidp_array (vh), (sizeof (void *) * num),
		     &t, &size);
      break;
    }

  t = realloc (t, size + 1);
  t[size] = '\0';
  xmlNodeAddContent (node, (xmlChar *) t);
  free (t);

  return ret;
}				/* write_value() */


int
keep_current_eval_file (const char *fn)
{
  char fn_save[MAX_PATH_RA];
  FILE *fp;
  int ret = 0;

  /* first check if there is a file to keep */
  fp = fopen (fn, "r");
  if (fp == NULL)
    return ret;			/* no, so we do not need to do anything */
  fclose (fp);

  /* build backup filename */
  strcpy (fn_save, fn);
  strcat (fn_save, "~");

  /* and remove the previous one (if available) */
  fp = fopen (fn_save, "r");
  if (fp)
    {
      fclose (fp);
      ret = remove (fn_save);
      if (ret != 0)
	return ret;
    }

  /* and finally rename the current version */
  ret = rename (fn, fn_save);

  return ret;
}				/* keep_current_eval_file() */
