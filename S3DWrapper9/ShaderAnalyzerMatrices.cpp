/* IZ3D_FILE: $Id$ 
*
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*
* $Author$
* $Revision$
* $Date$
* $LastChangedBy$
* $URL$
*/
#include "StdAfx.h"
#include "ShaderAnalyzerMatrices.h"
#include <algorithm>

using namespace std;

bool ShaderAnalyzerMatrices::move( std::vector<ScalarRegister> &m )
{
	if (m_Size < 4)
	{
		m_Matrix[m_Size].swap(m);
		m_Size++;
		return true;
	}
	else
		return false;
}

struct SortMatrixRegisters
{
	bool operator()(const ScalarRegister* reg1, const ScalarRegister* reg2)
	{
		if(reg1->registerNumber != reg2->registerNumber)
			return reg1->registerNumber < reg2->registerNumber;
		else
			return reg1->swizzleIndex < reg2->swizzleIndex;
	}
};

void ShaderAnalyzerMatrices::getSortedRegisters( std::vector<const ScalarRegister*> &m, int i, int j, int k )
{
	_ASSERT(m_Size == 3);
	m.resize(3);
	m[0] = &m_Matrix[0][i];
	m[1] = &m_Matrix[1][j];
	m[2] = &m_Matrix[2][k];
	sort(m.begin(), m.end(), SortMatrixRegisters());
}

void ShaderAnalyzerMatrices::getSortedRegisters( std::vector<const ScalarRegister*> &m, int i, int j, int k, int l )
{
	_ASSERT(m_Size == 4);
	m.resize(4);
	m[0] = &m_Matrix[0][i];
	m[1] = &m_Matrix[1][j];
	m[2] = &m_Matrix[2][k];
	m[3] = &m_Matrix[3][l];
	sort(m.begin(), m.end(), SortMatrixRegisters());
}

bool ShaderAnalyzerMatrices::for_each_4( CheckMatrix_t f, ShaderMatrixData& md, int oPosComponent )
{
	for(int i = (int)m_Matrix[0].size() - 1; i >= 0; i--)
	{
		for(int j = (int)m_Matrix[1].size() - 1; j >= 0; j--)
			for(int k = (int)m_Matrix[2].size() - 1; k >= 0; k--)
				for(int t = (int)m_Matrix[3].size() - 1; t >= 0; t--)
				{
					std::vector<const ScalarRegister*> m;
					getSortedRegisters(m, i, j, k, t);
					bool b = false;
					for (int p = 0; p < (int)m.size()-1; p++)
						if (m[p]->registerNumber == m[p+1]->registerNumber &&
							m[p]->swizzleIndex == m[p+1]->swizzleIndex )
							b = true;
					if (b)	continue;

					if (f(m, md, oPosComponent))
						return true;
				}
	}
	return false;
}

bool ShaderAnalyzerMatrices::for_each_3_4( CheckMatrix_t f, ShaderMatrixData& md, int oPosComponent )
{
	if (m_Size == 4)
		return for_each_4(f, md, oPosComponent);

	for(int i = (int)m_Matrix[0].size() - 1; i >= 0; i--)
	{
		for(int j = (int)m_Matrix[1].size() - 1; j >= 0; j--)
			for(int k = (int)m_Matrix[2].size() - 1; k >= 0; k--)
			{
				std::vector<const ScalarRegister*> m;
				getSortedRegisters(m, i, j, k);
				bool b = false;
				for (int p = 0; p < (int)m.size()-1; p++)
					if (m[p]->registerNumber == m[p+1]->registerNumber &&
						m[p]->swizzleIndex == m[p+1]->swizzleIndex )
						b = true;
				if (b)	continue;
				m.push_back(m[2]);

				if (f(m, md, oPosComponent))
					return true;
			}
	}
	return false;
}
