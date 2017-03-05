/*----------------------------------------------------------------------------
 * plugin_info_dlg_mfc.c
 *
 * 
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2003, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Header: /home/cvs_repos.bak/librasch/plugins/view/plugin_info_dlg/plugin_info_dlg_mfc.cpp,v 1.5 2004/06/30 16:44:26 rasch Exp $
 *--------------------------------------------------------------------------*/

// plugin_info_dlg_mfc.cpp : Legt die Initialisierungsroutinen für die DLL fest.
//

#include "stdafx.h"
#include "plugin_info_dlg_mfc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// Dialogfeld ch_sel_dlg_mfc 


plugin_info_dlg_mfc::plugin_info_dlg_mfc(struct proc_info *pi, CWnd * pParent /*=NULL*/ )
	: CDialog(plugin_info_dlg_mfc::IDD, pParent)
{
	//{{AFX_DATA_INIT(plugin_info_dlg_mfc)
	_build_time_stamp = _T("");
	_plugin_desc = _T("");
	_plugin_path = _T("");
	_use_plugin = FALSE;
	_txt_loaded_plugins = _T("");
	_txt_path = _T("");
	_txt_description = _T("");
	//}}AFX_DATA_INIT
	_pi = pi;
}


void
plugin_info_dlg_mfc::DoDataExchange(CDataExchange * pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(plugin_info_dlg_mfc)
	DDX_Control(pDX, IDC_PLUGIN_LIST, _plugin_list);
	DDX_Text(pDX, IDC_BUILD_TIMESTAMP, _build_time_stamp);
	DDX_Text(pDX, IDC_PLUGIN_DESC, _plugin_desc);
	DDX_Text(pDX, IDC_PLUGIN_PATH, _plugin_path);
	DDX_Check(pDX, IDC_USE_PLUGIN, _use_plugin);
	DDX_Text(pDX, IDC_TXT_LOADED_PLUGINS, _txt_loaded_plugins);
	DDX_Text(pDX, IDC_TXT_PATH, _txt_path);
	DDX_Text(pDX, IDC_TXT_DESCRIPTION, _txt_description);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(plugin_info_dlg_mfc, CDialog)
	//{{AFX_MSG_MAP(plugin_info_dlg_mfc)
	ON_BN_CLICKED(IDC_USE_PLUGIN, use_plugin)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_PLUGIN_LIST, Itemchanged_plugin_list)
	//}}AFX_MSG_MAP
	END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten plugin_info_dlg_mfc 

