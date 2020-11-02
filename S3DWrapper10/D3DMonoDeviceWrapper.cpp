#include "StdAfx.h"
#include "D3DMonoDeviceWrapper.h"
#include "D3DMonoDeviceFunctions-inl.h"
#include "ResourceWrapper.h"

void D3DMonoDeviceWrapper::SetInterfaceVersion ( UINT Interface )	
{
	m_nInterfaceVersion = Interface;

	m_tD3DVesion = GetD3DVersionFromInterface(Interface);
	_ASSERT( m_tD3DVesion != TD3DVersion_Unknown );
}


void D3DMonoDeviceWrapper::CreateResource( D3D10DDI_HRESOURCE& hRes, ResourceWrapper* pWrp )
{
	CreateResource(hRes, &pWrp->m_CreateResource11, pWrp->m_hRTResource);
}

void D3DMonoDeviceWrapper::CreateResource( D3D10DDI_HRESOURCE& hRes, const D3D11DDIARG_CREATERESOURCE* pCreateResourceData, D3D10DDI_HRTRESOURCE hRTResource )
{
	// Always call D3D11 version because all function version has identical parameters count with compatible structure as last argument
	D3D11DDI_DEVICEFUNCS& GET_ORIG11	= OriginalDeviceFuncs11;
	SIZE_T size = GET_ORIG11.pfnCalcPrivateResourceSize(hDevice, pCreateResourceData);
	NEW_HANDLE(hRes, size);
	GET_ORIG11.pfnCreateResource(hDevice, pCreateResourceData, hRes, hRTResource);
}

void D3DMonoDeviceWrapper::CreateRenderTargetView( D3D10DDI_HRENDERTARGETVIEW& hRTV, D3D10DDI_HRESOURCE hRes, D3D10DDIARG_CREATERENDERTARGETVIEW* pCreateRenderTargetViewData, D3D10DDI_HRTRENDERTARGETVIEW hRTRenderTargetView )
{
	pCreateRenderTargetViewData->hDrvResource = hRes;
	CreateRenderTargetView( hRTV, pCreateRenderTargetViewData, hRTRenderTargetView );
}

void D3DMonoDeviceWrapper::CreateRenderTargetView( D3D10DDI_HRENDERTARGETVIEW& hRTV, const D3D10DDIARG_CREATERENDERTARGETVIEW* pCreateRenderTargetViewData, D3D10DDI_HRTRENDERTARGETVIEW hRTRenderTargetView )
{
	D3D10DDI_DEVICEFUNCS& GET_ORIG		= OriginalDeviceFuncs;
	SIZE_T size = GET_ORIG.pfnCalcPrivateRenderTargetViewSize( hDevice, pCreateRenderTargetViewData );
	NEW_HANDLE( hRTV, size );
	GET_ORIG.pfnCreateRenderTargetView( hDevice, pCreateRenderTargetViewData, hRTV, hRTRenderTargetView );
}

void D3DMonoDeviceWrapper::CreateShaderResourceView( D3D10DDI_HSHADERRESOURCEVIEW& hSRV, D3D10DDI_HRESOURCE hRes, D3D11DDIARG_CREATESHADERRESOURCEVIEW* pCreateShaderResourceViewData, D3D10DDI_HRTSHADERRESOURCEVIEW hRTShaderResourceView )
{
	pCreateShaderResourceViewData->hDrvResource = hRes;
	CreateShaderResourceView( hSRV, pCreateShaderResourceViewData, hRTShaderResourceView );
}

void D3DMonoDeviceWrapper::CreateShaderResourceView( D3D10DDI_HSHADERRESOURCEVIEW& hSRV, const D3D11DDIARG_CREATESHADERRESOURCEVIEW* pCreateShaderResourceViewData, D3D10DDI_HRTSHADERRESOURCEVIEW hRTShaderResourceView )
{
	D3D11DDI_DEVICEFUNCS& GET_ORIG		= OriginalDeviceFuncs11;
	// Always call D3D11 version because all function version has identical parameters count with compatible structure as last argument
	SIZE_T size = GET_ORIG.pfnCalcPrivateShaderResourceViewSize(hDevice, pCreateShaderResourceViewData);
	NEW_HANDLE(hSRV, size);
	GET_ORIG.pfnCreateShaderResourceView(hDevice, pCreateShaderResourceViewData, hSRV, 
		hRTShaderResourceView);
}

