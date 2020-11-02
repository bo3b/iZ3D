#include "StdAfx.h"

#include "ShaderGraph.h"

#pragma warning ( push )
#pragma warning ( disable: 4503)

#include <strstream>

#include "IsShaderTreeStart.h"
#include "IsInstructionHasNeededOperand.h"
#include "SearchProjectMatrixBFSVisitor.h"
#include <Windows.h>
#include <typeinfo>

#include <boost/graph/graphviz.hpp>
using namespace shader_analyzer;
using namespace std;
using namespace boost;

void shader_analyzer::CreateShaderTree( const TShaderList& shList,
									   const unsigned index,	// output register index
									   TGraph& retGraph,		// out graph
									   TStartVerticesArray& retArr,	// source nodes
									   ShaderOperand::EComponentState components )
{
	// vertex index const ShaderInstruction*
	TVertexIndexMap vertexIndex = get( vertex_index, retGraph );
	// edge index ShaderOperandEdge
	TEdgeIndexMap edgeIndexMap = get( edge_index, retGraph );

	// find first node to start graph
	TShaderList::const_reverse_iterator prevI = shList.rend();

	// each edge is described by shader operand of shader instruction-node
	typedef map< const ShaderOperandEdge,
		graph_traits< TGraph >::vertex_descriptor > TLeafArray;
	ShaderOperandEdge startOperandEdge( 0 );
	startOperandEdge.setComponentState( components );
	TLeafArray leafs;

	IsInstructionHasNeededOperand binComp( const ShaderOperandEdge( 0 ),
		D3D10_SB_OPERAND_TYPE_OUTPUT, index );

	TGraph::vertex_descriptor v(0);

	leafs[startOperandEdge] = 0;

	// find first appropriate shader instruction
	binComp.setOperand( ShaderOperandEdge( 0 ) );	// creade finder object
	TShaderList::const_reverse_iterator i = find_first_of( shList.rbegin(), shList.rend(),
		leafs.begin(), leafs.end(), binComp );
	while(  i != shList.rend() )
	{
		const ShaderInstruction* instr = &(*i); // for VA

		string debStr = instr->toString();	// debug string
		debStr = binComp.getOperand().toString();
		if ( i != prevI )	// is it new vertex in graph
		{
			// add empty vertex
			v = add_vertex( retGraph );
			vertexIndex[v] = instr;

			// forming new leafs from 1 to n operands in instruction
			for( unsigned j = 1; j < instr->getOperandsCount(); j++ )
			{
				// j-th operand
				ShaderOperandEdge bufOperand( instr->getOperand( j ) );
				if ( components == ShaderOperand::ALL_COMPONENTS ||
					instr->getOperation()->isScalarCommand() )
				{
					// copy component states from operand
					bufOperand.setComponentState( bufOperand.getOperand()->getComponentState() );
				}
				else
				{
					unsigned outCS = instr->getOperand(0)->getComponentState();
					bufOperand.setComponentState( bufOperand.getOperand()->getComponentState(outCS) );
				}
				// debug
				debStr = bufOperand.toString();
				// if operand is not immediate value, it is added to graph leafs buffer
				if( bufOperand.getOperand()->getOperandType() !=
					D3D10_SB_OPERAND_TYPE_IMMEDIATE32 )
				{
					// clear previous component state if exist
					leafs.erase( bufOperand );
					leafs[bufOperand] = v;
				}
			}
		}
		else
		{
			bool isSelfLoop = false;
			const ShaderOperand* pDestOperand = binComp.getOperand().getOperand();
			for( unsigned j = 1; j < instr->getOperandsCount(); j++ )
			{
				if( instr->getOperand( j ) == pDestOperand ){
					isSelfLoop = true;
				}
			}
			if( isSelfLoop )
			{
				++i;
				instr = &(*i);
				continue;
			}
		}
		prevI = i;

		unsigned components = 0;
		if ( binComp.getOperand().getOperand() )	// if connected operand is not terminal
		{
			debStr = binComp.getOperand().toString();	//debug
			TGraph::edge_descriptor buf_edge;
			bool isAdded;
			debStr = vertexIndex[v]->toString();
			TGraph::vertex_descriptor bV = leafs[binComp.getOperand()];
			debStr = vertexIndex[bV]->toString();
			// add the edge of the last appropriate vertex as source
			tie( buf_edge, isAdded ) = add_edge( leafs[binComp.getOperand()], v, retGraph );
			// get common components to name edge of graph
			components = instr->getOperand( 0 )->getComponentState() &
				binComp.getOperand().getComponentState();
			edgeIndexMap[buf_edge] = ShaderOperandEdge( binComp.getOperand() );
			edgeIndexMap[buf_edge].setComponentState( components );
		}
		else		// if connected operand is terminal, it is added to terminal list
		{
			retArr.push_back( v );
		}

		// components that aren't utilized in current instruction
		unsigned restComponents = ~( instr->getOperand( 0 )->getComponentState() ) &
			binComp.getOperand().getComponentState();
		if( restComponents )	// if non utilized components are exist
		{
			// get operand with previous components
			TGraph::vertex_descriptor bV = leafs[binComp.getOperand()];
			// clear previous components
			leafs.erase( binComp.getOperand() );
			// new operand
			ShaderOperandEdge bufOperand( binComp.getOperand() );
			// non utilized components
			bufOperand.setComponentState( restComponents );
			// debug
			debStr = bufOperand.toString();
			// add vertex with new utilized components
			leafs[bufOperand] = bV;
		}
		else
		{
			// remove operand if all components are utilized
			leafs.erase( binComp.getOperand() );
		}

		// find next appropriate shader instruction
		binComp.setOperand( ShaderOperandEdge( 0 ) );	// create finder object
		i = find_first_of( i, shList.rend(),
			leafs.begin(), leafs.end(), binComp );
	}
}

