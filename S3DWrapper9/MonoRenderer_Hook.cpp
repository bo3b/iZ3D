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
#include "MonoRenderer_Hook.h"
#include "MonoRenderer.h"

//--- All interfaces GetDevice() interception data ---
HRESULT (__stdcall IDirect3DCubeTexture9::*Original_CubeTexture_GetDevice)(IDirect3DDevice9** ppDevice);
HRESULT (__stdcall IDirect3DIndexBuffer9::*Original_IndexBuffer_GetDevice)(IDirect3DDevice9** ppDevice);
HRESULT (__stdcall IDirect3DQuery9::*Original_Query_GetDevice)(IDirect3DDevice9** ppDevice);
HRESULT (__stdcall IDirect3DStateBlock9::*Original_StateBlock_GetDevice)(IDirect3DDevice9** ppDevice);
HRESULT (__stdcall IDirect3DVertexBuffer9::*Original_VertexBuffer_GetDevice)(IDirect3DDevice9** ppDevice);
HRESULT (__stdcall IDirect3DVertexDeclaration9::*Original_VertexDeclaration_GetDevice)(IDirect3DDevice9** ppDevice);
HRESULT (__stdcall IDirect3DVertexShader9::*Original_VertexShader_GetDevice)(IDirect3DDevice9** ppDevice);
HRESULT (__stdcall IDirect3DVolumeTexture9::*Original_VolumeTexture_GetDevice)(IDirect3DDevice9** ppDevice);
HRESULT (__stdcall IDirect3DPixelShader9::*Original_PixelShader_GetDevice)(IDirect3DDevice9** ppDevice);
HRESULT (__stdcall IDirect3DSurface9::*Original_TextureSurface_GetDevice)(IDirect3DDevice9** ppDevice);
HRESULT (__stdcall IDirect3DSurface9::*Original_Surface_GetDevice)(IDirect3DDevice9** ppDevice);
HRESULT (__stdcall IDirect3DTexture9::*Original_Texture_GetDevice)(IDirect3DDevice9** ppDevice);

template<typename T, typename F>
HRESULT	 Proxy_GetDevice(F origFunc, T* pInterface, IDirect3DDevice9** ppDevice)
{
	DEBUG_TRACE3(_T("GetDevice()\n"));
	HRESULT hr = (pInterface->*origFunc)(ppDevice);
	CMonoRenderer* pCurrentStereoRenderer = g_pRendererList.FindWrapper(*ppDevice);
	if (pCurrentStereoRenderer) {		
		pCurrentStereoRenderer->AddRef();
		(*ppDevice)->Release();
		*ppDevice = (IDirect3DDevice9*)pCurrentStereoRenderer;
	}
	else
		DEBUG_TRACE3(_T("\tDevice not found\n"));
	return hr;
}

#define DEFINE_GET_DEVICE(InterfaceName, FunctionName, OriginalFunction)												\
PROXYSTDMETHOD(FunctionName)(InterfaceName pInterface, IDirect3DDevice9** ppDevice)										\
{																														\
	if (CalledFromD3D9())																								\
		return (pInterface->*OriginalFunction)(ppDevice);																\
	return Proxy_GetDevice(OriginalFunction, pInterface, ppDevice);														\
}	

DEFINE_GET_DEVICE(IDirect3DCubeTexture9*, CubeTexture_GetDevice, Original_CubeTexture_GetDevice);
DEFINE_GET_DEVICE(IDirect3DIndexBuffer9*, IndexBuffer_GetDevice, Original_IndexBuffer_GetDevice);
DEFINE_GET_DEVICE(IDirect3DPixelShader9*, PixelShader_GetDevice, Original_PixelShader_GetDevice);
DEFINE_GET_DEVICE(IDirect3DQuery9*, Query_GetDevice, Original_Query_GetDevice);
DEFINE_GET_DEVICE(IDirect3DStateBlock9*, StateBlock_GetDevice, Original_StateBlock_GetDevice);
DEFINE_GET_DEVICE(IDirect3DSurface9*, TextureSurface_GetDevice, Original_TextureSurface_GetDevice);
DEFINE_GET_DEVICE(IDirect3DSurface9*, Surface_GetDevice, Original_Surface_GetDevice);
DEFINE_GET_DEVICE(IDirect3DTexture9*, Texture_GetDevice, Original_Texture_GetDevice);
DEFINE_GET_DEVICE(IDirect3DVertexBuffer9*, VertexBuffer_GetDevice, Original_VertexBuffer_GetDevice);
DEFINE_GET_DEVICE(IDirect3DVertexDeclaration9*, VertexDeclaration_GetDevice, Original_VertexDeclaration_GetDevice);
DEFINE_GET_DEVICE(IDirect3DVertexShader9*, VertexShader_GetDevice, Original_VertexShader_GetDevice);
DEFINE_GET_DEVICE(IDirect3DVolumeTexture9*, VolumeTexture_GetDevice, Original_VolumeTexture_GetDevice);
