/**
 * \file eval_internal.c
 *
 * This file includes routines which are used only from inside libRASCH. The
 * API functions for evaluations are in eval.c.
 *
 * \author Raphael Schneider (rasch@med1.med.tum.de)
 */

/*----------------------------------------------------------------------------
 *
 * Copyright (C) 2004-2007, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *
 *--------------------------------------------------------------------------*/


#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define _LIBRASCH_BUILD
#include <ra_eval.h>
#include <ra_linked_list.h>


int
load_eval (meas_handle mh, const char *eval_file)
{
  int ret = -1;

  if ((eval_file != NULL) && (eval_file[0] != '\0'))
    {
      if (strcmp (eval_file + strlen (eval_file) - 3, "mte") == 0)
	ret = eval_load_v0 (mh, eval_file);
      else
	ret = eval_load_v1 (mh, eval_file);
    }
  else
    {
      FILE *fp;
      char fn[MAX_PATH_RA];

      get_eval_file (mh, fn);
      if ((fp = fopen (fn, "r")) == NULL)
	ret = eval_load_v0 (mh, NULL);
      else
	{
	  fclose (fp);
	  ret = eval_load_v1 (mh, fn);
	}
    }

  return ret;
}				/* load_eval() */


/**
 * \brief returns evaluation filename
 * \param <mh> measurement handle
 * \param <f> will receive the filename of the evaluation
 *
 * If no evaluation file was given, this function returns the default filename
 * used for evaluations (based on the measurement name).
 */
int
get_eval_file (meas_handle mh, char *f)
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
    sprintf (f, "%s%ceval_ra.lre", ra_value_get_string (vh), SEPARATOR);
  else
    sprintf (f, "%s.lre", ra_value_get_string (vh));

  ra_value_free (vh);

  return 0;
}				/* get_eval_file() */


int
eval_get_info (any_handle h, int info_id, value_handle vh)
{
  int ret = 0;
  char a[MAX_ATTRIB_LEN];
  int get_attrib = 0;
  /* cast handle to all possible eval-structures; the function
     calling this function had done already a check that the info-id
     and the handle type fit */
  struct eval *e = (struct eval *) h;
  struct eval_class *c = (struct eval_class *) h;
  struct eval_property *p = (struct eval_property *) h;
  struct eval_summary *s = (struct eval_summary *) h;
  long number = ra_value_get_number (vh);

  switch (info_id)
    {
    case RA_INFO_EVAL_NAME_C:
    case RA_INFO_CLASS_NAME_C:
    case RA_INFO_PROP_NAME_C:
    case RA_INFO_SUM_NAME_C:
      strncpy (a, "name", MAX_ATTRIB_LEN);
      get_attrib = 1;
      break;
    case RA_INFO_EVAL_DESC_C:
    case RA_INFO_CLASS_DESC_C:
    case RA_INFO_PROP_DESC_C:
    case RA_INFO_SUM_DESC_C:
      strncpy (a, "description", MAX_ATTRIB_LEN);
      get_attrib = 1;
      break;
    case RA_INFO_EVAL_ADD_TS_C:
      strncpy (a, "add-timestamp", MAX_ATTRIB_LEN);
      get_attrib = 1;
      break;
    case RA_INFO_EVAL_MODIFY_TS_C:
      strncpy (a, "modify-timestamp", MAX_ATTRIB_LEN);
      get_attrib = 1;
      break;
    case RA_INFO_EVAL_USER_C:
      strncpy (a, "add-user", MAX_ATTRIB_LEN);
      get_attrib = 1;
      break;
    case RA_INFO_EVAL_HOST_C:
      strncpy (a, "add-host", MAX_ATTRIB_LEN);
      get_attrib = 1;
      break;
    case RA_INFO_EVAL_PROG_C:
      strncpy (a, "add-program", MAX_ATTRIB_LEN);
      get_attrib = 1;
      break;
    case RA_INFO_EVAL_ORIGINAL_L:
      if (e->original)
	ra_value_set_long (vh, 1);
      else
	ra_value_set_long (vh, 0);
      break;
    case RA_INFO_EVAL_DEFAULT_L:
      if (e->def)
	ra_value_set_long (vh, 1);
      else
	ra_value_set_long (vh, 0);
      break;
    case RA_INFO_CLASS_ASCII_ID_C:
      ra_value_set_string (vh, c->ascii_id);
      break;
    case RA_INFO_CLASS_EV_NUM_L:
      ra_value_set_long (vh, c->num_event);
      break;
    case RA_INFO_PROP_ASCII_ID_C:
      ra_value_set_string (vh, p->ascii_id);
      break;
    case RA_INFO_PROP_VALTYPE_L:
      ra_value_set_long (vh, p->value_type);
      break;
    case RA_INFO_PROP_UNIT_C:
      strncpy (a, "unit", MAX_ATTRIB_LEN);
      get_attrib = 1;
      break;
    case RA_INFO_PROP_HAS_MINMAX_L:
      strncpy (a, "use-minmax", MAX_ATTRIB_LEN);
      get_attrib = 1;
      break;
    case RA_INFO_PROP_MIN_D:
      strncpy (a, "min", MAX_ATTRIB_LEN);
      get_attrib = 1;
      break;
    case RA_INFO_PROP_MAX_D:
      strncpy (a, "max", MAX_ATTRIB_LEN);
      get_attrib = 1;
      break;
    case RA_INFO_PROP_IGNORE_VALUE_D:
      strncpy (a, "ignore-value", MAX_ATTRIB_LEN);
      get_attrib = 1;
      break;
    case RA_INFO_SUM_ASCII_ID_C:
      ra_value_set_string (vh, s->ascii_id);
      break;
    case RA_INFO_SUM_NUM_DIM_L:
      ra_value_set_long (vh, s->num_dim);
      break;
    case RA_INFO_SUM_DIM_UNIT_C:
      ra_value_set_string (vh, (const char *) (s->dim_unit[number]));
      break;
    case RA_INFO_SUM_DIM_NAME_C:
      ra_value_set_string (vh, (const char *) (s->dim_name[number]));
      break;
    case RA_INFO_SUM_NUM_CH_L:
      ra_value_set_long (vh, s->num_ch);
      break;
    case RA_INFO_SUM_CH_NUM_L:
      ra_value_set_long (vh, s->ch_desc[number].ch);
      break;
    case RA_INFO_SUM_CH_FIDUCIAL_L:
      ra_value_set_long (vh, s->ch_desc[number].fiducial_offset);
      break;
    default:
      _ra_set_error (h, RA_ERR_UNKNOWN_INFO, "libRASCH");
      ret = -1;
      break;
    }

  if (get_attrib)
    ret = ra_eval_attribute_get (h, a, vh);

  return ret;
}				/* eval_get_info() */


int
free_eval_infos (struct eval_info *ei)
{
  int ret = 0;
  struct eval_attribute *a;
  struct eval *e;

  while ((a = ei->attribute) != NULL)
    {
      if (a->value)
	ra_value_free (a->value);
      ra_list_del ((void **) &(ei->attribute), a);
      free (a);
    }

  while ((e = ei->evaluations) != NULL)
    {
      ra_eval_delete (e);
    }

  return ret;
}				/* free_eval_infos() */
