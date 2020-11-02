import xml.dom.minidom

class XmlScheme2CppClassWriter:
	# Constructor
	def __init__( self,\
				schemaFileName,\
				hFileName,\
				cppFileName,\
				baseClassNodeName,\
				baseClassName,\
				xmlDataFileName ):
		self.document =  xml.dom.minidom.parse( schemaFileName )
		self.hFile = open( hFileName, 'w' )
		self.cppFile = open ( cppFileName, 'w' )
		self.baseClassNodeName = baseClassNodeName
		self.baseClassName = baseClassName
		self.xmlDataFileName = xmlDataFileName
	
	def __del__(self):
		self.hFile.close()
		self.cppFile.close()

	# Table function xsd to c++ type converter
	def convXSDTypeToCppType ( type ):
		return {'xs:integer':'int',
		'xs:decimal':'float',
		'xs:boolean':'bool',
		'xs:string':'std::wstring'}[type]
		
	# Writes numTabs tabs
	def writeTabs( self, numTabs ):
		for i in range( 0, numTabs ):
			self.hFile.write('\t')
				
	# Writes class header "class className {"
	def writeClassHeader( self, numTabs, className ):
		self.writeTabs( numTabs )
		self.hFile.write( 'class ' + className + ' {\n' )
		self.writeTabs( numTabs )
		self.hFile.write( 'public:\n')
		self.writeTabs( numTabs + 1 )
		self.hFile.write( 'void Init( xercesc::DOMNode* pNode );\n' )

	# Writes attribute as terminal class field
	def writeAttribute( self, numTabs, field ):
		self.writeTabs( numTabs )
		cppType = XmlScheme2CppClassWriter.convXSDTypeToCppType( field.attributes['type'].value )
		self.hFile.write( cppType + ' ' + field.attributes['name'].value + ';\n')
		self.cppFile.write( '\t' + field.attributes['name'].value + ' = ' +\
						'boost::lexical_cast< ' + cppType + ' >( pNodeMap->getNamedItem( L"' +\
						field.attributes['name'].value +'" )->getNodeValue() );\n')
	
	# Writes sequence as subclass
	def writeSequence( self, numTabs, sequence):
		elements = sequence.getElementsByTagName('xs:element')
		self.cppFile.write( '\txercesc::DOMNodeList* pBufNodes = 0;\n')
		for element in elements:
			self.writeTabs( numTabs )
			className = element.attributes['ref'].value
			self.cppFile.write( '\tpBufNodes = pNode->getOwnerDocument()->getElementsByTagName( L"' + className + '" );\n')
			if 'maxOccurs' in element.attributes:
				self.hFile.write( 'std::vector<' + className + '> ' )
				self.hFile.write( 'vec' + className + ';\n')
				self.cppFile.write( '\tvec' + className + '.resize( pBufNodes->getLength() );\n')
				self.cppFile.write( '\tfor ( size_t i = 0; i < pBufNodes->getLength(); ++i )\n\t{\n\t\tvec'\
								 + className + '[i].Init( pBufNodes->item( i ) );\n\t}\n' )
			else:
				self.hFile.write( className + ' m_' + className + ';\n' )
				self.cppFile.write( '\tm_' + className + '.Init( pBufNodes->item( 0 ) );\n' )

	# Writes body of class
	def writeClassBody( self, numTabs, classNode ):
		complexElement = classNode.getElementsByTagName( 'xs:complexType' )[0]
		allAttributes = complexElement.getElementsByTagName( 'xs:attribute' )
		sequence = complexElement.getElementsByTagName( 'xs:sequence' )
		all = complexElement.getElementsByTagName( 'xs:all' )
		for attribute in allAttributes:
			self.writeAttribute( numTabs + 1, attribute )
		if len(sequence) > 0:
			self.writeSequence( numTabs + 1, sequence[0] )
		if len(all) > 0:
			self.writeSequence( numTabs + 1, all[0] )

	# writes };
	def writeClassEnd( self, numTabs ):
		self.writeTabs( numTabs )
		self.hFile.write( '};\n')

	# processing class/subclass node
	def processClass( self, numTabs, classNode, className, classConstructorPrefix ):
		self.writeClassHeader( numTabs, className )
		
		subClasses = XmlScheme2CppClassWriter.getSubclassesList( classNode )
		#self.writeSubclassesList( numTabs + 1, subClasses )
		
		elements = self.document.getElementsByTagName('xs:element')
		for element in elements:
			if 'name' in element.attributes:
				if element.attributes['name'].value in subClasses:
					self.processClass( numTabs + 1, element, element.attributes['name'].value, classConstructorPrefix + className + '::' )
		# write cpp Init function
		self.cppFile.write( 'void ' + classConstructorPrefix + className + '::Init( xercesc::DOMNode* pNode ) {\n')
		self.cppFile.write('\txercesc::DOMNamedNodeMap* pNodeMap = pNode->getAttributes();\n')
		self.writeClassBody( numTabs, classNode )
		
		self.cppFile.write( '}\n\n' )
		if className == self.baseClassName:
		    self.writeTabs( numTabs )
		    self.hFile.write( self.baseClassName + '();\n' )
		    #self.writeBaseClassConstructor()
		self.writeClassEnd( numTabs )
	
	# Finds element node with given attribute 'name'
	def findClassDescriptionNode( elementNodes, classDescriptionNodeName ):
		for el in elementNodes:
			if 'name' in el.attributes:
				if el.attributes["name"].value == classDescriptionNodeName:
					return el
		return None

	# Writes subclass declarations before subclass definitions
	def writeSubclassesList( self, numTabs, subClasses ):
		if len(subClasses) == 0:
			return
		self.writeTabs(numTabs)
		self.hFile.write( '// subclasses definitions\n' )
		for name in subClasses:
			self.writeTabs( numTabs )
			self.hFile.write('class ' + name + ';\n')
		self.writeTabs(numTabs)
		self.hFile.write( '/////////////////////////\n\n' )

	# returns subclasses names
	def getSubclassesList ( baseClassNode ):
		retList = [];
		elements = baseClassNode.getElementsByTagName('xs:element')
		for element in elements:
			if 'ref' in element.attributes:
				retList.append( element.attributes['ref'].value )
		return retList
	
	# writes *.h file header
	def writeHFileHeader(self):
		self.hFile.write("#pragma once\n")
		self.hFile.write("\n")
		self.hFile.write("#include <string>\n")
		self.hFile.write("#include <vector>\n")
		self.hFile.write('#include <xercesc/parsers/XercesDOMParser.hpp>\n#include <xercesc/util/XMLUni.hpp>\n#include <xercesc/dom/DOM.hpp>\n\n')
		self.hFile.write("namespace iz3d {\n")
		self.hFile.write("\tnamespace profile {\n")
		
	# writes *.cpp file header
	def writeCPPFileHeader(self):
		self.cppFile.write('#include "stdafx.h"\n')
		self.cppFile.write('#include <boost/lexical_cast.hpp>\n')
		self.cppFile.write('#include "' + self.hFile.name + '"\n\n')
	
	def writeBaseClassConstructor(self):
		self.cppFile.write('iz3d::profile::' + self.baseClassName + '::' + self.baseClassName + '()\n{\n')
		self.cppFile.write('\ttry\n\t{\n\t\txercesc::XMLPlatformUtils::Initialize();\n\t}\n')
		self.cppFile.write('\tcatch( xercesc::XMLException& ){}\n')
		self.cppFile.write('\txercesc::XercesDOMParser* pParser = new xercesc::XercesDOMParser();\n')
		self.cppFile.write('\tpParser->parse( L"'+self.xmlDataFileName + '" );\n')
		self.cppFile.write('\txercesc::DOMDocument* pXmlDoc = pParser->getDocument();\n')
		self.cppFile.write('\txercesc::DOMNode* startNode = pXmlDoc->getElementsByTagName( L"'\
		 + self.baseClassNodeName + '" )->item( 0 );\n')
		self.cppFile.write('\tInit( startNode );\n')
		self.cppFile.write('\ttry\n')
		self.cppFile.write('\t{\n')
		self.cppFile.write('\t\txercesc::XMLPlatformUtils::Terminate();\n')
		self.cppFile.write('\t}\n')
		self.cppFile.write('\tcatch ( xercesc::XMLException& ){}\n}')
		
	# main function generates class header from scheme
	def generateClassFromScheme( self ):
		self.writeHFileHeader()
		self.writeCPPFileHeader()
		
		elementNodes = self.document.getElementsByTagName('xs:element')
		baseClassNode = XmlScheme2CppClassWriter.findClassDescriptionNode( elementNodes, self.baseClassNodeName )
		if baseClassNode == None:
			return
		
		self.processClass( 2, baseClassNode, self.baseClassName, 'iz3d::profile::' )
		self.hFile.write("\t}\n")
		self.hFile.write("}\n")
		self.writeBaseClassConstructor()
		
if __name__ == "__main__":
	converter = XmlScheme2CppClassWriter( "BaseProfile.xsd", "GlobalInfo.h", "GlobalInfo.cpp", "DefaultProfile", "GlobalInfo", "BaseProfile.xml" )
	converter.generateClassFromScheme()