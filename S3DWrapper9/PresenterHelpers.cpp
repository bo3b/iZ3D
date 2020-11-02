/* IZ3D_FILE: $Id$ 
*
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*
* $Author$
* $Revision$
* $Date$
* $LastChangedBy$
* $URL$
*/

#include "stdafx.h"
#include <fstream>
#include "PresenterHelpers.h"

void DeviceTimeStamp::WriteToStream(std::wostream& fs, UINT height, LARGE_INTEGER& freq)
{
	float p = 1.0f * rasterStatus.ScanLine / height;
	float d = 1000.0f * time.QuadPart / freq.QuadPart;
	fs << rasterStatus.InVBlank << _T(";") << p << _T(";") << d << _T(";");
}

CPresentHistory::CPresentHistory( int refreshRate, int height ) :	m_RefreshRate( refreshRate )
,	m_Height( height )
,	m_BufferSize( refreshRate * 2 )
,	m_bSaveStatistic( false )
,	m_vFrameTimes( m_BufferSize )
{
	m_LastFrameTime.QuadPart = 0;
	m_LastVSyncTime.QuadPart = 0;
	m_Synchronizer.Initialize();

	m_CurrentData.flushCount = 0;
	m_CurrentData.getDataCount = 0;
	m_CurrentData.skipPerSecond = 0;
}

void CPresentHistory::PushData()
{
	if(m_LastFrameTime.QuadPart == 0)
	{
		m_LastFrameTime = m_CurrentData.afterPresent.time;
		m_LastVSyncTime = m_CurrentData.vsyncTime;
		return;
	}

	if(IsKeyPressed(222) && !m_bSaveStatistic) //--- reset statistic and start collection again ---
	{
		m_bSaveStatistic = true;
		Beep(3000, 300);
		m_vFrameTimes.clear();
	}

	LARGE_INTEGER curFrameTime = m_CurrentData.afterPresent.time;
	m_CurrentData.afterSleep.time.QuadPart		-= m_LastFrameTime.QuadPart;
	m_CurrentData.beforePresent.time.QuadPart	-= m_LastFrameTime.QuadPart;
	m_CurrentData.afterPresent.time.QuadPart	-= m_LastFrameTime.QuadPart;
	m_LastFrameTime = curFrameTime;
	LARGE_INTEGER curVSyncTime = m_CurrentData.vsyncTime;
	m_CurrentData.vsyncTime.QuadPart	-= m_LastVSyncTime.QuadPart;
	m_LastVSyncTime = curVSyncTime;
	m_vFrameTimes.push_back(m_CurrentData);

	m_CurrentData.flushCount = 0;
	m_CurrentData.getDataCount = 0;
	m_CurrentData.skipPerSecond = 0;
	if( m_vFrameTimes.is_linearized() && m_vFrameTimes.full() )
	{
		if ( m_bSaveStatistic )
		{
			SaveToFile();
			Beep(1000, 300);
			m_bSaveStatistic = false;
		}
	}
}

void CPresentHistory::SaveToFile()
{
#ifndef FINAL_RELEASE
	TCHAR fileName[MAX_PATH];
	_tcscpy(fileName, gData.DumpDirectory);
	TCHAR* s = _tcsrchr(gInfo.ApplicationFileName, '\\');
	if(s)
	{
		_tcscat(fileName, s);
		s = _tcsrchr(fileName, '.');
		if(s)
			_tcscpy(s, L"_ph.csv");
		else
			_tcscpy(fileName, L"_ph.csv");
	}
	else
	{
		_tcscat(fileName, L"\\S3DDebug_ph.csv");
	}
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	std::wofstream file(fileName);
	file.imbue(std::locale("rus_rus.1251"));
	file << _T("afterSleepVBlank;afterSleepScanLine;afterSleepTime;");
	file << _T("beforePresentVBlank;beforePresentScanLine;beforePresentTime;");
	file << _T("afterPresentVBlank;afterPresentScanLine;frameTime;");
	file << _T("opTime;presentTime;");
	file << _T("vSyncTime;");
	//file <<  _T("presentCount");
	file << _T("f - gameFps;");
	file << _T("P - presenterFps;");
	file << _T("T - sleepTime;");
	file << _T("S - skipPerSecond;");
	file <<  _T("\n");
	for( boost::circular_buffer<PresentData>::iterator it = m_vFrameTimes.begin(); it != m_vFrameTimes.end(); ++it )
	{
		it->afterSleep.WriteToStream(file, m_Height, freq);
		it->beforePresent.WriteToStream(file, m_Height, freq);
		it->afterPresent.WriteToStream(file, m_Height, freq);
		file <<  _T("=RC[-4]-RC[-7];=RC[-2]-RC[-5];");
		float d = 1000.0f * it->vsyncTime.QuadPart/ freq.QuadPart;
		file << d << _T(";");
		//file <<	it->presentCount;

		//--- data for x0ras-like  research and analysis ---
		file << it->gameFps << _T(";");
		file << it->presenterFps << _T(";");
		file << it->sleepTime << _T(";");
		file << it->skipPerSecond << _T(";");

		file << _T("\n");
	}
#endif
}

