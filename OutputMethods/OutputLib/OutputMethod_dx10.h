#pragma once

#include "OutputData.h"
#include "../../S3DWrapper10/D3DSwapChain.h"

class D3DDeviceWrapper;
class D3DSwapChain;
class SwapChainResources;

namespace DX10Output{

class OutputMethod
{
public:
	OutputMethod(DWORD mode, DWORD spanMode);
	virtual ~OutputMethod(void);

	virtual void ModifyResizeBuffersParams( UINT& BufferCount, UINT& Width, UINT& Height, DXGI_FORMAT& NewFormat, UINT& SwapChainFlags );
	virtual void ModifyResizeTargetParams( DXGI_MODE_DESC *pNewTargetParameters );
	virtual void Initialize(D3DDeviceWrapper* dev);
	virtual void Clear( D3DDeviceWrapper* dev );
	virtual void Output( D3DSwapChain* pSwapChain ) { }
	virtual void Output( bool bLeft_, D3DSwapChain* pSwapChain_, RECT* pDestRect_ ) {}
	virtual void AfterPresent( D3DDeviceWrapper* pD3DDevice_, bool bLeft_ ) { }
	virtual void StereoModeChanged(bool bNewMode) { }

	virtual bool FindAdapter( IDXGIFactory* pDXGIFactory_ );
	virtual UINT GetOutputChainsNumber() { return 1; }
	FLOAT* InitSecondBBColor() { return m_SecondBBColor; }
	virtual bool RenderDirectlyToBB() { return false; }
	virtual DWORD GetSecondWindowCaps() { return 0; }
	virtual int   GetPreferredRefreshRate() const { return 0; }

	int GetTrialDaysLeft();

	virtual bool NeedStateSaving() { return true; }
	virtual bool OutputFromTexture() { return true; }
	virtual void ProcessSubMode() {};

	bool								IsLogoPresent		() const { return m_hLogoResourceView.pDrvPrivate ? true : false ; } // anti warrning
	const D3D10DDI_HSHADERRESOURCEVIEW&	GetLogoResourceView	() const { return m_hLogoResourceView; }
	HRESULT								CreateLogoResources	( HMODULE hModule_, UINT nResID_, D3DDeviceWrapper* pD3DWrapper_ );
	void								DeleteLogoResources	( D3DDeviceWrapper* pD3DWrapper_ );

	UINT								GetLogoWidth		() const { return m_nLogoWidth; }
	UINT								GetLogoHeight		() const { return m_nLogoHeight; }

protected:
	DWORD							m_OutputMode;
	DWORD							m_SpanMode;
	DWORD							m_SubMode;
	FLOAT							m_SecondBBColor[4];
	bool							m_bInitialized;

	UINT							m_nLogoWidth;
	UINT							m_nLogoHeight;
	D3D10DDI_HRESOURCE				m_hLogoResource;
	D3D10DDI_HSHADERRESOURCEVIEW	m_hLogoResourceView;

	//--- gamma correction ---
	bool							m_bProcessGammaRamp; // should be changed only in constructor
	bool							m_bTrialMode;
};

}