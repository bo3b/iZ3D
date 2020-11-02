#include "StdAfx.h"
#include "D3DDeviceWrapper.h"
#include "XMLStreamer_ReadValue.h"
#include "EnumToString.h"

//#define READ_OLD_COMMANDFLOW_FORMAT

#define DXGI_2_D3D_DEVICE(x)		MAKE_D3D10DDI_HDEVICE( (void*)(((BYTE*)x)+sizeof(void*)) )
#define DXGI_2_D3D_RESOURCE(x)		MAKE_D3D10DDI_HRESOURCE( (void*)x )

template <> void XMLStreamer::ReadValue<UINT8>( UINT8& _val )
{
	ReadValue( (UINT&)_val );
}

template <> void XMLStreamer::ReadValue<SHADER_PIPELINE>( SHADER_PIPELINE& _val )
{
	ReadValue( (UINT&)_val );
}

template <> void XMLStreamer::ReadValue<VIEWINDEX>( VIEWINDEX& _val )
{
	ReadValue( (UINT&)_val );
}

template <> void XMLStreamer::ReadValue<D3D10DDIRESOURCE_TYPE>( D3D10DDIRESOURCE_TYPE& _val )
{
	ReadValue( (UINT&)_val );
}

template <> void XMLStreamer::ReadValue<DXGI_FORMAT>( DXGI_FORMAT& _val )
{
	ReadValue( (UINT&)_val );
}

