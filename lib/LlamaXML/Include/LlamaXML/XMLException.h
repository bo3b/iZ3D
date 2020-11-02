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

#ifndef LLAMAXML_XMLEXCEPTION_H
#define LLAMAXML_XMLEXCEPTION_H

#if (! __GNUC__) || __APPLE__
	#pragma once
#endif

#include <exception>
#include "LlamaXML/LlamaStdInt.h"
#include "LlamaXML/PlatformConfig.h"

namespace LlamaXML {

	/// \brief Base class for all exceptions thrown by the LlamaXML library.
	///
	/// Normally, client code does not create XMLExceptions, it just catches them.

	class XMLException: public std::exception
	{
	public:
		XMLException(int32_t err, const char * file, long line) throw();
		XMLException(int32_t err, const char * what) throw();
		
		/// Returns the operating system error code (if any) that caused the exception.
		
		int32_t ErrorCode() const {
			return mErrorCode;
		}
		
		/// Returns the source file the exception was thrown from.
		
		/// \deprecated This function may be removed in future versions of the library
		/// in order to reduce the size of the library.
		const char * File() const {
			return mFile;
		}
		
		/// Returns the line number of the source file the exception was thrown from.
		
		/// \deprecated This function may be removed in future versions of the library
		/// in order to reduce the size of the library.
		long Line() const {
			return mLine;
		}
		
		const char * what() const throw();

	private:
		int32_t					mErrorCode;
		const char *			mFile;
		long					mLine;
		const char *			mWhat;
	};

	void ThrowXMLException(int32_t err, const char * file, long line);
	void ThrowXMLException(int32_t err, const char * what);
}

#define ThrowXMLError(x) LlamaXML::ThrowXMLException(x, __FILE__, __LINE__)
#if defined(WIN32) && ! defined(__PALMOS__)
	#define ThrowIfXMLError(x) do {HRESULT e = x; if (FAILED(e)) LlamaXML::ThrowXMLException(e, __FILE__, __LINE__);} while(0)
#else
	#define ThrowIfXMLError(x) do {int32_t e = x; if (e != 0) LlamaXML::ThrowXMLException(e, __FILE__, __LINE__);} while(0)
#endif

#endif
