#include "stdafx.h"
#include <istream>
#include "ShaderInstruction.h"
#include <WTypes.h>
#include <mmsystem.h>
#include "ShaderOperand.h"
#include "ShaderOperandEdge.h"
#include <istream>
#include <iostream>

using namespace std;
using namespace shader_analyzer;

istream& shader_analyzer::operator >> (istream& is, ShaderInstruction& sdInst)
{
	DWORD OpcodeToken0;
	is.read((char*)&OpcodeToken0, sizeof(DWORD));
	if(is.rdstate() & ios::failbit)
		return is;
	DWORD length;
	D3D10_SB_OPCODE_TYPE opCodeType = DECODE_D3D10_SB_OPCODE_TYPE(OpcodeToken0);
	// decode opcode type read opcode
	if(opCodeType != D3D10_SB_OPCODE_CUSTOMDATA)	// standard opcode token
	{
		length = DECODE_D3D10_SB_TOKENIZED_INSTRUCTION_LENGTH(OpcodeToken0);
		sdInst.m_data.resize(length);
		sdInst.m_data[0] = OpcodeToken0;
		if(length > 1)
		{
			is.read( ( char* )&sdInst.m_data[1],
				sizeof( unsigned ) * ( length - 1 ) );
		}
		sdInst.ParseInstruction();
	}
	else
	{		
		// token starts a custom-data block
		is.read((char*)&length, sizeof(unsigned));
		sdInst.m_data.resize(length);
		sdInst.m_data[0] = OpcodeToken0;
		sdInst.m_data[1] = length;
		if(length > 2)
			is.read( (char*)&sdInst.m_data[2],
				sizeof(unsigned) * (length - 2));
	}
	return is;
}
//////////////////////////////////////////////////////////////////////////

std::string ResourceReturnTypeToString( D3D10_SB_RESOURCE_RETURN_TYPE type )
{
	switch( type )
	{
	case D3D10_SB_RETURN_TYPE_UNORM:
		return "unorm";
	case D3D10_SB_RETURN_TYPE_SNORM:
		return "snorm";
	case D3D10_SB_RETURN_TYPE_SINT:
		return "sint";
	case D3D10_SB_RETURN_TYPE_UINT:
		return "uint";
	case D3D10_SB_RETURN_TYPE_FLOAT:
		return "float";
	case D3D10_SB_RETURN_TYPE_MIXED:
		return "mxd";
	case D3D11_SB_RETURN_TYPE_DOUBLE:
		return "double";
	case D3D11_SB_RETURN_TYPE_CONTINUED:
		return "continued";
	case D3D11_SB_RETURN_TYPE_UNUSED:
		return "unused";
	default:
		return "";
	}
}
std::string ResourceDeclarationToString( const UINT* pOperandsData )
{
	UINT OpcodeToken0= pOperandsData[0];
	std::string strToReturn;
	switch( DECODE_D3D10_SB_RESOURCE_DIMENSION( OpcodeToken0 ))
	{
	case D3D10_SB_RESOURCE_DIMENSION_BUFFER:
		strToReturn += "buf";
		break;
	case D3D10_SB_RESOURCE_DIMENSION_TEXTURE1D:
		strToReturn += "txt1d";
		break;
	case D3D10_SB_RESOURCE_DIMENSION_TEXTURE2D:
		strToReturn += "txt2d";
		break;
	case D3D10_SB_RESOURCE_DIMENSION_TEXTURE2DMS:
		strToReturn += "txt2dms[";
		strToReturn += boost::lexical_cast<std::string>( DECODE_D3D10_SB_RESOURCE_SAMPLE_COUNT(OpcodeToken0) );
		strToReturn += "]";
		break;
	case D3D10_SB_RESOURCE_DIMENSION_TEXTURE3D:
		strToReturn += "txt3d";
		break;
	case D3D10_SB_RESOURCE_DIMENSION_TEXTURECUBE:
		strToReturn += "txtcube";
		break;
	case D3D10_SB_RESOURCE_DIMENSION_TEXTURE1DARRAY:
		strToReturn += "txt1darr";
		break;
	case D3D10_SB_RESOURCE_DIMENSION_TEXTURE2DARRAY:
		strToReturn += "txt2darr";
		break;
	case D3D10_SB_RESOURCE_DIMENSION_TEXTURE2DMSARRAY:
		strToReturn += "txt2dmsarr[";
		strToReturn += boost::lexical_cast<std::string>( DECODE_D3D10_SB_RESOURCE_SAMPLE_COUNT(OpcodeToken0) );
		strToReturn += "]";
		break;
	case D3D10_SB_RESOURCE_DIMENSION_TEXTURECUBEARRAY:
		strToReturn += "txtcubearr";
		break;
	default:
		strToReturn += "unknown";
		break;
	}
	shader_analyzer::ShaderOperand op( const_cast<UINT*>(&pOperandsData[1]));
	strToReturn += " ";
	strToReturn += op.toString();
	UINT ResorceReturnTypeTopen = pOperandsData[op.getLength()];
	strToReturn += "( ";
	strToReturn += ResourceReturnTypeToString( DECODE_D3D10_SB_RESOURCE_RETURN_TYPE(ResorceReturnTypeTopen, 0 ) );
	for( UINT i = 1; i < 4; ++ i)
	{
		strToReturn += ", ";
		strToReturn += ResourceReturnTypeToString( DECODE_D3D10_SB_RESOURCE_RETURN_TYPE(ResorceReturnTypeTopen, i ) );
	}
	strToReturn += " )";
	return strToReturn;
}

