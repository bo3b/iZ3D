#include "StdAfx.h"
#include "ShaderProcessor.h"
#include <algorithm>
#include <string>
#include <boost/tokenizer.hpp>
#include <d3d10umddi.h>
iz3d::shader::ShaderProcessor::TOperationBinaryElement
	iz3d::shader::ShaderProcessor::m_sOperationHandle;


iz3d::shader::ShaderProcessor::TOperationNumOperandsElement
	iz3d::shader::ShaderProcessor::m_sOperationOperandsNumber;


iz3d::shader::ShaderProcessor::TOperationBinaryElement
	iz3d::shader::ShaderProcessor::m_sOperandTypes;



iz3d::shader::ShaderProcessor::ShaderProcessor()
{
    if ( m_sOperationHandle.empty() )
    {
	    //////////////////////////////////////////////////////////////////////////
	    m_sOperationHandle["dp4"] = ENCODE_D3D10_SB_OPCODE_TYPE( D3D10_SB_OPCODE_DP4 );
	    m_sOperationHandle["mov"] = ENCODE_D3D10_SB_OPCODE_TYPE( D3D10_SB_OPCODE_MOV );
	    m_sOperationHandle["mad"] = ENCODE_D3D10_SB_OPCODE_TYPE( D3D10_SB_OPCODE_MAD );
	    m_sOperationHandle["add"] = ENCODE_D3D10_SB_OPCODE_TYPE( D3D10_SB_OPCODE_ADD );
	    m_sOperationHandle["lt"] = ENCODE_D3D10_SB_OPCODE_TYPE( D3D10_SB_OPCODE_LT );
	    //////////////////////////////////////////////////////////////////////////
	    m_sOperationOperandsNumber[D3D10_SB_OPCODE_DP4] = 3;
	    m_sOperationOperandsNumber[D3D10_SB_OPCODE_MOV] = 2;
	    m_sOperationOperandsNumber[D3D10_SB_OPCODE_MAD] = 4;
	    m_sOperationOperandsNumber[D3D10_SB_OPCODE_ADD] = 3;
	    m_sOperationOperandsNumber[D3D10_SB_OPCODE_LT] = 3;
	    //////////////////////////////////////////////////////////////////////////
	    m_sOperandTypes["o"] = ENCODE_D3D10_SB_OPERAND_TYPE( D3D10_SB_OPERAND_TYPE_OUTPUT );
	    m_sOperandTypes["r"] = ENCODE_D3D10_SB_OPERAND_TYPE( D3D10_SB_OPERAND_TYPE_TEMP );
	    m_sOperandTypes["cb"] = ENCODE_D3D10_SB_OPERAND_TYPE( D3D10_SB_OPERAND_TYPE_CONSTANT_BUFFER );
	    //////////////////////////////////////////////////////////////////////////
	    TOperationBinaryElement buf = m_sOperationHandle;
	    // add i* and r* commands
	    for( auto i = buf.begin(); i != buf.end(); ++i )
	    {
		    std::string bStr = "i";
		    bStr += i->first;
		    m_sOperationHandle[bStr] = i->second |
			    ENCODE_D3D10_SB_RESINFO_INSTRUCTION_RETURN_TYPE(D3D10_SB_RESINFO_INSTRUCTION_RETURN_UINT );
		    bStr[0] = 'r';
		    m_sOperationHandle[bStr] = i->second |
			    ENCODE_D3D10_SB_RESINFO_INSTRUCTION_RETURN_TYPE(D3D10_SB_RESINFO_INSTRUCTION_RETURN_RCPFLOAT );
	    }
    }
}

iz3d::shader::ShaderProcessor::~ShaderProcessor()
{

}

