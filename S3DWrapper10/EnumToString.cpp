#include "stdafx.h"
#include <intrin.h>
#include "Commands\Command.h"
#include "EnumToString.h"

class TempBuffersManager
{
public:
	TempBuffersManager();
	~TempBuffersManager();

	TCHAR *getTempBuffer( size_t size );
	void releaseTempBuffer( TCHAR * /*buffer*/, size_t /*size*/ );
private:
	long pos;
	TCHAR *list[32];
};

static TempBuffersManager temBuf;

TempBuffersManager::TempBuffersManager()
{
	for(int i = 0; i < _countof(list); i++)
		list[i] = NULL;
}

/* allocate some tmp string space */
/* FIXME: this is not 100% thread-safe */
TCHAR *TempBuffersManager::getTempBuffer( size_t size )
{
	TCHAR *ret;
	int idx;

	idx = _InterlockedExchangeAdd( &pos, 1 ) % _countof(list);
	ret = (TCHAR*)realloc( (void*)list[idx], size * sizeof(TCHAR) );
	if (ret) list[idx] = ret;
	return ret;
}

void TempBuffersManager::releaseTempBuffer( TCHAR * /*buffer*/, size_t /*size*/ )
{
	/* don't bother doing anything */
}

TempBuffersManager::~TempBuffersManager()
{
	for(int i = 0; i < _countof(list); i++)
	{
		if (list[i])
			free(list[i]);
	}
}

/* printf with temp buffer allocation */
const TCHAR *dbg_sprintf( const TCHAR *format, ... )
{
	static const int max_size = 300;
	TCHAR *ret;
	int len;
	va_list valist;

	va_start(valist, format);
	ret = temBuf.getTempBuffer( max_size );
	len = _vsntprintf( ret, max_size, format, valist );
	if (len == -1 || len >= max_size) ret[max_size-1] = 0;
	else temBuf.releaseTempBuffer( ret, len + 1 );
	va_end(valist);
	return ret;
}

/* printf with temp buffer allocation */
const TCHAR *dbg_sprintfEx( int max_size, const TCHAR *format, ... )
{
	TCHAR *ret;
	int len;
	va_list valist;

	va_start(valist, format);
	ret = temBuf.getTempBuffer( max_size );
	len = _vsntprintf( ret, max_size, format, valist );
	if (len == -1 || len >= max_size) ret[max_size-1] = 0;
	else temBuf.releaseTempBuffer( ret, len + 1 );
	va_end(valist);
	return ret;
}

