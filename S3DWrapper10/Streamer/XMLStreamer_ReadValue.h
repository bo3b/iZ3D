#ifndef __S3D_XMLSTREAMER_READVALUE_H__
#define __S3D_XMLSTREAMER_READVALUE_H__

template < class T >
inline void XMLStreamer::FromString( const std::string& temp, T* out ) const
{
	std::istringstream val( temp );
	val >> *out;

	//if ( val.fail() )
	//	TICPPTHROW( "Could not convert \"" << temp << "\" to target type" );
}

inline void XMLStreamer::FromString( const std::string& temp, std::string* out ) const
{
	*out = temp;
}

template <typename T>
void XMLStreamer::ReadValueByRef( T*& pVal )
{
	m_pXMLReader->IsStartElement();
	if (!m_pXMLReader->GetBoolAttribute("isNull",FALSE) &&
		!m_pXMLReader->HasAttribute("ref"))
	{
		InternalAllocateMem( pVal );
		ReadValue( *pVal);
	}
	else
	{
		pVal = 0;
		m_pXMLReader->Skip();
	}
}

template <typename T>
void XMLStreamer::ReadValueByRef( const T*& val )
{
	ReadValueByRef( (T*&)val );
}

template <typename T> void XMLStreamer::ReadReference( T& _val )
{
	m_pXMLReader->ReadStartElement();
	_val.pDrvPrivate = (void *)(UINT_PTR)m_pXMLReader->GetAttributeValue( "val", (UINT_PTR)0 );

	/*m_pXMLReader->ReadStartElement();
	//int nRef = m_pXMLReader->GetIntAttribute( "ref" );
	UINT_PTR val = m_pXMLReader->GetIntAttribute( "val" );
	_val.pDrvPrivate = (void *)val;
	/*if (nRef >= 0)
	{
		_ASSERT( m_pCurrentRM );
		bool bFind = m_pCurrentRM->FindResourceByID(nRef, _val );
		//_ASSERT( bFind );
	}*/
}

template<typename T> void XMLStreamer::ReadArrayValues_( SIZE_T _nCount, T  _pArray[] )
{
	m_pXMLReader->ReadStartElement();
	_nCount = m_pXMLReader->GetIntAttribute( "count" );

	if ( !_nCount )
		return;

	for ( SIZE_T i = 0; i < _nCount; i++ )
		ReadValue( _pArray[i] );
	m_pXMLReader->ReadEndElement();
}

template<typename T> void XMLStreamer::ReadArrayValues( SIZE_T _nCount, T  *&_pArray )
{
	m_pXMLReader->ReadStartElement();
	_nCount = m_pXMLReader->GetIntAttribute( "count" );

	if ( !_nCount )
		return;

	if ( !_pArray )
		InternalAllocateMem( _pArray, _nCount );

	for ( SIZE_T i = 0; i < _nCount; i++ )
		ReadValue( _pArray[i] );
	m_pXMLReader->ReadEndElement();
}

template<typename T, typename A> void XMLStreamer::ReadArrayValues( SIZE_T _nCount, std::vector<T, A>  &_pArray )
{	
	_pArray.resize(_nCount);
	T *pArray = GetPointerToVector(_pArray);
	BOOL bRecreate = !pArray;
	ReadArrayValues( _nCount, pArray );
	if (bRecreate) _pArray.swap(std::vector<T, A>(pArray,pArray + _nCount));
}

template<typename T, typename A, typename Tr> void XMLStreamer::ReadArrayValues( SIZE_T _nCount, array<T, A, Tr>  &_pArray )
{	
	_pArray.resize(_nCount);
	ReadArrayValues_( _nCount, GetPointerToVector(_pArray) );
}

template<typename T> void XMLStreamer::ReadArrayReferences	( SIZE_T _nCount, T*  _pArray )
{
	m_pXMLReader->ReadStartElement();
	_nCount = m_pXMLReader->GetIntAttribute( "count" );

	if ( !_nCount )
		return;

	if ( !_pArray )
		InternalAllocateMem( _pArray, _nCount );

	for ( size_t i = 0; i < _nCount; i++ )
		ReadReference( _pArray[i] );
	m_pXMLReader->ReadEndElement();
}

