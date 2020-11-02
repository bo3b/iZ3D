#pragma once

#ifndef OUTPUT_LIBRARY

#include "States.h"
#include "Trace.h"
#include "Hook.h"

#undef	PROXYMETHOD
#undef	PROXYMETHOD_
#undef	CALLORIGINAL
#undef	CALLORIGINALEX
#define PROXYMETHOD(a)			inline HRESULT  ProxyDevice9::a 
#define PROXYMETHOD_(type, a)	inline type ProxyDevice9::a
#define CALLORIGINAL(a)			(m_pHookedInterface->*m_fpOriginal.a)
#define CALLORIGINALEX(a)		(((IDirect3DDevice9Ex*)m_pHookedInterface)->*m_fpOriginal.a)

// IUnknown methods 
PROXYMETHOD(QueryInterface)(REFIID riid, void** ppvObj)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(QueryInterface)(riid, ppvObj);
}

PROXYMETHOD_(ULONG,AddRef)() 
{
	CHECKMODIFICATION();
	ULONG a = CALLORIGINAL(AddRef)();
	return a;
}

PROXYMETHOD_(ULONG,Release)()
{
	CHECKMODIFICATION();
	return CALLORIGINAL(Release)();
}

// IDirect3DDevice9 methods
PROXYMETHOD(TestCooperativeLevel)()
{
	CHECKMODIFICATION();
	return CALLORIGINAL(TestCooperativeLevel)();
}

PROXYMETHOD_(UINT, GetAvailableTextureMem)()
{
	CHECKMODIFICATION();
	return CALLORIGINAL(GetAvailableTextureMem)();
}

PROXYMETHOD(EvictManagedResources)()
{
	CHECKMODIFICATION();
	return CALLORIGINAL(EvictManagedResources)();
}

PROXYMETHOD(GetDirect3D)(IDirect3D9** ppD3D9)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(GetDirect3D)(ppD3D9);
}

PROXYMETHOD(GetDeviceCaps)(D3DCAPS9* pCaps)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(GetDeviceCaps)(pCaps);
}

PROXYMETHOD(GetDisplayMode)(UINT iSwapChain,D3DDISPLAYMODE* pMode)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(GetDisplayMode)(iSwapChain, pMode);
}

PROXYMETHOD(GetCreationParameters)(D3DDEVICE_CREATION_PARAMETERS *pParameters)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(GetCreationParameters)(pParameters);
}

PROXYMETHOD(SetCursorProperties)(UINT XHotSpot,UINT YHotSpot,IDirect3DSurface9* pCursorBitmap)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(SetCursorProperties)(XHotSpot, YHotSpot, pCursorBitmap);
}

PROXYMETHOD_(void, SetCursorPosition)(int X,int Y,DWORD Flags)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(SetCursorPosition)(X, Y, Flags);
}

PROXYMETHOD_(BOOL, ShowCursor)(BOOL bShow)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(ShowCursor)(bShow);
}

PROXYMETHOD(CreateAdditionalSwapChain)(D3DPRESENT_PARAMETERS* pPresentationParameters,IDirect3DSwapChain9** pSwapChain)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(CreateAdditionalSwapChain)(pPresentationParameters, pSwapChain);
}

PROXYMETHOD(GetSwapChain)(UINT iSwapChain,IDirect3DSwapChain9** pSwapChain)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(GetSwapChain)(iSwapChain, pSwapChain);
}

PROXYMETHOD_(UINT, GetNumberOfSwapChains)()
{
	CHECKMODIFICATION();
	return CALLORIGINAL(GetNumberOfSwapChains)();
}

PROXYMETHOD(Reset)(D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(Reset)(pPresentationParameters); 
}

PROXYMETHOD(Present)(CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(Present)(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

PROXYMETHOD(GetBackBuffer)(UINT iSwapChain,UINT iBackBuffer,D3DBACKBUFFER_TYPE Type,IDirect3DSurface9** ppBackBuffer)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(GetBackBuffer)(iSwapChain, iBackBuffer, Type,ppBackBuffer);
}

PROXYMETHOD(GetRasterStatus)(UINT iSwapChain,D3DRASTER_STATUS* pRasterStatus)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(GetRasterStatus)(iSwapChain, pRasterStatus);
}

PROXYMETHOD(SetDialogBoxMode)(BOOL bEnableDialogs)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(SetDialogBoxMode)(bEnableDialogs);
}