void D3DMonoDeviceWrapper::DynamicIABufferMapNoOverwrite( D3D10DDI_HRESOURCE hRes, const D3D11DDIARG_CREATERESOURCE* pCreateResourceData,
	UINT Subresource, D3D10_DDI_MAP MapType, UINT MapFlags, D3D10DDI_MAPPED_SUBRESOURCE* pMappedSubresource )
{
	D3D10DDI_DEVICEFUNCS& GET_ORIG = OriginalDeviceFuncs;
	_ASSERT( pCreateResourceData->ResourceDimension == D3D10DDIRESOURCE_BUFFER &&
		pCreateResourceData->Usage == D3D10_DDI_USAGE_DYNAMIC &&
		(pCreateResourceData->BindFlags & (D3D10_DDI_BIND_VERTEX_BUFFER | D3D10_DDI_BIND_INDEX_BUFFER)) &&
		!(pCreateResourceData->BindFlags & ~(D3D10_DDI_BIND_VERTEX_BUFFER | D3D10_DDI_BIND_INDEX_BUFFER)) &&
		Subresource == 0 &&
		MapType == D3D10_DDI_MAP_WRITE_NOOVERWRITE &&
		MapFlags == 0 );
	GET_ORIG.pfnResourceMap(hDevice, hRes, Subresource, MapType, MapFlags, pMappedSubresource);
}

void D3DMonoDeviceWrapper::DynamicIABufferMapDiscard( D3D10DDI_HRESOURCE hRes, const D3D11DDIARG_CREATERESOURCE* pCreateResourceData,
	UINT Subresource, D3D10_DDI_MAP MapType, UINT MapFlags, D3D10DDI_MAPPED_SUBRESOURCE* pMappedSubresource )
{
	D3D10DDI_DEVICEFUNCS& GET_ORIG = OriginalDeviceFuncs;
	_ASSERT( pCreateResourceData->ResourceDimension == D3D10DDIRESOURCE_BUFFER &&
		pCreateResourceData->Usage == D3D10_DDI_USAGE_DYNAMIC &&
		(pCreateResourceData->BindFlags & (D3D10_DDI_BIND_VERTEX_BUFFER | D3D10_DDI_BIND_INDEX_BUFFER)) &&
		!(pCreateResourceData->BindFlags & ~(D3D10_DDI_BIND_VERTEX_BUFFER | D3D10_DDI_BIND_INDEX_BUFFER)) &&
		Subresource == 0 &&
		MapType == D3D10_DDI_MAP_WRITE_DISCARD &&
		MapFlags == 0 );	
	GET_ORIG.pfnResourceMap(hDevice, hRes, Subresource, MapType, MapFlags, pMappedSubresource);
}

void D3DMonoDeviceWrapper::DynamicConstantBufferMapDiscard( D3D10DDI_HRESOURCE hRes, const D3D11DDIARG_CREATERESOURCE* pCreateResourceData,
	UINT Subresource, D3D10_DDI_MAP MapType, UINT MapFlags, D3D10DDI_MAPPED_SUBRESOURCE* pMappedSubresource )
{
	D3D10DDI_DEVICEFUNCS& GET_ORIG = OriginalDeviceFuncs;
	_ASSERT( pCreateResourceData->ResourceDimension == D3D10DDIRESOURCE_BUFFER &&
		pCreateResourceData->Usage == D3D10_DDI_USAGE_DYNAMIC &&
		(pCreateResourceData->BindFlags & D3D10_DDI_BIND_CONSTANT_BUFFER) &&
		!(pCreateResourceData->BindFlags & ~D3D10_DDI_BIND_CONSTANT_BUFFER) &&
		Subresource == 0 &&
		MapType == D3D10_DDI_MAP_WRITE_DISCARD &&
		MapFlags == 0 );
	GET_ORIG.pfnResourceMap(hDevice, hRes, Subresource, MapType, MapFlags, pMappedSubresource);
}

void D3DMonoDeviceWrapper::DynamicResourceMapDiscard( D3D10DDI_HRESOURCE hRes, const D3D11DDIARG_CREATERESOURCE* pCreateResourceData,
	UINT Subresource, D3D10_DDI_MAP MapType, UINT MapFlags, D3D10DDI_MAPPED_SUBRESOURCE* pMappedSubresource )
{
	D3D10DDI_DEVICEFUNCS& GET_ORIG = OriginalDeviceFuncs;
	_ASSERT( pCreateResourceData->Usage == D3D10_DDI_USAGE_DYNAMIC &&
		pCreateResourceData->ArraySize == 1 &&
		pCreateResourceData->MipLevels == 1 &&
		Subresource == 0 &&
		MapType == D3D10_DDI_MAP_WRITE_DISCARD &&
		MapFlags == 0 );
	GET_ORIG.pfnResourceMap(hDevice, hRes, Subresource, MapType, MapFlags, pMappedSubresource);
}

