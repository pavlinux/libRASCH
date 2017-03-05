/*
 * settings.cpp  -  description
 *
 * Copyright 2006 by Raphael Schneider <librasch@gmail.com>
 *
 * $Id: $
 */

#include <string.h>

#include "settings.h"


raschlab_settings::raschlab_settings()
{
	maximized = false;
	pos_ok_flag = false;
	pos_x_value = pos_y_value = 0;
	size_ok_flag = false;
	size_w_value = size_h_value = 0;
	save_eval_ascii_flag = false;
}  // constructor


raschlab_settings::~raschlab_settings()
{
}  // desctuctor


int
raschlab_settings::read()
{
	char fn[1024];
	int ret;
	xmlDocPtr doc;
	FILE *fp;

	get_filename(fn, 1024);

	/* fisrt check if eval-file is available */
	fp = fopen(fn, "r");
	if (fp == NULL)
		return 0;
	fclose(fp);

	if ((doc = xmlParseFile(fn)) == NULL)
		return -1;
	if (!doc->children || (strcmp((const char *)doc->children->name, "RASCHlab-settings") != 0))
	{
		xmlFreeDoc(doc);
		return -1;
	}

	ret = read_settings_xml(doc->children);

	xmlFreeDoc(doc);

	return ret;
}  // read()


int
raschlab_settings::read_settings_xml(xmlNodePtr parent)
{
	int ret = 0;
	xmlNodePtr node;

	node = parent->children;
	while (node)
	{		
		if (strcmp((const char *)node->name, "appearance") == 0)
			read_appearance(node);

		if (strcmp((const char *)node->name, "evaluation") == 0)
			read_eval(node);

		if (strcmp((const char *)node->name, "recent-files") == 0)
			read_recent_files(node);

		node = node->next;
	}

	return ret;
}  // read_settings_xml()


void
raschlab_settings::read_appearance(xmlNodePtr parent)
{
	xmlNodePtr child;
	xmlChar *c;

	child = get_node(parent, "maximized");
	if (child)
		maximized = true;

	child = get_node(parent, "pos");
	if (child)
	{
		pos_ok_flag = true;
		c = xmlGetProp(child, (xmlChar *)"x");
		if (c)
		{
			pos_x_value = atoi((const char *)c);
			xmlFree(c);
		}
		else
			pos_x_value = 0;

		c = xmlGetProp(child, (xmlChar *)"y");
		if (c)
		{
			pos_y_value = atoi((const char *)c);
			xmlFree(c);
		}
		else
			pos_x_value = 0;
	}

	child = get_node(parent, "size");
	if (child)
	{
		size_ok_flag = true;
		c = xmlGetProp(child, (xmlChar *)"w");
		if (c)
		{
			size_w_value = atoi((const char *)c);
			xmlFree(c);
		}
		else
			size_w_value = 640;

		c = xmlGetProp(child, (xmlChar *)"h");
		if (c)
		{
			size_h_value = atoi((const char *)c);
			xmlFree(c);
		}
		else
			size_h_value = 480;
	}

	child = get_node(parent, "last-path");
	if (child)
	{
		c = xmlNodeGetContent(child);
		if (c)
		{
			set_last_path((const char *)c);
			xmlFree(c);
		}
	}
}  // read_appearance()


void
raschlab_settings::read_eval(xmlNodePtr parent)
{
	xmlNodePtr child;

	save_eval_ascii_flag = false;

	child = get_node(parent, "save_ascii");
	if (child)
		save_eval_ascii_flag = true;
}  // read_eval()


void
raschlab_settings::read_recent_files(xmlNodePtr parent)
{
	xmlNodePtr child;
	xmlChar *c;
	char t[50];

	recent_used_files.empty();

	c = xmlGetProp(parent, (xmlChar *)"number");
	if (!c)
		return;

	int num = atoi((const char *)c);
	for (int i = 0; i < num; i++)
	{
		sprintf(t, "file%02d", i);
		child = get_node(parent, t);
		if (!child)
			continue;

		c = xmlNodeGetContent(child);
		if (c)
		{
			recent_used_files.insert(recent_used_files.end(), (const char *)c);
			xmlFree(c);
		}
	}
}  // read_recent_files()


