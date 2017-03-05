/**
 * \file read_evaluation.c
 *
 * This file provides the part of the API to access evaluations.
 *
 * \author Raphael Schneider (rasch@med1.med.tum.de)
 */

/*----------------------------------------------------------------------------
 *
 * Copyright (C) 2004-2007, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Header:  $
 *
 *--------------------------------------------------------------------------*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>


#define _LIBRASCH_BUILD
#include <ra_eval.h>


/**
 * \brief get the original evaluation from a measurement
 * \param <mh> measurement handle
 *
 * The function gets the original evaluation from a measurement.
 */
int
read_evaluation(meas_handle mh)
{
	int ret = -1;
	eval_handle eh;
	struct ra_meas *meas = (struct ra_meas *)mh;
	struct plugin_struct *pl;

	if (!mh)
		return -1;
	pl = meas->p;

	/* check if original evaluation is alread loaded */
	if (ra_eval_get_original(mh) != NULL)
		return 0;	/* yes -> exit func */

	/* check if original evaluation is available by the format */
	if (pl->access.get_eval_info == NULL)
		return 0;	/* no eval-info available by the format */

	ra_eval_edit_start(mh);

	if ((eh = add_eval_orig(mh, pl)) == NULL)
		goto error_eval;
	if (add_class_orig(mh, eh, pl) != 0)
		goto error_eval;

 	if (do_post_processing(mh, eh) != 0)
		goto error_eval;

	ra_eval_edit_complete(mh);

	return 0;

 error_eval:
	ra_eval_edit_cancel(mh);

	_ra_set_error(mh, RA_ERR_READ_EVAL, "libRASCH");
	
	return ret;
} /* read_evaluation() */


/**
 * \brief create original evaluation in a measurement
 * \param <mh> measurement handle
 * \param <pl> pointer to a plugin structure
 *
 * The function creates in a measurement the original evaluation.
 */
eval_handle
add_eval_orig(meas_handle mh, struct plugin_struct *pl)
{
	int ret = 0;
	eval_handle eh = NULL;
	value_handle vh, vh_name, vh_desc, vh_add_ts, vh_modify_ts, vh_user, vh_host, vh_prog;

	if (pl->access.get_eval_info == NULL)
		return NULL;

	vh = ra_value_malloc();
	vh_name = ra_value_malloc();
	vh_desc = ra_value_malloc();
	vh_add_ts = ra_value_malloc();
	vh_modify_ts = ra_value_malloc();
	vh_user = ra_value_malloc();
	vh_host = ra_value_malloc();
	vh_prog = ra_value_malloc();

	ret = (*pl->access.get_eval_info)(mh, vh_name, vh_desc, vh_add_ts,
					  vh_modify_ts, vh_user, vh_host, vh_prog);
	if (ret != 0)
		goto clean;

	eh = ra_eval_add(mh, ra_value_get_string(vh_name), ra_value_get_string(vh_desc), 1);
	if (!eh)
		goto clean;

	if (ra_value_is_ok(vh_add_ts))
		ra_eval_attribute_set(eh, "add-timestamp", vh_add_ts);
	if (ra_value_is_ok(vh_modify_ts))
		ra_eval_attribute_set(eh, "modify-timestamp", vh_modify_ts);
	if (ra_value_is_ok(vh_user))
		ra_eval_attribute_set(eh, "add-user", vh_user);
	if (ra_value_is_ok(vh_host))
		ra_eval_attribute_set(eh, "add-host", vh_host);
	if (ra_value_is_ok(vh_prog))
		ra_eval_attribute_set(eh, "add-program", vh_prog);
	
 clean:
	ra_value_free(vh);
	ra_value_free(vh_name);
	ra_value_free(vh_desc);
	ra_value_free(vh_add_ts);
	ra_value_free(vh_modify_ts);
	ra_value_free(vh_user);
	ra_value_free(vh_host);
	ra_value_free(vh_prog);

	return eh;
} /* add_eval_orig() */


