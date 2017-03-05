/**
 * \file load_eval_v1.c
 *
 * This file implements the functionality to read libRASCH evaluation files Version 1.
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


/**
 * \brief load evalution in XML format
 * \param <mh> measurement handle
 * \param <file> name of the evaluation file (optional)
 *
 * This function loads the evaluation(s) that belongs to the measurement mh. If no filename file
 * is given, the default-filename will be used.
 */
int
eval_load_v1(meas_handle mh, const char *file)
{
	struct ra_meas *mm = (struct ra_meas *) mh;
	char fn[MAX_PATH_RA];
	int ret = 0;
	xmlDocPtr doc;
	FILE *fp;
	xmlChar *c;

	if (!mh)
	{
		fprintf(stderr, "no meas-handle given\n");
		return -1;
	}

	_ra_set_error(mh, RA_ERR_NONE, "libRASCH");

	if (file && (file[0] != '\0'))
		strncpy(fn, file, MAX_PATH_RA);
	else
		get_eval_file(mm, fn);  /* function is in eval_internal.c */

	/* first check if eval-file is available */
	fp = fopen(fn, "r");
	if (fp == NULL)
		return 0;
	fclose(fp);

	if ((doc = xmlParseFile(fn)) == NULL)
	{
		_ra_set_error(mh, RA_ERR_READ_EVAL, "libRASCH");
		return -1;
	}
	if (!doc->children || (xmlStrcmp(doc->children->name, (xmlChar *)"libRASCH") != 0))
	{
		xmlFreeDoc(doc);
		_ra_set_error(mh, RA_ERR_EVAL_FILE_CORRUPT, "libRASCH");
		return -1;
	}

	c = xmlGetProp(doc->children, (xmlChar *)"format-version");
	if (c && (xmlStrcmp(c, (xmlChar *)"1.0") == 0))
		ret = read_evals(mm, doc->children);
	else
	{
		_ra_set_error(mh, RA_ERR_EVAL_FILE_WRONG_VERSION, "libRASCH");
		ret = -1;
	}
	if (c)
		xmlFree(c);

	xmlFreeDoc(doc);

	return ret;
} /* ra_eval_load_v1() */


/**
 * \brief reads the evaluations
 * \param <mh> measurement handle
 * \param <parent> pointer of the parent XML node
 *
 * The function reads all evaluations below the XML node 'parent'.
 */
int
read_evals(meas_handle mh, xmlNodePtr parent)
{
	eval_handle eh, eh_default;
	int ret = 0;
	xmlNodePtr node;
	xmlChar *c;
	xmlChar *name = NULL;
	xmlChar *desc = NULL;
	value_handle vh;

	vh = ra_value_malloc();

	eh_default = NULL;

	node = parent->children;
	while (node)
	{
		int original = 0;
		int def = 0;

		if (xmlStrcmp(node->name, (xmlChar *)"Evaluation") != 0)
		{
			node = node->next;
			continue;
		}

		/* get infos needed for ra_eval_add() function */
		original = 0;
		c = xmlGetProp(node, (xmlChar *)"original");
		if (((const char *)c)[0] == '1')
			original = 1;
		xmlFree(c);
		def = 0;
		c = xmlGetProp(node, (xmlChar *)"default");
		if (((const char *)c)[0] == '1')
			def = 1;
		xmlFree(c);

		eh = ra_eval_add(mh, (const char *)name, (const char *)desc, original);
		if (def == 1)
			eh_default = eh;

		if (name)
			xmlFree(name);
		if (desc)
			xmlFree(desc);

		read_attributes(eh, node);

		if ((ret = read_class(eh, node)) != 0)
			return ret;

		node = node->next;
	}

	/* if there is a default evaluation in the eval-file, set it now because at this point,
	   the last added evaluation is the default evaluation */
	if (eh_default)
		ra_eval_set_default(eh_default);

	ra_value_free(vh);

	return ret;
} /* read_evals() */


/**
 * \brief read event-classes
 * \param <e> pointer to an evaluation structure
 * \param <parent> pointer of the parent XML node
 *
 * The function reads all event-classes below the XML node 'parent'.
 */
