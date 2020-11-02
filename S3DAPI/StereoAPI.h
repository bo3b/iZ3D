#pragma once

#include <atlbase.h>
#include <atlcom.h>
#include <stack>
#include "iStereoAPI.h"

class StereoAPI :
	public IStereoAPI,
	public CComObjectRoot,
	public CComCoClass<StereoAPI> 
{
public:
	BEGIN_COM_MAP(StereoAPI)
		COM_INTERFACE_ENTRY(IStereoAPI)
	END_COM_MAP()

	STDMETHOD(GetDriverVersion)(DWORD* pMajor, DWORD* pMinor, DWORD* pBuild, DWORD* pQFE);

	STDMETHOD_(float,GetSeparation)();
	STDMETHOD(SetSeparation)(float Value);
	STDMETHOD_(float,GetConvergence)();
	STDMETHOD(SetConvergence)(float Value);
	STDMETHOD_(int,GetPreset)();
	STDMETHOD(SetPreset)(int Value);
	STDMETHOD_(bool,IsLaserSightOn)();
	STDMETHOD(SetLaserSight)(bool Value);
	STDMETHOD_(bool,IsAutoFocusOn)();
	STDMETHOD(SetAutoFocus)(bool Value);
	STDMETHOD_(bool,IsStereoOn)();
	STDMETHOD(SetStereoActive)(bool Value);
	STDMETHOD_(bool,IsShowOSDOn)();
	STDMETHOD(SetShowOSDOn)(bool Value);
	STDMETHOD_(bool,IsShowFPSOn)();
	STDMETHOD(SetShowFPSOn)(bool Value);

	STDMETHOD_(float,GetMinSeparation)();
	STDMETHOD_(float,GetMaxSeparation)();
	STDMETHOD_(float,GetMinConvergence)();
	STDMETHOD_(float,GetMaxConvergence)();
	STDMETHOD_(int,GetPresetsCount)();

	STDMETHOD_(int,GetRenderTargetCreationMode)();
	STDMETHOD(SetRenderTargetCreationMode)(int Value);

	STDMETHOD(BeginMonoBlock)();
	STDMETHOD(EndMonoBlock)();
	STDMETHOD(BeginStereoBlock)();
	STDMETHOD(EndStereoBlock)();
	STDMETHOD_(bool,IsLastDrawStereo)();

	STDMETHOD(SetBltSrcEye)(int Value);
	STDMETHOD_(int,GetBltSrcEye)();
	STDMETHOD(SetBltDstEye)(int Value);
	STDMETHOD_(int,GetBltDstEye)();

protected:
	std::stack<bool>	m_BlockStack;
};
