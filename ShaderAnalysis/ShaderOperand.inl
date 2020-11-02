//////////////////////////////////////////////////////////////////////////
// ShaderOperand inline function
#pragma once
D3D10_SB_OPERAND_NUM_COMPONENTS shader_analyzer::ShaderOperand::getOperandNumComponents() const{
	return DECODE_D3D10_SB_OPERAND_NUM_COMPONENTS(m_pData[0]);
}

D3D10_SB_OPERAND_TYPE shader_analyzer::ShaderOperand::getOperandType()const{
	return DECODE_D3D10_SB_OPERAND_TYPE(m_pData[0]);
}

D3D10_SB_OPERAND_4_COMPONENT_SELECTION_MODE shader_analyzer::ShaderOperand::getOperand4ComponentSelectionMode() const{
	return DECODE_D3D10_SB_OPERAND_4_COMPONENT_SELECTION_MODE(m_pData[0]);
}

D3D10_SB_OPERAND_INDEX_DIMENSION shader_analyzer::ShaderOperand::getOperandIndexDimension() const{
	return DECODE_D3D10_SB_OPERAND_INDEX_DIMENSION(m_pData[0]);
}

bool shader_analyzer::ShaderOperand::isOperandExtended() const{
	return DECODE_IS_D3D10_SB_OPERAND_EXTENDED(m_pData[0]) != 0;
}

D3D10_SB_EXTENDED_OPERAND_TYPE shader_analyzer::ShaderOperand::getExtendedOperandType() const{
	return DECODE_D3D10_SB_EXTENDED_OPERAND_TYPE(m_pData[1]);
}

D3D10_SB_OPERAND_MODIFIER shader_analyzer::ShaderOperand::getOperandModifier() const{
	return DECODE_D3D10_SB_OPERAND_MODIFIER(m_pData[1]);
}

unsigned shader_analyzer::ShaderOperand::getOperand4ComponentMask() const{
	return DECODE_D3D10_SB_OPERAND_4_COMPONENT_MASK(m_pData[0]);
}

unsigned shader_analyzer::ShaderOperand::getOperand4ComponentSwizzle() const{
	return DECODE_D3D10_SB_OPERAND_4_COMPONENT_SWIZZLE(m_pData[0]);
}

D3D10_SB_4_COMPONENT_NAME shader_analyzer::ShaderOperand::getOperand4ComponentSwizzleSource(D3D10_SB_4_COMPONENT_NAME destComp) const{
	return DECODE_D3D10_SB_OPERAND_4_COMPONENT_SWIZZLE_SOURCE(m_pData[0], destComp);
}

D3D10_SB_4_COMPONENT_NAME shader_analyzer::ShaderOperand::getOperand4ComponentSelect1() const{
	return DECODE_D3D10_SB_OPERAND_4_COMPONENT_SELECT_1(m_pData[0]);
}

D3D10_SB_OPERAND_INDEX_REPRESENTATION
shader_analyzer::ShaderOperand::getOperandIndexRepresentation(D3D10_SB_OPERAND_INDEX_DIMENSION dim) const
{
	return DECODE_D3D10_SB_OPERAND_INDEX_REPRESENTATION(dim, m_pData[0]);
}

unsigned shader_analyzer::ShaderOperand::getLength() const
{
	return m_length;
}


unsigned shader_analyzer::ShaderOperand::getIndex( const unsigned i ) const
{
	return m_indexes[i].val[0];
}

unsigned shader_analyzer::ShaderOperand::GetSwizzleComponentState( D3D10_SB_4_COMPONENT_NAME sbName ) const
{
	unsigned retValue = 0;
	switch( sbName )
	{
	case D3D10_SB_4_COMPONENT_X:
		retValue = X_STATE;
		break;
	case D3D10_SB_4_COMPONENT_Y:
		retValue = Y_STATE;
		break;
	case D3D10_SB_4_COMPONENT_Z:
		retValue = Z_STATE;
		break;
	case D3D10_SB_4_COMPONENT_W:
		retValue = W_STATE;
		break;
	}
	return retValue;
}

unsigned shader_analyzer::ShaderOperand::getComponentState( unsigned outCS ) const
{
	unsigned retValue( 0 );
	switch( getOperandNumComponents() )
	{
	case D3D10_SB_OPERAND_1_COMPONENT:
		retValue = X_STATE;
		break;
	case D3D10_SB_OPERAND_4_COMPONENT:
		switch( getOperand4ComponentSelectionMode() )
		{
		case D3D10_SB_OPERAND_4_COMPONENT_MASK_MODE:
			{
				unsigned componentMask = getOperand4ComponentMask();
				if( componentMask & D3D10_SB_OPERAND_4_COMPONENT_MASK_X )
					retValue |= X_STATE;
				if( componentMask & D3D10_SB_OPERAND_4_COMPONENT_MASK_Y )
					retValue |= Y_STATE;
				if( componentMask & D3D10_SB_OPERAND_4_COMPONENT_MASK_Z )
					retValue |= Z_STATE;
				if( componentMask & D3D10_SB_OPERAND_4_COMPONENT_MASK_W )
					retValue |= W_STATE;
				break;
			}
		case D3D10_SB_OPERAND_4_COMPONENT_SWIZZLE_MODE:
			{
				for( unsigned i = 0;
					i < 4; i += 1 )
				{
					if ( ((1 << i) & outCS) == 0 )
						continue;
					D3D10_SB_4_COMPONENT_NAME sbName = getOperand4ComponentSwizzleSource(
						( D3D10_SB_4_COMPONENT_NAME )i );
					retValue |= GetSwizzleComponentState(sbName);
				}
				break;
			}
		case D3D10_SB_OPERAND_4_COMPONENT_SELECT_1_MODE:
			{
				retValue = GetSwizzleComponentState(getOperand4ComponentSelect1());
				break;
			}
		}
		break;
	}
	return retValue;
}


