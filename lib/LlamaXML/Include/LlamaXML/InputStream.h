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

#ifndef LLAMAXML_INPUTSTREAM_H
#define LLAMAXML_INPUTSTREAM_H

#if (! __GNUC__) || __APPLE__
	#pragma once
#endif

#include "LlamaXML/LlamaStdInt.h"

namespace LlamaXML {

	/**
		\brief Abstract base class for the input to an XMLReader.
		
		You can't use this class directly.  Use one of the provided
		subclasses like StringInputStream or BufferInputStream instead.
		
		You can also create your own subclasses of InputStream for data
		sources that are not directly supported by the LlamaXML library.
	*/
	
	class InputStream {
	public:
	    virtual ~InputStream();
	    
		/**
			Fills the buffer with up to length chars, and returns
			the number of chars placed in the buffer.
		*/
		virtual uint32_t ReadUpTo(char * buffer, uint32_t length) = 0;
		
		/**
			Repositions the stream to the beginning of the input.
			This function is called at most once by XMLReader, and
			only when the reader is in the first buffer of text.
			
			(Implementer's note: This function could probably be
			eliminated by more clever programming in the XMLReader
			class.  This would make it easier to implement when data
			is coming from a source that cannot be backed up, like a
			network socket. 
		*/
		virtual void Restart() = 0;

		/**
			Returns true if the stream is currently positioned at the
			end of input.
		*/
		virtual bool EndOfFile() = 0;
	};
	
}

#endif
