#pragma once

#include "ResourceWrapper.h"
#include "StereoResourceWrapper.h"

class DepthStencilViewWrapper
	: public StereoResourceWrapper<D3D10DDI_HDEPTHSTENCILVIEW, DepthStencilViewWrapper>
{
public:
	DepthStencilViewWrapper( D3DDeviceWrapper* pWrapper, CONST D3D10DDIARG_CREATEDEPTHSTENCILVIEW	*pView );
	DepthStencilViewWrapper( D3DDeviceWrapper* pWrapper, CONST D3D11DDIARG_CREATEDEPTHSTENCILVIEW	*pView );
	void Destroy( D3DDeviceWrapper* pWrapper );	
	union
	{
		D3D10DDIARG_CREATEDEPTHSTENCILVIEW		View;
		D3D11DDIARG_CREATEDEPTHSTENCILVIEW		View11;
	};

	ResourceWrapper* GetResourceWrapper() 
	{ return (ResourceWrapper*)View.hDrvResource.pDrvPrivate; }
};

#define	CREATE_DSV_WRAPPER()					new(pWrp) DepthStencilViewWrapper(D3D10_GET_WRAPPER(), pCreateDepthStencilView)
#define	DESTROY_DSV_WRAPPER()					pWrp->Destroy(D3D10_GET_WRAPPER()); pWrp->~DepthStencilViewWrapper()

#define	ADDITIONAL_DSV_SIZE						sizeof(DepthStencilViewWrapper)
#define	GET_DSV_HANDLE(hDepthStencilView)		GetOriginalHandle<DepthStencilViewWrapper>(hDepthStencilView)
#define UNWRAP_CV_DSV_HANDLE(hDepthStencilView)	UnwrapCurrentViewHandle<DepthStencilViewWrapper>(hDepthStencilView)
#define IS_STEREO_DSV_HANDLE(hDepthStencilView)	IsUsedStereoHandle<DepthStencilViewWrapper>(hDepthStencilView)
