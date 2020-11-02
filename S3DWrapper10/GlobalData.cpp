// S3DAPI.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "GlobalData.h"
#include "..\S3DAPI\ShaderProfileData.h"

GlobalData gData;

// special for debugging, because external variables not work in watch window
GlobalInfo& gInfo = _gInfo;

ProfileData& g_ProfileData = _g_ProfileData;
