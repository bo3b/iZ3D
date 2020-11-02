/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "D3DInterfaceMethods.h"
#include "ProxyBase.h"

namespace IDirect3DDevice9MethodNames
{
	#define PROXYDEVICEMETHODTYPE(a)		typedef HRESULT (__stdcall IDirect3DDevice9::*a##_t)
	#define PROXYDEVICEMETHODTYPE_(type, a)	typedef type    (__stdcall IDirect3DDevice9::*a##_t)

	// IUnknown methods
	PROXYDEVICEMETHODTYPE(QueryInterface)(REFIID riid, void** ppvObj);
	PROXYDEVICEMETHODTYPE_(ULONG,AddRef)();
	PROXYDEVICEMETHODTYPE_(ULONG,Release)();

	// IDirect3DDevice9 methods
	PROXYDEVICEMETHODTYPE(TestCooperativeLevel)();
	PROXYDEVICEMETHODTYPE_(UINT, GetAvailableTextureMem)();
	PROXYDEVICEMETHODTYPE(EvictManagedResources)();
	PROXYDEVICEMETHODTYPE(GetDirect3D)(IDirect3D9** ppD3D9);
	PROXYDEVICEMETHODTYPE(GetDeviceCaps)(D3DCAPS9* pCaps);
	PROXYDEVICEMETHODTYPE(GetDisplayMode)(UINT iSwapChain,D3DDISPLAYMODE* pMode);
	PROXYDEVICEMETHODTYPE(GetCreationParameters)(D3DDEVICE_CREATION_PARAMETERS *pParameters);
	PROXYDEVICEMETHODTYPE(SetCursorProperties)(UINT XHotSpot,UINT YHotSpot,IDirect3DSurface9* pCursorBitmap);
	PROXYDEVICEMETHODTYPE_(void, SetCursorPosition)(int X,int Y,DWORD Flags);
	PROXYDEVICEMETHODTYPE_(BOOL, ShowCursor)(BOOL bShow);
	PROXYDEVICEMETHODTYPE(CreateAdditionalSwapChain)(D3DPRESENT_PARAMETERS* pPresentationParameters,IDirect3DSwapChain9** pSwapChain);
	PROXYDEVICEMETHODTYPE(GetSwapChain)(UINT iSwapChain,IDirect3DSwapChain9** pSwapChain);
	PROXYDEVICEMETHODTYPE_(UINT, GetNumberOfSwapChains)();
	PROXYDEVICEMETHODTYPE(Reset)(D3DPRESENT_PARAMETERS* pPresentationParameters);
	PROXYDEVICEMETHODTYPE(Present)(CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion);
	PROXYDEVICEMETHODTYPE(GetBackBuffer)(UINT iSwapChain,UINT iBackBuffer,D3DBACKBUFFER_TYPE Type,IDirect3DSurface9** ppBackBuffer);
	PROXYDEVICEMETHODTYPE(GetRasterStatus)(UINT iSwapChain,D3DRASTER_STATUS* pRasterStatus);
	PROXYDEVICEMETHODTYPE(SetDialogBoxMode)(BOOL bEnableDialogs);
	PROXYDEVICEMETHODTYPE_(void, SetGammaRamp)(UINT iSwapChain,DWORD Flags,CONST D3DGAMMARAMP* pRamp);
	PROXYDEVICEMETHODTYPE_(void, GetGammaRamp)(UINT iSwapChain,D3DGAMMARAMP* pRamp);
	PROXYDEVICEMETHODTYPE(CreateTexture)(UINT Width,UINT Height,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DTexture9** ppTexture,HANDLE* pSharedHandle);
	PROXYDEVICEMETHODTYPE(CreateVolumeTexture)(UINT Width,UINT Height,UINT Depth,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DVolumeTexture9** ppVolumeTexture,HANDLE* pSharedHandle);
	PROXYDEVICEMETHODTYPE(CreateCubeTexture)(UINT EdgeLength,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DCubeTexture9** ppCubeTexture,HANDLE* pSharedHandle);
	PROXYDEVICEMETHODTYPE(CreateVertexBuffer)(UINT Length,DWORD Usage,DWORD FVF,D3DPOOL Pool,IDirect3DVertexBuffer9** ppVertexBuffer,HANDLE* pSharedHandle);
	PROXYDEVICEMETHODTYPE(CreateIndexBuffer)(UINT Length,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DIndexBuffer9** ppIndexBuffer,HANDLE* pSharedHandle);
	PROXYDEVICEMETHODTYPE(CreateRenderTarget)(UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Lockable,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle);
	PROXYDEVICEMETHODTYPE(CreateDepthStencilSurface)(UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Discard,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle);
	PROXYDEVICEMETHODTYPE(UpdateSurface)(IDirect3DSurface9* pSourceSurface,CONST RECT* pSourceRect,IDirect3DSurface9* pDestinationSurface,CONST POINT* pDestPoint);
	PROXYDEVICEMETHODTYPE(UpdateTexture)(IDirect3DBaseTexture9* pSourceTexture,IDirect3DBaseTexture9* pDestinationTexture);
	PROXYDEVICEMETHODTYPE(GetRenderTargetData)(IDirect3DSurface9* pRenderTarget,IDirect3DSurface9* pDestSurface);
	PROXYDEVICEMETHODTYPE(GetFrontBufferData)(UINT iSwapChain,IDirect3DSurface9* pDestSurface);
	PROXYDEVICEMETHODTYPE(StretchRect)(IDirect3DSurface9* pSourceSurface,CONST RECT* pSourceRect,IDirect3DSurface9* pDestSurface,CONST RECT* pDestRect,D3DTEXTUREFILTERTYPE Filter);
	PROXYDEVICEMETHODTYPE(ColorFill)(IDirect3DSurface9* pSurface,CONST RECT* pRect,D3DCOLOR color);
	PROXYDEVICEMETHODTYPE(CreateOffscreenPlainSurface)(UINT Width,UINT Height,D3DFORMAT Format,D3DPOOL Pool,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle);
	PROXYDEVICEMETHODTYPE(SetRenderTarget)(DWORD RenderTargetIndex,IDirect3DSurface9* pRenderTarget);
	PROXYDEVICEMETHODTYPE(GetRenderTarget)(DWORD RenderTargetIndex,IDirect3DSurface9** ppRenderTarget);
	PROXYDEVICEMETHODTYPE(SetDepthStencilSurface)(IDirect3DSurface9* pNewZStencil);
	PROXYDEVICEMETHODTYPE(GetDepthStencilSurface)(IDirect3DSurface9** ppZStencilSurface);
	PROXYDEVICEMETHODTYPE(BeginScene)();
	PROXYDEVICEMETHODTYPE(EndScene)();
	PROXYDEVICEMETHODTYPE(Clear)(DWORD Count,CONST D3DRECT* pRects,DWORD Flags,D3DCOLOR Color,float Z,DWORD Stencil);
	PROXYDEVICEMETHODTYPE(SetTransform)(D3DTRANSFORMSTATETYPE State,CONST D3DMATRIX* pMatrix);
	PROXYDEVICEMETHODTYPE(GetTransform)(D3DTRANSFORMSTATETYPE State,D3DMATRIX* pMatrix);
	PROXYDEVICEMETHODTYPE(MultiplyTransform)(D3DTRANSFORMSTATETYPE,CONST D3DMATRIX*);
	PROXYDEVICEMETHODTYPE(SetViewport)(CONST D3DVIEWPORT9* pViewport);
	PROXYDEVICEMETHODTYPE(GetViewport)(D3DVIEWPORT9* pViewport);
	PROXYDEVICEMETHODTYPE(SetMaterial)(CONST D3DMATERIAL9* pMaterial);
	PROXYDEVICEMETHODTYPE(GetMaterial)(D3DMATERIAL9* pMaterial);
	PROXYDEVICEMETHODTYPE(SetLight)(DWORD Index,CONST D3DLIGHT9*);
	PROXYDEVICEMETHODTYPE(GetLight)(DWORD Index,D3DLIGHT9*);
	PROXYDEVICEMETHODTYPE(LightEnable)(DWORD Index,BOOL Enable);
	PROXYDEVICEMETHODTYPE(GetLightEnable)(DWORD Index,BOOL* pEnable);
	PROXYDEVICEMETHODTYPE(SetClipPlane)(DWORD Index,CONST float* pPlane);
	PROXYDEVICEMETHODTYPE(GetClipPlane)(DWORD Index,float* pPlane);
	PROXYDEVICEMETHODTYPE(SetRenderState)(D3DRENDERSTATETYPE State,DWORD Value);
	PROXYDEVICEMETHODTYPE(GetRenderState)(D3DRENDERSTATETYPE State,DWORD* pValue);
	PROXYDEVICEMETHODTYPE(CreateStateBlock)(D3DSTATEBLOCKTYPE Type,IDirect3DStateBlock9** ppSB);
	PROXYDEVICEMETHODTYPE(BeginStateBlock)();
	PROXYDEVICEMETHODTYPE(EndStateBlock)(IDirect3DStateBlock9** ppSB);
	PROXYDEVICEMETHODTYPE(SetClipStatus)(CONST D3DCLIPSTATUS9* pClipStatus);
	PROXYDEVICEMETHODTYPE(GetClipStatus)(D3DCLIPSTATUS9* pClipStatus);
	PROXYDEVICEMETHODTYPE(GetTexture)(DWORD Stage,IDirect3DBaseTexture9** ppTexture);
	PROXYDEVICEMETHODTYPE(SetTexture)(DWORD Stage,IDirect3DBaseTexture9* pTexture);
	PROXYDEVICEMETHODTYPE(GetTextureStageState)(DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD* pValue);
	PROXYDEVICEMETHODTYPE(SetTextureStageState)(DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD Value);
	PROXYDEVICEMETHODTYPE(GetSamplerState)(DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD* pValue);
	PROXYDEVICEMETHODTYPE(SetSamplerState)(DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD Value);
	PROXYDEVICEMETHODTYPE(ValidateDevice)(DWORD* pNumPasses);
	PROXYDEVICEMETHODTYPE(SetPaletteEntries)(UINT PaletteNumber,CONST PALETTEENTRY* pEntries);
	PROXYDEVICEMETHODTYPE(GetPaletteEntries)(UINT PaletteNumber,PALETTEENTRY* pEntries);
	PROXYDEVICEMETHODTYPE(SetCurrentTexturePalette)(UINT PaletteNumber);
	PROXYDEVICEMETHODTYPE(GetCurrentTexturePalette)(UINT *PaletteNumber);
	PROXYDEVICEMETHODTYPE(SetScissorRect)(CONST RECT* pRect);
	PROXYDEVICEMETHODTYPE(GetScissorRect)(RECT* pRect);
	PROXYDEVICEMETHODTYPE(SetSoftwareVertexProcessing)(BOOL bSoftware);
	PROXYDEVICEMETHODTYPE_(BOOL, GetSoftwareVertexProcessing)();
	PROXYDEVICEMETHODTYPE(SetNPatchMode)(float nSegments);
	PROXYDEVICEMETHODTYPE_(float, GetNPatchMode)();
	PROXYDEVICEMETHODTYPE(DrawPrimitive)(D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount);
	PROXYDEVICEMETHODTYPE(DrawIndexedPrimitive)(D3DPRIMITIVETYPE,INT BaseVertexIndex,UINT MinVertexIndex,UINT NumVertices,UINT startIndex,UINT primCount);
	PROXYDEVICEMETHODTYPE(DrawPrimitiveUP)(D3DPRIMITIVETYPE PrimitiveType,UINT PrimitiveCount,CONST void* pVertexStreamZeroData,UINT VertexStreamZeroStride);
	PROXYDEVICEMETHODTYPE(DrawIndexedPrimitiveUP)(D3DPRIMITIVETYPE PrimitiveType,UINT MinVertexIndex,UINT NumVertices,UINT PrimitiveCount,CONST void* pIndexData,D3DFORMAT IndexDataFormat,CONST void* pVertexStreamZeroData,UINT VertexStreamZeroStride);
	PROXYDEVICEMETHODTYPE(ProcessVertices)(UINT SrcStartIndex,UINT DestIndex,UINT VertexCount,IDirect3DVertexBuffer9* pDestBuffer,IDirect3DVertexDeclaration9* pVertexDecl,DWORD Flags);
	PROXYDEVICEMETHODTYPE(CreateVertexDeclaration)(CONST D3DVERTEXELEMENT9* pVertexElements,IDirect3DVertexDeclaration9** ppDecl);
	PROXYDEVICEMETHODTYPE(SetVertexDeclaration)(IDirect3DVertexDeclaration9* pDecl);
	PROXYDEVICEMETHODTYPE(GetVertexDeclaration)(IDirect3DVertexDeclaration9** ppDecl);
	PROXYDEVICEMETHODTYPE(SetFVF)(DWORD FVF);
	PROXYDEVICEMETHODTYPE(GetFVF)(DWORD* pFVF);
	PROXYDEVICEMETHODTYPE(CreateVertexShader)(CONST DWORD* pFunction,IDirect3DVertexShader9** ppShader);
	PROXYDEVICEMETHODTYPE(SetVertexShader)(IDirect3DVertexShader9* pShader);
	PROXYDEVICEMETHODTYPE(GetVertexShader)(IDirect3DVertexShader9** ppShader);
	PROXYDEVICEMETHODTYPE(SetVertexShaderConstantF)(UINT StartRegister,CONST float* pConstantData,UINT Vector4fCount);
	PROXYDEVICEMETHODTYPE(GetVertexShaderConstantF)(UINT StartRegister,float* pConstantData,UINT Vector4fCount);
	PROXYDEVICEMETHODTYPE(SetVertexShaderConstantI)(UINT StartRegister,CONST int* pConstantData,UINT Vector4iCount);
	PROXYDEVICEMETHODTYPE(GetVertexShaderConstantI)(UINT StartRegister,int* pConstantData,UINT Vector4iCount);
	PROXYDEVICEMETHODTYPE(SetVertexShaderConstantB)(UINT StartRegister,CONST BOOL* pConstantData,UINT  BoolCount);
	PROXYDEVICEMETHODTYPE(GetVertexShaderConstantB)(UINT StartRegister,BOOL* pConstantData,UINT BoolCount);
	PROXYDEVICEMETHODTYPE(SetStreamSource)(UINT StreamNumber,IDirect3DVertexBuffer9* pStreamData,UINT OffsetInBytes,UINT Stride);
	PROXYDEVICEMETHODTYPE(GetStreamSource)(UINT StreamNumber,IDirect3DVertexBuffer9** ppStreamData,UINT* pOffsetInBytes,UINT* pStride);
	PROXYDEVICEMETHODTYPE(SetStreamSourceFreq)(UINT StreamNumber,UINT Setting);
	PROXYDEVICEMETHODTYPE(GetStreamSourceFreq)(UINT StreamNumber,UINT* pSetting);
	PROXYDEVICEMETHODTYPE(SetIndices)(IDirect3DIndexBuffer9* pIndexData);
	PROXYDEVICEMETHODTYPE(GetIndices)(IDirect3DIndexBuffer9** ppIndexData);
	PROXYDEVICEMETHODTYPE(CreatePixelShader)(CONST DWORD* pFunction,IDirect3DPixelShader9** ppShader);
	PROXYDEVICEMETHODTYPE(SetPixelShader)(IDirect3DPixelShader9* pShader);
	PROXYDEVICEMETHODTYPE(GetPixelShader)(IDirect3DPixelShader9** ppShader);
	PROXYDEVICEMETHODTYPE(SetPixelShaderConstantF)(UINT StartRegister,CONST float* pConstantData,UINT Vector4fCount);
	PROXYDEVICEMETHODTYPE(GetPixelShaderConstantF)(UINT StartRegister,float* pConstantData,UINT Vector4fCount);
	PROXYDEVICEMETHODTYPE(SetPixelShaderConstantI)(UINT StartRegister,CONST int* pConstantData,UINT Vector4iCount);
	PROXYDEVICEMETHODTYPE(GetPixelShaderConstantI)(UINT StartRegister,int* pConstantData,UINT Vector4iCount);
	PROXYDEVICEMETHODTYPE(SetPixelShaderConstantB)(UINT StartRegister,CONST BOOL* pConstantData,UINT  BoolCount);
	PROXYDEVICEMETHODTYPE(GetPixelShaderConstantB)(UINT StartRegister,BOOL* pConstantData,UINT BoolCount);
	PROXYDEVICEMETHODTYPE(DrawRectPatch)(UINT Handle,CONST float* pNumSegs,CONST D3DRECTPATCH_INFO* pRectPatchInfo);
	PROXYDEVICEMETHODTYPE(DrawTriPatch)(UINT Handle,CONST float* pNumSegs,CONST D3DTRIPATCH_INFO* pTriPatchInfo);
	PROXYDEVICEMETHODTYPE(DeletePatch)(UINT Handle);
	PROXYDEVICEMETHODTYPE(CreateQuery)(D3DQUERYTYPE Type,IDirect3DQuery9** ppQuery);

