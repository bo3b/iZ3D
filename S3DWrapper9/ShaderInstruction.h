/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "ShaderRegister.h"
#include <boost/unordered_map.hpp>
#include <hash_map>

#pragma warning(disable : 4996)

class ShaderInstruction
{
public:
	ShaderInstruction();

	inline const char*	getUnmodifiedCommand()									{ return command; };
	inline const char*	getOperand(CommandParameters commandIndex)				{ return opField[0][commandIndex]; };
	inline char*	getUnmodifiedOperand(CommandParameters commandIndex)		{ return opField[1][commandIndex]; };
	inline char*	getSwizzle(char* reg);
	void	execute(DWORD shaderVersion, const char* shaderText, char* outputRegisterName, ProcessorRegister& outputRegister, DWORD &textures);

	size_t	GetTotalRegisters() { return registerManager.size(); }
protected:
	typedef boost::unordered_map<int, ProcessorRegister> Registers_t;
	Registers_t TmpRegisters;
	std::list<ShaderRegister> registerManager;
	const ShaderRegister* pZero;
	const ShaderRegister* pOne;

	int     opCount;
	const char*	nextTag;
	char	command[MAX_OPERAND_LENGTH]; //--- 0 - original instruction code, 1 - without modifiers ---
	char	opField[2][MAX_INSTRUCTION_PARAM][MAX_OPERAND_LENGTH]; //--- 0 - original instruction code, 1 - without modifiers ---
 
	bool	readCodeTag();			//--- read from nextTag ---
	bool    readOperand(int codeIndex);
	const char*	readNextInstruction();				//--- read from nextTag ---
	bool    isComponentWritable(RegisterComponent componentIndex, const char* registerMame);
	void	getComponentValue(RegisterComponent componentIndex, CommandParameters commandIndex, const ShaderRegister* &outValue);
	void	IncrementConstantRegisterIndex();
	void	SetRegister(ProcessorRegister* pDest, ProcessorRegister* pSrc, int maxCounter, bool bReplace);
	void	InitProcessorRegistor(ProcessorRegister &pr);
	void	InitRegisters();

	ShaderRegister* CreateRegister();
	const ShaderRegister* CreateRegister(char* srcOp);
	const ShaderRegister* CreateRegistersVector(const ShaderRegister* first, ...);
	const ShaderRegister* CreateRegistersVectorAndExpand(const ShaderRegister* first, ...);

	void GetVariableList(const ShaderRegister* expr, const ShaderRegister* &outList);
	void GetVariableList(const ShaderRegister* expr1, const ShaderRegister* expr2, const ShaderRegister* &outList);
	void BuildFullListWitoutDublicates( const ShaderRegister* expr,  ShaderRegister* &reg );
};

// for example:  version = 0x200
extern inline char* GetShaderStartPosition(const char* pShaderText);
extern inline bool GetShaderoPosName(DWORD shaderVersion, const char* inputShader, char* oPosName);
extern char* strrstr( const char *str, const char *strSearch);

