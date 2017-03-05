/*----------------------------------------------------------------------------
 * sel_data_dlg_mfc.cpp
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

// sel_data_dlg_mfc.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "ra_plot.h"
#include "sel_data_dlg_mfc.h"
#include "pair_opt_dlg_mfc.h"

#include <pl_general.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld sel_data_dlg_mfc 



void
sel_data_dlg_mfc::init(meas_handle meas, plugin_handle pl, struct ra_plot_options *opt)
{
	_meas = meas;
	_plugin = pl;
	_eval = opt->eval;

	_opt = (struct ra_plot_options *) ra_alloc_mem(sizeof(struct ra_plot_options));
	memset(_opt, 0, sizeof(struct ra_plot_options));
	_opt_orig = opt;
	_copy_opt_struct(_opt, opt);

	//{{AFX_DATA_INIT(sel_data_dlg_mfc)
	_show_grid = FALSE;
	_show_legend = FALSE;
	_use_z_axis = FALSE;
	_txt_data_set = _T("");
	_txt_x_axis = _T("");
	_txt_y_axis = _T("");
	_txt_z_axis = _T("");
	//}}AFX_DATA_INIT
} // init()


void
sel_data_dlg_mfc::DoDataExchange(CDataExchange * pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(sel_data_dlg_mfc)
	DDX_Control(pDX, IDC_Z_AXIS, _z_axis);
	DDX_Control(pDX, IDC_Y_AXIS, _y_axis);
	DDX_Control(pDX, IDC_X_AXIS, _x_axis);
	DDX_Control(pDX, IDC_PAIR_LIST, _pair_list);
	DDX_Control(pDX, IDC_DATA_SET, _data_set);
	DDX_Check(pDX, IDC_SHOW_GRID, _show_grid);
	DDX_Check(pDX, IDC_SHOW_LEGEND, _show_legend);
	DDX_Check(pDX, IDC_USE_Z_AXIS, _use_z_axis);
	DDX_Text(pDX, IDC_TXT_DATA_SET, _txt_data_set);
	DDX_Text(pDX, IDC_TXT_X_AXIS, _txt_x_axis);
	DDX_Text(pDX, IDC_TXT_Y_AXIS, _txt_y_axis);
	DDX_Text(pDX, IDC_TXT_Z_AXIS, _txt_z_axis);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(sel_data_dlg_mfc, CDialog)
	//{{AFX_MSG_MAP(sel_data_dlg_mfc)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
ON_CBN_SELCHANGE(IDC_DATA_SET, OnSelchangeDataSet)
ON_BN_CLICKED(IDC_DELETE, OnDelete)
ON_BN_CLICKED(IDC_OPTIONS, OnOptions)
ON_LBN_DBLCLK(IDC_PAIR_LIST, OnDblclkPairList) ON_LBN_SELCHANGE(IDC_PAIR_LIST, OnSelchangePairList)
	//}}AFX_MSG_MAP
	END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten sel_data_dlg_mfc 
     BOOL sel_data_dlg_mfc::OnInitDialog()
{
	CDialog::OnInitDialog();

	// do i18n stuff
	this->SetWindowText(gettext("Select Data"));
	_txt_data_set = gettext("data-set");
	GetDlgItem(IDC_SHOW_LEGEND)->SetWindowText(gettext("show legend"));
	GetDlgItem(IDC_SHOW_GRID)->SetWindowText(gettext("show grid"));
	GetDlgItem(IDC_USE_Z_AXIS)->SetWindowText(gettext("use Z-Axis"));
	_txt_x_axis = gettext("X-Axis");
	_txt_y_axis = gettext("Y-Axis");
	_txt_z_axis = gettext("Z-Axis");
	GetDlgItem(IDC_ADD)->SetWindowText(gettext("Add"));
	GetDlgItem(IDC_DELETE)->SetWindowText(gettext("Delete"));
	GetDlgItem(IDC_OPTIONS)->SetWindowText(gettext("Options..."));
	UpdateData(FALSE);

	_show_grid = _opt->show_grid;
	_show_legend = _opt->show_legend;

	// init data-set combos
	value_handle vh = ra_value_malloc();
	ra_class_get(_eval, NULL, vh);
	long n = ra_value_get_num_elem(vh);
	const void **clh_a = ra_value_get_voidp_array(vh);
	value_handle vh_n = ra_value_malloc();
	int sel_item = 0;
	int sel_combo_idx;
	for (long l = 0; l < n; l++)
	{
		ra_info_get((class_handle)(clh_a[l]), RA_INFO_CLASS_ASCII_ID_C, vh_n);
		int idx = _data_set.AddString(ra_value_get_string(vh_n));
		_data_set.SetItemData(idx, l);

		if (l == 0)
			sel_combo_idx = idx;

		if (_opt->clh == (class_handle)(clh_a[l]))
		{
			sel_combo_idx = idx;
			sel_item = l;
		}
	}
	ra_value_free(vh_n);
	if (n > 0)
	{
		_data_set.SetCurSel(sel_combo_idx);
		_opt->clh = (class_handle)(clh_a[sel_item]);
		CString s;
		_data_set.GetDlgItemText(sel_combo_idx, s);
		strcpy(_opt->class_name, s);
	}

	ra_value_free(vh);

	_init_axis();
	_init_pair_list();

	UpdateData(FALSE);

	return TRUE;		// return TRUE unless you set the focus to a control
	// EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
} // OnInitDialog()


void
sel_data_dlg_mfc::_init_axis()
{
	_x_axis.ResetContent();
	_y_axis.ResetContent();
	_z_axis.ResetContent();

	value_handle vh = ra_value_malloc();

	if (_opt->clh)
	{
		ra_prop_get_all(_opt->clh, vh);
		long n = ra_value_get_num_elem(vh);
		const void **ph_a = ra_value_get_voidp_array(vh);

		value_handle vh_n = ra_value_malloc();
		for (long l = 0; l < n; l++)
		{
			ra_info_get((prop_handle)(ph_a[l]), RA_INFO_PROP_NAME_C, vh_n);

			int idx = _x_axis.AddString(ra_value_get_string(vh_n));
			_x_axis.SetItemData(idx, l);
			idx = _y_axis.AddString(ra_value_get_string(vh_n));
			_y_axis.SetItemData(idx, l);
			idx = _z_axis.AddString(ra_value_get_string(vh_n));
			_z_axis.SetItemData(idx, l);
		}
		ra_value_free(vh_n);

		_x_axis.SetCurSel(0);
		_y_axis.SetCurSel(0);
		_z_axis.SetCurSel(0);
	}

	ra_value_free(vh);
} // _init_axis()


void
sel_data_dlg_mfc::_init_pair_list()
{
	_pair_list.ResetContent();

	for (int i = 0; i < _opt->num_plot_pairs; i++)
	{
		CString s;
		format_pair(i, s);

		int idx = _pair_list.AddString(s);
		_pair_list.SetItemData(idx, i);
	}
} // _init_pair_list()


void
sel_data_dlg_mfc::format_pair(int idx, CString &entry)
{
	entry = _opt->axis[_opt->pair[idx].x_idx].prop_name;
	if (_opt->axis[_opt->pair[idx].x_idx].ch != -1)
	{
		CString ch_name;
		get_ch_name(_opt->axis[_opt->pair[idx].x_idx].ch, ch_name);
		entry += " (ch-";
		entry += ch_name;
		entry += ")";
	}
	
	entry += " / ";
	entry += _opt->axis[_opt->pair[idx].y_idx].prop_name;
	if (_opt->axis[_opt->pair[idx].y_idx].ch != -1)
	{
		CString ch_name;
		get_ch_name(_opt->axis[_opt->pair[idx].y_idx].ch, ch_name);
		entry += " (ch-";
		entry += ch_name;
		entry += ")";
	}
	
	if (_opt->pair[idx].use_z_axis)
	{
		entry += " / ";
		entry += _opt->axis[_opt->pair[idx].z_idx].prop_name;
		if (_opt->axis[_opt->pair[idx].z_idx].ch != -1)
		{
			CString ch_name;
			get_ch_name(_opt->axis[_opt->pair[idx].z_idx].ch, ch_name);
			entry += " (ch-";
			entry += ch_name;
			entry += ")";
		}
	}
} // format_pair()


void
sel_data_dlg_mfc::get_ch_name(long ch, CString &name)
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
sel_data_dlg_mfc::OnAdd()
{
	UpdateData();

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

	t->use_z_axis = _use_z_axis;

	CString s;
	_x_axis.GetLBText(_x_axis.GetCurSel(), s);
	_set_axis(&(t->x_idx), s);
	_y_axis.GetLBText(_y_axis.GetCurSel(), s);
	_set_axis(&(t->y_idx), s);
	if (_use_z_axis)
	{
		_y_axis.GetLBText(_y_axis.GetCurSel(), s);
		_set_axis(&(t->z_idx), s);
	}

	CString entry = _opt->axis[t->y_idx].prop_name;
	if (_opt->axis[t->y_idx].ch != -1)
	{
		CString ch_name;
		get_ch_name(_opt->axis[t->y_idx].ch, ch_name);
		entry += "(ch-";
		entry += ch_name;
		entry += ")";
	}
	strcpy(t->name, (const char *)entry);

	format_pair(_opt->num_plot_pairs-1, s);
	_pair_list.AddString(s);
} // OnAdd()


void
sel_data_dlg_mfc::_set_axis(int *idx, CString &s)
{
	if (_opt->clh == NULL)
		return;

	char temp[EVAL_MAX_NAME];
	strcpy(temp, s);
	prop_handle p = ra_prop_get(_opt->clh, temp);
	for (int i = 0; i < _opt->num_axis; i++)
	{
		if (_opt->axis[i].event_property == p)
		{
			*idx = i;
			return;
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
	strcpy(a->prop_name, s);
	a->ch = -1;		// set ch to '-1' -> use ch-independet value as default

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
sel_data_dlg_mfc::OnSelchangeDataSet()
{
	int idx = _data_set.GetCurSel();
	if (idx == LB_ERR)
		return;

	CString s;
	_data_set.GetDlgItemText(idx, s);
	strcpy(_opt->class_name, s);

	value_handle vh = ra_value_malloc();
	ra_class_get(_eval, _opt->class_name, vh);
	if (ra_value_get_num_elem(vh) <= 0)
	{
		ra_value_free(vh);
		return;
	}
	_opt->clh = (class_handle)(ra_value_get_voidp_array(vh)[0]);

	_init_axis();
} // OnSelchangeDataSet()


void
sel_data_dlg_mfc::OnDelete()
{
	int idx = _pair_list.GetCurSel();
	if (idx == LB_ERR)
		return;

	int curr = _pair_list.GetItemData(idx);

	if (curr == -1)
		return;

	_opt->num_plot_pairs--;
	struct ra_plot_pair *t =
		(struct ra_plot_pair *)
		ra_alloc_mem(sizeof(struct ra_plot_pair) * _opt->num_plot_pairs);
	if (curr != 0)
		memcpy(t, _opt->pair, sizeof(struct ra_plot_pair) * curr);
	if (curr < _opt->num_plot_pairs)
		memcpy(&(t[curr]), &(_opt->pair[curr + 1]),
		       sizeof(struct ra_plot_pair) * (_opt->num_plot_pairs - curr));

	ra_free_mem(_opt->pair);
	_opt->pair = t;

	_init_pair_list();
} // OnDelete()


void
sel_data_dlg_mfc::OnOptions()
{
	int idx_combo = _pair_list.GetCurSel();
	if (idx_combo == LB_ERR)
		return;
	int idx = _pair_list.GetItemData(idx_combo);

	struct plugin_struct *pl = (struct plugin_struct *) _plugin;
	HMODULE mod = LoadLibrary(pl->info.file);
	HRSRC res = FindResource(mod, (LPCTSTR) IDD_PAIR_OPT_DLG, RT_DIALOG);
	HGLOBAL g = LoadResource(mod, res);
	LPVOID p = LockResource(g);

	pair_opt_dlg_mfc dlg;
	dlg.init(_meas, _plugin, _opt, _pair_list.GetCurSel());
	dlg.InitModalIndirect(p, this);
	dlg.DoModal();
	FreeLibrary(mod);

	// maybe channel was changed therefore build legend-name
	CString entry = _opt->axis[_opt->pair[idx].y_idx].prop_name;
	if (_opt->axis[_opt->pair[idx].y_idx].ch != -1)
	{
		CString ch_name;
		get_ch_name(_opt->axis[_opt->pair[idx].y_idx].ch, ch_name);
		entry += "(ch-";
		entry += ch_name;
		entry += ")";
	}
	strcpy(_opt->pair[idx].name, (const char *)entry);

	_init_pair_list();
} // OnOptions()


void
sel_data_dlg_mfc::OnDblclkPairList()
{
	OnOptions();
} // OnDblclkPairList()


void
sel_data_dlg_mfc::OnSelchangePairList()
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen

}				// OnSelchangePairList()


void
sel_data_dlg_mfc::OnCancel()
{
	_free_opt_struct(_opt);
	_opt = NULL;

	CDialog::OnCancel();
}				// OnCancel()


void
sel_data_dlg_mfc::OnOK()
{
	UpdateData();

	_opt->show_grid = _show_grid;
	_opt->show_legend = _show_legend;

	_copy_opt_struct(_opt_orig, _opt);

	_free_opt_struct(_opt);
	_opt = NULL;

	CDialog::OnOK();
}				// OnOK()


void
sel_data_dlg_mfc::_copy_opt_struct(struct ra_plot_options *dest, struct ra_plot_options *src)
{
	if ((src == NULL) || (dest == NULL))
		return;

	dest->eval = src->eval;
	dest->clh = src->clh;
	strcpy(dest->class_name, src->class_name);
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
sel_data_dlg_mfc::_copy_pair_struct(struct ra_plot_pair **dest, struct ra_plot_pair *src, int n)
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
sel_data_dlg_mfc::_copy_axis_struct(struct ra_plot_axis **dest, struct ra_plot_axis *src, int n)
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
sel_data_dlg_mfc::_free_opt_struct(struct ra_plot_options *opt)
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

	ra_free_mem(opt);
} // _free_opt_struct()
