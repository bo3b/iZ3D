/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include <atlbase.h>
#include <atlcom.h>
#include "ProxyDevice9.h"
#include "MonoSwapChain.h"
#include "GlobalData.h"
#include "Direct3D9.h"
#include "DeviceState.h"
#include "CommonUtils\System.h"

//#define WRAP_ONLY_PRIMARY_SWAPCHAIN

class CDirect3D9;

enum DumpTypes { dtNone, dtTrace, dtCommands, dtOnlyRT, dtFull };

class DECLSPEC_UUID("88D955D4-E26F-4093-8023-44D4621A92DD")
CMonoRenderer: 
	public IDirect3DDevice9Ex
{
protected:
	CriticalSection				m_D3DCS; // Critical section special for MPC-HC. Do not move it!
	long						m_refCount;

	CDirect3D9*					m_pDirect3DWrapper;

	bool					 	m_bOutOfVideoMemory;
	bool					 	m_bUseMultipleSwapChains;

	HRESULT InitializeDevice(CDirect3D9* pDirect3D9Wrapper, IDirect3D9* pDirect3D9, UINT nAdapter, D3DDEVTYPE DeviceType, 
		HWND hFocusWindow, DWORD dwBehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, 
		D3DDISPLAYMODEEX* pFullscreenDisplayMode = NULL, PROXY_EXMODE exMode = EXMODE_NONE);
	bool	Succeded(HRESULT hr);

	virtual HRESULT DoInitialize();
	virtual CMonoSwapChain* CreateSwapChainWrapper(UINT index);
	virtual void WriteStatistic() {} //stub

public:
	CComPtr<IDirect3D9>			m_pDirect3D;
	std::vector<CMonoSwapChain*>	m_SwapChains;
	UINT						m_TotalSwapChainsCount;

	HWND                        m_hFocusWindow;
	DWORD                       m_OriginalBehaviorFlags;
	UINT                        m_OriginalAdapter;
	D3DDEVTYPE                  m_DeviceType;

	bool                        m_bInStateBlock;
	bool                        m_bRenderScene;

	DumpTypes					m_DumpType;

	CMonoRenderer();
	virtual ~CMonoRenderer();	
	CMonoSwapChain* GetMonoSC(int i = 0) { return (CMonoSwapChain*)m_SwapChains[i]; }

	//--- create new instance of IDirect3DDevice9 by using IDirect3D9 ---
	HRESULT Initialize(CDirect3D9* pDirect3D9Wrapper, IDirect3D9* pDirect3D9, UINT nAdapter,
		D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, 
		D3DPRESENT_PARAMETERS* pPresentationParameters);
	//--- create new instance of IDirect3DDevice9Ex by using IDirect3D9Ex ---
	HRESULT InitializeEx(CDirect3D9* pDirect3D9Wrapper, IDirect3D9Ex* pDirect3D9, UINT nAdapter,
		D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags,
		D3DPRESENT_PARAMETERS* pPresentationParameters, D3DDISPLAYMODEEX* pFullscreenDisplayMode);
	//--- use existing IDirect3DDevice9 created by game ---
	HRESULT Initialize(CDirect3D9* pDirect3D9Wrapper, IDirect3D9* pDirect3D9, IDirect3DDevice9* pDirect3DDevice9);

	// Created IDIrect3DDevice9
	ProxyDirect9*	GetProxyDirect3D9() { return &m_pDirect3DWrapper->m_Direct3D9; }
	ProxyDevice9	m_Direct3DDevice;

	/*** IUnknown methods ***/
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, void** ppvObj);
	STDMETHOD_(ULONG,AddRef)(THIS);
	STDMETHOD_(ULONG,Release)(THIS);

	/*** IDirect3DDevice9 methods ***/
	STDMETHOD(GetDeviceCaps)(THIS_ D3DCAPS9* pCaps);
	STDMETHOD(Reset)(THIS_ D3DPRESENT_PARAMETERS* pPresentationParameters);
	STDMETHOD(Present)(THIS_ CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion);

	STDMETHOD(GetBackBuffer)(THIS_ UINT iSwapChain,UINT iBackBuffer,D3DBACKBUFFER_TYPE Type,IDirect3DSurface9** ppBackBuffer);
	STDMETHOD_(void, SetGammaRamp)(THIS_ UINT iSwapChain,DWORD Flags,CONST D3DGAMMARAMP* pRamp);
	STDMETHOD_(void, GetGammaRamp)(THIS_ UINT iSwapChain,D3DGAMMARAMP* pRamp);
	STDMETHOD(CreateTexture)(THIS_ UINT Width,UINT Height,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DTexture9** ppTexture,HANDLE* pSharedHandle);
	STDMETHOD(CreateVertexBuffer)(THIS_ UINT Length,DWORD Usage,DWORD FVF,D3DPOOL Pool,IDirect3DVertexBuffer9** ppVertexBuffer,HANDLE* pSharedHandle);
	STDMETHOD(CreateRenderTarget)(THIS_ UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Lockable,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle);
	STDMETHOD(CreateDepthStencilSurface)(THIS_ UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Discard,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle);
	STDMETHOD(StretchRect)(THIS_ IDirect3DSurface9* pSourceSurface,CONST RECT* pSourceRect,IDirect3DSurface9* pDestSurface,CONST RECT* pDestRect,D3DTEXTUREFILTERTYPE Filter);
	STDMETHOD(ColorFill)(THIS_ IDirect3DSurface9* pSurface,CONST RECT* pRect,D3DCOLOR color);
	STDMETHOD(SetRenderTarget)(THIS_ DWORD RenderTargetIndex,IDirect3DSurface9* pRenderTarget);
	STDMETHOD(SetDepthStencilSurface)(THIS_ IDirect3DSurface9* pNewZStencil);
	STDMETHOD(BeginScene)(THIS);
	STDMETHOD(EndScene)(THIS);
	STDMETHOD(BeginStateBlock)(THIS) ;
	STDMETHOD(EndStateBlock)(THIS_ IDirect3DStateBlock9** ppSB) ;
	STDMETHOD(Clear)(THIS_ DWORD Count,CONST D3DRECT* pRects,DWORD Flags,D3DCOLOR Color,float Z,DWORD Stencil);
	STDMETHOD(SetTexture)(THIS_ DWORD Stage,IDirect3DBaseTexture9* pTexture);
	STDMETHOD(DrawPrimitive)(THIS_ D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount);
	STDMETHOD(DrawIndexedPrimitive)(THIS_ D3DPRIMITIVETYPE,INT BaseVertexIndex,UINT MinVertexIndex,UINT NumVertices,UINT startIndex,UINT primCount);
	STDMETHOD(DrawPrimitiveUP)(THIS_ D3DPRIMITIVETYPE PrimitiveType,UINT PrimitiveCount,CONST void* pVertexStreamZeroData,UINT VertexStreamZeroStride);
	STDMETHOD(DrawIndexedPrimitiveUP)(THIS_ D3DPRIMITIVETYPE PrimitiveType,UINT MinVertexIndex,UINT NumVertices,UINT PrimitiveCount,CONST void* pIndexData,D3DFORMAT IndexDataFormat,CONST void* pVertexStreamZeroData,UINT VertexStreamZeroStride);
	STDMETHOD(CreateVertexShader)(THIS_ CONST DWORD* pFunction,IDirect3DVertexShader9** ppShader);
	STDMETHOD(DrawRectPatch)(THIS_ UINT Handle,CONST float* pNumSegs,CONST D3DRECTPATCH_INFO* pRectPatchInfo);
	STDMETHOD(DrawTriPatch)(THIS_ UINT Handle,CONST float* pNumSegs,CONST D3DTRIPATCH_INFO* pTriPatchInfo);
	STDMETHOD(CreatePixelShader)(THIS_ CONST DWORD* pFunction,IDirect3DPixelShader9** ppShader);

	STDMETHOD(TestCooperativeLevel)(THIS) ;
	STDMETHOD_(UINT, GetAvailableTextureMem)(THIS) ;
	STDMETHOD(EvictManagedResources)(THIS) ;
	STDMETHOD(GetDirect3D)(THIS_ IDirect3D9** ppD3D9) ;
	STDMETHOD(GetDisplayMode)(THIS_ UINT iSwapChain,D3DDISPLAYMODE* pMode) ;
	STDMETHOD(GetCreationParameters)(THIS_ D3DDEVICE_CREATION_PARAMETERS *pParameters) ;
	STDMETHOD(SetCursorProperties)(THIS_ UINT XHotSpot,UINT YHotSpot,IDirect3DSurface9* pCursorBitmap) ;
	STDMETHOD_(void, SetCursorPosition)(THIS_ int X,int Y,DWORD Flags) ;
	STDMETHOD_(BOOL, ShowCursor)(THIS_ BOOL bShow) ;
	STDMETHOD(CreateAdditionalSwapChain)(THIS_ D3DPRESENT_PARAMETERS* pPresentationParameters,IDirect3DSwapChain9** pSwapChain) ;
	STDMETHOD(GetSwapChain)(THIS_ UINT iSwapChain,IDirect3DSwapChain9** pSwapChain) ;
	STDMETHOD_(UINT, GetNumberOfSwapChains)(THIS) ;
	STDMETHOD(GetRasterStatus)(THIS_ UINT iSwapChain,D3DRASTER_STATUS* pRasterStatus) ;
	STDMETHOD(SetDialogBoxMode)(THIS_ BOOL bEnableDialogs) ;
	STDMETHOD(CreateVolumeTexture)(THIS_ UINT Width,UINT Height,UINT Depth,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DVolumeTexture9** ppVolumeTexture,HANDLE* pSharedHandle) ;
	STDMETHOD(CreateCubeTexture)(THIS_ UINT EdgeLength,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DCubeTexture9** ppCubeTexture,HANDLE* pSharedHandle);
	STDMETHOD(CreateIndexBuffer)(THIS_ UINT Length,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DIndexBuffer9** ppIndexBuffer,HANDLE* pSharedHandle) ;
	STDMETHOD(UpdateSurface)(THIS_ IDirect3DSurface9* pSourceSurface,CONST RECT* pSourceRect,IDirect3DSurface9* pDestinationSurface,CONST POINT* pDestPoint) ;
	STDMETHOD(UpdateTexture)(THIS_ IDirect3DBaseTexture9* pSourceTexture,IDirect3DBaseTexture9* pDestinationTexture) ;
	STDMETHOD(GetRenderTargetData)(THIS_ IDirect3DSurface9* pRenderTarget,IDirect3DSurface9* pDestSurface) ;
	STDMETHOD(GetFrontBufferData)(THIS_ UINT iSwapChain,IDirect3DSurface9* pDestSurface) ;
	STDMETHOD(CreateOffscreenPlainSurface)(THIS_ UINT Width,UINT Height,D3DFORMAT Format,D3DPOOL Pool,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle) ;
	STDMETHOD(GetRenderTarget)(THIS_ DWORD RenderTargetIndex,IDirect3DSurface9** ppRenderTarget);
	STDMETHOD(GetDepthStencilSurface)(THIS_ IDirect3DSurface9** ppZStencilSurface);
	STDMETHOD(SetTransform)(THIS_ D3DTRANSFORMSTATETYPE State,CONST D3DMATRIX* pMatrix);
	STDMETHOD(GetTransform)(THIS_ D3DTRANSFORMSTATETYPE State,D3DMATRIX* pMatrix) ;
	STDMETHOD(MultiplyTransform)(THIS_ D3DTRANSFORMSTATETYPE,CONST D3DMATRIX*);
	STDMETHOD(SetViewport)(THIS_ CONST D3DVIEWPORT9* pViewport);
	STDMETHOD(GetViewport)(THIS_ D3DVIEWPORT9* pViewport) ;
	STDMETHOD(SetMaterial)(THIS_ CONST D3DMATERIAL9* pMaterial) ;
	STDMETHOD(GetMaterial)(THIS_ D3DMATERIAL9* pMaterial) ;
	STDMETHOD(SetLight)(THIS_ DWORD Index,CONST D3DLIGHT9*) ;
	STDMETHOD(GetLight)(THIS_ DWORD Index,D3DLIGHT9*) ;
	STDMETHOD(LightEnable)(THIS_ DWORD Index,BOOL Enable) ;
	STDMETHOD(GetLightEnable)(THIS_ DWORD Index,BOOL* pEnable) ;
	STDMETHOD(SetClipPlane)(THIS_ DWORD Index,CONST float* pPlane) ;
	STDMETHOD(GetClipPlane)(THIS_ DWORD Index,float* pPlane) ;
	STDMETHOD(SetRenderState)(THIS_ D3DRENDERSTATETYPE State,DWORD Value);
	STDMETHOD(GetRenderState)(THIS_ D3DRENDERSTATETYPE State,DWORD* pValue) ;
	STDMETHOD(CreateStateBlock)(THIS_ D3DSTATEBLOCKTYPE Type,IDirect3DStateBlock9** ppSB) ;
	STDMETHOD(SetClipStatus)(THIS_ CONST D3DCLIPSTATUS9* pClipStatus) ;
	STDMETHOD(GetClipStatus)(THIS_ D3DCLIPSTATUS9* pClipStatus) ;
	STDMETHOD(GetTexture)(THIS_ DWORD Stage,IDirect3DBaseTexture9** ppTexture) ;
	STDMETHOD(GetTextureStageState)(THIS_ DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD* pValue) ;
	STDMETHOD(SetTextureStageState)(THIS_ DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD Value);
	STDMETHOD(GetSamplerState)(THIS_ DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD* pValue) ;
	STDMETHOD(SetSamplerState)(THIS_ DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD Value) ;
	STDMETHOD(ValidateDevice)(THIS_ DWORD* pNumPasses) ;
	STDMETHOD(SetPaletteEntries)(THIS_ UINT PaletteNumber,CONST PALETTEENTRY* pEntries) ;
	STDMETHOD(GetPaletteEntries)(THIS_ UINT PaletteNumber,PALETTEENTRY* pEntries) ;
	STDMETHOD(SetCurrentTexturePalette)(THIS_ UINT PaletteNumber) ;
	STDMETHOD(GetCurrentTexturePalette)(THIS_ UINT *PaletteNumber) ;
	STDMETHOD(SetScissorRect)(THIS_ CONST RECT* pRect);
	STDMETHOD(GetScissorRect)(THIS_ RECT* pRect);
	STDMETHOD(SetSoftwareVertexProcessing)(THIS_ BOOL bSoftware) ;
	STDMETHOD_(BOOL, GetSoftwareVertexProcessing)(THIS) ;
	STDMETHOD(SetNPatchMode)(THIS_ float nSegments) ;
	STDMETHOD_(float, GetNPatchMode)(THIS) ;
	STDMETHOD(ProcessVertices)(THIS_ UINT SrcStartIndex,UINT DestIndex,UINT VertexCount,IDirect3DVertexBuffer9* pDestBuffer,IDirect3DVertexDeclaration9* pVertexDecl,DWORD Flags) ;
	STDMETHOD(CreateVertexDeclaration)(THIS_ CONST D3DVERTEXELEMENT9* pVertexElements,IDirect3DVertexDeclaration9** ppDecl) ;
	STDMETHOD(SetVertexDeclaration)(THIS_ IDirect3DVertexDeclaration9* pDecl);
	STDMETHOD(GetVertexDeclaration)(THIS_ IDirect3DVertexDeclaration9** ppDecl) ;
	STDMETHOD(SetFVF)(THIS_ DWORD FVF);
	STDMETHOD(GetFVF)(THIS_ DWORD* pFVF) ;
	STDMETHOD(SetVertexShader)(THIS_ IDirect3DVertexShader9* pShader);
	STDMETHOD(GetVertexShader)(THIS_ IDirect3DVertexShader9** ppShader);
	STDMETHOD(SetVertexShaderConstantF)(THIS_ UINT StartRegister,CONST float* pConstantData,UINT Vector4fCount) ;
	STDMETHOD(GetVertexShaderConstantF)(THIS_ UINT StartRegister,float* pConstantData,UINT Vector4fCount) ;
	STDMETHOD(SetVertexShaderConstantI)(THIS_ UINT StartRegister,CONST int* pConstantData,UINT Vector4iCount) ;
	STDMETHOD(GetVertexShaderConstantI)(THIS_ UINT StartRegister,int* pConstantData,UINT Vector4iCount) ;
	STDMETHOD(SetVertexShaderConstantB)(THIS_ UINT StartRegister,CONST BOOL* pConstantData,UINT  BoolCount) ;
	STDMETHOD(GetVertexShaderConstantB)(THIS_ UINT StartRegister,BOOL* pConstantData,UINT BoolCount) ;
	STDMETHOD(SetStreamSource)(THIS_ UINT StreamNumber,IDirect3DVertexBuffer9* pStreamData,UINT OffsetInBytes,UINT Stride);
	STDMETHOD(GetStreamSource)(THIS_ UINT StreamNumber,IDirect3DVertexBuffer9** ppStreamData,UINT* pOffsetInBytes,UINT* pStride) ;
	STDMETHOD(SetStreamSourceFreq)(THIS_ UINT StreamNumber,UINT Setting) ;
	STDMETHOD(GetStreamSourceFreq)(THIS_ UINT StreamNumber,UINT* pSetting) ;
	STDMETHOD(SetIndices)(THIS_ IDirect3DIndexBuffer9* pIndexData) ;
	STDMETHOD(GetIndices)(THIS_ IDirect3DIndexBuffer9** ppIndexData) ;
	STDMETHOD(SetPixelShader)(THIS_ IDirect3DPixelShader9* pShader);
	STDMETHOD(GetPixelShader)(THIS_ IDirect3DPixelShader9** ppShader) ;
	STDMETHOD(SetPixelShaderConstantF)(THIS_ UINT StartRegister,CONST float* pConstantData,UINT Vector4fCount) ;
	STDMETHOD(GetPixelShaderConstantF)(THIS_ UINT StartRegister,float* pConstantData,UINT Vector4fCount) ;
	STDMETHOD(SetPixelShaderConstantI)(THIS_ UINT StartRegister,CONST int* pConstantData,UINT Vector4iCount) ;
	STDMETHOD(GetPixelShaderConstantI)(THIS_ UINT StartRegister,int* pConstantData,UINT Vector4iCount) ;
	STDMETHOD(SetPixelShaderConstantB)(THIS_ UINT StartRegister,CONST BOOL* pConstantData,UINT  BoolCount) ;
	STDMETHOD(GetPixelShaderConstantB)(THIS_ UINT StartRegister,BOOL* pConstantData,UINT BoolCount) ;
	STDMETHOD(DeletePatch)(THIS_ UINT Handle) ;
	STDMETHOD(CreateQuery)(THIS_ D3DQUERYTYPE Type,IDirect3DQuery9** ppQuery) ;

	/*** IDirect3DDevice9Ex methods ***/
	STDMETHOD(SetConvolutionMonoKernel)(THIS_ UINT width,UINT height,float* rows,float* columns);
	STDMETHOD(ComposeRects)(THIS_ IDirect3DSurface9* pSrc,IDirect3DSurface9* pDst,IDirect3DVertexBuffer9* pSrcRectDescs,UINT NumRects,IDirect3DVertexBuffer9* pDstRectDescs,D3DCOMPOSERECTSOP Operation,int Xoffset,int Yoffset);
	STDMETHOD(PresentEx)(THIS_ CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion,DWORD dwFlags);
	STDMETHOD(GetGPUThreadPriority)(THIS_ INT* pPriority);
	STDMETHOD(SetGPUThreadPriority)(THIS_ INT Priority);
	STDMETHOD(WaitForVBlank)(THIS_ UINT iSwapChain);
	STDMETHOD(CheckResourceResidency)(THIS_ IDirect3DResource9** pResourceArray,UINT32 NumResources);
	STDMETHOD(SetMaximumFrameLatency)(THIS_ UINT MaxLatency);
	STDMETHOD(GetMaximumFrameLatency)(THIS_ UINT* pMaxLatency);
	STDMETHOD(CheckDeviceState)(THIS_ HWND hDestinationWindow);
	STDMETHOD(CreateOffscreenPlainSurfaceEx)(THIS_ UINT Width,UINT Height,D3DFORMAT Format,D3DPOOL Pool,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle,DWORD Usage);
	STDMETHOD(ResetEx)(THIS_ D3DPRESENT_PARAMETERS* pPresentationParameters,D3DDISPLAYMODEEX *pFullscreenDisplayMode);
	STDMETHOD(CreateRenderTargetEx)(UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Lockable,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle,DWORD Usage);
	STDMETHOD(CreateDepthStencilSurfaceEx)(UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Discard,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle,DWORD Usage);

	STDMETHOD(GetDisplayModeEx)(THIS_ UINT iSwapChain,D3DDISPLAYMODEEX* pMode,D3DDISPLAYROTATION* pRotation);

#ifndef FINAL_RELEASE
protected:
	void OnNewTextureCreated(IDirect3DTexture9 *pTexture,UINT Levels);
	void OnNewTextureCreated(IDirect3DCubeTexture9 *pTexture,UINT Levels);
	void OnNewTextureCreated(IDirect3DVolumeTexture9 *pTexture,UINT Levels);
	void CaptureStateBlock(D3DSTATEBLOCKTYPE Type, CDeviceState& sb);

	CDeviceState m_deviceState;
	CDeviceState m_tempDeviceState;

	std::map<IDirect3DStateBlock9 *,CDeviceState> m_StateBlocks;
#else // FINAL_RELEASE
	void OnNewTextureCreated(...) { }
#endif // FINAL_RELEASE
};

#include "ProxyDevice9-inl.h"
