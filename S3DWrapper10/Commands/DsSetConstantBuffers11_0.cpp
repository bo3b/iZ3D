#include "stdafx.h"
#include "DsSetConstantBuffers11_0.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void DsSetConstantBuffers11_0::Execute( D3DDeviceWrapper *pWrapper )
	{
		if (CurrentShaderData_[SP_DS])
			SetMatrices(pWrapper, CurrentShaderData_[SP_DS], CurrentShaderCRC32_);

		for (UINT i = 0; i < NumValues_; i++)
		{
			if (Values_[i].pDrvPrivate)
			{
				DEBUG_TRACE3(L"\t\t DS CB %d: ", i);
				phTempBuffers_[i] = UNWRAP_DS_CB_HANDLE(pWrapper, Offset_ + i, Values_[i]);
			}
			else
				phTempBuffers_[i].pDrvPrivate = NULL;
		}

		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs11.pfnDsSetConstantBuffers( pWrapper->hDevice,
			Offset_, 
			NumValues_, 
			phTempBuffers_
			);
		AFTER_EXECUTE(this); 
	}

	bool DsSetConstantBuffers11_0::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		return WriteToFileEx(pWrapper, "DsSetConstantBuffers11_0");
	}
}

VOID ( APIENTRY DsSetConstantBuffers11_0 )(
									   D3D10DDI_HDEVICE  hDevice, 
									   UINT  StartBuffer, 
									   UINT  NumBuffers, 
									   CONST D3D10DDI_HRESOURCE*  phBuffers
									   )
{
	_ASSERT(D3D10_GET_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
#ifndef EXECUTE_IMMEDIATELY_G1
	Commands::DsSetConstantBuffers11_0* command = new Commands::DsSetConstantBuffers11_0( StartBuffer, NumBuffers, phBuffers );
	D3D10_GET_WRAPPER()->AddCommand(command);
#else
#ifdef DUMP_ENABLED
	command->WriteToFile( hDevice );
#endif
	Commands::fast_vector<D3D10DDI_HRESOURCE>::type hRESOURCEBuffer(NumBuffers);
	for (UINT i = 0; i < NumBuffers; i++)
		hRESOURCEBuffer[i] = UNWRAP_DS_CB_HANDLE(D3D10_GET_WRAPPER(), i, phBuffers[i]);
	{
		THREAD_GUARD_D3D10();
		D3D11_GET_ORIG().pfnDsSetConstantBuffers( D3D10_DEVICE, StartBuffer, NumBuffers, GetPointerToVector( hRESOURCEBuffer ) );
	}
#endif
}

