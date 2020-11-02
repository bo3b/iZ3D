/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "PostprocessedOutput_dx9.h"

namespace DX9Output
{

class DLP3DOutput :
	public PostprocessedOutput
{
public:
	DLP3DOutput(DWORD mode, DWORD spanMode);
	~DLP3DOutput(void);

	virtual HRESULT InitializeResources();
	virtual HRESULT SetStates(CBaseSwapChain* pSwapChain);
};

}
