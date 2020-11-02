/* IZ3D_FILE: $Id$ 
*
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*
* $Author$
* $Revision$
* $Date$
* $LastChangedBy$
* $URL$
*/
#include "StdAfx.h"
#include <vector>
#include <stack>
#include <algorithm>
#include "ShaderAnalyzer.h"
#include "ShaderInstruction.h"
#ifdef DEBUGANALYSING
#include <iostream>
#endif

using namespace std;

bool ShaderAnalyzer::CheckRegisters(const ShaderRegister* c1, const ShaderRegister* c2, ShaderAnalyzerMatrices &cMatrix, bool &incorrectProjection )
{
	if(c2->vRegCount == 0 && c2->cRegCount > 0)
	{
		if(c1->vRegCount > 0) // (v1) * (c1)
		{
			vector<ScalarRegister> s2;
			c2->GetConstantRegistersList(s2);
			//vector<ScalarRegister> v;
			//v.push_back(s2.back()); // use last for W * V * P
			cMatrix.move(s2);
			return true;
		} 
		else if(c2->cRegCount == 1) // && (c1.vRegCount == 0)
		{
			if(c1->cRegCount == 1) // (c1) * (c2)
			{
				vector<ScalarRegister> s1;
				c1->GetConstantRegistersList(s1);
				vector<ScalarRegister> s2;
				c2->GetConstantRegistersList(s2);
				vector<ScalarRegister> v;
				v.push_back(s1[0]);
				v.push_back(s2[0]);
				cMatrix.move(v);
				incorrectProjection = true; // should be only set if multiply after matrix register
				return true;			
			} 
			else if(c1->cRegCount >  1) // (c1,c2...) * (c3)
			{
				vector<ScalarRegister> s2;
				c2->GetConstantRegistersList(s2);
				cMatrix.move(s2);
				incorrectProjection = true;
				return true;
			}
			else // 0 * (c2)
			{
				vector<ScalarRegister> s2;
				c2->GetConstantRegistersList(s2);
				cMatrix.move(s2);
				return true;
			}
		} 
	}
	return false;
}

void ShaderAnalyzer::SearchForMatrixOperation( const ShaderRegister* cur, ShaderAnalyzerMatrices &cMatrix, bool &incorrectProjection )
{
	// search ()*()+()*()+()*()+()*()
	COMULTIPLIERS cm;
	cm.it = cur->elem.begin();
	incorrectProjection = false;
	while (cur->SearchCoMultiplies(cm))
	{
#if defined(_DEBUG) && defined(DEBUGANALYSING)
		string str1;
		cm.s1->reg->ToTreeString(str1);
#endif
		if (cm.s2)
		{
#if defined(_DEBUG) && defined(DEBUGANALYSING)
			string str2;
			cm.s2->reg->ToTreeString(str2);
#endif
			if (CheckRegisters(cm.s1->reg, cm.s2->reg, cMatrix, incorrectProjection))
				continue;
			if (CheckRegisters(cm.s2->reg, cm.s1->reg, cMatrix, incorrectProjection))
				continue;
		}
		else if(cMatrix.size() == 3 && cm.s1->reg->vRegCount == 0 && cm.s1->reg->cRegCount > 0)
		{
			vector<ScalarRegister> s1;
			cm.s1->reg->GetConstantRegistersList(s1);
			if (s1.size() > 1)
				s1.erase(s1.begin() + 1, s1.end());
			cMatrix.move(s1); // (1.0f) * (c1)
		}
	}
}

bool ShaderAnalyzer::SearchMatrices(const ShaderRegister* cur, int oPosComponent, ShaderMatrices& shaderData, 
									int recursionLevel, int currentLevel)
{
	if (shaderData.matrixSize >= SHADER_MATRIX_SIZE)
		return true;

#if defined(_DEBUG) && defined(DEBUGANALYSING)
	string str;
	cur->ToTreeString(str);
#endif

	ShaderAnalyzerMatrices matrix;
	bool incorrectProjection;
	SearchForMatrixOperation(cur, matrix, incorrectProjection);

	if(matrix.size() == 4 || matrix.size() == 3)
	{
		ShaderMatrixData md;
		md.inverse = FALSE;
		md.incorrectProjection = incorrectProjection;
		if (matrix.size() == 4 && matrix.for_each_4(Check4Matrix, md, oPosComponent))
		{
			shaderData.matrix[shaderData.matrixSize++] = md;
			return true;
		}
		if(currentLevel < recursionLevel && matrix.for_each_3_4(Check4MatrixEx, md, oPosComponent))
		{
			shaderData.matrix[shaderData.matrixSize++] = md;
			return true;
		}
	}
	else
	{
		registersIterator it = cur->elem.begin();
		const RegisterWithOp* subelement; // for each nested ()
		while(cur->GetNextBracketCouple(it, subelement))
		{
			if(!subelement->reg->IsRegister() &&
				SearchMatrices(subelement->reg, oPosComponent, shaderData, recursionLevel, currentLevel + 1))
				return true;
		}
	}
	return false;
}

