#include "stdafx.h"
#include "CreateComputeShader11_0.h"
#include "..\Streamer\ResourceManager.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"
#include "ShaderWrapper.h"

namespace Commands
{

	void CreateComputeShader11_0::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs11.pfnCreateComputeShader(pWrapper->hDevice, 
			pCode_, GET_SHADER_HANDLE(hShader_), hRTShader_);
		AFTER_EXECUTE(this); 
	}

	bool CreateComputeShader11_0::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "CreateComputeShader11_0" );
		//WriteStreamer::Handle( "pCode", pCode_ ); // TODO: Fix it
		WriteStreamer::Reference( "hShader", hShader_ );
		WriteStreamer::Handle( "hRTShader", hRTShader_.handle );

		if ( GINFO_DUMP_ON )
		{
			int pos = Bd().SaveToBinaryFile(fullCode,fullSize);
			WriteStreamer::Value( "data.bin", pos );
		}

		WriteStreamer::CmdEnd();

#ifndef FINAL_RELEASE
		/*if (GINFO_DUMP_ON)
		{
			CShader *p = (CShader*)GET_RESOURCE(hShader_);
			p->SetArgCode(pCode_);
			//p->SetArgSignatures(&Signatures_);
			p->SaveShaderData(pWrapper->m_ResourceManager.pXMLStreamer);

			if( gInfo.DumpInitialData )
				pWrapper->m_ResourceManager.SaveInitialResourceData( hShader_, pWrapper, EventID_ );
		}*/
#endif

		return true;
	}

	bool CreateComputeShader11_0::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		//it++; // ?!!
		ReadStreamer::Reference( hShader_ );
		ReadStreamer::Handle( hRTShader_ );

		ReadStreamer::Value( databin_pos );

		ReadStreamer::CmdEnd();
		return true;
	}

	void CreateComputeShader11_0::BuildCommand(CDumpState *ds)
	{
		fullCode = (char *)ds->LoadFromBinaryFile(databin_pos,&fullSize);

		output(glob,"ID3D11ComputeShader *cshader_%d;",EventID_);

		output(init,"char *shader_data_%d = GetFromFile2(%d);",EventID_,ds->ResaveBinaryData(databin_pos));
		output(init,"V_(device->CreateComputeShader(shader_data_%d,%d,0,&cshader_%d));",EventID_,fullSize,EventID_);

		ds->AddHandleEventId(hShader_,EventID_);
	}

}

VOID (APIENTRY  CreateComputeShader11_0)(D3D10DDI_HDEVICE  hDevice, CONST UINT*  pCode, D3D10DDI_HSHADER  hShader, 
									D3D10DDI_HRTSHADER  hRTShader)
{
	_ASSERT(D3D10_GET_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
	ShaderWrapper* pWrp;
	InitWrapper(hShader, pWrp);
	CREATE_SHADER_WRAPPER();
	pWrp->InitializeCS(D3D10_GET_WRAPPER(), pCode);
#ifndef EXECUTE_IMMEDIATELY_G2
	Commands::CreateComputeShader11_0* command = new Commands::CreateComputeShader11_0(pCode, hShader, hRTShader);
	D3D10_GET_WRAPPER()->AddCommand(command, true );
#else
	D3D10_GET_WRAPPER()->ProcessCB();
	{
		THREAD_GUARD_D3D10();
		D3D11_GET_ORIG().pfnCreateComputeShader(D3D10_DEVICE, pCode, GET_SHADER_HANDLE(hShader), hRTShader);
	}
#endif
}
