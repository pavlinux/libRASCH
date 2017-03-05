/*----------------------------------------------------------------------------
 * simple_calc_dlg_mfc.c
 *
 * 
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2003-2006, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

// simple_calc_dlg_mfc.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "Resource.h"
#include "simple_calc_dlg_mfc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// Dialogfeld simple_calc_dlg_mfc 


simple_calc_dlg_mfc::simple_calc_dlg_mfc(struct proc_info *pi, CWnd * pParent /*=NULL*/ )
 : CDialog(simple_calc_dlg_mfc::IDD, pParent)
{
	//{{AFX_DATA_INIT(ch_sel_dlg_mfc)
	// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
	_pi = pi;
	_num_proc_pl = 0;
	_pi_for_pl = NULL;
}


void
simple_calc_dlg_mfc::DoDataExchange(CDataExchange * pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(simple_calc_dlg_mfc)
	DDX_Control(pDX, IDC_PL_LIST, _pl_list);
	DDX_Text(pDX, IDC_RESULTS, _results);
	DDX_Text(pDX, IDC_TXT_PLUGINS, _txt_plugins);
	DDX_Text(pDX, IDC_TXT_RESULTS, _txt_results);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(simple_calc_dlg_mfc, CDialog)
	//{{AFX_MSG_MAP(simple_calc_dlg_mfc)
	ON_BN_CLICKED(IDC_CALC, OnCalc)
	ON_BN_CLICKED(IDC_EXIT, OnExit)
	ON_BN_CLICKED(IDC_COPY_CLIPBOARD, OnCopyClipboard)
	//}}AFX_MSG_MAP
	END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten ch_sel_dlg_mfc 

BOOL simple_calc_dlg_mfc::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// do l10n stuff
	this->SetWindowText(gettext("process data"));
	GetDlgItem(IDC_CALC)->SetWindowText(gettext("calculate"));
	GetDlgItem(IDC_COPY_CLIPBOARD)->SetWindowText(gettext("copy to clipboard"));
	GetDlgItem(IDC_EXIT)->SetWindowText(gettext("Exit"));
	_txt_plugins = gettext("Process Plugins");
	_txt_results = gettext("Results");
	UpdateData(FALSE);

	if (_pi->ra == NULL)
		_pi->ra = ra_lib_handle_from_any_handle(_pi->mh);

	value_handle vh = ra_value_malloc();
	if (ra_info_get(_pi->ra, RA_INFO_NUM_PLUGINS_L, vh) != 0)
		return FALSE;

	long num_pl = ra_value_get_long(vh);
	_pi_for_pl = NULL;
	_num_proc_pl = 0;
	for (long l = 0; l < num_pl; l++)
	{
		plugin_handle pl = ra_plugin_get_by_num(_pi->ra, l, 0);
		if (pl == NULL)
			continue;

		// only process plugins, which return results are used here
		if (ra_info_get(pl, RA_INFO_PL_TYPE_L, vh) != 0)
			continue;
		if (!(ra_value_get_long(vh) & PLUGIN_PROCESS))
			continue;
		
		if (ra_info_get(pl, RA_INFO_PL_NUM_RESULTS_L, vh) != 0)
			continue;
		if (ra_value_get_long(vh) <= 0)
			continue;

		if (ra_info_get(pl, RA_INFO_PL_NAME_C, vh) != 0)
			continue;
		_pl_list.AddString(ra_value_get_string(vh));

		_num_proc_pl++;
		_pi_for_pl = (struct proc_info **)realloc(_pi_for_pl, sizeof(struct proc_info *) * _num_proc_pl);
		_pi_for_pl[_num_proc_pl - 1] = 	(struct proc_info *)ra_proc_get(_pi->mh, pl, NULL);
	}
	ra_value_free(vh);

	_pl_list.SetCurSel(0);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void simple_calc_dlg_mfc::OnCalc() 
{
	CWaitCursor();

	_results = "";
	UpdateData(FALSE);

	int idx = _pl_list.GetCurSel();
	CString pl_sel;
	_pl_list.GetLBText(idx, pl_sel);
	if (pl_sel.IsEmpty())
		return;

	if (ra_proc_do(_pi_for_pl[idx]) != 0)
		return;

        /* get number of result sets and results */
	value_handle vh = ra_value_malloc();
	if (ra_info_get(_pi_for_pl[idx], RA_INFO_PROC_NUM_RES_SETS_L, vh) != 0)
	{
		ra_value_free(vh);
		return;
	}
	long n_res_set = ra_value_get_long(vh);

	if (ra_info_get(_pi_for_pl[idx]->plugin, RA_INFO_PL_NUM_RESULTS_L, vh) != 0)
	{
		ra_value_free(vh);
		return;
	}
	long n_res = ra_value_get_long(vh);

	for (long l = 0; l < n_res_set; l++)
	{
		char out[200];
		sprintf(out, "result set %ld:\n", l+1);
		_results += out;

		for (long m = 0; m < n_res; m++)
		{
			char t[100];
			
			/* test if result is a default value (some non-default results are
			   arrays which we skip) */
			ra_value_set_number(vh, m);
			ra_info_get(_pi_for_pl[idx]->plugin, RA_INFO_PL_RES_DEFAULT_L, vh);
			if (ra_value_get_long(vh) == 0)
				continue;
			
			sprintf(out, "  ");
			ra_value_set_number(vh, m);
			if (ra_info_get(_pi_for_pl[idx]->plugin, RA_INFO_PL_RES_NAME_C, vh) == 0)
			{
				strcpy(t, ra_value_get_string(vh));
				strcat(out, t);
			}
			
			if (ra_proc_get_result(_pi_for_pl[idx], m, l, vh) == 0)
			{
				switch (ra_value_get_type(vh))
				{
				case RA_VALUE_TYPE_LONG:
					sprintf(t, ": %ld", ra_value_get_long(vh));
					break;
				case RA_VALUE_TYPE_DOUBLE:
					sprintf(t, ": %f", ra_value_get_double(vh));
					break;
				default:
					strcpy(t, "not supported type");
					break;
				}
				strcat(out, t);
			}
			else
			{
				strcpy(t, ": ---");
				strcat(out, t);
			}

			_results += out;
			_results += "\n";
		}
	}
	ra_value_free(vh);

	UpdateData(FALSE);
}

void simple_calc_dlg_mfc::OnExit() 
{
	if (_pi_for_pl)
	{
		for (int i = 0; i < _num_proc_pl; i++)
		{
			ra_proc_free(_pi_for_pl[i]);
		}
		free(_pi_for_pl);
	}

	OnOK();
}

void simple_calc_dlg_mfc::OnCopyClipboard() 
{
	UpdateData();

	if (!OpenClipboard())
	{
		AfxMessageBox("Cannot open the Clipboard");
		return;
	}
	// Remove the current Clipboard contents
	if(!EmptyClipboard())
	{
		AfxMessageBox("Cannot empty the Clipboard");
		return;
	}

	HGLOBAL data = GlobalAlloc(GHND, _results.GetLength() + 1);
	LPVOID locked_mem = GlobalLock(data);
	memcpy(locked_mem, (const char *)_results, _results.GetLength());
	GlobalUnlock(data);

	if (::SetClipboardData(CF_TEXT, data) == NULL)
	{
		AfxMessageBox("Unable to set Clipboard data");
		CloseClipboard();
		return;
	}

	CloseClipboard();
}
