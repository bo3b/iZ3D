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
// DX9ShaderAnalyzingTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <windows.h>
#include <tinyxml.h>
#include <iostream>
#include "States.h"
#include "DX9ShaderConverter.h"
#include <boost/test/unit_test.hpp>
#include <boost/test/parameterized_test.hpp>
using boost::unit_test_framework::test_suite;

using namespace std;

static int index = 0;

// for example:  version = 0x200
inline DWORD GetShaderVersion(const char* pShaderText)
{
	char* p = strrstr(pShaderText, "vs_");
	if (p == NULL)
		p = strrstr(pShaderText, "vs.");
	if(p)
	{
		return ((p[3] - '0') << 8) + ((p[5] == 'x' || p[5] == 'X') ? 1 : p[5] - '0');
	}
	return 0;
}

struct TEST_DATA
{
	VS_PRIVATE_DATA sd;
	std::string shaderText;
	bool mustSuccess;
};

std::vector<TEST_DATA> testsData;

void ShaderTest( TEST_DATA& Data)
{
	VS_PRIVATE_DATA &sd = Data.sd;
	const char* shaderText = Data.shaderText.c_str();
	bool matrixFound = Data.mustSuccess;
	//if (sd.shaderGlobalIndex != 45)
	//	return true;
	index = sd.shaderGlobalIndex;
	ShaderAnalyzer sa; 
	DWORD shaderVersion = GetShaderVersion(shaderText);
	bool isMono = false;
	DWORD textures = 0xFFFFFFFF;
	ShaderMatrices matrices;
	ZeroMemory(&matrices, sizeof ShaderMatrices);
	cout << "Test " << sd.shaderGlobalIndex << "\n";
	ShaderAnalysisResult analyseResult = sa.Analyze(shaderVersion, shaderText, matrices, textures);
	if(analyseResult == AnalysisSharedIsMono)
		isMono = true;
	bool matrixFoundResult = analyseResult == AnalysisMatrixFound;
	BOOST_CHECK_EQUAL(matrixFoundResult, matrixFound);
	if(matrixFoundResult == matrixFound)
	{
		BOOST_CHECK_EQUAL(isMono, sd.shaderIsMono);
		if (!isMono)
		{
			BOOST_CHECK_EQUAL(matrices.matrixSize, sd.matricesData.matrixSize);
			for(DWORD j = 0; j < sd.matricesData.matrixSize; j++)
			{
				BOOST_CHECK_EQUAL(matrices.matrix[j].matrixRegister, sd.matricesData.matrix[j].matrixRegister);
				BOOST_CHECK_EQUAL(matrices.matrix[j].matrixIsTransposed, sd.matricesData.matrix[j].matrixIsTransposed);
				BOOST_CHECK_EQUAL(matrices.matrix[j].incorrectProjection, sd.matricesData.matrix[j].incorrectProjection);
			}
		}
	}
}

test_suite*
	init_unit_test_suite( int /*argc*/, char* /*argv*/[] ) {
	test_suite* test= BOOST_TEST_SUITE( "DX9ShaderAnalyzingTest" );

	TiXmlDocument doc( "shaderTests.xml" );
	if (!doc.LoadFile())
	{ 
		cout << "File not found or corrupted\n";
		return test;
	}

	testsData.reserve(100);

// 	LARGE_INTEGER freq, start, end;
// 	QueryPerformanceFrequency(&freq);
// 	QueryPerformanceCounter(&start);
	TiXmlNode* node = doc.FirstChild("Tests");
	for( TiXmlElement* item = node->FirstChildElement("Test"); item; item = item->NextSiblingElement("Test") )
	{
		testsData.push_back(TEST_DATA());
		TEST_DATA* pData = &testsData.back();
		ZeroMemory(&pData->sd, sizeof(pData->sd));
		item->QueryIntAttribute("Index", (int*)&pData->sd.shaderGlobalIndex);
		pData->mustSuccess = true;
		int val;
		item->QueryIntAttribute("MustSuccess", &val);
		pData->mustSuccess = val != 0;
		pData->shaderText = item->FirstChildElement("Shader")->Attribute("Text");
		//TiXmlText* text = item->FirstChild("Shader")->FirstChild()->ToText();
		//const char* shader = text->Value();
		item->FirstChildElement("IsMono")->QueryIntAttribute("Value", (int*)&pData->sd.shaderIsMono);
		node = item->FirstChild("Matrices");
		auto& data = pData->sd.matricesData;
		node->ToElement()->QueryIntAttribute("Count", (int*)&data.matrixSize);
		DWORD i = 0;
		if (data.matrixSize > 0)
		{
			for( TiXmlElement* item2 = node->FirstChildElement("Matrix"); item2; item2 = item2->NextSiblingElement("Matrix") )
			{
				item2->FirstChildElement("Register")->QueryIntAttribute("Value", (int*)&data.matrix[i].matrixRegister);
				item2->FirstChildElement("IsTransposed")->QueryIntAttribute("Value", (int*)&data.matrix[i].matrixIsTransposed);
				item2->FirstChildElement("IncorrectProjection")->QueryIntAttribute("Value", (int*)&data.matrix[i].incorrectProjection);
				i++;
				if (i >= data.matrixSize )
					break;
				else if (i >= SHADER_MATRIX_SIZE)
					break;
			}
		}
		i++;
	}
	test->add( BOOST_PARAM_TEST_CASE( ShaderTest, 
		testsData.begin(),  testsData.end()  ) );
// 	QueryPerformanceCounter(&end);
// 	cout << "Tests: total " << testCount << " / failed " << failCount << "\n";
// 	cout << "Total time: " << (end.QuadPart - start.QuadPart) * 1000 / freq.QuadPart << " ms\n";

	return test;
}

void LogFormatMessage(const char *fmt, ...)
{
}

DWORD CalculateCRC32(const void *data, DWORD Size)
{
	return 0;
}

#ifdef BOOST_NO_EXCEPTIONS

// to support compilation without exceptions /EHsc
void boost::throw_exception(const stdext::exception& ex)
{
}

#endif // BOOST_NO_EXCEPTIONS
