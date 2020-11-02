#ifndef __S3D_XMLSTREAMER_WRITEVALUE_H__
#define __S3D_XMLSTREAMER_WRITEVALUE_H__

#include "../S3DWrapper10/Commands/Command.h"

template < class T >
inline std::string XMLStreamer::ToString( const T& value ) const
{
	std::stringstream convert;
	convert << value;
	if ( convert.fail() )
		return "";
	return convert.str();
}

inline std::string XMLStreamer::ToString( const std::string& value ) const
{
	return value;
}

//////////////////////////////////////////////////////////////////////////
/*template <typename T>
void XMLStreamer::WriteRefValue_( const char* szAttribute, const T* pVal /*= m_pCommandElem * / )
{
	WriteValue( szAttribute, (LONG_PTR)pVal );
	//WriteValue( szAttribute, 0, pParent);
}*/

template <typename T>
void XMLStreamer::WriteValueByRef( const char* szAttribute, const T* pVal /*= m_pCommandElem */ )
{
	if( pVal )	
	{
		WriteValue( szAttribute, *pVal );
		//WriteValue( szAttribute, 0 );
	}
	else
	{
		m_pXMLWriter->StartElement( szAttribute );
		m_pXMLWriter->WriteAttribute( "isNull", TRUE );
		m_pXMLWriter->EndElement();
	}
}

template <typename T> void XMLStreamer::WriteReference( const char* szAttribute, const T&	pVal )
{
	m_pXMLWriter->StartElement( szAttribute );
	//m_pXMLWriter->WriteAttribute("ref", m_pCurrentRM->GetID(pVal));
	m_pCurrentRM->GetID(pVal);
	m_pXMLWriter->WriteAttribute("val", (UINT_PTR)pVal.pDrvPrivate);
	m_pXMLWriter->EndElement();
}

template <typename T> void XMLStreamer::WriteValue( const char* szAttribute, const T& nVal )
{
	m_pXMLWriter->StartElement( szAttribute );
	m_pXMLWriter->WriteAttribute("val", ToString(nVal).c_str());
	m_pXMLWriter->EndElement();
}

template <typename T> void XMLStreamer::WriteValueWithComment( const char* szAttribute, const T& nVal )
{
	m_pXMLWriter->WriteComment( EnumToString(nVal) );

	m_pXMLWriter->StartElement( szAttribute );
	UINT value = (UINT)nVal;
	m_pXMLWriter->WriteAttribute("val", value);
	m_pXMLWriter->EndElement();
}

template <typename T> void XMLStreamer::WriteArrayValues( const char* szAttribute, SIZE_T nCount, const T*  pArray )
{
	if ( !nCount || !pArray )
	{
		WriteValue( szAttribute, 0 );
		return;
	}

	m_pXMLWriter->StartElement( szAttribute );
	m_pXMLWriter->WriteAttribute("count", nCount);
	for ( size_t i = 0; i < nCount; i++ )	
		WriteValue( szAttribute, pArray[i] );
	m_pXMLWriter->EndElement( );
}

template <typename T, typename A> void XMLStreamer::WriteArrayValues( const char* szAttribute, SIZE_T nCount, const std::vector<T, A>  &pArray )
{
	WriteArrayValues(szAttribute, nCount, GetPointerToVector(pArray));
}

template <typename T, typename A, typename Tr> void XMLStreamer::WriteArrayValues( const char* szAttribute, SIZE_T nCount, const array<T, A, Tr>  &pArray )
{
	WriteArrayValues(szAttribute, nCount, GetPointerToVector(pArray));
}

template <typename T> void XMLStreamer::WriteArrayReferences( const char* szAttribute, SIZE_T nCount, const T*  pArray )
{
	if ( !nCount || !pArray )
	{
		WriteValue( szAttribute, 0 );
		return;
	}

	m_pXMLWriter->StartElement( szAttribute );
	m_pXMLWriter->WriteAttribute("count", nCount);
	for ( size_t i = 0; i < nCount; i++ )
		WriteReference( "item", pArray[i] );
	m_pXMLWriter->EndElement( );
}

template <typename T, typename A> void XMLStreamer::WriteArrayReferences( const char* szAttribute, SIZE_T nCount, const std::vector<T, A>  &pArray )
{
	WriteArrayReferences(szAttribute, nCount, GetPointerToVector(pArray));
}

