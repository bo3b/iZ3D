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

#ifndef LLAMAXML_XMLREADER_H
#define LLAMAXML_XMLREADER_H

#if (! __GNUC__) || __APPLE__
	#pragma once
#endif

#include "LlamaXML/PlatformConfig.h"
#include "LlamaXML/InputStream.h"
#include "LlamaXML/TextEncoding.h"
#include "LlamaXML/UnicodeString.h"
#include "LlamaXML/ConvertToUnicode.h"
#include <stddef.h>
#include <vector>
#include <sstream>

namespace LlamaXML {

	/**
		\brief The main XML parser class.
		
		To use this class, first construct one of the subclasses of
		InputStream, like FileInputStream or StringInputStream.
		Then construct the XMLReader on the InputStream.
		
		Once the XMLReader is constructed, you need to choose whether
		to parse the XML using the low-level interface or the high-level
		interface (though the two can be mixed).
		
		<b>Low-level interface</b>
		
		The low-level interface returns every node in the XML file,
		including comments, processing instructions, and whitespace.
		This is good when you're trying to parse an unknown XML
		document, or need to know every little detail.  The low-level
		interface can be
		tedious, though, if you know in advance the structure of the document
		you are parsing and just want to pull out specific data.
		The low-level interface only throws an XMLException if the
		document is not well-formed or the underlying InputStream throws
		an exception.
		
		To use the low-level interface, call Read() repeatedly to parse
		the document.  When Read() returns false, you have finished
		parsing the file.
		
		\code
	StringInputStream input("<a/>");
	XMLReader reader(input);

	while (reader.Read()) {
	    switch (reader.GetNodeType()) {
	        case kElement:
	            break;
	        case kEndElement:
	            break;
	        case kText:
	            break;
	    }
	}
		\endcode
		
		Each time you call Read(), the XMLReader moves to the next
		node in the parse tree.  You can determine the kind of node
		by calling the GetNodeType function.  Depending on the type
		of node, you can get additional information by calling the
		GetName(), GetLocalName(), GetNamespaceURI(), GetValue() and
		GetAttribute() functions.
		
		Note that attributes are not returned as separate nodes.
		You must retrieve attributes from the element node.
		
		<b>High-level interface</b>
		
		The high-level interface is generally easier to use than the
		low-level interface if you
		know in advance the structure of the XML document you are
		parsing.  The high-level interface skips processing instructions,
		comments and whitespace, checks to make sure that
		the next node matches what is expected, and returns the data
		from that node.  If the node does not match, it will throw 
		an XMLException.
	*/

	class XMLReader {
	public:
		/// Creates an XMLReader that parses data from the specified InputStream.
		
		/// The optional \a initialEncoding argument can be used to specify the
		/// initial encoding used to parse the XML document if there is some external
		/// information (like a MIME charset) as to what it is.  If the XML document
		/// specifies an encoding interally, this will override the \a initialEncoding.
		/// If you do not have information about the encoding of the XML document,
		/// then use the default of UTF-8.
		
		XMLReader(InputStream & input,
			TextEncoding initialEncoding = TextEncoding::UTF8());
			
		~XMLReader();

		enum NodeType {
			kAttribute,
			kCDATA,
			kComment,
			kDocument,
			kDocumentFragment,
			kDocumentType,
			kElement,
			kEndElement,
			kEndEntity,
			kEntity,
			kEntityReference,
			kNone,
			kNotation,
			kProcessingInstruction,
			kSignificantWhitespace,
			kText,
			kWhitespace,
			kXmlDeclaration
		};
		
		enum WhitespaceHandling {
		    kWhitespaceHandlingAll,
		    kWhitespaceHandlingNone,
		    kWhitespaceHandlingSignificant
		};
		
		/// \name Low-level interface
		//@{
		
		/// Move the reader to the next node in the parse tree.
		
		/// \return true if a node was read succesfully, or false on end-of-file
		///
		/// \exception XMLException the input is not well-formed, there was an
		/// encoding conversion error, or the underlying InputStream threw an
		/// exception

		bool Read();
		
		/// Returns true if the reader is positioned past the last node of the document.

		bool EndOfFile() const;
		
		/// Returns the type of the current XML parse node.

		NodeType GetNodeType() const
		{
			return mNodeType;
		}
		
		/// Returns the full name of a kElement or kEndElement node.
		
