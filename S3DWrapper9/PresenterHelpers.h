//--------------------------------------------------------------------------------------
// File: PresenterHelpers.h
//
//
//
//--------------------------------------------------------------------------------------

#pragma once

#include <windows.h>
#include <d3d9.h>
#include <dxgitype.h>
#include <ddraw.h>
#include <vector>
#define BOOST_CB_DISABLE_DEBUG 
#include <boost/circular_buffer.hpp>

class DDrawSyncronizer
{
public:
	DDrawSyncronizer		()
	:	m_hrCreate( S_FALSE ), m_hModDDraw(NULL)
	{
	}

	~DDrawSyncronizer		()
	{
		m_pIDD.Release();
		if (m_hModDDraw)
			FreeLibrary(m_hModDDraw);
	}

	void		Reset			()
	{
		m_pIDD.Release();
		m_hrCreate	= S_FALSE;
	}

	HRESULT		GetStatus		()
	{
		return m_hrCreate;
	}

	HRESULT		GetScanLine		( LPDWORD lpScanLine )
	{
		_ASSERT( m_pIDD );
		return m_pIDD->GetScanLine( lpScanLine );
	}

	void GetRasterStatus(D3DRASTER_STATUS* pStatus)
	{
		DWORD scanLine = 0;
		HRESULT hResult = m_pIDD->GetScanLine( &scanLine );
		pStatus->ScanLine = scanLine;
		pStatus->InVBlank = (hResult == DDERR_VERTICALBLANKINPROGRESS);
	}

	void		WaitForVBlank	()
	{
		_ASSERT( m_pIDD );
		while(DDERR_WASSTILLDRAWING == m_pIDD->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN, NULL))   Sleep(0);
	}
	void		WaitForNonVBlank()
	{
		_ASSERT( m_pIDD );
		while(DDERR_WASSTILLDRAWING == m_pIDD->WaitForVerticalBlank(DDWAITVB_BLOCKEND, NULL))   Sleep(0);
	}

	HRESULT		Initialize		()
	{
		if (m_pIDD)
			return S_OK;

		Reset();
		if (m_hModDDraw == NULL)
			m_hModDDraw = LoadLibrary(_T("ddraw.dll"));

		m_hrCreate = E_FAIL;
		if(m_hModDDraw)
		{
			typedef HRESULT (WINAPI *DirectDrawCreate_t)( GUID* lpGUID, IDirectDraw** lplpDD, IUnknown* pUnkOuter );
			DirectDrawCreate_t DirectDrawCreate = (DirectDrawCreate_t)GetProcAddress(m_hModDDraw, "DirectDrawCreate");
			if (DirectDrawCreate)
				m_hrCreate = DirectDrawCreate( NULL, &m_pIDD, NULL );
		}
		_ASSERT(m_pIDD != NULL);
		return m_hrCreate;
	}

protected:
private:
	HMODULE					m_hModDDraw;
	CComPtr<IDirectDraw>	m_pIDD;
	HRESULT					m_hrCreate;
};


class CAverage
{
public:
	CAverage( size_t samplesCount = 0 )
	:	m_nSamplesCount( samplesCount )
	,	m_vFrameTimes( samplesCount )
	,	m_bFullCycle( false )
	,	m_SleepTime( 0 )
	,	m_SleepTimeChange( 0 )
	,	m_StableCyclesCount( 0 )
	,	m_RefreshRate( )
	,	m_SleepTimeIncreaseDelay( m_DefSleepTimeIncreaseDelay )
	,	m_PresenterFlushTime( 1 )
	{}

	static const int m_DefSleepTimeIncreaseDelay = 10;
	static const int m_DefFlushTimeIncreaseDelay = 5;

#ifndef FINAL_RELEASE
	~CAverage( );
#endif

	int		GetMaxSleepTime( )
	{
		int maxSleepTime = 750 * m_RefreshRate.Denominator / m_RefreshRate.Numerator; //  75% from maximum value
		return maxSleepTime;
	}
	void	SetRefreshRate( ULONG refreshRate );
	void	SetRefreshRate( DXGI_RATIONAL refreshRate );
	void	PushFrameTime( float val );
	float	GetFPS();
	float	GetJitterDeviation();
	size_t	GetLagCount( );
	UINT	GetSleepTime();
	FLOAT	GetFlushTime();
	int		GetLastSleepTime()
	{
		return m_SleepTime;
	}

	DXGI_RATIONAL  GetRefreshRate()				{ return m_RefreshRate; }
	size_t GetBufferSize()						{ return m_vFrameTimes.size(); }
	float  GetFrameTime(size_t	nIndex)			{ return m_vFrameTimes[nIndex]; }

protected:
private:
	CAverage( CAverage const & ca );

	bool			m_bFullCycle;
	DXGI_RATIONAL	m_RefreshRate;
	FLOAT			m_PresenterFlushTime;
	int				m_SleepTime;
	int				m_SleepTimeChange;
	int				m_SleepTimeIncreaseDelay;
	int				m_StableCyclesCount; // 1 cycle - 1 second
	size_t			m_nSamplesCount;
	boost::circular_buffer<float>	m_vFrameTimes;
#ifndef FINAL_RELEASE
	std::vector<UINT64>	m_DebugData;
#endif
};

struct DeviceTimeStamp
{
	D3DRASTER_STATUS	rasterStatus;
	LARGE_INTEGER		time;
	void WriteToStream(std::wostream& fs, UINT height, LARGE_INTEGER& freq);
};

struct PresentData
{
	DeviceTimeStamp	afterSleep;
	DeviceTimeStamp	beforePresent;
	DeviceTimeStamp	afterPresent;
	int presentCount;
	LARGE_INTEGER	vsyncTime;
	
