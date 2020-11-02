/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once
#include "outputmethod_dx9.h"

namespace DX9Output{

class MonoOutput :
	public OutputMethod
{
public:
	MonoOutput(DWORD mode, DWORD spanMode);
	virtual ~MonoOutput(void);

	virtual HRESULT Output(CBaseSwapChain* pSwapChain);
};

}