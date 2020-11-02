#pragma once

#include "StereoResourceWrapper.h"
#include "ConstantBufferWrapper.h"
#include "Commands\xxMap.h"
#include <boost\scoped_ptr.hpp>
#include <list>
#include <vector>

enum DumpView { dvBoth, dvLeft, dvRight };

////////////////////////////////////////////////////////////////////////////////
class ResourceWrapper : public StereoResourceWrapper<D3D10DDI_HRESOURCE,ResourceWrapper>
{
public:
	ResourceWrapper(D3DDeviceWrapper* pWrapper, CONST D3D10DDIARG_CREATERESOURCE*&  pCreateResource, D3D10DDI_HRTRESOURCE hRTResource);
	ResourceWrapper(D3DDeviceWrapper* pWrapper, CONST D3D11DDIARG_CREATERESOURCE*&  pCreateResource, D3D10DDI_HRTRESOURCE hRTResource);
	ResourceWrapper(D3DDeviceWrapper* pWrapper, CONST D3D10DDIARG_OPENRESOURCE*  pOpenResource, D3D10DDI_HRTRESOURCE hRTResource);

	void	Initialize					( D3DDeviceWrapper* pWrapper, CONST D3D10DDIARG_CREATERESOURCE*& pCreateResource );
	void	Init						( D3DDeviceWrapper* pWrapper );
	void	Destroy						( D3DDeviceWrapper* pWrapper );
	
	UINT Size(UINT MipLevel) const;

	bool IsStereoState ()	
	{
		if (IsStereo())
		{
			if (m_Type == TT_STEREO)
				return true;
		}
		return false;
	}

	bool IsAQBSSurface () const
	{
		return m_bAQBSSurface;
	}

	D3D10DDI_HRESOURCE GetCurrentViewHandleEx()
	{
		if ( IsStereoState() ) 
		{
			if (Commands::Command::CurrentView_ == VIEWINDEX_LEFT && m_hLeftHandle.pDrvPrivate) {
				return m_hLeftHandle;
			}
			else if (Commands::Command::CurrentView_ == VIEWINDEX_RIGHT && m_hRightHandle.pDrvPrivate) {
				return m_hRightHandle;
			}
		}

		return GetHandle();
	}
	
	void	CreateLeftResource			( D3DDeviceWrapper* pWrapper );
	void	CreateRightResource			( D3DDeviceWrapper* pWrapper );
	void	CreateRightResourceFromLeft	( D3DDeviceWrapper* pWrapper );
	bool	IsPrimary					()
	{
		return m_hPrimaryResourceHandle.pDrvPrivate != NULL;
	}
	D3D10DDI_HRESOURCE	GetPrimaryHandle()
	{
		return IsPrimary() ? m_hPrimaryResourceHandle : GetHandle();
	}

	D3D10DDI_HRESOURCE			m_hPrimaryResourceHandle;	// Used for resources with D3D1x_DDI_BIND_PRESENT flag set
	union
	{
		D3D10DDIARG_CREATERESOURCE		m_CreateResource;
		D3D11DDIARG_CREATERESOURCE		m_CreateResource11;
	};
	UINT							m_BindFlags;
	std::vector<D3D10DDI_MIPINFO>	m_MipInfoList;
	DXGI_DDI_PRIMARY_DESC			m_PrimaryDesc;
	D3D10DDI_HRTRESOURCE			m_hRTResource;

	TextureType m_Type; // only for resources with bind flag SRV
	bool	m_bAQBSSurface;

	TextureType m_storedType;
	void StoreType() { m_storedType = m_Type; }
	void RestoreType() { m_Type = m_storedType; }
	bool IsTexture() const;

	bool InitializeCB( D3DDeviceWrapper* pWrapper );

	const wchar_t* GetResourceName() const;
	void DumpToFile( D3DDeviceWrapper* pWrapper, TCHAR* fileName, TCHAR* rescourceName, bool b2RT, bool bStereo, DumpView view = dvBoth );
	static void DumpToFile(D3DDeviceWrapper* pWrapper, D3D10DDI_HRESOURCE	hResource, const D3D11DDIARG_CREATERESOURCE* pCreateResource, TCHAR* fileName);

	boost::scoped_ptr<ConstantBufferWrapper>	m_pCB;

