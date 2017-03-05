/*----------------------------------------------------------------------------
 * gen_opt_dlg_mfc.cpp
 *
 * 
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002-2003, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Header: /home/cvs_repos.bak/librasch/plugins/view/ts_view/gen_opt_dlg_mfc.cpp,v 1.4 2004/06/30 16:44:26 rasch Exp $
 *--------------------------------------------------------------------------*/

// gen_opt_dlg_mfc.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "resource.h"
#include "gen_opt_dlg_mfc.h"
#include "ts_view.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld gen_opt_dlg_mfc 


gen_opt_dlg_mfc::gen_opt_dlg_mfc(struct ts_options *ts_opt, CWnd * pParent /*=NULL*/ )
	:
CDialog(gen_opt_dlg_mfc::IDD, pParent)
{
	if (ts_opt)
		memcpy(&_ts_opt, ts_opt, sizeof(struct ts_options));
	_ts_opt_save = ts_opt;

	//{{AFX_DATA_INIT(gen_opt_dlg_mfc)
	_ch_height = ts_opt->ch_height;
	_show_ch_info = ts_opt->show_ch_info;
	_show_grid = ts_opt->plot_grid;
	_show_status = ts_opt->show_status;
	_speed = ts_opt->mm_per_sec;
	_use_fix_ch_height = !ts_opt->ch_height_dynamic;
	//}}AFX_DATA_INIT
}


void
gen_opt_dlg_mfc::DoDataExchange(CDataExchange * pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(gen_opt_dlg_mfc)
	DDX_Text(pDX, IDC_CH_HEIGHT, _ch_height);
	DDX_Check(pDX, IDC_SHOW_CH_INFO, _show_ch_info);
	DDX_Check(pDX, IDC_SHOW_GRID, _show_grid);
	DDX_Check(pDX, IDC_SHOW_STATUS, _show_status);
	DDX_Text(pDX, IDC_SPEED, _speed);
	DDX_Check(pDX, IDC_USE_FIX_CH_HEIGHT, _use_fix_ch_height);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(gen_opt_dlg_mfc, CDialog)
	//{{AFX_MSG_MAP(gen_opt_dlg_mfc)
	ON_BN_CLICKED(IDC_USE_FIX_CH_HEIGHT, OnUseFixChHeight)
	//}}AFX_MSG_MAP
	END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten gen_opt_dlg_mfc 
     void gen_opt_dlg_mfc::set_opt(struct ts_options *ts_opt)
{
	if (ts_opt)
		memcpy(&_ts_opt, ts_opt, sizeof(struct ts_options));
	_ts_opt_save = ts_opt;

	//{{AFX_DATA_INIT(gen_opt_dlg_mfc)
	_ch_height = ts_opt->ch_height;
	_show_ch_info = ts_opt->show_ch_info;
	_show_grid = ts_opt->plot_grid;
	_show_status = ts_opt->show_status;
	_speed = ts_opt->mm_per_sec;
	_use_fix_ch_height = !ts_opt->ch_height_dynamic;
	//}}AFX_DATA_INIT
}				// set_opt()


BOOL
gen_opt_dlg_mfc::OnInitDialog()
{
	CDialog::OnInitDialog();

	// do l10n stuff
	this->SetWindowText(gettext("Set General Draw-Options"));
	GetDlgItem(IDC_SHOW_STATUS)->SetWindowText(gettext("show statusline"));
	GetDlgItem(IDC_SHOW_CH_INFO)->SetWindowText(gettext("show channel info"));
	GetDlgItem(IDC_SHOW_GRID)->SetWindowText(gettext("show grid"));
	GetDlgItem(IDC_USE_FIX_CH_HEIGHT)->SetWindowText(gettext("use fixed channel-height"));
	GetDlgItem(IDC_CH_HEIGHT_TEXT)->SetWindowText(gettext("channel-height [pixel]"));
	GetDlgItem(IDC_TXT_SPEED)->SetWindowText(gettext("\"speed\" [mm/sec]"));
	UpdateData(FALSE);

	OnUseFixChHeight();

	return TRUE;		// return TRUE unless you set the focus to a control
	// EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}


void
gen_opt_dlg_mfc::OnUseFixChHeight()
{
	UpdateData();
	if (_use_fix_ch_height)
	{
		GetDlgItem(IDC_CH_HEIGHT_TEXT)->EnableWindow(TRUE);
		GetDlgItem(IDC_CH_HEIGHT)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_CH_HEIGHT_TEXT)->EnableWindow(FALSE);
		GetDlgItem(IDC_CH_HEIGHT)->EnableWindow(FALSE);
	}
}


void
gen_opt_dlg_mfc::OnOK()
{
	UpdateData();
	_ts_opt_save->ch_height = _ch_height;
	_ts_opt_save->ch_height_dynamic = !_use_fix_ch_height;
	_ts_opt_save->mm_per_sec = _speed;
	_ts_opt_save->plot_grid = (_show_grid ? 1 : 0);
	_ts_opt_save->show_ch_info = (_show_ch_info ? 1 : 0);
	_ts_opt_save->show_status = (_show_status ? 1 : 0);

	CDialog::OnOK();
}
