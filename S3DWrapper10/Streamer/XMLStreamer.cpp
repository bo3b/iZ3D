#include "stdafx.h"
#include "../Commands/Command.h"
#include "XMLStreamer.h"
#include <LlamaXML\XMLException.h>
#include "EnumToString.h"
#include <string>

using namespace LlamaXML;

/************************************************************************/
/* XMLStreamer                                                          */
/************************************************************************/
XMLStreamer::XMLStreamer( TXMLFileMode mode )
:	m_pXMLWriter			( NULL )
,	m_pXMLReader			( NULL )
,	m_pOutputStream			( NULL )
,	m_pInputStream			( NULL )
,	m_pCurrCmd				( NULL )
,	m_pCurrentRM			( NULL )
,	m_tFileMode				( mode )
,	m_bIterateAllCmd		( false )
,	m_nLastSuccessfulEID	( -1 )
{
	QueryPerformanceFrequency( &m_Freq );
}

bool XMLStreamer::OpenFile( const wchar_t* fileName, const char* szComment /* = NULL  */ )
{
	try
	{
		if ( m_tFileMode == TXML_OpenForWrite )
		{	
			m_pOutputStream = new FileOutputStream(fileName);
			m_pXMLWriter = new XMLWriter(*m_pOutputStream, LlamaXML::TextEncoding::System());
			m_pXMLWriter->StartDocument("1.0", NULL, NULL);

			_ASSERT( szComment );
			m_pXMLWriter->StartElement( szComment );

			m_pXMLWriter->WriteAttribute( "appName", std::wstring(gInfo.ApplicationName) );			
		}
		else
		{
			m_pInputStream = new FileInputStream(fileName);
			m_pXMLReader = new XMLReader(*m_pInputStream, LlamaXML::TextEncoding::System());
			m_pXMLReader->ReadStartElement(szComment);

			m_appName = m_pXMLReader->GetAttribute("appName",TextEncoding::Application());
		}
		return true;
	}
	catch ( LlamaXML::XMLException /*ex*/ )
	{
		return false;
	}
}

/************************************************************************/
/* ~XMLStreamer                                                         */
/************************************************************************/
XMLStreamer::~XMLStreamer()
{
	if ( m_pXMLWriter )
	{
		m_pXMLWriter->EndDocument();
		delete m_pXMLWriter;
		m_pXMLWriter = NULL;
	}
	if ( m_pOutputStream )
	{
		delete m_pOutputStream;
		m_pOutputStream = NULL;
	}
	if ( m_pXMLReader )
	{
		delete m_pXMLReader;
		m_pXMLReader = NULL;
	}
	if ( m_pInputStream )
	{
		delete m_pInputStream;
		m_pInputStream = NULL;
	}
}

bool XMLStreamer::IsWriterOK()
{
#ifndef FINAL_RELEASE
	return m_pXMLWriter != NULL && m_tFileMode == TXML_OpenForWrite;
#else
	return false;
#endif
}

/************************************************************************/
/* SetCurrentRM                                                         */
/************************************************************************/
void XMLStreamer::SetCurrentRM( ResourceManager * pNewRM )
{
	m_pCurrentRM = pNewRM;
}

/************************************************************************/
/* GetCurrentRM                                                         */
/************************************************************************/
ResourceManager* XMLStreamer::GetCurrentRM() const
{
	return m_pCurrentRM;
}

/************************************************************************/
/* WriteCommandBegin                                                    */
/************************************************************************/
void XMLStreamer::WriteCommandBegin	( Commands::Command* pCmd, const char* szCommandName )
{
	_ASSERT( m_tFileMode == TXML_OpenForWrite );
	_ASSERT( m_pXMLWriter );
	_ASSERT( m_pCurrCmd == NULL );
	_ASSERT( pCmd );
	m_pCurrCmd = pCmd;
	m_pXMLWriter->StartElement( "COMMAND" );
#ifndef FINAL_RELEASE
	m_pXMLWriter->WriteAttribute( "eid", m_pCurrCmd->EventID_ );
	m_pXMLWriter->WriteAttribute( "cid", m_pCurrCmd->CommandId );
	m_pXMLWriter->WriteAttribute( "name", szCommandName );
#ifdef ENABLE_TIMING
	m_pXMLWriter->WriteAttribute( "start_time",  (int)(m_pCurrCmd->StartTime_.QuadPart * 1000 / m_Freq.QuadPart) );
#endif
#else
	m_pXMLWriter->WriteAttribute( "cid", m_pCurrCmd->CommandId );
#endif	
}

