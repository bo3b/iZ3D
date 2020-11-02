/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include <vector>
#include <map>
#include <list>
#include "shlwapi.h"
#include "ShaderAnalyzerMatrices.h"

#pragma warning(disable : 4996)

enum RegisterComponent { ComponentX = 0, ComponentY, ComponentZ, ComponentW, MAX_REGISTER_COMPONENTS };
enum CommandParameters { DEST = 0, SRC0, SRC1, SRC2, SRC3, SRC4, SRC5, MAX_INSTRUCTION_PARAM };
inline void operator++(RegisterComponent& i, int) { i = RegisterComponent(int(i) + 1); }
inline void operator++(CommandParameters& i, int) { i = CommandParameters(int(i) + 1); }

struct ShaderMatrixData;

class ShaderAnalyzerMatrices;

#define MAX_OPERAND_LENGTH		32

class ShaderRegister;

struct RegisterWithOp
{
	char comm; // \0, +, *
	const ShaderRegister* reg;
};

class ShaderInstruction;

typedef std::vector<RegisterWithOp>::const_iterator registersIterator;

struct COMULTIPLIERS
{
	registersIterator it;
	const RegisterWithOp* s1;
	const RegisterWithOp* s2;
	COMULTIPLIERS()
	{
		s1 = 0; s2 = 0;
	}
};

class ShaderRegister
{
public:
	char op[MAX_OPERAND_LENGTH]; // for example: \0, c1, v4

	std::vector<RegisterWithOp> elem;
	size_t cRegCountWithInd;
	size_t cRegCount;
	size_t vRegCount;

	void SetCount();
	bool GetCRegister( ScalarRegister& reg, bool skipIndexedConstantRegisters = true ) const;
	bool GetVRegister( ScalarRegister& reg ) const;
	void GetConstantRegistersList(std::vector<ScalarRegister> &s) const;
	void ToString( std::string &str ) const;
	void ToTreeString( std::string &str ) const;
	bool IsEmpty() const { return op[0] == '\0' && elem.size() == 0; }
	bool IsRegister() const { return elem.size() == 0; }

	// analyzing
	bool OperationsMoreThanOne() const;
	bool GetNextBracketCouple(registersIterator &it, const RegisterWithOp* &subelement) const;
	bool SearchCoMultiplies(COMULTIPLIERS& outVal) const;
private:
	ShaderRegister();
	friend ShaderInstruction;
};

struct treePos
{
	const ShaderRegister* p;
	registersIterator it;
};

struct ProcessorRegister
{
	const ShaderRegister* comp[4];
};
