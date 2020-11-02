#include "stdafx.h"
#include "GraphAnalysis.h"

#pragma warning ( push )
#pragma warning ( disable: 4100 )
#pragma warning ( disable: 4503 )
#pragma warning ( disable: 4510 )

#include <boost/graph/breadth_first_search.hpp>
#pragma warning ( pop )
#include "ShaderInstruction.h"
#include "SearchProjectMatrixBFSVisitor.h"

using namespace shader_analyzer;
using namespace std;
using namespace boost;

template < ShaderOperand::EComponentState state >
class IsNeededState
{
	const TGraph* pGr_;
public:
	IsNeededState( const TGraph* pGr ) : pGr_( pGr ){}
	bool operator () ( TStartVerticesArray::value_type v )
	{
		using namespace boost;
		const ShaderInstruction* pInstr = get( vertex_index, *pGr_)[v];
		if ( ( pInstr->getOperand( 0 )->getComponentState() & state ) != 0 )
			return true;
		else 
			return false;
	}
};

typedef map< graph_traits< TGraph >::vertex_descriptor,
default_color_type > TColorMap;

void shader_analyzer::FindProjectionMatrixFromGraph( const TGraph& gr,
													const TStartVerticesArray& arr,
													ProjectionShaderCBs& matList )
{
	// matrix to return
	ProjectionMatrixData retData;
	// shader instruction as vertex index
	TConstVertexIndexMap vCmap = get( vertex_index, gr );
	// start vertices
	TStartVerticesArray::const_iterator Xv, Wv;

	// start vertex with x-component
	Xv = find_if( arr.begin(), arr.end(), IsNeededState< ShaderOperand::X_STATE >( &gr ) );
	if ( Xv == arr.end() )
		return;	// if there is no x-component, there is no projection matrix

	// start vertex with w-component
	Wv = find_if( arr.begin(), arr.end(), IsNeededState< ShaderOperand::W_STATE >( &gr ) );
	if ( Wv == arr.end() )
		return;	// if there is no w-component, there is no projection matrix

	// visitor to find projection matrix with breadth search algorithm
	SearchProjectMatrixBFSVisitor scbX( gr, *Xv, ShaderOperand::X_STATE );
	// find valuable operation with x-component
	breadth_first_visit( gr, *Xv, visitor( scbX ) );
	if( !scbX.getInstruction() )
		return;	// if there is no x-component valuable operation, there is no projection matrix
	const ShaderOperand* pOperand = scbX.getOperand();
	if ( pOperand == NULL )
		return;
	if ( pOperand->getOperandIndexRepresentation( D3D10_SB_OPERAND_INDEX_1D )
		!= D3D10_SB_OPERAND_INDEX_IMMEDIATE32 )
		return;	// if second index is not immediate, there is no projection matrix

	// first index of matrix constant buffer
	unsigned firstIndex = pOperand->getIndex( 0 );

	// second index of matrix constant buffer
	retData.matrixRegister = pOperand->getIndex( 1 );

	

	if ( Xv == Wv )	// if x- and w- components are the same vertices
	{
		switch ( scbX.getInstruction()->getOperation()->getType() )
		{
		case D3D10_SB_OPCODE_DP4:
			retData.matrixIsTransposed = true;
			retData.incorrectProjection = false;
			break;
		case D3D10_SB_OPCODE_DP3:
			retData.matrixIsTransposed = true;
			retData.incorrectProjection = true;
			break;
		case D3D10_SB_OPCODE_MUL:
		case D3D10_SB_OPCODE_MAD:
		case D3D10_SB_OPCODE_ADD:
			retData.matrixIsTransposed = false;
			retData.incorrectProjection = false;
			switch( scbX.getOperand()->getComponentState() )
			{
			case ShaderOperand::X_STATE:
				break;
			case ShaderOperand::Y_STATE:
				if (retData.matrixRegister > 0)
					retData.matrixRegister -= 1;
				break;
			case ShaderOperand::Z_STATE:
				if (retData.matrixRegister > 1)
					retData.matrixRegister -= 2;
				break;
			case ShaderOperand::W_STATE:
				if (retData.matrixRegister > 2)
					retData.matrixRegister -= 3;
				break;
			}
			break;
		default:
			return;
		}
		matList.cb[firstIndex].push_back( retData );
		matList.mask = 1 << firstIndex;
		return;
	}

	switch( scbX.getInstruction()->getOperation()->getType() )
	{
	case D3D10_SB_OPCODE_DP4:
	case D3D10_SB_OPCODE_DP3:
		retData.matrixIsTransposed = true;
		break;
	case D3D10_SB_OPCODE_MUL:
	case D3D10_SB_OPCODE_MAD:
	case D3D10_SB_OPCODE_ADD:
		retData.matrixIsTransposed = false;
		switch( scbX.getOperand()->getComponentState() )
		{
		case ShaderOperand::X_STATE:
			break;
		case ShaderOperand::Y_STATE:
			if (retData.matrixRegister > 0)
				retData.matrixRegister -= 1;
			break;
		case ShaderOperand::Z_STATE:
			if (retData.matrixRegister > 1)
				retData.matrixRegister -= 2;
			break;
		case ShaderOperand::W_STATE:
			if (retData.matrixRegister > 2)
				retData.matrixRegister -= 3;
			break;
		default:
			return;
		}
		break;
	default:
		return;
	}

	SearchProjectMatrixBFSVisitor scbW( gr, *Wv, ShaderOperand::W_STATE );
	breadth_first_visit( gr, *Wv, visitor( scbW ) );
	if( !scbW.getInstruction() )
	{
		retData.incorrectProjection = true;
	}
	else
	{
		switch( scbW.getInstruction()->getOperation()->getType() )
		{
		case D3D10_SB_OPCODE_DP4:
		case D3D10_SB_OPCODE_DP3:
		case D3D10_SB_OPCODE_MUL:
		case D3D10_SB_OPCODE_MAD:
			retData.incorrectProjection = false;
			break;
		default:
			retData.incorrectProjection = true;
			break;
		}
	}
	matList.cb[firstIndex].push_back( retData );
	matList.mask |= 1 << firstIndex;
}