std::string ConstantBufferDeclarationToString( const UINT* pData ) 
{
	std::string strToReturn;
	if( DECODE_D3D10_SB_CONSTANT_BUFFER_ACCESS_PATTERN( pData[0] )
		== D3D10_SB_CONSTANT_BUFFER_IMMEDIATE_INDEXED )
		strToReturn += "imm_ind ";
	else
		strToReturn += "dyn_ind ";
	shader_analyzer::ShaderOperand op( const_cast<UINT*>( &pData[1] ) );
	strToReturn += op.toString();
	return strToReturn;
}

std::string IndexRangeDeclarationToString( const UINT* pData ) 
{
	std::string strToReturn;
	shader_analyzer::ShaderOperand op( const_cast<UINT*>( &pData[1] ) );
	strToReturn += op.toString();
	strToReturn += " ";
	strToReturn += boost::lexical_cast<std::string>( pData[op.getLength()] );
	return strToReturn;
}

std::string InputOutputDeclarationToString( const UINT* pData ) 
{
	shader_analyzer::ShaderOperand op ( const_cast<UINT*>( &pData[1] ) );
	return op.toString();
}

std::string NameTokenToString( const UINT* pData ) 
{
	switch( DECODE_D3D10_SB_NAME( pData[0] ) )
	{
	case D3D10_SB_NAME_UNDEFINED:
		return "undef";
	case D3D10_SB_NAME_POSITION:
		return "position";
	case D3D10_SB_NAME_CLIP_DISTANCE:
		return "clip_distance";
	case D3D10_SB_NAME_CULL_DISTANCE:
		return "cull_distance";
	case D3D10_SB_NAME_RENDER_TARGET_ARRAY_INDEX:
		return "render_target_array_index";
	case D3D10_SB_NAME_VIEWPORT_ARRAY_INDEX:
		return "viewport_array_index";
	case D3D10_SB_NAME_VERTEX_ID:
		return "vertex_id";
	case D3D10_SB_NAME_PRIMITIVE_ID:
		return "primitive_id";
	case D3D10_SB_NAME_INSTANCE_ID:
		return "instance_id";
	case D3D10_SB_NAME_IS_FRONT_FACE:
		return "is_front_face";
	case D3D10_SB_NAME_SAMPLE_INDEX:
		return "sample_index";
			// The following are added for D3D11
	case D3D11_SB_NAME_FINAL_QUAD_U_EQ_0_EDGE_TESSFACTOR:
		return "final_quad_u_eq0_edge_tessfactor";
	case D3D11_SB_NAME_FINAL_QUAD_V_EQ_0_EDGE_TESSFACTOR:
		return "final_quad_v_eq0_edge_tessfactor";
	case D3D11_SB_NAME_FINAL_QUAD_U_EQ_1_EDGE_TESSFACTOR:
		return "final_quad_u_eq1_edge_tessfactor";
	case D3D11_SB_NAME_FINAL_QUAD_V_EQ_1_EDGE_TESSFACTOR:
		return "final_quad_v_eq1_edge_tessfactor";
	case D3D11_SB_NAME_FINAL_QUAD_U_INSIDE_TESSFACTOR:
		return "final_quad_u_inside_tessfactor";
	case D3D11_SB_NAME_FINAL_QUAD_V_INSIDE_TESSFACTOR:
		return "final_quad_v_inside_tessfactor";
	case D3D11_SB_NAME_FINAL_TRI_U_EQ_0_EDGE_TESSFACTOR:
		return "final_tri_u_eq0_edge_tessfactor";
	case D3D11_SB_NAME_FINAL_TRI_V_EQ_0_EDGE_TESSFACTOR:
		return "final_tri_v_eq0_edge_tessfactor";
	case D3D11_SB_NAME_FINAL_TRI_W_EQ_0_EDGE_TESSFACTOR:
		return "final_tri_w_eq0_edge_tessfactor";
	case D3D11_SB_NAME_FINAL_TRI_INSIDE_TESSFACTOR:
		return "final_tri_inside_tessfactor";
	case D3D11_SB_NAME_FINAL_LINE_DETAIL_TESSFACTOR:
		return "final_line_detail_tessfactor";
	case D3D11_SB_NAME_FINAL_LINE_DENSITY_TESSFACTOR:
		return "final_line_density_tessfactor";
	}
	return "";
}