void CSuperDuperSynchronizer::Initialize( int refreshRate, int buffersCount )
{
	DXGI_RATIONAL rate;
	rate.Numerator = refreshRate;
	rate.Denominator = 1;
	Initialize( rate, buffersCount );
}

void CSuperDuperSynchronizer::Initialize( DXGI_RATIONAL refreshRate, int buffersCount )
{
	QueryPerformanceFrequency(&m_Freq);
	m_RefreshRate = refreshRate;
	LARGE_INTEGER minVSyncTime;
	LARGE_INTEGER maxVSyncTime; 
	LONGLONG val = m_Freq.QuadPart * m_RefreshRate.Denominator;
	minVSyncTime.QuadPart = val / (m_RefreshRate.Numerator + m_RefreshRate.Denominator);
	maxVSyncTime.QuadPart = val / (m_RefreshRate.Numerator - m_RefreshRate.Denominator);
	m_BuffersCount = buffersCount;
	m_pSynchronizer->Initialize();
	LARGE_INTEGER t2;
	LARGE_INTEGER vSyncTime;
	int c = 0;
	do 
	{
		LARGE_INTEGER t1;
		m_pSynchronizer->WaitForVBlank();
		QueryPerformanceCounter(&t1);
		m_pSynchronizer->WaitForNonVBlank();
		m_pSynchronizer->WaitForVBlank();
		QueryPerformanceCounter(&t2);
		vSyncTime.QuadPart = t2.QuadPart - t1.QuadPart;
		c++;
		if (c == 5)
			break; // too long, maybe wrong refresh rate
	} while (minVSyncTime.QuadPart > vSyncTime.QuadPart || vSyncTime.QuadPart > maxVSyncTime.QuadPart);
	m_VSyncTime.QuadPart = vSyncTime.QuadPart;
	m_VSyncEnd.QuadPart = t2.QuadPart;
	m_PrevTime = m_VSyncEnd;
	m_SleepTime = 0;
	//float time = 1000.0f * m_VSyncTime.QuadPart / m_Freq.QuadPart;
}

