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

#ifndef LLAMAXML_OUTPUTSTREAM_H
#define LLAMAXML_OUTPUTSTREAM_H

#if (! __GNUC__) || __APPLE__
	#pragma once
#endif


#include "LlamaXML/LlamaStdInt.h"

namespace LlamaXML {

	/// Abstract base class for the output of an XMLWriter.
	
	/// You do not use this class directly.  Instead, create one of the subclasses
	/// of this class (like StringOutputStream or FileOutputStream).  You can also
	/// subclass this class yourself to write XML data to other places.

	class OutputStream {
	public:
	    virtual ~OutputStream();
	    
		/// Writes \a length bytes of data from the \a buffer to the OutputStream.
		virtual void WriteData(const char * buffer, uint32_t length) = 0;
	};

}

/// Write the string \a s to the output stream.

LlamaXML::OutputStream & operator << (LlamaXML::OutputStream & stream,
	const char * s);

#endif
