#if !defined(AFX_RA_FILEDLG_MFC_H__FEE0E1F6_4CF8_44F4_B0DA_89A4735A3016__INCLUDED_)
#define AFX_RA_FILEDLG_MFC_H__FEE0E1F6_4CF8_44F4_B0DA_89A4735A3016__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ra_filedlg_mfc.h : Header-Datei
//

extern "C"
{
#include <ra.h>
}



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
// Dialogfeld ra_filedlg 

class AFX_EXT_CLASS ra_filedlg:public CDialog
{
// Konstruktion
      public:
	ra_filedlg(ra_handle ra, CWnd * pParent = NULL);	// Standardkonstruktor

	CString get_signal();
	void set_savepath(CString path);
	CString get_savepath();

// Dialogfelddaten
	//{{AFX_DATA(ra_filedlg)
	enum
	{ IDD = IDD_SEL_SIG_DLG };
	CTreeCtrl _dir;
	CListCtrl _signals;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(ra_filedlg)
      protected:
	  virtual void DoDataExchange(CDataExchange * pDX);	// DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
      protected:
	  ra_handle _ra;
	CString _sig_sel;
	CString _path;

	bool fill_dir(LPSHELLFOLDER shell_folder, LPITEMIDLIST item_parent, HTREEITEM parent);
	bool GetName(LPSHELLFOLDER shell, LPITEMIDLIST lpi, DWORD flags, LPSTR name, int name_len);
	void GetNormalAndSelectedIcons(LPITEMIDLIST lpifq, LPTV_ITEM lptvitem);
	int GetIcon(LPITEMIDLIST lpi, UINT uFlags);
	UINT GetSize(LPCITEMIDLIST pidl);
	LPITEMIDLIST Next(LPCITEMIDLIST pidl);
	LPITEMIDLIST Create(UINT cbSize);
	LPITEMIDLIST ConcatPidls(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2);
	LPITEMIDLIST CopyITEMID(LPMALLOC lpMalloc, LPITEMIDLIST lpi);

	void insert_signal(char *name, int cnt);
	void delete_sig_elements();


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
  DECLARE_MESSAGE_MAP()};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_RA_FILEDLG_MFC_H__FEE0E1F6_4CF8_44F4_B0DA_89A4735A3016__INCLUDED_
