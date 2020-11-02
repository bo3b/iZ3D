
#pragma once
#define NTSTATUS LONG

#include <d3dumddi.h> 
//#include "..\lib\d3d10\Include\d3dkmthk.h"
#include "BasePresenter.h"

class	UMDDeviceWrapper;
struct  CResourceHandle
{
	HANDLE		hResource;
	DWORD		SubResourceIndex;
};

typedef	std::vector<CResourceHandle>				CResourceHandleVector;

typedef	std::vector<D3DDDIARG_CREATEQUERY>			CQueryVector;

class	UMDShutterPresenterSCData;

class	UMDShutterPresenter : public CBasePresenter
{
public:
	UMDShutterPresenter( UMDDeviceWrapper * pUMDWrapper );
	~UMDShutterPresenter();

	virtual CBasePresenterSCData*	CreateSwapChainData( CBaseSwapChain* pSwapChain );

protected:
	virtual DWORD WINAPI			BackgroundThreadProc	();
private:
	D3DDDI_DEVICEFUNCS *			m_pDeviceFuncs;
	HANDLE							m_hDevice;
	CriticalSection *				m_pCS;
	HRESULT							m_hResult;

	UMDDeviceWrapper *				m_pUMDWrapper;
	bool							m_UseKMPresent;
	//PFND3DKMT_WAITFORVERTICALBLANKEVENT m_pfnWaitForVBlank;
	//PFND3DKMT_PRESENT				m_pfnPresent;

	friend UMDShutterPresenterSCData;
};

class	UMDShutterPresenterSCData : public CBasePresenterSCData
{
public:
	UMDShutterPresenterSCData( CBasePresenter* pPresenter, 
		CBaseSwapChain* pSwapChain );

	virtual void	Cleanup();
	virtual void	RenderBackground		();

	void SetData( UMDShutterPresenter* pPresenter, RECT * pSourceRect, /* Left and Right */ RECT * pDestRect, HANDLE hSrcResource, /* source surface with over/under stereo fields */ const CResourceHandleVector & hDstResource );

	CONST D3DDDIARG_PRESENT*	GetPresentData	(  )				{	return &m_PresentData;	}
	void		SetPresentData			( CONST D3DDDIARG_PRESENT* pPresent ){	m_PresentData = *pPresent;				}
	bool		GetPresentCall			(  )						{	return m_bUsePresent;		}
	void		SetPresentCall			( bool bPresent )			{	m_bUsePresent = bPresent;		}

	void		SetStereoResource		( HANDLE hSrcResource )
	{
		m_hSrcResource = hSrcResource;
	}

	void		SetStereoResourceParameters( RECT *	pSourceRect )			// Left and Right
	{
		m_SourceRect[0]	= pSourceRect[0];		// Left...
		m_SourceRect[1]	= pSourceRect[1];		// ... and Right
	}

	CONST RECT*	GetStereoResourceParameters()
	{
		return m_SourceRect;
	}

	DWORD		GetFrameNumber			()
	{
		return m_FrameNumber;
	}

	DWORD		GetBackBufferCount		()
	{
		return m_BackbufferCount;
	}

	void		CalculateFPS();

protected:
	UMDShutterPresenter*			m_pUMDPresenter;

	D3DDDIARG_PRESENT				m_PresentData;
	bool							m_bUsePresent;
	DWORD							m_BackbufferCount;
	LARGE_INTEGER					m_FrameTime;
	LARGE_INTEGER					m_LastFrameTime;
	LARGE_INTEGER					m_RenderingTime;
	LARGE_INTEGER					m_Frequency;

	DWORD							m_FrameNumber;
	RECT							m_SourceRect[2];		// Left and Right
	RECT							m_DestRect;
	HANDLE							m_hSrcResource;			// source surface with over/under stereo fields
	CResourceHandleVector 			m_hDstResource;			// the backbuffer

	// query pool
	CQueryVector					m_Query;
	DWORD							m_LastUsedQueryIndex;
};

#define UMDCALL(x)					SUCCEEDED(m_hResult=(m_pDeviceFuncs->pfn##x))
#define UMDCALL_(x)					{ if( FAILED(m_hResult=(x)) ) return m_hResult; } 