int
read_class(eval_handle eh, xmlNodePtr parent)
{
	int ret = 0;
	struct eval_class *cl;
	xmlNodePtr node = parent->children;
	xmlChar *c;

	while (node)
	{
		if (xmlStrcmp(node->name, (xmlChar *)"Event-Class") != 0)
		{
			node = node->next;
			continue;
		}

		c = xmlGetProp(node, (xmlChar *)"id");
		if (!c)
		{
			node = node->next;
			continue;
			/* TODO: think if an error should be returned */
		}

		cl = ra_class_add_predef(eh, (const char *)c);
		xmlFree(c);

		read_attributes(cl, node);

		c = xmlGetProp(node, (xmlChar *)"num-events");
		if (c)
		{
			cl->num_event = atol((const char *)c);
			xmlFree(c);
		}
		else
		{
			ra_class_delete(cl);
			node = node->next;
			continue;
			/* TODO: think if an error should be returned */
		}

		if ((ret = read_event_infos(cl, node)) != 0)
		{
			ra_class_delete(cl);
			node = node->next;
			continue;
			/* TODO: think if an error should be returned */
		}

		if ((ret = read_props(cl, node)) != 0)
			break;

  		if ((ret = read_summaries(cl, node)) != 0)
  			return ret;

		node = node->next;
	}

	return ret;
} /* read_class() */


/**
 * \brief read general event infos
 * \param <cl> pointer to an eval-class structure
 * \param <parent> pointer of the parent XML node
 *
 * The function read general event-infos: start- and end-position
 * and the id's for each event.
 */
int
read_event_infos(struct eval_class *cl, xmlNodePtr parent)
{
	unsigned long l;
	xmlChar *c;
	void *id, *start, *end;
	size_t size;
	xmlNodePtr node;
	
	cl->ev = calloc(cl->num_event, sizeof(struct eval_event));

	node = get_node(parent, "event-id");
	c = xmlNodeGetContent(node);
	decode_base64(c, strlen((const char *)c), &id, &size, sizeof(long) * cl->num_event);
	xmlFree(c);
	
	node = get_node(parent, "start-pos");
	c = xmlNodeGetContent(node);
	decode_base64(c, strlen((const char *)c), &start, &size, sizeof(long) * cl->num_event);
	xmlFree(c);

	node = get_node(parent, "end-pos");
	c = xmlNodeGetContent(node);
	decode_base64(c, strlen((const char *)c), &end, &size, sizeof(long) * cl->num_event);
	xmlFree(c);
	
	cl->last_id = -1;
	for (l = 0; l < cl->num_event; l++)
	{
		cl->ev[l].id = ((long *)id)[l];
		if (cl->last_id < cl->ev[l].id)
			cl->last_id = cl->ev[l].id;

		cl->ev[l].start = ((long *)start)[l];
		cl->ev[l].end = ((long *)end)[l];
	}

	free(id);
	free(start);
	free(end);

	return 0;
} /* read_event_infos() */


/**
 * \brief read event-properties
 * \param <set> pointer to an event-set structure
 * \param <parent> pointer of the parent XML node
 *
 * The function reads all event-properties below the XML node 'parent'.
 */
int
read_props(struct eval_class *cl, xmlNodePtr parent)
{
	xmlNodePtr node = parent->children;
	xmlChar *c;
	prop_handle prop;
	long value_type;

	while (node)
	{
		if (xmlStrcmp(node->name, (xmlChar *)"Event-Property") != 0)
		{
			node = node->next;
			continue;
		}

		c = xmlGetProp(node, (xmlChar *)"id");
		if (!c)
		{
			node = node->next;
			continue;
		}
		
		prop = ra_prop_add_predef(cl, (const char *)c);
		if (prop == NULL)
		{
			fprintf(stderr, "property %s can not be created\n", (const char *)c);
			exit(-1);
		}
		xmlFree(c);

		c = xmlGetProp(node, (xmlChar *)"type");
		if (c == NULL)
		{
			_ra_set_error(cl, RA_ERR_EVAL_FILE_CORRUPT, "libRASCH");
			return -1;	/* file corrupt */
		}
		if (xmlStrcmp(c, (xmlChar *)"short") == 0)
			value_type = RA_VALUE_TYPE_SHORT;
		else if (xmlStrcmp(c, (xmlChar *)"long") == 0)
			value_type = RA_VALUE_TYPE_LONG;
		else if (xmlStrcmp(c, (xmlChar *)"double") == 0)
			value_type = RA_VALUE_TYPE_DOUBLE;
		else if (xmlStrcmp(c, (xmlChar *)"char") == 0)
			value_type = RA_VALUE_TYPE_CHAR;
		else if (xmlStrcmp(c, (xmlChar *)"short-array") == 0)
			value_type = RA_VALUE_TYPE_SHORT_ARRAY;
		else if (xmlStrcmp(c, (xmlChar *)"long-array") == 0)
			value_type = RA_VALUE_TYPE_LONG_ARRAY;
		else if (xmlStrcmp(c, (xmlChar *)"double-array") == 0)
			value_type = RA_VALUE_TYPE_DOUBLE_ARRAY;
		else if (xmlStrcmp(c, (xmlChar *)"char-array") == 0)
			value_type = RA_VALUE_TYPE_CHAR_ARRAY;
		else
		{
			_ra_set_error(cl, RA_ERR_EVAL_FILE_CORRUPT, "libRASCH");
			return -1;	/* file corrupt */
		}
		xmlFree(c);

		read_events(prop, value_type, node);

		node = node->next;
	}

	return 0;
} /* read_props_xml() */


