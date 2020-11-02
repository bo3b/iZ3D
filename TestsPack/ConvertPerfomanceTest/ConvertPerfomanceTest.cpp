// ConvertPerfomanceTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Utils.h"
#include <stdio.h>
#include <iostream>
#include <mmsystem.h>
#include <IL/il.h>
#include "S3DAPI\ConvertImages.h"

int TestR16(int iter)
{
	DXGI_FORMAT Format = DXGI_FORMAT_R16_TYPELESS;
	DWORD width, height, len,SysMemPitch,SysMemSlicePitch;
	FILE *filePtr;	
	if (_wfopen_s(&filePtr, L"r16f_data.bin", _T("rb")) != 0)
		if (_wfopen_s(&filePtr, L"..\\r16f_data.bin", _T("rb")) != 0)
			if (_wfopen_s(&filePtr, L"..\\..\\r16f_data.bin", _T("rb")) != 0)
				return 1;
	fread(&len,				sizeof(len), 1, filePtr);
	fread(&SysMemPitch,		sizeof(SysMemPitch), 1, filePtr);
	fread(&width,			sizeof(width), 1, filePtr);
	fread(&height,			sizeof(height), 1, filePtr);
	fread(&SysMemPitch,		sizeof(SysMemPitch), 1, filePtr);
	fread(&SysMemSlicePitch,sizeof(SysMemSlicePitch), 1, filePtr);
	char* pSysMem = new char[len];
	fread(pSysMem,			len, 1, filePtr);
	fclose(filePtr);

	// Initialize DevIL.
	ilInit();
	int floatPitch = width * sizeof(float);
	int floatDataSize = floatPitch * height;
	void* floatData = _aligned_malloc(floatDataSize, 16);
	if( floatData == NULL )
		return 2;
	int bytePitch = width * sizeof(char) * 4;
	int byteDataSize = bytePitch * height;
	void* byteData = _aligned_malloc(byteDataSize, 16);
	if( byteData == NULL )
	{
		_aligned_free(floatData);
		return 2;
	}
	LARGE_INTEGER freq, start, end;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&start);
	for (int i = 0; i < iter; i++)
	{
		ConvertImageFrom_R16_FLOAT_To_R32_FLOAT(pSysMem, floatData, width, height, SysMemPitch);
	}
	QueryPerformanceCounter(&end);
	std::cout << "Total time R16_FLOAT_To_R32_FLOAT: " << (end.QuadPart - start.QuadPart) * 1000 / freq.QuadPart << " ms\n";
	// D3DXFloat16To32Array x 20 - 359 ms
	// old x 20 - 949 ms
	// old SSE2 x 20 - 910 ms

	QueryPerformanceCounter(&start);
	for (int i = 0; i < iter; i++)
	{
		ConvertImageFrom_R32_FLOAT_To_A8R8G8B8_TYPELESS(floatData, byteData, width, height, floatPitch, bytePitch);
	}
	QueryPerformanceCounter(&end);
	std::cout << "Total time R32_FLOAT_To_A8R8G8B8_TYPELESS: " << (end.QuadPart - start.QuadPart) * 1000 / freq.QuadPart << " ms\n";
	// old x 20 - 4610 ms
	// old SSE2 x 20 - 1156 ms

	QueryPerformanceCounter(&start);
	for (int i = 0; i < iter; i++)
	{
		DeleteFile(L"test1.jpg");
		SaveImageToFile(L"test1.jpg", DXGI_FORMAT_R8G8B8A8_TYPELESS, width, height, (char*)byteData, bytePitch);
	}
	QueryPerformanceCounter(&end);
	std::cout << "Total time SaveImageToFile: " << (end.QuadPart - start.QuadPart) * 1000 / freq.QuadPart << " ms\n";
	delete [] pSysMem;
	return 0;
}

