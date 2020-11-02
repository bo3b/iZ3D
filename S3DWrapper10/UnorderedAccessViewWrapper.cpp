#include "StdAfx.h"
#include "UnorderedAccessViewWrapper.h"
#include "D3DDeviceWrapper.h"

UnorderedAccessViewWrapper::UnorderedAccessViewWrapper( D3DDeviceWrapper* pWrapper, CONST D3D11DDIARG_CREATEUNORDEREDACCESSVIEW *pCreateUnorderedAccessView )
{
	m_hLeftHandle.pDrvPrivate = NULL;
	m_hRightHandle.pDrvPrivate = NULL;
	View = *pCreateUnorderedAccessView;

	ResourceWrapper* pResWrp; 
	InitWrapper(pCreateUnorderedAccessView->hDrvResource, pResWrp);
	if (pResWrp->IsStereo())
	{
		SIZE_T size = pWrapper->OriginalDeviceFuncs11.pfnCalcPrivateUnorderedAccessViewSize(pWrapper->hDevice, pCreateUnorderedAccessView);
		m_hRightHandle.pDrvPrivate = DNew BYTE[size];
		if (m_hRightHandle.pDrvPrivate)
		{
			D3D11DDIARG_CREATEUNORDEREDACCESSVIEW createUnorderedAccessView = *pCreateUnorderedAccessView;
			createUnorderedAccessView.hDrvResource = pResWrp->m_hRightHandle;
			D3D11DDI_HRTUNORDEREDACCESSVIEW hUAVRightView = { NULL };
			pWrapper->OriginalDeviceFuncs11.pfnCreateUnorderedAccessView(pWrapper->hDevice, &createUnorderedAccessView, m_hRightHandle, hUAVRightView);
		}
	}
}

void UnorderedAccessViewWrapper::Destroy( D3DDeviceWrapper* pWrapper )
{
	if (m_hRightHandle.pDrvPrivate)
	{
		pWrapper->OriginalDeviceFuncs11.pfnDestroyUnorderedAccessView(pWrapper->hDevice, m_hRightHandle);
		delete[] m_hRightHandle.pDrvPrivate;
		m_hRightHandle.pDrvPrivate = NULL;
	}
}