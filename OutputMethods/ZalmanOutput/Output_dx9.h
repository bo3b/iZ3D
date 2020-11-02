/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "PostprocessedOutput_dx9.h"

namespace DX9Output
{

class ZalmanOutput :
	public PostprocessedOutput
{
public:
	ZalmanOutput(DWORD mode, DWORD spanMode);
	~ZalmanOutput(void);

	virtual HRESULT InitializeResources();
	virtual HRESULT SetStates(CBaseSwapChain* pSwapChain);
	virtual bool FindAdapter( LPDIRECT3D9 pDirect3D9, UINT DeviceNum, D3DDEVTYPE &DeviceType, UINT Adapters[2] );
};

}
