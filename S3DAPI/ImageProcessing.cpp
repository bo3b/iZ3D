#include "StdAfx.h"
#include <emmintrin.h>
#include <intrin.h>
#include <Shlobj.h>
#include <math.h>
#include "ImageProcessing.h"
#include "..\CommonUtils\System.h"

float AutoFocus::UpdateZPS(ShiftFinder* pShift, CameraPreset* camera)
{
	//if(pShift->IsCalculationComplete())   //--- this checked in StereoRendererModified for optimization ---
	{
		//if(pShift->IsCurrentShiftValid())   //--- this checked in StereoRendererModified for optimization ---
		{
			if(pShift->IsShiftToHoldValid())
			{
				int pixelDelta = pShift->GetCurrentShift() - pShift->GetShiftToHold();
				if(pixelDelta != 0)
				{
					//--- find time delta between subsequent UpdateZPS() calls ---
					LARGE_INTEGER currentTime, freq;
					QueryPerformanceFrequency(&freq);
					QueryPerformanceCounter(&currentTime);
					m_CallTimeDelta.QuadPart = currentTime.QuadPart - m_PreviousCallTime.QuadPart;
					m_PreviousCallTime = currentTime;
					float frameTimeDelta = min(1.f, 1.0f * m_CallTimeDelta.QuadPart / freq.QuadPart);

					//--- convert time delta to shift speed ---
					float speed = 2.f / (1.f + gInfo.ShiftSpeed);
					float scale = (float)(pixelDelta * (1. - pow(0.1, double(frameTimeDelta / speed))));
					if(camera->StereoBase >= 0.1 * STEP_STEREOBASE)
					{
						float delta = scale * STEP_ONE_DIV_ZPS * pShift->GetReverseImageSizeX() / camera->StereoBase;

						//--- волшебная проверка на чудесное число ---
						//--- сделана временно, для теста, в дальнейшем возможно будет убрана ---
						if(gInfo.Max_ABS_rZPS == 0 || abs(camera->One_div_ZPS - delta) < gInfo.Max_ABS_rZPS) 
							//--- такая проверка мешает в тестовых примерах, например ShadowVolume ---
						{
							camera->One_div_ZPS -= delta;
							if(camera->One_div_ZPS > MAX_ONE_DIV_ZPS)
								camera->One_div_ZPS = MAX_ONE_DIV_ZPS;      
							else if(camera->One_div_ZPS < MIN_ONE_DIV_ZPS)
								camera->One_div_ZPS = MIN_ONE_DIV_ZPS;
						}
					}
				} 
			}
		}
	}
	return camera->One_div_ZPS;
}

DWORD WINAPI FindShiftThread(ShiftFinder* p)
{
	SetThreadName(-1, "Shift search thread");
	while(1)
	{
		InterlockedExchange(&p->m_bThreadReady, FALSE);
		if(p->m_bDeviceIsMultithreaded)
		{
			if(p->GetRTData())
			{
				p->findCorrelation();
				InterlockedExchange(&p->m_bThreadReady, TRUE);
			}
			else
			{
				DEBUG_MESSAGE(_T("Autofocus GetRenderTargetData() failed.\n"));
			}
		}
		else
		{
			p->findCorrelation();
			InterlockedExchange(&p->m_bThreadReady, TRUE);
		}

		if(p->m_bStopThread) 
			break;

		SuspendThread(p->m_hThread);
	}
	return 0;
}

