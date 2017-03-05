// show_sig_mfc.h : Haupt-Header-Datei für die Anwendung SHOW_SIG_MFC
//

#if !defined(AFX_SHOW_SIG_MFC_H__1C639CA0_D2F0_4203_B864_35A0C71121EB__INCLUDED_)
#define AFX_SHOW_SIG_MFC_H__1C639CA0_D2F0_4203_B864_35A0C71121EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // Hauptsymbole

// EXAMPLE-CODE (start)
extern "C" {
#include <ra.h>
}
// EXAMPLE-CODE (end)

/////////////////////////////////////////////////////////////////////////////
// CShow_sig_mfcApp:
// Siehe show_sig_mfc.cpp für die Implementierung dieser Klasse
//

class CShow_sig_mfcApp : public CWinApp
{
public:
	CShow_sig_mfcApp();

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CShow_sig_mfcApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementierung
protected:
	HMENU m_hMDIMenu;
	HACCEL m_hMDIAccel;

public:
	// EXAMPLE-CODE (start)
	ra_handle _ra;
	meas_handle _meas;
	// EXAMPLE-CODE (end)

	//{{AFX_MSG(CShow_sig_mfcApp)
	afx_msg void OnAppAbout();
	afx_msg void OnFileNew();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_SHOW_SIG_MFC_H__1C639CA0_D2F0_4203_B864_35A0C71121EB__INCLUDED_)