#if 0
void shader_analyzer::GraphVizShaderTree( const TGraph& graph,
										 GraphvizDigraph& retGraph )
{
	typedef TGraph::vertex_iterator TInVertexIterator;
	typedef TGraph::vertex_descriptor TInVertexDescriptor;
	typedef GraphvizDigraph::vertex_descriptor TOutVertexDescriptor;
	typedef GraphvizDigraph::edge_descriptor TOutEdgeDescriptor;

	typedef map< TInVertexDescriptor, TOutVertexDescriptor > TInVerOutVerMap;
	typedef property_map< GraphvizDigraph,
		vertex_attribute_t >::type TVerAttrMap;
	typedef property_map< GraphvizDigraph,
		edge_attribute_t >::type TEdgAttrMap;


	TVerAttrMap vertex_map = get( vertex_attribute, retGraph );
	TEdgAttrMap edge_map = get( edge_attribute, retGraph );
	TConstVertexIndexMap shader_inst = get( vertex_index, graph );

	TInVerOutVerMap mainMap;
	TInVertexIterator start, end;
	tie( start, end ) = vertices( graph );

	for( TInVertexIterator i = start; i != end; i++ )
	{
		TOutVertexDescriptor bufVD = add_vertex( retGraph );
		mainMap[*i] = bufVD;
		vertex_map[bufVD]["label"] = shader_inst[*i]->toString();
	}

	typedef graph_traits< TGraph >::edge_iterator TInEdgeIterator;

	TInEdgeIterator edgeStart, edgeEnd;
	tie( edgeStart, edgeEnd ) = edges( graph );
	TConstEdgeIndexMap shader_operand = get( edge_index, graph );

	for( TInEdgeIterator i = edgeStart; i != edgeEnd; i++ )
	{
		bool isAdded;
		TOutEdgeDescriptor bufED;
		tie( bufED, isAdded ) = add_edge(
			mainMap[source( *i, graph )],
			mainMap[target( *i, graph )],
			retGraph );

		edge_map[bufED]["label"] = shader_operand[*i].toString();
	}
}
#endif

