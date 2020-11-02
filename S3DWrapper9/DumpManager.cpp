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
#include "DumpManager.h"
#include <IL/il.h>
#include <IL/ilu.h>
#include "S3DAPI/ConvertImages.h"
#include "CommandDumper.h"

//#define SINGLE_THREADED
//#define NORMALIZE_FLOAT_TEXTURES
#define USE_GETRENDERTARGETDATA
#define MAX_QUEUE_SIZE 30

#ifndef FINAL_RELEASE

DumpManager::DumpManager()
: m_bMTDevice(false)
, m_hDumpThread(NULL)
{

}

DumpManager::~DumpManager()
{

}

bool DumpManager::AddSurface( LPTSTR pFileName, IDirect3DSurface9* pSrcSurface )
{
	HRESULT hResult = S_OK;
	_ASSERT(pSrcSurface);
	DumpData it;
	NSCALL(pSrcSurface->GetDesc(&it.desc));
	if (it.desc.Type != D3DRTYPE_SURFACE)
		return false; // Probably volume
	it.fileName = pFileName;
	return UseOldMethod(&it, pSrcSurface);
/*#ifndef SINGLE_THREADED
	if (m_Data.size() > MAX_QUEUE_SIZE)
		WaitWhenFinish();
#endif

	switch(it.desc.Format) 
	{
	case D3DFMT_R8G8B8:
	case D3DFMT_A8R8G8B8:
	case D3DFMT_X8R8G8B8:
	case D3DFMT_A8B8G8R8:
	case D3DFMT_X8B8G8R8:
		if (it.desc.MultiSampleType == D3DMULTISAMPLE_NONE)
		{
#ifndef USE_GETRENDERTARGETDATA
			if(!(it.desc.Usage & D3DUSAGE_RENDERTARGET))
#endif
			break;
		}
#ifndef NORMALIZE_FLOAT_TEXTURES
	//case D3DFMT_R16F:
	case D3DFMT_G16R16F:
	case D3DFMT_A16B16G16R16F:
	//case D3DFMT_R32F:
	case D3DFMT_G32R32F:
	case D3DFMT_A32B32G32R32F:
#endif
	case D3DFMT_R5G6B5:
	case D3DFMT_X1R5G5B5:
	case D3DFMT_A1R5G5B5:
	case D3DFMT_A4R4G4B4:
		if (ConvertDataByGPU(&it, pSrcSurface))
			pSrcSurface = it.pSurface;
		break;
	case D3DFMT_DXT1:
	case D3DFMT_DXT2:
	case D3DFMT_DXT3:
	case D3DFMT_DXT4:
	case D3DFMT_DXT5:
		if (DecompressData(&it, pSrcSurface))
			pSrcSurface = it.pSurface;
		break;
	}
#ifndef SINGLE_THREADED
	hResult = E_FAIL;
	if (!m_bMTDevice || !it.pSurface)
#endif
	hResult = pSrcSurface->LockRect(&it.lockedRect, NULL, D3DLOCK_NOSYSLOCK | D3DLOCK_READONLY);
	if (FAILED(hResult))
	{
		if(it.desc.Usage & D3DUSAGE_DEPTHSTENCIL)
			return false; // not supported
#ifdef USE_GETRENDERTARGETDATA
#ifndef SINGLE_THREADED
		if (!m_bMTDevice)
#endif
		{
			if(it.desc.Usage & D3DUSAGE_RENDERTARGET)
			{
				SAFE_FINAL_RELEASE(it.pSurface);
				if (!ResolveRT(&it, pSrcSurface)) 
					return false;
			}
		}
		if (!it.pSurface)
		{
			it.pSurface = pSrcSurface;
			it.pSurface->AddRef();
		}
#endif
	}
#ifdef SINGLE_THREADED
	ProcessDumpData(&it);
	if (SUCCEEDED(hResult))
		pSrcSurface->UnlockRect();
	SAFE_FINAL_RELEASE(it.pSurface);
#else
	if (!m_bMTDevice && it.pSurface && FAILED(hResult))
	{
		hResult = it.pSurface->LockRect(&it.lockedRect, NULL, D3DLOCK_NOSYSLOCK | D3DLOCK_READONLY);
		if (FAILED(hResult))
			return false;
	}

	if (it.lockedRect.pBits)
	{
		INT WidthSize = it.desc.Width * BitsPerPixel( it.desc.Format ) / 8;
		it.pData = (char*)_aligned_malloc(WidthSize * it.desc.Height, 16);
		char* pSrc = (char*)it.lockedRect.pBits;
		char* pDst = (char*)it.pData;
		if (it.lockedRect.Pitch > WidthSize)
		{
			for (DWORD i = 0; i < it.desc.Height; i++)
			{
				memcpy(pDst, pSrc, WidthSize);
				pSrc += it.lockedRect.Pitch;
				pDst += WidthSize;
			}
			it.lockedRect.Pitch = WidthSize;
		}
		else
			memcpy(pDst, pSrc, WidthSize * it.desc.Height);
		it.lockedRect.pBits = it.pData;
	}

	if (SUCCEEDED(hResult))
	{
		pSrcSurface->UnlockRect();
		SAFE_FINAL_RELEASE(it.pSurface);
	}

	m_Access.Enter();
	m_Data.push(it);
	m_Access.Leave();
	StartThread();
	//WaitWhenFinish();
	//DEBUG_MESSAGE(_T("Queue length: %d\n"), m_Data.size());
#endif
	return true;*/
}