void D3DMonoDeviceWrapper::StagingResourceMap( D3D10DDI_HRESOURCE hRes, const D3D11DDIARG_CREATERESOURCE* pCreateResourceData,
	UINT Subresource, D3D10_DDI_MAP MapType, UINT MapFlags, D3D10DDI_MAPPED_SUBRESOURCE* pMappedSubresource )
{
	D3D10DDI_DEVICEFUNCS& GET_ORIG = OriginalDeviceFuncs; 
	_ASSERT( pCreateResourceData->Usage == D3D10_DDI_USAGE_STAGING &&
		(MapType == D3D10_DDI_MAP_READ || MapType == D3D10_DDI_MAP_WRITE || MapType == D3D10_DDI_MAP_READWRITE) );
	GET_ORIG.pfnResourceMap(hDevice, hRes, Subresource, MapType, MapFlags, pMappedSubresource);
}

void D3DMonoDeviceWrapper::ResourceMap( D3D10DDI_HRESOURCE hRes, const D3D11DDIARG_CREATERESOURCE* pCreateResourceData,
	UINT Subresource, D3D10_DDI_MAP MapType, UINT MapFlags, D3D10DDI_MAPPED_SUBRESOURCE* pMappedSubresource )
{
	D3D10DDI_DEVICEFUNCS& GET_ORIG = OriginalDeviceFuncs;
	_ASSERT( pCreateResourceData->Usage != D3D10_DDI_USAGE_DYNAMIC &&
		pCreateResourceData->Usage != D3D10_DDI_USAGE_STAGING );
	GET_ORIG.pfnResourceMap(hDevice, hRes, Subresource, MapType, MapFlags, pMappedSubresource);
}