template <typename T, typename A, typename Tr> void XMLStreamer::WriteArrayReferences( const char* szAttribute, SIZE_T nCount, const array<T, A, Tr>  &pArray )
{
	WriteArrayReferences(szAttribute, nCount, GetPointerToVector(pArray));
}

//////////////////////////////////////////////////////////////////////////

template <> void XMLStreamer::WriteValue<UINT8>( const char* szAttribute, const UINT8& val );
template <> void XMLStreamer::WriteValue<SHADER_PIPELINE>( const char* szAttribute, const SHADER_PIPELINE& val );
template <> void XMLStreamer::WriteValue<VIEWINDEX>( const char* szAttribute, const VIEWINDEX& val );
template <> void XMLStreamer::WriteValue<D3D10_SB_NAME>( const char* szAttribute, const D3D10_SB_NAME& _val );
// D3D10_DDI_xxxxx_DESC
template <> void XMLStreamer::WriteValue<D3D10_DDI_RENDER_TARGET_BLEND_DESC1>( const char* szAttribute, const D3D10_DDI_RENDER_TARGET_BLEND_DESC1& val );
template <> void XMLStreamer::WriteValue<D3D10_DDI_BLEND_DESC>( const char* szAttribute, const D3D10_DDI_BLEND_DESC& val );
template <> void XMLStreamer::WriteValue<D3D10_1_DDI_BLEND_DESC>( const char* szAttribute, const D3D10_1_DDI_BLEND_DESC& val );
template <> void XMLStreamer::WriteValue<D3D10_DDI_DEPTH_STENCIL_DESC>( const char* szAttribute, const D3D10_DDI_DEPTH_STENCIL_DESC& val );
template <> void XMLStreamer::WriteValue<D3D10_DDI_RASTERIZER_DESC>( const char* szAttribute, const D3D10_DDI_RASTERIZER_DESC& val );
template <> void XMLStreamer::WriteValue<D3D10_DDI_SAMPLER_DESC>( const char* szAttribute, const D3D10_DDI_SAMPLER_DESC& val );
template <> void XMLStreamer::WriteValue<D3D10_DDI_DEPTH_STENCILOP_DESC>( const char* szAttribute, const D3D10_DDI_DEPTH_STENCILOP_DESC& val );

// D3D10_DDI_xxxxx
template <> void XMLStreamer::WriteValue<D3D10_DDI_MAP>( const char* szAttribute, const D3D10_DDI_MAP& nVal );
template <> void XMLStreamer::WriteValue<D3D10_DDI_BLEND>( const char* szAttribute, const D3D10_DDI_BLEND& nVal );
template <> void XMLStreamer::WriteValue<D3D10_DDI_BLEND_OP>( const char* szAttribute, const D3D10_DDI_BLEND_OP& nVal );
template <> void XMLStreamer::WriteValue<D3D10_DDI_RECT>( const char* szAttribute, const D3D10_DDI_RECT& val );
template <> void XMLStreamer::WriteValue<D3D10_DDI_BOX>( const char* szAttribute, const D3D10_DDI_BOX& val );
template <> void XMLStreamer::WriteValue<D3D10_DDI_VIEWPORT>( const char* szAttribute, const D3D10_DDI_VIEWPORT& val );
template <> void XMLStreamer::WriteValue<D3D10DDI_MAPPED_SUBRESOURCE>( const char* szAttribute, const D3D10DDI_MAPPED_SUBRESOURCE& val );
template <> void XMLStreamer::WriteValue<D3D10DDI_COUNTER_INFO>( const char* szAttribute, const D3D10DDI_COUNTER_INFO& val );
template <> void XMLStreamer::WriteValue<D3D10DDI_MIPINFO>( const char* szAttribute, const D3D10DDI_MIPINFO& val );
template <> void XMLStreamer::WriteValue<D3DDDI_OPENALLOCATIONINFO>( const char* szAttribute, const D3DDDI_OPENALLOCATIONINFO& val );
template <> void XMLStreamer::WriteValue<D3D10_DDI_PRIMITIVE_TOPOLOGY>( const char* szAttribute, const D3D10_DDI_PRIMITIVE_TOPOLOGY& val );
template <> void XMLStreamer::WriteValue<D3D10_DDI_STENCIL_OP>( const char* szAttribute, const D3D10_DDI_STENCIL_OP& val );
template <> void XMLStreamer::WriteValue<D3D10_DDI_COMPARISON_FUNC>( const char* szAttribute, const D3D10_DDI_COMPARISON_FUNC& val );
template <> void XMLStreamer::WriteValue<D3D10_DDI_RESOURCE_USAGE>( const char* szAttribute, const D3D10_DDI_RESOURCE_USAGE& val );
template <> void XMLStreamer::WriteValue<D3D10_DDI_MAP>( const char* szAttribute, const D3D10_DDI_MAP& val );
template <> void XMLStreamer::WriteValue<D3D10_DDI_FILTER>( const char* szAttribute, const D3D10_DDI_FILTER& val );
template <> void XMLStreamer::WriteValue<D3D10_DDI_TEXTURE_ADDRESS_MODE>( const char* szAttribute, const D3D10_DDI_TEXTURE_ADDRESS_MODE& val );
template <> void XMLStreamer::WriteValue<D3D10DDIRESOURCE_TYPE>( const char* szAttribute, const D3D10DDIRESOURCE_TYPE& val );
template <> void XMLStreamer::WriteValue<D3D11DDI_HANDLETYPE>( const char* szAttribute, const D3D11DDI_HANDLETYPE& val );

