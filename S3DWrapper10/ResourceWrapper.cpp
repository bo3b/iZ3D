#include "StdAfx.h"
#include "ResourceWrapper.h"
#include "../OutputMethods/OutputLib/OutputMethod_dx10.h"
#include "DXGISwapChainWrapper.h"
#include "D3DDeviceWrapper.h"
#include "ConstantBufferWrapperUsageDefault.h"
#include "ConstantBufferWrapperUsageDynamic.h"

#ifndef FINAL_RELEASE
UINT ResourceWrapper::m_nLastResourceID = 0;
#endif//FINAL_RELEASE

ResourceWrapper::ResourceWrapper( D3DDeviceWrapper* pWrapper, CONST D3D10DDIARG_CREATERESOURCE*& pCreateResource, D3D10DDI_HRTRESOURCE hRTResource  )
{
	_ASSERT( pWrapper->GetD3DVersion() < TD3DVersion_11_0 );
	m_EnableCaching = false;
	m_CreateResource = *pCreateResource;
	m_hRTResource = hRTResource;
	Initialize(pWrapper, pCreateResource);
}

ResourceWrapper::ResourceWrapper( D3DDeviceWrapper* pWrapper, CONST D3D11DDIARG_CREATERESOURCE*& pCreateResource, D3D10DDI_HRTRESOURCE hRTResource )
{
	_ASSERT( pWrapper->GetD3DVersion() >= TD3DVersion_11_0 );
	m_EnableCaching = false;
	m_CreateResource11 = *pCreateResource;
	m_hRTResource = hRTResource;
	Initialize(pWrapper, (CONST D3D10DDIARG_CREATERESOURCE*& )pCreateResource);
}

bool ResourceWrapper::IsTexture() const
{
	switch(m_CreateResource.ResourceDimension)
	{
	case D3D10DDIRESOURCE_TEXTURE1D:
	case D3D10DDIRESOURCE_TEXTURE2D:
	case D3D10DDIRESOURCE_TEXTURE3D:
	case D3D10DDIRESOURCE_TEXTURECUBE:
		return true;
	}
	return false;
}