//////////////////////////////////////////////////////////////////////////
std::string PixelShaderInputDeclarationToStirng( const UINT* pData ) 
{
	std::string strToReturn;
	switch( DECODE_D3D10_SB_INPUT_INTERPOLATION_MODE( pData[0] ) )
	{
	case D3D10_SB_INTERPOLATION_UNDEFINED:
		strToReturn += "undef ";
		break;
	case D3D10_SB_INTERPOLATION_CONSTANT:
		strToReturn += "interpol_const ";
		break;
	case D3D10_SB_INTERPOLATION_LINEAR:
		strToReturn += "interpol_lin ";
		break;
	case D3D10_SB_INTERPOLATION_LINEAR_CENTROID:
		strToReturn += "interpol_lin_centr ";
		break;
	case D3D10_SB_INTERPOLATION_LINEAR_NOPERSPECTIVE:
		strToReturn += "interpol_lin_nopersp ";
		break;
	case D3D10_SB_INTERPOLATION_LINEAR_NOPERSPECTIVE_CENTROID:
		strToReturn += "interpol_linear_npersp_cent ";
		break;
	case D3D10_SB_INTERPOLATION_LINEAR_SAMPLE: // DX10.1
		strToReturn += "interpol_lin_sample ";
		break;
	case D3D10_SB_INTERPOLATION_LINEAR_NOPERSPECTIVE_SAMPLE: // DX10.1
		strToReturn += "interpol_lin_nonpersp_sample ";
		break;
	}
	shader_analyzer::ShaderOperand op ( const_cast<UINT*>( &pData[1] ) );
	strToReturn += op.toString();
	return strToReturn;
}

std::string IndexableTempDeclarationToString( const UINT* pData ) 
{
	std::string strToReturn;
	UINT RegIndex = pData[1];
	UINT NumberOfRegistersInBank = pData[2];
	UINT NumberComponentsInArray = pData[3];
	strToReturn += "x";
	strToReturn += boost::lexical_cast<std::string>( RegIndex );
	strToReturn += "[" + boost::lexical_cast<std::string>( NumberOfRegistersInBank ) + "].";
	const char* strComponents = "xyzw";
	for( UINT i = 0; i < NumberComponentsInArray; ++i )
	{
		strToReturn += strComponents[i];
	}
	return strToReturn;
}

std::string GlobalFlagsDeclarationToString( const UINT* pData ) 
{
	std::string strToReturn;
	if( pData[0] & D3D10_SB_GLOBAL_FLAG_REFACTORING_ALLOWED )
		strToReturn += "RefactoringAllowed ";
	if( pData[0] & D3D11_SB_GLOBAL_FLAG_ENABLE_DOUBLE_PRECISION_FLOAT_OPS )
		strToReturn += "DoublePrecisionFloatOperandsEnabled ";
	if( pData[0] & D3D11_SB_GLOBAL_FLAG_FORCE_EARLY_DEPTH_STENCIL )
		strToReturn += "ForceEarlyDepthStencil";
	if( pData[0] & D3D11_SB_GLOBAL_FLAG_ENABLE_RAW_AND_STRUCTURED_BUFFERS )
		strToReturn += "RawAndStructuredBuffersEnabled";
	return strToReturn;
}