PROXYMETHOD_(void, SetGammaRamp)(UINT iSwapChain,DWORD Flags,CONST D3DGAMMARAMP* pRamp)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(SetGammaRamp)(iSwapChain, Flags, pRamp);
}

PROXYMETHOD_(void, GetGammaRamp)(UINT iSwapChain,D3DGAMMARAMP* pRamp)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(GetGammaRamp)(iSwapChain, pRamp);
}

extern const GUID PitchCorrectionGUID;

struct ProxyPitchData
{
	ProxyPitchData() { pData = NULL; }
	void*			pData;
	INT				dataPitch;
	D3DLOCKED_RECT	lockedRect;
	RECT			rect;
	UINT			lineCount; //--- for compressed formats ---
};

void Proxy_Pitch(IDirect3DSurface9* surface, D3DLOCKED_RECT * pLockedRect, CONST RECT * pRect, DWORD Flags);
void Proxy_UnPitch(IDirect3DSurface9* surface);

PROXYMETHOD(CreateTexture)(UINT Width,UINT Height,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DTexture9** ppTexture,HANDLE* pSharedHandle)
{
	CHECKMODIFICATION();
	
	bool pitchCorrection = false; 
	if (m_ExMode == EXMODE_EMULATE && Pool == D3DPOOL_MANAGED)
	{
		Pool = D3DPOOL_DEFAULT;
		Usage |= D3DUSAGE_DYNAMIC;
		pitchCorrection = true;
	}
	HRESULT hr = CALLORIGINAL(CreateTexture)(Width, Height, Levels, Usage, Format, Pool, ppTexture, pSharedHandle);
	if(SUCCEEDED(hr) && pitchCorrection)
	{
		ProxyPitchData p;
		for(DWORD i=0; i< (*ppTexture)->GetLevelCount(); i++)
		{
			CComPtr<IDirect3DSurface9> s;
			(*ppTexture)->GetSurfaceLevel(i, &s);
			s->SetPrivateData(PitchCorrectionGUID, &p, sizeof(p), 0); 
		}
	}

	return hr;
}

PROXYMETHOD(CreateVolumeTexture)(UINT Width,UINT Height,UINT Depth,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DVolumeTexture9** ppVolumeTexture,HANDLE* pSharedHandle)
{
	CHECKMODIFICATION();
	if (m_ExMode == EXMODE_EMULATE && Pool == D3DPOOL_MANAGED)
	{
		Pool = D3DPOOL_DEFAULT;
		Usage |= D3DUSAGE_DYNAMIC;
	}
	return CALLORIGINAL(CreateVolumeTexture)(Width, Height, Depth, Levels, Usage, Format, Pool, ppVolumeTexture, pSharedHandle);
}

PROXYMETHOD(CreateCubeTexture)(UINT EdgeLength,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DCubeTexture9** ppCubeTexture,HANDLE* pSharedHandle)
{
	CHECKMODIFICATION();
	if (m_ExMode == EXMODE_EMULATE && Pool == D3DPOOL_MANAGED)
	{
		Pool = D3DPOOL_DEFAULT;
		Usage |= D3DUSAGE_DYNAMIC;
	}
	return CALLORIGINAL(CreateCubeTexture)(EdgeLength, Levels, Usage, Format, Pool, ppCubeTexture, pSharedHandle);
}

PROXYMETHOD(CreateVertexBuffer)(UINT Length,DWORD Usage,DWORD FVF,D3DPOOL Pool,IDirect3DVertexBuffer9** ppVertexBuffer,HANDLE* pSharedHandle)
{
	CHECKMODIFICATION();
	if (m_ExMode == EXMODE_EMULATE && Pool == D3DPOOL_MANAGED)
		Pool = D3DPOOL_DEFAULT;
	return CALLORIGINAL(CreateVertexBuffer)(Length, Usage, FVF, Pool, ppVertexBuffer, pSharedHandle);
}

PROXYMETHOD(CreateIndexBuffer)(UINT Length,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DIndexBuffer9** ppIndexBuffer,HANDLE* pSharedHandle)
{
	CHECKMODIFICATION();
	if (m_ExMode == EXMODE_EMULATE && Pool == D3DPOOL_MANAGED)
		Pool = D3DPOOL_DEFAULT;
	return CALLORIGINAL(CreateIndexBuffer)(Length, Usage, Format, Pool, ppIndexBuffer, pSharedHandle);
}

