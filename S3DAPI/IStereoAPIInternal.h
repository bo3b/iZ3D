/* 
* iZ3D Stereoscopic Driver
* Copyright (C) iZ3D Inc. 2002-2010. All rights are reserved
*/

#ifndef _ISTEREOAPI_INTERNAL_H_
#define _ISTEREOAPI_INTERNAL_H_

#define COM_NO_WINDOWS_H
#include <objbase.h>

typedef void (* MakeScreenshotCallback)();

MIDL_INTERFACE("1231E648-E1DE-11DF-8F74-6047E0D72085")
IStereoAPIInternal: public IUnknown
{
	STDMETHOD(MakeScreenshot)(TCHAR *path,MakeScreenshotCallback callback) PURE;
};

MIDL_INTERFACE("1231E648-E1DE-11DF-8F74-6047E0D72086")
IStereoAPIInternal2: public IStereoAPIInternal
{
	STDMETHOD(SetEID)(UINT64 eid) PURE;
};

typedef HRESULT (__stdcall *PFNINITSTEREOAPIINTERNAL)(IStereoAPIInternal** ppAPI);

inline HRESULT CreateStereoAPIInternal(HMODULE hModule, IStereoAPIInternal** ppAPI)
{
	if(!hModule)
		return E_FAIL;

	PFNINITSTEREOAPIINTERNAL p = (PFNINITSTEREOAPIINTERNAL)GetProcAddress(hModule, "InitStereoAPIInternal");
	if(!p)
		return E_FAIL;

	return p(ppAPI);
}

#endif