bool DumpManager::ConvertDataByGPU( DumpData* pData, IDirect3DSurface9* pSurf )
{
	INSIDE;
	if (pData->desc.Pool != D3DPOOL_DEFAULT)
		return false;
	HRESULT hResult = S_OK;
	CComPtr<IDirect3DDevice9>  pDevice;
	NSCALL(pSurf->GetDevice(&pDevice));
	NSCALL(pDevice->CreateRenderTarget(pData->desc.Width, pData->desc.Height, D3DFMT_A8R8G8B8, 
		D3DMULTISAMPLE_NONE, 0, TRUE, &pData->pSurface, 0));
	if (SUCCEEDED(hResult)) {
		NSCALL(pDevice->StretchRect(pSurf, NULL, pData->pSurface, NULL, D3DTEXF_POINT));
		if (SUCCEEDED(hResult)) {
			pData->desc.Pool = D3DPOOL_DEFAULT;
			pData->desc.Format = D3DFMT_A8R8G8B8;
			pData->desc.Usage = D3DUSAGE_RENDERTARGET;
			pData->desc.MultiSampleType = D3DMULTISAMPLE_NONE;
			pData->desc.MultiSampleQuality = 0;
		} else
			SAFE_FINAL_RELEASE(pData->pSurface);
	}
	return SUCCEEDED(hResult);
}

bool DumpManager::DecompressData( DumpData* pData, IDirect3DSurface9* pSurf )
{
	HRESULT hResult = S_OK;
	CComPtr<IDirect3DDevice9>  pDevice;
	NSCALL(pSurf->GetDevice(&pDevice));
	NSCALL(pDevice->CreateOffscreenPlainSurface(pData->desc.Width, pData->desc.Height, D3DFMT_X8R8G8B8, 
		D3DPOOL_SYSTEMMEM, &pData->pSurface, 0));
	if (SUCCEEDED(hResult)) {
		NSCALL(D3DXLoadSurfaceFromSurface(pData->pSurface, NULL, NULL, pSurf, NULL, NULL, D3DX_FILTER_NONE, 0));
		if (SUCCEEDED(hResult)) {
			pData->desc.Pool = D3DPOOL_SYSTEMMEM;
			pData->desc.Format = D3DFMT_X8R8G8B8;
		} else
			SAFE_FINAL_RELEASE(pData->pSurface);
	}
	return SUCCEEDED(hResult);
}

