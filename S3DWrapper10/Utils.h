#pragma once

#include "CallGuard.h"
#include "..\CommonUtils\System.h"
#include "..\S3DAPI\ConvertImages.h"
 
struct D3D10DDIARG_STAGE_IO_SIGNATURES;
 
 
//--- shader write/read functions ---
void* ReadBufferFromFile(const TCHAR* fileName);
void  WriteBufferToFile (const TCHAR* fileName, const void* buffer, UINT codeSize);
bool  ReadShaderFromFile(const TCHAR* codeName, const TCHAR* declName, const TCHAR* subDirName, UINT* &shaderCode, UINT* &pSignaturesBuffer);
void  WriteShaderToFile (const TCHAR* codeName, const TCHAR* declName, const TCHAR* subDirName, CONST UINT* shaderCode, CONST D3D10DDIARG_STAGE_IO_SIGNATURES* pSignatures);
 
CONST TCHAR* GetStereoImageFileExtension( );
CONST TCHAR* GetImageFileExtension( );

bool SaveImageToFile( TCHAR* filename, DXGI_FORMAT format, DWORD width, DWORD height, char* buffer, UINT rowPitch );
 
enum TD3DVersion
{
TD3DVersion_Unknown,
TD3DVersion_10_0,
TD3DVersion_10_1,
TD3DVersion_11_0,
};
 
TD3DVersion GetD3DVersionFromInterface( UINT Interface );
 
bool IsWindows7();
