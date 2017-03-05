#include <string.h>
#include "ch_opt_dlg_gtk.h"


ch_opt_dlg::ch_opt_dlg(BaseObjectType* cobject, const Glib::RefPtr<Gnome::Glade::Xml> ref_xml)
	: Gtk::Dialog(cobject)
{
	ref_xml->get_widget("ch_draw_options", dlg);
	
	ref_xml->get_widget("ch_list", ch_list);
	ch_list->signal_changed().connect( sigc::mem_fun(*this, &ch_opt_dlg::on_combo_changed) );


	ref_xml->get_widget("show_channel", show_channel);
	ref_xml->get_widget("invert_channel", invert_channel);
	ref_xml->get_widget("resolution", resolution);
	ref_xml->get_widget("resolution_unit", res_unit);

	btn_centered = ref_xml->get_widget("btn_centered", btn_centered);
	btn_centered->signal_toggled().connect(sigc::mem_fun(*this, &ch_opt_dlg::centered_pressed));	
	ref_xml->get_widget("position_percent_text", position_percent_text);
	ref_xml->get_widget("position_percent", position_percent);

	ref_xml->get_widget("btn_minmax", btn_minmax);
	btn_minmax->signal_toggled().connect(sigc::mem_fun(*this, &ch_opt_dlg::minmax_pressed));
	ref_xml->get_widget("min_text", min_text);
	ref_xml->get_widget("min", min);
	ref_xml->get_widget("max_text", max_text);
	ref_xml->get_widget("max", max);
	ref_xml->get_widget("use_resolution", use_resolution);
	use_resolution->signal_clicked().connect(sigc::mem_fun(*this, &ch_opt_dlg::use_res_pressed));

	ref_xml->get_widget("btn_cancel_ch", btn_cancel);
	btn_cancel->signal_clicked().connect(sigc::mem_fun(*this, &ch_opt_dlg::cancel_pressed));

	ref_xml->get_widget("btn_ok_ch", btn_ok);
	btn_ok->signal_clicked().connect(sigc::mem_fun(*this, &ch_opt_dlg::ok_pressed));
} /* constructor */


ch_opt_dlg::~ch_opt_dlg()
{
	delete dlg;
} /* destructor */


void
ch_opt_dlg::init(struct ch_info *ch_infos, int num, int curr_ch)
{
	ch = ch_infos;
	num_ch = num;
	
	//Create the Tree model:
	ch_list->clear();
	tree_model = Gtk::ListStore::create(columns);
	ch_list->set_model(tree_model);

	//Fill the ComboBox's Tree Model:
	curr_ch_idx = -1;
	for (int i = 0; i < num_ch; i++)
	{
		if (curr_ch == ch[i].ch_number)
			curr_ch_idx = i;
		Gtk::TreeModel::Row row = *(tree_model->append());
		row[columns.col_idx] = i;
		row[columns.col_name] = ch[i].name;
	}
	
	//Add the model columns to the Combo (which is a kind of view),
	//rendering them in the default way:
	ch_list->pack_start(columns.col_name);

	if (curr_ch_idx >= 0)
	{
		ch_selected(curr_ch_idx);
		ch_list->set_active(curr_ch_idx);
	}
} /* init() */


void
ch_opt_dlg::on_combo_changed()
{
	Gtk::TreeModel::iterator iter = ch_list->get_active();
	if (!iter)
		return;

	Gtk::TreeModel::Row row = *iter;
	if (!row)
		return;

	if (curr_ch_idx >= 0)
		save_ch_data(curr_ch_idx);

	// Get the data for the selected row, using our knowledge of the tree model:
	curr_ch_idx = row[columns.col_idx];
	ch_selected(curr_ch_idx);
} /* on_combo_changed() */


void
ch_opt_dlg::save_ch_data(int idx)
{
	ch[idx].show_it = show_channel->get_active();
	ch[idx].inverse = invert_channel->get_active();
	ch[idx].mm_per_unit = atof(resolution->get_text().c_str());

	ch[idx].centered = btn_centered->get_active();
	ch[idx].center_pos = atoi(position_percent->get_text().c_str());
	ch[idx].min_value = atof(min->get_text().c_str());
	ch[idx].max_value = atof(max->get_text().c_str());
	ch[idx].use_mm_per_unit = use_resolution->get_active();
} /* save_ch_data() */


void
ch_opt_dlg::ch_selected(int idx)
{
	if ((idx < 0) || (idx >= num_ch))
		return;

	show_channel->set_active(ch[idx].show_it);
	invert_channel->set_active(ch[idx].inverse);

	char s[100];
	sprintf(s, "%f", ch[idx].mm_per_unit);
	resolution->set_text(s);
	snprintf(s, 99, "[mm/%s]", ch[idx].unit);
	res_unit->set_text(s);

	sprintf(s, "%d", ch[idx].center_pos);
	position_percent->set_text(s);

	sprintf(s, "%.2f", ch[idx].min_value);
	min->set_text(s);
	sprintf(s, "%.2f", ch[idx].max_value);
	max->set_text(s);
	use_resolution->set_active(ch[idx].use_mm_per_unit);

	
	if (ch[idx].centered)
	{
		centered_pressed();
		btn_centered->set_active(true);
	}
	else
	{
		minmax_pressed();
		btn_minmax->set_active(true);
	}
} /* ch_selected() */


void
ch_opt_dlg::centered_pressed()
{
	position_percent_text->set_sensitive(true);
	position_percent->set_sensitive(true);

	min_text->set_sensitive(false);
	min->set_sensitive(false);
	max_text->set_sensitive(false);
	max->set_sensitive(false);
	use_resolution->set_sensitive(false);
} /* centered_pressed() */


void
ch_opt_dlg::minmax_pressed()
{
	min_text->set_sensitive(true);
	min->set_sensitive(true);
	max_text->set_sensitive(true);
	max->set_sensitive(true);
	use_resolution->set_sensitive(true);

	position_percent_text->set_sensitive(false);
	position_percent->set_sensitive(false);

	use_res_pressed();
} /* minmax_pressed() */


void
ch_opt_dlg::use_res_pressed()
{
	max_text->set_sensitive(!use_resolution->get_active());
	max->set_sensitive(!use_resolution->get_active());
} /* use_res_pressed() */


void
ch_opt_dlg::cancel_pressed()
{
	hide();
} /* cancel_pressed() */


void
ch_opt_dlg::ok_pressed()
{
	if (curr_ch_idx >= 0)
		save_ch_data(curr_ch_idx);

	hide();
} /* ok_pressed() */
