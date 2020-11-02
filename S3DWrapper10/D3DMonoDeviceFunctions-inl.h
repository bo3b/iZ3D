#pragma once

namespace MonoDeviceWrapper
{
	#undef  D3D10_DEVICE
	#undef  D3D10_GET_ORIG
	#undef  D3D10_1_GET_ORIG
	#undef  D3D11_GET_ORIG
	#define D3D10_GET_MONO_WRAPPER()	((D3DMonoDeviceWrapper*)hDevice.pDrvPrivate)
	#define D3D10_GET_ORIG()			D3D10_GET_MONO_WRAPPER()->OriginalDeviceFuncs
	#define D3D10_1_GET_ORIG()			D3D10_GET_MONO_WRAPPER()->OriginalDeviceFuncs10_1
	#define D3D11_GET_ORIG()			D3D10_GET_MONO_WRAPPER()->OriginalDeviceFuncs11
	#define D3D10_DEVICE				D3D10_GET_MONO_WRAPPER()->hDevice

	#undef  DXGI_MODIFY_DEVICE
	#undef  DXGI_DEVICE
	#undef  DXGI_GET_ORIG
	#undef  DXGI_GET_ORIG2
	#undef  DXGI_RESTORE_DEVICE
	#define DXGI_GET_MONO_WRAPPER()				((D3DMonoDeviceWrapper*)hOrigDevice)
	#define DXGI_MODIFY_DEVICE(x)				DXGI_DDI_HDEVICE  hOrigDevice = x->hDevice; \
		x->hDevice = (DXGI_DDI_HDEVICE)DXGI_GET_MONO_WRAPPER()->hDevice.pDrvPrivate;
	#define DXGI_DEVICE							((DXGI_DDI_HDEVICE)DXGI_GET_MONO_WRAPPER()->hDevice.pDrvPrivate)
	#define DXGI_GET_ORIG()						DXGI_GET_MONO_WRAPPER()->OriginalDXGIDDIBaseFunctions
	#define DXGI_GET_ORIG2()					DXGI_GET_MONO_WRAPPER()->OriginalDXGIDDIBaseFunctions2
	#define DXGI_RESTORE_DEVICE(x)				x->hDevice = hOrigDevice;

