//--------------------------------------------------------------------------------------
// File:	Presenter.h
// Wrapper:	DX10
//
//
//--------------------------------------------------------------------------------------

#pragma once

#include <avrt.h>
#include "..\S3DWrapper9\PresenterHelpers.h"
#include <d3d10umddi.h>

#define USE_RENDERER_SLEEP			1
#define RENDERER_SLEEP_TIME			0

//#define	DEBUG_SHOW_WAIT_LINES		1	
#define SHARED_TEXTURE_PAIR_COUNT	3
#define SHARED_TEXTURE_COUNT		(SHARED_TEXTURE_PAIR_COUNT * 2)

// KM Shutter buffer count
#define KM_SHUTTER_SERVICE_BUFFER_COUNT		4

class CBaseStereoRenderer;
class D3DSwapChain;				// DX9: CBaseSwapChain;
class D3DDeviceWrapper;
class CPresenterSCData;

class CBaseStereoRenderer;
class CBasePresenterSCData;

///////////////////////////////////////////////////////////////////////////////////////////////////

#define USE_PRESENTER_SLEEP			1
#define	PRESENTER_SLEEP_TIME		1

#define PRESENTER_CPU_PRIORITY		THREAD_PRIORITY_TIME_CRITICAL		/* THREAD_PRIORITY_ABOVE_NORMAL					*/	
#define AVRT_THREAD_PRIORITY		AVRT_PRIORITY_HIGH
//#define USE_PRESENTER_ACTIVE_WAIT	1

enum PresenterAction
{
	ptNone, 
	ptInitialize, 
	ptInitializeSCData,
	ptUpdateWindow,
	ptRender,
	ptClearSCData, 
	ptClear,
	ptSetFullscreenState,
	ptResizeTarget,
	ptResizeBuffers, 
	ptReleaseSC, 
	ptReleaseDevice,
};

struct  CResourceHandle
{
	D3D10DDI_HRESOURCE		hResource;
	UINT					SubResourceIndex;
};

struct  CDDIResourceHandle
{
	DXGI_DDI_HRESOURCE		hResource;
	UINT					SubResourceIndex;
};

typedef	std::vector<CDDIResourceHandle>				CDDIResourceHandleVector;

union DXGIParameters
{
	struct
	{
		BOOL Fullscreen;
		IDXGIOutput * pTarget;
	};
	const DXGI_MODE_DESC * pNewTargetParameters;
};

