#include "stdafx.h"
#include "CreateGeometryShader.h"
#include "..\Streamer\CodeGenerator.h"
#include "..\Streamer\ResourceManager.h"
#include "D3DDeviceWrapper.h"
#include "ShaderWrapper.h"

namespace Commands
{

	void CreateGeometryShader::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnCreateGeometryShader(pWrapper->hDevice,
			pCode_, GET_SHADER_HANDLE(hShader_), hRTShader_, &Signatures_);
		AFTER_EXECUTE(this); 
	}

	bool CreateGeometryShader::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "CreateGeometryShader" );
		WriteStreamer::Reference( "hShader", hShader_ );
		WriteStreamer::Handle( "hRTShader", hRTShader_.handle );
		WriteStreamer::Value( "Signatures", Signatures_ );

		if ( GINFO_DUMP_ON )
		{
			int pos = Bd().SaveToBinaryFile(fullCode,fullSize);
			WriteStreamer::Value( "data.bin", pos );
		}

		WriteStreamer::CmdEnd();

/*#ifndef FINAL_RELEASE
		if (GINFO_DUMP_ON)
		{
			CShader *p = (CShader*)GET_RESOURCE(hShader_);
			p->SetArgCode(pCode_);
			p->SetArgSignatures(&Signatures_);
			p->SaveShaderData(pWrapper->m_ResourceManager.pXMLStreamer);

			if( gInfo.DumpInitialData )
				pWrapper->m_ResourceManager.SaveInitialResourceData( hShader_, pWrapper, EventID_ );
		}
#endif*/

		return true;
	}

	bool CreateGeometryShader::ReadFromFile	()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Reference( hShader_ );
		ReadStreamer::Handle( hRTShader_ );
		ReadStreamer::Value( Signatures_ );
		ReadStreamer::Value( databin_pos );
		ReadStreamer::CmdEnd();
		return true;
	}

	void CreateGeometryShader::BuildCommand(CDumpState *ds)
	{
		fullCode = (char *)ds->LoadFromBinaryFile(databin_pos,&fullSize);

		set_output_target(init);
		output("char *shader_data_%d = GetFromFile2(%d);",EventID_,ds->ResaveBinaryData(databin_pos));

		output(glob,"DXGSHADER *gshader_%d;",EventID_);
		output("V_(device->CreateGeometryShader(shader_data_%d,%d DXOPT1 ,&gshader_%d));",EventID_,fullSize,EventID_);

		ds->AddHandleEventId(hShader_,EventID_);
	}

}

VOID (APIENTRY  CreateGeometryShader)(D3D10DDI_HDEVICE  hDevice, CONST UINT*  pCode, D3D10DDI_HSHADER  hShader, 
									  D3D10DDI_HRTSHADER  hRTShader, CONST D3D10DDIARG_STAGE_IO_SIGNATURES*  pSignatures)
{
	ShaderWrapper* pWrp;
	InitWrapper(hShader, pWrp);
	CREATE_SHADER_WRAPPER();
	pWrp->InitializeGS(D3D10_GET_WRAPPER(), pCode, pSignatures);
#ifndef EXECUTE_IMMEDIATELY_G2
	Commands::CreateGeometryShader* command = new Commands::CreateGeometryShader(pCode, hShader, hRTShader, pSignatures);
	D3D10_GET_WRAPPER()->AddCommand(command, true );
#else
	D3D10_GET_WRAPPER()->ProcessCB();
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnCreateGeometryShader(D3D10_DEVICE, pCode, GET_SHADER_HANDLE(hShader), hRTShader, pSignatures);
	}
#endif
}