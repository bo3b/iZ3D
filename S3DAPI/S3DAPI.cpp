// S3DAPI.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "S3DAPI.h"
#include "StereoAPI.h"
#include "StereoAPIInternal.h"
#include "GlobalData.h"

HRESULT S3DAPI_API __stdcall InitStereoAPI(IStereoAPI** ppAPI)
{
	return StereoAPI::CreateInstance(ppAPI);
}

HRESULT S3DAPI_API __stdcall InitStereoAPIInternal(IStereoAPIInternal** ppAPI)
{
	return StereoAPIInternal::CreateInstance(ppAPI);
}