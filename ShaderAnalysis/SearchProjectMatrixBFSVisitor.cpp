#include "StdAfx.h"
#include "SearchProjectMatrixBFSVisitor.h"

using namespace boost;
using namespace std;
using namespace shader_analyzer;
//////////////////////////////////////////////////////////////////////////
// static members
const ShaderInstruction* SearchProjectMatrixBFSVisitor::pInst_( 0 );
const ShaderOperand* SearchProjectMatrixBFSVisitor::pOperand_( 0 );

//////////////////////////////////////////////////////////////////////////
// ctor
SearchProjectMatrixBFSVisitor::SearchProjectMatrixBFSVisitor(
	const TGraph& gr,										// graph to analyze
	graph_traits< TGraph >::vertex_descriptor startVertex,	// start vertex
	const unsigned startComponents )						// start components
	: boost::default_bfs_visitor(),
	edgeMap_( get( edge_index, gr ) ),
	vertMap_( get( vertex_index, gr ) ),
	gr_( gr )
{
	// initialize static member variables
	pInst_ = 0;
	pOperand_ = 0;

	// get shader instruction of the start vertex
	const ShaderInstruction* pBufInst = vertMap_[startVertex];

	// through all operands of start vertex
	for( unsigned i = 1; i < pBufInst->getOperandsCount(); i++ )
	{
		// get i-th operand that effects on given first operand components
		ShaderOperandEdge bufOpEdge = pBufInst->getComponentsEffectOnGiven( startComponents, i );

#ifdef _DEBUG
		string debStr = bufOpEdge.toString();
#endif
		// if i-th operand effects on given first operand components
		if( bufOpEdge.getComponentState() )
			frontSet.insert( bufOpEdge );	// add operand edge to front set

		// get operand from instruction as is
		const ShaderOperand* pBufOperand = pBufInst->getOperand( i );
		if ( pBufOperand->getOperandType() != D3D10_SB_OPERAND_TYPE_CONSTANT_BUFFER )
			continue;
		for( int j = 0; j < pBufOperand->getOperandIndexDimension(); j++ )
		{
			if( pBufOperand->getOperandIndexRepresentation( ( D3D10_SB_OPERAND_INDEX_DIMENSION )j )
				!= D3D10_SB_OPERAND_INDEX_IMMEDIATE32 )
				continue;
		}
	}

	if( !IsInstructionIsValuable( pBufInst ) )
		return;
	pInst_ = pBufInst;
	unsigned ind = GetConstantBufferOperandIndex( pBufInst );
	if( !ind )
		return;
	pOperand_ = pInst_->getOperand( ind );
}