// D3D10DDIARG_xxxxx
template <> void XMLStreamer::WriteValue<D3D10DDIARG_CREATEDEPTHSTENCILVIEW>( const char* szAttribute, const D3D10DDIARG_CREATEDEPTHSTENCILVIEW& val );
template <> void XMLStreamer::WriteValue<D3D11DDIARG_CREATEDEPTHSTENCILVIEW>( const char* szAttribute, const D3D11DDIARG_CREATEDEPTHSTENCILVIEW& val );
template <> void XMLStreamer::WriteValue<D3D11DDIARG_CREATEUNORDEREDACCESSVIEW>( const char* szAttribute, const D3D11DDIARG_CREATEUNORDEREDACCESSVIEW& val );
template <> void XMLStreamer::WriteValue<D3D10DDIARG_CREATEELEMENTLAYOUT>( const char* szAttribute, const D3D10DDIARG_CREATEELEMENTLAYOUT& val );
template <> void XMLStreamer::WriteValue<D3D10DDIARG_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT>( const char* szAttribute, const D3D10DDIARG_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT& val );
template <> void XMLStreamer::WriteValue<D3D10DDIARG_STAGE_IO_SIGNATURES>( const char* szAttribute, const D3D10DDIARG_STAGE_IO_SIGNATURES& val );
template <> void XMLStreamer::WriteValue<D3D10DDIARG_OPENRESOURCE>( const char* szAttribute, const D3D10DDIARG_OPENRESOURCE& val );
template <> void XMLStreamer::WriteValue<D3D10DDIARG_CREATEQUERY>( const char* szAttribute, const D3D10DDIARG_CREATEQUERY& val );
template <> void XMLStreamer::WriteValue<D3D10DDIARG_CREATERENDERTARGETVIEW>( const char* szAttribute, const D3D10DDIARG_CREATERENDERTARGETVIEW& val );
template <> void XMLStreamer::WriteValue<D3D10DDIARG_CREATERESOURCE>( const char* szAttribute, const D3D10DDIARG_CREATERESOURCE& val );
template <> void XMLStreamer::WriteValue<D3D11DDIARG_CREATERESOURCE>( const char* szAttribute, const D3D11DDIARG_CREATERESOURCE& val );
template <> void XMLStreamer::WriteValue<D3D10DDIARG_CREATESHADERRESOURCEVIEW>( const char* szAttribute, const D3D10DDIARG_CREATESHADERRESOURCEVIEW& val );
template <> void XMLStreamer::WriteValue<D3D11DDIARG_CREATESHADERRESOURCEVIEW>( const char* szAttribute, const D3D11DDIARG_CREATESHADERRESOURCEVIEW& val );
template <> void XMLStreamer::WriteValue<D3D10_1DDIARG_CREATESHADERRESOURCEVIEW>( const char* szAttribute, const D3D10_1DDIARG_CREATESHADERRESOURCEVIEW& val );
template <> void XMLStreamer::WriteValue<D3D11DDIARG_BUFFER_UNORDEREDACCESSVIEW>( const char* szAttribute, const D3D11DDIARG_BUFFER_UNORDEREDACCESSVIEW& val );
template <> void XMLStreamer::WriteValue<D3D11DDIARG_TEX1D_UNORDEREDACCESSVIEW>( const char* szAttribute, const D3D11DDIARG_TEX1D_UNORDEREDACCESSVIEW& val );
template <> void XMLStreamer::WriteValue<D3D11DDIARG_TEX2D_UNORDEREDACCESSVIEW>( const char* szAttribute, const D3D11DDIARG_TEX2D_UNORDEREDACCESSVIEW& val );
template <> void XMLStreamer::WriteValue<D3D11DDIARG_TEX3D_UNORDEREDACCESSVIEW>( const char* szAttribute, const D3D11DDIARG_TEX3D_UNORDEREDACCESSVIEW& val );
template <> void XMLStreamer::WriteValue<D3D10DDIARG_TEX1D_DEPTHSTENCILVIEW>( const char* szAttribute, const D3D10DDIARG_TEX1D_DEPTHSTENCILVIEW& val );
template <> void XMLStreamer::WriteValue<D3D10DDIARG_TEX2D_DEPTHSTENCILVIEW>( const char* szAttribute, const D3D10DDIARG_TEX2D_DEPTHSTENCILVIEW& val );
template <> void XMLStreamer::WriteValue<D3D10DDIARG_TEXCUBE_DEPTHSTENCILVIEW>( const char* szAttribute, const D3D10DDIARG_TEXCUBE_DEPTHSTENCILVIEW& val );
template <> void XMLStreamer::WriteValue<D3D10DDIARG_TEX1D_RENDERTARGETVIEW>( const char* szAttribute, const D3D10DDIARG_TEX1D_RENDERTARGETVIEW& val );
template <> void XMLStreamer::WriteValue<D3D10DDIARG_TEX2D_RENDERTARGETVIEW>( const char* szAttribute, const D3D10DDIARG_TEX2D_RENDERTARGETVIEW& val );
template <> void XMLStreamer::WriteValue<D3D10DDIARG_TEX3D_RENDERTARGETVIEW>( const char* szAttribute, const D3D10DDIARG_TEX3D_RENDERTARGETVIEW& val );
template <> void XMLStreamer::WriteValue<D3D10DDIARG_TEXCUBE_RENDERTARGETVIEW>( const char* szAttribute, const D3D10DDIARG_TEXCUBE_RENDERTARGETVIEW& val );
template <> void XMLStreamer::WriteValue<D3D10DDIARG_TEX1D_SHADERRESOURCEVIEW>( const char* szAttribute, const D3D10DDIARG_TEX1D_SHADERRESOURCEVIEW& val );
template <> void XMLStreamer::WriteValue<D3D10DDIARG_TEX2D_SHADERRESOURCEVIEW>( const char* szAttribute, const D3D10DDIARG_TEX2D_SHADERRESOURCEVIEW& val );
template <> void XMLStreamer::WriteValue<D3D10DDIARG_TEX3D_SHADERRESOURCEVIEW>( const char* szAttribute, const D3D10DDIARG_TEX3D_SHADERRESOURCEVIEW& val );
template <> void XMLStreamer::WriteValue<D3D10DDIARG_TEXCUBE_SHADERRESOURCEVIEW>( const char* szAttribute, const D3D10DDIARG_TEXCUBE_SHADERRESOURCEVIEW& val );
template <> void XMLStreamer::WriteValue<D3D10_1DDIARG_TEXCUBE_SHADERRESOURCEVIEW>( const char* szAttribute, const D3D10_1DDIARG_TEXCUBE_SHADERRESOURCEVIEW& val );
template <> void XMLStreamer::WriteValue<D3D10DDIARG_BUFFER_SHADERRESOURCEVIEW>( const char* szAttribute, const D3D10DDIARG_BUFFER_SHADERRESOURCEVIEW& val );
template <> void XMLStreamer::WriteValue<D3D10DDIARG_BUFFER_RENDERTARGETVIEW>( const char* szAttribute, const D3D10DDIARG_BUFFER_RENDERTARGETVIEW& val );
template <> void XMLStreamer::WriteValue<D3D10DDIARG_INPUT_ELEMENT_DESC>( const char* szAttribute, const D3D10DDIARG_INPUT_ELEMENT_DESC& val );
template <> void XMLStreamer::WriteValue<D3D10DDIARG_STREAM_OUTPUT_DECLARATION_ENTRY>( const char* szAttribute, const D3D10DDIARG_STREAM_OUTPUT_DECLARATION_ENTRY& val );
template <> void XMLStreamer::WriteValue<D3D10DDIARG_SIGNATURE_ENTRY>( const char* szAttribute, const D3D10DDIARG_SIGNATURE_ENTRY& val );
template <> void XMLStreamer::WriteValue<D3D10DDIARG_OPENRESOURCE>( const char* szAttribute, const D3D10DDIARG_OPENRESOURCE& val );
template <> void XMLStreamer::WriteValue<D3D10_DDIARG_SUBRESOURCE_UP>( const char* szAttribute, const D3D10_DDIARG_SUBRESOURCE_UP& val );

