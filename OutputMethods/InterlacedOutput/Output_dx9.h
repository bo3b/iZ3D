/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "PostprocessedOutput_dx9.h"

namespace DX9Output
{

class InterlacedOutput :
	public PostprocessedOutput
{
public:
	InterlacedOutput(DWORD mode, DWORD spanMode);
	~InterlacedOutput(void);

	virtual HRESULT InitializeResources();
	virtual HRESULT SetStates(CBaseSwapChain* pSwapChain);
};

}