	#define PROXYDEVICEEXMETHODTYPE(a)			typedef HRESULT (__stdcall IDirect3DDevice9Ex::*a##_t)
	#define PROXYDEVICEEXMETHODTYPE_(type, a)	typedef type    (__stdcall IDirect3DDevice9Ex::*a##_t)

	// IDirect3DDevice9 methods
	PROXYDEVICEEXMETHODTYPE(SetConvolutionMonoKernel)(UINT width,UINT height,float* rows,float* columns);
	PROXYDEVICEEXMETHODTYPE(ComposeRects)(IDirect3DSurface9* pSrc,IDirect3DSurface9* pDst,IDirect3DVertexBuffer9* pSrcRectDescs,UINT NumRects,IDirect3DVertexBuffer9* pDstRectDescs,D3DCOMPOSERECTSOP Operation,int Xoffset,int Yoffset);
	PROXYDEVICEEXMETHODTYPE(PresentEx)(CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion,DWORD dwFlags);
	PROXYDEVICEEXMETHODTYPE(GetGPUThreadPriority)(INT* pPriority);
	PROXYDEVICEEXMETHODTYPE(SetGPUThreadPriority)(INT Priority);
	PROXYDEVICEEXMETHODTYPE(WaitForVBlank)(UINT iSwapChain);
	PROXYDEVICEEXMETHODTYPE(CheckResourceResidency)(IDirect3DResource9** pResourceArray,UINT32 NumResources);
	PROXYDEVICEEXMETHODTYPE(SetMaximumFrameLatency)(UINT MaxLatency);
	PROXYDEVICEEXMETHODTYPE(GetMaximumFrameLatency)(UINT* pMaxLatency);
	PROXYDEVICEEXMETHODTYPE(CheckDeviceState)(HWND hDestinationWindow);
	PROXYDEVICEEXMETHODTYPE(CreateRenderTargetEx)(UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Lockable,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle,DWORD Usage);
	PROXYDEVICEEXMETHODTYPE(CreateOffscreenPlainSurfaceEx)(UINT Width,UINT Height,D3DFORMAT Format,D3DPOOL Pool,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle,DWORD Usage);
	PROXYDEVICEEXMETHODTYPE(CreateDepthStencilSurfaceEx)(UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Discard,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle,DWORD Usage);
	PROXYDEVICEEXMETHODTYPE(ResetEx)(D3DPRESENT_PARAMETERS* pPresentationParameters,D3DDISPLAYMODEEX *pFullscreenDisplayMode);
	PROXYDEVICEEXMETHODTYPE(GetDisplayModeEx)(UINT iSwapChain,D3DDISPLAYMODEEX* pMode,D3DDISPLAYROTATION* pRotation);