		/// \warning The full name of an element includes both the 
		/// namespace prefix and the local name.  If you are looking for
		/// a particular element, you should generally test the local name
		/// (and possibly the namespace URI) instead, since the writer of
		/// the XML file is free to choose arbitrary namespace prefixes.

		const UnicodeString & GetName() const
		{
			return mCurrentName.mName;
		}
		
		/// Returns the portion of the element name after the colon.
		
		/// When parsing an XML file that may be using namespaces, it
		/// is generally a better idea to rely on the local name of an
		/// element rather than the full name returned by GetName(),
		/// since the full name may include an arbitrary namespace prefix.

		const UnicodeString & GetLocalName() const
		{
			return mCurrentName.mLocalName;
		}
		
		/// Returns the text of a kText or kWhitespace node.

		const UnicodeString & GetValue() const
		{
			return mValue;
		}
		
		/// Returns the namespace prefix of the current node.
		
		/// Valid for kElement and kEndElement nodes.
		/// The prefix is the portion of the node name before the colon.
		///
		/// \warning It is generally bad practice to depend on a
		/// particular prefix, since the writer of an XML file is
		/// free to choose any prefix.  If you need to check the
		/// namespace of a node, use GetNamespaceURI() instead.

		const UnicodeString & GetPrefix() const
		{
			return mCurrentName.mPrefix;
		}
		
		/// Returns the namespace URI of the current node.
		
		/// \note This is the preferred way to check for the namespace of an element,
		/// since the writer of an XML file is free to choose an arbitrary namespace
		/// prefix.

		const UnicodeString & GetNamespaceURI() const
		{
			return mCurrentName.mNamespaceURI;
		}
		
		/// Returns true if a kElement node is self-terminated. (i.e. \<foo /\>)

		bool IsEmptyElement() const
		{
			return mIsEmptyElement;
		}
		
		/// Returns true if a kElement node has one or more attributes.

		bool HasAttributes() const
		{
			return ! mAttributes.empty();
		}
		
		/// Returns the number of attributes in the current element node.

		size_t AttributeCount() const
		{
			return mAttributes.size();
		}
		
		/// Returns the current whitespace handling setting of the XMLReader.  See SetWhitespaceHandling()
				
		WhitespaceHandling GetWhitespaceHandling() const {
		    return mWhitespaceHandling;
		}
		
		/// Sets the whitespace handling of the Read() function.
		
		/// Normally, the Read function returns every node in the parse tree.
		/// However, if you set whitespace handling to kWhitespaceHandlingNone,
		/// Read will skip over whitespace nodes.  You may change the whitespace
		/// handling at any time.
		///
		/// \note The high-level interface functions skip whitespace nodes
		/// regardless of this setting.
		
		void SetWhitespaceHandling(WhitespaceHandling whitespaceHandling) {
		    mWhitespaceHandling = whitespaceHandling;
		}
		
		/// Returns true if there is an attribute with the specified index.
		bool HasAttribute(size_t i) const;
		
		/// Returns true if there is an attribute with the specified name.
		bool HasAttribute(const char * name) const;
		
		/// Returns true if there is an attribute with the specified local name and namespace.
		bool HasAttribute(const char * localName, const char * namespaceURI) const;

		/// Returns the value of the attribute at the specified index.
		UnicodeString GetAttribute(size_t i) const;
		
		/// Returns the value of the attribute with the specified name.
		UnicodeString GetAttribute(const char * name) const;
		
		/// Returns the value of the attribute with the specified local name and namespace.
		UnicodeString GetAttribute(const char * localName, const char * namespaceURI) const;

		/// Returns the value of the attribute at the specified index.
		std::string GetAttribute(size_t i, TextEncoding encoding) const;
		
		/// Returns the value of the attribute with the specified local name and namespace.		
		std::string GetAttribute(const char * name, TextEncoding encoding) const;

		/// Returns the value of the attribute with the specified local name and namespace.
		std::string GetAttribute(const char * localName, const char * namespaceURI,
		    TextEncoding encoding) const;

		int GetIntAttribute( const char* name, int defVal = 0 ) const
		{
			int ival;
			if ( sscanf_s( GetAttribute(name, TextEncoding::Application()).c_str(), "%d", &ival ) == 1 )
				return ival;
			return defVal;
		}

		bool GetBoolAttribute( const char* name, bool bDef = false ) const {
			int ival = GetIntAttribute( name, (int)bDef );
			return (ival != 0);
		}