/**
 * \brief add original event-classes
 * \param <mh> measurement handle
 * \param <pl> pointer to a plugin structure
 *
 * The function adds all event-classes from a measurement to the original
 * evaluation.
 */
int
add_class_orig(meas_handle mh, eval_handle eh, struct plugin_struct *pl)
{
	int ret = -1;
	long n_class, l;
	value_handle vh_id, vh_name, vh_desc;
	class_handle clh;
	long *ev_ids = NULL;
	unsigned long num_ev;
	
	if ((pl->access.get_class_num == NULL) || (pl->access.get_class_info == NULL))
		return -1;

	vh_id = ra_value_malloc();
	vh_name = ra_value_malloc();
	vh_desc = ra_value_malloc();

	n_class = (*pl->access.get_class_num)(mh);
	for (l = 0; l < n_class; l++)
	{
		if ((*pl->access.get_class_info)(mh, l, vh_id, vh_name, vh_desc) != 0)
			goto error;

		if (!ra_value_is_ok(vh_id))
			goto error;

		clh = ra_class_add(eh, ra_value_get_string(vh_id), ra_value_get_string(vh_name), ra_value_get_string(vh_desc));
		if (!clh)
			goto error;

		if (add_events_orig(clh, l, pl, &ev_ids, &num_ev) != 0)
			goto error;

		if (add_prop_orig(clh, l, pl, ev_ids, num_ev) != 0)
			goto error;

		if (add_summaries_orig(clh, l, pl) != 0)
			goto error;
	}

	ret = 0;

 error:
	ra_value_free(vh_id);
	ra_value_free(vh_name);
	ra_value_free(vh_desc);

	if (ev_ids)
		free(ev_ids);

	return ret; 
} /* add_class_orig() */


/**
 * \brief add original events
 * \param <clh> event-class handle
 * \param <pl> pointer to a plugin structure
 *
 * The function adds all events from a measurement to the original
 * evaluation.
 */
int
add_events_orig(class_handle clh, long class_num, struct plugin_struct *pl, long **ev_ids, unsigned long *num)
{
	int ret = -1;
	value_handle vh_start, vh_end;
	const long *start, *end;

	if ((ev_ids == NULL) || (num == NULL))
		return -1;
	*ev_ids = NULL;
	*num = 0;
	
	if ((pl->access.get_ev_info == NULL))
		return 0;

	vh_start = ra_value_malloc();
	vh_end = ra_value_malloc();

	if ((*pl->access.get_ev_info)(clh, class_num, vh_start, vh_end) != 0)
		goto error;
	if (!ra_value_is_ok(vh_start) || !ra_value_is_ok(vh_end))
		goto error;

	start = ra_value_get_long_array(vh_start);
	end = ra_value_get_long_array(vh_end);

	*num = ra_value_get_num_elem(vh_start);
	*ev_ids = (long *)malloc(sizeof(long) * (*num));

	if ((ret = ra_class_add_event_mass(clh, *num, start, end, *ev_ids)) != 0)
		goto error;

	ret = 0;

 error:
	ra_value_free(vh_start);
	ra_value_free(vh_end);

	if (ret != 0)
	{
		free(*ev_ids);
		*ev_ids = NULL;
		*num = 0;
	}

	return ret;
} /* add_events_orig() */


/**
 * \brief add original event properties
 * \param <clh> event-class handle
 * \param <pl> pointer to a plugin structure
 *
 * The function adds all event-properties from a measurement to the original
 * evaluation.
 */
