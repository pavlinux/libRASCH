/**
 * \file ra_eval.h
 *
 * The header file contains all definitions for an evaluation. 
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

#ifndef RA_EVAL_H
#define RA_EVAL_H

#include <stdio.h>
#include <math.h>
#include <libxml/tree.h>

#include <ra_priv.h>
#include <ra.h>
#include <ra_event_props.h>

#ifdef __cplusplus
extern "C" {
#endif


/* -------------------- defines -------------------- */
#define EVAL_MAX_TS        21	/* when was evaluation done (date and time in UTC); format: 'dd.mm.yyyy  hh:mm:ss' */
#define EVAL_MAX_USER      50	/* who did evaluation */
#define EVAL_MAX_HOST      50	/* where was evaluation done */
#define EVAL_MAX_PRG       50	/* which program did evaluation */

#define EVENT_MEM_ADD      0.1	/* add 10% percent of already allocated events */

#define MAX_ID_LEN         100
#define MAX_ATTRIB_LEN     100

#define RA_PROP_NUM_ELEM_TYPE_BYTE  0
#define RA_PROP_NUM_ELEM_TYPE_LONG  1

/* -------------------- structures -------------------- */
/* forward declarations */
struct ra_meas;

/* --------------- general --------------- */

/**
 * \struct eval_attribute
 * \brief handle attributes associated with evaluations
 * \param <dummy> dummy variable (needed to be able to use ra_list_* functions)
 * \param <next> pointer to next attribute structure
 * \param <prev> pointer to previous attribute structure
 * \param <src_handle> handle of the source
 * \param <name> name of the attribute
 * \param <value> value of the attribute
 */
struct eval_attribute
{
	unsigned short dummy;

	struct eval_attribute *next;
	struct eval_attribute *prev;

	any_handle src_handle;

	char name[MAX_ATTRIB_LEN];
	value_handle value;
}; /* struct eval_attribute */


/**
 * \struct eval_head
 * \brief head of all eval* structures
 * \param <handle_id> type of the structure
 * \param <next> pointer to the next structure of the specific type
 * \param <prev> pointer to the previous structure of the specific type
 * \param <meas> handle of the measurement the evaluation belongs to
 */
struct eval_head
{
	unsigned short handle_id;

	struct eval_head *next;
	struct eval_head *prev;

	struct ra_meas *meas;
}; /* struct eval_head */


/* --------------- main --------------- */


/**
 * \struct eval
 * \brief contains all informations about an evaluation
 * \param <handle_id> set to RA_HANDLE_EVAL
 * \param <next> pointer to the next eval structure
 * \param <prev> pointer to the previous eval structure
 * \param <meas> pointer to the measurement which the evaluation belongs to
 */
struct eval
{
	unsigned short handle_id;

	struct eval *next;
	struct eval *prev;

	struct ra_meas *meas;

	int modified;
	int original;
	int def;

	struct eval_class *cl;
}; /* struct eval */


/* --------------- event class --------------- */


/**
 * \struct eval_event
 * \brief contains information about a single event
 * \param <id> unique id of the event
 * \param <start> start position of the event
 * \param <end> end position of the event
 */
struct eval_event
{
	long id;
	long start;
	long end;
}; /* struct eval_event */


/**
 * \struct eval_class
 * \brief contains all information needed for an event class
 * \param <handle_id> set to RA_HANDLE_EVENT_CLASS
 * \param <next> pointer to the next eval_class structure
 * \param <prev> pointer to the previous eval_class structure
 * \param <meas> pointer to the measurement which the event class belongs to
 * \param <eval> pointer to the evaluation this event class belongs to
 * \param <id> id of the event class (use only inside of libRASCH)
 * \param <ascii_id> string-id of the event class
 * \param <num_event> number of events in this event class
 * \param <ev> pointer to an array holding informations about the events
 */
struct eval_class
{
	unsigned short handle_id;

	struct eval_class *next;
	struct eval_class *prev;

	struct ra_meas *meas;

	struct eval *eval;

	long id;
	char ascii_id[MAX_ID_LEN];

	unsigned long num_event;
	struct eval_event *ev;
	long last_id;

	struct eval_property *prop;

	struct eval_summary *summaries;
}; /* struct eval_class */


/* --------------- event property --------------- */


/**
 * \struct eval_property
 * \brief contains all informations needed for event-properties
 * \param <id> set to RA_HANDLE_EVENT_PROP
 * \param <next> pointer to the next eval_property structure
 * \param <prev> pointer to the previous eval_property structure
 * \param <meas> pointer to the measurement which the event property belongs to
 * \param <evclass> pointer to the event class structure this event property belongs to
 * \param <id> id of the event property (use only inside of libRASCH)
 * \param <ascii_id> string-id of the event property
 * \param <allocated_events> size of available event "slots"
 * \param <single_channels> flag if values for each single channel can be stored (=1)
                            or only the overall value (=0)
 * \param <num_ch_values> number of channel values
 * \param <ch_map> maps the channel-number to the index in 'value'
 * \param <entry_size> size in bytes needed for each event (data for one event for all handled channels)
 * \param <num_elements> if value is an array, here the array-size is stored
 * \param <value_type> value type of the property
 * \param <value> array holding the values for all events and channels
 */
