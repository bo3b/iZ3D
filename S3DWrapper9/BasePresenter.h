//--------------------------------------------------------------------------------------
// File: BasePresenter.h
//
//
//
//--------------------------------------------------------------------------------------

#pragma once

#include <avrt.h>
#include "PresenterHelpers.h"

#define USE_PRESENTER_SLEEP			1
#define	PRESENTER_SLEEP_TIME		1

#define PRESENTER_CPU_PRIORITY		THREAD_PRIORITY_TIME_CRITICAL		/* THREAD_PRIORITY_ABOVE_NORMAL					*/	
#define AVRT_THREAD_PRIORITY		AVRT_PRIORITY_HIGH
#define PRESENTER_GPU_PRIORITY		7		/*3*/						/* The thread priority, ranging from -7 to 7	*/
//#define USE_PRESENTER_ACTIVE_WAIT	1

enum PresenterAction
{
	ptNone, 
	ptInitialize, 
	ptInitializeSCData, 
	ptRender,
	ptClearSCData, 
	ptClear,
	ptReset, 
	ptTestCooperativeLevel,  
	ptReleaseSC, 
	ptReleaseDevice, 
};

class CBaseStereoRenderer;
class CBaseSwapChain;
class CBasePresenterSCData;

class	CBasePresenter
{
public:
	virtual ~CBasePresenter();

	virtual CBasePresenterSCData*	CreateSwapChainData( CBaseSwapChain* pSwapChain ) = 0;

	void					SetCurrentRefreshRate();

	void					KillBackgroundThread		()
	{	
		m_bEndThread		= true;
	}

	void					ResumeThread	()
	{	
		::ResumeThread(m_hBackgroundThread);
	}

	void					SuspendThread	()
	{
		::SuspendThread( m_hBackgroundThread );
	}

	void					SetAction	(PresenterAction action, CBaseSwapChain* pSwapChain = NULL)
	{
		PresenterAction oldAction = m_Action;
		m_Action = action;
		m_pBaseSwapChain = pSwapChain;
		if (oldAction == ptNone)
			ResumeThread();
	}

	HRESULT					WaitForActionDone			()
	{
		while( ptNone != m_Action )
			Sleep( PRESENTER_SLEEP_TIME );
		return m_hrReset;
	}

	CBaseStereoRenderer*				m_pStereoRenderer;

	CSuperDuperSynchronizer				m_SuperSynchronizer;
	ULONG								m_RefreshRate;
	CPresentHistory*					m_pPresentHistory;

protected:

	static unsigned WINAPI	BackgroundThreadProc		( LPVOID lpParam );
	virtual DWORD WINAPI	BackgroundThreadProc		() = 0;

	HANDLE					CreateBackgroundThread		();
	HRESULT					D3DWindowedWaitForVsync		();

	CBasePresenter( CBaseStereoRenderer* pStereoRenderer );

	unsigned							m_ThreadID;
	HANDLE								m_hBackgroundThread;
	bool			volatile			m_bEndThread;
	PresenterAction volatile			m_Action;
	HRESULT								m_hrReset;
	DDrawSyncronizer					m_ddSyncronizer;
	CBaseSwapChain*						m_pBaseSwapChain;

	// avrt.dll
	DWORD								m_TaskIndex;
	HMODULE								m_hAVRTLib;
	HANDLE								m_hAvrt;

	typedef HANDLE (__stdcall *PTR_AvSetMmThreadCharacteristicsW)(LPCWSTR TaskName, LPDWORD TaskIndex);
	typedef BOOL (__stdcall *PTR_AvSetMmThreadPriority)(HANDLE AvrtHandle, AVRT_PRIORITY Priority);
	typedef BOOL (__stdcall *PTR_AvRevertMmThreadCharacteristics)(HANDLE AvrtHandle);

	PTR_AvSetMmThreadCharacteristicsW	pfAvSetMmThreadCharacteristicsW;
	PTR_AvSetMmThreadPriority			pfAvSetMmThreadPriority;
	PTR_AvRevertMmThreadCharacteristics	pfAvRevertMmThreadCharacteristics;
};

class	CBasePresenterSCData
{
public:
	CBasePresenterSCData( CBasePresenter* pPresenter, CBaseSwapChain* pSwapChain );
	virtual ~CBasePresenterSCData();

	virtual void	Initialize();
	virtual void	Cleanup();

	float					GetFPS	();
	float					GetJitterDeviation	()
	{	
		return m_AverageFPS.GetJitterDeviation();
	}
	UINT					GetLagsPerSecond		();
	int						GetLastSleepTime		();
	HRESULT					GetLastPresentResult	()
	{	
		return m_hrPresent;
	}

	// PresentHistory functions
	void	PH_AfterSleep( )
	{
		if (m_pBasePresenter->m_pPresentHistory)
			m_pBasePresenter->m_pPresentHistory->AfterSleep();
	}
	void	PH_BeforePresent( )
	{
		if (m_pBasePresenter->m_pPresentHistory)
			m_pBasePresenter->m_pPresentHistory->BeforePresent();
	}
	void	PH_SetVSyncTime( LARGE_INTEGER* pTime )
	{
		if (m_pBasePresenter->m_pPresentHistory)
			m_pBasePresenter->m_pPresentHistory->SetVSyncTime(pTime);
	}
	void	PH_AfterPresent( int presentCount = 1 )
	{
		if (m_pBasePresenter->m_pPresentHistory)
			m_pBasePresenter->m_pPresentHistory->AfterPresent(presentCount);
	}

	//--- data for x0ras-like research and analysis ---
	void PH_SaveGameFps(float gameFps)
	{ 
		if (m_pBasePresenter->m_pPresentHistory)
			m_pBasePresenter->m_pPresentHistory->SaveGameFps(gameFps);
	}
	void PH_SavePresenterFps(float presenterFps)
	{ 
		if (m_pBasePresenter->m_pPresentHistory)
			m_pBasePresenter->m_pPresentHistory->SavePresenterFps(presenterFps);
	}
	void PH_SaveSleepTime(int sleepTime)
	{ 
		if (m_pBasePresenter->m_pPresentHistory)
			m_pBasePresenter->m_pPresentHistory->SaveSleepTime(sleepTime);
	}
	void PH_SaveSkipPerSecond(int skipPerSecond)
	{ 
		if (m_pBasePresenter->m_pPresentHistory)
			m_pBasePresenter->m_pPresentHistory->SaveSkipPerSecond(skipPerSecond);
	}
	void PH_SaveFlushCount(int flushCount)
	{ 
		if (m_pBasePresenter->m_pPresentHistory)
			m_pBasePresenter->m_pPresentHistory->SaveFlushCount(flushCount);
	}
	void PH_SaveGetDataCount(int getDataCount)
	{ 
		if (m_pBasePresenter->m_pPresentHistory)
			m_pBasePresenter->m_pPresentHistory->SaveGetDataCount(getDataCount);
	}
	void ResetPresenter()
	{
		m_pBasePresenter = NULL;
	}

	virtual void			RenderBackground			( ) = 0;

	CAverage     						m_AverageFPS;

protected:
	int									GetSleepTime();
	CBasePresenter*						m_pBasePresenter;
	CBaseSwapChain*						m_pBaseSwapChain;
	HRESULT								m_hrPresent;

	bool		SkipRender		();

	friend CBasePresenter;
};
