/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include <vector>

struct ShaderMatrixData;

struct ScalarRegister
{
	// register index
	int registerNumber;
	// 
	int swizzleIndex;
	bool operator==(const ScalarRegister& reg) { return registerNumber == reg.registerNumber; }
};

typedef bool CheckMatrix_t( std::vector<const ScalarRegister*> &m, ShaderMatrixData& md, int oPosComponent );

class ShaderAnalyzerMatrices
{
public:
	typedef std::vector<ScalarRegister> VectorOfRegisters;
	ShaderAnalyzerMatrices() : m_Size(0), m_Matrix(4) { }
	bool move(VectorOfRegisters &m);
	size_t size() { return m_Size; }
	VectorOfRegisters& operator[](ptrdiff_t n) { return m_Matrix[n]; }
	void getSortedRegisters( std::vector<const ScalarRegister*> &m, int i, int j, int k );
	void getSortedRegisters( std::vector<const ScalarRegister*> &m, int i, int j, int k, int l );

	bool for_each_4(CheckMatrix_t f, ShaderMatrixData& md, int oPosComponent);
	bool for_each_3_4(CheckMatrix_t f, ShaderMatrixData& md, int oPosComponent);
private:
	std::vector<VectorOfRegisters> m_Matrix;
	size_t	m_Size;
};