struct eval_property
{
	unsigned short handle_id;

	struct eval_property *next;
	struct eval_property *prev;

	struct ra_meas *meas;

	struct eval_class *evclass;

	long id;
	char ascii_id[MAX_ID_LEN];

	unsigned long allocated_events;

	int single_channels;

	unsigned long num_ch_values;
	short *ch_map;

	size_t entry_size;

	int num_type;
	union
	{
		unsigned char *b;
		unsigned long *l;
	} num_elements;

	int value_type;
	union
	{
		short *s;
		long *l;
		double *d;
		char **c;
		short **sa;
		long **la;
		double **da;
		char ***ca;
	} value;
}; /* struct eval_property */


/* --------------- event summary --------------- */

/**
 * \struct eval_sum_ch_desc
 * \brief contains the description of the summary for one channel
 * \param <ch> channel number
 * \param <fiducial_offset> offset of the fiducial offset
 */
struct eval_sum_ch_desc
{
	long ch;
	long fiducial_offset;	
}; /* struct eval_sum_ch_desc */


/**
 * \struct eval_sum_data
 * \brief contains the data for each set of summary
 * \param <num_events> number of events used for the summary
 * \param <event_ids> event-ids used for the summary
 * \param <num_data> size of the data array (ra-value for each channel and dimension)
 * \param <data> data for each channel and dimension
 */
struct eval_sum_data
{
	unsigned short id;

	struct eval_sum_data *next;
	struct eval_sum_data *prev;

	unsigned long num_events;
	long *event_ids;

	value_handle *data;
}; /* struct eval_sum_data */


/**
 * \struct eval_summary
 * \brief contains infos about event summaries
 * \param <handle_id> set to RA_HANDLE_EVENT_SUMMARY
 * \param <next> pointer to the next eval_summary structure
 * \param <prev> pointer to the previous eval_summary structure
 * \param <meas> pointer to the measurement which the event summary belongs to
 * \param <evclass> pointer to the event class structure this event summary belongs to
 * \param <id> id of the event summary (use only inside of libRASCH)
 * \param <ascii_id> string-id of the event summary
 * \param <num_dim> number of dimensions
 * \param <dim_name> name for each dimension
 * \param <dim_unit> unit for each dimension
 * \param <num_ch> number of channels in the summary
 * \param <ch_desc> description for each channel
 * \param <num_summaries> number of summary elements
 * \param <sum> array of the summary elements
 */
struct eval_summary
{
	unsigned short handle_id;

	struct eval_summary *next;
	struct eval_summary *prev;

	struct ra_meas *meas;

	struct eval_class *evclass;

	long id;
	char ascii_id[MAX_ID_LEN];

	unsigned long num_dim;
	char **dim_name;
	char **dim_unit;

	unsigned long num_ch;
	struct eval_sum_ch_desc *ch_desc;

	unsigned long num_data_elements;
	unsigned short last_data_id;
	struct eval_sum_data *sum;
}; /* struct eval_summary */

/* forward declarations */
struct plugin_struct;
struct eval_info;

/**
 * \struct ra_auto_create_class
 * \brief Infos about event-classes which are added to 
 * \param <ascii_id> ascii-id of the event-class
 * \param <start_pos_idx> result-index holding the start position
 * \param <end_pos_idx> result-index holding the end position
 * \param <info_short> a short name describing this specific event-class
 * \param <info_long> a longer description of this specific event-class
 * \param <info_ext> a short string which will be added to property names
 * \param <clh> will hold the class_handle after adding the event-class (is valid only in ra_eval_save_result() function)
 * \param <num_events> number of added event-id's (is valid only in ra_eval_save_result() function)
 * \param <event_ids> will hold an array with the added event-id's (is valid only in ra_eval_save_result() function)
 */
struct ra_auto_create_class
{
	char *ascii_id;
	long start_pos_idx;
	long end_pos_idx;
	char *info_short;
	char *info_long;
	char *info_ext;
	class_handle clh;
	unsigned long num_events;
	long *event_ids;
}; /* struct ra_auto_create_class */