/**
 * \brief read event values
 * \param <prop> pointer to an event-property structure
 * \param <parent> pointer of the parent XML node
 *
 * The function reads all event-values below the XML node 'parent'.
 */
int
read_events(prop_handle ph, long value_type, xmlNodePtr parent)
{
	int ret = 0;
	xmlNodePtr node;
	xmlChar *c;
	struct eval_property *prop = ph;
	void *ch_map,  *num_elem;
	size_t size, l, num;
	value_handle vh;

	if ((c = xmlGetProp(parent, (xmlChar *)"num-ch-values")) != NULL)
	{
		prop->num_ch_values = atoi((const char *)c);
		xmlFree(c);
	}
	
	node = get_node(parent, "ch-map");
	c = xmlNodeGetContent(node);
	decode_base64(c, strlen((const char *)c), &ch_map, &size, sizeof(short) * prop->num_ch_values);
	xmlFree(c);
	free(prop->ch_map);
	prop->ch_map = calloc(prop->num_ch_values, sizeof(unsigned short));
	for (l = 0; l < (size_t)prop->num_ch_values; l++)
		prop->ch_map[l] = ((unsigned short *)ch_map)[l];
	free(ch_map);

	if ((c = xmlGetProp(parent, (xmlChar *)"num-elem-type")) != NULL)
	{
		prop->num_type = atoi((const char *)c);
		xmlFree(c);
	}

	node = get_node(parent, "num-elements");
	c = xmlNodeGetContent(node);
	num = prop->num_ch_values * prop->evclass->num_event;
	switch (prop->num_type)
	{
	case RA_PROP_NUM_ELEM_TYPE_BYTE: free(prop->num_elements.b); break;
	case RA_PROP_NUM_ELEM_TYPE_LONG: free(prop->num_elements.l); break;
	}
	if (prop->num_type == RA_PROP_NUM_ELEM_TYPE_BYTE)
	{
		decode_base64(c, strlen((const char *)c), &num_elem, &size, sizeof(unsigned char) * num);
		xmlFree(c);
		prop->num_elements.b = calloc(num, sizeof(unsigned char));
		for (l = 0; l < num; l++)
			prop->num_elements.b[l] = ((unsigned char *)num_elem)[l];			
	}
	else
	{
		decode_base64(c, strlen((const char *)c), &num_elem, &size, sizeof(long) * num);
		xmlFree(c);
		prop->num_elements.l = calloc(num, sizeof(long));
		for (l = 0; l < num; l++)
			prop->num_elements.l[l] = ((long *)num_elem)[l];			
	}
	free(num_elem);

	node = get_node(parent, "value");
	if (node == NULL)
	{
		fprintf(stderr, "read-events: no 'value' node\n");
		/* TODO: set error code */
		return -1;
	}
	vh = ra_value_malloc();
	if ((ret = read_value(vh, NULL, node)) != 0)
	{
		fprintf(stderr, "read-events: can not read value\n");
		ra_value_free(vh);
		/* TODO: set error code */
		return -1;
	}

	/* save values in property memory; also set the entry-size as given in eval-file */
	free_prop_values(prop);
	switch(value_type)
	{
	case RA_VALUE_TYPE_SHORT:
		prop->entry_size = sizeof(short) * prop->num_ch_values;
		ret = set_short_events(prop, vh);
		break;
	case RA_VALUE_TYPE_LONG:
		prop->entry_size = sizeof(long) * prop->num_ch_values;
		ret = set_long_events(prop, vh);
		break;
	case RA_VALUE_TYPE_DOUBLE:
		prop->entry_size = sizeof(double) * prop->num_ch_values;
		ret = set_double_events(prop, vh);
		break;
	case RA_VALUE_TYPE_CHAR:
		prop->entry_size = sizeof(char *) * prop->num_ch_values;
		ret = set_char_events(prop, vh);
		break;
	case RA_VALUE_TYPE_SHORT_ARRAY:
		prop->entry_size = sizeof(short *) * prop->num_ch_values;
		ret = set_short_array_events(prop, vh);
		break;
	case RA_VALUE_TYPE_LONG_ARRAY:
		prop->entry_size = sizeof(long *) * prop->num_ch_values;
		ret = set_long_array_events(prop, vh);
		break;
	case RA_VALUE_TYPE_DOUBLE_ARRAY:
		prop->entry_size = sizeof(double *) * prop->num_ch_values;
		ret = set_double_array_events(prop, vh);
		break;
	case RA_VALUE_TYPE_CHAR_ARRAY:
		prop->entry_size = sizeof(char **) * prop->num_ch_values;
		ret = set_char_array_events(prop, vh);
		break;
	default:
		/* TODO: set error code */
		fprintf(stderr, "read-events: not supported value-type\n");
		ret = -1;
		break;
	}
	ra_value_free(vh);

	return ret;
} /* read_events() */


