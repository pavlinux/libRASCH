/*----------------------------------------------------------------------------
 * pair_opt_dlg_mfc.cpp
 *
 * 
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002-2006, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

// pair_opt_dlg_mfc.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "ra_plot.h"
#include "pair_opt_dlg_mfc.h"

#include "plot_defines.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld pair_opt_dlg_mfc 


void
pair_opt_dlg_mfc::init(meas_handle meas, plugin_handle pl,
		       struct ra_plot_options *opt, int curr_pair /*=0*/ )
{
	_meas = meas;
	_plugin = pl;

	_opt = (struct ra_plot_options *) ra_alloc_mem(sizeof(struct ra_plot_options));
	memset(_opt, 0, sizeof(struct ra_plot_options));
	_opt_orig = opt;
	_copy_opt_struct(_opt, opt);

	_curr_pair = NULL;
	_curr_axis = NULL;

	_num_ch = 0;
	_ch = NULL;

	_init_pair_num = curr_pair;	// initial selected pair

	//{{AFX_DATA_INIT(pair_opt_dlg_mfc)
	_is_time = FALSE;
	_max = 0.0;
	_min = 0.0;
	_name = _T("");
	_plot_line = FALSE;
	_plot_log = FALSE;
	_use_max = FALSE;
	_use_min = FALSE;
	_plot_symbol = FALSE;
	_txt_axis = _T("");
	//}}AFX_DATA_INIT
}				// init()


void
pair_opt_dlg_mfc::DoDataExchange(CDataExchange * pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(pair_opt_dlg_mfc)
	DDX_Control(pDX, IDC_CHANNEL, _channel);
	DDX_Control(pDX, IDC_SYM_COLOR, _symbol_color);
	DDX_Control(pDX, IDC_LOG, _log);
	DDX_Control(pDX, IDC_TIME, _time);
	DDX_Control(pDX, IDC_SYMBOL, _symbol);
	DDX_Control(pDX, IDC_PAIR, _pair);
	DDX_Control(pDX, IDC_LINE, _line);
	DDX_Control(pDX, IDC_LINE_COLOR, _line_color);
	DDX_Control(pDX, IDC_AXIS, _axis);
	DDX_Check(pDX, IDC_IS_TIME, _is_time);
	DDX_Text(pDX, IDC_MAX, _max);
	DDX_Text(pDX, IDC_MIN, _min);
	DDX_Text(pDX, IDC_NAME, _name);
	DDX_Check(pDX, IDC_PLOT_LINE, _plot_line);
	DDX_Check(pDX, IDC_PLOT_LOG, _plot_log);
	DDX_Check(pDX, IDC_USE_MAX, _use_max);
	DDX_Check(pDX, IDC_USE_MIN, _use_min);
	DDX_Check(pDX, IDC_PLOT_SYMBOLS, _plot_symbol);
	DDX_Text(pDX, IDC_TXT_AXIS, _txt_axis);
	//}}AFX_DATA_MAP
}				// DoDataExchange()


BEGIN_MESSAGE_MAP(pair_opt_dlg_mfc, CDialog)
	//{{AFX_MSG_MAP(pair_opt_dlg_mfc)
	ON_CBN_SELCHANGE(IDC_PAIR, OnSelchangePair)
