/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "Hook.h"
	
//--- All interfaces GetDevice() interception data ---
extern HRESULT (__stdcall IDirect3DCubeTexture9::*Original_CubeTexture_GetDevice)(IDirect3DDevice9** ppDevice);
extern HRESULT (__stdcall IDirect3DIndexBuffer9::*Original_IndexBuffer_GetDevice)(IDirect3DDevice9** ppDevice);
extern HRESULT (__stdcall IDirect3DQuery9::*Original_Query_GetDevice)(IDirect3DDevice9** ppDevice);
extern HRESULT (__stdcall IDirect3DStateBlock9::*Original_StateBlock_GetDevice)(IDirect3DDevice9** ppDevice);
extern HRESULT (__stdcall IDirect3DVertexBuffer9::*Original_VertexBuffer_GetDevice)(IDirect3DDevice9** ppDevice);
extern HRESULT (__stdcall IDirect3DVertexDeclaration9::*Original_VertexDeclaration_GetDevice)(IDirect3DDevice9** ppDevice);
extern HRESULT (__stdcall IDirect3DVertexShader9::*Original_VertexShader_GetDevice)(IDirect3DDevice9** ppDevice);
extern HRESULT (__stdcall IDirect3DVolumeTexture9::*Original_VolumeTexture_GetDevice)(IDirect3DDevice9** ppDevice);
extern HRESULT (__stdcall IDirect3DPixelShader9::*Original_PixelShader_GetDevice)(IDirect3DDevice9** ppDevice);
extern HRESULT (__stdcall IDirect3DSurface9::*Original_TextureSurface_GetDevice)(IDirect3DDevice9** ppDevice);
extern HRESULT (__stdcall IDirect3DSurface9::*Original_Surface_GetDevice)(IDirect3DDevice9** ppDevice);
extern HRESULT (__stdcall IDirect3DTexture9::*Original_Texture_GetDevice)(IDirect3DDevice9** ppDevice);

#define DECLARE_GET_DEVICE(InterfaceName, FunctionName)																	\
	PROXYSTDMETHOD(FunctionName)(InterfaceName pInterface, IDirect3DDevice9** ppDevice)									\

DECLARE_GET_DEVICE(IDirect3DCubeTexture9*, CubeTexture_GetDevice);
DECLARE_GET_DEVICE(IDirect3DIndexBuffer9*, IndexBuffer_GetDevice);
DECLARE_GET_DEVICE(IDirect3DQuery9*, Query_GetDevice);
DECLARE_GET_DEVICE(IDirect3DStateBlock9*, StateBlock_GetDevice);
DECLARE_GET_DEVICE(IDirect3DVertexBuffer9*, VertexBuffer_GetDevice);
DECLARE_GET_DEVICE(IDirect3DVertexDeclaration9*, VertexDeclaration_GetDevice);
DECLARE_GET_DEVICE(IDirect3DVertexShader9*, VertexShader_GetDevice);
DECLARE_GET_DEVICE(IDirect3DVolumeTexture9*, VolumeTexture_GetDevice);
DECLARE_GET_DEVICE(IDirect3DPixelShader9*, PixelShader_GetDevice);
DECLARE_GET_DEVICE(IDirect3DSurface9*, Surface_GetDevice);
DECLARE_GET_DEVICE(IDirect3DSurface9*, TextureSurface_GetDevice);
DECLARE_GET_DEVICE(IDirect3DTexture9*, Texture_GetDevice);
