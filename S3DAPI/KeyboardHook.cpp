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
#include "StdAfx.h"
#include <windows.h>
#include <stdio.h>
#include <commctrl.h>
#include <Mmsystem.h>
#include <math.h>
#include "KeyboardHook.h"
#include "S3DAPI.h"
#include "GlobalData.h"
//#include "..\S3DWrapper9\Trace.h" 

S3DAPI_API KeyboardHook gKbdHook;

DWORD KeyboardHook::m_KeyRepeatRate = 30;
DWORD KeyboardHook::m_KeyRepeatDelay = 250;

/************************************************************************/
/* this function processes wizard hot keys								*/
/************************************************************************/
void ProcessWizardKeys(DWORD modifier)
{
	for (int i = 0; i < 8; i++)
	{
		gInfo.WizKeyInfo.keys[i].isPressed = (GetAsyncKeyState((int)gInfo.WizKeyInfo.keys[i].wParam) & 0x8000) != 0;
	}
	
	DWORD wizardCode = gInfo.HotKeyList[TOGGLE_WIZARD].code;
	DWORD keyVKCode = wizardCode & 0xFF;
	SHORT keyState = GetAsyncKeyState(keyVKCode);
	DWORD code = keyState != 0 ? modifier | keyVKCode : 0;
	gInfo.WizKeyInfo.shiftMultKey = (keyState && code && wizardCode == code);
}

bool KeyboardHook::IsKeyWorkInMono(EventId id)
{
	switch(id)
	{
	case TOGGLE_STEREO:
	case TOGGLE_FPS:
	case TOGGLE_HOTKEY:
	case INCREASE_PRESENTER_SLEEP_TIME:
	case DECREASE_PRESENTER_SLEEP_TIME:
#ifndef FINAL_RELEASE
	case SWITCH_DEBUG_INFO_PAGE:
#endif
		return true;
	}
	return false;
}

