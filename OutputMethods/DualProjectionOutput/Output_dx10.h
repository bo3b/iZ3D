/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "OutputMethod_dx10.h"

namespace DX10Output
{

class DualProjectionOutput :
	public OutputMethod
{
public:
	DualProjectionOutput(DWORD mode, DWORD spanMode);
	virtual ~DualProjectionOutput(void);

	virtual void Output( D3DSwapChain* pSwapChain );
	virtual UINT GetOutputChainsNumber() { return 2; }
	virtual bool RenderDirectlyToBB() { return true; } // IsRAW
};

}
