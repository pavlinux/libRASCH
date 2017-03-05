#ifndef CH_OPT_DLG_GTK_H
#define CH_OPT_DLG_GTK_H

#include <gtkmm.h>
#include <libglademm/xml.h>
#include <gtkmm/liststore.h>
#include "ts_view_general.h"

class ch_opt_dlg : public Gtk::Dialog
{
public:
	ch_opt_dlg(BaseObjectType* cobject, const Glib::RefPtr<Gnome::Glade::Xml> ref_xml);
	~ch_opt_dlg();
	
	void init(struct ch_info *ch_infos, int num, int curr);

protected:
	void on_combo_changed();
	void centered_pressed();
	void minmax_pressed();
	void use_res_pressed();
	void cancel_pressed();
	void ok_pressed();

	void ch_selected(int idx);
	void save_ch_data(int idx);

	struct ch_info *ch;
	int num_ch;
	int curr_ch_idx;

	//Tree model columns:
	class model_columns : public Gtk::TreeModel::ColumnRecord
	{
	public:
		model_columns() { add(col_idx); add(col_name); }
			
		Gtk::TreeModelColumn<int> col_idx;
		Gtk::TreeModelColumn<Glib::ustring> col_name;
	}; /* class model_columns */
	
	model_columns columns;

	Gtk::Window* dlg;
	
	Gtk::ComboBox *ch_list;	
	Glib::RefPtr<Gtk::ListStore> tree_model;

	Gtk::CheckButton *show_channel;
	Gtk::CheckButton *invert_channel;
	Gtk::Entry *resolution;
	Gtk::Label *res_unit;
	
	Gtk::RadioButton *btn_centered;
	Gtk::Label *position_percent_text;
	Gtk::Entry *position_percent;
	
	Gtk::RadioButton *btn_minmax;
	Gtk::Label *min_text;
	Gtk::Entry *min;
	Gtk::Label *max_text;
	Gtk::Entry *max;
	Gtk::CheckButton *use_resolution;

	Gtk::Button *btn_cancel;
	Gtk::Button *btn_ok;
}; /* class ch_opt_dlg */


#endif /* CH_OPT_DLG_GTK_H */