template <> void XMLStreamer::ReadValue<DXGI_GAMMA_CONTROL_CAPABILITIES>( DXGI_GAMMA_CONTROL_CAPABILITIES& _val )
{
	ReadStartElement();
	ReadValue( _val.ScaleAndOffsetSupported );
	ReadValue( _val.MaxConvertedValue );
	ReadValue( _val.MinConvertedValue );
	ReadValue( _val.NumGammaControlPoints );	
	ReadArrayValues_( _val.NumGammaControlPoints, _val.ControlPointPositions );
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<DXGI_SAMPLE_DESC>( DXGI_SAMPLE_DESC& _val )
{
	ReadStartElement();
	ReadValue( _val.Count );
	ReadValue( _val.Quality );
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<DXGI_DDI_PRIMARY_DESC>( DXGI_DDI_PRIMARY_DESC& _val )
{
	ReadStartElement();
	ReadValue( _val.Flags );
	ReadValue( _val.VidPnSourceId );
	ReadValue( _val.ModeDesc );
	ReadValue( _val.DriverFlags );
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<DXGI_DDI_MODE_DESC>( DXGI_DDI_MODE_DESC& _val )
{
	ReadStartElement();
	ReadValue( _val.Width );
	ReadValue( _val.Height );
	ReadValue( _val.Format );
	ReadValue( _val.RefreshRate );
	ReadValue( _val.ScanlineOrdering );
	ReadValue( _val.Rotation );
	ReadValue( _val.Scaling );
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<DXGI_DDI_RATIONAL>( DXGI_DDI_RATIONAL& _val )
{
	ReadStartElement();
	ReadValue( _val.Numerator );
	ReadValue( _val.Denominator );
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<DXGI_DDI_MODE_SCANLINE_ORDER>( DXGI_DDI_MODE_SCANLINE_ORDER& _val )
{
	ReadValue( (UINT&)_val );
}

template <> void XMLStreamer::ReadValue<DXGI_DDI_MODE_ROTATION>( DXGI_DDI_MODE_ROTATION& _val )
{
	ReadValue( (UINT&)_val );
}

template <> void XMLStreamer::ReadValue<DXGI_DDI_MODE_SCALING>( DXGI_DDI_MODE_SCALING& _val )
{
	ReadValue( (UINT&)_val );
}

template <> void XMLStreamer::ReadValue<DXGI_DDI_ARG_PRESENT>( DXGI_DDI_ARG_PRESENT& _val )
{
	ReadStartElement();
	ReadValue(	_val.hDevice );
	ReadValue(	_val.hSurfaceToPresent );
	ReadValue(		_val.SrcSubResourceIndex );
	ReadValue(	_val.hDstResource );
	ReadValue(		_val.DstSubResourceIndex );
	//ReadValue(	_val.pDXGIContext );
	Skip(); _val.pDXGIContext = 0;
	ReadValue(		_val.Flags.Value );
	ReadValue(		_val.FlipInterval );
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<DXGI_DDI_ARG_BLT>( DXGI_DDI_ARG_BLT& val )
{
	ReadStartElement();
	ReadValue(	val.hDevice );
	ReadValue(	val.hDstResource );
	ReadValue(		val.DstSubresource );
	ReadValue(		val.DstLeft );
	ReadValue(		val.DstTop );
	ReadValue(		val.DstRight );
	ReadValue(		val.DstBottom );
	ReadValue(	val.hSrcResource );
	ReadValue(		val.SrcSubresource );
	ReadValue(		val.Flags.Value );
	ReadValue(		val.Rotate );
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<DXGI_DDI_FLIP_INTERVAL_TYPE>( DXGI_DDI_FLIP_INTERVAL_TYPE& _val )
{
	ReadValue( (UINT&)_val );
}

template <> void XMLStreamer::ReadValue<DXGI_DDI_ARG_SETRESOURCEPRIORITY>( DXGI_DDI_ARG_SETRESOURCEPRIORITY & _val )
{
	ReadStartElement();
	ReadReference( DXGI_2_D3D_DEVICE( _val.hDevice ) );
	ReadReference( DXGI_2_D3D_RESOURCE( _val.hResource ) );
	ReadValue( _val.Priority );
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<DXGI_DDI_ARG_SETDISPLAYMODE>( DXGI_DDI_ARG_SETDISPLAYMODE& _val )
{
	ReadStartElement();
	ReadReference( DXGI_2_D3D_DEVICE( _val.hDevice ) );
	ReadReference( DXGI_2_D3D_RESOURCE( _val.hResource ) );
	ReadValue( _val.SubResourceIndex );
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<DXGI_DDI_ARG_GET_GAMMA_CONTROL_CAPS>( DXGI_DDI_ARG_GET_GAMMA_CONTROL_CAPS& _val )
{
	ReadStartElement();
	ReadReference( DXGI_2_D3D_DEVICE( _val.hDevice ) );
	ReadValueByRef( _val.pGammaCapabilities );
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<DXGI_DDI_ARG_ROTATE_RESOURCE_IDENTITIES>( DXGI_DDI_ARG_ROTATE_RESOURCE_IDENTITIES& _val )
{
	ReadStartElement();
	ReadReference( DXGI_2_D3D_DEVICE( _val.hDevice ) );
	ReadValue( _val.Resources );
	ReadArrayReferences( _val.Resources, (D3D10DDI_HRESOURCE*&)_val.pResources );
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<DXGI_DDI_ARG_QUERYRESOURCERESIDENCY>( DXGI_DDI_ARG_QUERYRESOURCERESIDENCY& _val )
{
	ReadStartElement();
	ReadReference( DXGI_2_D3D_DEVICE( _val.hDevice ) );
	ReadValue( _val.Resources );
	ReadArrayReferences( _val.Resources,  ( D3D10DDI_HRESOURCE* ) _val.pResources );
	ReadArrayValues( _val.Resources,  _val.pStatus );
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<DXGI_DDI_ARG_RESOLVESHAREDRESOURCE>( DXGI_DDI_ARG_RESOLVESHAREDRESOURCE& _val )
{
	ReadStartElement();
	ReadReference( DXGI_2_D3D_DEVICE( _val.hDevice ) );
	ReadReference( DXGI_2_D3D_RESOURCE( _val.hResource ) );
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<DXGI_DDI_RESIDENCY>( DXGI_DDI_RESIDENCY& _val )
{
	ReadValue( (UINT&)_val );
}

template <> void XMLStreamer::ReadValue<D3D10DDI_QUERY>( D3D10DDI_QUERY& _val )
{
	ReadValue( (UINT&)_val );
}

template <> void XMLStreamer::ReadValue<D3D10DDI_COUNTER_INFO>( D3D10DDI_COUNTER_INFO& _val )
{
	ReadStartElement();
	ReadValue( _val.LastDeviceDependentCounter );
	ReadValue( _val.NumSimultaneousCounters );
	ReadValue( _val.NumDetectableParallelUnits );
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<D3D10DDI_MIPINFO>( D3D10DDI_MIPINFO& _val )
{
	ReadStartElement();
	ReadValue( _val.TexelWidth );
	ReadValue( _val.TexelHeight );
	ReadValue( _val.TexelDepth );
	ReadValue( _val.PhysicalWidth );
	ReadValue( _val.PhysicalHeight );
	ReadValue( _val.PhysicalDepth );
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<D3D10DDI_MAPPED_SUBRESOURCE>( D3D10DDI_MAPPED_SUBRESOURCE& _val )
{
	ReadStartElement();
	//ReadValue( _val.pData );
	Skip();
	_val.pData = 0;
	ReadValue( _val.RowPitch );
	ReadValue( _val.DepthPitch );
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<D3D10_DDI_BLEND>( D3D10_DDI_BLEND& _val )
{
	ReadValue( (UINT&)_val );
}

template <> void XMLStreamer::ReadValue<D3D10_DDI_BLEND_OP>( D3D10_DDI_BLEND_OP& _val )
{
	ReadValue( (UINT&)_val );
}

template <> void XMLStreamer::ReadValue<D3D10_DDI_DEPTH_WRITE_MASK>( D3D10_DDI_DEPTH_WRITE_MASK& _val )
{
	ReadValue( (UINT&)_val );
}

template <> void XMLStreamer::ReadValue<D3D10_DDI_COMPARISON_FUNC>( D3D10_DDI_COMPARISON_FUNC& _val )
{
	ReadValue( (UINT&)_val );
}

template <> void XMLStreamer::ReadValue<D3DDDI_OPENALLOCATIONINFO>( D3DDDI_OPENALLOCATIONINFO& _val )
{
	ReadStartElement();
	ReadValue(	_val.hAllocation );
	Skip();//ReadValue(	_val.pPrivateDriverData );
	_val.pPrivateDriverData = 0;
	ReadValue(	_val.PrivateDriverDataSize );
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue< D3D10_DDI_PRIMITIVE_TOPOLOGY >( D3D10_DDI_PRIMITIVE_TOPOLOGY& _val )
{
	ReadValue( (UINT&)_val );
}

template <> void XMLStreamer::ReadValue<D3D10_DDI_STENCIL_OP>( D3D10_DDI_STENCIL_OP& _val )
{
	ReadValue( (UINT&)_val );
}

template <> void XMLStreamer::ReadValue<D3D10_DDI_FILL_MODE>( D3D10_DDI_FILL_MODE& _val )
{
	ReadValue( (UINT&)_val );
}

template <> void XMLStreamer::ReadValue<D3D10_DDI_CULL_MODE>( D3D10_DDI_CULL_MODE& _val )
{
	ReadValue( (UINT&)_val );
}

template <> void XMLStreamer::ReadValue<D3D10_DDI_FILTER>( D3D10_DDI_FILTER& _val )
{
	ReadValue( (UINT&)_val );
}

template <> void XMLStreamer::ReadValue<D3D10_DDI_TEXTURE_ADDRESS_MODE>( D3D10_DDI_TEXTURE_ADDRESS_MODE& _val )
{
	ReadValue( (UINT8&)_val );
}

template <> void XMLStreamer::ReadValue<D3D11DDI_HANDLETYPE>( D3D11DDI_HANDLETYPE& _val )
{
	ReadValue( (UINT8&)_val );
}

template <> void XMLStreamer::ReadValue<D3D10_DDI_BLEND_DESC>( D3D10_DDI_BLEND_DESC& _val )
{
	ReadStartElement();
	ReadValue( _val.AlphaToCoverageEnable );
	ReadArrayValues_( D3D10_DDI_SIMULTANEOUS_RENDER_TARGET_COUNT, _val.BlendEnable );
	ReadValue( _val.SrcBlend );
	ReadValue( _val.DestBlend );
	ReadValue( _val.BlendOp );
	ReadValue( _val.SrcBlendAlpha );
	ReadValue( _val.DestBlendAlpha );
	ReadValue( _val.BlendOpAlpha );
	ReadArrayValues_( D3D10_DDI_SIMULTANEOUS_RENDER_TARGET_COUNT, _val.RenderTargetWriteMask );
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<D3D10_DDI_DEPTH_STENCIL_DESC>( D3D10_DDI_DEPTH_STENCIL_DESC& _val )
{
	ReadStartElement();
	ReadValue( _val.DepthEnable );
	ReadValue( _val.DepthWriteMask );
	ReadValue( _val.DepthFunc );
	ReadValue( _val.StencilEnable );
	ReadValue( _val.FrontEnable );
	ReadValue( _val.BackEnable );
	ReadValue( _val.StencilReadMask );
	ReadValue( _val.StencilWriteMask );
	ReadValue( _val.FrontFace  );
	ReadValue( _val.BackFace  );
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<D3D10_DDI_DEPTH_STENCILOP_DESC>( D3D10_DDI_DEPTH_STENCILOP_DESC& _val )
{
	ReadStartElement();
	ReadValue( _val.StencilFailOp );
	ReadValue( _val.StencilDepthFailOp );
	ReadValue( _val.StencilPassOp );
	ReadValue( _val.StencilFunc );
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<D3D10_DDI_RASTERIZER_DESC>( D3D10_DDI_RASTERIZER_DESC& _val )
{
	ReadStartElement();	
	ReadValue( _val.CullMode );
	ReadValue( _val.FillMode );
	ReadValue( _val.FrontCounterClockwise );
	ReadValue( _val.DepthBias );
	ReadValue( _val.DepthBiasClamp );
	ReadValue( _val.SlopeScaledDepthBias );
	ReadValue( _val.DepthClipEnable );
	ReadValue( _val.ScissorEnable );
	ReadValue( _val.MultisampleEnable );
	ReadValue( _val.AntialiasedLineEnable );
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<D3D10_DDI_SAMPLER_DESC>( D3D10_DDI_SAMPLER_DESC& _val )
{
	ReadStartElement();
	ReadValue( _val.Filter );
	ReadValue( _val.AddressU );
	ReadValue( _val.AddressV );
	ReadValue( _val.AddressW );
	ReadValue( _val.MipLODBias );
	ReadValue( _val.MaxAnisotropy );
	ReadValue( _val.ComparisonFunc );
	ReadArrayValues_( 4, _val.BorderColor );
	ReadValue( _val.MinLOD );
	ReadValue( _val.MaxLOD );
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<D3D10_DDI_RECT>( D3D10_DDI_RECT&val )
{
	ReadStartElement();
	ReadValue( val.left );
	ReadValue( val.top );
	ReadValue( val.right );
	ReadValue( val.bottom );
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<D3D10_DDI_VIEWPORT>( D3D10_DDI_VIEWPORT& _val )
{
	ReadStartElement();
	ReadValue( _val.TopLeftX );
	ReadValue( _val.TopLeftY );
	ReadValue( _val.Width );
	ReadValue( _val.Height );
	ReadValue( _val.MinDepth );
	ReadValue( _val.MaxDepth );
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<D3D10_DDI_INPUT_CLASSIFICATION>( D3D10_DDI_INPUT_CLASSIFICATION& _val )
{
	ReadValue( (UINT&)_val );
}

template <> void XMLStreamer::ReadValue<D3D10_1_DDI_BLEND_DESC>( D3D10_1_DDI_BLEND_DESC& _val )
{
	ReadStartElement();
	ReadValue( _val.AlphaToCoverageEnable );
	ReadValue( _val.IndependentBlendEnable );
	ReadArrayValues_( D3D10_DDI_SIMULTANEOUS_RENDER_TARGET_COUNT, _val.RenderTarget );
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<D3D10_DDI_RENDER_TARGET_BLEND_DESC1>( D3D10_DDI_RENDER_TARGET_BLEND_DESC1& _val )
{
	ReadStartElement();
	ReadValue( _val.BlendEnable );
	ReadValue( _val.SrcBlend );
	ReadValue( _val.DestBlend );
	ReadValue( _val.BlendOp );
	ReadValue( _val.SrcBlendAlpha );
	ReadValue( _val.DestBlendAlpha );
	ReadValue( _val.BlendOpAlpha );
	ReadValue( _val.RenderTargetWriteMask );
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<D3D10_DDI_BOX>( D3D10_DDI_BOX &_val )
{
	ReadStartElement();
	ReadValue( _val.left );
	ReadValue( _val.top );
	ReadValue( _val.front );
	ReadValue( _val.right );
	ReadValue( _val.bottom );
	ReadValue( _val.back );
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<D3D10_DDI_MAP>( D3D10_DDI_MAP& _val )
{
	ReadValue((UINT&)_val );
}

template <> void XMLStreamer::ReadValue<D3D10DDIARG_CREATEQUERY>( D3D10DDIARG_CREATEQUERY& _val )
{
	ReadStartElement();
	ReadValue(	_val.MiscFlags );
	ReadValue(	_val.Query );
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<D3D10DDIARG_CREATERESOURCE>( D3D10DDIARG_CREATERESOURCE& _val )
{
	_val.pMipInfoList	= NULL;
	_val.pInitialDataUP	= NULL;

	ReadStartElement();
	ReadValue(	_val.MipLevels );
	ReadValue(	_val.ArraySize );
	ReadArrayValues( _val.MipLevels, ( D3D10DDI_MIPINFO* &)_val.pMipInfoList );
	ReadArrayValues( _val.MipLevels * _val.ArraySize, ( D3D10_DDIARG_SUBRESOURCE_UP* &)_val.pInitialDataUP );
	ReadValue(	_val.ResourceDimension );
	ReadValue(	_val.Usage);
	ReadValue(	_val.BindFlags);
	ReadValue(	_val.MapFlags );
	ReadValue(	_val.MiscFlags );
	ReadValue(	_val.Format );
	ReadValue(	_val.SampleDesc );

	ReadValueByRef(	_val.pPrimaryDesc );
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<D3D11DDIARG_CREATERESOURCE>( D3D11DDIARG_CREATERESOURCE& _val )
{
	_val.pMipInfoList	= NULL;
	_val.pInitialDataUP	= NULL;

	ReadStartElement();
	ReadValue(	_val.MipLevels );
	ReadValue(	_val.ArraySize );
	ReadArrayValues( _val.MipLevels, ( D3D10DDI_MIPINFO* &)_val.pMipInfoList );
	ReadArrayValues( _val.MipLevels * _val.ArraySize, ( D3D10_DDIARG_SUBRESOURCE_UP* &)_val.pInitialDataUP );
	ReadValue(	_val.ResourceDimension );
	ReadValue(	_val.Usage);
	ReadValue(	_val.BindFlags);
	ReadValue(	_val.MapFlags );
	ReadValue(	_val.MiscFlags );
	ReadValue(	_val.Format );
	ReadValue(	_val.SampleDesc );

	ReadValueByRef(	_val.pPrimaryDesc );
	ReadValue(	_val.ByteStride );
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<D3D10DDIARG_CREATERENDERTARGETVIEW>( D3D10DDIARG_CREATERENDERTARGETVIEW& _val )
{
	ReadStartElement();
	ReadReference( _val.hDrvResource );
	ReadValue( _val.Format );
	ReadValue( _val.ResourceDimension );
#ifdef READ_OLD_COMMANDFLOW_FORMAT
	ReadValue( _val.Buffer );
	ReadValue( _val.Tex1D );
	ReadValue( _val.Tex2D );
	ReadValue( _val.Tex3D );
	ReadValue( _val.TexCube );
#else
	switch( _val.ResourceDimension )
	{
	case D3D10DDIRESOURCE_BUFFER:
	case D3D11DDIRESOURCE_BUFFEREX:
		ReadValue( _val.Buffer );
		break;
	case D3D10DDIRESOURCE_TEXTURE1D:
		ReadValue( _val.Tex1D );
		break;
	case D3D10DDIRESOURCE_TEXTURE2D:
		ReadValue( _val.Tex2D );
		break;
	case D3D10DDIRESOURCE_TEXTURE3D:
		ReadValue( _val.Tex3D );
		break;
	case D3D10DDIRESOURCE_TEXTURECUBE:
		ReadValue( _val.TexCube );
		break;
	default:
		_ASSERT(FALSE);
	}
#endif
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<D3D11DDIARG_CREATEUNORDEREDACCESSVIEW>( D3D11DDIARG_CREATEUNORDEREDACCESSVIEW& _val )
{
	ReadStartElement();
#ifdef READ_OLD_COMMANDFLOW_FORMAT
	ReadValue( _val.Format );
	ReadReference( _val.hDrvResource );
	ReadValue( _val.ResourceDimension );
	ReadValue( _val.Buffer );
	ReadValue( _val.Tex1D );
	ReadValue( _val.Tex2D );
	ReadValue( _val.Tex3D );
#else
	ReadReference( _val.hDrvResource );
	ReadValue( _val.Format );
	ReadValue( _val.ResourceDimension );
	switch( _val.ResourceDimension )
	{
	case D3D10DDIRESOURCE_BUFFER:
	case D3D11DDIRESOURCE_BUFFEREX:
		ReadValue( _val.Buffer );
		break;
	case D3D10DDIRESOURCE_TEXTURE1D:
		ReadValue( _val.Tex1D );
		break;
	case D3D10DDIRESOURCE_TEXTURE2D:
		ReadValue( _val.Tex2D );
		break;
	case D3D10DDIRESOURCE_TEXTURE3D:
		ReadValue( _val.Tex3D );
		break;
	default:
		_ASSERT(FALSE);
	}
#endif
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<D3D10DDIARG_BUFFER_RENDERTARGETVIEW>( D3D10DDIARG_BUFFER_RENDERTARGETVIEW& _val )
{
	ReadStartElement();
	ReadValue( _val.ElementOffset );
	ReadValue( _val.ElementWidth );
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<D3D11DDIARG_BUFFER_UNORDEREDACCESSVIEW>( D3D11DDIARG_BUFFER_UNORDEREDACCESSVIEW& _val )
{
	ReadStartElement();
	ReadValue( _val.FirstElement );
	ReadValue( _val.NumElements );
	ReadValue( _val.Flags );
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<D3D11DDIARG_TEX1D_UNORDEREDACCESSVIEW>( D3D11DDIARG_TEX1D_UNORDEREDACCESSVIEW& _val )
{
	ReadStartElement();
	ReadValue( _val.MipSlice );
	ReadValue( _val.FirstArraySlice );
	ReadValue( _val.ArraySize );
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<D3D11DDIARG_TEX2D_UNORDEREDACCESSVIEW>( D3D11DDIARG_TEX2D_UNORDEREDACCESSVIEW& _val )
{
	ReadStartElement();
	ReadValue( _val.MipSlice );
	ReadValue( _val.FirstArraySlice );
	ReadValue( _val.ArraySize );
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<D3D11DDIARG_TEX3D_UNORDEREDACCESSVIEW>( D3D11DDIARG_TEX3D_UNORDEREDACCESSVIEW& _val )
{
	ReadStartElement();
	ReadValue( _val.MipSlice );
	ReadValue( _val.FirstW );
	ReadValue( _val.WSize );
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<D3D10DDIARG_TEX1D_RENDERTARGETVIEW>( D3D10DDIARG_TEX1D_RENDERTARGETVIEW& _val )
{
	ReadStartElement();
	ReadValue( _val.MipSlice );
	ReadValue( _val.FirstArraySlice );
	ReadValue( _val.ArraySize );
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<D3D10DDIARG_TEX2D_RENDERTARGETVIEW>( D3D10DDIARG_TEX2D_RENDERTARGETVIEW& _val )
{
	ReadStartElement();
	ReadValue( _val.MipSlice );
	ReadValue( _val.FirstArraySlice );
	ReadValue( _val.ArraySize );
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<D3D10DDIARG_TEX3D_RENDERTARGETVIEW>( D3D10DDIARG_TEX3D_RENDERTARGETVIEW& _val )
{
	ReadStartElement();
	ReadValue( _val.MipSlice );
	ReadValue( _val.FirstW );
	ReadValue( _val.WSize );
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<D3D10DDIARG_TEXCUBE_RENDERTARGETVIEW>( D3D10DDIARG_TEXCUBE_RENDERTARGETVIEW& _val )
{
	ReadStartElement();
	ReadValue( _val.MipSlice );
	ReadValue( _val.FirstArraySlice );
	ReadValue( _val.ArraySize );
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<D3D10DDIARG_STAGE_IO_SIGNATURES>( D3D10DDIARG_STAGE_IO_SIGNATURES& _val )
{	
	_val.pInputSignature	= NULL;
	_val.pOutputSignature	= NULL;

	ReadStartElement();	
	ReadValue( _val.NumInputSignatureEntries );
	ReadArrayValues( _val.NumInputSignatureEntries,	_val.pInputSignature );
	ReadValue( _val.NumOutputSignatureEntries );
	ReadArrayValues( _val.NumOutputSignatureEntries, _val.pOutputSignature );
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<D3D10DDIARG_OPENRESOURCE>( D3D10DDIARG_OPENRESOURCE& _val )
{
	ReadStartElement();
	ReadValue(	_val.NumAllocations );
	Skip();//ReadValue(	*(_val.pOpenAllocationInfo) );
	Skip();//ReadValue(	_val.hKMResource.handle );
	Skip();//ReadValue(	_val.pPrivateDriverData ); // TODO: Fix it
	ReadValue(	_val.PrivateDriverDataSize );
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<D3D10DDIARG_SIGNATURE_ENTRY>( D3D10DDIARG_SIGNATURE_ENTRY& _val )
{
	ReadStartElement();
	Skip();//ReadValue( _val.SystemValue );
	Skip();//ReadValue( _val.Register );
	Skip();//ReadValue( _val.Mask );
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<D3D10DDIARG_CREATEELEMENTLAYOUT>( D3D10DDIARG_CREATEELEMENTLAYOUT& _val )
{
	_val.pVertexElements = NULL;

	ReadStartElement();
	ReadValue( _val.NumElements );	
	ReadArrayValues( _val.NumElements,	( D3D10DDIARG_INPUT_ELEMENT_DESC* &)_val.pVertexElements );	
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<D3D10DDIARG_INPUT_ELEMENT_DESC>( D3D10DDIARG_INPUT_ELEMENT_DESC& _val )
{
	ReadStartElement();
	ReadValue( _val.InputSlot );
	ReadValue( _val.AlignedByteOffset );
	ReadValue( _val.Format );
	ReadValue( _val.InputSlotClass );
	ReadValue( _val.InstanceDataStepRate );
	ReadValue( _val.InputRegister );
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<D3D10DDIARG_CREATESHADERRESOURCEVIEW>( D3D10DDIARG_CREATESHADERRESOURCEVIEW& _val )
{
	ReadStartElement();
#ifdef READ_OLD_COMMANDFLOW_FORMAT
	ReadValue( _val.Buffer );
	ReadValue( _val.Format );
	ReadReference( _val.hDrvResource );
	ReadValue( _val.ResourceDimension );
	ReadValue( _val.Tex1D );
	ReadValue( _val.Tex2D );
	ReadValue( _val.Tex3D );
	ReadValue( _val.TexCube );
#else
	ReadReference( _val.hDrvResource );
	ReadValue( _val.Format );
	ReadValue( _val.ResourceDimension );
	switch( _val.ResourceDimension )
	{
	case D3D10DDIRESOURCE_BUFFER:
	case D3D11DDIRESOURCE_BUFFEREX:
		ReadValue( _val.Buffer );
		break;
	case D3D10DDIRESOURCE_TEXTURE1D:
		ReadValue( _val.Tex1D );
		break;
	case D3D10DDIRESOURCE_TEXTURE2D:
		ReadValue( _val.Tex2D );
		break;
	case D3D10DDIRESOURCE_TEXTURE3D:
		ReadValue( _val.Tex3D );
		break;
	case D3D10DDIRESOURCE_TEXTURECUBE:
		ReadValue( _val.TexCube );
		break;
	default:
		_ASSERT(FALSE);
	}
#endif
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<D3D11DDIARG_CREATESHADERRESOURCEVIEW>( D3D11DDIARG_CREATESHADERRESOURCEVIEW& _val )
{
	ReadStartElement();
#ifdef READ_OLD_COMMANDFLOW_FORMAT
	ReadValue( _val.Buffer );
	ReadValue( _val.Format );
	ReadReference( _val.hDrvResource );
	ReadValue( _val.ResourceDimension );
	ReadValue( _val.Tex1D );
	ReadValue( _val.Tex2D );
	ReadValue( _val.Tex3D );
	ReadValue( _val.TexCube );
#else
	ReadReference( _val.hDrvResource );
	ReadValue( _val.Format );
	ReadValue( _val.ResourceDimension );
	switch( _val.ResourceDimension )
	{
	case D3D10DDIRESOURCE_BUFFER:
	case D3D11DDIRESOURCE_BUFFEREX:
		ReadValue( _val.Buffer );
		break;
	case D3D10DDIRESOURCE_TEXTURE1D:
		ReadValue( _val.Tex1D );
		break;
	case D3D10DDIRESOURCE_TEXTURE2D:
		ReadValue( _val.Tex2D );
		break;
	case D3D10DDIRESOURCE_TEXTURE3D:
		ReadValue( _val.Tex3D );
		break;
	case D3D10DDIRESOURCE_TEXTURECUBE:
		ReadValue( _val.TexCube );
		break;
	default:
		_ASSERT(FALSE);
	}
#endif
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<D3D10DDIARG_BUFFER_SHADERRESOURCEVIEW>( D3D10DDIARG_BUFFER_SHADERRESOURCEVIEW& _val )
{
	ReadStartElement();
	ReadValue( _val.ElementOffset );
	ReadValue( _val.ElementWidth );
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<D3D10DDIARG_TEX1D_SHADERRESOURCEVIEW>( D3D10DDIARG_TEX1D_SHADERRESOURCEVIEW& _val )
{
	ReadStartElement();
	ReadValue( _val.ArraySize );
	ReadValue( _val.FirstArraySlice );
	ReadValue( _val.MipLevels );
	ReadValue( _val.MostDetailedMip );
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<D3D10DDIARG_TEX2D_SHADERRESOURCEVIEW>( D3D10DDIARG_TEX2D_SHADERRESOURCEVIEW& _val )
{
	ReadStartElement();
	ReadValue( _val.ArraySize );
	ReadValue( _val.FirstArraySlice );
	ReadValue( _val.MipLevels );
	ReadValue( _val.MostDetailedMip );
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<D3D10DDIARG_TEX3D_SHADERRESOURCEVIEW>( D3D10DDIARG_TEX3D_SHADERRESOURCEVIEW& _val )
{
	ReadStartElement();
	ReadValue( _val.MipLevels );
	ReadValue( _val.MostDetailedMip );
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<D3D10DDIARG_TEXCUBE_SHADERRESOURCEVIEW>( D3D10DDIARG_TEXCUBE_SHADERRESOURCEVIEW& _val )
{
	ReadStartElement();
	ReadValue( _val.MipLevels );
	ReadValue( _val.MostDetailedMip );
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<D3D10_1DDIARG_CREATESHADERRESOURCEVIEW>( D3D10_1DDIARG_CREATESHADERRESOURCEVIEW& _val )
{
	ReadStartElement();
#ifdef READ_OLD_COMMANDFLOW_FORMAT
	ReadValue( _val.Buffer );
	ReadValue( _val.Format );
	ReadReference( _val.hDrvResource );
	ReadValue( _val.ResourceDimension );
	ReadValue( _val.Tex1D );
	ReadValue( _val.Tex2D );
	ReadValue( _val.Tex3D );
	ReadValue( _val.TexCube );
#else
	ReadReference( _val.hDrvResource );
	ReadValue( _val.Format );
	ReadValue( _val.ResourceDimension );
	switch( _val.ResourceDimension )
	{
	case D3D10DDIRESOURCE_BUFFER:
	case D3D11DDIRESOURCE_BUFFEREX:
		ReadValue( _val.Buffer );
		break;
	case D3D10DDIRESOURCE_TEXTURE1D:
		ReadValue( _val.Tex1D );
		break;
	case D3D10DDIRESOURCE_TEXTURE2D:
		ReadValue( _val.Tex2D );
		break;
	case D3D10DDIRESOURCE_TEXTURE3D:
		ReadValue( _val.Tex3D );
		break;
	case D3D10DDIRESOURCE_TEXTURECUBE:
		ReadValue( _val.TexCube );
		break;
	default:
		_ASSERT(FALSE);
	}
#endif
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<D3D10_1DDIARG_TEXCUBE_SHADERRESOURCEVIEW>( D3D10_1DDIARG_TEXCUBE_SHADERRESOURCEVIEW& _val )
{
	ReadStartElement();
	ReadValue( _val.MipLevels );
	ReadValue( _val.MostDetailedMip );
	ReadValue( _val.NumCubes );
	ReadValue( _val.First2DArrayFace );
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<D3D10DDIARG_CREATEDEPTHSTENCILVIEW>( D3D10DDIARG_CREATEDEPTHSTENCILVIEW& _val )
{
	ReadStartElement();
#ifdef READ_OLD_COMMANDFLOW_FORMAT
	ReadValue( _val.Format );
	ReadReference( _val.hDrvResource );
	ReadValue( _val.ResourceDimension );
	ReadValue( _val.Tex1D );
	ReadValue( _val.Tex2D );
	ReadValue( _val.TexCube );
#else
	ReadReference( _val.hDrvResource );
	ReadValue( _val.Format );
	ReadValue( _val.ResourceDimension );
	switch( _val.ResourceDimension )
	{
	case D3D10DDIRESOURCE_TEXTURE1D:
		ReadValue( _val.Tex1D );
		break;
	case D3D10DDIRESOURCE_TEXTURE2D:
		ReadValue( _val.Tex2D );
		break;
	case D3D10DDIRESOURCE_TEXTURECUBE:
		ReadValue( _val.TexCube );
		break;
	default:
		_ASSERT(FALSE);
	}
#endif
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<D3D11DDIARG_CREATEDEPTHSTENCILVIEW>( D3D11DDIARG_CREATEDEPTHSTENCILVIEW& _val )
{
	ReadStartElement();
#ifdef READ_OLD_COMMANDFLOW_FORMAT
	ReadValue( _val.Format );
	ReadReference( _val.hDrvResource );
	ReadValue( _val.ResourceDimension );
	ReadValue( _val.Flags );
	ReadValue( _val.Tex1D );
	ReadValue( _val.Tex2D );
	ReadValue( _val.TexCube );
#else
	ReadReference( _val.hDrvResource );
	ReadValue( _val.Format );
	ReadValue( _val.ResourceDimension );
	ReadValue( _val.Flags );
	switch( _val.ResourceDimension )
	{
	case D3D10DDIRESOURCE_TEXTURE1D:
		ReadValue( _val.Tex1D );
		break;
	case D3D10DDIRESOURCE_TEXTURE2D:
		ReadValue( _val.Tex2D );
		break;
	case D3D10DDIRESOURCE_TEXTURECUBE:
		ReadValue( _val.TexCube );
		break;
	default:
		_ASSERT(FALSE);
	}
#endif
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<D3D10DDIARG_TEX1D_DEPTHSTENCILVIEW>( D3D10DDIARG_TEX1D_DEPTHSTENCILVIEW& _val )
{
	ReadStartElement();
	ReadValue( _val.ArraySize );
	ReadValue( _val.FirstArraySlice );
	ReadValue( _val.MipSlice );
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<D3D10DDIARG_TEX2D_DEPTHSTENCILVIEW>( D3D10DDIARG_TEX2D_DEPTHSTENCILVIEW& _val )
{
	ReadStartElement();
	ReadValue( _val.ArraySize );
	ReadValue( _val.FirstArraySlice );
	ReadValue( _val.MipSlice );
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<D3D10DDIARG_TEXCUBE_DEPTHSTENCILVIEW>( D3D10DDIARG_TEXCUBE_DEPTHSTENCILVIEW& _val )
{
	ReadStartElement();
	ReadValue( _val.ArraySize );
	ReadValue( _val.FirstArraySlice );
	ReadValue( _val.MipSlice );
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<D3D10DDIARG_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT>( D3D10DDIARG_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT& _val )
{
	_val.pOutputStreamDecl = NULL;
	ReadStartElement();
	ReadValue( _val.NumEntries );
	ReadArrayValues( _val.NumEntries, ( D3D10DDIARG_STREAM_OUTPUT_DECLARATION_ENTRY* &)_val.pOutputStreamDecl );
	//ReadReference( _val.pShaderCode ); // TODO: Fix it
	_val.pShaderCode = 0;
	ReadValue( _val.StreamOutputStrideInBytes );
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<D3D10DDIARG_STREAM_OUTPUT_DECLARATION_ENTRY>( D3D10DDIARG_STREAM_OUTPUT_DECLARATION_ENTRY& _val )
{
	ReadStartElement();
	ReadValue( _val.OutputSlot );
	ReadValue( _val.RegisterIndex );
	ReadValue( _val.RegisterMask );
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<D3D10_DDIARG_SUBRESOURCE_UP>( D3D10_DDIARG_SUBRESOURCE_UP& _val )
{	
	ReadStartElement();
	//ReadValue(	_val.pSysMem );
	Skip();
	_val.pSysMem = 0;
	ReadValue(	_val.SysMemPitch );
	ReadValue(	_val.SysMemSlicePitch );	
	ReadEndElement();
}

template <> void XMLStreamer::ReadValue<D3D10_SB_NAME>( D3D10_SB_NAME& _val )
{
	ReadValue( (UINT&)_val );
}