bool DumpManager::ResolveRT( DumpData* pData, IDirect3DSurface9* pSurf )
{
	INSIDE;
	HRESULT hResult = S_OK;
	CComPtr<IDirect3DDevice9>  pDevice;
	NSCALL(pSurf->GetDevice(&pDevice));
	if (pData->desc.MultiSampleType == D3DMULTISAMPLE_NONE)
	{
		NSCALL(pDevice->CreateOffscreenPlainSurface(pData->desc.Width, pData->desc.Height, pData->desc.Format, 
			D3DPOOL_SYSTEMMEM, &pData->pSurface, 0));
		if (SUCCEEDED(hResult)) {
			NSCALL(pDevice->GetRenderTargetData(pSurf, pData->pSurface));
		}
	}
	else
	{
#if 1
		NSCALL(pDevice->CreateRenderTarget(pData->desc.Width, pData->desc.Height, pData->desc.Format, 
			D3DMULTISAMPLE_NONE, 0, TRUE, &pData->pSurface, 0));
		if (SUCCEEDED(hResult)) {
			NSCALL(pDevice->StretchRect(pSurf, 0, pData->pSurface, 0, D3DTEXF_NONE));
		}
#else
		CComPtr<IDirect3DSurface9> rtSurface;
		NSCALL(pDevice->CreateOffscreenPlainSurface(pData->desc.Width, pData->desc.Height, pData->desc.Format, 
			D3DPOOL_SYSTEMMEM, &pData->pSurface, 0));
		if (SUCCEEDED(hResult)) 
		{
			NSCALL(pDevice->CreateRenderTarget(pData->desc.Width, pData->desc.Height, pData->desc.Format, 
				D3DMULTISAMPLE_NONE, 0, FALSE, &rtSurface, 0));
			NSCALL(pDevice->StretchRect(pSurf, 0, rtSurface, 0, D3DTEXF_NONE));
			if (SUCCEEDED(hResult)) {
				NSCALL(pDevice->GetRenderTargetData(rtSurface, pData->pSurface));
			}
		}
#endif
	}
	if (FAILED(hResult))
		SAFE_FINAL_RELEASE(pData->pSurface);
	return SUCCEEDED(hResult);
}

void DumpManager::ProcessDumpData( DumpData* pData )
{
	HRESULT hResult = E_FAIL;
#ifndef SINGLE_THREADED
#ifdef USE_GETRENDERTARGETDATA
	if (m_bMTDevice)
	{
		if(pData->pSurface && pData->desc.Usage & D3DUSAGE_RENDERTARGET)
		{
			CComPtr<IDirect3DSurface9> surf = pData->pSurface;
			SAFE_RELEASE(pData->pSurface);
			ResolveRT(pData, surf);
		}
	}
#endif
#endif
	if (!pData->lockedRect.pBits) {
		NSCALL(pData->pSurface->LockRect(&pData->lockedRect, NULL, D3DLOCK_NOSYSLOCK | D3DLOCK_READONLY));
		if (FAILED(hResult))
		{
			DEBUG_TRACE3(_T("\tFailed dump surface: Type = %s, Format = %s, MultiSample = %s,")
				_T("MultisampleQuality = %08X, Pool = %s\n"), 
				GetResourceTypeString(pData->desc.Type), GetFormatString(pData->desc.Format), 
				GetMultiSampleString(pData->desc.MultiSampleType), pData->desc.MultiSampleQuality, 
				GetPoolString(pData->desc.Pool));
			return;
		}
	}
	SaveImageToFile(pData);
	if (pData->pData)
		_aligned_free(pData->pData);
	if (SUCCEEDED(hResult))
		pData->pSurface->UnlockRect();
}