void ResourceWrapper::Initialize( D3DDeviceWrapper* pWrapper, CONST D3D10DDIARG_CREATERESOURCE*& pCreateResource )
{
	CriticalSectionLocker locker( pWrapper->m_CSUMCall );

	m_Type = TT_MONO;
	m_bAQBSSurface = false;
	m_MipInfoList.resize(m_CreateResource.MipLevels);
	memcpy(&m_MipInfoList.front(), pCreateResource->pMipInfoList, sizeof(D3D10DDI_MIPINFO) * m_CreateResource.MipLevels);
	m_CreateResource.pMipInfoList = &m_MipInfoList.front();
	m_CreateResource.pInitialDataUP = NULL;
	if (pCreateResource->pPrimaryDesc)
	{
		memcpy(&m_PrimaryDesc, pCreateResource->pPrimaryDesc, sizeof(DXGI_DDI_PRIMARY_DESC));
		m_CreateResource.pPrimaryDesc = &m_PrimaryDesc;
	}
	m_BindFlags = m_CreateResource.BindFlags;
	m_hLeftHandle.pDrvPrivate = NULL;
	m_hRightHandle.pDrvPrivate = NULL;
	m_hPrimaryResourceHandle.pDrvPrivate = NULL;

	bool bProcessed = false;
	if (m_CreateResource.ResourceDimension == D3D10DDIRESOURCE_TEXTURE2D)
	{
		D3D10DDI_HRESOURCE hResource = GetWrapperHandle();
		if (m_BindFlags & D3D10_DDI_BIND_PRESENT)
		{
			bProcessed = true;
			if (!IsInternalCall())
			{
				DEBUG_MESSAGE(_T("BackBuffer 0x%p( 0x%p ) (primary %d)\n"), hResource.pDrvPrivate, GET_RESOURCE_HANDLE(hResource),  m_CreateResource.pPrimaryDesc ? 1 : 0);
				WE_CALL;
				if (g_SwapChainMode == scCreating) // init output method
					pWrapper->CreateD3DSwapChain();

				//if( USE_IZ3DKMSERVICE_PRESENTER && pCreateResource->pPrimaryDesc )	// non-NULL indicates that the runtime will use the resource as a primary 	
				//{
				//	// only for primary-and-backbuffer resources: create primary copy
				//	pCreateResource = &m_CreateResource;
				//	CreateResourceInstance( pWrapper, m_hPrimaryResourceHandle );

				//	// replace parameters to create non-primary non-backbuffer resource
				//	//m_CreateResource.BindFlags &= (~D3D10_DDI_BIND_PRESENT);
				//	//m_CreateResource.BindFlags |= D3D10_DDI_BIND_SHADER_RESOURCE;
				//	//m_CreateResource.pPrimaryDesc = NULL;

				//	// DEBUG ONLY!
				//	//m_CreateResource.ArraySize = 2;			// ask for 2 subresources per primary
				//}

				// replace parameters
				if (m_CreateResource.SampleDesc.Count == 1 && pWrapper->m_pOutputMethod && !pWrapper->m_pOutputMethod->RenderDirectlyToBB() )
					m_CreateResource.BindFlags |= D3D10_DDI_BIND_SHADER_RESOURCE;
				pCreateResource = &m_CreateResource;
				CreateRightResource(pWrapper);
			}
			else
			{
				_ASSERT(pWrapper->m_bTwoWindows);
				pWrapper->SetSecondBackBuffer(hResource);
			}
		}
		else if (m_CreateResource.pPrimaryDesc != NULL)
		{
			bProcessed = true;
			if (!IsInternalCall()) {
				DEBUG_MESSAGE(_T("Frontbuffer 0x%p (primary 1)\n"), hResource.pDrvPrivate); 
			} else {
				DEBUG_TRACE1(_T("Created our frontbuffer - %p\n"), hResource.pDrvPrivate);
			}
		}
	}

	if (!bProcessed && IsTexture() && 
		( m_CreateResource.BindFlags & D3D10_DDI_BIND_RENDER_TARGET ||
		m_CreateResource.BindFlags & D3D10_DDI_BIND_DEPTH_STENCIL ))
	{
		bool bCreateResource = false;
		UINT Width = m_MipInfoList[0].TexelWidth;
		UINT Height = m_MipInfoList[0].TexelHeight;
		CONST SIZE* pBBSize = NULL;
		if (pWrapper->m_pTempSwapChain)
			pBBSize = &pWrapper->m_pTempSwapChain->m_BackBufferSize;
		if ( m_CreateResource.BindFlags & D3D10_DDI_BIND_DEPTH_STENCIL)
		{
			if ( m_CreateResource.BindFlags & D3D10_DDI_BIND_SHADER_RESOURCE)
				bCreateResource = IsStereoDepthStencilTexture(Width, Height, pBBSize);
			else
				bCreateResource = IsStereoDepthStencilSurface(Width, Height, pBBSize);
		} else
		{
			if ( m_CreateResource.BindFlags & D3D10_DDI_BIND_SHADER_RESOURCE)
				bCreateResource = IsStereoRenderTargetTexture(m_CreateResource.Format, Width, Height, pBBSize);
			else
				bCreateResource = IsStereoRenderTargetSurface(Width, Height, pBBSize);
		} 
		if (bCreateResource)
			CreateRightResource(pWrapper);
	}
	
	if ( m_CreateResource.BindFlags == 0 && m_CreateResource.pMipInfoList )
	{
		if ( m_CreateResource.pMipInfoList->TexelWidth == 28 && m_CreateResource.pMipInfoList->TexelHeight == 1 )
		{
			DEBUG_MESSAGE(_T("AQBS resource detected\n"));
			m_bAQBSSurface = true;
		}
	}

#ifndef FINAL_RELEASE
	m_nResourceID = m_nLastResourceID++;
#endif//FINAL_RELEASE
}

ResourceWrapper::ResourceWrapper( D3DDeviceWrapper* pWrapper, CONST D3D10DDIARG_OPENRESOURCE* pOpenResource, D3D10DDI_HRTRESOURCE hRTResource )
{
	m_EnableCaching = false;
	m_BindFlags = D3D10_DDI_BIND_SHADER_RESOURCE;
	m_Type = TT_MONO;
	m_bAQBSSurface = false;
	ZeroMemory(&m_CreateResource11, sizeof(m_CreateResource11));
	m_CreateResource.MiscFlags = D3D10_DDI_RESOURCE_MISC_SHARED;
	m_hLeftHandle.pDrvPrivate = NULL;
	m_hRightHandle.pDrvPrivate = NULL;
	m_hPrimaryResourceHandle.pDrvPrivate = NULL;
	m_hRTResource = hRTResource;

#ifndef FINAL_RELEASE
	m_nResourceID = m_nLastResourceID++;
#endif//FINAL_RELEASE
}