ShaderAnalysisResult ShaderAnalyzer::Analyze(DWORD shaderVersion, const char *shaderText, ShaderMatrices &matrices, DWORD &textures)
{
	char oPosName[16]; 
	ShaderInstruction si;
	ProcessorRegister oPos;
	if(!GetShaderoPosName(shaderVersion, shaderText, oPosName))
	{
		DEBUG_MESSAGE(_T("%s: Warning! oPos register not found!\n"), _T(__FUNCTION__));
		return AnalysisIncorrectShader;
	}
	textures = 0;
	si.execute(shaderVersion, shaderText, oPosName, oPos, textures);
#ifdef DEBUGANALYSING
	cout << "\tShader executed\n";
	cout << "\tTotal registers: " << si.GetTotalRegisters() << "\n";
#endif

	for(int i=ComponentX; i< MAX_REGISTER_COMPONENTS; i++)
	{
#ifdef DEBUGANALYSING
		cout << "\t" << i << ": vReg " << oPos.comp[i]->vRegCount << " cReg " << oPos.comp[i]->cRegCount << " cRegWithInd " << oPos.comp[i]->cRegCountWithInd << "\n";
#endif
		if (i == ComponentZ)
			continue;
		bool res = oPos.comp[i]->OperationsMoreThanOne();
		if(oPos.comp[i]->vRegCount == 0 || oPos.comp[i]->cRegCountWithInd == 0 || !res)
		{
#ifdef DEBUGANALYSING
			cout << "\t" << i << ": vReg " << oPos.comp[i]->vRegCount << " cReg " << oPos.comp[i]->cRegCountWithInd
			  << " +count > 1 && *count > 1 " << res << "\n";
#endif
			return AnalysisSharedIsMono;
		}
	}

	//		for(int i=ComponentX; i< MAX_REGISTER_COMPONENTS; i++)
	//		{
	//#ifdef DEBUGANALYSING
	//			cout << "\tSearchForMatrixOperation " << i << "\n";
	//#endif
	//			if(SearchForMatrixOperation(oPos[i].c_str(), i, m_CurrentVSData))
	//				return true;
	//		}

#ifdef DEBUGANALYSING
	cout << "\tSearchMatrices\n";
#endif
	//--- analyze all vector component and search most frequently matrix ---
	typedef pair<bool, ShaderMatrices> VSPair;
	vector<VSPair> VSData(4, VSPair(false, matrices));
	for(int i=ComponentX; i< MAX_REGISTER_COMPONENTS; i++)
	{
#ifdef DEBUGANALYSING
		cout << "\t" << i;
#endif
		VSData[i].first = SearchMatrices(oPos.comp[i], i, VSData[i].second, 1) != 0;
		if (!VSData[i].first)
		{
#ifdef DEBUGANALYSING
			cout << "s";
#endif
			VSData[i].first = SearchMatrices(oPos.comp[i], i, VSData[i].second, MAXINT) != 0;
		}
	}
#ifdef DEBUGANALYSING
	cout << "\n";
#endif

/*#ifdef DEBUGANALYSING
	for(int i=ComponentX; i< MAX_REGISTER_COMPONENTS; i++)
	{
		string str;
		oPos.comp[i]->ToTreeString(str);
		cout << "Tree " << i << "\n" << str.c_str();
		cout << "End of tree\n";
	}
#endif*/

	int maxFreqCount = 0;
	int maxFreqInd = 0;
	ShaderAnalysisResult analysisResult = AnalysisMatrixNotFound;
	for(int i=ComponentX; i< MAX_REGISTER_COMPONENTS; i++)
	{
		if(VSData[i].first)
		{
			int freqCount = 1;
			for(int j=ComponentX; j< MAX_REGISTER_COMPONENTS; j++)
			{
				if(VSData[j].first)
					if(VSData[i].second.matrix[0].matrixRegister == VSData[j].second.matrix[0].matrixRegister)
						freqCount++;
			}
			if(freqCount >= maxFreqCount)
			{
				maxFreqCount = freqCount;
				maxFreqInd = i;
			}
		}
	}
	if(maxFreqCount >= 2)
	{
		analysisResult = AnalysisMatrixFound;
		matrices = VSData[maxFreqInd].second;
	}
	return analysisResult;
}

