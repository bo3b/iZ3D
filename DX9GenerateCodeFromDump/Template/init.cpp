#include "stdafx.h"
#include "main.h"

HRESULT StateBlock_Apply(IDirect3DStateBlock9 *sblock)
{
	return sblock->Apply();
}

HRESULT StateBlock_Capture(IDirect3DStateBlock9 *sblock)
{
	return sblock->Capture();
}

HRESULT GetSurfaceLevel(IDirect3DTexture9 *tex,UINT level,IDirect3DSurface9 **surf)
{
	return tex->GetSurfaceLevel(level,surf);
}

HRESULT GetCubeMapSurface(IDirect3DCubeTexture9 *cube,D3DCUBEMAP_FACES face,UINT level,IDirect3DSurface9 **surf)
{
	return cube->GetCubeMapSurface(face,level,surf);
}

HRESULT BeginDeviceState()
{
	return defaultState->Apply();
}

HRESULT Init(HWND hWnd)
{
	HRESULT hr = S_OK;
	g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if(!g_pD3D) return E_FAIL;

@@INITIALIZE@@

	return S_OK;
}
