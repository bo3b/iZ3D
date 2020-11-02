#include "stdafx.h"
#include "PsSetConstantBuffers.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void PsSetConstantBuffers::Execute( D3DDeviceWrapper *pWrapper )
	{
		if (CurrentPixelShaderData_[SP_PS])
			SetMatrices(pWrapper, CurrentPixelShaderData_[SP_PS], CurrentShaderCRC32_);

		for (UINT i = 0; i < NumValues_; i++)
			phTempBuffers_[i] = UNWRAP_PS_CB_HANDLE(pWrapper, Offset_ + i, Values_[i]);

		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnPsSetConstantBuffers( pWrapper->hDevice,
			Offset_, 
			NumValues_, 
			phTempBuffers_
			);
		AFTER_EXECUTE(this); 

		SetLastResources(pWrapper);
	}

	bool PsSetConstantBuffers::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		return WriteToFileEx(pWrapper, "PsSetConstantBuffers");
	}
}

VOID ( APIENTRY PsSetConstantBuffers )(
									   D3D10DDI_HDEVICE  hDevice, 
									   UINT  StartBuffer, 
									   UINT  NumBuffers, 
									   CONST D3D10DDI_HRESOURCE*  phBuffers
									   )
{
#ifndef EXECUTE_IMMEDIATELY_G1
	Commands::PsSetConstantBuffers* command = new Commands::PsSetConstantBuffers( StartBuffer, NumBuffers, phBuffers );
	D3D10_GET_WRAPPER()->AddCommand(command);
#else
#ifdef DUMP_ENABLED
	command->WriteToFile( hDevice );
#endif
	Commands::fast_vector<D3D10DDI_HRESOURCE>::type hRESOURCEBuffer(NumBuffers);
	for (UINT i = 0; i < NumBuffers; i++)
		hRESOURCEBuffer[i] = UNWRAP_PS_CB_HANDLE(D3D10_GET_WRAPPER(), i, phBuffers[i]);
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnPsSetConstantBuffers( D3D10_DEVICE, StartBuffer, NumBuffers, GetPointerToVector( hRESOURCEBuffer ) );
	}
#endif
}