/**
 * \struct ra_auto_create_prop
 * \brief Infos about event-properties which are added to 
 * \param <ascii_id> ascii-id of the event-property
 * \param <class_index> index in ra_auto_create_class-array to the event-class the property should be added
 * \param <event_id_idx> result-index holding the event-id's (=-1 when event's are also added automatically)
 * \param <value_id> result-index holding the property values
 * \param <ch_id> result-index holding the channel numbers (=-1 when value is valid for all channels)
 * \param <ph> will hold the prop_handle after adding the event-property
 * \param <use_mass_insert> flag if property values can be set with ra_prop_set_value_mass()
 */
struct ra_auto_create_prop
{
	char *ascii_id;
	int class_index;
	int event_id_idx;
	long value_id;
	long ch_id;
	prop_handle ph;
	int use_mass_insert;
}; /* struct ra_auto_create_prop */


struct sort_pos
{
	long id;
	long pos;
}; /* struct sort_pos */


/* -------------------- things needed to read version 0 eval-files -------------------- */


struct event_info
{
	long id;
	long start;
	long end;
}; /* struct event_info */


struct ep_map_struct
{
	char v0_prop_name[EVAL_MAX_NAME];
	char v1_prop_name[EVAL_MAX_NAME];
	int same;
	int not_avail;
	int not_needed;
};  /* struct ep_map_struct */


/* -------------------- prototypes -------------------- */
struct eval_attribute * find_attribute(struct eval_head *h, const char *id);
int delete_attributes(struct eval_head *h);
long get_event_idx(struct eval_class *c, long event_id, long last_idx);
int del_prop_values(struct eval_class *c, unsigned long idx);
int free_prop_values(struct eval_property *p);
int del_ev_summary(class_handle clh, long event_id);
int add_prop_mem(struct eval_class *c);
int get_channels(class_handle clh, unsigned long *num_ch, long **ch);
int alloc_prop_mem(struct eval_property *p);
int set_short_events_mass(struct eval_property *p, const long *idx, const long *col,
			  value_handle vh, long num);
int set_long_events_mass(struct eval_property *p, const long *idx, const long *col,
			 value_handle vh, long num);
int set_double_events_mass(struct eval_property *p, const long *idx, const long *col,
			   value_handle vh, long num);
int get_short_events_all(struct eval_property *p, value_handle value, long *event_id,
			 long *ch_num, long num);
int get_long_events_all(struct eval_property *p, value_handle value, long *event_id,
			long *ch_num, long num);
int get_double_events_all(struct eval_property *p, value_handle value, long *event_id,
			  long *ch_num, long num);


/* ---------- read_evaluation.c ---------- */
int read_evaluation(meas_handle mh);
eval_handle add_eval_orig(meas_handle mh, struct plugin_struct *pl);
int add_class_orig(meas_handle mh, eval_handle eh, struct plugin_struct *pl);
int add_events_orig(class_handle clh, long class_num, struct plugin_struct *pl, long **ev_ids, unsigned long *num_ev);
int add_prop_orig(class_handle clh, long class_num, struct plugin_struct *pl, long *ev_ids, unsigned long num_ev);
int add_values_orig(prop_handle ph, long class_num, long prop_num, struct plugin_struct *pl, long *ev_ids, unsigned long num_ev);
int add_summaries_orig(class_handle clh, long class_num, struct plugin_struct *pl);
int add_sum_part_orig(sum_handle sh, long class_num, long sum_num, long part_num, long num_dim,
		      long num_ch, struct plugin_struct *pl);
int do_post_processing(meas_handle mh, eval_handle eh);
void post_process_ecg(meas_handle mh, class_handle clh);


/* ---------- eval_internal.c ---------- */
int load_eval(meas_handle mh, const char *eval_file);
int get_eval_file(meas_handle mh, char *f);
int eval_get_info(any_handle h, int info_id, value_handle vh);
int free_eval_infos(struct eval_info *ei);

/* ---------- eval_auto.c ---------- */
/* int create_auto_class(eval_handle eh, struct ra_auto_create_class *acc, value_handle *res); */
/* int create_auto_prop(eval_handle eh, class_handle clh, struct proc_info *pi, struct ra_auto_create_class *acc, */
/* 		     struct ra_auto_create_prop *acp, value_handle *res); */

