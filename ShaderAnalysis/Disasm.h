#pragma once
#include <istream>
#include <bitset>
#include "ShaderInstruction.h"
#include "AnalyzeData.h"

struct D3D10DDIARG_STAGE_IO_SIGNATURES;
namespace shader_analyzer{
	typedef std::list< ProjectionMatrixData > TProjMatrices;
	void disassemble( std::istream& data, std::ostream& output );
	void parseBinaryAssembler( std::istream& data, TShaderList& shList );
	int CheckDisasm();
	bool FindOutputSignature( const D3D10DDIARG_STAGE_IO_SIGNATURES* pSignatures, 
		const D3D10DDIARG_SIGNATURE_ENTRY*& pOutputSignature );
	void ParseShader( const unsigned* pCode, TShaderList& shList );
	void GetProjectionMatrices ( const D3D10DDIARG_SIGNATURE_ENTRY* pSignatures,
		ProjectionShaderData& shaderData, TShaderList& shList);
	void PrintShader( const TShaderList& shList, std::ostream& disShader, std::string initIdent = "" );
	void PrintMatrixData( const ProjectionShaderData& shaderData, std::ofstream& disShader );
	bool ShaderList2Blob( const TShaderList& sList, UINT ver, std::vector<UINT>& resData, UINT size );
	void GetUsedResourcesMask( const TShaderList& shList, std::bitset<128> &mask );
}