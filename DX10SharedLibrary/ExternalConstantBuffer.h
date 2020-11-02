#pragma once
#include "ResourceWrapper.h"

class ShaderWrapper;

namespace iz3d
{
	class ExternalConstantBuffer
	{
	public:
		typedef std::vector<float> TFloatData;
		typedef std::vector<int> TIntData;

		ExternalConstantBuffer( D3DDeviceWrapper* pDevice, UINT numConstants );
		~ExternalConstantBuffer();

		void initMonoData( ShaderWrapper* pShaderWrapper );
		void setConstantsF( TFloatData& data );
		void setConstantsI( TIntData& data );

		Commands::TCmdPtr setConstantsAsyncF( const void* data, UINT size );
		Commands::TCmdPtr setConstantsAsyncI( const void* data, UINT size );

		D3D10DDI_HRESOURCE getHResource() const;
	private:
		UINT				m_numConst;
		D3D10DDI_HRESOURCE	m_hCBView;
		D3DDeviceWrapper*	m_pDevice;
		ResourceWrapper*	m_pResourceWrapper;

	public:
		void*				m_Data;
	};
}