PROXYMETHOD(CreateRenderTarget)(UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Lockable,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(CreateRenderTarget)(Width, Height, Format, MultiSample, MultisampleQuality, Lockable, ppSurface, pSharedHandle);
}

PROXYMETHOD(CreateDepthStencilSurface)(UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Discard,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(CreateDepthStencilSurface)(Width,  Height, Format, MultiSample, MultisampleQuality, Discard, ppSurface, pSharedHandle);
}

PROXYMETHOD(UpdateSurface)(IDirect3DSurface9* pSourceSurface,CONST RECT* pSourceRect,IDirect3DSurface9* pDestinationSurface,CONST POINT* pDestPoint)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(UpdateSurface)(pSourceSurface, pSourceRect, pDestinationSurface,  pDestPoint);
}

PROXYMETHOD(UpdateTexture)(IDirect3DBaseTexture9* pSourceTexture,IDirect3DBaseTexture9* pDestinationTexture)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(UpdateTexture)(pSourceTexture, pDestinationTexture);
}

PROXYMETHOD(GetRenderTargetData)(IDirect3DSurface9* pRenderTarget,IDirect3DSurface9* pDestSurface)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(GetRenderTargetData)(pRenderTarget, pDestSurface);
}

PROXYMETHOD(GetFrontBufferData)(UINT iSwapChain,IDirect3DSurface9* pDestSurface)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(GetFrontBufferData)(iSwapChain, pDestSurface);
}

PROXYMETHOD(StretchRect)(IDirect3DSurface9* pSourceSurface,CONST RECT* pSourceRect,IDirect3DSurface9* pDestSurface,CONST RECT* pDestRect,D3DTEXTUREFILTERTYPE Filter)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(StretchRect)(pSourceSurface, pSourceRect, pDestSurface, pDestRect, Filter);
}

PROXYMETHOD(ColorFill)(IDirect3DSurface9* pSurface,CONST RECT* pRect,D3DCOLOR color)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(ColorFill)(pSurface, pRect, color);
}

PROXYMETHOD(CreateOffscreenPlainSurface)(UINT Width,UINT Height,D3DFORMAT Format,D3DPOOL Pool,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(CreateOffscreenPlainSurface)(Width, Height, Format, Pool, ppSurface, pSharedHandle);
}

PROXYMETHOD(SetRenderTarget)(DWORD RenderTargetIndex,IDirect3DSurface9* pRenderTarget)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(SetRenderTarget)(RenderTargetIndex, pRenderTarget);
}

PROXYMETHOD(GetRenderTarget)(DWORD RenderTargetIndex,IDirect3DSurface9** ppRenderTarget)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(GetRenderTarget)(RenderTargetIndex, ppRenderTarget);
}

PROXYMETHOD(SetDepthStencilSurface)(IDirect3DSurface9* pNewZStencil)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(SetDepthStencilSurface)(pNewZStencil);
}

PROXYMETHOD(GetDepthStencilSurface)(IDirect3DSurface9** ppZStencilSurface)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(GetDepthStencilSurface)(ppZStencilSurface);
}

PROXYMETHOD(BeginScene)()
{
	CHECKMODIFICATION();
	return CALLORIGINAL(BeginScene)();
}

PROXYMETHOD(EndScene)()
{
	CHECKMODIFICATION();
	return CALLORIGINAL(EndScene)();
}

PROXYMETHOD(Clear)(DWORD Count,CONST D3DRECT* pRects,DWORD Flags,D3DCOLOR Color,float Z,DWORD Stencil)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(Clear)(Count, pRects, Flags, Color, Z, Stencil);
}

PROXYMETHOD(SetTransform)(D3DTRANSFORMSTATETYPE State,CONST D3DMATRIX* pMatrix)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(SetTransform)(State, pMatrix);
}

PROXYMETHOD(GetTransform)(D3DTRANSFORMSTATETYPE State,D3DMATRIX* pMatrix)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(GetTransform)(State, pMatrix);
}

PROXYMETHOD(MultiplyTransform)(D3DTRANSFORMSTATETYPE State,CONST D3DMATRIX* pMatrix)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(MultiplyTransform)(State, pMatrix);
}

PROXYMETHOD(SetViewport)(CONST D3DVIEWPORT9* pViewport)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(SetViewport)(pViewport);
}

