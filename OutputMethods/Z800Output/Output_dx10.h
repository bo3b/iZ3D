#pragma once

#include "PostprocessedOutput_dx10.h"

class EMADevice;

namespace DX10Output
{
	class Z800Output :
		public OutputMethod
	{
	public:

		Z800Output( DWORD nMode_, DWORD nSpanMode_ );
		~Z800Output();

		virtual void		Initialize				( D3DDeviceWrapper* pD3DWrapper_ );
		virtual void		Clear					( D3DDeviceWrapper* pD3DWrapper_ );
		virtual void		StereoModeChanged		( bool bNewMode_ );
		virtual void		Output					( bool bLeft_, D3DSwapChain* pSwapChain_, RECT* pDestRect_ );
		virtual bool		FindAdapter				( IDXGIFactory* pDXGIFactory_ );

	protected:
	private:

		bool				m_bLastFrameStereoOn;
		bool				m_bAdapterFound;
		DWORD				m_dwFrameCount;
		EMADevice*			m_pZ800;
	};
}