//////////////////////////////////////////////////////////////////////////
// main function for breadth first visit function
void SearchProjectMatrixBFSVisitor::examine_edge(
	graph_traits< TGraph >::edge_descriptor e,
	const TGraph& g ) 
{
	// get operand from edge of graph
	ShaderOperandEdge OpEdgeFromGraph = edgeMap_[e];
	
#ifdef _DEBUG
	string debStr;
	debStr = OpEdgeFromGraph.toString();
	TGraph::vertex_descriptor stV = source( e, g );
	const ShaderInstruction* bbInst = vertMap_[stV];
	debStr = bbInst->toString();
	stV = target( e, g );
	bbInst = vertMap_[stV];
	debStr = bbInst->toString();
#endif
	TFrontSet::iterator fIt = frontSet.find( OpEdgeFromGraph );
	if( fIt == frontSet.end() )
		return;	// if current operand-edge is not in front set, return

	// operand-edge form front set
	ShaderOperandEdge OpEdgeFromFront = *fIt;

#ifdef _DEBUG
	debStr = OpEdgeFromFront.toString();
#endif

	// edge from graph and edge from front set common components
	unsigned commonComponents = OpEdgeFromFront.getComponentState() &
		OpEdgeFromGraph.getComponentState();
	if( !commonComponents )
		return;
	unsigned restComp = OpEdgeFromFront.getComponentState() &
		~OpEdgeFromGraph.getComponentState();
	if ( !restComp )	// if needed component is fully utilized
	{
		frontSet.erase( fIt );
	}
	else
	{
		// change needed components
		const_cast<TFrontSet::iterator::value_type&>(*fIt).setComponentState(restComp);
	}
	// add new front data to frontList
	// instruction 
	const ShaderInstruction* pTargetInstruction = vertMap_[ target( e, gr_ )];

#ifdef _DEBUG
	debStr = pTargetInstruction->toString();
#endif

	for( unsigned i = 1; i < pTargetInstruction->getOperandsCount(); i++ )
	{
		ShaderOperandEdge inputEdge =
			pTargetInstruction->getComponentsEffectOnGiven( commonComponents, i );

#ifdef _DEBUG
		debStr = inputEdge.toString();
#endif

		if( inputEdge.getComponentState() )
			frontSet.insert( inputEdge );
	}

	if( pInst_ )
	{
		if( pOperand_ )
			return;
		unsigned ind = GetConstantBufferOperandIndex( pTargetInstruction );
		if( !ind )
			return;
		pOperand_ = pTargetInstruction->getOperand( ind );
		return;
	}

	if( !IsInstructionIsValuable( pTargetInstruction ) )
		return;
	pInst_ = pTargetInstruction;
	// restart front
	frontSet.clear();
	for( unsigned i = 1; i < pTargetInstruction->getOperandsCount(); i++ )
	{
		ShaderOperandEdge inputEdge =
			pTargetInstruction->getComponentsEffectOnGiven( commonComponents, i );

#ifdef _DEBUG
		debStr = inputEdge.toString();
#endif

		if( inputEdge.getComponentState() )
			frontSet.insert( inputEdge );
	}
	unsigned ind = GetOperandIndexIfInstructionIsValuable( pTargetInstruction );
	if( ind )
		pOperand_ = pTargetInstruction->getOperand( ind );
}

unsigned GetOperandIndexIfInstructionIsValuable( const ShaderInstruction* pInst )
{
	if( !IsInstructionIsValuable( pInst ) )
		return 0;

	return GetConstantBufferOperandIndex( pInst );
}

bool IsInstructionIsValuable( const shader_analyzer::ShaderInstruction* pInst )
{
	if ( pInst->getOperandsCount()  < 2 )
		return false;
	D3D10_SB_OPCODE_TYPE opType = pInst->getOperation()->getType();
	switch( opType )
	{
		// valuable operation types
	case D3D10_SB_OPCODE_DP4:
	case D3D10_SB_OPCODE_DP3:
	case D3D10_SB_OPCODE_DP2:
	case D3D10_SB_OPCODE_MUL:
	case D3D10_SB_OPCODE_MAD:
	case D3D10_SB_OPCODE_ADD:
		break;
	default:
		return false;
	}
	unsigned ConstantBuffersInOperands( 0 );
	for( unsigned i = 1; i < pInst->getOperandsCount(); ++i )
	{
		if( pInst->getOperand( i )->getOperandType() == D3D10_SB_OPERAND_TYPE_CONSTANT_BUFFER )
			++ConstantBuffersInOperands;
		if( ConstantBuffersInOperands > 1 )
			return false;
	}
	return true;
}


unsigned GetConstantBufferOperandIndex( const shader_analyzer::ShaderInstruction* pInst )
{
	unsigned CBIndex( 0 );

	// if there is cb once in operands
	for( unsigned i = 1; i < pInst->getOperandsCount(); ++i )
	{
		const ShaderOperand* pBufOperand = pInst->getOperand( i );
		if( pBufOperand->getOperandType() != D3D10_SB_OPERAND_TYPE_CONSTANT_BUFFER )
			continue;
		if ( pBufOperand->getOperandIndexRepresentation(
			( D3D10_SB_OPERAND_INDEX_DIMENSION ) 0 ) != D3D10_SB_OPERAND_INDEX_IMMEDIATE32 )
			continue;
		if ( pBufOperand->getOperandIndexRepresentation(
			( D3D10_SB_OPERAND_INDEX_DIMENSION ) 1 ) != D3D10_SB_OPERAND_INDEX_IMMEDIATE32 )
			continue;
		if( !CBIndex )
			CBIndex = i;
		else
			CBIndex = 0;
	}
	return CBIndex;
}