unsigned FindOutputRegisterComponent( const TShaderList& shList,
									 ShaderOperand::EComponentState state,
									 unsigned outputRegisterIndex )
{
	D3D10_SB_OPERAND_TYPE type = D3D10_SB_OPERAND_TYPE_OUTPUT;
	unsigned reg = outputRegisterIndex;
	unsigned movImm = INVALID_REGISTER;
	for(int i = (int)shList.size() - 1; i > 0; i-- )
	{
#ifdef _DEBUG
		std::string strInstr = shList[i].toString();
#endif

		if( shList[i].getOperandsCount() < 2 )
			continue;
		const ShaderOperand* pBufOperand = shList[i].getOperand( 0 );
		if( pBufOperand->getOperandType() != type )
			continue;
		if( pBufOperand->getIndex( 0 ) != reg )
			continue;
		if( !( pBufOperand->getComponentState() & state) )
			continue;
		D3D10_SB_OPCODE_TYPE opType = shList[i].getOperation()->getType();
		if (opType == D3D10_SB_OPCODE_MOV)
		{
			const ShaderOperand* pSrcOperand = shList[i].getOperand( 1 );
			// for BF BC 2
			if( pSrcOperand->getOperandType() == D3D10_SB_OPERAND_TYPE_IMMEDIATE32 )
				continue;
			if( pSrcOperand->getOperandType() == D3D10_SB_OPERAND_TYPE_IMMEDIATE64 )
				continue;
			// skip move from temp register
			if( pSrcOperand->getOperandType() == D3D10_SB_OPERAND_TYPE_TEMP )
			{
				type = D3D10_SB_OPERAND_TYPE_TEMP;
				reg = pSrcOperand->getIndex(0);
				state = (ShaderOperand::EComponentState)pSrcOperand->getComponentState(state);
				movImm = (unsigned)i;
				continue;
			}
		}
		else if (opType == D3D10_SB_OPCODE_MOVC)
		{
			const ShaderOperand* pSrcOperand = shList[i].getOperand( 3 );
			// HACK: for Just Cause 2 & RE 5
			// TODO: Disable that and try search 2 matrices (operand 2 and 3)
			if( pSrcOperand->getOperandType() == D3D10_SB_OPERAND_TYPE_IMMEDIATE32 )
				continue;
			if( pSrcOperand->getOperandType() == D3D10_SB_OPERAND_TYPE_IMMEDIATE64 )
				continue;
			// skip move from temp register
			if( pSrcOperand->getOperandType() == D3D10_SB_OPERAND_TYPE_TEMP )
			{
				type = D3D10_SB_OPERAND_TYPE_TEMP;
				reg = pSrcOperand->getIndex(0);
				state = (ShaderOperand::EComponentState)pSrcOperand->getComponentState(state);
				movImm = (unsigned)i;
				continue;
			}
		}
		return (unsigned)i;
	}
	if (movImm != INVALID_REGISTER)
		return movImm;
	return INVALID_REGISTER;
}