	#define PROXYDEVICEMETHOD(a)			a##_t a
	#define PROXYDEVICEMETHOD_(type, a)		a##_t a

	union FunctionList
	{
		static const int MAX_METHODS    = _MaxMethods_;
		static const int MAX_METHODS_EX = _MaxMethodsEx_;

		FunctionList()
		{
			ZeroMemory(func, sizeof(func));
		}
		struct
		{
			// IUnknown methods
			PROXYDEVICEMETHOD(QueryInterface);
			PROXYDEVICEMETHOD_(ULONG,AddRef);
			PROXYDEVICEMETHOD_(ULONG,Release);

			// IDirect3DDevice9 methods
			PROXYDEVICEMETHOD(TestCooperativeLevel);
			PROXYDEVICEMETHOD_(UINT, GetAvailableTextureMem);
			PROXYDEVICEMETHOD(EvictManagedResources);
			PROXYDEVICEMETHOD(GetDirect3D);
			PROXYDEVICEMETHOD(GetDeviceCaps);
			PROXYDEVICEMETHOD(GetDisplayMode);
			PROXYDEVICEMETHOD(GetCreationParameters);
			PROXYDEVICEMETHOD(SetCursorProperties);
			PROXYDEVICEMETHOD_(void, SetCursorPosition);
			PROXYDEVICEMETHOD_(BOOL, ShowCursor);
			PROXYDEVICEMETHOD(CreateAdditionalSwapChain);
			PROXYDEVICEMETHOD(GetSwapChain);
			PROXYDEVICEMETHOD_(UINT, GetNumberOfSwapChains);
			PROXYDEVICEMETHOD(Reset);
			PROXYDEVICEMETHOD(Present);
			PROXYDEVICEMETHOD(GetBackBuffer);
			PROXYDEVICEMETHOD(GetRasterStatus);
			PROXYDEVICEMETHOD(SetDialogBoxMode);
			PROXYDEVICEMETHOD_(void, SetGammaRamp);
			PROXYDEVICEMETHOD_(void, GetGammaRamp);
			PROXYDEVICEMETHOD(CreateTexture);
			PROXYDEVICEMETHOD(CreateVolumeTexture);
			PROXYDEVICEMETHOD(CreateCubeTexture);
			PROXYDEVICEMETHOD(CreateVertexBuffer);
			PROXYDEVICEMETHOD(CreateIndexBuffer);
			PROXYDEVICEMETHOD(CreateRenderTarget);
			PROXYDEVICEMETHOD(CreateDepthStencilSurface);
			PROXYDEVICEMETHOD(UpdateSurface);
			PROXYDEVICEMETHOD(UpdateTexture);
			PROXYDEVICEMETHOD(GetRenderTargetData);
			PROXYDEVICEMETHOD(GetFrontBufferData);
			PROXYDEVICEMETHOD(StretchRect);
			PROXYDEVICEMETHOD(ColorFill);
			PROXYDEVICEMETHOD(CreateOffscreenPlainSurface);
			PROXYDEVICEMETHOD(SetRenderTarget);
			PROXYDEVICEMETHOD(GetRenderTarget);
			PROXYDEVICEMETHOD(SetDepthStencilSurface);
			PROXYDEVICEMETHOD(GetDepthStencilSurface);
			PROXYDEVICEMETHOD(BeginScene);
			PROXYDEVICEMETHOD(EndScene);
			PROXYDEVICEMETHOD(Clear);
			PROXYDEVICEMETHOD(SetTransform);
			PROXYDEVICEMETHOD(GetTransform);
			PROXYDEVICEMETHOD(MultiplyTransform);
			PROXYDEVICEMETHOD(SetViewport);
			PROXYDEVICEMETHOD(GetViewport);
			PROXYDEVICEMETHOD(SetMaterial);
			PROXYDEVICEMETHOD(GetMaterial);
			PROXYDEVICEMETHOD(SetLight);
			PROXYDEVICEMETHOD(GetLight);
			PROXYDEVICEMETHOD(LightEnable);
			PROXYDEVICEMETHOD(GetLightEnable);
			PROXYDEVICEMETHOD(SetClipPlane);
			PROXYDEVICEMETHOD(GetClipPlane);
			PROXYDEVICEMETHOD(SetRenderState);
			PROXYDEVICEMETHOD(GetRenderState);
			PROXYDEVICEMETHOD(CreateStateBlock);
			PROXYDEVICEMETHOD(BeginStateBlock);
			PROXYDEVICEMETHOD(EndStateBlock);
			PROXYDEVICEMETHOD(SetClipStatus);
			PROXYDEVICEMETHOD(GetClipStatus);
			PROXYDEVICEMETHOD(GetTexture);
			PROXYDEVICEMETHOD(SetTexture);
			PROXYDEVICEMETHOD(GetTextureStageState);
			PROXYDEVICEMETHOD(SetTextureStageState);
			PROXYDEVICEMETHOD(GetSamplerState);
			PROXYDEVICEMETHOD(SetSamplerState);
			PROXYDEVICEMETHOD(ValidateDevice);
			PROXYDEVICEMETHOD(SetPaletteEntries);
			PROXYDEVICEMETHOD(GetPaletteEntries);
			PROXYDEVICEMETHOD(SetCurrentTexturePalette);
			PROXYDEVICEMETHOD(GetCurrentTexturePalette);
			PROXYDEVICEMETHOD(SetScissorRect);
			PROXYDEVICEMETHOD(GetScissorRect);
			PROXYDEVICEMETHOD(SetSoftwareVertexProcessing);
			PROXYDEVICEMETHOD_(BOOL, GetSoftwareVertexProcessing);
			PROXYDEVICEMETHOD(SetNPatchMode);
			PROXYDEVICEMETHOD_(float, GetNPatchMode);
			PROXYDEVICEMETHOD(DrawPrimitive);
			PROXYDEVICEMETHOD(DrawIndexedPrimitive);
			PROXYDEVICEMETHOD(DrawPrimitiveUP);
			PROXYDEVICEMETHOD(DrawIndexedPrimitiveUP);
			PROXYDEVICEMETHOD(ProcessVertices);
			PROXYDEVICEMETHOD(CreateVertexDeclaration);
			PROXYDEVICEMETHOD(SetVertexDeclaration);
			PROXYDEVICEMETHOD(GetVertexDeclaration);
			PROXYDEVICEMETHOD(SetFVF);
			PROXYDEVICEMETHOD(GetFVF);
			PROXYDEVICEMETHOD(CreateVertexShader);
			PROXYDEVICEMETHOD(SetVertexShader);
			PROXYDEVICEMETHOD(GetVertexShader);
			PROXYDEVICEMETHOD(SetVertexShaderConstantF);
			PROXYDEVICEMETHOD(GetVertexShaderConstantF);
			PROXYDEVICEMETHOD(SetVertexShaderConstantI);
			PROXYDEVICEMETHOD(GetVertexShaderConstantI);
			PROXYDEVICEMETHOD(SetVertexShaderConstantB);
			PROXYDEVICEMETHOD(GetVertexShaderConstantB);
			PROXYDEVICEMETHOD(SetStreamSource);
			PROXYDEVICEMETHOD(GetStreamSource);
			PROXYDEVICEMETHOD(SetStreamSourceFreq);
			PROXYDEVICEMETHOD(GetStreamSourceFreq);
			PROXYDEVICEMETHOD(SetIndices);
			PROXYDEVICEMETHOD(GetIndices);
			PROXYDEVICEMETHOD(CreatePixelShader);
			PROXYDEVICEMETHOD(SetPixelShader);
			PROXYDEVICEMETHOD(GetPixelShader);
			PROXYDEVICEMETHOD(SetPixelShaderConstantF);
			PROXYDEVICEMETHOD(GetPixelShaderConstantF);
			PROXYDEVICEMETHOD(SetPixelShaderConstantI);
			PROXYDEVICEMETHOD(GetPixelShaderConstantI);
			PROXYDEVICEMETHOD(SetPixelShaderConstantB);
			PROXYDEVICEMETHOD(GetPixelShaderConstantB);
			PROXYDEVICEMETHOD(DrawRectPatch);
			PROXYDEVICEMETHOD(DrawTriPatch);
			PROXYDEVICEMETHOD(DeletePatch);
			PROXYDEVICEMETHOD(CreateQuery);

