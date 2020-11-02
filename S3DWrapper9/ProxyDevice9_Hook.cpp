#include "stdafx.h"
#include "BaseStereoRenderer.h"
#include "ProxyDevice9-inl.h"
#include "ProxyDevice9_Hook.h"

using namespace IDirect3DDevice9MethodNames;

void ProxyDevice9::refreshMutablePointers()
{
	using namespace IDirect3DDevice9MethodNames;

	void** p = *(void***)m_pHookedInterface;
	m_fpMutableFunctions[0].func[ 0] = m_fpOriginal.func[_SetMaterial_];
	m_fpMutableFunctions[0].func[ 1] = m_fpOriginal.func[_SetRenderState_];
	m_fpMutableFunctions[0].func[ 2] = m_fpOriginal.func[_SetTexture_];
	m_fpMutableFunctions[0].func[ 3] = m_fpOriginal.func[_SetTextureStageState_];
	m_fpMutableFunctions[0].func[ 4] = m_fpOriginal.func[_SetSamplerState_];
	m_fpMutableFunctions[0].func[ 5] = m_fpOriginal.func[_SetFVF_];
	m_fpMutableFunctions[0].func[ 6] = m_fpOriginal.func[_SetVertexShader_];
	m_fpMutableFunctions[0].func[ 7] = m_fpOriginal.func[_SetVertexShaderConstantF_];
	m_fpMutableFunctions[0].func[ 8] = m_fpOriginal.func[_SetVertexShaderConstantI_];
	m_fpMutableFunctions[0].func[ 9] = m_fpOriginal.func[_SetVertexShaderConstantB_];
	m_fpMutableFunctions[0].func[10] = m_fpOriginal.func[_SetStreamSource_];
	m_fpMutableFunctions[0].func[11] = m_fpOriginal.func[_SetStreamSourceFreq_];
	m_fpMutableFunctions[0].func[12] = m_fpOriginal.func[_SetIndices_];
	m_fpMutableFunctions[0].func[13] = m_fpOriginal.func[_SetPixelShader_];
	m_fpMutableFunctions[0].func[14] = m_fpOriginal.func[_SetPixelShaderConstantF_];
	m_fpMutableFunctions[0].func[15] = m_fpOriginal.func[_SetPixelShaderConstantI_];
	m_fpMutableFunctions[0].func[16] = m_fpOriginal.func[_SetPixelShaderConstantB_];
	HRESULT hr = (m_pHookedInterface->*m_fpOriginal.BeginStateBlock)();
	m_fpMutableFunctions[1].func[ 0] = UniqueGetCode(p[_SetMaterial_]);
	m_fpMutableFunctions[1].func[ 1] = UniqueGetCode(p[_SetRenderState_]);
	m_fpMutableFunctions[1].func[ 2] = UniqueGetCode(p[_SetTexture_]);
	m_fpMutableFunctions[1].func[ 3] = UniqueGetCode(p[_SetTextureStageState_]);
	m_fpMutableFunctions[1].func[ 4] = UniqueGetCode(p[_SetSamplerState_]);
	m_fpMutableFunctions[1].func[ 5] = UniqueGetCode(p[_SetFVF_]);
	m_fpMutableFunctions[1].func[ 6] = UniqueGetCode(p[_SetVertexShader_]);
	m_fpMutableFunctions[1].func[ 7] = UniqueGetCode(p[_SetVertexShaderConstantF_]);
	m_fpMutableFunctions[1].func[ 8] = UniqueGetCode(p[_SetVertexShaderConstantI_]);
	m_fpMutableFunctions[1].func[ 9] = UniqueGetCode(p[_SetVertexShaderConstantB_]);
	m_fpMutableFunctions[1].func[10] = UniqueGetCode(p[_SetStreamSource_]);
	m_fpMutableFunctions[1].func[11] = UniqueGetCode(p[_SetStreamSourceFreq_]);
	m_fpMutableFunctions[1].func[12] = UniqueGetCode(p[_SetIndices_]);
	m_fpMutableFunctions[1].func[13] = UniqueGetCode(p[_SetPixelShader_]);
	m_fpMutableFunctions[1].func[14] = UniqueGetCode(p[_SetPixelShaderConstantF_]);
	m_fpMutableFunctions[1].func[15] = UniqueGetCode(p[_SetPixelShaderConstantI_]);
	m_fpMutableFunctions[1].func[16] = UniqueGetCode(p[_SetPixelShaderConstantB_]);
	CComPtr<IDirect3DStateBlock9> pSB;
	hr = (m_pHookedInterface->*m_fpOriginal.EndStateBlock)(&pSB);
}

//--- unmodified IDirect3DDevice9 function.  List of remain from BaseStereoRenderer::HookDevice() class functions.  Hooked in NGuard compatible hooking case ---

void CBaseStereoRenderer::HookIDirect3DDevice9All()
{
#define HOOKINTERFACEENTRY(x)		m_Direct3DDevice.HookInterfaceEntry(this, _ ##x ##_, Proxy_ ##x)

	//--- it's not necessary to hook unmodified functions ---
	//--- uncomment appropriate hook if you wish modify function ---
	
	// IUnknown methods
	//HOOKINTERFACEENTRY(QueryInterface);
	HOOKINTERFACEENTRY(AddRef);
	HOOKINTERFACEENTRY(Release);
	
	// IDirect3DDevice9 methods
	HOOKINTERFACEENTRY(TestCooperativeLevel);
	//HOOKINTERFACEENTRY(GetAvailableTextureMem);
	//HOOKINTERFACEENTRY(EvictManagedResources);
	//HOOKINTERFACEENTRY(GetDirect3D);
	//HOOKINTERFACEENTRY(GetDeviceCaps);
	//HOOKINTERFACEENTRY(GetDisplayMode);
	//HOOKINTERFACEENTRY(GetCreationParameters);
	//HOOKINTERFACEENTRY(SetCursorProperties);
	//HOOKINTERFACEENTRY(SetCursorPosition);
	HOOKINTERFACEENTRY(ShowCursor);
	HOOKINTERFACEENTRY(CreateAdditionalSwapChain);
	HOOKINTERFACEENTRY(GetSwapChain);
	HOOKINTERFACEENTRY(GetNumberOfSwapChains);
	HOOKINTERFACEENTRY(Reset);
	HOOKINTERFACEENTRY(Present);
	HOOKINTERFACEENTRY(GetBackBuffer);
	//HOOKINTERFACEENTRY(GetRasterStatus);
	//HOOKINTERFACEENTRY(SetDialogBoxMode);
	HOOKINTERFACEENTRY(SetGammaRamp);
	HOOKINTERFACEENTRY(GetGammaRamp);
	HOOKINTERFACEENTRY(CreateTexture);
	HOOKINTERFACEENTRY(CreateVolumeTexture);
	HOOKINTERFACEENTRY(CreateCubeTexture);
	HOOKINTERFACEENTRY(CreateVertexBuffer);
	//HOOKINTERFACEENTRY(CreateIndexBuffer);
	HOOKINTERFACEENTRY(CreateRenderTarget);
	HOOKINTERFACEENTRY(CreateDepthStencilSurface);
	if(gInfo.WideRenderTarget)
	{
		HOOKINTERFACEENTRY(UpdateSurface);
		HOOKINTERFACEENTRY(UpdateTexture);
		HOOKINTERFACEENTRY(GetRenderTargetData);
	}
	//HOOKINTERFACEENTRY(GetFrontBufferData);
	HOOKINTERFACEENTRY(StretchRect);
	HOOKINTERFACEENTRY(ColorFill);
	//HOOKINTERFACEENTRY(CreateOffscreenPlainSurface);
	HOOKINTERFACEENTRY(SetRenderTarget);
	//HOOKINTERFACEENTRY(GetRenderTarget);
	HOOKINTERFACEENTRY(SetDepthStencilSurface);
	//HOOKINTERFACEENTRY(GetDepthStencilSurface);
	HOOKINTERFACEENTRY(BeginScene);
	HOOKINTERFACEENTRY(EndScene);
	HOOKINTERFACEENTRY(Clear);
	//HOOKINTERFACEENTRY(SetTransform);						//--- already hooked in BaseStereoRenderer ---
	//HOOKINTERFACEENTRY(GetTransform);
	//HOOKINTERFACEENTRY(MultiplyTransform);				//--- already hooked in BaseStereoRenderer ---
	//HOOKINTERFACEENTRY(SetViewport);						//--- already hooked in BaseStereoRenderer ---
	//HOOKINTERFACEENTRY(GetViewport);
	//HOOKINTERFACEENTRY(SetMaterial);
	//HOOKINTERFACEENTRY(GetMaterial);
	//HOOKINTERFACEENTRY(SetLight);
	//HOOKINTERFACEENTRY(GetLight);
	//HOOKINTERFACEENTRY(LightEnable);
	//HOOKINTERFACEENTRY(GetLightEnable);
	//HOOKINTERFACEENTRY(SetClipPlane);
	//HOOKINTERFACEENTRY(GetClipPlane);
	//HOOKINTERFACEENTRY(SetRenderState);					//--- already hooked in BaseStereoRenderer ---
	//HOOKINTERFACEENTRY(GetRenderState);
	//HOOKINTERFACEENTRY(CreateStateBlock);
	HOOKINTERFACEENTRY(BeginStateBlock);
	HOOKINTERFACEENTRY(EndStateBlock);
	//HOOKINTERFACEENTRY(SetClipStatus);
	//HOOKINTERFACEENTRY(GetClipStatus);
	if(gInfo.WideRenderTarget) {
		HOOKINTERFACEENTRY(GetTexture);
	}
	//HOOKINTERFACEENTRY(SetTexture);						//--- already hooked in BaseStereoRenderer ---
	//HOOKINTERFACEENTRY(GetTextureStageState);
	//HOOKINTERFACEENTRY(SetTextureStageState);
	//HOOKINTERFACEENTRY(GetSamplerState);
	//HOOKINTERFACEENTRY(SetSamplerState);
	//HOOKINTERFACEENTRY(ValidateDevice);
	//HOOKINTERFACEENTRY(SetPaletteEntries);
	//HOOKINTERFACEENTRY(GetPaletteEntries);
	//HOOKINTERFACEENTRY(SetCurrentTexturePalette);
	//HOOKINTERFACEENTRY(GetCurrentTexturePalette);
	//HOOKINTERFACEENTRY(SetScissorRect);					//--- already hooked in BaseStereoRenderer ---
	//HOOKINTERFACEENTRY(GetScissorRect);
	//HOOKINTERFACEENTRY(SetSoftwareVertexProcessing);
	//HOOKINTERFACEENTRY(GetSoftwareVertexProcessing);
	//HOOKINTERFACEENTRY(SetNPatchMode);
	//HOOKINTERFACEENTRY(GetNPatchMode);
	HOOKINTERFACEENTRY(DrawPrimitive);
	HOOKINTERFACEENTRY(DrawIndexedPrimitive);
	HOOKINTERFACEENTRY(DrawPrimitiveUP);
	HOOKINTERFACEENTRY(DrawIndexedPrimitiveUP);
	//HOOKINTERFACEENTRY(ProcessVertices);
	//HOOKINTERFACEENTRY(CreateVertexDeclaration);
	//HOOKINTERFACEENTRY(SetVertexDeclaration);				//--- already hooked in BaseStereoRenderer ---
	//HOOKINTERFACEENTRY(GetVertexDeclaration);
	//HOOKINTERFACEENTRY(SetFVF);							//--- already hooked in BaseStereoRenderer ---
	//HOOKINTERFACEENTRY(GetFVF);
	HOOKINTERFACEENTRY(CreateVertexShader);
	//HOOKINTERFACEENTRY(SetVertexShader);					//--- already hooked in BaseStereoRenderer ---
	//HOOKINTERFACEENTRY(GetVertexShader);					//--- already hooked in BaseStereoRenderer ---
	//HOOKINTERFACEENTRY(SetVertexShaderConstantF);			//--- already hooked in BaseStereoRenderer ---
	//HOOKINTERFACEENTRY(GetVertexShaderConstantF);			//--- already hooked in BaseStereoRenderer ---
	//HOOKINTERFACEENTRY(SetVertexShaderConstantI);
	//HOOKINTERFACEENTRY(GetVertexShaderConstantI);
	//HOOKINTERFACEENTRY(SetVertexShaderConstantB);
	//HOOKINTERFACEENTRY(GetVertexShaderConstantB);
	//HOOKINTERFACEENTRY(SetStreamSource);					//--- already hooked in BaseStereoRenderer ---
	//HOOKINTERFACEENTRY(GetStreamSource);
	//HOOKINTERFACEENTRY(SetStreamSourceFreq);
	//HOOKINTERFACEENTRY(GetStreamSourceFreq);
	//HOOKINTERFACEENTRY(SetIndices);
	//HOOKINTERFACEENTRY(GetIndices);
	HOOKINTERFACEENTRY(CreatePixelShader);
	//HOOKINTERFACEENTRY(SetPixelShader);					//--- already hooked in BaseStereoRenderer ---
	HOOKINTERFACEENTRY(GetPixelShader);
	HOOKINTERFACEENTRY(SetPixelShaderConstantF);
	HOOKINTERFACEENTRY(GetPixelShaderConstantF);
	//HOOKINTERFACEENTRY(SetPixelShaderConstantI);
	//HOOKINTERFACEENTRY(GetPixelShaderConstantI);
	//HOOKINTERFACEENTRY(SetPixelShaderConstantB);
	//HOOKINTERFACEENTRY(GetPixelShaderConstantB);
	HOOKINTERFACEENTRY(DrawRectPatch);
	HOOKINTERFACEENTRY(DrawTriPatch);
	//HOOKINTERFACEENTRY(DeletePatch);
	//HOOKINTERFACEENTRY(CreateQuery);

	if(m_Direct3DDevice.getExMode() != EXMODE_NONE)
	{
		// IDirect3DDevice9Ex methods
		//HOOKINTERFACEENTRY(SetConvolutionMonoKernel);
		//HOOKINTERFACEENTRY(ComposeRects);
		HOOKINTERFACEENTRY(PresentEx);
		//HOOKINTERFACEENTRY(GetGPUThreadPriority);
		//HOOKINTERFACEENTRY(SetGPUThreadPriority);
		//HOOKINTERFACEENTRY(WaitForVBlank);
		//HOOKINTERFACEENTRY(CheckResourceResidency);
		//HOOKINTERFACEENTRY(SetMaximumFrameLatency);
		//HOOKINTERFACEENTRY(GetMaximumFrameLatency);
		//HOOKINTERFACEENTRY(CheckDeviceState);
		HOOKINTERFACEENTRY(CreateRenderTargetEx);
		//HOOKINTERFACEENTRY(CreateOffscreenPlainSurfaceEx);
		HOOKINTERFACEENTRY(CreateDepthStencilSurfaceEx);
		HOOKINTERFACEENTRY(ResetEx);
		//HOOKINTERFACEENTRY(GetDisplayModeEx);
	}
#undef HOOKINTERFACEENTRY
}

