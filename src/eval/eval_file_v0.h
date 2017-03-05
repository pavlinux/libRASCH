/**
 * \file eval_file_v0.h
 *
 * 
 *
 * \author Raphael Schneider (rasch@med1.med.tum.de)
 */

/*----------------------------------------------------------------------------
 *
 * Copyright (C) 2005, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *
 *--------------------------------------------------------------------------*/

#ifndef EVAL_FILE_V0_H
#define EVAL_FILE_V0_H

struct event_info
{
  long id;
  long start;
  long end;
};				/* struct event_info */

struct ep_map_struct
{
  char v0_prop_name[EVAL_MAX_NAME];
  char v1_prop_name[EVAL_MAX_NAME];
  int same;
  int not_avail;
  int not_needed;
};				/* struct ep_map_struct */

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

int get_eval_file_xml (meas_handle mh, char *f);
int read_evals_xml (meas_handle mh, xmlNodePtr parent);
xmlNodePtr get_node (xmlNodePtr parent, char *name);
int read_sets_xml (eval_handle eh, xmlNodePtr parent);
int read_events_start_end_xml (xmlNodePtr parent, struct event_info *evi,
			       long num_events);
int read_props_xml (class_handle clh, xmlNodePtr parent, int use_ascii,
		    struct event_info *evi, long num_events);
struct ep_map_struct *get_ep_map_info (const char *v0_prop_name);
int get_comment_data (xmlNodePtr parent, value_handle vh, long num_events);
int get_comment_ch_data (xmlNodePtr parent, value_handle vh, long num_events);
int read_events_xml (prop_handle prop, long src_value_type, xmlNodePtr parent,
		     int use_ascii);
int handle_value_events_ascii (prop_handle prop, long src_value_type,
			       xmlNodePtr parent);
int read_short (xmlNodePtr node, prop_handle prop, long idx, value_handle vh);
int read_long (xmlNodePtr node, prop_handle prop, long idx, value_handle vh);
int read_double (xmlNodePtr node, prop_handle prop, long idx,
		 value_handle vh);
int read_char (xmlNodePtr node, prop_handle prop, long idx, value_handle vh);
/* int read_short_array(xmlNodePtr node, prop_handle prop, long idx, value_handle vh); */
/* int read_long_array(xmlNodePtr node, prop_handle prop, long idx, value_handle vh); */
/* int read_double_array(xmlNodePtr node, prop_handle prop, long idx, value_handle vh); */
int handle_char_events (prop_handle prop, xmlChar * c);
char *find_next_string (char *s);
int handle_value_events (prop_handle prop, long src_value_type, xmlChar * c);
int read_templates_xml (class_handle clh, xmlNodePtr parent);
prop_handle find_prop (eval_handle eh, const char *name);
int read_single_template_xml (sum_handle sh, xmlNodePtr parent);

#endif /* EVAL_FILE_V0_H */