ShiftFinder::ShiftFinder()
{
	m_hThread = 0;
	m_bThreadReady = TRUE;
	m_bStopThread = FALSE;
	m_bDeviceIsMultithreaded = FALSE;

	m_bInitializationSuccessful = false;
	m_ImageLeft  = 0;
	m_ImageRight = 0;
	m_rImageSizeX = 0.001f;
	m_CurrentShift = 0;
	m_ShiftToHold = 0;
	m_MinimumCorrelation = 0;
	m_SearchAreaSizeX = 0;
	m_HalfSearchIndexRange = 0;
	m_FrameSizeX = 512;
	m_FrameSizeY = 64;
	m_rFramePixelCount = 1.f;
	m_FrameYCoordinate = 0.495f;
	m_bCurrentShiftValid = false;
	m_bShiftToHoldValid = false;
	m_bInterfaceDetected = false;
	m_ZeroShiftCount = 0;
	m_ShiftInValidationFrame = 0;
	m_ShiftFindingFrame = 0;
	m_Curve = 0;

	int CPUInfo[4];
	__cpuid(CPUInfo, 1);
	if(CPUInfo[3] & (1 << 26))
		summarizeDeltas = &ShiftFinder::summarizeDeltasSSE;
	else
		summarizeDeltas = &ShiftFinder::summarizeDeltasCPP;

#if defined(OLEG_RESEARCH) && !defined(FINAL_RELEASE)
	m_StatsFile = NULL;
	m_bWriteStats = false;
	m_WriteStatsButton.selectButton('K');
#endif
}

void ShiftFinder::Clear()
{
	//--- must stop thread before memory release ---
	if(m_hThread)
	{
		InterlockedExchange(&m_bStopThread, TRUE);
		if(WAIT_TIMEOUT == WaitForSingleObject(m_hThread, 100))
			TerminateThread(m_hThread, 0);
		CloseHandle(m_hThread);
		m_hThread = 0;
	}

	_aligned_free(m_Curve);
	_aligned_free(m_ImageLeft);
	_aligned_free(m_ImageRight);

	m_Curve = 0;
	m_ImageLeft = 0;
	m_ImageRight = 0;

	m_ZeroShiftCount = 0;
	m_bInterfaceDetected = false;
	m_bCurrentShiftValid = false;
	m_bShiftToHoldValid  = false;
	m_ShiftInValidationFrame = 0;

	m_bStopThread  = FALSE;
	m_bThreadReady = TRUE;
	m_bInitializationSuccessful = false;

#if defined(OLEG_RESEARCH) && !defined(FINAL_RELEASE)
	if(m_StatsFile)
	{
		fclose(m_StatsFile);
		m_StatsFile = NULL;
	}
#endif
}

