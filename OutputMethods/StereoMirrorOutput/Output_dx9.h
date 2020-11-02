/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include <vector>
#include "PostprocessedOutput_dx9.h"

namespace DX9Output
{

class StereoMirrorOutput :
	public PostprocessedOutput
{
public:
	StereoMirrorOutput(DWORD mode, DWORD spanMode);
	virtual ~StereoMirrorOutput(void);

	virtual UINT GetOutputChainsNumber() { return 2; }
	virtual DWORD GetSecondWindowCaps();
	virtual HRESULT InitializeResources();
	virtual HRESULT InitializeSCResources(CBaseSwapChain* pSwapChain);
	virtual HRESULT SetStates(CBaseSwapChain* pSwapChain);

protected:
	bool InvertPrimaryMonitorX();
	bool InvertPrimaryMonitorY();
	bool InvertSecondaryMonitorX();
	bool InvertSecondaryMonitorY();
};

}