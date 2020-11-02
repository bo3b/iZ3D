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
#include "stdafx.h"
#include "NVPanelAPI.h"
#include <shlobj.h>
#include <shlwapi.h>
#include <stdio.h>

// NVidia part
typedef struct GAMMARAMP
{
	WORD wRed [256];
	WORD wGreen[256];
	WORD wBlue [256];
} GAMMARAMP, *PGAMMARAMP;

bool SetGammaNVidia()
{
	bool result = false;
	HINSTANCE hCpl = NULL;
	fNvColorSetGammaRamp pSetGamma = NULL;
	GAMMARAMP Gamma;
	memset( &Gamma, 0, sizeof(Gamma) );
	
	// Load the NVIDIA control panel applet. This from where the
	// gamma functions are exported.
	hCpl = LoadLibrary( _T("nvcpl.dll") );
	if( hCpl == NULL )
		return result;

	pSetGamma = (fNvColorSetGammaRamp)GetProcAddress( hCpl, "NvColorSetGammaRamp" );
	if( pSetGamma == NULL )
	{
		FreeLibrary( hCpl );
		return result;
	}
	
	for( int i = 0; i < 256; i++ )
	{
		Gamma.wRed  [ i ] = i << 8 | i;
		Gamma.wGreen[ i ] = i << 8 | i;
		Gamma.wBlue [ i ] = i << 8 | i;
	}
	// Set identity gamma 
	if(pSetGamma( "all", 0, &Gamma))
		result = true;
/*
	//--- just for test ---
	fNvGetDisplayInfo GetDisplayInfo = NULL;
	NVDISPLAYINFO displayInfo = { sizeof(NVDISPLAYINFO),  NVDISPLAYINFO1_ALL, 0, NVDISPLAYINFO2_ALL };
	NVDISPLAYINFO displayInfo1 = displayInfo, displayInfo2 = displayInfo;
	GetDisplayInfo = (fNvGetDisplayInfo)GetProcAddress(hCpl, "NvGetDisplayInfo");
	if (GetDisplayInfo)
	{
		GetDisplayInfo("aa", &displayInfo1 );
		GetDisplayInfo("ab", &displayInfo2 );
	}
*/
	FreeLibrary( hCpl );
	return result;
}

bool SetGammaATI()
{
	bool result = false;
	TCHAR xmlPath[MAX_PATH];
	if(SUCCEEDED(SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA, NULL, SHGFP_TYPE_CURRENT, xmlPath)))
	{
		StrCat(xmlPath, _T("\\ATI\\ACE\\Profiles.xml") );
		HANDLE hFile = CreateFile(xmlPath, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
		if(hFile)
		{
			DWORD fileSize = GetFileSize(hFile, NULL);
			if(fileSize != INVALID_FILE_SIZE)
			{
				char* p = DNew char[fileSize];
				if(p)
				{
					DWORD NumberOfBytesRead;
					if(ReadFile(hFile, p, fileSize, &NumberOfBytesRead, NULL))
					{
						char* s = strstr(p, "Property name=\"LUT_Desktop\"");
						if(s)
						{
							s = strstr(s, "value=\"");
							if(s)
							{
								s += sizeof("value=\"") - 1;
								int r, g, b;
								result = true;
								for(int i=0; i< 256; i++)
								{
									DWORD color = i << 8 | i;
									sscanf_s(s, "%d,%d,%d,", &r, &g, &b);
									if(color != r || color  != g || color  != b)
									{
										result = false;
										MessageBox(NULL, _T("Gamma will be reset each time when game started."), _T("ATI gamma warning"), MB_OK);
										break;
									}
									// jump to next r,g,b
									s = strchr(s, ',') + 1;
									if(s > (char*)1)
										s = strchr(s, ',') + 1;
									if(s > (char*)1)
										s = strchr(s, ',') + 1;
									if(s <= (char*)1)
										break;
								} 
							}
						}
					}
					delete [] p;
				}
			}
			CloseHandle(hFile);
		}
	}
	return result;
}

void __declspec(dllexport) CALLBACK SetGamma(int ShowWarning)
{
	if(SetGammaNVidia()) return;
	if(ShowWarning)
	{
		if(SetGammaATI())
			return;
	}
}

