/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "PostprocessedOutput_dx9.h"

namespace DX9Output
{

class AvitridOutput :
	public PostprocessedOutput
{
public:
	AvitridOutput(DWORD mode, DWORD spanMode);
	~AvitridOutput(void);

	virtual HRESULT InitializeResources();
	virtual HRESULT SetStates(CBaseSwapChain* pSwapChain);
	virtual bool FindAdapter( LPDIRECT3D9 pDirect3D9, UINT DeviceNum, D3DDEVTYPE &DeviceType, UINT Adapters[2] );
};

}
