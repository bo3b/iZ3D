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
#include <Psapi.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <Tlhelp32.h>
#include <madCHook.h>
#include "../../CommonUtils/StringUtils.h"

#define  OGL_DLL_NAME			_T("opengl32.dll")
#define  DX8_DLL_NAME			_T("d3d8.dll")
#define  DX9_DLL_NAME			_T("d3d9.dll")
#define  DX9_VISTA_SP1_DLL_NAME _T("d3d9VistaNoSP1.dll")
#define  WOW_DLL_NAME			_T("wow64.dll")

using namespace std;
HMODULE g_hModule = NULL;

#pragma comment(linker, "/SECTION:.shared,RWS")
#pragma data_seg(".shared")
HHOOK				g_hMouseHook = NULL;
vector<CHAR*>		g_IncludedProcess;
vector<string>		g_ExcludedProcess;
HANDLE				g_hFile = NULL;
DWORD*				g_pVar = NULL;
#pragma data_seg()

string processName;

void BuildExcludedProcessList(vector<string>& included3DProcess);

bool MouseClippedByDriver()
{
	if (g_pVar && *g_pVar)
	{    
		HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |	PROCESS_VM_READ, FALSE, *g_pVar );
		if (hProcess)
		{
			CloseHandle( hProcess );
			return true;
		}
		else
		{
			*g_pVar = NULL;
			return false;
		}
	}
	else 
		return false;
}

bool CanClipProcess()
{
	processName.clear();
	bool result = true, excludeProcess = false;
	HWND focusWnd = GetForegroundWindow();
	if(focusWnd)
	{
		DWORD processID;
		GetWindowThreadProcessId(focusWnd, &processID);

		MODULEENTRY32  me = { sizeof(MODULEENTRY32) };
		HANDLE CTHM = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processID);
		if(CTHM != INVALID_HANDLE_VALUE)
		{
			BOOL res = Module32First(CTHM, &me); //--- get process module name ---
			if(res)
			{
				processName = common::utils::to_multibyte(me.szModule);

				if (!MouseClippedByDriver())
				{
					//--- optimization: reduce searching in desktop mode ---
					for(DWORD i = 0; i< g_IncludedProcess.size(); i++)
						if(_stricmp(processName.c_str(), g_IncludedProcess[i]) == 0)
						{
							res = false;  //--- stop search, clip this process ---
							break;
						}

						if(res)
						{
							for(DWORD i = 0; i< g_ExcludedProcess.size(); i++)
								if(_stricmp(processName.c_str(), g_ExcludedProcess[i].c_str()) == 0)
								{
									//--- clip this process only if 3D library will be found ---
									excludeProcess = true;
									break;
								}
						}
				}
				else
					excludeProcess = true;
			}
			if(res)
			{
				if(excludeProcess)
					result = false; //--- do not clip this process ---
			}
			CloseHandle(CTHM);
		}
		else
			result = false;	//--- always then 32-bit hook in 64-bit app.  do not clip! ---
	}
	return result;  
}

bool ClipCursorByRect(RECT* rt, POINT* pt)
{
	bool clipStatus = false;
	if(pt->x > rt->right)		{ pt->x = rt->right;	clipStatus = true; }
	else if (pt->x < rt->left)  { pt->x = rt->left;		clipStatus = true; }
	if(pt->y > rt->bottom)		{ pt->y = rt->bottom;	clipStatus = true; }
	else if (pt->y < rt->top)   { pt->y = rt->top;		clipStatus = true; }

	if(clipStatus)
		return CanClipProcess();

	return clipStatus;
}

void RepositionMouse(long x, long y)
{
	INPUT inp;
	inp.type = INPUT_MOUSE;
	memset(&inp.mi, 0, sizeof(MOUSEINPUT));
	inp.mi.dx = x;
	inp.mi.dy = y;
	inp.mi.dwFlags = MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_MOVE; 
	SendInput(1, &inp, sizeof(inp));
}

LRESULT CALLBACK MouseProcLL(int nCode, WPARAM wParam, LPARAM lParam)
{
	if(nCode >= 0 && wParam == WM_MOUSEMOVE)
	{
		POINT pt = ((MSLLHOOKSTRUCT*)lParam)->pt;
		RECT  rt;
		SetRect(&rt, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));

#ifdef _DEBUG
		TCHAR s[MAX_PATH];
		HDC dc = GetDC(0);
		#ifndef WIN64
			int y = 0;
			_stprintf_s(s, MAX_PATH, _T("win32:  x = %i (0..%i), y = %i (0..%i)   "), pt.x, rt.right, pt.y, rt.bottom);
		#else 
			int y = 16;
			_stprintf_s(s, MAX_PATH, _T("win64:  x = %i (0..%i), y = %i (0..%i)   "), pt.x, rt.right, pt.y, rt.bottom);
		#endif 
		TextOut(dc, -350, y, s, (int)_tcslen(s));
		ReleaseDC(0, dc);
#endif

		if(ClipCursorByRect(&rt, &pt))
		{		

#ifdef _DEBUG
			_stprintf_s(s, MAX_PATH, _T("clipped"));
			TextOut(dc, -450, y, s, (int)_tcslen(s));
			ReleaseDC(0, dc);
#endif	

			double scaleX = 65535. / rt.right;
			double scaleY = 65535. / rt.bottom;
			RepositionMouse((LONG)(pt.x * scaleX), (LONG)(pt.y * scaleY));
			return 1;
		}
		else
		{
#ifdef _DEBUG
			_stprintf_s(s, MAX_PATH,  _T("               "));
			TextOut(dc, -450, y, s, (int)_tcslen(s));
			ReleaseDC(0, dc);
#endif	
		}
	}
	return CallNextHookEx(g_hMouseHook, nCode, wParam, lParam);
}	

__declspec(dllexport) bool CALLBACK S3DSetLock(bool lock)
{
	if(lock)
	{
		if(g_hMouseHook == NULL)
		{
			g_hFile = OpenGlobalFileMapping("iZ3DMouseLocked", TRUE);
			if (!g_hFile)
				g_hFile = CreateGlobalFileMapping("iZ3DMouseLocked", 1);
			g_pVar = (DWORD*) MapViewOfFile(g_hFile, FILE_MAP_ALL_ACCESS, 0, 0, 1);

			//--- initialize excluded process list ---
			//excludedProcess.push_back("Bioshock.exe");
			//excludedProcess.push_back("BF2.exe");
			g_IncludedProcess.push_back("explorer.exe");
			g_IncludedProcess.push_back("totalcmd.exe");
			BuildExcludedProcessList(g_ExcludedProcess);

			//--- set mouse hook ---
			RECT  rt;
			POINT pt;
			SetRect(&rt, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
			GetCursorPos(&pt);
			if(ClipCursorByRect(&rt, &pt))
			{
				float scaleX = 65535.f / rt.right;
				float scaleY = 65535.f / rt.bottom;
				RepositionMouse((LONG)(pt.x * scaleX), (LONG)(pt.y * scaleY));
			}
			g_hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseProcLL, (HINSTANCE)g_hModule, 0);
		}
		return g_hMouseHook != NULL;
	}
	else
	{
		if(g_hMouseHook)
		{
			UnhookWindowsHookEx(g_hMouseHook);
			UnmapViewOfFile(g_pVar);
			CloseHandle(g_hFile);
			g_hMouseHook = NULL;
		}
		return true;
	}
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hModule);
		_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
		//_CrtSetBreakAlloc(82291);
		g_hModule = hModule;
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		_RPT0(0, "S3DMouseLock memory leaks:\n");
		break;
	}
	return TRUE;
}