pair< ShaderGraph::out_edge_iterator, ShaderGraph::out_edge_iterator >
ShaderGraph::GetOutEdges( vertex_descriptor u_local, unsigned /*component*/ ) const
{
	//TEdgesArray::const_iterator it = edgesArray_.find( u_local );
	/*if( it == edgesArray_.end() )
	{
		edgesArray_[u_local] = TAdjacentVerticesArray();
		CreateOutEdges( u_local, component );
	}*/
	return pair< out_edge_iterator, out_edge_iterator >(
		edgesArray_[u_local].begin(),
		edgesArray_[u_local].end() );
}

class IsOperandUsed{
	const ShaderOperand& Operand_;
public:
	IsOperandUsed( const ShaderOperand& Operand ) :
	  Operand_( Operand )
	{}

	bool operator () ( const ShaderOperandEdge& operandEdge )
	{
		if( Operand_.getOperandType() != operandEdge.getOperand()->getOperandType() )
			return false;
		unsigned commonComponent = Operand_.getComponentState() &
			operandEdge.getComponentState();
		if( !commonComponent )
			return false;

		int indexNum = Operand_.getOperandIndexDimension();
		if( operandEdge.getOperand()->getOperandIndexDimension() != indexNum )
			return false;

		for( int i = 0; i < indexNum; i++ )
		{
			if( Operand_.getOperandIndexRepresentation(
				( D3D10_SB_OPERAND_INDEX_DIMENSION )i ) != 
				D3D10_SB_OPERAND_INDEX_IMMEDIATE32 )
				return false;
			if( operandEdge.getOperand()->getOperandIndexRepresentation(
				( D3D10_SB_OPERAND_INDEX_DIMENSION )i ) !=
				D3D10_SB_OPERAND_INDEX_IMMEDIATE32 )
				return false;
			if( Operand_.getIndex( i ) != operandEdge.getOperand()->getIndex( i ) )
				return false;
		}
		return true;
	}
};

typedef set< ShaderOperandEdge > TOperandEdgeArray;

void AddFrontData( const ShaderInstruction& startInstruction, const unsigned component, TOperandEdgeArray &bufOperands )
{
	D3D10_SB_OPCODE_TYPE opType = startInstruction.getOperation()->getType();
	// initialize start edges
	for( unsigned i = 1; i < startInstruction.getOperandsCount(); i++ )
	{
		if( i == 1 && opType == D3D10_SB_OPCODE_MOVC ) // HACK: for Just Cause 2
			continue;
		const ShaderOperand* pOperand = startInstruction.getOperand( i );
		if( pOperand->getOperandType() == D3D10_SB_OPERAND_TYPE_TEMP )
		{
			ShaderOperandEdge bufEdge = startInstruction.getComponentsEffectOnGiven( component, i );
			if ( bufEdge.getComponentState() )
			{
				bufOperands.insert( bufEdge );
			}
		}
	}
}

void ShaderGraph::CreateOutEdges( vertex_descriptor u_local, ShaderOperand::EComponentState component )
{	
	TOperandEdgeArray bufOperands;
	const ShaderInstruction& startInstruction = shaderList_[u_local];

#ifdef _DEBUG
	std::string strInst = startInstruction.toString();
#endif

	TAdjacentVerticesArray& adjArr = edgesArray_[u_local];
	AddFrontData( startInstruction, component, bufOperands );

	for( unsigned i = u_local - 1; i > 0; --i )
	{
		const ShaderInstruction& currentInstruction = shaderList_[i];
		if( currentInstruction.getOperandsCount() < 2 )
			continue;
		const ShaderOperand* pCurrentOutputOperand = currentInstruction.getOperand( 0 );
		TOperandEdgeArray::iterator it = find_if(
			bufOperands.begin(),
			bufOperands.end(),
			IsOperandUsed( *pCurrentOutputOperand ) );
		if( it == bufOperands.end() )
			continue;
#ifdef _DEBUG
		std::string curInst = currentInstruction.toString();
#endif
		unsigned commonComponents =	pCurrentOutputOperand->getComponentState() & 
			it->getComponentState();
		unsigned restComponenst = it->getComponentState() &
			~( pCurrentOutputOperand->getComponentState() );
		ShaderOperandEdge bufEdge = ShaderOperandEdge( it->getOperand() );
		if( !restComponenst )
			const_cast<TOperandEdgeArray::iterator::value_type&>(*it).setComponentState(restComponenst);
		else
			bufOperands.erase( it );
		bufEdge.setComponentState( commonComponents );
		adjArr.push_back( TAdjElement(i, bufEdge ) );
	}
}