bool GetProjectionMatrix(const TShaderList& shList, 
	DWORD i, ShaderOperand::ShaderRegister &curRetInstr, ShaderOperand::ShaderRegister &curRetCB, MatrixDataWithCB<ProjectionMatrixData> &curmd );

void shader_analyzer::FindProjectionMatrixFromShader(
	const TShaderList& shList,
	unsigned outputRegisterIndex,
	ProjectionShaderCBs& matList )
{
	// TODO: If we can't find matrix, return input vector index with position
	// matrix to return
	ProjectionMatrixData retData;

	ShaderGraph shGr( shList );
	unsigned componentIndex[4];
	ShaderOperand::ShaderRegister retInstr[4];
	ShaderOperand::ShaderRegister retCB[4];
	// find component output
	for (int i = 0; i < 4; i++)
	{
		ShaderOperand::EComponentState cs = (ShaderOperand::EComponentState)(1 << i);
		componentIndex[i] = FindOutputRegisterComponent(
			shList,	cs, outputRegisterIndex );
		if( componentIndex[i] == INVALID_REGISTER )
			return;

		shGr.ClearEdgesArray();
		retInstr[i] = shGr.GetPreviousInstruction(
			componentIndex[i], cs, 
			[] (const TShaderList& /*shList*/, unsigned /*startIndex*/, unsigned& /*opIndex*/) { return true; } );

		if( retInstr[i].commandIndex == INVALID_REGISTER )
		{
			retCB[i].commandIndex = INVALID_REGISTER;
			if (i == 2)
				continue;
			else
				return;
		}

#ifdef _DEBUG
		std::string strInstr = shList[retInstr[i].commandIndex].toString();
#endif

		retCB[i] = shGr.GetPreviousInstruction(
			retInstr[i].commandIndex, retInstr[i].mask, 
			[] (const TShaderList& shList, unsigned startIndex, unsigned& opIndex) -> bool { 
				opIndex = GetConstantBufferOperandIndex( &shList[startIndex] );
				return opIndex != 0; 
		} );

		if( retCB[i].commandIndex == INVALID_REGISTER )
			return;

#ifdef _DEBUG
		std::string strCB = shList[retCB[i].commandIndex].toString();
#endif
	}

	typedef pair<bool, MatrixDataWithCB<ProjectionMatrixData>> ComponentPair;
	vector<ComponentPair> shaderData(4);
	for (int i = 0; i < 4; i++)
	{
		shaderData[i].first = GetProjectionMatrix(shList, i, retInstr[i], retCB[i], shaderData[i].second);
	}
	
	if (!shaderData[0].first)
		return;
	
	int maxFreqCount = 0;
	int maxFreqInd = 0;
	for (int i = 0; i < 4; i++)
	{
		if(shaderData[i].first)
		{
			int freqCount = 1;
			for(int j = 0; j < 4; j++)
			{
				if(shaderData[j].first)
				{
					if (shaderData[i].second.constantBuffer == shaderData[j].second.constantBuffer &&
						shaderData[i].second.md.matrixRegister == shaderData[j].second.md.matrixRegister)
						freqCount++;
				}
			}
			if(freqCount >= maxFreqCount)
			{
				maxFreqCount = freqCount;
				maxFreqInd = i;
			}
		}
	}
	if(maxFreqCount < 2)
		return;

	unsigned firstIndex = shaderData[maxFreqInd].second.constantBuffer;
	retData = shaderData[maxFreqInd].second.md;

	if( componentIndex[3] == INVALID_REGISTER )
	{
		retData.incorrectProjection = true;
	}
	else
	{
		D3D10_SB_OPCODE_TYPE WopType = shList[componentIndex[3]].getOperation()->getType();
		switch( WopType )
		{
		case D3D10_SB_OPCODE_DP4:
		case D3D10_SB_OPCODE_DP3:
		case D3D10_SB_OPCODE_MUL:
		case D3D10_SB_OPCODE_MAD:
		case D3D10_SB_OPCODE_ADD:
			retData.incorrectProjection = false;
			break;
		default:
			retData.incorrectProjection = true;
			break;
		}
	}

	matList.cb[firstIndex].push_back( retData );
	matList.mask = 1 << firstIndex;
}

