#pragma once

#include "Singleton.h"
#include "profilevar.h"
#include <list>
#include <string>
namespace iz3d
{
	namespace profile
	{
		class ProfileVarRepository
		{
			friend class iz3d::Singleton<ProfileVarRepository>;
			typedef std::list< iz3d::profile::AProfileVar* > TProfileVarList;
			TProfileVarList varList_;
		public:
			void RegisterProfileVar( iz3d::profile::AProfileVar* pVar );
			void UnregisterProfileVar( iz3d::profile::AProfileVar* pVar );
			void SaveDifferences( const std::wstring& filePath ) const;
			void LoadDifferences( const std::wstring& filePath );
		};
	}

}