#pragma once

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>

#include <list>
#include <vector>

#include "ShaderInstruction.h"
#include "ShaderOperandEdge.h"

namespace shader_analyzer{
	typedef boost::property< boost::vertex_color_t, boost::default_color_type > TVertexColor;
	typedef boost::property< boost::vertex_index_t,
		const shader_analyzer::ShaderInstruction*, TVertexColor > TVertexProperty;
	typedef boost::property< boost::edge_index_t,
		shader_analyzer::ShaderOperandEdge > TEdgeProperty;

	typedef boost::adjacency_list< boost::listS,
		boost::listS,
		boost::directedS,
		TVertexProperty,
		TEdgeProperty > TGraph;
	typedef boost::property_map< TGraph, boost::vertex_index_t >::type TVertexIndexMap;
	typedef boost::property_map< TGraph, boost::vertex_index_t >::const_type TConstVertexIndexMap;
	typedef boost::property_map< TGraph, boost::edge_index_t>::type TEdgeIndexMap;
	typedef boost::property_map< TGraph, boost::edge_index_t>::const_type TConstEdgeIndexMap;
	typedef std::list< boost::graph_traits< TGraph >::vertex_descriptor > TStartVerticesArray;

	void CreateShaderTree(
		const TShaderList& shList,
		const unsigned index,
		TGraph& retGraph,
		TStartVerticesArray& retArr,
		ShaderOperand::EComponentState components = ShaderOperand::ALL_COMPONENTS );

#if 0
	void GraphVizShaderTree( const TGraph& graph, boost::GraphvizDigraph& retGraph );
#endif

	class ShaderGraph{
		
	public:
		// typedefs
		typedef std::pair< unsigned, ShaderOperandEdge > TAdjElement;
		typedef std::list< TAdjElement > TAdjacentVerticesArray;
		typedef std::vector< TAdjacentVerticesArray > TEdgesArray;
		// graph category
		typedef unsigned							vertex_descriptor;
		typedef boost::directed_tag					directed_category;
		typedef boost::incidence_graph_tag					traversal_category;
		typedef boost::disallow_parallel_edge_tag	edge_parallel_category;
		// incidence graph category
		typedef std::pair< unsigned, unsigned >		edge_descriptor;
		typedef unsigned							degree_size_type;
		typedef TAdjacentVerticesArray::const_iterator	out_edge_iterator;

		// unused typedefs
		typedef void								adjacency_iterator;
		typedef void								edge_iterator;
		typedef unsigned							edges_size_type;
		typedef void								in_edge_iterator;
		typedef void								vertex_iterator;
		typedef unsigned							vertices_size_type;
		
		ShaderGraph( const TShaderList& list );

		std::pair< out_edge_iterator, out_edge_iterator > GetOutEdges(
			vertex_descriptor u_local,
			const unsigned component = ShaderOperand::ALL_COMPONENTS ) const;

		vertex_descriptor GetSource( edge_descriptor e_local ) const;

		vertex_descriptor GetTarget( edge_descriptor e_local ) const;

		degree_size_type GetOutDegree( vertex_descriptor u_local ) const;

		void ClearEdgesArray()
		{
			size_t size = edgesArray_.size();
			edgesArray_.clear( );
			edgesArray_.resize( size );
		}

