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
#include "WideStereoRenderer.h"
#include "WideStereoRenderer_Hook.h"
#include "CommandDumper.h"

namespace Wide
{
	HRESULT (__stdcall IDirect3DTexture9::*Original_Texture_GetLevelDesc)(UINT Level, D3DSURFACE_DESC *pDesc);
	HRESULT (__stdcall IDirect3DSurface9::*Original_Surface_GetDesc)(D3DSURFACE_DESC *pDesc);
	HRESULT (__stdcall IDirect3DSurface9::*Original_TexSurface_GetDesc)(D3DSURFACE_DESC *pDesc);
	HRESULT (__stdcall IDirect3DSurface9::*Original_DepthSurface_GetDesc)(D3DSURFACE_DESC *pDesc);
	ULONG (__stdcall IDirect3DSurface9::*Original_Surface_Release)();
	ULONG (__stdcall IDirect3DTexture9::*Original_Texture_Release)();

	Surface_LockRect_t Original_Surface_LockRect;
	Surface_LockRect_t Original_TexSurface_PDef_LockRect;
	Surface_LockRect_t Original_DepthSurface_LockRect;
	Texture_LockRect_t Original_Texture_LockRect;
	Surface_UnlockRect_t Original_Surface_UnlockRect;
	Surface_UnlockRect_t Original_TexSurface_PDef_UnlockRect;
	Surface_UnlockRect_t Original_DepthSurface_UnlockRect;
	Texture_UnlockRect_t Original_Texture_UnlockRect;

	void	 Proxy_GetDesc(IDirect3DSurface9* This, D3DSURFACE_DESC *pDesc)
	{
		DWORD size = sizeof(IUnknown *);
		CComPtr<IWideSurfaceData> pWideSurfaceData;
		if (SUCCEEDED(This->GetPrivateData(__uuidof(IWideSurfaceData), (void *)&pWideSurfaceData, &size)))
		{
			CONST SIZE* NewSize = pWideSurfaceData->GetSize();
			pDesc->Width = NewSize->cx;
			pDesc->Height = NewSize->cy;
			pDesc->Format = pWideSurfaceData->GetFormat();
			pDesc->Usage = pWideSurfaceData->GetUsage();
			DEBUG_TRACE2(_T("Surface::GetDesc(This = %p [%s], Desc = (%d, %d), Format = %s)\n"),
				This, GetObjectName(This), pDesc->Width, pDesc->Height, GetFormatString(pDesc->Format));
		}																	
	}

	PROXYSTDMETHOD(Texture_GetLevelDesc)(IDirect3DTexture9* This, UINT Level, D3DSURFACE_DESC *pDesc)
	{
		HRESULT hr = (This->*Original_Texture_GetLevelDesc)(Level, pDesc);
		if (CalledFromApp() && SUCCEEDED(hr))
		{
			CComPtr<IDirect3DSurface9> pSurface;
			This->GetSurfaceLevel(Level, &pSurface);
			Proxy_GetDesc(pSurface, pDesc);
		}
		return hr;
	}

	PROXYSTDMETHOD(Surface_GetDesc)(IDirect3DSurface9* This, D3DSURFACE_DESC *pDesc)
	{
		HRESULT hr = (This->*Original_Surface_GetDesc)(pDesc);
		if (CalledFromApp() && SUCCEEDED(hr))
			Proxy_GetDesc(This, pDesc);
		return hr;
	}

	PROXYSTDMETHOD(DepthSurface_GetDesc)( IDirect3DSurface9* This, D3DSURFACE_DESC *pDesc )
	{
		HRESULT hr = (This->*Original_DepthSurface_GetDesc)(pDesc);
		if (CalledFromApp() && SUCCEEDED(hr))
			Proxy_GetDesc(This, pDesc);
		return hr;
	}

	PROXYSTDMETHOD(TexSurface_GetDesc)(IDirect3DSurface9* This, D3DSURFACE_DESC *pDesc)
	{
		HRESULT hr = (This->*Original_TexSurface_GetDesc)(pDesc);
		if (CalledFromApp() && SUCCEEDED(hr))
			Proxy_GetDesc(This, pDesc);
		return hr;
	}