const char *CommandIDToString(CommandsId id) 
{
#define UNPACK_VALUE(VALUE) case id##VALUE: return #VALUE;
	switch (id) 
	{
		UNPACK_VALUE(None);

		//DXGIFunctions
		UNPACK_VALUE(Blt);
		UNPACK_VALUE(Present);
		UNPACK_VALUE(GetGammaCaps);
		UNPACK_VALUE(SetDisplayMode);
		UNPACK_VALUE(SetResourcePriority);
		UNPACK_VALUE(QueryResourceResidency);
		UNPACK_VALUE(RotateResourceIdentities);
		UNPACK_VALUE(ResolveSharedResource);

		//D3D10Functions
		UNPACK_VALUE(GsSetShader);
		UNPACK_VALUE(GsSetConstantBuffers);
		UNPACK_VALUE(GsSetSamplers);
		UNPACK_VALUE(GsSetShaderResources);
		UNPACK_VALUE(PsSetShader);
		UNPACK_VALUE(PsSetConstantBuffers);
		UNPACK_VALUE(PsSetSamplers);
		UNPACK_VALUE(PsSetShaderResources);
		UNPACK_VALUE(VsSetShader);
		UNPACK_VALUE(VsSetConstantBuffers);
		UNPACK_VALUE(VsSetSamplers);
		UNPACK_VALUE(VsSetShaderResources);
		UNPACK_VALUE(IaSetIndexBuffer);
		UNPACK_VALUE(IaSetInputLayout);
		UNPACK_VALUE(IaSetTopology);
		UNPACK_VALUE(IaSetVertexBuffers);
		UNPACK_VALUE(SetRasterizerState);
		UNPACK_VALUE(SetViewports);
		UNPACK_VALUE(SetScissorRects);
		UNPACK_VALUE(SetBlendState);
		UNPACK_VALUE(SetDepthStencilState);
		UNPACK_VALUE(SetRenderTargets);
		UNPACK_VALUE(SetPredication);
		UNPACK_VALUE(SoSetTargets);
		UNPACK_VALUE(SetTextFilterSize);

		UNPACK_VALUE(DefaultConstantBufferUpdateSubresourceUP);
		UNPACK_VALUE(DrawIndexed);
		UNPACK_VALUE(Draw);
		UNPACK_VALUE(DynamicIABufferMapNoOverwrite);
		UNPACK_VALUE(DynamicIABufferUnmap);
		UNPACK_VALUE(DynamicConstantBufferMapDiscard);
		UNPACK_VALUE(DynamicIABufferMapDiscard);
		UNPACK_VALUE(DynamicConstantBufferUnmap);
		UNPACK_VALUE(DrawIndexedInstanced);
		UNPACK_VALUE(DrawInstanced);
		UNPACK_VALUE(DynamicResourceMapDiscard);
		UNPACK_VALUE(DynamicResourceUnmap);
		UNPACK_VALUE(StagingResourceMap);
		UNPACK_VALUE(StagingResourceUnmap);
		UNPACK_VALUE(ShaderResourceViewReadAfterWriteHazard);
		UNPACK_VALUE(ResourceReadAfterWriteHazard);
		UNPACK_VALUE(QueryEnd);
		UNPACK_VALUE(QueryBegin);
		UNPACK_VALUE(ResourceCopyRegion);
		UNPACK_VALUE(ResourceUpdateSubresourceUP);
		UNPACK_VALUE(DrawAuto);
		UNPACK_VALUE(ClearRenderTargetView);
		UNPACK_VALUE(ClearDepthStencilView);
		UNPACK_VALUE(QueryGetData);
		UNPACK_VALUE(Flush);
		UNPACK_VALUE(GenMips);
		UNPACK_VALUE(ResourceCopy);
		UNPACK_VALUE(ResourceResolveSubresource);
		UNPACK_VALUE(ResourceMap);
		UNPACK_VALUE(ResourceUnmap);
		UNPACK_VALUE(ResourceIsStagingBusy);
		UNPACK_VALUE(RelocateDeviceFuncs);
		UNPACK_VALUE(CalcPrivateResourceSize);
		UNPACK_VALUE(CalcPrivateOpenedResourceSize);
		UNPACK_VALUE(CreateResource);
		UNPACK_VALUE(OpenResource);
		UNPACK_VALUE(DestroyResource);
		UNPACK_VALUE(CalcPrivateShaderResourceViewSize);
		UNPACK_VALUE(CreateShaderResourceView);
		UNPACK_VALUE(DestroyShaderResourceView);
		UNPACK_VALUE(CalcPrivateRenderTargetViewSize);
		UNPACK_VALUE(CreateRenderTargetView);
		UNPACK_VALUE(DestroyRenderTargetView);
		UNPACK_VALUE(CalcPrivateDepthStencilViewSize);
		UNPACK_VALUE(CreateDepthStencilView);
		UNPACK_VALUE(DestroyDepthStencilView);
		UNPACK_VALUE(CalcPrivateElementLayoutSize);
		UNPACK_VALUE(CreateElementLayout);
		UNPACK_VALUE(DestroyElementLayout);
		UNPACK_VALUE(CalcPrivateBlendStateSize);
		UNPACK_VALUE(CreateBlendState);
		UNPACK_VALUE(DestroyBlendState);
		UNPACK_VALUE(CalcPrivateDepthStencilStateSize);
		UNPACK_VALUE(CreateDepthStencilState);
		UNPACK_VALUE(DestroyDepthStencilState);
		UNPACK_VALUE(CalcPrivateRasterizerStateSize);
		UNPACK_VALUE(CreateRasterizerState);
		UNPACK_VALUE(DestroyRasterizerState);
		UNPACK_VALUE(CalcPrivateShaderSize);
		UNPACK_VALUE(CreateVertexShader);
		UNPACK_VALUE(CreateGeometryShader);
		UNPACK_VALUE(CreatePixelShader);
		UNPACK_VALUE(CalcPrivateGeometryShaderWithStreamOutput);
		UNPACK_VALUE(CreateGeometryShaderWithStreamOutput);
		UNPACK_VALUE(DestroyShader);
		UNPACK_VALUE(CalcPrivateSamplerSize);
		UNPACK_VALUE(CreateSampler);
		UNPACK_VALUE(DestroySampler);
		UNPACK_VALUE(CalcPrivateQuerySize);
		UNPACK_VALUE(CreateQuery);
		UNPACK_VALUE(DestroyQuery);
		UNPACK_VALUE(CheckFormatSupport);
		UNPACK_VALUE(CheckMultisampleQualityLevels);
		UNPACK_VALUE(CheckCounterInfo);
		UNPACK_VALUE(CheckCounter);
		UNPACK_VALUE(DestroyDevice);

		//D3D10_1Functions
		UNPACK_VALUE(RelocateDeviceFuncs10_1);
		UNPACK_VALUE(CalcPrivateShaderResourceViewSize10_1);
		UNPACK_VALUE(CreateShaderResourceView10_1);
		UNPACK_VALUE(CalcPrivateBlendStateSize10_1);
		UNPACK_VALUE(CreateBlendState10_1);
		UNPACK_VALUE(ResourceConvert10_1);
		UNPACK_VALUE(ResourceConvertRegion10_1);

		//D3D11_0Functions
		UNPACK_VALUE(SetRenderTargets11_0);
		UNPACK_VALUE(RelocateDeviceFuncs11_0);
		UNPACK_VALUE(CalcPrivateResourceSize11_0);
		UNPACK_VALUE(CreateResource11_0);
		UNPACK_VALUE(CalcPrivateShaderResourceViewSize11_0);
		UNPACK_VALUE(CreateShaderResourceView11_0);
		UNPACK_VALUE(CalcPrivateDepthStencilViewSize11_0);
		UNPACK_VALUE(CreateDepthStencilView11_0);
		UNPACK_VALUE(CalcPrivateGeometryShaderWithStreamOutput11_0);
		UNPACK_VALUE(CreateGeometryShaderWithStreamOutput11_0);
		UNPACK_VALUE(DrawIndexedInstancedIndirect11_0);
		UNPACK_VALUE(DrawInstancedIndirect11_0);
		UNPACK_VALUE(CommandListExecute11_0);
		UNPACK_VALUE(HsSetShaderResources11_0);
		UNPACK_VALUE(HsSetShader11_0);
		UNPACK_VALUE(HsSetSamplers11_0);
		UNPACK_VALUE(HsSetConstantBuffers11_0);
		UNPACK_VALUE(DsSetShaderResources11_0);
		UNPACK_VALUE(DsSetShader11_0);
		UNPACK_VALUE(DsSetSamplers11_0);
		UNPACK_VALUE(DsSetConstantBuffers11_0);
		UNPACK_VALUE(CreateHullShader11_0);
		UNPACK_VALUE(CreateDomainShader11_0);
		UNPACK_VALUE(CheckDeferredContextHandleSizes11_0);
		UNPACK_VALUE(CalcDeferredContextHandleSize11_0);
		UNPACK_VALUE(CalcPrivateDeferredContextSize11_0);
		UNPACK_VALUE(CreateDeferredContext11_0);
		UNPACK_VALUE(AbandonCommandList11_0);
		UNPACK_VALUE(CalcPrivateCommandListSize11_0);
		UNPACK_VALUE(CreateCommandList11_0);
		UNPACK_VALUE(DestroyCommandList11_0);
		UNPACK_VALUE(CalcPrivateTessellationShaderSize11_0);
		UNPACK_VALUE(PsSetShaderWithIfaces11_0);
		UNPACK_VALUE(VsSetShaderWithIfaces11_0);
		UNPACK_VALUE(GsSetShaderWithIfaces11_0);
		UNPACK_VALUE(HsSetShaderWithIfaces11_0);
		UNPACK_VALUE(DsSetShaderWithIfaces11_0);
		UNPACK_VALUE(CsSetShaderWithIfaces11_0);
		UNPACK_VALUE(CreateComputeShader11_0);
		UNPACK_VALUE(CsSetShader11_0);
		UNPACK_VALUE(CsSetShaderResources11_0);
		UNPACK_VALUE(CsSetSamplers11_0);
		UNPACK_VALUE(CsSetConstantBuffers11_0);
		UNPACK_VALUE(CalcPrivateUnorderedAccessViewSize11_0);
		UNPACK_VALUE(CreateUnorderedAccessView11_0);
		UNPACK_VALUE(DestroyUnorderedAccessView11_0);
		UNPACK_VALUE(ClearUnorderedAccessViewUint11_0);
		UNPACK_VALUE(ClearUnorderedAccessViewFloat11_0);
		UNPACK_VALUE(CsSetUnorderedAccessViews11_0);
		UNPACK_VALUE(Dispatch11_0);
		UNPACK_VALUE(DispatchIndirect11_0);
		UNPACK_VALUE(SetResourceMinLOD11_0);
		UNPACK_VALUE(CopyStructureCount11_0);

		//Driver commands
		UNPACK_VALUE(ChangeDeviceView);
		UNPACK_VALUE(ChangeShaderMatrixView);
		UNPACK_VALUE(SetDestResourceType);
	}
#undef UNPACK_VALUE
	return "UNKNOWN";
}

