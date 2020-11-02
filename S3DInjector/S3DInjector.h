#pragma once

class  IngectorSettings
{
public:
	IngectorSettings()
	{
		m_RenewHookMode = 0;
		m_RouterType = 0;
	}

	DWORD m_RenewHookMode;
	DWORD m_RouterType;
};
