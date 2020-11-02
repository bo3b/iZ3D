#pragma once

#include "PostprocessedOutput_dx10.h"

namespace DX10Output
{

	class CMOShutterOutput :
		public OutputMethod
	{
	public:
								CMOShutterOutput		( DWORD nMode_, DWORD nSpanMode_ );
		virtual					~CMOShutterOutput		();

		virtual void			Output					( bool bLeft_, D3DSwapChain* pSwapChain_, RECT* pDestRect_ );\

		virtual bool			FindAdapter				( IDXGIFactory* pDXGIFactory_ );
		virtual void			Initialize				( D3DDeviceWrapper* pD3DWrapper_ );
		virtual void			Clear					( D3DDeviceWrapper* pD3DWrapper_ );
		virtual int				GetPreferredRefreshRate	() const;

		//virtual bool			RenderDirectlyToBB		()	{ return false; }	// by default it is false

	private:
		D3D10DDI_HRESOURCE		m_hMarkedTextureLeft;
		D3D10DDI_HRESOURCE		m_hMarkedTextureRight;
		D3D10DDI_HRESOURCE		m_hMarkedTextureMono;

		//virtual HRESULT Output(bool bLeft, CBaseSwapChain* pSwapChain, RECT* pDestRect);
		//virtual int GetPreferredRefreshRate() const;
		//virtual bool FindAdapter( LPDIRECT3D9 pDirect3D9, UINT DeviceNum, D3DDEVTYPE &DeviceType, UINT Adapters[2] );
		//HRESULT MarkSurface(int view, IDirect3DSurface9* pView, RECT* pDestRect);
	};

}