void D3DMonoDeviceWrapper::HookDeviceFuncs( struct D3D10DDI_DEVICEFUNCS * pDeviceFuncs )
{
	memcpy(&OriginalDeviceFuncs, pDeviceFuncs, sizeof(D3D10DDI_DEVICEFUNCS) );
	ZeroMemory(pDeviceFuncs, sizeof(*pDeviceFuncs));
#define SET_FUNC(x) pDeviceFuncs->pfn##x = MonoDeviceWrapper::##x;
	// Order of functions is in decreasing order of priority ( as far as performance is concerned ).
	// !!! BEGIN HIGH-FREQUENCY !!!
	SET_FUNC(DefaultConstantBufferUpdateSubresourceUP);
	SET_FUNC(VsSetConstantBuffers);
	SET_FUNC(PsSetShaderResources);
	SET_FUNC(PsSetShader);
	SET_FUNC(PsSetSamplers);
	SET_FUNC(VsSetShader);
	SET_FUNC(DrawIndexed);
	SET_FUNC(Draw);
	SET_FUNC(DynamicIABufferMapNoOverwrite);
	SET_FUNC(DynamicIABufferUnmap);
	SET_FUNC(DynamicConstantBufferMapDiscard);
	SET_FUNC(DynamicIABufferMapDiscard);
	SET_FUNC(DynamicConstantBufferUnmap);
	SET_FUNC(PsSetConstantBuffers);
	SET_FUNC(IaSetInputLayout);
	SET_FUNC(IaSetVertexBuffers);
	SET_FUNC(IaSetIndexBuffer);
	// !!! END HIGH-FREQUENCY !!!

	// Order of functions is in decreasing order of priority ( as far as performance is concerned ).
	// !!! BEGIN MIDDLE-FREQUENCY !!!
	SET_FUNC(DrawIndexedInstanced);
	SET_FUNC(DrawInstanced);
	SET_FUNC(DynamicResourceMapDiscard);
	SET_FUNC(DynamicResourceUnmap);
	SET_FUNC(GsSetConstantBuffers);
	SET_FUNC(GsSetShader);
	SET_FUNC(IaSetTopology);
	SET_FUNC(StagingResourceMap);
	SET_FUNC(StagingResourceUnmap);
	SET_FUNC(VsSetShaderResources);
	SET_FUNC(VsSetSamplers);
	SET_FUNC(GsSetShaderResources);
	SET_FUNC(GsSetSamplers);
	SET_FUNC(SetRenderTargets);
	SET_FUNC(ShaderResourceViewReadAfterWriteHazard);
	SET_FUNC(ResourceReadAfterWriteHazard);
	SET_FUNC(SetBlendState);
	SET_FUNC(SetDepthStencilState);
	SET_FUNC(SetRasterizerState);
	SET_FUNC(QueryEnd);
	SET_FUNC(QueryBegin);
	SET_FUNC(ResourceCopyRegion);
	SET_FUNC(ResourceUpdateSubresourceUP);
	SET_FUNC(SoSetTargets);
	SET_FUNC(DrawAuto);
	SET_FUNC(SetViewports);
	SET_FUNC(SetScissorRects);
	SET_FUNC(ClearRenderTargetView);
	SET_FUNC(ClearDepthStencilView);
	SET_FUNC(SetPredication);
	SET_FUNC(QueryGetData);
	SET_FUNC(Flush);
	SET_FUNC(GenMips);
	SET_FUNC(ResourceCopy);
	SET_FUNC(ResourceResolveSubresource);
	// !!! END MIDDLE-FREQUENCY !!!

	// Infrequent paths:
	SET_FUNC(ResourceMap);
	SET_FUNC(ResourceUnmap);
	SET_FUNC(ResourceIsStagingBusy);
	SET_FUNC(RelocateDeviceFuncs);
	SET_FUNC(CalcPrivateResourceSize);
	SET_FUNC(CalcPrivateOpenedResourceSize);
	SET_FUNC(CreateResource);
	SET_FUNC(OpenResource);
	SET_FUNC(DestroyResource);
	SET_FUNC(CalcPrivateShaderResourceViewSize);
	SET_FUNC(CreateShaderResourceView);
	SET_FUNC(DestroyShaderResourceView);
	SET_FUNC(CalcPrivateRenderTargetViewSize);
	SET_FUNC(CreateRenderTargetView);
	SET_FUNC(DestroyRenderTargetView);
	SET_FUNC(CalcPrivateDepthStencilViewSize);
	SET_FUNC(CreateDepthStencilView);
	SET_FUNC(DestroyDepthStencilView);
	SET_FUNC(CalcPrivateElementLayoutSize);
	SET_FUNC(CreateElementLayout);
	SET_FUNC(DestroyElementLayout);
	SET_FUNC(CalcPrivateBlendStateSize);
	SET_FUNC(CreateBlendState);
	SET_FUNC(DestroyBlendState);
	SET_FUNC(CalcPrivateDepthStencilStateSize);
	SET_FUNC(CreateDepthStencilState);
	SET_FUNC(DestroyDepthStencilState);
	SET_FUNC(CalcPrivateRasterizerStateSize);
	SET_FUNC(CreateRasterizerState);
	SET_FUNC(DestroyRasterizerState);
	SET_FUNC(CalcPrivateShaderSize);
	SET_FUNC(CreateVertexShader);
	SET_FUNC(CreateGeometryShader);
	SET_FUNC(CreatePixelShader);
	SET_FUNC(CalcPrivateGeometryShaderWithStreamOutput);
	SET_FUNC(CreateGeometryShaderWithStreamOutput);
	SET_FUNC(DestroyShader);
	SET_FUNC(CalcPrivateSamplerSize);
	SET_FUNC(CreateSampler);
	SET_FUNC(DestroySampler);
	SET_FUNC(CalcPrivateQuerySize);
	SET_FUNC(CreateQuery);
	SET_FUNC(DestroyQuery);

	SET_FUNC(CheckFormatSupport);
	SET_FUNC(CheckMultisampleQualityLevels);
	SET_FUNC(CheckCounterInfo);
	SET_FUNC(CheckCounter);

	SET_FUNC(DestroyDevice);
	SET_FUNC(SetTextFilterSize);
#undef SET_FUNC
	CheckFunctions(&OriginalDeviceFuncs, pDeviceFuncs);
}

