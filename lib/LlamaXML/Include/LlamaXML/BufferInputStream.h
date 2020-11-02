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

#ifndef LLAMAXML_BUFFERINPUTSTREAM_H
#define LLAMAXML_BUFFERINPUTSTREAM_H

#if (! __GNUC__) || __APPLE__
	#pragma once
#endif


#include "LlamaXML/InputStream.h"
#include <string>

namespace LlamaXML {

	/// \brief An InputStream to a fixed buffer of data.

	/// Provides an InputStream to a fixed buffer of user-provided data.
	/// The buffer is not copied, so it must continue to exist for the lifetime
	/// of the BufferInputStream.

	class BufferInputStream: public InputStream {
	public:
		/// Creates a BufferInputStream from a pointer to a buffer of data,
		/// and the length of that data in bytes.  You may pass either 8-bit
		/// or Unicode data in the buffer, but the length must always be in
		/// bytes.  Note that the buffer is not copied, so it must continue 
		/// to exist for the lifetime of the BufferInputStream.
		BufferInputStream(const void * buffer, uint32_t length);
		
		virtual uint32_t ReadUpTo(char * buffer, uint32_t length);
		virtual void Restart();
		virtual bool EndOfFile();
	
	private:
		const char *	mBuffer;
		uint32_t		mLength;
		uint32_t		mOffset;
	};
}

#endif