HRESULT ShiftFinder::Initialize(RECT* pLeftRect, RECT* pRightRect)
{
	m_bInitializationSuccessful = false;

	m_hThread = (HANDLE)_beginthreadex( NULL, 0, (unsigned(__stdcall*)(void *))FindShiftThread, 
		this, CREATE_SUSPENDED, NULL);
	if (m_hThread == NULL) 
	{
		DEBUG_MESSAGE(_T("Autofocus thread creation failed.\n"));
		return E_FAIL;
	}

	//--- convert to mono by 4 pixels at once ---
	m_SearchAreaSizeX = (pLeftRect->right - pLeftRect->left)& ~(4 - 1);
	//--- analyze 128 pixel at once ---
	m_FrameSizeX = gInfo.SearchFrameSizeX & ~(128 - 1); 
	//--- check X size of search area ---
	if(m_FrameSizeX == 0 || m_FrameSizeX >= m_SearchAreaSizeX)
	{
		DEBUG_TRACE1(_T("Incorrect Autofocus FrameSizeX\n"));

		//--- try to correct m_FrameSizeX ---
		m_FrameSizeX = max(128, (m_SearchAreaSizeX / 2) & ~(128 - 1)); 
		if(m_FrameSizeX <= 0 || m_FrameSizeX >= m_SearchAreaSizeX)
			return S_OK;

		DEBUG_TRACE1(_T("Autofocus FrameSizeX set to: %i\n"), m_FrameSizeX);
	}

	//--- try to correct m_FrameSizeY ---
	int searchFrameSizeY = gInfo.SearchFrameSizeY;
	int centerLeft  = pLeftRect->top  + (int)((1 - gInfo.FrameYCoordinate) * (pLeftRect->bottom  - pLeftRect->top ));
	int centerRight = pRightRect->top + (int)((1 - gInfo.FrameYCoordinate) * (pRightRect->bottom - pRightRect->top));
	if(centerLeft - searchFrameSizeY / 2 < pLeftRect->top)
		searchFrameSizeY -= 2 * (pLeftRect->top - (centerLeft - searchFrameSizeY / 2));
	if(centerLeft + searchFrameSizeY / 2 > pLeftRect->bottom)
		searchFrameSizeY -= 2 * ((centerLeft + searchFrameSizeY / 2) - pLeftRect->bottom);
	
	//--- analyze each INTERLACE_STEP line ---
	m_FrameSizeY = searchFrameSizeY / INTERLACE_STEP; 
	//--- check Y size of search area ---
	if(m_FrameSizeY <= 0)
	{
		DEBUG_TRACE1(_T("Incorrect Autofocus FrameSizeY or FrameYCoordinate\n"));
		return S_OK;
	}
	if(searchFrameSizeY != gInfo.SearchFrameSizeY)
	{
		DEBUG_TRACE1(_T("Autofocus FrameSizeY set to: %i\n"), searchFrameSizeY);
	}

	m_HalfSearchIndexRange = (m_SearchAreaSizeX - m_FrameSizeX) / 2;
	m_rImageSizeX = 1.f / (pLeftRect->right - pLeftRect->left);
	m_rFramePixelCount = 1.f / (m_FrameSizeX * m_FrameSizeY);

	m_srcRectLeft.left   = (pLeftRect->right + pLeftRect->left - m_FrameSizeX) / 2;
	m_srcRectLeft.right  = m_srcRectLeft.left + m_FrameSizeX;
	m_srcRectLeft.top    = centerLeft - searchFrameSizeY / 2;
	m_srcRectLeft.bottom = m_srcRectLeft.top + searchFrameSizeY;
	m_dstPointLeft.x     = 0;
	m_dstPointLeft.y     = 0;

	m_srcRectRight.left   = (pRightRect->right + pRightRect->left - m_SearchAreaSizeX) / 2;
	m_srcRectRight.right  = m_srcRectRight.left + m_SearchAreaSizeX;
	m_srcRectRight.top    = centerRight - searchFrameSizeY / 2;
	m_srcRectRight.bottom = m_srcRectRight.top + searchFrameSizeY;
	m_dstPointRight.x     = 0;
	m_dstPointRight.y     = searchFrameSizeY;

	DEBUG_TRACE1(_T("Autofocus src left: %s\n"),  GetRectString(&m_srcRectLeft));
	DEBUG_TRACE1(_T("Autofocus src right: %s\n"), GetRectString(&m_srcRectRight));

	//--- select m_FrameSizeX from left view and slide it throw m_SearchAreaSizeX from right view ---
	m_ImageLeft  = (BYTE*)_aligned_malloc(m_FrameSizeX      * m_FrameSizeY, 16);
	m_ImageRight = (BYTE*)_aligned_malloc(m_SearchAreaSizeX * m_FrameSizeY, 16);

	//--- save correlation curve ---
	m_Curve = (UINT32*)_aligned_malloc((2 * m_HalfSearchIndexRange + 1) * sizeof(UINT32), 16);

#if defined(OLEG_RESEARCH) && !defined(FINAL_RELEASE)
	PrepareDataDir();
#endif
	m_bInitializationSuccessful = true;

	return S_OK;
}

void ShiftFinder::FindShift(UINT currentFrame)
{
	//--- It's optimization.  This checking must exist, but we do it in child class ---
	//--- do not remove this lines,  because one can be forgotten ---
	//if(m_bInitializationSuccessful)
	{
		//if(m_bThreadReady)
		{
			m_ShiftFindingFrame = currentFrame;

#if defined(OLEG_RESEARCH) && !defined(FINAL_RELEASE)
			CheckButton(currentFrame);
			WriteLeftAndRight();
#endif
			if(m_bDeviceIsMultithreaded == false)
				GetRTData();
			ResumeThread(m_hThread);
		}
	}
}

