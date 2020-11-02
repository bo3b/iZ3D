#pragma once

#include "Command.h"

namespace Commands
{

	class SetDestResourceType : public CommandWithAllocator<SetDestResourceType>
	{
	public:
		virtual void Execute(D3DDeviceWrapper *pWrapper);
		virtual	bool WriteToFile(D3DDeviceWrapper *pWrapper) const;
		virtual bool ReadFromFile();

		SetDestResourceType ()		
		{
			CommandId = idSetDestResourceType;
			m_type = TT_CLEARED;
		}

		SetDestResourceType(const TCmdPtr& cmd,TextureType type)
		{
			CommandId = idSetDestResourceType;
			m_cmd = cmd;
			m_type = type;
		}

	public:
		TCmdPtr		m_cmd;
		TextureType m_type;
	};

}
