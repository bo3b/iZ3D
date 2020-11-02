#include "stdafx.h"
#include "ShaderOperand.h"
#include "ShaderOperation.h"
#include <ostream>
#include <iomanip>
#include <boost/lexical_cast.hpp>

const shader_analyzer::ShaderOperation* shader_analyzer::ShaderOperation::m_pLast = NULL;

shader_analyzer::ShaderOperand::ShaderOperand( unsigned* data ): m_pData(data), m_indexOperands(3), m_length(0)
{	 
	if (!m_pData)
	{
		m_length = 0;
		return;
	}
	++m_length;
	if (isOperandExtended())
		++m_length;
	if(getOperandType() == D3D10_SB_OPERAND_TYPE_IMMEDIATE32)
	{
		switch(getOperandNumComponents())
		{
		case D3D10_SB_OPERAND_0_COMPONENT:
			break;
		case D3D10_SB_OPERAND_1_COMPONENT:
			++m_length;
			break;
		case D3D10_SB_OPERAND_4_COMPONENT:
			m_length += 4;
			break;
		case D3D10_SB_OPERAND_N_COMPONENT:
			break;
		}
		return;
	}
	for(int i = 0; i < 3; i++)
	{
		D3D10_SB_OPERAND_INDEX_DIMENSION dim = getOperandIndexDimension();
		if(dim <= i)
			break;
		switch(getOperandIndexRepresentation((D3D10_SB_OPERAND_INDEX_DIMENSION)(i)))
		{
		case D3D10_SB_OPERAND_INDEX_IMMEDIATE32:
			m_indexes[i].val[0] = m_pData[m_length];
			++m_length;
			break;
		case D3D10_SB_OPERAND_INDEX_IMMEDIATE64:
			m_indexes[i].val[0] = (m_pData[m_length]);
			m_indexes[i].val[1] = (m_pData[m_length + 1]);
			m_length += 2;
			break;
		case D3D10_SB_OPERAND_INDEX_RELATIVE:
			m_indexOperands[i] = ShaderOperand(&m_pData[m_length]);
			m_length += m_indexOperands[i].m_length;
			break;
		case D3D10_SB_OPERAND_INDEX_IMMEDIATE32_PLUS_RELATIVE:
			m_indexes[i].val[0] = m_pData[m_length];
			++m_length;
			m_indexOperands[i] = ShaderOperand(&m_pData[m_length]);
			m_length += m_indexOperands[i].m_length;
			break;
		case D3D10_SB_OPERAND_INDEX_IMMEDIATE64_PLUS_RELATIVE:

			m_indexes[i].val[0] = (m_pData[m_length]);
			m_indexes[i].val[1] = (m_pData[m_length + 1]);
			m_length += 2;
			m_indexOperands[i] = ShaderOperand(&m_pData[m_length]);
			m_length += m_indexOperands[i].m_length;
			break;
		}
	}
}

shader_analyzer::ShaderOperand::ShaderOperand( const shader_analyzer::ShaderOperand& shOperand ) :
m_length(shOperand.m_length),
m_pData(shOperand.m_pData)
{
	memcpy_s(m_indexes, sizeof(m_indexes), shOperand.m_indexes, sizeof(m_indexes));
	m_indexOperands.resize(shOperand.m_indexOperands.size());
	for(unsigned i = 0; i < m_indexOperands.size(); i++)
	{
		if(shOperand.m_indexOperands[i].m_pData)
			m_indexOperands[i] = shOperand.m_indexOperands[i];
	}
}

shader_analyzer::ShaderOperand::ShaderOperand():
m_length(0),
m_pData(0)
{
	memset(m_indexes, 0, sizeof(m_indexes));
}


const unsigned shader_analyzer::ShaderOperand::getImmediateValue() const
{
	return *m_pData;
}

