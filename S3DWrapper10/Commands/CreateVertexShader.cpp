#include "stdafx.h"
#include "CreateVertexShader.h"
#include "..\Streamer\ResourceManager.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"
#include "ShaderWrapper.h"

namespace Commands
{

	DWORD *CreateVertexShader::GetInputsHeader()
	{
		DWORD *p = (DWORD *)fullCode;
		for(int i = 0;;i++)
		{
			if (i > 10000) { __debugbreak(); break; }
			const DWORD sig = 0x4e475349;
			if (*p == sig) break;
			p++;
		}
		return p;
	}

	void CreateVertexShader::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnCreateVertexShader(pWrapper->hDevice, 
			pCode_, GET_SHADER_HANDLE(hShader_), hRTShader_, &Signatures_);
		AFTER_EXECUTE(this); 
	}

	bool CreateVertexShader::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "CreateVertexShader" );

		WriteStreamer::Reference( "hShader", hShader_ );
		WriteStreamer::Handle( "hRTShader", hRTShader_.handle );
		WriteStreamer::Value( "Signatures", Signatures_ );

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
			p->SetArgSignatures(&Signatures_);
			p->SaveShaderData(pWrapper->m_ResourceManager.pXMLStreamer);

			if( gInfo.DumpInitialData )
				pWrapper->m_ResourceManager.SaveInitialResourceData( hShader_, pWrapper, EventID_ );
		}*/
#endif

		return true;
	}

	bool CreateVertexShader::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL }; 
		ReadStreamer::CmdBegin( hDevice);

		ReadStreamer::Reference( hShader_);
		ReadStreamer::Handle( hRTShader_);
		ReadStreamer::Value( Signatures_);

		ReadStreamer::Value( databin_pos );
		fullCode = (char *)CDumpState::LoadFromBinaryFile(databin_pos,&fullSize); // should be loaded BEFORE BuildCommand

		ReadStreamer::CmdEnd();

		// retrieve shader inputs count
		DWORD *p = GetInputsHeader();
		nInputs = *(p + 2);

		return true;
	}

	void CreateVertexShader::BuildCommand(CDumpState *ds)
	{
		output(decl,"extern DXVSHADER *vshader_%d;",EventID_);
		output(glob,"DXVSHADER *vshader_%d;",EventID_);

		set_output_target(init);
		output(decl,"extern char *shader_data_%d;",EventID_);
		output(glob,"char *shader_data_%d = GetFromFile2(%d);",EventID_,ds->ResaveBinaryData(databin_pos));
		output("V_(device->CreateVertexShader(shader_data_%d,%d DXOPT1 ,&vshader_%d));",EventID_,fullSize,EventID_);

		ds->AddHandleEventId(hShader_,EventID_);
	}

}

VOID (APIENTRY  CreateVertexShader)(D3D10DDI_HDEVICE  hDevice, CONST UINT*  pCode, D3D10DDI_HSHADER  hShader, 
									D3D10DDI_HRTSHADER  hRTShader, CONST D3D10DDIARG_STAGE_IO_SIGNATURES*  pSignatures)
{	
	ShaderWrapper* pWrp;
	InitWrapper(hShader, pWrp);
	CREATE_SHADER_WRAPPER();
	pWrp->InitializeVS(D3D10_GET_WRAPPER(), pCode, hRTShader, pSignatures);
#ifndef EXECUTE_IMMEDIATELY_G2
	Commands::CreateVertexShader* command = new Commands::CreateVertexShader(pCode, hShader, hRTShader, pSignatures);
	D3D10_GET_WRAPPER()->AddCommand(command, true );
#else
	D3D10_GET_WRAPPER()->ProcessCB();
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnCreateVertexShader(D3D10_DEVICE, pCode, GET_SHADER_HANDLE(hShader), hRTShader, pSignatures);
	}
#endif
}
