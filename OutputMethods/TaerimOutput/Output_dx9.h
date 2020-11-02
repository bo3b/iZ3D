/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "PostprocessedOutput_dx9.h"

namespace DX9Output
{

class TaerimOutput :
	public PostprocessedOutput
{
public:
	TaerimOutput(DWORD mode, DWORD spanMode);
	~TaerimOutput(void);

	virtual HRESULT InitializeResources();
	virtual HRESULT SetStates(CBaseSwapChain* pSwapChain);
};

}
