/***************************************************************************
                          settings.cpp  -  description
                             -------------------
    begin                : Die Aug 13 2002
    copyright            : (C) 2002-2004 by Raphael Schneider
    email                : rasch@med1.med.tum.de

    $Header: /home/cvs_repos.bak/librasch/raschlab/general/settings.cpp,v 1.2 2004/04/26 20:00:02 rasch Exp $
 ***************************************************************************/

#include <string.h>

#include "settings.h"


settings::settings()
{
	_maximized = false;
	_pos_ok = false;
	_pos_x = _pos_y = 0;
	_size_ok = false;
	_size_w = _size_h = 0;
	_save_eval_ascii = false;
}  // constructor


settings::~settings()
{
}  // desctuctor


int
settings::read()
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
settings::read_settings_xml(xmlNodePtr parent)
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

		node = node->next;

	}

	return ret;
}  // read_settings_xml()


void
settings::read_appearance(xmlNodePtr parent)
{
	xmlNodePtr child;
	xmlChar *c;

	child = get_node(parent, "maximized");
	if (child)
		_maximized = true;

	child = get_node(parent, "pos");
	if (child)
	{
		_pos_ok = true;
		c = xmlGetProp(child, (xmlChar *)"x");
		if (c)
		{
			_pos_x = atoi((const char *)c);
			xmlFree(c);
		}
		else
			_pos_x = 0;

		c = xmlGetProp(child, (xmlChar *)"y");
		if (c)
		{
			_pos_y = atoi((const char *)c);
			xmlFree(c);
		}
		else
			_pos_x = 0;
	}

	child = get_node(parent, "size");
	if (child)
	{
		_size_ok = true;
		c = xmlGetProp(child, (xmlChar *)"w");
		if (c)
		{
			_size_w = atoi((const char *)c);
			xmlFree(c);
		}
		else
			_size_w = 640;

		c = xmlGetProp(child, (xmlChar *)"h");
		if (c)
		{
			_size_h = atoi((const char *)c);
			xmlFree(c);
		}
		else
			_size_h = 480;
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
settings::read_eval(xmlNodePtr parent)
{
	xmlNodePtr child;

	_save_eval_ascii = false;

	child = get_node(parent, "save_ascii");
	if (child)
		_save_eval_ascii = true;
}  // read_eval()


int
settings::save()
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
settings::write_settings_xml(xmlNodePtr parent)
{
	write_appearance(parent);
	write_eval(parent);

	return 0;
}  // write_settings_xml()


void
settings::write_appearance(xmlNodePtr parent)
{
	xmlNodePtr node;
	xmlNodePtr values;
	char t[50];

	node = xmlNewChild(parent, NULL, (xmlChar *)"appearance", NULL);
	if (_maximized)
		xmlNewChild(node, NULL, (xmlChar *)"maximized", NULL);

	values = xmlNewChild(node, NULL, (xmlChar *)"pos", NULL);
	sprintf(t, "%d", _pos_x);
	xmlSetProp(values, (xmlChar *)"x", (xmlChar *)t);
	sprintf(t, "%d", _pos_y);
	xmlSetProp(values, (xmlChar *)"y", (xmlChar *)t);

	values = xmlNewChild(node, NULL, (xmlChar *)"size", NULL);
	sprintf(t, "%d", _size_w);
	xmlSetProp(values, (xmlChar *)"w", (xmlChar *)t);
	sprintf(t, "%d", _size_h);
	xmlSetProp(values, (xmlChar *)"h", (xmlChar *)t);

	values = xmlNewChild(node, NULL, (xmlChar *)"last-path", (xmlChar *)_last_path);
	
}  // write_appearance()


void
settings::write_eval(xmlNodePtr parent)
{
	xmlNodePtr node;

	node = xmlNewChild(parent, NULL, (xmlChar *)"evaluation", NULL);
	if (_save_eval_ascii)
		xmlNewChild(node, NULL, (xmlChar *)"save_ascii", NULL);
}  // write_eval()


void
settings::get_filename(char *fn, int )
{
#ifdef LINUX
	sprintf(fn, "%s/.RASCHlab.conf", getenv("HOME"));
#else
	sprintf(fn, "RASCHlab.conf");
#endif
}  // get_filename()


xmlNodePtr
settings::get_node(xmlNodePtr parent, char *name)
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
settings::set_last_path(const char *path)
{
	strncpy(_last_path, path, 1024);
	_last_path[1023] = '\0';
}  // set_last_path()


int
settings::get_last_path(char *path, int buf_size)
{
	int size_use = buf_size;
	if (size_use > 1024)
		size_use = 1024;

	strncpy(path, _last_path, size_use);

	return strlen(_last_path);
}  // get_last_path()

