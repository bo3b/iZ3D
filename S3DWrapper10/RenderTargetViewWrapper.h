#pragma once

#include "ResourceWrapper.h"
#include "StereoResourceWrapper.h"

class RenderTargetViewWrapper 
	: public StereoResourceWrapper<D3D10DDI_HRENDERTARGETVIEW, RenderTargetViewWrapper>
{
public:
	RenderTargetViewWrapper( D3DDeviceWrapper* pWrapper, CONST D3D10DDIARG_CREATERENDERTARGETVIEW	*pView );
	void Destroy( D3DDeviceWrapper* pWrapper );	

	D3D10DDIARG_CREATERENDERTARGETVIEW		View;
	ResourceWrapper* GetResourceWrapper() 
	{ return (ResourceWrapper*)View.hDrvResource.pDrvPrivate; }
};

#define	CREATE_RTV_WRAPPER()						new(pWrp) RenderTargetViewWrapper(D3D10_GET_WRAPPER(), pCreateRenderTargetView)
#define	DESTROY_RTV_WRAPPER()						pWrp->Destroy(D3D10_GET_WRAPPER()); pWrp->~RenderTargetViewWrapper()

#define	ADDITIONAL_RTV_SIZE							sizeof(RenderTargetViewWrapper)
#define	GET_RTV_HANDLE(hRenderTargetView)			GetOriginalHandle<RenderTargetViewWrapper>(hRenderTargetView)
#define UNWRAP_CV_RTV_HANDLE(hRenderTargetView)		UnwrapCurrentViewHandle<RenderTargetViewWrapper>(hRenderTargetView)
#define IS_STEREO_RTV_HANDLE(hRenderTargetView)		IsUsedStereoHandle<RenderTargetViewWrapper>(hRenderTargetView)
