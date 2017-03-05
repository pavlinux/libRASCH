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

#if !defined(AFX_SMARTINTERFACEPTR_H__A0E1BBE7_A5D8_4A80_9A36_ED5553FE74F6__INCLUDED_)
#define AFX_SMARTINTERFACEPTR_H__A0E1BBE7_A5D8_4A80_9A36_ED5553FE74F6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

template <class TYPE>
class TInterfacePtr
{
public:
	~TInterfacePtr()
	{ Free(); }
	
	BOOL IsValid() const
	{ return m_pIface != NULL; }

	TYPE * operator -> ()
	{ return m_pIface; }
	
	const TYPE * operator -> () const
	{ return m_pIface; }

	operator TYPE * () const
	{ return m_pIface; }

protected:
	TYPE * m_pIface;

	// must derive from this class
	TInterfacePtr()
	{ m_pIface = NULL; }

	TInterfacePtr(const TInterfacePtr& obj)
	{ Copy(obj.m_pIface); }

	const TInterfacePtr& operator = (const TInterfacePtr& obj)
	{
		if (obj.m_pIface != NULL)
			obj.m_pIface->AddRef();

		if (m_pIface != NULL)
			m_pIface->Release();

		m_pIface = obj.m_pIface;
		return *this;
	}

	void Copy(TYPE * pIface);
	void Free();
};

template <class TYPE>
inline void TInterfacePtr<TYPE>::Copy(TYPE * pIface)
{
	m_pIface = pIface;
	if (m_pIface != NULL)
		m_pIface->AddRef();
}

template <class TYPE>
inline void TInterfacePtr<TYPE>::Free()
{
	if (m_pIface != NULL)
	{
		m_pIface->Release();
		m_pIface = NULL;
	}
}

// static storage

template <class TYPE>
class TStaticInterfacePtr
{
public:
	~TStaticInterfacePtr()
	{ Free(); }
	
	BOOL IsValid() const
	{ return m_pIface != NULL; }

	TYPE * operator -> ()
	{ return m_pIface; }

	const TYPE * operator -> () const
	{ return m_pIface; }
	
	operator TYPE * () const
	{ return m_pIface; }
	
protected:
	static TYPE * m_pIface;

	TStaticInterfacePtr() {}
	// cannot copy - use another instance
	TStaticInterfacePtr(const TStaticInterfacePtr&) {}
	const TStaticInterfacePtr& operator = (const TStaticInterfacePtr&) {}

	void Free();
};

template <class TYPE>
TYPE * TStaticInterfacePtr<TYPE>::m_pIface = NULL;

template <class TYPE>
inline void TStaticInterfacePtr<TYPE>::Free()
{
	if (m_pIface != NULL)
		if (0 == m_pIface->Release())
			m_pIface = NULL;
}


#endif // !defined(AFX_SMARTINTERFACEPTR_H__A0E1BBE7_A5D8_4A80_9A36_ED5553FE74F6__INCLUDED_)