int TestR32(int iter)
{
	DXGI_FORMAT Format = DXGI_FORMAT_R32_FLOAT;
	DWORD width, height, len,SysMemPitch;
	FILE *filePtr;	
	if (_wfopen_s(&filePtr, L"r32f_data.bin", _T("rb")) != 0)
		if (_wfopen_s(&filePtr, L"..\\r32f_data.bin", _T("rb")) != 0)
			if (_wfopen_s(&filePtr, L"..\\..\\r32f_data.bin", _T("rb")) != 0)
				return 1;
	DWORD fourcc = MAKEFOURCC('i','Z','3','D');
	DWORD reserved = 0;
	fread(&reserved,		sizeof(reserved), 1, filePtr);
	_ASSERT(reserved == fourcc);
	fread(&len,				sizeof(len), 1, filePtr);
	fread(&Format,			sizeof(Format), 1, filePtr);
	fread(&SysMemPitch,		sizeof(SysMemPitch), 1, filePtr);
	fread(&width,			sizeof(width), 1, filePtr);
	fread(&height,			sizeof(height), 1, filePtr);
	fread(&reserved,		sizeof(reserved), 1, filePtr);
	fread(&reserved,		sizeof(reserved), 1, filePtr);
	char* pSysMem = new char[len];
	fread(pSysMem,			len, 1, filePtr);
	fclose(filePtr);

	// Initialize DevIL.
	ilInit();
	int bytePitch = width * sizeof(char) * 4;
	int byteDataSize = bytePitch * height;
	void* byteData = _aligned_malloc(byteDataSize, 16);
	if( byteData == NULL )
		return 2;
	LARGE_INTEGER freq, start, end;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&start);
	for (int i = 0; i < iter; i++)
	{
		ConvertImageFrom_R32_FLOAT_To_A8R8G8B8_TYPELESS(pSysMem, byteData, width, height, SysMemPitch, bytePitch);
	}
	QueryPerformanceCounter(&end);
	std::cout << "Total time R32_FLOAT_To_A8R8G8B8_TYPELESS: " << (end.QuadPart - start.QuadPart) * 1000 / freq.QuadPart << " ms\n";
	// old x 50 - 292 ms
	// old SSE2 x 50 - 320 ms

	QueryPerformanceCounter(&start);
	for (int i = 0; i < iter; i++)
	{
		DeleteFile(L"test2.jpg");
		SaveImageToFile(L"test2.jpg", DXGI_FORMAT_R8G8B8A8_TYPELESS, width, height, (char*)byteData, bytePitch);
	}
	QueryPerformanceCounter(&end);
	std::cout << "Total time SaveImageToFile: " << (end.QuadPart - start.QuadPart) * 1000 / freq.QuadPart << " ms\n";
	delete [] pSysMem;
	return 0;
}

int TestR11G11B10(int iter)
{
	DXGI_FORMAT Format = DXGI_FORMAT_R11G11B10_FLOAT;
	DWORD width, height, len,SysMemPitch,fourcc,reserved;
	FILE *filePtr;	
	if (_wfopen_s(&filePtr, L"r11g11b10_data.bin", _T("rb")) != 0)
		if (_wfopen_s(&filePtr, L"..\\r11g11b10_data.bin", _T("rb")) != 0)
			if (_wfopen_s(&filePtr, L"..\\..\\r11g11b10_data.bin", _T("rb")) != 0)
				return 1;
	fread(&fourcc,			sizeof(fourcc), 1, filePtr);
	_ASSERT(fourcc == MAKEFOURCC('i','Z','3','D'));
	fread(&len,				sizeof(len), 1, filePtr);
	fread(&Format,			sizeof(Format), 1, filePtr);
	fread(&SysMemPitch,		sizeof(SysMemPitch), 1, filePtr);
	fread(&width,			sizeof(width), 1, filePtr);
	fread(&height,			sizeof(height), 1, filePtr);
	fread(&reserved,		sizeof(reserved), 1, filePtr);
	fread(&reserved,		sizeof(reserved), 1, filePtr);
	char* pSysMem = new char[len];
	fread(pSysMem,			len, 1, filePtr);
	fclose(filePtr);

	// Initialize DevIL.
	ilInit();
	int floatPitch = width * sizeof(float) * 4;
	int floatDataSize = floatPitch * height;
	void* floatData = _aligned_malloc(floatDataSize, 16);
	if( floatData == NULL )
		return 2;
	int bytePitch = width * sizeof(char) * 4;
	int byteDataSize = bytePitch * height;
	void* byteData = _aligned_malloc(byteDataSize, 16);
	if( byteData == NULL )
	{
		_aligned_free(floatData);
		return 2;
	}
	LARGE_INTEGER freq, start, end;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&start);
	for (int i = 0; i < iter; i++)
	{
		ConvertImageFrom_R11G11B10_FLOAT_To_R32G32B32A32_FLOAT(pSysMem, floatData, width, height, SysMemPitch);
	}
	QueryPerformanceCounter(&end);
	std::cout << "Total time R11G11B10_FLOAT_To_R32G32B32A32_FLOAT: " << (end.QuadPart - start.QuadPart) * 1000 / freq.QuadPart << " ms\n";
	// old x 20 - 209 ms

	QueryPerformanceCounter(&start);
	for (int i = 0; i <iter; i++)
	{
		ConvertImageFrom_R32G32B32A32_FLOAT_To_A8R8G8B8_TYPELESS(floatData, byteData, width * height);
	}
	QueryPerformanceCounter(&end);
	std::cout << "Total time R32G32B32A32_FLOAT_To_A8R8G8B8_TYPELESS: " << (end.QuadPart - start.QuadPart) * 1000 / freq.QuadPart << " ms\n";
	// old x 20 - 615 ms
	// old SSE2 x 20 - 409 ms

	QueryPerformanceCounter(&start);
	for (int i = 0; i <iter; i++)
	{
		DeleteFile(L"test3.jpg");
		SaveImageToFile(L"test3.jpg", DXGI_FORMAT_R8G8B8A8_TYPELESS, width, height, (char*)byteData, bytePitch);
	}
	QueryPerformanceCounter(&end);
	std::cout << "Total time SaveImageToFile: " << (end.QuadPart - start.QuadPart) * 1000 / freq.QuadPart << " ms\n";
	delete [] pSysMem;
	return 0;
}