std::string TessDomainDeclarationToString( const UINT* pData ) 
{
	switch(DECODE_D3D11_SB_TESS_DOMAIN( pData[0] ) ){
	case D3D11_SB_TESSELLATOR_DOMAIN_UNDEFINED:
		return "Tess domain undefined";
	case D3D11_SB_TESSELLATOR_DOMAIN_ISOLINE:
		return "dcl tess domain isoline";
	case D3D11_SB_TESSELLATOR_DOMAIN_TRI:
		return "dcl tess domain tri";
	case D3D11_SB_TESSELLATOR_DOMAIN_QUAD:
		return "dcl tess domain quad";
	default:
		return "";
	}
}

std::string TessPartitioningDeclarationToString( const UINT* pData ) 
{
	switch( DECODE_D3D11_SB_TESS_PARTITIONING( pData[0] ) ){
	case D3D11_SB_TESSELLATOR_PARTITIONING_UNDEFINED:
		return "Tess partitioning undefined";
	case D3D11_SB_TESSELLATOR_PARTITIONING_INTEGER:
		return "dcl tess part int";
	case D3D11_SB_TESSELLATOR_PARTITIONING_POW2:
		return "dcl tess part pow2";
	case D3D11_SB_TESSELLATOR_PARTITIONING_FRACTIONAL_ODD:
		return "dcl tess part frac odd";
	case D3D11_SB_TESSELLATOR_PARTITIONING_FRACTIONAL_EVEN:
		return "dcl tess part frac even";
	default:
		return "";
	}
}

