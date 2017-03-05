/***************************************************************************
                          settings.h  -  description
                             -------------------
    begin                : Die Aug 13 2002
    copyright            : (C) 2002-2004 by Raphael Schneider
    email                : rasch@med1.med.tum.de

    $Header: /home/cvs_repos.bak/librasch/raschlab/general/settings.h,v 1.2 2004/04/26 20:00:02 rasch Exp $
 ***************************************************************************/


#ifndef SETTINGS_H
#define SETTINGS_H

extern "C"
{
#include <libxml/tree.h>
#include <libxml/parser.h>
}

class settings
{
public:
	settings();
	~settings();

public:
	int read();
	int save();

	bool is_maximized() { return _maximized; }
	void set_maximized(bool m) { _maximized = m; }
	bool pos_ok() { return _pos_ok; }
	bool size_ok() { return _size_ok; }

	int pos_x() { return _pos_x; }
	void set_pos_x(int x) { _pos_x = x; }

	int pos_y() { return _pos_y; }
	void set_pos_y(int y) { _pos_y = y; }

	int size_w() { return _size_w; }
	void set_size_w(int w) { _size_w = w; }

	int size_h() { return _size_h; }
	void set_size_h(int h) { _size_h = h; }

	void set_last_path(const char *path);
	int get_last_path(char *path, int buf_size);

	bool save_eval_ascii() { return _save_eval_ascii; }
	void set_save_eval_ascii(bool b) { _save_eval_ascii = b; }

protected:
	bool _maximized;

	bool _pos_ok;
	int _pos_x, _pos_y;

	bool _size_ok;
	int _size_w, _size_h;

	char _last_path[1024];

	bool _save_eval_ascii;

	void get_filename(char *fn, int buf_size);

	int read_settings_xml(xmlNodePtr parent);
	void read_appearance(xmlNodePtr parent);
	void read_eval(xmlNodePtr parent);

	int write_settings_xml(xmlNodePtr parent);
	void write_appearance(xmlNodePtr parent);
	void write_eval(xmlNodePtr parent);

	xmlNodePtr get_node(xmlNodePtr parent, char *name);
};  // class settings


#endif  // SETTINGS_H