PROXYMETHOD(GetViewport)(D3DVIEWPORT9* pViewport)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(GetViewport)(pViewport);
}

PROXYMETHOD(SetMaterial)(CONST D3DMATERIAL9* pMaterial)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(SetMaterial)(pMaterial);
}

PROXYMETHOD(GetMaterial)(D3DMATERIAL9* pMaterial)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(GetMaterial)(pMaterial);
}

PROXYMETHOD(SetLight)(DWORD Index,CONST D3DLIGHT9* light)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(SetLight)(Index, light);
}

PROXYMETHOD(GetLight)(DWORD Index,D3DLIGHT9* light)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(GetLight)(Index, light);
}

PROXYMETHOD(LightEnable)(DWORD Index,BOOL Enable)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(LightEnable)(Index, Enable);
}

PROXYMETHOD(GetLightEnable)(DWORD Index,BOOL* pEnable)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(GetLightEnable)(Index, pEnable);
}

PROXYMETHOD(SetClipPlane)(DWORD Index,CONST float* pPlane)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(SetClipPlane)(Index, pPlane);
}

PROXYMETHOD(GetClipPlane)(DWORD Index,float* pPlane)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(GetClipPlane)(Index, pPlane);
}

PROXYMETHOD(SetRenderState)(D3DRENDERSTATETYPE State,DWORD Value)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(SetRenderState)(State, Value);
}

PROXYMETHOD(GetRenderState)(D3DRENDERSTATETYPE State,DWORD* pValue)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(GetRenderState)(State, pValue);
}

PROXYMETHOD(CreateStateBlock)(D3DSTATEBLOCKTYPE Type,IDirect3DStateBlock9** ppSB)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(CreateStateBlock)(Type, ppSB);
}

PROXYMETHOD(BeginStateBlock)()
{
	CHECKMODIFICATION();
	HRESULT hr = CALLORIGINAL(BeginStateBlock)();
	if (SUCCEEDED(hr))
	{
		using namespace IDirect3DDevice9MethodNames;
		m_fpOriginal.func[_SetMaterial_] = m_fpMutableFunctions[1].func[0];
		m_fpOriginal.func[_SetRenderState_] = m_fpMutableFunctions[1].func[1];
		m_fpOriginal.func[_SetTexture_] = m_fpMutableFunctions[1].func[2];
		m_fpOriginal.func[_SetTextureStageState_] = m_fpMutableFunctions[1].func[3];
		m_fpOriginal.func[_SetSamplerState_] = m_fpMutableFunctions[1].func[4];
		m_fpOriginal.func[_SetFVF_] = m_fpMutableFunctions[1].func[5];
		m_fpOriginal.func[_SetVertexShader_] = m_fpMutableFunctions[1].func[6];
		m_fpOriginal.func[_SetVertexShaderConstantF_] = m_fpMutableFunctions[1].func[7];
		m_fpOriginal.func[_SetVertexShaderConstantI_] = m_fpMutableFunctions[1].func[8];
		m_fpOriginal.func[_SetVertexShaderConstantB_] = m_fpMutableFunctions[1].func[9];
		m_fpOriginal.func[_SetStreamSource_] = m_fpMutableFunctions[1].func[10];
		m_fpOriginal.func[_SetStreamSourceFreq_] = m_fpMutableFunctions[1].func[11];
		m_fpOriginal.func[_SetIndices_] = m_fpMutableFunctions[1].func[12];
		m_fpOriginal.func[_SetPixelShader_] = m_fpMutableFunctions[1].func[13];
		m_fpOriginal.func[_SetPixelShaderConstantF_] = m_fpMutableFunctions[1].func[14];
		m_fpOriginal.func[_SetPixelShaderConstantI_] = m_fpMutableFunctions[1].func[15];
		m_fpOriginal.func[_SetPixelShaderConstantB_] = m_fpMutableFunctions[1].func[16];
	}
	return hr;
}

