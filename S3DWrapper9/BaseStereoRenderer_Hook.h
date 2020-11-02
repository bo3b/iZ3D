/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "Hook.h"

//typedef HRESULT (__stdcall IDirect3DTexture9::*Surface_LockRect_t)(D3DLOCKED_RECT * pLockedRect,CONST RECT * pRect,DWORD Flags);
//typedef HRESULT (__stdcall IDirect3DTexture9::*Surface_UnlockRect_t)();
typedef HRESULT (__stdcall IDirect3DSurface9::*Surface_LockRect_t)(D3DLOCKED_RECT * pLockedRect,CONST RECT * pRect,DWORD Flags);
typedef HRESULT (__stdcall IDirect3DSurface9::*Surface_UnlockRect_t)();
typedef HRESULT (__stdcall IDirect3DTexture9::*Texture_LockRect_t)(UINT Level, D3DLOCKED_RECT * pLockedRect,CONST RECT * pRect,DWORD Flags);
typedef HRESULT (__stdcall IDirect3DTexture9::*Texture_UnlockRect_t)(UINT Level);

typedef HRESULT (__stdcall IDirect3DVolume9::*Volume_LockBox_t)(D3DLOCKED_BOX * pLockedVolume, CONST D3DBOX * pBox, DWORD Flags);
typedef HRESULT (__stdcall IDirect3DVolume9::*Volume_UnlockBox_t)();
extern Volume_LockBox_t		Original_Volume_LockBox;
extern Volume_UnlockBox_t	Original_Volume_UnlockBox;
PROXYSTDMETHOD(Volume_LockBox)(IDirect3DVolume9* This, D3DLOCKED_BOX * pLockedVolume, CONST D3DBOX * pBox, DWORD Flags);
PROXYSTDMETHOD(Volume_UnlockBox)(IDirect3DVolume9* This);

PROXYSTDMETHOD(VertexBuffer_LockBox)(IDirect3DVertexBuffer9* This, UINT OffsetToLock,UINT SizeToLock,VOID ** ppbData,DWORD Flags);
PROXYSTDMETHOD(VertexBuffer_Unlock)(IDirect3DVertexBuffer9* This);
PROXYSTDMETHOD(DriverVertexBuffer_Lock)(IDirect3DVertexBuffer9* This, UINT OffsetToLock,UINT SizeToLock,VOID ** ppbData,DWORD Flags);
PROXYSTDMETHOD(DriverVertexBuffer_Unlock)(IDirect3DVertexBuffer9* This);

extern ULONG (__stdcall IDirect3DSurface9::*Original_Surface_AddRef)();
extern ULONG (__stdcall IDirect3DSurface9::*Original_Surface_Release)();
extern ULONG (__stdcall IDirect3DSurface9::*Original_TexSurf_AddRef)();
extern ULONG (__stdcall IDirect3DSurface9::*Original_TexSurf_Release)();
extern ULONG (__stdcall IDirect3DTexture9::*Original_Texture_AddRef)();
extern ULONG (__stdcall IDirect3DTexture9::*Original_Texture_Release)();
extern ULONG (__stdcall IDirect3DPixelShader9::*Original_PixelShader_AddRef)();
extern ULONG (__stdcall IDirect3DPixelShader9::*Original_PixelShader_Release)();
extern ULONG (__stdcall IDirect3DVertexShader9::*Original_VertexShader_AddRef)();
extern ULONG (__stdcall IDirect3DVertexShader9::*Original_VertexShader_Release)();
PROXYSTDMETHOD_(ULONG,Surface_AddRef)(IDirect3DSurface9* This);
PROXYSTDMETHOD_(ULONG,Surface_Release)(IDirect3DSurface9* This);
PROXYSTDMETHOD_(ULONG,TexSurf_AddRef)(IDirect3DSurface9* This);
PROXYSTDMETHOD_(ULONG,TexSurf_Release)(IDirect3DSurface9* This);
PROXYSTDMETHOD_(ULONG,Texture_AddRef)(IDirect3DTexture9* This);
PROXYSTDMETHOD_(ULONG,Texture_Release)(IDirect3DTexture9* This);
PROXYSTDMETHOD_(ULONG,PixelShader_AddRef)(IDirect3DPixelShader9* This);
PROXYSTDMETHOD_(ULONG,PixelShader_Release)(IDirect3DPixelShader9* This);
PROXYSTDMETHOD_(ULONG,VertexShader_AddRef)(IDirect3DVertexShader9* This);
PROXYSTDMETHOD_(ULONG,VertexShader_Release)(IDirect3DVertexShader9* This);

