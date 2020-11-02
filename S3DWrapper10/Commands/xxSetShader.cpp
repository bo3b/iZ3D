#include "stdafx.h"
#include "xxSetShader.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{
	bool xxSetShader::WriteToFileEx( D3DDeviceWrapper *pWrapper, const char* szCommandName ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  szCommandName );
		WriteStreamer::Reference( "hShader", hShader_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool xxSetShader::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Reference( hShader_ );
		ReadStreamer::CmdEnd();
		return true;
	}

	void xxSetShader::BuildCommand( CDumpState *ds )
	{
		char c = Char();
		if (hShader_.pDrvPrivate)
		{
			output("ctx->%cSSetShader(%cshader_%d DXOPT);",c,tolower(c),ds->GetHandleEventId(hShader_));
		}
		else
		{
			output("ctx->%cSSetShader(0 DXOPT);",c);
		}
	}
}
