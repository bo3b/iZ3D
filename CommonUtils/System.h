#pragma once
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <commctrl.h>
#include <cassert>
#include <iostream>

// you should consider CriticalSectionLocker whenever possible instead of
// directly working with CriticalSection class - it is safer
class CriticalSection
{
public:
	// ctor & dtor
	inline CriticalSection();
	inline ~CriticalSection();

	// enter the section
	inline void Enter();

	// try enter the section
	inline bool TryEnter();

	// leave the critical section
	inline void Leave();

private:
	CRITICAL_SECTION m_critRegion;
};

// implement CriticalSection
inline CriticalSection::CriticalSection() { InitializeCriticalSection(&m_critRegion); }
inline CriticalSection::~CriticalSection() { DeleteCriticalSection(&m_critRegion); }

inline void CriticalSection::Enter() { ULONG_PTR currentOwner = (ULONG_PTR)m_critRegion.OwningThread; UNREFERENCED_PARAMETER(currentOwner); EnterCriticalSection(&m_critRegion); }
inline bool CriticalSection::TryEnter() { return (TryEnterCriticalSection(&m_critRegion) != 0); }
inline void CriticalSection::Leave() { LeaveCriticalSection(&m_critRegion); }

class CriticalSectionLocker
{
public:
	explicit CriticalSectionLocker(CriticalSection& cs)
		: m_critsect(cs)
	{
		m_critsect.Enter();
	}

	~CriticalSectionLocker()
	{
		m_critsect.Leave();
	}

private:
	CriticalSection& m_critsect;
};

// Helper class checks that code fragment is executed by single thread 
class CriticalSectionDebugLocker
{
public:
	enum NOTIFICATION
	{
		ERROR_MESSAGE,
		DEBUG_BREAK,
		ASSERTION,
		MESSAGE_BOX
	};

public:
	CriticalSectionDebugLocker(CriticalSection& cs, 
					           const char*      message = "Non thread safe code executed simultaneously", 
					           NOTIFICATION     notification = MESSAGE_BOX)
		: m_critsect(cs)
	{
		if ( !m_critsect.TryEnter() ) 
		{
			switch(notification)
			{
			case ERROR_MESSAGE:
				std::cerr << message << std::endl;
				break;

			case DEBUG_BREAK:
				DebugBreak();
				break;

			case ASSERTION:
				assert(!message);
				break;

			case MESSAGE_BOX:
				MessageBoxA(0, message, "Error", MB_OK | MB_ICONASTERISK);
				break;
			}

			m_critsect.Enter();
		}
	}

	~CriticalSectionDebugLocker()
	{
		m_critsect.Leave();
	}

private:
	CriticalSection& m_critsect;
};

class Timer
{
public:
	void Start()	{ 	QueryPerformanceCounter( &m_StartTime ); };
	void Stop()		{ 	QueryPerformanceCounter( &m_StopTime ); };

	LARGE_INTEGER GetDiffQPC()	
	{ 
		LARGE_INTEGER diff; 
		diff.QuadPart = m_StopTime.QuadPart - m_StartTime.QuadPart;
		return diff;
	}

	float GetDiffMs()	
	{ 
		LARGE_INTEGER diff; 
		diff.QuadPart = m_StopTime.QuadPart - m_StartTime.QuadPart;
		LARGE_INTEGER freq;
		QueryPerformanceFrequency( &freq );
		return 1000.0f * diff.QuadPart / freq.QuadPart;
	}

private:
	LARGE_INTEGER m_StartTime; 
	LARGE_INTEGER m_StopTime; 
};

BOOL IsWin64();
BOOL SetUserSecurityLevel(TCHAR* fileName);

bool FileExists(const TCHAR *fileName);
void DeleteAllFiles( CONST TCHAR * dir );
void ClearDirectory( CONST TCHAR * dir );
void SetThreadName( DWORD dwThreadID, LPCSTR szThreadName);

DWORD CalculateCRC32(const void *p, UINT size, UINT startValue = 0xFFFFFFFF);

#define MODE_BEGIN	0
#define	MODE_CONT	1
#define	MODE_END	2
#define	MODE_BODY	3
#define CRC32C_INIT_REFLECTED 0xFFFFFFFF
DWORD CalculateCRC32CFast(const void* p_buf, UINT length, BYTE mode = MODE_BODY, DWORD running_crc = CRC32C_INIT_REFLECTED);

