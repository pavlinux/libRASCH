#include <QtGui/QComboBox>
#include <QtGui/QCheckBox>
#include <QtGui/QPushButton>
#include <QtGui/QLineEdit>
#include <QtGui/QColor>
#include <QtGui/QLabel>
#include <QtGui/QColorDialog>

#include "pair_opt_dlg_qt.h"

#include "plot_defines.h"


pair_opt_dlg_qt::pair_opt_dlg_qt(meas_handle meas, struct ra_plot_options *opt, int curr_pair/*=0*/)
	: QDialog(NULL)
{
	setupUi(this);

	_num_ch = 0;
	_ch = NULL;
	_curr_pair = NULL;
	_curr_axis = NULL;

	_meas = meas;

	_opt = (struct ra_plot_options *) ra_alloc_mem(sizeof(struct ra_plot_options));
	memset(_opt, 0, sizeof(struct ra_plot_options));
	_opt_orig = opt;
	_copy_opt_struct(_opt, opt);

	// number of different available plot options are only needed in this class
	_plot_num_symbol = sizeof(ra_plot_symbol) / sizeof(ra_plot_symbol[0]);
	_plot_num_line = sizeof(ra_plot_line) / sizeof(ra_plot_line[0]);
	_plot_num_time_format = sizeof(ra_plot_time_format) / sizeof(ra_plot_time_format[0]);
	_plot_num_log_type = sizeof(ra_plot_log_type) / sizeof(ra_plot_log_type[0]);

	_init();
	_do_connect();

	if (!opt || (curr_pair < 0) || (curr_pair >= opt->num_plot_pairs))
		curr_pair = 0;

	pairs->setCurrentIndex(curr_pair);
	_pairs_select(curr_pair);
} // constructor


pair_opt_dlg_qt::~pair_opt_dlg_qt()
{
} // destructor


void
pair_opt_dlg_qt::_init()
{
	// init pairs
	for (int i = 0; i < _opt->num_plot_pairs; i++)
	{
		QString s = _opt->axis[_opt->pair[i].x_idx].prop_name;
		s += "/";
		s += _opt->axis[_opt->pair[i].y_idx].prop_name;
		if (_opt->pair[i].use_z_axis)
		{
			s += "/";
			s += _opt->axis[_opt->pair[i].z_idx].prop_name;
		}

		pairs->addItem(s);
	}

	// init comboboxes (symbol, line, time, log)
	for (int i = 0; i < _plot_num_symbol; i++)
		symbol->addItem(ra_plot_symbol[i].name);
	for (int i = 0; i < _plot_num_line; i++)
		line->addItem(ra_plot_line[i].name);
	for (int i = 0; i < _plot_num_time_format; i++)
		time_format->addItem(ra_plot_time_format[i].name);
	for (int i = 0; i < _plot_num_log_type; i++)
		log_type->addItem(ra_plot_log_type[i].name);

	// init channels
	channel->addItem("Ch-independent values");

	rec_handle rh = ra_rec_get_first(_meas, 0);
	value_handle vh = ra_value_malloc();
	if (ra_info_get(rh, RA_INFO_REC_GEN_NUM_CHANNEL_L, vh) != 0)
	{
		ra_value_free(vh);
		return;
	}
	long n = ra_value_get_long(vh);
	_num_ch = n + 1;
	_ch = new long[_num_ch];
	_ch[0] = -1;
	for (long l = 0; l < n; l++)
	{
		_ch[l+1] = l;
		ra_value_set_number(vh, l);
		if (ra_info_get(rh, RA_INFO_REC_CH_NAME_C, vh) != 0)
			break;
		channel->addItem(ra_value_get_string(vh));
	}
	ra_value_free(vh);
} // _init()


