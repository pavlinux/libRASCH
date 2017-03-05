// ShellPidlNavigator.cpp: implementation of the CShellPidlNavigator class.
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
#include "shell_pidl_navigator.h"
#include "pidl.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CShellPidlNavigator::CShellPidlNavigator()
{
}

CShellPidlNavigator::~CShellPidlNavigator()
{
}

void CShellPidlNavigator::Initialize(LPCITEMIDLIST pidl)
{
	m_pidl = pidl;
	m_arrPidl.RemoveAll();

	LPCITEMIDLIST pidlObj = pidl;
	while (pidlObj != NULL)
	{
		m_arrPidl.Add(pidlObj);
		pidlObj = PIDL::GetRawNext(pidlObj);
	}
}

int CShellPidlNavigator::GetCount()
{
	return m_arrPidl.GetSize()-1;
}

// use CShellPidl::Attach() on the result to handle Pidl
// and get automatic destruction
LPITEMIDLIST CShellPidlNavigator::GetPidl(int iIndex)
{
	if (iIndex < 0 || iIndex >= GetCount())
		return NULL;

	return PIDL::CloneParentAt(m_arrPidl.GetAt(iIndex), m_arrPidl.GetAt(iIndex+1));
}