		double GetDoubleAttribute( const char* name, double defVal= 0.0 ) const
		{
			double dval;
			if ( sscanf_s( GetAttribute(name, TextEncoding::Application()).c_str(), "%lf", &dval ) == 1 )
				return dval;
			return defVal;
		}

		float GetFloatAttribute( const char* name, float defVal= 0.0f ) const
		{
			return (float)GetDoubleAttribute(name, defVal);
		}

		template< typename T >
		T GetAttributeValue( const char* name, T defValue ) const
		{
			T outValue = defValue;
			std::istringstream sstream( GetAttribute(name, TextEncoding::Application()).c_str() );
			sstream >> outValue;
			if ( !sstream.fail() )
				return outValue;
			return defValue;
		}

		//@}
		
		/// \name High-level interface
		//@{
		
		/// Moves to the next "interesting" node.
		
		/// "Interesting" nodes are kElement, kEndElement and kText nodes.
		/// If the reader is already on such a node, the function does nothing.
		/// Otherwise, it calls Read() until the reader is either positioned
		/// on an interesting node or the end of input is reached.
		///
		/// \note Many of the other high-level functions begin by calling
		/// MoveToContent, so there is no need to call MoveToContent before
		/// these functions.
		
		NodeType MoveToContent();
		
		/// Reads past the current node or element.
		
		/// If the current node is an non-empty kElement node, this function
		/// positions the reader after the matching kEndElement node.  For
		/// all other nodes, this function is equivalent to Read().
		
		void Skip();
		
		/// Returns the value of one or more consecutive text nodes.

		/// If the current node is a non-empty element node, then
		/// this function calls Read() to move to the next element.
		/// Then the function concatenates together the values of
		/// any text and whitespace nodes that it finds, and returns
		/// that value.  The reader is left pointing at the next
		/// non-textual node.
		///
		/// \warning This function does <b>not</b> recursively 
		/// descend into sub-elements, so it cannot be used to read
		/// the contents of an element if sub-elements may be present.
		
		UnicodeString ReadString();
		
		/// Returns the value of one or more consecutive text nodes.

		/// If the current node is a non-empty element node, then
		/// this function calls Read() to move to the next element.
		/// Then the function concatenates together the values of
		/// any text and whitespace nodes that it finds, and returns
		/// that value.  The reader is left pointing at the next
		/// non-textual node.
		///
		/// \warning This function does <b>not</b> recursively 
		/// descend into sub-elements, so it cannot be used to read
		/// the contents of an element if sub-elements may be present.
		
		std::string ReadString(TextEncoding encoding);
		
		/// Returns true if the next interesting node is an element node.
		
		/// This function calls MoveToContent(), so it automatically skips over
		/// whitespace, comments and processing instructions.
		bool IsStartElement();
		
		/// Returns true if the next interesting node is a element with the specified name.
		
		/// This function calls MoveToContent(), so it automatically skips over
		/// whitespace, comments and processing instructions.
		bool IsStartElement(const char * name);
		
		/// Returns true if the next interesting node is an element with the specified local name and namespace.
		
		/// This function calls MoveToContent(), so it automatically skips over
		/// whitespace, comments and processing instructions.
		bool IsStartElement(const char * localName, const char * namespaceURI);
		
		/// Reads an element node.
		
		/// This function calls MoveToContent(), so it automatically skips over
		/// whitespace, comments and processing instructions.  It then makes sure
		/// that the next interesting node is a kElement node, and reads past it.
		///
		/// \exception XMLException if the next interesting node is not a kElement node

		void ReadStartElement();
		
		/// Reads an element node with the specified name.
		
		/// This function calls MoveToContent(), so it automatically skips over
		/// whitespace, comments and processing instructions.  It then makes sure
		/// that the next interesting node is a kElement node with the specified name,
		/// and reads past it.
		///
		/// \exception XMLException if the next interesting node is not the specified kElement node
		
		void ReadStartElement(const char * name);
		
		/// Reads an element node with the specified local name and namespace.
		
		/// This function calls MoveToContent(), so it automatically skips over
		/// whitespace, comments and processing instructions.  It then makes sure
		/// that the next interesting node is a kElement node with the specified local
		/// name and namespace, and reads past it.
		///
		/// \exception XMLException if the next interesting node is not the specified kElement node
		
		void ReadStartElement(const char * localName, const char * namespaceURI);
		
		/// Reads the next node, return true if the previous node was not empty.
		