std::string shader_analyzer::ShaderOperand::toString() const
{
	char b[255];
	std::ostrstream a( b, sizeof( b ) );
	SecureZeroMemory( b, sizeof( b ) );
	a << *this;
	return b;
}

std::string shader_analyzer::ShaderOperand::getName() const
{
	std::string outStr;
	switch( getOperandType() )
	{
	case D3D10_SB_OPERAND_TYPE_TEMP:
		{
			outStr = "r";
			break;
		}
	case D3D10_SB_OPERAND_TYPE_INPUT:
		{
			outStr = "v";
			break;
		}
	case D3D10_SB_OPERAND_TYPE_OUTPUT:
		{
			outStr = "o";
			break;
		}
	case D3D10_SB_OPERAND_TYPE_INDEXABLE_TEMP:
		{
			outStr = "indextemp";
			break;
		}
	case D3D10_SB_OPERAND_TYPE_IMMEDIATE32:
		{
			outStr = "imm32";
			break;
		}
	case D3D10_SB_OPERAND_TYPE_IMMEDIATE64:
		{
			outStr = "imm64";
			break;
		}
	case D3D10_SB_OPERAND_TYPE_SAMPLER:
		{
			outStr = "samp";
			break;
		}
	case D3D10_SB_OPERAND_TYPE_RESOURCE:
		{
			outStr = "t";
			break;
		}
	case D3D10_SB_OPERAND_TYPE_CONSTANT_BUFFER:
		{
			outStr = "cb";
			break;
		}
	case D3D10_SB_OPERAND_TYPE_IMMEDIATE_CONSTANT_BUFFER:
		{
			outStr = "immconst";
			break;
		}
	case D3D10_SB_OPERAND_TYPE_LABEL:
		{
			outStr = "label";
			break;
		}
	case D3D10_SB_OPERAND_TYPE_INPUT_PRIMITIVEID:
		{
			outStr = "input_primitive_id";
			break;
		}
	case D3D10_SB_OPERAND_TYPE_OUTPUT_DEPTH:
		{
			outStr = "output_depth";
			break;
		}
	case D3D10_SB_OPERAND_TYPE_NULL:
		{
			outStr = "null";
			break;
		}
	case D3D10_SB_OPERAND_TYPE_RASTERIZER:
		{
			outStr = "rasterizer";
			break;
		}
	case D3D10_SB_OPERAND_TYPE_OUTPUT_COVERAGE_MASK:
		{
			outStr = "output_coverage_mask";
			break;
		}
	}
	return outStr;
}

std::string shader_analyzer::ShaderOperand::getIndexString() const
{
	std::string retStr;
	for( unsigned i = 0; i < (unsigned)getOperandIndexDimension(); i++ )
	{
		if( i )
			retStr += "[";
		switch(getOperandIndexRepresentation((D3D10_SB_OPERAND_INDEX_DIMENSION)i))
		{
		case D3D10_SB_OPERAND_INDEX_IMMEDIATE32:
			retStr += boost::lexical_cast<std::string>( m_indexes[i].val[0] );
			break;
		case D3D10_SB_OPERAND_INDEX_IMMEDIATE64:
			retStr += boost::lexical_cast<std::string>( m_indexes[i].value );
			break;
		case D3D10_SB_OPERAND_INDEX_RELATIVE:
			retStr += m_indexOperands[i].toString();
			retStr += " + 0";
			break;
		case D3D10_SB_OPERAND_INDEX_IMMEDIATE32_PLUS_RELATIVE:
			retStr += m_indexOperands[i].toString();
			retStr += " + ";
			retStr += boost::lexical_cast<std::string>( m_indexes[i].val[0] );
			break;
		case D3D10_SB_OPERAND_INDEX_IMMEDIATE64_PLUS_RELATIVE:
			retStr += m_indexOperands[i].toString();
			retStr += " + ";
			retStr += boost::lexical_cast<std::string>( m_indexes[i].value );
			break;
		}
		if( i )
			retStr += "]";
	}
	return retStr;
}