void CBaseStereoRenderer::UnHookIDirect3DDevice9All()
{
#define UNHOOKINTERFACEENTRY(x)			m_Direct3DDevice.UnhookInterfaceEntry(this, _ ##x ##_)

	//--- it's not necessary to unhook unmodified functions ---
	//--- uncomment appropriate unhook if you wish modify function ---

	// IUnknown methods
	//UNHOOKINTERFACEENTRY(QueryInterface);
	UNHOOKINTERFACEENTRY(AddRef);
	UNHOOKINTERFACEENTRY(Release);

	// IDirect3DDevice9 methods
	UNHOOKINTERFACEENTRY(TestCooperativeLevel);
	//UNHOOKINTERFACEENTRY(GetAvailableTextureMem);
	//UNHOOKINTERFACEENTRY(EvictManagedResources);
	//UNHOOKINTERFACEENTRY(GetDirect3D);
	//UNHOOKINTERFACEENTRY(GetDeviceCaps);
	//UNHOOKINTERFACEENTRY(GetDisplayMode);
	//UNHOOKINTERFACEENTRY(GetCreationParameters);
	//UNHOOKINTERFACEENTRY(SetCursorProperties);
	//UNHOOKINTERFACEENTRY(SetCursorPosition);
	UNHOOKINTERFACEENTRY(ShowCursor);
	UNHOOKINTERFACEENTRY(CreateAdditionalSwapChain);
	UNHOOKINTERFACEENTRY(GetSwapChain);
	UNHOOKINTERFACEENTRY(GetNumberOfSwapChains);
	UNHOOKINTERFACEENTRY(Reset);
	UNHOOKINTERFACEENTRY(Present);
	UNHOOKINTERFACEENTRY(GetBackBuffer);
	//UNHOOKINTERFACEENTRY(GetRasterStatus);
	//UNHOOKINTERFACEENTRY(SetDialogBoxMode);
	UNHOOKINTERFACEENTRY(SetGammaRamp);
	UNHOOKINTERFACEENTRY(GetGammaRamp);
	UNHOOKINTERFACEENTRY(CreateTexture);
	UNHOOKINTERFACEENTRY(CreateVolumeTexture);
	UNHOOKINTERFACEENTRY(CreateCubeTexture);
	UNHOOKINTERFACEENTRY(CreateVertexBuffer);
	//UNHOOKINTERFACEENTRY(CreateIndexBuffer);
	UNHOOKINTERFACEENTRY(CreateRenderTarget);
	UNHOOKINTERFACEENTRY(CreateDepthStencilSurface);
	if(gInfo.WideRenderTarget)
	{
		UNHOOKINTERFACEENTRY(UpdateSurface);
		UNHOOKINTERFACEENTRY(UpdateTexture);
		UNHOOKINTERFACEENTRY(GetRenderTargetData);
	}
	//UNHOOKINTERFACEENTRY(GetFrontBufferData);
	UNHOOKINTERFACEENTRY(StretchRect);
	UNHOOKINTERFACEENTRY(ColorFill);
	//UNHOOKINTERFACEENTRY(CreateOffscreenPlainSurface);
	UNHOOKINTERFACEENTRY(SetRenderTarget);
	//UNHOOKINTERFACEENTRY(GetRenderTarget);
	UNHOOKINTERFACEENTRY(SetDepthStencilSurface);
	//UNHOOKINTERFACEENTRY(GetDepthStencilSurface);
	UNHOOKINTERFACEENTRY(BeginScene);
	UNHOOKINTERFACEENTRY(EndScene);
	UNHOOKINTERFACEENTRY(Clear);
	//UNHOOKINTERFACEENTRY(SetTransform);					//--- already unhooked in BaseStereoRenderer ---
	//UNHOOKINTERFACEENTRY(GetTransform);
	//UNHOOKINTERFACEENTRY(MultiplyTransform);				//--- already unhooked in BaseStereoRenderer ---
	//UNHOOKINTERFACEENTRY(SetViewport);					//--- already unhooked in BaseStereoRenderer ---
	//UNHOOKINTERFACEENTRY(GetViewport);
	//UNHOOKINTERFACEENTRY(SetMaterial);
	//UNHOOKINTERFACEENTRY(GetMaterial);
	//UNHOOKINTERFACEENTRY(SetLight);
	//UNHOOKINTERFACEENTRY(GetLight);
	//UNHOOKINTERFACEENTRY(LightEnable);
	//UNHOOKINTERFACEENTRY(GetLightEnable);
	//UNHOOKINTERFACEENTRY(SetClipPlane);
	//UNHOOKINTERFACEENTRY(GetClipPlane);
	//UNHOOKINTERFACEENTRY(SetRenderState);					//--- already unhooked in BaseStereoRenderer ---
	//UNHOOKINTERFACEENTRY(GetRenderState);
	//UNHOOKINTERFACEENTRY(CreateStateBlock);
	UNHOOKINTERFACEENTRY(BeginStateBlock);
	UNHOOKINTERFACEENTRY(EndStateBlock);
	//UNHOOKINTERFACEENTRY(SetClipStatus);
	//UNHOOKINTERFACEENTRY(GetClipStatus);
	if(gInfo.WideRenderTarget) {
		UNHOOKINTERFACEENTRY(GetTexture);
	}
	//UNHOOKINTERFACEENTRY(SetTexture);						//--- already unhooked in BaseStereoRenderer ---
	//UNHOOKINTERFACEENTRY(GetTextureStageState);
	//UNHOOKINTERFACEENTRY(SetTextureStageState);
	//UNHOOKINTERFACEENTRY(GetSamplerState);
	//UNHOOKINTERFACEENTRY(SetSamplerState);
	//UNHOOKINTERFACEENTRY(ValidateDevice);
	//UNHOOKINTERFACEENTRY(SetPaletteEntries);
	//UNHOOKINTERFACEENTRY(GetPaletteEntries);
	//UNHOOKINTERFACEENTRY(SetCurrentTexturePalette);
	//UNHOOKINTERFACEENTRY(GetCurrentTexturePalette);
	//UNHOOKINTERFACEENTRY(SetScissorRect);					//--- already unhooked in BaseStereoRenderer ---
	//UNHOOKINTERFACEENTRY(GetScissorRect);
	//UNHOOKINTERFACEENTRY(SetSoftwareVertexProcessing);
	//UNHOOKINTERFACEENTRY(GetSoftwareVertexProcessing);
	//UNHOOKINTERFACEENTRY(SetNPatchMode);
	//UNHOOKINTERFACEENTRY(GetNPatchMode);
	UNHOOKINTERFACEENTRY(DrawPrimitive);
	UNHOOKINTERFACEENTRY(DrawIndexedPrimitive);
	UNHOOKINTERFACEENTRY(DrawPrimitiveUP);
	UNHOOKINTERFACEENTRY(DrawIndexedPrimitiveUP);
	//UNHOOKINTERFACEENTRY(ProcessVertices);
	//UNHOOKINTERFACEENTRY(CreateVertexDeclaration);
	//UNHOOKINTERFACEENTRY(SetVertexDeclaration);			//--- already unhooked in BaseStereoRenderer ---
	//UNHOOKINTERFACEENTRY(GetVertexDeclaration);
	//UNHOOKINTERFACEENTRY(SetFVF);							//--- already unhooked in BaseStereoRenderer ---
	//UNHOOKINTERFACEENTRY(GetFVF);
	UNHOOKINTERFACEENTRY(CreateVertexShader);
	//UNHOOKINTERFACEENTRY(SetVertexShader);				//--- already unhooked in BaseStereoRenderer ---
	//UNHOOKINTERFACEENTRY(GetVertexShader);				//--- already unhooked in BaseStereoRenderer ---
	//UNHOOKINTERFACEENTRY(SetVertexShaderConstantF);		//--- already unhooked in BaseStereoRenderer ---
	//UNHOOKINTERFACEENTRY(GetVertexShaderConstantF);		//--- already unhooked in BaseStereoRenderer ---
	//UNHOOKINTERFACEENTRY(SetVertexShaderConstantI);
	//UNHOOKINTERFACEENTRY(GetVertexShaderConstantI);
	//UNHOOKINTERFACEENTRY(SetVertexShaderConstantB);
	//UNHOOKINTERFACEENTRY(GetVertexShaderConstantB);
	//UNHOOKINTERFACEENTRY(SetStreamSource);				//--- already unhooked in BaseStereoRenderer ---
	//UNHOOKINTERFACEENTRY(GetStreamSource);
	//UNHOOKINTERFACEENTRY(SetStreamSourceFreq);
	//UNHOOKINTERFACEENTRY(GetStreamSourceFreq);
	//UNHOOKINTERFACEENTRY(SetIndices);
	//UNHOOKINTERFACEENTRY(GetIndices);
	UNHOOKINTERFACEENTRY(CreatePixelShader);
	//UNHOOKINTERFACEENTRY(SetPixelShader);					//--- already unhooked in BaseStereoRenderer ---
	UNHOOKINTERFACEENTRY(GetPixelShader);
	UNHOOKINTERFACEENTRY(SetPixelShaderConstantF);
	UNHOOKINTERFACEENTRY(GetPixelShaderConstantF);
	//UNHOOKINTERFACEENTRY(SetPixelShaderConstantI);
	//UNHOOKINTERFACEENTRY(GetPixelShaderConstantI);
	//UNHOOKINTERFACEENTRY(SetPixelShaderConstantB);
	//UNHOOKINTERFACEENTRY(GetPixelShaderConstantB);
	UNHOOKINTERFACEENTRY(DrawRectPatch);
	UNHOOKINTERFACEENTRY(DrawTriPatch);
	//UNHOOKINTERFACEENTRY(DeletePatch);
	//UNHOOKINTERFACEENTRY(CreateQuery);

	if(m_Direct3DDevice.getExMode() != EXMODE_NONE)
	{
		// IDirect3DDevice9Ex methods
		//UNHOOKINTERFACEENTRY(SetConvolutionMonoKernel);
		//UNHOOKINTERFACEENTRY(ComposeRects);
		UNHOOKINTERFACEENTRY(PresentEx);
		//UNHOOKINTERFACEENTRY(GetGPUThreadPriority);
		//UNHOOKINTERFACEENTRY(SetGPUThreadPriority);
		//UNHOOKINTERFACEENTRY(WaitForVBlank);
		//UNHOOKINTERFACEENTRY(CheckResourceResidency);
		//UNHOOKINTERFACEENTRY(SetMaximumFrameLatency);
		//UNHOOKINTERFACEENTRY(GetMaximumFrameLatency);
		//UNHOOKINTERFACEENTRY(CheckDeviceState);
		UNHOOKINTERFACEENTRY(CreateRenderTargetEx);
		//UNHOOKINTERFACEENTRY(CreateOffscreenPlainSurfaceEx);
		UNHOOKINTERFACEENTRY(CreateDepthStencilSurfaceEx);
		UNHOOKINTERFACEENTRY(ResetEx);
		//UNHOOKINTERFACEENTRY(GetDisplayModeEx);
	}
#undef UNHOOKINTERFACEENTRY
}