PROXYMETHOD(EndStateBlock)(IDirect3DStateBlock9** ppSB)
{
	CHECKMODIFICATION();
	HRESULT hr = CALLORIGINAL(EndStateBlock)(ppSB);
	if (SUCCEEDED(hr))
	{
		using namespace IDirect3DDevice9MethodNames;
		m_fpOriginal.func[_SetMaterial_] = m_fpMutableFunctions[0].func[0];
		m_fpOriginal.func[_SetRenderState_] = m_fpMutableFunctions[0].func[1];
		m_fpOriginal.func[_SetTexture_] = m_fpMutableFunctions[0].func[2];
		m_fpOriginal.func[_SetTextureStageState_] = m_fpMutableFunctions[0].func[3];
		m_fpOriginal.func[_SetSamplerState_] = m_fpMutableFunctions[0].func[4];
		m_fpOriginal.func[_SetFVF_] = m_fpMutableFunctions[0].func[5];
		m_fpOriginal.func[_SetVertexShader_] = m_fpMutableFunctions[0].func[6];
		m_fpOriginal.func[_SetVertexShaderConstantF_] = m_fpMutableFunctions[0].func[7];
		m_fpOriginal.func[_SetVertexShaderConstantI_] = m_fpMutableFunctions[0].func[8];
		m_fpOriginal.func[_SetVertexShaderConstantB_] = m_fpMutableFunctions[0].func[9];
		m_fpOriginal.func[_SetStreamSource_] = m_fpMutableFunctions[0].func[10];
		m_fpOriginal.func[_SetStreamSourceFreq_] = m_fpMutableFunctions[0].func[11];
		m_fpOriginal.func[_SetIndices_] = m_fpMutableFunctions[0].func[12];
		m_fpOriginal.func[_SetPixelShader_] = m_fpMutableFunctions[0].func[13];
		m_fpOriginal.func[_SetPixelShaderConstantF_] = m_fpMutableFunctions[0].func[14];
		m_fpOriginal.func[_SetPixelShaderConstantI_] = m_fpMutableFunctions[0].func[15];
		m_fpOriginal.func[_SetPixelShaderConstantB_] = m_fpMutableFunctions[0].func[16];
	}
	return hr;
}

PROXYMETHOD(SetClipStatus)(CONST D3DCLIPSTATUS9* pClipStatus)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(SetClipStatus)(pClipStatus);
}

PROXYMETHOD(GetClipStatus)(D3DCLIPSTATUS9* pClipStatus)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(GetClipStatus)(pClipStatus);
}

PROXYMETHOD(GetTexture)(DWORD Stage,IDirect3DBaseTexture9** ppTexture)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(GetTexture)(Stage, ppTexture);
}

PROXYMETHOD(SetTexture)(DWORD Stage,IDirect3DBaseTexture9* pTexture)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(SetTexture)(Stage, pTexture);
}

PROXYMETHOD(GetTextureStageState)(DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD* pValue)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(GetTextureStageState)(Stage, Type, pValue);
}

PROXYMETHOD(SetTextureStageState)(DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD Value)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(SetTextureStageState)(Stage, Type, Value);
}

PROXYMETHOD(GetSamplerState)(DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD* pValue)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(GetSamplerState)(Sampler, Type, pValue);
}

PROXYMETHOD(SetSamplerState)(DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD Value)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(SetSamplerState)(Sampler, Type, Value);
}

PROXYMETHOD(ValidateDevice)(DWORD* pNumPasses)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(ValidateDevice)(pNumPasses);
}

PROXYMETHOD(SetPaletteEntries)(UINT PaletteNumber,CONST PALETTEENTRY* pEntries)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(SetPaletteEntries)(PaletteNumber, pEntries);
}

PROXYMETHOD(GetPaletteEntries)(UINT PaletteNumber,PALETTEENTRY* pEntries)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(GetPaletteEntries)(PaletteNumber, pEntries);
}

PROXYMETHOD(SetCurrentTexturePalette)(UINT PaletteNumber)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(SetCurrentTexturePalette)(PaletteNumber);
}

PROXYMETHOD(GetCurrentTexturePalette)(UINT *PaletteNumber)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(GetCurrentTexturePalette)(PaletteNumber);
}

PROXYMETHOD(SetScissorRect)(CONST RECT* pRect)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(SetScissorRect)(pRect);
}

PROXYMETHOD(GetScissorRect)(RECT* pRect)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(GetScissorRect)(pRect);
}

PROXYMETHOD(SetSoftwareVertexProcessing)(BOOL bSoftware)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(SetSoftwareVertexProcessing)(bSoftware);
}

PROXYMETHOD_(BOOL, GetSoftwareVertexProcessing)()
{
	CHECKMODIFICATION();
	return CALLORIGINAL(GetSoftwareVertexProcessing)();
}

