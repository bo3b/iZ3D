#pragma once

#include "ResourceWrapper.h"
#include "StereoResourceWrapper.h"

class ShaderResourceViewWrapper
	: public StereoResourceWrapper<D3D10DDI_HSHADERRESOURCEVIEW, ShaderResourceViewWrapper>
{
public:
	ShaderResourceViewWrapper( D3DDeviceWrapper* pWrapper, CONST D3D10DDIARG_CREATESHADERRESOURCEVIEW	*pView );
	ShaderResourceViewWrapper( D3DDeviceWrapper* pWrapper, CONST D3D10_1DDIARG_CREATESHADERRESOURCEVIEW	*pView );
	ShaderResourceViewWrapper( D3DDeviceWrapper* pWrapper, CONST D3D11DDIARG_CREATESHADERRESOURCEVIEW	*pView );
	void Destroy( D3DDeviceWrapper* pWrapper );	
	void CreateRightView( D3DDeviceWrapper* pWrapper );
	void CreateViewInstance( D3DDeviceWrapper* pWrapper, D3D10DDI_HRESOURCE hRes, D3D10DDI_HSHADERRESOURCEVIEW& hSRV );
	bool IsStereoState ()
	{
		if (IsStereo())
		{
			ResourceWrapper* pRes = GetResourceWrapper();
			if (pRes->m_Type == TT_STEREO)
				return true;
		}
		return false;
	}

	D3D10DDI_HSHADERRESOURCEVIEW GetCurrentViewHandle();

	union
	{
		D3D10DDIARG_CREATESHADERRESOURCEVIEW		View;
		D3D10_1DDIARG_CREATESHADERRESOURCEVIEW		View10_1;
		D3D11DDIARG_CREATESHADERRESOURCEVIEW		View11;
	};
	ResourceWrapper* GetResourceWrapper() 
	{ return (ResourceWrapper*)View.hDrvResource.pDrvPrivate; }

private:
	void InternalCreateView10( D3DDeviceWrapper* pWrapper, D3D10DDI_HRESOURCE hRes, D3D10DDI_HSHADERRESOURCEVIEW& hSRV );
	void InternalCreateView10_1( D3DDeviceWrapper* pWrapper, D3D10DDI_HRESOURCE hRes, D3D10DDI_HSHADERRESOURCEVIEW& hSRV );
	void InternalCreateView11( D3DDeviceWrapper* pWrapper, D3D10DDI_HRESOURCE hRes, D3D10DDI_HSHADERRESOURCEVIEW& hSRV );
};

#define	CREATE_SRV_WRAPPER()							new(pWrp) ShaderResourceViewWrapper(D3D10_GET_WRAPPER(), pCreateShaderResourceView)
#define	DESTROY_SRV_WRAPPER()							pWrp->Destroy(D3D10_GET_WRAPPER()); pWrp->~ShaderResourceViewWrapper()

#define	ADDITIONAL_SRV_SIZE								sizeof(ShaderResourceViewWrapper)
#define	GET_SRV_HANDLE(hShaderResourceView)				GetOriginalHandle<ShaderResourceViewWrapper>(hShaderResourceView)
#define UNWRAP_CV_SRV_HANDLE(hShaderResourceView)		UnwrapCurrentViewHandle<ShaderResourceViewWrapper>(hShaderResourceView)
#define IS_STEREO_SRV_HANDLE(hShaderResourceView)		IsUsedStereoHandle<ShaderResourceViewWrapper>(hShaderResourceView)