		/**
			This function is intended to simplify handling of element nodes that
			may be empty, by simultaneously testing to see if the node is empty and
			moving to the next node.  It can be used to conditionalize the reading
			of the contents of an element node.
			
			\code
	if (reader.IsStartElement("tasks")) {
		if (reader.IsNotEmptyElementRead()) {
			// read contents of "tasks" element
		}
	}
			\endcode
			
			Note that this function should be used with IsStartElement() rather than
			ReadStartElement(), because ReadStartElement will read past the start element
			before IsStartElementRead has had a chance to see if the element is empty.
			
			\sa MoveToSubElement()
		**/
		bool IsNotEmptyElementRead();
		
		bool IsOpenElementRead();
		bool IsOpenElementRead(const char * name);
		bool IsOpenElementRead(const char * localName, const char * namespaceURI);
		
		/// Moves to the next sub-element in the current element.
		
		/**
			This function searches for the next sub-element of the current element,
			skipping over any intervening nodes.  If a sub-element is found, the
			reader is positioned at the start of the sub-element and this function returns
			true.  If the end of the current element is found, the reader is positioned
			after the end element and this function returns false.
			
			This function is useful for looping over the sub-elements of an element.
			If the main element may be empty, be sure to call IsNotEmptyElementRead()
			first to handle that case.
			
			\code
	if (reader.IsStartElement("tasks")) {
		if (reader.IsNotEmptyElementRead()) {
			while (reader.MoveToSubElement()) {
				// parse the sub-element
			}
		}
		// the reader is positioned after the "tasks" element
	}
			\endcode
			
			\return true if a sub-element was found, or false if the end of the current
			element has been reached.
		**/
		bool MoveToSubElement();
		
		/// Reads an end element node, or throws an XMLException.
		void ReadEndElement();
		
		/// Verifies the current start element, and returns the text following that element.
		
		/// This function calls IsStartElement to verify that the next interesting node
		/// is the specified start element.  If it is, the function calls ReadString() to
		/// read the text after the start element.  Otherwise, it throws an XMLException.
		///
		/// \warning This function does <b>not</b> read the end tag at the end of the
		/// element.  Call Read() yourself to move past the end tag.
		
		UnicodeString ReadElementString();
		
		/// Verifies the current start element, and returns the text following that element.
		
		/// This function calls IsStartElement to verify that the next interesting node
		/// is the specified start element.  If it is, the function calls ReadString() to
		/// read the text after the start element.  Otherwise, it throws an XMLException.
		///
		/// \warning This function does <b>not</b> read the end tag at the end of the
		/// element.  Call Read() yourself to move past the end tag.
		
		UnicodeString ReadElementString(const char * name);
		
		/// Verifies the current start element, and returns the text following that element.
		
		/// This function calls IsStartElement to verify that the next interesting node
		/// is the specified start element.  If it is, the function calls ReadString() to
		/// read the text after the start element.  Otherwise, it throws an XMLException.
		///
		/// \warning This function does <b>not</b> read the end tag at the end of the
		/// element.  Call Read() yourself to move past the end tag.
		
		UnicodeString ReadElementString(const char * localName, const char * namespaceURI);

		
		/// Verifies the current start element, and returns the text following that element.
		
		/// This function calls IsStartElement to verify that the next interesting node
		/// is the specified start element.  If it is, the function calls ReadString() to
		/// read the text after the start element.  Otherwise, it throws an XMLException.
		///
		/// \warning This function does <b>not</b> read the end tag at the end of the
		/// element.  Call Read() yourself to move past the end tag.
		
		std::string ReadElementString(TextEncoding encoding);
		
		/// Verifies the current start element, and returns the text following that element.
		
		/// This function calls IsStartElement to verify that the next interesting node
		/// is the specified start element.  If it is, the function calls ReadString() to
		/// read the text after the start element.  Otherwise, it throws an XMLException.
		///
		/// \warning This function does <b>not</b> read the end tag at the end of the
		/// element.  Call Read() yourself to move past the end tag.
		
		std::string ReadElementString(const char * name, TextEncoding encoding);
		
		/// Verifies the current start element, and returns the text following that element.
		
		/// This function calls IsStartElement to verify that the next interesting node
		/// is the specified start element.  If it is, the function calls ReadString() to
		/// read the text after the start element.  Otherwise, it throws an XMLException.
		///
		/// \warning This function does <b>not</b> read the end tag at the end of the
		/// element.  Call Read() yourself to move past the end tag.
		
