/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "OutputMethod_dx9.h"

namespace DX9Output
{

class S3D120HzProjectorsOutput :
	public OutputMethod
{
public:
	S3D120HzProjectorsOutput(DWORD mode, DWORD spanMode);
	virtual ~S3D120HzProjectorsOutput(void);

	virtual HRESULT	Output(bool bLeft, CBaseSwapChain* pSwapChain, RECT* pDestRect);
	virtual bool	FindAdapter( LPDIRECT3D9 pDirect3D9, UINT DeviceNum, D3DDEVTYPE &DeviceType, UINT Adapters[2] );
	virtual int   GetPreferredRefreshRate() const { return 120; }
};

}
