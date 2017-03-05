// ShellPidl.h: interface for the CShellPidl class.
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

#if !defined(AFX_SHELLPIDL_H__98F90381_3F3A_4DF6_948D_B9BB960E7A96__INCLUDED_)
#define AFX_SHELLPIDL_H__98F90381_3F3A_4DF6_948D_B9BB960E7A96__INCLUDED_

#include "PIDL.h"
#include "shell_wrappers.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CShellPidl : public TSharedObject<ITEMIDLIST>
{
public:
	CShellPidl(LPCTSTR pszPath, HWND hOwner = NULL);
	CShellPidl(UINT nSpecialFolder, HWND hOwner = NULL);
	CShellPidl(LPCITEMIDLIST pidlParent, LPCITEMIDLIST pidlRel);
	CShellPidl(LPCITEMIDLIST pidl);
	CShellPidl();
	virtual ~CShellPidl();

	int GetIconIndex(UINT uFlags = SHGFI_SMALLICON) const;
	BOOL IsRoot() const;

	void Combine(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2);
	void CloneLastParent(LPCITEMIDLIST pidl);
	void CloneLastChild(LPCITEMIDLIST pidl);
	void CloneFirstParent(LPCITEMIDLIST pidl);
	void CloneFirstChild(LPCITEMIDLIST pidl);

	LPCITEMIDLIST GetFirstChild();
	LPCITEMIDLIST GetLastChild();

	CString GetPath() const;
	CString GetTypeName() const;

	// debug support 
#ifdef _DEBUG
	void Dump(CDumpContext& dc) const;
#endif //_DEBUG
};

//////////////////////////////////////////////////////////////////////
// Inline Member Functions

inline CShellPidl::CShellPidl()
{
}

inline CShellPidl::~CShellPidl()
{
}

inline CShellPidl::CShellPidl(LPCITEMIDLIST pidl)
{
	m_pObj = PIDL::Clone(pidl);
}

inline CShellPidl::CShellPidl(LPCITEMIDLIST pidlParent, LPCITEMIDLIST pidlRel)
{
	m_pObj = PIDL::Combine(pidlParent, pidlRel);
}

inline CShellPidl::CShellPidl(UINT nSpecialFolder, HWND hOwner)
{
	SHGetSpecialFolderLocation(hOwner, nSpecialFolder, &m_pObj);
}

inline CShellPidl::CShellPidl(LPCTSTR pszPath, HWND hOwner)
{
	m_pObj = PIDL::FromPath(pszPath, hOwner);
}

inline BOOL CShellPidl::IsRoot() const
{
	return (m_pObj != NULL) && (m_pObj->mkid.cb == 0);
}

inline void CShellPidl::Combine(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2)
{
	Destroy();
	Attach(PIDL::Combine(pidl1, pidl2));
}

// get the first ancestor
inline void CShellPidl::CloneFirstParent(LPCITEMIDLIST pidl)
{
	Destroy();
	Attach(PIDL::CloneFirst(pidl));
}

inline void CShellPidl::CloneFirstChild(LPCITEMIDLIST pidl)
{
	Destroy();
	Attach(PIDL::Clone(PIDL::GetNext(pidl)));
}

inline LPCITEMIDLIST CShellPidl::GetFirstChild()
{
	return PIDL::GetNext(m_pObj);
}

// get the immediate parent
inline void CShellPidl::CloneLastParent(LPCITEMIDLIST pidl)
{
	Destroy();
	Attach(PIDL::CloneParentAt(pidl, PIDL::GetLast(pidl)));
}

inline void CShellPidl::CloneLastChild(LPCITEMIDLIST pidl)
{
	Destroy();
	Attach(PIDL::Clone(PIDL::GetLast(pidl)));
}

inline LPCITEMIDLIST CShellPidl::GetLastChild()
{
	return PIDL::GetLast(m_pObj);
}

#endif // !defined(AFX_SHELLPIDL_H__98F90381_3F3A_4DF6_948D_B9BB960E7A96__INCLUDED_)

