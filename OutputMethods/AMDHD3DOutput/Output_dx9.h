/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "OutputMethod_dx9.h"

namespace DX9Output
{

	class AMDHD3DOutput :
		public OutputMethod
	{
	public:
		AMDHD3DOutput(DWORD mode, DWORD spanMode);
		virtual ~AMDHD3DOutput(void);

		virtual HRESULT Output(bool bLeft, CBaseSwapChain* pSwapChain, RECT* pDestRect);
		virtual int GetPreferredRefreshRate() const;
	};

}
