/*----------------------------------------------------------------------------
 * evprop_dlg_mfc.cpp  -  
 *
 * Description:
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
#include "evprop_dlg_mfc.h"

/*
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
*/
/////////////////////////////////////////////////////////////////////////////
// Dialogfeld evprop_dlg_mfc 


evprop_dlg_mfc::evprop_dlg_mfc(meas_handle mh, eval_handle eh, CWnd* pParent /*=NULL*/)
	: CDialog(evprop_dlg_mfc::IDD, pParent)
{
	//{{AFX_DATA_INIT(evprop_dlg_mfc)
	_desc_field = _T("");
	_occurence = _T("");
	_value = _T("");
	_txt_description = _T("");
	//}}AFX_DATA_INIT

	_mh = mh;
	_eh = eh;

	_class_all = NULL;
	_num_class = 0;
	_curr_class = -1;

	_prop_all = NULL;
	_num_prop = 0;
	_curr_prop = -1;
}


void evprop_dlg_mfc::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(evprop_dlg_mfc)
	DDX_Control(pDX, IDC_CLASS_LIST, _class_list);
	DDX_Control(pDX, IDC_DEL_PROP_BTN, _del_prop);
	DDX_Control(pDX, IDC_DEL_CLASS_BTN, _del_class);
	DDX_Control(pDX, IDC_PROP_LIST, _prop_list);
	DDX_Control(pDX, IDC_GROUP, _group);
	DDX_Text(pDX, IDC_DESC_FIELD, _desc_field);
	DDX_Text(pDX, IDC_OCCURENCE, _occurence);
	DDX_Text(pDX, IDC_VALUE, _value);
	DDX_Text(pDX, IDC_TXT_DESCRIPTION, _txt_description);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(evprop_dlg_mfc, CDialog)
	//{{AFX_MSG_MAP(evprop_dlg_mfc)
	ON_LBN_SELCHANGE(IDC_PROP_LIST, OnSelchangePropList)
	ON_BN_CLICKED(IDC_DEL_CLASS_BTN, OnDelClassBtn)
	ON_BN_CLICKED(IDC_DEL_PROP_BTN, OnDelPropBtn)
	ON_LBN_SELCHANGE(IDC_CLASS_LIST, OnSelchangeClassList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten evprop_dlg_mfc 

BOOL
evprop_dlg_mfc::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// do l10n stuff
	this->SetWindowText(gettext("Event Properties"));
	GetDlgItem(IDC_DEL_CLASS_BTN)->SetWindowText(gettext("delete"));
	GetDlgItem(IDC_DEL_PROP_BTN)->SetWindowText(gettext("delete"));
	GetDlgItem(IDOK)->SetWindowText(gettext("Exit"));
	_txt_description = gettext("Description");
	UpdateData(FALSE);

	init();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}  // OnInitDialog()


void
evprop_dlg_mfc::init()
{
	_curr_prop = -1;

	_group.SetWindowText("");
	_desc_field = "";
	_value = gettext("value-type:");
	_occurence = gettext("occurence:");

	UpdateData(FALSE);

	if (_class_all)
	{
		delete[] _class_all;
		_class_all = NULL;
		_num_class = 0;
		_curr_class = -1;
	}
	if (_prop_all)
	{		
		delete[] _prop_all;
		_prop_all = NULL;
		_num_prop = 0;
		_curr_prop = -1;
	}

	value_handle vh = ra_value_malloc();
	if (ra_class_get(_eh, NULL, vh) != 0)
		return;
	_num_class = ra_value_get_num_elem(vh);
	if (_num_class <= 0)
	{
		ra_value_free(vh);
		return;
	}

	_class_all = new class_handle[_num_class];
	const void **p = ra_value_get_voidp_array(vh);
	value_handle vh2 = ra_value_malloc();
	for (int i = 0; i < _num_class; i++)
	{
		_class_all[i] = (class_handle)(p[i]);
		ra_info_get(_class_all[i], RA_INFO_CLASS_NAME_C, vh2);
		int idx = _class_list.AddString(ra_value_get_string(vh2));
		_class_list.SetItemData(idx, i);
	}

	_class_list.SetCurSel(0);
	OnSelchangeClassList();

	ra_value_free(vh);
	ra_value_free(vh2);
} // init()