	//--- data for x0ras-like research and analysis ---
	float	gameFps;
	float	presenterFps;
	int		sleepTime;
	int		skipPerSecond;
	int		flushCount;
	int		getDataCount;
};

class CPresentHistory
{
public:
	CPresentHistory( int refreshRate, int height );

	~CPresentHistory()
	{
		m_Synchronizer.Reset();
	}

	UINT GetRefreshRate()
	{ 
		return m_RefreshRate; 
	}

	void	AfterSleep( )
	{
		GetTimeStamp(m_CurrentData.afterSleep);
	}

	void	BeforePresent( )
	{
		GetTimeStamp(m_CurrentData.beforePresent);
	}

	void	SetVSyncTime( LARGE_INTEGER* pTime )
	{
		m_CurrentData.vsyncTime = *pTime;
	}

	void	AfterPresent( int presentCount = 1 )
	{
		GetTimeStamp(m_CurrentData.afterPresent);
		m_CurrentData.presentCount = presentCount;
		PushData();
		m_CurrentData.vsyncTime.QuadPart = 0;
	}

	//--- data for x0ras-like research and analysis ---
	void SaveGameFps(float gameFps)				{ m_CurrentData.gameFps			= gameFps;			}
	void SavePresenterFps(float presenterFps)	{ m_CurrentData.presenterFps	= presenterFps;		}
	void SaveSleepTime(int sleepTime)			{ m_CurrentData.sleepTime		= sleepTime;		}
	void SaveSkipPerSecond(int skipPerSecond)	{ m_CurrentData.skipPerSecond	= skipPerSecond;	}
	void SaveFlushCount(int flushCount)			{ m_CurrentData.flushCount		= flushCount;		}
	void SaveGetDataCount(int getDataCount)		{ m_CurrentData.getDataCount	= getDataCount;		}

	size_t GetBufferSize()						{ return m_vFrameTimes.size(); }
	PresentData* GetItem(size_t	nIndex)			{ return &m_vFrameTimes[nIndex]; }

private:

	void	GetTimeStamp( DeviceTimeStamp& val )
	{
		QueryPerformanceCounter(&val.time);
		val.rasterStatus.InVBlank = 0;
		val.rasterStatus.ScanLine = 0;
		//m_Synchronizer.GetRasterStatus(&val.rasterStatus);
	}

	void	PushData( );
	void	SaveToFile();

	DDrawSyncronizer	m_Synchronizer;
	UINT				m_RefreshRate;
	UINT				m_Height;
	size_t				m_BufferSize;
	bool				m_bSaveStatistic;
	LARGE_INTEGER		m_LastFrameTime;
	LARGE_INTEGER		m_LastVSyncTime;
	PresentData			m_CurrentData;
	boost::circular_buffer<PresentData>	m_vFrameTimes;
};


class CSuperDuperSynchronizer
{
public:
	CSuperDuperSynchronizer( DDrawSyncronizer*	pSynchronizer )
	:	m_pSynchronizer( pSynchronizer )
	,	m_RefreshRate(  )
	,	m_BuffersCount( 0 )
	,	m_ActiveBuffers( 0 )
	,	m_SleepTime(0)
	,	m_FloatSleepTime(0.0f)
	{
		m_PrevVSyncTime.QuadPart = 0;
		m_bWaitVSync = false;
		m_FramesFromLastVSync = 0;
	}

	void	Initialize( int refreshRate, int buffersCount );
	void	Initialize( DXGI_RATIONAL refreshRate, int buffersCount );

	void	Clear( )
	{
		m_SleepTime = 0;
		m_PrevVSyncTime.QuadPart = 0;
		m_bWaitVSync = false;
		m_FramesFromLastVSync = 0;
		m_pSynchronizer->Reset();
	}

	void	BeforePresent( )
	{
		QueryPerformanceCounter(&m_BeforePresentTime);
	}

	void	AfterPresent( )
	{
		QueryPerformanceCounter(&m_FrameTime);
		m_pSynchronizer->GetRasterStatus(&m_RasterLine);
		CalculateNextVSyncTime();
	}

	void	AfterPresentOne( )
	{
		MagicMethod();
	}

	void	AfterPresentTwo( )
	{
		QueryPerformanceCounter(&m_AfterPresentTime);
		MagicMethodTwo();
	}

	DWORD	GetLastSleepTime( )
	{
		return m_SleepTime;
	}
	LARGE_INTEGER*	GetVSyncTime( )
	{
		return &m_PrevVSyncTime;
	}

	DXGI_RATIONAL	GetRefreshRate( )
	{
		return m_RefreshRate;
	}

	UINT	GetFramesFromLastVSync( )
	{
		return m_FramesFromLastVSync;
	}

	void CalculateNextVSyncTime();

	void MagicMethod( );
	void MagicMethodTwo( );

private:

	DDrawSyncronizer*	m_pSynchronizer;
	DXGI_RATIONAL		m_RefreshRate;
	UINT				m_BuffersCount;
	UINT				m_FramesFromLastVSync;
	bool				m_bWaitVSync;

	LARGE_INTEGER		m_FrameTime;
	D3DRASTER_STATUS	m_RasterLine;
	LARGE_INTEGER		m_PrevVSyncTime;
	LARGE_INTEGER		m_VSyncEnd;
	LARGE_INTEGER		m_VSyncTime;
	LARGE_INTEGER		m_PrevTime;
	LARGE_INTEGER		m_Freq;
	LARGE_INTEGER		m_BeforePresentTime;
	LARGE_INTEGER		m_AfterPresentTime;
	LARGE_INTEGER		m_PrevPresentTime;
	int 				m_ActiveBuffers;
	DWORD 				m_SleepTime;
	float				m_FloatSleepTime;
};
