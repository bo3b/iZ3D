/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "OutputMethod_dx9.h"

namespace DX9Output
{

class ShutterOutput :
	public OutputMethod
{
public:
	ShutterOutput(DWORD mode, DWORD spanMode);
	virtual ~ShutterOutput(void);

	virtual HRESULT Output(bool bLeft, CBaseSwapChain* pSwapChain, RECT* pDestRect);
};

}