bool ResourceWrapper::IsStereoRenderTargetSurface( UINT Width, UINT Height, CONST SIZE* pBBSize )
{
	switch(gInfo.RenderTargetCreationMode)
	{
	case 0:
		return false;
	case 1:
		return true;
	default:
		return !gInfo.MonoRenderTargetSurfaces
			&& !(Width == 1 || Height == 1)
			&& !(gInfo.CreateSquareRTInMono && IsSquareSize(Width, Height, gInfo.CreateBigSquareRTInStereo, pBBSize))
			&& !(gInfo.CreateRTThatLessThanBBInMono && IsLessThanBB(Width, Height, pBBSize));
	}
}

bool ResourceWrapper::IsStereoDepthStencilSurface( UINT Width, UINT Height, CONST SIZE* pBBSize )
{
	switch(gInfo.RenderTargetCreationMode)
	{
	case 0:
		return false;
	case 1:
		return true;
	default:
		return !gInfo.MonoDepthStencilSurfaces
			&& !(gInfo.CreateSquareDSInMono && IsSquareSize(Width, Height, gInfo.CreateBigSquareDSInStereo, pBBSize))
			&& !(gInfo.CreateDSThatLessThanBBInMono && IsLessThanBB(Width, Height, pBBSize));
	}
}

bool ResourceWrapper::IsStereoRenderTargetTexture( DXGI_FORMAT Format, UINT Width, UINT Height, CONST SIZE* pBBSize )
{
	switch(gInfo.RenderTargetCreationMode)
	{
	case 0:
		return false;
	case 1:
		return true;
	default:
		bool CreateInStereo = !ShadowFormat(Format) && !gInfo.MonoRenderTargetTextures;
		if (gInfo.CreateSquareRTInMono && IsSquareSize(Width, Height, gInfo.CreateBigSquareRTInStereo, pBBSize))
			CreateInStereo = false;
		else if (gInfo.CreateRTThatLessThanBBInMono && IsLessThanBB(Width, Height, pBBSize))
			CreateInStereo = false;
		if (Width == 1 || Height == 1) 
			CreateInStereo = false;
		return CreateInStereo;
	}
}

bool ResourceWrapper::IsStereoDepthStencilTexture( UINT Width, UINT Height, CONST SIZE* pBBSize )
{
	switch(gInfo.RenderTargetCreationMode)
	{
	case 0:
		return false;
	case 1:
		return true;
	default:
		bool CreateInStereo = !gInfo.MonoDepthStencilTextures;
		if (gInfo.CreateSquareDSInMono && IsSquareSize(Width, Height, gInfo.CreateBigSquareDSInStereo, pBBSize))
			CreateInStereo = false;
		else if (gInfo.CreateDSThatLessThanBBInMono && IsLessThanBB(Width, Height, pBBSize))
			CreateInStereo = false;
		if (Width == 1 || Height == 1) 
			CreateInStereo = false;	
		return CreateInStereo;
	}
}

bool ResourceWrapper::ShadowFormat( DXGI_FORMAT Format ) { 
	if (Format == DXGI_FORMAT_R32_TYPELESS ||
		Format == DXGI_FORMAT_R32_FLOAT ||
		Format == DXGI_FORMAT_R32_UINT ||
		Format == DXGI_FORMAT_R32_SINT)
		return gInfo.CreateOneComponentRTInMono != 0; 
	else
		return false;
}

bool ResourceWrapper::IsSquareSize( UINT Width, UINT Height, BOOL CreateBigInStereo, CONST SIZE* pBBSize )
{
	if (Width == Height)
	{
		if (pBBSize)
		{
			// if square backbuffer and equal to size
			if (pBBSize->cx == pBBSize->cy && Width == (UINT)pBBSize->cx)
				return false;
			if (CreateBigInStereo && (Width >= (UINT)pBBSize->cx && Height >= (UINT)pBBSize->cy))
				return false;
		}
		return true;
	}
	return false;
}

