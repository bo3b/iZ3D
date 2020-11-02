#include "stdafx.h"

#include <iomanip>
#include <istream>
#include <fstream>
#include <strstream>

#include <vector>
#include <list>
#include <queue>


#include "ShaderInstruction.h"
#include <d3dx10async.h>

#include "Disasm.h"

#include "ShaderGraph.h"
#include "GraphAnalysis.h"
#include <iostream>
#include <d3d10umddi.h>

void shader_analyzer::disassemble(std::istream& data, std::ostream& output)
{
	TShaderList shList;
	parseBinaryAssembler( data, shList );
	output << shList.size() << std::endl;
	for(shader_analyzer::TShaderList::const_iterator i = shList.begin();
		i != shList.end(); i++)
	{
		output << (*i) << std::endl;
	}
}
//----------------------------------------------------------------------------
// D3DX10DisassembleShader:
// ----------------------
// Takes a binary shader, and returns a buffer containing text assembly.
//
// Parameters:
//  pShader
//      Pointer to the shader byte code.
//  BytecodeLength
//      Size of the shader byte code in bytes.
//  EnableColorCode
//      Emit HTML tags for color coding the output?
//  pComments
//      Pointer to a comment string to include at the top of the shader.
//  ppDisassembly
//      Returns a buffer containing the disassembled shader.
//----------------------------------------------------------------------------

typedef HRESULT (WINAPI *PFND3DX10DISASSEMBLESHADER)(
						CONST void *pShader,
						SIZE_T BytecodeLength,
						BOOL EnableColorCode,
						LPCSTR pComments,
						interface ID3D10Blob** ppDisassembly);

int shader_analyzer::CheckDisasm()
{
	//////////////////////////////////////////////////////////////////////////
	// Compiling source shader
	LPD3D10BLOB pBlob;
	LPD3D10BLOB pError;
	HRESULT hr;

	D3DX10CompileFromFileA("C:/Program Files/Microsoft DirectX SDK (November 2008)/Samples/C++/Direct3D10/AdvancedParticles/AdvancedParticles.fx", 0, 0,
		0, "fx_4_0", 0, 0, 0, &pBlob, &pError, &hr);

	//////////////////////////////////////////////////////////////////////////
	// Write compiled shader to file 

	std::fstream compiledShader("C:/Users/Developer2/Documents/sample.xxx",
		std::ios_base::in | std::ios_base::binary | std::ios_base::out | std::ios_base::trunc);
	if(compiledShader.bad())
	{
		std::cerr << "fuck!";
	}
	compiledShader.write((char*)pBlob->GetBufferPointer(),
		pBlob->GetBufferSize());
	compiledShader.seekg(0);

	//////////////////////////////////////////////////////////////////////////
	// Find shader tokens and write it to file
	std::fstream tokensFile("C:/Users/Developer2/Documents/sample.tok",
		std::ios_base::in | std::ios_base::binary | std::ios_base::out | std::ios_base::trunc);

	shader_analyzer::GetShaderCode(compiledShader, tokensFile);

	//////////////////////////////////////////////////////////////////////////
	// our disasm
	tokensFile.seekg(0);

	shader_analyzer::disassemble(tokensFile, std::cout);

	//////////////////////////////////////////////////////////////////////////
	// reference disasm
	LPD3D10BLOB pDisass = 0;
	LPVOID pPo = pBlob->GetBufferPointer();
	SIZE_T sIze = pBlob->GetBufferSize();
	HMODULE hD3DX38 = NULL;
	hD3DX38 = LoadLibrary(_T("d3dx10_38.dll"));
	if(!hD3DX38)
		return 1;
	PFND3DX10DISASSEMBLESHADER D3DX10DisassembleShader = NULL;
	D3DX10DisassembleShader = (PFND3DX10DISASSEMBLESHADER)GetProcAddress(hD3DX38, "D3DX10DisassembleShader");
	if(!D3DX10DisassembleShader)
		return 1;
	D3DX10DisassembleShader( pPo, sIze, 0, 0, &pDisass );
	FreeLibrary(hD3DX38);
	std::ofstream refShader("C:/Users/Developer2/Documents/sample.asm",
		std::ios_base::binary | std::ios_base::out);
	refShader.write((char*)pDisass->GetBufferPointer(), pDisass->GetBufferSize());
	return 0;
}

void shader_analyzer::parseBinaryAssembler( std::istream& data, TShaderList& shList )
{
	using namespace shader_analyzer;
	DWORD VerTok;
	data.read((char*)&VerTok, sizeof(DWORD));
	DWORD LenTok;
	data.read((char*)&LenTok, sizeof(DWORD));
	while(!data.eof())
	{
		ShaderInstruction shInst;
		data >> shInst;
		if(data.rdstate() & std::ios::failbit)
			break;
		shList.push_back(shInst);
	}
}

class TextIdent{
private:
	std::string str_;
public:
	TextIdent(int spaces) 
	{
		while (--spaces > 0) {
			str_ += ' ';
		}
	}
	TextIdent(std::string& initIdent) : str_(initIdent) {}
	TextIdent() {}
	void operator++ ( )
	{
		str_ += "  ";
	}
	void operator-- ( )
	{
		str_.erase(str_.size() - 2, 2);
	}
	friend std::ostream& operator << (std::ostream& os, const TextIdent& ident);
};