inline int RoundUp(int num, int multiple)
{
	int t = num + multiple - 1; 
	return (t / multiple)*multiple;
}

inline bool IsFocusInCurrentProcess()
{
	HWND focusWnd = GetForegroundWindow();
	if(focusWnd)
	{
		DWORD processID;
		GetWindowThreadProcessId(focusWnd, &processID);
		if(processID == GetCurrentProcessId())
			return true;
	}
	return false;
}

inline bool IsKeyPressedGlobal(int vKey)
{ 
	SHORT State = GetAsyncKeyState(vKey);
	if(State & 0x7FFF)
		return true;
	return false; 
}

inline bool IsKeyDownGlobal(int vKey)
{ 
	SHORT State = GetAsyncKeyState(vKey);
	if(State & 0xFFFF)
		return true;
	return false; 
}

inline bool IsKeyPressed(int vKey)
{
	return IsKeyPressedGlobal(vKey) && IsFocusInCurrentProcess();
}

inline bool IsKeyDown(int vKey)
{
	return IsKeyDownGlobal(vKey) && IsFocusInCurrentProcess();
}

template< typename T>
inline T clamp(const T value, const T minValue, const T maxValue) 
{
	if (value < minValue)
		return minValue;
	else if (value > maxValue)
		return maxValue;
	return value;
}

class AsyncButton
{
public:
	//--- select button ---
	//--- modofier supported only in isButtonWasPressed() function ---
	void selectButton(int vKey);

	//--- get button states ---
	bool isButtonWasPressed();
	bool isButtonWasReleased();
	bool isButtonNowDown() { return IsKeyDown(m_vKey);    }
	bool isButtonNowUp()   { return !isButtonNowDown(); }

	int getKey() { return m_vKey; };
	int getModifier() { return m_Modifier; };

protected:
	int	 m_vKey;
	int	 m_Modifier;
	//--- states MUST be separate due to anisochronous ---
	bool m_PreviousStateIsDown;	
	bool m_PreviousStateIsUp;
};

inline void AsyncButton::selectButton(int vKey)
{ 
	m_vKey		= vKey & 0xFF;
	m_Modifier	= (vKey >> 8)  & 0xFF;
	m_PreviousStateIsDown = isButtonNowDown();
	m_PreviousStateIsUp	  = !m_PreviousStateIsDown;
}

inline bool AsyncButton::isButtonWasPressed()
{
	bool result = false;
	bool buttonDown = isButtonNowDown();
	bool modifierPressed = true;
	DWORD key_CTRL  =  IsKeyDownGlobal(VK_CONTROL) ? HOTKEYF_CONTROL : 0;
	DWORD key_SHIFT =  IsKeyDownGlobal(VK_SHIFT)   ? HOTKEYF_SHIFT   : 0;
	DWORD key_ALT   =  IsKeyDownGlobal(VK_MENU)    ? HOTKEYF_ALT     : 0;
	DWORD key_WIN   = (IsKeyDownGlobal(VK_LWIN) || IsKeyDownGlobal(VK_RWIN)) ? HOTKEYF_EXT : 0;
	int modifier  = key_ALT | key_CTRL | key_SHIFT | key_WIN;
	if(m_Modifier && m_Modifier != modifier)
		modifierPressed = false;
	if(buttonDown && !m_PreviousStateIsDown && modifierPressed)
		result = true;
	m_PreviousStateIsDown = buttonDown;
	return result;
}

inline bool AsyncButton::isButtonWasReleased()
{
	bool result = false;
	bool buttonUp = !isButtonNowDown();
	if(buttonUp && !m_PreviousStateIsUp)
		result = true;
	m_PreviousStateIsUp = buttonUp;
	return result;
}

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if (p) { delete (p);     (p)=NULL; } }
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p);   (p)=NULL; } }
#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p) = NULL; } }
#endif

#ifndef SAFE_FINAL_RELEASE
#define SAFE_FINAL_RELEASE(p) { if(p) { _ASSERT((p)->Release() == 0); (p) = NULL; } }
#endif
