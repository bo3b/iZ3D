#include "StdAfx.h"
#include "DepthStencilViewWrapper.h"
#include "D3DDeviceWrapper.h"

DepthStencilViewWrapper::DepthStencilViewWrapper( D3DDeviceWrapper* pWrapper, CONST D3D10DDIARG_CREATEDEPTHSTENCILVIEW *pCreateDepthStencilView )
{
	m_hLeftHandle.pDrvPrivate = NULL;
	m_hRightHandle.pDrvPrivate = NULL;
	View = *pCreateDepthStencilView;

	ResourceWrapper* pResWrp;		
	InitWrapper(View.hDrvResource, pResWrp);
	if (pResWrp->IsStereo())
	{
		SIZE_T size = pWrapper->OriginalDeviceFuncs.pfnCalcPrivateDepthStencilViewSize(pWrapper->hDevice, pCreateDepthStencilView);
		m_hRightHandle.pDrvPrivate = DNew BYTE[size];
		if (m_hRightHandle.pDrvPrivate)
		{
			D3D10DDIARG_CREATEDEPTHSTENCILVIEW createDepthStencilView = *pCreateDepthStencilView;
			createDepthStencilView.hDrvResource = pResWrp->m_hRightHandle;
			D3D10DDI_HRTDEPTHSTENCILVIEW hRTRightView = { NULL };
			pWrapper->OriginalDeviceFuncs.pfnCreateDepthStencilView(pWrapper->hDevice, &createDepthStencilView, 
				m_hRightHandle, hRTRightView);
		}
	}
}

DepthStencilViewWrapper::DepthStencilViewWrapper( D3DDeviceWrapper* pWrapper, CONST D3D11DDIARG_CREATEDEPTHSTENCILVIEW *pCreateDepthStencilView )
{
	m_hLeftHandle.pDrvPrivate = NULL;
	m_hRightHandle.pDrvPrivate = NULL;
	View11 = *pCreateDepthStencilView;

	ResourceWrapper* pResWrp;		
	InitWrapper(pCreateDepthStencilView->hDrvResource, pResWrp);
	if (pResWrp->IsStereo())
	{
		SIZE_T size = pWrapper->OriginalDeviceFuncs11.pfnCalcPrivateDepthStencilViewSize(pWrapper->hDevice, pCreateDepthStencilView);
		m_hRightHandle.pDrvPrivate = DNew BYTE[size];
		if (m_hRightHandle.pDrvPrivate)
		{
			D3D11DDIARG_CREATEDEPTHSTENCILVIEW createDepthStencilView = *pCreateDepthStencilView;
			createDepthStencilView.hDrvResource = pResWrp->m_hRightHandle;
			D3D10DDI_HRTDEPTHSTENCILVIEW hRTRightView = { NULL };
			pWrapper->OriginalDeviceFuncs11.pfnCreateDepthStencilView(pWrapper->hDevice, &createDepthStencilView, 
				m_hRightHandle, hRTRightView);
		}
	}
}

void DepthStencilViewWrapper::Destroy( D3DDeviceWrapper* pWrapper )
{
	if (m_hRightHandle.pDrvPrivate)
	{
		pWrapper->OriginalDeviceFuncs.pfnDestroyDepthStencilView(pWrapper->hDevice, m_hRightHandle);
		delete[] m_hRightHandle.pDrvPrivate;
		m_hRightHandle.pDrvPrivate = NULL;
	}
}