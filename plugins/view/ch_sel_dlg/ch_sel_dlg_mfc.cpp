/**
 * \file ch_sel_dlg_mfc.c
 *
 * MFC specific code for the channel select dialog.
 *
 * \author Raphael Schneider (rasch@med1.med.tum.de)
 */

/*--------------------------------------------------------------------------
 *
 * Copyright (C) 2002-2004, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Header: /home/cvs_repos.bak/librasch/plugins/view/ch_sel_dlg/ch_sel_dlg_mfc.cpp,v 1.9 2004/06/30 16:44:26 rasch Exp $
 *
 *--------------------------------------------------------------------------*/

// ch_sel_dlg_mfc.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "Resource.h"
#include "ch_sel_dlg_mfc.h"

#include <ra_ch_list.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// Dialogfeld ch_sel_dlg_mfc 


ch_sel_dlg_mfc::ch_sel_dlg_mfc(struct proc_info *pi, CWnd * pParent /*=NULL*/ )
	:
CDialog(ch_sel_dlg_mfc::IDD, pParent)
{
	//{{AFX_DATA_INIT(ch_sel_dlg_mfc)
	_txt_ch_use = _T("");
	_txt_rec_ch = _T("");
	//}}AFX_DATA_INIT
	_pi = pi;
}  // constructor


void
ch_sel_dlg_mfc::DoDataExchange(CDataExchange * pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ch_sel_dlg_mfc)
	DDX_Control(pDX, IDC_USE_CH_LIST, _use_ch_list);
	DDX_Control(pDX, IDC_USE, _use_btn);
	DDX_Control(pDX, IDC_UP, _up_btn);
	DDX_Control(pDX, IDC_NOT_USE, _not_use_btn);
	DDX_Control(pDX, IDC_DOWN, _down_btn);
	DDX_Control(pDX, IDC_CH_LIST, _ch_list);
	DDX_Text(pDX, IDC_TXT_CH_USE, _txt_ch_use);
	DDX_Text(pDX, IDC_TXT_REC_CH, _txt_rec_ch);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ch_sel_dlg_mfc, CDialog)
	//{{AFX_MSG_MAP(ch_sel_dlg_mfc)
	ON_LBN_SELCHANGE(IDC_CH_LIST, all_ch_sel)
