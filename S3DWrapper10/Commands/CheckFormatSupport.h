#pragma once

#include "Command.h"
#include <memory.h>

namespace Commands
{

	class CheckFormatSupport : public CommandWithAllocator<CheckFormatSupport>
	{
	public:
		EMPTY_OUTPUT();

		CheckFormatSupport()
		{
			CommandId	  = idCheckFormatSupport;
		}

		CheckFormatSupport(DXGI_FORMAT  Format, unsigned int*  pFormatCaps)
		{
			CommandId	  = idCheckFormatSupport;
			Format_       = Format;
			pFormatCaps_  = pFormatCaps;
		}

		virtual void Execute		( D3DDeviceWrapper *pWrapper );
		virtual bool WriteToFile	( D3DDeviceWrapper *pWrapper ) const;
		virtual bool ReadFromFile	();

	public:

		DXGI_FORMAT  Format_;
		unsigned int *pFormatCaps_;
	};

}

VOID (APIENTRY  CheckFormatSupport)(D3D10DDI_HDEVICE  hDevice, DXGI_FORMAT  Format, UINT*  pFormatCaps);