void CSuperDuperSynchronizer::CalculateNextVSyncTime()
{
	m_bWaitVSync = m_RasterLine.InVBlank != 0 || m_RasterLine.ScanLine <= 60;
	LARGE_INTEGER t;
	t.QuadPart = m_FrameTime.QuadPart - m_VSyncEnd.QuadPart;
	// number of frames from latest correct VSync time
	m_FramesFromLastVSync = (DWORD)(1.0f * t.QuadPart / m_VSyncTime.QuadPart + 0.1f);

	// m_ActiveBuffers only for debug
	LARGE_INTEGER timeFromPrev;
	timeFromPrev.QuadPart = m_FrameTime.QuadPart - m_PrevTime.QuadPart;
	int framesFromPrevious = (int)(1.0f * timeFromPrev.QuadPart / m_VSyncTime.QuadPart + 0.1f);

	if (m_bWaitVSync)
	{
		if (framesFromPrevious > 1)
			m_ActiveBuffers = std::max(m_ActiveBuffers - (framesFromPrevious - 1), 1);
		//m_CurrentData.presentCount = n + m_ActiveBuffers * 1000;

		if (m_FramesFromLastVSync > 0 && m_FramesFromLastVSync < 3)
		{
			LARGE_INTEGER minVSyncTime;
			LARGE_INTEGER maxVSyncTime; 
			LARGE_INTEGER vSyncTime;
			LONGLONG val = m_Freq.QuadPart * m_RefreshRate.Denominator;
			minVSyncTime.QuadPart = val / (m_RefreshRate.Numerator + m_RefreshRate.Denominator);
			maxVSyncTime.QuadPart = val / (m_RefreshRate.Numerator - m_RefreshRate.Denominator);
			vSyncTime.QuadPart = (m_VSyncTime.QuadPart + t.QuadPart / m_FramesFromLastVSync) / 2; // average
			if (minVSyncTime.QuadPart <= vSyncTime.QuadPart && vSyncTime.QuadPart <= maxVSyncTime.QuadPart)
				m_VSyncTime.QuadPart = vSyncTime.QuadPart;
		}
		m_VSyncEnd = m_FrameTime;
		m_PrevVSyncTime  = m_FrameTime;
	}
	else
	{	
		if (framesFromPrevious > 1)
			m_ActiveBuffers = std::max(m_ActiveBuffers - (framesFromPrevious - 1), 1);
		else if (framesFromPrevious == 0)
			m_ActiveBuffers++;

		// time of previous VSync signal
		m_PrevVSyncTime.QuadPart = m_VSyncEnd.QuadPart + m_FramesFromLastVSync * m_VSyncTime.QuadPart;
	}
	m_PrevTime = m_FrameTime;
}

void CSuperDuperSynchronizer::MagicMethod()
{
	LARGE_INTEGER sleepTime = { 0 };

	if (m_bWaitVSync)
	{
		sleepTime = m_VSyncTime;
		m_SleepTime = (DWORD)(1000 * sleepTime.QuadPart / m_Freq.QuadPart);
	}
	else
	{
		LARGE_INTEGER diff = { 0 };
		diff.QuadPart = m_PrevTime.QuadPart - m_PrevVSyncTime.QuadPart;
		sleepTime.QuadPart = m_VSyncTime.QuadPart - diff.QuadPart;
		m_SleepTime = (DWORD)(1000 * sleepTime.QuadPart / m_Freq.QuadPart);
		m_SleepTime += 2;
	}
}

void CSuperDuperSynchronizer::MagicMethodTwo()
{
	float			sleepTime = m_FloatSleepTime;
	LARGE_INTEGER	presentTime;
	presentTime.QuadPart = m_AfterPresentTime.QuadPart - m_BeforePresentTime.QuadPart;
	const float		sleepTimeIncrement	= 0.125f;			// 1/8 of a millisecond
	const float		sleepTimeDecrement	= 0.25f;			// 1/4 of a millisecond
	const float		timeDelta			= 0.0625f;			// in milliseconds
	bool			bWait1 = (1000.0f * m_PrevPresentTime.QuadPart / m_Freq.QuadPart) > timeDelta;
	bool			bWait2 = (1000.0f *       presentTime.QuadPart / m_Freq.QuadPart) > timeDelta;

	if( bWait1  &&  bWait2 )								// both Present() calls were waiting for V-Sync
		sleepTime += sleepTimeIncrement;					// ...so increase the presenter Sleep() period

	if( !bWait1 && !bWait2 )								// both Present() didn't waiting for V-Sync
		sleepTime -= sleepTimeDecrement;					// ...so decrease the presenter Sleep() period		

	if( bWait1  !=  bWait2 )								// only one Present() call was waiting
	{
		// do nothing, it's perfect timing 
	}

	m_PrevTime	      = m_FrameTime = m_AfterPresentTime;
	m_PrevPresentTime = presentTime;
	m_FloatSleepTime  = std::min( std::max( sleepTime, 0.0f ), 1000.0f * m_RefreshRate.Denominator / m_RefreshRate.Numerator );
	m_SleepTime       = (DWORD)m_FloatSleepTime;
}

