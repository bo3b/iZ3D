/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "S3DAPI.h"
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <hash_map>
#include <vector>
#include <d3dx9math.h>
#include <list>
#include <string>

#define SHADER_MATRIX_SIZE 2

struct ShaderMatrixData
{
	DWORD					constantBuffer;
	DWORD                   matrixRegister;             // shader matrix register index
	BOOL					matrixIsTransposed;
	BOOL					incorrectProjection;
	BOOL					inverse;
};

struct ShaderMatrices
{
	BYTE					matrixSize;
	ShaderMatrixData		matrix[SHADER_MATRIX_SIZE];
};

class ResourceProfileData
{
public:
	ResourceProfileData() : m_Multiplier(1.0f), m_ConvergenceShift(0.0f) {}
	float	m_Multiplier;
	float	m_ConvergenceShift;
};

#ifdef _DEBUG
typedef stdext::hash_map<DWORD, ResourceProfileData>			ResourceCRCMultiplierMap;
typedef stdext::hash_map<DWORD, ResourceCRCMultiplierMap>		ResourcesCRCMultiplierMapBySize;
#else
typedef boost::unordered_map<DWORD, ResourceProfileData>		ResourceCRCMultiplierMap;
typedef boost::unordered_map<DWORD, ResourceCRCMultiplierMap>	ResourcesCRCMultiplierMapBySize;
#endif
typedef std::pair<DWORD, ResourceProfileData>					ResourceCRCMultiplier;
typedef std::pair<DWORD, ResourceCRCMultiplierMap>				ResourcesCRCMultiplierMapWithSize;

class ShaderProfileData : public ResourceProfileData
{
public:
	ShaderProfileData() : 
		ResourceProfileData(), 
		m_bModifyShader(false), 
		m_bAddZNearCheck(false), 
		m_ZNear(1.0f), 
		m_pMatrices(NULL),
		m_PreTransformProjectionId(-1)
	{}

	~ShaderProfileData()
	{	
		delete m_pMatrices;
	}

	bool				m_bModifyShader;
	bool				m_bAddZNearCheck;
	float				m_ZNear;
	ShaderMatrices*		m_pMatrices;
	int					m_PreTransformProjectionId;	// id of the matrix for shader modification in particular frame (for Bioshock2 shadows), -1 - not used
	float				m_ProjTransform[16];
};

typedef std::pair<DWORD, ShaderProfileData>				ShaderProfileDataWithCRC;
typedef boost::unordered_map<DWORD, ShaderProfileData>	ShaderProfileDataMap;

class ProfileData
{
public:
	ResourcesCRCMultiplierMapBySize	MeshCRCMultiplier;
	ResourcesCRCMultiplierMapBySize	TextureCRCMultiplier;
	ResourceProfileData				WeaponMultiplier;
	float							WeaponMaxZ;
	boost::unordered_set<float>		MonoRHWValues;
	ShaderProfileDataMap			VSCRCData;
	ShaderProfileDataMap			GSCRCData;
	ShaderProfileDataMap			PSCRCData;
	std::list<std::string>			AdditionalMatrixName;

	void clear();
};

extern S3DAPI_API ProfileData	_g_ProfileData;

extern ProfileData& g_ProfileData;
