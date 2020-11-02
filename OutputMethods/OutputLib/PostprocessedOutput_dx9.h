/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "OutputMethod_dx9.h"
#include "S3DWrapper9\VertexType.h"

#define LEARNING_CONSTANT 2
#define MAX_STATES 128

namespace DX9Output{

class PostprocessedOutput : 
	public OutputMethod
{
public:
	PostprocessedOutput(DWORD mode, DWORD spanMode);
	virtual ~PostprocessedOutput(void);
	virtual HRESULT Initialize(IDirect3DDevice9* dev, bool MultiPass = false);
	virtual HRESULT InitializeSCData(CBaseSwapChain* pSwapChain);
	virtual void Clear();
	virtual HRESULT Output(CBaseSwapChain* pSwapChain);

	virtual HRESULT InitializeResources() = 0;
	virtual HRESULT InitializeSCResources(CBaseSwapChain* pSwapChain) { return S_OK; }
	virtual HRESULT SetStates(CBaseSwapChain* pSwapChain) { return S_OK; }
	virtual void ClearResources() {}

	virtual void ProcessSubMode() {};
protected:

	HRESULT SetStatesBeforeDraw( CBaseSwapChain* pSwapChain );
	HRESULT DrawFirstPass( CBaseSwapChain* pSwapChain, IDirect3DSurface9* primary, IDirect3DSurface9* secondary );
	HRESULT DrawSecondPass( CBaseSwapChain* pSwapChain, IDirect3DSurface9* secondary );

	CComPtr<IDirect3DStateBlock9>   m_pState;
	CComPtr<IDirect3DStateBlock9>   m_pStateWithGamma;

	bool                            m_bMultiPassRendering;
	CComPtr<IDirect3DPixelShader9>  m_pShaderMono;
	CComPtr<IDirect3DPixelShader9>  m_pShaderMonoAndGamma;
	CComPtr<IDirect3DPixelShader9>  m_pShader1stPass;
	CComPtr<IDirect3DPixelShader9>  m_pShader1stPassAndGamma;
	CComPtr<IDirect3DPixelShader9>  m_pShader2ndPass;
	CComPtr<IDirect3DPixelShader9>  m_pShader2ndPassAndGamma;
};

}