std::string PixelShaderInputSivDeclarationToString( const UINT* pData, UINT length ) 
{
	return PixelShaderInputDeclarationToStirng( pData ) + " " + NameTokenToString( &pData[length - 1 ]);
}
//////////////////////////////////////////////////////////////////////////
std::string InputOutputSgvSivDeclarationToString( const UINT* pData ) 
{
	std::string strToReturn;
	shader_analyzer::ShaderOperand op ( const_cast<UINT*>( &pData[1] ) );
	strToReturn += op.toString();
	strToReturn += " ";
	strToReturn += NameTokenToString( &pData[op.getLength()] );
	return strToReturn;
}
std::string GeometryShaderInputPrimitiveDeclarationToString( const UINT* pData ) 
{
	D3D10_SB_PRIMITIVE prim = DECODE_D3D10_SB_GS_INPUT_PRIMITIVE( pData[0] );
	switch( prim )
	{
	case D3D10_SB_PRIMITIVE_UNDEFINED:
		return "undef";
	case D3D10_SB_PRIMITIVE_POINT:
		return "point";
	case D3D10_SB_PRIMITIVE_LINE:
		return "line";
	case D3D10_SB_PRIMITIVE_TRIANGLE:
		return "triangle";
	case D3D10_SB_PRIMITIVE_LINE_ADJ:
		return "line_adj";
	case D3D10_SB_PRIMITIVE_TRIANGLE_ADJ:
		return "triangle_adj";
	default:
		return boost::lexical_cast<std::string>(prim - D3D10_SB_PRIMITIVE_TRIANGLE_ADJ) + "_cp_patch";
	}
}
std::string GeometryShaderOutputPrimitiveTopologyDeclarationToString( const UINT* pData ) 
{
	switch( DECODE_D3D10_SB_GS_OUTPUT_PRIMITIVE_TOPOLOGY( pData[0] ) )
	{
	case D3D10_SB_PRIMITIVE_TOPOLOGY_UNDEFINED:
		return "undef";
	case D3D10_SB_PRIMITIVE_TOPOLOGY_POINTLIST:
		return "pointlist";
	case D3D10_SB_PRIMITIVE_TOPOLOGY_LINELIST:
		return "linelist";
	case D3D10_SB_PRIMITIVE_TOPOLOGY_LINESTRIP:
		return "linestrip";
	case D3D10_SB_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
		return "trianglelist";
	case D3D10_SB_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
		return "trianglestrip";
	case D3D10_SB_PRIMITIVE_TOPOLOGY_LINELIST_ADJ:
		return "linelist_adj";
	case D3D10_SB_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ:
		return "linestrip_adj";
	case D3D10_SB_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ:
		return "trianglelist_adj";
	case D3D10_SB_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ:
		return "trianglestrip_adj";
	}
	return "";
}
std::string ControlPointCountToString( const UINT* pData ) 
{
	UINT count = DECODE_D3D11_SB_OUTPUT_CONTROL_POINT_COUNT( pData[0] );
	return boost::lexical_cast<std::string>(count);
}
std::string HSMaxTessFactorToString( const UINT* pData ) 
{
#define DECODE_D3D11_SB_HS_MAX_TESSFACTOR(OpcodeToken0) ((FLOAT)(((OpcodeToken0)&D3D11_SB_INPUT_CONTROL_POINT_COUNT_MASK)>>D3D11_SB_INPUT_CONTROL_POINT_COUNT_SHIFT))
	float factor = DECODE_D3D11_SB_HS_MAX_TESSFACTOR( pData[0] );
	return boost::lexical_cast<std::string>(factor);
}
ostream& shader_analyzer::operator << (ostream& os, const ShaderInstruction& sdInst)
{
	ShaderOperation::m_pLast = &sdInst.m_sdOperation;

	// I haven't found pretty way to print correctly dcl_temps operation,
	// so wrote greasy code path
	os << sdInst.m_sdOperation << " ";
	const UINT* pData = &sdInst.m_data[0];
	if (sdInst.m_sdOperation.isDeclaration())
	{
		switch( sdInst.m_sdOperation.getType() )
		{
		case D3D10_SB_OPCODE_DCL_RESOURCE:
			os << ResourceDeclarationToString( pData );
			break;
		case D3D10_SB_OPCODE_DCL_CONSTANT_BUFFER:
			os << ConstantBufferDeclarationToString( pData );
			break;
		case D3D10_SB_OPCODE_DCL_SAMPLER:
			os << SamplerDeclarationToString( pData );
			break;
		case D3D10_SB_OPCODE_DCL_INDEX_RANGE:
			os << IndexRangeDeclarationToString( pData );
			break;
		case D3D10_SB_OPCODE_DCL_GS_OUTPUT_PRIMITIVE_TOPOLOGY:
			os << GeometryShaderOutputPrimitiveTopologyDeclarationToString( pData );
			break;
		case D3D10_SB_OPCODE_DCL_GS_INPUT_PRIMITIVE:
			os << GeometryShaderInputPrimitiveDeclarationToString( pData );
			break;
		case D3D10_SB_OPCODE_DCL_INPUT:
			os << InputOutputDeclarationToString( pData );
			break;
		case D3D10_SB_OPCODE_DCL_INPUT_SGV:
			os << InputOutputSgvSivDeclarationToString( pData );
			break;
		case D3D10_SB_OPCODE_DCL_INPUT_SIV:
			os << InputOutputSgvSivDeclarationToString( pData );
			break;
		case D3D10_SB_OPCODE_DCL_INPUT_PS:
			os << PixelShaderInputDeclarationToStirng( pData );
			break;
		case D3D10_SB_OPCODE_DCL_INPUT_PS_SGV:
			os << InputOutputSgvSivDeclarationToString( pData );
			break;
		case D3D10_SB_OPCODE_DCL_INPUT_PS_SIV:
			os << PixelShaderInputSivDeclarationToString( pData, (UINT)sdInst.GetLength() );
			break;
		case D3D10_SB_OPCODE_DCL_OUTPUT:
			os << InputOutputDeclarationToString( pData );
		case D3D10_SB_OPCODE_DCL_OUTPUT_SGV:
		case D3D10_SB_OPCODE_DCL_OUTPUT_SIV:
			os << InputOutputSgvSivDeclarationToString( pData );
			break;
		case D3D10_SB_OPCODE_DCL_MAX_OUTPUT_VERTEX_COUNT:
		case D3D10_SB_OPCODE_DCL_TEMPS:
		case D3D11_SB_OPCODE_DCL_GS_INSTANCE_COUNT:
			assert( sdInst.m_data.size() == 2 );
			os << sdInst.m_data[1];
			break;
		case D3D10_SB_OPCODE_DCL_INDEXABLE_TEMP:
			os << IndexableTempDeclarationToString( pData );
			break;
		case D3D10_SB_OPCODE_DCL_GLOBAL_FLAGS:
			os << GlobalFlagsDeclarationToString( pData );
			break;
		case D3D11_SB_OPCODE_DCL_STREAM:
			break;
		case D3D11_SB_OPCODE_DCL_FUNCTION_BODY:
			break;
		case D3D11_SB_OPCODE_DCL_FUNCTION_TABLE:
			break;
		case D3D11_SB_OPCODE_DCL_INTERFACE:
			break;
		case D3D11_SB_OPCODE_DCL_INPUT_CONTROL_POINT_COUNT:
		case D3D11_SB_OPCODE_DCL_OUTPUT_CONTROL_POINT_COUNT:
		case D3D11_SB_OPCODE_DCL_HS_FORK_PHASE_INSTANCE_COUNT:
		case D3D11_SB_OPCODE_DCL_HS_JOIN_PHASE_INSTANCE_COUNT:
			os << ControlPointCountToString( pData );
			break;
		case D3D11_SB_OPCODE_DCL_TESS_DOMAIN:
			os << TessDomainDeclarationToString( pData );
			break;
		case D3D11_SB_OPCODE_DCL_TESS_PARTITIONING:
			os << TessPartitioningDeclarationToString( pData );
			break;
		case D3D11_SB_OPCODE_DCL_TESS_OUTPUT_PRIMITIVE:
			os << TessOutputPrimitiveDeclarationToString( pData );
			break;
		case D3D11_SB_OPCODE_DCL_HS_MAX_TESSFACTOR:
			os << HSMaxTessFactorToString( pData );
			break;
		case D3D11_SB_OPCODE_DCL_THREAD_GROUP:
			os << ThreadGroupDeclarationToString( pData );
			break;
		case D3D11_SB_OPCODE_DCL_UNORDERED_ACCESS_VIEW_TYPED:
			os << UnorderedAccessViewTypedDeclarationToString( pData );
			break;
		case D3D11_SB_OPCODE_DCL_UNORDERED_ACCESS_VIEW_RAW:
			break;
		case D3D11_SB_OPCODE_DCL_UNORDERED_ACCESS_VIEW_STRUCTURED:
			break;
		case D3D11_SB_OPCODE_DCL_THREAD_GROUP_SHARED_MEMORY_RAW:
			break;
		case D3D11_SB_OPCODE_DCL_THREAD_GROUP_SHARED_MEMORY_STRUCTURED:
			break;
		case D3D11_SB_OPCODE_DCL_RESOURCE_RAW:
			break;
		case D3D11_SB_OPCODE_DCL_RESOURCE_STRUCTURED:
			break;
		}
	}
	else
	{
		bool bAddComma = false;
		for( auto i = sdInst.m_operands.begin();
			i != sdInst.m_operands.end(); i++)
		{
			if (bAddComma)
				os << ", " << (*i);
			else
			{
				os << " " << (*i);
				bAddComma = true;
			}
		}
	}
	return os;
}

