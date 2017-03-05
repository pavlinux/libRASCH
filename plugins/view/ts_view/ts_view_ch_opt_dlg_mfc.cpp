/*----------------------------------------------------------------------------
 * ts_view_ch_opt_dlg_mfc.cpp
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

// ts_view_ch_opt_dlg_mfc.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "resource.h"
#include "ts_view_ch_opt_dlg_mfc.h"
#include "ts_view.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld ts_view_ch_opt_dlg_mfc 


//ts_view_ch_opt_dlg_mfc::ts_view_ch_opt_dlg_mfc(CWnd* pParent /*=NULL*/)
//      : CDialog(ts_view_ch_opt_dlg_mfc::IDD, pParent)
//{
//      //{{AFX_DATA_INIT(ts_view_ch_opt_dlg_mfc)
//      _invert = FALSE;
//      _max = 0.0;
//      _pos_percent = 0;
//      _res = 0.0;
//      _res_unit = _T("");
//      _show_ch = FALSE;
//      _use_res = FALSE;
//      _min = 0.0;
//      //}}AFX_DATA_INIT
//}


void
ts_view_ch_opt_dlg_mfc::DoDataExchange(CDataExchange * pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ts_view_ch_opt_dlg_mfc)
	DDX_Control(pDX, IDC_CH_LIST, _ch_list);
	DDX_Check(pDX, IDC_INVERT, _invert);
	DDX_Text(pDX, IDC_MAX, _max);
	DDX_Text(pDX, IDC_POS_PERCENT, _pos_percent);
	DDX_Text(pDX, IDC_RES, _res);
	DDX_Text(pDX, IDC_RES_UNIT, _res_unit);
	DDX_Check(pDX, IDC_SHOW_CHANNEL, _show_ch);
	DDX_Check(pDX, IDC_USE_RES, _use_res);
	DDX_Text(pDX, IDC_MIN, _min);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ts_view_ch_opt_dlg_mfc, CDialog)
	//{{AFX_MSG_MAP(ts_view_ch_opt_dlg_mfc)
	ON_BN_CLICKED(IDC_USE_RES, OnUseRes)
ON_CBN_SELCHANGE(IDC_CH_LIST, OnSelchangeChList)
ON_BN_CLICKED(IDC_POS_CENTER, OnPosCenter) ON_BN_CLICKED(IDC_POS_MINMAX, OnPosMinmax)
	//}}AFX_MSG_MAP
	END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten ts_view_ch_opt_dlg_mfc 
     void ts_view_ch_opt_dlg_mfc::init(struct ch_info **ch, int num, int curr_ch)
{
	_ch_save = ch;
	_num = num;

	if (num > 0)
	{
		_ch = new struct ch_info[num];
		memcpy(_ch, *_ch_save, sizeof(struct ch_info) * num);
	}
	else
		_ch = NULL;

	_prev_ch = -1;
	_curr_ch = curr_ch;
}				// init()