#ifndef FINAL_RELEASE
CAverage::~CAverage()
{
	UINT64 total = 0;
	for (size_t i = 0; i < m_DebugData.size(); i++)
		total += m_DebugData[i];
	if (total == 0)
		return;

	float rtotal = 1.0f / total;

	DEBUG_MESSAGE(_T("Stable Cycles Information\n"));
	for (size_t i = 0; i < m_DebugData.size(); i++)
	{
		float p = m_DebugData[i] * rtotal;
		DEBUG_MESSAGE(_T("% d\t"), i);
	}
	DEBUG_MESSAGE(_T("\n"));
	for (size_t i = 0; i < m_DebugData.size(); i++)
	{
		float p = 100 * m_DebugData[i] * rtotal;
		DEBUG_MESSAGE(_T("% 3.2f%%\t"), p);
	}
	DEBUG_MESSAGE(_T("\n"));
}
#endif

void CAverage::SetRefreshRate( ULONG refreshRate )
{
	DXGI_RATIONAL rate;
	rate.Numerator = refreshRate;
	rate.Denominator = 1;
	SetRefreshRate(rate);
}

void CAverage::SetRefreshRate( DXGI_RATIONAL refreshRate )
{
	m_RefreshRate = refreshRate;
	m_SleepTime = 0;

	int maxSleepTime = refreshRate.Numerator > 0 ? GetMaxSleepTime() : 0;
	m_SleepTime = maxSleepTime / 2;
	m_SleepTimeChange = 0;
#ifndef FINAL_RELEASE
	DEBUG_MESSAGE(_T("Max sleep time: %d\n"), maxSleepTime);
	DEBUG_MESSAGE(_T("Def sleep time: %d\n"), m_SleepTime);
	m_DebugData.resize(maxSleepTime + 1, 0);
#endif

	std::vector<float>	emptyVec;
	m_nSamplesCount	= (size_t)((float)refreshRate.Numerator / refreshRate.Denominator + 0.5f); //  1 sec
	m_vFrameTimes.clear();
	m_vFrameTimes.set_capacity( m_nSamplesCount );
}

void CAverage::PushFrameTime( float val )
{
	m_vFrameTimes.push_back( val );

	if( m_vFrameTimes.is_linearized() && m_vFrameTimes.full() )
		m_bFullCycle = true;
}

float CAverage::GetFPS()
{
	if( m_vFrameTimes.empty() )
		return 0;

	float	sum = 0;
	for( boost::circular_buffer<float>::const_iterator it = m_vFrameTimes.begin();it != m_vFrameTimes.end(); ++it )
		sum += *it;
	return( m_nSamplesCount / (sum / 1000) );
}

float CAverage::GetJitterDeviation()
{
	if( m_vFrameTimes.empty() )
		return 0;

	float	jitterSum = 0;
	for( boost::circular_buffer<float>::const_iterator it = m_vFrameTimes.begin();it != m_vFrameTimes.end(); ++it )
		jitterSum += *it;
	float jitterMean = jitterSum / 1000 / m_nSamplesCount;
	float DeviationSum = 0;
	for( boost::circular_buffer<float>::const_iterator it = m_vFrameTimes.begin();it != m_vFrameTimes.end(); ++it )
	{
		float Deviation = *it / 1000 - jitterMean;
		DeviationSum += Deviation*Deviation;
	}
	float StdDev = sqrt(DeviationSum/m_nSamplesCount);
	return StdDev;
}

size_t CAverage::GetLagCount()
{
	const float threshold = 1.5f;
	size_t	count = 0;
	float vblankTime = 1000.0f * m_RefreshRate.Denominator / m_RefreshRate.Numerator;
	for( boost::circular_buffer<float>::const_iterator it = m_vFrameTimes.begin(); it != m_vFrameTimes.end(); ++it )
	{
		float frameTime = *it;
		if( frameTime / vblankTime > threshold )
			++count;
	}
	return count;
}

