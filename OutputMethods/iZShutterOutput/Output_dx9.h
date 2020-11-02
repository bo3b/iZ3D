/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "OutputMethod_dx9.h"

namespace DX9Output
{

	class iZShutterOutput :
		public OutputMethod
	{
	public:
		iZShutterOutput(DWORD mode, DWORD spanMode);
		virtual ~iZShutterOutput(void);

		virtual HRESULT Output(bool bLeft, CBaseSwapChain* pSwapChain, RECT* pDestRect);
		virtual int GetPreferredRefreshRate() const;
		virtual bool FindAdapter( LPDIRECT3D9 pDirect3D9, UINT DeviceNum, D3DDEVTYPE &DeviceType, UINT Adapters[2] );
		HRESULT MarkSurface(int view, IDirect3DSurface9* pView, RECT* pDestRect);
	};

}