PROXYMETHOD(SetNPatchMode)(float nSegments)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(SetNPatchMode)(nSegments);
}

PROXYMETHOD_(float, GetNPatchMode)()
{
	CHECKMODIFICATION();
	return CALLORIGINAL(GetNPatchMode)();
}

PROXYMETHOD(DrawPrimitive)(D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(DrawPrimitive)(PrimitiveType, StartVertex, PrimitiveCount);
}

PROXYMETHOD(DrawIndexedPrimitive)(D3DPRIMITIVETYPE PrimitiveType,INT BaseVertexIndex,UINT MinVertexIndex,UINT NumVertices,UINT startIndex,UINT primCount)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(DrawIndexedPrimitive)(PrimitiveType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
}

PROXYMETHOD(DrawPrimitiveUP)(D3DPRIMITIVETYPE PrimitiveType,UINT PrimitiveCount,CONST void* pVertexStreamZeroData,UINT VertexStreamZeroStride)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(DrawPrimitiveUP)(PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride);
}

PROXYMETHOD(DrawIndexedPrimitiveUP)(D3DPRIMITIVETYPE PrimitiveType,UINT MinVertexIndex,UINT NumVertices,UINT PrimitiveCount,CONST void* pIndexData,D3DFORMAT IndexDataFormat,CONST void* pVertexStreamZeroData,UINT VertexStreamZeroStride)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(DrawIndexedPrimitiveUP)(PrimitiveType, MinVertexIndex, NumVertices, PrimitiveCount, pIndexData, IndexDataFormat, pVertexStreamZeroData, VertexStreamZeroStride);
}

PROXYMETHOD(ProcessVertices)(UINT SrcStartIndex,UINT DestIndex,UINT VertexCount,IDirect3DVertexBuffer9* pDestBuffer,IDirect3DVertexDeclaration9* pVertexDecl,DWORD Flags)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(ProcessVertices)(SrcStartIndex, DestIndex, VertexCount, pDestBuffer, pVertexDecl, Flags);
}

PROXYMETHOD(CreateVertexDeclaration)(CONST D3DVERTEXELEMENT9* pVertexElements,IDirect3DVertexDeclaration9** ppDecl)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(CreateVertexDeclaration)(pVertexElements, ppDecl);
}

PROXYMETHOD(SetVertexDeclaration)(IDirect3DVertexDeclaration9* pDecl)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(SetVertexDeclaration)(pDecl);
}

PROXYMETHOD(GetVertexDeclaration)(IDirect3DVertexDeclaration9** ppDecl)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(GetVertexDeclaration)(ppDecl);
}

PROXYMETHOD(SetFVF)(DWORD FVF)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(SetFVF)(FVF);
}

PROXYMETHOD(GetFVF)(DWORD* pFVF)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(GetFVF)(pFVF);
}

PROXYMETHOD(CreateVertexShader)(CONST DWORD* pFunction,IDirect3DVertexShader9** ppShader)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(CreateVertexShader)(pFunction, ppShader);
}

PROXYMETHOD(SetVertexShader)(IDirect3DVertexShader9* pShader)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(SetVertexShader)(pShader);
}

PROXYMETHOD(GetVertexShader)(IDirect3DVertexShader9** ppShader)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(GetVertexShader)(ppShader);
}

PROXYMETHOD(SetVertexShaderConstantF)(UINT StartRegister,CONST float* pConstantData,UINT Vector4fCount)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(SetVertexShaderConstantF)(StartRegister, pConstantData, Vector4fCount);
}

PROXYMETHOD(GetVertexShaderConstantF)(UINT StartRegister,float* pConstantData,UINT Vector4fCount)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(GetVertexShaderConstantF)(StartRegister, pConstantData, Vector4fCount);
}

PROXYMETHOD(SetVertexShaderConstantI)(UINT StartRegister,CONST int* pConstantData,UINT Vector4iCount)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(SetVertexShaderConstantI)(StartRegister, pConstantData, Vector4iCount);
}

PROXYMETHOD(GetVertexShaderConstantI)(UINT StartRegister,int* pConstantData,UINT Vector4iCount)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(GetVertexShaderConstantI)(StartRegister, pConstantData, Vector4iCount);
}