			// IDirect3DDevice9Ex methods
			PROXYDEVICEMETHOD(SetConvolutionMonoKernel);
			PROXYDEVICEMETHOD(ComposeRects);
			PROXYDEVICEMETHOD(PresentEx);
			PROXYDEVICEMETHOD(GetGPUThreadPriority);
			PROXYDEVICEMETHOD(SetGPUThreadPriority);
			PROXYDEVICEMETHOD(WaitForVBlank);
			PROXYDEVICEMETHOD(CheckResourceResidency);
			PROXYDEVICEMETHOD(SetMaximumFrameLatency);
			PROXYDEVICEMETHOD(GetMaximumFrameLatency);
			PROXYDEVICEMETHOD(CheckDeviceState);
			PROXYDEVICEMETHOD(CreateRenderTargetEx);
			PROXYDEVICEMETHOD(CreateOffscreenPlainSurfaceEx);
			PROXYDEVICEMETHOD(CreateDepthStencilSurfaceEx);
			PROXYDEVICEMETHOD(ResetEx);
			PROXYDEVICEMETHOD(GetDisplayModeEx);
		};

		static const TCHAR* GetFunctionName(int id)
		{
			return GetD3DDevice9MethodName(id);
		}

		void* func[MAX_METHODS_EX];
	};

