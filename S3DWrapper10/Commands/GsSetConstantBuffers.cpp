#include "stdafx.h"
#include "GsSetConstantBuffers.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void GsSetConstantBuffers::Execute( D3DDeviceWrapper *pWrapper )
	{
		if (CurrentShaderData_[SP_GS]) {
			SetMatrices(pWrapper, CurrentShaderData_[SP_GS], CurrentShaderCRC32_);
		}

		for (UINT i = 0; i < NumValues_; i++)
		{
			if (Values_[i].pDrvPrivate)
			{
				DEBUG_TRACE3(L"\t\t GS CB %d: ", i);
				phTempBuffers_[i] = UNWRAP_GS_CB_HANDLE(pWrapper, Offset_ + i, Values_[i]);
			}
			else
				phTempBuffers_[i].pDrvPrivate = NULL;
		}

		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnGsSetConstantBuffers(pWrapper->hDevice, Offset_, NumValues_, phTempBuffers_);
		AFTER_EXECUTE(this); 

		SetLastResources(pWrapper);
	}

	bool GsSetConstantBuffers::WriteToFile( D3DDeviceWrapper * pWrapper ) const
	{
		return WriteToFileEx(pWrapper, "GsSetConstantBuffers");
	}
}

VOID ( APIENTRY GsSetConstantBuffers )( D3D10DDI_HDEVICE  hDevice, UINT  StartBuffer, UINT  NumBuffers, 
									   CONST D3D10DDI_HRESOURCE*  phBuffers )
{	
#ifndef EXECUTE_IMMEDIATELY_G1
	Commands::GsSetConstantBuffers* command = new Commands::GsSetConstantBuffers(StartBuffer, NumBuffers, phBuffers);
	D3D10_GET_WRAPPER()->AddCommand(command);
#else
	Commands::fast_vector<D3D10DDI_HRESOURCE>::type hRESOURCEBuffer(NumBuffers);
	for (UINT i = 0; i < NumBuffers; i++)
		hRESOURCEBuffer[i] = UNWRAP_GS_CB_HANDLE(D3D10_GET_WRAPPER(), i, phBuffers[i]);
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnGsSetConstantBuffers(D3D10_DEVICE, StartBuffer, NumBuffers, GetPointerToVector(hRESOURCEBuffer));
	}
#endif
}
