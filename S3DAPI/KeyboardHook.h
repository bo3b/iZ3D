/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "..\CommonUtils\System.h"
//#include "BaseSwapChain.h"

//--- keyboard settings ---
#define KEYBOARD_SCAN_RATE  100
#define REPEAT_ACCELERATION 1.01f   // Acceleration factor for speed multiplication

class KeyboardHook
{
public:
	KeyboardHook(); 
	~KeyboardHook();
	void S3DAPI_API initialize(DataInput* input);
	void S3DAPI_API setBackBufferWidth(DWORD width);
	//void setSwapChain(CBaseSwapChain* pSwapChain)	{ m_pSwapChain = pSwapChain; }
	void S3DAPI_API clear();
#pragma warning(disable:4251)
	CriticalSection m_Access;
#pragma warning(default:4251)
private:
	bool canRun() { return WaitForSingleObject(m_hThreadStopEvent, 1000/KEYBOARD_SCAN_RATE) != WAIT_OBJECT_0; }
	void KeyboardProcedure();
	bool IsKeyWorkInMono(EventId id);

	HANDLE		m_hHotKeyThread;
	unsigned    m_HotKeyThreadId;
	DWORD       m_FirstTimeKeyPressed;
	DWORD       m_LastKeyCodePressed;
	DWORD       m_RepeatCount;
	float       m_rBackBufferWidth;
	float       m_Scale;
	//CBaseSwapChain*	 m_pSwapChain;		
	HANDLE		m_hThreadStopEvent;
	//BOOL		m_RepaintNextFrame;
	//LARGE_INTEGER m_LastUpdateTime;
	long volatile	m_rc;
	static DWORD m_KeyRepeatRate;
	static DWORD m_KeyRepeatDelay;

	friend DWORD WINAPI HotKeyThread(KeyboardHook* lpParameter);
};

extern S3DAPI_API KeyboardHook gKbdHook;