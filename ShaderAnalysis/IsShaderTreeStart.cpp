#include "stdafx.h"
#include "IsShaderTreeStart.h"

shader_analyzer::IsShaderTreeStart::IsShaderTreeStart( const D3D10_SB_OPERAND_TYPE opType,
													  const unsigned index )
													  : opType_(opType), index_(index)
{}

bool shader_analyzer::IsShaderTreeStart::operator () ( const shader_analyzer::ShaderInstruction& inst )
{
	if( !inst.getOperation()->isAppropriateForProjectionMatrixFinding() )
		return false;
	const shader_analyzer::ShaderOperand* pBufOperand = inst.getOperand( 0 );
	if( pBufOperand->getOperandType() != opType_ )
		return false;
	if( pBufOperand->getOperandIndexDimension() != D3D10_SB_OPERAND_INDEX_1D )
		return false;
	if( pBufOperand->getOperandIndexRepresentation( D3D10_SB_OPERAND_INDEX_1D ) != 
		D3D10_SB_OPERAND_INDEX_IMMEDIATE32 )
		return false;
	if( pBufOperand->getIndex( 0 ) != index_ )
		return false;
	return true;
}