	template<typename F>
	HRESULT	 Proxy_SurfLockRect(F origFunc, IDirect3DSurface9* This, D3DLOCKED_RECT * pLockedRect, CONST RECT * pRect, DWORD Flags)
	{
		BOOL bLockForWrite = FALSE;
		RECT lockedRect;
		D3DSURFACE_DESC desc;
		DWORD size = sizeof(IUnknown *);
		CComPtr<IWideSurfaceData> pWideSurfaceData;
		CBaseStereoRenderer* pCurrentStereoRenderer;
		GetCurrentStereoRenderer(This, &pCurrentStereoRenderer);
		if (SUCCEEDED(This->GetPrivateData(__uuidof(IWideSurfaceData), (void *)&pWideSurfaceData, &size)))
		{
			DEBUG_TRACE2(_T("Surface::LockRect(This = %p [%s], pLockedRect = %p, pRect = %s, Flags = %s)\n"),
				This, GetObjectName(This), 
				pLockedRect, GetRectString(pRect), GetLockFlagString(Flags));

			This->GetDesc(&desc);
			desc.Height /= HEIGHTMUL;
			if (pRect == NULL)
			{
				SetRect(&lockedRect, 0, 0, desc.Width, desc.Height);
				pRect = &lockedRect;
			}
			else
				lockedRect = *pRect;
			bLockForWrite = !(Flags & D3DLOCK_READONLY);
			if (bLockForWrite)
			{
				if (pCurrentStereoRenderer && pCurrentStereoRenderer->m_Input.StereoActive)
					bLockForWrite = FALSE;
			}
		}
		HRESULT hr = (This->*origFunc)(pLockedRect, pRect, Flags);
		if (pWideSurfaceData)
		{
			if (SUCCEEDED(hr) && bLockForWrite)
			{
				_ASSERT(pCurrentStereoRenderer);
				pWideSurfaceData->SetLockForWrite(TRUE);
				pWideSurfaceData->SetLockedRect(&lockedRect);
				CComPtr<IDirect3DSurface9> pSurf;
				pCurrentStereoRenderer->m_Direct3DDevice.CreateRenderTarget(desc.Width, desc.Height, desc.Format, desc.MultiSampleType, desc.MultiSampleQuality,
					FALSE, &pSurf, NULL);
				pWideSurfaceData->SetAdditionalSurface(pSurf);
			}
			else
				pWideSurfaceData->SetLockForWrite(FALSE);
		}
		return hr;															
	}

	template<typename F>
	HRESULT	 Proxy_TexLockRect(F origFunc, IDirect3DTexture9* This, UINT Level, IDirect3DSurface9* Surf, D3DLOCKED_RECT * pLockedRect, CONST RECT * pRect, DWORD Flags)
	{
		BOOL bLockForWrite = FALSE;
		RECT lockedRect;
		D3DSURFACE_DESC desc;
		DWORD size = sizeof(IUnknown *);
		CComPtr<IWideSurfaceData> pWideSurfaceData;
		CBaseStereoRenderer* pCurrentStereoRenderer;
		GetCurrentStereoRenderer(Surf, &pCurrentStereoRenderer);
		if (SUCCEEDED(Surf->GetPrivateData(__uuidof(IWideSurfaceData), (void *)&pWideSurfaceData, &size)))
		{
			DEBUG_TRACE2(_T("Texture::LockRect(This = %p [%s], pLockedRect = %p, pRect = %s, Flags = %s)\n"),
				This, GetObjectName(This), 
				pLockedRect, GetRectString(pRect), GetLockFlagString(Flags));

			Surf->GetDesc(&desc);
			desc.Height /= HEIGHTMUL;
			if (pRect == NULL)
			{
				SetRect(&lockedRect, 0, 0, desc.Width, desc.Height);
				pRect = &lockedRect;
			}
			else
				lockedRect = *pRect;
			bLockForWrite = !(Flags & D3DLOCK_READONLY);
			if (bLockForWrite)
			{
				if (pCurrentStereoRenderer && pCurrentStereoRenderer->m_Input.StereoActive)
					bLockForWrite = FALSE;
			}
		}
		HRESULT hr = (This->*origFunc)(Level, pLockedRect, pRect, Flags);
		if (pWideSurfaceData)
		{
			if (SUCCEEDED(hr) && bLockForWrite)
			{
				_ASSERT(pCurrentStereoRenderer);
				pWideSurfaceData->SetLockForWrite(TRUE);
				pWideSurfaceData->SetLockedRect(&lockedRect);
				CComPtr<IDirect3DSurface9> pSurf;
				pCurrentStereoRenderer->m_Direct3DDevice.CreateRenderTarget(desc.Width, desc.Height, desc.Format, desc.MultiSampleType, desc.MultiSampleQuality,
					FALSE, &pSurf, NULL);
				pWideSurfaceData->SetAdditionalSurface(pSurf);
			}
			else
				pWideSurfaceData->SetLockForWrite(FALSE);
		}
		return hr;															
	}

