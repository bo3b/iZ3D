#include "stdafx.h"
#include "ShaderOperand.h"
#include "IsInstructionHasNeededOperand.h"

shader_analyzer::ShaderOperandEdge
shader_analyzer::IsInstructionHasNeededOperand::operand_( 0 );
shader_analyzer::IsInstructionHasNeededOperand::IsInstructionHasNeededOperand(
	const shader_analyzer::ShaderOperandEdge& Operand,
	D3D10_SB_OPERAND_TYPE startOperandType,
	unsigned startIndex )
	: startOperandType_( startOperandType ),
	startIndex_(startIndex)
{
	operand_ = Operand;
}

bool shader_analyzer::IsInstructionHasNeededOperand::operator () ( const ShaderInstruction& pInstruction, 
																  TOperandElement pOpEdge )
{
	// if instruction has less than 2 operands
	if( pInstruction.getOperandsCount() < 2 )
		return false;

	const shader_analyzer::ShaderOperand* pBufOperand = pInstruction.getOperand( 0 );
	if ( !( pBufOperand->getComponentState() &
		pOpEdge.first.getComponentState() ) )
		return false;
	if( pOpEdge.first.getOperand() )
	{
		// get destination operand of instruction
		if ( !( *pBufOperand == *pOpEdge.first.getOperand() ) )
			return false;
		
	}
	else
	{
		if( pBufOperand->getOperandType() != startOperandType_ )
			return false;
		if( pBufOperand->getOperandIndexDimension() !=
			D3D10_SB_OPERAND_INDEX_1D )
			return false;
		if ( pBufOperand->getIndex( 0 ) != startIndex_ )
			return false;
	}

	if( !operand_.getOperand() )
		operand_ = pOpEdge.first;
	return true;
}

shader_analyzer::ShaderOperandEdge shader_analyzer::IsInstructionHasNeededOperand::getOperand() const
{
	return operand_;
}

void shader_analyzer::IsInstructionHasNeededOperand::setOperand( const ShaderOperandEdge& op )
{
	operand_ = op;
}