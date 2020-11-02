#pragma once
#include <vector>

class ExplicitLibaries{
	typedef std::vector<HMODULE> TModules;
	TModules m_modules;
	HMODULE m_hDllModule;
	void GetLibrariesToLoad( const TCHAR* fileName, std::vector< std::basic_string< TCHAR > >& libNames );
public:
	TCHAR bufDirectory[MAX_PATH];
	ExplicitLibaries( HMODULE hModule );
	~ExplicitLibaries();
};

// return false if can't read profile
bool ReadProfile(IngectorSettings* iSettings);