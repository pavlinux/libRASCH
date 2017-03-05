// show_sig_mfc.cpp : Legt das Klassenverhalten für die Anwendung fest.
//

#include "stdafx.h"
#include "show_sig_mfc.h"

#include "MainFrm.h"
#include "ChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CShow_sig_mfcApp

BEGIN_MESSAGE_MAP(CShow_sig_mfcApp, CWinApp)
	//{{AFX_MSG_MAP(CShow_sig_mfcApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShow_sig_mfcApp Konstruktion

CShow_sig_mfcApp::CShow_sig_mfcApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// Das einzige CShow_sig_mfcApp-Objekt

CShow_sig_mfcApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CShow_sig_mfcApp Initialisierung

BOOL CShow_sig_mfcApp::InitInstance()
{
	// Standardinitialisierung

#ifdef _AFXDLL
	Enable3dControls();			// Diese Funktion bei Verwendung von MFC in gemeinsam genutzten DLLs aufrufen
#else
	Enable3dControlsStatic();	// Diese Funktion bei statischen MFC-Anbindungen aufrufen
#endif

	// Ändern des Registrierungsschlüssels, unter dem unsere Einstellungen gespeichert sind.
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	// EXAMPLE-CODE (start)
	// needs to be done before frame window is created
	// because view is created when frame window is created
	_ra = ra_lib_init();
	_meas = ra_meas_open(_ra, m_lpCmdLine, 0);
	// EXAMPLE-CODE (end)

	CMDIFrameWnd* pFrame = new CMainFrame;
	m_pMainWnd = pFrame;

	// Haupt-MDI-Rahmenfenster erstellen
	if (!pFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;

	// Gemeinsam genutzte MDI-Menüs und Zugriffstastentabelle laden

	HINSTANCE hInst = AfxGetResourceHandle();
	m_hMDIMenu  = ::LoadMenu(hInst, MAKEINTRESOURCE(IDR_SHOW_STYPE));
	m_hMDIAccel = ::LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_SHOW_STYPE));

	pFrame->ShowWindow(m_nCmdShow);
	pFrame->UpdateWindow();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CShow_sig_mfcApp Nachrichten-Handler

int CShow_sig_mfcApp::ExitInstance() 
{
	if (m_hMDIMenu != NULL)
		FreeResource(m_hMDIMenu);
	if (m_hMDIAccel != NULL)
		FreeResource(m_hMDIAccel);

	return CWinApp::ExitInstance();
}

void CShow_sig_mfcApp::OnFileNew() 
{
	CMainFrame* pFrame = STATIC_DOWNCAST(CMainFrame, m_pMainWnd);

	// Neues untergeordnetes MDI-Fenster erstellen
	pFrame->CreateNewChild(
		RUNTIME_CLASS(CChildFrame), IDR_SHOW_STYPE, m_hMDIMenu, m_hMDIAccel);
}



/////////////////////////////////////////////////////////////////////////////
// CAboutDlg-Dialog für Info über Anwendung

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialogdaten
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// Überladungen für virtuelle Funktionen, die vom Anwendungs-Assistenten erzeugt wurden
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	//{{AFX_MSG(CAboutDlg)
		// Keine Nachrichten-Handler
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// Keine Nachrichten-Handler
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// Anwendungsbefehl zum Ausführen des Dialogfelds
void CShow_sig_mfcApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CShow_sig_mfcApp-Nachrichtenbehandlungsroutinen