	void	 Proxy_UnlockRect(IDirect3DSurface9* This)
	{
		INSIDE;
		DWORD size = sizeof(IUnknown *);
		CComPtr<IWideSurfaceData> pWideSurfaceData;
		if (SUCCEEDED(This->GetPrivateData(__uuidof(IWideSurfaceData), (void *)&pWideSurfaceData, &size)) && pWideSurfaceData->GetLockForWrite())
		{
			DEBUG_TRACE2(_T("Surface::UnlockRect(This = %p [%s])\n"),
				This, GetObjectName(This));

			CBaseStereoRenderer* pCurrentStereoRenderer;
			GetCurrentStereoRenderer(This, &pCurrentStereoRenderer);
			if (!pCurrentStereoRenderer)
				return;

			IDirect3DSurface9* pSurf = pWideSurfaceData->GetAdditionalSurface();
			CONST RECT* pLockedRect = pWideSurfaceData->GetLockedRect();
			CONST SIZE* pOffset = pWideSurfaceData->GetOffset();
			HRESULT hResult = pCurrentStereoRenderer->m_Direct3DDevice.StretchRect(This, pLockedRect, pSurf, NULL, D3DTEXF_POINT);
			RECT RightRect;
			SetRect(&RightRect, pLockedRect->left + pOffset->cx, pLockedRect->top + pOffset->cy,
				pLockedRect->right + pOffset->cx, pLockedRect->bottom + pOffset->cy);
			hResult = pCurrentStereoRenderer->m_Direct3DDevice.StretchRect(pSurf, NULL, This, &RightRect, D3DTEXF_POINT);
			CWideStereoRenderer::IncRevision(This);
		}														
	}

	PROXYSTDMETHOD(Surface_LockRect)(IDirect3DSurface9* This, D3DLOCKED_RECT * pLockedRect, CONST RECT * pRect, DWORD Flags)
	{
		if (CalledFromD3D9())
			return (This->*Original_Surface_LockRect)(pLockedRect, pRect, Flags);
		return Proxy_SurfLockRect(Original_Surface_LockRect, This, pLockedRect, pRect, Flags);
	}

	PROXYSTDMETHOD(Surface_UnlockRect)(IDirect3DSurface9* This)
	{
		HRESULT hResult = (This->*Original_Surface_UnlockRect)();
		if (SUCCEEDED(hResult) && CalledFromApp())
			Proxy_UnlockRect(This);
		return hResult;
	}

	PROXYSTDMETHOD(DepthSurface_LockRect)(IDirect3DSurface9* This, D3DLOCKED_RECT * pLockedRect, CONST RECT * pRect, DWORD Flags)
	{
		if (CalledFromD3D9())
			return (This->*Original_DepthSurface_LockRect)(pLockedRect, pRect, Flags);
		return Proxy_SurfLockRect(Original_DepthSurface_LockRect, This, pLockedRect, pRect, Flags);
	}

	PROXYSTDMETHOD(DepthSurface_UnlockRect)(IDirect3DSurface9* This)
	{
		HRESULT hResult = (This->*Original_DepthSurface_UnlockRect)();
		if (SUCCEEDED(hResult) && CalledFromApp())
			Proxy_UnlockRect(This);
		return hResult;
	}

