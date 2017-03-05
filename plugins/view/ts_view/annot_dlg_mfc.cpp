// annot_dlg_mfc.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "annot_dlg_mfc.h"


/////////////////////////////////////////////////////////////////////////////
// Dialogfeld annot_dlg_mfc 


void annot_dlg_mfc::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(annot_dlg_mfc)
	DDX_Control(pDX, IDC_ANNOT_LIST, _annot_list);
	DDX_Control(pDX, IDC_CH_LIST, _ch_list);
	DDX_Text(pDX, IDC_ANNOT, _annot);
	DDX_Check(pDX, IDC_IGNORE, _ignore);
	DDX_Check(pDX, IDC_NOISE, _noise);
	DDX_Text(pDX, IDC_TXT_ANNOT, _txt_annot);
	DDX_Text(pDX, IDC_TXT_ANNOT_FOR_CH, _txt_annot_for_ch);
	DDX_Text(pDX, IDC_TXT_PRE_DEF_ANNOT, _txt_pre_def_annot);
	DDX_Text(pDX, IDC_TXT_INDICATORS, _txt_indicators);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(annot_dlg_mfc, CDialog)
	//{{AFX_MSG_MAP(annot_dlg_mfc)
	ON_BN_CLICKED(IDC_DEL_BTN, OnDelBtn)
	ON_CBN_SELCHANGE(IDC_ANNOT_LIST, OnSelchangeAnnotList)
	ON_CBN_SELCHANGE(IDC_CH_LIST, OnSelchangeChList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten annot_dlg_mfc 

void
annot_dlg_mfc::set_opt(struct ch_info *ch, int num_ch, int curr_ch, char **predefs, int num_predefs,
			   char *text/*=NULL*/, bool noise_flag/*=false*/, bool ignore_flag/*=false*/, int ch_sel/*=-1*/)
{
	//{{AFX_DATA_INIT(annot_dlg_mfc)
	_annot = _T("");
	_ignore = FALSE;
	_noise = FALSE;
	_txt_annot = _T("");
	_txt_annot_for_ch = _T("");
	_txt_pre_def_annot = _T("");
	_txt_indicators = _T("");
	//}}AFX_DATA_INIT

	_del = false;
	_new_annot = false;
	_ch_sel = -1;

	_ch = ch;
	_num_ch = num_ch;
	_curr_ch = curr_ch;
	_predefs = predefs;
	_num_predefs = num_predefs;

	if (text)
	{
		_annot = _T(text);
		_ignore = ignore_flag;
		_noise = noise_flag;
		_ch_sel = ch_sel;
	}
	else
		_new_annot = true;
} // set_opt()


BOOL
annot_dlg_mfc::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// do l10n stuff
	this->SetWindowText(gettext("Edit Annotations"));
	_txt_annot_for_ch = gettext("Annotation for Channel");
	_txt_indicators = gettext("Indicators");
	GetDlgItem(IDC_NOISE)->SetWindowText(gettext("Noise"));
	GetDlgItem(IDC_IGNORE)->SetWindowText(gettext("Ignore"));
	_txt_pre_def_annot = gettext("pre-defined Annotations");
	_txt_annot = gettext("Annotation");
	GetDlgItem(IDC_DEL_BTN)->SetWindowText(gettext("Delete"));
	UpdateData(FALSE);

	_ch_list.AddString("all channels");
	int i = 0;
	for (i = 0; i < _num_ch; i++)
		_ch_list.AddString(_ch[i].name);
	if (_ch_sel == -1)
		_ch_list.SetCurSel(0);
	else
	{
		for (i = 0; i < _num_ch; i++)
		{
			if (_ch_sel == _ch[i].ch_number)
			{
				_ch_list.SetCurSel(i+1);
				break;
			}
		}
	}

	for (i = 0; i < _num_predefs; i++)
		_annot_list.AddString(_predefs[i]);
	if (_num_predefs > 0)
		_annot_list.SetCurSel(0);

	if (_new_annot)
		GetDlgItem(IDC_DEL_BTN)->EnableWindow(FALSE);
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
} // OnInitDialog()


void
annot_dlg_mfc::OnDelBtn() 
{
	_del = true;
	OnOK();
} // OnDelBtn()


void
annot_dlg_mfc::OnSelchangeAnnotList() 
{
	int idx = _annot_list.GetCurSel();
	if (idx == LB_ERR)
		return;

	UpdateData();

	_annot_list.GetLBText(idx, _annot);

	UpdateData(FALSE);
} // OnSelchangeAnnotList()


void
annot_dlg_mfc::OnSelchangeChList() 
{
	int idx = _ch_list.GetCurSel();
	if ((idx == LB_ERR) || (idx > _num_ch))
		return;

	if (idx == 0)
		_ch_sel = -1;
	else
		_ch_sel = _ch[idx-1].ch_number;
} // OnSelchangeChList();
