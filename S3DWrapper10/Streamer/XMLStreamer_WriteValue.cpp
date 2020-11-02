#include "StdAfx.h"
#include "D3DDeviceWrapper.h"
#include "XMLStreamer_WriteValue.h"
#include "EnumToString.h"

//////////////////////////////////////////////////////////////////////////

template <> void XMLStreamer::WriteValue<UINT8>( const char* szAttribute, const UINT8& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	m_pXMLWriter->WriteAttribute( "val", (UINT)val );
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<SHADER_PIPELINE>( const char* szAttribute, const SHADER_PIPELINE& _val )
{
	WriteValueWithComment( szAttribute, _val );
}

template <> void XMLStreamer::WriteValue<VIEWINDEX>( const char* szAttribute, const VIEWINDEX& _val )
{
	WriteValueWithComment( szAttribute, _val );
}

template <> void XMLStreamer::WriteValue<D3D10_SB_NAME>( const char* szAttribute, const D3D10_SB_NAME& _val )
{
	WriteValueWithComment( szAttribute, _val );
}

//-----------------------------------------------------------------------------
//	WriteValue<> template specifications for D3D10_DDI_xxxxx_DESC types
//-----------------------------------------------------------------------------
template <> void XMLStreamer::WriteValue<D3D10_DDI_BLEND_DESC>( const char* szAttribute, const D3D10_DDI_BLEND_DESC& val   )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteValue( "AlphaToCoverageEnable",		val.AlphaToCoverageEnable );
	WriteArrayValues( "BlendEnable",			D3D10_DDI_SIMULTANEOUS_RENDER_TARGET_COUNT, val.BlendEnable );
	WriteValue( "SrcBlend",						val.SrcBlend	);
	WriteValue( "DestBlend",					val.DestBlend	);
	WriteValue( "BlendOp",						val.BlendOp	);
	WriteValue( "SrcBlendAlpha",				val.SrcBlendAlpha  );
	WriteValue( "DestBlendAlpha",				val.DestBlendAlpha );
	WriteValue( "BlendOpAlpha",					val.BlendOpAlpha   );
	WriteArrayValues( "RenderTargetWriteMask",	D3D10_DDI_SIMULTANEOUS_RENDER_TARGET_COUNT, val.RenderTargetWriteMask );
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<D3D10_DDI_RENDER_TARGET_BLEND_DESC1>( const char* szAttribute, const D3D10_DDI_RENDER_TARGET_BLEND_DESC1& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteValue( "BlendEnable",				val.BlendEnable );
	WriteValue( "SrcBlend",					val.SrcBlend );
	WriteValue( "DestBlend",				val.DestBlend );
	WriteValue( "BlendOp",					val.BlendOp );
	WriteValue( "SrcBlendAlpha",			val.SrcBlendAlpha );
	WriteValue( "DestBlendAlpha",			val.DestBlendAlpha );
	WriteValue( "BlendOpAlpha",				val.BlendOpAlpha );
	WriteValue( "RenderTargetWriteMask",	val.RenderTargetWriteMask );
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<D3D10_1_DDI_BLEND_DESC>( const char* szAttribute, const D3D10_1_DDI_BLEND_DESC& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteValue( "AlphaToCoverageEnable",	val.AlphaToCoverageEnable  );
	WriteValue( "IndependentBlendEnable",	val.IndependentBlendEnable );
	WriteArrayValues( "D3D10_DDI_RENDER_TARGET_BLEND_DESC1", D3D10_DDI_SIMULTANEOUS_RENDER_TARGET_COUNT, val.RenderTarget );
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<D3D10_DDI_DEPTH_STENCIL_DESC>( const char* szAttribute, const D3D10_DDI_DEPTH_STENCIL_DESC& val )
{
	m_pXMLWriter->WriteComment( EnumToString(val.DepthFunc) );
	m_pXMLWriter->StartElement( szAttribute );
	WriteValue( "DepthEnable",				val.DepthEnable		);
	WriteValue( "DepthWriteMask",			val.DepthWriteMask	);
	WriteValue( "DepthFunc",				val.DepthFunc			);
	WriteValue( "StencilEnable",			val.StencilEnable		);
	WriteValue( "FrontEnable",				val.FrontEnable		);
	WriteValue( "BackEnable",				val.BackEnable		);
	WriteValue( "StencilReadMask",			val.StencilReadMask	);
	WriteValue( "StencilWriteMask",			val.StencilWriteMask	);

	WriteValue("FrontFace", val.FrontFace);
	WriteValue("BackFace", val.BackFace);
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<D3D10_DDI_RASTERIZER_DESC>( const char* szAttribute, const D3D10_DDI_RASTERIZER_DESC& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteValue( "CullMode",					val.CullMode	);
	WriteValue( "FillMode",					val.FillMode	);

	WriteValue( "FrontCounterClockwise",	val.FrontCounterClockwise	);
	WriteValue( "DepthFunc",				val.DepthBias	);
	WriteValue( "DepthBiasClamp",			val.DepthBiasClamp	);
	WriteValue( "SlopeScaledDepthBias",		val.SlopeScaledDepthBias );
	WriteValue( "ScissorEnable",			val.DepthClipEnable	);
	WriteValue( "ScissorEnable",			val.ScissorEnable	);
	WriteValue( "MultisampleEnable",		val.MultisampleEnable	);
	WriteValue( "AntialiasedLineEnable",	val.AntialiasedLineEnable	);
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<D3D10_DDI_SAMPLER_DESC>( const char* szAttribute, const D3D10_DDI_SAMPLER_DESC& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteValue( "Filter",			val.Filter );
	WriteValue( "AddressU",			val.AddressU	);
	WriteValue( "AddressV",			val.AddressV	);
	WriteValue( "AddressW",			val.AddressW	);
	WriteValue( "MipLODBias",		val.MipLODBias	);
	WriteValue( "MaxAnisotropy",	val.MaxAnisotropy );
	WriteValue( "ComparisonFunc",	val.ComparisonFunc	);
	WriteArrayValues( "BorderColor", 4, val.BorderColor	);
	WriteValue( "MinLOD",			val.MinLOD	);
	WriteValue( "MaxLOD",			val.MaxLOD	);
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<D3D10_DDI_DEPTH_STENCILOP_DESC>( const char* szAttribute, const D3D10_DDI_DEPTH_STENCILOP_DESC& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteValue( "StencilFailOp",		val.StencilFailOp	);
	WriteValue( "StencilDepthFailOp",	val.StencilDepthFailOp	);
	WriteValue( "StencilPassOp",		val.StencilPassOp	);
	WriteValue( "StencilFunc",			val.StencilFunc	);
	m_pXMLWriter->EndElement();
}

//-----------------------------------------------------------------------------
//	WriteValue<> template specifications for D3D10_DDI_xxxxx types
//-----------------------------------------------------------------------------

template <> void XMLStreamer::WriteValue<D3D10_DDI_RECT>( const char* szAttribute, const D3D10_DDI_RECT& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteValue("left", val.left );
	WriteValue("top", val.top );
	WriteValue("right", val.right );
	WriteValue("bottom", val.bottom );
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<D3D10_DDI_VIEWPORT>( const char* szAttribute, const D3D10_DDI_VIEWPORT& val )
{	
	m_pXMLWriter->StartElement( szAttribute );
	WriteValue("TopLeftX",	val.TopLeftX );
	WriteValue("TopLeftY",	val.TopLeftY );
	WriteValue("Width",		val.Width );
	WriteValue("Height",	val.Height );
	WriteValue("MinDepth",	val.MinDepth );
	WriteValue("MaxDepth",	val.MaxDepth );
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<D3D10DDI_MAPPED_SUBRESOURCE>( const char* szAttribute, const D3D10DDI_MAPPED_SUBRESOURCE& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteValue("pData",			val.pData ); // TODO: Fix it
	WriteValue("RowPitch",		val.RowPitch );
	WriteValue("DepthPitch",	val.DepthPitch );
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<D3D10_DDI_BOX>( const char* szAttribute, const D3D10_DDI_BOX& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteValue("left",		val.left );
	WriteValue("top",		val.top );
	WriteValue("front",		val.front );
	WriteValue("right",		val.right );
	WriteValue("bottom",	val.bottom );
	WriteValue("back",		val.back );
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<D3D10DDI_COUNTER_INFO>( const char* szAttribute, const D3D10DDI_COUNTER_INFO& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteValue(	"LastDeviceDependentCounter",	val.LastDeviceDependentCounter );
	WriteValue( "NumSimultaneousCounters",		val.NumSimultaneousCounters );
	WriteValue( "NumDetectableParallelUnits",	val.NumDetectableParallelUnits );
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<D3D10DDI_MIPINFO>( const char* szAttribute, const D3D10DDI_MIPINFO& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteValue(	"TexelWidth",		val.TexelWidth );
	WriteValue(	"TexelHeight",		val.TexelHeight );
	WriteValue(	"TexelDepth",		val.TexelDepth );
	WriteValue(	"PhysicalWidth",	val.PhysicalWidth );
	WriteValue(	"PhysicalHeight",	val.PhysicalHeight );
	WriteValue(	"PhysicalDepth",	val.PhysicalDepth );
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<D3DDDI_OPENALLOCATIONINFO>( const char* szAttribute, const D3DDDI_OPENALLOCATIONINFO& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteValue(	"hAllocation",				val.hAllocation );
	WriteValue(	"pPrivateDriverData",		val.pPrivateDriverData );
	WriteValue(	"PrivateDriverDataSize",	val.PrivateDriverDataSize );
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue< D3D10_DDI_PRIMITIVE_TOPOLOGY >( const char* szAttribute, const D3D10_DDI_PRIMITIVE_TOPOLOGY& val )
{
	WriteValueWithComment(szAttribute, val);
}

template <> void XMLStreamer::WriteValue<D3D10_DDI_MAP>( const char* szAttribute, const D3D10_DDI_MAP& nVal  )
{
	WriteValueWithComment(szAttribute, nVal);
}

template <> void XMLStreamer::WriteValue<D3D10_DDI_BLEND>( const char* szAttribute, const D3D10_DDI_BLEND& nVal )
{
	WriteValueWithComment(szAttribute, nVal);
}

template <> void XMLStreamer::WriteValue<D3D10_DDI_BLEND_OP>( const char* szAttribute, const D3D10_DDI_BLEND_OP& nVal )
{
	WriteValueWithComment(szAttribute, nVal);
}

template <> void XMLStreamer::WriteValue< D3D10_DDI_STENCIL_OP >( const char* szAttribute, const D3D10_DDI_STENCIL_OP& val )
{
	WriteValueWithComment(szAttribute, val);
}

template <> void XMLStreamer::WriteValue< D3D10_DDI_COMPARISON_FUNC >( const char* szAttribute, const D3D10_DDI_COMPARISON_FUNC& val )
{
	WriteValueWithComment(szAttribute, val);
}

template <> void XMLStreamer::WriteValue< D3D10_DDI_RESOURCE_USAGE >( const char* szAttribute, const D3D10_DDI_RESOURCE_USAGE& val )
{
	WriteValueWithComment(szAttribute, val);
}

template <> void XMLStreamer::WriteValue<D3D10_DDI_FILTER>( const char* szAttribute, const D3D10_DDI_FILTER& nVal  )
{
	WriteValueWithComment(szAttribute, nVal);
}

template <> void XMLStreamer::WriteValue<D3D10_DDI_TEXTURE_ADDRESS_MODE>( const char* szAttribute, const D3D10_DDI_TEXTURE_ADDRESS_MODE& nVal  )
{
	WriteValueWithComment(szAttribute, nVal);
}

template <> void XMLStreamer::WriteValue<D3D10DDIRESOURCE_TYPE>( const char* szAttribute, const D3D10DDIRESOURCE_TYPE& nVal  )
{
	WriteValueWithComment(szAttribute, nVal);
}

template <> void XMLStreamer::WriteValue<D3D11DDI_HANDLETYPE>( const char* szAttribute, const D3D11DDI_HANDLETYPE& nVal  )
{
	WriteValueWithComment(szAttribute, nVal);
}

//-----------------------------------------------------------------------------
//	WriteValue<> template specifications for D3D10DDIARG_xxxxx types
//-----------------------------------------------------------------------------

template <> void XMLStreamer::WriteValue<D3D10DDIARG_CREATEDEPTHSTENCILVIEW>( const char* szAttribute, const D3D10DDIARG_CREATEDEPTHSTENCILVIEW& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteReference(	"hDrvResource",			val.hDrvResource );
	WriteValue(		"Format",				val.Format );
	WriteValue(		"ResourceDimension",	val.ResourceDimension );
	switch( val.ResourceDimension )
	{
	case D3D10DDIRESOURCE_TEXTURE1D:
		WriteValue(	"Tex1D", val.Tex1D );
		break;
	case D3D10DDIRESOURCE_TEXTURE2D:
		WriteValue(	"Tex2D", val.Tex2D );
		break;
	case  D3D10DDIRESOURCE_TEXTURECUBE:
		WriteValue(	"TexCube", val.TexCube );
		break;
	default:
		_ASSERT(FALSE);
	}
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<D3D11DDIARG_CREATEDEPTHSTENCILVIEW>( const char* szAttribute, const D3D11DDIARG_CREATEDEPTHSTENCILVIEW& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteReference(	"hDrvResource",			val.hDrvResource );
	WriteValue(		"Format",				val.Format );
	WriteValue(		"ResourceDimension",	val.ResourceDimension );
	WriteValue(		"Flags",	val.Flags );
	switch( val.ResourceDimension )
	{
	case D3D10DDIRESOURCE_TEXTURE1D:
		WriteValue(	"Tex1D", val.Tex1D );
		break;
	case D3D10DDIRESOURCE_TEXTURE2D:
		WriteValue(	"Tex2D", val.Tex2D );
		break;
	case  D3D10DDIRESOURCE_TEXTURECUBE:
		WriteValue(	"TexCube", val.TexCube );
		break;
	default:
		_ASSERT(FALSE);
	}
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<D3D11DDIARG_CREATEUNORDEREDACCESSVIEW>( const char* szAttribute, const D3D11DDIARG_CREATEUNORDEREDACCESSVIEW& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteReference(	"hDrvResource",			val.hDrvResource );
	WriteValue(		"Format",				val.Format );
	WriteValue(		"ResourceDimension",	val.ResourceDimension );
	switch( val.ResourceDimension )
	{
	case D3D10DDIRESOURCE_BUFFER:
	case D3D11DDIRESOURCE_BUFFEREX:
		WriteValue( "Buffer", val.Buffer );
		break;
	case D3D10DDIRESOURCE_TEXTURE1D:
		WriteValue(	"Tex1D", val.Tex1D );
		break;
	case D3D10DDIRESOURCE_TEXTURE2D:
		WriteValue(	"Tex2D", val.Tex2D );
		break;
	case D3D10DDIRESOURCE_TEXTURE3D:
		WriteValue(	"Tex3D", val.Tex3D );
		break;
	default:
		_ASSERT(FALSE);
	}
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<D3D10DDIARG_CREATEELEMENTLAYOUT>( const char* szAttribute, const D3D10DDIARG_CREATEELEMENTLAYOUT& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteValue(			"NumElements",		val.NumElements );
	WriteArrayValues(	"VertexElements",	val.NumElements,	val.pVertexElements );
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<D3D10DDIARG_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT>( const char* szAttribute, const D3D10DDIARG_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteValue(			"NumEntries",		val.NumEntries );
	WriteArrayValues(	"OutputStreamDecl",	val.NumEntries,	val.pOutputStreamDecl );
	//WriteReference(	"ShaderCode",			val.pShaderCode ); // TODO: Fix it
	WriteValue(		"StreamOutputStrideInBytes",	val.StreamOutputStrideInBytes );
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<D3D10DDIARG_STAGE_IO_SIGNATURES>( const char* szAttribute, const D3D10DDIARG_STAGE_IO_SIGNATURES& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteValue(			"NumInputSignatureEntries",		val.NumInputSignatureEntries );
	WriteArrayValues(	"InputSignature",				val.NumInputSignatureEntries,	val.pInputSignature );
	WriteValue(			"NumOutputSignatureEntries",	val.NumOutputSignatureEntries );
	WriteArrayValues(	"OutputSignature",				val.NumOutputSignatureEntries,	val.pOutputSignature );
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<D3D10DDIARG_CREATEQUERY>( const char* szAttribute, const D3D10DDIARG_CREATEQUERY& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteValue(	"MiscFlags",	val.MiscFlags );
	WriteValue(	"Query",		val.Query );
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<D3D10DDIARG_CREATERESOURCE>( const char* szAttribute, const D3D10DDIARG_CREATERESOURCE& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteValue(	"MipLevels", val.MipLevels );		// записывать первыми, чтобы потом прочитать восстановить структуру
	WriteValue(	"ArraySize", val.ArraySize );

	WriteArrayValues(	"MipInfoList",			val.MipLevels,	val.pMipInfoList );
	WriteArrayValues(	"InitialDataUP",		val.MipLevels*val.ArraySize, val.pInitialDataUP );  // TODO: Fix it
	WriteValue(	"ResourceDimension",	val.ResourceDimension );
	WriteValue(	"Usage", (D3D10_DDI_RESOURCE_USAGE) val.Usage );
	WriteValue(	"BindFlags", val.BindFlags );
	WriteValue(	"MapFlags",	(D3D10_DDI_MAP) val.MapFlags );
	WriteValue(	"MiscFlags", val.MiscFlags );
	WriteValue(	"Format",	val.Format );
	WriteValue(	"SampleDesc", val.SampleDesc );

	WriteValueByRef( "PrimaryDesc",	val.pPrimaryDesc );
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<D3D11DDIARG_CREATERESOURCE>( const char* szAttribute, const D3D11DDIARG_CREATERESOURCE& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteValue(	"MipLevels", val.MipLevels );		// записывать первыми, чтобы потом прочитать восстановить структуру
	WriteValue(	"ArraySize", val.ArraySize );

	WriteArrayValues(	"MipInfoList",			val.MipLevels,	val.pMipInfoList );
	WriteArrayValues(	"InitialDataUP",		val.MipLevels*val.ArraySize, val.pInitialDataUP );  // TODO: Fix it
	WriteValue(	"ResourceDimension",	val.ResourceDimension );
	WriteValue(	"Usage", (D3D10_DDI_RESOURCE_USAGE) val.Usage );
	WriteValue(	"BindFlags", val.BindFlags );
	WriteValue(	"MapFlags",	(D3D10_DDI_MAP) val.MapFlags );
	WriteValue(	"MiscFlags", val.MiscFlags );
	WriteValue(	"Format",	val.Format );
	WriteValue(	"SampleDesc", val.SampleDesc );

	WriteValueByRef( "PrimaryDesc",	val.pPrimaryDesc );
	WriteValue(	"ByteStride", val.ByteStride );
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<D3D10DDIARG_CREATESHADERRESOURCEVIEW>( const char* szAttribute, const D3D10DDIARG_CREATESHADERRESOURCEVIEW& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteReference(	"hDrvResource",			val.hDrvResource );
	WriteValue(		"Format",				val.Format );
	WriteValue(		"ResourceDimension",	val.ResourceDimension );
	switch( val.ResourceDimension )
	{
	case D3D10DDIRESOURCE_BUFFER:
	case D3D11DDIRESOURCE_BUFFEREX:
		WriteValue( "Buffer", val.Buffer );
		break;
	case D3D10DDIRESOURCE_TEXTURE1D:
		WriteValue(	"Tex1D", val.Tex1D );
		break;
	case D3D10DDIRESOURCE_TEXTURE2D:
		WriteValue(	"Tex2D", val.Tex2D );
		break;
	case D3D10DDIRESOURCE_TEXTURE3D:
		WriteValue(	"Tex3D", val.Tex3D );
		break;
	case  D3D10DDIRESOURCE_TEXTURECUBE:
		WriteValue(	"TexCube", val.TexCube );
		break;
	default:
		_ASSERT(FALSE);
	}
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<D3D11DDIARG_CREATESHADERRESOURCEVIEW>( const char* szAttribute, const D3D11DDIARG_CREATESHADERRESOURCEVIEW& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteReference(	"hDrvResource",			val.hDrvResource );
	WriteValue(		"Format",				val.Format );
	WriteValue(		"ResourceDimension",	val.ResourceDimension );
	switch( val.ResourceDimension )
	{
	case D3D10DDIRESOURCE_BUFFER:
	case D3D11DDIRESOURCE_BUFFEREX:
		WriteValue( "Buffer", val.Buffer );
		break;
	case D3D10DDIRESOURCE_TEXTURE1D:
		WriteValue(	"Tex1D", val.Tex1D );
		break;
	case D3D10DDIRESOURCE_TEXTURE2D:
		WriteValue(	"Tex2D", val.Tex2D );
		break;
	case D3D10DDIRESOURCE_TEXTURE3D:
		WriteValue(	"Tex3D", val.Tex3D );
		break;
	case  D3D10DDIRESOURCE_TEXTURECUBE:
		WriteValue(	"TexCube", val.TexCube );
		break;
	default:
		_ASSERT(FALSE);
	}
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<D3D10_1DDIARG_CREATESHADERRESOURCEVIEW>( const char* szAttribute, const D3D10_1DDIARG_CREATESHADERRESOURCEVIEW& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteReference(	"hDrvResource",			val.hDrvResource );
	WriteValue(		"Format",				val.Format );
	WriteValue(		"ResourceDimension",	val.ResourceDimension );
	switch( val.ResourceDimension )
	{
	case D3D10DDIRESOURCE_BUFFER:
	case D3D11DDIRESOURCE_BUFFEREX:
		WriteValue( "Buffer", val.Buffer );
		break;
	case D3D10DDIRESOURCE_TEXTURE1D:
		WriteValue(	"Tex1D", val.Tex1D );
		break;
	case D3D10DDIRESOURCE_TEXTURE2D:
		WriteValue(	"Tex2D", val.Tex2D );
		break;
	case D3D10DDIRESOURCE_TEXTURE3D:
		WriteValue(	"Tex3D", val.Tex3D );
		break;
	case  D3D10DDIRESOURCE_TEXTURECUBE:
		WriteValue(	"TexCube", val.TexCube );
		break;
	default:
		_ASSERT(FALSE);
	}
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<D3D10DDIARG_CREATERENDERTARGETVIEW>( const char* szAttribute, const D3D10DDIARG_CREATERENDERTARGETVIEW& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteReference(	"hDrvResource",			val.hDrvResource );
	WriteValue(		"Format",				val.Format );
	WriteValue(		"ResourceDimension",	val.ResourceDimension );
	switch( val.ResourceDimension )
	{
	case D3D10DDIRESOURCE_BUFFER:
	case D3D11DDIRESOURCE_BUFFEREX:
		WriteValue( "Buffer", val.Buffer );
		break;
	case D3D10DDIRESOURCE_TEXTURE1D:
		WriteValue(	"Tex1D", val.Tex1D );
		break;
	case D3D10DDIRESOURCE_TEXTURE2D:
		WriteValue(	"Tex2D", val.Tex2D );
		break;
	case D3D10DDIRESOURCE_TEXTURE3D:
		WriteValue(	"Tex3D", val.Tex3D );
		break;
	case  D3D10DDIRESOURCE_TEXTURECUBE:
		WriteValue(	"TexCube", val.TexCube );
		break;
	default:
		_ASSERT(FALSE);
	}
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<D3D11DDIARG_BUFFER_UNORDEREDACCESSVIEW>( const char* szAttribute, const D3D11DDIARG_BUFFER_UNORDEREDACCESSVIEW& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteValue(	"FirstElement",		val.FirstElement );
	WriteValue(	"NumElements",			val.NumElements );
	WriteValue(	"Flags",			val.Flags );
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<D3D11DDIARG_TEX1D_UNORDEREDACCESSVIEW>( const char* szAttribute, const D3D11DDIARG_TEX1D_UNORDEREDACCESSVIEW& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteValue(	"MipSlice",		val.MipSlice );
	WriteValue(	"FirstArraySlice",	val.FirstArraySlice );
	WriteValue(	"ArraySize",			val.ArraySize );
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<D3D11DDIARG_TEX2D_UNORDEREDACCESSVIEW>( const char* szAttribute, const D3D11DDIARG_TEX2D_UNORDEREDACCESSVIEW& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteValue(	"MipSlice",		val.MipSlice );
	WriteValue(	"FirstArraySlice",	val.FirstArraySlice );
	WriteValue(	"ArraySize",			val.ArraySize );
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<D3D11DDIARG_TEX3D_UNORDEREDACCESSVIEW>( const char* szAttribute, const D3D11DDIARG_TEX3D_UNORDEREDACCESSVIEW& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteValue(	"MipSlice",		val.MipSlice );
	WriteValue(	"FirstW",	val.FirstW );
	WriteValue(	"WSize",			val.WSize );
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<D3D10DDIARG_TEX1D_DEPTHSTENCILVIEW>( const char* szAttribute, const D3D10DDIARG_TEX1D_DEPTHSTENCILVIEW& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteValue(	"ArraySize",		val.ArraySize );
	WriteValue(	"FirstArraySlice",	val.FirstArraySlice );
	WriteValue(	"MipSlice",			val.MipSlice );
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<D3D10DDIARG_TEX2D_DEPTHSTENCILVIEW>( const char* szAttribute, const D3D10DDIARG_TEX2D_DEPTHSTENCILVIEW& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteValue(	"ArraySize",		val.ArraySize );
	WriteValue(	"FirstArraySlice",	val.FirstArraySlice );
	WriteValue(	"MipSlice",			val.MipSlice );
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<D3D10DDIARG_TEXCUBE_DEPTHSTENCILVIEW>( const char* szAttribute, const D3D10DDIARG_TEXCUBE_DEPTHSTENCILVIEW& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteValue(	"ArraySize",		val.ArraySize );
	WriteValue(	"FirstArraySlice",	val.FirstArraySlice );
	WriteValue(	"MipSlice",			val.MipSlice );
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<D3D10DDIARG_TEX1D_RENDERTARGETVIEW>( const char* szAttribute, const D3D10DDIARG_TEX1D_RENDERTARGETVIEW& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteValue(	"MipSlice",			val.MipSlice );
	WriteValue(	"FirstArraySlice",	val.FirstArraySlice );
	WriteValue(	"ArraySize",		val.ArraySize );
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<D3D10DDIARG_TEX2D_RENDERTARGETVIEW>( const char* szAttribute, const D3D10DDIARG_TEX2D_RENDERTARGETVIEW& val )
{
	m_pXMLWriter->StartElement( szAttribute );	
	WriteValue(	"MipSlice",			val.MipSlice );
	WriteValue(	"FirstArraySlice",	val.FirstArraySlice );
	WriteValue(	"ArraySize",		val.ArraySize );
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<D3D10DDIARG_TEX3D_RENDERTARGETVIEW>( const char* szAttribute, const D3D10DDIARG_TEX3D_RENDERTARGETVIEW& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteValue(	"MipSlice",	val.MipSlice );
	WriteValue(	"FirstW",	val.FirstW );
	WriteValue(	"WSize",	val.WSize );
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<D3D10DDIARG_TEXCUBE_RENDERTARGETVIEW>( const char* szAttribute, const D3D10DDIARG_TEXCUBE_RENDERTARGETVIEW& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteValue(	"MipSlice",			val.MipSlice );
	WriteValue(	"FirstArraySlice",	val.FirstArraySlice );
	WriteValue(	"ArraySize",		val.ArraySize );
	m_pXMLWriter->EndElement();
}


template <> void XMLStreamer::WriteValue<D3D10DDIARG_TEX1D_SHADERRESOURCEVIEW>( const char* szAttribute, const D3D10DDIARG_TEX1D_SHADERRESOURCEVIEW& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteValue(	"ArraySize",		val.ArraySize );
	WriteValue(	"FirstArraySlice",	val.FirstArraySlice );
	WriteValue(	"MipLevels",		val.MipLevels );
	WriteValue(	"MostDetailedMip",	val.MostDetailedMip );
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<D3D10DDIARG_TEX2D_SHADERRESOURCEVIEW>( const char* szAttribute, const D3D10DDIARG_TEX2D_SHADERRESOURCEVIEW& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteValue(	"ArraySize",		val.ArraySize );
	WriteValue(	"FirstArraySlice",	val.FirstArraySlice );
	WriteValue(	"MipLevels",		val.MipLevels );
	WriteValue(	"MostDetailedMip",	val.MostDetailedMip );
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<D3D10DDIARG_TEX3D_SHADERRESOURCEVIEW>( const char* szAttribute, const D3D10DDIARG_TEX3D_SHADERRESOURCEVIEW& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteValue(	"MipLevels",		val.MipLevels );
	WriteValue(	"MostDetailedMip",	val.MostDetailedMip );
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<D3D10DDIARG_TEXCUBE_SHADERRESOURCEVIEW>( const char* szAttribute, const D3D10DDIARG_TEXCUBE_SHADERRESOURCEVIEW& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteValue(	"MipLevels",		val.MipLevels );
	WriteValue(	"MostDetailedMip",	val.MostDetailedMip );
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<D3D10_1DDIARG_TEXCUBE_SHADERRESOURCEVIEW>( const char* szAttribute, const D3D10_1DDIARG_TEXCUBE_SHADERRESOURCEVIEW& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteValue(	"MipLevels",		val.MipLevels );
	WriteValue(	"MostDetailedMip",	val.MostDetailedMip );
	WriteValue(	"NumCubes",			val.NumCubes );
	WriteValue( "First2DArrayFace", val.First2DArrayFace );
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<D3D10DDIARG_BUFFER_SHADERRESOURCEVIEW>( const char* szAttribute, const D3D10DDIARG_BUFFER_SHADERRESOURCEVIEW& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteValue(	"ElementOffset",	val.ElementOffset );
	WriteValue(	"ElementWidth",		val.ElementWidth );
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<D3D10DDIARG_BUFFER_RENDERTARGETVIEW>( const char* szAttribute, const D3D10DDIARG_BUFFER_RENDERTARGETVIEW& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteValue(	"ElementOffset",	val.ElementOffset );
	WriteValue(	"ElementWidth",		val.ElementWidth );
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<D3D10DDIARG_INPUT_ELEMENT_DESC>( const char* szAttribute, const D3D10DDIARG_INPUT_ELEMENT_DESC& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteValue(	"InputSlot",			val.InputSlot );
	WriteValue(	"AlignedByteOffset",	val.AlignedByteOffset );
	WriteValue(	"Format",				val.Format );
	WriteValue(	"InputSlotClass",		val.InputSlotClass );
	WriteValue(	"InstanceDataStepRate",	val.InstanceDataStepRate );
	WriteValue(	"InputRegister",		val.InputRegister );
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<D3D10DDIARG_STREAM_OUTPUT_DECLARATION_ENTRY>( const char* szAttribute, const D3D10DDIARG_STREAM_OUTPUT_DECLARATION_ENTRY& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteValue(	"OutputSlot",		val.OutputSlot );
	WriteValue(	"RegisterIndex",	val.RegisterIndex );
	WriteValue(	"RegisterMask",		val.RegisterMask );
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<D3D10DDIARG_SIGNATURE_ENTRY>( const char* szAttribute, const D3D10DDIARG_SIGNATURE_ENTRY& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteValue(	"SystemValue",	val.SystemValue );
	WriteValue(	"Register",		val.Register );
	WriteValue(	"Mask",			val.Mask );
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<D3D10DDIARG_OPENRESOURCE>( const char* szAttribute, const D3D10DDIARG_OPENRESOURCE& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteValue(	"NumAllocations",			val.NumAllocations );
	WriteValue(	"pOpenAllocationInfo",		*(val.pOpenAllocationInfo) );
	WriteValue(	"hKMResource",				val.hKMResource.handle );
	WriteValue(	"pPrivateDriverData",		val.pPrivateDriverData ); // TODO: Fix it
	WriteValue(	"PrivateDriverDataSize",	val.PrivateDriverDataSize );
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<D3D10_DDIARG_SUBRESOURCE_UP>( const char* szAttribute, const D3D10_DDIARG_SUBRESOURCE_UP& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteValue(	"pSysMem",		val.pSysMem );
	WriteValue(	"SysMemPitch",		val.SysMemPitch );
	WriteValue(	"SysMemSlicePitch",	val.SysMemSlicePitch );
	m_pXMLWriter->EndElement();
}


//-----------------------------------------------------------------------------
//	WriteValue<> template specifications for DXGI_xxxxx types
//-----------------------------------------------------------------------------
template <> void XMLStreamer::WriteValue<DXGI_FORMAT>( const char* szAttribute, const DXGI_FORMAT& val )
{
	WriteValueWithComment(szAttribute, val);
}

template <> void XMLStreamer::WriteValue<DXGI_SAMPLE_DESC>( const char* szAttribute, const DXGI_SAMPLE_DESC& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteValue("Count",		val.Count );
	WriteValue("Quality",	val.Quality );
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<DXGI_GAMMA_CONTROL_CAPABILITIES>( const char* szAttribute, const DXGI_GAMMA_CONTROL_CAPABILITIES& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteValue("ScaleAndOffsetSupported", val.ScaleAndOffsetSupported );
	WriteValue("MaxConvertedValue", val.MaxConvertedValue );
	WriteValue("MinConvertedValue", val.MinConvertedValue );
	WriteValue("NumGammaControlPoints", val.NumGammaControlPoints );	
	WriteArrayValues("ControlPointPositions", val.NumGammaControlPoints, val.ControlPointPositions );
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<DXGI_DDI_FLIP_INTERVAL_TYPE>( const char* szAttribute, const DXGI_DDI_FLIP_INTERVAL_TYPE& val )
{
	WriteValueWithComment(szAttribute, val);
}

template <> void XMLStreamer::WriteValue<DXGI_DDI_RESIDENCY>( const char* szAttribute, const DXGI_DDI_RESIDENCY& val )
{
	WriteValueWithComment(szAttribute, val);
}

template <> void XMLStreamer::WriteValue<DXGI_DDI_PRIMARY_DESC>( const char* szAttribute, const DXGI_DDI_PRIMARY_DESC& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteValue( "Flags", val.Flags );
	WriteValue( "VidPnSourceId", val.VidPnSourceId );
	WriteValue( "ModeDesc", val.ModeDesc );
	WriteValue( "DriverFlags", val.DriverFlags );
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<DXGI_DDI_MODE_DESC>( const char* szAttribute, const DXGI_DDI_MODE_DESC& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteValue( "Width", val.Width );
	WriteValue( "Height", val.Height );
	WriteValue( "Format", val.Format );
	WriteValue( "RefreshRate", val.RefreshRate );
	WriteValue( "ScanlineOrdering", val.ScanlineOrdering );
	WriteValue( "Rotation", val.Rotation );
	WriteValue( "Scaling", val.Scaling );
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<DXGI_DDI_RATIONAL>( const char* szAttribute, const DXGI_DDI_RATIONAL& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteValue( "Numerator", val.Numerator );
	WriteValue( "Denominator", val.Denominator );
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<DXGI_DDI_MODE_SCANLINE_ORDER>( const char* szAttribute, const DXGI_DDI_MODE_SCANLINE_ORDER & val )
{
	WriteValueWithComment(szAttribute, val);
}

template <> void XMLStreamer::WriteValue<DXGI_DDI_MODE_ROTATION>( const char* szAttribute, const DXGI_DDI_MODE_ROTATION & val )
{
	WriteValueWithComment(szAttribute, val);
}

template <> void XMLStreamer::WriteValue<DXGI_DDI_MODE_SCALING>( const char* szAttribute, const DXGI_DDI_MODE_SCALING & val )
{
	WriteValueWithComment(szAttribute, val);
}

//-----------------------------------------------------------------------------
//	WriteValue<> template specifications for DXGI_DDI_ARG_xxxxx types
//-----------------------------------------------------------------------------

#define DXGI_2_D3D_DEVICE		MAKE_D3D10DDI_HDEVICE( (void*)(((BYTE*)val.hDevice)+sizeof(void*)) )
#define DXGI_2_D3D_RESOURCE(x)	MAKE_D3D10DDI_HRESOURCE( (void*)x )

template <> void XMLStreamer::WriteValue<DXGI_DDI_ARG_PRESENT>( const char* szAttribute, const DXGI_DDI_ARG_PRESENT& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteReference(	"hDevice",				DXGI_2_D3D_DEVICE );
	WriteReference(	"hSurfaceToPresent",	DXGI_2_D3D_RESOURCE(val.hSurfaceToPresent) );
	WriteValue(		"SrcSubResourceIndex",	val.SrcSubResourceIndex );
	WriteReference(	"hDstResource",			DXGI_2_D3D_RESOURCE(val.hDstResource) );
	WriteValue(		"DstSubResourceIndex",	val.DstSubResourceIndex );
	WriteReference(	"pDXGIContext",			DXGI_2_D3D_RESOURCE(val.pDXGIContext) );
	WriteValue(		"Flags",				val.Flags.Value );
	WriteValue(		"FlipInterval",			val.FlipInterval );
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<DXGI_DDI_ARG_BLT>( const char* szAttribute, const DXGI_DDI_ARG_BLT& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteReference(	"hDevice",				DXGI_2_D3D_DEVICE );
	WriteReference(	"hDstResource",			DXGI_2_D3D_RESOURCE(val.hDstResource) );
	WriteValue(		"DstSubresource",		val.DstSubresource );
	WriteValue(		"DstLeft",				val.DstLeft );
	WriteValue(		"DstTop",				val.DstTop );
	WriteValue(		"DstRight",				val.DstRight );
	WriteValue(		"DstBottom",			val.DstBottom );
	WriteReference(	"hSrcResource",			DXGI_2_D3D_RESOURCE(val.hSrcResource) );
	WriteValue(		"SrcSubresource",		val.SrcSubresource );
	WriteValue(		"Flags",				val.Flags.Value );
	WriteValue(		"Rotate",				val.Rotate );
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<DXGI_DDI_ARG_RESOLVESHAREDRESOURCE>( const char* szAttribute, const DXGI_DDI_ARG_RESOLVESHAREDRESOURCE& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteReference(	"hDevice",		DXGI_2_D3D_DEVICE );
	WriteReference(	"hResource",	DXGI_2_D3D_RESOURCE(val.hResource) );
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<DXGI_DDI_ARG_SETRESOURCEPRIORITY>( const char* szAttribute, const DXGI_DDI_ARG_SETRESOURCEPRIORITY& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteReference("hDevice",	DXGI_2_D3D_DEVICE );
	WriteReference("hResource",	DXGI_2_D3D_RESOURCE(val.hResource) );
	WriteValue("Priority",		val.Priority );
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<DXGI_DDI_ARG_GET_GAMMA_CONTROL_CAPS>( const char* szAttribute, const DXGI_DDI_ARG_GET_GAMMA_CONTROL_CAPS& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteReference("hDevice",			DXGI_2_D3D_DEVICE );
	WriteValue("pGammaCapabilities",	*val.pGammaCapabilities );
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<DXGI_DDI_ARG_ROTATE_RESOURCE_IDENTITIES>( const char* szAttribute, const DXGI_DDI_ARG_ROTATE_RESOURCE_IDENTITIES& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteReference("hDevice",		DXGI_2_D3D_DEVICE );
	WriteValue("ResourcesCount",	val.Resources );
	WriteArrayReferences("Resources",val.Resources,  (CONST D3D10DDI_HRESOURCE*)val.pResources );
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<DXGI_DDI_ARG_SETDISPLAYMODE>( const char* szAttribute, const DXGI_DDI_ARG_SETDISPLAYMODE& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteReference("hDevice",	DXGI_2_D3D_DEVICE );
	WriteReference("hResource",	DXGI_2_D3D_RESOURCE(val.hResource) );
	WriteValue("SubResourceIndex", val.SubResourceIndex );
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<DXGI_DDI_ARG_QUERYRESOURCERESIDENCY>( const char* szAttribute, const DXGI_DDI_ARG_QUERYRESOURCERESIDENCY& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteReference("hDevice",			DXGI_2_D3D_DEVICE );
	WriteValue("ResourcesCount",		val.Resources );
	WriteArrayReferences("Resources",	val.Resources,  (CONST D3D10DDI_HRESOURCE*)val.pResources );
	WriteArrayValues("Status",			val.Resources,  val.pStatus );
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<CBaseResource::THistory>( const char* szAttribute, const CBaseResource::THistory& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	int nCount = (int)val.size();
	WriteValue("NumRevisions",	nCount );
	CBaseResource::THistory::const_iterator it = val.begin();	
	for ( ; it != val.end(); ++it )
	{
		const CBaseResource::THistoryElement& ch_vector = it->second;
		m_pXMLWriter->StartElement( "REVISION" );
		WriteValue("NumDataChunks", ch_vector.size() );
		CBaseResource::THistoryElement::const_iterator ch_it = ch_vector.begin();
		for( ; ch_it != ch_vector.end(); ++ch_it )
		{
			WriteValue( "DATA_CHUNK", *ch_it );
		}
		m_pXMLWriter->EndElement();
	}
	m_pXMLWriter->EndElement();
}

template <> void XMLStreamer::WriteValue<ArchiveFile::TArchiveChunk>( const char* szAttribute, const ArchiveFile::TArchiveChunk& val )
{
	m_pXMLWriter->StartElement( szAttribute );
	WriteValue("Offset",			val.nOffset );
	WriteValue("SizeBeforArchive",	val.nSizeBeforArchive );
	WriteValue("SizeAfterArchive",	val.nSizeAfterArchive );
	m_pXMLWriter->EndElement();	
}

//////////////////////////////////////////////////////////////////////////