ON_BN_CLICKED(IDC_PLOT_SYMBOLS, OnPlotSymbols)
ON_BN_CLICKED(IDC_SYM_COLOR, OnSymColor)
ON_BN_CLICKED(IDC_PLOT_LINE, OnPlotLine)
ON_BN_CLICKED(IDC_LINE_COLOR, OnLineColor)
ON_CBN_SELCHANGE(IDC_AXIS, OnSelchangeAxis)
ON_BN_CLICKED(IDC_USE_MIN, OnUseMin)
ON_BN_CLICKED(IDC_USE_MAX, OnUseMax)
ON_BN_CLICKED(IDC_IS_TIME, OnIsTime) ON_BN_CLICKED(IDC_PLOT_LOG, OnPlotLog)
	ON_CBN_SELCHANGE(IDC_CHANNEL, OnSelchangeChannel)
	//}}AFX_MSG_MAP
	END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten pair_opt_dlg_mfc 
     BOOL pair_opt_dlg_mfc::OnInitDialog()
{
	CDialog::OnInitDialog();

	// do l10n stuff
	this->SetWindowText(gettext("Options"));
	GetDlgItem(IDC_PLOT_SYMBOLS)->SetWindowText(gettext("plot symbols"));
	GetDlgItem(IDC_SYM_TEXT)->SetWindowText(gettext("symbol"));
	GetDlgItem(IDC_SYM_COLOR_TEXT)->SetWindowText(gettext("color"));
	GetDlgItem(IDC_PLOT_LINE)->SetWindowText(gettext("plot line"));
	GetDlgItem(IDC_LINE_TEXT)->SetWindowText(gettext("line"));
	GetDlgItem(IDC_LINE_COLOR_TEXT)->SetWindowText(gettext("color"));
	GetDlgItem(IDC_TXT_NAME)->SetWindowText(gettext("name"));
	GetDlgItem(IDC_ADD_ANALYSES)->SetWindowText(gettext("add. Analyses"));
	GetDlgItem(IDC_TXT_AXIS_OPTIONS)->SetWindowText(gettext("Axis Options"));
	GetDlgItem(IDC_TXT_AXIS)->SetWindowText(gettext("Axis"));
	GetDlgItem(IDC_USE_MIN)->SetWindowText(gettext("use min"));
	GetDlgItem(IDC_MIN_TEXT)->SetWindowText(gettext("value"));
	GetDlgItem(IDC_USE_MAX)->SetWindowText(gettext("use max"));
	GetDlgItem(IDC_MAX_TEXT)->SetWindowText(gettext("value"));
	GetDlgItem(IDC_IS_TIME)->SetWindowText(gettext("is time"));
	GetDlgItem(IDC_TIME_TEXT)->SetWindowText(gettext("format"));
	GetDlgItem(IDC_PLOT_LOG)->SetWindowText(gettext("log"));
	GetDlgItem(IDC_LOG_TEXT)->SetWindowText(gettext("type"));
	UpdateData(FALSE);

	// init pairs
	for (int i = 0; i < _opt->num_plot_pairs; i++)
	{
		CString s = _opt->axis[_opt->pair[i].x_idx].prop_name;
		s += "/";
		s += _opt->axis[_opt->pair[i].y_idx].prop_name;
		if (_opt->pair[i].use_z_axis)
		{
			s += "/";
			s += _opt->axis[_opt->pair[i].z_idx].prop_name;
		}

		_pair.AddString(s);
	}

	// init channels
	_channel.AddString("Ch-independent values");

	rec_handle rh = ra_rec_get_first(_meas, 0);
	value_handle vh = ra_value_malloc();
	_num_ch = 0;
	long n = 0;
	if (ra_info_get(rh, RA_INFO_REC_GEN_NUM_CHANNEL_L, vh) != 0)
		ra_value_free(vh);
	else
	{
		n = ra_value_get_long(vh);
		_num_ch = n + 1;
	}
	_ch = new long[_num_ch];
	_ch[0] = -1;
	for (long l = 0; l < n; l++)
	{
		_ch[l+1] = l;
		ra_value_set_number(vh, l);
		if (ra_info_get(rh, RA_INFO_REC_CH_NAME_C, vh) != 0)
			break;
		int idx = _channel.AddString(ra_value_get_string(vh));
		_channel.SetItemData(idx, l+1);
	}
	ra_value_free(vh);

	// init comboboxes (symbol, line, time, log)
	n = sizeof(ra_plot_symbol) / sizeof(ra_plot_symbol[0]);
	for (i = 0; i < n; i++)
		_symbol.AddString(ra_plot_symbol[i].name);

	n = sizeof(ra_plot_line) / sizeof(ra_plot_line[0]);
	for (i = 0; i < n; i++)
		_line.AddString(ra_plot_line[i].name);

	n = sizeof(ra_plot_time_format) / sizeof(ra_plot_time_format[0]);
	for (i = 0; i < n; i++)
		_time.AddString(ra_plot_time_format[i].name);

	n = sizeof(ra_plot_log_type) / sizeof(ra_plot_log_type[0]);
	for (i = 0; i < n; i++)
		_log.AddString(ra_plot_log_type[i].name);

	if (!_opt || (_init_pair_num < 0) || (_init_pair_num >= _opt->num_plot_pairs))
		_init_pair_num = 0;

	_pair.SetCurSel(_init_pair_num);
	OnSelchangePair();

	UpdateData(FALSE);

	return TRUE;		// return TRUE unless you set the focus to a control
	// EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
	} // OnInitDialog()


void
pair_opt_dlg_mfc::_save_pair_data()
{
	if (!_curr_pair)
		return;

	_curr_pair->plot_symbol = _plot_symbol;
	_curr_pair->symbol_type = _symbol.GetCurSel();

	_curr_pair->plot_line = _plot_line;
	_curr_pair->line_type = _line.GetCurSel();

	strcpy(_curr_pair->name, _name);
} // _save_pair_data()


