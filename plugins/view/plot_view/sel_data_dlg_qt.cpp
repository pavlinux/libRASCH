#include <QtGui/QComboBox>
#include <QtGui/QPushButton>
#include <QtGui/QListWidget>
#include <QtGui/QCheckBox>

#include "sel_data_dlg_qt.h"
#include "pair_opt_dlg_qt.h"

#include <stdlib.h>
#include <pl_general.h>

sel_data_dlg_qt::sel_data_dlg_qt(meas_handle meas, struct ra_plot_options *opt)
	: QDialog(NULL)
{
	setupUi(this);

	_meas = meas;
	_eval = opt->eval;

	_opt = (struct ra_plot_options *) ra_alloc_mem(sizeof(struct ra_plot_options));
	memset(_opt, 0, sizeof(struct ra_plot_options));
	_opt_orig = opt;
	_copy_opt_struct(_opt, opt);

	_init();
	_do_connect();
} // constructor


void
sel_data_dlg_qt::_init()
{
	show_grid->setChecked(_opt->show_grid);
	show_legend->setChecked(_opt->show_legend);

	// init data-set combos
	value_handle vh = ra_value_malloc();
	ra_class_get(_eval, NULL, vh);
	long n = ra_value_get_num_elem(vh);
	const void **clh_a = ra_value_get_voidp_array(vh);
	value_handle vh_n = ra_value_malloc();
	for (long l = 0; l < n; l++)
	{		
		ra_info_get((class_handle)(clh_a[l]), RA_INFO_CLASS_ASCII_ID_C, vh_n);
		eval_class->addItem(ra_value_get_string(vh_n));
	}
	ra_value_free(vh_n);
	if (n > _opt->num_event_class)
	{
		class_handle *clh_new = (class_handle *)ra_alloc_mem(sizeof(class_handle *) * n);
		memset(clh_new, 0, sizeof(class_handle *) * n);

		char **name_new = (char **)ra_alloc_mem(sizeof(char *) * n);
		memset(name_new, 0, sizeof(char *) * n);

		for (long l = 0; l < _opt->num_event_class; l++)
		{
			clh_new[l] = _opt->clh[l];
			name_new[l] = _opt->class_name[l];
		}

		for (long l = 0; l < n; l++)
		{
			bool already_avail = false;
			for (int i = 0; i < _opt->num_event_class; i++)
			{
				if (clh_a[l] == clh_new[i])
				{
					already_avail = true;
					break;
				}
			}
			if (already_avail)
				continue;

			clh_new[_opt->num_event_class] = (class_handle)clh_a[l];
			name_new[_opt->num_event_class] = (char *)ra_alloc_mem(sizeof(char) * EVAL_MAX_NAME);
			strcpy(name_new[_opt->num_event_class], eval_class->itemText(0).toAscii().constData());

			_opt->num_event_class++;
		}

		ra_free_mem(_opt->clh);
		_opt->clh = clh_new;

		ra_free_mem(_opt->class_name);
		_opt->class_name = name_new;
	}

	ra_value_free(vh);

	_init_axis();

	_init_pair_list();
} // _init()


void
sel_data_dlg_qt::_init_axis()
{
	x_axis->clear();
	y_axis->clear();
	z_axis->clear();

	class_handle curr_clh = NULL;
	if (_opt->clh)
		curr_clh = _opt->clh[eval_class->currentIndex()];
	else
		return;

	value_handle vh = ra_value_malloc();
	if (curr_clh)
	{
		ra_prop_get_all(curr_clh, vh);
		long n = ra_value_get_num_elem(vh);
		const void **ph_a = ra_value_get_voidp_array(vh);

		value_handle vh_n = ra_value_malloc();
		for (long l = 0; l < n; l++)
		{
			ra_info_get((prop_handle)(ph_a[l]), RA_INFO_PROP_NAME_C, vh_n);
			x_axis->addItem(ra_value_get_string(vh_n));
			y_axis->addItem(ra_value_get_string(vh_n));
			z_axis->addItem(ra_value_get_string(vh_n));
		}
		ra_value_free(vh_n);
	}

	ra_value_free(vh);
} // _init_axis()


void
sel_data_dlg_qt::_init_pair_list()
{
	pair_list->clear();

	for (int i = 0; i < _opt->num_plot_pairs; i++)
	{
		QString s;
		format_pair(i, s);
		pair_list->addItem(s);
	}
} // _init_pair_list()


