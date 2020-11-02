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
#include "StereoRenderer.h"
#include "StereoRenderer_Hook.h"
#include "ProxyDevice9-inl.h"
#include "CommandDumper.h"

namespace NonWide
{
	HRESULT (__stdcall IDirect3DSurface9::*Original_Surface_LockRect)(D3DLOCKED_RECT * pLockedRect,CONST RECT * pRect,DWORD Flags);
	HRESULT (__stdcall IDirect3DSurface9::*Original_TexSurface_PDef_LockRect)(D3DLOCKED_RECT * pLockedRect,CONST RECT * pRect,DWORD Flags);
	HRESULT (__stdcall IDirect3DSurface9::*Original_DepthSurface_LockRect)(D3DLOCKED_RECT * pLockedRect,CONST RECT * pRect,DWORD Flags);
	HRESULT (__stdcall IDirect3DTexture9::*Original_Texture_LockRect)(UINT Level, D3DLOCKED_RECT * pLockedRect,CONST RECT * pRect,DWORD Flags);
	HRESULT (__stdcall IDirect3DSurface9::*Original_Surface_UnlockRect)();
	HRESULT (__stdcall IDirect3DSurface9::*Original_TexSurface_PDef_UnlockRect)();
	HRESULT (__stdcall IDirect3DSurface9::*Original_DepthSurface_UnlockRect)();
	HRESULT (__stdcall IDirect3DTexture9::*Original_Texture_UnlockRect)(UINT Level);

	struct SyncRect
	{
		SyncRect() {};
		SyncRect(CONST RECT * pRect) 
		{
			RectIsNull = !pRect;
			if(pRect)
				rect = *pRect;
		};

		BOOL RectIsNull;
		RECT rect;
		static const GUID ID;
	};

	const GUID SyncRect::ID = { 0xb31801c3, 0x70c2, 0x415c, { 0x94, 0x5f, 0xfa, 0x84, 0x2c, 0xfe, 0xed, 0x72 } };

	void	 Proxy_LockRect(IDirect3DSurface9* This, D3DLOCKED_RECT * pLockedRect, CONST RECT * pRect, DWORD Flags)
	{
		CBaseStereoRenderer* pCurrentStereoRenderer;
		GetCurrentStereoRenderer(This, &pCurrentStereoRenderer);
		if (pCurrentStereoRenderer && pCurrentStereoRenderer->m_Input.StereoActive)
		{
			if(!(Flags & D3DLOCK_READONLY))
			{
				DEBUG_TRACE2(_T( __FUNCTION__ ) _T("(This = %p [%s], pLockedRect = %s, pRect = %s, Flags = %s)\n"),
					This, GetObjectName(This), GetLockedRectString(pLockedRect),
					GetRectString(pRect), GetLockFlagString(Flags));

				CComPtr<IDirect3DSurface9> pRightSurface;
				DWORD size = sizeof(IUnknown *);
				This->GetPrivateData(__uuidof(CBaseStereoRenderer), (void *)&pRightSurface, &size);
				if(pRightSurface)
				{
					SyncRect sr(pRect);
					This->SetPrivateData(SyncRect::ID, &sr, sizeof(SyncRect), 0);
				}
			}
		}
	}

	void	 Proxy_UnlockRect(IDirect3DSurface9* This)
	{
		INSIDE;
		CBaseStereoRenderer* pCurrentStereoRenderer;
		GetCurrentStereoRenderer(This, &pCurrentStereoRenderer);
		if (pCurrentStereoRenderer && pCurrentStereoRenderer->m_Input.StereoActive)
		{
			DEBUG_TRACE2(_T( __FUNCTION__ ) _T("(This = %p [%s])\n"),	This, GetObjectName(This));

			SyncRect sr;
			DWORD size = sizeof(SyncRect);
			if (SUCCEEDED(This->GetPrivateData(SyncRect::ID, (void *)&sr, &size)))
			{
				CComPtr<IDirect3DSurface9> pRightSurface;
				DWORD size = sizeof(IUnknown *);
				This->GetPrivateData(__uuidof(CBaseStereoRenderer), (void *)&pRightSurface, &size);
				if(pRightSurface)
				{
					RECT* p = sr.RectIsNull ? NULL : &sr.rect;
					pCurrentStereoRenderer->m_Direct3DDevice.StretchRect(This, p, pRightSurface, p, D3DTEXF_POINT);
				}
				This->FreePrivateData(SyncRect::ID);
			}
		}
	}	

#define DefineLockerFunctions(FunctionLock, FunctionUnLock)																	\
	PROXYSTDMETHOD(FunctionLock)(IDirect3DSurface9* This, D3DLOCKED_RECT * pLockedRect, CONST RECT * pRect, DWORD Flags)	\
	{																														\
		HRESULT hResult = (This->*Original_##FunctionLock)(pLockedRect, pRect, Flags);										\
		if (SUCCEEDED(hResult) && CalledFromApp())																			\
			Proxy_LockRect(This, pLockedRect, pRect, Flags);																\
		return hResult;																										\
	}																														\
	PROXYSTDMETHOD(FunctionUnLock)(IDirect3DSurface9* This)																	\
	{																														\
		HRESULT hResult = (This->*Original_##FunctionUnLock)();																\
		if (SUCCEEDED(hResult) && CalledFromApp())																			\
			Proxy_UnlockRect(This);																							\
		return hResult;																										\
	}																														

	DefineLockerFunctions(Surface_LockRect,		 Surface_UnlockRect);
	DefineLockerFunctions(DepthSurface_LockRect, DepthSurface_UnlockRect);

	PROXYSTDMETHOD(TexSurface_LockRect)(IDirect3DSurface9* This, D3DLOCKED_RECT * pLockedRect, CONST RECT * pRect, DWORD Flags)
	{					
		HRESULT hResult = (This->*Original_TexSurface_PDef_LockRect)(pLockedRect, pRect, Flags);		
		if (SUCCEEDED(hResult) && CalledFromApp() && !IsInternalCall())
		{
			Proxy_LockRect(This, pLockedRect, pRect, Flags);
			Proxy_Pitch(This, pLockedRect, pRect, Flags);
		}
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
		HRESULT hResult = (This->*Original_Texture_LockRect)(Level, pLockedRect, pRect, Flags);
		if (CalledFromApp() && SUCCEEDED(hResult))
		{	
			WE_CALL;
			CComPtr<IDirect3DSurface9> s;
			This->GetSurfaceLevel(Level, &s);
			Proxy_LockRect(s, pLockedRect, pRect, Flags);
			Proxy_Pitch(s, pLockedRect, pRect, Flags);
		}
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
		HRESULT hResult =(s->*Original_TexSurface_PDef_UnlockRect)();
		if (SUCCEEDED(hResult))
			Proxy_UnlockRect(s);
		return hResult;
	}

}