template<typename T, typename A> void XMLStreamer::ReadArrayReferences	( SIZE_T _nCount, std::vector<T, A>  &_pArray )
{	
	_pArray.resize(_nCount);
	ReadArrayReferences( _nCount, GetPointerToVector(_pArray) );
}

template<typename T, typename A, typename Tr> void XMLStreamer::ReadArrayReferences	( SIZE_T _nCount, array<T, A, Tr>  &_pArray )
{	
	_pArray.resize(_nCount);
	ReadArrayReferences( _nCount, GetPointerToVector(_pArray) );
}

template <typename T> void XMLStreamer::ReadValue( T& _val )
{
	m_pXMLReader->ReadStartElement();
	_val = m_pXMLReader->GetAttributeValue( "val", (T)0 );
}

template <> void XMLStreamer::ReadValue<UINT8>( UINT8& _val );
template <> void XMLStreamer::ReadValue<SHADER_PIPELINE>( SHADER_PIPELINE& _val );
template <> void XMLStreamer::ReadValue<VIEWINDEX>( VIEWINDEX& _val );
template <> void XMLStreamer::ReadValue<D3D10DDIRESOURCE_TYPE>( D3D10DDIRESOURCE_TYPE& _val );

template <> void XMLStreamer::ReadValue<DXGI_FORMAT>( DXGI_FORMAT& _val );
template <> void XMLStreamer::ReadValue<DXGI_GAMMA_CONTROL_CAPABILITIES>( DXGI_GAMMA_CONTROL_CAPABILITIES& _val );
template <> void XMLStreamer::ReadValue<DXGI_SAMPLE_DESC>( DXGI_SAMPLE_DESC& _val );
template <> void XMLStreamer::ReadValue<DXGI_DDI_PRIMARY_DESC>( DXGI_DDI_PRIMARY_DESC& _val );
template <> void XMLStreamer::ReadValue<DXGI_DDI_MODE_DESC>( DXGI_DDI_MODE_DESC& _val );
template <> void XMLStreamer::ReadValue<DXGI_DDI_RATIONAL>( DXGI_DDI_RATIONAL& _val );
template <> void XMLStreamer::ReadValue<DXGI_DDI_MODE_SCANLINE_ORDER>( DXGI_DDI_MODE_SCANLINE_ORDER& _val );
template <> void XMLStreamer::ReadValue<DXGI_DDI_MODE_ROTATION>( DXGI_DDI_MODE_ROTATION& _val );
template <> void XMLStreamer::ReadValue<DXGI_DDI_MODE_SCALING>( DXGI_DDI_MODE_SCALING& _val );
template <> void XMLStreamer::ReadValue<DXGI_DDI_ARG_PRESENT>( DXGI_DDI_ARG_PRESENT& _val );
template <> void XMLStreamer::ReadValue<DXGI_DDI_ARG_BLT>( DXGI_DDI_ARG_BLT& _val );
template <> void XMLStreamer::ReadValue<DXGI_DDI_FLIP_INTERVAL_TYPE>( DXGI_DDI_FLIP_INTERVAL_TYPE& _val );
template <> void XMLStreamer::ReadValue<DXGI_DDI_ARG_SETRESOURCEPRIORITY>( DXGI_DDI_ARG_SETRESOURCEPRIORITY & _val );
template <> void XMLStreamer::ReadValue<DXGI_DDI_ARG_SETDISPLAYMODE>( DXGI_DDI_ARG_SETDISPLAYMODE& _val );
template <> void XMLStreamer::ReadValue<DXGI_DDI_ARG_GET_GAMMA_CONTROL_CAPS>( DXGI_DDI_ARG_GET_GAMMA_CONTROL_CAPS& _val );
template <> void XMLStreamer::ReadValue<DXGI_DDI_ARG_ROTATE_RESOURCE_IDENTITIES>( DXGI_DDI_ARG_ROTATE_RESOURCE_IDENTITIES& _val );
template <> void XMLStreamer::ReadValue<DXGI_DDI_ARG_QUERYRESOURCERESIDENCY>( DXGI_DDI_ARG_QUERYRESOURCERESIDENCY& _val );
template <> void XMLStreamer::ReadValue<DXGI_DDI_ARG_RESOLVESHAREDRESOURCE>( DXGI_DDI_ARG_RESOLVESHAREDRESOURCE& _val );
template <> void XMLStreamer::ReadValue<DXGI_DDI_RESIDENCY>( DXGI_DDI_RESIDENCY& _val );

