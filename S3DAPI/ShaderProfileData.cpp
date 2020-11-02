// S3DAPI.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "ShaderProfileData.h"

ProfileData		_g_ProfileData;

ProfileData&	g_ProfileData = _g_ProfileData;

void ProfileData::clear()
{
	TextureCRCMultiplier.clear();
	MeshCRCMultiplier.clear();
	WeaponMaxZ = 0.5f;
	WeaponMultiplier.m_Multiplier = 1.0f;
	WeaponMultiplier.m_ConvergenceShift = 0.0f;
	VSCRCData.clear();
	PSCRCData.clear();
	AdditionalMatrixName.clear();
	MonoRHWValues.clear();
}