int
set_short_events(struct eval_property *prop, value_handle vh)
{
	long size;

	size = ra_value_get_num_elem(vh) * sizeof(short);
	prop->value.s = malloc(size);
	memcpy(prop->value.s, ra_value_get_short_array(vh), size);

	return 0;
} /* set_short_events() */


int
set_long_events(struct eval_property *prop, value_handle vh)
{
	long size;

	size = ra_value_get_num_elem(vh) * sizeof(long);
	prop->value.l = malloc(size);
	memcpy(prop->value.l, ra_value_get_long_array(vh), size);

	return 0;
} /* set_long_events() */


int
set_double_events(struct eval_property *prop, value_handle vh)
{
	long size;

	size = ra_value_get_num_elem(vh) * sizeof(double);
	prop->value.d = malloc(size);
	memcpy(prop->value.d, ra_value_get_double_array(vh), size);

	return 0;
} /* set_double_events() */


int
set_char_events(struct eval_property *prop, value_handle vh)
{
	const char **values;
	unsigned long l, m, curr, size, offset;

	values = ra_value_get_string_array_utf8(vh);
	size = prop->evclass->num_event * prop->num_ch_values;
	prop->value.c = calloc(size, sizeof(char *));

	curr = 0;
	for (l = 0; l < prop->evclass->num_event; l++)
	{
		for (m = 0; m < prop->num_ch_values; m++)
		{
			offset = l * prop->num_ch_values + m;

			if (prop->num_elements.b[offset] == 0)
				continue;

			prop->value.c[offset] = malloc(sizeof(char) * strlen(values[curr]) + 1);
			strcpy(prop->value.c[offset], values[curr]);
			curr++;
		}
	}

	return 0;	
} /* set_char_events() */


int
set_short_array_events(struct eval_property *prop, value_handle vh)
{
	const short *values;
	unsigned long l, m, n, curr, size, offset, num;

	values = ra_value_get_short_array(vh);
	size = prop->evclass->num_event * prop->num_ch_values;
	prop->value.sa = calloc(size, sizeof(short *));

	curr = 0;
	for (l = 0; l < prop->evclass->num_event; l++)
	{
		for (m = 0; m < prop->num_ch_values; m++)
		{
			offset = l * prop->num_ch_values + m;

			if (prop->num_type == RA_PROP_NUM_ELEM_TYPE_BYTE)
				num = prop->num_elements.b[offset];
			else
				num = prop->num_elements.l[offset];
			if (num == 0)
				continue;

			prop->value.sa[offset] = malloc(sizeof(short) * num);
			for (n = 0; n < num; n++)
				prop->value.sa[offset][n] = values[curr++];
		}
	}

	return 0;	
} /* set_short_array_events() */


int
set_long_array_events(struct eval_property *prop, value_handle vh)
{
	const long *values;
	unsigned long l, m, n, curr, size, offset, num;

	values = ra_value_get_long_array(vh);
	size = prop->evclass->num_event * prop->num_ch_values;
	prop->value.la = calloc(size, sizeof(long *));

	curr = 0;
	for (l = 0; l < prop->evclass->num_event; l++)
	{
		for (m = 0; m < prop->num_ch_values; m++)
		{
			offset = l * prop->num_ch_values + m;

			if (prop->num_type == RA_PROP_NUM_ELEM_TYPE_BYTE)
				num = prop->num_elements.b[offset];
			else
				num = prop->num_elements.l[offset];
			if (num == 0)
				continue;

			prop->value.la[offset] = malloc(sizeof(long) * num);
			for (n = 0; n < num; n++)
				prop->value.la[offset][n] = values[curr++];
		}
	}

	return 0;	
} /* set_long_array_events() */