/************************************************************************/
/* WriteCommandEnd                                                      */
/************************************************************************/
void XMLStreamer::WriteCommandEnd()
{
	_ASSERT( m_tFileMode == TXML_OpenForWrite );
	_ASSERT( m_pCurrCmd );
#ifdef ENABLE_TIMING
	/*m_pXMLWriter->StartElement( "Duration" );
	m_pXMLWriter->WriteAttribute( "val", (int)(m_pCurrCmd->Duration_.QuadPart * 1000 / m_Freq.QuadPart) );
	m_pXMLWriter->EndElement( );*/
#endif
	m_pXMLWriter->EndElement();
	m_pCurrCmd = NULL;
}

/************************************************************************/
/* WriteComment															*/
/************************************************************************/
void XMLStreamer::WriteComment( const char* Val )
{
	m_pXMLWriter->WriteComment( Val );
}

/************************************************************************/
/* StartNewElement                                                        */
/************************************************************************/
void XMLStreamer::StartNewElement( const char* szName )
{
	m_pXMLWriter->StartElement(szName);
}

/************************************************************************/
/* EndElement                                                        */
/************************************************************************/
void XMLStreamer::EndElement( )
{
	m_pXMLWriter->EndElement();
}

/************************************************************************/
/* Skip                                                      */ 
/************************************************************************/
void XMLStreamer::Skip()
{
	while (m_pXMLReader->GetNodeType() != XMLReader::kElement)
		m_pXMLReader->Skip();
	m_pXMLReader->Skip();
}

/************************************************************************/
/* ReadNextCommand                                                      */ 
/************************************************************************/
Commands::Command* XMLStreamer::ReadNextCommand( std::string& _sError )
{
	Commands::Command* pNewCmd = NULL;
#ifndef FINAL_RELEASE
	_ASSERT( m_tFileMode == TXML_OpenForRead );
	_ASSERT( m_pXMLReader );

	if ( m_bIterateAllCmd )	
		return NULL;

	std::string name	= "";
	int nCID			= 0;
	int nEID			= 0;	

	try
	{
		m_pXMLReader->ReadStartElement("COMMAND");

		nCID = m_pXMLReader->GetIntAttribute("cid");
		nEID = m_pXMLReader->GetIntAttribute("eid");
		name = m_pXMLReader->GetAttribute("name", TextEncoding::Application());		

		pNewCmd = Commands::Command::CreateCommandFromCID( nCID );		
		_ASSERT( pNewCmd );
		_ASSERT( pNewCmd->CommandId == nCID );
		//pNewCmd->EventID_ = nEID;
		pNewCmd->ReadFromFile();

		if (m_pXMLReader->IsStartElement())
		{
			m_pXMLReader->ReadStartElement("Duration");
			if (!m_pXMLReader->IsEmptyElement())
				m_pXMLReader->ReadEndElement();
		}
		
		m_pXMLReader->ReadEndElement();

		if ( m_pXMLReader->EndOfFile() || 
			(m_pXMLReader->MoveToContent() == XMLReader::kEndElement))
		{
			m_bIterateAllCmd	= true;		
		}
		m_nLastSuccessfulEID = nEID;
	}
	catch ( LlamaXML::XMLException ex )
	{	
		printf("\nProbably incorrect implementation ReadFromFile() for %s",CommandIDToString((CommandsId)nCID));
		_ASSERT(FALSE);
		pNewCmd = NULL;
	}
#endif
	return pNewCmd;
}

/************************************************************************/
/* ReadStartElement		                                                    */ 
/************************************************************************/
void XMLStreamer::ReadStartElement( ) const
{
	m_pXMLReader->ReadStartElement();
}

/************************************************************************/
/* ReadEndElement		                                                    */ 
/************************************************************************/
void XMLStreamer::ReadEndElement( ) const
{
	m_pXMLReader->ReadEndElement();
}

/************************************************************************/
/* FreeAllocatedMem                                                     */
/************************************************************************/
void XMLStreamer::FreeAllocatedMem()
{
	std::list< CTypePtr* >::iterator it = m_vAllocatedMem.begin();
	for( ; it != m_vAllocatedMem.end(); ++it )
	{
		CTypePtr* ptr = *it;
		ptr->MemFree();
		delete ptr;
	}
}

void XMLStreamer::DeviceStateBlockBegin()
{
	WriteComment("Device States Begin");
}

void XMLStreamer::DeviceStateBlockEnd()
{
	WriteComment("Device States End");
}
