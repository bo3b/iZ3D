#pragma once

#include "S3DAPI.h"
#include <d3d9.h>
#include <DXGIFormat.h>
#include <squish.h>

bool S3DAPI_API IsCompressedFormat( DXGI_FORMAT format );
size_t S3DAPI_API GetBitWidthOfDXGIFormat( DXGI_FORMAT format );
DXGI_FORMAT S3DAPI_API GetTypedDXGIFormat( DXGI_FORMAT format );
DXGI_FORMAT S3DAPI_API ConvertFormatD3D9ToDXGI( D3DFORMAT fmt );
D3DFORMAT S3DAPI_API ConvertFormatDXGIToD3D9( DXGI_FORMAT fmt );

void S3DAPI_API ConvertImageFrom_R16_FLOAT_To_R32_FLOAT(void* srcBuffer, void* dstBuffer, DWORD width, DWORD height, UINT srcPitch);
void S3DAPI_API ConvertImageFrom_R16_FLOAT_To_R32G32B32A32_FLOAT(void* srcBuffer, void* dstBuffer, DWORD width, DWORD height, UINT srcPitch);
void S3DAPI_API ConvertImageFrom_R16G16_FLOAT_To_R32G32B32A32_FLOAT(void* srcBuffer, void* dstBuffer, DWORD width, DWORD height, UINT srcPitch);
void S3DAPI_API ConvertImageFrom_R16G16B16A16_FLOAT_To_R32G32B32A32_FLOAT(void* srcBuffer, void* dstBuffer, DWORD width, DWORD height, UINT srcPitch);
void S3DAPI_API ConvertImageFrom_R11G11B10_FLOAT_To_R32G32B32A32_FLOAT(void* srcBuffer, void* dstBuffer, DWORD width, DWORD height, UINT srcPitch);
void S3DAPI_API ConvertImageFrom_R10G10B10A2_UNORM_To_R32G32B32A32_FLOAT(void* srcBuffer, void* dstBuffer, DWORD width, DWORD height, UINT srcPitch);
void S3DAPI_API ConvertImageFrom_R32_FLOAT_To_R32G32B32A32_FLOAT(void* srcBuffer, void* dstBuffer, DWORD width, DWORD height, UINT srcPitch);
void S3DAPI_API ConvertImageFrom_R32G32_FLOAT_To_R32G32B32A32_FLOAT(void* srcBuffer, void* convertedData, DWORD width, DWORD height, UINT rowPitch);
void S3DAPI_API ConvertImageFrom_R24G8_UNORM_To_R32G32B32A32_FLOAT(void* srcBuffer, void* dstBuffer, DWORD width, DWORD height, UINT srcPitch);
void S3DAPI_API ConvertImageFrom_BCx_To_A8R8G8B8_TYPELESS(void* srcBuffer, void* dstBuffer, DWORD width, DWORD height, UINT srcPitch, int flag);
void S3DAPI_API ConvertImageFrom_8_BYTE_To_A8R8G8B8_TYPELESS(void* srcBuffer, void* dstBuffer, DWORD width, DWORD height, UINT srcPitch);
void S3DAPI_API ConvertImageFrom_R8G8_UNORM_To_A8R8G8B8_TYPELESS(void* __restrict srcBuffer, void* __restrict dstBuffer, DWORD width, DWORD height, UINT rowPitch);
void S3DAPI_API ConvertImageFrom_R32_FLOAT_To_A8R8G8B8_TYPELESS(void* srcBuffer, void* dstBuffer, DWORD width, DWORD height, UINT srcPitch, UINT dstPitch);
void S3DAPI_API ConvertImageFrom_R32G32B32A32_FLOAT_To_A8R8G8B8_TYPELESS(void* srcBuffer, void* dstBuffer, DWORD dataSize);
void S3DAPI_API ConvertImageFrom_B8G8R8A8_UNORM_To_A8R8G8B8_TYPELESS(void* srcBuffer, void* dstBuffer, DWORD width, DWORD height, UINT rowPitch);
void S3DAPI_API ConvertImageFrom_R10G10B10A2_UNORM_To_A8R8G8B8_TYPELESS(void* srcBuffer, void* dstBuffer, DWORD width, DWORD height, UINT rowPitch);

void S3DAPI_API SaveImageToBMP( const TCHAR * filename, DWORD width, DWORD height, char * bmpBuffer );