void ShiftFinder::SetShiftToHoldInvalid(UINT currentFrame)
{
	m_bShiftToHoldValid = false; 
	m_ShiftInValidationFrame = currentFrame;
}

void ShiftFinder::findCorrelation()
{
	m_MinimumCorrelation = 0xFFFFFFFF;
	(this->*summarizeDeltas)();
	//--- find first local minimum ---
	bool searchResult   = false;
	int searchAreaDelta = m_SearchAreaSizeX - m_FrameSizeX;
	int localMinHalfWidth = m_FrameSizeX / 16;
	//--- for strong monotonic checking ---
	int maxSignCounter  = int(2 * localMinHalfWidth * 0.5);
	int zeroCounter     = 0; 

	for(int k = localMinHalfWidth; k < searchAreaDelta - localMinHalfWidth; k++)
	{
		if(m_Curve[k] == 0)
			zeroCounter++;
		bool loclaMinFound = false;
		int  signCounter = 0;
		for(int j=localMinHalfWidth; j > 0; j--)
		{
			if(m_Curve[k-j] > m_Curve[k-j+1])
				signCounter++;
			else
				signCounter--;
		}
		for(int j=0; j< localMinHalfWidth; j++)
		{
			if(m_Curve[k+j] < m_Curve[k+j+1])
				signCounter++;
			else
				signCounter--;
		}
		if(signCounter >= maxSignCounter)
			loclaMinFound = true;
		if(loclaMinFound)
		{
			if(m_Curve[k-localMinHalfWidth] - m_Curve[k] >= m_Curve[k-localMinHalfWidth] / 16 && m_Curve[k+localMinHalfWidth] - m_Curve[k] >= m_Curve[k+localMinHalfWidth] / 16)
			{
				if(m_Curve[k] < m_MinimumCorrelation)  
				{
					m_MinimumCorrelation = m_Curve[k];
					m_CurrentShift = k - m_HalfSearchIndexRange;
					searchResult = true;
					//break;						//--- comment this string for finding global minimum ---
				}
			}
		}
	}
	//--- analyze result ---
	DWORD searchResultMask = !searchResult;

	if(searchResult)
	{
		if(m_CurrentShift == 0)
		{
			m_ZeroShiftCount++;
			if(m_ZeroShiftCount >= 4)
				m_bInterfaceDetected = true;

			if(m_bInterfaceDetected)
				searchResultMask += 1 << 4;
		}
		else
		{
			m_ZeroShiftCount = 0;
			m_bInterfaceDetected = false;
		}

		int shiftRange = min(m_HalfSearchIndexRange, (int)(m_FrameSizeX * 0.75));
		searchResultMask += ((m_CurrentShift <= -shiftRange || m_CurrentShift >= shiftRange) << 1) +
			((zeroCounter >= searchAreaDelta / 2) << 2) +
			((m_MinimumCorrelation > 128.0f * m_FrameSizeX * m_FrameSizeY) << 3);
	}

	if(searchResultMask)
		m_bCurrentShiftValid = false;
	else
	{
		m_bCurrentShiftValid = true;
		if(!m_bShiftToHoldValid && m_ShiftFindingFrame > m_ShiftInValidationFrame)
		{	
			m_ShiftToHold = m_CurrentShift;
			m_bShiftToHoldValid = true;
		}
	}
#if defined(OLEG_RESEARCH) && !defined(FINAL_RELEASE)
	WriteStats(searchResultMask);
#endif
}

void ShiftFinder::convert_R8G8B8A8_ToMonochrome(BYTE* src, int srcPith, BYTE* dest, int destSizeX)
{
	int srcDelta = srcPith - destSizeX * 4;
	for(int i=0; i< m_FrameSizeY; i++, src += srcDelta)
	{
		BYTE* end = dest + destSizeX;
		for(; dest < end; dest += 4, src += 16)
		{	//                               R                             G                             B
			dest[0]  = (BYTE(256 * 0.299) * src[ 0] + BYTE(256 * 0.587) * src[ 1] + BYTE(256 * 0.114) * src[ 2]) >> 8;
			dest[1]  = (BYTE(256 * 0.299) * src[ 4] + BYTE(256 * 0.587) * src[ 5] + BYTE(256 * 0.114) * src[ 6]) >> 8;
			dest[2]  = (BYTE(256 * 0.299) * src[ 8] + BYTE(256 * 0.587) * src[ 9] + BYTE(256 * 0.114) * src[10]) >> 8;
			dest[3]  = (BYTE(256 * 0.299) * src[12] + BYTE(256 * 0.587) * src[13] + BYTE(256 * 0.114) * src[14]) >> 8;
		}
	}
}

