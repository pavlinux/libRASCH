// ralab_mfc.h : Haupt-Header-Datei für die Anwendung RALAB_MFC
//

#if !defined(AFX_RASCHLAB_MFC_H__955353C4_D109_4A7D_913E_25EB48991D3F__INCLUDED_)
#define AFX_RASCHLAB_MFC_H__955353C4_D109_4A7D_913E_25EB48991D3F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // Hauptsymbole

extern "C" {
#include <libintl.h>
#include <ra.h>
#include <ra_pl_comm.h>
}

#include"..\\general\\settings.h"


/////////////////////////////////////////////////////////////////////////////
// CRASCHlab_mfcApp:
// Siehe raschlab_mfc.cpp für die Implementierung dieser Klasse
//

class CRASCHlab_mfcApp : public CWinApp
{
public:
	CRASCHlab_mfcApp();

	ra_handle get_ra_handle() { return _ra; }

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CRASCHlab_mfcApp)
	public:
	virtual BOOL InitInstance();
	virtual CDocument* OpenDocumentFile(LPCTSTR lpszFileName);
	//}}AFX_VIRTUAL

// Implementierung
	//{{AFX_MSG(CRASCHlab_mfcApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	settings _settings;

protected:
	ra_handle _ra;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_RASCHLAB_MFC_H__955353C4_D109_4A7D_913E_25EB48991D3F__INCLUDED_)
