#if !defined(AFX_ANNOT_DLG_MFC_H__8A7E75BC_9815_4D3E_A12F_84B621E7CB38__INCLUDED_)
#define AFX_ANNOT_DLG_MFC_H__8A7E75BC_9815_4D3E_A12F_84B621E7CB38__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// annot_dlg_mfc.h : Header-Datei
//

#include "resource.h"
#include "ts_view_general.h"

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld annot_dlg_mfc 

class annot_dlg_mfc : public CDialog
{
// Konstruktion
public:
	annot_dlg_mfc() { }
//	annot_dlg_mfc(CWnd* pParent = NULL);   // Standardkonstruktor

	void set_opt(struct ch_info *ch, int num_ch, int curr_ch, char **predefs, int num_predefs,
			   char *text = NULL, bool noise_flag = false, bool ignore_flag = false, int ch_sel = -1);

	BOOL do_ignore() { return _ignore; }
	BOOL is_noise() { return _noise; }
	bool del_annot() { return _del; }
	CString annot() { return _annot; }
	int channel() { return _ch_sel; }

// Dialogfelddaten
	//{{AFX_DATA(annot_dlg_mfc)
	enum { IDD = IDD_ANNOT_DLG };
	CComboBox	_annot_list;
	CComboBox	_ch_list;
	CString	_annot;
	BOOL	_ignore;
	BOOL	_noise;
	CString	_txt_annot;
	CString	_txt_annot_for_ch;
	CString	_txt_pre_def_annot;
	CString	_txt_indicators;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(annot_dlg_mfc)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	bool _del;
	bool _new_annot;
	struct ch_info *_ch;
	int _num_ch;
	int _curr_ch;
	char **_predefs;
	int _num_predefs;
	int _ch_sel;

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(annot_dlg_mfc)
	virtual BOOL OnInitDialog();
	afx_msg void OnDelBtn();
	afx_msg void OnSelchangeAnnotList();
	afx_msg void OnSelchangeChList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_ANNOT_DLG_MFC_H__8A7E75BC_9815_4D3E_A12F_84B621E7CB38__INCLUDED_