BOOL
plugin_info_dlg_mfc::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// do l10n of dialog strings
	_txt_loaded_plugins = gettext("loaded plugins");
	_txt_description = gettext("Description:");
	_txt_path = gettext("path");
	GetDlgItem(IDC_USE_PLUGIN)->SetWindowText(gettext("use plugin"));

	UpdateData(FALSE);

	_ra = ra_lib_handle_from_any_handle(_pi->plugin);

	TCHAR rgtsz[3][20] = {"Name", "Type", "License"};
	int size[3] = {180, 70, 80};

	LV_COLUMN col;
	for (int i = 0; i < 3; i++)  // add the columns to the list control
	{
		col.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
		col.fmt = LVCFMT_LEFT;
		col.pszText = gettext(rgtsz[i]);
		col.iSubItem = i;
		col.cx = size[i];
		_plugin_list.InsertColumn(i, &col);  // assumes return value is OK.
	}

	// get all plugin-infos and put names in list
	_plugin_list.DeleteAllItems();
	value_handle vh = ra_value_malloc();
	ra_info_get(_ra, RA_INFO_NUM_PLUGINS_L, vh);
	long n = ra_value_get_long(vh);
	for (long l = 0; l < n; l++)
	{
		plugin_handle pl = ra_plugin_get_by_num(_ra, l, 1);


		// will now insert the items and subitems into the list view.
		LV_ITEM item;
		CString elem[3];
		ra_info_get(pl, RA_INFO_PL_NAME_C, vh);
		elem[0] = ra_value_get_string(vh);

		ra_info_get(pl, RA_INFO_PL_TYPE_L, vh);
		elem[1] = "";
		switch (ra_value_get_long(vh))
		{
		case PLUGIN_ACCESS:
			elem[1] += gettext("access ");
			break;
		case PLUGIN_PROCESS:
			elem[1] += gettext("process ");
			break;
		case PLUGIN_GUI:
			elem[1] += gettext("GUI ");
			break;
		case PLUGIN_VIEW:
			elem[1] += gettext("view ");
			break;
		default:
			elem[1] = "???";
			break;
		}
	
		ra_info_get(pl, RA_INFO_PL_LICENSE_L, vh);
		switch (ra_value_get_long(vh))
		{
		case LICENSE_LGPL:
			elem[2] = "LGPL";
			break;
		case LICENSE_PROPRIETARY:
			elem[2] = gettext("proprietary");
			break;
		case LICENSE_NOT_CHOOSEN:
			elem[2] = gettext("not choosen");
			break;
		default:
			elem[2] = "???";
			break;
		}
		
		int curr_item;
		for (int sub_item = 0; sub_item < 3; sub_item++)
		{
			if (sub_item == 0)
				item.mask = LVIF_TEXT | LVIF_PARAM;
			else
				item.mask = LVIF_TEXT;
			item.iItem = (sub_item == 0) ? (n-1) : curr_item;
			item.iSubItem = sub_item;
			char t[100];
			strncpy(t, (const char *)elem[sub_item], 99);
			t[99] = '\0';
			item.pszText = t;
			item.lParam = (LPARAM)l;

/*			if ((l == 0) && (sub_item == 0))
				item.state = LVIS_SELECTED; */

			if (sub_item == 0)
				curr_item = _plugin_list.InsertItem(&item);
			else
				_plugin_list.SetItem(&item);

/*			if ((l == 0) && (sub_item == 0))
			{
				long res;
				Itemchanged_plugin_list((NMHDR *)&item, &res);
			}*/
		}
	}
	ra_value_free(vh);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}  // OnInitDialog()


void
plugin_info_dlg_mfc::use_plugin() 
{
	int idx = _plugin_list.GetNextItem(-1, LVNI_SELECTED);
	if (idx < 0)
		return;

	UpdateData();

	long pl_num = _plugin_list.GetItemData(idx);
	ra_lib_use_plugin(_ra, pl_num, _use_plugin);
}  // use_plugin()


void
plugin_info_dlg_mfc::Itemchanged_plugin_list(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	plugin_handle pl = ra_plugin_get_by_num(_ra, pNMListView->lParam, 1);

	value_handle vh = ra_value_malloc();
	ra_info_get(pl, RA_INFO_PL_NAME_C, vh);
	CString s = ra_value_get_string(vh);
	ra_info_get(pl, RA_INFO_PL_VERSION_C, vh);
	s += " (Version ";
	s += ra_value_get_string(vh);
	s += ")";
	GetDlgItem(IDC_PLUGIN)->SetWindowText(s);

	ra_info_get(pl, RA_INFO_PL_DESC_C, vh);
	_plugin_desc = gettext(ra_value_get_string(vh));

	ra_info_get(pl, RA_INFO_PL_USE_IT_L, vh);
	if (ra_value_get_long(vh))
		_use_plugin = TRUE;
	else
		_use_plugin = FALSE;

	ra_info_get(pl, RA_INFO_PL_FILE_C, vh);
	_plugin_path = ra_value_get_string(vh);

	ra_info_get(pl, RA_INFO_PL_BUILD_TS_C, vh);
	s = gettext("plugin-build: ");
	s += ra_value_get_string(vh);
	_build_time_stamp = s;

	ra_info_get(pl, RA_INFO_PL_TYPE_L, vh);
	
	ra_info_get(pl, RA_INFO_PL_LICENSE_L, vh);

	ra_value_free(vh);

	UpdateData(FALSE);

	*pResult = 0;
}  // Itemchanged_plugin_list()