//--- IDirect3DStateBlock9 interception data ---
extern HRESULT (__stdcall IDirect3DStateBlock9::*Original_StateBlock_Apply)();
PROXYSTDMETHOD(StateBlock_Apply)(IDirect3DStateBlock9* pBlock);

#ifndef FINAL_RELEASE
extern HRESULT (__stdcall IDirect3DStateBlock9::*Original_StateBlock_Capture)();
PROXYSTDMETHOD(StateBlock_Capture)(IDirect3DStateBlock9* pBlock);
#endif // FINAL_RELEASE

//--- IDirect3DVertexBuffer9 interception data ---
typedef HRESULT (__stdcall IDirect3DVertexBuffer9::*VertexBuffer_Lock_t)(UINT OffsetToLock,UINT SizeToLock,VOID ** ppbData,DWORD Flags);
typedef HRESULT (__stdcall IDirect3DVertexBuffer9::*VertexBuffer_Unlock_t)();
extern VertexBuffer_Lock_t		Original_VertexBuffer_Lock;
extern VertexBuffer_Unlock_t	Original_VertexBuffer_Unlock;
extern VertexBuffer_Lock_t		Original_DriverVertexBuffer_Lock;
extern VertexBuffer_Unlock_t	Original_DriverVertexBuffer_Unlock;

PROXYSTDMETHOD(VertexBuffer_Lock)(IDirect3DVertexBuffer9* This, UINT OffsetToLock,UINT SizeToLock,VOID ** ppbData,DWORD Flags);
PROXYSTDMETHOD(VertexBuffer_Unlock)(IDirect3DVertexBuffer9* This);
PROXYSTDMETHOD(DriverVertexBuffer_Lock)(IDirect3DVertexBuffer9* This, UINT OffsetToLock,UINT SizeToLock,VOID ** ppbData,DWORD Flags);
PROXYSTDMETHOD(DriverVertexBuffer_Unlock)(IDirect3DVertexBuffer9* This);

//--- Hooks for SetViewStage() optimization ---
PROXYSTDMETHOD(SetVertexDeclaration)(IDirect3DDevice9* This, IDirect3DVertexDeclaration9* pDecl);
PROXYSTDMETHOD(SetFVF)(IDirect3DDevice9* This, DWORD FVF);
PROXYSTDMETHOD(SetVertexShader)(IDirect3DDevice9* This, IDirect3DVertexShader9* pShader);
PROXYSTDMETHOD(GetVertexShader)(IDirect3DDevice9* This, IDirect3DVertexShader9** ppShader);
PROXYSTDMETHOD(SetPixelShader)(IDirect3DDevice9* This, IDirect3DPixelShader9* pShader);
PROXYSTDMETHOD(SetStreamSource)(IDirect3DDevice9* This, UINT StreamNumber, IDirect3DVertexBuffer9 * pStreamData, UINT OffsetInBytes, UINT Stride);
PROXYSTDMETHOD(SetTexture)(IDirect3DDevice9* This, DWORD Sampler, IDirect3DBaseTexture9 * pTexture);
PROXYSTDMETHOD(SetRenderState)(IDirect3DDevice9* This, D3DRENDERSTATETYPE State, DWORD Value);
PROXYSTDMETHOD(SetRenderState)(IDirect3DDevice9* This, D3DRENDERSTATETYPE State, DWORD Value);
PROXYSTDMETHOD(SetVertexShaderConstantF)(IDirect3DDevice9* This, UINT StartRegister, CONST float * pConstantData, UINT Vector4fCount);
PROXYSTDMETHOD(GetVertexShaderConstantF)(IDirect3DDevice9* This, UINT StartRegister,	   float * pConstantData, UINT Vector4fCount);
PROXYSTDMETHOD(SetTransform)(IDirect3DDevice9* This, D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX * pMatrix);
PROXYSTDMETHOD(MultiplyTransform)(IDirect3DDevice9* This, D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX * pMatrix);
PROXYSTDMETHOD(SetViewport)(IDirect3DDevice9* This, CONST D3DVIEWPORT9 * pViewport);
PROXYSTDMETHOD(SetScissorRect)(IDirect3DDevice9* This, CONST RECT * pRect);