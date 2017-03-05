// raschlab_mfc.cpp : Legt das Klassenverhalten für die Anwendung fest.
//

#include "stdafx.h"
#include "raschlab_mfc.h"

#include "MainFrm.h"
#include "ChildFrm.h"

#include "..\\general\\defines.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRASCHlab_mfcApp

BEGIN_MESSAGE_MAP(CRASCHlab_mfcApp, CWinApp)
	//{{AFX_MSG_MAP(CRASCHlab_mfcApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//}}AFX_MSG_MAP
	// Dateibasierte Standard-Dokumentbefehle
	// Standard-Druckbefehl "Seite einrichten"
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMtklab_mfcApp Konstruktion

CRASCHlab_mfcApp::CRASCHlab_mfcApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// Das einzige CRASCHlab_mfcApp-Objekt

CRASCHlab_mfcApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CMtklab_mfcApp Initialisierung

BOOL CRASCHlab_mfcApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standardinitialisierung
	// Wenn Sie diese Funktionen nicht nutzen und die Größe Ihrer fertigen 
	//  ausführbaren Datei reduzieren wollen, sollten Sie die nachfolgenden
	//  spezifischen Initialisierungsroutinen, die Sie nicht benötigen, entfernen.

#ifdef _AFXDLL
	Enable3dControls();			// Diese Funktion bei Verwendung von MFC in gemeinsam genutzten DLLs aufrufen
#else
	Enable3dControlsStatic();	// Diese Funktion bei statischen MFC-Anbindungen aufrufen
#endif

	// Ändern des Registrierungsschlüssels, unter dem unsere Einstellungen gespeichert sind.
	// ZU ERLEDIGEN: Sie sollten dieser Zeichenfolge einen geeigneten Inhalt geben
	// wie z.B. den Namen Ihrer Firma oder Organisation.
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	LoadStdProfileSettings();  // Standard INI-Dateioptionen laden (einschließlich MRU)

	_settings.read();

	// init libRASCH (will be needed by CMainFrame)
	_ra = ra_lib_init();
	if (!_ra)
	{
		MessageBox(NULL, "Can't init libRASCH. Perhaps configuration-file is wrong.",
					"RASCHlab", MB_ICONSTOP | MB_OK);
		return FALSE;
	}

	// Haupt-MDI-Rahmenfenster erzeugen
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

	// Befehlszeile parsen, um zu prüfen auf Standard-Umgebungsbefehle DDE, Datei offen
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Verteilung der in der Befehlszeile angegebenen Befehle
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// Das Hauptfenster ist initialisiert und kann jetzt angezeigt und aktualisiert werden.
	char t[200];
	sprintf(t, "RASCHlab-MFC %s", RASCHLAB_VERSION);
	m_pMainWnd->SetWindowText(t);

	if (_settings.is_maximized())
		pMainFrame->ShowWindow(SW_SHOWMAXIMIZED);
	else
	{
		if (_settings.pos_ok() && _settings.size_ok())
		{
			pMainFrame->MoveWindow(_settings.pos_x(), _settings.pos_y(),
					_settings.size_w(), _settings.size_h());
		}

		pMainFrame->ShowWindow(m_nCmdShow);
	}
	pMainFrame->UpdateWindow();

	return TRUE;
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
	CString	_prog_name;
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
	_prog_name = _T("");
	//}}AFX_DATA_INIT

	value_handle vh = ra_value_malloc();
	ra_handle ra = ((CRASCHlab_mfcApp *)AfxGetApp())->get_ra_handle();
	ra_info_get(ra, RA_INFO_VERSION_C, vh);

	char t[300];
	sprintf(t, gettext("RASCHlab-MFC 0.2.4\n(build: %s  %s)\nusing libRASCH Version %s"), __DATE__, __TIME__, ra_value_get_string(vh));
	_prog_name = t;

	ra_value_free(vh);
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Text(pDX, IDC_PROGNAME, _prog_name);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// Keine Nachrichten-Handler
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// Anwendungsbefehl zum Ausführen des Dialogfelds
void CRASCHlab_mfcApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CRASCHlab_mfcApp-Nachrichtenbehandlungsroutinen


CDocument* CRASCHlab_mfcApp::OpenDocumentFile(LPCTSTR lpszFileName) 
{
	((CMainFrame *)m_pMainWnd)->open_sig(lpszFileName);	
	
	return (CDocument*)1;  // so the file will not be deleted from LRU-list
}
