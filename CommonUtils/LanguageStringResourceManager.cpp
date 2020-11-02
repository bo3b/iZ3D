#include "StdAfx.h"
#include "LanguageStringResourceManager.h"
#include <xercesc\parsers\XercesDOMParser.hpp>
#include <xercesc\util\XMLUni.hpp>
#include <xercesc\dom\DOM.hpp>
#include <set>
#include <algorithm>

#ifndef _DEBUG
#define _SECURE_SCL 0
#endif

using namespace std;

iz3d::resources::LanguageStringResourceManager::LanguageStringResourceManager(
	const wchar_t* languageFileName )
	: languageFileName_( languageFileName ), pDictionary_(DNew TDictionary() )
{
	try
	{
		xercesc::XMLPlatformUtils::Initialize();
	}
	catch( xercesc::XMLException& )
	{
	}
	
	xercesc::XercesDOMParser* pParser = new xercesc::XercesDOMParser();
	pParser->parse( languageFileName );
	xercesc::DOMDocument* pXmlDoc = pParser->getDocument();
	if (!pXmlDoc)
	{
		delete pParser;
		xercesc::XMLPlatformUtils::Terminate();
		return;
	}

	xercesc::DOMElement* pRootElement = pXmlDoc->getDocumentElement();
	//pDictionary_ = DNew TDictionary();
	FillDictionary( pRootElement );
	try
	{
		delete pParser;
		xercesc::XMLPlatformUtils::Terminate();
	}
	catch ( xercesc::XMLException& )
	{
	}
}

iz3d::resources::LanguageStringResourceManager::~LanguageStringResourceManager()
{
	delete pDictionary_;
}

const wchar_t* iz3d::resources::LanguageStringResourceManager::getString( const wchar_t* resName )
{
	std::wstring name = resName;
	TDictionary::iterator it = pDictionary_->find( name );
	TDictionary::iterator itEnd = pDictionary_->end();
	if ( it == itEnd )
		return 0;
	return it->second.c_str();
}

//////////////////////////////////////////////////////////////////////////
void iz3d::resources::LanguageStringResourceManager::FillDictionary(
	xercesc::DOMElement* pRootElement )
{
	ProcessSubnode( L"", pRootElement );
}

//////////////////////////////////////////////////////////////////////////
void iz3d::resources::LanguageStringResourceManager::ProcessSubnode( const wchar_t* name, xercesc::DOMNode* pNode )
{
	xercesc::DOMNodeList* pNodeList = pNode->getChildNodes();
	if( pNode->getNodeType() != xercesc::DOMNode::ELEMENT_NODE )
	{
		TDictionary::iterator it = pDictionary_->end();
		if (! pDictionary_->empty() )
			it = pDictionary_->find( name );
		TDictionary::iterator itEnd = pDictionary_->end();
		wstring datum = pNode->getNodeValue();
		if ( wstring( name ).size() &&
			wstring( pNode->getNodeValue() ).size() )
		{
			if ( it == itEnd )
			{
				pDictionary_->insert(TDictionary::value_type(name, datum) );
			}
		}
		return;
	}

	for( XMLSize_t i = 0; i < pNodeList->getLength(); ++i )
	{
		xercesc::DOMNode* pCurNode = pNodeList->item( i );
		wstring nodeName = name;
		if( pCurNode->getNodeType() != xercesc::DOMNode::TEXT_NODE )
		{
			nodeName += L"\\";
			nodeName += pCurNode->getNodeName();
		}
		ProcessSubnode( nodeName.c_str(), pCurNode );
	}
}

const std::wstring iz3d::resources::LanguageStringResourceManager::getAllLettersString() const
{
	std::set<wchar_t> retSet;
	for( auto it = pDictionary_->begin(); it != pDictionary_->end(); it++ )
	{
		retSet.insert( it->second.begin(), it->second.end() );
	}
	const wchar_t numericsStr[] = L"0123456789";
	retSet.insert( numericsStr, numericsStr + _countof( numericsStr ) );
	std::wstring toRet( retSet.begin(), retSet.end() );
	std::sort( toRet.begin(), toRet.end() );
	return toRet;
}