bool DumpManager::SaveImageToFile( DumpData* pData )
{
	D3DFORMAT format = pData->desc.Format;
	UINT width = pData->desc.Width;
	UINT height = pData->desc.Height;
	char* buffer = (char*)pData->lockedRect.pBits;
	size_t rowPitch = (INT)pData->lockedRect.Pitch;

	bool isFormatOkay = false;
	ILubyte bytesPerPixel = 4;
	ILenum ILFormat;
	ILenum dataType = IL_UNSIGNED_BYTE;
	void* floatData = NULL;
	void* byteData = NULL;
	switch( format )
	{
	case D3DFMT_L8:
		break;
	case D3DFMT_X8R8G8B8:
	case D3DFMT_A8R8G8B8:
		isFormatOkay = true;
		ILFormat = IL_BGRA;
		break;
	case D3DFMT_X8B8G8R8:
	case D3DFMT_A8B8G8R8:
		isFormatOkay = true;
		ILFormat = IL_RGBA;
		break;
		//--- try to convert in R8G8B8A8 format ---
	case D3DFMT_G16R16F:
	case D3DFMT_G32R32F:
	case D3DFMT_A2R10G10B10:
	case D3DFMT_A2B10G10R10:
		{
			DEBUG_MESSAGE(_T("SaveImageToFile format: %s\n"), GetFormatString(format));
			isFormatOkay = true;
			ILFormat = IL_RGBA;
			size_t floatPitch = width * sizeof(float) * 4;
			size_t floatDataSize = floatPitch * height;
			floatData = _aligned_malloc(floatDataSize, 16);

			switch(format)
			{
			case D3DFMT_G16R16F:
				ConvertImageFrom_R16G16_FLOAT_To_R32G32B32A32_FLOAT(buffer, floatData, width, height, (UINT)rowPitch);
				break;
			case D3DFMT_G32R32F:
				ConvertImageFrom_R32G32_FLOAT_To_R32G32B32A32_FLOAT(buffer, floatData, width, height, (UINT)rowPitch);
				break;
			case D3DFMT_A16B16G16R16F:
				ConvertImageFrom_R16G16B16A16_FLOAT_To_R32G32B32A32_FLOAT(buffer, floatData, width, height, (UINT)rowPitch);
				break;
			case D3DFMT_A2R10G10B10:
			case D3DFMT_A2B10G10R10:
				ConvertImageFrom_R10G10B10A2_UNORM_To_R32G32B32A32_FLOAT(buffer, floatData, width, height, (UINT)rowPitch);
				break;
			}
			size_t bytePitch = width * sizeof(byte) * 4;
			size_t byteDataSize = bytePitch * height;
			byteData = _aligned_malloc(byteDataSize, 4);
			ConvertImageFrom_R32G32B32A32_FLOAT_To_A8R8G8B8_TYPELESS(floatData, byteData, width * height);
			buffer = (char*)byteData;
			rowPitch = bytePitch;
			format = D3DFMT_A8R8G8B8;
			break;
		}
	case D3DFMT_R16F:
		{
			DEBUG_MESSAGE(_T("SaveImageToFile format: %s\n"), GetFormatString(format));
			isFormatOkay = true;
			size_t floatPitch = width * sizeof(float);
			size_t floatDataSize = floatPitch * height;
			floatData = _aligned_malloc(floatDataSize, 16);
			if( floatData == NULL )
			{
				DEBUG_MESSAGE(_T("SaveImageToFile failed, cannot allocate memory\n"));
				return false;
			}
			ConvertImageFrom_R16_FLOAT_To_R32_FLOAT(buffer, floatData, width, height, (UINT)rowPitch);
			size_t bytePitch = width * sizeof(byte) * 4;
			size_t byteDataSize = bytePitch * height;
			byteData = _aligned_malloc(byteDataSize, 16);
			if( byteData == NULL )
			{
				_aligned_free(floatData);
				DEBUG_MESSAGE(_T("SaveImageToFile failed, cannot allocate memory\n"));
				return false;
			}
			ConvertImageFrom_R32_FLOAT_To_A8R8G8B8_TYPELESS(floatData, byteData, width, height, floatPitch, bytePitch);
			buffer = (char*)byteData;
			rowPitch = bytePitch;
			format = D3DFMT_A8R8G8B8;
			break;
		}
	case D3DFMT_D32F_LOCKABLE:
	case D3DFMT_R32F:
		{
			DEBUG_MESSAGE(_T("SaveImageToFile format: %s\n"), GetFormatString(format));
			isFormatOkay = true;
			size_t bytePitch = width * sizeof(byte) * 4;
			size_t byteDataSize = bytePitch * height;
			byteData = _aligned_malloc(byteDataSize, 16);
			if( byteData == NULL )
			{
				_aligned_free(floatData);
				DEBUG_MESSAGE(_T("SaveImageToFile failed, cannot allocate memory\n"));
				return false;
			}
			ConvertImageFrom_R32_FLOAT_To_A8R8G8B8_TYPELESS(buffer, byteData, width, height, (UINT)rowPitch, bytePitch);
			buffer = (char*)byteData;
			rowPitch = bytePitch;
			format = D3DFMT_A8R8G8B8;
			break;
		}
	case D3DFMT_A32B32G32R32F:
		{
			DEBUG_MESSAGE(_T("SaveImageToFile format: %s\n"), GetFormatString(format));
			isFormatOkay = true;
			ILFormat = IL_RGBA;
			size_t bytePitch = width * sizeof(byte) * 4;
			size_t byteDataSize = bytePitch * height;
			byteData = _aligned_malloc(byteDataSize, 4);
			ConvertImageFrom_R32G32B32A32_FLOAT_To_A8R8G8B8_TYPELESS(buffer, byteData, width * height);
			buffer = (char*)byteData;
			rowPitch = bytePitch;
			format = D3DFMT_A8R8G8B8;
			break;
		}
	}

	if( !isFormatOkay )
	{
		DEBUG_MESSAGE(_T("SaveImageToFile failed, not supported format: %s\n"), GetFormatString(format));
		return false;
	}

	size_t WidthSize = width * BitsPerPixel( format ) / 8;
	_ASSERT(rowPitch >= WidthSize);
	char* newData = NULL;
	if (rowPitch > WidthSize)
	{
		newData = (char*)_aligned_malloc(WidthSize * height, 16);
		char* pSrc = buffer;
		char* pDst = newData;
		for (DWORD i = 0; i < height; i++)
		{
			memcpy(pDst, pSrc, WidthSize);
			pSrc += pData->lockedRect.Pitch;
			pDst += WidthSize;
		}
		buffer = newData;
	}

	ILuint	ImgId;  
	ilGenImages(1, &ImgId);
	ilBindImage(ImgId);	
	ilTexImage(width, height, 0, bytesPerPixel, ILFormat, dataType, buffer);
	iluFlipImage();
	ILenum fileFormat = GetILImageFileFormat();
	CONST TCHAR* pFilename = pData->fileName.c_str();
	ilSave(fileFormat, pFilename);
	ilDeleteImages(1, &ImgId);  
	if (byteData)
		_aligned_free(byteData);
	if (floatData)
		_aligned_free(floatData);
	if (newData)
		_aligned_free(newData);
	return true;
}

