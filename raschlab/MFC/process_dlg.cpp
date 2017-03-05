// process_dlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "raschlab_mfc.h"
#include "process_dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld process_dlg 


process_dlg::process_dlg(CWnd* pParent /*=NULL*/)
	: CDialog(process_dlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(process_dlg)
	_percent = _T("");
	_text = _T("");
	//}}AFX_DATA_INIT
}


void process_dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(process_dlg)
	DDX_Control(pDX, IDC_PROGRESS, _progress);
	DDX_Text(pDX, IDC_PERCENT, _percent);
	DDX_Text(pDX, IDC_TEXT, _text);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(process_dlg, CDialog)
	//{{AFX_MSG_MAP(process_dlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten process_dlg 

BOOL process_dlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	_progress.SetRange(0, 100);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}


void
process_dlg::set_text(CString s)
{
	_text = s;
	UpdateData(FALSE);
}  // set_text()


void
process_dlg::set_progress(int percent)
{
	char t[10];
	sprintf(t, "%d%%", percent);
	_percent = t;
	UpdateData(FALSE);
	_progress.SetPos(percent);
}  // set_progress()


void process_dlg::OnCancel() 
{
	// TODO: Zusätzlichen Bereinigungscode hier einfügen
	
	CDialog::OnCancel();
}