class PrintShaderInstruction{
private:
	std::ostream& str_;
	TextIdent&	ident_;
	TextIdent	defIdent_;
	bool		printBinary_;
	size_t		instTextWidth_;
public:
	PrintShaderInstruction( std::ostream& str, TextIdent& ident, bool printBinary, size_t instTextWidth = 80 ) 
	:	str_( str )
	,	ident_( ident )
	,	printBinary_( printBinary )
	,	instTextWidth_( instTextWidth )
	{}

	PrintShaderInstruction( std::ostream& str ) 
	:	str_( str )
	,	ident_( defIdent_ )
	,	printBinary_( false )
	{}

	void operator () ( const shader_analyzer::ShaderInstruction& inst )
	{
		if (inst.getOperation()->isDecreaseIdent())
			--ident_;

		std::ostringstream strStream;
		strStream << ident_ << inst;
		if (printBinary_) 
		{
			int numSpaces = (int)(instTextWidth_ - strStream.str().length());
			strStream << TextIdent(numSpaces > 0 ? numSpaces : 0);
			std::vector<UINT> blob;
			inst.GetBlob(blob);
			for (size_t i = 0; i<blob.size(); ++i) {
				strStream << "0x" << std::setfill('0') << std::setw(8) << std::hex << blob[i] << " ";
			}
		}
		str_ << strStream.str() << std::endl;
		if (inst.getOperation()->isIncreaseIdent())
			++ident_;
	}
};

bool shader_analyzer::FindOutputSignature( const D3D10DDIARG_STAGE_IO_SIGNATURES* pSignatures, 
						 const D3D10DDIARG_SIGNATURE_ENTRY*& pOutputSignature )
{
	pOutputSignature = NULL;

	if( !pSignatures )
		return false;

	for( unsigned i = 0; i < pSignatures->NumOutputSignatureEntries; i++ )
	{
		if( pSignatures->pOutputSignature[i].SystemValue == D3D10_SB_NAME_POSITION )
		{
			pOutputSignature = &pSignatures->pOutputSignature[i];
			return true;
		}
	}
	return false;
}

void shader_analyzer::ParseShader( const unsigned* pCode, TShaderList& shList )
{
	std::istrstream shaderStream((char*)pCode, sizeof(DWORD) * pCode[1]);
	parseBinaryAssembler( shaderStream, shList );
}

void shader_analyzer::GetProjectionMatrices( const D3D10DDIARG_SIGNATURE_ENTRY* pOutputSignature, 
											ProjectionShaderData& shaderData, TShaderList& shList )
{
	//TGraph graph;
	//TStartVerticesArray arr;
	if( pOutputSignature )
	{
		//CreateShaderTree( shList, pOutputSignature->Register, graph, arr );
		//boost::GraphvizDigraph retGraph;
		/*GraphVizShaderTree( graph, retGraph );
		boost::write_graphviz( std::ofstream( "abracadabra" ), retGraph );
		FindProjectionMatrixFromGraph( graph, arr, shaderData.matrixData );*/
		FindProjectionMatrixFromShader( shList, pOutputSignature->Register, shaderData.matrixData );
	}
}

void shader_analyzer::PrintShader( const TShaderList &shList, std::ostream& disShader, std::string initIdent )
{
	TextIdent ident(initIdent);
	for_each( shList.begin(), shList.end(), PrintShaderInstruction( disShader, ident, false ) );
}

class PrintFoundedMatrix
{
private:
	std::ostream& str_;
public:
	PrintFoundedMatrix( std::ostream& ostr ) : str_( ostr ){}
	void operator () ( const std::pair<DWORD, ShaderCBs<ProjectionMatrixData>::CBMatrices>& mat )
	{
		str_ << "matrix register cb" << mat.first << "[" << mat.second[0].matrixRegister << "]" << std::endl;
		str_ << "is correct " << !mat.second[0].incorrectProjection << std::endl;
		str_ << "is transposed " << mat.second[0].matrixIsTransposed << std ::endl;
	}
};

std::ostream& operator << (std::ostream& os, const TextIdent& ident)
{
	return os << ident.str_;
}

void shader_analyzer::PrintMatrixData( const ProjectionShaderData& shaderData, std::ofstream& disShader )
{
	for_each(shaderData.matrixData.cb.begin(), shaderData.matrixData.cb.end(), 
		PrintFoundedMatrix( disShader ) );
}

bool shader_analyzer::ShaderList2Blob( const TShaderList& sList, UINT ver, std::vector<UINT>& resData, UINT size )
{
	resData.clear();
	resData.reserve(size);
	resData.push_back( ver );
	resData.push_back( 0 );
	std::vector<UINT> tempVector;
	for( auto i = sList.begin(); i != sList.end(); ++i )
	{
		i->GetBlob(tempVector);
		resData.insert( resData.end(), tempVector.begin(), tempVector. end() );
	}
	resData[1] = (UINT)resData.size();
	return true;
}