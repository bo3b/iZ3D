#pragma once
#include <d3d10umddi.h>
#include <ostream>
#include "ShaderOperand.h"

namespace shader_analyzer{
class ShaderOperation
{
public:
	// ctors
	ShaderOperation();
	ShaderOperation(const unsigned* data);
	ShaderOperation(const shader_analyzer::ShaderOperation& shOperation);
	inline unsigned getLength() const;	// returns length in dwords
	inline unsigned getInstructionLength() const;	// returns length in dwords
	inline D3D10_SB_OPCODE_TYPE getType() const;
	inline D3D10_SB_CUSTOMDATA_CLASS getCustomDataClass() const;
	inline bool isSaturated() const;
	inline D3D10_SB_INSTRUCTION_TEST_BOOLEAN getTestBoolean() const;
	inline D3D10_SB_RESINFO_INSTRUCTION_RETURN_TYPE getInstructionReturnType() const;
	inline bool isOpcodeExtended() const;
	inline bool isDeclaration() const;
	inline bool isIntCommand() const;
	inline bool isScalarCommand() const;
	inline bool isCustomData() const;
	inline bool isAppropriateForProjectionMatrixFinding() const;
	inline bool	isIncreaseIdent() const;
	inline bool	isDecreaseIdent() const;
	inline unsigned getDependentComponents( const ShaderOperand::EComponentState inComp, const ShaderOperand* pGivenOperand ) const;
	static const ShaderOperation* m_pLast; // for operator <<
private:
	const unsigned* m_pData;
	unsigned m_length;
	friend std::ostream& operator << (std::ostream& os, const shader_analyzer::ShaderOperation& sdOperation);
};
}

#include "ShaderOperation.inl"