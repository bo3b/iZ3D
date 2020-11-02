#include "stdafx.h"
#include "xxSetShaderWithIfaces11_0.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{
	bool xxSetShaderWithIfaces11_0::WriteToFileEx( D3DDeviceWrapper *pWrapper, const char* szCommandName ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  szCommandName );
		WriteStreamer::Reference( "hShader", hShader_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool xxSetShaderWithIfaces11_0::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Reference( hShader_ );
		ReadStreamer::CmdEnd();
		return true;
	}

	void xxSetShaderWithIfaces11_0::BuildCommand( CDumpState *ds )
	{
		// TODO
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
