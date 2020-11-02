/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "S3DAPI.h"
#include <tinyxml.h>
#include <vector>
#include <string>

struct DataInput;

#define PROFILES_VERSION 1
bool ReadConfigRouterType();
void ReadProfilesRouterType();
void FreeProfiles();

void S3DAPI_API ReadCurrentProfile( DWORD Vendor );
void S3DAPI_API WriteInputData(DataInput* overrideInput);
extern S3DAPI_API TiXmlNode* g_outputConfig;

bool S3DAPI_API GetConfigFullPath(TCHAR path[MAX_PATH]);

enum LOCALIZATION_TEXT_ID
{
	LT_CountingFPS = 0,
	LT_Mono,
	LT_Stereo,
	LT_drop,
	LT_Preset,
	LT_Convergence,
	LT_ConvergenceINF,
	LT_Separation,
	LT_AutoFocus,
	LT_ON,
	LT_OFF,
	LT_tooMuchSeparation,
	LT_SwapLR,
	LT_DaysLeft,
	LT_CantFindStereoDevice,
	LT_CantLoadOutputDLL,
	LT_UpdateAMDDriver,
	LT_NotSupported3DDevice,

	LT_TotalStrings //--- just string counter ---
};

class LocalizationData
{
public:
	LocalizationData();
	void ReadLocalizationText();
	const std::wstring& GetText(LOCALIZATION_TEXT_ID id)
	{
		const std::wstring& text = Text[id];
		return text;
	}

private:
	std::vector<std::wstring> Text;
};

extern S3DAPI_API LocalizationData g_LocalData;