	union DeviceSetFunctions
	{
		DeviceSetFunctions()
		{
			ZeroMemory(func, sizeof(func));
		}
		struct
		{
			PROXYDEVICEMETHOD(SetMaterial);
			PROXYDEVICEMETHOD(SetRenderState);
			PROXYDEVICEMETHOD(SetTexture);
			PROXYDEVICEMETHOD(SetTextureStageState);
			PROXYDEVICEMETHOD(SetSamplerState);
			PROXYDEVICEMETHOD(SetFVF);
			PROXYDEVICEMETHOD(SetVertexShader);
			PROXYDEVICEMETHOD(SetVertexShaderConstantF);
			PROXYDEVICEMETHOD(SetVertexShaderConstantI);
			PROXYDEVICEMETHOD(SetVertexShaderConstantB);
			PROXYDEVICEMETHOD(SetStreamSource);
			PROXYDEVICEMETHOD(SetStreamSourceFreq);
			PROXYDEVICEMETHOD(SetIndices);
			PROXYDEVICEMETHOD(SetPixelShader);
			PROXYDEVICEMETHOD(SetPixelShaderConstantF);
			PROXYDEVICEMETHOD(SetPixelShaderConstantI);
			PROXYDEVICEMETHOD(SetPixelShaderConstantB);
		};
		void* func[17];
	};
}

class CBaseStereoRenderer;

