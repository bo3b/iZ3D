#pragma once

#include "Command.h"

namespace Commands
{

	class ChangeDeviceView : public CommandWithAllocator<ChangeDeviceView>
	{
	public:
		ChangeDeviceView ()		
		{
			CommandId		= idChangeDeviceView;
		}

		ChangeDeviceView( VIEWINDEX newView )
		{
			CommandId		= idChangeDeviceView;
			NewView_ = newView;
		}

		virtual void Execute		( D3DDeviceWrapper *pWrapper );
		virtual	bool WriteToFile	( D3DDeviceWrapper *pWrapper ) const;
		virtual bool ReadFromFile	();
		virtual void Dump(std::ostream& os) const;

	public:

		VIEWINDEX  NewView_;
	};

}