int
raschlab_settings::save()
{
	char fn[1024];
	int ret;
	xmlDocPtr doc;

	get_filename(fn, 1024);

	doc = xmlNewDoc((xmlChar *)"1.0");
	doc->children = xmlNewDocNode(doc, NULL, (xmlChar *)"RASCHlab-settings", NULL);

	ret = write_settings_xml(doc->children);
	if (ret != 0)
		return ret;

	xmlSaveFile(fn, doc);

	xmlFreeDoc(doc);

	return ret;
}  // save()


int
raschlab_settings::write_settings_xml(xmlNodePtr parent)
{
	write_appearance(parent);
	write_eval(parent);
	write_recent_files(parent);

	return 0;
}  // write_settings_xml()


void
raschlab_settings::write_appearance(xmlNodePtr parent)
{
	xmlNodePtr node;
	xmlNodePtr values;
	char t[50];

	node = xmlNewChild(parent, NULL, (xmlChar *)"appearance", NULL);
	if (maximized)
		xmlNewChild(node, NULL, (xmlChar *)"maximized", NULL);

	values = xmlNewChild(node, NULL, (xmlChar *)"pos", NULL);
	sprintf(t, "%d", pos_x_value);
	xmlSetProp(values, (xmlChar *)"x", (xmlChar *)t);
	sprintf(t, "%d", pos_y_value);
	xmlSetProp(values, (xmlChar *)"y", (xmlChar *)t);

	values = xmlNewChild(node, NULL, (xmlChar *)"size", NULL);
	sprintf(t, "%d", size_w_value);
	xmlSetProp(values, (xmlChar *)"w", (xmlChar *)t);
	sprintf(t, "%d", size_h_value);
	xmlSetProp(values, (xmlChar *)"h", (xmlChar *)t);

	values = xmlNewChild(node, NULL, (xmlChar *)"last-path", (xmlChar *)last_path);
	
}  // write_appearance()


void
raschlab_settings::write_eval(xmlNodePtr parent)
{
	xmlNodePtr node;

	node = xmlNewChild(parent, NULL, (xmlChar *)"evaluation", NULL);
	if (save_eval_ascii_flag)
		xmlNewChild(node, NULL, (xmlChar *)"save_ascii", NULL);
}  // write_eval()


void
raschlab_settings::write_recent_files(xmlNodePtr parent)
{
	xmlNodePtr node;
	char t[50];

	node = xmlNewChild(parent, NULL, (xmlChar *)"recent-files", NULL);
	sprintf(t, "%d", recent_used_files.size());
	xmlSetProp(node, (xmlChar *)"number", (xmlChar *)t);

	for (size_t i = 0; i < recent_used_files.size(); i++)
	{
		sprintf(t, "file%02d", i);
		xmlNewChild(node, NULL, (xmlChar *)t, (xmlChar *)recent_used_files[i].c_str());
	}
}  // write_recent_files()


void
raschlab_settings::get_filename(char *fn, int )
{
#ifdef LINUX
	sprintf(fn, "%s/.RASCHlab.conf", getenv("HOME"));
#else
	sprintf(fn, "RASCHlab.conf");
#endif
}  // get_filename()


xmlNodePtr
raschlab_settings::get_node(xmlNodePtr parent, const char *name)
{
	xmlNodePtr child = parent->children;

	while (child)
	{
		if (strcmp((const char *)child->name, name) == 0)
			return child;
		child = child->next;
	}

	return NULL;
} // get_node()


void
raschlab_settings::set_last_path(const char *path)
{
	strncpy(last_path, path, 1024);
	last_path[1023] = '\0';
}  // set_last_path()


int
raschlab_settings::get_last_path(char *path, int buf_size)
{
	int size_use = buf_size;
	if (size_use > 1024)
		size_use = 1024;

	strncpy(path, last_path, size_use);

	return strlen(last_path);
}  // get_last_path()


void
raschlab_settings::add_recent_used_file(string fn)
{
	vector<string>::iterator it = recent_used_files.begin();
	for(vector<string>::iterator it = recent_used_files.begin(); it != recent_used_files.end(); it++)
	{
		if (fn.compare(*it) == 0)
		{
			recent_used_files.erase(it);
			break;
		}
	}

	recent_used_files.insert(recent_used_files.begin(), fn);
	if (recent_used_files.size() >= 10)
		recent_used_files.pop_back();
}  // add_recent_used_file()

