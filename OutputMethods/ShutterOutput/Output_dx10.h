/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "OutputMethod_dx10.h"

namespace DX10Output
{

class ShutterOutput :
	public OutputMethod
{
public:
	ShutterOutput(DWORD mode, DWORD spanMode);
	virtual ~ShutterOutput(void);

	virtual void	Output( bool bLeft_, D3DSwapChain* pSwapChain_, RECT* pDestRect_ );		
};

}