bool ResourceWrapper::IsLessThanBB( UINT Width, UINT Height, CONST SIZE* pBBSize )
{
	if (pBBSize && (Width != Height) && ((Width + 5) < (UINT)pBBSize->cx && (Height + 5) < (UINT)pBBSize->cy))
	{
		float aRT = (1.0f * Width / Height);
		float aBB = (1.0f * pBBSize->cx / pBBSize->cy);
		float f = abs(aRT - aBB);
		if (f >= 0.01f)
		{
			DEBUG_TRACE1(_T("\tAspect RT=%f (aspect BB=%f, diff = %f)\n"), aRT, aBB, f);
			return true;
		}
	}
	return false;
}

UINT ResourceWrapper::Size(UINT MipLevel) const
{
	assert(MipLevel < m_CreateResource.MipLevels);

	const D3D10DDI_MIPINFO& info = m_MipInfoList[MipLevel];
	UINT size = info.PhysicalWidth * info.PhysicalHeight * info.PhysicalDepth;

	if (m_CreateResource.ResourceDimension != D3D10DDIRESOURCE_BUFFER && m_CreateResource.ResourceDimension != D3D11DDIRESOURCE_BUFFEREX) {
		size = (UINT)(size * GetBitWidthOfDXGIFormat(m_CreateResource.Format) / 8);
	}

	return size;
}

void ResourceWrapper::Init( D3DDeviceWrapper* pWrapper )
{
	// after resource created
	if (m_BindFlags & D3D10_DDI_BIND_PRESENT)
	{
		D3D10DDI_HRESOURCE hResource = GetWrapperHandle();
		DXGI_DDI_PRIMARY_DESC* pPrimaryDesc = m_CreateResource.pPrimaryDesc;
		if (m_BindFlags & D3D10_DDI_BIND_SHADER_RESOURCE)
		{
			DEBUG_TRACE1(_T("\tShared\n"));
		}
		if (pPrimaryDesc) 
		{
			if (pPrimaryDesc->Flags & DXGI_DDI_PRIMARY_OPTIONAL) {
				DEBUG_TRACE1(_T("\tpPrimaryDesc -> Optional\n"));
			} else {
				DEBUG_TRACE1(_T("\tpPrimaryDesc\n"));
			}
		}
	}
}

void ResourceWrapper::Destroy( D3DDeviceWrapper* pWrapper )
{
	if (m_BindFlags & D3D10_DDI_BIND_PRESENT)
	{
		D3D10DDI_HRESOURCE hResource = GetWrapperHandle();
		DEBUG_MESSAGE(_T("BackBuffer 0x%p( 0x%p ) Destroy()\n"), hResource.pDrvPrivate, GET_RESOURCE_HANDLE(hResource) );
		if (!IsInternalCall())
		{
			WE_CALL;
			pWrapper->RemoveSwapChain(hResource);
		}
		else
		{
			pWrapper->RemoveSwapChain(hResource, false);
		}
	}

	if (m_hLeftHandle.pDrvPrivate)
	{
		pWrapper->OriginalDeviceFuncs.pfnDestroyResource(pWrapper->hDevice, m_hLeftHandle);
		DELETE_HANDLE(m_hLeftHandle);
	}

	if (m_hRightHandle.pDrvPrivate)
	{
		pWrapper->OriginalDeviceFuncs.pfnDestroyResource(pWrapper->hDevice, m_hRightHandle);
		DELETE_HANDLE(m_hRightHandle);
	}

	m_pCB.reset();

	if (m_hPrimaryResourceHandle.pDrvPrivate)
	{
		pWrapper->OriginalDeviceFuncs.pfnDestroyResource(pWrapper->hDevice, m_hPrimaryResourceHandle);
		DELETE_HANDLE(m_hPrimaryResourceHandle);
	}
}

bool ResourceWrapper::InitializeCB( D3DDeviceWrapper* pWrapper )
{
	if (m_pCB)
		return m_pCB->IsMonoDataInitialized();
	switch(m_CreateResource.Usage)
	{
	case D3D10_DDI_USAGE_DEFAULT:
		m_pCB.reset(DNew ConstantBufferWrapperUsageDefault(pWrapper, this));
		break;
	case D3D10_DDI_USAGE_IMMUTABLE:
		DEBUG_TRACE3(_T("WARNING: Imutable constant buffer cannot be stereo\n"));
		break;
	case D3D10_DDI_USAGE_DYNAMIC:
		m_pCB.reset(DNew ConstantBufferWrapperUsageDynamic(pWrapper, this));
		break;
	case D3D10_DDI_USAGE_STAGING:
		_ASSERT(FALSE);
		break;
	}
	return false;
}