int
set_double_array_events(struct eval_property *prop, value_handle vh)
{
	const double *values;
	unsigned long l, m, n, curr, size, offset, num;

	values = ra_value_get_double_array(vh);
	size = prop->evclass->num_event * prop->num_ch_values;
	prop->value.da = calloc(size, sizeof(double *));

	curr = 0;
	for (l = 0; l < prop->evclass->num_event; l++)
	{
		for (m = 0; m < prop->num_ch_values; m++)
		{
			offset = l * prop->num_ch_values + m;

			if (prop->num_type == RA_PROP_NUM_ELEM_TYPE_BYTE)
				num = prop->num_elements.b[offset];
			else
				num = prop->num_elements.l[offset];
			if (num == 0)
				continue;

			prop->value.da[offset] = malloc(sizeof(double) * num);
			for (n = 0; n < num; n++)
				prop->value.da[offset][n] = values[curr++];
		}
	}

	return 0;	
} /* set_double_array_events() */


int
set_char_array_events(struct eval_property *prop, value_handle vh)
{
	const char **values;
	unsigned long l, m, n, curr, size, offset, num;

	values = ra_value_get_string_array_utf8(vh);
	size = prop->evclass->num_event * prop->num_ch_values;
	prop->value.ca = calloc(size, sizeof(char **));

	curr = 0;
	for (l = 0; l < prop->evclass->num_event; l++)
	{
		for (m = 0; m < prop->num_ch_values; m++)
		{
			offset = l * prop->num_ch_values + m;

			if (prop->num_type == RA_PROP_NUM_ELEM_TYPE_BYTE)
				num = prop->num_elements.b[offset];
			else
				num = prop->num_elements.l[offset];
			if (l == 0)
				continue;

			prop->value.ca[offset] = malloc(sizeof(char *) * num);
			for (n = 0; n < num; n++)
			{
				prop->value.ca[offset][n] = malloc(sizeof(char) * strlen(values[curr]) + 1);
				strcpy(prop->value.ca[offset][n], values[curr]);
				curr++;
			}
		}
	}

	return 0;	
} /* set_char_array_events() */


/**
 * \brief read event-summaries
 * \param <cl> pointer to an event-set structure
 * \param <parent> pointer of the parent XML node
 *
 * The function reads all template informations below the XML node 'parent'.
 */
int
read_summaries(struct eval_class *cl, xmlNodePtr parent)
{
	int ret = -1;
	xmlNodePtr node;
	xmlChar *c;
	sum_handle sh;
	struct eval_summary *s;
	value_handle vh_dim_name, vh_dim_unit;
		
	node = parent->children;

	vh_dim_name = ra_value_malloc();
	vh_dim_unit = ra_value_malloc();

	while (node)
	{
		long *ch_size = NULL;
		long *ch_num = NULL;

		if (xmlStrcmp(node->name, (xmlChar *)"Summary") != 0)
		{
			node = node->next;
			continue;
		}

		if ((ret = read_sum_dimensions(node, vh_dim_name, vh_dim_unit)) != 0)
		{
			_ra_set_error(cl, RA_ERR_EVAL_FILE_CORRUPT, "libRASCH");
			goto error;
		}

		c = xmlGetProp(node, (xmlChar *)"id");
		if (!c)
		{
			_ra_set_error(cl, RA_ERR_EVAL_FILE_CORRUPT, "libRASCH");
			return -1;
		}
		sh = ra_sum_add(cl, (const char *)c, NULL, NULL, ra_value_get_num_elem(vh_dim_name),
				ra_value_get_string_array(vh_dim_name), ra_value_get_string_array(vh_dim_unit));
		xmlFree(c);
		if (!sh)
		{
			fprintf(stderr, "read-summaries: can't add summary\n");
			return -1;
		}

		read_attributes(node, sh);
		
		c = xmlGetProp(node, (xmlChar *)"num-data-elements");
		if (!c)
		{
			_ra_set_error(cl, RA_ERR_EVAL_FILE_CORRUPT, "libRASCH");
			return -1;
		}
		s = (struct eval_summary *)sh;
		s->num_data_elements = atol((const char *)c);
		xmlFree(c);

		if ((ret = read_summary_ch_desc(sh, node)) != 0)
			return ret;
		

		if ((ret = read_summary_data_element(sh, node)) != 0)
			return ret;

		node = node->next;

		free(ch_size);
		free(ch_num);
	}
	
	ret = 0;

 error:
	ra_value_free(vh_dim_name);
	ra_value_free(vh_dim_unit);
	
	return ret;
} /* read_summaries() */


