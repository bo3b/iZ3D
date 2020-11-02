#pragma once

#include "ShaderGraph.h"
#include "ShaderOperandEdge.h"
namespace shader_analyzer
{
	class ShaderOperand;
	class IsInstructionHasNeededOperand
	{
	public:
		typedef std::pair< const shader_analyzer::ShaderOperandEdge,
			const TGraph::vertex_descriptor > TOperandElement;

		IsInstructionHasNeededOperand(
			const shader_analyzer::ShaderOperandEdge& Operand,
			D3D10_SB_OPERAND_TYPE startOperandType,
			unsigned startIndex );

		bool operator () (
			const shader_analyzer::ShaderInstruction& pInstruction,
			TOperandElement pOpEdge );
		shader_analyzer::ShaderOperandEdge getOperand() const;
		void setOperand( const ShaderOperandEdge& op );
	private:
		static shader_analyzer::ShaderOperandEdge operand_;
		const D3D10_SB_OPERAND_TYPE startOperandType_;
		const unsigned startIndex_;
	};

}