void SetIstreamAfterSHDRtoken(istream& iStream)
{
	const unsigned token = MAKEFOURCC('S', 'H', 'D', 'R');
	unsigned curToken;
	while(!iStream.eof())
	{
		iStream.read((char*)&curToken, sizeof(unsigned));
		if(curToken == token)
		{
			return;
		}
	}
}

unsigned GetShaderCodeSize( istream& iStream )
{
	SetIstreamAfterSHDRtoken(iStream);
	if(iStream.eof())
	{
		iStream.seekg(0);
		return 0;
	}
	unsigned token;
	iStream.read((char*)&token, sizeof(unsigned));
	iStream.seekg(0);
	return token;
}

void shader_analyzer::GetShaderCode( istream& iStream, ostream& oStream )
{
	SetIstreamAfterSHDRtoken(iStream);
	if(iStream.eof())
	{
		iStream.seekg(0);
		return;
	}
	unsigned size;
	iStream.read((char*)&size, sizeof(unsigned));
	char* buf = DNew char[size];
	iStream.read(buf, size);
	oStream.write(buf, size);
	iStream.seekg(0);
	delete []buf;
	return;
}

std::string shader_analyzer::SamplerDeclarationToString( const UINT* pData )
{
	std::string strToReturn;
	switch( DECODE_D3D10_SB_SAMPLER_MODE( pData[0] ) )
	{
	case D3D10_SB_SAMPLER_MODE_DEFAULT:
		strToReturn += "dflt ";
		break;
	case D3D10_SB_SAMPLER_MODE_COMPARISON:
		strToReturn += "cmpr ";
		break;
	case D3D10_SB_SAMPLER_MODE_MONO:
		strToReturn += "mono ";
		break;
	default:
		break;
	}
	shader_analyzer::ShaderOperand op( const_cast<UINT*>( &pData[1] ) );
	strToReturn += op.toString();
	return strToReturn;
}