UINT CAverage::GetSleepTime()
{
	if (!m_bFullCycle)
		return m_SleepTime;

	m_bFullCycle = false;
	float	lagPercent = 100.0f * GetLagCount() / m_nSamplesCount;
	if (lagPercent <= gInfo.PresenterMaxLagPercent)
	{
#ifndef FINAL_RELEASE
		if (m_SleepTime < (int)m_DebugData.size())
			m_DebugData[m_SleepTime]++;
		else {
			DEBUG_MESSAGE(_T("WARNING: SleepTime bigger than max SleepTime\n"));
		}
#endif
		if (m_StableCyclesCount < m_SleepTimeIncreaseDelay) // approximately 10 seconds
		{
			m_StableCyclesCount++;
			if (m_StableCyclesCount <= 3 && m_SleepTimeChange > 0)
			{
				DEBUG_MESSAGE(_T("Reset SleepTimeIncreaseDelay after increase SleepTime\n"));
				m_SleepTimeIncreaseDelay = m_DefSleepTimeIncreaseDelay;
			}
		}
		else
		{
			if (m_SleepTime < GetMaxSleepTime())
			{
				m_StableCyclesCount = 0; 
				m_SleepTime++;
				m_SleepTimeChange = +1;
			}
		}
	}
	else 
	{
		if (m_SleepTime > 0)
		{
			if (m_StableCyclesCount <= 3 && m_SleepTimeChange > 0)
			{
				m_SleepTimeIncreaseDelay *= 3;
				DEBUG_MESSAGE(_T("SleepTimeIncreaseDelay increased %d\n"), m_SleepTimeIncreaseDelay);
			}
			else
			{
				DEBUG_MESSAGE(_T("Reset SleepTimeIncreaseDelay after decrease SleepTime\n"));
				m_SleepTimeIncreaseDelay = m_DefSleepTimeIncreaseDelay;
			}
			m_SleepTime--;
			m_SleepTimeChange = -1;
		}
		m_StableCyclesCount = 0;
	}
	return m_SleepTime;
}

FLOAT CAverage::GetFlushTime()
{
	if (!m_bFullCycle)
		return m_PresenterFlushTime;

	m_bFullCycle = false;
	if (GetLagCount() <= 0)
	{
		if (m_StableCyclesCount < m_SleepTimeIncreaseDelay) // approximately 10 seconds
		{
			m_StableCyclesCount++;
			if (m_StableCyclesCount <= 3 && m_SleepTimeChange > 0 && m_SleepTimeIncreaseDelay != m_DefFlushTimeIncreaseDelay)
			{
				DEBUG_MESSAGE(_T("Reset FlushTimeIncreaseDelay after increase FlushTime\n"));
				m_SleepTimeIncreaseDelay = m_DefFlushTimeIncreaseDelay;
			}
		}
		else
		{
			if (m_PresenterFlushTime < GetMaxSleepTime())
			{
				m_StableCyclesCount = 0; 
				if (m_PresenterFlushTime >= 0.49f)
					m_PresenterFlushTime += 0.5f;
				else
					m_PresenterFlushTime += 0.1f;
				m_SleepTimeChange = +1;
			}
		}
	}
	else 
	{
		if (m_PresenterFlushTime > 0.19f)
		{
			if (m_StableCyclesCount <= 3 && m_SleepTimeChange > 0)
			{
				m_SleepTimeIncreaseDelay *= 3;
				DEBUG_MESSAGE(_T("FlushTimeIncreaseDelay increased %d\n"), m_SleepTimeIncreaseDelay);
			}
			else if (m_SleepTimeIncreaseDelay != m_DefFlushTimeIncreaseDelay)
			{
				DEBUG_MESSAGE(_T("Reset FlushTimeIncreaseDelay after decrease FlushTime\n"));
				m_SleepTimeIncreaseDelay = m_DefFlushTimeIncreaseDelay;
			}
			if (m_PresenterFlushTime >= 0.99f)
				m_PresenterFlushTime -= 0.5f;
			else
				m_PresenterFlushTime -= 0.1f;
			m_SleepTimeChange = -1;
		}
		m_StableCyclesCount = 0;
	}
	return m_PresenterFlushTime;
}