/*
 * Copyright (C) 2005 Llamagraphics, Inc.
 * 
 * This file is part of LlamaXML.
 *
 * LlamaXML is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * LlamaXML is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
 * USA.
 *
 * LlamaXML is also available under an alternative commercial license for
 * developers who need the flexibility to include this library in proprietary
 * applications.  Visit http://www.llamagraphics.com/LlamaXML/ for more 
 * information.
 */

#ifndef LLAMAXML_COMPONENT_H
#define LLAMAXML_COMPONENT_H

#if (! __GNUC__) || __APPLE__
	#pragma once
#endif

#include <objbase.h>
#include "LlamaXML/XMLException.h"

namespace LlamaXML {

	template <class T>
	class Component {
	public:
		Component();
		Component(const Component<T> & other);
		Component(REFCLSID rclsid, DWORD dwClsContext = CLSCTX_INPROC_HANDLER | CLSCTX_INPROC_SERVER);
		Component(IUnknown * x);
		Component(IStream * pStm);
		~Component();

		Component<T> & operator = (const Component<T> & other);

		T * operator -> () const
		{
			return mComponent;
		}

		operator T * () const
		{
			return mComponent;
		}

		T ** Adopt();

	private:
		static const IID & kInterfaceID;

		T * mComponent;
	};


	template <class T>
		Component<T>::Component()
		: mComponent(0)
	{
	}


	template <class T>
		Component<T>::Component(const Component<T> & other)
		: mComponent(other.mComponent)
	{
		if (mComponent != 0) mComponent->AddRef();
	}


	template <class T>
		Component<T>::Component(REFCLSID rclsid, DWORD dwClsContext)
		: mComponent(0)
	{
		ThrowIfXMLError(::CoCreateInstance(rclsid, NULL, dwClsContext, 
			kInterfaceID, (LPVOID*)&mComponent));
	}


	template <class T>
		Component<T>::Component(IUnknown * x)
		: mComponent(0)
	{
		ThrowIfXMLError(x->QueryInterface(kInterfaceID, (LPVOID*)&mComponent));
	}


	template <class T>
		Component<T>::Component(IStream * pStm)
		: mComponent(0)
	{
		ThrowIfXMLError(::OleLoadFromStream(pStm, kInterfaceID, &mComponent));
	}


	template <class T>
		Component<T>::~Component()
	{
		if (mComponent != 0) mComponent->Release();
	}


	template <class T>
		Component<T> & Component<T>::operator = (const Component<T> & other)
	{
		if (mComponent != other.mComponent) {
			if (mComponent != 0) mComponent->Release();
			mComponent = other.mComponent;
			if (mComponent != 0) mComponent->AddRef();
		}
		return *this;
	}


	template <class T>
		T ** Component<T>::Adopt()
	{
		if (mComponent != 0) {
			mComponent->Release();
			mComponent = 0;
		}
		return &mComponent;
	}


}

#endif