class ProxyDevice9 : public ProxyBase<IDirect3DDevice9, IDirect3DDevice9MethodNames::FunctionList>
{
public:
	void	refreshMutablePointers();

	#undef	PROXYDEVICEMETHOD
	#undef	PROXYDEVICEMETHOD_
	#define PROXYDEVICEMETHOD(a)		HRESULT  a 
	#define PROXYDEVICEMETHOD_(type, a)	type a
	
	// IUnknown methods
	PROXYDEVICEMETHOD(QueryInterface)(REFIID riid, void** ppvObj);
	PROXYDEVICEMETHOD_(ULONG,AddRef)();
	PROXYDEVICEMETHOD_(ULONG,Release)();

	// IDirect3DDevice9 methods
	PROXYDEVICEMETHOD(TestCooperativeLevel)();
	PROXYDEVICEMETHOD_(UINT, GetAvailableTextureMem)();
	PROXYDEVICEMETHOD(EvictManagedResources)();
	PROXYDEVICEMETHOD(GetDirect3D)(IDirect3D9** ppD3D9);
	PROXYDEVICEMETHOD(GetDeviceCaps)(D3DCAPS9* pCaps);
	PROXYDEVICEMETHOD(GetDisplayMode)(UINT iSwapChain,D3DDISPLAYMODE* pMode);
	PROXYDEVICEMETHOD(GetCreationParameters)(D3DDEVICE_CREATION_PARAMETERS *pParameters);
	PROXYDEVICEMETHOD(SetCursorProperties)(UINT XHotSpot,UINT YHotSpot,IDirect3DSurface9* pCursorBitmap);
	PROXYDEVICEMETHOD_(void, SetCursorPosition)(int X,int Y,DWORD Flags);
	PROXYDEVICEMETHOD_(BOOL, ShowCursor)(BOOL bShow);
	PROXYDEVICEMETHOD(CreateAdditionalSwapChain)(D3DPRESENT_PARAMETERS* pPresentationParameters,IDirect3DSwapChain9** pSwapChain);
	PROXYDEVICEMETHOD(GetSwapChain)(UINT iSwapChain,IDirect3DSwapChain9** pSwapChain);
	PROXYDEVICEMETHOD_(UINT, GetNumberOfSwapChains)();
	PROXYDEVICEMETHOD(Reset)(D3DPRESENT_PARAMETERS* pPresentationParameters);
	PROXYDEVICEMETHOD(Present)(CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion);
	PROXYDEVICEMETHOD(GetBackBuffer)(UINT iSwapChain,UINT iBackBuffer,D3DBACKBUFFER_TYPE Type,IDirect3DSurface9** ppBackBuffer);
	PROXYDEVICEMETHOD(GetRasterStatus)(UINT iSwapChain,D3DRASTER_STATUS* pRasterStatus);
	PROXYDEVICEMETHOD(SetDialogBoxMode)(BOOL bEnableDialogs);
	PROXYDEVICEMETHOD_(void, SetGammaRamp)(UINT iSwapChain,DWORD Flags,CONST D3DGAMMARAMP* pRamp);
	PROXYDEVICEMETHOD_(void, GetGammaRamp)(UINT iSwapChain,D3DGAMMARAMP* pRamp);
	PROXYDEVICEMETHOD(CreateTexture)(UINT Width,UINT Height,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DTexture9** ppTexture,HANDLE* pSharedHandle);
	PROXYDEVICEMETHOD(CreateVolumeTexture)(UINT Width,UINT Height,UINT Depth,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DVolumeTexture9** ppVolumeTexture,HANDLE* pSharedHandle);
	PROXYDEVICEMETHOD(CreateCubeTexture)(UINT EdgeLength,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DCubeTexture9** ppCubeTexture,HANDLE* pSharedHandle);
	PROXYDEVICEMETHOD(CreateVertexBuffer)(UINT Length,DWORD Usage,DWORD FVF,D3DPOOL Pool,IDirect3DVertexBuffer9** ppVertexBuffer,HANDLE* pSharedHandle);
	PROXYDEVICEMETHOD(CreateIndexBuffer)(UINT Length,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DIndexBuffer9** ppIndexBuffer,HANDLE* pSharedHandle);
	PROXYDEVICEMETHOD(CreateRenderTarget)(UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Lockable,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle);
	PROXYDEVICEMETHOD(CreateDepthStencilSurface)(UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Discard,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle);
	PROXYDEVICEMETHOD(UpdateSurface)(IDirect3DSurface9* pSourceSurface,CONST RECT* pSourceRect,IDirect3DSurface9* pDestinationSurface,CONST POINT* pDestPoint);
	PROXYDEVICEMETHOD(UpdateTexture)(IDirect3DBaseTexture9* pSourceTexture,IDirect3DBaseTexture9* pDestinationTexture);
	PROXYDEVICEMETHOD(GetRenderTargetData)(IDirect3DSurface9* pRenderTarget,IDirect3DSurface9* pDestSurface);
	PROXYDEVICEMETHOD(GetFrontBufferData)(UINT iSwapChain,IDirect3DSurface9* pDestSurface);
	PROXYDEVICEMETHOD(StretchRect)(IDirect3DSurface9* pSourceSurface,CONST RECT* pSourceRect,IDirect3DSurface9* pDestSurface,CONST RECT* pDestRect,D3DTEXTUREFILTERTYPE Filter);
	PROXYDEVICEMETHOD(ColorFill)(IDirect3DSurface9* pSurface,CONST RECT* pRect,D3DCOLOR color);
	PROXYDEVICEMETHOD(CreateOffscreenPlainSurface)(UINT Width,UINT Height,D3DFORMAT Format,D3DPOOL Pool,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle);
	PROXYDEVICEMETHOD(SetRenderTarget)(DWORD RenderTargetIndex,IDirect3DSurface9* pRenderTarget);
	PROXYDEVICEMETHOD(GetRenderTarget)(DWORD RenderTargetIndex,IDirect3DSurface9** ppRenderTarget);
	PROXYDEVICEMETHOD(SetDepthStencilSurface)(IDirect3DSurface9* pNewZStencil);
	PROXYDEVICEMETHOD(GetDepthStencilSurface)(IDirect3DSurface9** ppZStencilSurface);
	PROXYDEVICEMETHOD(BeginScene)();
	PROXYDEVICEMETHOD(EndScene)();
	PROXYDEVICEMETHOD(Clear)(DWORD Count,CONST D3DRECT* pRects,DWORD Flags,D3DCOLOR Color,float Z,DWORD Stencil);
	PROXYDEVICEMETHOD(SetTransform)(D3DTRANSFORMSTATETYPE State,CONST D3DMATRIX* pMatrix);
	PROXYDEVICEMETHOD(GetTransform)(D3DTRANSFORMSTATETYPE State,D3DMATRIX* pMatrix);
	PROXYDEVICEMETHOD(MultiplyTransform)(D3DTRANSFORMSTATETYPE,CONST D3DMATRIX*);
	PROXYDEVICEMETHOD(SetViewport)(CONST D3DVIEWPORT9* pViewport);
	PROXYDEVICEMETHOD(GetViewport)(D3DVIEWPORT9* pViewport);
	PROXYDEVICEMETHOD(SetMaterial)(CONST D3DMATERIAL9* pMaterial);
	PROXYDEVICEMETHOD(GetMaterial)(D3DMATERIAL9* pMaterial);
	PROXYDEVICEMETHOD(SetLight)(DWORD Index,CONST D3DLIGHT9*);
	PROXYDEVICEMETHOD(GetLight)(DWORD Index,D3DLIGHT9*);
	PROXYDEVICEMETHOD(LightEnable)(DWORD Index,BOOL Enable);
	PROXYDEVICEMETHOD(GetLightEnable)(DWORD Index,BOOL* pEnable);
	PROXYDEVICEMETHOD(SetClipPlane)(DWORD Index,CONST float* pPlane);
	PROXYDEVICEMETHOD(GetClipPlane)(DWORD Index,float* pPlane);
	PROXYDEVICEMETHOD(SetRenderState)(D3DRENDERSTATETYPE State,DWORD Value);
	PROXYDEVICEMETHOD(GetRenderState)(D3DRENDERSTATETYPE State,DWORD* pValue);
	PROXYDEVICEMETHOD(CreateStateBlock)(D3DSTATEBLOCKTYPE Type,IDirect3DStateBlock9** ppSB);
	PROXYDEVICEMETHOD(BeginStateBlock)();
	PROXYDEVICEMETHOD(EndStateBlock)(IDirect3DStateBlock9** ppSB);
	PROXYDEVICEMETHOD(SetClipStatus)(CONST D3DCLIPSTATUS9* pClipStatus);
	PROXYDEVICEMETHOD(GetClipStatus)(D3DCLIPSTATUS9* pClipStatus);
	PROXYDEVICEMETHOD(GetTexture)(DWORD Stage,IDirect3DBaseTexture9** ppTexture);
	PROXYDEVICEMETHOD(SetTexture)(DWORD Stage,IDirect3DBaseTexture9* pTexture);
	PROXYDEVICEMETHOD(GetTextureStageState)(DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD* pValue);
	PROXYDEVICEMETHOD(SetTextureStageState)(DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD Value);
	PROXYDEVICEMETHOD(GetSamplerState)(DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD* pValue);
	PROXYDEVICEMETHOD(SetSamplerState)(DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD Value);
	PROXYDEVICEMETHOD(ValidateDevice)(DWORD* pNumPasses);
	PROXYDEVICEMETHOD(SetPaletteEntries)(UINT PaletteNumber,CONST PALETTEENTRY* pEntries);
	PROXYDEVICEMETHOD(GetPaletteEntries)(UINT PaletteNumber,PALETTEENTRY* pEntries);
	PROXYDEVICEMETHOD(SetCurrentTexturePalette)(UINT PaletteNumber);
	PROXYDEVICEMETHOD(GetCurrentTexturePalette)(UINT *PaletteNumber);
	PROXYDEVICEMETHOD(SetScissorRect)(CONST RECT* pRect);
	PROXYDEVICEMETHOD(GetScissorRect)(RECT* pRect);
	PROXYDEVICEMETHOD(SetSoftwareVertexProcessing)(BOOL bSoftware);
	PROXYDEVICEMETHOD_(BOOL, GetSoftwareVertexProcessing)();
	PROXYDEVICEMETHOD(SetNPatchMode)(float nSegments);
	PROXYDEVICEMETHOD_(float, GetNPatchMode)();
	PROXYDEVICEMETHOD(DrawPrimitive)(D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount);
	PROXYDEVICEMETHOD(DrawIndexedPrimitive)(D3DPRIMITIVETYPE,INT BaseVertexIndex,UINT MinVertexIndex,UINT NumVertices,UINT startIndex,UINT primCount);
	PROXYDEVICEMETHOD(DrawPrimitiveUP)(D3DPRIMITIVETYPE PrimitiveType,UINT PrimitiveCount,CONST void* pVertexStreamZeroData,UINT VertexStreamZeroStride);
	PROXYDEVICEMETHOD(DrawIndexedPrimitiveUP)(D3DPRIMITIVETYPE PrimitiveType,UINT MinVertexIndex,UINT NumVertices,UINT PrimitiveCount,CONST void* pIndexData,D3DFORMAT IndexDataFormat,CONST void* pVertexStreamZeroData,UINT VertexStreamZeroStride);
	PROXYDEVICEMETHOD(ProcessVertices)(UINT SrcStartIndex,UINT DestIndex,UINT VertexCount,IDirect3DVertexBuffer9* pDestBuffer,IDirect3DVertexDeclaration9* pVertexDecl,DWORD Flags);
	PROXYDEVICEMETHOD(CreateVertexDeclaration)(CONST D3DVERTEXELEMENT9* pVertexElements,IDirect3DVertexDeclaration9** ppDecl);
	PROXYDEVICEMETHOD(SetVertexDeclaration)(IDirect3DVertexDeclaration9* pDecl);
	PROXYDEVICEMETHOD(GetVertexDeclaration)(IDirect3DVertexDeclaration9** ppDecl);
	PROXYDEVICEMETHOD(SetFVF)(DWORD FVF);
	PROXYDEVICEMETHOD(GetFVF)(DWORD* pFVF);
	PROXYDEVICEMETHOD(CreateVertexShader)(CONST DWORD* pFunction,IDirect3DVertexShader9** ppShader);
	PROXYDEVICEMETHOD(SetVertexShader)(IDirect3DVertexShader9* pShader);
	PROXYDEVICEMETHOD(GetVertexShader)(IDirect3DVertexShader9** ppShader);
	PROXYDEVICEMETHOD(SetVertexShaderConstantF)(UINT StartRegister,CONST float* pConstantData,UINT Vector4fCount);
	PROXYDEVICEMETHOD(GetVertexShaderConstantF)(UINT StartRegister,float* pConstantData,UINT Vector4fCount);
	PROXYDEVICEMETHOD(SetVertexShaderConstantI)(UINT StartRegister,CONST int* pConstantData,UINT Vector4iCount);
	PROXYDEVICEMETHOD(GetVertexShaderConstantI)(UINT StartRegister,int* pConstantData,UINT Vector4iCount);
	PROXYDEVICEMETHOD(SetVertexShaderConstantB)(UINT StartRegister,CONST BOOL* pConstantData,UINT  BoolCount);
	PROXYDEVICEMETHOD(GetVertexShaderConstantB)(UINT StartRegister,BOOL* pConstantData,UINT BoolCount);
	PROXYDEVICEMETHOD(SetStreamSource)(UINT StreamNumber,IDirect3DVertexBuffer9* pStreamData,UINT OffsetInBytes,UINT Stride);
	PROXYDEVICEMETHOD(GetStreamSource)(UINT StreamNumber,IDirect3DVertexBuffer9** ppStreamData,UINT* pOffsetInBytes,UINT* pStride);
	PROXYDEVICEMETHOD(SetStreamSourceFreq)(UINT StreamNumber,UINT Setting);
	PROXYDEVICEMETHOD(GetStreamSourceFreq)(UINT StreamNumber,UINT* pSetting);
	PROXYDEVICEMETHOD(SetIndices)(IDirect3DIndexBuffer9* pIndexData);
	PROXYDEVICEMETHOD(GetIndices)(IDirect3DIndexBuffer9** ppIndexData);
	PROXYDEVICEMETHOD(CreatePixelShader)(CONST DWORD* pFunction,IDirect3DPixelShader9** ppShader);
	PROXYDEVICEMETHOD(SetPixelShader)(IDirect3DPixelShader9* pShader);
	PROXYDEVICEMETHOD(GetPixelShader)(IDirect3DPixelShader9** ppShader);
	PROXYDEVICEMETHOD(SetPixelShaderConstantF)(UINT StartRegister,CONST float* pConstantData,UINT Vector4fCount);
	PROXYDEVICEMETHOD(GetPixelShaderConstantF)(UINT StartRegister,float* pConstantData,UINT Vector4fCount);
	PROXYDEVICEMETHOD(SetPixelShaderConstantI)(UINT StartRegister,CONST int* pConstantData,UINT Vector4iCount);
	PROXYDEVICEMETHOD(GetPixelShaderConstantI)(UINT StartRegister,int* pConstantData,UINT Vector4iCount);
	PROXYDEVICEMETHOD(SetPixelShaderConstantB)(UINT StartRegister,CONST BOOL* pConstantData,UINT  BoolCount);
	PROXYDEVICEMETHOD(GetPixelShaderConstantB)(UINT StartRegister,BOOL* pConstantData,UINT BoolCount);
	PROXYDEVICEMETHOD(DrawRectPatch)(UINT Handle,CONST float* pNumSegs,CONST D3DRECTPATCH_INFO* pRectPatchInfo);
	PROXYDEVICEMETHOD(DrawTriPatch)(UINT Handle,CONST float* pNumSegs,CONST D3DTRIPATCH_INFO* pTriPatchInfo);
	PROXYDEVICEMETHOD(DeletePatch)(UINT Handle);
	PROXYDEVICEMETHOD(CreateQuery)(D3DQUERYTYPE Type,IDirect3DQuery9** ppQuery);

