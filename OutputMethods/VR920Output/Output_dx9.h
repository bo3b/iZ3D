/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "OutputMethod_dx9.h"

namespace DX9Output
{

class VR920Output :
	public OutputMethod
{
private:
	bool m_bLastFrameStereoOn;
	bool m_bAdapterFound;
	HANDLE m_StereoHandle;
	CComPtr<IDirect3DQuery9>	m_pLeftEyeQuery;
	CComPtr<IDirect3DQuery9>	m_pRightEyeQuery;
public:
	VR920Output(DWORD mode, DWORD spanMode);
	virtual ~VR920Output(void);

	virtual void	StereoModeChanged(bool bNewMode);
	virtual HRESULT	Output(bool bLeft, CBaseSwapChain* pSwapChain, RECT* pDestRect);
	virtual void	AfterPresent(bool bLeft);
	virtual bool	FindAdapter( LPDIRECT3D9 pDirect3D9, UINT DeviceNum, D3DDEVTYPE &DeviceType, UINT Adapters[2] );
	virtual HRESULT	Initialize(IDirect3DDevice9* dev, bool MultiPass);
	virtual void	Clear();
};

}