std::string shader_analyzer::TessOutputPrimitiveDeclarationToString( const UINT* pData )
{
	switch( DECODE_D3D11_SB_TESS_OUTPUT_PRIMITIVE( pData[0] ) ){
	case D3D11_SB_TESSELLATOR_OUTPUT_UNDEFINED:
		return "tess output primitive undefined";
	case D3D11_SB_TESSELLATOR_OUTPUT_POINT:
		return "dcl tess output point";
	case D3D11_SB_TESSELLATOR_OUTPUT_LINE:
		return "dcl tess output line";
	case D3D11_SB_TESSELLATOR_OUTPUT_TRIANGLE_CW:
		return "dcl tess output triangle cw";
	case D3D11_SB_TESSELLATOR_OUTPUT_TRIANGLE_CCW:
		return "dcl tess output triangle ccw";
	default:
		return "";
	}
}

std::string shader_analyzer::ThreadGroupDeclarationToString( const UINT* pData )
{
	// ----------------------------------------------------------------------------
	// Thread Group Declaration (Compute Shader)
	//
	// OpcodeToken0:
	//
	// [10:00] D3D11_SB_OPCODE_DCL_THREAD_GROUP
	// [23:11] Ignored, 0
	// [30:24] Instruction length in DWORDs including the opcode token.
	// [31]    0 normally. 1 if extended operand definition, meaning next DWORD
	//         contains extended operand description.  If it is extended, then
	//         it contains the actual instruction length in DWORDs, since
	//         it may not fit into 7 bits if enough types are used.
	//
	// OpcodeToken0 is followed by 3 DWORDs, the Thread Group dimensions as UINT32:
	// x, y, z
	//
	// ----------------------------------------------------------------------------
	std::string toRet = "thread group x[";
	toRet += boost::lexical_cast<std::string>( pData[1] );
	toRet += "], y[";
	toRet += boost::lexical_cast<std::string>( pData[2] );
	toRet += "], z[";
	toRet += boost::lexical_cast<std::string>( pData[3] );
	toRet += "]";
	return toRet;
}

std::string shader_analyzer::UnorderedAccessViewTypedDeclarationToString( const UINT* pData )
{
	// ----------------------------------------------------------------------------
	// Typed Unordered Access View Declaration
	//
	// OpcodeToken0:
	//
	// [10:00] D3D11_SB_OPCODE_DCL_UNORDERED_ACCESS_VIEW_TYPED
	// [15:11] D3D10_SB_RESOURCE_DIMENSION
	// [16:16] D3D11_SB_GLOBALLY_COHERENT_ACCESS or 0 (LOCALLY_COHERENT)
	// [23:17] Ignored, 0
	// [30:24] Instruction length in DWORDs including the opcode token.
	// [31]    0 normally. 1 if extended operand definition, meaning next DWORD
	//         contains extended operand description.  This dcl is currently not
	//         extended.
	//
	// OpcodeToken0 is followed by 2 operands:
	// (1) an operand, starting with OperandToken0, defining which
	//     u# register (D3D11_SB_OPERAND_TYPE_UNORDERED_ACCESS_VIEW) is being declared.
	// (2) a Resource Return Type token (ResourceReturnTypeToken)
	//
	// ----------------------------------------------------------------------------
	std::string toRet = "dcl unord access view typed [";
	switch( DECODE_D3D10_SB_RESOURCE_DIMENSION( pData[0] ) ){
	case D3D10_SB_RESOURCE_DIMENSION_UNKNOWN:
		toRet += "unknown";
		break;
	case D3D10_SB_RESOURCE_DIMENSION_BUFFER:
		toRet += "buf";
		break;
	case D3D10_SB_RESOURCE_DIMENSION_TEXTURE1D:
		toRet += "1D";
		break;
	case D3D10_SB_RESOURCE_DIMENSION_TEXTURE2D:
		toRet += "2D";
		break;
	case D3D10_SB_RESOURCE_DIMENSION_TEXTURE2DMS:
		toRet += "2DMS";
		break;
	case D3D10_SB_RESOURCE_DIMENSION_TEXTURE3D:
		toRet += "3D";
		break;
	case D3D10_SB_RESOURCE_DIMENSION_TEXTURECUBE:
		toRet += "Cube";
		break;
	case D3D10_SB_RESOURCE_DIMENSION_TEXTURE1DARRAY:
		toRet += "1DArray";
		break;
	case D3D10_SB_RESOURCE_DIMENSION_TEXTURE2DARRAY:
		toRet += "2DArray";
		break;
	case D3D10_SB_RESOURCE_DIMENSION_TEXTURE2DMSARRAY:
		toRet += "2DMSArray";
		break;
	case D3D10_SB_RESOURCE_DIMENSION_TEXTURECUBEARRAY:
		toRet += "CubeArray";
		break;
	default:
		return "";
	}
	toRet += "] ";
	if ( DECODE_D3D11_SB_ACCESS_COHERENCY_FLAGS( pData[0] ) ){
		toRet += "globally coherent access ";
	}
	else{
		toRet += "locally coherent ";
	}
	ShaderOperand Op( const_cast<unsigned int*>( &pData[1] ) );
	toRet += Op.toString();
	return toRet;
}