// DXGI_xxxxx
template <> void XMLStreamer::WriteValue<DXGI_GAMMA_CONTROL_CAPABILITIES>( const char* szAttribute, const DXGI_GAMMA_CONTROL_CAPABILITIES& val );
template <> void XMLStreamer::WriteValue<DXGI_FORMAT>( const char* szAttribute, const DXGI_FORMAT& val );
template <> void XMLStreamer::WriteValue<DXGI_SAMPLE_DESC>( const char* szAttribute, const DXGI_SAMPLE_DESC& val );
template <> void XMLStreamer::WriteValue<DXGI_DDI_FLIP_INTERVAL_TYPE>( const char* szAttribute, const DXGI_DDI_FLIP_INTERVAL_TYPE& val );
template <> void XMLStreamer::WriteValue<DXGI_DDI_RESIDENCY>( const char* szAttribute, const DXGI_DDI_RESIDENCY& val );
template <> void XMLStreamer::WriteValue<DXGI_DDI_PRIMARY_DESC>( const char* szAttribute, const DXGI_DDI_PRIMARY_DESC& val );
template <> void XMLStreamer::WriteValue<DXGI_DDI_MODE_DESC>( const char* szAttribute, const DXGI_DDI_MODE_DESC& val );
template <> void XMLStreamer::WriteValue<DXGI_DDI_RATIONAL>( const char* szAttribute, const DXGI_DDI_RATIONAL& val );
template <> void XMLStreamer::WriteValue<DXGI_DDI_MODE_SCANLINE_ORDER>( const char* szAttribute, const DXGI_DDI_MODE_SCANLINE_ORDER& val );
template <> void XMLStreamer::WriteValue<DXGI_DDI_MODE_ROTATION>( const char* szAttribute, const DXGI_DDI_MODE_ROTATION& val );
template <> void XMLStreamer::WriteValue<DXGI_DDI_MODE_SCALING>( const char* szAttribute, const DXGI_DDI_MODE_SCALING& val );

