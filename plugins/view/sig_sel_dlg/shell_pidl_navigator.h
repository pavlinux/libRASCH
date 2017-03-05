// ShellPidlNavigator.h: interface for the CShellPidlNavigator class.
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

#if !defined(AFX_SHELLPIDLNAVIGATOR_H__443C1615_E764_47F2_8513_D977C9942984__INCLUDED_)
#define AFX_SHELLPIDLNAVIGATOR_H__443C1615_E764_47F2_8513_D977C9942984__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>
#include "shell_pidl.h"

class CShellPidlNavigator
{
protected:
	CShellPidl m_pidl;
	CArray<LPCITEMIDLIST, LPCITEMIDLIST> m_arrPidl;

public:
	LPITEMIDLIST GetPidl(int iIndex);
	int GetCount();
	void Initialize(LPCITEMIDLIST pidl);
	CShellPidlNavigator();
	virtual ~CShellPidlNavigator();

};

#endif // !defined(AFX_SHELLPIDLNAVIGATOR_H__443C1615_E764_47F2_8513_D977C9942984__INCLUDED_)