#define UNPACK_VALUE(VALUE) case VALUE: return _T(#VALUE)

const TCHAR * GetDXGIModeString( const DXGI_MODE_DESC *pMode )
{
	if (!pMode)
	{
		return _T("NULL");
	}
	return dbg_sprintf(
		_T("Width = %d, Height = %d, RefreshRate = %d/%d, Format = %S, ScanlineOrdering = %S, Scaling = %S"),
		pMode->Width, pMode->Height, pMode->RefreshRate.Numerator, pMode->RefreshRate.Denominator, 
		EnumToString(pMode->Format), EnumToString(pMode->ScanlineOrdering), EnumToString(pMode->Scaling));
}

const TCHAR * GetDXGISampleDescString( const DXGI_SAMPLE_DESC *pDesc )
{
	if (!pDesc)
	{
		return _T("NULL");
	}
	return dbg_sprintf(_T("Count = %d, Quality = %d"), pDesc->Count, pDesc->Quality);
}

const TCHAR * GetDXGISwapChainDescString( const DXGI_SWAP_CHAIN_DESC *pDesc )
{
	if (!pDesc)
	{
		return _T("NULL");
	}
	return dbg_sprintfEx(512, 
		_T("\n\tBufferDesc = (%s), \n\tSampleDesc = (%s), BufferUsage = %d, BufferCount = %d, \
OutputWindow = %p, Windowed = %d, SwapEffect = %S, Flags = %d"),
		GetDXGIModeString(&pDesc->BufferDesc), GetDXGISampleDescString(&pDesc->SampleDesc), pDesc->BufferUsage, pDesc->BufferCount, 
		pDesc->OutputWindow, pDesc->Windowed, EnumToString(pDesc->SwapEffect), pDesc->Flags );
}

