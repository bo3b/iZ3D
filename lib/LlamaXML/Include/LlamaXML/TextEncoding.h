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

#ifndef LLAMAXML_TEXTENCODING_H
#define LLAMAXML_TEXTENCODING_H

#if (! __GNUC__) || __APPLE__
	#pragma once
#endif

#include "LlamaXML/UnicodeString.h"
#include "LlamaXML/PlatformConfig.h"

namespace LlamaXML {

	class TextEncoding {
	public:
		TextEncoding();
		
		// Returns true if conversion to/from Unicode is available on this system.
		bool IsAvailable() const;
		
		// LlamaPlatform::String Name() const;
		
		bool operator == (const TextEncoding & other) const;
		bool operator != (const TextEncoding & other) const;
		
		operator UINT() const;
		
		UINT AsWindowsCodePage() const;
		
		static TextEncoding System();
		static TextEncoding Application();
		
		static TextEncoding WindowsLatin1();
		static TextEncoding PalmLatin1();
		static TextEncoding ISOLatin1();
		static TextEncoding ASCII();
		static TextEncoding WindowsShiftJIS();
		static TextEncoding PalmShiftJIS();
		static TextEncoding ShiftJIS();
		static TextEncoding UCS2();
		static TextEncoding UTF7();
		static TextEncoding UTF8();
		static TextEncoding UTF16();
		static TextEncoding UTF16BE();
		static TextEncoding UTF16LE();
		static TextEncoding UTF32();
		static TextEncoding UTF32BE();
		static TextEncoding UTF32LE();
		static TextEncoding Windows(UINT codePage);
		// static TextEncoding Macintosh(::TextEncoding encoding);
		// static TextEncoding Palm(UInt8 encoding);

		static TextEncoding WebCharset(const UnicodeChar * name);
	
	private:
		explicit TextEncoding(UINT codePage);
	
	private:
		UINT mCodePage;
	};
	
	
	inline TextEncoding::TextEncoding(UINT codePage)
	: mCodePage(codePage)
	{
	}
	
	
	inline bool TextEncoding::operator == (const TextEncoding & other) const
	{
		return mCodePage == other.mCodePage;
	}
	
	
	inline bool TextEncoding::operator != (const TextEncoding & other) const
	{
		return mCodePage != other.mCodePage;
	}
	
	
	inline TextEncoding::operator UINT() const
	{
		return mCodePage;
	}
	
	
	inline UINT TextEncoding::AsWindowsCodePage() const
	{
		return mCodePage;
	}

}

#endif