void
pair_opt_dlg_qt::_do_connect()
{
	QObject::connect(pairs, SIGNAL(activated(int)), this, SLOT(_pairs_select(int)));

	QObject::connect(plot_symbol, SIGNAL(clicked()), this, SLOT(_plot_symbol_click()));
	QObject::connect(symbol, SIGNAL(activated(int)), this, SLOT(_symbol_select(int)));
	QObject::connect(symbol_color, SIGNAL(clicked()), this, SLOT(_sym_color_click()));

	QObject::connect(plot_line, SIGNAL(clicked()), this, SLOT(_plot_line_click()));
	QObject::connect(line, SIGNAL(activated(int)), this, SLOT(_line_select(int)));
	QObject::connect(line_color, SIGNAL(clicked()), this, SLOT(_line_color_click()));

	QObject::connect(channel, SIGNAL(activated(int)), this, SLOT(_channel_select(int)));

//    QObject::connect(btn_add_analyses, SIGNAL(clicked()), this, SLOT(_add_analyses_click()));

	QObject::connect(axis, SIGNAL(activated(int)), this, SLOT(_axis_select(int)));

	QObject::connect(use_min, SIGNAL(clicked()), this, SLOT(_use_min_click()));
	QObject::connect(use_max, SIGNAL(clicked()), this, SLOT(_use_max_click()));


	QObject::connect(is_time, SIGNAL(clicked()), this, SLOT(_is_time_click()));
	QObject::connect(time_format, SIGNAL(activated(int)), this, SLOT(_time_select(int)));

	QObject::connect(plot_log, SIGNAL(clicked()), this, SLOT(_plot_log_click()));
	QObject::connect(log_type, SIGNAL(activated(int)), this, SLOT(_log_select(int)));


	QObject::connect(btn_cancel, SIGNAL(clicked()), this, SLOT(_cancel_click()));
	QObject::connect(btn_ok, SIGNAL(clicked()), this, SLOT(_ok_click()));
} // _do_connect()



void
pair_opt_dlg_qt::_save_pair_data()
{
	if (!_curr_pair)
		return;

	_curr_pair->plot_symbol = plot_symbol->isChecked()? 1 : 0;
	_curr_pair->symbol_type = symbol->currentIndex();

	_curr_pair->plot_line = plot_line->isChecked()? 1 : 0;
	_curr_pair->line_type = line->currentIndex();

	strcpy(_curr_pair->name, legend_text->text().toAscii().constData());
} // _save_pair_data()


void
pair_opt_dlg_qt::_pairs_select(int index)
{
	_save_pair_data();

	_curr_pair = &(_opt->pair[index]);

	plot_symbol->setChecked(_curr_pair->plot_symbol);
	symbol->setCurrentIndex(_curr_pair->symbol_type);
	symbol_color->setAutoFillBackground(true);
	QPalette pal = symbol_color->palette();
	pal.setBrush(QPalette::Button, QColor(_curr_pair->sym_r, _curr_pair->sym_g, _curr_pair->sym_b));
	symbol_color->setPalette(pal);

	plot_line->setChecked(_curr_pair->plot_line);
	line->setCurrentIndex(_curr_pair->line_type);
	line_color->setAutoFillBackground(true);
	pal = line_color->palette();
	pal.setBrush(QPalette::Button, QColor(_curr_pair->line_r, _curr_pair->line_g, _curr_pair->line_b));
	line_color->setPalette(pal);

	legend_text->setText(_curr_pair->name);

	// make sure to enable/disable widgets
	_plot_symbol_click();
	_plot_line_click();

	_axis_init();
} // _pairs_select()


void
pair_opt_dlg_qt::_plot_symbol_click()
{
	if (plot_symbol->isChecked())
		_curr_pair->plot_symbol = 1;
	else
		_curr_pair->plot_symbol = 0;

	symbol->setEnabled(_curr_pair->plot_symbol);
	sym_text->setEnabled(_curr_pair->plot_symbol);
	symbol_color->setEnabled(_curr_pair->plot_symbol);
	sym_color_text->setEnabled(_curr_pair->plot_symbol);
} // _plot_symbol_click()


void
pair_opt_dlg_qt::_symbol_select(int index)
{
	_curr_pair->symbol_type = index;
} // _symbol_select()


