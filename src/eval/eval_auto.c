/* ------------------------------ evaluation helper functions ------------------------------ */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define _LIBRASCH_BUILD
#include <ra_eval.h>
#include <ra_linked_list.h>
#include <pl_general.h>


LIBRAAPI int
ra_eval_save_result (eval_handle eh, class_handle clh, proc_handle proc,
		     long res_set)
{
  int ret = 0;
  int i;
  eval_handle eh_use;
  value_handle *res;
  struct proc_info *pi = proc;
  struct plugin_struct *pl;
  struct plugin_infos *pli;
  long res_offset, n;

  pl = pi->plugin;
  pli = &(pl->info);
  res = pi->results;

  res_offset = res_set * pi->num_results;

  if (res_set >= pi->num_result_sets)
    {
      _ra_set_error (pi->mh, RA_ERR_ERROR, "libRASCH");
      return -1;
    }

  if (eh == NULL)
    {
      eh_use =
	ra_eval_add (pi->mh, "auto-eval",
		     "evaluation created within libRASCH-function ra_eval_save_result()",
		     0);
      if (!eh_use)
	return -1;
    }
  else
    eh_use = eh;

  for (i = 0; i < pli->num_create_class; i++)
    {
      if (pli->create_class[i].clh != NULL)
	continue;

      n =
	ra_value_get_num_elem (res
			       [pli->create_class[i].start_pos_idx +
				res_offset]);
      pli->create_class[i].event_ids = ra_alloc_mem (sizeof (long) * n);
      ret = create_auto_class (eh, pli->create_class + i, res, res_offset);
      if (ret != 0)
	goto clean;
    }

  for (i = 0; i < pli->num_create_prop; i++)
    {
      ret =
	create_auto_prop (eh, clh, pli->create_class, pli->create_prop + i,
			  res, res_offset);
      if (ret != 0)
	goto clean;
    }

clean:
  /* clean up */
/* 	if (clh == NULL) */
/* 	{ */
/* 		for (i = 0; i < pli->num_create_class; i++) */
/* 		{ */
/* 			if (pli->create_class[i].event_ids) */
/* 				free(pli->create_class[i].event_ids); */
/* 		} */
/* 	} */

  return ret;
}				/* ra_eval_save_result() */


int
create_auto_class (eval_handle eh, struct ra_auto_create_class *acc,
		   value_handle * res, long res_offset)
{
  int ret = 0;
  value_handle vh = NULL;

  ra_eval_edit_start (eh);

  if ((acc->clh = ra_class_add_predef (eh, acc->ascii_id)) == NULL)
    goto error;

  vh = ra_value_malloc ();

  ra_value_set_string (vh, acc->info_short);
  if (ra_eval_attribute_set (acc->clh, "info-short", vh) != 0)
    goto error;
  ra_value_set_string (vh, acc->info_long);
  if (ra_eval_attribute_set (acc->clh, "info-long", vh) != 0)
    goto error;
  ra_value_set_string (vh, acc->info_ext);
  if (ra_eval_attribute_set (acc->clh, "info-ext", vh) != 0)
    goto error;

  ret = add_events (eh, acc, res, res_offset);

error:
  if (vh)
    ra_value_free (vh);

  if (ret == 0)
    ra_eval_edit_complete (eh);
  else
    ra_eval_edit_cancel (eh);

  return ret;
}				/* create_auto_class */


int
add_events (eval_handle eh, struct ra_auto_create_class *acc,
	    value_handle * res, long res_offset)
{
  int ret = 0;
  long l, num;
  const long *startp, *endp;

  num = ra_value_get_num_elem (res[acc->start_pos_idx + res_offset]);
  startp = ra_value_get_long_array (res[acc->start_pos_idx + res_offset]);
  endp = ra_value_get_long_array (res[acc->end_pos_idx + res_offset]);
  if ((num > 0) && ((startp == NULL) || (endp == NULL)))
    {
      _ra_set_error (eh, RA_ERR_ERROR_INTERNAL, "libRASCH");
      return -1;
    }

  acc->num_events = 0;
  for (l = 0; l < num; l++)
    {
      acc->event_ids[l] = ra_class_add_event (acc->clh, startp[l], endp[l]);
      if (acc->event_ids[l] < 0)
	{
	  ret = -1;
	  break;
	}
    }

  acc->num_events = num;

  return ret;
}				/* add_events() */