		std::string ReadElementString(const char * localName, const char * namespaceURI,
		    TextEncoding encoding);
		
		//@}

		static bool IsBaseChar(UnicodeChar c);
		static bool IsIdeographic(UnicodeChar c);
		static bool IsLetter(UnicodeChar c);
		static bool IsDigit(UnicodeChar c);
		static bool IsWhitespace(UnicodeChar c);
		static bool IsCombiningChar(UnicodeChar c);
		static bool IsNameChar(UnicodeChar c);
		static bool IsNameToken(const UnicodeString & token);
		static bool IsName(const UnicodeString & name);
		
		/// Returns true if the Unicode string equals the specified ASCII string.
		
		/// \warning The second argument to this function should contain
		/// only ASCII characters, because this function does not provide
		/// encoding conversion.
		
		static bool Equals(const UnicodeString & a, const char * b);
		
		/// Returns true if the Unicode string starts with a specified ASCII string.
		
		/// \warning The \a needle argument to this function should contain
		/// only ASCII characters, because this function does not provide
		/// encoding conversion.
		
		static bool StartsWith(const UnicodeChar * haystack, const char * needle);

	private:
		struct UniCharRange {
			UnicodeChar	mLowChar;
			UnicodeChar	mHighChar;
		};

		static const UniCharRange sBaseCharRanges[];
		static const UniCharRange sCombiningCharRanges[];
		static const UniCharRange sDigitRanges[];

		struct Name {
			void SetName(const char * name);
			void SetName(const UnicodeChar * name);
			void SetName(const UnicodeString & name);
			void DivideName();
			void Clear();
		
			UnicodeString	mName;
			UnicodeString	mPrefix;
			UnicodeString	mLocalName;
			UnicodeString	mNamespaceURI;
		};

		struct Tag: public Name {
			size_t			mPreviousMappingSize;
		};

		struct Attribute: public Name {
			UnicodeString	mValue;
		};

		struct NamespaceMapping {
			UnicodeString	mPrefix;
			UnicodeString	mNamespaceURI;
		};

	private:
		bool ReadInternal();
		void FillInputBuffer();
		void FillOutputBuffer();
		size_t ConvertInput(char * outputBuffer, size_t len);

		bool BufferStartsWith(const char * prefix);
		bool StartsWithWhitespace();

		bool ParseElement();
		bool ParseEndElement();
		bool ParseOptionalWhitespace();
		bool ParseRequiredWhitespace();
		bool ParseXmlDeclaration();
		bool ParseAttribute(Attribute & attribute);
		bool ParseAttValue(UnicodeString & value);
		bool ParseEq();
		bool ParseName(Name & name);
		bool ParseReference(UnicodeString & value);
		bool ParseString(const char * s);
		bool ParseText();
		bool ParseComment();
		bool ParseCommentText();

		UnicodeChar PeekChar();
		UnicodeChar ReadChar();
		bool ParseChar(UnicodeChar c);

		void LookupNamespace(Name & name) const;
		size_t PushNamespaces();
		void PopNamespaces(size_t n);
		void PushTag();
		void PopTag();

		static bool IsInRange(UnicodeChar c, const UniCharRange ranges[],
			size_t n);
	
	private:
		// This private class is used to ensure that the mSkippingContent flag
		// is restored even if an exception is thrown while content is being
		// skipped.
		class SkippingContent {
		public:
			SkippingContent(XMLReader & reader);
			~SkippingContent();
		private:
			XMLReader &	mReader;
			bool		mWasSkippingContent;
		};
		friend class SkippingContent;

	private:
		static UnicodeString	sEmptyUniCharString;
		
		static const size_t kInputBufferCount = 512;
		static const size_t kOutputBufferCount = 512;
		
		WhitespaceHandling              mWhitespaceHandling;

		InputStream &					mInput;
		TextEncoding					mDocumentEncoding;
		char *							mInputBuffer;
		char *							mInputStart;
		char *							mInputEnd;
		UnicodeChar	*					mOutputBuffer;
		UnicodeChar *					mOutputStart;
		UnicodeChar *					mOutputEnd;

		NodeType						mNodeType;
		Name							mCurrentName;
		UnicodeString					mValue;
		std::vector<Attribute>			mAttributes;
		std::vector<NamespaceMapping>	mNamespaceMappings;
		std::vector<Tag>				mOpenTags;
		bool							mIsEmptyElement;

		ConvertToUnicode				mConverter;
		bool							mSkipContent;
	};

}


#endif
