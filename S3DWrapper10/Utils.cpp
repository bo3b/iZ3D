#include "stdafx.h"
#include <fcntl.h>
#include <io.h>
#include <sys/stat.h>
#include <share.h>
#include <direct.h>
#include <Shlwapi.h>
#include <crtdbg.h>
#include <IL/il.h>
#include <IL/ilu.h>
#include "Utils.h"

#ifdef TEST
#define DNew new
#define DEBUG_MESSAGE __noop
#define EnumToString __noop
#endif

// --- shader write/read functions ---
void* ReadBufferFromFile(const TCHAR* fileName)
{
	void* s = NULL;
	int fileHandle;
	_tsopen_s(&fileHandle, fileName, _O_RDONLY | _O_BINARY, _SH_DENYWR, _S_IREAD);
	if(fileHandle != -1)
	{
		long fileSize = _lseek(fileHandle, 0, SEEK_END);
		_lseek(fileHandle, 0, SEEK_SET);
		s = DNew char[fileSize];
		if(_read(fileHandle, s, fileSize) != fileSize)
		{
			delete s;
			s = NULL;
		}
		_close(fileHandle);
	}
	return s;
}

void WriteBufferToFile(const TCHAR* fileName, const void* buffer, UINT codeSize)
{
	int fileHandle;
	_tsopen_s(&fileHandle, fileName, _O_CREAT | _O_WRONLY | _O_BINARY, _SH_DENYRW, _S_IREAD | _S_IWRITE);
	if(fileHandle != -1)
	{
		_write(fileHandle, buffer, codeSize);
		_close(fileHandle);
	}
}

bool ReadShaderFromFile(const TCHAR* codeName, const TCHAR* declName, const TCHAR* subDirName, UINT* &shaderCode, UINT* &pSignaturesBuffer)
{
	TCHAR fullNameF[MAX_PATH];
	TCHAR fullNameD[MAX_PATH];
	_tcscpy_s(fullNameF, _countof(fullNameF), subDirName);
	PathAppend(fullNameF, codeName);
	_tcscpy_s(fullNameD, _countof(fullNameD), subDirName);
	PathAppend(fullNameD, declName);
	shaderCode = (UINT*)ReadBufferFromFile(fullNameF);
	pSignaturesBuffer = (UINT*)ReadBufferFromFile(fullNameD);
	if(shaderCode && pSignaturesBuffer)
		return true;
	return false;
}

#ifndef TEST
void WriteShaderToFile(const TCHAR* codeName, const TCHAR* declName, const TCHAR* subDirName, CONST UINT* shaderCode, CONST D3D10DDIARG_STAGE_IO_SIGNATURES* pSignatures)
{
	TCHAR fullNameF[MAX_PATH];
	_tcscpy_s(fullNameF, _countof(fullNameF), subDirName);
	_tmkdir(subDirName);
	PathAppend(fullNameF, codeName);
	WriteBufferToFile(fullNameF, shaderCode, sizeof(DWORD) * shaderCode[1]);
	// --- write shader declarations ---
	if (!pSignatures)
		return;
	TCHAR fullNameD[MAX_PATH];
	_tcscpy_s(fullNameD, _countof(fullNameD), subDirName);
	PathAppend(fullNameD, declName);
	UINT declBufferSize = 2*sizeof(UINT) + sizeof(D3D10DDIARG_SIGNATURE_ENTRY) * 
						  (pSignatures->NumInputSignatureEntries + pSignatures->NumOutputSignatureEntries);
	UINT* p = (UINT*)DNew BYTE[declBufferSize];
	p[0] = pSignatures->NumInputSignatureEntries;
	p[1] = pSignatures->NumOutputSignatureEntries;
	BYTE* p1 = (BYTE*)&p[2];
	BYTE* p2 = p1 + pSignatures->NumInputSignatureEntries * sizeof(D3D10DDIARG_SIGNATURE_ENTRY);
	memcpy(p1, pSignatures->pInputSignature, pSignatures->NumInputSignatureEntries * sizeof(D3D10DDIARG_SIGNATURE_ENTRY));
	memcpy(p2, pSignatures->pOutputSignature, pSignatures->NumOutputSignatureEntries * sizeof(D3D10DDIARG_SIGNATURE_ENTRY));
	WriteBufferToFile(fullNameD, (BYTE*)p, declBufferSize);
	delete p;
}
#endif