void D3DMonoDeviceWrapper::HookDeviceFuncs( struct D3D10_1DDI_DEVICEFUNCS * pDeviceFuncs )
{
	memcpy(&OriginalDeviceFuncs10_1, pDeviceFuncs, sizeof(D3D10_1DDI_DEVICEFUNCS) );
	ZeroMemory(pDeviceFuncs, sizeof(*pDeviceFuncs));
#define SET_FUNC(x) pDeviceFuncs->pfn##x = MonoDeviceWrapper::##x;
#define SET_FUNC_10_1(x) pDeviceFuncs->pfn##x = MonoDeviceWrapper::##x ##10_1;
	// Order of functions is in decreasing order of priority ( as far as performance is concerned ).
	// !!! BEGIN HIGH-FREQUENCY !!!
	SET_FUNC(DefaultConstantBufferUpdateSubresourceUP);
	SET_FUNC(VsSetConstantBuffers);
	SET_FUNC(PsSetShaderResources);
	SET_FUNC(PsSetShader);
	SET_FUNC(PsSetSamplers);
	SET_FUNC(VsSetShader);
	SET_FUNC(DrawIndexed);
	SET_FUNC(Draw);
	SET_FUNC(DynamicIABufferMapNoOverwrite);
	SET_FUNC(DynamicIABufferUnmap);
	SET_FUNC(DynamicConstantBufferMapDiscard);
	SET_FUNC(DynamicIABufferMapDiscard);
	SET_FUNC(DynamicConstantBufferUnmap);
	SET_FUNC(PsSetConstantBuffers);
	SET_FUNC(IaSetInputLayout);
	SET_FUNC(IaSetVertexBuffers);
	SET_FUNC(IaSetIndexBuffer);
	// !!! END HIGH-FREQUENCY !!!

	// Order of functions is in decreasing order of priority ( as far as performance is concerned ).
	// !!! BEGIN MIDDLE-FREQUENCY !!!
	SET_FUNC(DrawIndexedInstanced);
	SET_FUNC(DrawInstanced);
	SET_FUNC(DynamicResourceMapDiscard);
	SET_FUNC(DynamicResourceUnmap);
	SET_FUNC(GsSetConstantBuffers);
	SET_FUNC(GsSetShader);
	SET_FUNC(IaSetTopology);
	SET_FUNC(StagingResourceMap);
	SET_FUNC(StagingResourceUnmap);
	SET_FUNC(VsSetShaderResources);
	SET_FUNC(VsSetSamplers);
	SET_FUNC(GsSetShaderResources);
	SET_FUNC(GsSetSamplers);
	SET_FUNC(SetRenderTargets);
	SET_FUNC(ShaderResourceViewReadAfterWriteHazard);
	SET_FUNC(ResourceReadAfterWriteHazard);
	SET_FUNC(SetBlendState);
	SET_FUNC(SetDepthStencilState);
	SET_FUNC(SetRasterizerState);
	SET_FUNC(QueryEnd);
	SET_FUNC(QueryBegin);
	SET_FUNC(ResourceCopyRegion);
	SET_FUNC(ResourceUpdateSubresourceUP);
	SET_FUNC(SoSetTargets);
	SET_FUNC(DrawAuto);
	SET_FUNC(SetViewports);
	SET_FUNC(SetScissorRects);
	SET_FUNC(ClearRenderTargetView);
	SET_FUNC(ClearDepthStencilView);
	SET_FUNC(SetPredication);
	SET_FUNC(QueryGetData);
	SET_FUNC(Flush);
	SET_FUNC(GenMips);
	SET_FUNC(ResourceCopy);
	SET_FUNC(ResourceResolveSubresource);
	// !!! END MIDDLE-FREQUENCY !!!

	// Infrequent paths:
	SET_FUNC(ResourceMap);
	SET_FUNC(ResourceUnmap);
	SET_FUNC(ResourceIsStagingBusy);
	SET_FUNC_10_1(RelocateDeviceFuncs);
	SET_FUNC(CalcPrivateResourceSize);
	SET_FUNC(CalcPrivateOpenedResourceSize);
	SET_FUNC(CreateResource);
	SET_FUNC(OpenResource);
	SET_FUNC(DestroyResource);
	SET_FUNC_10_1(CalcPrivateShaderResourceViewSize);
	SET_FUNC_10_1(CreateShaderResourceView);
	SET_FUNC(DestroyShaderResourceView);
	SET_FUNC(CalcPrivateRenderTargetViewSize);
	SET_FUNC(CreateRenderTargetView);
	SET_FUNC(DestroyRenderTargetView);
	SET_FUNC(CalcPrivateDepthStencilViewSize);
	SET_FUNC(CreateDepthStencilView);
	SET_FUNC(DestroyDepthStencilView);
	SET_FUNC(CalcPrivateElementLayoutSize);
	SET_FUNC(CreateElementLayout);
	SET_FUNC(DestroyElementLayout);
	SET_FUNC_10_1(CalcPrivateBlendStateSize);
	SET_FUNC_10_1(CreateBlendState);
	SET_FUNC(DestroyBlendState);
	SET_FUNC(CalcPrivateDepthStencilStateSize);
	SET_FUNC(CreateDepthStencilState);
	SET_FUNC(DestroyDepthStencilState);
	SET_FUNC(CalcPrivateRasterizerStateSize);
	SET_FUNC(CreateRasterizerState);
	SET_FUNC(DestroyRasterizerState);
	SET_FUNC(CalcPrivateShaderSize);
	SET_FUNC(CreateVertexShader);
	SET_FUNC(CreateGeometryShader);
	SET_FUNC(CreatePixelShader);
	SET_FUNC(CalcPrivateGeometryShaderWithStreamOutput);
	SET_FUNC(CreateGeometryShaderWithStreamOutput);
	SET_FUNC(DestroyShader);
	SET_FUNC(CalcPrivateSamplerSize);
	SET_FUNC(CreateSampler);
	SET_FUNC(DestroySampler);
	SET_FUNC(CalcPrivateQuerySize);
	SET_FUNC(CreateQuery);
	SET_FUNC(DestroyQuery);

	SET_FUNC(CheckFormatSupport);
	SET_FUNC(CheckMultisampleQualityLevels);
	SET_FUNC(CheckCounterInfo);
	SET_FUNC(CheckCounter);

	SET_FUNC(DestroyDevice);
	SET_FUNC(SetTextFilterSize);

	// Start additional 10.1 entries:
	SET_FUNC_10_1(ResourceConvert);
	SET_FUNC_10_1(ResourceConvertRegion);
#undef SET_FUNC_10_1
#undef SET_FUNC
	CheckFunctions(&OriginalDeviceFuncs10_1, pDeviceFuncs);
}

