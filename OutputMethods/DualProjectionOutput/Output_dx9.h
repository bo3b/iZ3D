/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "OutputMethod_dx9.h"

namespace DX9Output
{

class DualProjectionOutput :
	public OutputMethod
{
public:
	DualProjectionOutput(DWORD mode, DWORD spanMode);
	virtual ~DualProjectionOutput(void);

	virtual HRESULT Output(CBaseSwapChain* pSwapChain);
	virtual UINT GetOutputChainsNumber() { return 2; }
};

}
