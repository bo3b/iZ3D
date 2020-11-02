#pragma once

#include "Command.h"

namespace Commands
{
	class CreateShaderBase : public Command
	{
	public:
		void LookForFullCode(const void *pCode);
		DWORD GetFullCodeSize() { return fullSize; }

	protected:
		char *fullCode;
		DWORD fullSize;
		int databin_pos;
	};
}