void D3DMonoDeviceWrapper::HookDeviceFuncs( struct D3D11DDI_DEVICEFUNCS * pDeviceFuncs )
{
	memcpy(&OriginalDeviceFuncs11, pDeviceFuncs, sizeof(D3D11DDI_DEVICEFUNCS) );
	ZeroMemory(pDeviceFuncs, sizeof(*pDeviceFuncs));
#define SET_FUNC(x) pDeviceFuncs->pfn##x = MonoDeviceWrapper::##x;
#define SET_FUNC_10_1(x) pDeviceFuncs->pfn##x = MonoDeviceWrapper::##x ##10_1;
#define SET_FUNC_11(x) pDeviceFuncs->pfn##x = MonoDeviceWrapper::##x ##11_0;
	// Order of functions is in decreasing order of priority ( as far as performance is concerned ).
	// !!! BEGIN HIGH-FREQUENCY !!!
	SET_FUNC(DefaultConstantBufferUpdateSubresourceUP);
	SET_FUNC(VsSetConstantBuffers);
	SET_FUNC(PsSetShaderResources);
	SET_FUNC(PsSetShader);
	SET_FUNC(PsSetSamplers);
	SET_FUNC(VsSetShader);
	SET_FUNC(DrawIndexed);
	SET_FUNC(Draw);
	SET_FUNC(DynamicIABufferMapNoOverwrite);
	SET_FUNC(DynamicIABufferUnmap);
	SET_FUNC(DynamicConstantBufferMapDiscard);
	SET_FUNC(DynamicIABufferMapDiscard);
	SET_FUNC(DynamicConstantBufferUnmap);
	SET_FUNC(PsSetConstantBuffers);
	SET_FUNC(IaSetInputLayout);
	SET_FUNC(IaSetVertexBuffers);
	SET_FUNC(IaSetIndexBuffer);
	// !!! END HIGH-FREQUENCY !!!

	// Order of functions is in decreasing order of priority ( as far as performance is concerned ).
	// !!! BEGIN MIDDLE-FREQUENCY !!!
	SET_FUNC(DrawIndexedInstanced);
	SET_FUNC(DrawInstanced);
	SET_FUNC(DynamicResourceMapDiscard);
	SET_FUNC(DynamicResourceUnmap);
	SET_FUNC(GsSetConstantBuffers);
	SET_FUNC(GsSetShader);
	SET_FUNC(IaSetTopology);
	SET_FUNC(StagingResourceMap);
	SET_FUNC(StagingResourceUnmap);
	SET_FUNC(VsSetShaderResources);
	SET_FUNC(VsSetSamplers);
	SET_FUNC(GsSetShaderResources);
	SET_FUNC(GsSetSamplers);
	SET_FUNC_11(SetRenderTargets);
	SET_FUNC(ShaderResourceViewReadAfterWriteHazard);
	SET_FUNC(ResourceReadAfterWriteHazard);
	SET_FUNC(SetBlendState);
	SET_FUNC(SetDepthStencilState);
	SET_FUNC(SetRasterizerState);
	SET_FUNC(QueryEnd);
	SET_FUNC(QueryBegin);
	SET_FUNC(ResourceCopyRegion);
	SET_FUNC(ResourceUpdateSubresourceUP);
	SET_FUNC(SoSetTargets);
	SET_FUNC(DrawAuto);
	SET_FUNC(SetViewports);
	SET_FUNC(SetScissorRects);
	SET_FUNC(ClearRenderTargetView);
	SET_FUNC(ClearDepthStencilView);
	SET_FUNC(SetPredication);
	SET_FUNC(QueryGetData);
	SET_FUNC(Flush);
	SET_FUNC(GenMips);
	SET_FUNC(ResourceCopy);
	SET_FUNC(ResourceResolveSubresource);
	// !!! END MIDDLE-FREQUENCY !!!

	// Infrequent paths:
	SET_FUNC(ResourceMap);
	SET_FUNC(ResourceUnmap);
	SET_FUNC(ResourceIsStagingBusy);
	SET_FUNC_11(RelocateDeviceFuncs);
	SET_FUNC_11(CalcPrivateResourceSize);
	SET_FUNC(CalcPrivateOpenedResourceSize);
	SET_FUNC_11(CreateResource);
	SET_FUNC(OpenResource);
	SET_FUNC(DestroyResource);
	SET_FUNC_11(CalcPrivateShaderResourceViewSize);
	SET_FUNC_11(CreateShaderResourceView);
	SET_FUNC(DestroyShaderResourceView);
	SET_FUNC(CalcPrivateRenderTargetViewSize);
	SET_FUNC(CreateRenderTargetView);
	SET_FUNC(DestroyRenderTargetView);
	SET_FUNC_11(CalcPrivateDepthStencilViewSize);
	SET_FUNC_11(CreateDepthStencilView);
	SET_FUNC(DestroyDepthStencilView);
	SET_FUNC(CalcPrivateElementLayoutSize);
	SET_FUNC(CreateElementLayout);
	SET_FUNC(DestroyElementLayout);
	SET_FUNC_10_1(CalcPrivateBlendStateSize);
	SET_FUNC_10_1(CreateBlendState);
	SET_FUNC(DestroyBlendState);
	SET_FUNC(CalcPrivateDepthStencilStateSize);
	SET_FUNC(CreateDepthStencilState);
	SET_FUNC(DestroyDepthStencilState);
	SET_FUNC(CalcPrivateRasterizerStateSize);
	SET_FUNC(CreateRasterizerState);
	SET_FUNC(DestroyRasterizerState);
	SET_FUNC(CalcPrivateShaderSize);
	SET_FUNC(CreateVertexShader);
	SET_FUNC(CreateGeometryShader);
	SET_FUNC(CreatePixelShader);
	SET_FUNC_11(CalcPrivateGeometryShaderWithStreamOutput);
	SET_FUNC_11(CreateGeometryShaderWithStreamOutput);
	SET_FUNC(DestroyShader);
	SET_FUNC(CalcPrivateSamplerSize);
	SET_FUNC(CreateSampler);
	SET_FUNC(DestroySampler);
	SET_FUNC(CalcPrivateQuerySize);
	SET_FUNC(CreateQuery);
	SET_FUNC(DestroyQuery);

	SET_FUNC(CheckFormatSupport);
	SET_FUNC(CheckMultisampleQualityLevels);
	SET_FUNC(CheckCounterInfo);
	SET_FUNC(CheckCounter);

	SET_FUNC(DestroyDevice);
	SET_FUNC(SetTextFilterSize);

	// Start additional 10.1 entries:
	SET_FUNC_10_1(ResourceConvert);
	SET_FUNC_10_1(ResourceConvertRegion);

	// Start additional 11.0 entries:
	SET_FUNC_11(DrawIndexedInstancedIndirect);
	SET_FUNC_11(DrawInstancedIndirect);
	SET_FUNC_11(CommandListExecute); // Only required when supporting D3D11DDICAPS_COMMANDLISTS
	SET_FUNC_11(HsSetShaderResources);
	SET_FUNC_11(HsSetShader);
	SET_FUNC_11(HsSetSamplers);
	SET_FUNC_11(HsSetConstantBuffers);
	SET_FUNC_11(DsSetShaderResources);
	SET_FUNC_11(DsSetShader);
	SET_FUNC_11(DsSetSamplers);
	SET_FUNC_11(DsSetConstantBuffers);
	SET_FUNC_11(CreateHullShader);
	SET_FUNC_11(CreateDomainShader);
	SET_FUNC_11(CheckDeferredContextHandleSizes); // Only required when supporting D3D11DDICAPS_COMMANDLISTS
	SET_FUNC_11(CalcDeferredContextHandleSize); // Only required when supporting D3D11DDICAPS_COMMANDLISTS
	SET_FUNC_11(CalcPrivateDeferredContextSize); // Only required when supporting D3D11DDICAPS_COMMANDLISTS
	SET_FUNC_11(CreateDeferredContext); // Only required when supporting D3D11DDICAPS_COMMANDLISTS
	SET_FUNC_11(AbandonCommandList); // Only required when supporting D3D11DDICAPS_COMMANDLISTS
	SET_FUNC_11(CalcPrivateCommandListSize); // Only required when supporting D3D11DDICAPS_COMMANDLISTS
	SET_FUNC_11(CreateCommandList); // Only required when supporting D3D11DDICAPS_COMMANDLISTS
	SET_FUNC_11(DestroyCommandList); // Only required when supporting D3D11DDICAPS_COMMANDLISTS
	SET_FUNC_11(CalcPrivateTessellationShaderSize);
	SET_FUNC_11(PsSetShaderWithIfaces);
	SET_FUNC_11(VsSetShaderWithIfaces);
	SET_FUNC_11(GsSetShaderWithIfaces);
	SET_FUNC_11(HsSetShaderWithIfaces);
	SET_FUNC_11(DsSetShaderWithIfaces);
	SET_FUNC_11(CsSetShaderWithIfaces);
	SET_FUNC_11(CreateComputeShader);
	SET_FUNC_11(CsSetShader);
	SET_FUNC_11(CsSetShaderResources);
	SET_FUNC_11(CsSetSamplers);
	SET_FUNC_11(CsSetConstantBuffers);
	SET_FUNC_11(CalcPrivateUnorderedAccessViewSize);
	SET_FUNC_11(CreateUnorderedAccessView);
	SET_FUNC_11(DestroyUnorderedAccessView);
	SET_FUNC_11(ClearUnorderedAccessViewUint);
	SET_FUNC_11(ClearUnorderedAccessViewFloat);
	SET_FUNC_11(CsSetUnorderedAccessViews);
	SET_FUNC_11(Dispatch);
	SET_FUNC_11(DispatchIndirect);
	SET_FUNC_11(SetResourceMinLOD);
	SET_FUNC_11(CopyStructureCount);
#undef SET_FUNC_11
#undef SET_FUNC_10_1
#undef SET_FUNC
	CheckFunctions(&OriginalDeviceFuncs11, pDeviceFuncs);
}