void ResourceWrapper::CreateLeftResource( D3DDeviceWrapper* pWrapper )
{
	CreateResourceInstance( pWrapper, m_hLeftHandle );
}

void ResourceWrapper::CreateRightResource( D3DDeviceWrapper* pWrapper )
{
	CreateResourceInstance( pWrapper, m_hRightHandle );
}

void ResourceWrapper::CreateRightResourceFromLeft( D3DDeviceWrapper* pWrapper )
{
	DEBUG_TRACE1(_T("CreateRightResourceFromLeft\n"));
	CreateResourceInstance( pWrapper, m_hRightHandle );
	pWrapper->OriginalDeviceFuncs.pfnResourceCopy(pWrapper->GetHandle(), GetHandle(), m_hRightHandle);
}

void ResourceWrapper::CreateResourceInstance( D3DDeviceWrapper* pWrapper, D3D10DDI_HRESOURCE& hResource )
{
	CriticalSectionLocker locker( pWrapper->m_CSUMCall );
	pWrapper->CreateResource(hResource, &m_CreateResource11, m_hRTResource);
}

bool ResourceWrapper::CreateTempResources( D3DDeviceWrapper* pWrapper, const D3D11DDIARG_CREATERESOURCE* pCreateResource, D3D10DDI_HRESOURCE& hDstBuffer, D3D10DDI_HRESOURCE& hDstBufferWOAA )
{
	CriticalSectionLocker locker( pWrapper->m_CSUMCall );
	if( pCreateResource->ResourceDimension != D3D10DDIRESOURCE_TEXTURE2D && 
		pCreateResource->ResourceDimension != D3D10DDIRESOURCE_TEXTURE3D && 
		pCreateResource->ResourceDimension != D3D10DDIRESOURCE_TEXTURECUBE )
	{
		DEBUG_MESSAGE(_T("Dump Error: Unsupported resource dimension (%S)\n"), EnumToString(pCreateResource->ResourceDimension));
		return false;
	}
	//if( pCreateResource->Usage != D3D10_DDI_USAGE_DEFAULT )
	//{
	//	DEBUG_MESSAGE(_T("Dump Warning: Unsupported resource usage (%S)\n"), EnumToString((D3D10_DDI_RESOURCE_USAGE)pCreateResource->Usage));
	//}
	D3D11DDIARG_CREATERESOURCE cres = *pCreateResource;
	cres.Usage     = D3D10_DDI_USAGE_STAGING;
	cres.MapFlags  = D3D10_DDI_MAP_READ;
	cres.BindFlags = 0;				// Don't change this - we can't bind our copy to the graphics pipeline!
	cres.pInitialDataUP = NULL;		// Don't change this - we can't present our copy!
	cres.pPrimaryDesc = NULL;		// Don't change this - we can't present our copy!
	cres.SampleDesc.Count	= 1;	// Don't change this - we can't copy-and-map multi-sampled resources!
	cres.SampleDesc.Quality = 0;

	D3D10DDI_HRTRESOURCE	hrtbuffer = { NULL };
	pWrapper->CreateResource(hDstBuffer, &cres, hrtbuffer);

	if( 1 != pCreateResource->SampleDesc.Count )
	{
		// multi-sampled resource must be resolved prior to it being copied!
		// resolve the multi-sampled resource
		D3D11DDIARG_CREATERESOURCE cres2 = *pCreateResource;
		cres2.Usage     = D3D10_DDI_USAGE_DEFAULT;
		cres2.MapFlags  = 0;
		cres2.BindFlags = 0;			// Don't change this - we can't bind our copy to the graphics pipeline!
		cres2.pInitialDataUP = NULL;
		cres2.pPrimaryDesc = NULL;		// Don't change this - we can't present our copy!
		cres2.SampleDesc.Count	= 1;	// Don't change this - we can't copy-and-map multi-sampled resources!
		cres2.SampleDesc.Quality = 0;

		pWrapper->CreateResource(hDstBufferWOAA, &cres2, hrtbuffer);
	}
	return true;
}