void
pair_opt_dlg_qt::_sym_color_click()
{
	QColor c(_curr_pair->sym_r, _curr_pair->sym_g, _curr_pair->sym_b);
	c = QColorDialog::getColor(c, this);

	if (c.isValid())
	{		
		_curr_pair->sym_r = c.red();
		_curr_pair->sym_g = c.green();
		_curr_pair->sym_b = c.blue();

		QPalette pal = symbol_color->palette();
		pal.setBrush(QPalette::Button, QColor(_curr_pair->sym_r, _curr_pair->sym_g, _curr_pair->sym_b));
		symbol_color->setPalette(pal);
	}
} // _sym_color_click()


void
pair_opt_dlg_qt::_plot_line_click()
{
	if (plot_line->isChecked())
		_curr_pair->plot_line = 1;
	else
		_curr_pair->plot_line = 0;

	line->setEnabled(_curr_pair->plot_line);
	line_text->setEnabled(_curr_pair->plot_line);
	line_color->setEnabled(_curr_pair->plot_line);
	line_color_text->setEnabled(_curr_pair->plot_line);
} // _plot_line_click()


void
pair_opt_dlg_qt::_line_select(int index)
{
	_curr_pair->line_type = index;
} // _line_select()


void
pair_opt_dlg_qt::_line_color_click()
{
	QColor c(_curr_pair->line_r, _curr_pair->line_g, _curr_pair->line_b);
	c = QColorDialog::getColor(c, this);

	if (c.isValid())
	{
		_curr_pair->line_r = c.red();
		_curr_pair->line_g = c.green();
		_curr_pair->line_b = c.blue();

		QPalette pal = line_color->palette();
		pal.setBrush(QPalette::Button, QColor(_curr_pair->line_r, _curr_pair->line_g, _curr_pair->line_b));
		line_color->setPalette(pal);
	}
} // _line_color_click()


void
pair_opt_dlg_qt::_axis_init()
{
	axis->clear();

	QString s = "x: ";
	s += _opt->axis[_curr_pair->x_idx].prop_name;
	axis->addItem(s);

	s = "y: ";
	s += _opt->axis[_curr_pair->y_idx].prop_name;
	axis->addItem(s);

	if (_curr_pair->use_z_axis)
	{
		s = "z: ";
		s += _opt->axis[_curr_pair->z_idx].prop_name;
		axis->addItem(s);
	}

	_axis_select(0);
} // _axis_init()


void
pair_opt_dlg_qt::_save_axis_data()
{
	if (!_curr_axis)
		return;

	_curr_axis->use_min = use_min->isChecked()? 1 : 0;
	QString s = min_value->text();
	_curr_axis->min = s.toDouble();
	_curr_axis->use_max = use_max->isChecked()? 1 : 0;
	s = max_value->text();
	_curr_axis->max = s.toDouble();

	_curr_axis->is_time = is_time->isChecked()? 1 : 0;
	_curr_axis->time_format = time_format->currentIndex();
	_curr_axis->plot_log = plot_log->isChecked()? 1 : 0;
	_curr_axis->log_type = log_type->currentIndex();
} // _save_axis_data()


void
pair_opt_dlg_qt::_axis_select(int index)
{
	_save_axis_data();

	switch (index)
	{
	case 0:		// x
		_curr_axis = &(_opt->axis[_curr_pair->x_idx]);
		break;
	case 1:		// y
		_curr_axis = &(_opt->axis[_curr_pair->y_idx]);
		break;
	case 2:		// z
		_curr_axis = &(_opt->axis[_curr_pair->z_idx]);
		break;
	}

	use_min->setChecked(_curr_axis->use_min);
	QString s;
	s.setNum(_curr_axis->min);
	min_value->setText(s);
	use_max->setChecked(_curr_axis->use_max);
	s.setNum(_curr_axis->max);
	max_value->setText(s);

	is_time->setChecked(_curr_axis->is_time);
	time_format->setCurrentIndex(_curr_axis->time_format);
	plot_log->setChecked(_curr_axis->plot_log);
	log_type->setCurrentIndex(_curr_axis->log_type);

	// enable/diasble widgets
	_use_min_click();
	_use_max_click();
	_is_time_click();
	_plot_log_click();

	// set channel
	for (long l = 0; l < _num_ch; l++)
	{
		if (_ch[l] == _curr_axis->ch)
		{
			channel->setCurrentIndex(l);
			break;
		}
	}
} // _axis_select()