inline void PumpMessage() 
{
	MSG msg;
	if (PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE)) 
	{
		if(msg.message == WM_QUIT)
			return;
		if (GetMessage(&msg, NULL, 0, 0) != -1)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

class	CBasePresenterX
{
public:
										CBasePresenterX			();
	virtual								~CBasePresenterX		();
	virtual	void						Init					( D3DDeviceWrapper* pDeviceWrapper_ );
	void								ReInit					( DXGI_RATIONAL newRefreshRate  );
	virtual	void						InitResources			() {}
	void								KillBackgroundThread	() { m_bEndThread		= true; }
	void								ResumeThread			() { ::ResumeThread( m_hBackgroundThread ); }
	void								SuspendThread			() { ::SuspendThread( m_hBackgroundThread );}

	PresenterAction						GetAction				() { return m_Action; }
	void								SetAction				( PresenterAction action )
	{
		PresenterAction oldAction = m_Action;
		m_Action = action;
		if ( oldAction == ptNone )
			ResumeThread();
	}

	HRESULT					SetActionAndWait				( PresenterAction action )
	{
		SetAction(action);

		if (action == ptNone)
			action = m_Action;

		if (action != ptNone && action != ptInitialize)
		{
			while( action == m_Action )
			{
				PumpMessage();
				Sleep( PRESENTER_SLEEP_TIME );
			}
		}

		return m_hrReset;
	}

	void								SetRefreshRate			(  DXGI_RATIONAL refreshRate  );

	D3DDeviceWrapper*					GetDeviceWrapper		()	const	{ return	m_pDeviceWrapper; }
	D3DSwapChain*						GetSwapChain			()		const	{	return	m_pSwapChainWrapper;			}
	UINT								GetFrameCount			()		const	{	return	m_FrameNumber;			}
	virtual D3D10DDI_HRESOURCE			GetSharedResource		( bool bLeft ) const = 0;

	void								SetBltData				( CONST DXGI_DDI_ARG_BLT*	pBltData )			{ _ASSERT( pBltData ); m_bUsePresent = false; m_BltData	= *pBltData; }
	void								SetPresentData			( CONST DXGI_DDI_ARG_PRESENT* pPresentData )	{ _ASSERT( pPresentData ); m_bUsePresent = true; m_PresentData	= *pPresentData; }
	void								AddBackBuffer			( DXGI_DDI_HRESOURCE hResource, UINT	SubResourceCount );
	void								AddResourceIdentities	( DXGI_DDI_ARG_ROTATE_RESOURCE_IDENTITIES*  pRotateData  );
	void								SetResizeTargetParams	( const DXGI_MODE_DESC * pNewTargetParameters ) { m_DXGIParametrs.pNewTargetParameters = pNewTargetParameters; }
	void								SetFullscreenStateParams	( BOOL Fullscreen, IDXGIOutput * pTarget ) { m_DXGIParametrs.Fullscreen = Fullscreen; m_DXGIParametrs.pTarget = pTarget; }
	bool								IsPresenterThread		()	{ return m_ThreadID == GetCurrentThreadId(); }

	CSuperDuperSynchronizer				m_SuperSynchronizer;
	CPresentHistory*					m_pPresentHistory;
	
protected:

	static unsigned WINAPI				BackgroundThreadProc		( LPVOID lpParam );
	virtual DWORD WINAPI				BackgroundThreadProc		() = 0;
	HANDLE								CreateBackgroundThread		();	
	HWND								CreatePresenterWindow		( HWND hSrcWnd );

	D3DDeviceWrapper*					m_pDeviceWrapper;
	D3DSwapChain*						m_pSwapChainWrapper;

	unsigned							m_ThreadID;
	HANDLE								m_hBackgroundThread;
	bool			volatile			m_bEndThread;
	PresenterAction volatile			m_Action;
	HRESULT								m_hrReset;
	DDrawSyncronizer					m_ddSyncronizer;
	ULONG								m_FrameNumber;

	bool								m_bUseSimplePresenter;
	bool								m_bUsePresent;
	DXGI_DDI_ARG_BLT					m_BltData;
	DXGI_DDI_ARG_PRESENT				m_PresentData;
	std::vector<DXGI_DDI_HRESOURCE>		m_ResourceIdentities;
	CDDIResourceHandleVector			m_Backbuffer;
	UINT								m_BackbufferCount;

	DXGI_RATIONAL						m_RefreshRate;
	DXGIParameters						m_DXGIParametrs;
	
	// avrt.dll
	DWORD								m_TaskIndex;
	HMODULE								m_hAVRTLib;
	HANDLE								m_hAvrt;	

	typedef HANDLE		(__stdcall *PTR_AvSetMmThreadCharacteristicsW)	(LPCWSTR TaskName, LPDWORD TaskIndex);
	typedef BOOL		(__stdcall *PTR_AvSetMmThreadPriority)			(HANDLE AvrtHandle, AVRT_PRIORITY Priority);
	typedef BOOL (__stdcall *PTR_AvRevertMmThreadCharacteristics)(HANDLE AvrtHandle);

	PTR_AvSetMmThreadCharacteristicsW	pfAvSetMmThreadCharacteristicsW;
	PTR_AvSetMmThreadPriority			pfAvSetMmThreadPriority;
	PTR_AvRevertMmThreadCharacteristics	pfAvRevertMmThreadCharacteristics;

public:
#pragma region SC Data
	CAverage     						m_AverageFPS;

	// PresentHistory functions
	void								PH_AfterSleep				()
	{
		if (m_pPresentHistory)
			m_pPresentHistory->AfterSleep();
	}
	void								PH_BeforePresent			()
	{
		if (m_pPresentHistory)
			m_pPresentHistory->BeforePresent();
	}
	void								PH_SetVSyncTime				( LARGE_INTEGER* pTime )
	{
		if (m_pPresentHistory)
			m_pPresentHistory->SetVSyncTime(pTime);
	}
	void								PH_AfterPresent				( int presentCount = 1 )
	{
		if (m_pPresentHistory)
			m_pPresentHistory->AfterPresent(presentCount);
	}

	//--- data for x0ras-like research and analysis ---
	void								PH_SaveGameFps				( float gameFps )
	{ 
		if (m_pPresentHistory)
			m_pPresentHistory->SaveGameFps(gameFps);
	}
	void								PH_SavePresenterFps			( float presenterFps )
	{ 
		if (m_pPresentHistory)
			m_pPresentHistory->SavePresenterFps(presenterFps);
	}
	void								PH_SaveSleepTime			( int sleepTime )
	{ 
		if (m_pPresentHistory)
			m_pPresentHistory->SaveSleepTime(sleepTime);
	}
	void								PH_SaveSkipPerSecond		( int skipPerSecond )
	{ 
		if (m_pPresentHistory)
			m_pPresentHistory->SaveSkipPerSecond(skipPerSecond);
	}
	void								PH_SaveFlushCount			( int flushCount )
	{ 
		if (m_pPresentHistory)
			m_pPresentHistory->SaveFlushCount(flushCount);
	}
	void								PH_SaveGetDataCount			( int getDataCount )
	{ 
		if (m_pPresentHistory)
			m_pPresentHistory->SaveGetDataCount(getDataCount);
	}

	bool	GetPresentCall(  )	{ return m_bUsePresent; }
	CONST DXGI_DDI_ARG_BLT*		GetBltData	(  )		{	return &m_BltData;	}
	CONST DXGI_DDI_ARG_PRESENT*	GetPresentData	(  )	{	return &m_PresentData;	}

	float	GetFPS()
	{
		return m_AverageFPS.GetFPS();
	}

	int		GetLastSleepTime()
	{
		if (gInfo.PresenterSleepTime == -1)
			return m_AverageFPS.GetLastSleepTime();
		else if ((gInfo.PresenterSleepTime == -2) || (gInfo.PresenterSleepTime == -3))
			return m_SuperSynchronizer.GetLastSleepTime();
		return gInfo.PresenterSleepTime;
	}

	UINT	GetLagsPerSecond()
	{
		return (UINT)m_AverageFPS.GetLagCount();
	}

	int GetSleepTime()
	{
		if (gInfo.PresenterSleepTime == -1)
			return m_AverageFPS.GetSleepTime();
		else if ((gInfo.PresenterSleepTime == -2) || (gInfo.PresenterSleepTime == -3))
			return m_SuperSynchronizer.GetLastSleepTime();
		return gInfo.PresenterSleepTime;
	}
#pragma endregion
};

///////////////////////////////////////////////////////////////////////////////////////////////////

class	CPresenterX : public CBasePresenterX
{
public:
										CPresenterX					();
	virtual								~CPresenterX				();
	INT									GetPresenterMode			()		const { return	(INT)gData.ShutterMode; }
	HRESULT					WaitD3DCreation		()
	{	
		while( m_hrReset == 0xFFFFFFFF )
			Sleep( PRESENTER_SLEEP_TIME );
		return m_hrReset;
	}
	
	virtual D3D10DDI_HRESOURCE			GetSharedResource			( bool bLeft )	const {	return bLeft ? m_hSrcResourceLeft : m_hSrcResourceRight; }	
	virtual void						Init						( D3DDeviceWrapper* pWrapper );
	virtual	void						InitResources				();
	static void Create ( DXGI_RATIONAL refreshRate );
	static void							Delete						();
	static CPresenterX*					Get							() { return m_pPresenterX; }
	HRESULT								GetLastPresentResult		() const { return m_hrPresent; }

protected:
	void								TestCooperativeLevel		();	
	virtual DWORD WINAPI				BackgroundThreadProc		();
	void								CleanupBackground			();	
	void								RenderBackground			();

	bool								SkipRender();
	void								CalculateFPS();

	//--- data members ---
	static size_t						m_NumCopies;	
	UINT								m_Adapter;
	bool								m_bReadScanLineSupported;

	//SwapChainData section
	CriticalSection						m_CS;
	long volatile						m_PresentedPairIndex;
	long volatile						m_RenderingPairIndex;
	LARGE_INTEGER						m_Frequency;
	LARGE_INTEGER						m_FrameTime;
	LARGE_INTEGER						m_LastFrameTime;
	ULONG								m_VSyncCount;
	LARGE_INTEGER						m_RenderingTime;
	HRESULT								m_hrPresent;
	D3D10_DDI_BOX						m_SourceRect[2];			// Left and Right
	D3D10_DDI_BOX						m_DestRect;
	D3D10DDI_HRESOURCE					m_hSrcResource;				// source surface with over/under stereo fields
	D3D10DDI_HRESOURCE					m_hSrcResourceLeft;
	D3D10DDI_HRESOURCE					m_hSrcResourceRight;

private:
	static CPresenterX*					m_pPresenterX;
};
