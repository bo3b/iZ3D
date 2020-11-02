#include "stdafx.h"
#include "CmdFlowStreamers.h"
#include "XMLStreamer.h"

//////////////////////////////////////////////////////////////////////////

namespace WriteStreamer
{
	CmdFlowWriteStreamer& Get()
	{
		return boost::details::pool::singleton_default<CmdFlowWriteStreamer>::instance();
	}

	bool IsOK()
	{
		return Get().IsWriterOK();
	}

	void	CmdBegin( D3D10DDI_HDEVICE hDevice_, Commands::Command* pCmd_, const char* szName_  )
	{
		Get().WriteCommandBegin( pCmd_, szName_ );
		Get().WriteReference( "hDevice", hDevice_ );
	}

	void	CmdEnd()
	{
		Get().WriteCommandEnd();
	}

	void	DeviceStateBegin()
	{
		Get().DeviceStateBlockBegin();
	}

	void	DeviceStateEnd()
	{
		Get().DeviceStateBlockEnd();
	}

	void	Comment	( char* szComment_, ... )
	{
#ifndef FINAL_RELEASE
		if (!IsOK())
			return;

		static char szBuff[ MAX_PATH ];	

		if( szComment_ != "" )
		{
			va_list args;
			va_start( args, szComment_ );
			vsprintf( szBuff, szComment_, args );
			va_end( args );
		}
		else
			szBuff[0] = '\0';			

		Get().WriteComment( szBuff );
#endif
	}
};

//////////////////////////////////////////////////////////////////////////

namespace ReadStreamer
{
	CmdFlowReadStreamer& Get()
	{
		return boost::details::pool::singleton_default<CmdFlowReadStreamer>::instance();
	}

	void	CmdBegin( D3D10DDI_HDEVICE& hDevice_  )
	{
		Get().ReadReference( hDevice_ );
	}

	void	CmdEnd()
	{
	}

	void	Skip()
	{
		Get().Skip( );
	}
};

//////////////////////////////////////////////////////////////////////////