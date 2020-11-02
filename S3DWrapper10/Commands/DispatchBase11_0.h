#pragma once

#include "Command.h"

namespace Commands
{

	class DispatchBase11_0 : public Command
	{
	public:

#ifndef FINAL_RELEASE
		DispatchBase11_0() : m_bStereoDispatch(false) {}

		bool			m_bStereoDispatch;
#endif
	};

}
