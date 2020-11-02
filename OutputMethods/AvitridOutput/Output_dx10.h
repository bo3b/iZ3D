#pragma once

#include "PostprocessedOutput_dx10.h"

namespace DX10Output
{

	class AvitridOutput :
		public PostprocessedOutput
	{
	public:

		AvitridOutput( DWORD nMode_, DWORD nSpanMode_ );
		~AvitridOutput();

		virtual void						InitializeResources( D3DDeviceWrapper* pD3DWrapper_ );
		virtual void						ClearResources( D3DDeviceWrapper* pD3DWrapper_ );
		virtual void						SetStates(  D3DDeviceWrapper* pDevice_, D3DSwapChain* pSwapChain_ );
		virtual bool						FindAdapter( IDXGIFactory* pDXGIFactory_ );

	protected:
	private:

		D3D10DDI_HSAMPLER					m_hSamplerState;
		D3D10DDI_HRESOURCE					m_hCB;
		static const UINT					m_FloatCount = 8;
		float								m_pCB[m_FloatCount];

		D3D10DDIARG_STAGE_IO_SIGNATURES		m_SigPS;
		D3D10DDIARG_SIGNATURE_ENTRY			m_SigEntryPS[ 2 ];
		D3D10DDIARG_STAGE_IO_SIGNATURES		m_SigVS;
		D3D10DDIARG_SIGNATURE_ENTRY			m_SigEntryVS[ 3 ];
	};
}