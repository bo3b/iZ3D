/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "BaseStereoRenderer.h"
#include "WideSwapChain.h"

#define HEIGHTMUL 2

MIDL_INTERFACE("243DD9D3-A23F-4d3d-9F92-30EF4DADDBCB")
IWideSurfaceData: public IUnknown
{
	SIZE		m_Offset;
	SIZE		m_Size;
	D3DFORMAT	m_Format;
	DWORD		m_Usage;

	BOOL		m_bLockForWrite;
	RECT		m_LockedRect;
	CComPtr<IDirect3DSurface9> m_pAdditionalSurface;
public:
	inline CONST SIZE*	GetOffset() { return &m_Offset; };
	inline void			SetOffset(CONST SIZE *pOffset) { m_Offset = *pOffset; };
	inline CONST SIZE*	GetSize(){ return &m_Size; };
	inline void			SetSize(CONST SIZE *pSize) { m_Size = *pSize; };
	inline D3DFORMAT	GetFormat(){ return m_Format; };
	inline void			SetFormat(D3DFORMAT format) { m_Format = format; };
	inline DWORD		GetUsage(){ return m_Usage; };
	inline void			SetUsage(DWORD usage) { m_Usage = usage; };

	inline BOOL			GetLockForWrite()  { return m_bLockForWrite; };
	inline void			SetLockForWrite(BOOL bLockForWrite)  { m_bLockForWrite = bLockForWrite; };
	inline CONST RECT*	GetLockedRect()  { return &m_LockedRect; };
	inline void			SetLockedRect(CONST RECT *lockedRect)  { m_LockedRect = *lockedRect; };
	inline IDirect3DSurface9*	GetAdditionalSurface()  { return m_pAdditionalSurface; };
	inline void			SetAdditionalSurface(IDirect3DSurface9 *pSurf)  { m_pAdditionalSurface = pSurf; };
};

class CWideSurfaceData: 
	public IWideSurfaceData,
	public CComObjectRoot,
	public CComCoClass<CWideSurfaceData>
{
public:
	BEGIN_COM_MAP(CWideSurfaceData)
		COM_INTERFACE_ENTRY(IWideSurfaceData)
	END_COM_MAP()
};

template <typename T, typename Q>
class ShaderPipelineDataEx 
{
public:
	ShaderPipelineDataEx(ShaderPipelineData<T, Q>& data) : m_Data(data), m_WideTexturesMask(0) {}
	void Init();
	void Clear();
	ShaderPipelineData<T, Q>&	m_Data;
	DWORD						m_WideTexturesMask;
};