int
read_sum_dimensions(xmlNodePtr node, value_handle vh_dim_name, value_handle vh_dim_unit)
{
	int ret = -1;
	long l, num_dim, idx;
	xmlChar *c;
	char **name;
	char **unit;
	xmlNodePtr dim_node;

	if ((c = xmlGetProp(node, (xmlChar *)"num-dimensions")) == NULL)
		return -1;

	num_dim = atol((const char *)c);
	xmlFree(c);

	name = calloc(num_dim, sizeof(char *));
	unit = calloc(num_dim, sizeof(char *));

	dim_node = node->children;
	while (dim_node)
	{
		if (xmlStrcmp(dim_node->name, (xmlChar *)"dimension") != 0)
		{
			dim_node = dim_node->next;
			continue;
		}
		
		if ((c = xmlGetProp(dim_node, (xmlChar *)"index")) == NULL)
			goto error;
		idx = atol((const char *)c);
		xmlFree(c);
		if (idx >= num_dim)
			goto error;
		
		if ((c = xmlGetProp(dim_node, (xmlChar *)"name")) == NULL)
			return -1;
		name[idx] = malloc(sizeof(char) * (strlen((const char *)c) + 1));
		strcpy(name[idx], (const char *)c);
		xmlFree(c);
		
		if ((c = xmlGetProp(dim_node, (xmlChar *)"unit")) == NULL)
			goto error;
		unit[idx] = malloc(sizeof(char) * (strlen((const char *)c) + 1));
		strcpy(unit[idx], (const char *)c);
		xmlFree(c);
		
		dim_node = dim_node->next;
	}

	ra_value_set_string_array(vh_dim_name, (const char **)name, num_dim);
	ra_value_set_string_array(vh_dim_unit, (const char **)unit, num_dim);

	ret = 0;

 error:
	for (l = 0; l < num_dim; l++)
	{
		if (name && name[l])
			free(name[l]);
		if (unit && unit[l])
			free(unit[l]);
	}
	if (name)
		free(name);
	if (unit)
		free(unit);

	return ret;
} /* read_sum_dimensions() */


int
read_summary_ch_desc(sum_handle sh, xmlNodePtr parent)
{
	xmlNodePtr node;
	xmlChar *c;
	struct eval_summary *s = (struct eval_summary *)sh;

	s->num_ch = 0;
	s->ch_desc = NULL;

	node = parent->children;
	while (node)
	{
		if (xmlStrcmp(node->name, (xmlChar *)"channel-description") != 0)
		{
			node = node->next;
			continue;
		}

		s->num_ch++;
		s->ch_desc = realloc(s->ch_desc, sizeof(struct eval_sum_ch_desc) * s->num_ch);
		memset(s->ch_desc + s->num_ch - 1, 0, sizeof(struct eval_sum_ch_desc));

		if ((c = xmlGetProp(node, (xmlChar *)"ch-number")) == NULL)
		{
			_ra_set_error(sh, RA_ERR_EVAL_FILE_CORRUPT, "libRASCH");
			return -1;
		}
		s->ch_desc[s->num_ch-1].ch = atol((const char *)c);
		xmlFree(c);
		if ((c = xmlGetProp(node, (xmlChar *)"fiducial-offset")) == NULL)
		{
			_ra_set_error(sh, RA_ERR_EVAL_FILE_CORRUPT, "libRASCH");
			return -1;
		}
		s->ch_desc[s->num_ch-1].fiducial_offset = atol((const char *)c);
		xmlFree(c);

		node = node->next;
	}

	return 0;
} /* read_summary_ch_desc() */


int
read_summary_data_element(sum_handle sh, xmlNodePtr parent)
{
	int ret = 0;
	unsigned long l, idx;
	struct eval_sum_data *d;
	xmlNodePtr node, node_id, child;
	xmlChar *c;
	value_handle vh;
	const long *ids;
	struct eval_summary *s = (struct eval_summary *)sh;

	node = parent->children;

	vh = ra_value_malloc();
	s->last_data_id = 0;
	while (node)
	{
		if (xmlStrcmp(node->name, (xmlChar *)"data") != 0)
		{
			node = node->next;
			continue;
		}

		d = calloc(1, sizeof(struct eval_sum_data));
		ra_list_add((void **)&(s->sum), d);

		c = xmlGetProp(node, (xmlChar *)"id");
		if (!c)
		{
			_ra_set_error(sh, RA_ERR_EVAL_FILE_CORRUPT, "libRASCH");
			return -1;
		}
		d->id = (unsigned short)atoi((const char *)c);
		xmlFree(c);
		if (s->last_data_id < d->id)
			s->last_data_id = d->id;

		node_id = get_node(node, "event-ids");
		if ((node_id == NULL) || (node_id->children == NULL) || (node_id->children->next != NULL))
		{
			_ra_set_error(sh, RA_ERR_EVAL_FILE_CORRUPT, "libRASCH");
			return -1;
		}
		if ((ret = read_value(vh, NULL, node_id->children)) != 0)
			return ret;

		d->num_events = ra_value_get_num_elem(vh);
		d->event_ids = malloc(sizeof(long) * d->num_events);
		ids = ra_value_get_long_array(vh);
		for (l = 0; l < d->num_events; l++)
			d->event_ids[l] = ids[l];

		d->data = malloc(sizeof(value_handle *) * s->num_data_elements);

		child = node->children;
		while(child)
		{
			if (xmlStrcmp(child->name, (xmlChar *)"value") != 0)
			{
				child = child->next;
				continue;
			}

			if ((ret = read_value(vh, &idx, child)) != 0)
				return ret;
			if ((idx < 0) || (idx >= s->num_data_elements))
			{
				_ra_set_error(sh, RA_ERR_EVAL_FILE_CORRUPT, "libRASCH");
				return -1;
			}
			
			d->data[idx] = ra_value_malloc();
			ra_value_copy(d->data[idx], vh);

			child = child->next;
		}

		node = node->next;
	}
	ra_value_free(vh);

	return ret;
} /* read_summary_data_element() */