void
sel_data_dlg_qt::format_pair(int idx, QString &entry)
{
	entry = _opt->axis[_opt->pair[idx].x_idx].prop_name;
	if (_opt->axis[_opt->pair[idx].x_idx].ch != -1)
	{
		QString ch_name;
		get_ch_name(_opt->axis[_opt->pair[idx].x_idx].ch, ch_name);
		entry += "(ch-";
		entry += ch_name;
		entry += ")";
	}
	
	entry += " / ";
	entry += _opt->axis[_opt->pair[idx].y_idx].prop_name;
	if (_opt->axis[_opt->pair[idx].y_idx].ch != -1)
	{
		QString ch_name;
		get_ch_name(_opt->axis[_opt->pair[idx].y_idx].ch, ch_name);
		entry += "(ch-";
		entry += ch_name;
		entry += ")";
	}
	
	if (_opt->pair[idx].use_z_axis)
	{
		entry += " / ";
		entry += _opt->axis[_opt->pair[idx].z_idx].prop_name;
		if (_opt->axis[_opt->pair[idx].z_idx].ch != -1)
		{
			QString ch_name;
			get_ch_name(_opt->axis[_opt->pair[idx].z_idx].ch, ch_name);
			entry += "(ch-";
			entry += ch_name;
			entry += ")";
		}
	}
} // format_pair()


void
sel_data_dlg_qt::get_ch_name(long ch, QString &name)
{
	name = "";

	value_handle vh = ra_value_malloc();
	rec_handle rh = ra_rec_get_first(_meas, 0);

	ra_value_set_number(vh, ch);
	if (ra_info_get(rh, RA_INFO_REC_CH_NAME_C, vh) == 0)
		name = ra_value_get_string(vh);

	ra_value_free(vh);
} // get_ch_name()

void
sel_data_dlg_qt::_do_connect()
{
	QObject::connect(eval_class, SIGNAL(activated(int)), this, SLOT(_eval_class_select(int)));

	QObject::connect(use_z_axis, SIGNAL(clicked()), this, SLOT(_use_z_axis_click()));

	QObject::connect(x_axis, SIGNAL(activated(int)), this, SLOT(_x_axis_select(int)));
	QObject::connect(y_axis, SIGNAL(activated(int)), this, SLOT(_y_axis_select(int)));
	QObject::connect(z_axis, SIGNAL(activated(int)), this, SLOT(_z_axis_select(int)));

	QObject::connect(pair_list, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this,
			 SLOT(_opt_click()));

	QObject::connect(btn_add, SIGNAL(clicked()), this, SLOT(_add_click()));
	QObject::connect(btn_del, SIGNAL(clicked()), this, SLOT(_del_click()));
	QObject::connect(btn_opt, SIGNAL(clicked()), this, SLOT(_opt_click()));

	QObject::connect(btn_cancel, SIGNAL(clicked()), this, SLOT(_cancel_click()));
	QObject::connect(btn_ok, SIGNAL(clicked()), this, SLOT(_ok_click()));
} // _do_connect()


void
sel_data_dlg_qt::_eval_class_select(int index)
{
	strcpy(_opt->class_name[index], eval_class->itemText(index).toAscii().constData());
	value_handle vh = ra_value_malloc();
	ra_class_get(_eval, _opt->class_name[index], vh);
	if (ra_value_get_num_elem(vh) <= 0)
	{
		ra_value_free(vh);
		return;
	}
	_opt->clh[index] = (class_handle)(ra_value_get_voidp_array(vh)[0]);

	ra_value_free(vh);

	_init_axis();
} // _eval_set_select()


void
sel_data_dlg_qt::_use_z_axis_click()
{
} // _use_z_axis_click()


void
sel_data_dlg_qt::_x_axis_select(int /*index */ )
{
} // _x_axis_select()


void
sel_data_dlg_qt::_y_axis_select(int /*index */ )
{
} // _y_axis_select()


void
sel_data_dlg_qt::_z_axis_select(int /*index */ )
{
} // _z_axis_select()


void
sel_data_dlg_qt::_pair_select(int /*index */ )
{

} // _pair_select()