int
add_prop_orig(class_handle clh, long class_num, struct plugin_struct *pl, long *ev_ids, unsigned long num_ev)
{
	int ret = -1;
	long n_prop, l;
	value_handle vh, vh_id, vh_type, vh_len, vh_name, vh_desc, vh_unit,
		vh_use_minmax, vh_min, vh_max, vh_has_ign_value, vh_ign_value;
	prop_handle ph;

	if (pl->access.get_prop_info == NULL)
		return -1;

	vh = ra_value_malloc();
	vh_id = ra_value_malloc();
	vh_name = ra_value_malloc();
	vh_desc = ra_value_malloc();
	vh_unit = ra_value_malloc();
	vh_type = ra_value_malloc();
	vh_len = ra_value_malloc();
	vh_use_minmax = ra_value_malloc();
	vh_min = ra_value_malloc();
	vh_max = ra_value_malloc();
	vh_has_ign_value = ra_value_malloc();
	vh_ign_value = ra_value_malloc();

	n_prop = (*pl->access.get_prop_num)(clh, class_num);
	for (l = 0; l < n_prop; l++)
	{
		if ((*pl->access.get_prop_info)(clh, class_num, l, vh_id, vh_type, vh_len, vh_name,
						vh_desc, vh_unit, vh_use_minmax, vh_min, vh_max,
						vh_has_ign_value, vh_ign_value) != 0)
		{
			goto error;
		}

		if (!ra_value_is_ok(vh_id) || !ra_value_is_ok(vh_type) || !ra_value_is_ok(vh_len))
			goto error;

		ph = ra_prop_add(clh, ra_value_get_string(vh_id), ra_value_get_long(vh_type),
				 ra_value_get_string(vh_name),
				 ra_value_get_string(vh_desc), ra_value_get_string(vh_unit),
				 ra_value_get_long(vh_use_minmax),
				 ra_value_get_double(vh_min), ra_value_get_double(vh_max),
				 ra_value_get_long(vh_has_ign_value), ra_value_get_double(vh_ign_value));
		if (!ph)
			goto error;

		add_values_orig(ph, class_num, l, pl, ev_ids, num_ev);
	}

	ret = 0;

 error:
	ra_value_free(vh);
	ra_value_free(vh_id);
	ra_value_free(vh_name);
	ra_value_free(vh_desc);
	ra_value_free(vh_unit);
	ra_value_free(vh_type);
	ra_value_free(vh_len);
	ra_value_free(vh_use_minmax);
	ra_value_free(vh_min);
	ra_value_free(vh_max);
	ra_value_free(vh_has_ign_value);
	ra_value_free(vh_ign_value);
 
	return ret;
} /* add_prop_orig() */


/**
 * \brief add original event values
 * \param <ph> event-property handle
 * \param <pl> pointer to a plugin structure
 *
 * The function sets the original event values (stored in a measurement) in the
 * evaluation 'e' for the given event properties 'proph'.
 */
int
add_values_orig(prop_handle ph, long class_num, long prop_num, struct plugin_struct *pl, long *ev_ids, unsigned long num_ev)
{
	int ret = -1;
	value_handle vh = NULL;
	value_handle vh_ch = NULL;
	value_handle vh_single = NULL;
	value_handle vh_ids = NULL;
	unsigned long l, m, n_ch;
	const long *ch;

	if ((pl->access.get_ev_value == NULL) && (pl->access.get_ev_value_all == NULL))
		return 0;

	vh = ra_value_malloc();
	vh_ch = ra_value_malloc();

	if (pl->access.get_ev_value_all)
	{
		if ((ret = (*pl->access.get_ev_value_all)(ph, class_num, prop_num, vh, vh_ch)) != 0)
			goto error;

		ret = ra_prop_set_value_mass(ph, ev_ids, ra_value_get_long_array(vh_ch), vh);		
	}
	else
	{
		vh_single = ra_value_malloc();
		vh_ids = ra_value_malloc();

		for (l = 0; l < num_ev; l++)
		{
			if ((ret = (*pl->access.get_ev_value)(ph, class_num, prop_num, l, vh, vh_ch)) != 0)
				goto error;
			
			n_ch = ra_value_get_num_elem(vh_ch);
			ch = ra_value_get_long_array(vh_ch);
			
			for (m = 0; m < n_ch; m++)
			{
				ra_value_get_single_elem(vh_single, vh, m);
				if ((ret = ra_prop_set_value(ph, ev_ids[l], ch[m], vh_single)) != 0)
					goto error;
			}
		}
		ret = 0;
	}

 error:
	ra_value_free(vh);
	ra_value_free(vh_ch);
	ra_value_free(vh_single);
	ra_value_free(vh_ids);

	return ret;
} /* add_values_orig() */


