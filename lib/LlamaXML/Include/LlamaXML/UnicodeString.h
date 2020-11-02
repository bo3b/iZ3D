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

#ifndef LLAMAXML_UNICODESTRING_H
#define LLAMAXML_UNICODESTRING_H

#if (! __GNUC__) || __APPLE__
	#pragma once
#endif


#include "LlamaXML/StringWorkaround.h"
#include "LlamaXML/LlamaStdInt.h"

/*! \file UnicodeString.h
    \brief Type definitions for UnicodeChar and UnicodeString.
*/

namespace LlamaXML {

	/*
		I really which I could think of a simpler way to do this
		than using template specialization.  All I really want is
		a portable way of saying:
		
		if (sizeof(wchar_t) == 2) {
			typedef wchar_t UnicodeChar;
			typedef std::wstring UnicodeString;
		}
		else {
			typedef uint16_t UnicodeChar;
			typedef std::basic_string<uint16_t> UnicodeString;
		}
	*/	

	/// \cond INTERNAL
	template <int N>
	struct UnicodeTypeInfo {
		typedef uint16_t UnicodeChar;
		typedef std::basic_string<uint16_t> UnicodeString;
	};
	
	template <>
	struct UnicodeTypeInfo<2> {
		typedef wchar_t UnicodeChar;
		typedef std::wstring UnicodeString;
	};
	/// \endcond
	
	/// A 16-bit Unicode character.
	
	/// On platforms where wchar_t is 16-bits, this is defined as wchar_t.
	/// On other platforms, this is defined as uint16_t.
	typedef UnicodeTypeInfo<sizeof(wchar_t)>::UnicodeChar UnicodeChar;
	
	/// A string of Unicode characters.
	
	/// On platforms where wchar_t is 16-bits, this is defined as std::wstring.
	/// On other platforms, this is defined as std::basic_string<uint16_t>.
	typedef UnicodeTypeInfo<sizeof(wchar_t)>::UnicodeString UnicodeString;
	
}

#endif