void D3DMonoDeviceWrapper::HookDXGIFuncs( struct DXGI_DDI_BASE_FUNCTIONS  *pDXGIDDIBaseFunctions )
{
	memcpy(&OriginalDXGIDDIBaseFunctions, pDXGIDDIBaseFunctions, sizeof DXGI_DDI_BASE_FUNCTIONS);
	ZeroMemory(pDXGIDDIBaseFunctions, sizeof DXGI_DDI_BASE_FUNCTIONS);
#define SET_FUNC(x) if (OriginalDXGIDDIBaseFunctions.pfn##x) pDXGIDDIBaseFunctions->pfn##x = MonoDeviceWrapper::##x;
	SET_FUNC(Present);
	SET_FUNC(GetGammaCaps);
	SET_FUNC(SetDisplayMode);
	SET_FUNC(SetResourcePriority);
	SET_FUNC(QueryResourceResidency);
	SET_FUNC(RotateResourceIdentities);
	SET_FUNC(Blt);
#undef SET_FUNC
	CheckFunctions(&OriginalDXGIDDIBaseFunctions, pDXGIDDIBaseFunctions);
}

void D3DMonoDeviceWrapper::HookDXGIFuncs( struct DXGI1_1_DDI_BASE_FUNCTIONS  *pDXGIDDIBaseFunctions )
{
	memcpy(&OriginalDXGIDDIBaseFunctions2, pDXGIDDIBaseFunctions, sizeof DXGI1_1_DDI_BASE_FUNCTIONS);
	ZeroMemory(pDXGIDDIBaseFunctions, sizeof DXGI1_1_DDI_BASE_FUNCTIONS);
#define SET_FUNC(x) if (OriginalDXGIDDIBaseFunctions2.pfn##x) pDXGIDDIBaseFunctions->pfn##x = MonoDeviceWrapper::##x;
	SET_FUNC(Present);
	SET_FUNC(GetGammaCaps);
	SET_FUNC(SetDisplayMode);
	SET_FUNC(SetResourcePriority);
	SET_FUNC(QueryResourceResidency);
	SET_FUNC(RotateResourceIdentities);
	SET_FUNC(Blt);
	SET_FUNC(ResolveSharedResource);
#undef SET_FUNC
	CheckFunctions(&OriginalDXGIDDIBaseFunctions2, pDXGIDDIBaseFunctions);
}
