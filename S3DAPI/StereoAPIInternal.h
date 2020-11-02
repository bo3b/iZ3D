#pragma once

#include <atlbase.h>
#include <atlcom.h>
#include <stack>
#include "iStereoAPIInternal.h"

class StereoAPIInternal :
	public IStereoAPIInternal2,
	public CComObjectRoot,
	public CComCoClass<StereoAPIInternal> 
{
public:
	BEGIN_COM_MAP(StereoAPIInternal)
		COM_INTERFACE_ENTRY(IStereoAPIInternal)
		COM_INTERFACE_ENTRY(IStereoAPIInternal2)
	END_COM_MAP()

	STDMETHOD(MakeScreenshot)(TCHAR *path,MakeScreenshotCallback callback);
	STDMETHOD(SetEID)(UINT64 eid);
};
