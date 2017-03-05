/*
 * settings.h  -  
 *
 * Copyright 2006 by Raphael Schneider <librasch@gmail.com>
 *
 * $Id: $
 */


#ifndef SETTINGS_H
#define SETTINGS_H

#include <vector>
#include <string>

using namespace std;

extern "C"
{
#include <libxml/tree.h>
#include <libxml/parser.h>
}


class raschlab_settings
{
public:
	raschlab_settings();
	~raschlab_settings();

public:
	int read();
	int save();

	bool is_maximized() { return maximized; }
	void set_maximized(bool m) { maximized = m; }
	bool pos_ok() { return pos_ok_flag; }
	bool size_ok() { return size_ok_flag; }

	int pos_x() { return pos_x_value; }
	void set_pos_x(int x) { pos_x_value = x; }

	int pos_y() { return pos_y_value; }
	void set_pos_y(int y) { pos_y_value = y; }

	int size_w() { return size_w_value; }
	void set_size_w(int w) { size_w_value = w; }

	int size_h() { return size_h_value; }
	void set_size_h(int h) { size_h_value = h; }

	void set_last_path(const char *path);
	int get_last_path(char *path, int buf_size);

	bool save_eval_ascii() { return save_eval_ascii_flag; }
	void set_save_eval_ascii(bool b) { save_eval_ascii_flag = b; }

	void add_recent_used_file(string fn);
	vector<string> get_recent_used_files() { return recent_used_files; }

protected:
	bool maximized;

	bool pos_ok_flag;
	int pos_x_value, pos_y_value;

	bool size_ok_flag;
	int size_w_value, size_h_value;

	char last_path[1024];

	bool save_eval_ascii_flag;

	vector<string> recent_used_files;

	void get_filename(char *fn, int buf_size);

	int read_settings_xml(xmlNodePtr parent);
	void read_appearance(xmlNodePtr parent);
	void read_eval(xmlNodePtr parent);
	void read_recent_files(xmlNodePtr parent);

	int write_settings_xml(xmlNodePtr parent);
	void write_appearance(xmlNodePtr parent);
	void write_eval(xmlNodePtr parent);
	void write_recent_files(xmlNodePtr parent);

	xmlNodePtr get_node(xmlNodePtr parent, const char *name);
};  // class raschlab_settings


#endif  // SETTINGS_H
