#include "stdafx.h"
#include "CreateGeometryShaderWithStreamOutput.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"
#include "ShaderWrapper.h"

namespace Commands
{

	void CreateGeometryShaderWithStreamOutput::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnCreateGeometryShaderWithStreamOutput(pWrapper->hDevice,
			&CreateGeometryWithShaderOutput_, GET_SHADER_HANDLE(hShader_), hRTShader_, &Signatures_);
		AFTER_EXECUTE(this); 
	}

	bool CreateGeometryShaderWithStreamOutput::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "CreateGeometryShaderWithStreamOutput" );
		WriteStreamer::Value( "CreateGeometryWithShaderOutput", CreateGeometryWithShaderOutput_ );
		WriteStreamer::Reference( "hShader", hShader_ );
		WriteStreamer::Handle( "hRTShader", hRTShader_.handle );
		WriteStreamer::Value( "Signatures", Signatures_ );
		WriteStreamer::CmdEnd();

#ifndef FINAL_RELEASE
		/*if (GINFO_DUMP_ON)
		{
			CShader *p = (CShader*)GET_RESOURCE(hShader_);
			p->SetArgCode(CreateGeometryWithShaderOutput_.pShaderCode);
			p->SetArgSignatures(&Signatures_);
			p->SaveShaderData(pWrapper->m_ResourceManager.pXMLStreamer);

			if( gInfo.DumpInitialData )
				pWrapper->m_ResourceManager.SaveInitialResourceData( hShader_, pWrapper, EventID_ );
		}*/
#endif

		return true;
	}

	bool CreateGeometryShaderWithStreamOutput::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Value( CreateGeometryWithShaderOutput_ );
		ReadStreamer::Reference( hShader_ );
		ReadStreamer::Handle( hRTShader_ );
		ReadStreamer::Value( Signatures_ );
		ReadStreamer::CmdEnd();
		return true;
	}

	void CreateGeometryShaderWithStreamOutput::BuildCommand(CDumpState *ds)
	{
		ds->AddHandleEventId(hShader_,EventID_);
		output(decl,"extern ID3D10GeometryShader *gshader_%d;",EventID_);
		output(glob,"ID3D10GeometryShader *gshader_%d = 0;",EventID_);
	}
}

VOID (APIENTRY  CreateGeometryShaderWithStreamOutput)(D3D10DDI_HDEVICE  hDevice, 
													  CONST D3D10DDIARG_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT*  pCreateGeometryWithShaderOutput, 
													  D3D10DDI_HSHADER  hShader, D3D10DDI_HRTSHADER  hRTShader, CONST D3D10DDIARG_STAGE_IO_SIGNATURES*  pSignatures)
{
	_ASSERT(D3D10_GET_WRAPPER()->GetD3DVersion() == TD3DVersion_10_0 ||
		D3D10_GET_WRAPPER()->GetD3DVersion() == TD3DVersion_10_1);
	ShaderWrapper* pWrp;
	InitWrapper(hShader, pWrp);
	CREATE_SHADER_WRAPPER();
	pWrp->InitializeGSWithSO(D3D10_GET_WRAPPER(), pCreateGeometryWithShaderOutput, pSignatures);
#ifndef EXECUTE_IMMEDIATELY_G2
	Commands::CreateGeometryShaderWithStreamOutput* command = new Commands::CreateGeometryShaderWithStreamOutput(
		pCreateGeometryWithShaderOutput, hShader, hRTShader, pSignatures);
	D3D10_GET_WRAPPER()->AddCommand(command, true );
#else
	D3D10_GET_WRAPPER()->ProcessCB();
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnCreateGeometryShaderWithStreamOutput(D3D10_DEVICE, 
			pCreateGeometryWithShaderOutput, GET_SHADER_HANDLE(hShader), hRTShader, pSignatures);
	}
#endif
}