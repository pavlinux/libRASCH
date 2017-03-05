#if !defined(AFX_EVPROP_DLG_MFC_H__34BB55B6_AAA9_4523_B563_8E03EA395931__INCLUDED_)
#define AFX_EVPROP_DLG_MFC_H__34BB55B6_AAA9_4523_B563_8E03EA395931__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// evprop_dlg_mfc.h : Header-Datei
//

#define _LIBRASCH_BUILD
#include <ra.h>

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld evprop_dlg_mfc 

class evprop_dlg_mfc : public CDialog
{
// Konstruktion
public:
	evprop_dlg_mfc() { }
	evprop_dlg_mfc(meas_handle mh, eval_handle eh, CWnd* pParent = NULL);   // Standardkonstruktor

	void set_info(meas_handle mh, eval_handle eh)
	{
		_mh = mh;
		_eh = eh;

		_class_all = NULL;
		_num_class = 0;
		_curr_class = -1;

		_prop_all = NULL;
		_num_prop = 0;
		_curr_prop = -1;
	}

// Dialogfelddaten
	//{{AFX_DATA(evprop_dlg_mfc)
	enum { IDD = IDD_EVPROP_DLG };
	CListBox	_class_list;
	CButton	_del_prop;
	CButton	_del_class;
	CListBox	_prop_list;
	CButton	_group;
	CString	_desc_field;
	CString	_occurence;
	CString	_value;
	CString	_txt_description;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(evprop_dlg_mfc)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	meas_handle _mh;
	eval_handle _eh;

	int _num_class;
	class_handle *_class_all;
	int _curr_class;

	int _num_prop;
	prop_handle *_prop_all;
	int _curr_prop;

	void init();

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(evprop_dlg_mfc)
	afx_msg void OnSelchangePropList();
	virtual BOOL OnInitDialog();
	afx_msg void OnDelClassBtn();
	afx_msg void OnDelPropBtn();
	afx_msg void OnSelchangeClassList();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_EVPROP_DLG_MFC_H__34BB55B6_AAA9_4523_B563_8E03EA395931__INCLUDED_
