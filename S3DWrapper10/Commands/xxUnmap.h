#pragma once

#include "Command.h"

namespace Commands
{
	class xxUnmap : public Command
	{
	public:
		xxUnmap();

	public:
		D3D10DDI_HRESOURCE	hResource_;
		UINT				Subresource_;
	};
}
