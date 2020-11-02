#pragma once
#include "outputmethod_dx10.h"

namespace DX10Output{

class MonoOutput :
	public OutputMethod
{
public:
	MonoOutput(DWORD mode, DWORD spanMode);
	virtual ~MonoOutput(void);

	virtual void Output( D3DSwapChain* pSwapChain);
	virtual bool RenderDirectlyToBB() { return true; } // IsRAW
};

}