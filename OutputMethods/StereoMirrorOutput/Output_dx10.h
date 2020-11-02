#pragma once

#include "PostprocessedOutput_dx10.h"

namespace DX10Output
{

	class StereoMirrorOutput:
		public PostprocessedOutput
	{
	public:

		StereoMirrorOutput( DWORD nMode_, DWORD nSpanMode_ );
		~StereoMirrorOutput();

		virtual void						InitializeResources( D3DDeviceWrapper* pD3DWrapper_ );
		virtual void						ClearResources( D3DDeviceWrapper* pD3DWrapper_ );
		virtual void						SetStates(  D3DDeviceWrapper* pDevice_, D3DSwapChain* pSwapChain_ );
		virtual UINT						GetOutputChainsNumber() { return 2; };
		virtual DWORD						GetSecondWindowCaps();

	protected:		
	private:
		D3D10DDI_HSAMPLER					m_hSState;
		D3D10DDI_HRESOURCE					m_hCB;
		float								m_pCB[ 4 ];

		D3D10DDIARG_STAGE_IO_SIGNATURES		m_SigPS;
		D3D10DDIARG_SIGNATURE_ENTRY			m_SigEntryPS[ 4 ];
		D3D10DDIARG_STAGE_IO_SIGNATURES		m_SigVS;
		D3D10DDIARG_SIGNATURE_ENTRY			m_SigEntryVS[ 4 ];
	};
}