#if !defined(AFX_PROCESS_DLG_H__E372B125_6380_45D9_923E_D6F19DB5043D__INCLUDED_)
#define AFX_PROCESS_DLG_H__E372B125_6380_45D9_923E_D6F19DB5043D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// process_dlg.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld process_dlg 

class process_dlg : public CDialog
{
// Konstruktion
public:
	process_dlg(CWnd* pParent = NULL);   // Standardkonstruktor

	void set_text(CString s);
	void set_progress(int percent);

// Dialogfelddaten
	//{{AFX_DATA(process_dlg)
	enum { IDD = IDD_PROGRESS_DLG };
	CProgressCtrl	_progress;
	CString	_percent;
	CString	_text;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(process_dlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(process_dlg)
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_PROCESS_DLG_H__E372B125_6380_45D9_923E_D6F19DB5043D__INCLUDED_
