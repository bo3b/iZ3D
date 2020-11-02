#include "stdafx.h"
#include "VsSetConstantBuffers.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void VsSetConstantBuffers::Execute( D3DDeviceWrapper *pWrapper )
	{
		if (CurrentModifiedShaderData_[SP_VS]) {
			SetMatrices(pWrapper, CurrentModifiedShaderData_[SP_VS], CurrentShaderCRC32_);
		} 
		else if (CurrentShaderData_[SP_VS]) {
			SetMatrices(pWrapper, CurrentShaderData_[SP_VS], CurrentShaderCRC32_);
		}

		for (UINT i = 0; i < NumValues_; i++)
		{
			if (Values_[i].pDrvPrivate)
			{
				DEBUG_TRACE3(L"\t\t VS CB %d: ", i);
				phTempBuffers_[i] = UNWRAP_VS_CB_HANDLE(pWrapper, Offset_ + i, Values_[i]);
			}
			else
				phTempBuffers_[i].pDrvPrivate = NULL;
		}

		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnVsSetConstantBuffers(pWrapper->hDevice, Offset_, NumValues_, phTempBuffers_);
		AFTER_EXECUTE(this); 

		SetLastResources(pWrapper);
	}

	bool VsSetConstantBuffers::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		return WriteToFileEx(pWrapper, "VsSetConstantBuffers");
	}
}


VOID ( APIENTRY VsSetConstantBuffers )( D3D10DDI_HDEVICE  hDevice, UINT  StartBuffer, UINT  NumBuffers, 
									   CONST D3D10DDI_HRESOURCE*  phBuffers )
{
#ifndef EXECUTE_IMMEDIATELY_G1
	Commands::VsSetConstantBuffers* command = new Commands::VsSetConstantBuffers(StartBuffer, 
		NumBuffers, phBuffers);
	D3D10_GET_WRAPPER()->AddCommand(command);
#else
	Commands::fast_vector<D3D10DDI_HRESOURCE>::type hRESOURCEBuffer(NumBuffers);
	for (UINT i = 0; i < NumBuffers; i++)
		hRESOURCEBuffer[i] = UNWRAP_VS_CB_HANDLE(D3D10_GET_WRAPPER(), i, phBuffers[i]);

	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnVsSetConstantBuffers(D3D10_DEVICE, StartBuffer, 
			NumBuffers, GetPointerToVector( hRESOURCEBuffer ) );
	}
#endif
}