xmlChar *
get_attribute(xmlNodePtr parent, const char *attribute)
{
	xmlNodePtr node;
	xmlChar *c = NULL;

	node = parent->children;
	while (node)
	{
		if (xmlStrcmp(node->name, (xmlChar *)"attribute") != 0)
		{
			node = node->next;
			continue;
		}
		
		c = xmlGetProp(node, (xmlChar *)"name");
		if (!c)
		{
			fprintf(stderr, "get-attribute: no 'name' property\n");
			/* TODO: set error */
			return NULL;
		}
		if (strcmp((const char *)c, attribute) == 0)
		{
			xmlFree(c);
			c = xmlNodeGetContent(node);
			break;
		}

		node = node->next;
	}

	return c;
} /* get_attribute() */


int
read_attributes(any_handle h, xmlNodePtr parent)
{
	int ret = 0;
	value_handle vh = NULL;
	xmlChar *name;
	xmlNodePtr node;

	vh = ra_value_malloc();
	node = parent->children;
	while (node)
	{
		if (xmlStrcmp(node->name, (xmlChar *)"attribute") != 0)
		{
			node = node->next;
			continue;
		}
		
		name = xmlGetProp(node, (xmlChar *)"name");
		if (!name)
		{
			/* TODO: set error */
			fprintf(stderr, "read-attributes: no 'name' property\n");
			ret = -1;
			break;
		}

		if (node->children == NULL)
		{
			fprintf(stderr, "read-attributes: no child-node\n");
			/* TODO: set error code */
			ret = -1;
			break;
		}
		if (node->children->next != NULL)
		{
			/* at the moment only one children, the value, is allowed */
			/* TODO: set error code */
			fprintf(stderr, "read-attributes: more than one child-node\n");
			ret = -1;
			break;
		}

		if ((ret = read_value(vh, NULL, node->children)) != 0)
			break;
		ra_eval_attribute_set(h, (const char *)name, vh);
		xmlFree(name);

		node = node->next;
	}
	ra_value_free(vh);

	return ret;
} /* read_attributes() */


