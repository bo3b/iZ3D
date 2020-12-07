#pragma once

#include "Wrapper.h"
#include "D3DSwapChain.h"
#include <boost/shared_ptr.hpp>

class SetCurrentSwapChain
{
	IDXGISwapChain* pOld;
public:
	SetCurrentSwapChain(IDXGISwapChain* This) { pOld = g_pCurrentSwapChain; g_pCurrentSwapChain = This; }
	~SetCurrentSwapChain(void) { g_pCurrentSwapChain = pOld; }
};

class ScalingHook;
typedef boost::shared_ptr<ScalingHook> ScalingHookPtrT;

class DXGISwapChainWrapper:
	public CWrapper<IDXGISwapChain>,
	public CComCoClass<DXGISwapChainWrapper>
{
public:
	BEGIN_COM_MAP(DXGISwapChainWrapper)
		COM_INTERFACE_ENTRY(IDXGISwapChain)
		COM_INTERFACE_ENTRY(IWrapper)
	END_COM_MAP()

	DXGISwapChainWrapper(void);
	~DXGISwapChainWrapper(void);

	STDMETHODIMP Init(IUnknown *pReal);

	STDMETHODIMP GetParent(REFGUID riid, void **ppParent)
	{	SetCurrentSwapChain setSwapChain(m_pReal); return m_pReal->GetParent(riid, ppParent);	}
	STDMETHODIMP GetPrivateData(REFGUID Name, UINT *pDataSize, void *pData)
	{	SetCurrentSwapChain setSwapChain(m_pReal); return m_pReal->GetPrivateData(Name, pDataSize, pData);	}
	STDMETHODIMP SetPrivateData(REFGUID Name, UINT DataSize, const void *pData)
	{	SetCurrentSwapChain setSwapChain(m_pReal); return m_pReal->SetPrivateData(Name, DataSize, pData);	}
	STDMETHODIMP SetPrivateDataInterface(REFGUID Name, const IUnknown *pUnknown)
	{	SetCurrentSwapChain setSwapChain(m_pReal); return m_pReal->SetPrivateDataInterface(Name, pUnknown);	}

	STDMETHODIMP GetDevice(REFIID riid, void **ppDevice)
	{	SetCurrentSwapChain setSwapChain(m_pReal); return m_pReal->GetDevice(riid, ppDevice);	}

	STDMETHODIMP GetBuffer(UINT Buffer, REFIID riid, void **ppSurface)
	{	SetCurrentSwapChain setSwapChain(m_pReal); return m_pReal->GetBuffer(Buffer, riid, ppSurface);	}
	STDMETHODIMP GetContainingOutput(IDXGIOutput **ppOutput)
	{	SetCurrentSwapChain setSwapChain(m_pReal); return m_pReal->GetContainingOutput(ppOutput);	}
	STDMETHODIMP GetDesc(DXGI_SWAP_CHAIN_DESC *pDesc);
	STDMETHODIMP GetFrameStatistics(DXGI_FRAME_STATISTICS *pStats)
	{	SetCurrentSwapChain setSwapChain(m_pReal); return m_pReal->GetFrameStatistics(pStats);	}
	STDMETHODIMP GetFullscreenState(BOOL *pFullscreen, IDXGIOutput **ppTarget)
	{	SetCurrentSwapChain setSwapChain(m_pReal); return m_pReal->GetFullscreenState(pFullscreen, ppTarget);	}
	STDMETHODIMP GetLastPresentCount(UINT *pLastPresentCount)
	{	SetCurrentSwapChain setSwapChain(m_pReal); return m_pReal->GetLastPresentCount(pLastPresentCount);	}
	STDMETHODIMP Present(UINT SyncInterval, UINT Flags);
	STDMETHODIMP ResizeBuffers(UINT BufferCount, UINT Width, UINT Height,
		DXGI_FORMAT NewFormat, UINT SwapChainFlags);
	STDMETHODIMP ResizeTarget(const DXGI_MODE_DESC *pNewTargetParameters);

	STDMETHODIMP SetFullscreenState(BOOL Fullscreen, IDXGIOutput *pTarget);

	void setD3DSwapChain(IDXGIDevice* pDevice, D3DSwapChain* p);
	HRESULT SetFullscreenStateEx(BOOL Fullscreen, IDXGIOutput *pTarget);

	void setOriginalSwapChainDesc(DXGI_SWAP_CHAIN_DESC *pOriginalSCDesc) 
	{	m_OriginalSCDesc = *pOriginalSCDesc;	}

private:
	HRESULT ResizeTargetImpl(const DXGI_MODE_DESC *pNewTargetParameters);
	HRESULT ResizeTargetDualMonitors( const DXGI_MODE_DESC * pNewTargetParameters );

	IDXGIDevice*			m_pDXGIDevice;
	D3DSwapChain*			m_pD3DSwapChain;
	ScalingHookPtrT			m_pScalingHook;
	DXGI_SWAP_CHAIN_DESC	m_OriginalSCDesc;

	friend STDMETHODIMP Hooked_GetDesc( ID3D10Texture2D* This, D3D10_TEXTURE2D_DESC *pDesc );
};

// ToDo:   RESTORE_CODE_BEFORE_HOOK has been removed, needs to be RestoreCode
#define HOOKING_FLAG NO_SAFE_UNHOOKING