void
pair_opt_dlg_qt::_channel_select(int index)
{
	if ((index < 0) || (index >= _num_ch))
		return;
	_curr_axis->ch = _ch[index];
} // _channel_select()


void
pair_opt_dlg_qt::_use_min_click()
{
	if (use_min->isChecked())
		_curr_axis->use_min = 1;
	else
		_curr_axis->use_min = 0;

	min_value->setEnabled(_curr_axis->use_min);
	min_val_text->setEnabled(_curr_axis->use_min);
}				// _use_min_click()


void
pair_opt_dlg_qt::_use_max_click()
{
	if (use_max->isChecked())
		_curr_axis->use_max = 1;
	else
		_curr_axis->use_max = 0;

	max_value->setEnabled(_curr_axis->use_max);
	max_val_text->setEnabled(_curr_axis->use_max);
}				// _use_max_click()


void
pair_opt_dlg_qt::_is_time_click()
{
	if (is_time->isChecked())
		_curr_axis->is_time = 1;
	else
		_curr_axis->is_time = 0;

	time_format->setEnabled(_curr_axis->is_time);
	time_format_text->setEnabled(_curr_axis->is_time);
}				// _is_time_click()


void
pair_opt_dlg_qt::_time_select(int index)
{
	_curr_axis->time_format = index;
}				// _time_select()


void
pair_opt_dlg_qt::_plot_log_click()
{
	if (plot_log->isChecked())
		_curr_axis->plot_log = 1;
	else
		_curr_axis->plot_log = 0;

	log_type->setEnabled(_curr_axis->plot_log);
	log_type_text->setEnabled(_curr_axis->plot_log);
}				// _plot_log_click()


void
pair_opt_dlg_qt::_log_select(int index)
{
	_curr_axis->log_type = index;
}				// _log_select()


void
pair_opt_dlg_qt::_cancel_click()
{
	_free_opt_struct(_opt);
	_opt = NULL;

	reject();
}				// _cancel_click()


void
pair_opt_dlg_qt::_ok_click()
{
	_save_pair_data();
	_save_axis_data();

	_copy_opt_struct(_opt_orig, _opt);

	_free_opt_struct(_opt);
	_opt = NULL;

	if (_ch)
		delete[] _ch;

	accept();
}				// _ok_click()


void
pair_opt_dlg_qt::_copy_opt_struct(struct ra_plot_options *dest, struct ra_plot_options *src)
{
	dest->eval = src->eval;
	dest->clh = src->clh;
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
}				// _copy_opt_struct()


void
pair_opt_dlg_qt::_copy_pair_struct(struct ra_plot_pair **dest, struct ra_plot_pair *src, int n)
{
	if (*dest != NULL)
		ra_free_mem(*dest);
	*dest = NULL;

	*dest = (struct ra_plot_pair *) ra_alloc_mem(sizeof(struct ra_plot_pair) * n);
	memcpy(*dest, src, sizeof(struct ra_plot_pair) * n);
}				// _copy_pair_struct()


void
pair_opt_dlg_qt::_copy_axis_struct(struct ra_plot_axis **dest, struct ra_plot_axis *src, int n)
{
	if (*dest != NULL)
		ra_free_mem(*dest);
	*dest = NULL;

	*dest = (struct ra_plot_axis *) ra_alloc_mem(sizeof(struct ra_plot_axis) * n);
	memcpy(*dest, src, sizeof(struct ra_plot_axis) * n);
}				// _copy_axis_struct()


void
pair_opt_dlg_qt::_free_opt_struct(struct ra_plot_options *opt)
{
	if (opt->pair)
		ra_free_mem(opt->pair);
	if (opt->axis)
		ra_free_mem(opt->axis);
	ra_free_mem(opt);
}				// _free_opt_struct()