void
evprop_dlg_mfc::OnSelchangeClassList() 
{
	_curr_class = -1;

	_group.SetWindowText("");
	_desc_field = "";
	_value = gettext("value-type:");
	_occurence = gettext("occurence:");

	_prop_list.ResetContent();

	UpdateData(FALSE);

	if (_prop_all)
	{		
		delete[] _prop_all;
		_prop_all = NULL;
		_num_prop = 0;
		_curr_prop = -1;
	}

	if (_class_list.GetCurSel() == LB_ERR)
		return;

	_curr_class = _class_list.GetItemData(_class_list.GetCurSel());

	value_handle vh = ra_value_malloc();
	if (ra_prop_get_all(_class_all[_curr_class], vh) != 0)
	{
		ra_value_free(vh);
		return;
	}

	_num_prop = ra_value_get_num_elem(vh);
	if (_num_prop <= 0)
	{
		ra_value_free(vh);
		return;
	}

	_prop_all = new prop_handle[_num_prop];
	const void **p = ra_value_get_voidp_array(vh);
	value_handle vh2 = ra_value_malloc();
	for (int i = 0; i < _num_prop; i++)
	{
		_prop_all[i] = (prop_handle)(p[i]);
		ra_info_get(_prop_all[i], RA_INFO_PROP_NAME_C, vh2);
		int idx = _prop_list.AddString(ra_value_get_string(vh2));
		_prop_list.SetItemData(idx, i);
	}

	_prop_list.SetCurSel(0);
	OnSelchangePropList();

	ra_value_free(vh);
	ra_value_free(vh2);
} // OnSelchangeClassList()


void
evprop_dlg_mfc::OnSelchangePropList() 
{
	_curr_prop = -1;

	_group.SetWindowText("");
	_desc_field = "";
	_value = gettext("value-type:");
	_occurence = gettext("occurence:");

	UpdateData(FALSE);

	if (_prop_list.GetCurSel() == LB_ERR)
		return;

	_curr_prop = _prop_list.GetItemData(_prop_list.GetCurSel());

	value_handle vh = ra_value_malloc();
	ra_info_get(_class_all[_curr_class], RA_INFO_CLASS_EV_NUM_L, vh);
	long num_events = ra_value_get_long(vh);

	ra_info_get(_prop_all[_curr_prop], RA_INFO_PROP_NAME_C, vh);
	char t[100];
	sprintf(t, gettext("%s (%ld events)"), ra_value_get_string(vh), num_events);
	_group.SetWindowText(t);

	ra_info_get(_prop_all[_curr_prop], RA_INFO_PROP_DESC_C, vh);
	_desc_field = ra_value_get_string(vh);

	ra_info_get(_prop_all[_curr_prop], RA_INFO_PROP_VALTYPE_L, vh);
	_value = gettext("value-type:");
	_value += " ";
	switch (ra_value_get_long(vh))
	{
	case RA_VALUE_TYPE_SHORT:
		_value += "short";
		break;
	case RA_VALUE_TYPE_LONG:
		_value += "long";
		break;
	case RA_VALUE_TYPE_DOUBLE:
		_value += "double";
		break;
	case RA_VALUE_TYPE_CHAR:
		_value += "string";
		break;
	case RA_VALUE_TYPE_VOIDP:
		_value += "handle";
		break;
	case RA_VALUE_TYPE_SHORT_ARRAY:
		_value += "short array";
		break;
	case RA_VALUE_TYPE_LONG_ARRAY:
		_value += "long array";
		break;
	case RA_VALUE_TYPE_DOUBLE_ARRAY:
		_value += "double array";
		break;
	case RA_VALUE_TYPE_CHAR_ARRAY:
		_value += "string array";
		break;
	case RA_VALUE_TYPE_VOIDP_ARRAY:
		_value += "handle array";
		break;
	case RA_VALUE_TYPE_VH_ARRAY:
		_value += "libRASCH-values array";
		break;
	default:
		_value += "---";
		break;
	}

	ra_value_free(vh);

	UpdateData(FALSE);
}  // OnSelchangePropList()


void
evprop_dlg_mfc::OnDelClassBtn() 
{
	if (_curr_class == -1)
		return;

	ra_class_delete(_class_all[_curr_class]);

	init();
} // OnDelClassBtn


void
evprop_dlg_mfc::OnDelPropBtn() 
{
	if (_curr_prop == -1)
		return;

	ra_prop_delete(_prop_all[_curr_prop]);

	init();
} // OnDelPropBtn()



void evprop_dlg_mfc::OnOK() 
{
	if (_class_all)
	{
		delete[] _class_all;
		_class_all = NULL;
		_num_class = 0;
		_curr_class = -1;
	}
	if (_prop_all)
	{		
		delete[] _prop_all;
		_prop_all = NULL;
		_num_prop = 0;
		_curr_prop = -1;
	}
	
	CDialog::OnOK();
}
