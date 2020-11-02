//////////////////////////////////////////////////////////////////////////
// ShaderOperation inline function
D3D10_SB_OPCODE_TYPE shader_analyzer::ShaderOperation::getType() const{
	return DECODE_D3D10_SB_OPCODE_TYPE(m_pData[0]);
}

D3D10_SB_CUSTOMDATA_CLASS shader_analyzer::ShaderOperation::getCustomDataClass() const {
	return DECODE_D3D10_SB_CUSTOMDATA_CLASS(m_pData[0]);
}

bool shader_analyzer::ShaderOperation::isSaturated() const{
	return DECODE_IS_D3D10_SB_INSTRUCTION_SATURATE_ENABLED(m_pData[0]) != 0;
}

D3D10_SB_INSTRUCTION_TEST_BOOLEAN shader_analyzer::ShaderOperation::getTestBoolean() const{
	return DECODE_D3D10_SB_INSTRUCTION_TEST_BOOLEAN(m_pData[0]);
}

D3D10_SB_RESINFO_INSTRUCTION_RETURN_TYPE shader_analyzer::ShaderOperation::getInstructionReturnType() const{
	return DECODE_D3D10_SB_RESINFO_INSTRUCTION_RETURN_TYPE(m_pData[0]);
}

bool shader_analyzer::ShaderOperation::isOpcodeExtended() const{
	return DECODE_IS_D3D10_SB_OPCODE_EXTENDED(m_pData[0]) != 0;
}

unsigned shader_analyzer::ShaderOperation::getLength() const
{
	return m_length;
}

unsigned shader_analyzer::ShaderOperation::getInstructionLength() const
{
	return DECODE_D3D10_SB_TOKENIZED_INSTRUCTION_LENGTH(m_pData[0]);
}

bool shader_analyzer::ShaderOperation::isCustomData() const
{
	return getType() == D3D10_SB_OPCODE_CUSTOMDATA;
}

bool shader_analyzer::ShaderOperation::isDeclaration() const
{
	return ((getType() >= D3D10_SB_OPCODE_DCL_RESOURCE) && (getType() <= D3D10_SB_OPCODE_DCL_GLOBAL_FLAGS)) ||
		((getType() >= D3D11_SB_OPCODE_DCL_STREAM) && (getType() <= D3D11_SB_OPCODE_DCL_RESOURCE_STRUCTURED)) ||
		(getType() == D3D11_SB_OPCODE_DCL_GS_INSTANCE_COUNT);
}

bool shader_analyzer::ShaderOperation::isAppropriateForProjectionMatrixFinding() const
{
	switch(getType())
	{
	case D3D10_SB_OPCODE_ADD:
	case D3D10_SB_OPCODE_DP2:
	case D3D10_SB_OPCODE_DP3:
	case D3D10_SB_OPCODE_DP4:
	case D3D10_SB_OPCODE_MAD:
	case D3D10_SB_OPCODE_MAX:
	case D3D10_SB_OPCODE_MIN:
	case D3D10_SB_OPCODE_MUL:
	case D3D10_SB_OPCODE_MOV:
	case D3D10_SB_OPCODE_MOVC:
		return true;
	}
	return false;
}

bool shader_analyzer::ShaderOperation::isIntCommand() const
{
	switch(getType())
	{
	case D3D10_SB_OPCODE_IADD:
	case D3D10_SB_OPCODE_IGE:
	case D3D10_SB_OPCODE_IEQ:
	case D3D10_SB_OPCODE_ILT:
	case D3D10_SB_OPCODE_IMAD:
	case D3D10_SB_OPCODE_IMAX:
	case D3D10_SB_OPCODE_IMIN:
	case D3D10_SB_OPCODE_IMUL:
	case D3D10_SB_OPCODE_INE:
	case D3D10_SB_OPCODE_INEG:
	case D3D10_SB_OPCODE_ISHL:
	case D3D10_SB_OPCODE_ISHR:
	case D3D10_SB_OPCODE_ITOF:
		return true;
	}
	return false;
}

bool shader_analyzer::ShaderOperation::isScalarCommand() const
{
	switch(getType())
	{
	case D3D10_SB_OPCODE_DP2:
	case D3D10_SB_OPCODE_DP3:
	case D3D10_SB_OPCODE_DP4:
		return true;
	}
	return false;
}

unsigned shader_analyzer::ShaderOperation::getDependentComponents(
	const shader_analyzer::ShaderOperand::EComponentState inComp, const ShaderOperand* pGivenOperand ) const
{
	unsigned comp = inComp;
	if (isScalarCommand())
	{
		switch( getType() )
		{
		case D3D10_SB_OPCODE_DP3:
			comp = ShaderOperand::X_STATE | ShaderOperand::Y_STATE | ShaderOperand::Z_STATE;
			break;
		case D3D10_SB_OPCODE_DP2:
			comp = ShaderOperand::X_STATE | ShaderOperand::Y_STATE;
			break;
		case D3D10_SB_OPCODE_DP4:
		default:
			comp = ShaderOperand::X_STATE | ShaderOperand::Y_STATE |
				ShaderOperand::Z_STATE | ShaderOperand::W_STATE;
		}
	}
	return pGivenOperand->getComponentState(comp);
}

bool shader_analyzer::ShaderOperation::isIncreaseIdent() const
{
	switch( getType() )
	{
	case D3D10_SB_OPCODE_IF:
	case D3D10_SB_OPCODE_LOOP:
	case D3D10_SB_OPCODE_SWITCH:
	case D3D10_SB_OPCODE_ELSE:
	case D3D10_SB_OPCODE_CASE:
		return true;
	default:
		return false;
	}
}

bool shader_analyzer::ShaderOperation::isDecreaseIdent() const
{
	switch( getType() )
	{
	case D3D10_SB_OPCODE_ENDIF:
	case D3D10_SB_OPCODE_ENDLOOP:
	case D3D10_SB_OPCODE_ENDSWITCH:
	case D3D10_SB_OPCODE_ELSE:
	case D3D10_SB_OPCODE_CASE:
		return true;
	default:
		return false;
	}
}