void iz3d::shader::ShaderProcessor::CompileInstruction( const char* pStr,
	TBinData& retData )
{
	retData.erase( retData.begin(), retData.end() );
	//auto len = strlen( pStr );
	typedef boost::char_separator<char> TSeparator;
	typedef boost::tokenizer< TSeparator > TTokenizer;
	TSeparator sep(" \t\n,");
	std::string strToParse( pStr );
	TTokenizer tok( strToParse, sep );
	std::vector<std::string> words;
	for(auto beg = tok.begin(); beg != tok.end(); ++beg )
	{
		words.push_back( *beg );
	}
	TBinData operationCode;
	parseOperation( words[0].c_str(), operationCode );
	// if this operation is not exists
	if( operationCode.empty() )
		return;
	// if number of operands is not passed
	if ( m_sOperationOperandsNumber[DECODE_D3D10_SB_OPCODE_TYPE(operationCode[0])] + 1 != words.size() )
		return;

	retData = operationCode;
	parseOperand( words[1].c_str(), operationCode, true );
	retData.insert( retData.end(), operationCode.begin(), operationCode.end() );
	for( size_t i = 2; i < words.size(); ++i )
	{
		parseOperand( words[i].c_str(), operationCode, false );
		retData.insert( retData.end(), operationCode.begin(), operationCode.end() );
	}
	// insert instruction size in OperationCode0
	retData[0] |= ENCODE_D3D10_SB_TOKENIZED_INSTRUCTION_LENGTH( retData.size() );
	return;
}

bool iz3d::shader::ShaderProcessor::parseOperation( const char* pStr, TBinData& operationCode  )
{
	DWORD opcode0 = 0;
	auto i = m_sOperationHandle.find( pStr );
	if ( i == m_sOperationHandle.end() )
		return false;
	opcode0 = i->second;
	operationCode.push_back( opcode0 );
	return true;
}

bool iz3d::shader::ShaderProcessor::parseOperand( const char* pStr, TBinData& operandCode, bool isFirst )
{
	operandCode.clear();
	auto len = strlen(pStr);
	if( len < 2 )
		return false;
	typedef boost::char_separator<char> TSeparator;
	typedef boost::tokenizer< TSeparator > TTokenizer;
	TSeparator sep("0123456789-");
	std::string strToParse( pStr );
	TTokenizer tok( strToParse, sep );
	std::string operandType = *tok.begin();
	auto opBinType = m_sOperandTypes.find( operandType );
	if( opBinType == m_sOperandTypes.end() )
		return false;
	DWORD opCode0 = opBinType->second;
	bool isNegate = pStr[0] == '-';
	const char* pCurPos = pStr + operandType.size() + ( isNegate ? 1 : 0 );
	TBinData indexes;
	defineOperandIndices( pCurPos, indexes );
	auto componentStart = std::find( pCurPos, pStr + len, '.' );
	TBinData components;
	if( componentStart != pStr + len )
		defineComponents( componentStart + 1, components );
	opCode0 |= getBinDataForComponents( components, isFirst );
	opCode0 |= getBinDataForIndices( indexes );
	opCode0 |= ENCODE_D3D10_SB_OPERAND_EXTENDED( isNegate );
	operandCode.push_back(opCode0);
	if( isNegate )
	{
		operandCode.push_back(
			ENCODE_D3D10_SB_EXTENDED_OPERAND_TYPE( D3D10_SB_EXTENDED_OPERAND_MODIFIER ) |
			ENCODE_D3D10_SB_EXTENDED_OPERAND_MODIFIER( D3D10_SB_OPERAND_MODIFIER_NEG ) );
	}
	for( size_t i = 0; i < indexes.size(); ++i )
	{
		operandCode.push_back( indexes[i] );
	}
	return true;
}

bool iz3d::shader::ShaderProcessor::defineOperandIndices(
	const char* pStr, iz3d::shader::ShaderProcessor::TBinData& indexes )
{
	indexes.clear();
	auto len = strlen( pStr );
	if ( !len )
		return false;

	const char sep[] = ".[";
	auto endOfNum = std::find_first_of( pStr, pStr + len, sep, sep + sizeof( sep ) );
	DBG_UNREFERENCED_LOCAL_VARIABLE(endOfNum);
	auto num = atoi( pStr );
	if( errno == ERANGE )
		return false;
	indexes.push_back( num );
	auto startOfNum = std::find( pStr, pStr + len, '[');
	if( startOfNum == pStr + len )
		return true;
	num = atoi( startOfNum + 1 );
	if( errno == ERANGE )
		return false;
	indexes.push_back( num );
	startOfNum = std::find( startOfNum + 1, pStr + len, '[' );
	if( startOfNum == pStr + len )
		return true;
	num = atoi( startOfNum + 1 );
	if( errno == ERANGE )
		return false;
	indexes.push_back( num );
	return true;
}

