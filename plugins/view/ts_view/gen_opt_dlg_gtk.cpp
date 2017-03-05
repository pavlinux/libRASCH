#include <string.h>
#include "gen_opt_dlg_gtk.h"


gen_opt_dlg::gen_opt_dlg(BaseObjectType* cobject, const Glib::RefPtr<Gnome::Glade::Xml> ref_xml)
	: Gtk::Dialog(cobject)
{
	ref_xml->get_widget("general_options", dlg);
	
	ref_xml->get_widget("ch_height", ch_height);
	ch_height->set_alignment(Gtk::ALIGN_RIGHT);
	ch_height->set_text("");
	ref_xml->get_widget("ch_height_text", ch_height_text);
	
	ref_xml->get_widget("speed", speed);
	speed->set_alignment(Gtk::ALIGN_RIGHT);
	speed->set_text("");
		
	ref_xml->get_widget("show_status", show_status);
	ref_xml->get_widget("show_ch_info", show_ch_info);
	ref_xml->get_widget("show_grid", show_grid);

	ref_xml->get_widget("fixed_ch_height", fixed_ch_height);
	fixed_ch_height->signal_clicked().connect(sigc::mem_fun(*this, &gen_opt_dlg::fixed_ch_height_pressed));

	ref_xml->get_widget("btn_cancel", btn_cancel);
	btn_cancel->signal_clicked().connect(sigc::mem_fun(*this, &gen_opt_dlg::cancel_pressed));

	ref_xml->get_widget("btn_ok", btn_ok);
	btn_ok->signal_clicked().connect(sigc::mem_fun(*this, &gen_opt_dlg::ok_pressed));
} /* constructor */


gen_opt_dlg::~gen_opt_dlg()
{
	delete dlg;
} /* destructor */


void
gen_opt_dlg::init(struct ts_options *o)
{
	opt = o;
	
	char s[100];
	sprintf(s, "%.2f", opt->mm_per_sec);
	speed->set_text(s);

	show_status->set_active(opt->show_status);
	show_ch_info->set_active(opt->show_ch_info);
	show_grid->set_active(opt->plot_grid);

	fixed_ch_height->set_active(!opt->ch_height_dynamic);
	sprintf(s, "%d", opt->ch_height);
	ch_height->set_text(s);

	ch_height->set_sensitive(fixed_ch_height->get_active());
	ch_height_text->set_sensitive(fixed_ch_height->get_active());

} /* init() */


void
gen_opt_dlg::fixed_ch_height_pressed()
{
	ch_height->set_sensitive(fixed_ch_height->get_active());
	ch_height_text->set_sensitive(fixed_ch_height->get_active());
} /* fixed_ch_height_pressed() */


void
gen_opt_dlg::cancel_pressed()
{
	hide();
} /* cancel_pressed() */


void
gen_opt_dlg::ok_pressed()
{
	opt->mm_per_sec = atof(speed->get_text().c_str());

	opt->show_status = show_status->get_active();
	opt->show_ch_info = show_ch_info->get_active();
	opt->plot_grid = show_grid->get_active();

	opt->ch_height_dynamic = !fixed_ch_height->get_active();
	opt->ch_height = atoi(ch_height->get_text().c_str());

	hide();
} /* ok_pressed() */
