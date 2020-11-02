#include "StdAfx.h"
#include "RenderTargetViewWrapper.h"
#include "D3DDeviceWrapper.h"

RenderTargetViewWrapper::RenderTargetViewWrapper( D3DDeviceWrapper* pWrapper, CONST D3D10DDIARG_CREATERENDERTARGETVIEW *pCreateRenderTargetView )
{
	m_hLeftHandle.pDrvPrivate = NULL;
	m_hRightHandle.pDrvPrivate = NULL;
	View = *pCreateRenderTargetView;

	ResourceWrapper* pResWrp; 
	InitWrapper(pCreateRenderTargetView->hDrvResource, pResWrp);
	if (pResWrp->IsStereo())
	{
		SIZE_T size = pWrapper->OriginalDeviceFuncs.pfnCalcPrivateRenderTargetViewSize(pWrapper->hDevice, pCreateRenderTargetView);
		m_hRightHandle.pDrvPrivate = DNew BYTE[size];
		if (m_hRightHandle.pDrvPrivate)
		{
			D3D10DDIARG_CREATERENDERTARGETVIEW createRenderTargetView = *pCreateRenderTargetView;
			createRenderTargetView.hDrvResource = pResWrp->m_hRightHandle;
			D3D10DDI_HRTRENDERTARGETVIEW hRTRightView = { NULL };
			pWrapper->OriginalDeviceFuncs.pfnCreateRenderTargetView(pWrapper->hDevice, &createRenderTargetView, 
				m_hRightHandle, hRTRightView);
		}
	}
}

void RenderTargetViewWrapper::Destroy( D3DDeviceWrapper* pWrapper )
{
	if (m_hRightHandle.pDrvPrivate)
	{
		pWrapper->OriginalDeviceFuncs.pfnDestroyRenderTargetView(pWrapper->hDevice, m_hRightHandle);
		delete[] m_hRightHandle.pDrvPrivate;
		m_hRightHandle.pDrvPrivate = NULL;
	}
}