void ResourceWrapper::DestroyTempResources( D3DDeviceWrapper* pWrapper, D3D10DDI_HRESOURCE& hDstBuffer, D3D10DDI_HRESOURCE& hDstBufferWOAA )
{
	CriticalSectionLocker locker( pWrapper->m_CSUMCall );
	if (hDstBufferWOAA.pDrvPrivate)
	{
		pWrapper->OriginalDeviceFuncs.pfnDestroyResource(
			pWrapper->hDevice, 
			hDstBufferWOAA
			);
		delete [] hDstBufferWOAA.pDrvPrivate;
	}

	pWrapper->OriginalDeviceFuncs.pfnDestroyResource(
		pWrapper->hDevice, 
		hDstBuffer
		);
	delete [] hDstBuffer.pDrvPrivate;
}

void ResourceWrapper::DumpToFile(D3DDeviceWrapper* pWrapper, D3D10DDI_HRESOURCE	hResource, const D3D11DDIARG_CREATERESOURCE* pCreateResource, TCHAR* fileName)
{
#ifndef FINAL_RELEASE
	D3D10DDI_HRESOURCE		hDstBuffer = { NULL };
	D3D10DDI_HRESOURCE		hDstBufferWOAA = { NULL };
	if (!CreateTempResources(pWrapper, pCreateResource, hDstBuffer, hDstBufferWOAA))
		return;

	TCHAR* dstFileName = fileName + _tcsclen(fileName);
	dstFileName = dstFileName + _tcsclen(dstFileName);

	_tcscat(dstFileName, _T(".jpg"));

	DumpDefaultResource(pWrapper, pCreateResource, fileName, hResource, hDstBuffer, hDstBufferWOAA);

	DestroyTempResources(pWrapper, hDstBuffer, hDstBufferWOAA);
#endif
}

void ResourceWrapper::DumpToFile(D3DDeviceWrapper* pWrapper, TCHAR* fileName, TCHAR* rescourceName, bool b2RT, bool bStereo, DumpView view)
{
#ifndef FINAL_RELEASE
	D3D10DDI_HRESOURCE		hDstBuffer = { NULL };
	D3D10DDI_HRESOURCE		hDstBufferWOAA = { NULL };
	if (!CreateTempResources(pWrapper, &m_CreateResource11, hDstBuffer, hDstBufferWOAA))
		return;

	if (!IsStereo())
		b2RT = false;
	// b2RT >= bStereo
	if (!b2RT)
		bStereo = false;

	TCHAR* dstFileName = fileName + _tcsclen(fileName);
	if (!bStereo)
		_tcscat(dstFileName, _T("Mono."));
	else
		_tcscat(dstFileName, _T("Stereo."));
	if (b2RT)
		_tcscat(dstFileName, _T("Left."));
	if (rescourceName != NULL)
		_tcscat(dstFileName, rescourceName);

	_tcscat(dstFileName, _T(".jpg"));

	if (view != dvRight)
		DumpDefaultResource(pWrapper, &m_CreateResource11, fileName, GetHandle(), hDstBuffer, hDstBufferWOAA);

	if (b2RT && view != dvLeft)
	{
		dstFileName[0] = '\0';
		if (!bStereo)
			_tcscat(dstFileName, _T("Mono."));
		else
			_tcscat(dstFileName, _T("Stereo."));
		_tcscat(dstFileName, _T("Right."));
		if (rescourceName != NULL)
			_tcscat(dstFileName, rescourceName);
		_tcscat(dstFileName, _T(".jpg"));
		DumpDefaultResource(pWrapper, &m_CreateResource11, fileName, m_hRightHandle, hDstBuffer, hDstBufferWOAA );
	}

	//if (m_hBackBuffer.pDrvPrivate)
	//{
	//	dstFileName[0] = '\0';
	//	_tcscat(dstFileName, _T(".BackBuffer"));
	//	_tcscat(dstFileName, _T(".jpg"));
	//	DumpDefaultResource(pWrapper, fileName, m_hBackBuffer, hDstBuffer, hDstBufferWOAA );
	//}

	DestroyTempResources(pWrapper, hDstBuffer, hDstBufferWOAA);
#endif
}

