#pragma once

#include <d3d10umddi.h>
#include <vector>
#include <list>
#include "ShaderOperation.h"

namespace shader_analyzer{
class ShaderOperandEdge;

unsigned GetShaderCodeSize(std::istream& iStream);

void GetShaderCode(std::istream& iStream, std::ostream& oStream);
std::string SamplerDeclarationToString( const UINT* pData );
std::string TessOutputPrimitiveDeclarationToString( const UINT* pData );
std::string ThreadGroupDeclarationToString( const UINT* pData );
std::string UnorderedAccessViewTypedDeclarationToString( const UINT* pData );
class ShaderInstruction
{
public:
	// ctors
	ShaderInstruction();
	ShaderInstruction( const unsigned* data, const size_t sizeInBytes );
	ShaderInstruction( const shader_analyzer::ShaderInstruction& shInst );
	ShaderInstruction( const ShaderOperation& shOperation );
	ShaderInstruction( const std::string& instrStr );

	ShaderInstruction& operator = ( const shader_analyzer::ShaderInstruction& shInst );
	inline size_t GetLength() const{return m_data.size();}
	const ShaderOperation* getOperation() const;

	size_t getOperandsCount() const;
	const ShaderOperand* getOperand( const unsigned index ) const;

	std::string toString() const;
	ShaderOperandEdge getComponentsEffectOnGiven(
		const unsigned inComponents,
		const unsigned operandIndex ) const;
	void GetBlob( std::vector<UINT> &retData ) const;
	const std::vector<unsigned>& GetData( ) const
	{
		return m_data;
	}
private:
	typedef std::vector<unsigned> TDataArray;
	typedef std::vector<ShaderOperand> TOperandArray;
	TDataArray m_data;
	ShaderOperation m_sdOperation;
	TOperandArray m_operands;
	void ParseInstruction();
	friend std::istream& operator >> (std::istream& is, shader_analyzer::ShaderInstruction& sdInst);
	friend std::ostream& operator << (std::ostream& os, const shader_analyzer::ShaderInstruction& sdInst);
};

typedef std::vector< shader_analyzer::ShaderInstruction > TShaderList;

}

