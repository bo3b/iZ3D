// DX10ShaderAnalyzingTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <windows.h>
#include <iostream>
#include <conio.h>
#include <boost/graph/adjacency_list.hpp>
#include <tinyxml.h>

#include "disasm.h"
#include "modify.h"
#include "AnalyzeData.h"
#include "ShaderGraph.h"
#include "GraphAnalysis.h"
#include "../CommonUtils/StringUtils.h"

// for Boost
#include <exception>
namespace boost // for shared_ptr
{
	void throw_exception( std::exception const & e ) 
	{
	}
}

using namespace std;

#pragma warning(disable:4273)
GlobalInfo _gInfo;
GlobalInfo& gInfo = _gInfo;

bool ShaderTest(ProjectionMatricesMap& trueMap, UINT* shaderCode, const D3D10DDIARG_STAGE_IO_SIGNATURES* pSignatures, int shaderIndex)
{
	//if (shaderIndex != 17) // for debug
	//	return true;
	if(!shaderCode)
		return false;
	using namespace shader_analyzer;
	ProjectionShaderData testShaderData;
	cout << "Try to analyze shader #" << shaderIndex << "\n";
	const D3D10DDIARG_SIGNATURE_ENTRY* pOutputSignature;
	bool b = FindOutputSignature(pSignatures, pOutputSignature);

	TShaderList shList;
	std::istrstream shaderStream((char*)shaderCode, sizeof(DWORD) * shaderCode[1]);
	parseBinaryAssembler( shaderStream, shList );
	if( pOutputSignature )
		FindProjectionMatrixFromShader( shList, pOutputSignature->Register, testShaderData.matrixData );

	//--- just my test TEMPORARY!!! ---
	//gInfo.VertexShaderModificationMethod = 0;
	//ModifiedShaderData mMData;
	//TShaderList modifiedShader;
	//ModifyShader(shList, pOutputSignature->Register, false, modifiedShader, mMData);
	//--------------------

	ProjectionMatricesMap& testMap = testShaderData.matrixData.cb;

	bool testOK = true;

	//---- is shader mono ? ---
	if(testMap.size() == 0 && trueMap.size() == 0)
	{
		cout << "\tShader is mono\n";
	}
	else if(trueMap.size() == 0)
	{
		cout << "\tShader should be mono\n";
		testOK = false;
	}
	else
	{
		if(testMap.size() == 0)
		{
			cout << "\tWe not found matrix\n";
		}

		//--- Check for incorrect indices in testMap ---
		for(auto itTest = testMap.begin(); itTest != testMap.end(); itTest++)
		{
			int i = itTest->first;
			if(trueMap.count(i) == 0)
			{
				cout << "\tIncorrect map index " << i << " found\n";
				testOK = false;
				continue;
			}
		}

		//--- Check matrices for correct indices in testMap ---
		for(auto itTrue = trueMap.begin(); itTrue != trueMap.end(); itTrue++)
		{
			int i = itTrue->first;
			if(testMap.count(i) == 0)
			{
				cout << "\tMap index " << i << " not found\n";
				testOK = false;
				continue;
			}

			//--- Check vector size for each map index ---
			if(trueMap[i].size() != testMap[i].size())
			{
				cout << "\tMap index is: " << i;
				cout << "  True matrix count is: " << trueMap[i].size() << ", but analyzer matrix count is: " << testMap[i].size() << "\n";
				testOK = false;
				continue;
			}

			//--- Check each matrix in vector ---
			for(UINT j=0; j< trueMap[i].size(); j++)
			{
				testOK = (trueMap[i][j].matrixRegister == testMap[i][j].matrixRegister);
				if (testOK)
				{
					testOK = trueMap[i][j].matrixIsTransposed == testMap[i][j].matrixIsTransposed;
					if (testOK)
					{
						testOK = trueMap[i][j].incorrectProjection == testMap[i][j].incorrectProjection;
						if (!testOK)
						{
							cout << "\tTrue matrix[" << i << "]" << "[" << j << "]" << ": incorrectProjection = " << trueMap[i][j].incorrectProjection << " but\n,";
							cout << "\tTest matrix[" << i << "]" << "[" << j << "]" << ": incorrectProjection = " << testMap[i][j].incorrectProjection << "\n";
						}
					}
					else
					{
						cout << "\tTrue matrix[" << i << "]" << "[" << j << "]" << ": is transposed = " << trueMap[i][j].matrixIsTransposed << " but\n,";
						cout << "\tTest matrix[" << i << "]" << "[" << j << "]" << ": is transposed = " << testMap[i][j].matrixIsTransposed << "\n";
					}
				}
				else
				{
					cout << "\tTrue matrix[" << i << "]" << "[" << j << "]" << ": matrix Register = " << trueMap[i][j].matrixRegister << " but\n,";
					cout << "\tTest matrix[" << i << "]" << "[" << j << "]" << ": matrix Register = " << testMap[i][j].matrixRegister << "\n";
				}
			}
		}
	}
	if(testOK)
		cout << "Test PASSED" << endl << endl;
	else
	{
		cout << "\t----------------" << endl;

		if( pOutputSignature )
		{
			cout << "\tX Graph" << endl;
			TGraph graph;
			TStartVerticesArray arr;
			CreateShaderTree( shList, pOutputSignature->Register, graph, arr, ShaderOperand::X_STATE );

#if 0
			boost::GraphvizDigraph vizGraph;
			GraphVizShaderTree( graph, vizGraph );
			boost::write_graphviz( std::cout, vizGraph );
#endif
		}

		cout << endl;
		shader_analyzer::PrintShader( shList, std::cout, "\t" );

		cout << "Test FAILED" << endl << endl;
	}

	return testOK;
}