const TCHAR *GetDXGIBltArgString(const DXGI_DDI_ARG_BLT *pDesc)
{
	if (!pDesc)
	{
		return _T("NULL");
	}
	return dbg_sprintfEx(512, 
		_T("\n\thDevice = %p,\
		    \n\thDstResource = %p,\
			\n\tDstSubresource = %d,\
			\n\tDstLeft = %d,\
			\n\tDstTop = %d,\
			\n\tDstRight = %d,\
			\n\tDstBottom = %d,\
			\n\thSrcResource = %p,\
			\n\tSrcSubresource = %d\
			\n\tFlags = %d,\
			\n\tRotate = %s"),
		pDesc->hDevice,
		pDesc->hDstResource,
		pDesc->DstSubresource,
		pDesc->DstLeft,
		pDesc->DstTop,
		pDesc->DstRight,
		pDesc->DstBottom,
		pDesc->hSrcResource,
		pDesc->SrcSubresource,
		pDesc->Flags,
		EnumToString(pDesc->Rotate));
}

const TCHAR *GetBoxString(CONST D3D10_BOX * pBox)
{
	if (pBox == NULL)
	{
		return _T("NULL");
	}
	return dbg_sprintf(_T("(%ld,%ld)-(%ld,%ld)-(%ld,%ld)"),	pBox->left, pBox->top, pBox->right, pBox->bottom, pBox->back, pBox->front);
}

const TCHAR *GetBoxString(CONST D3D10_DDI_BOX * pBox)
{
	if (pBox == NULL)
	{
		return _T("NULL");
	}
	return dbg_sprintf(_T("(%ld,%ld)-(%ld,%ld)-(%ld,%ld)"),	pBox->left, pBox->top, pBox->right, pBox->bottom, pBox->back, pBox->front);
}