bool shader_analyzer::ShaderOperand::setImmediate32IndexIfItIsTheOne( const unsigned ind )
{
	//////////////////////////////////////////////////////////////////////////
	unsigned m_length = 0;
	++m_length;
	if (isOperandExtended())
		++m_length;
	if(getOperandType() == D3D10_SB_OPERAND_TYPE_IMMEDIATE32)
	{
		switch(getOperandNumComponents())
		{
		case D3D10_SB_OPERAND_0_COMPONENT:
			break;
		case D3D10_SB_OPERAND_1_COMPONENT:
			++m_length;
			break;
		case D3D10_SB_OPERAND_4_COMPONENT:
			m_length += 4;
			break;
		case D3D10_SB_OPERAND_N_COMPONENT:
			break;
		}
	}

	// the operand doesn't have the one index
	if ( getOperandIndexDimension() != 1 )
		return false;

	// if the first index is not Immediate 32
	if ( getOperandIndexRepresentation( ( D3D10_SB_OPERAND_INDEX_DIMENSION )( 0 ) )
		!= D3D10_SB_OPERAND_INDEX_IMMEDIATE32 )
		return false;

	m_pData[m_length] = ind;
	m_indexes[0].val[0] = ind;
	return true;
}

std::ostream& shader_analyzer::operator << (std::ostream& os, const shader_analyzer::ShaderOperand& sdOperand)
{
	if(sdOperand.isOperandExtended())
	{
		if(sdOperand.getExtendedOperandType() == D3D10_SB_EXTENDED_OPERAND_MODIFIER)
		{
			switch (sdOperand.getOperandModifier())
			{
			case D3D10_SB_OPERAND_MODIFIER_NEG:
				os << "-";
				break;
			case D3D10_SB_OPERAND_MODIFIER_ABS:
				os << "|";
				break;
			case D3D10_SB_OPERAND_MODIFIER_ABSNEG:
				os << "-|";
				break;
			}
		}
	}

	D3D10_SB_OPERAND_TYPE type = sdOperand.getOperandType();
	if (type != D3D10_SB_OPERAND_TYPE_IMMEDIATE32)
	{
		std::string componentStr;
		switch(sdOperand.getOperandNumComponents())
		{
		case D3D10_SB_OPERAND_1_COMPONENT:
			componentStr = ".x";
			break;
		case D3D10_SB_OPERAND_4_COMPONENT:
			componentStr += ".";
			switch(sdOperand.getOperand4ComponentSelectionMode())
			{
			case D3D10_SB_OPERAND_4_COMPONENT_MASK_MODE:
				{
					unsigned componentMask = sdOperand.getOperand4ComponentMask();
					if(componentMask & D3D10_SB_OPERAND_4_COMPONENT_MASK_X)
						componentStr += "x";
					if(componentMask & D3D10_SB_OPERAND_4_COMPONENT_MASK_Y)
						componentStr += "y";
					if(componentMask & D3D10_SB_OPERAND_4_COMPONENT_MASK_Z)
						componentStr += "z";
					if(componentMask & D3D10_SB_OPERAND_4_COMPONENT_MASK_W)
						componentStr += "w";
					break;
				}
			case D3D10_SB_OPERAND_4_COMPONENT_SWIZZLE_MODE:
				{
					for(unsigned i = 0;
						i < 4; i += 1)
					{
						switch(sdOperand.getOperand4ComponentSwizzleSource((D3D10_SB_4_COMPONENT_NAME)i))
						{
						case D3D10_SB_4_COMPONENT_X:
							componentStr += "x";
							break;
						case D3D10_SB_4_COMPONENT_Y:
							componentStr += "y";
							break;
						case D3D10_SB_4_COMPONENT_Z:
							componentStr += "z";
							break;
						case D3D10_SB_4_COMPONENT_W:
							componentStr += "w";
							break;
						}
					}
					break;
				}
			case D3D10_SB_OPERAND_4_COMPONENT_SELECT_1_MODE:
				{
					switch(sdOperand.getOperand4ComponentSelect1())
					{
					case D3D10_SB_4_COMPONENT_X:
						componentStr += "x";
						break;
					case D3D10_SB_4_COMPONENT_Y:
						componentStr += "y";
						break;
					case D3D10_SB_4_COMPONENT_Z:
						componentStr += "z";
						break;
					case D3D10_SB_4_COMPONENT_W:
						componentStr += "w";
						break;
					}
					break;
				}
			}
			break;
		}

		os << sdOperand.getName().c_str();
		os << sdOperand.getIndexString().c_str();
		os << componentStr.c_str();
	}
	else 
	{
		int n = sdOperand.getOperandNumComponents();
		if(n == D3D10_SB_OPERAND_4_COMPONENT)
			n = 4;
		os << "l(";
		if (ShaderOperation::m_pLast)
		{
			bool bAddComma = false;
			if (!ShaderOperation::m_pLast->isIntCommand())
			{
				float* p = (float*)&sdOperand.m_pData[1];
				for (int i = 0; i < n; i ++)
				{
					if (bAddComma)
						os << ", " << p[i];
					else
					{
						os << std::setw(2) << std::fixed << p[i];
						bAddComma = true;
					}
				}
			}
			else
			{
				int* p = (int*)&sdOperand.m_pData[1];
				for (int i = 0; i < n; i ++)
				{
					if (bAddComma)
						os << ", " << p[i];
					else
					{
						os << p[i];
						bAddComma = true;
					}
				}
			}
		}
		os << ")";
	}

	if(sdOperand.isOperandExtended())
	{
		if(sdOperand.getExtendedOperandType() == D3D10_SB_EXTENDED_OPERAND_MODIFIER)
		{
			switch (sdOperand.getOperandModifier())
			{
			case D3D10_SB_OPERAND_MODIFIER_ABS:
			case D3D10_SB_OPERAND_MODIFIER_ABSNEG:
				os << "|";
				break;
			}
		}
		// todo
	}

	return os;
}