int _tmain(int argc, _TCHAR* argv[])
{
	TiXmlDocument doc( "shaderTests.xml" );
	if (!doc.LoadFile())
	{
		cout << "File not found or corrupted\n";
		return 1;
	}
	TiXmlNode* node = doc.FirstChild("Tests");
	for( TiXmlElement* item = node->FirstChildElement("Test"); item; item = item->NextSiblingElement("Test") )
	{
		ProjectionMatricesMap trueMap;
		int shaderIndex;
		item->QueryIntAttribute("Index", &shaderIndex);
		const char* declName = item->FirstChildElement("Declaration")->Attribute("File");
		const char* codeName = item->FirstChildElement("Shader")->Attribute("File");
		WCHAR declNameT[MAX_PATH], codeNameT[MAX_PATH];
		common::utils::UTF8ToUCS2(declName, declNameT);
		common::utils::UTF8ToUCS2(codeName, codeNameT);
		UINT* shaderCode, *pSignaturesBuffer;
		D3D10DDIARG_STAGE_IO_SIGNATURES signatures;
		if(ReadShaderFromFile(codeNameT, declNameT, L"Shaders", shaderCode, pSignaturesBuffer))
		{			
			signatures.NumInputSignatureEntries  = pSignaturesBuffer[0];
			signatures.NumOutputSignatureEntries = pSignaturesBuffer[1];
			signatures.pInputSignature = (D3D10DDIARG_SIGNATURE_ENTRY*)&pSignaturesBuffer[2];
			signatures.pOutputSignature = (D3D10DDIARG_SIGNATURE_ENTRY*)&pSignaturesBuffer[2] + 
				signatures.NumInputSignatureEntries;
			int shaderIsMono;
			item->FirstChildElement("IsMono")->QueryIntAttribute("Value", (int*)&shaderIsMono);
			if(!shaderIsMono)
			{
				TiXmlNode* nodeCBs = item->FirstChild("CBuffers");
				for( TiXmlElement* itemCB = nodeCBs->FirstChildElement("CBuffer"); itemCB; itemCB = itemCB->NextSiblingElement("CBuffer") )
				{
					int CBIndex;
					itemCB->QueryIntAttribute("Value", &CBIndex);
					ShaderCBs<ProjectionMatrixData>::CBMatrices& CBM = trueMap[CBIndex];
					TiXmlNode* nodeMX = itemCB->FirstChild("Matrices");
					for( TiXmlElement* itemMX = nodeMX->FirstChildElement("Matrix"); itemMX; itemMX = itemMX->NextSiblingElement("Matrix") )
					{
						ProjectionMatrixData md;
						itemMX->QueryIntAttribute("Register",				(int*)&md.matrixRegister);
						itemMX->QueryIntAttribute("IsTransposed",			(int*)&md.matrixIsTransposed);
						itemMX->QueryIntAttribute("IncorrectProjection",	(int*)&md.incorrectProjection);
						CBM.push_back(md);
					}
		
				}
			}
			try
			{
				if(!ShaderTest(trueMap, shaderCode, &signatures, shaderIndex) )
					Beep(750, 300);
					
			}
			catch ( std::exception& )
			{
				Beep(1,50);
			}
			delete shaderCode;
			delete pSignaturesBuffer;
		}
		else
			cout << "Can't read shader files: " << codeName << ", " << declName << "\n";

	}
	_getch();
	return 0;
}
