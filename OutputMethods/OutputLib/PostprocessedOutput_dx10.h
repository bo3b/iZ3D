#pragma once

#include "OutputMethod_dx10.h"
#include <d3d10umddi.h>

#define LEARNING_CONSTANT 2
#define MAX_STATES 128

namespace DX10Output{

class PostprocessedOutput : 
	public OutputMethod
{
public:
	PostprocessedOutput(DWORD mode, DWORD spanMode);
	virtual ~PostprocessedOutput(void);
	virtual void					Initialize( D3DDeviceWrapper* dev );
	virtual void					Clear( D3DDeviceWrapper* dev );
	virtual void					Output( D3DSwapChain* pSwapChain );

	virtual void					InitializeResources( D3DDeviceWrapper* dev ) = 0;
	virtual void					ClearResources( D3DDeviceWrapper* dev ) {};
	virtual void					SetStates(  D3DDeviceWrapper* pDevice_, D3DSwapChain* pSwapChain_ ) {};
	void							InitStateObjects( D3DDeviceWrapper* pWrapper_ );
	void							DestroyStateObjects ( D3DDeviceWrapper* pWrapper_ );

	virtual bool					NeedStateSaving() { return true; }
	virtual bool					OutputFromTexture() { return true; }
	virtual void					ProcessSubMode() {};
protected:
	D3D10DDI_HSHADER				m_hVSShader;
	D3D10DDI_HSHADER				m_hPSShader;	
private:
	D3D10DDI_HRASTERIZERSTATE		m_hRastState;
	D3D10DDI_HDEPTHSTENCILSTATE		m_hDSState;
	D3D10DDI_HBLENDSTATE			m_hBlendState;
	D3D10DDI_HSAMPLER				m_hSamplerState;
};

extern bool ParseShaderBlob ( CONST DWORD* pData, DWORD dataSize, CONST UINT** ppCode);
extern bool ParseShaderBlob ( CONST BYTE* pData, DWORD dataSize, CONST UINT** ppCode);

}