	PROXYSTDMETHOD(TexSurface_LockRect)( IDirect3DSurface9* This, D3DLOCKED_RECT * pLockedRect, CONST RECT * pRect, DWORD Flags)
	{
		if (CalledFromD3D9() || IsInternalCall())
			return (This->*Original_TexSurface_PDef_LockRect)(pLockedRect, pRect, Flags);

		HRESULT hResult = Proxy_SurfLockRect(Original_TexSurface_PDef_LockRect, This, pLockedRect, pRect, Flags);
		if (SUCCEEDED(hResult))
			Proxy_Pitch(This, pLockedRect, pRect, Flags);
		return hResult;
	}

	PROXYSTDMETHOD(TexSurface_UnlockRect)(IDirect3DSurface9* This)
	{
		if (CalledFromD3D9() || IsInternalCall())
			return (This->*Original_TexSurface_PDef_UnlockRect)();

		Proxy_UnPitch(This);
		HRESULT hResult = (This->*Original_TexSurface_PDef_UnlockRect)();
		if (SUCCEEDED(hResult))
			Proxy_UnlockRect(This);
		return hResult;
	}

	PROXYSTDMETHOD(Texture_LockRect)(IDirect3DTexture9* This, UINT Level, D3DLOCKED_RECT * pLockedRect, CONST RECT * pRect, DWORD Flags)
	{
		if (CalledFromD3D9())
			return (This->*Original_Texture_LockRect)(Level, pLockedRect, pRect, Flags);

		WE_CALL;
		CComPtr<IDirect3DSurface9> s;
		This->GetSurfaceLevel(Level, &s);
		// TODO: Fix when Original_Texture_LockRect call Surface_LockRect
		HRESULT hResult = Proxy_TexLockRect(Original_Texture_LockRect, This, Level, s, pLockedRect, pRect, Flags);
		if (SUCCEEDED(hResult))
			Proxy_Pitch(s, pLockedRect, pRect, Flags);
		return hResult;
	}

	PROXYSTDMETHOD(Texture_UnlockRect)(IDirect3DTexture9* This, UINT Level)
	{
		if (CalledFromD3D9())
			return (This->*Original_Texture_UnlockRect)(Level);

		WE_CALL;
		CComPtr<IDirect3DSurface9> s;
		This->GetSurfaceLevel(Level, &s);
		Proxy_UnPitch(s);
		HRESULT hResult = (This->*Original_Texture_UnlockRect)(Level);
		if (SUCCEEDED(hResult))
			Proxy_UnlockRect(s);
		return hResult;
	}

	PROXYSTDMETHOD_(ULONG,Texture_Release)(IDirect3DTexture9* This)
	{
		//DEBUG_TRACE3(_T("Texture::Release(This = %p [%s])\n"),
		//	This, GetObjectName(This));				

		/*DWORD Size = sizeof(IUnknown *);
		IDirect3DTexture9* pWideTexture = NULL;
		This->GetPrivateData(WideTexture_GUID, &pWideTexture, &Size);
		if (pWideTexture)
		{
			// for G-Force
			ULONG wideRC = (pWideTexture->*Original_Texture_Release)();
			ULONG leftRC = This->AddRef() - 1;
			if (leftRC == 1 && wideRC > 2) {
				DEBUG_MESSAGE(_T("Release wide texture instead left\n"));
				DEBUG_MESSAGE(_T("\twideRC %d leftRC %d\n"), wideRC, leftRC);
				wideRC = (pWideTexture->*Original_Texture_Release)();
			} else {
				//DEBUG_TRACE3(_T("\twideRC %d leftRC %d\n"), wideRC, leftRC);
				leftRC = (This->*Original_Texture_Release)();
			}
		}*/

		ULONG rc = (This->*Original_Texture_Release)();
		//DEBUG_TRACE3(_T("\trc %d\n"), rc);
		return rc;
	}

	PROXYSTDMETHOD_(ULONG,Surface_Release)(IDirect3DSurface9* This)
	{
		//DEBUG_TRACE3(_T("Surface::Release(This = %p [%s])\n"),
		//	This, GetObjectName(This));
		ULONG rc = (This->*Original_Surface_Release)();
		//DEBUG_TRACE3(_T("\trc %d\n"), rc);
		return rc;
	}
}
