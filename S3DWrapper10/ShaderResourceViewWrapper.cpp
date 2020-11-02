#include "StdAfx.h"
#include "ShaderResourceViewWrapper.h"
#include "D3DDeviceWrapper.h"

ShaderResourceViewWrapper::ShaderResourceViewWrapper( D3DDeviceWrapper* pWrapper, CONST D3D10DDIARG_CREATESHADERRESOURCEVIEW *pCreateShaderResourceView )
{
	m_hLeftHandle.pDrvPrivate = NULL;
	m_hRightHandle.pDrvPrivate = NULL;
	View = *pCreateShaderResourceView;

	ResourceWrapper* pResWrp; 
	InitWrapper(pCreateShaderResourceView->hDrvResource, pResWrp);
	if (pResWrp->IsStereo())
		InternalCreateView10(pWrapper, pResWrp->m_hRightHandle, m_hRightHandle);
}

ShaderResourceViewWrapper::ShaderResourceViewWrapper( D3DDeviceWrapper* pWrapper, CONST D3D10_1DDIARG_CREATESHADERRESOURCEVIEW *pCreateShaderResourceView )
{
	m_hRightHandle.pDrvPrivate = NULL;
	View10_1 = *pCreateShaderResourceView;

	ResourceWrapper* pResWrp; 
	InitWrapper(pCreateShaderResourceView->hDrvResource, pResWrp);
	if (pResWrp->IsStereo())
		InternalCreateView10_1(pWrapper, pResWrp->m_hRightHandle, m_hRightHandle);
}

ShaderResourceViewWrapper::ShaderResourceViewWrapper( D3DDeviceWrapper* pWrapper, CONST D3D11DDIARG_CREATESHADERRESOURCEVIEW *pCreateShaderResourceView )
{
	m_hRightHandle.pDrvPrivate = NULL;
	View11 = *pCreateShaderResourceView;

	ResourceWrapper* pResWrp; 
	InitWrapper(pCreateShaderResourceView->hDrvResource, pResWrp);
	if (pResWrp->IsStereo())
		InternalCreateView11(pWrapper, pResWrp->m_hRightHandle, m_hRightHandle);
}

void ShaderResourceViewWrapper::Destroy( D3DDeviceWrapper* pWrapper )
{
	if (m_hRightHandle.pDrvPrivate)
	{
		pWrapper->OriginalDeviceFuncs.pfnDestroyShaderResourceView(pWrapper->hDevice, m_hRightHandle);
		delete[] m_hRightHandle.pDrvPrivate;
		m_hRightHandle.pDrvPrivate = NULL;
	}
}

void ShaderResourceViewWrapper::CreateRightView( D3DDeviceWrapper* pWrapper )
{
	ResourceWrapper* pResWrp; 
	InitWrapper(View.hDrvResource, pResWrp);
	CreateViewInstance( pWrapper, pResWrp->m_hRightHandle, m_hRightHandle );
}

void ShaderResourceViewWrapper::CreateViewInstance( D3DDeviceWrapper* pWrapper, D3D10DDI_HRESOURCE hRes, D3D10DDI_HSHADERRESOURCEVIEW& hSRV )
{
	if ( pWrapper->GetD3DVersion() == TD3DVersion_10_0 )
		InternalCreateView10( pWrapper, hRes, hSRV );
	else if ( pWrapper->GetD3DVersion() == TD3DVersion_10_1 )
		InternalCreateView10_1( pWrapper, hRes, hSRV );
	else
		InternalCreateView11( pWrapper, hRes, hSRV );
}

void ShaderResourceViewWrapper::InternalCreateView10( D3DDeviceWrapper* pWrapper, D3D10DDI_HRESOURCE hRes, D3D10DDI_HSHADERRESOURCEVIEW& hSRV )
{
	D3D10DDIARG_CREATESHADERRESOURCEVIEW createShaderResourceView = View;
	createShaderResourceView.hDrvResource = hRes;
	SIZE_T size = pWrapper->OriginalDeviceFuncs.pfnCalcPrivateShaderResourceViewSize(pWrapper->hDevice, &createShaderResourceView);
	hSRV.pDrvPrivate = DNew BYTE[size];
	if (hSRV.pDrvPrivate)
	{
		D3D10DDI_HRTSHADERRESOURCEVIEW hRTRightView = { NULL };
		pWrapper->OriginalDeviceFuncs.pfnCreateShaderResourceView(pWrapper->hDevice, &createShaderResourceView, hSRV, hRTRightView);
	}
}

void ShaderResourceViewWrapper::InternalCreateView10_1( D3DDeviceWrapper* pWrapper, D3D10DDI_HRESOURCE hRes, D3D10DDI_HSHADERRESOURCEVIEW& hSRV )
{
	D3D10_1DDIARG_CREATESHADERRESOURCEVIEW createShaderResourceView = View10_1;
	createShaderResourceView.hDrvResource = hRes;
	SIZE_T size = pWrapper->OriginalDeviceFuncs10_1.pfnCalcPrivateShaderResourceViewSize(pWrapper->hDevice, &createShaderResourceView);
	hSRV.pDrvPrivate = DNew BYTE[size];
	if (hSRV.pDrvPrivate)
	{
		D3D10DDI_HRTSHADERRESOURCEVIEW hRTRightView = { NULL };
		pWrapper->OriginalDeviceFuncs10_1.pfnCreateShaderResourceView(pWrapper->hDevice, &createShaderResourceView, 
			hSRV, hRTRightView);	
	}
}

void ShaderResourceViewWrapper::InternalCreateView11( D3DDeviceWrapper* pWrapper, D3D10DDI_HRESOURCE hRes, D3D10DDI_HSHADERRESOURCEVIEW& hSRV )
{
	D3D11DDIARG_CREATESHADERRESOURCEVIEW createShaderResourceView = View11;
	createShaderResourceView.hDrvResource = hRes;
	SIZE_T size = pWrapper->OriginalDeviceFuncs11.pfnCalcPrivateShaderResourceViewSize(pWrapper->hDevice, &createShaderResourceView);
	hSRV.pDrvPrivate = DNew BYTE[size];
	if (hSRV.pDrvPrivate)
	{
		D3D10DDI_HRTSHADERRESOURCEVIEW hRTRightView = { NULL };
		pWrapper->OriginalDeviceFuncs11.pfnCreateShaderResourceView(pWrapper->hDevice, &createShaderResourceView, 
			hSRV, hRTRightView);	
	}
}

D3D10DDI_HSHADERRESOURCEVIEW ShaderResourceViewWrapper::GetCurrentViewHandle()
{
	DEBUG_TRACE3(_T("\t\t\tTextureType = %S (stereo %d)\n"), EnumToString(GetResourceWrapper()->m_Type), GetResourceWrapper()->IsStereo());
	if (Commands::Command::CurrentView_ != VIEWINDEX_RIGHT || !IsStereoState())
	{
		DEBUG_TRACE3(_T("\t\t\t\tLeft\n"));
		return GetHandle();
	}
	else
	{
		DEBUG_TRACE3(_T("\t\t\t\tRight\n"));
		return m_hRightHandle;
	}
}