void
pair_opt_dlg_mfc::OnSelchangePair()
{
	int index = _pair.GetCurSel();
	if (index == LB_ERR)
		return;

	_save_pair_data();

	_curr_pair = &(_opt->pair[index]);

	_plot_symbol = _curr_pair->plot_symbol;
	_symbol.SetCurSel(_curr_pair->symbol_type);
	_symbol_color.SetColor(RGB(_curr_pair->sym_r, _curr_pair->sym_g, _curr_pair->sym_b));

	_plot_line = _curr_pair->plot_line;
	_line.SetCurSel(_curr_pair->line_type);
	_line_color.SetColor(RGB(_curr_pair->line_r, _curr_pair->line_g, _curr_pair->line_b));

	_name = _curr_pair->name;

	UpdateData(FALSE);

	// make sure to enable/disable widgets
	OnPlotSymbols();
	OnPlotLine();

	_axis_init();
} // OnSelchangePair()


void
pair_opt_dlg_mfc::OnPlotSymbols()
{
	UpdateData();

	if (_plot_symbol)
		_curr_pair->plot_symbol = 1;
	else
		_curr_pair->plot_symbol = 0;

	GetDlgItem(IDC_SYM_TEXT)->EnableWindow(_curr_pair->plot_symbol);
	GetDlgItem(IDC_SYMBOL)->EnableWindow(_curr_pair->plot_symbol);
	GetDlgItem(IDC_SYM_COLOR_TEXT)->EnableWindow(_curr_pair->plot_symbol);
	GetDlgItem(IDC_SYM_COLOR)->EnableWindow(_curr_pair->plot_symbol);
}				// OnPlotSymbols()


void
pair_opt_dlg_mfc::OnSymColor()
{
	CColorDialog dlg(RGB(_curr_pair->sym_r, _curr_pair->sym_g, _curr_pair->sym_b));
	if (dlg.DoModal() == IDOK)
	{
		COLORREF c = dlg.GetColor();
		_curr_pair->sym_r = GetRValue(c);
		_curr_pair->sym_g = GetGValue(c);
		_curr_pair->sym_b = GetBValue(c);
		_symbol_color.SetColor(c);
	}
}				// OnSymColor()


void
pair_opt_dlg_mfc::OnPlotLine()
{
	UpdateData();

	if (_plot_line)
		_curr_pair->plot_line = 1;
	else
		_curr_pair->plot_line = 0;

	GetDlgItem(IDC_LINE_TEXT)->EnableWindow(_curr_pair->plot_line);
	GetDlgItem(IDC_LINE)->EnableWindow(_curr_pair->plot_line);
	GetDlgItem(IDC_LINE_COLOR_TEXT)->EnableWindow(_curr_pair->plot_line);
	GetDlgItem(IDC_LINE_COLOR)->EnableWindow(_curr_pair->plot_line);
}				// OnPlotLine()


void
pair_opt_dlg_mfc::OnLineColor()
{
	CColorDialog dlg(RGB(_curr_pair->line_r, _curr_pair->line_g, _curr_pair->line_b));
	if (dlg.DoModal() == IDOK)
	{
		COLORREF c = dlg.GetColor();
		_curr_pair->line_r = GetRValue(c);
		_curr_pair->line_g = GetGValue(c);
		_curr_pair->line_b = GetBValue(c);
		_line_color.SetColor(c);
	}
}				// OnLineColor()


void
pair_opt_dlg_mfc::OnSelchangeAxis()
{
	int index = _axis.GetCurSel();
	if (index == LB_ERR)
		return;

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

	_use_min = _curr_axis->use_min;
	_min = _curr_axis->min;
	_use_max = _curr_axis->use_max;
	_max = _curr_axis->max;

	_is_time = _curr_axis->is_time;
	_time.SetCurSel(_curr_axis->time_format);
	_plot_log = _curr_axis->plot_log;
	_log.SetCurSel(_curr_axis->log_type);

	UpdateData(FALSE);

	// set channel
	for (long l = 0; l < _num_ch; l++)
	{
		if (_ch[l] == _curr_axis->ch)
		{
			_channel.SetCurSel(l);
			break;
		}
	}

	// enable/diasble widgets
	OnUseMin();
	OnUseMax();
	OnIsTime();
	OnPlotLog();
} // OnSelchangeAxis()


void
pair_opt_dlg_mfc::_axis_init()
{
	_axis.ResetContent();

	CString s = "x: ";
	s += _opt->axis[_curr_pair->x_idx].prop_name;
	_axis.AddString(s);

	s = "y: ";
	s += _opt->axis[_curr_pair->y_idx].prop_name;
	_axis.AddString(s);

	if (_curr_pair->use_z_axis)
	{
		s = "z: ";
		s += _opt->axis[_curr_pair->z_idx].prop_name;
		_axis.AddString(s);
	}

	_axis.SetCurSel(0);
	OnSelchangeAxis();
}				// _axis_init()


