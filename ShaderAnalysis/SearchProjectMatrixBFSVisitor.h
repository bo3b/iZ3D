#pragma once

#pragma warning ( push )
#pragma warning ( disable : 4503 )
#include <boost/graph/breadth_first_search.hpp>
#pragma warning ( pop )
#include "ShaderGraph.h"

namespace shader_analyzer
{
class SearchProjectMatrixBFSVisitor : public boost::default_bfs_visitor
{
	// shader instruction as vertex index
	TConstVertexIndexMap vertMap_;

	// shader operand as edge index
	TConstEdgeIndexMap edgeMap_;

	// graph to analyze
	const TGraph& gr_;

	// first valuable instruction
	static const ShaderInstruction* pInst_;

	// constant buffer operand of valuable instruction
	static const ShaderOperand* pOperand_;

	typedef std::set< ShaderOperandEdge > TFrontSet;

	// optional front set
	TFrontSet frontSet;
public:
	SearchProjectMatrixBFSVisitor( const TGraph& gr,
		boost::graph_traits< TGraph >::vertex_descriptor startVertex,
		const unsigned startComponents );

	void examine_edge( boost::graph_traits< TGraph >::edge_descriptor e,
		const TGraph& g );

	const ShaderInstruction* getInstruction(){
		return pInst_;
	}

	const ShaderOperand* getOperand(){
		return pOperand_;
	}
};
}


//************************************
// Method:    GetOperandIndexIfInstructionIsValuable
// FullName:  GetOperandIndexIfInstructionIsValuable
// Access:    public 
// Returns:   unsigned	index of cb operand 
// Qualifier:
// Parameter: const ShaderInstruction * pInst
//************************************
unsigned GetOperandIndexIfInstructionIsValuable(
	const shader_analyzer::ShaderInstruction* pInst );

bool IsInstructionIsValuable(
	const shader_analyzer::ShaderInstruction* pInst );

unsigned GetConstantBufferOperandIndex(
	const shader_analyzer::ShaderInstruction* pInst );