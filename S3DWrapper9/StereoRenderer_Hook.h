/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "BaseStereoRenderer_Hook.h"

namespace NonWide
{
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