void ShiftFinder::convert_X8R8G8B8_ToMonochrome(BYTE* src, int srcPith, BYTE* dest, int destSizeX)
{
	int srcDelta = srcPith - destSizeX * 4;
	for(int i=0; i< m_FrameSizeY; i++, src += srcDelta)
	{
		BYTE* end = dest + destSizeX;
		for(; dest < end; dest += 4, src += 16)
		{
			dest[0]  = (BYTE(256 * 0.114) * src[ 0] + BYTE(256 * 0.587) * src[ 1] + BYTE(256 * 0.299) * src[ 2]) >> 8;
			dest[1]  = (BYTE(256 * 0.114) * src[ 4] + BYTE(256 * 0.587) * src[ 5] + BYTE(256 * 0.299) * src[ 6]) >> 8;
			dest[2]  = (BYTE(256 * 0.114) * src[ 8] + BYTE(256 * 0.587) * src[ 9] + BYTE(256 * 0.299) * src[10]) >> 8;
			dest[3]  = (BYTE(256 * 0.114) * src[12] + BYTE(256 * 0.587) * src[13] + BYTE(256 * 0.299) * src[14]) >> 8;
		}
	}
}

void ShiftFinder::summarizeDeltasCPP()
{
	for(int i = -m_HalfSearchIndexRange; i<= m_HalfSearchIndexRange; i++)
	{
		UINT diff = 0;
		BYTE* p1 =  m_ImageLeft;
		BYTE* p2 = &m_ImageRight[i + m_HalfSearchIndexRange];
		for(int k=0; k< m_FrameSizeY; k++, p2 += m_SearchAreaSizeX - m_FrameSizeX)
		{
			BYTE* end = p1 + m_FrameSizeX;
			for(; p1< end; p1++, p2++)
				diff += abs(*p1 - *p2);
		}
		m_Curve[i + m_HalfSearchIndexRange] = diff;
	}
}

//--- for disable warning 4700 and skip debug run-time initialization checking ---
void fake(__m128i*) {}

