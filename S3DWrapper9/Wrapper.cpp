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
#include "Wrapper.h"
#include "WrapperTable.h"
#include "Direct3D9.h"

#define CREATE_WRAPPER(D3D9) \
	CComQIPtr<I##D3D9> p##D3D9(pReal); \
	if (p##D3D9) \
	{ \
		NSCALL(C##D3D9::CreateInstance(ppWrapper)); \
		return hResult; \
	} 

HRESULT ConstructWrapper(IUnknown *pReal, IWrapper **ppWrapper)
{
	HRESULT hResult = S_OK;
	CREATE_WRAPPER(Direct3D9);
	return hResult;
}

HRESULT ConstructWrapper(IDirect3D9 *pReal, IWrapper **ppWrapper)
{
	HRESULT hResult = S_OK;
	NSCALL(CDirect3D9::CreateInstance(ppWrapper));
	return hResult;
}

char * GetUUID(IDirect3D9 *pReal)
{
	return "IID_IDirect3D9";
}

char * GetUUID(IDirect3DDevice9 *pReal)
{
	return "IID_IDirect3DDevice9";
}

char * GetUUID(IDirect3DCubeTexture9 *pReal)
{
	return "IID_IDirect3DCubeTexture9";
}

char * GetUUID(IDirect3DIndexBuffer9 *pReal)
{
	return "IID_IDirect3DIndexBuffer9";
}

char * GetUUID(IDirect3DPixelShader9 *pReal)
{
	return "IID_IDirect3DPixelShader9";
}

char * GetUUID(IDirect3DQuery9 *pReal)
{
	return "IID_IDirect3DQuery9";
}

char * GetUUID(IDirect3DStateBlock9 *pReal)
{
	return "IID_IDirect3DStateBlock9";
}

char * GetUUID(IDirect3DSurface9 *pReal)
{
	return "IID_IDirect3DSurface9";
}

char * GetUUID(IDirect3DSwapChain9 *pReal)
{
	return "IID_IDirect3DSwapChain9";
}

char * GetUUID(IDirect3DTexture9 *pReal)
{
	return "IID_IDirect3DTexture9";
}

char * GetUUID(IDirect3DVertexBuffer9 *pReal)
{
	return "IID_IDirect3DVertexBuffer9";
}

char * GetUUID(IDirect3DVertexDeclaration9 *pReal)
{
	return "IID_IDirect3DVertexDeclaration9";
}

char * GetUUID(IDirect3DVertexShader9 *pReal)
{
	return "IID_IDirect3DVertexShader9";
}

char * GetUUID(IDirect3DVolume9 *pReal)
{
	return "IID_IDirect3DVolume9";
}

char * GetUUID(IDirect3DVolumeTexture9 *pReal)
{
	return "IID_IDirect3DVolumeTexture9";
}

char * GetUUID(IDirect3DBaseTexture9 *pReal)
{
	return "IID_IDirect3DBaseTexture9";
}

#define GET_UUID(Text) \
{\
	CComQIPtr<I##Text> p##Text(pUnknown); \
	if (p##Text) \
	{ \
		return "IID_I"#Text; \
	} \
}
char * GetUUID(IUnknown *pUnknown)
{
	GET_UUID(Direct3D9);
	GET_UUID(Direct3DDevice9);
	GET_UUID(Direct3DCubeTexture9);
	GET_UUID(Direct3DIndexBuffer9);
	GET_UUID(Direct3DPixelShader9);
	GET_UUID(Direct3DQuery9);
	GET_UUID(Direct3DStateBlock9);
	GET_UUID(Direct3DSurface9);
	GET_UUID(Direct3DSwapChain9);
	GET_UUID(Direct3DTexture9);
	GET_UUID(Direct3DVertexBuffer9);
	GET_UUID(Direct3DVertexDeclaration9);
	GET_UUID(Direct3DVertexShader9);
	GET_UUID(Direct3DVolume9);
	GET_UUID(Direct3DVolumeTexture9);
	return "Unrecognized";
}