const TCHAR *GetMatrixString(CONST D3DMATRIX *mat, bool bTranspose)
{
	if (!bTranspose)
		return dbg_sprintf(
		_T("%12.7f %12.7f %12.7f %12.7f\n")
		_T("%12.7f %12.7f %12.7f %12.7f\n")
		_T("%12.7f %12.7f %12.7f %12.7f\n")
		_T("%12.7f %12.7f %12.7f %12.7f\n"),
		mat->_11, mat->_12, mat->_13, mat->_14,
		mat->_21, mat->_22, mat->_23, mat->_24,
		mat->_31, mat->_32, mat->_33, mat->_34,
		mat->_41, mat->_42, mat->_43, mat->_44 );
	else
		return dbg_sprintf(
		_T("%12.7f %12.7f %12.7f %12.7f\n")
		_T("%12.7f %12.7f %12.7f %12.7f\n")
		_T("%12.7f %12.7f %12.7f %12.7f\n")
		_T("%12.7f %12.7f %12.7f %12.7f\n"),
		mat->_11, mat->_21, mat->_31, mat->_41,
		mat->_12, mat->_22, mat->_32, mat->_42,
		mat->_13, mat->_23, mat->_33, mat->_43,
		mat->_14, mat->_24, mat->_34, mat->_44 );
}

const TCHAR *GetFlipIntervalString( DXGI_DDI_FLIP_INTERVAL_TYPE Interval ) 
{
	switch ( Interval )
	{	
		UNPACK_VALUE(DXGI_DDI_FLIP_INTERVAL_IMMEDIATE);
		UNPACK_VALUE(DXGI_DDI_FLIP_INTERVAL_ONE);
		UNPACK_VALUE(DXGI_DDI_FLIP_INTERVAL_TWO);
		UNPACK_VALUE(DXGI_DDI_FLIP_INTERVAL_THREE);
		UNPACK_VALUE(DXGI_DDI_FLIP_INTERVAL_FOUR);
	}
	return _T("UNKNOWN");
}