template <> void XMLStreamer::ReadValue<D3D10DDI_COUNTER_INFO>( D3D10DDI_COUNTER_INFO& _val );
template <> void XMLStreamer::ReadValue<D3D10DDI_QUERY>( D3D10DDI_QUERY& _val );
template <> void XMLStreamer::ReadValue<D3D10DDI_MIPINFO>( D3D10DDI_MIPINFO& _val );
template <> void XMLStreamer::ReadValue<D3D10DDI_MAPPED_SUBRESOURCE>( D3D10DDI_MAPPED_SUBRESOURCE& _val );

template <> void XMLStreamer::ReadValue<D3D10_DDI_BLEND_DESC>( D3D10_DDI_BLEND_DESC& _val );
template <> void XMLStreamer::ReadValue<D3D10_DDI_BLEND>( D3D10_DDI_BLEND& _val );
template <> void XMLStreamer::ReadValue<D3D10_DDI_BLEND_OP>( D3D10_DDI_BLEND_OP& _val );
template <> void XMLStreamer::ReadValue<D3D10_DDI_DEPTH_STENCIL_DESC>( D3D10_DDI_DEPTH_STENCIL_DESC& _val );
template <> void XMLStreamer::ReadValue<D3D10_DDI_DEPTH_WRITE_MASK>( D3D10_DDI_DEPTH_WRITE_MASK& _val );
template <> void XMLStreamer::ReadValue<D3D10_DDI_COMPARISON_FUNC>( D3D10_DDI_COMPARISON_FUNC& _val );
template <> void XMLStreamer::ReadValue<D3D10_DDI_DEPTH_STENCILOP_DESC>( D3D10_DDI_DEPTH_STENCILOP_DESC& _val );
template <> void XMLStreamer::ReadValue<D3D10_DDI_PRIMITIVE_TOPOLOGY>( D3D10_DDI_PRIMITIVE_TOPOLOGY& _val );
template <> void XMLStreamer::ReadValue<D3D10_DDI_STENCIL_OP>( D3D10_DDI_STENCIL_OP& _val );
template <> void XMLStreamer::ReadValue<D3D10_DDI_RASTERIZER_DESC>( D3D10_DDI_RASTERIZER_DESC& _val );
template <> void XMLStreamer::ReadValue<D3D10_DDI_FILL_MODE>( D3D10_DDI_FILL_MODE& _val );
template <> void XMLStreamer::ReadValue<D3D10_DDI_CULL_MODE>( D3D10_DDI_CULL_MODE& _val );
template <> void XMLStreamer::ReadValue<D3D10_DDI_SAMPLER_DESC>( D3D10_DDI_SAMPLER_DESC& _val );
template <> void XMLStreamer::ReadValue<D3D10_DDI_FILTER>( D3D10_DDI_FILTER& _val );
template <> void XMLStreamer::ReadValue<D3D10_DDI_TEXTURE_ADDRESS_MODE>( D3D10_DDI_TEXTURE_ADDRESS_MODE& _val );
template <> void XMLStreamer::ReadValue<D3D10_DDI_RECT>( D3D10_DDI_RECT& _val );
template <> void XMLStreamer::ReadValue<D3D10_DDI_VIEWPORT>( D3D10_DDI_VIEWPORT& _val );
template <> void XMLStreamer::ReadValue<D3D10_DDI_INPUT_CLASSIFICATION>( D3D10_DDI_INPUT_CLASSIFICATION& _val );
template <> void XMLStreamer::ReadValue<D3D10_1_DDI_BLEND_DESC>( D3D10_1_DDI_BLEND_DESC& _val );
template <> void XMLStreamer::ReadValue<D3D10_DDI_RENDER_TARGET_BLEND_DESC1>( D3D10_DDI_RENDER_TARGET_BLEND_DESC1& _val );
template <> void XMLStreamer::ReadValue<D3D10_DDI_BOX>( D3D10_DDI_BOX& _val );
template <> void XMLStreamer::ReadValue<D3D10_DDI_MAP>( D3D10_DDI_MAP& _val );
template <> void XMLStreamer::ReadValue<D3D11DDI_HANDLETYPE>( D3D11DDI_HANDLETYPE& _val );