void KeyboardHook::KeyboardProcedure()
{
	int i;
	if(!IsFocusInCurrentProcess())
		return;

	HotKeyInfo*  HotKeyList = (HotKeyInfo*)gInfo.HotKeyList;
	DWORD key_CTRL  =  IsKeyDownGlobal(VK_CONTROL) ? HOTKEYF_CONTROL : 0;
	DWORD key_SHIFT =  IsKeyDownGlobal(VK_SHIFT)   ? HOTKEYF_SHIFT   : 0;
	DWORD key_ALT   =  IsKeyDownGlobal(VK_MENU)    ? HOTKEYF_ALT     : 0;
	DWORD key_WIN   = (IsKeyDownGlobal(VK_LWIN) || IsKeyDownGlobal(VK_RWIN)) ? HOTKEYF_EXT : 0;
	DWORD modifier  = (key_ALT | key_CTRL | key_SHIFT | key_WIN) << 8;

	ProcessWizardKeys(modifier);
	for(i=0; i< MAX_EVENTS; i++)		 
	{
		DWORD keyVKCode = HotKeyList[i].code & 0xFF;
		SHORT keyState = GetAsyncKeyState(keyVKCode);
		DWORD code = keyState != 0 ? modifier | keyVKCode : 0;
		//--- keyState != 0                when GetAsyncKeyState SUCCEEDED ---
		//--- code     != 0                when key is hold down           ---
		//--- HotKeyList[i].code == code   when code is valid              ---
		if(keyState && code && HotKeyList[i].code == code)
		{
			bool processKey = true;
			DWORD currentTime = timeGetTime();
			DWORD currentCount;
			if(code == m_LastKeyCodePressed)
			{
				if(currentTime - m_FirstTimeKeyPressed >= KeyboardHook::m_KeyRepeatDelay)
				{
					currentCount = KeyboardHook::m_KeyRepeatRate * (currentTime - m_FirstTimeKeyPressed - KeyboardHook::m_KeyRepeatDelay) / 1000;
					if(currentCount == m_RepeatCount)
						processKey = false;
					else
						m_RepeatCount = currentCount;
				}
				else
					processKey = false;
			}
			else
			{
				m_FirstTimeKeyPressed = currentTime;
				m_LastKeyCodePressed  = code;
				m_RepeatCount = 0;
			}

			if(!processKey)
				break;

			if(m_RepeatCount == 0)
				m_Scale = 1.f;
			else if(!(m_RepeatCount % 10))
				m_Scale *= REPEAT_ACCELERATION;

			CriticalSectionLocker locker(m_Access);
			if (gInfo.Input.StereoActive || IsKeyWorkInMono(HotKeyList[i].id))
			{
#define OFFSET(x, y) (y * m_Scale + abs(x) * (m_Scale - 1))
				switch(HotKeyList[i].id)
				{
				case TOGGLE_STEREO:
					gInfo.Input.StereoActive = !gInfo.Input.StereoActive;
					break;

				case INCREASE_ZPS:
					{
						if(gInfo.Input.GetActivePreset()->StereoBase >= 0.1 * STEP_STEREOBASE)
						{
							float delta = STEP_ONE_DIV_ZPS * m_rBackBufferWidth * m_Scale / gInfo.Input.GetActivePreset()->StereoBase;
							gInfo.Input.GetActivePreset()->One_div_ZPS -= delta;
							if(gInfo.Input.GetActivePreset()->One_div_ZPS < MIN_ONE_DIV_ZPS)
								gInfo.Input.GetActivePreset()->One_div_ZPS = MIN_ONE_DIV_ZPS;
						}
						break;
					}
				case DECREASE_ZPS:
					{
						if(gInfo.Input.GetActivePreset()->StereoBase >= 0.1 * STEP_STEREOBASE)
						{
							float delta = STEP_ONE_DIV_ZPS * m_rBackBufferWidth * m_Scale / gInfo.Input.GetActivePreset()->StereoBase;
							gInfo.Input.GetActivePreset()->One_div_ZPS += delta;
							if(gInfo.Input.GetActivePreset()->One_div_ZPS > MAX_ONE_DIV_ZPS)
								gInfo.Input.GetActivePreset()->One_div_ZPS = MAX_ONE_DIV_ZPS;      
						}
						break;
					}  
				case INCREASE_STEREOBASE:
					gInfo.Input.GetActivePreset()->StereoBase += OFFSET(gInfo.Input.GetActivePreset()->StereoBase, STEP_STEREOBASE);
					if (gInfo.Input.GetActivePreset()->StereoBase > MAX_STEREOBASE) 
						gInfo.Input.GetActivePreset()->StereoBase = MAX_STEREOBASE;
					break;

				case DECREASE_STEREOBASE:
					gInfo.Input.GetActivePreset()->StereoBase -= OFFSET(gInfo.Input.GetActivePreset()->StereoBase, STEP_STEREOBASE);
					if (gInfo.Input.GetActivePreset()->StereoBase < 0) 
						gInfo.Input.GetActivePreset()->StereoBase = 0;
					break;

				case SET_PRESET_1:
					gInfo.Input.ActivePreset = 0;
					break;

				case SET_PRESET_2:
					gInfo.Input.ActivePreset = 1;
					break;

				case SET_PRESET_3:
					gInfo.Input.ActivePreset = 2;
					break;

				case SWAP_EYES:
					gInfo.Input.SwapEyes = !gInfo.Input.SwapEyes;
					break;

				case TOGGLE_AUTOFOCUS:
					gInfo.Input.SwitchCurrentPresetAutofocus();
					break;

				case TOGGLE_LASER_SIGHT:
					gInfo.Input.LaserSightEnable = !gInfo.Input.LaserSightEnable;
					break;

				case INCREASE_PRESENTER_SLEEP_TIME:
					if (gInfo.PresenterSleepTime < 0)
						gInfo.PresenterSleepTime = 0;
					else if (gInfo.PresenterSleepTime < 50)
						gInfo.PresenterSleepTime++;
					break;

				case DECREASE_PRESENTER_SLEEP_TIME:
					if (gInfo.PresenterSleepTime > 0)
						gInfo.PresenterSleepTime--;
					break;

				case INCREASE_PRESENTER_FLUSH_TIME:
					if (gInfo.PresenterFlushTime < 0)
						gInfo.PresenterFlushTime = 0;
					else if (gInfo.PresenterFlushTime < 49.6f)
						gInfo.PresenterFlushTime += 0.5f;
					break;

				case DECREASE_PRESENTER_FLUSH_TIME:
					if (gInfo.PresenterFlushTime > 0.4f)
						gInfo.PresenterFlushTime -= 0.5f;
					break;
				case TOGGLE_FPS:
					gInfo.Input.ShowFPS = !gInfo.Input.ShowFPS;
					break;
				case TOGGLE_HOTKEY:
					gInfo.Input.ShowHotKeyOSD = !gInfo.Input.ShowHotKeyOSD;
					break;
#ifndef FINAL_RELEASE
				case INCREASE_ZNEAR:
					gInfo.Input.ZNear += OFFSET(gInfo.Input.ZNear, STEP_MULTIPLIER);
					break;
				case DECREASE_ZNEAR:
					gInfo.Input.ZNear -= OFFSET(gInfo.Input.ZNear, STEP_MULTIPLIER);
					break;

				case INCREASE_MULTIPLIER:
					gInfo.Input.Multiplier += OFFSET(gInfo.Input.Multiplier, STEP_MULTIPLIER);
					break;
				case DECREASE_MULTIPLIER:
					gInfo.Input.Multiplier -= OFFSET(gInfo.Input.Multiplier, STEP_MULTIPLIER);
					break;

				case INCREASE_CONVERGENCE_SHIFT:
					{
						float delta = STEP_ONE_DIV_ZPS / 50 * m_Scale / gInfo.Input.Multiplier;
						gInfo.Input.ConvergenceShift += delta;
					}
					break;
				case DECREASE_CONVERGENCE_SHIFT:
					{
						float delta = STEP_ONE_DIV_ZPS / 50 * m_Scale / gInfo.Input.Multiplier;
						gInfo.Input.ConvergenceShift -= delta;
					}
					break;

				case SWITCH_DEBUG_INFO_PAGE:
					if ((++gInfo.Input.DebugInfoPage) > DEBUG_INFO_PAGES_COUNT) gInfo.Input.DebugInfoPage = 0;
					break;
#endif
				}
#undef OFFSET
			}
			break;
		}
	}
	bool bCheckRepaint = false;
	if(i == MAX_EVENTS)	//--- because only ONE button repeated ---
	{
		m_FirstTimeKeyPressed = 0;
		m_LastKeyCodePressed  = 0;
		m_RepeatCount = 0;
		//if (m_RepaintNextFrame)
		//	bCheckRepaint = true;
	}
	/*else 
		bCheckRepaint = true;
	if (bCheckRepaint && m_pSwapChain->m_PresentationParameters[0].Windowed)
	{	
		if (m_pSwapChain->m_nLastFrameTime.QuadPart > m_LastUpdateTime.QuadPart)
			m_LastUpdateTime = m_pSwapChain->m_nLastFrameTime;

		LARGE_INTEGER Freq, Time;
		QueryPerformanceFrequency(&Freq);
		QueryPerformanceCounter(&Time);

		if ((Time.QuadPart - m_LastUpdateTime.QuadPart) > Freq.QuadPart / 4 ) // 1/4 sec.
		{
			//--- button was handled ---
			RedrawWindow(m_pSwapChain->GetAppWindow(), NULL, NULL, RDW_INVALIDATE | RDW_INTERNALPAINT | RDW_ERASE);
			m_RepaintNextFrame = !m_RepaintNextFrame;
			m_LastUpdateTime = Time;
		}
	}*/
}

