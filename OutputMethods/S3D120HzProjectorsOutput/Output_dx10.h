/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "OutputMethod_dx10.h"

namespace DX10Output
{

class S3D120HzProjectorsOutput :
	public OutputMethod
{
public:
	S3D120HzProjectorsOutput(DWORD mode, DWORD spanMode);
	virtual ~S3D120HzProjectorsOutput(void);

	virtual void	Output					( bool bLeft_, D3DSwapChain* pSwapChain_, RECT* pDestRect_ );
	virtual bool	FindAdapter				( IDXGIFactory* pDXGIFactory_ );
	virtual int		GetPreferredRefreshRate() const { return 120; }
};

}