ON_LBN_DBLCLK(IDC_CH_LIST, ch_use)
ON_LBN_SELCHANGE(IDC_USE_CH_LIST, use_ch_sel)
ON_LBN_DBLCLK(IDC_USE_CH_LIST, ch_dont_use)
ON_BN_CLICKED(IDC_NOT_USE, ch_dont_use)
ON_BN_CLICKED(IDC_USE, ch_use)
ON_BN_CLICKED(IDC_UP, ch_up) ON_BN_CLICKED(IDC_DOWN, ch_down) ON_BN_CLICKED(IDOK, ok)
	//}}AFX_MSG_MAP
	END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten ch_sel_dlg_mfc 
     BOOL ch_sel_dlg_mfc::OnInitDialog()
{
	CDialog::OnInitDialog();

	// do l10n stuff
	this->SetWindowText(gettext("Select channel's to process"));
	_txt_rec_ch = gettext("recorded channels");
	_txt_ch_use = gettext("channels use");
	GetDlgItem(IDC_UP)->SetWindowText(gettext("up"));
	GetDlgItem(IDC_DOWN)->SetWindowText(gettext("down"));
	UpdateData(FALSE);

	value_handle vh = ra_value_malloc();
	if (ra_info_get(_pi->rh, RA_INFO_REC_GEN_NUM_CHANNEL_L, vh) != 0)
	{
		ra_value_free(vh);
		return FALSE;
	}

	long num_ch = ra_value_get_long(vh);
	for (long l = 0; l < num_ch; l++)
	{
		ra_value_set_number(vh, l);
		if (ra_info_get(_pi->rh, RA_INFO_REC_CH_NAME_C, vh) != 0)
			continue;
		int idx = _ch_list.AddString(ra_value_get_string(vh));
		_ch_list.SetItemData(idx, l);
	}
	ra_value_free(vh);

	return TRUE;		// return TRUE unless you set the focus to a control
	// EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}  // OnInitDialog()


void
ch_sel_dlg_mfc::all_ch_sel()
{
	int idx = _ch_list.GetCurSel();

	if (idx == LB_ERR)
		GetDlgItem(IDC_USE)->EnableWindow(FALSE);
	else
		GetDlgItem(IDC_USE)->EnableWindow(TRUE);
} // all_ch_sel()


void
ch_sel_dlg_mfc::ch_use()
{
	int idx = _ch_list.GetCurSel();

	if (idx == LB_ERR)
		return;

	int value = _ch_list.GetItemData(idx);
	CString s;
	_ch_list.GetText(idx, s);
	_ch_list.DeleteString(idx);
	_ch_list.SetCurSel(-1);

	idx = _use_ch_list.AddString(s);
	_use_ch_list.SetItemData(idx, value);
} // ch_use()


void
ch_sel_dlg_mfc::use_ch_sel()
{
	int idx = _use_ch_list.GetCurSel();

	if (idx == LB_ERR)
	{
		GetDlgItem(IDC_UP)->EnableWindow(FALSE);
		GetDlgItem(IDC_DOWN)->EnableWindow(FALSE);
		GetDlgItem(IDC_DONT_USE)->EnableWindow(FALSE);
	}
	else
	{
		if (idx == 0)
			GetDlgItem(IDC_UP)->EnableWindow(FALSE);
		else
			GetDlgItem(IDC_UP)->EnableWindow(TRUE);

		if (idx == ((int) _use_ch_list.GetCount() - 1))
			GetDlgItem(IDC_DOWN)->EnableWindow(FALSE);
		else
			GetDlgItem(IDC_DOWN)->EnableWindow(TRUE);

		GetDlgItem(IDC_DONT_USE)->EnableWindow(TRUE);
	}
} // use_ch_sel()


void
ch_sel_dlg_mfc::ch_dont_use()
{
	int idx = _use_ch_list.GetCurSel();
	if (idx == LB_ERR)
		return;

	int value = _use_ch_list.GetItemData(idx);
	CString s;
	_use_ch_list.GetText(idx, s);

	// find position in all-ch-list to insert removed channel
	int idx_dest = -1;
	for (int i = 0; i < (int) _ch_list.GetCount(); i++)
	{
		if (_ch_list.GetItemData(i) > (DWORD) value)
		{
			idx_dest = i;
			break;
		}
	}
	_ch_list.InsertString(idx_dest, s);
	_ch_list.SetItemData(idx_dest, value);
	_use_ch_list.DeleteString(idx);
	_use_ch_list.SetCurSel(-1);
} // ch_dont_use()


void
ch_sel_dlg_mfc::ch_up()
{
	int idx = _use_ch_list.GetCurSel();
	if ((idx == LB_ERR) || (idx == 0))
		return;

	int value = _use_ch_list.GetItemData(idx);
	CString s;
	_use_ch_list.GetText(idx, s);

	_use_ch_list.DeleteString(idx);
	idx--;
	_use_ch_list.InsertString(idx, s);
	_use_ch_list.SetItemData(idx, value);
	_use_ch_list.SetCurSel(idx);
} // ch_up()


void
ch_sel_dlg_mfc::ch_down()
{
	int idx = _use_ch_list.GetCurSel();
	if ((idx == LB_ERR) || (idx == (_use_ch_list.GetCount() - 1)))
		return;

	int value = _use_ch_list.GetItemData(idx);
	CString s;
	_use_ch_list.GetText(idx, s);

	_use_ch_list.DeleteString(idx);
	idx++;
	_use_ch_list.InsertString(idx, s);
	_use_ch_list.SetItemData(idx, value);
	_use_ch_list.SetCurSel(idx);
} // down()


void
ch_sel_dlg_mfc::ok()
{
	_pi->num_results = 1;
	if (_use_ch_list.GetCount() <= 0)
	{
		OnOK();
		return;
	}

	long *ch = new long[_use_ch_list.GetCount()];
	for (int i = 0; i < _use_ch_list.GetCount(); i++)
		ch[i] = _use_ch_list.GetItemData(i);

	value_handle *res = (value_handle *)_pi->results;
	ra_value_set_long_array(res[0], ch, _use_ch_list.GetCount());

	delete[] ch;

	OnOK();
} // ok()
