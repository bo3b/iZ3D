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
#include "Wrapper.h"
#include "WrapperTable.h"
#include "Direct3D8.h"
#include "Direct3DDevice8.h"
#include "Direct3DCubeTexture8.h"
#include "Direct3DIndexBuffer8.h"
#include "Direct3DSurface8.h"
#include "Direct3DSwapChain8.h"
#include "Direct3DTexture8.h"
#include "Direct3DVertexBuffer8.h"
#include "Direct3DVolume8.h"
#include "Direct3DVolumeTexture8.h"
#include "Direct3DShader8.h"
#include <Psapi.h>

#define CREATE_WRAPPER(D3D9, D3D8) \
    CComQIPtr<I##D3D9> p##D3D9(pReal); \
    if (p##D3D9) \
    { \
    	NSCALL(C##D3D8::CreateInstance(ppWrapper)); \
    	return hResult; \
    } 

HRESULT ConstructWrapper(IUnknown *pReal, IWrapper **ppWrapper)
{
    HRESULT hResult = E_FAIL;
    CREATE_WRAPPER(Direct3D9, Direct3D8);
    CREATE_WRAPPER(Direct3DDevice9, Direct3DDevice8);
    CREATE_WRAPPER(Direct3DCubeTexture9, Direct3DCubeTexture8);
    CREATE_WRAPPER(Direct3DIndexBuffer9, Direct3DIndexBuffer8);
    CREATE_WRAPPER(Direct3DSurface9, Direct3DSurface8);
    CREATE_WRAPPER(Direct3DSwapChain9, Direct3DSwapChain8);
    CREATE_WRAPPER(Direct3DTexture9, Direct3DTexture8);
    CREATE_WRAPPER(Direct3DVertexBuffer9, Direct3DVertexBuffer8);
    CREATE_WRAPPER(Direct3DVertexDeclaration9, Direct3DVertexShader8);
    CREATE_WRAPPER(Direct3DVolume9, Direct3DVolume8);
    CREATE_WRAPPER(Direct3DVolumeTexture9, Direct3DVolumeTexture8);
    return hResult;
}

HRESULT ConstructWrapper(IDirect3D9 *pReal, IWrapper **ppWrapper)
{
	HRESULT hResult = S_OK;
	NSCALL(CDirect3D8::CreateInstance(ppWrapper));
	return hResult;
}

HRESULT ConstructWrapper(IDirect3DDevice9 *pReal, IWrapper **ppWrapper)
{
	HRESULT hResult = S_OK;
	NSCALL(CDirect3DDevice8::CreateInstance(ppWrapper));
	return hResult;
}

HRESULT ConstructWrapper(IDirect3DCubeTexture9 *pReal, IWrapper **ppWrapper)
{
	HRESULT hResult = S_OK;
	NSCALL(CDirect3DCubeTexture8::CreateInstance(ppWrapper));
	return hResult;
}

HRESULT ConstructWrapper(IDirect3DIndexBuffer9 *pReal, IWrapper **ppWrapper)
{
	HRESULT hResult = S_OK;
	NSCALL(CDirect3DIndexBuffer8::CreateInstance(ppWrapper));
	return hResult;
}

HRESULT ConstructWrapper(IDirect3DSurface9 *pReal, IWrapper **ppWrapper)
{
	HRESULT hResult = S_OK;
	NSCALL(CDirect3DSurface8::CreateInstance(ppWrapper));
	return hResult;
}

HRESULT ConstructWrapper(IDirect3DSwapChain9 *pReal, IWrapper **ppWrapper)
{
	HRESULT hResult = S_OK;;
	NSCALL(CDirect3DSwapChain8::CreateInstance(ppWrapper));
	return hResult;
}

HRESULT ConstructWrapper(IDirect3DTexture9 *pReal, IWrapper **ppWrapper)
{
	HRESULT hResult = S_OK;
	NSCALL(CDirect3DTexture8::CreateInstance(ppWrapper));
	return hResult;
}

HRESULT ConstructWrapper(IDirect3DVertexBuffer9 *pReal, IWrapper **ppWrapper)
{
	HRESULT hResult = S_OK;
	NSCALL(CDirect3DVertexBuffer8::CreateInstance(ppWrapper));
	return hResult;
}

HRESULT ConstructWrapper(IDirect3DVolume9 *pReal, IWrapper **ppWrapper)
{
	HRESULT hResult = S_OK;
	NSCALL(CDirect3DVolume8::CreateInstance(ppWrapper));
	return hResult;
}

HRESULT ConstructWrapper(IDirect3DVolumeTexture9 *pReal, IWrapper **ppWrapper)
{
	HRESULT hResult = S_OK;
	NSCALL(CDirect3DVolumeTexture8::CreateInstance(ppWrapper));
	return hResult;
}

HRESULT ConstructWrapper(IDirect3DBaseTexture9 *pReal, IWrapper **ppWrapper)
{
	HRESULT hResult = E_FAIL;
	CComQIPtr<IDirect3DTexture9> pDirect3DTexture9(pReal);
	if (pDirect3DTexture9)
	{
		return ConstructWrapper(pDirect3DTexture9, ppWrapper);
	}
	CComQIPtr<IDirect3DVolumeTexture9> pDirect3DVolumeTexture9(pReal);
	if (pDirect3DVolumeTexture9)
	{
		return ConstructWrapper(pDirect3DVolumeTexture9, ppWrapper);
	}
	CComQIPtr<IDirect3DCubeTexture9> pDirect3DCubeTexture9(pReal);
	if (pDirect3DCubeTexture9)
	{
		return ConstructWrapper(pDirect3DCubeTexture9, ppWrapper);
	}
	return hResult;
}

HRESULT ConstructWrapper(IDirect3DVertexDeclaration9 *pReal, IWrapper **ppWrapper)
{
	HRESULT hResult = S_OK;
	NSCALL(CDirect3DVertexShader8::CreateInstance(ppWrapper));
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

char * GetUUID(IDirect3DVertexDeclaration9 *pReal)
{
	return "IID_IDirect3DVertexDeclaration9";
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
	GET_UUID(Direct3DSwapChain9);
	GET_UUID(Direct3DTexture9);
	GET_UUID(Direct3DCubeTexture9);
	GET_UUID(Direct3DVolumeTexture9);
	GET_UUID(Direct3DSurface9);
	GET_UUID(Direct3DVolume9);
	GET_UUID(Direct3DVertexDeclaration9)
	GET_UUID(Direct3DIndexBuffer9);
	GET_UUID(Direct3DVertexBuffer9);;
	return "Unrecognized";
}

void InitD3DInfo()
{
	//--- determine dll address range for non-device interface router ---
	MODULEINFO	mInfo;
	HANDLE hProcess = GetCurrentProcess();
	HMODULE D3D9Handle = GetModuleHandleA("D3D9VistaNoSP1.dll");	//--- if we fix Vista SP1 bug ---
	if(D3D9Handle == NULL)
	{
		D3D9Handle = GetModuleHandleA("d3d9d.dll");
		if(D3D9Handle == NULL)
			D3D9Handle = GetModuleHandleA("d3d9.dll");
	}
	if(GetModuleInformation(hProcess, D3D9Handle, &mInfo, sizeof(MODULEINFO)))
	{
		D3D9StartAddress = (LONG_PTR&)mInfo.lpBaseOfDll;
		D3D9EndAddress   = (LONG_PTR&)mInfo.lpBaseOfDll + mInfo.SizeOfImage;
	}
}