void
pair_opt_dlg_mfc::_save_axis_data()
{
	UpdateData();

	if (!_curr_axis)
		return;

	_curr_axis->use_min = _use_min;
	_curr_axis->min = _min;
	_curr_axis->use_max = _use_max;
	_curr_axis->max = _max;

	_curr_axis->is_time = _is_time;
	_curr_axis->time_format = _time.GetCurSel();
	_curr_axis->plot_log = _plot_log;
	_curr_axis->log_type = _log.GetCurSel();
}				// _save_axis_data()


void
pair_opt_dlg_mfc::OnUseMin()
{
	UpdateData();

	if (_use_min)
		_curr_axis->use_min = 1;
	else
		_curr_axis->use_min = 0;

	GetDlgItem(IDC_MIN_TEXT)->EnableWindow(_curr_axis->use_min);
	GetDlgItem(IDC_MIN)->EnableWindow(_curr_axis->use_min);
}				// OnUseMin()


void
pair_opt_dlg_mfc::OnUseMax()
{
	UpdateData();

	if (_use_max)
		_curr_axis->use_max = 1;
	else
		_curr_axis->use_max = 0;

	GetDlgItem(IDC_MAX_TEXT)->EnableWindow(_curr_axis->use_max);
	GetDlgItem(IDC_MAX)->EnableWindow(_curr_axis->use_max);
}				// OnUseMax()


void
pair_opt_dlg_mfc::OnIsTime()
{
	UpdateData();

	if (_is_time)
		_curr_axis->is_time = 1;
	else
		_curr_axis->is_time = 0;

	GetDlgItem(IDC_TIME_TEXT)->EnableWindow(_curr_axis->is_time);
	GetDlgItem(IDC_TIME)->EnableWindow(_curr_axis->is_time);
}				// OnIsTime()


void
pair_opt_dlg_mfc::OnPlotLog()
{
	UpdateData();

	if (_plot_log)
		_curr_axis->plot_log = 1;
	else
		_curr_axis->plot_log = 0;

	GetDlgItem(IDC_LOG_TEXT)->EnableWindow(_curr_axis->plot_log);
	GetDlgItem(IDC_LOG)->EnableWindow(_curr_axis->plot_log);
} // OnPlotLog()


void
pair_opt_dlg_mfc::OnCancel()
{
	_free_opt_struct(_opt);
	_opt = NULL;

	if (_ch)
		delete[] _ch;
	_ch = NULL;

	CDialog::OnCancel();
}  // OnCancel()


void
pair_opt_dlg_mfc::OnOK()
{
	UpdateData();

	_save_pair_data();
	_save_axis_data();

	_copy_opt_struct(_opt_orig, _opt);

	_free_opt_struct(_opt);
	_opt = NULL;

	if (_ch)
		delete[] _ch;
	_ch = NULL;

	CDialog::OnOK();
}  // OnOK()


void
pair_opt_dlg_mfc::_copy_opt_struct(struct ra_plot_options *dest, struct ra_plot_options *src)
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
} // _copy_opt_struct()


void
pair_opt_dlg_mfc::_copy_pair_struct(struct ra_plot_pair **dest, struct ra_plot_pair *src, int n)
{
	if (*dest != NULL)
		ra_free_mem(*dest);
	*dest = NULL;

	*dest = (struct ra_plot_pair *) ra_alloc_mem(sizeof(struct ra_plot_pair) * n);
	memcpy(*dest, src, sizeof(struct ra_plot_pair) * n);
} // _copy_pair_struct()


void
pair_opt_dlg_mfc::_copy_axis_struct(struct ra_plot_axis **dest, struct ra_plot_axis *src, int n)
{
	if (*dest != NULL)
		ra_free_mem(*dest);
	*dest = NULL;

	*dest = (struct ra_plot_axis *) ra_alloc_mem(sizeof(struct ra_plot_axis) * n);
	memcpy(*dest, src, sizeof(struct ra_plot_axis) * n);
} // _copy_axis_struct()


void
pair_opt_dlg_mfc::_free_opt_struct(struct ra_plot_options *opt)
{
	if (opt->pair)
		ra_free_mem(opt->pair);
	if (opt->axis)
		ra_free_mem(opt->axis);
	ra_free_mem(opt);
} // _free_opt_struct()


void
pair_opt_dlg_mfc::OnSelchangeChannel() 
{
	int index = _channel.GetCurSel();
	if (index == LB_ERR)
		return;

	if ((index < 0) || (index >= _num_ch))
		return;
	_curr_axis->ch = _ch[index];
}  // OnSelchangeChannel()

