#include "stdafx.h"
#include "CreatePixelShader.h"
#include "..\Streamer\CodeGenerator.h"
#include "..\Streamer\ResourceManager.h"
#include "D3DDeviceWrapper.h"
#include "ShaderWrapper.h"

namespace Commands
{

	void CreatePixelShader::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnCreatePixelShader(pWrapper->hDevice, 
			pCode_, GET_SHADER_HANDLE(hShader_), hRTShader_, &Signatures_);
		AFTER_EXECUTE(this); 
	}

	bool CreatePixelShader::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "CreatePixelShader" );
		WriteStreamer::Reference( "hShader", hShader_ );
		WriteStreamer::Handle( "hRTShader", hRTShader_.handle );
		WriteStreamer::Value( "Signatures", Signatures_ );

		if ( GINFO_DUMP_ON )
		{
			int pos = Bd().SaveToBinaryFile(fullCode,fullSize);
			WriteStreamer::Value( "data.bin", pos );
			WriteStreamer::Value( "bad_code", 0 );
		}

		WriteStreamer::CmdEnd();

#ifndef FINAL_RELEASE
		/*if (GINFO_DUMP_ON)
		{
			CShader *p = (CShader*)GET_RESOURCE(hShader_);
			p->SetArgCode(pCode_);
			p->SetArgSignatures(&Signatures_);
			p->SaveShaderData(pWrapper->m_ResourceManager.pXMLStreamer);

			if( gInfo.DumpInitialData )
				pWrapper->m_ResourceManager.SaveInitialResourceData( hShader_, pWrapper, EventID_ );
		}*/
#endif

		return true;
	}

	bool CreatePixelShader::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL }; 
		ReadStreamer::CmdBegin( hDevice);

		ReadStreamer::Reference( hShader_ );
		ReadStreamer::Handle( hRTShader_);
		ReadStreamer::Value( Signatures_);

		ReadStreamer::Value( databin_pos );

		BOOL bad_code;
		ReadStreamer::Value( bad_code );

		ReadStreamer::CmdEnd();

		return true;
	}

	void CreatePixelShader::BuildCommand(CDumpState *ds)
	{
		fullCode = (char *)ds->LoadFromBinaryFile(databin_pos,&fullSize);

		output(glob,"DXPSHADER *pshader_%d;",EventID_);

		output(init,"char *shader_data_%d = GetFromFile2(%d);",EventID_,ds->ResaveBinaryData(databin_pos));
		output(init,"V_(device->CreatePixelShader(shader_data_%d,%d DXOPT1 ,&pshader_%d));",EventID_,fullSize,EventID_);

		ds->AddHandleEventId(hShader_,EventID_);
	}

}

VOID (APIENTRY  CreatePixelShader)(D3D10DDI_HDEVICE  hDevice, CONST UINT*  pCode, D3D10DDI_HSHADER  hShader, 
								   D3D10DDI_HRTSHADER  hRTShader, CONST D3D10DDIARG_STAGE_IO_SIGNATURES*  pSignatures)
{
	ShaderWrapper* pWrp;
	InitWrapper(hShader, pWrp);
	CREATE_SHADER_WRAPPER();		
	pWrp->InitializePS(D3D10_GET_WRAPPER(), pCode, pSignatures);
#ifndef EXECUTE_IMMEDIATELY_G2
	Commands::CreatePixelShader* command = new Commands::CreatePixelShader(pCode, hShader, hRTShader, pSignatures);
	D3D10_GET_WRAPPER()->AddCommand(command, true );
#else
	D3D10_GET_WRAPPER()->ProcessCB();
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnCreatePixelShader(D3D10_DEVICE, pCode, GET_SHADER_HANDLE(hShader), hRTShader, pSignatures);
	}
#endif
}