// DXGI_DDI_ARG_xxxxx
template <> void XMLStreamer::WriteValue<DXGI_DDI_ARG_PRESENT>( const char* szAttribute, const DXGI_DDI_ARG_PRESENT& val );
template <> void XMLStreamer::WriteValue<DXGI_DDI_ARG_BLT>( const char* szAttribute, const DXGI_DDI_ARG_BLT& val );
template <> void XMLStreamer::WriteValue<DXGI_DDI_ARG_SETRESOURCEPRIORITY>( const char* szAttribute, const DXGI_DDI_ARG_SETRESOURCEPRIORITY& val );
template <> void XMLStreamer::WriteValue<DXGI_DDI_ARG_GET_GAMMA_CONTROL_CAPS>( const char* szAttribute, const DXGI_DDI_ARG_GET_GAMMA_CONTROL_CAPS& val );
template <> void XMLStreamer::WriteValue<DXGI_DDI_ARG_ROTATE_RESOURCE_IDENTITIES>( const char* szAttribute, const DXGI_DDI_ARG_ROTATE_RESOURCE_IDENTITIES& val );
template <> void XMLStreamer::WriteValue<DXGI_DDI_ARG_SETDISPLAYMODE>( const char* szAttribute, const DXGI_DDI_ARG_SETDISPLAYMODE& val );
template <> void XMLStreamer::WriteValue<DXGI_DDI_ARG_QUERYRESOURCERESIDENCY>( const char* szAttribute, const DXGI_DDI_ARG_QUERYRESOURCERESIDENCY& val );
template <> void XMLStreamer::WriteValue<DXGI_DDI_ARG_RESOLVESHAREDRESOURCE>( const char* szAttribute, const DXGI_DDI_ARG_RESOLVESHAREDRESOURCE& val );

// Advanced
template <> void XMLStreamer::WriteValue<CBaseResource::THistory>( const char* szAttribute, const CBaseResource::THistory& val );
template <> void XMLStreamer::WriteValue<ArchiveFile::TArchiveChunk>( const char* szAttribute, const ArchiveFile::TArchiveChunk& val );

//////////////////////////////////////////////////////////////////////////

#endif //__S3D_XMLSTREAMER_WRITEVALUE_H__