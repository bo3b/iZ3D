/* 
* iZ3D Stereoscopic Driver
* Copyright (C) iZ3D Inc. 2002-2010. All rights are reserved
*/

#ifndef _ISTEREOAPI_H_
#define _ISTEREOAPI_H_

#define COM_NO_WINDOWS_H
#include <objbase.h>

MIDL_INTERFACE("51F594BA-F32E-4fb4-98A4-5F92277DEC20")
IStereoAPI: public IUnknown
{
	/*** IStereoAPI methods ***/
	STDMETHOD(GetDriverVersion)(DWORD* pMajor, DWORD* pMinor, DWORD* pBuild, DWORD* pQFE) PURE;

	STDMETHOD_(float,GetSeparation)() PURE;
	STDMETHOD(SetSeparation)(float Value) PURE;
	STDMETHOD_(float,GetConvergence)() PURE;
	STDMETHOD(SetConvergence)(float Value) PURE;
	STDMETHOD_(int,GetPreset)() PURE;
	STDMETHOD(SetPreset)(int Value) PURE;
	STDMETHOD_(bool,IsLaserSightOn)() PURE;
	STDMETHOD(SetLaserSight)(bool Value) PURE;
	STDMETHOD_(bool,IsAutoFocusOn)() PURE;
	STDMETHOD(SetAutoFocus)(bool Value) PURE;
	STDMETHOD_(bool,IsStereoOn)() PURE;
	STDMETHOD(SetStereoActive)(bool Value) PURE;
	STDMETHOD_(bool,IsShowOSDOn)() PURE;
	STDMETHOD(SetShowOSDOn)(bool Value) PURE;
	STDMETHOD_(bool,IsShowFPSOn)() PURE;
	STDMETHOD(SetShowFPSOn)(bool Value) PURE;

	STDMETHOD_(float,GetMinSeparation)() PURE;
	STDMETHOD_(float,GetMaxSeparation)() PURE;
	STDMETHOD_(float,GetMinConvergence)() PURE;
	STDMETHOD_(float,GetMaxConvergence)() PURE;
	STDMETHOD_(int,GetPresetsCount)() PURE;

	STDMETHOD_(int,GetRenderTargetCreationMode)() PURE;
	STDMETHOD(SetRenderTargetCreationMode)(int Value) PURE;

	STDMETHOD(BeginMonoBlock)() PURE;
	STDMETHOD(EndMonoBlock)() PURE;
	STDMETHOD(BeginStereoBlock)() PURE;
	STDMETHOD(EndStereoBlock)() PURE;
	STDMETHOD_(bool,IsLastDrawStereo)() PURE;

	STDMETHOD(SetBltSrcEye)(int Value) PURE;
	STDMETHOD_(int,GetBltSrcEye)() PURE;
	STDMETHOD(SetBltDstEye)(int Value) PURE;
	STDMETHOD_(int,GetBltDstEye)() PURE;
};

typedef HRESULT (__stdcall *PFNINITSTEREOAPI)(IStereoAPI** ppAPI);

inline HMODULE GetStereoLibraryHandle()
{
	return GetModuleHandle(_T("S3DAPI.dll"));
}

inline HRESULT CreateStereoAPI(HMODULE hModule, IStereoAPI** ppAPI)
{
	if(!hModule)
		return E_FAIL;

	PFNINITSTEREOAPI p = (PFNINITSTEREOAPI)GetProcAddress(hModule, "InitStereoAPI");
	if(!p)
		return E_FAIL;

	return p(ppAPI);
}

#endif