DWORD iz3d::shader::ShaderProcessor::defineOperandType( const char* /*pStr*/ )
{
	return 0;
}

bool iz3d::shader::ShaderProcessor::defineComponents( const char* pStr, TBinData& components )
{
	components.clear();
	size_t len = strlen( pStr );
	if( !len )
		return true;
	for( size_t i = 0; i < len; ++i )
	{
		auto ret = D3D10_SB_4_COMPONENT_X;
		switch( pStr[i] )
		{
		case 'x':
			break;
		case 'y':
			ret = D3D10_SB_4_COMPONENT_Y;
			break;
		case 'z':
			ret = D3D10_SB_4_COMPONENT_Z;
			break;
		case 'w':
			ret = D3D10_SB_4_COMPONENT_W;
			break;
		default:
			return false;
		}
		components.push_back( ret );
	}
	return true;
}

DWORD iz3d::shader::ShaderProcessor::getBinDataForComponents( const TBinData& components, bool isFirst )
{
	if( components.empty() )
		return 0;
	DWORD ret = ENCODE_D3D10_SB_OPERAND_NUM_COMPONENTS( D3D10_SB_OPERAND_4_COMPONENT );
	if( isFirst )
	{
		ret |= ENCODE_D3D10_SB_OPERAND_4_COMPONENT_SELECTION_MODE( D3D10_SB_OPERAND_4_COMPONENT_MASK_MODE );

		auto i = std::find( components.begin(), components.end(), D3D10_SB_4_COMPONENT_X );
		if( i != components.end() )
			ret |= ENCODE_D3D10_SB_OPERAND_4_COMPONENT_MASK( D3D10_SB_OPERAND_4_COMPONENT_MASK_X );

		i = std::find( components.begin(), components.end(), D3D10_SB_4_COMPONENT_Y );
		if( i != components.end() )
			ret |= ENCODE_D3D10_SB_OPERAND_4_COMPONENT_MASK( D3D10_SB_OPERAND_4_COMPONENT_MASK_Y );

		i = std::find( components.begin(), components.end(), D3D10_SB_4_COMPONENT_Z );
		if( i != components.end() )
			ret |= ENCODE_D3D10_SB_OPERAND_4_COMPONENT_MASK( D3D10_SB_OPERAND_4_COMPONENT_MASK_Z );

		i = std::find( components.begin(), components.end(), D3D10_SB_4_COMPONENT_W );
		if( i != components.end() )
			ret |= ENCODE_D3D10_SB_OPERAND_4_COMPONENT_MASK( D3D10_SB_OPERAND_4_COMPONENT_MASK_W );
	}
	else if( components.size() == 1 )
	{
		ret |= ENCODE_D3D10_SB_OPERAND_4_COMPONENT_SELECTION_MODE( D3D10_SB_OPERAND_4_COMPONENT_SELECT_1_MODE );
		ret |= ENCODE_D3D10_SB_OPERAND_4_COMPONENT_SELECT_1( components[0] );
	}
	else
	{
		ret |= ENCODE_D3D10_SB_OPERAND_4_COMPONENT_SELECTION_MODE( D3D10_SB_OPERAND_4_COMPONENT_SWIZZLE_MODE );
		switch( components.size() )
		{
		case 2:
			ret |= ENCODE_D3D10_SB_OPERAND_4_COMPONENT_SWIZZLE( components[0], components[1], 0, 0 );
			break;
		case 3:
			ret |= ENCODE_D3D10_SB_OPERAND_4_COMPONENT_SWIZZLE( components[0], components[1], components[2], 0 );
			break;
		case 4:
			ret |= ENCODE_D3D10_SB_OPERAND_4_COMPONENT_SWIZZLE( components[0], components[1], components[2], components[3] );
			break;
		default:
			return 0;
		}
	}
	return ret;
}

DWORD iz3d::shader::ShaderProcessor::getBinDataForIndices( const TBinData& indices )
{
	// all indices should be immediate 32
	return ENCODE_D3D10_SB_OPERAND_INDEX_DIMENSION( indices.size() );
}
