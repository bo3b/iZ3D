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
#include <emmintrin.h>
#include <intrin.h>
#include <Shlobj.h>
#include "AutoFocus.h"
#include "D3DDeviceWrapper.h"
#define  INTERLACE_STEP (4)

ShiftFinder10::ShiftFinder10()
{
	m_pD3DDeviceWrapper = NULL;
	m_hMemoryBuffer.pDrvPrivate = NULL;
}

void ShiftFinder10::Clear()
{
	ShiftFinder::Clear();

	//--- destroy texture ---
	if(m_pD3DDeviceWrapper)
	{
		D3D10DDI_HDEVICE hDevice = m_pD3DDeviceWrapper->GetHandle();
		D3D10DDI_DEVICEFUNCS& OriginalDevice10Funcs = m_pD3DDeviceWrapper->OriginalDeviceFuncs;
		OriginalDevice10Funcs.pfnDestroyResource(hDevice, m_hMemoryBuffer);
		DELETE_HANDLE(m_hMemoryBuffer);
		m_pD3DDeviceWrapper = 0;
	}
}

HRESULT ShiftFinder10::Initialize(D3DDeviceWrapper* pD3DDeviceWrapper, RECT* pLeftRect, RECT* pRightRect)
{
	if (m_bInitializationSuccessful)
		return S_OK;

	ShiftFinder::Initialize(pLeftRect, pRightRect);
	if(m_bInitializationSuccessful)
	{
		m_pD3DDeviceWrapper = pD3DDeviceWrapper;

		m_bDeviceIsMultithreaded = false;
		//--- in user mode all command must be called in one thread ---
		//UINT dwCreateDeviceFlags = pD3DDeviceWrapper->GetCreationFlags(); 
		//if(!(dwCreateDeviceFlags && D3D10DDI_CREATEDEVICE_FLAG_DISABLE_EXTRA_THREAD_CREATION))
		//	m_bDeviceIsMultithreaded = true;

		InitBoxFromRect(&m_srcBoxLeft,  &m_srcRectLeft);
		InitBoxFromRect(&m_srcBoxRight, &m_srcRectRight);

		_ASSERT(m_hMemoryBuffer.pDrvPrivate == NULL);
		HRESULT hResult = CreateMemoryBufferTexture(m_SearchAreaSizeX, 2 * m_FrameSizeY * INTERLACE_STEP);
		if(FAILED(hResult))
		{
			m_bInitializationSuccessful = false;
			return hResult;
		}
		return S_OK;
	}
	return E_FAIL;
}

HRESULT ShiftFinder10::CreateMemoryBufferTexture(int width, int height)
{
	HRESULT hResult = S_OK;
	D3D10DDI_HDEVICE hDevice = m_pD3DDeviceWrapper->GetHandle();
	D3D10DDI_DEVICEFUNCS& OriginalDevice10Funcs = m_pD3DDeviceWrapper->OriginalDeviceFuncs;
	D3D11DDI_DEVICEFUNCS& OriginalDevice11Funcs = m_pD3DDeviceWrapper->OriginalDeviceFuncs11;

	D3D10DDI_MIPINFO TexMipInfo;
	TexMipInfo.TexelWidth    = width;
	TexMipInfo.TexelHeight    = height;
	TexMipInfo.TexelDepth    = 1;
	TexMipInfo.PhysicalWidth   = width;
	TexMipInfo.PhysicalHeight   = height;
	TexMipInfo.PhysicalDepth   = 1;

	D3D11DDIARG_CREATERESOURCE CreateTexRes;
	CreateTexRes.pMipInfoList		= &TexMipInfo;
	CreateTexRes.pInitialDataUP		= 0;
	CreateTexRes.ResourceDimension	= D3D10DDIRESOURCE_TEXTURE2D;
	CreateTexRes.Usage				= D3D10_DDI_USAGE_STAGING;
	CreateTexRes.BindFlags			= 0;
	CreateTexRes.Format				= DXGI_FORMAT_R8G8B8A8_UNORM;
	CreateTexRes.SampleDesc.Count	= 1;
	CreateTexRes.SampleDesc.Quality	= 0;
	CreateTexRes.MapFlags			= D3D10_DDI_CPU_ACCESS_READ;
	CreateTexRes.MipLevels			= 1;  
	CreateTexRes.MiscFlags			= 0;
	CreateTexRes.ArraySize			= 1;
	CreateTexRes.ByteStride			= 0;
	CreateTexRes.pPrimaryDesc		= 0;
	static char RTData[4]			= { 0x0, 0x0, 0x0, 0x0 };
	D3D10DDI_HRTRESOURCE hRTTexture	= { RTData };
	m_pD3DDeviceWrapper->CreateResource(m_hMemoryBuffer, &CreateTexRes, hRTTexture);
	return S_OK;
}

void ShiftFinder10::FindShift(D3D10DDI_HRESOURCE left, D3D10DDI_HRESOURCE right, UINT currentFrame)
{
	if(m_bInitializationSuccessful)
	{
		if(m_bThreadReady)
		{
			D3D10DDI_HDEVICE hDevice = m_pD3DDeviceWrapper->GetHandle();
			D3D10DDI_DEVICEFUNCS& OriginalDevice10Funcs = m_pD3DDeviceWrapper->OriginalDeviceFuncs;
			OriginalDevice10Funcs.pfnFlush(hDevice);
			m_pD3DDeviceWrapper->m_nFlushCounter++;
			OriginalDevice10Funcs.pfnResourceCopyRegion(hDevice, m_hMemoryBuffer, 0, m_dstPointLeft.x, m_dstPointLeft.y, 0, left, 0, &m_srcBoxLeft);
			OriginalDevice10Funcs.pfnResourceCopyRegion(hDevice, m_hMemoryBuffer, 0, m_dstPointRight.x, m_dstPointRight.y, 0, right, 0, &m_srcBoxRight);

			//----------------------------- just for debugging ------------------------------------
			// HRESULT hResult = D3DX10SaveTextureToFile(m_pMemoryBuffer, D3DX10_IFF_DDS, TEXT("c:\\Work\\Utilities\\dx10_autofocus\\bin\\SelectedLines.dds"));
			//-------------------------------------------------------------------------------------

			ShiftFinder::FindShift(currentFrame);
		}
	}
}

bool ShiftFinder10::GetRTData()
{
	HRESULT hResult = S_OK;
	D3D10DDI_HDEVICE hDevice = m_pD3DDeviceWrapper->GetHandle();
	D3D10DDI_DEVICEFUNCS& OriginalDevice10Funcs = m_pD3DDeviceWrapper->OriginalDeviceFuncs;
	D3D10DDI_MAPPED_SUBRESOURCE pBits;

	OriginalDevice10Funcs.pfnStagingResourceMap(hDevice, m_hMemoryBuffer, 0, D3D10_DDI_MAP_READ, 0, &pBits);
	if(SUCCEEDED(hResult))
	{ 
		DWORD interlacedPitch = pBits.RowPitch * INTERLACE_STEP;
		BYTE* srcL = (BYTE*)pBits.pData;
		BYTE* srcR = (BYTE*)pBits.pData + m_FrameSizeY * interlacedPitch;
		convert_R8G8B8A8_ToMonochrome(srcL, interlacedPitch, m_ImageLeft,  m_FrameSizeX);
		convert_R8G8B8A8_ToMonochrome(srcR, interlacedPitch, m_ImageRight, m_SearchAreaSizeX);
		OriginalDevice10Funcs.pfnStagingResourceUnmap(hDevice, m_hMemoryBuffer, 0);
		return true;
	}
	return false;
}
