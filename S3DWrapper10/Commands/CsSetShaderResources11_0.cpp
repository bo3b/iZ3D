#include "stdafx.h"
#include "CsSetShaderResources11_0.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"
#include "ShaderResourceViewWrapper.h"

namespace Commands
{

	void CsSetShaderResources11_0::Execute( D3DDeviceWrapper *pWrapper )
	{
		for (UINT i = 0; i < NumValues_; i++)
		{
			if (Values_[i].pDrvPrivate)
			{
#ifndef FINAL_RELEASE
				ShaderResourceViewWrapper* pSRV;
				InitWrapper(Values_[i], pSRV);
				ResourceWrapper* pResWrp = pSRV->GetResourceWrapper();
				int resID = pWrapper->m_ResourceManager.GetID(pResWrp->GetHandle());
				DEBUG_TRACE3(L"\t\t CS SRV %d: %s%d", i, pResWrp->GetResourceName(), resID);
#endif
				phTempShaderResourceViews_[i] = UNWRAP_CV_SRV_HANDLE(Values_[i]);
			}
			else
				phTempShaderResourceViews_[i].pDrvPrivate = NULL;
		}

		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs11.pfnCsSetShaderResources( pWrapper->hDevice,
			Offset_, 
			NumValues_, 
			phTempShaderResourceViews_	
			);
		AFTER_EXECUTE(this); 
	}

	bool CsSetShaderResources11_0::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		return WriteToFileEx(pWrapper, "CsSetShaderResources11_0");
	}
}

VOID ( APIENTRY CsSetShaderResources11_0 )(
									   D3D10DDI_HDEVICE  hDevice, 
									   UINT  Offset, 
									   UINT  NumViews, 
									   CONST D3D10DDI_HSHADERRESOURCEVIEW*  phShaderResourceViews
									   )
{
	_ASSERT(D3D10_GET_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
#ifndef EXECUTE_IMMEDIATELY_G1
	Commands::CsSetShaderResources11_0* command = new Commands::CsSetShaderResources11_0( Offset, NumViews, phShaderResourceViews );
	D3D10_GET_WRAPPER()->AddCommand(command);
#else
	Commands::fast_vector<D3D10DDI_HSHADERRESOURCEVIEW>::type  SRVBuffer(NumViews);
	for (UINT i = 0; i < NumViews; i++)
		SRVBuffer[i] = UNWRAP_CV_SRV_HANDLE(phShaderResourceViews[i]);
	{
		THREAD_GUARD_D3D10();
		D3D11_GET_ORIG().pfnCsSetShaderResources( D3D10_DEVICE, Offset, NumViews, GetPointerToVector(SRVBuffer) );
	}
#endif	
}