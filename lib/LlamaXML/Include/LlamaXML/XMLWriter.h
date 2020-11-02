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

#ifndef LLAMAXML_XMLWRITER_H
#define LLAMAXML_XMLWRITER_H

#if (! __GNUC__) || __APPLE__
	#pragma once
#endif


#include "LlamaXML/OutputStream.h"
#include "LlamaXML/TextEncoding.h"
#include "LlamaXML/ConvertToUnicode.h"
#include "LlamaXML/ConvertFromUnicode.h"
#include <vector>
#if __APPLE_CC__ && TARGET_OS_MAC && (! TARGET_OS_IPHONE)
#pragma GCC visibility push(default)
#endif
#include <string>
#if __APPLE_CC__ && TARGET_OS_MAC && (! TARGET_OS_IPHONE)
#pragma GCC visibility pop
#endif

namespace LlamaXML {

	class XMLWriter {
	public:
		XMLWriter(OutputStream & output, TextEncoding applicationEncoding);
		~XMLWriter();
		
		void StartDocument(const char * version,
                           const char * encoding,
                           const char * standalone);
        void EndDocument();
        
        void StartElement(const char * name);
        void StartElement(const char * prefix, const char * name,
        	const char * namespaceURI = 0);
        void EndElement();

		void WriteElement(const char * name);
        
        template <class T>
        void WriteElement(const char * name, const T & value)
		{
			StartElement(name);
			*this << value;
			EndElement();
		}

		void WriteComment(const char * content);
		void WriteComment(const UnicodeChar * content);
		void WriteComment(const UnicodeString & content);

		void WriteString(const char * content);
		void WriteString(const UnicodeChar * content);
		void WriteString(const UnicodeString & content);
        
        void StartAttribute(const char * name);
        void StartAttribute(const char * prefix, const char * name,
                            const char * namespaceURI = 0);
        void EndAttribute();
        
        template <class T>
        void WriteAttribute(const char * name, const T & value)
		{
			StartAttribute(name);
			*this << value;
			EndAttribute();
		}
    
    private:
		static const int kBufferSize = 256;

    	enum State {
    		kStateNormal,
    		kStateOpenTag,
    		kStateOpenAttribute,
    		kStateDocumentClosed
    	};

		static const char * Scan(const char * content, const char * tokens);
		static const UnicodeChar * Scan(const UnicodeChar * content, const UnicodeChar * contentEnd, const char * tokens);

		static const char * StringEnd(const char * s);
		static const UnicodeChar * StringEnd(const UnicodeChar * s);

		void WriteRawUnicode(const UnicodeChar * unicodeStart, const UnicodeChar * unicodeEnd);
		void WriteApplicationContent(const char * content);
		void WriteUTF8Content(const char * content);
		void WriteUnicodeContent(const UnicodeChar * content, const UnicodeChar * contentEnd);

		void EndComment();
		void StartComment();
	private:
		static const char *			kNewline;
		static const char *			kIndent;
		static const char *			kAmpersand;
		static const char *			kLessThan;
		static const char *			kGreaterThan;
		static const char *			kQuote;

		OutputStream &				mOutput;
		State						mState;
		std::vector<std::string>	mElementStack;
		size_t						mIndentLevel;
		ConvertToUnicode			mApplicationToUnicode;
		ConvertFromUnicode		    mUnicodeToUTF8;
	};

}


LlamaXML::XMLWriter & operator << (LlamaXML::XMLWriter & output, const char * s);
LlamaXML::XMLWriter & operator << (LlamaXML::XMLWriter & output, const std::string & s);
LlamaXML::XMLWriter & operator << (LlamaXML::XMLWriter & output, const LlamaXML::UnicodeString & s);
LlamaXML::XMLWriter & operator << (LlamaXML::XMLWriter & output, bool n);
LlamaXML::XMLWriter & operator << (LlamaXML::XMLWriter & output, int n);
LlamaXML::XMLWriter & operator << (LlamaXML::XMLWriter & output, unsigned int n);
LlamaXML::XMLWriter & operator << (LlamaXML::XMLWriter & output, long n);
LlamaXML::XMLWriter & operator << (LlamaXML::XMLWriter & output, unsigned long n);
LlamaXML::XMLWriter & operator << (LlamaXML::XMLWriter & output, unsigned long long n);
LlamaXML::XMLWriter & operator << (LlamaXML::XMLWriter & output, double n);

#endif