int
create_auto_prop (eval_handle eh, class_handle clh,
		  struct ra_auto_create_class *acc,
		  struct ra_auto_create_prop *acp, value_handle * res,
		  long res_offset)
{
  int ret = 0;
  long l, num, num_events;
  class_handle clh_use;
  value_handle vh;
  const long *ch_nums, *ev_ids;

  /* get class-handle (class-handle in acc is prefered) */
  if ((acc == NULL) || (acc[acp->class_index].clh == NULL))
    clh_use = clh;
  else
    clh_use = acc[acp->class_index].clh;
  if (clh_use == NULL)
    return -1;

  /* check that we have values to set */
  if (ra_value_get_num_elem (res[acp->value_id + res_offset]) <= 0)
    return 0;

  /* get event-id's */
  ev_ids = NULL;
  num_events = 0;
  /* are the event-id's available in the results? */
  if (acp->event_id_idx >= 0)
    {
      ev_ids = ra_value_get_long_array (res[acp->event_id_idx + res_offset]);
      num_events =
	ra_value_get_num_elem (res[acp->event_id_idx + res_offset]);
    }
  else
    {
      /* no; are the events added in create_auto_class()? */
      if (acc && acc[acp->class_index].event_ids)
	{
	  ev_ids = acc[acp->class_index].event_ids;
	  num_events = acc[acp->class_index].num_events;
	}
      else if (acc)
	{
	  /* no, so add them now */
	  num_events =
	    ra_value_get_num_elem (res
				   [acc[acp->class_index].start_pos_idx +
				    res_offset]);
	  acc[acp->class_index].event_ids =
	    ra_alloc_mem (sizeof (long) * num_events);
	  ret = add_events (eh, acc, res, res_offset);
	  ev_ids = acc[acp->class_index].event_ids;
	}
    }
  if (ev_ids == NULL)
    return -1;

  /* add the property (if necessary) */
  if ((acp->ph = ra_prop_get (clh_use, acp->ascii_id)) == NULL)
    if ((acp->ph = ra_prop_add_predef (clh_use, acp->ascii_id)) == NULL)
      return -1;

  if (acp->use_mass_insert)
    {
      ret = do_mass_insert (acp, ev_ids, num_events, res, res_offset);
      return ret;
    }

  if (acp->ch_id >= 0)
    ch_nums = ra_value_get_long_array (res[acp->ch_id + res_offset]);
  else
    ch_nums = NULL;

  /* set the values */
  vh = ra_value_malloc ();
  num = ra_value_get_num_elem (res[acp->value_id + res_offset]);
  for (l = 0; l < num; l++)
    {
      long ch;

      if (ch_nums)
	ch = ch_nums[l];
      else
	ch = -1;

      ra_value_get_single_elem (vh, res[acp->value_id + res_offset], l);
      if ((ret = ra_prop_set_value (acp->ph, ev_ids[l], ch, vh)) != 0)
	break;
    }
  ra_value_free (vh);

  return ret;
}				/* create_auto_prop */


int
do_mass_insert (struct ra_auto_create_prop *acp, const long *ev_ids,
		long num_events, value_handle * res, long res_offset)
{
  int ret;
  long l, m, num;
  long *ch_nums;
  long *ev_ids_use;
  long num_sets;

  num = ra_value_get_num_elem (res[acp->value_id + res_offset]);
  if ((num % num_events) != 0)
    {
      fprintf (stderr,
	       "do_mass_insert() in eval_auto.c: number of values does not match number of events!!!\n");
      fprintf (stderr, "num=%ld  num-events=%ld\n", num, num_events);
      return -1;
    }
  num_sets = num / num_events;
  ev_ids_use = malloc (sizeof (long) * num);
  for (l = 0; l < num_sets; l++)
    for (m = 0; m < num_events; m++)
      ev_ids_use[m + l * num_events] = ev_ids[m];

  ch_nums = malloc (sizeof (long) * num);
  if (acp->ch_id >= 0)
    {
      const long *ch =
	(long *) ra_value_get_long_array (res[acp->ch_id + res_offset]);
      for (l = 0; l < num; l++)
	ch_nums[l] = ch[l];
    }
  else
    {
      for (l = 0; l < num; l++)
	ch_nums[l] = -1;
    }

  ret =
    ra_prop_set_value_mass (acp->ph, ev_ids_use, ch_nums,
			    res[acp->value_id + res_offset]);

  free (ev_ids_use);
  free (ch_nums);

  return ret;
}				/* do_mass_insert() */
