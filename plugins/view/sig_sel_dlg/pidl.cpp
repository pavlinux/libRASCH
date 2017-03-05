// PIDL.cpp: implementation of the PIDL class.
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
#include "shell_wrappers.h"
#include "pidl.h"

namespace PIDL
{

//////////////////////////////////////////////////////////////////////
// Low-Level Pidl Functions

// TRUE if specified pidl is a terminator
BOOL IsRawEnd(LPCITEMIDLIST pidl)
{
	return (pidl != NULL) && (pidl->mkid.cb == 0);
}

// NULL objects get a zero length
UINT GetRawLength(LPCITEMIDLIST pidl)
{
	if (pidl == NULL)
		return 0;

	UINT length = sizeof(USHORT), cb;
	do
	{
		cb = pidl->mkid.cb;
		pidl = (LPCITEMIDLIST)((LPBYTE)pidl + cb);
		length += cb;
	}
	while (cb != 0);

	return length;
}

LPCITEMIDLIST GetRawNext(LPCITEMIDLIST pidl)
{
	if (pidl == NULL)
		return NULL;

	// Get the size of the specified item identifier. 
	int cb = pidl->mkid.cb; 
	
	// If the size is zero, it is the end of the list. 
	if (cb == 0) 
		return NULL; 
	
	// Add cb to pidl (casting to increment by bytes). 
	return (LPCITEMIDLIST)((LPBYTE)pidl + cb); 
}


//////////////////////////////////////////////////////////////////////
// High-Level Pidl Functions

// does not include terminating zero
// NULL objects get a zero length
UINT GetLength(LPCITEMIDLIST pidl)
{
	UINT length = GetRawLength(pidl);

	return (length == 0) ? length : length - sizeof(USHORT);
}

LPCITEMIDLIST GetNext(LPCITEMIDLIST pidl)
{
	pidl = GetRawNext(pidl); 
	
	// Return NULL if we reached the terminator, or a pidl otherwise. 
	return IsRawEnd(pidl) ? NULL : pidl;
}

LPCITEMIDLIST GetLast(LPCITEMIDLIST pidl)
{
	LPCITEMIDLIST pidlLast = pidl, tmp = GetNext(pidl);
	while (tmp != NULL)
	{
		pidlLast = tmp;
		tmp = GetNext(tmp);
	}

	return pidlLast;
}

LPITEMIDLIST Combine(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2)
{
	// check arguments
	if (pidl1 == NULL && pidl2 == NULL)
		return NULL;

	// Get the size of the resulting item identifier list
	UINT cb1 = GetLength(pidl1);
	UINT cb2 = GetLength(pidl2);

	// Allocate a new item identifier list
	SMallocPtr pMalloc;
	LPITEMIDLIST pidlNew = (LPITEMIDLIST)pMalloc->Alloc(cb1 + cb2 + sizeof(USHORT));
	LPITEMIDLIST pidlEnd = pidlNew;
	if (pidlNew != NULL)
	{		
		// Copy the first item identifier list and terminating 0
		if (cb1 > 0)
		{
			CopyMemory(pidlEnd, pidl1, cb1);
			pidlEnd = (LPITEMIDLIST)((LPBYTE)pidlEnd + cb1);
		}

		// Copy the second item identifier list and terminating 0
		if (cb2 > 0)
		{
			CopyMemory(pidlEnd, pidl2, cb2);
			pidlEnd = (LPITEMIDLIST)((LPBYTE)pidlEnd + cb2);
		}

		// Append a terminating zero. 
		pidlEnd->mkid.cb = 0;
	}
	return pidlNew;
}

LPITEMIDLIST Copy(LPCITEMIDLIST pidl, UINT cb)
{
	// Does not check for argument validity!

	// Allocate a new item identifier list. 
	SMallocPtr pMalloc;
	LPITEMIDLIST pidlNew = (LPITEMIDLIST)pMalloc->Alloc(cb + sizeof(USHORT)); 

	if (pidlNew != NULL) 
	{		
		// Copy the specified item identifier. 
		CopyMemory(pidlNew, pidl, cb); 
		
		// Append a terminating zero. 
		((LPITEMIDLIST)((LPBYTE)pidlNew + cb))->mkid.cb = 0; 
	}
	return pidlNew;
}

LPITEMIDLIST Clone(LPCITEMIDLIST pidl)
{
	if (pidl == NULL)
		return NULL;

	// zero-length is ok
	UINT cb = GetLength(pidl);

	return Copy(pidl, cb);
}

LPITEMIDLIST CloneFirst(LPCITEMIDLIST pidl)
{
	if (pidl == NULL)
		return NULL;

	// Get the size of the first item identifier.
	int cb = pidl->mkid.cb; 
	
	return Copy(pidl, cb);
}

LPITEMIDLIST CloneParentAt(LPCITEMIDLIST pidl, LPCITEMIDLIST pidlDescendant)
{
	if (pidl == NULL || pidlDescendant == NULL)
		return NULL;

	// Get the size of the parent item identifier. 
	UINT cb = (UINT)pidlDescendant - (UINT)pidl;

	return Copy(pidl, cb);
}

LPITEMIDLIST FromPath(LPCTSTR pszPath, HWND hOwner)
{
	// buffer large enough for either Ansi or Unicode string
	WCHAR buffer[MAX_PATH];
#ifdef UNICODE
	lstrcpynW(buffer, pszPath, MAX_PATH);
#else
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pszPath, -1, buffer, MAX_PATH);
#endif //UNICODE

	ULONG count = 0;
	LPITEMIDLIST pidlNew = NULL;

	SDesktopFolderPtr pDesktopFolder;
	pDesktopFolder->ParseDisplayName(hOwner, NULL, buffer, &count, &pidlNew, NULL);

	return pidlNew;
}

}; // namespace PIDL