//////////////////////////////////////////////////////////////////////////
// Empty ctor
ShaderInstruction::ShaderInstruction():m_sdOperation(0)
{
}

//////////////////////////////////////////////////////////////////////////
// copy ctor
ShaderInstruction::ShaderInstruction(const ShaderInstruction& shInst) :
m_data(shInst.m_data), m_sdOperation(&m_data[0])
{
	ParseInstruction();
}

//////////////////////////////////////////////////////////////////////////
// parses instruction to operation and operands
void ShaderInstruction::ParseInstruction()
{
	m_operands.clear();
	m_sdOperation = ShaderOperation(&m_data[0]);
	unsigned currIndex = m_sdOperation.getLength();
	auto operType = m_sdOperation.getType();
	if (m_sdOperation.isDeclaration() || operType == D3D10_SB_OPCODE_BREAKC)
		return;
	while(currIndex < m_data.size())
	{
		ShaderOperand op(&m_data[currIndex]);
		m_operands.push_back(op);
		currIndex += op.getLength();
	}
}

const ShaderOperation* ShaderInstruction::getOperation() const
{
	return &m_sdOperation;
}

size_t ShaderInstruction::getOperandsCount() const
{
	return m_operands.size();
}

const ShaderOperand* ShaderInstruction::getOperand( const unsigned index ) const
{
	_ASSERT(!m_sdOperation.isDeclaration());
	return &( m_operands[index] );
}

string ShaderInstruction::toString() const
{
	char b[255];
	ostrstream a( b, sizeof( b ) );
	SecureZeroMemory( b, sizeof( b ) );
	a << *this;
	return b;
}

ShaderOperandEdge ShaderInstruction::getComponentsEffectOnGiven(
	const unsigned inComponents,
	const unsigned operandIndex ) const
{
	if( getOperandsCount() < 2)
		return ShaderOperandEdge( 0 );
	if( operandIndex < 1 )
		return ShaderOperandEdge( 0 );
	const ShaderOperand* pFirstOperand = getOperand( 0 );
	const ShaderOperand* pGivenOperand = getOperand( operandIndex );
	ShaderOperandEdge retOpEdge( pGivenOperand );
	unsigned retComponents( 0 );
	for( unsigned i = 0; i < 4; i++ )
	{
		ShaderOperand::EComponentState state = ( ShaderOperand::EComponentState )( 1 << i );
		if( pFirstOperand->getComponentFormGiven( state & inComponents ) )
			retComponents |= m_sdOperation.getDependentComponents( state, pGivenOperand );
	}
	retOpEdge.setComponentState( retComponents );
	return retOpEdge;
}

ShaderInstruction& ShaderInstruction::operator = ( const ShaderInstruction& shInst )
{
	m_data = shInst.m_data;
	m_sdOperation = &m_data[0];
	ParseInstruction();
	return *this;
}

shader_analyzer::ShaderInstruction::ShaderInstruction( const ShaderOperation& /*shOperation*/ )
{
	// TODO
}

shader_analyzer::ShaderInstruction::ShaderInstruction( const std::string& /*instrStr*/ )
{
	// TODO
}

shader_analyzer::ShaderInstruction::ShaderInstruction( const unsigned* data, const size_t sizeInBytes )
{
	std::istrstream shaderStream( (char*)data, (int)sizeInBytes );
	shaderStream >> *this;
}

void shader_analyzer::ShaderInstruction::GetBlob( std::vector<UINT>&retData ) const
{
	retData.clear();
	retData.assign( m_data.begin(), m_data.end() );
}