	// IDirect3DDevice9Ex methods 
	PROXYDEVICEMETHOD(SetConvolutionMonoKernel)(UINT width,UINT height,float* rows,float* columns);
	PROXYDEVICEMETHOD(ComposeRects)(IDirect3DSurface9* pSrc,IDirect3DSurface9* pDst,IDirect3DVertexBuffer9* pSrcRectDescs,UINT NumRects,IDirect3DVertexBuffer9* pDstRectDescs,D3DCOMPOSERECTSOP Operation,int Xoffset,int Yoffset);
	PROXYDEVICEMETHOD(PresentEx)(CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion,DWORD dwFlags);
	PROXYDEVICEMETHOD(GetGPUThreadPriority)(INT* pPriority);
	PROXYDEVICEMETHOD(SetGPUThreadPriority)(INT Priority);
	PROXYDEVICEMETHOD(WaitForVBlank)(UINT iSwapChain);
	PROXYDEVICEMETHOD(CheckResourceResidency)(IDirect3DResource9** pResourceArray,UINT32 NumResources);
	PROXYDEVICEMETHOD(SetMaximumFrameLatency)(UINT MaxLatency);
	PROXYDEVICEMETHOD(GetMaximumFrameLatency)(UINT* pMaxLatency);
	PROXYDEVICEMETHOD(CheckDeviceState)(HWND hDestinationWindow);
	PROXYDEVICEMETHOD(CreateRenderTargetEx)(UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Lockable,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle,DWORD Usage);
	PROXYDEVICEMETHOD(CreateOffscreenPlainSurfaceEx)(UINT Width,UINT Height,D3DFORMAT Format,D3DPOOL Pool,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle,DWORD Usage);
	PROXYDEVICEMETHOD(CreateDepthStencilSurfaceEx)(UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Discard,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle,DWORD Usage);
	PROXYDEVICEMETHOD(ResetEx)(D3DPRESENT_PARAMETERS* pPresentationParameters,D3DDISPLAYMODEEX *pFullscreenDisplayMode);
	PROXYDEVICEMETHOD(GetDisplayModeEx)(UINT iSwapChain,D3DDISPLAYMODEEX* pMode,D3DDISPLAYROTATION* pRotation);

protected:
	IDirect3DDevice9MethodNames::DeviceSetFunctions	m_fpMutableFunctions[2];
};