DWORD WINAPI HotKeyThread(KeyboardHook* lpParameter)
{
	while(lpParameter->canRun())
	{
		_ASSERT(lpParameter);
		lpParameter->KeyboardProcedure();
	}
	return 0;
}

KeyboardHook::KeyboardHook()
{	
	m_rc = 0;
	m_hHotKeyThread = 0;
	m_hThreadStopEvent = 0;
	m_HotKeyThreadId = 0;
	m_FirstTimeKeyPressed = 0;
	m_LastKeyCodePressed = 0;
	m_RepeatCount = 0;
	m_rBackBufferWidth = 1.0f / 1680;
	m_Scale = 0; 
	//m_pSwapChain = NULL;
	//m_LastUpdateTime.QuadPart = 0;
	//m_RepaintNextFrame = FALSE;
}

void KeyboardHook::initialize(DataInput* pInput)
{
	_ASSERT(pInput);
	InterlockedIncrement(&m_rc);
	if (m_rc > 1)
		return;
	int delay;
	DWORD speed;
	gInfo.Input = *pInput;
	m_FirstTimeKeyPressed = 0;
	m_LastKeyCodePressed = 0;
	m_RepeatCount = 0;
	m_Scale = 1.f;           
	SystemParametersInfo(SPI_GETKEYBOARDSPEED, 0, &speed, 0);
	SystemParametersInfo(SPI_GETKEYBOARDDELAY, 0, &delay, 0);
	KeyboardHook::m_KeyRepeatDelay = 250 * (1 + delay);
	KeyboardHook::m_KeyRepeatRate = DWORD(2.5 + speed * (30 - 2.5f) / 31);
	m_hThreadStopEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hHotKeyThread = (HANDLE)_beginthreadex( NULL, 0, (unsigned(__stdcall*)(void *))HotKeyThread, this, 0, &m_HotKeyThreadId);
	SetThreadPriority(m_hHotKeyThread, THREAD_PRIORITY_ABOVE_NORMAL);
	SetThreadName(m_HotKeyThreadId, "HotKey Thread");
	gInfo.WizKeyInfo.keys[0].wParam = 'C';
	gInfo.WizKeyInfo.keys[1].wParam = 'N';
	gInfo.WizKeyInfo.keys[2].wParam = 'Y';
	gInfo.WizKeyInfo.keys[3].wParam = 'P';
	gInfo.WizKeyInfo.keys[4].wParam = 'S';
	gInfo.WizKeyInfo.keys[5].wParam = 'F';
	gInfo.WizKeyInfo.keys[6].wParam = 'R';
	gInfo.WizKeyInfo.keys[7].wParam = 'B';
}

void KeyboardHook::clear()
{
	if (m_rc > 0)
		InterlockedDecrement(&m_rc);
	if(m_rc > 0 || !m_hHotKeyThread)
		return;
	SetEvent(m_hThreadStopEvent);
	if(WaitForSingleObject(m_hHotKeyThread, 500) == WAIT_TIMEOUT)
		TerminateThread(m_hHotKeyThread, 0);
	CloseHandle(m_hHotKeyThread);
	CloseHandle(m_hThreadStopEvent);
	m_hHotKeyThread = 0;
}

KeyboardHook::~KeyboardHook()
{
	clear();
}

void KeyboardHook::setBackBufferWidth(DWORD width)
{
	if(width)
		m_rBackBufferWidth = 1.f / width;
}
