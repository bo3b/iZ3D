#pragma once

#include "Command.h"
#include <memory.h>

namespace Commands
{

	class CheckMultisampleQualityLevels : public CommandWithAllocator<CheckMultisampleQualityLevels>
	{
	public:

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	ReadFromFile	();

		CheckMultisampleQualityLevels()		
		{
			CommandId		   = idCheckMultisampleQualityLevels;
		}

		CheckMultisampleQualityLevels(DXGI_FORMAT Format, unsigned int SampleCount, unsigned int*  pNumQualityLevels)
		{
			CommandId		   = idCheckMultisampleQualityLevels;
			Format_            = Format;
			SampleCount_       = SampleCount;
			pNumQualityLevels_ = pNumQualityLevels;
		}

	public:

		DXGI_FORMAT     Format_;
		unsigned int    SampleCount_;
		unsigned int*   pNumQualityLevels_;
	};

}

VOID (APIENTRY  CheckMultisampleQualityLevels)(D3D10DDI_HDEVICE  hDevice, DXGI_FORMAT  Format, 
											   UINT  SampleCount, UINT*  pNumQualityLevels);