/**
 * \brief add event-summaries from a measurement
 * \param <mh> measurement handle
 * \param <pl> pointer to a plugin structure
 *
 * The function adds the original templates (stored in a measurement) in the
 * evaluation 'e'.
 */
int
add_summaries_orig(class_handle clh, long class_num, struct plugin_struct *pl)
{
	int ret = -1;
	value_handle vh_id, vh_name, vh_desc, vh_dim_name, vh_dim_unit, vh_ch, vh_fid_point;
	unsigned long l, m, num_sum, num_dim, num_parts;
	sum_handle sh;

	if ((pl->access.get_sum_num == NULL) || (pl->access.get_sum_info == NULL))
	{
		return 0;
	}

	num_sum = (*pl->access.get_sum_num)(clh, class_num);
	if (num_sum <= 0)
		return 0;

	vh_id = ra_value_malloc();
	vh_name = ra_value_malloc();
	vh_desc = ra_value_malloc();
	vh_dim_name = ra_value_malloc();
	vh_dim_unit = ra_value_malloc();
	vh_ch = ra_value_malloc();
	vh_fid_point = ra_value_malloc();
	
	for (l = 0; l < num_sum; l++)
	{
		ret = (*pl->access.get_sum_info)(clh, class_num, l, vh_id, vh_name, vh_desc,
						 vh_dim_name, vh_dim_unit, vh_ch, vh_fid_point,
						 &num_parts);
		if (ret != 0)
			goto error;

		if (!ra_value_is_ok(vh_id))
			goto error;

		num_dim = ra_value_get_num_elem(vh_dim_name);

		sh = ra_sum_add(clh, ra_value_get_string(vh_id), ra_value_get_string(vh_name), ra_value_get_string(vh_desc),
				num_dim, ra_value_get_string_array(vh_dim_name), ra_value_get_string_array(vh_dim_unit));
		if (!sh)
			goto error;

		for (m = 0; m < ra_value_get_num_elem(vh_ch); m++)
		{
			if ((ret = ra_sum_add_ch(sh, (ra_value_get_long_array(vh_ch))[m],
						 (ra_value_get_long_array(vh_fid_point))[m])) != 0)
			{
				goto error;
			}
		}

		for (m = 0; m < num_parts; m++)
		{
			if ((ret = add_sum_part_orig(sh, class_num, l, m, num_dim, ra_value_get_num_elem(vh_ch), pl)) != 0)
			{
				goto error;
			}
		}
	}

	ret = 0;

 error:
	ra_value_free(vh_id);
	ra_value_free(vh_name);
	ra_value_free(vh_desc);
	ra_value_free(vh_dim_name);
	ra_value_free(vh_dim_unit);
	ra_value_free(vh_ch);
	ra_value_free(vh_fid_point);

	return ret;
} /* add_summaries_orig() */


int
add_sum_part_orig(sum_handle sh, long class_num, long sum_num, long part_num, long num_dim,
		  long num_ch, struct plugin_struct *pl)
{
	int ret = -1;
	value_handle vh_events, vh;
	long part_id;
	long l, m;

	if ((pl->access.get_sum_events == NULL) || (pl->access.get_sum_part_data == NULL))
		return -1;

	vh_events = ra_value_malloc();
	vh = ra_value_malloc();

	if ((ret = (*pl->access.get_sum_events)(sh, class_num, sum_num, part_num, vh_events)) != 0)
		return -1;
	if (ra_value_get_num_elem(vh_events) <= 0)
	{
		ret = 0;
		goto clean;
	}

	part_id = ra_sum_add_part(sh, ra_value_get_num_elem(vh_events), ra_value_get_long_array(vh_events));
	if (part_id < 0)
		goto clean;

	for (l = 0; l < num_ch; l++)
	{
		for (m = 0; m < num_dim; m++)
		{
			if ((*pl->access.get_sum_part_data)(sh, class_num, sum_num, part_num, l, m, vh) != 0)
				goto clean;
			if (ra_value_get_num_elem(vh) <= 0)
				continue;

			if ((ret = ra_sum_set_part_data(sh, part_id, l, m, vh)) != 0)
				goto clean;
		}
	}

	ret = 0;

 clean:
	ra_value_free(vh);
	ra_value_free(vh_events);

	return ret;
} /* add_sum_part_orig() */