BOOL ts_view_ch_opt_dlg_mfc::OnInitDialog()
{
	CDialog::OnInitDialog();

	// do l10n stuff
	this->SetWindowText(gettext("Set Channel Draw-Options"));
	GetDlgItem(IDC_TXT_SEL_CH)->SetWindowText(gettext("Select Channel"));
	GetDlgItem(IDC_SHOW_CHANNEL)->SetWindowText(gettext("show channel"));
	GetDlgItem(IDC_INVERT)->SetWindowText(gettext("invert channel"));
	GetDlgItem(IDC_TXT_RESOLUTION)->SetWindowText(gettext("Resolution"));
	GetDlgItem(IDC_FRAME_POS)->SetWindowText(gettext("Position"));
	GetDlgItem(IDC_POS_CENTER)->SetWindowText(gettext("centered"));
	GetDlgItem(IDC_POS_PERCENT_TEXT)->SetWindowText(gettext("position [%]"));
	GetDlgItem(IDC_MIN_TEXT)->SetWindowText(gettext("min"));
	GetDlgItem(IDC_MAX_TEXT)->SetWindowText(gettext("max"));
	GetDlgItem(IDC_USE_RES)->SetWindowText(gettext("use Res."));
	UpdateData(FALSE);

	for (int i = 0; i < _num; i++)
		_ch_list.AddString(_ch[i].name);

	if (_num > 0)
	{
		int
			idx =
			0;

		if ((_curr_ch >= 0) && (_curr_ch <= _num))
			idx = _curr_ch;

		_ch_list.SetCurSel(idx);
		OnSelchangeChList();
	}

	return TRUE;		// return TRUE unless you set the focus to a control
	// EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}				// OnInitDialog()


void
ts_view_ch_opt_dlg_mfc::OnSelchangeChList()
{
	int idx = _ch_list.GetCurSel();
	if (idx == LB_ERR)
		return;

	if (idx != _prev_ch)
	{
		_save_data(_prev_ch);
		_prev_ch = idx;
	}

	_invert = _ch[idx].inverse;
	_pos_percent = _ch[idx].center_pos;
	_max = _ch[idx].max_value;
	_min = _ch[idx].min_value;
	_res = _ch[idx].mm_per_unit;
	_show_ch = _ch[idx].show_it;
	_use_res = _ch[idx].use_mm_per_unit;

	CString s = "mm/";
	s += _ch[idx].unit;
	_res_unit = s;

	UpdateData(FALSE);

	s = gettext("min/max");
	s += " [";
	s += _ch[idx].unit;
	s += "]";
	GetDlgItem(IDC_POS_MINMAX)->SetWindowText(s);

	_pos_sel();
}				// OnSelchangeChList()


void
ts_view_ch_opt_dlg_mfc::OnPosMinmax()
{
	int idx = _ch_list.GetCurSel();
	if (idx == LB_ERR)
		return;

	_ch[idx].centered = false;
	_pos_sel();
}				// OnPosMinmax()


void
ts_view_ch_opt_dlg_mfc::OnPosCenter()
{
	int idx = _ch_list.GetCurSel();
	if (idx == LB_ERR)
		return;

	_ch[idx].centered = true;
	_pos_sel();
}				// OnPosCenter()


void
ts_view_ch_opt_dlg_mfc::OnUseRes()
{
	int idx = _ch_list.GetCurSel();
	if (idx == LB_ERR)
		return;

	_save_data(idx);

	_ch[idx].use_mm_per_unit = (_use_res ? true : false);

	if (_ch[idx].use_mm_per_unit)
	{
		GetDlgItem(IDC_MAX_TEXT)->EnableWindow(FALSE);
		GetDlgItem(IDC_MAX)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_MAX_TEXT)->EnableWindow(TRUE);
		GetDlgItem(IDC_MAX)->EnableWindow(TRUE);
	}
}				// OnUseRes()


void
ts_view_ch_opt_dlg_mfc::_save_data(int ch)
{
	if (ch < 0)
		return;

	UpdateData();

	_ch[ch].inverse = (_invert ? true : false);
	_ch[ch].max_value = _max;
	_ch[ch].min_value = _min;
	_ch[ch].mm_per_unit = _res;
	_ch[ch].show_it = (_show_ch ? true : false);
	_ch[ch].use_mm_per_unit = (_use_res ? true : false);
	_ch[ch].center_pos = _pos_percent;

	if (GetCheckedRadioButton(IDC_POS_CENTER, IDC_POS_MINMAX) == IDC_POS_CENTER)
		_ch[ch].centered = true;
	else
		_ch[ch].centered = false;
}				// _save_data()


void
ts_view_ch_opt_dlg_mfc::_pos_sel()
{
	int idx = _ch_list.GetCurSel();
	if (idx == LB_ERR)
		return;

	if (_ch[idx].centered)
	{
		CheckRadioButton(IDC_POS_CENTER, IDC_POS_MINMAX, IDC_POS_CENTER);

		GetDlgItem(IDC_POS_PERCENT_TEXT)->EnableWindow(TRUE);
		GetDlgItem(IDC_POS_PERCENT)->EnableWindow(TRUE);
		GetDlgItem(IDC_INVERT)->EnableWindow(TRUE);

		GetDlgItem(IDC_MIN_TEXT)->EnableWindow(FALSE);
		GetDlgItem(IDC_MIN)->EnableWindow(FALSE);
		GetDlgItem(IDC_MAX_TEXT)->EnableWindow(FALSE);
		GetDlgItem(IDC_MAX)->EnableWindow(FALSE);
		GetDlgItem(IDC_USE_RES)->EnableWindow(FALSE);
	}
	else
	{
		CheckRadioButton(IDC_POS_CENTER, IDC_POS_MINMAX, IDC_POS_MINMAX);

		GetDlgItem(IDC_POS_PERCENT_TEXT)->EnableWindow(FALSE);
		GetDlgItem(IDC_POS_PERCENT)->EnableWindow(FALSE);
		GetDlgItem(IDC_INVERT)->EnableWindow(FALSE);

		GetDlgItem(IDC_MIN_TEXT)->EnableWindow(TRUE);
		GetDlgItem(IDC_MIN)->EnableWindow(TRUE);

		GetDlgItem(IDC_USE_RES)->EnableWindow(TRUE);
		OnUseRes();
	}

	UpdateData(FALSE);
}				// _pos_sel()


void
ts_view_ch_opt_dlg_mfc::OnOK()
{
	int idx = _ch_list.GetCurSel();
	if (idx != LB_ERR)
		_save_data(idx);

	if (_num > 0)
		memcpy(*_ch_save, _ch, sizeof(struct ch_info) * _num);

	CDialog::OnOK();
}				// OnOK()
