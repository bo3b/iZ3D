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

#include "windows.h"
#include "tinyxml.h"
#include <iostream>
#include "States.h"
#include <d3d9.h>
#include "DX9ShaderConverter.h"
#include "../../S3DAPI/GlobalData.h"
#include "../../S3DWrapper9/GlobalData.h"

using namespace std;

#ifdef _DEBUG
volatile long ProxyGuard::callLevel = 0;
#endif

#pragma warning(disable: 4273)
GlobalData gData;
GlobalInfo _gInfo;
ProfileData _g_ProfileData;
GlobalInfo& gInfo = _gInfo;
ProfileData& g_ProfileData = _g_ProfileData;

void iZ3DReleaseShutter() {}
void iZ3DStopShutter()    {}

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

bool ShaderTest(char* inputShaderText)
{
	bool AddZNearCheck = false;
	gInfo.VertexShaderModificationMethod = 2;

	DWORD shaderVersion = GetShaderVersion(inputShaderText);
	CComPtr<ID3DXBuffer> pureShader;
	CComPtr<ID3DXBuffer> pConvertedShader;
	CComPtr<ID3DXBuffer> pModifiedShader;
	DX9VertexShaderConverter sc;
	sc.PrepareShaderText(inputShaderText, &pureShader);
	char* pureShaderText = (char*)pureShader->GetBufferPointer();
	char* convertedShaderText = NULL;

	bool bSucceded = sc.Modify(shaderVersion, pureShaderText, &pModifiedShader, AddZNearCheck);
	if(!bSucceded && shaderVersion < 0x200)
	{
		sc.Convert_1_x_ShaderTo_2_x(shaderVersion, pureShaderText, &pConvertedShader);
		convertedShaderText = (char*)pConvertedShader->GetBufferPointer();
		shaderVersion = 0x200;
		sc.Modify(shaderVersion, convertedShaderText, &pModifiedShader, AddZNearCheck);
	}
	char* modifiedShaderText = pModifiedShader ? (char*)pModifiedShader->GetBufferPointer() : NULL;

	CComPtr<ID3DXBuffer> pCompiledShader;
	CComPtr<ID3DXBuffer> pCompilationErrors;
	CComPtr<IDirect3DVertexShader9> pShader;
	if(SUCCEEDED(D3DXAssembleShader(modifiedShaderText, (UINT)strlen(modifiedShaderText), NULL, NULL, 0, &pCompiledShader, &pCompilationErrors)))
	{
		//--- create stereo shader ---
		DWORD* modifiedShader = (DWORD*)pCompiledShader->GetBufferPointer();
		//if(SUCCEEDED(sc.CreateShader(modifiedShader, &pShader)))
			return true;
	}
	char* errorText = pCompilationErrors ? (char*)pCompilationErrors->GetBufferPointer() : NULL;
	__debugbreak();
	return false;
}

int _tmain(int argc, _TCHAR* argv[])
{
	FILE* fp = fopen("shaderTest.vsh", "r");
	if (!fp)
		return 1;

	char pST[65536];
	size_t size = fread(pST, sizeof(char), 65535, fp);
	pST[size] = '\0';	
	fclose(fp);
	ShaderTest(pST);
	return 0;
}

DWORD CalculateCRC32(const void *p, UINT size, UINT startValue)
{
	return 0;
}
