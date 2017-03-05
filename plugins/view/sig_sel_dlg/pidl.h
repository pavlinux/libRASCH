// PIDL.h: interface for the PIDL class.
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

#if !defined(AFX_PIDL_H__7F448AB9_6FE6_443A_81F1_3623AB669A6B__INCLUDED_)
#define AFX_PIDL_H__7F448AB9_6FE6_443A_81F1_3623AB669A6B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace PIDL
{
	// high-level functions
	UINT GetLength(LPCITEMIDLIST pidl);
	LPCITEMIDLIST GetNext(LPCITEMIDLIST pidl);
	LPCITEMIDLIST GetLast(LPCITEMIDLIST pidl);
	LPITEMIDLIST Combine(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2);
	LPITEMIDLIST Copy(LPCITEMIDLIST pidl, UINT cb);
	LPITEMIDLIST Clone(LPCITEMIDLIST pidl);
	LPITEMIDLIST CloneFirst(LPCITEMIDLIST pidl);
	LPITEMIDLIST CloneParentAt(LPCITEMIDLIST pidl, LPCITEMIDLIST pidlDescendant);
	LPITEMIDLIST FromPath(LPCTSTR pszPath, HWND hOwner = NULL);
//	int Compare(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2);

	// low-level functions
	BOOL IsRawEnd(LPCITEMIDLIST pidl);
	UINT GetRawLength(LPCITEMIDLIST pidl);
	LPCITEMIDLIST GetRawNext(LPCITEMIDLIST pidl);

}; // namespace PIDL

#endif // !defined(AFX_PIDL_H__7F448AB9_6FE6_443A_81F1_3623AB669A6B__INCLUDED_)
