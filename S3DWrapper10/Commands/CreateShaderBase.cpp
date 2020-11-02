#include "stdafx.h"
#include "CreateShaderBase.h"

namespace Commands
{
	void CreateShaderBase::LookForFullCode(const void *pCode)
	{
		// look for full shader code
		const DWORD sig = 0x43425844;

		DWORD *c = (DWORD *)pCode;
		DWORD *p = c;
		for(int i = 0;;i++)
		{
			if (i > 10000)	
			{ 
				// give up
				p = c; 

				__debugbreak();

				break; 
			}
			p--;
			if (*p == sig) break;
		}

		fullSize = p == c ? c[1] * 4 : *(p + 6);
		fullCode = (char *)p;	
	}
}