	VOID ( APIENTRY DefaultConstantBufferUpdateSubresourceUP )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HRESOURCE hDstResource, 
		UINT DstSubresource, CONST D3D10_DDI_BOX* pDstBox, CONST VOID* pSysMemUP, UINT RowPitch, UINT DepthPitch )
	{
		DEBUG_TRACE3(_T("DefaultConstantBufferUpdateSubresourceUP\n"));
		D3D10_GET_ORIG().pfnDefaultConstantBufferUpdateSubresourceUP( D3D10_DEVICE, hDstResource, DstSubresource, pDstBox, pSysMemUP, RowPitch, DepthPitch );
	}

	VOID ( APIENTRY VsSetConstantBuffers )( D3D10DDI_HDEVICE hDevice, UINT StartBuffer, UINT NumBuffers, CONST D3D10DDI_HRESOURCE* phBuffers )
	{
		DEBUG_TRACE3(_T("VsSetConstantBuffers\n"));
		D3D10_GET_ORIG().pfnVsSetConstantBuffers(D3D10_DEVICE, StartBuffer, NumBuffers, phBuffers );
	}

	VOID ( APIENTRY PsSetShaderResources )( D3D10DDI_HDEVICE hDevice, UINT Offset, UINT NumViews, CONST D3D10DDI_HSHADERRESOURCEVIEW* phShaderResourceViews )
	{
		DEBUG_TRACE3(_T("PsSetShaderResources\n"));
		D3D10_GET_ORIG().pfnPsSetShaderResources( D3D10_DEVICE, Offset, NumViews, phShaderResourceViews );
	}

	VOID ( APIENTRY PsSetShader )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HSHADER  hShader )
	{	
		DEBUG_TRACE3(_T("PsSetShader\n"));
		D3D10_GET_ORIG().pfnPsSetShader( D3D10_DEVICE, hShader );
	}

	VOID ( APIENTRY PsSetSamplers )( D3D10DDI_HDEVICE hDevice, UINT Offset, UINT NumSamplers, CONST D3D10DDI_HSAMPLER* phSamplers )
	{
		DEBUG_TRACE3(_T("PsSetSamplers\n"));
		D3D10_GET_ORIG().pfnPsSetSamplers( D3D10_DEVICE, Offset, NumSamplers, phSamplers );
	}

	VOID ( APIENTRY VsSetShader )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HSHADER hShader )
	{
		DEBUG_TRACE3(_T("VsSetShader\n"));
		D3D10_GET_ORIG().pfnVsSetShader(D3D10_DEVICE, hShader);
	}

	VOID ( APIENTRY DrawIndexed )( D3D10DDI_HDEVICE hDevice, UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation )
	{
		DEBUG_TRACE3(_T("DrawIndexed\n"));
		D3D10_GET_ORIG().pfnDrawIndexed( D3D10_DEVICE, IndexCount, StartIndexLocation, BaseVertexLocation );
	}

	VOID ( APIENTRY Draw )( D3D10DDI_HDEVICE hDevice, UINT VertexCount, UINT StartVertexLocation )
	{
		DEBUG_TRACE3(_T("Draw\n"));
		D3D10_GET_ORIG().pfnDraw( D3D10_DEVICE, VertexCount, StartVertexLocation );
	}

	VOID ( APIENTRY DynamicIABufferMapNoOverwrite )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HRESOURCE hResource, 
		UINT Subresource, D3D10_DDI_MAP DDIMap, UINT Flags, D3D10DDI_MAPPED_SUBRESOURCE* pMappedSubResource )
	{
		DEBUG_TRACE3(_T("DynamicIABufferMapNoOverwrite\n"));
		D3D10_GET_ORIG().pfnDynamicIABufferMapNoOverwrite( D3D10_DEVICE, hResource, Subresource, DDIMap, Flags, pMappedSubResource );
	}

	VOID ( APIENTRY DynamicIABufferUnmap )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HRESOURCE hResource, UINT Subresource )
	{
		DEBUG_TRACE3(_T("DynamicIABufferUnmap\n"));
		D3D10_GET_ORIG().pfnDynamicIABufferUnmap( D3D10_DEVICE, hResource, Subresource );
	}

	VOID ( APIENTRY DynamicConstantBufferMapDiscard )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HRESOURCE hResource, UINT Subresource, 
		D3D10_DDI_MAP DDIMap, UINT Flags, D3D10DDI_MAPPED_SUBRESOURCE* pMappedSubResource )
	{
		DEBUG_TRACE3(_T("DynamicConstantBufferMapDiscard\n"));
		D3D10_GET_ORIG().pfnDynamicConstantBufferMapDiscard( D3D10_DEVICE, hResource, Subresource, DDIMap, Flags, pMappedSubResource );
	}
	
	VOID ( APIENTRY DynamicIABufferMapDiscard )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HRESOURCE hResource, UINT Subresource,
		D3D10_DDI_MAP DDIMap, UINT Flags, D3D10DDI_MAPPED_SUBRESOURCE* pMappedSubResource )
	{
		DEBUG_TRACE3(_T("DynamicIABufferMapDiscard\n"));
		D3D10_GET_ORIG().pfnDynamicIABufferMapDiscard( D3D10_DEVICE, hResource, Subresource, DDIMap, Flags, pMappedSubResource );
	}

	VOID ( APIENTRY DynamicConstantBufferUnmap )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HRESOURCE hResource, UINT Subresource )
	{
		DEBUG_TRACE3(_T("DynamicConstantBufferUnmap\n"));
		D3D10_GET_ORIG().pfnDynamicConstantBufferUnmap( D3D10_DEVICE, hResource, Subresource );
	}

	VOID ( APIENTRY PsSetConstantBuffers )( D3D10DDI_HDEVICE  hDevice, UINT StartBuffer, UINT NumBuffers, CONST D3D10DDI_HRESOURCE* phBuffers )
	{
		DEBUG_TRACE3(_T("PsSetConstantBuffers\n"));
		D3D10_GET_ORIG().pfnPsSetConstantBuffers( D3D10_DEVICE, StartBuffer, NumBuffers, phBuffers );
	}

	VOID ( APIENTRY IaSetInputLayout )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HELEMENTLAYOUT hInputLayout )
	{
		DEBUG_TRACE3(_T("IaSetInputLayout\n"));
		D3D10_GET_ORIG().pfnIaSetInputLayout( D3D10_DEVICE, hInputLayout );
	}
	
	VOID ( APIENTRY IaSetVertexBuffers )( D3D10DDI_HDEVICE hDevice, UINT StartBuffer, UINT NumBuffers, CONST D3D10DDI_HRESOURCE* phBuffers, 
		CONST UINT* pStrides, CONST UINT* pOffsets )
	{
		DEBUG_TRACE3(_T("IaSetVertexBuffers\n"));	
		D3D10_GET_ORIG().pfnIaSetVertexBuffers(D3D10_DEVICE, StartBuffer, NumBuffers, phBuffers, pStrides, pOffsets);
	}

	VOID ( APIENTRY IaSetIndexBuffer )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HRESOURCE hBuffer, DXGI_FORMAT Format, UINT Offset )
	{
		DEBUG_TRACE3(_T("IaSetIndexBuffer\n"));	
		D3D10_GET_ORIG().pfnIaSetIndexBuffer(D3D10_DEVICE, hBuffer, Format, Offset);
	}

	VOID ( APIENTRY DrawIndexedInstanced )( D3D10DDI_HDEVICE hDevice, UINT IndexCountPerInstance, UINT InstanceCount, 
		UINT StartIndexLocation, INT BaseVertexLocation, UINT StartInstanceLocation )
	{
		DEBUG_TRACE3(_T("DrawIndexedInstanced\n"));
		D3D10_GET_ORIG().pfnDrawIndexedInstanced( D3D10_DEVICE, IndexCountPerInstance, InstanceCount, StartIndexLocation, BaseVertexLocation, StartInstanceLocation );
	}

	VOID ( APIENTRY DrawInstanced )( D3D10DDI_HDEVICE hDevice, UINT VertexCountPerInstance, UINT InstanceCount, UINT StartVertexLocation, UINT StartInstanceLocation )
	{
		DEBUG_TRACE3(_T("DrawInstanced\n"));
		D3D10_GET_ORIG().pfnDrawInstanced( D3D10_DEVICE, VertexCountPerInstance, InstanceCount, StartVertexLocation, StartInstanceLocation );
	}

	VOID ( APIENTRY DynamicResourceMapDiscard )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HRESOURCE hResource, UINT Subresource,
		D3D10_DDI_MAP DDIMap, UINT Flags, D3D10DDI_MAPPED_SUBRESOURCE* pMappedSubResource )
	{
		DEBUG_TRACE3(_T("DynamicResourceMapDiscard\n"));
		D3D10_GET_ORIG().pfnDynamicResourceMapDiscard( D3D10_DEVICE, hResource, Subresource, DDIMap, Flags, pMappedSubResource );
	}

	VOID ( APIENTRY DynamicResourceUnmap )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HRESOURCE hResource, UINT Subresource )
	{
		DEBUG_TRACE3(_T("DynamicResourceUnmap\n"));
		D3D10_GET_ORIG().pfnDynamicResourceUnmap( D3D10_DEVICE, hResource, Subresource );
	}

	VOID ( APIENTRY GsSetConstantBuffers )( D3D10DDI_HDEVICE hDevice, UINT StartBuffer, UINT NumBuffers, CONST D3D10DDI_HRESOURCE* phBuffers )
	{
		DEBUG_TRACE3(_T("GsSetConstantBuffers\n"));	
		D3D10_GET_ORIG().pfnGsSetConstantBuffers( D3D10_DEVICE, StartBuffer, NumBuffers, phBuffers );
	}

	VOID ( APIENTRY GsSetShader )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HSHADER hShader )
	{
		DEBUG_TRACE3(_T("GsSetShader\n"));
		D3D10_GET_ORIG().pfnGsSetShader( D3D10_DEVICE, hShader );
	}

	VOID ( APIENTRY IaSetTopology )( D3D10DDI_HDEVICE hDevice, D3D10_DDI_PRIMITIVE_TOPOLOGY PrimitiveTopology )
	{
		DEBUG_TRACE3(_T("IaSetTopology\n"));
		D3D10_GET_ORIG().pfnIaSetTopology( D3D10_DEVICE, PrimitiveTopology );
	}

	VOID ( APIENTRY StagingResourceMap )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HRESOURCE hResource, UINT Subresource, D3D10_DDI_MAP DDIMap, 
		UINT Flags, D3D10DDI_MAPPED_SUBRESOURCE* pMappedSubResource )
	{
		DEBUG_TRACE3(_T("StagingResourceMap\n"));
		D3D10_GET_ORIG().pfnStagingResourceMap( D3D10_DEVICE, hResource, Subresource, DDIMap, Flags, pMappedSubResource	);
	}

	VOID ( APIENTRY StagingResourceUnmap )(	D3D10DDI_HDEVICE hDevice, D3D10DDI_HRESOURCE hResource, UINT Subresource )
	{
		DEBUG_TRACE3(_T("StagingResourceUnmap\n"));
		D3D10_GET_ORIG().pfnStagingResourceUnmap( D3D10_DEVICE,	hResource, Subresource );
	}
	
	VOID ( APIENTRY VsSetShaderResources )( D3D10DDI_HDEVICE hDevice, UINT Offset, UINT NumViews, CONST D3D10DDI_HSHADERRESOURCEVIEW* phShaderResourceViews )
	{
		DEBUG_TRACE3(_T("VsSetShaderResources\n"));
		D3D10_GET_ORIG().pfnVsSetShaderResources( D3D10_DEVICE, Offset, NumViews, phShaderResourceViews );
	}

	VOID ( APIENTRY VsSetSamplers )( D3D10DDI_HDEVICE hDevice, UINT Offset, UINT NumSamplers, CONST D3D10DDI_HSAMPLER* phSamplers )
	{
		DEBUG_TRACE3(_T("VsSetSamplers\n"));
		D3D10_GET_ORIG().pfnVsSetSamplers( D3D10_DEVICE, Offset, NumSamplers, phSamplers );
	}

	VOID ( APIENTRY GsSetShaderResources )( D3D10DDI_HDEVICE hDevice, UINT Offset, UINT NumViews, CONST D3D10DDI_HSHADERRESOURCEVIEW* phShaderResourceViews )
	{
		DEBUG_TRACE3(_T("GsSetShaderResources\n"));
		D3D10_GET_ORIG().pfnGsSetShaderResources( D3D10_DEVICE, Offset, NumViews, phShaderResourceViews );
	}

	VOID ( APIENTRY GsSetSamplers )( D3D10DDI_HDEVICE hDevice, UINT Offset, UINT NumSamplers, CONST D3D10DDI_HSAMPLER* phSamplers )
	{
		DEBUG_TRACE3(_T("GsSetSamplers\n"));
		D3D10_GET_ORIG().pfnGsSetSamplers( D3D10_DEVICE, Offset, NumSamplers, phSamplers );
	}

	VOID ( APIENTRY SetRenderTargets )( D3D10DDI_HDEVICE hDevice,  CONST D3D10DDI_HRENDERTARGETVIEW* phRenderTargetView, 
		UINT RTargets, UINT ClearTargets, D3D10DDI_HDEPTHSTENCILVIEW hDepthStencilView )
	{
		DEBUG_TRACE3(_T("SetRenderTargets\n"));	
		D3D10_GET_ORIG().pfnSetRenderTargets( D3D10_DEVICE, phRenderTargetView, RTargets, ClearTargets, hDepthStencilView );
	}
	
	VOID ( APIENTRY ShaderResourceViewReadAfterWriteHazard )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HSHADERRESOURCEVIEW hShaderResourceView, D3D10DDI_HRESOURCE hResource )
	{
		DEBUG_TRACE3(_T("ShaderResourceViewReadAfterWriteHazard\n"));
		D3D10_GET_ORIG().pfnShaderResourceViewReadAfterWriteHazard( D3D10_DEVICE, hShaderResourceView, hResource );
	}

	VOID ( APIENTRY ResourceReadAfterWriteHazard )(	D3D10DDI_HDEVICE hDevice, D3D10DDI_HRESOURCE hResource )
	{
		DEBUG_TRACE3(_T("ResourceReadAfterWriteHazard\n"));
		D3D10_GET_ORIG().pfnResourceReadAfterWriteHazard( D3D10_DEVICE, hResource );
	}

	VOID ( APIENTRY SetBlendState )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HBLENDSTATE hState, const FLOAT pBlendFactor[4], UINT SampleMask )
	{
		DEBUG_TRACE3(_T("SetBlendState\n"));
		D3D10_GET_ORIG().pfnSetBlendState( D3D10_DEVICE, hState, pBlendFactor, SampleMask );
	}

	VOID ( APIENTRY SetDepthStencilState )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HDEPTHSTENCILSTATE hState, UINT StencilRef )
	{
		DEBUG_TRACE3(_T("SetDepthStencilState\n"));
		D3D10_GET_ORIG().pfnSetDepthStencilState( D3D10_DEVICE, hState, StencilRef );
	}

	VOID ( APIENTRY SetRasterizerState )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HRASTERIZERSTATE hRasterizerState )
	{
		DEBUG_TRACE3(_T("SetRasterizerState\n"));
		D3D10_GET_ORIG().pfnSetRasterizerState( D3D10_DEVICE, hRasterizerState );
	}

	VOID ( APIENTRY QueryEnd )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HQUERY hQuery )
	{
		DEBUG_TRACE3(_T("QueryEnd\n"));
		D3D10_GET_ORIG().pfnQueryEnd( D3D10_DEVICE, hQuery );
	}
	
	VOID ( APIENTRY QueryBegin )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HQUERY hQuery )
	{
		DEBUG_TRACE3(_T("QueryBegin\n"));
		D3D10_GET_ORIG().pfnQueryBegin( D3D10_DEVICE, hQuery );
	}
	
	VOID ( APIENTRY ResourceCopyRegion )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HRESOURCE hDstResource, UINT DstSubresource, UINT DstX, 
		UINT DstY, UINT DstZ, D3D10DDI_HRESOURCE hSrcResource, UINT SrcSubresource, CONST D3D10_DDI_BOX* pSrcBox )
	{
		DEBUG_TRACE3(_T("ResourceCopyRegion\n"));
		D3D10_GET_ORIG().pfnResourceCopyRegion( D3D10_DEVICE, hDstResource, DstSubresource, DstX, DstY, DstZ, hSrcResource, 0, pSrcBox );
	}

	VOID ( APIENTRY ResourceUpdateSubresourceUP )( D3D10DDI_HDEVICE hDevice,  D3D10DDI_HRESOURCE hDstResource, UINT DstSubresource, 
		CONST D3D10_DDI_BOX* pDstBox, CONST VOID* pSysMemUP, UINT RowPitch, UINT DepthPitch	)
	{
		DEBUG_TRACE3(_T("ResourceUpdateSubresourceUP\n"));
		D3D10_GET_ORIG().pfnResourceUpdateSubresourceUP( D3D10_DEVICE,	hDstResource, DstSubresource, pDstBox, pSysMemUP, RowPitch, DepthPitch );
	}

	VOID ( APIENTRY SoSetTargets )( D3D10DDI_HDEVICE hDevice, UINT SOTargets, UINT ClearTargets, CONST D3D10DDI_HRESOURCE* phResource, CONST UINT* pOffsets )
	{
		DEBUG_TRACE3(_T("SoSetTargets\n"));	
		D3D10_GET_ORIG().pfnSoSetTargets( D3D10_DEVICE, SOTargets, ClearTargets, phResource, pOffsets );
	}

	VOID ( APIENTRY DrawAuto )( D3D10DDI_HDEVICE hDevice )
	{
		DEBUG_TRACE3(_T("DrawAuto\n"));
		D3D10_GET_ORIG().pfnDrawAuto( D3D10_DEVICE );
	}

	VOID ( APIENTRY SetViewports )( D3D10DDI_HDEVICE hDevice, UINT NumViewports, UINT ClearViewports, CONST D3D10_DDI_VIEWPORT* pViewports )
	{
		DEBUG_TRACE3(_T("SetViewports\n"));
		D3D10_GET_ORIG().pfnSetViewports( D3D10_DEVICE, NumViewports, ClearViewports, pViewports );
	}

	VOID ( APIENTRY SetScissorRects )( D3D10DDI_HDEVICE hDevice, UINT NumScissorRects, UINT ClearScissorRects, CONST D3D10_DDI_RECT* pRects )
	{
		DEBUG_TRACE3(_T("SetScissorRects\n"));
		D3D10_GET_ORIG().pfnSetScissorRects( D3D10_DEVICE, NumScissorRects, ClearScissorRects, pRects );
	}
	
	VOID ( APIENTRY ClearRenderTargetView )( D3D10DDI_HDEVICE  hDevice, D3D10DDI_HRENDERTARGETVIEW hRenderTargetView, FLOAT pColorRGBA[4] )
	{
		DEBUG_TRACE3(_T("ClearRenderTargetView\n"));
		D3D10_GET_ORIG().pfnClearRenderTargetView( D3D10_DEVICE, hRenderTargetView, pColorRGBA );
	}

	VOID ( APIENTRY ClearDepthStencilView )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HDEPTHSTENCILVIEW hDepthStencilView, UINT Flags, FLOAT Depth, UINT8 Stencil )
	{
		DEBUG_TRACE3(_T("ClearDepthStencilView\n"));
		D3D10_GET_ORIG().pfnClearDepthStencilView( D3D10_DEVICE, hDepthStencilView, Flags, Depth, Stencil );
	}
	
	VOID ( APIENTRY SetPredication )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HQUERY hQuery, BOOL PredicateValue	)
	{
		DEBUG_TRACE3(_T("SetPredication\n"));
		D3D10_GET_ORIG().pfnSetPredication( D3D10_DEVICE, hQuery, PredicateValue );
	}

	VOID ( APIENTRY QueryGetData )(	D3D10DDI_HDEVICE hDevice, D3D10DDI_HQUERY hQuery, VOID* pData, UINT DataSize, UINT Flags )
	{
		DEBUG_TRACE3(_T("QueryGetData\n"));
		D3D10_GET_ORIG().pfnQueryGetData( D3D10_DEVICE, hQuery, pData, DataSize, Flags );
	}

	VOID ( APIENTRY Flush )( D3D10DDI_HDEVICE hDevice )
	{
		DEBUG_TRACE3(_T("Flush\n"));
		D3D10_GET_ORIG().pfnFlush(D3D10_DEVICE);
	}

	VOID ( APIENTRY GenMips )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HSHADERRESOURCEVIEW hShaderResourceView )
	{
		DEBUG_TRACE3(_T("GenMips\n"));
		D3D10_GET_ORIG().pfnGenMips( D3D10_DEVICE, hShaderResourceView );
	}

	VOID ( APIENTRY ResourceCopy )(	D3D10DDI_HDEVICE hDevice, D3D10DDI_HRESOURCE hDstResource, D3D10DDI_HRESOURCE hSrcResource )
	{
		DEBUG_TRACE3(_T("ResourceCopy\n"));
		D3D10_GET_ORIG().pfnResourceCopy( D3D10_DEVICE, hDstResource, hSrcResource );
	}

	VOID ( APIENTRY ResourceResolveSubresource )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HRESOURCE hDstResource, UINT DstSubresource, 
		D3D10DDI_HRESOURCE hSrcResource, UINT SrcSubresource, DXGI_FORMAT ResolveFormat	)
	{
		DEBUG_TRACE3(_T("ResourceResolveSubresource\n"));	
		D3D10_GET_ORIG().pfnResourceResolveSubresource( D3D10_DEVICE, hDstResource, DstSubresource, hSrcResource, SrcSubresource, ResolveFormat	);
	}

	VOID ( APIENTRY ResourceMap )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HRESOURCE hResource, UINT Subresource, D3D10_DDI_MAP DDIMap, UINT Flags, D3D10DDI_MAPPED_SUBRESOURCE*  pMappedSubResource	)
	{
		DEBUG_TRACE3(_T("ResourceMap\n"));
		D3D10_GET_ORIG().pfnResourceMap( D3D10_DEVICE, hResource, Subresource, DDIMap, Flags, pMappedSubResource );
	}

	VOID ( APIENTRY ResourceUnmap )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HRESOURCE hResource, UINT Subresource )
	{
		DEBUG_TRACE3(_T("ResourceUnmap\n"));	
		D3D10_GET_ORIG().pfnResourceUnmap( D3D10_DEVICE, hResource, Subresource	);
	}

	BOOL ( APIENTRY ResourceIsStagingBusy )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HRESOURCE hResource	)
	{
		DEBUG_TRACE3(_T("ResourceIsStagingBusy\n"));
		return D3D10_GET_ORIG().pfnResourceIsStagingBusy( D3D10_DEVICE, hResource );
	}
	
	VOID ( APIENTRY RelocateDeviceFuncs )( D3D10DDI_HDEVICE hDevice,  struct D3D10DDI_DEVICEFUNCS* pDeviceFunctions )
	{
		DEBUG_TRACE3(_T("RelocateDeviceFuncs\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() == TD3DVersion_10_0);
		memcpy(pDeviceFunctions, &D3D10_GET_MONO_WRAPPER()->OriginalDeviceFuncs, sizeof D3D10DDI_DEVICEFUNCS); // restore
		D3D10_GET_ORIG().pfnRelocateDeviceFuncs(D3D10_DEVICE, pDeviceFunctions);
		D3D10_GET_MONO_WRAPPER()->HookDeviceFuncs( pDeviceFunctions );
	}

	SIZE_T ( APIENTRY CalcPrivateResourceSize )( D3D10DDI_HDEVICE hDevice, CONST D3D10DDIARG_CREATERESOURCE* pCreateResource )
	{
		DEBUG_TRACE3(_T("CalcPrivateResourceSize\n"));  
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() == TD3DVersion_10_0 || D3D10_GET_MONO_WRAPPER()->GetD3DVersion() == TD3DVersion_10_1);
		SIZE_T size = D3D10_GET_ORIG().pfnCalcPrivateResourceSize( D3D10_DEVICE, pCreateResource );
		return size;
	}

	SIZE_T ( APIENTRY CalcPrivateOpenedResourceSize )( D3D10DDI_HDEVICE hDevice, CONST D3D10DDIARG_OPENRESOURCE* pOpenResource )
	{
		DEBUG_TRACE3(_T("CalcPrivateOpenedResourceSize\n"));  
		SIZE_T size = D3D10_GET_ORIG().pfnCalcPrivateOpenedResourceSize( D3D10_DEVICE, pOpenResource );
		return size;
	}

	VOID ( APIENTRY CreateResource )( D3D10DDI_HDEVICE hDevice, CONST D3D10DDIARG_CREATERESOURCE* pCreateResource, D3D10DDI_HRESOURCE hResource, D3D10DDI_HRTRESOURCE hRTResource )
	{
		DEBUG_TRACE3(_T("CreateResource\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() == TD3DVersion_10_0 || D3D10_GET_MONO_WRAPPER()->GetD3DVersion() == TD3DVersion_10_1);
		D3D10_GET_ORIG().pfnCreateResource( D3D10_DEVICE, pCreateResource, hResource, hRTResource );
	}

	VOID ( APIENTRY OpenResource )( D3D10DDI_HDEVICE hDevice, CONST D3D10DDIARG_OPENRESOURCE* pOpenResource, D3D10DDI_HRESOURCE hResource, D3D10DDI_HRTRESOURCE hRTResource )
	{
		DEBUG_TRACE3(_T("OpenResource\n"));
		D3D10_GET_ORIG().pfnOpenResource( D3D10_DEVICE, pOpenResource, hResource, hRTResource );
	}

	VOID ( APIENTRY DestroyResource )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HRESOURCE hResource )
	{
		DEBUG_TRACE3(_T("DestroyResource\n"));
		D3D10_GET_ORIG().pfnDestroyResource( D3D10_DEVICE, hResource );
	}

	SIZE_T ( APIENTRY CalcPrivateShaderResourceViewSize )( D3D10DDI_HDEVICE hDevice, CONST D3D10DDIARG_CREATESHADERRESOURCEVIEW* pCreateShaderResourceView )
	{
		DEBUG_TRACE3(_T("CalcPrivateShaderResourceViewSize\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() == TD3DVersion_10_0);
		SIZE_T size = D3D10_GET_ORIG().pfnCalcPrivateShaderResourceViewSize( D3D10_DEVICE, pCreateShaderResourceView );
		return size;
	}

	VOID ( APIENTRY CreateShaderResourceView )( D3D10DDI_HDEVICE hDevice, CONST D3D10DDIARG_CREATESHADERRESOURCEVIEW* pCreateShaderResourceView, 
		D3D10DDI_HSHADERRESOURCEVIEW hShaderResourceView, D3D10DDI_HRTSHADERRESOURCEVIEW hRTShaderResourceView )
	{
		DEBUG_TRACE3(_T("CreateShaderResourceView\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() == TD3DVersion_10_0);
		D3D10_GET_ORIG().pfnCreateShaderResourceView( D3D10_DEVICE, pCreateShaderResourceView, hShaderResourceView, hRTShaderResourceView );
	}
	
	VOID ( APIENTRY DestroyShaderResourceView )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HSHADERRESOURCEVIEW hShaderResourceView )
	{
		DEBUG_TRACE3(_T("DestroyShaderResourceView\n"));
		D3D10_GET_ORIG().pfnDestroyShaderResourceView( D3D10_DEVICE, hShaderResourceView );
	}

	SIZE_T ( APIENTRY CalcPrivateRenderTargetViewSize )( D3D10DDI_HDEVICE  hDevice, CONST D3D10DDIARG_CREATERENDERTARGETVIEW* pCreateRenderTargetView )
	{
		DEBUG_TRACE3(_T("CalcPrivateRenderTargetViewSize\n"));
		SIZE_T size = D3D10_GET_ORIG().pfnCalcPrivateRenderTargetViewSize( D3D10_DEVICE, pCreateRenderTargetView );
		return size;
	}

	VOID ( APIENTRY CreateRenderTargetView )( D3D10DDI_HDEVICE hDevice, CONST D3D10DDIARG_CREATERENDERTARGETVIEW* pCreateRenderTargetView, 
		D3D10DDI_HRENDERTARGETVIEW hRenderTargetView, D3D10DDI_HRTRENDERTARGETVIEW hRTRenderTargetView )
	{
		DEBUG_TRACE3(_T("CreateRenderTargetView\n"));
		D3D10_GET_ORIG().pfnCreateRenderTargetView( D3D10_DEVICE, pCreateRenderTargetView, hRenderTargetView, hRTRenderTargetView );
	}

	VOID ( APIENTRY DestroyRenderTargetView )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HRENDERTARGETVIEW hRenderTargetView )
	{
		DEBUG_TRACE3(_T("DestroyRenderTargetView\n"));
		D3D10_GET_ORIG().pfnDestroyRenderTargetView( D3D10_DEVICE, hRenderTargetView );
	}

	SIZE_T ( APIENTRY CalcPrivateDepthStencilViewSize )( D3D10DDI_HDEVICE hDevice, CONST D3D10DDIARG_CREATEDEPTHSTENCILVIEW* pCreateDepthStencilView )
	{
		DEBUG_TRACE3(_T("CalcPrivateDepthStencilViewSize\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() == TD3DVersion_10_0 || D3D10_GET_MONO_WRAPPER()->GetD3DVersion() == TD3DVersion_10_1);
		SIZE_T size = D3D10_GET_ORIG().pfnCalcPrivateDepthStencilViewSize( D3D10_DEVICE, pCreateDepthStencilView );
		return size;
	}
	
	VOID ( APIENTRY CreateDepthStencilView )( D3D10DDI_HDEVICE hDevice, CONST D3D10DDIARG_CREATEDEPTHSTENCILVIEW* pCreateDepthStencilView, 
		D3D10DDI_HDEPTHSTENCILVIEW hDepthStencilView, D3D10DDI_HRTDEPTHSTENCILVIEW hRTDepthStencilView )
	{
		DEBUG_TRACE3(_T("CreateDepthStencilView\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() == TD3DVersion_10_0 || D3D10_GET_MONO_WRAPPER()->GetD3DVersion() == TD3DVersion_10_1);
		D3D10_GET_ORIG().pfnCreateDepthStencilView( D3D10_DEVICE, pCreateDepthStencilView, hDepthStencilView, hRTDepthStencilView);
	}

	VOID ( APIENTRY DestroyDepthStencilView )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HDEPTHSTENCILVIEW hDepthStencilView )
	{
		DEBUG_TRACE3(_T("DestroyDepthStencilView\n"));
		D3D10_GET_ORIG().pfnDestroyDepthStencilView( D3D10_DEVICE, hDepthStencilView );
	}

	SIZE_T ( APIENTRY CalcPrivateElementLayoutSize )( D3D10DDI_HDEVICE hDevice, CONST D3D10DDIARG_CREATEELEMENTLAYOUT* pCreateElementLayout )
	{
		DEBUG_TRACE3(_T("CalcPrivateElementLayoutSize\n"));
		SIZE_T size = D3D10_GET_ORIG().pfnCalcPrivateElementLayoutSize( D3D10_DEVICE, pCreateElementLayout );
		return size;
	}

	VOID ( APIENTRY CreateElementLayout )( D3D10DDI_HDEVICE hDevice, CONST D3D10DDIARG_CREATEELEMENTLAYOUT* pCreateElementLayout, 
		D3D10DDI_HELEMENTLAYOUT hElementLayout, D3D10DDI_HRTELEMENTLAYOUT hRTElementLayout )
	{
		DEBUG_TRACE3(_T("CreateElementLayout\n"));
		D3D10_GET_ORIG().pfnCreateElementLayout( D3D10_DEVICE, pCreateElementLayout,	hElementLayout, hRTElementLayout );
	}

	VOID ( APIENTRY DestroyElementLayout )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HELEMENTLAYOUT hElementLayout )
	{
		DEBUG_TRACE3(_T("DestroyElementLayout\n"));
		D3D10_GET_ORIG().pfnDestroyElementLayout( D3D10_DEVICE, hElementLayout );
	}

	SIZE_T ( APIENTRY CalcPrivateBlendStateSize )( D3D10DDI_HDEVICE hDevice, CONST D3D10_DDI_BLEND_DESC* pBlendDesc )
	{
		DEBUG_TRACE3(_T("CalcPrivateBlendStateSize\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() == TD3DVersion_10_0);
		SIZE_T size = D3D10_GET_ORIG().pfnCalcPrivateBlendStateSize( D3D10_DEVICE, pBlendDesc );
		return size;
	}

	VOID ( APIENTRY CreateBlendState )( D3D10DDI_HDEVICE hDevice, CONST D3D10_DDI_BLEND_DESC* pBlendDesc, 
		D3D10DDI_HBLENDSTATE hBlendState, D3D10DDI_HRTBLENDSTATE hRTBlendState )
	{
		DEBUG_TRACE3(_T("CreateBlendState\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() == TD3DVersion_10_0);
		D3D10_GET_ORIG().pfnCreateBlendState( D3D10_DEVICE, pBlendDesc, hBlendState, hRTBlendState );
	}

	VOID ( APIENTRY DestroyBlendState )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HBLENDSTATE hBlendState )
	{
		DEBUG_TRACE3(_T("DestroyBlendState\n"));
		D3D10_GET_ORIG().pfnDestroyBlendState( D3D10_DEVICE, hBlendState );
	}

	SIZE_T ( APIENTRY CalcPrivateDepthStencilStateSize )( D3D10DDI_HDEVICE hDevice, CONST D3D10_DDI_DEPTH_STENCIL_DESC* pDepthStencilDesc )
	{
		DEBUG_TRACE3(_T("CalcPrivateDepthStencilStateSize\n"));
		SIZE_T size = D3D10_GET_ORIG().pfnCalcPrivateDepthStencilStateSize( D3D10_DEVICE, pDepthStencilDesc );
		return size;
	}

	VOID ( APIENTRY CreateDepthStencilState )( D3D10DDI_HDEVICE hDevice, CONST D3D10_DDI_DEPTH_STENCIL_DESC* pDepthStencilDesc, 
		D3D10DDI_HDEPTHSTENCILSTATE hDepthStencilState, D3D10DDI_HRTDEPTHSTENCILSTATE hRTDepthStencilState )
	{
		DEBUG_TRACE3(_T("CreateDepthStencilState\n"));
		D3D10_GET_ORIG().pfnCreateDepthStencilState( D3D10_DEVICE, pDepthStencilDesc, hDepthStencilState, hRTDepthStencilState );
	}

	VOID ( APIENTRY DestroyDepthStencilState )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HDEPTHSTENCILSTATE hDepthStencilState )
	{
		DEBUG_TRACE3(_T("DestroyDepthStencilState\n"));
		D3D10_GET_ORIG().pfnDestroyDepthStencilState( D3D10_DEVICE, hDepthStencilState );
	}

	SIZE_T ( APIENTRY CalcPrivateRasterizerStateSize )( D3D10DDI_HDEVICE hDevice, CONST D3D10_DDI_RASTERIZER_DESC* pRasterizerDesc )
	{
		DEBUG_TRACE3(_T("CalcPrivateRasterizerStateSize\n"));  
		SIZE_T size = D3D10_GET_ORIG().pfnCalcPrivateRasterizerStateSize( D3D10_DEVICE, pRasterizerDesc );
		return size;
	}

	VOID ( APIENTRY CreateRasterizerState )( D3D10DDI_HDEVICE hDevice, CONST D3D10_DDI_RASTERIZER_DESC* pRasterizerDesc, 
		D3D10DDI_HRASTERIZERSTATE hRasterizerState, D3D10DDI_HRTRASTERIZERSTATE hRTRasterizerState )
	{
		DEBUG_TRACE3(_T("CreateRasterizerState\n"));
		D3D10_GET_ORIG().pfnCreateRasterizerState( D3D10_DEVICE, pRasterizerDesc, hRasterizerState, hRTRasterizerState );
	}

	VOID ( APIENTRY DestroyRasterizerState )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HRASTERIZERSTATE hRasterizerState )
	{
		DEBUG_TRACE3(_T("DestroyRasterizerState\n"));
		D3D10_GET_ORIG().pfnDestroyRasterizerState( D3D10_DEVICE, hRasterizerState );
	}

	SIZE_T ( APIENTRY CalcPrivateShaderSize )( D3D10DDI_HDEVICE hDevice, CONST UINT* pCode, CONST D3D10DDIARG_STAGE_IO_SIGNATURES* pSignatures )
	{
		DEBUG_TRACE3(_T("CalcPrivateShaderSize\n"));
		SIZE_T size = D3D10_GET_ORIG().pfnCalcPrivateShaderSize( D3D10_DEVICE, pCode, pSignatures );
		return size;
	}

	VOID ( APIENTRY CreateVertexShader )( D3D10DDI_HDEVICE hDevice, CONST UINT*  pCode, D3D10DDI_HSHADER hShader, 
		D3D10DDI_HRTSHADER hRTShader, CONST D3D10DDIARG_STAGE_IO_SIGNATURES* pSignatures)
	{
		DEBUG_TRACE3(_T("CreateVertexShader\n"));	
		D3D10_GET_ORIG().pfnCreateVertexShader( D3D10_DEVICE, pCode, hShader, hRTShader, pSignatures );
	}

	VOID ( APIENTRY CreateGeometryShader )( D3D10DDI_HDEVICE hDevice, CONST UINT* pCode, D3D10DDI_HSHADER hShader, 
		D3D10DDI_HRTSHADER hRTShader, CONST D3D10DDIARG_STAGE_IO_SIGNATURES* pSignatures)
	{
		DEBUG_TRACE3(_T("CreateGeometryShader\n"));
		D3D10_GET_ORIG().pfnCreateGeometryShader( D3D10_DEVICE, pCode, hShader, hRTShader, pSignatures );
	}

	VOID ( APIENTRY CreatePixelShader )( D3D10DDI_HDEVICE hDevice, CONST UINT* pCode, D3D10DDI_HSHADER hShader, 
		D3D10DDI_HRTSHADER hRTShader, CONST D3D10DDIARG_STAGE_IO_SIGNATURES* pSignatures)
	{
		DEBUG_TRACE3(_T("CreatePixelShader\n"));
		D3D10_GET_ORIG().pfnCreatePixelShader( D3D10_DEVICE, pCode, hShader, hRTShader, pSignatures);
	}

	SIZE_T ( APIENTRY CalcPrivateGeometryShaderWithStreamOutput )( D3D10DDI_HDEVICE hDevice, 
		CONST D3D10DDIARG_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT* pCreateGeometryShaderWithStreamOutput, CONST D3D10DDIARG_STAGE_IO_SIGNATURES* pSignatures)
	{
		DEBUG_TRACE3(_T("CalcPrivateGeometryShaderWithStreamOutput\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() == TD3DVersion_10_0 || D3D10_GET_MONO_WRAPPER()->GetD3DVersion() == TD3DVersion_10_1);
		SIZE_T size = D3D10_GET_ORIG().pfnCalcPrivateGeometryShaderWithStreamOutput(D3D10_DEVICE, pCreateGeometryShaderWithStreamOutput, pSignatures);
		return size;
	}

	VOID ( APIENTRY CreateGeometryShaderWithStreamOutput )( D3D10DDI_HDEVICE hDevice, 
		CONST D3D10DDIARG_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT* pCreateGeometryWithShaderOutput, 
		D3D10DDI_HSHADER hShader, D3D10DDI_HRTSHADER hRTShader, CONST D3D10DDIARG_STAGE_IO_SIGNATURES* pSignatures)
	{
		DEBUG_TRACE3(_T("CreateGeometryShaderWithStreamOutput\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() == TD3DVersion_10_0 || D3D10_GET_MONO_WRAPPER()->GetD3DVersion() == TD3DVersion_10_1);
		D3D10_GET_ORIG().pfnCreateGeometryShaderWithStreamOutput( D3D10_DEVICE, pCreateGeometryWithShaderOutput, hShader, hRTShader, pSignatures );
	}

	VOID ( APIENTRY DestroyShader )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HSHADER hShader )
	{
		DEBUG_TRACE3(_T("DestroyShader\n"));
		D3D10_GET_ORIG().pfnDestroyShader( D3D10_DEVICE, hShader );
	}

	SIZE_T ( APIENTRY CalcPrivateSamplerSize )( D3D10DDI_HDEVICE hDevice, CONST D3D10_DDI_SAMPLER_DESC* pSamplerDesc )
	{
		DEBUG_TRACE3(_T("CalcPrivateSamplerSize\n"));
		SIZE_T size = D3D10_GET_ORIG().pfnCalcPrivateSamplerSize(D3D10_DEVICE, pSamplerDesc);
		return size;
	}


	VOID ( APIENTRY CreateSampler )( D3D10DDI_HDEVICE hDevice, CONST D3D10_DDI_SAMPLER_DESC* pSamplerDesc, D3D10DDI_HSAMPLER hSampler, D3D10DDI_HRTSAMPLER hRTSampler)
	{
		DEBUG_TRACE3(_T("CreateSampler\n"));
		D3D10_GET_ORIG().pfnCreateSampler( D3D10_DEVICE, pSamplerDesc, hSampler, hRTSampler );
	}

	VOID ( APIENTRY DestroySampler )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HSAMPLER hSampler )
	{
		DEBUG_TRACE3(_T("DestroySampler\n"));
		D3D10_GET_ORIG().pfnDestroySampler( D3D10_DEVICE, hSampler );
	}

	SIZE_T ( APIENTRY CalcPrivateQuerySize )( D3D10DDI_HDEVICE hDevice, CONST D3D10DDIARG_CREATEQUERY* pCreateQuery )
	{
		DEBUG_TRACE3(_T("CalcPrivateQuerySize\n"));
		SIZE_T size = D3D10_GET_ORIG().pfnCalcPrivateQuerySize( D3D10_DEVICE, pCreateQuery );
		return size;
	}

	VOID ( APIENTRY CreateQuery )( D3D10DDI_HDEVICE hDevice, CONST D3D10DDIARG_CREATEQUERY* pCreateQuery, D3D10DDI_HQUERY hQuery, D3D10DDI_HRTQUERY hRTQuery )
	{
		DEBUG_TRACE3(_T("CreateQuery\n"));
		D3D10_GET_ORIG().pfnCreateQuery( D3D10_DEVICE, pCreateQuery, hQuery, hRTQuery );
	}
	
	VOID ( APIENTRY DestroyQuery )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HQUERY hQuery )
	{
		DEBUG_TRACE3(_T("DestroyQuery\n"));
		D3D10_GET_ORIG().pfnDestroyQuery( D3D10_DEVICE, hQuery );
	}

	VOID ( APIENTRY CheckFormatSupport )( D3D10DDI_HDEVICE hDevice, DXGI_FORMAT Format, UINT* pFormatCaps)
	{
		DEBUG_TRACE3(_T("CheckFormatSupport\n"));
		D3D10_GET_ORIG().pfnCheckFormatSupport( D3D10_DEVICE, Format, pFormatCaps );
	}

	VOID ( APIENTRY CheckMultisampleQualityLevels )( D3D10DDI_HDEVICE hDevice, DXGI_FORMAT Format, UINT SampleCount, UINT* pNumQualityLevels)
	{
		DEBUG_TRACE3(_T("CheckMultisampleQualityLevels\n"));
		D3D10_GET_ORIG().pfnCheckMultisampleQualityLevels( D3D10_DEVICE, Format, SampleCount, pNumQualityLevels );
	}

	VOID ( APIENTRY  CheckCounterInfo )( D3D10DDI_HDEVICE hDevice, D3D10DDI_COUNTER_INFO* pCounterInfo )
	{
		DEBUG_TRACE3(_T("CheckCounterInfo\n"));
		D3D10_GET_ORIG().pfnCheckCounterInfo( D3D10_DEVICE, pCounterInfo );
	}

	VOID ( APIENTRY  CheckCounter )( D3D10DDI_HDEVICE hDevice, D3D10DDI_QUERY Query, D3D10DDI_COUNTER_TYPE* pCounterType, 
		UINT* pActiveCounters, LPSTR pName, UINT* pNameLength, LPSTR pUnits, UINT* pUnitsLength, LPSTR pDescription, UINT* pDescriptionLength )
	{
		DEBUG_TRACE3(_T("CheckCounter\n"));
		D3D10_GET_ORIG().pfnCheckCounter( D3D10_DEVICE, Query, pCounterType, pActiveCounters, pName, pNameLength, pUnits, pUnitsLength, pDescription, pDescriptionLength );
	}

	VOID ( APIENTRY DestroyDevice )( D3D10DDI_HDEVICE hDevice )
	{
		DEBUG_TRACE3(_T("DestroyDevice\n"));
		D3D10_GET_MONO_WRAPPER()->~D3DMonoDeviceWrapper();
		D3D10_GET_ORIG().pfnDestroyDevice( D3D10_DEVICE );
	}

	VOID ( APIENTRY SetTextFilterSize )( D3D10DDI_HDEVICE hDevice, UINT Width, UINT Height )
	{
		DEBUG_TRACE3(_T("SetTextFilterSize\n"));  
		D3D10_GET_ORIG().pfnSetTextFilterSize( D3D10_DEVICE, Width, Height );
	}

	VOID ( APIENTRY RelocateDeviceFuncs10_1 )( D3D10DDI_HDEVICE  hDevice, struct D3D10_1DDI_DEVICEFUNCS* pDeviceFunctions )
	{
		DEBUG_TRACE3(_T("RelocateDeviceFuncs10_1\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() == TD3DVersion_10_1);
		memcpy(pDeviceFunctions, &D3D10_GET_MONO_WRAPPER()->OriginalDeviceFuncs10_1, sizeof D3D10_1DDI_DEVICEFUNCS); // restore
		D3D10_1_GET_ORIG().pfnRelocateDeviceFuncs( D3D10_DEVICE, pDeviceFunctions );
		D3D10_GET_MONO_WRAPPER()->HookDeviceFuncs( pDeviceFunctions );
	}

	SIZE_T ( APIENTRY CalcPrivateShaderResourceViewSize10_1 )( D3D10DDI_HDEVICE hDevice, CONST D3D10_1DDIARG_CREATESHADERRESOURCEVIEW* pCreateShaderResourceView )
	{
		DEBUG_TRACE3(_T("CalcPrivateShaderResourceViewSize10_1\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() == TD3DVersion_10_1);
		SIZE_T size = D3D10_1_GET_ORIG().pfnCalcPrivateShaderResourceViewSize( D3D10_DEVICE, pCreateShaderResourceView );
		return size;
	}

	VOID ( APIENTRY CreateShaderResourceView10_1 )( D3D10DDI_HDEVICE hDevice, CONST D3D10_1DDIARG_CREATESHADERRESOURCEVIEW* pCreateShaderResourceView, 
		D3D10DDI_HSHADERRESOURCEVIEW hShaderResourceView, D3D10DDI_HRTSHADERRESOURCEVIEW hRTShaderResourceView )
	{
		DEBUG_TRACE3(_T("CreateShaderResourceView10_1\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() == TD3DVersion_10_1);
		D3D10_1_GET_ORIG().pfnCreateShaderResourceView( D3D10_DEVICE, pCreateShaderResourceView,	hShaderResourceView, hRTShaderResourceView );
	}

	SIZE_T ( APIENTRY CalcPrivateBlendStateSize10_1 )( D3D10DDI_HDEVICE hDevice, CONST D3D10_1_DDI_BLEND_DESC* pBlendDesc )
	{
		DEBUG_TRACE3(_T("CalcPrivateBlendStateSize10_1\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() >= TD3DVersion_10_1);
		SIZE_T size = D3D10_1_GET_ORIG().pfnCalcPrivateBlendStateSize( D3D10_DEVICE, pBlendDesc );
		return size;
	}

	VOID ( APIENTRY CreateBlendState10_1 )( D3D10DDI_HDEVICE hDevice, CONST D3D10_1_DDI_BLEND_DESC* pBlendDesc, 
		D3D10DDI_HBLENDSTATE hBlendState, D3D10DDI_HRTBLENDSTATE hRTBlendState )
	{
		DEBUG_TRACE3(_T("CreateBlendState10_1\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() == TD3DVersion_10_1 || D3D10_GET_MONO_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
		D3D10_1_GET_ORIG().pfnCreateBlendState( D3D10_DEVICE, pBlendDesc, hBlendState, hRTBlendState );
	}

	VOID ( APIENTRY ResourceConvert10_1 )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HRESOURCE hDstResource, D3D10DDI_HRESOURCE hSrcResource)
	{
		DEBUG_TRACE3(_T("ResourceConvert10_1\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() >= TD3DVersion_10_1);
		D3D10_1_GET_ORIG().pfnResourceConvert( D3D10_DEVICE, hDstResource, hSrcResource );
	}

	VOID ( APIENTRY ResourceConvertRegion10_1 )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HRESOURCE hDstResource, UINT DstSubresource, 
		UINT DstX, UINT DstY, UINT DstZ, D3D10DDI_HRESOURCE hSrcResource, UINT SrcSubresource, CONST D3D10_DDI_BOX*  pSrcBox )
	{
		DEBUG_TRACE3(_T("ResourceConvertRegion10_1\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() >= TD3DVersion_10_1);
		D3D10_1_GET_ORIG().pfnResourceConvertRegion(D3D10_DEVICE, hDstResource, DstSubresource, DstX, DstY, DstZ, hSrcResource, SrcSubresource, pSrcBox);
	}

	VOID ( APIENTRY SetRenderTargets11_0 )( D3D10DDI_HDEVICE hDevice, CONST D3D10DDI_HRENDERTARGETVIEW* phRenderTargetView, 
		UINT RTargets, UINT ClearTargets, D3D10DDI_HDEPTHSTENCILVIEW hDepthStencilView,	CONST D3D11DDI_HUNORDEREDACCESSVIEW* phUnorderedAccessView,
		CONST UINT* pUAV, UINT UAVIndex, UINT NumUAVs, UINT UAVFirsttoSet, UINT UAVNumberUpdated )
	{
		DEBUG_TRACE3(_T("SetRenderTargets11_0\n"));	
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
		D3D11_GET_ORIG().pfnSetRenderTargets(D3D10_DEVICE, phRenderTargetView, RTargets, ClearTargets, hDepthStencilView,
				phUnorderedAccessView, pUAV, UAVIndex, NumUAVs, UAVFirsttoSet, UAVNumberUpdated );
	}

	VOID ( APIENTRY RelocateDeviceFuncs11_0 )( D3D10DDI_HDEVICE hDevice,  struct D3D11DDI_DEVICEFUNCS* pDeviceFunctions )
	{
		DEBUG_TRACE3(_T("RelocateDeviceFuncs11_0\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() == TD3DVersion_11_0);
		memcpy(pDeviceFunctions, &D3D10_GET_MONO_WRAPPER()->OriginalDeviceFuncs11, sizeof D3D11DDI_DEVICEFUNCS); // restore
		D3D11_GET_ORIG().pfnRelocateDeviceFuncs( D3D10_DEVICE, pDeviceFunctions );
		D3D10_GET_MONO_WRAPPER()->HookDeviceFuncs( pDeviceFunctions );
	}

	SIZE_T ( APIENTRY CalcPrivateResourceSize11_0 )( D3D10DDI_HDEVICE hDevice, CONST D3D11DDIARG_CREATERESOURCE* pCreateResource )
	{
		DEBUG_TRACE3(_T("CalcPrivateResourceSize11_0\n"));  
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
		SIZE_T size = D3D11_GET_ORIG().pfnCalcPrivateResourceSize( D3D10_DEVICE, pCreateResource );
		return size;
	}

	VOID ( APIENTRY CreateResource11_0 )( D3D10DDI_HDEVICE hDevice, CONST D3D11DDIARG_CREATERESOURCE* pCreateResource,
		D3D10DDI_HRESOURCE hResource, D3D10DDI_HRTRESOURCE hRTResource )
	{
		DEBUG_TRACE3(_T("CreateResource11_0\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
		D3D11_GET_ORIG().pfnCreateResource( D3D10_DEVICE, pCreateResource, hResource, hRTResource );
	}

	SIZE_T ( APIENTRY CalcPrivateShaderResourceViewSize11_0 )( D3D10DDI_HDEVICE hDevice, CONST D3D11DDIARG_CREATESHADERRESOURCEVIEW* pCreateShaderResourceView )
	{
		DEBUG_TRACE3(_T("CalcPrivateShaderResourceViewSize11_0\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
		SIZE_T size = D3D11_GET_ORIG().pfnCalcPrivateShaderResourceViewSize( D3D10_DEVICE, pCreateShaderResourceView );
		return size;
	}

	VOID ( APIENTRY CreateShaderResourceView11_0 )( D3D10DDI_HDEVICE hDevice, CONST D3D11DDIARG_CREATESHADERRESOURCEVIEW* pCreateShaderResourceView, 
		D3D10DDI_HSHADERRESOURCEVIEW hShaderResourceView, D3D10DDI_HRTSHADERRESOURCEVIEW hRTShaderResourceView )
	{
		DEBUG_TRACE3(_T("CreateShaderResourceView11_0\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
		D3D11_GET_ORIG().pfnCreateShaderResourceView( D3D10_DEVICE, pCreateShaderResourceView, hShaderResourceView, hRTShaderResourceView);
	}

	SIZE_T ( APIENTRY CalcPrivateDepthStencilViewSize11_0 )( D3D10DDI_HDEVICE hDevice, CONST D3D11DDIARG_CREATEDEPTHSTENCILVIEW* pCreateDepthStencilView )
	{
		DEBUG_TRACE3(_T("CalcPrivateDepthStencilViewSize11_0\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
		SIZE_T size = D3D11_GET_ORIG().pfnCalcPrivateDepthStencilViewSize( D3D10_DEVICE, pCreateDepthStencilView );
		return size;
	}

	VOID ( APIENTRY CreateDepthStencilView11_0 )( D3D10DDI_HDEVICE hDevice, CONST D3D11DDIARG_CREATEDEPTHSTENCILVIEW* pCreateDepthStencilView, 
		D3D10DDI_HDEPTHSTENCILVIEW hDepthStencilView, D3D10DDI_HRTDEPTHSTENCILVIEW hRTDepthStencilView )
	{
		DEBUG_TRACE3(_T("CreateDepthStencilView11_0\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
		D3D11_GET_ORIG().pfnCreateDepthStencilView( D3D10_DEVICE, pCreateDepthStencilView, hDepthStencilView, hRTDepthStencilView );
	}

	SIZE_T ( APIENTRY CalcPrivateGeometryShaderWithStreamOutput11_0 )( D3D10DDI_HDEVICE hDevice, 
		CONST D3D11DDIARG_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT* pCreateGeometryShaderWithStreamOutput, CONST D3D10DDIARG_STAGE_IO_SIGNATURES* pSignatures)
	{
		DEBUG_TRACE3(_T("CalcPrivateGeometryShaderWithStreamOutput11_0\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
		SIZE_T size = D3D11_GET_ORIG().pfnCalcPrivateGeometryShaderWithStreamOutput( D3D10_DEVICE, pCreateGeometryShaderWithStreamOutput, pSignatures );
		return size;
	}

	VOID ( APIENTRY CreateGeometryShaderWithStreamOutput11_0 )( D3D10DDI_HDEVICE hDevice, 
		CONST D3D11DDIARG_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT* pCreateGeometryWithShaderOutput, 
		D3D10DDI_HSHADER hShader, D3D10DDI_HRTSHADER hRTShader, CONST D3D10DDIARG_STAGE_IO_SIGNATURES* pSignatures )
	{
		DEBUG_TRACE3(_T("CreateGeometryShaderWithStreamOutput11_0\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
		D3D11_GET_ORIG().pfnCreateGeometryShaderWithStreamOutput( D3D10_DEVICE, pCreateGeometryWithShaderOutput, hShader, hRTShader, pSignatures);
	}

	VOID ( APIENTRY DrawIndexedInstancedIndirect11_0 )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HRESOURCE hBufferForArgs, UINT AlignedByteOffsetForArgs )
	{
		DEBUG_TRACE3(_T("DrawIndexedInstancedIndirect11_0\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
		D3D11_GET_ORIG().pfnDrawIndexedInstancedIndirect( D3D10_DEVICE, hBufferForArgs, AlignedByteOffsetForArgs );
	}

	VOID ( APIENTRY DrawInstancedIndirect11_0 )( D3D10DDI_HDEVICE hDevice,  D3D10DDI_HRESOURCE hBufferForArgs, UINT AlignedByteOffsetForArgs )
	{
		DEBUG_TRACE3(_T("DrawInstancedIndirect11_0\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
		D3D11_GET_ORIG().pfnDrawInstancedIndirect( D3D10_DEVICE, hBufferForArgs, AlignedByteOffsetForArgs );
	}

	VOID ( APIENTRY CommandListExecute11_0 )( D3D10DDI_HDEVICE hDevice, D3D11DDI_HCOMMANDLIST hCommandList )
	{
		DEBUG_TRACE3(_T("CommandListExecute11_0\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
		D3D11_GET_ORIG().pfnCommandListExecute( D3D10_DEVICE, hCommandList );
	}

	VOID ( APIENTRY HsSetShaderResources11_0 )(	D3D10DDI_HDEVICE hDevice, UINT Offset, UINT NumViews, CONST D3D10DDI_HSHADERRESOURCEVIEW* phShaderResourceViews	)
	{
		DEBUG_TRACE3(_T("HsSetShaderResources11_0\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
		D3D11_GET_ORIG().pfnHsSetShaderResources( D3D10_DEVICE, Offset, NumViews, phShaderResourceViews );
	}

	VOID ( APIENTRY HsSetShader11_0 )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HSHADER hShader )
	{
		DEBUG_TRACE3(_T("HsSetShader11_0\n"));	
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
		D3D11_GET_ORIG().pfnHsSetShader( D3D10_DEVICE, hShader );
	}

	VOID ( APIENTRY HsSetSamplers11_0 )( D3D10DDI_HDEVICE hDevice, UINT Offset, UINT NumSamplers, CONST D3D10DDI_HSAMPLER* phSamplers )
	{
		DEBUG_TRACE3(_T("HsSetSamplers11_0\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
		D3D11_GET_ORIG().pfnHsSetSamplers( D3D10_DEVICE, Offset, NumSamplers, phSamplers );
	}

	VOID ( APIENTRY HsSetConstantBuffers11_0 )(	D3D10DDI_HDEVICE hDevice, UINT StartBuffer, UINT NumBuffers, CONST D3D10DDI_HRESOURCE* phBuffers )
	{
		DEBUG_TRACE3(_T("HsSetConstantBuffers11_0\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
		D3D11_GET_ORIG().pfnHsSetConstantBuffers( D3D10_DEVICE, StartBuffer, NumBuffers, phBuffers );
	}

	VOID ( APIENTRY DsSetShaderResources11_0 )(	D3D10DDI_HDEVICE hDevice, UINT Offset, UINT NumViews, CONST D3D10DDI_HSHADERRESOURCEVIEW* phShaderResourceViews	)
	{
		DEBUG_TRACE3(_T("DsSetShaderResources11_0\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
		D3D11_GET_ORIG().pfnDsSetShaderResources( D3D10_DEVICE, Offset, NumViews, phShaderResourceViews );
	}

	VOID ( APIENTRY DsSetShader11_0 )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HSHADER hShader )
	{
		DEBUG_TRACE3(_T("DsSetShader11_0\n"));	
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
		D3D11_GET_ORIG().pfnDsSetShader( D3D10_DEVICE, hShader );
	}

	VOID ( APIENTRY DsSetSamplers11_0 )( D3D10DDI_HDEVICE hDevice, UINT Offset, UINT NumSamplers, CONST D3D10DDI_HSAMPLER* phSamplers )
	{
		DEBUG_TRACE3(_T("DsSetSamplers11_0\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
		D3D11_GET_ORIG().pfnDsSetSamplers( D3D10_DEVICE, Offset, NumSamplers, phSamplers );
	}

	VOID ( APIENTRY DsSetConstantBuffers11_0 )(	D3D10DDI_HDEVICE hDevice, UINT StartBuffer, UINT NumBuffers, CONST D3D10DDI_HRESOURCE* phBuffers )
	{
		DEBUG_TRACE3(_T("DsSetConstantBuffers11_0\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
		D3D11_GET_ORIG().pfnDsSetConstantBuffers( D3D10_DEVICE, StartBuffer, NumBuffers, phBuffers );
	}

	VOID ( APIENTRY CreateHullShader11_0 )( D3D10DDI_HDEVICE hDevice, CONST UINT* pCode, D3D10DDI_HSHADER hShader, 
		D3D10DDI_HRTSHADER hRTShader, CONST D3D11DDIARG_TESSELLATION_IO_SIGNATURES* pSignatures )
	{
		DEBUG_TRACE3(_T("CreateHullShader11_0\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
		D3D11_GET_ORIG().pfnCreateHullShader( D3D10_DEVICE, pCode, hShader, hRTShader, pSignatures );
	}

	VOID ( APIENTRY CreateDomainShader11_0 )( D3D10DDI_HDEVICE hDevice, CONST UINT* pCode, D3D10DDI_HSHADER hShader, 
		D3D10DDI_HRTSHADER hRTShader, CONST D3D11DDIARG_TESSELLATION_IO_SIGNATURES* pSignatures)
	{
		DEBUG_TRACE3(_T("CreateDomainShader11_0\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
		D3D11_GET_ORIG().pfnCreateDomainShader( D3D10_DEVICE, pCode, hShader, hRTShader, pSignatures );
	}

	VOID ( APIENTRY CheckDeferredContextHandleSizes11_0 )( D3D10DDI_HDEVICE hDevice, UINT* pHSizes, D3D11DDI_HANDLESIZE* pHandleSize)
	{
		DEBUG_TRACE3(_T("CheckDeferredContextHandleSizes11_0\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
		D3D11_GET_ORIG().pfnCheckDeferredContextHandleSizes( D3D10_DEVICE, pHSizes, pHandleSize );
	}

	SIZE_T ( APIENTRY CalcDeferredContextHandleSize11_0 )( D3D10DDI_HDEVICE hDevice, D3D11DDI_HANDLETYPE HandleType, VOID* pICObject )
	{
		DEBUG_TRACE3(_T("CalcDeferredContextHandleSize11_0\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
		SIZE_T size = D3D11_GET_ORIG().pfnCalcDeferredContextHandleSize( D3D10_DEVICE, HandleType, pICObject );
		return size;
	}

	SIZE_T ( APIENTRY CalcPrivateDeferredContextSize11_0 )( D3D10DDI_HDEVICE hDevice, CONST D3D11DDIARG_CALCPRIVATEDEFERREDCONTEXTSIZE* pCalcPrivateDeferredContextSize )
	{
		DEBUG_TRACE3(_T("CalcPrivateDeferredContextSize11_0\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
		SIZE_T size = D3D11_GET_ORIG().pfnCalcPrivateDeferredContextSize( D3D10_DEVICE, pCalcPrivateDeferredContextSize );
		return size;
	}

	VOID ( APIENTRY CreateDeferredContext11_0 )( D3D10DDI_HDEVICE hDevice, CONST D3D11DDIARG_CREATEDEFERREDCONTEXT* pCreateDeferredContext )
	{
		DEBUG_TRACE3(_T("CreateDeferredContext11_0\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
		D3D11_GET_ORIG().pfnCreateDeferredContext( D3D10_DEVICE, pCreateDeferredContext );
	}

	VOID ( APIENTRY AbandonCommandList11_0 )( D3D10DDI_HDEVICE hDevice )
	{
		DEBUG_TRACE3(_T("AbandonCommandList11_0\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
		D3D11_GET_ORIG().pfnAbandonCommandList( D3D10_DEVICE );
	}

	SIZE_T ( APIENTRY CalcPrivateCommandListSize11_0 )( D3D10DDI_HDEVICE hDevice, CONST D3D11DDIARG_CREATECOMMANDLIST* pCreateCommandList )
	{
		DEBUG_TRACE3(_T("CalcPrivateCommandListSize11_0\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
		SIZE_T size = D3D11_GET_ORIG().pfnCalcPrivateCommandListSize( D3D10_DEVICE, pCreateCommandList );
		return size;
	}

	VOID ( APIENTRY CreateCommandList11_0 )( D3D10DDI_HDEVICE hDevice, CONST D3D11DDIARG_CREATECOMMANDLIST* pCreateCommandList, 
		D3D11DDI_HCOMMANDLIST hCommandList, D3D11DDI_HRTCOMMANDLIST hRTCommandList)
	{
		DEBUG_TRACE3(_T("CreateCommandList11_0\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
		DEBUG_MESSAGE(_T("\tWarning: CreateCommandList called!\n"));
		D3D11_GET_ORIG().pfnCreateCommandList( D3D10_DEVICE, pCreateCommandList, hCommandList, hRTCommandList );
	}

	VOID ( APIENTRY DestroyCommandList11_0 )( D3D10DDI_HDEVICE hDevice, D3D11DDI_HCOMMANDLIST hCommandList )
	{
		DEBUG_TRACE3(_T("DestroyCommandList11_0\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
		D3D11_GET_ORIG().pfnDestroyCommandList( D3D10_DEVICE, hCommandList );
	}
	
	SIZE_T ( APIENTRY CalcPrivateTessellationShaderSize11_0 )( D3D10DDI_HDEVICE hDevice, CONST UINT* pCode, CONST D3D11DDIARG_TESSELLATION_IO_SIGNATURES* pSignatures )
	{
		DEBUG_TRACE3(_T("CalcPrivateTessellationShaderSize11_0\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
		SIZE_T size = D3D11_GET_ORIG().pfnCalcPrivateTessellationShaderSize( D3D10_DEVICE, pCode, pSignatures );
		return size;
	}

	VOID ( APIENTRY PsSetShaderWithIfaces11_0 )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HSHADER hShader, UINT NumClassInstances, const UINT* pIfaces, 
		const D3D11DDIARG_POINTERDATA* pPointerData )
	{
		DEBUG_TRACE3(_T("PsSetShaderWithIfaces11_0\n"));	
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
		D3D11_GET_ORIG().pfnPsSetShaderWithIfaces( D3D10_DEVICE, hShader, NumClassInstances, pIfaces, pPointerData );
	}

	VOID ( APIENTRY VsSetShaderWithIfaces11_0 )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HSHADER hShader,
		UINT NumClassInstances, const UINT* pIfaces, const D3D11DDIARG_POINTERDATA* pPointerData )
	{
		DEBUG_TRACE3(_T("VsSetShaderWithIfaces11_0\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
		D3D11_GET_ORIG().pfnVsSetShaderWithIfaces( D3D10_DEVICE, hShader, NumClassInstances, pIfaces, pPointerData );
	}

	VOID ( APIENTRY GsSetShaderWithIfaces11_0 )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HSHADER hShader,
		UINT NumClassInstances, const UINT* pIfaces, const D3D11DDIARG_POINTERDATA* pPointerData  )
	{
		DEBUG_TRACE3(_T("GsSetShaderWithIfaces11_0\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
		D3D11_GET_ORIG().pfnGsSetShaderWithIfaces( D3D10_DEVICE, hShader, NumClassInstances, pIfaces, pPointerData );
	}

	VOID ( APIENTRY HsSetShaderWithIfaces11_0 )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HSHADER hShader,
		UINT NumClassInstances, const UINT* pIfaces, const D3D11DDIARG_POINTERDATA* pPointerData  )
	{
		DEBUG_TRACE3(_T("HsSetShaderWithIfaces11_0\n"));	
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
		D3D11_GET_ORIG().pfnHsSetShaderWithIfaces( D3D10_DEVICE, hShader, NumClassInstances, pIfaces, pPointerData );
	}

	VOID ( APIENTRY DsSetShaderWithIfaces11_0 )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HSHADER hShader,
		UINT NumClassInstances, const UINT* pIfaces, const D3D11DDIARG_POINTERDATA* pPointerData  )
	{
		DEBUG_TRACE3(_T("DsSetShaderWithIfaces11_0\n"));	
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
		D3D11_GET_ORIG().pfnDsSetShaderWithIfaces( D3D10_DEVICE, hShader, NumClassInstances, pIfaces, pPointerData );
	}

	VOID ( APIENTRY CsSetShaderWithIfaces11_0 )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HSHADER hShader, 
		UINT NumClassInstances, const UINT* pIfaces, const D3D11DDIARG_POINTERDATA* pPointerData )
	{
		DEBUG_TRACE3(_T("CsSetShaderWithIfaces11_0\n"));	
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
		D3D11_GET_ORIG().pfnCsSetShaderWithIfaces( D3D10_DEVICE, hShader, NumClassInstances, pIfaces, pPointerData );
	}

	VOID ( APIENTRY CreateComputeShader11_0 )( D3D10DDI_HDEVICE hDevice, CONST UINT* pCode, D3D10DDI_HSHADER hShader, D3D10DDI_HRTSHADER hRTShader )
	{
		DEBUG_TRACE3(_T("CreateComputeShader11_0\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
		D3D11_GET_ORIG().pfnCreateComputeShader( D3D10_DEVICE, pCode, hShader, hRTShader );
	}
	
	VOID ( APIENTRY CsSetShader11_0 )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HSHADER hShader )
	{
		DEBUG_TRACE3(_T("CsSetShader11_0\n"));	
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
		D3D11_GET_ORIG().pfnCsSetShader( D3D10_DEVICE, hShader );
	}

	VOID ( APIENTRY CsSetShaderResources11_0 )(	D3D10DDI_HDEVICE hDevice, UINT Offset, UINT NumViews, CONST D3D10DDI_HSHADERRESOURCEVIEW* phShaderResourceViews	)
	{
		DEBUG_TRACE3(_T("CsSetShaderResources11_0\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
		D3D11_GET_ORIG().pfnCsSetShaderResources( D3D10_DEVICE, Offset, NumViews, phShaderResourceViews );
	}
	
	VOID ( APIENTRY CsSetSamplers11_0 )( D3D10DDI_HDEVICE hDevice, UINT Offset, UINT NumSamplers, CONST D3D10DDI_HSAMPLER* phSamplers )
	{
		DEBUG_TRACE3(_T("CsSetSamplers11_0\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
		D3D11_GET_ORIG().pfnCsSetSamplers( D3D10_DEVICE, Offset, NumSamplers, phSamplers );
	}

	VOID ( APIENTRY CsSetConstantBuffers11_0 )(	D3D10DDI_HDEVICE hDevice, UINT StartBuffer, UINT NumBuffers, CONST D3D10DDI_HRESOURCE* phBuffers )
	{
		DEBUG_TRACE3(_T("CsSetConstantBuffers11_0\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
		D3D11_GET_ORIG().pfnCsSetConstantBuffers( D3D10_DEVICE, StartBuffer, NumBuffers, phBuffers );
	}

	SIZE_T ( APIENTRY CalcPrivateUnorderedAccessViewSize11_0 )( D3D10DDI_HDEVICE hDevice, CONST D3D11DDIARG_CREATEUNORDEREDACCESSVIEW* pCreateUnorderedAccessView )
	{
		DEBUG_TRACE3(_T("CalcPrivateUnorderedAccessViewSize11_0\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
		SIZE_T size = D3D11_GET_ORIG().pfnCalcPrivateUnorderedAccessViewSize( D3D10_DEVICE, pCreateUnorderedAccessView );
		return size;
	}
	
	VOID ( APIENTRY CreateUnorderedAccessView11_0 )( D3D10DDI_HDEVICE hDevice, CONST D3D11DDIARG_CREATEUNORDEREDACCESSVIEW* pCreateUnorderedAccessView, 
		D3D11DDI_HUNORDEREDACCESSVIEW hUnorderedAccessView, D3D11DDI_HRTUNORDEREDACCESSVIEW hRTUnorderedAccessView )
	{
		DEBUG_TRACE3(_T("CreateUnorderedAccessView11_0\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
		D3D11_GET_ORIG().pfnCreateUnorderedAccessView( D3D10_DEVICE, pCreateUnorderedAccessView,	hUnorderedAccessView, hRTUnorderedAccessView );
	}

	VOID ( APIENTRY DestroyUnorderedAccessView11_0 )( D3D10DDI_HDEVICE hDevice, D3D11DDI_HUNORDEREDACCESSVIEW hUnorderedAccessView )
	{
		DEBUG_TRACE3(_T("DestroyUnorderedAccessView11_0\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
		D3D11_GET_ORIG().pfnDestroyUnorderedAccessView( D3D10_DEVICE, hUnorderedAccessView );
	}

	VOID ( APIENTRY ClearUnorderedAccessViewUint11_0 )( D3D10DDI_HDEVICE hDevice, D3D11DDI_HUNORDEREDACCESSVIEW hUnorderedAccessView, CONST UINT pUints[4] )
	{
		DEBUG_TRACE3(_T("ClearUnorderedAccessViewUint11_0\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
		D3D11_GET_ORIG().pfnClearUnorderedAccessViewUint( D3D10_DEVICE, hUnorderedAccessView, pUints);
	}
	
	VOID ( APIENTRY ClearUnorderedAccessViewFloat11_0 )( D3D10DDI_HDEVICE hDevice, D3D11DDI_HUNORDEREDACCESSVIEW hUnorderedAccessView, CONST FLOAT pFloats[4] )
	{
		DEBUG_TRACE3(_T("ClearUnorderedAccessViewFloat11_0\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
		D3D11_GET_ORIG().pfnClearUnorderedAccessViewFloat( D3D10_DEVICE, hUnorderedAccessView, pFloats );
	}

	VOID ( APIENTRY CsSetUnorderedAccessViews11_0 )( D3D10DDI_HDEVICE hDevice, UINT Offset, UINT NumViews, 
		CONST D3D11DDI_HUNORDEREDACCESSVIEW* phUnorderedAccessView, CONST UINT* phSizes )
	{
		DEBUG_TRACE3(_T("CsSetUnorderedAccessViews11_0\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
		D3D11_GET_ORIG().pfnCsSetUnorderedAccessViews( D3D10_DEVICE, Offset, NumViews, phUnorderedAccessView, phSizes );
	}

	VOID ( APIENTRY Dispatch11_0 )( D3D10DDI_HDEVICE hDevice, UINT ThreadGroupCountX, UINT ThreadGroupCountY, UINT ThreadGroupCountZ )
	{
		DEBUG_TRACE3(_T("Dispatch11_0\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
		D3D11_GET_ORIG().pfnDispatch( D3D10_DEVICE, ThreadGroupCountX, ThreadGroupCountY, ThreadGroupCountZ );
	}

	VOID ( APIENTRY DispatchIndirect11_0 )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HRESOURCE hBufferForArgs, UINT AlignedByteOffsetForArgs )
	{
		DEBUG_TRACE3(_T("DispatchIndirect11_0\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
		D3D11_GET_ORIG().pfnDispatchIndirect( D3D10_DEVICE, hBufferForArgs, AlignedByteOffsetForArgs );
	}

	VOID ( APIENTRY SetResourceMinLOD11_0 )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HRESOURCE hResource, FLOAT MinLOD )
	{
		DEBUG_TRACE3(_T("SetResourceMinLOD11_0\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
		D3D11_GET_ORIG().pfnSetResourceMinLOD( D3D10_DEVICE, hResource, MinLOD );
	}

	VOID ( APIENTRY CopyStructureCount11_0 )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HRESOURCE hDstBuffer, UINT DstAlignedByteOffset, D3D11DDI_HUNORDEREDACCESSVIEW hSrcView )
	{
		DEBUG_TRACE3(_T("CopyStructureCount11_0\n"));
		_ASSERT(D3D10_GET_MONO_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
		D3D11_GET_ORIG().pfnCopyStructureCount( D3D10_DEVICE, hDstBuffer, DstAlignedByteOffset, hSrcView );
	}

	HRESULT ( APIENTRY  Present )  (DXGI_DDI_ARG_PRESENT* pPresentData)
	{
		DEBUG_TRACE3(_T("Present\n"));
		DXGI_MODIFY_DEVICE(pPresentData);
		HRESULT hResult = DXGI_GET_ORIG().pfnPresent(pPresentData);
		DXGI_RESTORE_DEVICE(pPresentData);
		return hResult;
	}
	
	HRESULT ( APIENTRY  GetGammaCaps )  (DXGI_DDI_ARG_GET_GAMMA_CONTROL_CAPS*  pGammaData)
	{
		DEBUG_TRACE3(_T("GetGammaCaps\n"));
		DXGI_MODIFY_DEVICE(pGammaData);
		HRESULT hResult	= DXGI_GET_ORIG().pfnGetGammaCaps(pGammaData);
		DXGI_RESTORE_DEVICE(pGammaData);
		return hResult;
	}

	HRESULT ( APIENTRY  SetDisplayMode )  (DXGI_DDI_ARG_SETDISPLAYMODE*  pDisplayModeData)
	{
		DEBUG_TRACE3(_T("SetDisplayMode\n"));
		DXGI_MODIFY_DEVICE(pDisplayModeData);	
		HRESULT hResult = DXGI_GET_ORIG().pfnSetDisplayMode(pDisplayModeData);
		DXGI_RESTORE_DEVICE(pDisplayModeData);
		return hResult;
	}

	HRESULT ( APIENTRY  SetResourcePriority )  (DXGI_DDI_ARG_SETRESOURCEPRIORITY*  pPriorityData)
	{
		DEBUG_TRACE3(_T("SetResourcePriority\n"));
		DXGI_MODIFY_DEVICE(pPriorityData);
		HRESULT hResult	= DXGI_GET_ORIG().pfnSetResourcePriority(pPriorityData);
		DXGI_RESTORE_DEVICE(pPriorityData);
		return hResult;
	}

	HRESULT ( APIENTRY QueryResourceResidency )( DXGI_DDI_ARG_QUERYRESOURCERESIDENCY*  pResidencyData )
	{
		DEBUG_TRACE3(_T("QueryResourceResidency\n"));
		DXGI_MODIFY_DEVICE(pResidencyData);
		HRESULT hResult	= DXGI_GET_ORIG().pfnQueryResourceResidency(pResidencyData);
		DXGI_RESTORE_DEVICE(pResidencyData);
		return hResult;
	}

	HRESULT ( APIENTRY  RotateResourceIdentities )  (DXGI_DDI_ARG_ROTATE_RESOURCE_IDENTITIES*  pRotateData)
	{
		DEBUG_TRACE3(_T("RotateResourceIdentities\n"));
		DXGI_MODIFY_DEVICE(pRotateData);
		HRESULT hResult = DXGI_GET_ORIG().pfnRotateResourceIdentities(pRotateData);
		DXGI_RESTORE_DEVICE(pRotateData);
		return hResult;
	}

	HRESULT ( APIENTRY  Blt )  (DXGI_DDI_ARG_BLT* pBltData)
	{
		DEBUG_TRACE3(_T("Blt\n"));
		DXGI_MODIFY_DEVICE(pBltData);
		HRESULT hResult = DXGI_GET_ORIG().pfnBlt(pBltData);	
		DXGI_RESTORE_DEVICE(pBltData);
		return hResult;
	}

	HRESULT ( APIENTRY  ResolveSharedResource )  (DXGI_DDI_ARG_RESOLVESHAREDRESOURCE* pResolveData)
	{
		DEBUG_TRACE3(_T("ResolveSharedResource\n"));
		DXGI_MODIFY_DEVICE(pResolveData);
		HRESULT hResult = DXGI_GET_ORIG2().pfnResolveSharedResource(pResolveData);
		DXGI_RESTORE_DEVICE(pResolveData);
		return hResult;
	}

};
