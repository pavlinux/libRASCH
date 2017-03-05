/*----------------------------------------------------------------------------
 * sig_sel_dlg_mfc.cpp
 *
 * 
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002-2007, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#include "stdafx.h"
#include "Resource.h"
#include <direct.h>

#include "shell_pidl.h"
#include "shell_pidl_navigator.h"

#include "sig_sel_dlg_mfc.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld ra_filedlg 


sig_sel_dlg_mfc::sig_sel_dlg_mfc(ra_handle ra, CWnd * pParent /*=NULL*/ )
:	CDialog(sig_sel_dlg_mfc::IDD, pParent)
{
	//{{AFX_DATA_INIT(sig_sel_dlg_mfc)
	// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT

	_ra = ra;
	_item_data = NULL;
}


void
sig_sel_dlg_mfc::DoDataExchange(CDataExchange * pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(sig_sel_dlg_mfc)
	DDX_Control(pDX, IDC_DIR_TREE, _dir);
	DDX_Control(pDX, IDC_SIG_LIST, _signals);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(sig_sel_dlg_mfc, CDialog)
	//{{AFX_MSG_MAP(sig_sel_dlg_mfc)
	ON_NOTIFY(TVN_SELCHANGED, IDC_DIR_TREE, OnSelchangedDirTree)
ON_NOTIFY(TVN_ITEMEXPANDING, IDC_DIR_TREE, OnItemexpandingDirTree)
ON_NOTIFY(NM_DBLCLK, IDC_SIG_LIST, OnDblclkSigList)
ON_NOTIFY(NM_CLICK, IDC_SIG_LIST, OnClickSigList)
	//}}AFX_MSG_MAP
	END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten ra_filedlg 
BOOL
sig_sel_dlg_mfc::OnInitDialog()
{
	CDialog::OnInitDialog();

	// do l10n stuff
	this->SetWindowText(gettext("Select Signal"));
	UpdateData(FALSE);

	// Get the handle to the system image list, for our icons
	HIMAGELIST hImageList;
	SHFILEINFO sfi;

	hImageList = (HIMAGELIST) SHGetFileInfo((LPCSTR) "C:\\", 0, &sfi,
						sizeof(SHFILEINFO),
						SHGFI_SYSICONINDEX | SHGFI_SMALLICON);

	// Attach ImageList to TreeView
	if (hImageList)
		::SendMessage(_dir.m_hWnd, TVM_SETIMAGELIST,
			      (WPARAM) TVSIL_NORMAL, (LPARAM) hImageList);

	_dir.ModifyStyle(0, TVS_LINESATROOT);
	_dir.ModifyStyle(0, TVS_HASLINES);

	// Get a pointer to the desktop folder.
	LPSHELLFOLDER shell_folder = NULL;
	HRESULT hr = SHGetDesktopFolder(&shell_folder);
	if (FAILED(hr))
		return false;

	fill_dir(shell_folder, NULL, TVI_ROOT);

	TCHAR rgtsz[7][20] = {
		_T("Name"),
		_T("Birthday"),
		_T("id"),
		_T("rec.date"),
		_T("eval.date"), _T("signal"), _T("signal-type")
	};
	int npSize[7] = {
		100,
		70,
		40,
		70,
		70,
		70,
		130
	};

	LV_COLUMN lvColumn;
	for (int i = 0; i < 7; i++)	// add the columns to the list control
	{
		lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
		lvColumn.fmt = LVCFMT_LEFT;
		lvColumn.pszText = gettext(rgtsz[i]);
		lvColumn.iSubItem = i;
		lvColumn.cx = npSize[i];
		_signals.InsertColumn(i, &lvColumn);	// assumes return value is OK.
	}

	// disable OK
	GetDlgItem(IDOK)->EnableWindow(FALSE);

	// set selected signal back (if dlg called more than one)
	_sig_sel.Empty();

	// set focus to dir-tree
	select_path();
	GetDlgItem(IDC_DIR_TREE)->SetFocus();

	return FALSE;		// return TRUE unless you set the focus to a control
	// EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}


void
sig_sel_dlg_mfc::OnOK()
{
	HTREEITEM curr_dir = _dir.GetNextItem(TVI_ROOT, TVGN_CARET);
	if (curr_dir == NULL)
		_path.Empty();
	else
	{
		LPTVITEMDATA lptvid = (LPTVITEMDATA) _dir.GetItemData(curr_dir);
		_item_data = lptvid->item;
		char path[MAX_PATH_RA];
		SHGetPathFromIDList(lptvid->item, path);
		_path = path;
	}

	int curr_sig = _signals.GetNextItem(-1, LVNI_SELECTED);
	if (curr_sig == -1)
	{
		_sig_sel.Empty();
		return;		// not ok if no signal was selected
	}
	else
		_sig_sel = (char *) _signals.GetItemData(curr_sig);

	delete_sig_elements();

	CDialog::OnOK();
}


void *
sig_sel_dlg_mfc::get_path_data()
{
	return (void *)_item_data;
}  // get_path_data()


void
sig_sel_dlg_mfc::set_path_data(void *data)
{
	_item_data = (LPITEMIDLIST)data;
}  // set_path_data()


void
sig_sel_dlg_mfc::OnCancel()
{
	delete_sig_elements();

	CDialog::OnCancel();
}


CString sig_sel_dlg_mfc::get_signal()
{
	return _sig_sel;
}				// get_signal()


void
sig_sel_dlg_mfc::OnSelchangedDirTree(NMHDR * pNMHDR, LRESULT * pResult)
{
	NM_TREEVIEW *pNMTreeView = (NM_TREEVIEW *) pNMHDR;

	delete_sig_elements();

	*pResult = 0;

	LPTVITEMDATA lptvid = (LPTVITEMDATA) pNMTreeView->itemNew.lParam;
	if (lptvid)
		process_dir(lptvid);
}


void
sig_sel_dlg_mfc::process_dir(LPTVITEMDATA lptvid)
{
	if (!lptvid)
		return;

	HCURSOR wait_cursor = LoadCursor(NULL, IDC_WAIT);
	HCURSOR save_cursor = SetCursor(wait_cursor);

	static char path[MAX_PATH_RA];
	SHGetPathFromIDList(lptvid->item_fq, path);
	if (path[0] == '\0')
	{
		SetCursor(save_cursor);
		return;
	}

	static struct ra_find_struct info;
	ra_find_handle find = ra_meas_find_first(_ra, path, &info);
	if (!find)
	{
		SetCursor(save_cursor);
		return;
	}

	int cnt = 0;
	do
	{
		insert_signal(info.name, cnt);
		cnt++;

		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	while (ra_meas_find_next(find, &info));
	ra_meas_close_find(find);

	SetCursor(save_cursor);
} /* process_dir()*/


void
sig_sel_dlg_mfc::insert_signal(char *name, int cnt)
{
	HCURSOR wait_cursor = LoadCursor(NULL, IDC_WAIT);
	HCURSOR save_cursor = SetCursor(wait_cursor);

	meas_handle mh = ra_meas_open(_ra, name, NULL, true);
	if (!mh)
	{
		SetCursor(save_cursor);
		return;
	}

	value_handle vh = ra_value_malloc();
	char elem[7][MAX_PATH_RA];
	/* first get rec-name (if no obj-person-name avail. use rec-name) */
	rec_handle rh = ra_rec_get_first(mh, 0);
	if (ra_info_get(rh, RA_INFO_REC_GEN_NAME_C, vh) == 0)
		strcpy(elem[5], ra_value_get_string(vh));

	if (ra_info_get(mh, RA_INFO_OBJ_PERSON_NAME_C, vh) == 0)
		strcpy(elem[0], ra_value_get_string(vh));
	else
		strcpy(elem[0], elem[5]);

	if (elem[0][0] == '\0')
		strcpy(elem[0], elem[5]);

	if (ra_info_get(mh, RA_INFO_OBJ_PERSON_FORENAME_C, vh) == 0)
	{
		const char *t = ra_value_get_string(vh);
		if (t[0] != '\0')
		{
			strcat(elem[0], ", ");
			strcat(elem[0], t);
		}
	}

	if (ra_info_get(mh, RA_INFO_OBJ_PERSON_BIRTHDAY_C, vh) == 0)
		strcpy(elem[1], ra_value_get_string(vh));
	else
		elem[1][0] = '\0';

	if (ra_info_get(mh, RA_INFO_OBJ_PATIENT_ID_C, vh) == 0)
		strcpy(elem[2], ra_value_get_string(vh));
	else
		elem[2][0] = '\0';

	if (ra_info_get(rh, RA_INFO_REC_GEN_DATE_C, vh) == 0)
		strcpy(elem[3], ra_value_get_string(vh));
	else
		elem[3][0] = '\0';

	eval_handle eh = ra_eval_get_default(mh);
	if (ra_info_get(eh, RA_INFO_EVAL_ADD_TS_C, vh) == 0)
		strcpy(elem[4], ra_value_get_string(vh));
	else
		elem[4][0] = '\0';

	if (ra_info_get(rh, RA_INFO_REC_GEN_DESC_C, vh) == 0)
		strcpy(elem[6], ra_value_get_string(vh));
	else
		elem[6][0] = '\0';

	ra_meas_close(mh);
	ra_value_free(vh);

	LV_ITEM item;
	int curr;
	for (int sub = 0; sub < 7; sub++)
	{
		if (sub == 0)
			item.mask = LVIF_TEXT | LVIF_PARAM;
		else
			item.mask = LVIF_TEXT;
		item.iItem = (sub == 0) ? cnt : curr;
		item.iSubItem = sub;
		item.pszText = elem[sub];
		if (sub == 0)
		{
			char *fn = new char[MAX_PATH_RA];
			strcpy(fn, name);
			item.lParam = (LPARAM) fn;
		}

		if (sub == 0)
			curr = _signals.InsertItem(&item);
		else
			_signals.SetItem(&item);
	}

	SetCursor(save_cursor);
} // insert_signal()


bool sig_sel_dlg_mfc::fill_dir(LPSHELLFOLDER shell_folder, LPITEMIDLIST item_parent,
			       HTREEITEM parent)
{
	// Allocate a shell memory object. 
	LPMALLOC mem = NULL;
	HRESULT	hr =::SHGetMalloc(&mem);
	if (FAILED(hr))
		return false;

	// Get the IEnumIDList object for the given folder.
	LPENUMIDLIST items = NULL;
	hr = shell_folder->EnumObjects(m_hWnd, SHCONTF_FOLDERS, &items);
	if (FAILED(hr))
	{
		// TODO free mem
		return false;
	}

	// get drives-folder and check if it is the current parent folder
	LPSHELLFOLDER sf = NULL;
	hr = SHGetDesktopFolder(&sf);
	bool parent_is_drives = false;
	LPITEMIDLIST drives_folder = NULL;
	LPITEMIDLIST *drive_letters = NULL;
	hr = SHGetSpecialFolderLocation(m_hWnd, CSIDL_DRIVES, &drives_folder);
	if (SUCCEEDED(hr))
	{
		hr = sf->CompareIDs(0, drives_folder, item_parent);
		if ((short)HRESULT_CODE(hr) == 0)
		{
			// we are processing now the drives, therefore we need the
			// pidls for each mapped drive to get them sorted later
			parent_is_drives = true;
			drive_letters = new LPITEMIDLIST[26];
			for (int i = 0; i < 26; i++)
			{
				char path[10];
				sprintf(path, "%c:\\", 'A'+i);
				drive_letters[i] = PIDL::FromPath(path);
			}
		}
	}

	// Enumerate through the list of folder and nonfolder objects.
	LPITEMIDLIST lpi = NULL;
	ULONG ulFetched;
	while (S_OK == items->Next(1, &lpi, &ulFetched))
	{
		ULONG ulAttrs =	SFGAO_HASSUBFOLDER | SFGAO_FOLDER;
		// Determine what type of object you have.
		shell_folder->GetAttributesOf(1, (const struct _ITEMIDLIST **) &lpi, &ulAttrs);

		if (!(ulAttrs & (SFGAO_FOLDER | SFGAO_HASSUBFOLDER)))
			continue;

		TV_ITEM tvi;
		if (ulAttrs & SFGAO_FOLDER)
			tvi.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
		if (ulAttrs & SFGAO_HASSUBFOLDER)
		{
			// This item has subfolders, so put a plus sign in the 
			// tree view control. The first time the user clicks 
			// the item, you should populate the subfolders.
			tvi.cChildren = 1;
			tvi.mask |= TVIF_CHILDREN;
		}

		// get friendly name
		char name[MAX_PATH_RA];
		sig_sel_dlg_mfc::GetName(shell_folder, lpi, SHGDN_NORMAL, name, MAX_PATH_RA);
		if (parent_is_drives)
		{
			// when we list the drives, sort them according to the drive letter
			// (A:  B:  C: ...) by adding the index of the drive in the drive
			// letter "alphabet"; I am not sure if this is the best way to do it
			// but it works
			int idx = 100;  // if not a drive use '100'
			for (int i = 0; i < 26; i++)
			{
				if (drive_letters[i] == NULL)
					continue;

				char s1[1000], s2[1000];
				GetName(shell_folder, lpi, SHGDN_NORMAL, s1, 1000);
				GetName(sf, drive_letters[i], SHGDN_NORMAL, s2, 1000);
				if (strcmp(s1, s2) == 0)
				{
					idx = i;
					break;
				}

			}

			char new_name[MAX_PATH_RA+10];
			sprintf(new_name, "%03d_%s", idx, name);
			tvi.pszText = new_name;
			tvi.cchTextMax = MAX_PATH_RA+10;
		}
		else
		{
			tvi.pszText = name;
			tvi.cchTextMax = MAX_PATH_RA;
		}

		// get icons
		LPITEMIDLIST lpi_q = ConcatPidls(item_parent, lpi);
		GetNormalAndSelectedIcons(lpi_q, &tvi);
		mem->Free(lpi_q);

		// Get some memory for the ITEMDATA structure.
		LPTVITEMDATA lptvid = (LPTVITEMDATA)mem->Alloc(sizeof(TVITEMDATA));

		// Now make a copy of the ITEMIDLIST
		lptvid->item = CopyITEMID(mem, lpi);
		lptvid->parent = shell_folder;	// store the parent folder's SF
		shell_folder->AddRef();
		lptvid->item_fq = ConcatPidls(item_parent, lpi);

		// store pointer to saved data
		tvi.lParam = (LPARAM) lptvid;

		// insert item
		TVINSERTSTRUCT ins;
		ins.hParent = parent;
		ins.hInsertAfter = TVI_LAST;
		ins.item = tvi;
		HTREEITEM curr = _dir.InsertItem(&ins);

		// check if saved path
		if (strncmp(name, (const char *) _path, strlen(name)) == 0)
			fill_dir(shell_folder, lpi, curr);
	}
	mem->Free(lpi);		// free PIDL the shell gave you

	_dir.SortChildren(parent);
	if (parent_is_drives)
	{
		// now we have to remove the drive letter index
		HTREEITEM child_item = _dir.GetChildItem(parent);
		while (child_item != NULL)
		{
			CString s = _dir.GetItemText(child_item);
			s = s.Right(s.GetLength()-4);
			_dir.SetItemText(child_item, s);

			child_item = _dir.GetNextItem(child_item, TVGN_NEXT);
		}
	}

	if (drive_letters)
		delete[] drive_letters;

	return true;
}				// fill_dir()


bool sig_sel_dlg_mfc::GetName(LPSHELLFOLDER shell, LPITEMIDLIST lpi,
			      DWORD flags, LPSTR name, int name_len)
{
	bool ok = true;
	STRRET str;
	if (NOERROR == shell->GetDisplayNameOf(lpi, flags, &str))
	{
		switch (str.uType)
		{
		case STRRET_WSTR:
			WideCharToMultiByte(CP_ACP, 0, str.pOleStr, -1, name, name_len, NULL, NULL);
			break;
		case STRRET_OFFSET:
			lstrcpy(name, (LPSTR) lpi + str.uOffset);
			break;
		case STRRET_CSTR:
			lstrcpy(name, (LPSTR) str.cStr);
			break;
		default:
			ok = false;
			break;
		}
	}
	else
		ok = false;

	return ok;
}


void
sig_sel_dlg_mfc::GetNormalAndSelectedIcons(LPITEMIDLIST lpifq, LPTV_ITEM lptvitem)
{
	// Don't check the return value here. 
	// If GetIcon() fails, you're in big trouble.
	lptvitem->iImage = GetIcon(lpifq, SHGFI_PIDL | SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
	lptvitem->iSelectedImage =
		GetIcon(lpifq, SHGFI_PIDL | SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_OPENICON);

	return;
}


int
sig_sel_dlg_mfc::GetIcon(LPITEMIDLIST lpi, UINT uFlags)
{
	SHFILEINFO sfi;

	SHGetFileInfo((LPCSTR) lpi, 0, &sfi, sizeof(SHFILEINFO), uFlags);

	return sfi.iIcon;
}


UINT sig_sel_dlg_mfc::GetSize(LPCITEMIDLIST pidl)
{
	UINT cbTotal = 0;
	if (pidl)
	{
		cbTotal += sizeof(pidl->mkid.cb);	// Null terminator
		while (pidl->mkid.cb)
		{
			cbTotal += pidl->mkid.cb;
			pidl = Next(pidl);
		}
	}

	return cbTotal;
}				// GetSize()


LPITEMIDLIST sig_sel_dlg_mfc::Next(LPCITEMIDLIST pidl)
{
	LPSTR lpMem = (LPSTR)pidl;

	lpMem += pidl->mkid.cb;

	return (LPITEMIDLIST) lpMem;
}				// Next()


LPITEMIDLIST sig_sel_dlg_mfc::ConcatPidls(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2)
{
	LPITEMIDLIST pidlNew;
	UINT cb1;
	UINT cb2;

	if (pidl1)		//May be NULL
		cb1 = GetSize(pidl1) - sizeof(pidl1->mkid.cb);
	else
		cb1 = 0;

	cb2 = GetSize(pidl2);

	pidlNew = Create_item(cb1 + cb2);
	if (pidlNew)
	{
		if (pidl1)
			memcpy(pidlNew, pidl1, cb1);
		memcpy(((LPSTR) pidlNew) + cb1, pidl2, cb2);

	}
	return pidlNew;
}				// ConcatPidls()


LPITEMIDLIST sig_sel_dlg_mfc::Create_item(UINT cbSize)
{
	LPMALLOC lpMalloc;
	HRESULT hr;
	LPITEMIDLIST pidl =	NULL;

	hr = SHGetMalloc(&lpMalloc);

	if (FAILED(hr))
		return 0;

	pidl = (LPITEMIDLIST) lpMalloc->Alloc(cbSize);

	if (pidl)
		memset(pidl, 0, cbSize);	// zero-init for external task   alloc

	if (lpMalloc)
		lpMalloc->Release();

	return pidl;
}


LPITEMIDLIST sig_sel_dlg_mfc::CopyITEMID(LPMALLOC lpMalloc, LPITEMIDLIST lpi)
{
	LPITEMIDLIST lpiTemp;

	lpiTemp = (LPITEMIDLIST) lpMalloc->Alloc(lpi->mkid.cb + sizeof(lpi->mkid.cb));
	CopyMemory((PVOID) lpiTemp, (CONST VOID *) lpi, lpi->mkid.cb + sizeof(lpi->mkid.cb));

	return lpiTemp;
}				// CopyITEMID()


void
sig_sel_dlg_mfc::OnItemexpandingDirTree(NMHDR * pNMHDR, LRESULT * pResult)
{
	NM_TREEVIEW *pNMTreeView = (NM_TREEVIEW *) pNMHDR;

	if ((pNMTreeView->itemNew.state & TVIS_EXPANDEDONCE))
		return;

	LPTVITEMDATA lptvid = (LPTVITEMDATA) pNMTreeView->itemNew.lParam;
	if (lptvid)
	{
		HCURSOR wait_cursor = LoadCursor(NULL, IDC_WAIT);
		HCURSOR save_cursor = SetCursor(wait_cursor);

		LPSHELLFOLDER shell = NULL;
		HRESULT hr = lptvid->parent->BindToObject(lptvid->item, 0,
							  IID_IShellFolder,
							  (LPVOID *) & shell);

		if (SUCCEEDED(hr))
			fill_dir(shell, lptvid->item_fq, pNMTreeView->itemNew.hItem);

		SetCursor(save_cursor);
	}

	*pResult = 0;
}

void
sig_sel_dlg_mfc::OnDblclkSigList(NMHDR * pNMHDR, LRESULT * pResult)
{
	NMLISTVIEW *view = (NMLISTVIEW *) pNMHDR;

	*pResult = 0;

	OnOK();
}

void
sig_sel_dlg_mfc::OnClickSigList(NMHDR * pNMHDR, LRESULT * pResult)
{
	NMLISTVIEW *view = (NMLISTVIEW *) pNMHDR;

	int curr = _signals.GetNextItem(-1, LVNI_SELECTED);
	if (curr == -1)
		GetDlgItem(IDOK)->EnableWindow(FALSE);
	else
		GetDlgItem(IDOK)->EnableWindow(TRUE);

	*pResult = 0;
}


void
sig_sel_dlg_mfc::set_savepath(CString path)
{
	_path = path;
}  /* set_savepath() */


CString sig_sel_dlg_mfc::get_savepath()
{
	return _path;
}  /* get_savepath() */


void
sig_sel_dlg_mfc::delete_sig_elements()
{
	int curr = -1;
	while ((curr = _signals.GetNextItem(curr, LVNI_ALL)) != -1)
	{
		char *f = (char *) _signals.GetItemData(curr);
		if (f)
			delete[]f;
	}
	_signals.DeleteAllItems();
}  /* delete_sig_elements() */


void
sig_sel_dlg_mfc::select_path()
{
	if (_path.IsEmpty())
		return;

	CShellPidl pidl((const char *)_path);
	HTREEITEM item = find_item_by_pidl(pidl);
	if (item != NULL)
		_dir.SelectItem(item);
}  // select_path()


LPITEMIDLIST
sig_sel_dlg_mfc::GetItemIDList(HTREEITEM hItem)
{
	TVITEMDATA* pData = (TVITEMDATA*)_dir.GetItemData(hItem);

	return pData->item;
}  /* GetItemIDList() */


/////////////////////////////////////////////////////////////////////////////
//
// The function find_item_by_pidl() was written by Paolo Messina
//
// Copyright information from the file the function was taken:
//
// Copyright (C) 2000-2001 by Paolo Messina
// (http://www.geocities.com/ppescher - ppescher@yahoo.com)
//
// The contents of this file are subject to the Artistic License (the "License").
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.opensource.org/licenses/artistic-license.html
//
// If you find this code useful, credits would be nice!
//
/////////////////////////////////////////////////////////////////////////////
HTREEITEM
sig_sel_dlg_mfc::find_item_by_pidl(LPCITEMIDLIST pidl, HTREEITEM hParentItem/* = TVI_ROOT*/)
{
	LPCITEMIDLIST pidlItem;
	CShellPidlNavigator navObj, navItem;
	navObj.Initialize(pidl);
	int countObj = navObj.GetCount();
	
	HTREEITEM hItem;
	for (hItem = _dir.GetChildItem(hParentItem);
		hItem != NULL; hItem = _dir.GetNextSiblingItem(hItem))
	{
		pidlItem = GetItemIDList(hItem);
		if (pidlItem == NULL) // invalid shell item, skip it!
			continue;

		navItem.Initialize(pidlItem);
		int countItem = navItem.GetCount();
		if (countItem > countObj) // they can't match, try the next
			continue;
		
		// compare the whole item's pidl with the specified one
		
		// start from the Desktop
		LPSHELLFOLDER parent_folder = NULL;
		HRESULT hr = SHGetDesktopFolder(&parent_folder);
		if (FAILED(hr))
			continue;
		
		int idx = 0; // current level in the specified pidl

		if (PIDL::GetLength(pidlItem) != 0) // root item always match!
		{
			BOOL bEqual = TRUE; // think positive!
			for (; idx < countItem && bEqual; idx++)
			{
				// not a valid folder object
				if (parent_folder == NULL)
				{
					bEqual = FALSE;
					break;
				}

				// do the partial comparison
				CShellPidl pidl1(navItem.GetPidl(idx));
				CShellPidl pidl2(navObj.GetPidl(idx));
				HRESULT hr = parent_folder->CompareIDs(0, pidl1, pidl2);
				if ((short)HRESULT_CODE(hr) != 0)
				{
					bEqual = FALSE;
					break;
				}

				// update parent folder (take one, they're equal)
				parent_folder = SShellFolderPtr(parent_folder, pidl1);
			}

			if (!bEqual)
				continue;
		}

		// we should go on now, down in the levels below
		// doing only partial comparisons

		HTREEITEM hItemSaved = hItem; // save for later use
		
		BOOL bEqual = TRUE; // think positive!
		for (; idx < countObj; idx++)
		{
			if (parent_folder == NULL) // not a valid folder object
			{
				bEqual = FALSE;
				break;
			}

			if (!_dir.Expand(hItem, TVE_EXPAND)) // partial match, but could not go on!
			{
				bEqual = FALSE;
				break;
			}

			LPCITEMIDLIST pidl1 = NULL;
			for (hItem = _dir.GetChildItem(hItem);
				hItem != NULL; hItem = _dir.GetNextSiblingItem(hItem))
			{
				pidlItem = GetItemIDList(hItem);
				if (pidlItem == NULL) // invalid shell item, skip it!
					continue;

				// do the partial comparison
				pidl1 = PIDL::GetLast(pidlItem);
				CShellPidl pidl2(navObj.GetPidl(idx));
				HRESULT hr = parent_folder->CompareIDs(0, pidl1, pidl2);
				if ((short)HRESULT_CODE(hr) == 0)
					break;
			}

			if (hItem == NULL) // no item found!
			{
				bEqual = FALSE;
				break;
			}

			// update parent folder and proceed
			parent_folder = SShellFolderPtr(parent_folder, pidl1);
		}

		if (bEqual) // all the comparisons were ok
			break;

		// restore first level item and proceed with siblings
		hItem = hItemSaved;
	}

	return hItem; // NULL if not found
} /* find_item_by_pidl() */