bool shader_analyzer::operator == ( const shader_analyzer::ShaderOperand& rhw, const shader_analyzer::ShaderOperand& lhw )
{
	if( rhw.getLength() != lhw.getLength() )
		return false;
	if( rhw.getOperandType() != lhw.getOperandType() )
		return false;
	if( rhw.getOperandIndexDimension() != lhw.getOperandIndexDimension() )
		return false;
	for( unsigned i = 0; i < ( unsigned )rhw.getOperandIndexDimension(); i++ )
	{
		switch( rhw.getOperandIndexRepresentation( ( D3D10_SB_OPERAND_INDEX_DIMENSION )i ) )
		{
		case D3D10_SB_OPERAND_INDEX_IMMEDIATE32_PLUS_RELATIVE:
			if( !( rhw.m_indexOperands[i] == lhw.m_indexOperands[i] ) )
				return false;
		case D3D10_SB_OPERAND_INDEX_IMMEDIATE32:
			if( rhw.m_indexes[i].val[0] != lhw.m_indexes[i].val[0] )
				return false;
			break;
		case D3D10_SB_OPERAND_INDEX_IMMEDIATE64_PLUS_RELATIVE:
			if( !( rhw.m_indexOperands[i] == lhw.m_indexOperands[i] ) )
				return false;
		case D3D10_SB_OPERAND_INDEX_IMMEDIATE64:
			if( rhw.m_indexes[i].value != lhw.m_indexes[i].value )
				return false;
			break;
		case D3D10_SB_OPERAND_INDEX_RELATIVE:
			if( !( rhw.m_indexOperands[i] == lhw.m_indexOperands[i] ) )
				return false;
			break;
		}
	}
	return true;
}