void ResourceWrapper::DumpDefaultResource( D3DDeviceWrapper* pWrapper, const D3D11DDIARG_CREATERESOURCE* pCreateResource, TCHAR* fileName, D3D10DDI_HRESOURCE hSrcBuffer, D3D10DDI_HRESOURCE hDstBuffer, D3D10DDI_HRESOURCE hDstBufferWOAA )
{
	CriticalSectionLocker locker( pWrapper->m_CSUMCall );

	if( pCreateResource->SampleDesc.Count == 1 )
		pWrapper->OriginalDeviceFuncs.pfnResourceCopy(
		pWrapper->hDevice,
		hDstBuffer,					// hDstResource
		hSrcBuffer					// hSrcResource
		);
	else
	{
		DXGI_FORMAT format = GetTypedDXGIFormat( pCreateResource->Format );
		for( UINT i=0; i<1/*numsubresources*/; ++i )
			pWrapper->OriginalDeviceFuncs.pfnResourceResolveSubresource(
			pWrapper->hDevice,
			hDstBufferWOAA,				// hDstResource
			i,						// DstSubresource
			hSrcBuffer,			// hSrcResource
			i,						// SrcSubresource
			format					// ResolveFormat
			);

		pWrapper->OriginalDeviceFuncs.pfnResourceCopy(
			pWrapper->hDevice,
			hDstBuffer,					// hDstResource
			hDstBufferWOAA					// hSrcResource
			);

	}
	DumpStagingResource(pWrapper, pCreateResource, fileName, hDstBuffer );
}

void ResourceWrapper::DumpStagingResource( D3DDeviceWrapper* pWrapper, const D3D11DDIARG_CREATERESOURCE* pCreateResource, TCHAR* fileName, D3D10DDI_HRESOURCE hSurface )
{
	CriticalSectionLocker locker( pWrapper->m_CSUMCall );

	UINT numsubresources = pCreateResource->MipLevels * pCreateResource->ArraySize;
	TCHAR* dstFileName = fileName + _tcsclen(fileName);
	//iterate all sub-resources to save the whole resource
	for( UINT i = 0; i<1; ++i ) // numsubresources
	{

		D3D10DDI_MAPPED_SUBRESOURCE	subresource;
		pWrapper->OriginalDeviceFuncs.pfnStagingResourceMap(
			pWrapper->hDevice,
			hSurface,
			i,			// subresource
			D3D10_DDI_MAP_READ,
			0,			// flags (D3D10_DDI_MAP_FLAG_DONOTWAIT, etc.)
			&subresource
			);

		char* pData = (char*)subresource.pData;
		if (!pData)
		{
			DEBUG_MESSAGE(_T("Dump Error: Map pData == NULL\n"));
			return;
		}

		DWORD subresourcesize;
		switch( pCreateResource->ResourceDimension )
		{
		case D3D10DDIRESOURCE_BUFFER:
		case D3D10DDIRESOURCE_TEXTURE1D:		subresourcesize = subresource.RowPitch;		break;

		case D3D10DDIRESOURCE_TEXTURE2D:
		case D3D10DDIRESOURCE_TEXTURE3D:
		case D3D10DDIRESOURCE_TEXTURECUBE:		subresourcesize = subresource.DepthPitch;	break;
		}

		dstFileName[0] = '\0';
		DEBUG_MESSAGE(_T("Dump to file: %s\n"), _tcsrchr(fileName, '\\') + 1);
		SaveImageToFile(fileName, pCreateResource->Format, 
			pCreateResource->pMipInfoList[0].TexelWidth, pCreateResource->pMipInfoList[0].TexelHeight, 
			pData, subresource.RowPitch);

		pWrapper->OriginalDeviceFuncs.pfnStagingResourceUnmap(
			pWrapper->hDevice,
			hSurface,
			i			// subresource
			);
	}
}

const wchar_t* ResourceWrapper::GetResourceName() const
{
#ifndef FINAL_RELEASE
	if (IsTexture())
	{
		if ( m_CreateResource.BindFlags & D3D10_DDI_BIND_SHADER_RESOURCE)
		{
			if ( m_CreateResource.BindFlags & D3D10_DDI_BIND_RENDER_TARGET )
				return L"RenderTargetTexture";
			else if ( m_CreateResource.BindFlags & D3D10_DDI_BIND_DEPTH_STENCIL )
				return L"DepthStencilTexture";
			else
				return L"Texture";
		}
		else
		{
			if ( m_CreateResource.BindFlags & D3D10_DDI_BIND_RENDER_TARGET )
				return L"RenderTargetSurface";
			else if ( m_CreateResource.BindFlags & D3D10_DDI_BIND_DEPTH_STENCIL )
				return L"DepthStencilSurface";
			else
				return L"Surface";
		}
	}
	else
		return L"Buffer";
#else
	return L"";
#endif
}
