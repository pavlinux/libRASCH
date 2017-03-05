/*----------------------------------------------------------------------------
 * eval_dlg_mfc.cpp
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


#include "stdafx.h"
#include "eval_dlg.h"
#include "eval_dlg_mfc.h"
#include "evprop_dlg_mfc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld eval_dlg_mfc 


eval_dlg_mfc::eval_dlg_mfc(struct proc_info *pi, CWnd * pParent /*=NULL*/ )
 : CDialog(eval_dlg_mfc::IDD, pParent)
{
	//{{AFX_DATA_INIT(eval_dlg_mfc)
	_created = _T("");
	_default_eval = FALSE;
	_description = _T("");
	_host = _T("");
	_original = _T("");
	_program = _T("");
	_user = _T("");
	_modified = _T("");
	_txt_description = _T("");
	//}}AFX_DATA_INIT

	_pi = pi;

	_changed = false;
	_curr_eval = -1;
}


void
eval_dlg_mfc::DoDataExchange(CDataExchange * pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(eval_dlg_mfc)
	DDX_Control(pDX, IDC_GROUP, _group);
	DDX_Control(pDX, IDC_EVAL_LIST, _eval_list);
	DDX_Text(pDX, IDC_CREATED, _created);
	DDX_Check(pDX, IDC_DEFAULT, _default_eval);
	DDX_Text(pDX, IDC_DESC, _description);
	DDX_Text(pDX, IDC_HOST, _host);
	DDX_Text(pDX, IDC_ORIGINAL, _original);
	DDX_Text(pDX, IDC_PROGRAM, _program);
	DDX_Text(pDX, IDC_USER, _user);
	DDX_Text(pDX, IDC_MODIFIED, _modified);
	DDX_Text(pDX, IDC_TXT_DESCRIPTION, _txt_description);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(eval_dlg_mfc, CDialog)
	//{{AFX_MSG_MAP(eval_dlg_mfc)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_DEFAULT, OnDefault)
	ON_LBN_SELCHANGE(IDC_EVAL_LIST, OnSelchangeEvalList)
	ON_BN_CLICKED(IDC_EVENT_TYPES, OnEventTypes)
	//}}AFX_MSG_MAP
	END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten eval_dlg_mfc 
BOOL
eval_dlg_mfc::OnInitDialog()
{
	CDialog::OnInitDialog();

	// do l10n stuff
	this->SetWindowText(gettext("Evaluations"));
	GetDlgItem(IDC_DELETE)->SetWindowText(gettext("delete"));
	_txt_description = gettext("Description");
	GetDlgItem(IDC_DEFAULT)->SetWindowText(gettext("default evaluation"));
	GetDlgItem(IDC_EVENT_TYPES)->SetWindowText(gettext("event-types"));
	UpdateData(FALSE);

	_all_eval = NULL;
	_num_eval = 0;
	_curr_eval = -1;

	init();

	return TRUE;		// return TRUE unless you set the focus to a control
	// EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}  // OnInitDialog()


void
eval_dlg_mfc::init()
{
	if (_all_eval)
	{
		delete[] _all_eval;
		_all_eval = NULL;
		_num_eval = 0;
		_curr_eval = -1;
	}

	value_handle vh = ra_value_malloc();
	ra_eval_get_all(_pi->mh, vh);
	_num_eval = ra_value_get_num_elem(vh);
	_all_eval = new eval_handle[_num_eval];
	long l;
	for (l = 0; l < _num_eval; l++)
		_all_eval[l] = (eval_handle *)(ra_value_get_voidp_array(vh))[l];

	_eval_list.ResetContent();

	for (l = 0; l < _num_eval; l++)
	{
		ra_info_get(_all_eval[l], RA_INFO_EVAL_NAME_C, vh);
		int idx = _eval_list.AddString(ra_value_get_string(vh));
		_eval_list.SetItemData(idx, l);
	}
	ra_value_free(vh);

	if (_num_eval > 0)
	{
		_eval_list.SetCurSel(0);
		OnSelchangeEvalList();
	}
}  // init()


void
eval_dlg_mfc::OnDelete()
{
	if (_curr_eval == -1)
		return;

	if (MessageBox("Do you really want to delete the evaluation?", "libRASCH", MB_ICONQUESTION | MB_YESNO) == IDYES)
	{
		ra_eval_delete(_all_eval[_curr_eval]);
		init();
	}
}  // OnDelete()


void
eval_dlg_mfc::OnDefault()
{
	UpdateData();

	if (_curr_eval == -1)
		return;

	if (_default_eval)
		ra_eval_set_default(_all_eval[_curr_eval]);
	else
	{
		MessageBox("You can't deselect default evalution.\n"
			       "Please choose evaluation which you want to be the\n"
			       "default-evalution and check there the default-box.",
				   "libRASCH", MB_ICONEXCLAMATION | MB_OK);

		_default_eval = TRUE;
		UpdateData(FALSE);
	}
}  // OnDefault()


void
eval_dlg_mfc::OnOK()
{
	if (_all_eval)
	{
		delete[] _all_eval;
		_all_eval = NULL;
		_num_eval = 0;
		_curr_eval = -1;
	}

	CDialog::OnOK();
}  // OnOK()


void
eval_dlg_mfc::OnSelchangeEvalList()
{
	if (_eval_list.GetCurSel() == LB_ERR)
		return;

/*  needed later when real cancel is supported
	if (_changed)
	{
		CString s = "Settings were modified, do you want to discard changes?";
		if (MessageBox(s, "libRASCH", MB_ICONSTOP | MB_YESNO) == IDNO)
		{
			_eval_list.SetCurSel(_curr_eval);
			return;
		}
	}
*/

	_curr_eval = _eval_list.GetItemData(_eval_list.GetCurSel());

	value_handle vh = ra_value_malloc();

	ra_info_get(_all_eval[_curr_eval], RA_INFO_EVAL_NAME_C, vh);
	_group.SetWindowText(ra_value_get_string(vh));
	ra_info_get(_all_eval[_curr_eval], RA_INFO_EVAL_DESC_C, vh);
	_description = ra_value_get_string(vh);

	_original = gettext("original evaluation:");
	_original += " ";
	ra_info_get(_all_eval[_curr_eval], RA_INFO_EVAL_ORIGINAL_L, vh);
	if (ra_value_get_long(vh) == 0)
		_original += gettext("no");
	else
		_original += gettext("yes");

	ra_info_get(_all_eval[_curr_eval], RA_INFO_EVAL_DEFAULT_L, vh);
	if (ra_value_get_long(vh) == 0)
		_default_eval = FALSE;
	else
		_default_eval = TRUE;

	_created = gettext("created:");
	_created += " ";
	ra_info_get(_all_eval[_curr_eval], RA_INFO_EVAL_ADD_TS_C, vh);
	_created += ra_value_get_string(vh);

	_modified = gettext("last modified:");
	_modified += " ";
	ra_info_get(_all_eval[_curr_eval], RA_INFO_EVAL_MODIFY_TS_C, vh);
	_modified += ra_value_get_string(vh);

	_user = gettext("user:");
	_user += " ";
	ra_info_get(_all_eval[_curr_eval], RA_INFO_EVAL_USER_C, vh);
	_user += ra_value_get_string(vh);

	_host = gettext("host:");
	_host += " ";
	ra_info_get(_all_eval[_curr_eval], RA_INFO_EVAL_HOST_C, vh);
	_host += ra_value_get_string(vh);

	_program = gettext("program:");
	_program += " ";
	ra_info_get(_all_eval[_curr_eval], RA_INFO_EVAL_PROG_C, vh);
	_program += ra_value_get_string(vh);

	ra_value_free(vh);

	_changed = false;

	UpdateData(FALSE);
	GetDlgItem(IDC_DELETE)->EnableWindow(TRUE);
	GetDlgItem(IDC_EVENT_TYPES)->EnableWindow(TRUE);
}


void
eval_dlg_mfc::OnEventTypes()
{
	struct plugin_struct *pl = (struct plugin_struct *)_pi->plugin;
	HMODULE mod = LoadLibrary(pl->info.file);
	HRSRC res = FindResource(mod, (LPCTSTR) IDD_EVPROP_DLG, RT_DIALOG);
	HGLOBAL g = LoadResource(mod, res);
	LPVOID p = LockResource(g);
	evprop_dlg_mfc dlg;

	_curr_eval = _eval_list.GetItemData(_eval_list.GetCurSel());

	dlg.set_info(_pi->mh, _all_eval[_curr_eval]);
	dlg.InitModalIndirect(p);
	dlg.DoModal();

	FreeResource(g);
} // OnEventTypes()


void
eval_dlg_mfc::OnCancel() 
{
	if (_all_eval)
	{
		delete[] _all_eval;
		_all_eval = NULL;
		_num_eval = 0;
		_curr_eval = -1;
	}
	
	CDialog::OnCancel();
}  // OnCancel()