/* ---------- load_eval_v0.c ---------- */
int eval_load_v0(meas_handle mh, const char *eval_file);
int get_eval_file_xml(meas_handle mh, char *f);
int read_evals_xml(meas_handle mh, xmlNodePtr parent);
void fix_string(char *string);
xmlNodePtr get_node(xmlNodePtr parent, char *name);
int read_sets_xml(eval_handle eh, xmlNodePtr parent);
int read_events_start_end_xml(xmlNodePtr parent, struct event_info *evi, unsigned long num_events);
int read_props_xml(class_handle clh, xmlNodePtr parent, int use_ascii, unsigned long num_events);
struct ep_map_struct *get_ep_map_info(struct ep_map_struct *map, long num_entries, const char *v0_prop_name);
int get_comment_data(xmlNodePtr parent, value_handle vh, unsigned long num_events);
int get_comment_ch_data(xmlNodePtr parent, value_handle vh, unsigned long num_events);
int read_events_xml(prop_handle prop, long src_value_type, xmlNodePtr parent, int use_ascii);
int handle_value_events_ascii(prop_handle prop, long src_value_type, xmlNodePtr parent);
int read_short(xmlNodePtr node, prop_handle prop, long idx, value_handle vh);
int read_long(xmlNodePtr node, prop_handle prop, long idx, value_handle vh);
int read_double(xmlNodePtr node, prop_handle prop, long idx, value_handle vh);
int read_char(xmlNodePtr node, prop_handle prop, long idx, value_handle vh);
/* int read_short_array(xmlNodePtr node, prop_handle prop, long idx, value_handle vh); */
/* int read_long_array(xmlNodePtr node, prop_handle prop, long idx, value_handle vh); */
/* int read_double_array(xmlNodePtr node, prop_handle prop, long idx, value_handle vh); */
int handle_char_events(prop_handle prop, xmlChar *c);
char * find_next_string(char *s);
int handle_value_events(prop_handle prop, long src_value_type, xmlChar *c);
int read_templates_xml(class_handle clh, xmlNodePtr parent);
prop_handle find_prop(eval_handle eh, const char *name);
int read_single_template_xml(sum_handle sh, xmlNodePtr parent);

/* ---------- load_eval_v1.c ---------- */
int eval_load_v1(meas_handle mh, const char *eval_file);
int read_evals(meas_handle mh, xmlNodePtr parent);
int read_class(eval_handle eh, xmlNodePtr parent);
int read_event_infos(struct eval_class *cl, xmlNodePtr parent);
int read_props(struct eval_class *cl, xmlNodePtr parent);
int read_events(prop_handle prop, long src_value_type, xmlNodePtr parent);
int set_short_events(struct eval_property *prop, value_handle vh);
int set_long_events(struct eval_property *prop, value_handle vh);
int set_double_events(struct eval_property *prop, value_handle vh);
int set_char_events(struct eval_property *prop, value_handle vh);
int set_short_array_events(struct eval_property *prop, value_handle vh);
int set_long_array_events(struct eval_property *prop, value_handle vh);
int set_double_array_events(struct eval_property *prop, value_handle vh);
int set_char_array_events(struct eval_property *prop, value_handle vh);
int read_summaries(struct eval_class *cl, xmlNodePtr parent);
int read_sum_dimensions(xmlNodePtr node, value_handle vh_dim_name, value_handle vh_dim_unit);
int read_summary_ch_desc(sum_handle sh, xmlNodePtr parent);
int read_summary_data_element(sum_handle sh, xmlNodePtr parent);
xmlNodePtr get_node(xmlNodePtr parent, char *name);
xmlChar * get_attribute(xmlNodePtr parent, const char *attribute);
int read_attributes(any_handle h, xmlNodePtr parent);
int read_value(value_handle vh, unsigned long *idx, xmlNodePtr parent);
char * find_next_string(char *s);

/* ---------- save_eval_v1.c ---------- */
int write_evals(meas_handle mh, xmlNodePtr parent, int use_ascii);
int write_class(struct eval *e, xmlNodePtr parent, int use_ascii);
int write_event_infos(struct eval_class *cl, xmlNodePtr node);
int write_props(struct eval_class *cl, xmlNodePtr parent, int use_ascii);
int write_events(struct eval_property *prop, xmlNodePtr parent);
int write_short_events(struct eval_property *prop, xmlNodePtr node);
int write_long_events(struct eval_property *prop, xmlNodePtr node);
int write_double_events(struct eval_property *prop, xmlNodePtr node);
int write_char_events(struct eval_property *prop, xmlNodePtr node, long num_data_complete);
int write_short_array_events(struct eval_property *prop, xmlNodePtr node, long num_data_complete);
int write_long_array_events(struct eval_property *prop, xmlNodePtr node, long num_data_complete);
int write_double_array_events(struct eval_property *prop, xmlNodePtr node, long num_data_complete);
int write_char_array_events(struct eval_property *prop, xmlNodePtr node, long num_data_complete);
int write_summaries(struct eval_class *cl, xmlNodePtr parent);
int write_summary_ch_desc(struct eval_sum_ch_desc *desc, xmlNodePtr parent);
int write_summary_data_element(struct eval_summary *s, xmlNodePtr parent);
int write_attributes(any_handle h, xmlNodePtr parent, const char *modify_timestamp);
int write_value(value_handle vh, long idx, xmlNodePtr parent);
int keep_current_eval_file(const char *fn);



#ifdef __cplusplus
}
#endif

#endif /* RA_EVAL_H */
