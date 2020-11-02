/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once
#define	PS_SHADER_TYPE 0xFFFF
#define	VS_SHADER_TYPE 0xFFFE
#include "ShaderInstruction.h"
#include "..\S3DAPI\ShaderProfileData.h"

enum ShaderAnalysisResult { AnalysisSharedIsMono, AnalysisMatrixFound, AnalysisMatrixNotFound, AnalysisIncorrectShader };

//---  AnalysisSharedIsMono    - we will be able to use mono draw call ---
//---  AnalysisMatrixFound     - we will be able to use stereo draw, modify only constant registers ---
//---  AnalysisMatrixNotFound  - we will be able to use stereo draw, but we need to modify shader code ---
//---  AnalysisIncorrectShader - illegal shader. probably CreateShader() function will fail on it. ---

class ShaderAnalyzer
{
public:
	ShaderAnalysisResult Analyze(DWORD shaderVersion, const char *shaderText, ShaderMatrices &matrices, DWORD &textures);

private:
	bool CheckRegisters(const ShaderRegister* c1, const ShaderRegister* c2, ShaderAnalyzerMatrices &cMatrix, bool &incorrectProjection );
	void SearchForMatrixOperation( const ShaderRegister* cur, ShaderAnalyzerMatrices &cMatrix, bool &incorrectProjection );
	bool SearchMatrices(const ShaderRegister* cur, int oPosComponent, ShaderMatrices& shaderData, int recursionLev = 0, int curLev = 0);

	static bool CheckNonTransposedMatrix( std::vector<const ScalarRegister*> &m, ShaderMatrixData &md );
	static bool CheckTransposedMatrix( std::vector<const ScalarRegister*> &m, ShaderMatrixData &md, int oPosComponent );
	static bool Check4Matrix( std::vector<const ScalarRegister*> &m, ShaderMatrixData& md, int oPosComponent );
	static bool Check4MatrixEx( std::vector<const ScalarRegister*> &m, ShaderMatrixData& md, int oPosComponent );
};
