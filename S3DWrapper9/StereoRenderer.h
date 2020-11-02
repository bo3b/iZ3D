/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "BaseStereoRenderer.h"
#include "SwapChain.h"

class CStereoRenderer :
	public CBaseStereoRenderer
{
public:
	CStereoRenderer(void);
	virtual ~CStereoRenderer(void);
	virtual CMonoSwapChain* CreateSwapChainWrapper(UINT index);
	CSwapChain* GetSC(int i = 0) { return (CSwapChain*)m_SwapChains[i]; }

	void ChangeSizeOfRightRTWithAA(UINT &Width, UINT &Height);
	void ChangeSizeOfRightRTWithAA(LONG &Width, LONG &Height);

	bool RenderInStereo(bool bCW);

	template<typename T, typename Q> 
	void SetPipelineTexture( ShaderPipelineData<T, Q>& pipeline, DWORD ind, IDirect3DBaseTexture9* pTexture );

	virtual bool    RenderInStereo();
	virtual bool	StereoDS();
	virtual void    GetStereoSurfRects(IDirect3DSurface9* pLeft, IDirect3DSurface9* &pRight, D3DFORMAT &format, RECT *pLeftRect, RECT *pRightRect);
	template <VIEWINDEX view>
	void SetStereoRenderTarget( );
	virtual void SetStereoRenderTargetLeft()
	{	SetStereoRenderTarget<VIEWINDEX_LEFT>();	}
	virtual void SetStereoRenderTargetRight()
	{	SetStereoRenderTarget<VIEWINDEX_RIGHT>();	}
	virtual bool    RenderToTextures() { return m_RenderTarget[0].m_pMainSurf != GetBaseSC()->m_pLeftBackBufferBeforeScaling; };

	virtual HRESULT StartEngineMode();
	virtual void    StopEngineMode();
	virtual	void	ViewStagesSynchronization() {}

	STDMETHODIMP SetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9 *pRenderTarget);
	STDMETHODIMP SetDepthStencilSurface(IDirect3DSurface9 * pNewZStencil);
	STDMETHODIMP SetTexture(DWORD Stage, IDirect3DBaseTexture9* pTexture);
	STDMETHODIMP CreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, 
		D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, 
		BOOL Lockable, IDirect3DSurface9** ppSurface, HANDLE * pSharedHandle);
	STDMETHODIMP Clear(DWORD Count, CONST D3DRECT * pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil );
	STDMETHODIMP StretchRect( 
		IDirect3DSurface9 *pSourceSurface, CONST RECT * pSourceRect, 
		IDirect3DSurface9 *pDestSurface, CONST RECT * pDestRect, D3DTEXTUREFILTERTYPE Filter );
	STDMETHODIMP CreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, 
		D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, 
		BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle );
	STDMETHODIMP CreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format,
		D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE * pSharedHandle);
	STDMETHODIMP CreateCubeTexture(UINT EdgeLength,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DCubeTexture9** ppCubeTexture,HANDLE* pSharedHandle) ;
	STDMETHODIMP ColorFill(IDirect3DSurface9 * pSurface, CONST RECT * pRect, D3DCOLOR color );
	STDMETHODIMP UpdateSurface(IDirect3DSurface9* pSourceSurface,CONST RECT* pSourceRect,IDirect3DSurface9* pDestinationSurface,CONST POINT* pDestPoint) ;
	STDMETHODIMP UpdateTexture(IDirect3DBaseTexture9* pSourceTexture,IDirect3DBaseTexture9* pDestinationTexture) ;

	STDMETHODIMP CreateRenderTargetEx(UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Lockable,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle,DWORD Usage);
	STDMETHODIMP CreateDepthStencilSurfaceEx(UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Discard,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle,DWORD Usage);

private:
	SIZE	m_Offset;	
	
	CComPtr<IDirect3DSurface9>	m_pStereoBBLeft;
	CComPtr<IDirect3DSurface9>	m_pStereoBBRight;
};

// {0DD7EBB9-8739-497A-A724-D4201FC399DB}
const GUID OriginalSurfaceSize_GUID = { 0x0DD7EBB9, 0x8739, 0x497A, { 0xA7, 0x24, 0xD4, 0x20, 0x1F, 0xC3, 0x99, 0xDB } };