#ifndef DEBUGANALYSING
bool SaveImageToFile( TCHAR* filename, DXGI_FORMAT format, DWORD width, DWORD height, char* buffer, UINT rowPitch )
{
#if 0 && !defined(FINAL_RELEASE)
	// This code only for debugging texture saving
	DEBUG_MESSAGE(_T("SaveImageToFile binformat: %S\n"), EnumToString(format));
	TCHAR filenameBin[MAX_PATH];
	_stprintf_s( filenameBin, MAX_PATH, _T("%s.bin"), filename );
	FILE *filePtr;	
	_wfopen_s(&filePtr, filenameBin, _T("wb"));
	// 32 bytes header
	DWORD fourcc = MAKEFOURCC('i','Z','3','D');
	fwrite(&fourcc,			sizeof(fourcc), 1, filePtr);
	DWORD len = rowPitch * height;
	fwrite(&len,			sizeof(len), 1, filePtr);
	fwrite(&format,			sizeof(format), 1, filePtr);
	fwrite(&rowPitch,		sizeof(rowPitch), 1, filePtr);
	fwrite(&width,			sizeof(width), 1, filePtr);
	fwrite(&height,			sizeof(height), 1, filePtr);
	DWORD reserved = 0;
	fwrite(&reserved,		sizeof(reserved), 1, filePtr);
	fwrite(&reserved,		sizeof(reserved), 1, filePtr);
	fwrite(buffer,			len, 1, filePtr);
	fclose(filePtr);
#endif

	bool isFormatOkay = false;
	DXGI_FORMAT dstFormat = format;
	ILubyte bytesPerPixel = 4;
	ILenum ILFormat = IL_RGBA;
	ILenum dataType = IL_UNSIGNED_BYTE;
	void* byteData = NULL;
	switch( format )
	{
	case DXGI_FORMAT_A8_UNORM:
	case DXGI_FORMAT_R8_UNORM:	
		{
			DEBUG_MESSAGE(_T("SaveImageToFile format: %S\n"), EnumToString(format));
			isFormatOkay = true;
			int bytePitch = width * sizeof(byte) * 4;
			int byteDataSize = bytePitch * height;
			byteData = _aligned_malloc(byteDataSize, 4);
			if( byteData == NULL )
			{
				DEBUG_MESSAGE(_T("SaveImageToFile failed, cannot allocate memory\n"));
				return false;
			}
			ConvertImageFrom_8_BYTE_To_A8R8G8B8_TYPELESS(buffer, byteData, width, height, rowPitch);
			buffer = (char*)byteData;
			rowPitch = bytePitch;
			dstFormat = DXGI_FORMAT_R8G8B8A8_TYPELESS;
		}
		break;
	case DXGI_FORMAT_R8G8_UNORM:
		{
			DEBUG_MESSAGE(_T("SaveImageToFile format: %S\n"), EnumToString(format));
			isFormatOkay = true;
			int bytePitch = width * sizeof(byte) * 4;
			int byteDataSize = bytePitch * height;
			byteData = _aligned_malloc(byteDataSize, 4);
			if( byteData == NULL )
			{
				DEBUG_MESSAGE(_T("SaveImageToFile failed, cannot allocate memory\n"));
				return false;
			}
			ConvertImageFrom_R8G8_UNORM_To_A8R8G8B8_TYPELESS(buffer, byteData, width, height, rowPitch);
			buffer = (char*)byteData;
			rowPitch = bytePitch;
			dstFormat = DXGI_FORMAT_R8G8B8A8_TYPELESS;
		}
	case DXGI_FORMAT_B8G8R8A8_TYPELESS:
	case DXGI_FORMAT_B8G8R8A8_UNORM:
	case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
	case DXGI_FORMAT_B8G8R8X8_TYPELESS:
	case DXGI_FORMAT_B8G8R8X8_UNORM:
	case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
		{
			DEBUG_MESSAGE(_T("SaveImageToFile format: %S\n"), EnumToString(format));
			isFormatOkay = true;
			int bytePitch = width * sizeof(byte) * 4;
			int byteDataSize = bytePitch * height;
			byteData = _aligned_malloc(byteDataSize, 4);
			if( byteData == NULL )
			{
				DEBUG_MESSAGE(_T("SaveImageToFile failed, cannot allocate memory\n"));
				return false;
			}
			ConvertImageFrom_B8G8R8A8_UNORM_To_A8R8G8B8_TYPELESS(buffer, byteData, width, height, rowPitch);
			buffer = (char*)byteData;
			rowPitch = bytePitch;
			dstFormat = DXGI_FORMAT_R8G8B8A8_TYPELESS;
		}
		break;
	case DXGI_FORMAT_R8G8B8A8_TYPELESS:
	case DXGI_FORMAT_R8G8B8A8_UNORM:
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
	case DXGI_FORMAT_R8G8B8A8_UINT:	
		isFormatOkay = true;
		break;
	case DXGI_FORMAT_R8G8B8A8_SNORM:
	case DXGI_FORMAT_R8G8B8A8_SINT:	
		isFormatOkay = true;
		dataType = IL_BYTE;
		break;
		// --- try to convert in R8G8B8A8 format ---
	case DXGI_FORMAT_R10G10B10A2_TYPELESS:
	case DXGI_FORMAT_R10G10B10A2_UNORM:
	case DXGI_FORMAT_R10G10B10A2_UINT:
		{
			DEBUG_MESSAGE(_T("SaveImageToFile format: %S\n"), EnumToString(format));
			isFormatOkay = true;
			int bytePitch = width * sizeof(byte) * 4;
			int byteDataSize = bytePitch * height;
			byteData = _aligned_malloc(byteDataSize, 4);
			if( byteData == NULL )
			{
				DEBUG_MESSAGE(_T("SaveImageToFile failed, cannot allocate memory\n"));
				return false;
			 }
			ConvertImageFrom_R10G10B10A2_UNORM_To_A8R8G8B8_TYPELESS(buffer, byteData, width, height, rowPitch);
			buffer = (char*)byteData;
			rowPitch = bytePitch;
			dstFormat = DXGI_FORMAT_R8G8B8A8_TYPELESS;
			break;
		}
	// --- try to convert in R8G8B8A8 format ---
	case DXGI_FORMAT_R16G16_FLOAT:
	case DXGI_FORMAT_R16G16B16A16_FLOAT:
	case DXGI_FORMAT_R32G32_FLOAT:
	case DXGI_FORMAT_R11G11B10_FLOAT:
		{
			DEBUG_MESSAGE(_T("SaveImageToFile format: %S\n"), EnumToString(format));
			isFormatOkay = true;
			int floatPitch = width * sizeof(float) * 4;
			int floatDataSize = floatPitch * height;
			void* floatData = _aligned_malloc(floatDataSize, 16);
			if( floatData == NULL )
			{
				DEBUG_MESSAGE(_T("SaveImageToFile failed, cannot allocate memory\n"));
				return false;
			}
			switch(format)
			{
			case DXGI_FORMAT_R16G16_FLOAT:
				ConvertImageFrom_R16G16_FLOAT_To_R32G32B32A32_FLOAT(buffer, floatData, width, height, rowPitch);
				break;
			case DXGI_FORMAT_R32G32_FLOAT:
				ConvertImageFrom_R32G32_FLOAT_To_R32G32B32A32_FLOAT(buffer, floatData, width, height, rowPitch);
				break;
			case DXGI_FORMAT_R16G16B16A16_FLOAT:
				ConvertImageFrom_R16G16B16A16_FLOAT_To_R32G32B32A32_FLOAT(buffer, floatData, width, height, rowPitch);
				break;
			case DXGI_FORMAT_R11G11B10_FLOAT:
				ConvertImageFrom_R11G11B10_FLOAT_To_R32G32B32A32_FLOAT(buffer, floatData, width, height, rowPitch);
				break;
			}
			int bytePitch = width * sizeof(byte) * 4;
			int byteDataSize = bytePitch * height;
			byteData = _aligned_malloc(byteDataSize, 4);
			if( byteData == NULL )
			{
				_aligned_free(floatData);
				DEBUG_MESSAGE(_T("SaveImageToFile failed, cannot allocate memory\n"));
				return false;
			}
			ConvertImageFrom_R32G32B32A32_FLOAT_To_A8R8G8B8_TYPELESS(floatData, byteData, width * height);
			_aligned_free(floatData);
			buffer = (char*)byteData;
			rowPitch = bytePitch;
			dstFormat = DXGI_FORMAT_R8G8B8A8_TYPELESS;
			break;
		}
	case DXGI_FORMAT_R16_TYPELESS:
	case DXGI_FORMAT_R16_FLOAT:
		{
			DEBUG_MESSAGE(_T("SaveImageToFile format: %S\n"), EnumToString(format));
			isFormatOkay = true;
			int floatPitch = width * sizeof(float);
			int floatDataSize = floatPitch * height;
			void* floatData = _aligned_malloc(floatDataSize, 16);
			if( floatData == NULL )
			{
				DEBUG_MESSAGE(_T("SaveImageToFile failed, cannot allocate memory\n"));
				return false;
			}
			ConvertImageFrom_R16_FLOAT_To_R32_FLOAT(buffer, floatData, width, height, rowPitch);
			int bytePitch = width * sizeof(byte) * 4;
			int byteDataSize = bytePitch * height;
			byteData = _aligned_malloc(byteDataSize, 16);
			if( byteData == NULL )
			{
				_aligned_free(floatData);
				DEBUG_MESSAGE(_T("SaveImageToFile failed, cannot allocate memory\n"));
				return false;
			}
			ConvertImageFrom_R32_FLOAT_To_A8R8G8B8_TYPELESS(floatData, byteData, width, height, floatPitch, bytePitch);
			_aligned_free(floatData);
			buffer = (char*)byteData;
			rowPitch = bytePitch;
			dstFormat = DXGI_FORMAT_R8G8B8A8_TYPELESS;
			break;
		}
	case DXGI_FORMAT_R32_FLOAT:
		{
			DEBUG_MESSAGE(_T("SaveImageToFile format: %S\n"), EnumToString(format));
			isFormatOkay = true;
			int bytePitch = width * sizeof(byte) * 4;
			int byteDataSize = bytePitch * height;
			byteData = _aligned_malloc(byteDataSize, 16);
			if( byteData == NULL )
			{
				DEBUG_MESSAGE(_T("SaveImageToFile failed, cannot allocate memory\n"));
				return false;
			}
			ConvertImageFrom_R32_FLOAT_To_A8R8G8B8_TYPELESS(buffer, byteData, width, height, rowPitch, bytePitch);
			buffer = (char*)byteData;
			rowPitch = bytePitch;
			dstFormat = DXGI_FORMAT_R8G8B8A8_TYPELESS;
			break;
		}
	case DXGI_FORMAT_R32G32B32A32_FLOAT:
		{
			int floatPitch = width * sizeof(float) * 4;
			if (floatPitch != rowPitch)
				break;
			DEBUG_MESSAGE(_T("SaveImageToFile format: %S\n"), EnumToString(format));
			isFormatOkay = true;
			int bytePitch = width * sizeof(byte) * 4;
			int byteDataSize = bytePitch * height;
			byteData = _aligned_malloc(byteDataSize, 4);
			if( byteData == NULL )
			{
				DEBUG_MESSAGE(_T("SaveImageToFile failed, cannot allocate memory\n"));
				return false;
			}
			ConvertImageFrom_R32G32B32A32_FLOAT_To_A8R8G8B8_TYPELESS(buffer, byteData, width * height);
			buffer = (char*)byteData;
			rowPitch = bytePitch;
			dstFormat = DXGI_FORMAT_R8G8B8A8_TYPELESS;
			break;
		}
	case DXGI_FORMAT_R24G8_TYPELESS:
	case DXGI_FORMAT_D24_UNORM_S8_UINT:
	case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
	case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
		{
			DEBUG_MESSAGE(_T("SaveImageToFile format: %S\n"), EnumToString(format));
			isFormatOkay = true;
			int floatPitch = width * sizeof(float) * 4;
			int floatDataSize = floatPitch * height;
			void* floatData = _aligned_malloc(floatDataSize, 16);
			if( floatData == NULL )
			{
				DEBUG_MESSAGE(_T("SaveImageToFile failed, cannot allocate memory\n"));
				return false;
			}
			ConvertImageFrom_R24G8_UNORM_To_R32G32B32A32_FLOAT(buffer, floatData, width, height, rowPitch);
			int bytePitch = width * sizeof(byte) * 4;
			int byteDataSize = bytePitch * height;
			byteData = _aligned_malloc(byteDataSize, 4);
			if( byteData == NULL )
			{
				_aligned_free(floatData);
				DEBUG_MESSAGE(_T("SaveImageToFile failed, cannot allocate memory\n"));
				return false;
			}
			ConvertImageFrom_R32G32B32A32_FLOAT_To_A8R8G8B8_TYPELESS(floatData, byteData, width * height);
			_aligned_free(floatData);
			buffer = (char*)byteData;
			rowPitch = bytePitch;
			dstFormat = DXGI_FORMAT_R8G8B8A8_TYPELESS;
			break;
		}
	case DXGI_FORMAT_BC1_TYPELESS:
	case DXGI_FORMAT_BC1_UNORM:
	case DXGI_FORMAT_BC1_UNORM_SRGB:
		{
			DEBUG_MESSAGE(_T("SaveImageToFile format: %S\n"), EnumToString(format));
			isFormatOkay = true;
			int bytePitch = width * sizeof(byte) * 4;
			int byteDataSize = bytePitch * height;
			byteData = _aligned_malloc(byteDataSize, 4);
			if( byteData == NULL )
			{
				DEBUG_MESSAGE(_T("SaveImageToFile failed, cannot allocate memory\n"));
				return false;
			}
			ConvertImageFrom_BCx_To_A8R8G8B8_TYPELESS(buffer, byteData, width, height, rowPitch, squish::kDxt1);
			buffer = (char*)byteData;
			rowPitch = bytePitch;
			dstFormat = DXGI_FORMAT_R8G8B8A8_TYPELESS;
		}
		break;
	case DXGI_FORMAT_BC2_TYPELESS:
	case DXGI_FORMAT_BC2_UNORM:
	case DXGI_FORMAT_BC2_UNORM_SRGB:
		{
			DEBUG_MESSAGE(_T("SaveImageToFile format: %S\n"), EnumToString(format));
			isFormatOkay = true;
			int bytePitch = width * sizeof(byte) * 4;
			int byteDataSize = bytePitch * height;
			byteData = _aligned_malloc(byteDataSize, 4);
			if( byteData == NULL )
			{
				DEBUG_MESSAGE(_T("SaveImageToFile failed, cannot allocate memory\n"));
				return false;
			}
			ConvertImageFrom_BCx_To_A8R8G8B8_TYPELESS(buffer, byteData, width, height, rowPitch, squish::kDxt3);
			buffer = (char*)byteData;
			rowPitch = bytePitch;
			dstFormat = DXGI_FORMAT_R8G8B8A8_TYPELESS;
		}
		break;
	case DXGI_FORMAT_BC3_TYPELESS:
	case DXGI_FORMAT_BC3_UNORM:
	case DXGI_FORMAT_BC3_UNORM_SRGB:
		{
			DEBUG_MESSAGE(_T("SaveImageToFile format: %S\n"), EnumToString(format));
			isFormatOkay = true;
			int bytePitch = width * sizeof(byte) * 4;
			int byteDataSize = bytePitch * height;
			byteData = _aligned_malloc(byteDataSize, 4);
			if( byteData == NULL )
			{
				DEBUG_MESSAGE(_T("SaveImageToFile failed, cannot allocate memory\n"));
				return false;
			}
			ConvertImageFrom_BCx_To_A8R8G8B8_TYPELESS(buffer, byteData, width, height, rowPitch, squish::kDxt5);
			buffer = (char*)byteData;
			rowPitch = bytePitch;
			dstFormat = DXGI_FORMAT_R8G8B8A8_TYPELESS;
		}
		break;
	}

	if( !isFormatOkay )
	{
		DEBUG_MESSAGE(_T("SaveImageToFile failed, not supported format: %S\n"), EnumToString(dstFormat));
		return false;
	}	

	size_t WidthSize = width * GetBitWidthOfDXGIFormat( dstFormat ) / 8;
	_ASSERT(rowPitch >= WidthSize);
	if (rowPitch > WidthSize)
	{
		char* newData = (char*)_aligned_malloc(WidthSize * height, 16);
		char* pSrc = buffer;
		char* pDst = newData;
		for (DWORD i = 0; i < height; i++)
		{
			memcpy(pDst, pSrc, WidthSize);
			pSrc += rowPitch;
			pDst += WidthSize;
		}
		if (byteData)
			_aligned_free(byteData);
		byteData = newData;
		buffer = newData;
	}

	ILuint	ImgId;  
	ilGenImages(1, &ImgId);
	ilBindImage(ImgId);	
	ILboolean result = ilTexImage(width, height, 0, bytesPerPixel, ILFormat, dataType, buffer);
	if (byteData)
		_aligned_free(byteData);
	if (!result)
	{
		ilDeleteImages(1, &ImgId);  
		return false;
	}
	iluFlipImage();
	ilSave(IL_JPG, filename);
	ilDeleteImages(1, &ImgId);  
	return true;

	//SaveImageToBMP(filename, width, height, buffer);
}