int
read_value(value_handle vh, unsigned long *idx, xmlNodePtr node)
{
	int ret = 0;
	xmlChar *c;
	char **st;
	long value_type;
	unsigned long num, num_elem, cnt;
	void *t = NULL;
	size_t size;
	char *p_curr, *p_next;

	ra_value_reset(vh);

	if (xmlStrcmp(node->name, (xmlChar *)"value") != 0)
	{
		/* wrong tag */
		/* TODO: set error code */
		fprintf(stderr, "read-value: no 'value' property\n");
		return -1;
	}

	c = xmlGetProp(node, (xmlChar *)"index");
	if ((c != NULL) && (idx != NULL))
	{
		*idx = atol((const char *)c);
		xmlFree(c);
	}

	c = xmlGetProp(node, (xmlChar *)"type");
	if (c == NULL)
	{
		fprintf(stderr, "read-value: no 'type' property\n");
		/* TODO: set error code */
		return -1;
	}

	if (xmlStrcmp(c, (xmlChar *)"short") == 0)
		value_type = RA_VALUE_TYPE_SHORT;
	else if (xmlStrcmp(c, (xmlChar *)"long") == 0)
		value_type = RA_VALUE_TYPE_LONG;
	else if (xmlStrcmp(c, (xmlChar *)"double") == 0)
		value_type = RA_VALUE_TYPE_DOUBLE;
	else if (xmlStrcmp(c, (xmlChar *)"char") == 0)
		value_type = RA_VALUE_TYPE_CHAR;
	else if (xmlStrcmp(c, (xmlChar *)"voidp") == 0)
		value_type = RA_VALUE_TYPE_VOIDP;
	else if (xmlStrcmp(c, (xmlChar *)"short-array") == 0)
		value_type = RA_VALUE_TYPE_SHORT_ARRAY;
	else if (xmlStrcmp(c, (xmlChar *)"long-array") == 0)
		value_type = RA_VALUE_TYPE_LONG_ARRAY;
	else if (xmlStrcmp(c, (xmlChar *)"double-array") == 0)
		value_type = RA_VALUE_TYPE_DOUBLE_ARRAY;
	else if (xmlStrcmp(c, (xmlChar *)"char-array") == 0)
		value_type = RA_VALUE_TYPE_CHAR_ARRAY;
	else if (xmlStrcmp(c, (xmlChar *)"voidp-array") == 0)
		value_type = RA_VALUE_TYPE_VOIDP_ARRAY;
	else
	{
		xmlFree(c);
		fprintf(stderr, "read-value: type %s not supported\n", (const char *)c);
		/* TODO: set error code */
		return -1;
	}
	xmlFree(c);

	num = ra_value_get_num_elem(vh);
	c = xmlGetProp(node, (xmlChar *)"size");
	if (c == NULL)
	{
		fprintf(stderr, "read-value: no 'size' property\n");
		/* TODO: set error code */
		return -1;
	}
	num_elem = atol((const char *)c);
	xmlFree(c);

	c = xmlNodeGetContent(node);

	switch (value_type)
	{
	case RA_VALUE_TYPE_SHORT:
		decode_base64(c, strlen((const char *)c), &t, &size, sizeof(short));
		ra_value_set_short(vh, *((short *)t));
		break;
	case RA_VALUE_TYPE_LONG:
		decode_base64(c, strlen((const char *)c), &t, &size, sizeof(long));
		ra_value_set_long(vh, *((long *)t));
		break;
	case RA_VALUE_TYPE_DOUBLE:
		decode_base64(c, strlen((const char *)c), &t, &size, sizeof(double));
		ra_value_set_double(vh, *((double *)t));
		break;
	case RA_VALUE_TYPE_CHAR:
		ra_value_set_string_utf8(vh, (const char *)c);
		break;
	case RA_VALUE_TYPE_VOIDP:
		decode_base64(c, strlen((const char *)c), &t, &size, sizeof(void *));
		ra_value_set_voidp(vh, (void *)t);
		break;
	case RA_VALUE_TYPE_SHORT_ARRAY:
		decode_base64(c, strlen((const char *)c), &t, &size, sizeof(short) * num_elem);
		ra_value_set_short_array(vh, (short *)t, num_elem);
		break;
	case RA_VALUE_TYPE_LONG_ARRAY:
		decode_base64(c, strlen((const char *)c), &t, &size, sizeof(long) * num_elem);
		ra_value_set_long_array(vh, (long *)t, num_elem);
		break;
	case RA_VALUE_TYPE_DOUBLE_ARRAY:
		decode_base64(c, strlen((const char *)c), &t, &size, sizeof(double) * num_elem);
		ra_value_set_double_array(vh, (double *)t, num_elem);
		break;
	case RA_VALUE_TYPE_CHAR_ARRAY:
		st = malloc(sizeof(char *) * num_elem);
		p_curr = (char *)c;
		p_next = find_next_string(p_curr);
		cnt = 0;
		while (p_curr)
		{
			if (cnt >= num_elem)
				break;

			st[cnt] = malloc(sizeof(char *) * strlen(p_curr) + 1);
			strcpy(st[cnt], p_curr);
			cnt++;

			if (!p_next)
				break;

			p_curr = p_next;
			p_next = find_next_string(p_curr);
		}
		if (cnt == num_elem)
			ra_value_set_string_array_utf8(vh, (const char **)st, num_elem);
		else
		{
			fprintf(stderr, "read-value - char-array: cnt(=%ld) != num_elem(%ld)\n", cnt, num_elem);
			/* TODO: set error code */
			ret = -1;
		}
		break;
	case RA_VALUE_TYPE_VOIDP_ARRAY:
		decode_base64(c, strlen((const char *)c), &t, &size, sizeof(void *) * num_elem);
		ra_value_set_voidp_array(vh, (const void **)t, num_elem);
		break;
	}	
	xmlFree(c);
	free(t);

	return ret;
} /* read_value() */