void
sel_data_dlg_qt::_add_click()
{
	_opt->num_plot_pairs++;
	struct ra_plot_pair *t = (struct ra_plot_pair *)ra_alloc_mem(sizeof(struct ra_plot_pair) * _opt->num_plot_pairs);
	memset(t, 0, sizeof(struct ra_plot_pair) * _opt->num_plot_pairs);

	if (_opt->num_plot_pairs > 1)
		memcpy(t, _opt->pair, sizeof(struct ra_plot_pair) * (_opt->num_plot_pairs - 1));

	ra_free_mem(_opt->pair);
	_opt->pair = t;

	t = &(t[_opt->num_plot_pairs - 1]);	// less writing needed

	t->plot_symbol = 0;
	t->symbol_type = RA_PLOT_SYM_CIRCLE;
	t->sym_r = t->sym_g = t->sym_b = 0;	// black

	t->plot_line = 1;
	t->line_type = RA_PLOT_LINE_SOLID;
	t->line_r = t->line_g = t->line_b = 0;	// black

	t->use_z_axis = use_z_axis->isChecked()? 1 : 0;

	_set_axis(&(t->x_idx), x_axis->itemText(x_axis->currentIndex()), false);
	_set_axis(&(t->y_idx), y_axis->itemText(y_axis->currentIndex()));
	if (use_z_axis->isChecked())
		_set_axis(&(t->z_idx), z_axis->itemText(z_axis->currentIndex()));

	QString entry = _opt->axis[t->y_idx].prop_name;
	if (_opt->axis[t->y_idx].ch != -1)
	{
		QString ch_name;
		get_ch_name(_opt->axis[t->y_idx].ch, ch_name);
		entry += "(ch-";
		entry += ch_name;
		entry += ")";
	}
	strcpy(t->name, entry.toAscii().constData());

	QString s;
	format_pair(_opt->num_plot_pairs-1, s);
	pair_list->addItem(s);
} // _add_click()


void
sel_data_dlg_qt::_set_axis(int *idx, QString s, bool allow_same_prop_axis /*=true*/)
{
	if (_opt->clh == NULL)
		return;

	class_handle curr_clh = _opt->clh[eval_class->currentIndex()];

	char temp[EVAL_MAX_NAME];
	strcpy(temp, s.toAscii().constData());
	prop_handle p = ra_prop_get(curr_clh, temp);
	if (!allow_same_prop_axis)
	{
		for (int i = 0; i < _opt->num_axis; i++)
		{
			if (_opt->axis[i].event_property == p)
			{
				*idx = i;
				return;
			}
		}
	}

	*idx = _opt->num_axis;

	_opt->num_axis++;
	struct ra_plot_axis *t = (struct ra_plot_axis *)ra_alloc_mem(sizeof(struct ra_plot_axis) * _opt->num_axis);
	memset(t, 0, sizeof(struct ra_plot_axis) * _opt->num_axis);

	if (_opt->axis)
	{
		memcpy(t, _opt->axis, sizeof(struct ra_plot_axis) * (_opt->num_axis - 1));
		ra_free_mem(_opt->axis);
	}
	_opt->axis = t;

	struct ra_plot_axis *a = &(t[_opt->num_axis - 1]);	// less writing needed

	a->event_property = p;
	strcpy(a->prop_name, s.toAscii().constData());
	a->ch = -1;		// set ch to '-1' -> use ch-independet value as default

	a->event_class_idx = eval_class->currentIndex();

	// check if event-property has a min-/max-value
	value_handle vh = ra_value_malloc();
	if ((ra_info_get(a->event_property, RA_INFO_PROP_HAS_MINMAX_L, vh) == 0) && ra_value_get_long(vh))
	{
		a->use_min = a->use_max = 1;
		ra_info_get(a->event_property, RA_INFO_PROP_MIN_D, vh);
		a->min = ra_value_get_double(vh);
		ra_info_get(a->event_property, RA_INFO_PROP_MAX_D, vh);
		a->max = ra_value_get_double(vh);
	}
	else
	{
		a->use_min = 0;
		a->min = 0.0;
		a->use_max = 0;
		a->max = 0.0;
	}

	if (ra_info_get(a->event_property, RA_INFO_PROP_UNIT_C, vh) == 0)
		strncpy(a->unit, ra_value_get_string(vh), EVAL_MAX_UNIT);
	else
		a->unit[0] = '\0';

	a->is_time = 0;
	a->time_format = RA_PLOT_TIME_HOUR;
	a->plot_log = 0;
	a->log_type = RA_PLOT_LOG_10;

	ra_value_free(vh);
} // _set_axis()


void
sel_data_dlg_qt::_del_click()
{
	int curr = pair_list->currentRow();

	if (curr == -1)
		return;

	_opt->num_plot_pairs--;
	struct ra_plot_pair *t = (struct ra_plot_pair *)ra_alloc_mem(sizeof(struct ra_plot_pair) * _opt->num_plot_pairs);
	if (curr != 0)
		memcpy(t, _opt->pair, sizeof(struct ra_plot_pair) * curr);
	if (curr < _opt->num_plot_pairs)
		memcpy(&(t[curr]), &(_opt->pair[curr + 1]),
		       sizeof(struct ra_plot_pair) * (_opt->num_plot_pairs - curr));

	ra_free_mem(_opt->pair);
	_opt->pair = t;

	_init_pair_list();
} // _del_click()