const char* EnumToString( DXGI_FORMAT nVal )
{
	static const char *enum_name[] = 
	{	
		"DXGI_FORMAT_UNKNOWN",
		"DXGI_FORMAT_R32G32B32A32_TYPELESS",
		"DXGI_FORMAT_R32G32B32A32_FLOAT",
		"DXGI_FORMAT_R32G32B32A32_UINT",
		"DXGI_FORMAT_R32G32B32A32_SINT",
		"DXGI_FORMAT_R32G32B32_TYPELESS",
		"DXGI_FORMAT_R32G32B32_FLOAT",
		"DXGI_FORMAT_R32G32B32_UINT",
		"DXGI_FORMAT_R32G32B32_SINT",
		"DXGI_FORMAT_R16G16B16A16_TYPELESS",
		"DXGI_FORMAT_R16G16B16A16_FLOAT",
		"DXGI_FORMAT_R16G16B16A16_UNORM",
		"DXGI_FORMAT_R16G16B16A16_UINT",
		"DXGI_FORMAT_R16G16B16A16_SNORM",
		"DXGI_FORMAT_R16G16B16A16_SINT",
		"DXGI_FORMAT_R32G32_TYPELESS",
		"DXGI_FORMAT_R32G32_FLOAT",
		"DXGI_FORMAT_R32G32_UINT",
		"DXGI_FORMAT_R32G32_SINT",
		"DXGI_FORMAT_R32G8X24_TYPELESS",
		"DXGI_FORMAT_D32_FLOAT_S8X24_UINT",
		"DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS",
		"DXGI_FORMAT_X32_TYPELESS_G8X24_UINT",
		"DXGI_FORMAT_R10G10B10A2_TYPELESS",
		"DXGI_FORMAT_R10G10B10A2_UNORM",
		"DXGI_FORMAT_R10G10B10A2_UINT",
		"DXGI_FORMAT_R11G11B10_FLOAT",
		"DXGI_FORMAT_R8G8B8A8_TYPELESS",
		"DXGI_FORMAT_R8G8B8A8_UNORM",
		"DXGI_FORMAT_R8G8B8A8_UNORM_SRGB",
		"DXGI_FORMAT_R8G8B8A8_UINT",
		"DXGI_FORMAT_R8G8B8A8_SNORM",
		"DXGI_FORMAT_R8G8B8A8_SINT",
		"DXGI_FORMAT_R16G16_TYPELESS",
		"DXGI_FORMAT_R16G16_FLOAT",
		"DXGI_FORMAT_R16G16_UNORM",
		"DXGI_FORMAT_R16G16_UINT",
		"DXGI_FORMAT_R16G16_SNORM",
		"DXGI_FORMAT_R16G16_SINT",
		"DXGI_FORMAT_R32_TYPELESS",
		"DXGI_FORMAT_D32_FLOAT",
		"DXGI_FORMAT_R32_FLOAT",
		"DXGI_FORMAT_R32_UINT",
		"DXGI_FORMAT_R32_SINT",
		"DXGI_FORMAT_R24G8_TYPELESS",
		"DXGI_FORMAT_D24_UNORM_S8_UINT",
		"DXGI_FORMAT_R24_UNORM_X8_TYPELESS",
		"DXGI_FORMAT_X24_TYPELESS_G8_UINT",
		"DXGI_FORMAT_R8G8_TYPELESS",
		"DXGI_FORMAT_R8G8_UNORM",
		"DXGI_FORMAT_R8G8_UINT",
		"DXGI_FORMAT_R8G8_SNORM",
		"DXGI_FORMAT_R8G8_SINT",
		"DXGI_FORMAT_R16_TYPELESS",
		"DXGI_FORMAT_R16_FLOAT",
		"DXGI_FORMAT_D16_UNORM",
		"DXGI_FORMAT_R16_UNORM",
		"DXGI_FORMAT_R16_UINT",
		"DXGI_FORMAT_R16_SNORM",
		"DXGI_FORMAT_R16_SINT",
		"DXGI_FORMAT_R8_TYPELESS",
		"DXGI_FORMAT_R8_UNORM",
		"DXGI_FORMAT_R8_UINT",
		"DXGI_FORMAT_R8_SNORM",
		"DXGI_FORMAT_R8_SINT",
		"DXGI_FORMAT_A8_UNORM",
		"DXGI_FORMAT_R1_UNORM",
		"DXGI_FORMAT_R9G9B9E5_SHAREDEXP",
		"DXGI_FORMAT_R8G8_B8G8_UNORM",
		"DXGI_FORMAT_G8R8_G8B8_UNORM",
		"DXGI_FORMAT_BC1_TYPELESS",
		"DXGI_FORMAT_BC1_UNORM",
		"DXGI_FORMAT_BC1_UNORM_SRGB",
		"DXGI_FORMAT_BC2_TYPELESS",
		"DXGI_FORMAT_BC2_UNORM",
		"DXGI_FORMAT_BC2_UNORM_SRGB",
		"DXGI_FORMAT_BC3_TYPELESS",
		"DXGI_FORMAT_BC3_UNORM",
		"DXGI_FORMAT_BC3_UNORM_SRGB",
		"DXGI_FORMAT_BC4_TYPELESS",
		"DXGI_FORMAT_BC4_UNORM",
		"DXGI_FORMAT_BC4_SNORM",
		"DXGI_FORMAT_BC5_TYPELESS",
		"DXGI_FORMAT_BC5_UNORM",
		"DXGI_FORMAT_BC5_SNORM",
		"DXGI_FORMAT_B5G6R5_UNORM",
		"DXGI_FORMAT_B5G5R5A1_UNORM",
		"DXGI_FORMAT_B8G8R8A8_UNORM",
		"DXGI_FORMAT_B8G8R8X8_UNORM",
		"DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM ",
		"DXGI_FORMAT_B8G8R8A8_TYPELESS",
		"DXGI_FORMAT_B8G8R8A8_UNORM_SRGB",
		"DXGI_FORMAT_B8G8R8X8_TYPELESS",
		"DXGI_FORMAT_B8G8R8X8_UNORM_SRGB",
		"DXGI_FORMAT_BC6H_TYPELESS",
		"DXGI_FORMAT_BC6H_UF16",
		"DXGI_FORMAT_BC6H_SF16",
		"DXGI_FORMAT_BC7_TYPELESS",
		"DXGI_FORMAT_BC7_UNORM",
		"DXGI_FORMAT_BC7_UNORM_SRGB",
		"DXGI_FORMAT_FORCE_UINT"
	};
	return ( nVal >= 0 ) ? enum_name[nVal] : "INVALID_FORMAT";
}
