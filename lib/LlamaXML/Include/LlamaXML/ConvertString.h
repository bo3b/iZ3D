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

#ifndef LLAMAXML_CONVERTSTRING_H
#define LLAMAXML_CONVERTSTRING_H

#if (! __GNUC__) || __APPLE__
#pragma once
#endif

#include "LlamaXML/PlatformConfig.h"
#include "LlamaXML/UnicodeString.h"
#include "LlamaXML/TextEncoding.h"
#include "LlamaXML/LlamaStdInt.h"

/*! \file ConvertString.h
    \brief Utility functions for converting to and from Unicode text.
*/

namespace LlamaXML {
	
	/// Returns the number of UnicodeChars in s, not including the trailing NULL. (Like std::wcslen)
	size_t UnicodeStringLength(const UnicodeChar * s);
	
    //@{
	/// Converts 8-bit text in the specified sourceEncoding to Unicode. */
	UnicodeString ToUnicode(const char * start, const char * end, TextEncoding sourceEncoding);
	UnicodeString ToUnicode(const char * s, TextEncoding sourceEncoding);
	UnicodeString ToUnicode(const std::string & s, TextEncoding sourceEncoding);
	//@}

    //@{
	/// Converts Unicode text to the specified 8-bit destinationEncoding.
	std::string FromUnicode(const UnicodeChar * start, const UnicodeChar * end, TextEncoding destinationEncoding);
	std::string FromUnicode(const UnicodeChar * s, TextEncoding destinationEncoding);
	std::string FromUnicode(const UnicodeString & s, TextEncoding destinationEncoding);
	//@}
}

#endif
