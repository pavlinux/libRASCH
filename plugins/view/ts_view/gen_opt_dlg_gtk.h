#ifndef GEN_OPT_DLG_GTK_H
#define GEN_OPT_DLG_GTK_H

#include <gtkmm.h>
#include <libglademm/xml.h>
#include "ts_view_general.h"

class gen_opt_dlg : public Gtk::Dialog
{
public:
	gen_opt_dlg(BaseObjectType* cobject, const Glib::RefPtr<Gnome::Glade::Xml> ref_xml);
	~gen_opt_dlg();
	
	void init(struct ts_options *o);

protected:
	void fixed_ch_height_pressed();
	void cancel_pressed();
	void ok_pressed();

	struct ts_options *opt;

	Gtk::Window* dlg;
	
	Gtk::CheckButton *show_status;
	Gtk::CheckButton *show_ch_info;
	Gtk::CheckButton *show_grid;

	Gtk::CheckButton *fixed_ch_height;
	Gtk::Widget *ch_height_text;
	Gtk::Entry *ch_height;
	
	Gtk::Entry *speed;

	Gtk::Button *btn_cancel;
	Gtk::Button *btn_ok;
}; /* class gen_opt_dlg */


#endif /* GEN_OPT_DLG_GTK_H */
