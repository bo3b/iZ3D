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

#ifndef LLAMAXML_CONVERTTOUNICODE_H
#define LLAMAXML_CONVERTTOUNICODE_H

#if (! __GNUC__) || __APPLE__
	#pragma once
#endif


#include "LlamaXML/UnicodeString.h"
#include "LlamaXML/TextEncoding.h"
#include "LlamaXML/Component.h"
#include <mlang.h>

namespace LlamaXML {

	/**
		\brief An internal class used by XMLReader to convert text in other
		encodings to Unicode.  You should not need to use this class
		directly.
		
		This class has different implementations on different platforms.
	*/

	class ConvertToUnicode {
	public:
		ConvertToUnicode(TextEncoding sourceEncoding);
		~ConvertToUnicode();

		void Reset(TextEncoding sourceEncoding);
		
		void Convert(const char * & sourceStart, const char * sourceEnd,
			UnicodeChar * & destStart, UnicodeChar * destEnd);
		
		TextEncoding GetSourceEncoding() const
		{
			return mSourceEncoding;
		}

	private:
		TextEncoding						mSourceEncoding;
	};

}

#endif
