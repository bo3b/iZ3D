#include "StdAfx.h"
#include "ProfileVarRepository.h"
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/dom/DOM.hpp>
#include <algorithm>
#include <fstream>

using namespace xercesc;
//////////////////////////////////////////////////////////////////////////
void iz3d::profile::ProfileVarRepository::RegisterProfileVar( iz3d::profile::AProfileVar* pVar )
{
	varList_.push_back( pVar );
}

// utility class
class SaveDiff{
	xercesc::DOMDocument* pDoc_;
public:
	SaveDiff( xercesc::DOMDocument* pDoc ) : pDoc_( pDoc ){}
	void operator () ( const iz3d::profile::AProfileVar* pVar ){
		if ( !pVar->isNewEqualOld() )
			pVar->SaveNewValue( pDoc_ );
	}
};
//////////////////////////////////////////////////////////////////////////
void iz3d::profile::ProfileVarRepository::SaveDifferences( const std::wstring& filePath ) const
{
	BOOST_TRY {
		XMLPlatformUtils::Initialize();
	}
	BOOST_CATCH( ... ){

	}
	BOOST_CATCH_END
	DOMImplementation *pImpl = DOMImplementation::getImplementation();
	xercesc::DOMDocument* pXmlDoc = pImpl->createDocument();
	std::for_each( varList_.begin(), varList_.end(), SaveDiff( pXmlDoc ) );
	DOMLSSerializer* pSer = pImpl->createLSSerializer();
	BOOST_TRY{
		pSer->writeToURI( pXmlDoc, filePath.c_str() );
	}
	BOOST_CATCH( ... ){

	}
	BOOST_CATCH_END
	
	BOOST_TRY{
		pSer->release();
		pXmlDoc->release();
		XMLPlatformUtils::Terminate();
	}
	BOOST_CATCH( ... ){

	}
	BOOST_CATCH_END
}

// utility class
class LoadDiff{
	xercesc::DOMDocument* pDoc_;
public:
	LoadDiff( xercesc::DOMDocument* pDoc ) : pDoc_( pDoc ){}
	void operator () ( iz3d::profile::AProfileVar* pVar ){
		pVar->LoadNewValue( pDoc_ );
	}
};

void iz3d::profile::ProfileVarRepository::LoadDifferences( const std::wstring& filePath )
{
	BOOST_TRY {
		XMLPlatformUtils::Initialize();
		XercesDOMParser* pParser = DNew XercesDOMParser();
		pParser->parse( filePath.c_str() );
		if( pParser->getDocument() )
			std::for_each( varList_.begin(), varList_.end(), LoadDiff( pParser->getDocument() ) );
		delete pParser;
		XMLPlatformUtils::Terminate();
	}
	BOOST_CATCH( XMLException& ex )	{

	}
	BOOST_CATCH ( DOMException& ex ) {

	}
	BOOST_CATCH_END
}

void iz3d::profile::ProfileVarRepository::UnregisterProfileVar( iz3d::profile::AProfileVar* pVar )
{
	varList_.remove( pVar );
}