void ShaderGraph::InsertInBufIfSecondNodeIsUnique( TAdjacentVerticesArray& baseList,
									TAdjacentVerticesArray::const_iterator startIt,
									TAdjacentVerticesArray::const_iterator finishIt)
{
	for(auto it = startIt; startIt != finishIt; ++startIt)
	{
		ShaderGraph::TAdjacentVerticesArray::const_iterator findedIt =
			std::find( baseList.begin(), baseList.end(), *it );
		if( findedIt == baseList.end() )
			baseList.push_back( *it );
		/*ShaderGraph::TAdjacentVerticesArray::const_iterator it2 = baseList.begin();
		for( ;
			it2 != baseList.end(); it2++ )
		{
			if( it->first == it2->first &&
				it->second.getOperand() == it2->second.getOperand() &&
				it->second.getComponentState() == it2->second.getComponentState() )
			{
					continue;
			}
		}
		if( it2 != baseList.end() )
			baseList.push_back( *it );*/
	}
}

shader_analyzer::ShaderGraph::vertex_descriptor shader_analyzer::ShaderGraph::GetSource( edge_descriptor e_local ) const
{
	return e_local.first;
}

shader_analyzer::ShaderGraph::vertex_descriptor shader_analyzer::ShaderGraph::GetTarget( edge_descriptor e_local ) const
{
	return e_local.second;
}

shader_analyzer::ShaderGraph::degree_size_type shader_analyzer::ShaderGraph::GetOutDegree(
	vertex_descriptor u_local ) const
{
	return (shader_analyzer::ShaderGraph::degree_size_type)edgesArray_[u_local].size();
	/*TEdgesArray::const_iterator i = edgesArray_.find( u_local );
	if( i == edgesArray_.end() )
		return 0;
	return i->second.size();*/
}

bool IsElementShouldBeEarlier( const ShaderGraph::TAdjElement& _Left,
							  const ShaderGraph::TAdjElement& _Right)
{
	return _Left.first > _Right.first;
}

shader_analyzer::ShaderGraph::ShaderGraph( const TShaderList& list ) :
shaderList_( list )
{
	edgesArray_.resize( list.size() );
}

std::pair< graph_traits< shader_analyzer::ShaderGraph >::out_edge_iterator,
graph_traits< shader_analyzer::ShaderGraph >::out_edge_iterator > boost::out_edges( graph_traits< shader_analyzer::ShaderGraph >::vertex_descriptor u_local, const shader_analyzer::ShaderGraph& g )
{
	return g.GetOutEdges( u_local );
}

graph_traits< shader_analyzer::ShaderGraph >::vertex_descriptor boost::source( graph_traits< shader_analyzer::ShaderGraph >::edge_descriptor e_local, const shader_analyzer::ShaderGraph& g )
{
	return g.GetSource( e_local );
}

graph_traits< shader_analyzer::ShaderGraph >::vertex_descriptor boost::target( graph_traits< shader_analyzer::ShaderGraph >::edge_descriptor e_local, const shader_analyzer::ShaderGraph& g )
{
	return g.GetTarget( e_local );
}

graph_traits< shader_analyzer::ShaderGraph >::degree_size_type boost::out_degree( graph_traits< shader_analyzer::ShaderGraph >::vertex_descriptor u_local, const shader_analyzer::ShaderGraph& g )
{
	return g.GetOutDegree( u_local );
}
