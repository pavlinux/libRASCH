#ifndef ANNOT_DLG_GTK_H
#define ANNOT_DLG_GTK_H

#include <gtkmm.h>
#include <libglademm/xml.h>
#include <gtkmm/liststore.h>
#include "ts_view_general.h"

class annot_dlg_gtk : public Gtk::Dialog
{
public:
	annot_dlg_gtk(BaseObjectType* cobject, const Glib::RefPtr<Gnome::Glade::Xml> ref_xml);
	~annot_dlg_gtk();
	
	void init(struct ch_info *ch_infos, int num, int curr_ch, char **predefs, int num_predefs,
		  Glib::ustring &text, bool noise = false, bool ignore = false);

	Glib::ustring annot() { return annot_string; }
	bool is_noise() { return noise; }
	bool do_ignore() { return ignore; }
	int channel() { return ch_selected; }

 protected:
	void delete_pressed();
	void cancel_pressed();
	void ok_pressed();

	void on_annot_changed();
	
	Glib::ustring annot_string;
	bool noise;
	bool ignore;
	int ch_selected;

	//Tree model columns:
	class model_columns : public Gtk::TreeModel::ColumnRecord
		{
		public:
			model_columns() { add(col_ch); add(col_name); }
			
			Gtk::TreeModelColumn<int> col_ch;
			Gtk::TreeModelColumn<Glib::ustring> col_name;
		}; /* class model_columns */
	
	model_columns columns;
	
	Gtk::Window* dlg;
	
	Gtk::ComboBox *ch_list;	
	Glib::RefPtr<Gtk::ListStore> ch_tree_model;
	
	Gtk::ComboBox *annot_list;	
	Glib::RefPtr<Gtk::ListStore> annot_tree_model;
	
	Gtk::CheckButton *btn_noise;
	Gtk::CheckButton *btn_ignore;
	
	Gtk::TextView *annot_text;
	
	Gtk::Button *btn_delete;
	Gtk::Button *btn_cancel;
	Gtk::Button *btn_ok;
}; /* class annot_dlg_gtk */


#endif /* ANNOT_DLG_GTK_H */