bool GetProjectionMatrix( const TShaderList& shList,
	DWORD i, ShaderOperand::ShaderRegister &curRetInstr, ShaderOperand::ShaderRegister &curRetCB, MatrixDataWithCB<ProjectionMatrixData> &curmd )
{
	if( curRetInstr.commandIndex == INVALID_REGISTER )
		return false;

	auto& retInstr = shList[curRetInstr.commandIndex];
#ifdef _DEBUG
	std::string strInstr = retInstr.toString();
#endif
	D3D10_SB_OPCODE_TYPE opType = retInstr.getOperation()->getType();

	auto& retCBInstr = shList[curRetCB.commandIndex];
#ifdef _DEBUG
	std::string strCB = retCBInstr.toString();
#endif
	const ShaderOperand* op = retCBInstr.getOperand( curRetCB.operandIndex );
	curmd.constantBuffer = op->getIndex( 0 );
	curmd.md.matrixRegister = op->getIndex( 1 );	

	switch( opType )
	{
	case D3D10_SB_OPCODE_DP4:
	case D3D10_SB_OPCODE_DP3:
		curmd.md.matrixIsTransposed = true;
		if (curmd.md.matrixRegister >= i)
		{
			curmd.md.matrixRegister -= i;
			return true;
		}
		return false;
	case D3D10_SB_OPCODE_MUL:
	case D3D10_SB_OPCODE_MAD:
	case D3D10_SB_OPCODE_ADD:
		curmd.md.matrixIsTransposed = false;
		ShaderOperand::EComponentState cs = (ShaderOperand::EComponentState)op->getComponentState();
		if( cs == ShaderOperand::ALL_COMPONENTS )
		{
			if (curmd.md.matrixRegister >= 3)
			{
				curmd.md.matrixRegister -= 3;
			}
			return true;
		}
		break;
	}
	return false;
}

void shader_analyzer::GetUsedResourcesMask( const TShaderList& shList, std::bitset<128> &mask )
{
	mask.reset();
	for(size_t i = 0; i < shList.size(); i++ )
	{
		auto& instruction = shList[i];
		auto operation = instruction.getOperation();
		if( !operation->isDeclaration() )
			break;

#ifdef _DEBUG
		std::string strInstr = instruction.toString();
#endif

		const UINT* pData = instruction.GetData().data();
		if ( operation->getType() ==  D3D10_SB_OPCODE_DCL_RESOURCE || 
			operation->getType() ==  D3D11_SB_OPCODE_DCL_RESOURCE_RAW || 
			operation->getType() ==  D3D11_SB_OPCODE_DCL_RESOURCE_STRUCTURED )
		{
			shader_analyzer::ShaderOperand op( const_cast<UINT*>(&pData[1]));
			if (op.getOperandIndexDimension() == 1)
			{
				if (op.getOperandIndexRepresentation(D3D10_SB_OPERAND_INDEX_0D) == D3D10_SB_OPERAND_INDEX_IMMEDIATE32)
				{
					DWORD tex = op.getIndex(0);
					mask.set(tex);
				}
			}
		}
	}
}