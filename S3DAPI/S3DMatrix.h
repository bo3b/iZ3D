#pragma once

#include "d3dx9math.h"

template<bool bTransposed>
struct S3DMatrix : public D3DXMATRIX
{
public:
	// access grants
	FLOAT& operator () ( UINT Row, UINT Col );
	FLOAT  operator () ( UINT Row, UINT Col ) const;
};

// access grants
template<bool bTransposed>
inline FLOAT&
S3DMatrix<bTransposed>::operator () ( UINT iRow, UINT iCol )
{
	if (!bTransposed)
		return m[iRow][iCol];
	else
		return m[iCol][iRow];
}

template<bool bTransposed>
inline FLOAT
S3DMatrix<bTransposed>::operator () ( UINT iRow, UINT iCol ) const
{
	if (!bTransposed)
		return m[iRow][iCol];
	else
		return m[iCol][iRow];
}
