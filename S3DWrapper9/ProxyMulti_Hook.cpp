#include "stdafx.h"
#include "ProxyMulti_Hook.h"
#include "BaseStereoRenderer_Hook.h"
#include "ProxySwapChain9_Hook.h"
/*
ULONG (__stdcall IUnknown::*Original_Surface_And_SwapChain_Release)();

PROXYSTDMETHOD_(ULONG,Surface_And_SwapChain_Release)(IUnknown* This)
{
	bool isSurface = true;
	{
		IDirect3DSurface9* surface = NULL;
		This->QueryInterface(&surface);
		if(surface == NULL)
			isSurface = false;
		
		(This->*Original_Surface_And_SwapChain_Release)();
	}
	if(isSurface)
		return Proxy_Surface_Release((IDirect3DSurface9*)This);
	
	return IDirect3DSwapChain9MethodNames::Proxy_Release((IDirect3DSwapChain9*)This);
}
*/