class CWideStereoRenderer :
	public CBaseStereoRenderer
{
public:
	CWideStereoRenderer(void);
	virtual ~CWideStereoRenderer(void);
	
	static void	IncRevision(IDirect3DSurface9* pWideSurf);
	static IDirect3DSurface9* GetLeftSurface( IDirect3DSurface9* pWideSurf, IDirect3DSurface9** ppLeftSurf );
	static CONST SIZE* GetOffset( IDirect3DSurface9* pSurf);

private:
	ShaderPipelineDataEx<PS_PRIVATE_DATA, IDirect3DPixelShader9> m_PSPipelineDataEx;
	ShaderPipelineDataEx<VS_PRIVATE_DATA, IDirect3DVertexShader9> m_VSPipelineDataEx;

	// Current states
	CONST SIZE*	m_pOffset;
	CONST SIZE*	m_pDepthOffset;

	CComPtr<IDirect3DPixelShader9>  m_pDepthShader;
	CComPtr<IDirect3DSurface9>		m_pSmallSurface;
	CComPtr<IDirect3DStateBlock9>   m_pState;

	CWideSwapChain* GetWideSC(int i = 0) { return (CWideSwapChain*)m_SwapChains[i]; }

	virtual CMonoSwapChain* CreateSwapChainWrapper(UINT index);
	HRESULT	InitializeOffset(IDirect3DSurface9 *pOriginalSurface, SIZE *pOffset, UINT &NewWidth, UINT &NewHeight);
	HRESULT	InitializeOffset(UINT Width, UINT Height, SIZE *pOffset, UINT &NewWidth, UINT &NewHeight);
	IDirect3DTexture9* GetWideTexture(IDirect3DBaseTexture9* pTexture, IDirect3DBaseTexture9* pLeftTex);
	static CComPtr<IWideSurfaceData> GetWideSurfaceData( IDirect3DSurface9* pSurf );
	template <VIEWINDEX view>
	HRESULT SynchronizeRT( IDirect3DSurface9* pSurf, IDirect3DSurface9* pWideSurf);
	template <VIEWINDEX view>
	HRESULT SynchronizeTexture( IDirect3DTexture9* pWideTex, IDirect3DTexture9* pTex);
	template <VIEWINDEX view>
	HRESULT SynchronizeDepthTexture( IDirect3DTexture9* pWideTex, IDirect3DTexture9* pTex);
	HRESULT CheckSynchronizeRT( IDirect3DSurface9* pWideSurf);
	HRESULT CheckSynchronizeTexture( IDirect3DTexture9* pWideTex, IDirect3DTexture9* pLeftTex, IDirect3DTexture9* pRightTex);

	void	SelectOffset(bool bCW, LONG Width, LONG Height);
	void	CheckSynchronizeAll();
	void	UpdateRTsRevision(DWORD updateRTs[], bool updateDS);

	template<typename T, typename Q>
	void SetPipelineTexture( ShaderPipelineDataEx<T, Q>& pipeline, DWORD ind, IDirect3DBaseTexture9* pLeftTexture, IDirect3DTexture9* pWideTexture );

	virtual void	SynchronizeDepthBuffer( CBaseSwapChain* pSwapChain );
	virtual bool	CopyDepthSurfaces(IDirect3DTexture9* pDepthTexure, CComPtr<IDirect3DSurface9> &pLeft, CComPtr<IDirect3DSurface9> &pRight);
	virtual bool    RenderToTextures() { return m_RenderTarget[0].m_pMainSurf != GetWideSC()->m_pWidePrimaryBackBufferBeforeScaling; };

	virtual bool    RenderInStereo();
	virtual bool	StereoDS();
	virtual void    GetStereoSurfRects(IDirect3DSurface9* pLeft, IDirect3DSurface9* &pRight, D3DFORMAT &format, RECT *pLeftRect, RECT *pRightRect);
	template <VIEWINDEX view>
	void SetStereoRenderTarget( bool bDraw = true );
	virtual void SetStereoRenderTargetLeft()
	{	SetStereoRenderTarget<VIEWINDEX_LEFT>();	}
	virtual void SetStereoRenderTargetRight()
	{	SetStereoRenderTarget<VIEWINDEX_RIGHT>();	}

	virtual HRESULT StartEngineMode();
	virtual void    StopEngineMode();
	virtual	void	ViewStagesSynchronization();

	STDMETHODIMP SetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9 *pRenderTarget);
	STDMETHODIMP SetDepthStencilSurface(IDirect3DSurface9 * pNewZStencil);
	STDMETHODIMP SetTexture(DWORD Stage, IDirect3DBaseTexture9* pTexture);
	STDMETHODIMP GetTexture(DWORD Stage, IDirect3DBaseTexture9** ppTexture) ;

	STDMETHODIMP CreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, 
		D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, 
		BOOL Lockable, IDirect3DSurface9** ppSurface, HANDLE * pSharedHandle);
	STDMETHODIMP Clear(DWORD Count, CONST D3DRECT * pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil );
	STDMETHODIMP StretchRect( 
		IDirect3DSurface9 *pSourceSurface, CONST RECT * pSourceRect, 
		IDirect3DSurface9 *pDestSurface, CONST RECT * pDestRect, D3DTEXTUREFILTERTYPE Filter );

	void StretchRectToMonoDest( 
		IDirect3DSurface9 * pSourceSurface, CONST RECT * pSourceRect, CONST SIZE* pSourceOffset, 
		IDirect3DSurface9 * pDestSurface, CONST RECT * pDestRect, D3DTEXTUREFILTERTYPE Filter );
	void StretchRectToWideDest( 
		IDirect3DSurface9 *pSourceSurface, CONST RECT * pSourceRect, CONST SIZE* pSourceOffset, 
		IDirect3DSurface9 *pDestSurface, CONST RECT * pDestRect, CONST SIZE* pDestOffset, D3DTEXTUREFILTERTYPE Filter );

	STDMETHODIMP CreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, 
		D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, 
		BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle );
	STDMETHODIMP CreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format,
		D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE * pSharedHandle);

	STDMETHODIMP CreateCubeTexture(UINT EdgeLength,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DCubeTexture9** ppCubeTexture,HANDLE* pSharedHandle) ;
	STDMETHODIMP ColorFill(IDirect3DSurface9 * pSurface, CONST RECT * pRect, D3DCOLOR color );

	STDMETHODIMP UpdateSurface(IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, 
		IDirect3DSurface9* pDestinationSurface, CONST POINT* pDestPoint);
	STDMETHODIMP UpdateTexture(IDirect3DBaseTexture9* pSourceTexture, IDirect3DBaseTexture9* pDestinationTexture);
	STDMETHODIMP GetRenderTargetData(IDirect3DSurface9* pRenderTarget, IDirect3DSurface9* pDestSurface);

	STDMETHODIMP CreateRenderTargetEx(UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Lockable,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle,DWORD Usage);
	STDMETHODIMP CreateDepthStencilSurfaceEx(UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Discard,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle,DWORD Usage);
	friend CWideSwapChain;
};