bool ShaderAnalyzer::CheckTransposedMatrix( std::vector<const ScalarRegister*> &m, ShaderMatrixData &md, int oPosComponent )
{
	// dp4
	if( m[0]->registerNumber == m[1]->registerNumber &&
		m[0]->registerNumber == m[2]->registerNumber &&
		m[0]->registerNumber == m[3]->registerNumber && 
		m[0]->swizzleIndex == m[1]->swizzleIndex-1 && 
		m[0]->swizzleIndex == m[2]->swizzleIndex-2 && 
		m[0]->swizzleIndex == m[3]->swizzleIndex-3  )
	{
		md.matrixIsTransposed = TRUE;
		md.matrixRegister = m[0]->registerNumber - oPosComponent;
		return true;
	}
	return false;
}

bool ShaderAnalyzer::CheckNonTransposedMatrix( std::vector<const ScalarRegister*> &m, ShaderMatrixData &md )
{
	// mul + mad
	if( m[0]->registerNumber == m[1]->registerNumber-1 &&
		m[0]->registerNumber == m[2]->registerNumber-2 &&
		m[0]->registerNumber == m[3]->registerNumber-3 && 
		m[0]->swizzleIndex == m[1]->swizzleIndex && 
		m[0]->swizzleIndex == m[2]->swizzleIndex && 
		m[0]->swizzleIndex == m[3]->swizzleIndex  )
	{
		md.matrixIsTransposed = FALSE;
		md.matrixRegister = m[0]->registerNumber;
		return true;
	}
	return false;
}

bool ShaderAnalyzer::Check4Matrix( std::vector<const ScalarRegister*> &m, ShaderMatrixData& md, int oPosComponent )
{
	if (CheckTransposedMatrix(m, md, oPosComponent) || CheckNonTransposedMatrix(m, md))
		return true;
	return false;
}

bool ShaderAnalyzer::Check4MatrixEx( std::vector<const ScalarRegister*> &m, ShaderMatrixData& md, int oPosComponent )
{
	// dp4
	for (int i = 0; i < 4; i++)
	{
		int reg = m[i]->registerNumber;
		int fi = -1; // incorrect element
		int c = 0;
		DWORD si = 0;
		for (int j = 0; j < 4; j++)
		{
			if (reg == m[j]->registerNumber &&
				(m[i]->swizzleIndex ==  (m[j]->swizzleIndex - j + i) || i == j))
			{
				c++;
				si |= 1 << m[j]->swizzleIndex;
			}
			else
				fi = j;
		}

		if (c == 4 && CheckTransposedMatrix(m, md, oPosComponent)) {
			return true;
		} 
		else if (c == 3)
		{
			_ASSERT(fi != -1);
			// fix incorrect element
			std::vector<const ScalarRegister*> cm = m;
			ScalarRegister tempRegister = *cm[i];
			cm[fi] = &tempRegister;
			for (int k = 0; k < 4; k++)
				if ((si & (1 << k)) == 0)
					tempRegister.swizzleIndex = k;

			if (CheckTransposedMatrix(cm, md, oPosComponent))
			{
				md.incorrectProjection = TRUE;
				return true;
			}
		}
	}

	// mul + mad
	for (int i = 0; i < 4; i++)
	{
		int si = m[i]->swizzleIndex;
		int fi = -1;
		int c = 0;
		for (int j = 0; j < 4; j++)
		{
			if (si == m[j]->swizzleIndex &&
				(m[i]->registerNumber ==  (m[j]->registerNumber - j + i) || i == j))
			{
				c++;
			}
			else
				fi = j;
		}

		if (c == 4 && CheckNonTransposedMatrix(m, md)) {
			return true;
		} 
		else if (c == 3)
		{
			_ASSERT(fi != -1);
			// fix incorrect element
			std::vector<const ScalarRegister*> cm = m;
			ScalarRegister tempRegister = *cm[i];
			cm[fi] = &tempRegister;
			if (fi == 0)
				tempRegister.registerNumber = cm[1]->registerNumber - 1;
			else
				tempRegister.registerNumber = cm[fi - 1]->registerNumber + 1;

			if (CheckNonTransposedMatrix(cm, md))
			{
				md.incorrectProjection = TRUE;
				return true;
			}
		}		
	}
	return false;
}