PROXYMETHOD(SetVertexShaderConstantB)(UINT StartRegister,CONST BOOL* pConstantData,UINT  BoolCount)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(SetVertexShaderConstantB)(StartRegister, pConstantData, BoolCount);
}

PROXYMETHOD(GetVertexShaderConstantB)(UINT StartRegister,BOOL* pConstantData,UINT BoolCount)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(GetVertexShaderConstantB)(StartRegister, pConstantData, BoolCount);
}

PROXYMETHOD(SetStreamSource)(UINT StreamNumber,IDirect3DVertexBuffer9* pStreamData,UINT OffsetInBytes,UINT Stride)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(SetStreamSource)(StreamNumber, pStreamData, OffsetInBytes, Stride);
}

PROXYMETHOD(GetStreamSource)(UINT StreamNumber,IDirect3DVertexBuffer9** ppStreamData,UINT* pOffsetInBytes,UINT* pStride)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(GetStreamSource)(StreamNumber, ppStreamData, pOffsetInBytes, pStride);
}

PROXYMETHOD(SetStreamSourceFreq)(UINT StreamNumber,UINT Setting)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(SetStreamSourceFreq)(StreamNumber, Setting);
}

PROXYMETHOD(GetStreamSourceFreq)(UINT StreamNumber,UINT* pSetting)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(GetStreamSourceFreq)(StreamNumber, pSetting);
}

PROXYMETHOD(SetIndices)(IDirect3DIndexBuffer9* pIndexData)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(SetIndices)(pIndexData);
}

PROXYMETHOD(GetIndices)(IDirect3DIndexBuffer9** ppIndexData)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(GetIndices)(ppIndexData);
}

PROXYMETHOD(CreatePixelShader)(CONST DWORD* pFunction,IDirect3DPixelShader9** ppShader)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(CreatePixelShader)(pFunction, ppShader);
}

PROXYMETHOD(SetPixelShader)(IDirect3DPixelShader9* pShader)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(SetPixelShader)(pShader);
}

PROXYMETHOD(GetPixelShader)(IDirect3DPixelShader9** ppShader)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(GetPixelShader)(ppShader);
}

PROXYMETHOD(SetPixelShaderConstantF)(UINT StartRegister,CONST float* pConstantData,UINT Vector4fCount)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(SetPixelShaderConstantF)(StartRegister, pConstantData, Vector4fCount);
}

PROXYMETHOD(GetPixelShaderConstantF)(UINT StartRegister,float* pConstantData,UINT Vector4fCount)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(GetPixelShaderConstantF)(StartRegister, pConstantData, Vector4fCount);
}

PROXYMETHOD(SetPixelShaderConstantI)(UINT StartRegister,CONST int* pConstantData,UINT Vector4iCount)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(SetPixelShaderConstantI)(StartRegister, pConstantData, Vector4iCount);
}

PROXYMETHOD(GetPixelShaderConstantI)(UINT StartRegister,int* pConstantData,UINT Vector4iCount)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(GetPixelShaderConstantI)(StartRegister, pConstantData, Vector4iCount);
}

PROXYMETHOD(SetPixelShaderConstantB)(UINT StartRegister,CONST BOOL* pConstantData,UINT  BoolCount)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(SetPixelShaderConstantB)(StartRegister, pConstantData, BoolCount);
}

PROXYMETHOD(GetPixelShaderConstantB)(UINT StartRegister,BOOL* pConstantData,UINT BoolCount)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(GetPixelShaderConstantB)(StartRegister, pConstantData, BoolCount);
}

PROXYMETHOD(DrawRectPatch)(UINT Handle,CONST float* pNumSegs,CONST D3DRECTPATCH_INFO* pRectPatchInfo)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(DrawRectPatch)(Handle, pNumSegs, pRectPatchInfo);
}

PROXYMETHOD(DrawTriPatch)(UINT Handle,CONST float* pNumSegs,CONST D3DTRIPATCH_INFO* pTriPatchInfo)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(DrawTriPatch)(Handle, pNumSegs, pTriPatchInfo);
}

PROXYMETHOD(DeletePatch)(UINT Handle)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(DeletePatch)(Handle);
}

PROXYMETHOD(CreateQuery)(D3DQUERYTYPE Type,IDirect3DQuery9** ppQuery)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(CreateQuery)(Type, ppQuery);
}