bool GetLeftTexture(IDirect3DBaseTexture9* pWideTexture, IDirect3DBaseTexture9** ppLeftTex);
bool GetLeftTexture(IDirect3DBaseTexture9* pWideTexture, IDirect3DTexture9** ppLeftTex);
IDirect3DBaseTexture9* GetWideTexture(IDirect3DBaseTexture9* pLeftTexture);
HRESULT GetRightViewRect(RECT *pDestRect, CONST RECT *pSourceRect, CONST SIZE* pOffset);
HRESULT GetRightViewRect(D3DRECT *pDestRect, CONST D3DRECT *pSourceRect, CONST SIZE* pOffset);
HRESULT GetRightViewPort(D3DVIEWPORT9 *pDestViewPort, CONST D3DVIEWPORT9 *pSourceViewPort, CONST SIZE* pOffset);
HRESULT GetNarrowViewPort(D3DVIEWPORT9 *pDestViewPort, CONST D3DVIEWPORT9 *pSourceViewPort);
HRESULT GetNarrowViewPort(D3DVIEWPORT9 *pDestViewPort);
void GetSurfRect( D3DSURFACE_DESC *pDesc, RECT *pDestRect );
HRESULT GetNarrowRect(RECT *pDestRect);
HRESULT GetNarrowRect( IDirect3DSurface9 *pSurface, RECT *pDestRect );

inline HRESULT	CWideStereoRenderer::InitializeOffset(UINT Width, UINT Height, SIZE *pOffset, UINT &NewWidth, UINT &NewHeight)
{
	HRESULT hResult = S_OK;
	NewWidth = Width;
	pOffset->cx = 0;
	if ((HEIGHTMUL * Height) > m_dwMaxTextureHeight)
	{
		DEBUG_MESSAGE(_T("WARNING: (%d * Height [%d]) > m_dwMaxTextureHeight (%d)!\n"), 
			HEIGHTMUL, Height, m_dwMaxTextureHeight);
		m_bDebugWarningCantCreateWideSurface = TRUE;
		NewHeight = Height;
		pOffset->cy = 0;
		return E_FAIL;
	}
	NewHeight = Height * HEIGHTMUL;
	pOffset->cy = Height;
	return hResult;
}

