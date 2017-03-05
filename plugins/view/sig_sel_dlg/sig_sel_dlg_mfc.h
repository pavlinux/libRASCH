/*----------------------------------------------------------------------------
 * sig_sel_dlg_mfc.cpp
 *
 * 
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002-2006, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#if !defined(AFX_RA_FILEDLG_H__FEE0E1F6_4CF8_44F4_B0DA_89A4735A3016__INCLUDED_)
#define AFX_RA_FILEDLG_H__FEE0E1F6_4CF8_44F4_B0DA_89A4735A3016__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define _LIBRASCH_BUILD
#include <ra.h>
#include <ra_priv.h>
#include <pl_general.h>


#ifndef IDD_SEL_SIG_DLG
#define IDD_SEL_SIG_DLG  1000
#endif


typedef struct tagID
{
	LPSHELLFOLDER parent;
	LPITEMIDLIST item;
	LPITEMIDLIST item_fq;
}
TVITEMDATA, *LPTVITEMDATA;


/////////////////////////////////////////////////////////////////////////////
// Dialogfeld sig_sel_dlg_mfc 

class sig_sel_dlg_mfc:public CDialog
{
// Konstruktion
      public:
	sig_sel_dlg_mfc()
	{
	}
	sig_sel_dlg_mfc(ra_handle ra, CWnd * pParent = NULL);	// Standardkonstruktor

	void set_ra(ra_handle ra)
	{
		_ra = ra;
	}
	CString get_signal();
	void set_savepath(CString path);
	void select_path();

	CString get_savepath();

	void *get_path_data();
	void set_path_data(void *);

// Dialogfelddaten
	//{{AFX_DATA(sig_sel_dlg_mfc)
	enum
	{ IDD = IDD_SEL_SIG_DLG };
	CTreeCtrl _dir;
	CListCtrl _signals;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(sig_sel_dlg_mfc)
      protected:
	virtual void DoDataExchange(CDataExchange * pDX);	// DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
      protected:
	ra_handle _ra;
	CString _sig_sel;
	CString _path;
	LPITEMIDLIST _item_data;
	LPSHELLFOLDER _shell_folder;

	void process_dir(LPTVITEMDATA lptvid);
	bool fill_dir(LPSHELLFOLDER shell_folder, LPITEMIDLIST item_parent, HTREEITEM parent);
	bool GetName(LPSHELLFOLDER shell, LPITEMIDLIST lpi, DWORD flags, LPSTR name, int name_len);
	void GetNormalAndSelectedIcons(LPITEMIDLIST lpifq, LPTV_ITEM lptvitem);
	int GetIcon(LPITEMIDLIST lpi, UINT uFlags);
	UINT GetSize(LPCITEMIDLIST pidl);
	LPITEMIDLIST Next(LPCITEMIDLIST pidl);
	LPITEMIDLIST Create_item(UINT cbSize);
	LPITEMIDLIST ConcatPidls(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2);
	LPITEMIDLIST CopyITEMID(LPMALLOC lpMalloc, LPITEMIDLIST lpi);

	void insert_signal(char *name, int cnt);
	void delete_sig_elements();

	LPITEMIDLIST GetItemIDList(HTREEITEM hItem);
	HTREEITEM find_item_by_pidl(LPCITEMIDLIST pidl, HTREEITEM hParentItem = TVI_ROOT);

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(ra_filedlg)
	virtual void OnOK();
	afx_msg void OnSelchangedDirTree(NMHDR * pNMHDR, LRESULT * pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnItemexpandingDirTree(NMHDR * pNMHDR, LRESULT * pResult);
	afx_msg void OnDblclkSigList(NMHDR * pNMHDR, LRESULT * pResult);
	afx_msg void OnClickSigList(NMHDR * pNMHDR, LRESULT * pResult);
	virtual void OnCancel();
	//}}AFX_MSG
DECLARE_MESSAGE_MAP()};		// class sig_sel_dlg_mfc

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_RA_FILEDLG_H__FEE0E1F6_4CF8_44F4_B0DA_89A4735A3016__INCLUDED_
