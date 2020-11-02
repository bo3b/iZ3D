/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "BaseStereoRenderer_Hook.h"

namespace Wide
{
	extern HRESULT (__stdcall IDirect3DTexture9::*Original_Texture_GetLevelDesc)(UINT Level, D3DSURFACE_DESC *pDesc);
	extern HRESULT (__stdcall IDirect3DSurface9::*Original_Surface_GetDesc)(D3DSURFACE_DESC *pDesc);
	extern HRESULT (__stdcall IDirect3DSurface9::*Original_TexSurface_GetDesc)(D3DSURFACE_DESC *pDesc);
	extern HRESULT (__stdcall IDirect3DSurface9::*Original_DepthSurface_GetDesc)(D3DSURFACE_DESC *pDesc);
	extern ULONG (__stdcall IDirect3DSurface9::*Original_Surface_Release)();
	extern ULONG (__stdcall IDirect3DTexture9::*Original_Texture_Release)();

	PROXYSTDMETHOD(Texture_GetLevelDesc)(IDirect3DTexture9* This, UINT Level, D3DSURFACE_DESC *pDesc);
	PROXYSTDMETHOD(Surface_GetDesc)(IDirect3DSurface9* This, D3DSURFACE_DESC *pDesc);
	PROXYSTDMETHOD(TexSurface_GetDesc)(IDirect3DSurface9* This, D3DSURFACE_DESC *pDesc);
	PROXYSTDMETHOD(DepthSurface_GetDesc)(IDirect3DSurface9* This, D3DSURFACE_DESC *pDesc);
	PROXYSTDMETHOD_(ULONG,Surface_Release)(IDirect3DSurface9* This);
	PROXYSTDMETHOD_(ULONG,Texture_Release)(IDirect3DTexture9* This);

	extern Surface_LockRect_t Original_Surface_LockRect;
	extern Surface_LockRect_t Original_TexSurface_PDef_LockRect;
	extern Surface_LockRect_t Original_DepthSurface_LockRect;
	extern Texture_LockRect_t Original_Texture_LockRect;
	extern Surface_UnlockRect_t Original_Surface_UnlockRect;
	extern Surface_UnlockRect_t Original_TexSurface_PDef_UnlockRect;
	extern Surface_UnlockRect_t Original_DepthSurface_UnlockRect;
	extern Texture_UnlockRect_t Original_Texture_UnlockRect;

	PROXYSTDMETHOD(Surface_LockRect)(IDirect3DSurface9* This, D3DLOCKED_RECT * pLockedRect,CONST RECT * pRect,DWORD Flags);
	PROXYSTDMETHOD(TexSurface_LockRect)(IDirect3DSurface9* This, D3DLOCKED_RECT * pLockedRect,CONST RECT * pRect,DWORD Flags);
	PROXYSTDMETHOD(DepthSurface_LockRect)(IDirect3DSurface9* This, D3DLOCKED_RECT * pLockedRect,CONST RECT * pRect,DWORD Flags);
	PROXYSTDMETHOD(Texture_LockRect)(IDirect3DTexture9* This, UINT Level, D3DLOCKED_RECT * pLockedRect,CONST RECT * pRect,DWORD Flags);
	PROXYSTDMETHOD(Surface_UnlockRect)(IDirect3DSurface9* This);
	PROXYSTDMETHOD(TexSurface_UnlockRect)(IDirect3DSurface9* This);
	PROXYSTDMETHOD(DepthSurface_UnlockRect)(IDirect3DSurface9* This);
	PROXYSTDMETHOD(Texture_UnlockRect)(IDirect3DTexture9* This, UINT Level);
}
