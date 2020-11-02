/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "PostprocessedOutput_dx9.h"

namespace DX9Output
{

class LenovoOutput :
	public PostprocessedOutput
{
public:
	LenovoOutput(DWORD mode, DWORD spanMode);
	~LenovoOutput(void);

	virtual HRESULT InitializeResources();
};

}