/**
 * \brief process original evaluation after getting the data
 * \param <mh> measurement handle
 * \param <e> pointer to an evaluation structure
 *
 * The function performs a post-processing on the original evaluation
 * after the values are got from the measurement. TODO: Think about how 
 * all this post-processing stuff can be implemented in a more general way.
 */
int
do_post_processing(meas_handle mh, eval_handle eh)
{
	unsigned long l;
	value_handle vh;

/* TODO: think about if post-processing should be done for each channel or if
   post-processing should be based on available event-properties */


/* 	rec_handle rh; */
/* 	int num_ch, i; */

/* 	rh = ra_rec_get_first(mh, 0); */
/* 	ra_info_get(rh, RA_INFO_REC_GEN_NUM_CHANNEL_L, inf); */
/* 	num_ch = (int) inf->value.l; */

/* 	for (i = 0; i < num_ch; i++) */
/* 	{ */
/* 		inf->ch = i; */
/* 		if (ra_info_get(rh, RA_INFO_REC_CH_TYPE_L, inf) != 0) */
/* 			continue; */

/* 		if ((inf->value.l == RA_CH_TYPE_ECG) && !ecg) */
/* 		{ */
/* 			post_process_ecg(mh, e); */
/* 			ecg = 1; */
/* 		} */
/* 	} */


	/* check if qrs-positions are available */
	vh = ra_value_malloc();
	ra_class_get(eh, "heartbeat", vh);
	for (l = 0; l < ra_value_get_num_elem(vh); l++)
	{
		class_handle clh = (class_handle)(ra_value_get_voidp_array(vh)[l]);
		if (ra_prop_get(clh, "qrs-pos") != NULL)
			post_process_ecg(mh, clh);
	}
	ra_value_free(vh);

	return 0;
} /* do_post_processing() */


/**
 * \brief post-process ecg data
 * \param <mh> measurement handle
 * \param <clh> pointer to a 'heartbeat' event-class
 *
 * The function performs a post-processing for ecg values.
 */
void
post_process_ecg(meas_handle mh, class_handle clh)
{
	ra_handle ra;	
	plugin_handle p;
	rec_handle rec;
	value_handle vh;
	int skip_artifact_detection = 0;
	struct proc_info *pi;
	
	ra = ra_lib_handle_from_any_handle(mh);
	p = ra_plugin_get_by_name(ra, "ecg", 1);
	if (!p)
		return;

	/* if 'mh' is a libRASCH-RRI file, do not run artifact detection */
	rec = ra_rec_get_first(mh, 0);
	if (!rec)
		return;
	vh = ra_value_malloc();
	if (ra_info_get(rec, RA_INFO_REC_GEN_DESC_C, vh) == 0)
	{
		if (strcmp(ra_value_get_string(vh), "libRASCH RRI-file") == 0)
			skip_artifact_detection = 1;
	}	
	
	pi = (struct proc_info *)ra_proc_get(mh, p, NULL);
	ra_value_set_voidp(vh, clh);
	ra_lib_set_option(pi, "clh", vh);
	ra_value_set_short(vh, skip_artifact_detection);
	ra_lib_set_option(pi, "skip_artifact_detection", vh);
	ra_value_free(vh);
	ra_proc_do(pi);
	ra_proc_free(pi);
} /* post_process_ecg() */