unsigned shader_analyzer::ShaderOperand::getComponentStateCount() const
{
	unsigned cState = getComponentState();
	unsigned retCount( 0 );
	if ( cState & X_STATE )
		retCount++;
	if ( cState & Y_STATE )
		retCount++;
	if ( cState & W_STATE )
		retCount++;
	if ( cState & Z_STATE )
		retCount++;
	return retCount;
}

//////////////////////////////////////////////////////////////////////////
unsigned shader_analyzer::ShaderOperand::getComponentFormGiven( const unsigned inComponent ) const
{
	unsigned retValue( 0 );
	if ( inComponent == 0 )
		return retValue;
	switch( getOperandNumComponents() )
	{
	case D3D10_SB_OPERAND_1_COMPONENT:
		retValue = inComponent & X_STATE;
		break;
	case D3D10_SB_OPERAND_4_COMPONENT:
		switch( getOperand4ComponentSelectionMode() )
		{
		case D3D10_SB_OPERAND_4_COMPONENT_MASK_MODE:
			{
				unsigned componentMask = getOperand4ComponentMask();
				if( componentMask & D3D10_SB_OPERAND_4_COMPONENT_MASK_X )
					if ( inComponent & ( 1 << D3D10_SB_4_COMPONENT_X ) )
						retValue |= X_STATE;
				if(componentMask & D3D10_SB_OPERAND_4_COMPONENT_MASK_Y)
					if ( inComponent & ( 1 << D3D10_SB_4_COMPONENT_Y ) )
						retValue |= Y_STATE;
				if(componentMask & D3D10_SB_OPERAND_4_COMPONENT_MASK_Z)
					if ( inComponent & ( 1 << D3D10_SB_4_COMPONENT_Z ) )
						retValue |= Z_STATE;
				if(componentMask & D3D10_SB_OPERAND_4_COMPONENT_MASK_W)
					if ( inComponent & ( 1 << D3D10_SB_4_COMPONENT_W ) )
						retValue |= W_STATE;
				break;
			}
		case D3D10_SB_OPERAND_4_COMPONENT_SWIZZLE_MODE:
			{
				for( unsigned i = 0; i < 4; i++ )
				{
					switch( getOperand4ComponentSwizzleSource( ( D3D10_SB_4_COMPONENT_NAME )i ) )
					{
					case D3D10_SB_4_COMPONENT_X:
						if ( inComponent & ( 1 << D3D10_SB_4_COMPONENT_X ) )
							retValue |= X_STATE;
						break;
					case D3D10_SB_4_COMPONENT_Y:
						if ( inComponent & ( 1 << D3D10_SB_4_COMPONENT_Y ) )
							retValue |= Y_STATE;
						break;
					case D3D10_SB_4_COMPONENT_Z:
						if ( inComponent & ( 1 << D3D10_SB_4_COMPONENT_Z ) )
							retValue |= Z_STATE;
						break;
					case D3D10_SB_4_COMPONENT_W:
						if ( inComponent & ( 1 << D3D10_SB_4_COMPONENT_W ) )
							retValue |= W_STATE;
						break;
					}
				}
				break;
			}
		case D3D10_SB_OPERAND_4_COMPONENT_SELECT_1_MODE:
			{
				if ( inComponent & X_STATE )
				{
					switch( getOperand4ComponentSelect1() )
					{
					case D3D10_SB_4_COMPONENT_X:
						retValue |= X_STATE;
						break;
					case D3D10_SB_4_COMPONENT_Y:
						retValue |= Y_STATE;
						break;
					case D3D10_SB_4_COMPONENT_Z:
						retValue |= Z_STATE;
						break;
					case D3D10_SB_4_COMPONENT_W:
						retValue |= W_STATE;
						break;
					}
					break;
				}
			}
		}
		break;
	}
	return retValue;

	void GetSwizzleComponentState( D3D10_SB_4_COMPONENT_NAME sbName, unsigned retValue );
}

//////////////////////////////////////////////////////////////////////////
void shader_analyzer::ShaderOperand::setOperandType( D3D10_SB_OPERAND_TYPE opType )
{
	// Clean operand type
	m_pData[0] &= ~D3D10_SB_OPERAND_TYPE_MASK;
	m_pData[0] |= ENCODE_D3D10_SB_OPERAND_TYPE(opType);
}