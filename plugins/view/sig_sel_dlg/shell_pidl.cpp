// ShellPidl.cpp: implementation of the CShellPidl class.
//
/////////////////////////////////////////////////////////////////////////////
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
// $Id$
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "shell_pidl.h"

//////////////////////////////////////////////////////////////////////
// Member Functions

int CShellPidl::GetIconIndex(UINT uFlags) const
{
	SHFILEINFO sfi;
	ZeroMemory(&sfi, sizeof(SHFILEINFO));
	uFlags |= SHGFI_PIDL | SHGFI_SYSICONINDEX;
	SHGetFileInfo((LPCTSTR)m_pObj, 0, &sfi, sizeof(SHFILEINFO), uFlags);
	return sfi.iIcon;
}

CString CShellPidl::GetTypeName() const
{
	SHFILEINFO sfi;
	ZeroMemory(&sfi, sizeof(SHFILEINFO));
	UINT uFlags = SHGFI_PIDL | SHGFI_TYPENAME;
	SHGetFileInfo((LPCTSTR)m_pObj, 0, &sfi, sizeof(SHFILEINFO), uFlags);
	return sfi.szTypeName;
}

CString CShellPidl::GetPath() const
{
	CString path;
	BOOL bSuccess = SHGetPathFromIDList(m_pObj, path.GetBuffer(MAX_PATH));
	path.ReleaseBuffer();
	if (!bSuccess)
		path.Empty();
	return path;
}


// debug

#ifdef _DEBUG
void CShellPidl::Dump(CDumpContext& dc) const
{
	dc << "addr = " << (void*)m_pObj << "\n";
	if (m_pObj == NULL)
		return;

	dc << "[structure]\n";
	
	LPCITEMIDLIST pidl = m_pObj;
	while (pidl != NULL)
	{
		dc << " " << pidl->mkid.cb;
		pidl = PIDL::GetNext(pidl);
	}
	dc << "\n";
}
#endif //_DEBUG

