#include "stdafx.h"
#include "CreateElementLayout.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"
#include "CreateVertexShader.h"

namespace Commands
{

	void CreateElementLayout::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnCreateElementLayout(pWrapper->hDevice,
			&CreateElementLayout_, hElementLayout_, hRTElementLayout_);		
		AFTER_EXECUTE(this); 
	}

	bool CreateElementLayout::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "CreateElementLayout" );
		WriteStreamer::Value( "CreateElementLayout", CreateElementLayout_ );
		WriteStreamer::Reference( "hElementLayout", hElementLayout_ );
		WriteStreamer::Handle( "hRTElementLayout", hRTElementLayout_.handle );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool  CreateElementLayout::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Value( CreateElementLayout_ );
		ReadStreamer::Reference( hElementLayout_ );
		ReadStreamer::Handle( hRTElementLayout_ );
		ReadStreamer::CmdEnd();
		return true;
	}

	void CreateElementLayout::BuildCommand(CDumpState *ds)
	{
		set_output_target(init);

		std::set<CreateVertexShader *>shaders = ds->GetShadersOfLayout(this);
		if (!shaders.size())
		{
			output("// skip_%d",EventID_);
			return;
		}

		CreateVertexShader *shader = 0;
		for(auto it = shaders.begin();it != shaders.end();++it)
		{
			if ((*it)->nInputs == CreateElementLayout_.NumElements)
			{
				shader = *it;
				break;
			}
			else if (!shader || (*it)->nInputs > shader->nInputs)
			{
				shader = *it;
			}
		}

		unsigned realcnt = shader->nInputs;
		if (realcnt != CreateElementLayout_.NumElements)
		{
			if (CreateElementLayout_.NumElements < realcnt)
			{
				output("// warning! REQUIRED *%d* INPUTS, BUT FOUND %d",CreateElementLayout_.NumElements,realcnt);
			}
			else
			{
				output("#error(REQUIRED *%d* INPUTS, BUT FOUND %d)",CreateElementLayout_.NumElements,realcnt);
			}
		}

		if (CreateElementLayout_.NumElements > 0)
		{
			DWORD *p = shader->GetInputsHeader();

			output("static DXELEMENTDESC elementDesc_%d[%d];",EventID_,CreateElementLayout_.NumElements);
			for(unsigned i = 0;i < CreateElementLayout_.NumElements;i++)
			{
				const D3D10DDIARG_INPUT_ELEMENT_DESC *el = CreateElementLayout_.pVertexElements + i;

				if (i >= realcnt)
				{
					output("elementDesc_%d[%d].SemanticName = ""; // ?",EventID_,i);
					output("elementDesc_%d[%d].SemanticIndex = 0; // ?",EventID_,i);
				}
				else
				{
					DWORD offset = *(p + 4 + 6 * i);
					DWORD index  = *(p + 4 + 6 * i + 1);
					char *name = (char *)(p + 2) + offset;

					output("elementDesc_%d[%d].SemanticName = \"%s\";",EventID_,i,name);
					output("elementDesc_%d[%d].SemanticIndex = %d;",EventID_,i,index);
				}
				output("elementDesc_%d[%d].Format = %s;",EventID_,i,EnumToString(el->Format));
				output("elementDesc_%d[%d].InputSlot = %d;",EventID_,i,el->InputSlot);
				output("elementDesc_%d[%d].AlignedByteOffset = %d;",EventID_,i,el->AlignedByteOffset);
				output("elementDesc_%d[%d].InputSlotClass = (DXINPUTCLASSIF)%d;",EventID_,i,el->InputSlotClass);
				output("elementDesc_%d[%d].InstanceDataStepRate = %d;",EventID_,i,el->InstanceDataStepRate);
			}

			output(decl,"extern DXLAYOUT *vertexLayout_%d;",EventID_);
			output(glob,"DXLAYOUT *vertexLayout_%d;",EventID_);
			output("V_(device->CreateInputLayout(elementDesc_%d, %d, shader_data_%d,%d, &vertexLayout_%d ));",
				EventID_,CreateElementLayout_.NumElements,shader->EventID_,shader->GetFullCodeSize(),EventID_);
		}
		else
		{
			output(decl,"extern DXLAYOUT *vertexLayout_%d;",EventID_);
			output(glob,"DXLAYOUT *vertexLayout_%d = 0;",EventID_);
		}

		ds->AddHandleEventId(hElementLayout_,EventID_);
	}

}

VOID ( APIENTRY CreateElementLayout )( D3D10DDI_HDEVICE  hDevice, 
									  CONST D3D10DDIARG_CREATEELEMENTLAYOUT*  pCreateElementLayout, 
									  D3D10DDI_HELEMENTLAYOUT  hElementLayout, D3D10DDI_HRTELEMENTLAYOUT  hRTElementLayout )
{
#ifndef EXECUTE_IMMEDIATELY_G2
	Commands::CreateElementLayout* command = new Commands::CreateElementLayout(pCreateElementLayout,
		hElementLayout, hRTElementLayout);
	D3D10_GET_WRAPPER()->AddCommand(command, true );
#else
	D3D10_GET_WRAPPER()->ProcessCB();
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnCreateElementLayout(D3D10_DEVICE, pCreateElementLayout,
			hElementLayout, hRTElementLayout);
	}
#endif
}