void
sel_data_dlg_qt::_opt_click()
{
	int idx = pair_list->currentRow();
	if (idx < 0)
		return;

	pair_opt_dlg_qt dlg(_meas, _opt, idx);
	dlg.exec();
	_init_pair_list();

	// maybe channel was changed therefore build legend-name
	QString entry = _opt->axis[_opt->pair[idx].y_idx].prop_name;
	if (_opt->axis[_opt->pair[idx].y_idx].ch != -1)
	{
		QString ch_name;
		get_ch_name(_opt->axis[_opt->pair[idx].y_idx].ch, ch_name);
		entry += "(ch-";
		entry += ch_name;
		entry += ")";
	}
	strcpy(_opt->pair[idx].name, entry.toAscii().constData());
} // _opt_click()


void
sel_data_dlg_qt::_cancel_click()
{
	_free_opt_struct(_opt);
	_opt = NULL;

	reject();
} // _cancel_click()


void
sel_data_dlg_qt::_ok_click()
{
	_opt->show_grid = show_grid->isChecked();
	_opt->show_legend = show_legend->isChecked();

	_copy_opt_struct(_opt_orig, _opt);

	_free_opt_struct(_opt);
	_opt = NULL;

	accept();
} // _ok_click()


void
sel_data_dlg_qt::_copy_opt_struct(struct ra_plot_options *dest, struct ra_plot_options *src)
{
	if ((src == NULL) || (dest == NULL))
		return;

	dest->eval = src->eval;
	dest->num_event_class = src->num_event_class;
	dest->clh = (class_handle *)ra_alloc_mem(sizeof(class_handle) * dest->num_event_class);
	dest->class_name = (char **)ra_alloc_mem(sizeof(char *) * dest->num_event_class);
	for (int i = 0; i < dest->num_event_class; i++)
	{
		dest->clh[i] = src->clh[i];
		dest->class_name[i] = (char *)ra_alloc_mem(sizeof(char) * EVAL_MAX_NAME);
		strcpy(dest->class_name[i], src->class_name[i]);
	}

	dest->num_plot_pairs = src->num_plot_pairs;
	dest->num_axis = src->num_axis;
	dest->show_grid = src->show_grid;
	dest->frame_type = src->frame_type;
	dest->show_legend = src->show_legend;

	if (src->pair)
		_copy_pair_struct(&(dest->pair), src->pair, src->num_plot_pairs);
	else
		dest->pair = NULL;
	if (src->axis)
		_copy_axis_struct(&(dest->axis), src->axis, src->num_axis);
	else
		dest->axis = NULL;
} // _copy_opt_struct()


void
sel_data_dlg_qt::_copy_pair_struct(struct ra_plot_pair **dest, struct ra_plot_pair *src, int n)
{
	if (*dest != NULL)
		ra_free_mem(*dest);
	*dest = NULL;

	if (n <= 0)
		return;

	*dest = (struct ra_plot_pair *) ra_alloc_mem(sizeof(struct ra_plot_pair) * n);
	memcpy(*dest, src, sizeof(struct ra_plot_pair) * n);
} // _copy_pair_struct()


void
sel_data_dlg_qt::_copy_axis_struct(struct ra_plot_axis **dest, struct ra_plot_axis *src, int n)
{
	if (*dest != NULL)
		ra_free_mem(*dest);
	*dest = NULL;

	if (n <= 0)
		return;

	*dest = (struct ra_plot_axis *) ra_alloc_mem(sizeof(struct ra_plot_axis) * n);
	memcpy(*dest, src, sizeof(struct ra_plot_axis) * n);
} // _copy_axis_struct()


void
sel_data_dlg_qt::_free_opt_struct(struct ra_plot_options *opt)
{
	if (opt->pair)
	{
		ra_free_mem(opt->pair);
		opt->pair = NULL;
	}
	if (opt->axis)
	{
		ra_free_mem(opt->axis);
		opt->axis = NULL;
	}

	if (opt->clh)
		ra_free_mem(opt->clh);
	if (opt->class_name)
	{
		for (int i = 0; i < opt->num_event_class; i++)
			ra_free_mem(opt->class_name[i]);
		ra_free_mem(opt->class_name);
	}
	ra_free_mem(opt);
} // _free_opt_struct()
