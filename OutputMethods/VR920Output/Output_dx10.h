#pragma once

#include "OutputMethod_dx10.h"

namespace DX10Output
{

	class VR920Output :
		public OutputMethod
	{
	public:

		VR920Output( DWORD nMode_, DWORD nSpanMode_ );
		~VR920Output();

		virtual void		Initialize				( D3DDeviceWrapper* pD3DWrapper_ );
		virtual void		Clear					( D3DDeviceWrapper* pD3DWrapper_ );
		virtual void		StereoModeChanged		( bool bNewMode_ );
		virtual void		Output					( bool bLeft_, D3DSwapChain* pSwapChain_, RECT* pDestRect_ );		
		virtual void		AfterPresent			( D3DDeviceWrapper* pD3DDevice_, bool bLeft_ );
		virtual bool		FindAdapter				( IDXGIFactory* pDXGIFactory_ );

	protected:
	private:
		D3D10DDI_HQUERY		m_hLeftEyeQuery;
		D3D10DDI_HQUERY		m_hRightEyeQuery;

		bool				m_bLastFrameStereoOn;
		bool				m_bAdapterFound;
		HANDLE				m_StereoHandle;

	};
}