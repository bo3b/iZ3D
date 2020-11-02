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

#ifndef LLAMAXML_STRINGINPUTSTREAM_H
#define LLAMAXML_STRINGINPUTSTREAM_H

#if (! __GNUC__) || __APPLE__
	#pragma once
#endif


#include "LlamaXML/InputStream.h"
#if __APPLE_CC__ && TARGET_OS_MAC && (! TARGET_OS_IPHONE)
#pragma GCC visibility push(default)
#endif
#include <string>
#if __APPLE_CC__ && TARGET_OS_MAC && (! TARGET_OS_IPHONE)
#pragma GCC visibility pop
#endif

namespace LlamaXML {

	/**
		InputStream subclass for reading data from a C or C++ string.
		
		Note that this class copies the input string.  While this is safe,
		if you don't need the data to be copied the BufferInputStream
		class is more efficient.
	*/

	class StringInputStream: public InputStream {
	public:
		StringInputStream(const std::string & s);
		StringInputStream(const char * s);
		
		virtual uint32_t ReadUpTo(char * buffer, uint32_t length);
		virtual void Restart();
		virtual bool EndOfFile();
	
	private:
		std::string	mString;
		uint32_t	mOffset;
	};
	
}

#endif