int TestR10G10B10A2(int iter)
{
	DXGI_FORMAT Format = DXGI_FORMAT_R10G10B10A2_TYPELESS;
	DWORD width, height, len,SysMemPitch,fourcc,reserved;
	FILE *filePtr;	
	if (_wfopen_s(&filePtr, L"r10g10b10a2_data.bin", _T("rb")) != 0)
		if (_wfopen_s(&filePtr, L"..\\r10g10b10a2_data.bin", _T("rb")) != 0)
			if (_wfopen_s(&filePtr, L"..\\..\\r10g10b10a2_data.bin", _T("rb")) != 0)
				return 1;
	fread(&fourcc,			sizeof(fourcc), 1, filePtr);
	_ASSERT(fourcc == MAKEFOURCC('i','Z','3','D'));
	fread(&len,				sizeof(len), 1, filePtr);
	fread(&Format,			sizeof(Format), 1, filePtr);
	fread(&SysMemPitch,		sizeof(SysMemPitch), 1, filePtr);
	fread(&width,			sizeof(width), 1, filePtr);
	fread(&height,			sizeof(height), 1, filePtr);
	fread(&reserved,		sizeof(reserved), 1, filePtr);
	fread(&reserved,		sizeof(reserved), 1, filePtr);
	char* pSysMem = new char[len];
	fread(pSysMem,			len, 1, filePtr);
	fclose(filePtr);

	// Initialize DevIL.
	ilInit();
	int bytePitch = width * sizeof(char) * 4;
	int byteDataSize = bytePitch * height;
	void* byteData = _aligned_malloc(byteDataSize, 16);
	if( byteData == NULL )
		return 2;
	LARGE_INTEGER freq, start, end;
	QueryPerformanceFrequency(&freq);

	QueryPerformanceCounter(&start);
	for (int i = 0; i < iter; i++)
	{
		ConvertImageFrom_R10G10B10A2_UNORM_To_A8R8G8B8_TYPELESS(pSysMem, byteData, width, height, SysMemPitch);
	}
	QueryPerformanceCounter(&end);
	std::cout << "Total time R10G10B10A2_UNORM_To_A8R8G8B8_TYPELESS: " << (end.QuadPart - start.QuadPart) * 1000 / freq.QuadPart << " ms\n";
	// old x 20 - 4610 ms
	// old SSE2 x 20 - 1156 ms

	QueryPerformanceCounter(&start);
	for (int i = 0; i < iter; i++)
	{
		DeleteFile(L"test4.jpg");
		SaveImageToFile(L"test4.jpg", DXGI_FORMAT_R8G8B8A8_TYPELESS, width, height, (char*)byteData, bytePitch);
	}
	QueryPerformanceCounter(&end);
	std::cout << "Total time SaveImageToFile: " << (end.QuadPart - start.QuadPart) * 1000 / freq.QuadPart << " ms\n";
	delete [] pSysMem;
	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	int iter = 15;
	TestR16(iter);
	TestR32(iter);
	TestR11G11B10(iter);
	TestR10G10B10A2(iter);
	return 0;
}