void ShiftFinder::summarizeDeltasSSE()
{
	bool searchResult = false;
	int frameSizeX_SSE2 = m_FrameSizeX / sizeof(__m128i);
	int searchAreaDelta = m_SearchAreaSizeX - m_FrameSizeX;
	for(int i = -m_HalfSearchIndexRange; i<= m_HalfSearchIndexRange; i++)
	{
		__m128i diff_SSE2; fake(&diff_SSE2);
		diff_SSE2 = _mm_xor_si128(diff_SSE2, diff_SSE2);
		__m128i* p1 = (__m128i*) m_ImageLeft;
		__m128i* p2 = (__m128i*)&m_ImageRight[i + m_HalfSearchIndexRange];
		for(int k=0; k< m_FrameSizeY; k++, p2 = (__m128i*)((BYTE*)p2 + searchAreaDelta))
		{
			__m128i* end = p1 + frameSizeX_SSE2;
			for(; p1 < end; p1 += 8, p2 += 8)
			{
				__m128i sum0 = _mm_sad_epu8(_mm_load_si128(p1 + 0), _mm_loadu_si128(p2 + 0));
				__m128i sum1 = _mm_sad_epu8(_mm_load_si128(p1 + 1), _mm_loadu_si128(p2 + 1));
				__m128i sum2 = _mm_sad_epu8(_mm_load_si128(p1 + 2), _mm_loadu_si128(p2 + 2));
				__m128i sum3 = _mm_sad_epu8(_mm_load_si128(p1 + 3), _mm_loadu_si128(p2 + 3));
				__m128i sum4 = _mm_sad_epu8(_mm_load_si128(p1 + 4), _mm_loadu_si128(p2 + 4));
				__m128i sum5 = _mm_sad_epu8(_mm_load_si128(p1 + 5), _mm_loadu_si128(p2 + 5));
				__m128i sum6 = _mm_sad_epu8(_mm_load_si128(p1 + 6), _mm_loadu_si128(p2 + 6));
				__m128i sum7 = _mm_sad_epu8(_mm_load_si128(p1 + 7), _mm_loadu_si128(p2 + 7));

				diff_SSE2 = _mm_add_epi32(diff_SSE2, sum0);
				diff_SSE2 = _mm_add_epi32(diff_SSE2, sum1);
				diff_SSE2 = _mm_add_epi32(diff_SSE2, sum2);
				diff_SSE2 = _mm_add_epi32(diff_SSE2, sum3);
				diff_SSE2 = _mm_add_epi32(diff_SSE2, sum4);
				diff_SSE2 = _mm_add_epi32(diff_SSE2, sum5);
				diff_SSE2 = _mm_add_epi32(diff_SSE2, sum6);
				diff_SSE2 = _mm_add_epi32(diff_SSE2, sum7);
			}
		}
		//--- convert correlation to UINT32 ---
		diff_SSE2 = _mm_add_epi32(_mm_srli_si128(diff_SSE2, 8), diff_SSE2);
		m_Curve[i + m_HalfSearchIndexRange] = diff_SSE2.m128i_u32[0];
	}
}


#if defined(OLEG_RESEARCH) && !defined(FINAL_RELEASE)
void ShiftFinder::CheckButton(UINT currentFrame)
{
	if(m_WriteStatsButton.isButtonWasPressed())
	{
		m_bWriteStats = !m_bWriteStats;
		if(m_bWriteStats)  Beep(3000, 500);
		if(!m_bWriteStats)  Beep(1000, 500);
	}
	m_CurrentFrame = currentFrame;
}

HRESULT ShiftFinder::PrepareDataDir()
{
	HRESULT hResult = SHGetFolderPath(NULL, CSIDL_MYPICTURES|CSIDL_FLAG_CREATE, NULL, 0, m_DataPath);
	if(SUCCEEDED(hResult))
	{
		PathAppend(m_DataPath, _T(PRODUCT_NAME) _T(" AutoFocus Data"));
		_tcscpy_s(m_StatsFileName, _countof(m_StatsFileName), m_DataPath);
		PathAppend(m_StatsFileName, _T("Statistics.csv"));
		CreateDirectory(m_DataPath, NULL);
		DeleteAllFiles(m_DataPath);
		_tfopen_s(&m_StatsFile, m_StatsFileName, _T("w"));
	}
	return hResult;
}

void ShiftFinder::WriteLeftAndRight()
{
	if(m_bWriteStats)
	{
		TCHAR str[MAX_PATH];
		_stprintf_s(str, _T("%s\\Data %5i.bmp"), m_DataPath, m_CurrentFrame);
		D3DXSaveSurfaceToFile(str, D3DXIFF_BMP, m_hMemoryBuffer, NULL, NULL);
	}
}

void ShiftFinder::WriteStats(DWORD searchResultMask)
{
	if(m_bWriteStats)
	{
		fprintf(m_StatsFile, "Frame = %8i  Shift = %8i ShiftToHold = %8i Correlation = %8i searchResultMask = %i  Data: ",  m_CurrentFrame, m_CurrentShift, m_ShiftToHold, m_MinimumCorrelation, searchResultMask);
		for(int i=0; i< 2 * m_HalfSearchIndexRange + 1; i++)
			fprintf(m_StatsFile, "%08i ", m_Curve[i]);
		fprintf(m_StatsFile, "\n");
		fflush(m_StatsFile);
	}
}
#endif 