template <> void XMLStreamer::ReadValue<D3D10DDIARG_CREATERESOURCE>( D3D10DDIARG_CREATERESOURCE& _val );
template <> void XMLStreamer::ReadValue<D3D11DDIARG_CREATERESOURCE>( D3D11DDIARG_CREATERESOURCE& _val );
template <> void XMLStreamer::ReadValue<D3D10DDIARG_CREATEQUERY>( D3D10DDIARG_CREATEQUERY& _val );
template <> void XMLStreamer::ReadValue<D3D10DDIARG_CREATERENDERTARGETVIEW>( D3D10DDIARG_CREATERENDERTARGETVIEW& _val );
template <> void XMLStreamer::ReadValue<D3D11DDIARG_CREATEUNORDEREDACCESSVIEW>( D3D11DDIARG_CREATEUNORDEREDACCESSVIEW& _val );
template <> void XMLStreamer::ReadValue<D3D11DDIARG_BUFFER_UNORDEREDACCESSVIEW>( D3D11DDIARG_BUFFER_UNORDEREDACCESSVIEW& _val );
template <> void XMLStreamer::ReadValue<D3D11DDIARG_TEX1D_UNORDEREDACCESSVIEW>( D3D11DDIARG_TEX1D_UNORDEREDACCESSVIEW& _val );
template <> void XMLStreamer::ReadValue<D3D11DDIARG_TEX2D_UNORDEREDACCESSVIEW>( D3D11DDIARG_TEX2D_UNORDEREDACCESSVIEW& _val );
template <> void XMLStreamer::ReadValue<D3D11DDIARG_TEX3D_UNORDEREDACCESSVIEW>( D3D11DDIARG_TEX3D_UNORDEREDACCESSVIEW& _val );
template <> void XMLStreamer::ReadValue<D3D10DDIARG_BUFFER_RENDERTARGETVIEW>( D3D10DDIARG_BUFFER_RENDERTARGETVIEW& _val );
template <> void XMLStreamer::ReadValue<D3D10DDIARG_TEX1D_RENDERTARGETVIEW>( D3D10DDIARG_TEX1D_RENDERTARGETVIEW& _val );
template <> void XMLStreamer::ReadValue<D3D10DDIARG_TEX2D_RENDERTARGETVIEW>( D3D10DDIARG_TEX2D_RENDERTARGETVIEW& _val );
template <> void XMLStreamer::ReadValue<D3D10DDIARG_TEX3D_RENDERTARGETVIEW>( D3D10DDIARG_TEX3D_RENDERTARGETVIEW& _val );
template <> void XMLStreamer::ReadValue<D3D10DDIARG_TEXCUBE_RENDERTARGETVIEW>( D3D10DDIARG_TEXCUBE_RENDERTARGETVIEW& _val );
template <> void XMLStreamer::ReadValue<D3D10DDIARG_STAGE_IO_SIGNATURES>( D3D10DDIARG_STAGE_IO_SIGNATURES& _val );
template <> void XMLStreamer::ReadValue<D3D10DDIARG_OPENRESOURCE>( D3D10DDIARG_OPENRESOURCE& _val );
template <> void XMLStreamer::ReadValue<D3D10DDIARG_SIGNATURE_ENTRY>( D3D10DDIARG_SIGNATURE_ENTRY& _val );
template <> void XMLStreamer::ReadValue<D3D10DDIARG_CREATEELEMENTLAYOUT>( D3D10DDIARG_CREATEELEMENTLAYOUT& _val );
template <> void XMLStreamer::ReadValue<D3D10DDIARG_INPUT_ELEMENT_DESC>( D3D10DDIARG_INPUT_ELEMENT_DESC& _val );
template <> void XMLStreamer::ReadValue<D3D10DDIARG_CREATESHADERRESOURCEVIEW>( D3D10DDIARG_CREATESHADERRESOURCEVIEW& _val );
template <> void XMLStreamer::ReadValue<D3D11DDIARG_CREATESHADERRESOURCEVIEW>( D3D11DDIARG_CREATESHADERRESOURCEVIEW& _val );
template <> void XMLStreamer::ReadValue<D3D10DDIARG_BUFFER_SHADERRESOURCEVIEW>( D3D10DDIARG_BUFFER_SHADERRESOURCEVIEW& _val );
template <> void XMLStreamer::ReadValue<D3D10DDIARG_TEX1D_SHADERRESOURCEVIEW>( D3D10DDIARG_TEX1D_SHADERRESOURCEVIEW& _val );
template <> void XMLStreamer::ReadValue<D3D10DDIARG_TEX2D_SHADERRESOURCEVIEW>( D3D10DDIARG_TEX2D_SHADERRESOURCEVIEW& _val );
template <> void XMLStreamer::ReadValue<D3D10DDIARG_TEX3D_SHADERRESOURCEVIEW>( D3D10DDIARG_TEX3D_SHADERRESOURCEVIEW& _val );
template <> void XMLStreamer::ReadValue<D3D10DDIARG_TEXCUBE_SHADERRESOURCEVIEW>( D3D10DDIARG_TEXCUBE_SHADERRESOURCEVIEW& _val );
template <> void XMLStreamer::ReadValue<D3D10_1DDIARG_CREATESHADERRESOURCEVIEW>( D3D10_1DDIARG_CREATESHADERRESOURCEVIEW& _val );
template <> void XMLStreamer::ReadValue<D3D10_1DDIARG_TEXCUBE_SHADERRESOURCEVIEW>( D3D10_1DDIARG_TEXCUBE_SHADERRESOURCEVIEW& _val );
template <> void XMLStreamer::ReadValue<D3D10DDIARG_CREATEDEPTHSTENCILVIEW>( D3D10DDIARG_CREATEDEPTHSTENCILVIEW& _val );
template <> void XMLStreamer::ReadValue<D3D11DDIARG_CREATEDEPTHSTENCILVIEW>( D3D11DDIARG_CREATEDEPTHSTENCILVIEW& _val );
template <> void XMLStreamer::ReadValue<D3D10DDIARG_TEX1D_DEPTHSTENCILVIEW>( D3D10DDIARG_TEX1D_DEPTHSTENCILVIEW& _val );
template <> void XMLStreamer::ReadValue<D3D10DDIARG_TEX2D_DEPTHSTENCILVIEW>( D3D10DDIARG_TEX2D_DEPTHSTENCILVIEW& _val );
template <> void XMLStreamer::ReadValue<D3D10DDIARG_TEXCUBE_DEPTHSTENCILVIEW>( D3D10DDIARG_TEXCUBE_DEPTHSTENCILVIEW& _val );
template <> void XMLStreamer::ReadValue<D3D10DDIARG_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT>( D3D10DDIARG_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT& _val );
template <> void XMLStreamer::ReadValue<D3D10DDIARG_STREAM_OUTPUT_DECLARATION_ENTRY>( D3D10DDIARG_STREAM_OUTPUT_DECLARATION_ENTRY& _val );

template <> void XMLStreamer::ReadValue<D3D10_DDIARG_SUBRESOURCE_UP>( D3D10_DDIARG_SUBRESOURCE_UP& _val );
template <> void XMLStreamer::ReadValue<D3D10_SB_NAME>( D3D10_SB_NAME& _val );



#endif//__S3D_XMLSTREAMER_READVALUE_H__