	bool                                        m_EnableCaching;
	Commands::TMapCmdPtr						m_pCurrentMapCmd;
	Commands::TUpdateSubresourceUpCmdPtr        m_pRestoreCmd;

#ifndef FINAL_RELEASE
	static UINT						m_nLastResourceID;	// id последнего созданного ресурса
	int								m_nResourceID;		// id созданного ресурса ( чтобыможно было отлавливать момент создания)
#endif //FINAL_RELEASE

	operator D3D10DDI_HRESOURCE() 
	{
		D3D10DDI_HRESOURCE hres = { this };
		return hres;
	}

private:
	static bool CreateTempResources( D3DDeviceWrapper* pWrapper, const D3D11DDIARG_CREATERESOURCE* pCreateResource, D3D10DDI_HRESOURCE& hDstBuffer, D3D10DDI_HRESOURCE& hDstBufferWOAA);
	static void DestroyTempResources( D3DDeviceWrapper* pWrapper, D3D10DDI_HRESOURCE& hDstBuffer, D3D10DDI_HRESOURCE& hDstBufferWOAA );

	static void DumpDefaultResource( D3DDeviceWrapper* pWrapper, const D3D11DDIARG_CREATERESOURCE* pCreateResource, TCHAR* fileName, D3D10DDI_HRESOURCE hSrcBuffer, D3D10DDI_HRESOURCE hDstBuffer, D3D10DDI_HRESOURCE hDstBufferWOAA );
	static void DumpStagingResource( D3DDeviceWrapper* pWrapper, const D3D11DDIARG_CREATERESOURCE* pCreateResource, TCHAR* fileName, D3D10DDI_HRESOURCE hSurface );

	bool IsStereoRenderTargetSurface( UINT Width, UINT Height, CONST SIZE* pBBSize );
	bool IsStereoDepthStencilSurface( UINT Width, UINT Height, CONST SIZE* pBBSize );
	bool IsStereoRenderTargetTexture( DXGI_FORMAT Format, UINT Width, UINT Height, CONST SIZE* pBBSize );
	bool IsStereoDepthStencilTexture( UINT Width, UINT Height, CONST SIZE* pBBSize );
	bool IsSquareSize( UINT Width, UINT Height, BOOL CreateBigInStereo, CONST SIZE* pBBSize );
	bool IsLessThanBB( UINT Width, UINT Height, CONST SIZE* pBBSize );
	bool ShadowFormat( DXGI_FORMAT Format );

	void	CreateResourceInstance		( D3DDeviceWrapper* pWrapper, D3D10DDI_HRESOURCE& hResource );
};

static D3D10DDI_HRESOURCE UnwrapCurrentViewHandleEx(D3D10DDI_HRESOURCE handle)
{
	D3D10DDI_HRESOURCE hRes;
	if (handle.pDrvPrivate)
		hRes = ((ResourceWrapper*)handle.pDrvPrivate)->GetCurrentViewHandleEx();
	else
		hRes.pDrvPrivate = NULL;
	return hRes;
}

#define ENABLE_BUFFERING_MAP TRUE	// Usage Dynamic
#define ENABLE_BUFFERING_USU TRUE	// Usage Default
#if !defined(EXECUTE_IMMEDIATELY_G1) || !defined(EXECUTE_IMMEDIATELY_G2) || !defined(EXECUTE_IMMEDIATELY_G3)
#define ENABLE_BUFFERING (ENABLE_BUFFERING_USU || ENABLE_BUFFERING_MAP)
#endif

#define	ADDITIONAL_RESOURCE_SIZE								sizeof(ResourceWrapper)
#define	CREATE_RESOURCE_WRAPPER(pCreateResource, hRTResource)	new(pWrp) ResourceWrapper(D3D10_GET_WRAPPER(), pCreateResource, hRTResource)
#define	GET_RESOURCE_HANDLE(hResource)							GetOriginalHandle<ResourceWrapper>(hResource)
#define	DESTROY_RESOURCE_WRAPPER()								pWrp->Destroy(D3D10_GET_WRAPPER()); pWrp->~ResourceWrapper()
#define UNWRAP_CV_RESOURCE_HANDLE(hResource)					UnwrapCurrentViewHandle<ResourceWrapper>(hResource)
#define UNWRAP_CV_RESOURCE_HANDLE_EX(hResource)					UnwrapCurrentViewHandleEx(hResource)
#define IS_STEREO_RESOURCE_HANDLE(hResource)					IsUsedStereoHandle<ResourceWrapper>(hResource)
