#pragma once
#include <wtypes.h>
#include <bitset>
#include <vector>
#include <hash_map>
#include <d3dx9math.h>
#include <boost/unordered_map.hpp>

struct ProjectionMatrixData
{
	DWORD		matrixRegister;				// shader matrix register index   - 2'nd index in asm
	BOOL		matrixIsTransposed;
	BOOL		incorrectProjection;
	INT			preTransformProjectionId;	// id of the matrix for shader modification in particular frame (for Bioshock2 shadows), -1 - if not used
	D3DXMATRIX	projTransform;
	D3DXMATRIX	invProjTransform;

	ProjectionMatrixData()
	:	matrixRegister(0)
	,	matrixIsTransposed(FALSE)
	,	incorrectProjection(FALSE)
	,	preTransformProjectionId(-1)
	{
	}

	bool operator==(const ProjectionMatrixData& Right) const
	{
		return matrixRegister == Right.matrixRegister &&
			matrixIsTransposed == Right.matrixIsTransposed &&
			incorrectProjection == Right.incorrectProjection &&
			preTransformProjectionId == Right.preTransformProjectionId;
	}

	bool operator!=(const ProjectionMatrixData& Right) const
	{
		return !(*this == Right);
	}
};

struct UnprojectionMatrixData
{
	DWORD	matrixRegister;		// shader matrix register index - 2'nd index in asm
	BOOL	matrixIsTransposed;

	UnprojectionMatrixData()
	:	matrixRegister(0)
	,	matrixIsTransposed(FALSE)
	{
	}

	bool operator==(const UnprojectionMatrixData& Right) const
	{
		return matrixRegister == Right.matrixRegister && matrixIsTransposed == Right.matrixIsTransposed;
	}

	bool operator!=(const UnprojectionMatrixData& Right) const
	{
		return !(*this == Right);
	}
};

template<typename MatrixData>
struct MatrixDataWithCB
{
	DWORD		constantBuffer;	
	MatrixData	md;

	MatrixDataWithCB() : md()
	{
		constantBuffer = 0;
	}
};

typedef std::vector<ProjectionMatrixData>	ProjectionCBMatrices;
typedef std::vector<UnprojectionMatrixData> UnprojectionCBMatrices;

typedef std::hash_map<DWORD, ProjectionCBMatrices>		ProjectionMatricesMap;
typedef std::hash_map<DWORD, UnprojectionCBMatrices>	UnprojectionMatricesMap;

template<typename MatrixData>
struct ShaderCBs
{
	typedef std::vector<MatrixData>				CBMatrices;
	typedef std::hash_map<DWORD, CBMatrices>	MatricesMap;
	
	MatricesMap cb;
	DWORD		mask;

	ShaderCBs()
		: mask(0)
	{ 
	}

	void clear() 
	{ 
		cb.clear();
		mask = 0;
	}

	bool operator==(const ShaderCBs& Right) const
	{
		return mask == Right.mask &&
			cb == Right.cb;
	}

	bool operator!=(const ShaderCBs& Right) const
	{
		return !(*this == Right);
	}
};

typedef ShaderCBs<ProjectionMatrixData>		ProjectionShaderCBs;
typedef ShaderCBs<UnprojectionMatrixData>	UnprojectionShaderCBs;


struct ProjectionShaderData
{
	ShaderCBs<ProjectionMatrixData> matrixData;
};

struct UnprojectionShaderData
{
	ShaderCBs<UnprojectionMatrixData> matrixData;
};

struct ShaderData
{
	ProjectionShaderData	projectionShaderData;
	std::bitset<128>		userResourcesMask;
};
