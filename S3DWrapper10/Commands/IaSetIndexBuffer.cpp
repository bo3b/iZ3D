#include "stdafx.h"
#include "IaSetIndexBuffer.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void IaSetIndexBuffer::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		// only mono
		pWrapper->OriginalDeviceFuncs.pfnIaSetIndexBuffer(pWrapper->hDevice, GET_RESOURCE_HANDLE(hBuffer_), Format_, Offset_);
		AFTER_EXECUTE(this); 

		Bd().SetLastResources(pWrapper,CommandId,hBuffer_.pDrvPrivate ? &hBuffer_ : 0,1,0);
	}

	void IaSetIndexBuffer::UpdateDeviceState( D3DDeviceWrapper *pWrapper, D3DDeviceState* pState )
	{
		pState->m_IaIndexBuffer = this;
	}

	bool IaSetIndexBuffer::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "IaSetIndexBuffer" );
		WriteStreamer::Reference( "hBuffer", hBuffer_ );
		WriteStreamer::Value( "Format", Format_ );
		WriteStreamer::Value( "Offset", Offset_ );
		WriteStreamer::CmdEnd();
		return true;		
	}

	bool IaSetIndexBuffer::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL }; 
		ReadStreamer::CmdBegin( hDevice);
		ReadStreamer::Reference( hBuffer_);
		ReadStreamer::Value( Format_);
		ReadStreamer::Value( Offset_);
		ReadStreamer::CmdEnd();
		return true;
	}

	void IaSetIndexBuffer::BuildCommand(CDumpState *ds)
	{
		if (hBuffer_.pDrvPrivate)
		{
			output("ctx->IASetIndexBuffer(buffer_%d,%s,%d);",
				ds->GetHandleEventId(hBuffer_),EnumToString(Format_),Offset_);
		}
		else
		{
			output("ctx->IASetIndexBuffer(0,%s,%d);",EnumToString(Format_),Offset_);
		}
	}

}

VOID ( APIENTRY IaSetIndexBuffer )( D3D10DDI_HDEVICE  hDevice, D3D10DDI_HRESOURCE  hBuffer, 
								   DXGI_FORMAT  Format, UINT  Offset )
{	
#ifndef EXECUTE_IMMEDIATELY_G1
	Commands::IaSetIndexBuffer* command = new Commands::IaSetIndexBuffer(hBuffer, Format, Offset);
	D3D10_GET_WRAPPER()->AddCommand(command);
#else
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnIaSetIndexBuffer(D3D10_DEVICE, UNWRAP_CV_RESOURCE_HANDLE(hBuffer), Format, Offset);
	}
#endif
}