/*
bool CheckCodeInDll( CONST TCHAR* DLL, UINT_PTR firstMethod )
{
	HMODULE hDLL = GetModuleHandle(DLL);
	if (hDLL)
	{
		MODULEINFO	mInfo;
		HANDLE hProcess = GetCurrentProcess();
		if(GetModuleInformation(hProcess, hDLL, &mInfo, sizeof(MODULEINFO)))
		{
			UINT_PTR start = (UINT_PTR)mInfo.lpBaseOfDll;
			UINT_PTR end   = start + mInfo.SizeOfImage;
			if (start <= firstMethod && firstMethod <= end)
				return true;
		}
	}
	return false;
}*/

#ifndef TEST
CONST TCHAR* GetStereoImageFileExtension( )
{
	switch ( gInfo.ScreenshotType )
	{
	case 1: // PNG
		return _T(".pns");
	case 2: // BMP
		return _T(".bmp");
	case 3: // DDS
		return _T(".dds");
	default: // JPG
		return _T(".jps");
	}
}

CONST TCHAR* GetImageFileExtension( )
{
	switch ( gInfo.ScreenshotType )
	{
	case 1: // PNG
		return _T(".png");
	case 2: // BMP
		return _T(".bmp");
	case 3: // DDS
		return _T(".dds");
	default: // JPG
		return _T(".jpg");
	}
}
#endif
#endif

bool IsWindows7()
{
	static int sWin7 = 2; // not initialized
	if (sWin7 == 2)
	{
		sWin7 = 0;
		if ((GetVersion() & 0x80000000) == 0)
		{
			OSVERSIONINFO verInfo;
			ZeroMemory(&verInfo, sizeof(OSVERSIONINFO));
			verInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
			GetVersionEx(&verInfo);
			if ((verInfo.dwMajorVersion == 6) && (verInfo.dwMinorVersion >= 1) || 
				(verInfo.dwMajorVersion > 6))
				sWin7 = 1;
		}
	}
	return sWin7 != 0;
}
