#pragma once

#include "ResourceWrapper.h"
#include "StereoResourceWrapper.h"

class UnorderedAccessViewWrapper
	: public StereoResourceWrapper<D3D11DDI_HUNORDEREDACCESSVIEW, UnorderedAccessViewWrapper>
{
public:
	UnorderedAccessViewWrapper( D3DDeviceWrapper* pWrapper, CONST D3D11DDIARG_CREATEUNORDEREDACCESSVIEW	*pView );
	void Destroy( D3DDeviceWrapper* pWrapper );	

	D3D11DDIARG_CREATEUNORDEREDACCESSVIEW		View;
	ResourceWrapper* GetResourceWrapper() 
	{ return (ResourceWrapper*)View.hDrvResource.pDrvPrivate; }
};

#define	CREATE_UAV_WRAPPER()						new(pWrp) UnorderedAccessViewWrapper(D3D10_GET_WRAPPER(), pCreateUnorderedAccessView)
#define	DESTROY_UAV_WRAPPER()						pWrp->Destroy(D3D10_GET_WRAPPER()); pWrp->~UnorderedAccessViewWrapper()

#define	ADDITIONAL_UAV_SIZE							sizeof(UnorderedAccessViewWrapper)
#define	GET_UAV_HANDLE(hUnorderedAccessView)		GetOriginalHandle<UnorderedAccessViewWrapper>(hUnorderedAccessView)
#define UNWRAP_CV_UAV_HANDLE(hUnorderedAccessView)	UnwrapCurrentViewHandle<UnorderedAccessViewWrapper>(hUnorderedAccessView)
#define IS_STEREO_UAV_HANDLE(hUnorderedAccessView)	IsUsedStereoHandle<UnorderedAccessViewWrapper>(hUnorderedAccessView)