void DumpManager::WaitWhenFinish()
{
#ifndef SINGLE_THREADED
	if (m_hDumpThread)
		WaitForSingleObject(m_hDumpThread, 10000);
#endif
}

bool DumpManager::UseOldMethod( DumpData* pData, IDirect3DSurface9* pSrcSurface )
{
	INSIDE;
	HRESULT hResult = S_OK;
	CComPtr<IDirect3DSurface9> offScreenSurface;
	CComPtr<IDirect3DSurface9> offScreenSurface2;
	if(pData->desc.Usage & D3DUSAGE_RENDERTARGET || pData->desc.Usage & D3DUSAGE_DEPTHSTENCIL)
	{
		CComPtr<IDirect3DDevice9>  pDevice;
		NSCALL(pSrcSurface->GetDevice(&pDevice));
		switch(pData->desc.Format)
		{
		case D3DFMT_D16:
		case D3DFMT_D32:
		case D3DFMT_D24S8:
		case D3DFMT_D24X8:
			return false; // not supported
		case D3DFMT_D16_LOCKABLE:
		case D3DFMT_D32F_LOCKABLE:
			// D3D9Ex
			//CComPtr<IDirect3DSurface9> pOffDepth;
			//NSCALL(pDevice->CreateOffscreenPlainSurface(Desc.Width, Desc.Height, D3DFMT_D32F_LOCKABLE, D3DPOOL_SYSTEMMEM, &pOffDepth, 0));
			//NSCALL(pDevice->UpdateSurface(pSrcSurface, NULL, pOffDepth, NULL));
			//pSrcSurface = pOffDepth;

			D3DLOCKED_RECT SourceRect;
			NSCALL(pSrcSurface->LockRect(&SourceRect, 0, D3DLOCK_NOSYSLOCK | D3DLOCK_READONLY));
			if (SUCCEEDED(hResult))
			{
				NSCALL(pDevice->CreateOffscreenPlainSurface(pData->desc.Width, pData->desc.Height, D3DFMT_X8R8G8B8, D3DPOOL_SYSTEMMEM, &offScreenSurface, 0));
				D3DLOCKED_RECT DestRect;
				NSCALL(offScreenSurface->LockRect(&DestRect, 0, 0));
				if (pData->desc.Format != D3DFMT_A8 && pData->desc.Format != D3DFMT_L8)
				{
					for (ULONG y = 0; y < pData->desc.Height; y++)
					{
						WORD *pSource = (WORD *)((BYTE *)SourceRect.pBits + y * SourceRect.Pitch);
						BYTE *pDest = (BYTE *)DestRect.pBits + y * DestRect.Pitch;
						for (ULONG x = 0; x < pData->desc.Width; x++)
						{
							*pDest++ = (*pSource) >> 8;
							*pDest++ = (*pSource) >> 8;
							*pDest++ = (*pSource) >> 8;
							pDest++; //alpha
							pSource++;
						}
					}
				}
				NSCALL(pSrcSurface->UnlockRect());
				NSCALL(offScreenSurface->UnlockRect());
			}
			break;
		//case D3DFMT_G16R16F:
		//case D3DFMT_A16B16G16R16F:
		//case D3DFMT_R16F:
		//case D3DFMT_G32R32F:
		//case D3DFMT_A32B32G32R32F:
		case D3DFMT_R32F:
			NSCALL(pDevice->CreateOffscreenPlainSurface(pData->desc.Width, pData->desc.Height, pData->desc.Format, D3DPOOL_SYSTEMMEM, &offScreenSurface2, 0));
			if (SUCCEEDED(hResult))
			{
				if (pData->desc.MultiSampleType == D3DMULTISAMPLE_NONE)
				{
					NSCALL(pDevice->GetRenderTargetData(pSrcSurface, offScreenSurface2));
				}
				else
				{
					CComPtr<IDirect3DSurface9> rtSurface;
					NSCALL(pDevice->CreateRenderTarget(pData->desc.Width, pData->desc.Height, pData->desc.Format, D3DMULTISAMPLE_NONE, 0, FALSE, &rtSurface, 0));
					NSCALL(pDevice->StretchRect(pSrcSurface, 0, rtSurface, 0, D3DTEXF_NONE));
					NSCALL(pDevice->GetRenderTargetData(rtSurface, offScreenSurface2));
				}
				D3DLOCKED_RECT SourceRect;
				NSCALL(offScreenSurface2->LockRect(&SourceRect, 0, D3DLOCK_NOSYSLOCK | D3DLOCK_READONLY));
				if (SUCCEEDED(hResult))
				{
#if 0
					float dmin = +FLT_MAX;
					float dmax = -FLT_MAX;
					float sum = 0;
					for (ULONG y = 0; y < pData->desc.Height; y++)
					{
						float *pSource = (float *)((BYTE *)SourceRect.pBits + y * SourceRect.Pitch);
						for (ULONG x = 0; x < pData->desc.Width; x++)
						{
							float d = (*pSource);
							sum += d;
							if (d < dmin)
								dmin = d;
							else if (d > dmax/* && d != 2.0f*/)
								dmax = d;
							pSource++;
						}
					}
					if (dmax == -FLT_MAX)
						dmax = dmin;
					float davg = sum / (pData->desc.Width * pData->desc.Height);
					DEBUG_TRACE3(_T("Min: %f, Max: %f, Avg: %f\n"), dmin, dmax, davg);
					dmax -= dmin;
					if (dmax == 0.0f)
						dmax = 0.1f;
					dmax = 1.0f / dmax;
#endif

					NSCALL(pDevice->CreateOffscreenPlainSurface(pData->desc.Width, pData->desc.Height, D3DFMT_X8R8G8B8, D3DPOOL_SYSTEMMEM, &offScreenSurface, 0));
					D3DLOCKED_RECT DestRect;
					NSCALL(offScreenSurface->LockRect(&DestRect, 0, 0));

#if 1
					ConvertImageFrom_R32_FLOAT_To_A8R8G8B8_TYPELESS(SourceRect.pBits, DestRect.pBits, 
						pData->desc.Width, pData->desc.Height, SourceRect.Pitch, DestRect.Pitch);
#else					
					for (ULONG y = 0; y < pData->desc.Height; y++)
					{
						float *pSource = (float *)((BYTE *)SourceRect.pBits + y * SourceRect.Pitch);
						BYTE *pDest = (BYTE *)DestRect.pBits + y * DestRect.Pitch;
						for (ULONG x = 0; x < pData->desc.Width; x++)
						{
							float d = (*pSource);
							BYTE c;
							if (d != 0.0f)
								c = (BYTE)(pow((d - dmin) * dmax, 1.0f / 2.2f) * 255);
							else
								c = 0;
							*pDest++ = c;
							*pDest++ = c;
							*pDest++ = c;
							*pDest++ = 255; //alpha
							pSource++;
						}
					}
#endif

					NSCALL(offScreenSurface2->UnlockRect());
					NSCALL(offScreenSurface->UnlockRect());
				}
			}
			break;
		default:
			NSCALL(pDevice->CreateOffscreenPlainSurface(pData->desc.Width, pData->desc.Height, pData->desc.Format, D3DPOOL_SYSTEMMEM, &offScreenSurface, 0));
			if (SUCCEEDED(hResult))
			{
				if (pData->desc.MultiSampleType == D3DMULTISAMPLE_NONE)
				{
					NSCALL(pDevice->GetRenderTargetData(pSrcSurface, offScreenSurface));
				}
				else
				{
					CComPtr<IDirect3DSurface9> rtSurface;
					NSCALL(pDevice->CreateRenderTarget(pData->desc.Width, pData->desc.Height, pData->desc.Format, D3DMULTISAMPLE_NONE, 0, FALSE, &rtSurface, 0));
					NSCALL(pDevice->StretchRect(pSrcSurface, 0, rtSurface, 0, D3DTEXF_NONE));
					NSCALL(pDevice->GetRenderTargetData(rtSurface, offScreenSurface));
				}
			}
			break;
		}
	}
	else
	{
		offScreenSurface = pSrcSurface;
	}
	if (SUCCEEDED(hResult)) {
		hResult = D3DXSaveSurfaceToFile(pData->fileName.c_str(), GetImageFileFormat(), offScreenSurface, NULL, NULL);
	}
	if (FAILED(hResult))
	{
		DEBUG_TRACE3(_T("\tFailed dump surface: Type = %s, Format = %s, MultiSample = %s,")
			_T("MultisampleQuality = %08X, Pool = %s\n"), 
			GetResourceTypeString(pData->desc.Type), GetFormatString(pData->desc.Format), 
			GetMultiSampleString(pData->desc.MultiSampleType), pData->desc.MultiSampleQuality, 
			GetPoolString(pData->desc.Pool));
	}
	return SUCCEEDED(hResult);
}

void DumpManager::StartThread()
{
	if (!m_hDumpThread)
		m_hDumpThread = (HANDLE)_beginthreadex( NULL, 0, (unsigned(__stdcall*)(void *))DumpThreadProc, this, 0, NULL);
}

DWORD WINAPI DumpManager::DumpThreadProc( DumpManager* manager )
{
	SetThreadName(-1, "Dump Manager");
	manager->DumpThread();
	manager->m_hDumpThread = NULL;
	return 0;
}

void DumpManager::DumpThread()
{
	do 
	{
		DumpData* pData = &m_Data.front();
		ProcessDumpData(pData);
		SAFE_FINAL_RELEASE(pData->pSurface);
		m_Access.Enter();
		m_Data.pop();
		m_Access.Leave();
	} while (!m_Data.empty());
}
#endif