PROXYMETHOD(SetConvolutionMonoKernel)(UINT width,UINT height,float* rows,float* columns)
{
	CHECKMODIFICATION();
	return CALLORIGINALEX(SetConvolutionMonoKernel)(width, height, rows, columns);
}

PROXYMETHOD(ComposeRects)(IDirect3DSurface9* pSrc,IDirect3DSurface9* pDst,IDirect3DVertexBuffer9* pSrcRectDescs,
						  UINT NumRects,IDirect3DVertexBuffer9* pDstRectDescs,D3DCOMPOSERECTSOP Operation,int Xoffset,int Yoffset)
{
	CHECKMODIFICATION();
	return CALLORIGINALEX(ComposeRects)(pSrc, pDst, pSrcRectDescs, NumRects, pDstRectDescs, Operation, Xoffset, Yoffset);
}

PROXYMETHOD(PresentEx)(CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion,DWORD dwFlags)
{
	CHECKMODIFICATION();
	return CALLORIGINALEX(PresentEx)(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion, dwFlags);
}

PROXYMETHOD(GetGPUThreadPriority)(INT* pPriority)
{
	CHECKMODIFICATION();
	return CALLORIGINALEX(GetGPUThreadPriority)(pPriority);
}

PROXYMETHOD(SetGPUThreadPriority)(INT Priority)
{
	CHECKMODIFICATION();
	return CALLORIGINALEX(SetGPUThreadPriority)(Priority);
}

PROXYMETHOD(WaitForVBlank)(UINT iSwapChain)
{
	CHECKMODIFICATION();
	return CALLORIGINALEX(WaitForVBlank)(iSwapChain);
}

PROXYMETHOD(CheckResourceResidency)(IDirect3DResource9** pResourceArray,UINT32 NumResources)
{
	CHECKMODIFICATION();
	return CALLORIGINALEX(CheckResourceResidency)(pResourceArray, NumResources);
}

PROXYMETHOD(SetMaximumFrameLatency)(UINT MaxLatency)
{
	CHECKMODIFICATION();
	return CALLORIGINALEX(SetMaximumFrameLatency)(MaxLatency);
}

PROXYMETHOD(GetMaximumFrameLatency)(UINT* pMaxLatency)
{
	CHECKMODIFICATION();
	return CALLORIGINALEX(GetMaximumFrameLatency)(pMaxLatency);
}

PROXYMETHOD(CheckDeviceState)(HWND hDestinationWindow)
{
	CHECKMODIFICATION();
	return CALLORIGINALEX(CheckDeviceState)(hDestinationWindow);
}

PROXYMETHOD(CreateRenderTargetEx)(UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,
								  BOOL Lockable,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle,DWORD Usage)
{
	CHECKMODIFICATION();
	return CALLORIGINALEX(CreateRenderTargetEx)(Width, Height, Format, MultiSample, MultisampleQuality,
		Lockable, ppSurface, pSharedHandle, Usage);
}

PROXYMETHOD(CreateOffscreenPlainSurfaceEx)(UINT Width,UINT Height,D3DFORMAT Format,D3DPOOL Pool,
										   IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle,DWORD Usage)
{
	CHECKMODIFICATION();
	return CALLORIGINALEX(CreateOffscreenPlainSurfaceEx)(Width, Height, Format, Pool, ppSurface, pSharedHandle, Usage);
}

PROXYMETHOD(CreateDepthStencilSurfaceEx)(UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,
										 BOOL Discard,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle,DWORD Usage)
{
	CHECKMODIFICATION();
	return CALLORIGINALEX(CreateDepthStencilSurfaceEx)(Width, Height, Format, MultiSample, MultisampleQuality, Discard, ppSurface, pSharedHandle, Usage);
}

PROXYMETHOD(ResetEx)(D3DPRESENT_PARAMETERS* pPresentationParameters,D3DDISPLAYMODEEX *pFullscreenDisplayMode)
{
	CHECKMODIFICATION();
	return CALLORIGINALEX(ResetEx)(pPresentationParameters, pFullscreenDisplayMode);
}

PROXYMETHOD(GetDisplayModeEx)(UINT iSwapChain,D3DDISPLAYMODEEX* pMode,D3DDISPLAYROTATION* pRotation)
{
	CHECKMODIFICATION();
	return CALLORIGINALEX(GetDisplayModeEx)(iSwapChain, pMode, pRotation);
}

#endif
