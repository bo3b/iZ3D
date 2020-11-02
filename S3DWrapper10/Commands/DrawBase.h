#pragma once

#include "Command.h"

namespace Commands
{
	class DrawBase : public Command
	{
	public:
		mutable std::vector<D3D10DDI_HRESOURCE> ResourcesSrc; 
		mutable std::vector< std::vector<int> > databin_poses;

		
#ifndef FINAL_RELEASE
		DrawBase() : m_bStereoDraw(false) {}

		bool			m_bStereoDraw;
#endif

		void BuildCommand(CDumpState *ds);

		void OnRead();
		void OnWrite(D3DDeviceWrapper *pWrapper) const;
	};
}
