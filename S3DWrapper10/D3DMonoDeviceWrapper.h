#pragma once

// Ёто текстовый файл. ¬ нЄм содержитс€ —++ текст, разные классы, фенкци€, но вообще вс€ та
// хренотень, что обычно понапишут программисты. ¬ начале таких файлов пристуствует комментарий
// ну то есть много тесте, написанного зелеными буквами, которые никто никогда не читает.
// Ќе читают лишь по тому, что написан он на английском €зыке. я вот сейчас напишу его по-русски,
// все прочтут, вопросы сразу задвать будут, зачем € это написал, да к чему, да почему...

#include "d3d10umddi.h"
#include "Utils.h"
#include "UMHandleWrapper.h"

class ResourceWrapper;

class D3DMonoDeviceWrapper : public UMHandleWrapper<D3D10DDI_HDEVICE>
{
public:
	D3DMonoDeviceWrapper() : m_tD3DVesion ( TD3DVersion_Unknown ), m_nInterfaceVersion ( 0 ) {}

	D3D10DDI_HDEVICE				hDevice;
	void							SetInterfaceVersion ( UINT nVersion );
	TD3DVersion						GetD3DVersion		() const { return m_tD3DVesion; }
	inline D3D10DDI_HDEVICE			GetHandle()					 { return hDevice;		}
	union
	{
		D3D10DDI_DEVICEFUNCS		OriginalDeviceFuncs;
		D3D10_1DDI_DEVICEFUNCS		OriginalDeviceFuncs10_1;
		D3D11DDI_DEVICEFUNCS		OriginalDeviceFuncs11;
	};
	union
	{
		DXGI_DDI_BASE_FUNCTIONS		OriginalDXGIDDIBaseFunctions;
		DXGI1_1_DDI_BASE_FUNCTIONS	OriginalDXGIDDIBaseFunctions2;
	};

	template<class T> void			CheckFunctions( T  *pOrigFunctions, T *pFunctions );	

	void							HookDeviceFuncs(struct D3D10DDI_DEVICEFUNCS*  pDeviceFunctions);
	void							HookDeviceFuncs(struct D3D10_1DDI_DEVICEFUNCS*  pDeviceFunctions);
	void							HookDeviceFuncs(struct D3D11DDI_DEVICEFUNCS*  pDeviceFunctions);
	void							HookDXGIFuncs(struct DXGI_DDI_BASE_FUNCTIONS  *pDXGIDDIBaseFunctions);
	void							HookDXGIFuncs(struct DXGI1_1_DDI_BASE_FUNCTIONS  *pDXGIDDIBaseFunctions);

	void CreateResource( D3D10DDI_HRESOURCE& hRes, ResourceWrapper* pWrp );
	void CreateResource( D3D10DDI_HRESOURCE& hRes, const D3D11DDIARG_CREATERESOURCE* pCreateResourceData, D3D10DDI_HRTRESOURCE hRTResource );
	void CreateRenderTargetView( D3D10DDI_HRENDERTARGETVIEW& hRTV, D3D10DDI_HRESOURCE hRes, D3D10DDIARG_CREATERENDERTARGETVIEW* pCreateRenderTargetViewData, D3D10DDI_HRTRENDERTARGETVIEW hRTRenderTargetView );
	void CreateRenderTargetView( D3D10DDI_HRENDERTARGETVIEW& hRTV, const D3D10DDIARG_CREATERENDERTARGETVIEW* pCreateRenderTargetViewData, D3D10DDI_HRTRENDERTARGETVIEW hRTRenderTargetView );
	void CreateShaderResourceView( D3D10DDI_HSHADERRESOURCEVIEW& hSRV, D3D10DDI_HRESOURCE hRes, D3D11DDIARG_CREATESHADERRESOURCEVIEW* pCreateShaderResourceViewData, D3D10DDI_HRTSHADERRESOURCEVIEW hRTShaderResourceView );
	void CreateShaderResourceView( D3D10DDI_HSHADERRESOURCEVIEW& hSRV, const D3D11DDIARG_CREATESHADERRESOURCEVIEW* pCreateShaderResourceViewData, D3D10DDI_HRTSHADERRESOURCEVIEW hRTShaderResourceView );

	void DynamicIABufferMapNoOverwrite( D3D10DDI_HRESOURCE hRes, const D3D11DDIARG_CREATERESOURCE* pCreateResourceData, UINT Subresource, D3D10_DDI_MAP MapType, UINT MapFlags, D3D10DDI_MAPPED_SUBRESOURCE* pMappedSubresource );
	void DynamicIABufferMapDiscard( D3D10DDI_HRESOURCE hRes, const D3D11DDIARG_CREATERESOURCE* pCreateResourceData, UINT Subresource, D3D10_DDI_MAP MapType, UINT MapFlags, D3D10DDI_MAPPED_SUBRESOURCE* pMappedSubresource );
	void DynamicConstantBufferMapDiscard( D3D10DDI_HRESOURCE hRes, const D3D11DDIARG_CREATERESOURCE* pCreateResourceData, UINT Subresource, D3D10_DDI_MAP MapType, UINT MapFlags, D3D10DDI_MAPPED_SUBRESOURCE* pMappedSubresource );
	void DynamicResourceMapDiscard( D3D10DDI_HRESOURCE hRes, const D3D11DDIARG_CREATERESOURCE* pCreateResourceData, UINT Subresource, D3D10_DDI_MAP MapType, UINT MapFlags, D3D10DDI_MAPPED_SUBRESOURCE* pMappedSubresource );
	void StagingResourceMap( D3D10DDI_HRESOURCE hRes, const D3D11DDIARG_CREATERESOURCE* pCreateResourceData, UINT Subresource, D3D10_DDI_MAP MapType, UINT MapFlags, D3D10DDI_MAPPED_SUBRESOURCE* pMappedSubresource );
	void ResourceMap( D3D10DDI_HRESOURCE hRes, const D3D11DDIARG_CREATERESOURCE* pCreateResourceData, UINT Subresource, D3D10_DDI_MAP MapType, UINT MapFlags, D3D10DDI_MAPPED_SUBRESOURCE* pMappedSubresource );
protected:
	UINT							m_nInterfaceVersion;
	TD3DVersion						m_tD3DVesion;
};

template<class T> 
void D3DMonoDeviceWrapper::CheckFunctions( T *pOrigFunctions, T *pFunctions )
{
#ifdef _DEBUG
	void** p = (void**)pFunctions;
	void** pOrig = (void**)pOrigFunctions;
	size_t cnt = sizeof(T) / sizeof(void*);
	for (size_t i = 0; i < cnt; i++)
	{
		if (pOrig[i] != NULL && p[i] == NULL)
			__debugbreak();
	}
#endif
}