		//************************************
		// Method:    GetPreviousInstruction
		// FullName:  shader_analyzer::ShaderGraph::GetPreviousInstruction
		// Access:    public 
		// Returns:   std::pair< unsigned, unsigned > - first=valuable shader index
		// second - component style
		// Qualifier:
		// Parameter: const unsigned startIndex - start shader index
		// Parameter: const unsigned components	- start shader components
		//************************************
		template<class T>
		ShaderOperand::ShaderRegister GetPreviousInstruction(
			unsigned startIndex, ShaderOperand::EComponentState components, T func )
		{
			if( IsInstructionIsValuable( &shaderList_[startIndex] ) )
			{
				unsigned CBOperandIndex = 1; // TODO:  Fix it
				if (func(shaderList_, startIndex, CBOperandIndex))
				{
					return ShaderOperand::ShaderRegister( startIndex, CBOperandIndex, components );
				}
			}
			TAdjacentVerticesArray buf;
			CreateOutEdges( startIndex, components );
			// complete searched elements
			buf.insert( buf.end(), edgesArray_[startIndex].begin(), edgesArray_[startIndex].end() );
			while( buf.size() )
			{
				//buf.sort( IsElementShouldBeEarlier );
				buf.sort(  [] (const ShaderGraph::TAdjElement& _Left, const ShaderGraph::TAdjElement& _Right)
				{ 
					return _Left.first > _Right.first; 
				} );
				TAdjacentVerticesArray::const_iterator itWriteElement = buf.begin();
#ifdef _DEBUG
				std::string strInst = shaderList_[itWriteElement->first].toString();
#endif
				if( IsInstructionIsValuable( &shaderList_[itWriteElement->first] ) )
				{
					unsigned writeMask = itWriteElement->second.getComponentState();
					ShaderOperand::EComponentState mask = (ShaderOperand::EComponentState)(writeMask & components);
					unsigned CBOperandIndex = 1; // TODO:  Fix it
					if (func(shaderList_, itWriteElement->first, CBOperandIndex))
					{
						return ShaderOperand::ShaderRegister( itWriteElement->first, CBOperandIndex, mask );
					}
				}
				//unsigned writeMask = itFirstElement->second.getComponentState();
				//ShaderOperand::EComponentState CBOperandIndex = (ShaderOperand::EComponentState)(writeMask & components);
				CreateOutEdges( itWriteElement->first, (ShaderOperand::EComponentState)itWriteElement->second.getComponentState() );
				//buf.insert( buf.end(), edgesArray_[itFirstElement->first].begin(),
				//	edgesArray_[itFirstElement->first].end() );
				TAdjacentVerticesArray& destInstrVertices = edgesArray_[itWriteElement->first];
				InsertInBufIfSecondNodeIsUnique( buf,
					destInstrVertices.begin(), destInstrVertices.end());
				buf.pop_front();
			}

			return ShaderOperand::ShaderRegister( INVALID_REGISTER, 0, (ShaderOperand::EComponentState)0 );
		}

	private:
		void CreateOutEdges( const vertex_descriptor u_local, ShaderOperand::EComponentState component );
		void InsertInBufIfSecondNodeIsUnique( TAdjacentVerticesArray& baseList,
			TAdjacentVerticesArray::const_iterator startIt,
			TAdjacentVerticesArray::const_iterator finishIt);
		const TShaderList& shaderList_;
		TEdgesArray edgesArray_;
	};
}

namespace boost
{
	// incidence graph category
	std::pair< graph_traits< shader_analyzer::ShaderGraph >::out_edge_iterator,
		graph_traits< shader_analyzer::ShaderGraph >::out_edge_iterator >
		out_edges( graph_traits< shader_analyzer::ShaderGraph >::vertex_descriptor u_local,
		const shader_analyzer::ShaderGraph& g );

	graph_traits< shader_analyzer::ShaderGraph >::vertex_descriptor
		source( graph_traits< shader_analyzer::ShaderGraph >::edge_descriptor e_local,
		const shader_analyzer::ShaderGraph& g );

	graph_traits< shader_analyzer::ShaderGraph >::vertex_descriptor
		target( graph_traits< shader_analyzer::ShaderGraph >::edge_descriptor e_local,
		const shader_analyzer::ShaderGraph& g );

	graph_traits< shader_analyzer::ShaderGraph >::degree_size_type
		out_degree( graph_traits< shader_analyzer::ShaderGraph >::vertex_descriptor u_local,
		const shader_analyzer::ShaderGraph& g );
}
