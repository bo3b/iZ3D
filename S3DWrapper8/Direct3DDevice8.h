/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include <vector>

class CDirect3DDevice8: 
	public CWrapper<IDirect3DDevice8, IDirect3DDevice9>,
	public CComCoClass<CDirect3DDevice8>
{
public:
	friend class CDirect3D8;
	CDirect3DDevice8();
	virtual ~CDirect3DDevice8();
	BEGIN_COM_MAP(CDirect3DDevice8)
		COM_INTERFACE_ENTRY(IDirect3DDevice8)
		COM_INTERFACE_ENTRY(IWrapper)
	END_COM_MAP()
	IMPLEMENT_INTERNAL_RELEASE_STRONG();

public:
	// IDirect3DDevice8
	STDMETHODIMP TestCooperativeLevel();
	STDMETHODIMP_(UINT) GetAvailableTextureMem();
	STDMETHODIMP ResourceManagerDiscardBytes(DWORD Bytes);
	STDMETHODIMP GetDirect3D( IDirect3D8** ppD3D8);
	STDMETHODIMP GetDeviceCaps( D3DCAPS8* pCaps);
	STDMETHODIMP GetDisplayMode( D3DDISPLAYMODE* pMode);
	STDMETHODIMP GetCreationParameters( D3DDEVICE_CREATION_PARAMETERS *pParameters);
	STDMETHODIMP SetCursorProperties( UINT XHotSpot,UINT YHotSpot,IDirect3DSurface8* pCursorBitmap);
	STDMETHODIMP_(void) SetCursorPosition( int X,int Y,DWORD Flags);
	STDMETHODIMP_(BOOL) ShowCursor( BOOL bShow);
	STDMETHODIMP CreateAdditionalSwapChain( D3DPRESENT_PARAMETERS8* pPresentationParameters,IDirect3DSwapChain8** pSwapChain);
	STDMETHODIMP Reset( D3DPRESENT_PARAMETERS8* pPresentationParameters);
	STDMETHODIMP Present( CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion);
	STDMETHODIMP GetBackBuffer( UINT BackBuffer,D3DBACKBUFFER_TYPE Type,IDirect3DSurface8** ppBackBuffer);
	STDMETHODIMP GetRasterStatus( D3DRASTER_STATUS* pRasterStatus);
	STDMETHODIMP_(void) SetGammaRamp( DWORD Flags,CONST D3DGAMMARAMP* pRamp);
	STDMETHODIMP_(void) GetGammaRamp( D3DGAMMARAMP* pRamp);
	STDMETHODIMP CreateTexture( UINT Width,UINT Height,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DTexture8** ppTexture);
	STDMETHODIMP CreateVolumeTexture( UINT Width,UINT Height,UINT Depth,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DVolumeTexture8** ppVolumeTexture);
	STDMETHODIMP CreateCubeTexture( UINT EdgeLength,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DCubeTexture8** ppCubeTexture);
	STDMETHODIMP CreateVertexBuffer( UINT Length,DWORD Usage,DWORD FVF,D3DPOOL Pool,IDirect3DVertexBuffer8** ppVertexBuffer);
	STDMETHODIMP CreateIndexBuffer( UINT Length,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DIndexBuffer8** ppIndexBuffer);
	STDMETHODIMP CreateRenderTarget( UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,BOOL Lockable,IDirect3DSurface8** ppSurface);
	STDMETHODIMP CreateDepthStencilSurface( UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,IDirect3DSurface8** ppSurface);
	STDMETHODIMP CreateImageSurface( UINT Width,UINT Height,D3DFORMAT Format,IDirect3DSurface8** ppSurface);
	STDMETHODIMP CopyRects( IDirect3DSurface8* pSourceSurface,CONST RECT* pSourceRectsArray,UINT cRects,IDirect3DSurface8* pDestinationSurface,CONST POINT* pDestPointsArray);
	STDMETHODIMP UpdateTexture( IDirect3DBaseTexture8* pSourceTexture,IDirect3DBaseTexture8* pDestinationTexture);
	STDMETHODIMP GetFrontBuffer( IDirect3DSurface8* pDestSurface);
	STDMETHODIMP SetRenderTarget( IDirect3DSurface8* pRenderTarget,IDirect3DSurface8* pNewZStencil);
	STDMETHODIMP GetRenderTarget( IDirect3DSurface8** ppRenderTarget);
	STDMETHODIMP GetDepthStencilSurface(IDirect3DSurface8** ppZStencilSurface);
	STDMETHODIMP BeginScene();
	STDMETHODIMP EndScene();
	STDMETHODIMP Clear( DWORD Count,CONST D3DRECT* pRects,DWORD Flags,D3DCOLOR Color,float Z,DWORD Stencil);
	STDMETHODIMP SetTransform( D3DTRANSFORMSTATETYPE State,CONST D3DMATRIX* pMatrix);
	STDMETHODIMP GetTransform( D3DTRANSFORMSTATETYPE State,D3DMATRIX* pMatrix);
	STDMETHODIMP MultiplyTransform( D3DTRANSFORMSTATETYPE State,CONST D3DMATRIX* pMatrix);
	STDMETHODIMP SetViewport( CONST D3DVIEWPORT8* pViewport);
	STDMETHODIMP GetViewport( D3DVIEWPORT8* pViewport);
	STDMETHODIMP SetMaterial( CONST D3DMATERIAL8* pMaterial);
	STDMETHODIMP GetMaterial( D3DMATERIAL8* pMaterial);
	STDMETHODIMP SetLight( DWORD Index,CONST D3DLIGHT8* pLight);
	STDMETHODIMP GetLight( DWORD Index,D3DLIGHT8* pLight);
	STDMETHODIMP LightEnable( DWORD Index,BOOL Enable);
	STDMETHODIMP GetLightEnable( DWORD Index,BOOL* pEnable);
	STDMETHODIMP SetClipPlane( DWORD Index,CONST float* pPlane);
	STDMETHODIMP GetClipPlane( DWORD Index,float* pPlane);
	STDMETHODIMP SetRenderState( D3DRENDERSTATETYPE State,DWORD Value);
	STDMETHODIMP GetRenderState( D3DRENDERSTATETYPE State,DWORD* pValue);
	STDMETHODIMP BeginStateBlock();
	STDMETHODIMP EndStateBlock( DWORD* pToken);
	STDMETHODIMP ApplyStateBlock( DWORD Token);
	STDMETHODIMP CaptureStateBlock( DWORD Token);
	STDMETHODIMP DeleteStateBlock( DWORD Token);
	STDMETHODIMP CreateStateBlock( D3DSTATEBLOCKTYPE Type,DWORD* pToken);
	STDMETHODIMP SetClipStatus( CONST D3DCLIPSTATUS8* pClipStatus);
	STDMETHODIMP GetClipStatus( D3DCLIPSTATUS8* pClipStatus);
	STDMETHODIMP GetTexture( DWORD Stage,IDirect3DBaseTexture8** ppTexture);
	STDMETHODIMP SetTexture( DWORD Stage,IDirect3DBaseTexture8* pTexture);
	STDMETHODIMP GetTextureStageState( DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD* pValue);
	STDMETHODIMP SetTextureStageState( DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD Value);
	STDMETHODIMP ValidateDevice( DWORD* pNumPasses);
	STDMETHODIMP GetInfo( DWORD DevInfoID,void* pDevInfoStruct,DWORD DevInfoStructSize);
	STDMETHODIMP SetPaletteEntries( UINT PaletteNumber,CONST PALETTEENTRY* pEntries);
	STDMETHODIMP GetPaletteEntries( UINT PaletteNumber,PALETTEENTRY* pEntries);
	STDMETHODIMP SetCurrentTexturePalette( UINT PaletteNumber);
	STDMETHODIMP GetCurrentTexturePalette( UINT *PaletteNumber);
	STDMETHODIMP DrawPrimitive( D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount);
	STDMETHODIMP DrawIndexedPrimitive( D3DPRIMITIVETYPE,UINT minIndex,UINT NumVertices,UINT startIndex,UINT primCount);
	STDMETHODIMP DrawPrimitiveUP( D3DPRIMITIVETYPE PrimitiveType,UINT PrimitiveCount,CONST void* pVertexStreamZeroData,UINT VertexStreamZeroStride);
	STDMETHODIMP DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType,UINT MinVertexIndex,UINT NumVertexIndices,UINT PrimitiveCount,CONST void* pIndexData,D3DFORMAT IndexDataFormat,CONST void* pVertexStreamZeroData,UINT VertexStreamZeroStride);
	STDMETHODIMP ProcessVertices( UINT SrcStartIndex,UINT DestIndex,UINT VertexCount,IDirect3DVertexBuffer8* pDestBuffer,DWORD Flags);
	STDMETHODIMP CreateVertexShader( CONST DWORD* pDeclaration,CONST DWORD* pFunction,DWORD* pHandle,DWORD Usage);
	STDMETHODIMP SetVertexShader( DWORD Handle);
	STDMETHODIMP GetVertexShader( DWORD* pHandle);
	STDMETHODIMP DeleteVertexShader( DWORD Handle);
	STDMETHODIMP SetVertexShaderConstant( DWORD Register,CONST void* pConstantData,DWORD ConstantCount);
	STDMETHODIMP GetVertexShaderConstant( DWORD Register,void* pConstantData,DWORD ConstantCount);
	STDMETHODIMP GetVertexShaderDeclaration( DWORD Handle,void* pData,DWORD* pSizeOfData);
	STDMETHODIMP GetVertexShaderFunction( DWORD Handle,void* pData,DWORD* pSizeOfData);
	STDMETHODIMP SetStreamSource( UINT StreamNumber,IDirect3DVertexBuffer8* pStreamData,UINT Stride);
	STDMETHODIMP GetStreamSource( UINT StreamNumber,IDirect3DVertexBuffer8** ppStreamData,UINT* pStride);
	STDMETHODIMP SetIndices( IDirect3DIndexBuffer8* pIndexData,UINT BaseVertexIndex);
	STDMETHODIMP GetIndices( IDirect3DIndexBuffer8** ppIndexData,UINT* pBaseVertexIndex);
	STDMETHODIMP CreatePixelShader( CONST DWORD* pFunction,DWORD* pHandle);
	STDMETHODIMP SetPixelShader( DWORD Handle);
	STDMETHODIMP GetPixelShader( DWORD* pHandle);
	STDMETHODIMP DeletePixelShader( DWORD Handle);
	STDMETHODIMP SetPixelShaderConstant( DWORD Register,CONST void* pConstantData,DWORD ConstantCount);
	STDMETHODIMP GetPixelShaderConstant( DWORD Register,void* pConstantData,DWORD ConstantCount);
	STDMETHODIMP GetPixelShaderFunction( DWORD Handle,void* pData,DWORD* pSizeOfData);
	STDMETHODIMP DrawRectPatch( UINT Handle,CONST float* pNumSegs,CONST D3DRECTPATCH_INFO* pRectPatchInfo);
	STDMETHODIMP DrawTriPatch( UINT Handle,CONST float* pNumSegs,CONST D3DTRIPATCH_INFO* pTriPatchInfo);
	STDMETHODIMP DeletePatch( UINT Handle);

	inline void Init( CDirect3D8* pDirect3D, UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT BackBufferFormat ) 
	{
		m_pDirect3D = pDirect3D;
		m_Adapter = Adapter;
		m_DeviceType = DeviceType;
		m_BackBufferFormat = BackBufferFormat;
	}
	void CreateMcClaud();
	void KillMcClaud();
private:
	INT m_BaseVertexIndex;
	CDirect3D8* m_pDirect3D;
	UINT m_Adapter;
	D3DDEVTYPE m_DeviceType;
	D3DFORMAT m_BackBufferFormat;
	std::vector<DWORD> m_VSHandle;
	std::vector<DWORD> m_PSHandle;
	std::vector<DWORD> m_SBHandle;

	STDMETHODIMP CopyRectsUsingLock( IDirect3DSurface9* pSrc,D3DSURFACE_DESC &sourceDesc,CONST RECT* pSourceRectsArray,UINT cRects,
		IDirect3DSurface9* pDest,D3DSURFACE_DESC &destDesc,CONST POINT* pDestPointsArray);
};

void ConvertPresentParameters(D3DPRESENT_PARAMETERS8 *pPpDx8, D3DPRESENT_PARAMETERS *pPpDx9);
UINT BitsPerPixel( D3DFORMAT fmt );
