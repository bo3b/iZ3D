#include "stdafx.h"
#include "DestroyShader.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"
#include "ShaderWrapper.h"

namespace Commands
{

	void DestroyShader::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnDestroyShader(pWrapper->hDevice, GET_SHADER_HANDLE(hShader_));
		AFTER_EXECUTE(this); 
	}

	bool DestroyShader::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "DestroyShader" );
		WriteStreamer::Reference( "hShader", hShader_ );
		WriteStreamer::CmdEnd();
		DESTROY_RESOURCE(hShader_);
		return true;
	}

	bool DestroyShader::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Reference( hShader_ );
		ReadStreamer::CmdEnd();
		return true;
	}

}

VOID ( APIENTRY DestroyShader )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HSHADER hShader )
{
	ShaderWrapper* pWrp;
	InitWrapper(hShader, pWrp);
#ifndef EXECUTE_IMMEDIATELY_G2
	Commands::DestroyShader* command = new Commands::DestroyShader( hShader );
	D3D10_GET_WRAPPER()->AddCommand( command, true );
#else
	D3D10_GET_WRAPPER()->ProcessCB();
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnDestroyShader( D3D10_DEVICE, GET_SHADER_HANDLE(hShader) );
	}
#endif
	DESTROY_SHADER_WRAPPER();
}