namespace IDirect3DDevice9MethodNames
{
	// IUnknown methods
	PROXYSTDMETHOD(QueryInterface)(IDirect3DDevice9* pDevice, REFIID riid, void** ppvObj)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->QueryInterface(riid, ppvObj);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.QueryInterface(riid, ppvObj);
		}
		else
		{
			DECLAREPOINTER(pDevice, QueryInterface);
			HRESULT hResult = CALLFORPOINTER(pDevice, QueryInterface)(riid, ppvObj);
			return hResult;
		}
	}
	
	PROXYSTDMETHOD_(ULONG,AddRef)(IDirect3DDevice9* pDevice)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (pCurrentStereoRenderer->m_Direct3DDevice.m_RouterHookGuard > 0)
				InterlockedIncrement(&pCurrentStereoRenderer->m_Direct3DDevice.m_InternalReferenceCount);
			ULONG rc = pCurrentStereoRenderer->m_Direct3DDevice.AddRef();
			//DEBUG_TRACE3(_T("%s: RefCount = %2i, InternalRefCount = %2i  IsCallInternal = %2i\n"), _T(__FUNCTION__), rc, pCurrentStereoRenderer->m_Direct3DDevice.m_InternalReferenceCount, pCurrentStereoRenderer->m_Direct3DDevice.m_RouterHookGuard);
			return rc - pCurrentStereoRenderer->m_Direct3DDevice.m_InternalReferenceCount;
		}
		else
		{
			DECLAREPOINTER(pDevice, AddRef);
			ULONG count = CALLFORPOINTER(pDevice, AddRef)();
			return count;
		}
	}

	PROXYSTDMETHOD_(ULONG,Release)(IDirect3DDevice9* pDevice)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (pCurrentStereoRenderer->m_Direct3DDevice.m_RouterHookGuard > 0)
				InterlockedDecrement(&pCurrentStereoRenderer->m_Direct3DDevice.m_InternalReferenceCount);
			
			pCurrentStereoRenderer->m_Direct3DDevice.AddRef();
			ULONG rc = pCurrentStereoRenderer->m_Direct3DDevice.Release();
			LONG internalCount = std::max<LONG>(0, pCurrentStereoRenderer->m_Direct3DDevice.m_InternalReferenceCount);
			//DEBUG_TRACE3(_T("%s: RefCount = %2i, InternalRefCount = %2i  IsCallInternal = %2i\n"), _T(__FUNCTION__), rc-1, pCurrentStereoRenderer->m_Direct3DDevice.m_InternalReferenceCount, pCurrentStereoRenderer->m_Direct3DDevice.m_RouterHookGuard);
			if ((LONG)rc <= pCurrentStereoRenderer->m_Direct3DDevice.m_InternalReferenceCount + 1)
			{
				//--- guard from recursion wrapper releasing ---
				pCurrentStereoRenderer->m_Direct3DDevice.m_InternalReferenceCount = -1;
				rc = pCurrentStereoRenderer->Release();
				internalCount = 0;
			}
			else
				rc = pCurrentStereoRenderer->m_Direct3DDevice.Release();
			return rc - internalCount;
		}
		else
		{
			DECLAREPOINTER(pDevice, Release);
			ULONG count = CALLFORPOINTER(pDevice, Release)();
			return count;
		}
	}


	// IDirect3DDevice9 methods
	PROXYSTDMETHOD(TestCooperativeLevel)(IDirect3DDevice9* pDevice)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->TestCooperativeLevel();
			else
			return pCurrentStereoRenderer->m_Direct3DDevice.TestCooperativeLevel();
		}
		else
		{
			DECLAREPOINTER(pDevice, TestCooperativeLevel);
			HRESULT hResult = CALLFORPOINTER(pDevice, TestCooperativeLevel)();
			return hResult;
		}
	}

	PROXYSTDMETHOD_(UINT, GetAvailableTextureMem)(IDirect3DDevice9* pDevice)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->GetAvailableTextureMem();
			else
			return pCurrentStereoRenderer->m_Direct3DDevice.GetAvailableTextureMem();
		}
		else
		{
			DECLAREPOINTER(pDevice, GetAvailableTextureMem);
			UINT count = CALLFORPOINTER(pDevice, GetAvailableTextureMem)();
			return count;
		}
	}
	
	PROXYSTDMETHOD(EvictManagedResources)(IDirect3DDevice9* pDevice)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->EvictManagedResources();
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.EvictManagedResources();
		}
		else
		{
			DECLAREPOINTER(pDevice, EvictManagedResources);
			HRESULT hResult = CALLFORPOINTER(pDevice, EvictManagedResources)();
			return hResult;
		}
	}
		
	PROXYSTDMETHOD(GetDirect3D)(IDirect3DDevice9* pDevice, IDirect3D9** ppD3D9)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->GetDirect3D(ppD3D9);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.GetDirect3D(ppD3D9);
		}
		else
		{
			DECLAREPOINTER(pDevice, GetDirect3D);
			HRESULT hResult = CALLFORPOINTER(pDevice, GetDirect3D)(ppD3D9);
			return hResult;
		}
	}	
	
	PROXYSTDMETHOD(GetDeviceCaps)(IDirect3DDevice9* pDevice, D3DCAPS9* pCaps)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->GetDeviceCaps(pCaps);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.GetDeviceCaps(pCaps);
		}
		else
		{
			DECLAREPOINTER(pDevice, GetDeviceCaps);
			HRESULT hResult = CALLFORPOINTER(pDevice, GetDeviceCaps)(pCaps);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(GetDisplayMode)(IDirect3DDevice9* pDevice, UINT iSwapChain,D3DDISPLAYMODE* pMode)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->GetDisplayMode(iSwapChain,pMode);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.GetDisplayMode(iSwapChain,pMode);
		}
		else
		{
			DECLAREPOINTER(pDevice, GetDisplayMode);
			HRESULT hResult = CALLFORPOINTER(pDevice, GetDisplayMode)(iSwapChain,pMode);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(GetCreationParameters)(IDirect3DDevice9* pDevice, D3DDEVICE_CREATION_PARAMETERS *pParameters)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->GetCreationParameters(pParameters);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.GetCreationParameters(pParameters);
		}
		else
		{
			DECLAREPOINTER(pDevice, GetCreationParameters);
			HRESULT hResult = CALLFORPOINTER(pDevice, GetCreationParameters)(pParameters);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(SetCursorProperties)(IDirect3DDevice9* pDevice, UINT XHotSpot,UINT YHotSpot,IDirect3DSurface9* pCursorBitmap)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->SetCursorProperties(XHotSpot,YHotSpot,pCursorBitmap);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.SetCursorProperties(XHotSpot,YHotSpot,pCursorBitmap);
		}
		else
		{
			DECLAREPOINTER(pDevice, SetCursorProperties);
			HRESULT hResult = CALLFORPOINTER(pDevice, SetCursorProperties)(XHotSpot,YHotSpot,pCursorBitmap);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD_(void, SetCursorPosition)(IDirect3DDevice9* pDevice, int X,int Y,DWORD Flags)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->SetCursorPosition(X,Y,Flags);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.SetCursorPosition(X,Y,Flags);
		}
		else
		{
			DECLAREPOINTER(pDevice, SetCursorPosition);
			CALLFORPOINTER(pDevice, SetCursorPosition)(X,Y,Flags);
		}
	}	
		
	PROXYSTDMETHOD_(BOOL, ShowCursor)(IDirect3DDevice9* pDevice, BOOL bShow)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->ShowCursor(bShow);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.ShowCursor(bShow);
		}
		else
		{
			DECLAREPOINTER(pDevice, ShowCursor);
			BOOL show = CALLFORPOINTER(pDevice, ShowCursor)(bShow);
			return show;
		}
	}	
		
	PROXYSTDMETHOD(CreateAdditionalSwapChain)(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters,IDirect3DSwapChain9** pSwapChain)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->CreateAdditionalSwapChain(pPresentationParameters,pSwapChain);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.CreateAdditionalSwapChain(pPresentationParameters,pSwapChain);
		}
		else
		{
			DECLAREPOINTER(pDevice, CreateAdditionalSwapChain);
			HRESULT hResult = CALLFORPOINTER(pDevice, CreateAdditionalSwapChain)(pPresentationParameters,pSwapChain);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(GetSwapChain)(IDirect3DDevice9* pDevice, UINT iSwapChain,IDirect3DSwapChain9** pSwapChain)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->GetSwapChain(iSwapChain,pSwapChain);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.GetSwapChain(iSwapChain,pSwapChain);
		}
		else
		{
			DECLAREPOINTER(pDevice, GetSwapChain);
			HRESULT hResult = CALLFORPOINTER(pDevice, GetSwapChain)(iSwapChain,pSwapChain);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD_(UINT, GetNumberOfSwapChains)(IDirect3DDevice9* pDevice)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->GetNumberOfSwapChains();
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.GetNumberOfSwapChains();
		}
		else
		{
			DECLAREPOINTER(pDevice, GetNumberOfSwapChains);
			UINT count = CALLFORPOINTER(pDevice, GetNumberOfSwapChains)();
			return count;
		}
	}	
		
	PROXYSTDMETHOD(Reset)(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->Reset(pPresentationParameters);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.Reset(pPresentationParameters);
		}
		else
		{
			DECLAREPOINTER(pDevice, Reset);
			HRESULT hResult = CALLFORPOINTER(pDevice, Reset)(pPresentationParameters);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(Present)(IDirect3DDevice9* pDevice, CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->Present(pSourceRect,pDestRect,hDestWindowOverride,pDirtyRegion);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.Present(pSourceRect,pDestRect,hDestWindowOverride,pDirtyRegion);
		}
		else
		{
			DECLAREPOINTER(pDevice, Present);
			HRESULT hResult = CALLFORPOINTER(pDevice, Present)(pSourceRect,pDestRect,hDestWindowOverride,pDirtyRegion);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(GetBackBuffer)(IDirect3DDevice9* pDevice, UINT iSwapChain,UINT iBackBuffer,D3DBACKBUFFER_TYPE Type,IDirect3DSurface9** ppBackBuffer)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->GetBackBuffer(iSwapChain,iBackBuffer,Type,ppBackBuffer);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.GetBackBuffer(iSwapChain,iBackBuffer,Type,ppBackBuffer);
		}
		else
		{
			DECLAREPOINTER(pDevice, GetBackBuffer);
			HRESULT hResult = CALLFORPOINTER(pDevice, GetBackBuffer)(iSwapChain,iBackBuffer,Type,ppBackBuffer);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(GetRasterStatus)(IDirect3DDevice9* pDevice, UINT iSwapChain,D3DRASTER_STATUS* pRasterStatus)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->GetRasterStatus(iSwapChain,pRasterStatus);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.GetRasterStatus(iSwapChain,pRasterStatus);
		}
		else
		{
			DECLAREPOINTER(pDevice, GetRasterStatus);
			HRESULT hResult = CALLFORPOINTER(pDevice, GetRasterStatus)(iSwapChain,pRasterStatus);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(SetDialogBoxMode)(IDirect3DDevice9* pDevice, BOOL bEnableDialogs)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->SetDialogBoxMode(bEnableDialogs);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.SetDialogBoxMode(bEnableDialogs);
		}
		else
		{
			DECLAREPOINTER(pDevice, SetDialogBoxMode);
			HRESULT hResult = CALLFORPOINTER(pDevice, SetDialogBoxMode)(bEnableDialogs);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD_(void, SetGammaRamp)(IDirect3DDevice9* pDevice, UINT iSwapChain,DWORD Flags,CONST D3DGAMMARAMP* pRamp)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->SetGammaRamp(iSwapChain,Flags,pRamp);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.SetGammaRamp(iSwapChain,Flags,pRamp);
		}
		else
		{
			DECLAREPOINTER(pDevice, SetGammaRamp);
			CALLFORPOINTER(pDevice, SetGammaRamp)(iSwapChain,Flags,pRamp);
		}
	}	
		
	PROXYSTDMETHOD_(void, GetGammaRamp)(IDirect3DDevice9* pDevice, UINT iSwapChain,D3DGAMMARAMP* pRamp)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->GetGammaRamp(iSwapChain,pRamp);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.GetGammaRamp(iSwapChain,pRamp);
		}
		else
		{
			DECLAREPOINTER(pDevice, GetGammaRamp);
			CALLFORPOINTER(pDevice, GetGammaRamp)(iSwapChain,pRamp);
		}
	}	
		
	PROXYSTDMETHOD(CreateTexture)(IDirect3DDevice9* pDevice, UINT Width,UINT Height,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DTexture9** ppTexture,HANDLE* pSharedHandle)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->CreateTexture(Width,Height,Levels,Usage,Format,Pool,ppTexture,pSharedHandle);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.CreateTexture(Width,Height,Levels,Usage,Format,Pool,ppTexture,pSharedHandle);
		}
		else
		{
			DECLAREPOINTER(pDevice, CreateTexture);
			HRESULT hResult = CALLFORPOINTER(pDevice, CreateTexture)(Width,Height,Levels,Usage,Format,Pool,ppTexture,pSharedHandle);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(CreateVolumeTexture)(IDirect3DDevice9* pDevice, UINT Width,UINT Height,UINT Depth,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DVolumeTexture9** ppVolumeTexture,HANDLE* pSharedHandle)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->CreateVolumeTexture(Width,Height,Depth,Levels,Usage,Format,Pool,ppVolumeTexture,pSharedHandle);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.CreateVolumeTexture(Width,Height,Depth,Levels,Usage,Format,Pool,ppVolumeTexture,pSharedHandle);
		}
		else
		{
			DECLAREPOINTER(pDevice, CreateVolumeTexture);
			HRESULT hResult = CALLFORPOINTER(pDevice, CreateVolumeTexture)(Width,Height,Depth,Levels,Usage,Format,Pool,ppVolumeTexture,pSharedHandle);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(CreateCubeTexture)(IDirect3DDevice9* pDevice, UINT EdgeLength,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DCubeTexture9** ppCubeTexture,HANDLE* pSharedHandle)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->CreateCubeTexture(EdgeLength,Levels,Usage,Format,Pool,ppCubeTexture,pSharedHandle);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.CreateCubeTexture(EdgeLength,Levels,Usage,Format,Pool,ppCubeTexture,pSharedHandle);
		}
		else
		{
			DECLAREPOINTER(pDevice, CreateCubeTexture);
			HRESULT hResult = CALLFORPOINTER(pDevice, CreateCubeTexture)(EdgeLength,Levels,Usage,Format,Pool,ppCubeTexture,pSharedHandle);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(CreateVertexBuffer)(IDirect3DDevice9* pDevice, UINT Length,DWORD Usage,DWORD FVF,D3DPOOL Pool,IDirect3DVertexBuffer9** ppVertexBuffer,HANDLE* pSharedHandle)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->CreateVertexBuffer(Length,Usage,FVF,Pool,ppVertexBuffer,pSharedHandle);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.CreateVertexBuffer(Length,Usage,FVF,Pool,ppVertexBuffer,pSharedHandle);
		}
		else
		{
			DECLAREPOINTER(pDevice, CreateVertexBuffer);
			HRESULT hResult = CALLFORPOINTER(pDevice, CreateVertexBuffer)(Length,Usage,FVF,Pool,ppVertexBuffer,pSharedHandle);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(CreateIndexBuffer)(IDirect3DDevice9* pDevice, UINT Length,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DIndexBuffer9** ppIndexBuffer,HANDLE* pSharedHandle)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->CreateIndexBuffer(Length,Usage,Format,Pool,ppIndexBuffer,pSharedHandle);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.CreateIndexBuffer(Length,Usage,Format,Pool,ppIndexBuffer,pSharedHandle);
		}
		else
		{
			DECLAREPOINTER(pDevice, CreateIndexBuffer);
			HRESULT hResult = CALLFORPOINTER(pDevice, CreateIndexBuffer)(Length,Usage,Format,Pool,ppIndexBuffer,pSharedHandle);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(CreateRenderTarget)(IDirect3DDevice9* pDevice, UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Lockable,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->CreateRenderTarget(Width,Height,Format,MultiSample,MultisampleQuality,Lockable,ppSurface,pSharedHandle);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.CreateRenderTarget(Width,Height,Format,MultiSample,MultisampleQuality,Lockable,ppSurface,pSharedHandle);
		}
		else
		{
			DECLAREPOINTER(pDevice, CreateRenderTarget);
			HRESULT hResult = CALLFORPOINTER(pDevice, CreateRenderTarget)(Width,Height,Format,MultiSample,MultisampleQuality,Lockable,ppSurface,pSharedHandle);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(CreateDepthStencilSurface)(IDirect3DDevice9* pDevice, UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Discard,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->CreateDepthStencilSurface(Width,Height,Format,MultiSample,MultisampleQuality,Discard,ppSurface,pSharedHandle);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.CreateDepthStencilSurface(Width,Height,Format,MultiSample,MultisampleQuality,Discard,ppSurface,pSharedHandle);
		}
		else
		{
			DECLAREPOINTER(pDevice, CreateDepthStencilSurface);
			HRESULT hResult = CALLFORPOINTER(pDevice, CreateDepthStencilSurface)(Width,Height,Format,MultiSample,MultisampleQuality,Discard,ppSurface,pSharedHandle);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(UpdateSurface)(IDirect3DDevice9* pDevice, IDirect3DSurface9* pSourceSurface,CONST RECT* pSourceRect,IDirect3DSurface9* pDestinationSurface,CONST POINT* pDestPoint)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->UpdateSurface(pSourceSurface,pSourceRect,pDestinationSurface,pDestPoint);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.UpdateSurface(pSourceSurface,pSourceRect,pDestinationSurface,pDestPoint);
		}
		else
		{
			DECLAREPOINTER(pDevice, UpdateSurface);
			HRESULT hResult = CALLFORPOINTER(pDevice, UpdateSurface)(pSourceSurface,pSourceRect,pDestinationSurface,pDestPoint);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(UpdateTexture)(IDirect3DDevice9* pDevice, IDirect3DBaseTexture9* pSourceTexture,IDirect3DBaseTexture9* pDestinationTexture)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->UpdateTexture(pSourceTexture,pDestinationTexture);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.UpdateTexture(pSourceTexture,pDestinationTexture);
		}
		else
		{
			DECLAREPOINTER(pDevice, UpdateTexture);
			HRESULT hResult = CALLFORPOINTER(pDevice, UpdateTexture)(pSourceTexture,pDestinationTexture);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(GetRenderTargetData)(IDirect3DDevice9* pDevice, IDirect3DSurface9* pRenderTarget,IDirect3DSurface9* pDestSurface)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->GetRenderTargetData(pRenderTarget,pDestSurface);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.GetRenderTargetData(pRenderTarget,pDestSurface);
		}
		else
		{
			DECLAREPOINTER(pDevice, GetRenderTargetData);
			HRESULT hResult = CALLFORPOINTER(pDevice, GetRenderTargetData)(pRenderTarget,pDestSurface);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(GetFrontBufferData)(IDirect3DDevice9* pDevice, UINT iSwapChain,IDirect3DSurface9* pDestSurface)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->GetFrontBufferData(iSwapChain,pDestSurface);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.GetFrontBufferData(iSwapChain,pDestSurface);
		}
		else
		{
			DECLAREPOINTER(pDevice, GetFrontBufferData);
			HRESULT hResult = CALLFORPOINTER(pDevice, GetFrontBufferData)(iSwapChain,pDestSurface);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(StretchRect)(IDirect3DDevice9* pDevice, IDirect3DSurface9* pSourceSurface,CONST RECT* pSourceRect,IDirect3DSurface9* pDestSurface,CONST RECT* pDestRect,D3DTEXTUREFILTERTYPE Filter)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->StretchRect(pSourceSurface,pSourceRect,pDestSurface,pDestRect,Filter);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.StretchRect(pSourceSurface,pSourceRect,pDestSurface,pDestRect,Filter);
		}
		else
		{
			DECLAREPOINTER(pDevice, StretchRect);
			HRESULT hResult = CALLFORPOINTER(pDevice, StretchRect)(pSourceSurface,pSourceRect,pDestSurface,pDestRect,Filter);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(ColorFill)(IDirect3DDevice9* pDevice, IDirect3DSurface9* pSurface,CONST RECT* pRect,D3DCOLOR color)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->ColorFill(pSurface,pRect,color);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.ColorFill(pSurface,pRect,color);
		}
		else
		{
			DECLAREPOINTER(pDevice, ColorFill);
			HRESULT hResult = CALLFORPOINTER(pDevice, ColorFill)(pSurface,pRect,color);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(CreateOffscreenPlainSurface)(IDirect3DDevice9* pDevice, UINT Width,UINT Height,D3DFORMAT Format,D3DPOOL Pool,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->CreateOffscreenPlainSurface(Width,Height,Format,Pool,ppSurface,pSharedHandle);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.CreateOffscreenPlainSurface(Width,Height,Format,Pool,ppSurface,pSharedHandle);
		}
		else
		{
			DECLAREPOINTER(pDevice, CreateOffscreenPlainSurface);
			HRESULT hResult = CALLFORPOINTER(pDevice, CreateOffscreenPlainSurface)(Width,Height,Format,Pool,ppSurface,pSharedHandle);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(SetRenderTarget)(IDirect3DDevice9* pDevice, DWORD RenderTargetIndex,IDirect3DSurface9* pRenderTarget)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->SetRenderTarget(RenderTargetIndex,pRenderTarget);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.SetRenderTarget(RenderTargetIndex,pRenderTarget);
		}
		else
		{
			DECLAREPOINTER(pDevice, SetRenderTarget);
			HRESULT hResult = CALLFORPOINTER(pDevice, SetRenderTarget)(RenderTargetIndex,pRenderTarget);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(GetRenderTarget)(IDirect3DDevice9* pDevice, DWORD RenderTargetIndex,IDirect3DSurface9** ppRenderTarget)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->GetRenderTarget(RenderTargetIndex,ppRenderTarget);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.GetRenderTarget(RenderTargetIndex,ppRenderTarget);
		}
		else
		{
			DECLAREPOINTER(pDevice, GetRenderTarget);
			HRESULT hResult = CALLFORPOINTER(pDevice, GetRenderTarget)(RenderTargetIndex,ppRenderTarget);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(SetDepthStencilSurface)(IDirect3DDevice9* pDevice, IDirect3DSurface9* pNewZStencil)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->SetDepthStencilSurface(pNewZStencil);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.SetDepthStencilSurface(pNewZStencil);
		}
		else
		{
			DECLAREPOINTER(pDevice, SetDepthStencilSurface);
			HRESULT hResult = CALLFORPOINTER(pDevice, SetDepthStencilSurface)(pNewZStencil);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(GetDepthStencilSurface)(IDirect3DDevice9* pDevice, IDirect3DSurface9** ppZStencilSurface)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->GetDepthStencilSurface(ppZStencilSurface);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.GetDepthStencilSurface(ppZStencilSurface);
		}
		else
		{
			DECLAREPOINTER(pDevice, GetDepthStencilSurface);
			HRESULT hResult = CALLFORPOINTER(pDevice, GetDepthStencilSurface)(ppZStencilSurface);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(BeginScene)(IDirect3DDevice9* pDevice)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->BeginScene();
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.BeginScene();
		}
		else
		{
			DECLAREPOINTER(pDevice, BeginScene);
			HRESULT hResult = CALLFORPOINTER(pDevice, BeginScene)();
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(EndScene)(IDirect3DDevice9* pDevice)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->EndScene();
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.EndScene();
		}
		else
		{
			DECLAREPOINTER(pDevice, EndScene);
			HRESULT hResult = CALLFORPOINTER(pDevice, EndScene)();
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(Clear)(IDirect3DDevice9* pDevice, DWORD Count,CONST D3DRECT* pRects,DWORD Flags,D3DCOLOR Color,float Z,DWORD Stencil)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->Clear(Count,pRects,Flags,Color,Z,Stencil);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.Clear(Count,pRects,Flags,Color,Z,Stencil);
		}
		else
		{
			DECLAREPOINTER(pDevice, Clear);
			HRESULT hResult = CALLFORPOINTER(pDevice, Clear)(Count,pRects,Flags,Color,Z,Stencil);
			return hResult;
		}
	}	
	/*	
	PROXYSTDMETHOD(SetTransform)(DIDirect3DDevice9* pDevice, 3DTRANSFORMSTATETYPE State,CONST D3DMATRIX* pMatrix)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->SetTransform(State, pMatrix);
			else
				return pCurrentStereoRenderer->m_pDirect3DDevice.SetTransform(State, pMatrix);
		}
		else
		{
			DECLAREPOINTER(pDevice, SetTransform);
			HRESULT hResult = CALLFORPOINTER(pDevice, SetTransform)(State, pMatrix);
			return hResult;
		}
	}	
	*/
	PROXYSTDMETHOD(GetTransform)(IDirect3DDevice9* pDevice, D3DTRANSFORMSTATETYPE State,D3DMATRIX* pMatrix)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->GetTransform(State,pMatrix);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.GetTransform(State,pMatrix);
		}
		else
		{
			DECLAREPOINTER(pDevice, GetTransform);
			HRESULT hResult = CALLFORPOINTER(pDevice, GetTransform)(State,pMatrix);
			return hResult;
		}
	}	
	/*	
	PROXYSTDMETHOD(MultiplyTransform)(DIDirect3DDevice9* pDevice, 3DTRANSFORMSTATETYPE,CONST D3DMATRIX*)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->
			else
				return pCurrentStereoRenderer->m_pDirect3DDevice.
		}
		else
		{
			DECLAREPOINTER(pDevice, );
			HRESULT hResult = CALLFORPOINTER(pDevice, );
			return hResult;
		}
	}	
	*/
	/*
	PROXYSTDMETHOD(SetViewport)(CIDirect3DDevice9* pDevice, ONST D3DVIEWPORT9* pViewport)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->
			else
				return pCurrentStereoRenderer->m_pDirect3DDevice.
		}
		else
		{
			DECLAREPOINTER(pDevice, );
			HRESULT hResult = CALLFORPOINTER(pDevice, );
			return hResult;
		}
	}	
	*/	
	PROXYSTDMETHOD(GetViewport)(IDirect3DDevice9* pDevice, D3DVIEWPORT9* pViewport)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->GetViewport(pViewport);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.GetViewport(pViewport);
		}
		else
		{
			DECLAREPOINTER(pDevice, GetViewport);
			HRESULT hResult = CALLFORPOINTER(pDevice, GetViewport)(pViewport);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(SetMaterial)(IDirect3DDevice9* pDevice, CONST D3DMATERIAL9* pMaterial)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->SetMaterial(pMaterial);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.SetMaterial(pMaterial);
		}
		else
		{
			DECLAREPOINTER(pDevice, SetMaterial);
			HRESULT hResult = CALLFORPOINTER(pDevice, SetMaterial)(pMaterial);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(GetMaterial)(IDirect3DDevice9* pDevice, D3DMATERIAL9* pMaterial)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->GetMaterial(pMaterial);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.GetMaterial(pMaterial);
		}
		else
		{
			DECLAREPOINTER(pDevice, GetMaterial);
			HRESULT hResult = CALLFORPOINTER(pDevice, GetMaterial)(pMaterial);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(SetLight)(IDirect3DDevice9* pDevice, DWORD Index,CONST D3DLIGHT9* pLight)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->SetLight(Index,pLight);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.SetLight(Index,pLight);
		}
		else
		{
			DECLAREPOINTER(pDevice, SetLight);
			HRESULT hResult = CALLFORPOINTER(pDevice, SetLight)(Index,pLight);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(GetLight)(IDirect3DDevice9* pDevice, DWORD Index,D3DLIGHT9* pLight)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->GetLight(Index,pLight);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.GetLight(Index,pLight);
		}
		else
		{
			DECLAREPOINTER(pDevice, GetLight);
			HRESULT hResult = CALLFORPOINTER(pDevice, GetLight)(Index,pLight);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(LightEnable)(IDirect3DDevice9* pDevice, DWORD Index,BOOL Enable)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->LightEnable(Index,Enable);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.LightEnable(Index,Enable);
		}
		else
		{
			DECLAREPOINTER(pDevice, LightEnable);
			HRESULT hResult = CALLFORPOINTER(pDevice, LightEnable)(Index,Enable);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(GetLightEnable)(IDirect3DDevice9* pDevice, DWORD Index,BOOL* pEnable)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->GetLightEnable(Index,pEnable);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.GetLightEnable(Index,pEnable);
		}
		else
		{
			DECLAREPOINTER(pDevice, GetLightEnable);
			HRESULT hResult = CALLFORPOINTER(pDevice, GetLightEnable)(Index,pEnable);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(SetClipPlane)(IDirect3DDevice9* pDevice, DWORD Index,CONST float* pPlane)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->SetClipPlane(Index,pPlane);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.SetClipPlane(Index,pPlane);
		}
		else
		{
			DECLAREPOINTER(pDevice, SetClipPlane);
			HRESULT hResult = CALLFORPOINTER(pDevice, SetClipPlane)(Index,pPlane);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(GetClipPlane)(IDirect3DDevice9* pDevice, DWORD Index,float* pPlane)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->GetClipPlane(Index,pPlane);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.GetClipPlane(Index,pPlane);
		}
		else
		{
			DECLAREPOINTER(pDevice, GetClipPlane);
			HRESULT hResult = CALLFORPOINTER(pDevice, GetClipPlane)(Index,pPlane);
			return hResult;
		}
	}	
	/*	
	PROXYSTDMETHOD(SetRenderState)(DIDirect3DDevice9* pDevice, 3DRENDERSTATETYPE State,DWORD Value)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->
			else
				return pCurrentStereoRenderer->m_pDirect3DDevice.
		}
		else
		{
			DECLAREPOINTER(pDevice, );
			HRESULT hResult = CALLFORPOINTER(pDevice, );
			return hResult;
		}
	}	
	*/	
	PROXYSTDMETHOD(GetRenderState)(IDirect3DDevice9* pDevice, D3DRENDERSTATETYPE State,DWORD* pValue)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->GetRenderState(State,pValue);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.GetRenderState(State,pValue);
		}
		else
		{
			DECLAREPOINTER(pDevice, GetRenderState);
			HRESULT hResult = CALLFORPOINTER(pDevice, GetRenderState)(State,pValue);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(CreateStateBlock)(IDirect3DDevice9* pDevice, D3DSTATEBLOCKTYPE Type,IDirect3DStateBlock9** ppSB)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->CreateStateBlock(Type,ppSB);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.CreateStateBlock(Type,ppSB);
		}
		else
		{
			DECLAREPOINTER(pDevice, CreateStateBlock);
			HRESULT hResult = CALLFORPOINTER(pDevice, CreateStateBlock)(Type,ppSB);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(BeginStateBlock)(IDirect3DDevice9* pDevice)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->BeginStateBlock();
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.BeginStateBlock();
		}
		else
		{
			DECLAREPOINTER(pDevice, BeginStateBlock);
			HRESULT hResult = CALLFORPOINTER(pDevice, BeginStateBlock)();
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(EndStateBlock)(IDirect3DDevice9* pDevice, IDirect3DStateBlock9** ppSB)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->EndStateBlock(ppSB);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.EndStateBlock(ppSB);
		}
		else
		{
			DECLAREPOINTER(pDevice, EndStateBlock);
			HRESULT hResult = CALLFORPOINTER(pDevice, EndStateBlock)(ppSB);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(SetClipStatus)(IDirect3DDevice9* pDevice, CONST D3DCLIPSTATUS9* pClipStatus)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->SetClipStatus(pClipStatus);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.SetClipStatus(pClipStatus);
		}
		else
		{
			DECLAREPOINTER(pDevice, SetClipStatus);
			HRESULT hResult = CALLFORPOINTER(pDevice, SetClipStatus)(pClipStatus);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(GetClipStatus)(IDirect3DDevice9* pDevice, D3DCLIPSTATUS9* pClipStatus)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->GetClipStatus(pClipStatus);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.GetClipStatus(pClipStatus);
		}
		else
		{
			DECLAREPOINTER(pDevice, GetClipStatus);
			HRESULT hResult = CALLFORPOINTER(pDevice, GetClipStatus)(pClipStatus);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(GetTexture)(IDirect3DDevice9* pDevice, DWORD Stage,IDirect3DBaseTexture9** ppTexture)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->GetTexture(Stage,ppTexture);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.GetTexture(Stage,ppTexture);
		}
		else
		{
			DECLAREPOINTER(pDevice, GetTexture);
			HRESULT hResult = CALLFORPOINTER(pDevice, GetTexture)(Stage,ppTexture);
			return hResult;
		}
	}	
	/*	
	PROXYSTDMETHOD(SetTexture)(DIDirect3DDevice9* pDevice, WORD Stage,IDirect3DBaseTexture9* pTexture)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->
			else
				return pCurrentStereoRenderer->m_pDirect3DDevice.
		}
		else
		{
			DECLAREPOINTER(pDevice, );
			HRESULT hResult = CALLFORPOINTER(pDevice, );
			return hResult;
		}
	}	
	*/	
	PROXYSTDMETHOD(GetTextureStageState)(IDirect3DDevice9* pDevice, DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD* pValue)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->GetTextureStageState(Stage,Type,pValue);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.GetTextureStageState(Stage,Type,pValue);
		}
		else
		{
			DECLAREPOINTER(pDevice, GetTextureStageState);
			HRESULT hResult = CALLFORPOINTER(pDevice, GetTextureStageState)(Stage,Type,pValue);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(SetTextureStageState)(IDirect3DDevice9* pDevice, DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD Value)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->SetTextureStageState(Stage,Type,Value);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.SetTextureStageState(Stage,Type,Value);
		}
		else
		{
			DECLAREPOINTER(pDevice, SetTextureStageState);
			HRESULT hResult = CALLFORPOINTER(pDevice, SetTextureStageState)(Stage,Type,Value);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(GetSamplerState)(IDirect3DDevice9* pDevice, DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD* pValue)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->GetSamplerState(Sampler,Type,pValue);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.GetSamplerState(Sampler,Type,pValue);
		}
		else
		{
			DECLAREPOINTER(pDevice, GetSamplerState);
			HRESULT hResult = CALLFORPOINTER(pDevice, GetSamplerState)(Sampler,Type,pValue);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(SetSamplerState)(IDirect3DDevice9* pDevice, DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD Value)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->SetSamplerState(Sampler,Type,Value);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.SetSamplerState(Sampler,Type,Value);
		}
		else
		{
			DECLAREPOINTER(pDevice, SetSamplerState);
			HRESULT hResult = CALLFORPOINTER(pDevice, SetSamplerState)(Sampler,Type,Value);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(ValidateDevice)(IDirect3DDevice9* pDevice, DWORD* pNumPasses)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->ValidateDevice(pNumPasses);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.ValidateDevice(pNumPasses);
		}
		else
		{
			DECLAREPOINTER(pDevice, ValidateDevice);
			HRESULT hResult = CALLFORPOINTER(pDevice, ValidateDevice)(pNumPasses);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(SetPaletteEntries)(IDirect3DDevice9* pDevice, UINT PaletteNumber,CONST PALETTEENTRY* pEntries)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->SetPaletteEntries(PaletteNumber,pEntries);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.SetPaletteEntries(PaletteNumber,pEntries);
		}
		else
		{
			DECLAREPOINTER(pDevice, SetPaletteEntries);
			HRESULT hResult = CALLFORPOINTER(pDevice, SetPaletteEntries)(PaletteNumber,pEntries);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(GetPaletteEntries)(IDirect3DDevice9* pDevice, UINT PaletteNumber,PALETTEENTRY* pEntries)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->GetPaletteEntries(PaletteNumber,pEntries);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.GetPaletteEntries(PaletteNumber,pEntries);
		}
		else
		{
			DECLAREPOINTER(pDevice, GetPaletteEntries);
			HRESULT hResult = CALLFORPOINTER(pDevice, GetPaletteEntries)(PaletteNumber,pEntries);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(SetCurrentTexturePalette)(IDirect3DDevice9* pDevice, UINT PaletteNumber)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->SetCurrentTexturePalette(PaletteNumber);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.SetCurrentTexturePalette(PaletteNumber);
		}
		else
		{
			DECLAREPOINTER(pDevice, SetCurrentTexturePalette);
			HRESULT hResult = CALLFORPOINTER(pDevice, SetCurrentTexturePalette)(PaletteNumber);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(GetCurrentTexturePalette)(IDirect3DDevice9* pDevice, UINT *PaletteNumber)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->GetCurrentTexturePalette(PaletteNumber);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.GetCurrentTexturePalette(PaletteNumber);
		}
		else
		{
			DECLAREPOINTER(pDevice, GetCurrentTexturePalette);
			HRESULT hResult = CALLFORPOINTER(pDevice, GetCurrentTexturePalette)(PaletteNumber);
			return hResult;
		}
	}	
	/*	
	PROXYSTDMETHOD(SetScissorRect)(CIDirect3DDevice9* pDevice, ONST RECT* pRect)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->
			else
				return pCurrentStereoRenderer->m_pDirect3DDevice.
		}
		else
		{
			DECLAREPOINTER(pDevice, );
			HRESULT hResult = CALLFORPOINTER(pDevice, );
			return hResult;
		}
	}	
	*/	
	PROXYSTDMETHOD(GetScissorRect)(IDirect3DDevice9* pDevice, RECT* pRect)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->GetScissorRect(pRect);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.GetScissorRect(pRect);
		}
		else
		{
			DECLAREPOINTER(pDevice, GetScissorRect);
			HRESULT hResult = CALLFORPOINTER(pDevice, GetScissorRect)(pRect);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(SetSoftwareVertexProcessing)(IDirect3DDevice9* pDevice, BOOL bSoftware)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->SetSoftwareVertexProcessing(bSoftware);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.SetSoftwareVertexProcessing(bSoftware);
		}
		else
		{
			DECLAREPOINTER(pDevice, SetSoftwareVertexProcessing);
			HRESULT hResult = CALLFORPOINTER(pDevice, SetSoftwareVertexProcessing)(bSoftware);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD_(BOOL, GetSoftwareVertexProcessing)(IDirect3DDevice9* pDevice)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->GetSoftwareVertexProcessing();
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.GetSoftwareVertexProcessing();
		}
		else
		{
			DECLAREPOINTER(pDevice, GetSoftwareVertexProcessing);
			BOOL flag = CALLFORPOINTER(pDevice, GetSoftwareVertexProcessing)();
			return flag;
		}
	}	
		
	PROXYSTDMETHOD(SetNPatchMode)(IDirect3DDevice9* pDevice, float nSegments)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->SetNPatchMode(nSegments);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.SetNPatchMode(nSegments);
		}
		else
		{
			DECLAREPOINTER(pDevice, SetNPatchMode);
			HRESULT hResult = CALLFORPOINTER(pDevice, SetNPatchMode)(nSegments);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD_(float, GetNPatchMode)(IDirect3DDevice9* pDevice)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->GetNPatchMode();
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.GetNPatchMode();
		}
		else
		{
			DECLAREPOINTER(pDevice, GetNPatchMode);
			float mode = CALLFORPOINTER(pDevice, GetNPatchMode)();
			return mode;
		}
	}	
		
	PROXYSTDMETHOD(DrawPrimitive)(IDirect3DDevice9* pDevice, D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->DrawPrimitive(PrimitiveType,StartVertex,PrimitiveCount);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.DrawPrimitive(PrimitiveType,StartVertex,PrimitiveCount);
		}
		else
		{
			DECLAREPOINTER(pDevice, DrawPrimitive);
			HRESULT hResult = CALLFORPOINTER(pDevice, DrawPrimitive)(PrimitiveType,StartVertex,PrimitiveCount);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(DrawIndexedPrimitive)(IDirect3DDevice9* pDevice, D3DPRIMITIVETYPE PrimitiveType,INT BaseVertexIndex,UINT MinVertexIndex,UINT NumVertices,UINT startIndex,UINT primCount)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->DrawIndexedPrimitive(PrimitiveType,BaseVertexIndex,MinVertexIndex,NumVertices,startIndex,primCount);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.DrawIndexedPrimitive(PrimitiveType,BaseVertexIndex,MinVertexIndex,NumVertices,startIndex,primCount);
		}
		else
		{
			DECLAREPOINTER(pDevice, DrawIndexedPrimitive);
			HRESULT hResult = CALLFORPOINTER(pDevice, DrawIndexedPrimitive)(PrimitiveType,BaseVertexIndex,MinVertexIndex,NumVertices,startIndex,primCount);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(DrawPrimitiveUP)(IDirect3DDevice9* pDevice, D3DPRIMITIVETYPE PrimitiveType,UINT PrimitiveCount,CONST void* pVertexStreamZeroData,UINT VertexStreamZeroStride)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->DrawPrimitiveUP(PrimitiveType,PrimitiveCount,pVertexStreamZeroData,VertexStreamZeroStride);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.DrawPrimitiveUP(PrimitiveType,PrimitiveCount,pVertexStreamZeroData,VertexStreamZeroStride);
		}
		else
		{
			DECLAREPOINTER(pDevice, DrawPrimitiveUP);
			HRESULT hResult = CALLFORPOINTER(pDevice, DrawPrimitiveUP)(PrimitiveType,PrimitiveCount,pVertexStreamZeroData,VertexStreamZeroStride);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(DrawIndexedPrimitiveUP)(IDirect3DDevice9* pDevice, D3DPRIMITIVETYPE PrimitiveType,UINT MinVertexIndex,UINT NumVertices,UINT PrimitiveCount,CONST void* pIndexData,D3DFORMAT IndexDataFormat,CONST void* pVertexStreamZeroData,UINT VertexStreamZeroStride)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->DrawIndexedPrimitiveUP(PrimitiveType,MinVertexIndex,NumVertices,PrimitiveCount,pIndexData,IndexDataFormat,pVertexStreamZeroData,VertexStreamZeroStride);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.DrawIndexedPrimitiveUP(PrimitiveType,MinVertexIndex,NumVertices,PrimitiveCount,pIndexData,IndexDataFormat,pVertexStreamZeroData,VertexStreamZeroStride);
		}
		else
		{
			DECLAREPOINTER(pDevice, DrawIndexedPrimitiveUP);
			HRESULT hResult = CALLFORPOINTER(pDevice, DrawIndexedPrimitiveUP)(PrimitiveType,MinVertexIndex,NumVertices,PrimitiveCount,pIndexData,IndexDataFormat,pVertexStreamZeroData,VertexStreamZeroStride);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(ProcessVertices)(IDirect3DDevice9* pDevice, UINT SrcStartIndex,UINT DestIndex,UINT VertexCount,IDirect3DVertexBuffer9* pDestBuffer,IDirect3DVertexDeclaration9* pVertexDecl,DWORD Flags)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->ProcessVertices(SrcStartIndex,DestIndex,VertexCount,pDestBuffer,pVertexDecl,Flags);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.ProcessVertices(SrcStartIndex,DestIndex,VertexCount,pDestBuffer,pVertexDecl,Flags);
		}
		else
		{
			DECLAREPOINTER(pDevice, ProcessVertices);
			HRESULT hResult = CALLFORPOINTER(pDevice, ProcessVertices)(SrcStartIndex,DestIndex,VertexCount,pDestBuffer,pVertexDecl,Flags);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(CreateVertexDeclaration)(IDirect3DDevice9* pDevice, CONST D3DVERTEXELEMENT9* pVertexElements,IDirect3DVertexDeclaration9** ppDecl)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->CreateVertexDeclaration(pVertexElements,ppDecl);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.CreateVertexDeclaration(pVertexElements,ppDecl);
		}
		else
		{
			DECLAREPOINTER(pDevice, CreateVertexDeclaration);
			HRESULT hResult = CALLFORPOINTER(pDevice, CreateVertexDeclaration)(pVertexElements,ppDecl);
			return hResult;
		}
	}	
	/*	
	PROXYSTDMETHOD(SetVertexDeclaration)(IDirect3DDevice9* pDevice, Direct3DVertexDeclaration9* pDecl)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->
			else
				return pCurrentStereoRenderer->m_pDirect3DDevice.
		}
		else
		{
			DECLAREPOINTER(pDevice, );
			HRESULT hResult = CALLFORPOINTER(pDevice, );
			return hResult;
		}
	}	
	*/	
	PROXYSTDMETHOD(GetVertexDeclaration)(IDirect3DDevice9* pDevice, IDirect3DVertexDeclaration9** ppDecl)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->GetVertexDeclaration(ppDecl);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.GetVertexDeclaration(ppDecl);
		}
		else
		{
			DECLAREPOINTER(pDevice, GetVertexDeclaration);
			HRESULT hResult = CALLFORPOINTER(pDevice, GetVertexDeclaration)(ppDecl);
			return hResult;
		}
	}	
	/*	
	PROXYSTDMETHOD(SetFVF)(DIDirect3DDevice9* pDevice, WORD FVF)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->
			else
				return pCurrentStereoRenderer->m_pDirect3DDevice.
		}
		else
		{
			DECLAREPOINTER(pDevice, );
			HRESULT hResult = CALLFORPOINTER(pDevice, );
			return hResult;
		}
	}	
	*/	
	PROXYSTDMETHOD(GetFVF)(IDirect3DDevice9* pDevice, DWORD* pFVF)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->GetFVF(pFVF);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.GetFVF(pFVF);
		}
		else
		{
			DECLAREPOINTER(pDevice, GetFVF);
			HRESULT hResult = CALLFORPOINTER(pDevice, GetFVF)(pFVF);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(CreateVertexShader)(IDirect3DDevice9* pDevice, CONST DWORD* pFunction,IDirect3DVertexShader9** ppShader)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->CreateVertexShader(pFunction,ppShader);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.CreateVertexShader(pFunction,ppShader);
		}
		else
		{
			DECLAREPOINTER(pDevice, CreateVertexShader);
			HRESULT hResult = CALLFORPOINTER(pDevice, CreateVertexShader)(pFunction,ppShader);
			return hResult;
		}
	}	
	/*	
	PROXYSTDMETHOD(SetVertexShader)(IIDirect3DDevice9* pDevice, Direct3DVertexShader9* pShader)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->
			else
				return pCurrentStereoRenderer->m_pDirect3DDevice.
		}
		else
		{
			DECLAREPOINTER(pDevice, );
			HRESULT hResult = CALLFORPOINTER(pDevice, );
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(GetVertexShader)(IIDirect3DDevice9* pDevice, Direct3DVertexShader9** ppShader)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->
			else
				return pCurrentStereoRenderer->m_pDirect3DDevice.
		}
		else
		{
			DECLAREPOINTER(pDevice, );
			HRESULT hResult = CALLFORPOINTER(pDevice, );
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(SetVertexShaderConstantF)(UIDirect3DDevice9* pDevice, INT StartRegister,CONST float* pConstantData,UINT Vector4fCount)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->
			else
				return pCurrentStereoRenderer->m_pDirect3DDevice.
		}
		else
		{
			DECLAREPOINTER(pDevice, );
			HRESULT hResult = CALLFORPOINTER(pDevice, );
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(GetVertexShaderConstantF)(UIDirect3DDevice9* pDevice, INT StartRegister,float* pConstantData,UINT Vector4fCount)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->
			else
				return pCurrentStereoRenderer->m_pDirect3DDevice.
		}
		else
		{
			DECLAREPOINTER(pDevice, );
			HRESULT hResult = CALLFORPOINTER(pDevice, );
			return hResult;
		}
	}	
	*/	
	PROXYSTDMETHOD(SetVertexShaderConstantI)(IDirect3DDevice9* pDevice, UINT StartRegister,CONST int* pConstantData,UINT Vector4iCount)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->SetVertexShaderConstantI(StartRegister,pConstantData,Vector4iCount);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.SetVertexShaderConstantI(StartRegister,pConstantData,Vector4iCount);
		}
		else
		{
			DECLAREPOINTER(pDevice, SetVertexShaderConstantI);
			HRESULT hResult = CALLFORPOINTER(pDevice, SetVertexShaderConstantI)(StartRegister,pConstantData,Vector4iCount);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(GetVertexShaderConstantI)(IDirect3DDevice9* pDevice, UINT StartRegister,int* pConstantData,UINT Vector4iCount)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->GetVertexShaderConstantI(StartRegister,pConstantData,Vector4iCount);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.GetVertexShaderConstantI(StartRegister,pConstantData,Vector4iCount);
		}
		else
		{
			DECLAREPOINTER(pDevice, GetVertexShaderConstantI);
			HRESULT hResult = CALLFORPOINTER(pDevice, GetVertexShaderConstantI)(StartRegister,pConstantData,Vector4iCount);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(SetVertexShaderConstantB)(IDirect3DDevice9* pDevice, UINT StartRegister,CONST BOOL* pConstantData,UINT  BoolCount)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->SetVertexShaderConstantB(StartRegister,pConstantData,BoolCount);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.SetVertexShaderConstantB(StartRegister,pConstantData,BoolCount);
		}
		else
		{
			DECLAREPOINTER(pDevice, SetVertexShaderConstantB);
			HRESULT hResult = CALLFORPOINTER(pDevice, SetVertexShaderConstantB)(StartRegister,pConstantData,BoolCount);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(GetVertexShaderConstantB)(IDirect3DDevice9* pDevice, UINT StartRegister,BOOL* pConstantData,UINT BoolCount)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->GetVertexShaderConstantB(StartRegister,pConstantData,BoolCount);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.GetVertexShaderConstantB(StartRegister,pConstantData,BoolCount);
		}
		else
		{
			DECLAREPOINTER(pDevice, GetVertexShaderConstantB);
			HRESULT hResult = CALLFORPOINTER(pDevice, GetVertexShaderConstantB)(StartRegister,pConstantData,BoolCount);
			return hResult;
		}
	}	
	/*	
	PROXYSTDMETHOD(SetStreamSource)(UIDirect3DDevice9* pDevice, INT StreamNumber,IDirect3DVertexBuffer9* pStreamData,UINT OffsetInBytes,UINT Stride)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->
			else
				return pCurrentStereoRenderer->m_pDirect3DDevice.
		}
		else
		{
			DECLAREPOINTER(pDevice, );
			HRESULT hResult = CALLFORPOINTER(pDevice, );
			return hResult;
		}
	}	
	*/	
	PROXYSTDMETHOD(GetStreamSource)(IDirect3DDevice9* pDevice, UINT StreamNumber,IDirect3DVertexBuffer9** ppStreamData,UINT* pOffsetInBytes,UINT* pStride)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->GetStreamSource(StreamNumber,ppStreamData,pOffsetInBytes,pStride);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.GetStreamSource(StreamNumber,ppStreamData,pOffsetInBytes,pStride);
		}
		else
		{
			DECLAREPOINTER(pDevice, GetStreamSource);
			HRESULT hResult = CALLFORPOINTER(pDevice, GetStreamSource)(StreamNumber,ppStreamData,pOffsetInBytes,pStride);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(SetStreamSourceFreq)(IDirect3DDevice9* pDevice, UINT StreamNumber,UINT Setting)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->SetStreamSourceFreq(StreamNumber,Setting);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.SetStreamSourceFreq(StreamNumber,Setting);
		}
		else
		{
			DECLAREPOINTER(pDevice, SetStreamSourceFreq);
			HRESULT hResult = CALLFORPOINTER(pDevice, SetStreamSourceFreq)(StreamNumber,Setting);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(GetStreamSourceFreq)(IDirect3DDevice9* pDevice, UINT StreamNumber,UINT* pSetting)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->GetStreamSourceFreq(StreamNumber,pSetting);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.GetStreamSourceFreq(StreamNumber,pSetting);
		}
		else
		{
			DECLAREPOINTER(pDevice, GetStreamSourceFreq);
			HRESULT hResult = CALLFORPOINTER(pDevice, GetStreamSourceFreq)(StreamNumber,pSetting);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(SetIndices)(IDirect3DDevice9* pDevice, IDirect3DIndexBuffer9* pIndexData)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->SetIndices(pIndexData);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.SetIndices(pIndexData);
		}
		else
		{
			DECLAREPOINTER(pDevice, SetIndices);
			HRESULT hResult = CALLFORPOINTER(pDevice, SetIndices)(pIndexData);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(GetIndices)(IDirect3DDevice9* pDevice, IDirect3DIndexBuffer9** ppIndexData)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->GetIndices(ppIndexData);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.GetIndices(ppIndexData);
		}
		else
		{
			DECLAREPOINTER(pDevice, GetIndices);
			HRESULT hResult = CALLFORPOINTER(pDevice, GetIndices)(ppIndexData);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(CreatePixelShader)(IDirect3DDevice9* pDevice, CONST DWORD* pFunction,IDirect3DPixelShader9** ppShader)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->CreatePixelShader(pFunction,ppShader);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.CreatePixelShader(pFunction,ppShader);
		}
		else
		{
			DECLAREPOINTER(pDevice, CreatePixelShader);
			HRESULT hResult = CALLFORPOINTER(pDevice, CreatePixelShader)(pFunction,ppShader);
			return hResult;
		}
	}	
	/*	
	PROXYSTDMETHOD(SetPixelShader)(IIDirect3DDevice9* pDevice, Direct3DPixelShader9* pShader)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->
			else
				return pCurrentStereoRenderer->m_pDirect3DDevice.
		}
		else
		{
			DECLAREPOINTER(pDevice, );
			HRESULT hResult = CALLFORPOINTER(pDevice, );
			return hResult;
		}
	}	
	*/	
	PROXYSTDMETHOD(GetPixelShader)(IDirect3DDevice9* pDevice, IDirect3DPixelShader9** ppShader)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->GetPixelShader(ppShader);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.GetPixelShader(ppShader);
		}
		else
		{
			DECLAREPOINTER(pDevice, GetPixelShader);
			HRESULT hResult = CALLFORPOINTER(pDevice, GetPixelShader)(ppShader);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(SetPixelShaderConstantF)(IDirect3DDevice9* pDevice, UINT StartRegister,CONST float* pConstantData,UINT Vector4fCount)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->SetPixelShaderConstantF(StartRegister,pConstantData,Vector4fCount);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.SetPixelShaderConstantF(StartRegister,pConstantData,Vector4fCount);
		}
		else
		{
			DECLAREPOINTER(pDevice, SetPixelShaderConstantF);
			HRESULT hResult = CALLFORPOINTER(pDevice, SetPixelShaderConstantF)(StartRegister,pConstantData,Vector4fCount);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(GetPixelShaderConstantF)(IDirect3DDevice9* pDevice, UINT StartRegister,float* pConstantData,UINT Vector4fCount)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->GetPixelShaderConstantF(StartRegister,pConstantData,Vector4fCount);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.GetPixelShaderConstantF(StartRegister,pConstantData,Vector4fCount);
		}
		else
		{
			DECLAREPOINTER(pDevice, GetPixelShaderConstantF);
			HRESULT hResult = CALLFORPOINTER(pDevice, GetPixelShaderConstantF)(StartRegister,pConstantData,Vector4fCount);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(SetPixelShaderConstantI)(IDirect3DDevice9* pDevice, UINT StartRegister,CONST int* pConstantData,UINT Vector4iCount)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->SetPixelShaderConstantI(StartRegister,pConstantData,Vector4iCount);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.SetPixelShaderConstantI(StartRegister,pConstantData,Vector4iCount);
		}
		else
		{
			DECLAREPOINTER(pDevice, SetPixelShaderConstantI);
			HRESULT hResult = CALLFORPOINTER(pDevice, SetPixelShaderConstantI)(StartRegister,pConstantData,Vector4iCount);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(GetPixelShaderConstantI)(IDirect3DDevice9* pDevice, UINT StartRegister,int* pConstantData,UINT Vector4iCount)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->GetPixelShaderConstantI(StartRegister,pConstantData,Vector4iCount);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.GetPixelShaderConstantI(StartRegister,pConstantData,Vector4iCount);
		}
		else
		{
			DECLAREPOINTER(pDevice, GetPixelShaderConstantI);
			HRESULT hResult = CALLFORPOINTER(pDevice, GetPixelShaderConstantI)(StartRegister,pConstantData,Vector4iCount);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(SetPixelShaderConstantB)(IDirect3DDevice9* pDevice, UINT StartRegister,CONST BOOL* pConstantData,UINT  BoolCount)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->SetPixelShaderConstantB(StartRegister,pConstantData,BoolCount);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.SetPixelShaderConstantB(StartRegister,pConstantData,BoolCount);
		}
		else
		{
			DECLAREPOINTER(pDevice, SetPixelShaderConstantB);
			HRESULT hResult = CALLFORPOINTER(pDevice, SetPixelShaderConstantB)(StartRegister,pConstantData,BoolCount);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(GetPixelShaderConstantB)(IDirect3DDevice9* pDevice, UINT StartRegister,BOOL* pConstantData,UINT BoolCount)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->GetPixelShaderConstantB(StartRegister,pConstantData,BoolCount);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.GetPixelShaderConstantB(StartRegister,pConstantData,BoolCount);
		}
		else
		{
			DECLAREPOINTER(pDevice, GetPixelShaderConstantB);
			HRESULT hResult = CALLFORPOINTER(pDevice, GetPixelShaderConstantB)(StartRegister,pConstantData,BoolCount);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(DrawRectPatch)(IDirect3DDevice9* pDevice, UINT Handle,CONST float* pNumSegs,CONST D3DRECTPATCH_INFO* pRectPatchInfo)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->DrawRectPatch(Handle,pNumSegs,pRectPatchInfo);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.DrawRectPatch(Handle,pNumSegs,pRectPatchInfo);
		}
		else
		{
			DECLAREPOINTER(pDevice, DrawRectPatch);
			HRESULT hResult = CALLFORPOINTER(pDevice, DrawRectPatch)(Handle,pNumSegs,pRectPatchInfo);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(DrawTriPatch)(IDirect3DDevice9* pDevice, UINT Handle,CONST float* pNumSegs,CONST D3DTRIPATCH_INFO* pTriPatchInfo)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->DrawTriPatch(Handle,pNumSegs,pTriPatchInfo);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.DrawTriPatch(Handle,pNumSegs,pTriPatchInfo);
		}
		else
		{
			DECLAREPOINTER(pDevice, DrawTriPatch);
			HRESULT hResult = CALLFORPOINTER(pDevice, DrawTriPatch)(Handle,pNumSegs,pTriPatchInfo);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(DeletePatch)(IDirect3DDevice9* pDevice, UINT Handle)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->DeletePatch(Handle);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.DeletePatch(Handle);
		}
		else
		{
			DECLAREPOINTER(pDevice, DeletePatch);
			HRESULT hResult = CALLFORPOINTER(pDevice, DeletePatch)(Handle);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(CreateQuery)(IDirect3DDevice9* pDevice, D3DQUERYTYPE Type,IDirect3DQuery9** ppQuery)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->CreateQuery(Type,ppQuery);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.CreateQuery(Type,ppQuery);
		}
		else
		{
			DECLAREPOINTER(pDevice, CreateQuery);
			HRESULT hResult = CALLFORPOINTER(pDevice, CreateQuery)(Type,ppQuery);
			return hResult;
		}
	}	
		
	// IDirect3DDevice9Ex methods
	PROXYSTDMETHOD(SetConvolutionMonoKernel)(IDirect3DDevice9Ex* pDevice, UINT width,UINT height,float* rows,float* columns)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->SetConvolutionMonoKernel(width,height,rows,columns);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.SetConvolutionMonoKernel(width,height,rows,columns);
		}
		else
		{
			DECLAREPOINTER(pDevice, SetConvolutionMonoKernel);
			HRESULT hResult = CALLFORPOINTER(pDevice, SetConvolutionMonoKernel)(width,height,rows,columns);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(ComposeRects)(IDirect3DDevice9Ex* pDevice, IDirect3DSurface9* pSrc,IDirect3DSurface9* pDst,IDirect3DVertexBuffer9* pSrcRectDescs,UINT NumRects,IDirect3DVertexBuffer9* pDstRectDescs,D3DCOMPOSERECTSOP Operation,int Xoffset,int Yoffset)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->ComposeRects(pSrc,pDst,pSrcRectDescs,NumRects,pDstRectDescs,Operation,Xoffset,Yoffset);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.ComposeRects(pSrc,pDst,pSrcRectDescs,NumRects,pDstRectDescs,Operation,Xoffset,Yoffset);
		}
		else
		{
			DECLAREPOINTER(pDevice, ComposeRects);
			HRESULT hResult = CALLFORPOINTER(pDevice, ComposeRects)(pSrc,pDst,pSrcRectDescs,NumRects,pDstRectDescs,Operation,Xoffset,Yoffset);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(PresentEx)(IDirect3DDevice9Ex* pDevice, CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion,DWORD dwFlags)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->PresentEx(pSourceRect,pDestRect,hDestWindowOverride,pDirtyRegion,dwFlags);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.PresentEx(pSourceRect,pDestRect,hDestWindowOverride,pDirtyRegion,dwFlags);
		}
		else
		{
			DECLAREPOINTER(pDevice, PresentEx);
			HRESULT hResult = CALLFORPOINTER(pDevice, PresentEx)(pSourceRect,pDestRect,hDestWindowOverride,pDirtyRegion,dwFlags);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(GetGPUThreadPriority)(IDirect3DDevice9Ex* pDevice, INT* pPriority)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->GetGPUThreadPriority(pPriority);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.GetGPUThreadPriority(pPriority);
		}
		else
		{
			DECLAREPOINTER(pDevice, GetGPUThreadPriority);
			HRESULT hResult = CALLFORPOINTER(pDevice, GetGPUThreadPriority)(pPriority);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(SetGPUThreadPriority)(IDirect3DDevice9Ex* pDevice, INT Priority)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->SetGPUThreadPriority(Priority);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.SetGPUThreadPriority(Priority);
		}
		else
		{
			DECLAREPOINTER(pDevice, SetGPUThreadPriority);
			HRESULT hResult = CALLFORPOINTER(pDevice, SetGPUThreadPriority)(Priority);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(WaitForVBlank)(IDirect3DDevice9Ex* pDevice, UINT iSwapChain)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->WaitForVBlank(iSwapChain);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.WaitForVBlank(iSwapChain);
		}
		else
		{
			DECLAREPOINTER(pDevice, WaitForVBlank);
			HRESULT hResult = CALLFORPOINTER(pDevice, WaitForVBlank)(iSwapChain);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(CheckResourceResidency)(IDirect3DDevice9Ex* pDevice, IDirect3DResource9** pResourceArray,UINT32 NumResources)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->CheckResourceResidency(pResourceArray,NumResources);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.CheckResourceResidency(pResourceArray,NumResources);
		}
		else
		{
			DECLAREPOINTER(pDevice, CheckResourceResidency);
			HRESULT hResult = CALLFORPOINTER(pDevice, CheckResourceResidency)(pResourceArray,NumResources);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(SetMaximumFrameLatency)(IDirect3DDevice9Ex* pDevice, UINT MaxLatency)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->SetMaximumFrameLatency(MaxLatency);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.SetMaximumFrameLatency(MaxLatency);
		}
		else
		{
			DECLAREPOINTER(pDevice, SetMaximumFrameLatency);
			HRESULT hResult = CALLFORPOINTER(pDevice, SetMaximumFrameLatency)(MaxLatency);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(GetMaximumFrameLatency)(IDirect3DDevice9Ex* pDevice, UINT* pMaxLatency)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->GetMaximumFrameLatency(pMaxLatency);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.GetMaximumFrameLatency(pMaxLatency);
		}
		else
		{
			DECLAREPOINTER(pDevice, GetMaximumFrameLatency);
			HRESULT hResult = CALLFORPOINTER(pDevice, GetMaximumFrameLatency)(pMaxLatency);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(CheckDeviceState)(IDirect3DDevice9Ex* pDevice, HWND hDestinationWindow)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->CheckDeviceState(hDestinationWindow);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.CheckDeviceState(hDestinationWindow);
		}
		else
		{
			DECLAREPOINTER(pDevice, CheckDeviceState);
			HRESULT hResult = CALLFORPOINTER(pDevice, CheckDeviceState)(hDestinationWindow);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(CreateRenderTargetEx)(IDirect3DDevice9Ex* pDevice, UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Lockable,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle,DWORD Usage)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->CreateRenderTargetEx(Width,Height,Format,MultiSample,MultisampleQuality,Lockable,ppSurface,pSharedHandle,Usage);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.CreateRenderTargetEx(Width,Height,Format,MultiSample,MultisampleQuality,Lockable,ppSurface,pSharedHandle,Usage);
		}
		else
		{
			DECLAREPOINTER(pDevice, CreateRenderTargetEx);
			HRESULT hResult = CALLFORPOINTER(pDevice, CreateRenderTargetEx)(Width,Height,Format,MultiSample,MultisampleQuality,Lockable,ppSurface,pSharedHandle,Usage);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(CreateOffscreenPlainSurfaceEx)(IDirect3DDevice9Ex* pDevice, UINT Width,UINT Height,D3DFORMAT Format,D3DPOOL Pool,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle,DWORD Usage)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->CreateOffscreenPlainSurfaceEx(Width,Height,Format,Pool,ppSurface,pSharedHandle,Usage);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.CreateOffscreenPlainSurfaceEx(Width,Height,Format,Pool,ppSurface,pSharedHandle,Usage);
		}
		else
		{
			DECLAREPOINTER(pDevice, CreateOffscreenPlainSurfaceEx);
			HRESULT hResult = CALLFORPOINTER(pDevice, CreateOffscreenPlainSurfaceEx)(Width,Height,Format,Pool,ppSurface,pSharedHandle,Usage);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(CreateDepthStencilSurfaceEx)(IDirect3DDevice9Ex* pDevice, UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Discard,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle,DWORD Usage)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->CreateDepthStencilSurfaceEx(Width,Height,Format,MultiSample,MultisampleQuality,Discard,ppSurface,pSharedHandle,Usage);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.CreateDepthStencilSurfaceEx(Width,Height,Format,MultiSample,MultisampleQuality,Discard,ppSurface,pSharedHandle,Usage);
		}
		else
		{
			DECLAREPOINTER(pDevice, CreateDepthStencilSurfaceEx);
			HRESULT hResult = CALLFORPOINTER(pDevice, CreateDepthStencilSurfaceEx)(Width,Height,Format,MultiSample,MultisampleQuality,Discard,ppSurface,pSharedHandle,Usage);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(ResetEx)(IDirect3DDevice9Ex* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters,D3DDISPLAYMODEEX *pFullscreenDisplayMode)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->ResetEx(pPresentationParameters,pFullscreenDisplayMode);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.ResetEx(pPresentationParameters,pFullscreenDisplayMode);
		}
		else
		{
			DECLAREPOINTER(pDevice, ResetEx);
			HRESULT hResult = CALLFORPOINTER(pDevice, ResetEx)(pPresentationParameters,pFullscreenDisplayMode);
			return hResult;
		}
	}	
		
	PROXYSTDMETHOD(GetDisplayModeEx)(IDirect3DDevice9Ex* pDevice, UINT iSwapChain,D3DDISPLAYMODEEX* pMode,D3DDISPLAYROTATION* pRotation)
	{		
		CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
		if (pCurrentStereoRenderer)
		{
			if (CalledFromApp())
				return pCurrentStereoRenderer->GetDisplayModeEx(iSwapChain,pMode,pRotation);
			else
				return pCurrentStereoRenderer->m_Direct3DDevice.GetDisplayModeEx(iSwapChain,pMode,pRotation);
		}
		else
		{
			DECLAREPOINTER(pDevice, GetDisplayModeEx);
			HRESULT hResult = CALLFORPOINTER(pDevice, GetDisplayModeEx)(iSwapChain,pMode,pRotation);
			return hResult;
		}
	}	
}