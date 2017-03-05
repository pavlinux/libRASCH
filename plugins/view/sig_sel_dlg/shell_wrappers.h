// SmartInterfacePtr.h: interface for the TInterfacePtr class.
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

#if !defined(AFX_SHELLWRAPPERS_H__B0E1BBE7_A5D8_4A80_9A36_ED5553FE74F6__INCLUDED_)
#define AFX_SHELLWRAPPERS_H__B0E1BBE7_A5D8_4A80_9A36_ED5553FE74F6__INCLUDED_

#include <shlobj.h>
#include "smart_interface_ptr.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class SMallocPtr : public TStaticInterfacePtr<IMalloc>
{
public:
	SMallocPtr()
	{
		if (m_pIface == NULL)
			SHGetMalloc(&m_pIface);
		else
			m_pIface->AddRef();
	}
};

class SDesktopFolderPtr : public TStaticInterfacePtr<IShellFolder>
{
public:
	SDesktopFolderPtr()
	{
		if (m_pIface == NULL)
			SHGetDesktopFolder(&m_pIface);
		else
			m_pIface->AddRef();
	}
};

class SShellFolderPtr : public TInterfacePtr<IShellFolder>
{
	typedef TInterfacePtr<IShellFolder> BASE;
	typedef TStaticInterfacePtr<IShellFolder> STATIC;

public:
	SShellFolderPtr() {}

	SShellFolderPtr(const TInterfacePtr<IShellFolder>& obj)
		: TInterfacePtr<IShellFolder>(obj) { }

	SShellFolderPtr(const TStaticInterfacePtr<IShellFolder>& obj)
	{ Copy((IShellFolder*)obj); }

	SShellFolderPtr(IShellFolder* pFolder)
	{ Copy(pFolder); }

	SShellFolderPtr(IShellFolder* pParentFolder, LPCITEMIDLIST pidlRel)
	{
		if (pidlRel != NULL && pidlRel->mkid.cb == 0)
		{
			// it's a root element, copy the parent folder
			Copy(pParentFolder);
		}
		else
		{
			pParentFolder->BindToObject(pidlRel,
				NULL, IID_IShellFolder, (LPVOID*)&m_pIface);
		}
	}
	
	SShellFolderPtr(LPCITEMIDLIST pidlAbs)
	{
		SDesktopFolderPtr pDesktopFolder;
		if (pidlAbs != NULL && pidlAbs->mkid.cb == 0)
		{
			// it's a root element, copy the parent folder
			Copy(pDesktopFolder);
		}
		else
		{
			pDesktopFolder->BindToObject(pidlAbs,
				NULL, IID_IShellFolder, (LPVOID*)&m_pIface);
		}
	}
};

class SEnumIDListPtr : public TInterfacePtr<IEnumIDList>
{
public:
	SEnumIDListPtr(IShellFolder* pParentFolder, UINT nFlags, HWND hOwner = NULL)
	{
		pParentFolder->EnumObjects(hOwner, nFlags, &m_pIface);
	}
};

class SContextMenuPtr : public TInterfacePtr<IContextMenu>
{
public:
	SContextMenuPtr() {}

	SContextMenuPtr(IShellFolder* pParentFolder, LPCITEMIDLIST pidlRel, HWND hOwner = NULL)
	{
		pParentFolder->GetUIObjectOf(hOwner, 1, &pidlRel,
			IID_IContextMenu, NULL, (LPVOID*)&m_pIface);
	}
};

class SContextMenu2Ptr : public TInterfacePtr<IContextMenu2>
{
public:
	SContextMenu2Ptr(IContextMenu* pIface)
	{
		if (pIface != NULL)
			pIface->QueryInterface(IID_IContextMenu2, (LPVOID*)&m_pIface);
	}
};

class SContextMenu3Ptr : public TInterfacePtr<IContextMenu3>
{
public:
	SContextMenu3Ptr(IContextMenu* pIface)
	{
		if (pIface != NULL)
			pIface->QueryInterface(IID_IContextMenu3, (LPVOID*)&m_pIface);
	}
};

// templates

template <class TYPE>
class TSharedObject
{
public:
	~TSharedObject()
	{ Destroy(); }

protected:
	TYPE* m_pObj;

	TSharedObject()
	{ m_pObj = NULL; }

	TSharedObject(const TSharedObject& shobj)
	{ Copy(shobj); }

	void Copy(const TSharedObject& shobj);

public:
	operator const TYPE * () const
	{ return m_pObj; }

	const TSharedObject& operator = (const TSharedObject& shobj)
	{
		Destroy();
		Copy(shobj);

		return *this;
	}

	BOOL IsValid() const
	{ return m_pObj != NULL; }

	void Attach(TYPE* pObj)
	{
		ASSERT(m_pObj == NULL); // can't attach two times!
		m_pObj = pObj;
	}

	TYPE* Detach()
	{
		TYPE* ret = m_pObj;
		m_pObj = NULL;
		return ret;
	}

	// obtain a void object by pointer
	TYPE** GetPointer()
	{
		Destroy();
		return &m_pObj;
	}

#ifdef _DEBUG
	void DumpRaw(CDumpContext& dc) const;
#endif //_DEBUG

protected:

	void Destroy()
	{
		SMallocPtr pMalloc;
		pMalloc->Free(Detach());
	}
};

template <class TYPE>
void TSharedObject<TYPE>::Copy(const TSharedObject<TYPE>& shobj)
{
	m_pObj = NULL;
	if (shobj.m_pObj == NULL)
		return;

	// obj to copy must have been allocated this way
	SMallocPtr pMalloc;
	ULONG size = pMalloc->GetSize((void*)shobj.m_pObj);
	ASSERT(size > 0);

	if (shobj.m_pObj != NULL)
	{
		m_pObj = (TYPE*)pMalloc->Alloc(size);
		CopyMemory(m_pObj, shobj.m_pObj, size);
	}
}

#ifdef _DEBUG
template <class TYPE>
void TSharedObject<TYPE>::DumpRaw(CDumpContext& dc) const
{
	dc << "addr = " << (void*)m_pObj << "\n";
	if (m_pObj == NULL)
		return;

	SMallocPtr pMalloc;
	ULONG size = pMalloc->GetSize((void*)m_pObj);
	dc << "[hex dump]\n";
	dc.HexDump(".", (BYTE*)m_pObj, (int)size, 16);
	dc << "\n";
}
#endif //_DEBUG

#endif //!defined(AFX_SHELLWRAPPERS_H__B0E1BBE7_A5D8_4A80_9A36_ED5553FE74F6__INCLUDED_)
