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
#include <algorithm>
#include <tchar.h>
#include <boost/unordered_map.hpp>
#include "Trace.h"
#include "Renderer.h"
#include <madCHook.h>

#define HOOKING_FLAG 0

PFNGDIRELEASEDC pfnOrig_ReleaseDC = NULL;
PFNWGLCREATECONTEX pfnOrig_wglCreateContext = NULL;
PFNWGLDELETECONTEX pfnOrig_wglDeleteContext = NULL;
PFNWGLGETCURRENTDC pfnOrig_wglGetCurrentDC = NULL;
PFNWGLGETCURRENTCONTEX pfnOrig_wglGetCurrentContext = NULL;
PFNWGLMAKECURRENT pfnOrig_wglMakeCurrent = NULL;
PFNWGLVIEWPORT pfnOrig_glViewport = NULL;
PFNWGLSCISSOR pfnOrig_glScissor = NULL;
PFNWGLSWAPBUFFERS pfnOrig_wglSwapBuffers = NULL;
PFNWGLDRAWBUFFER pfnOrig_glDrawBuffer = NULL;
PFNWGLREADBUFFER pfnOrig_glReadBuffer = NULL;
PFNWGLCHOOSEPIXELFORMAT pfnOrig_wglChoosePixelFormat = NULL;
PFNWGLDESCRIBEPIXELFORMAT pfnOrig_wglDescribePixelFormat = NULL;
PFNGLGETBOOLEANV pfnOrig_glGetBooleanv = NULL;
PFNWGLGETPIXELFORMAT pfnOrig_wglGetPixelFormat = NULL;
PFNWGLSETPIXELFORMAT pfnOrig_wglSetPixelFormat = NULL;
PFNWGLAPICALL pfnOrig_wglGetProcAddress = NULL;

typedef boost::unordered_map<HDC, bool> DCMap_t;
static DCMap_t g_StereoPixelFormat;

static Renderer *GetRenderer(HDC hdc)
{
	if (hdc == 0)
		return NULL;
	
	size_t nCount = g_RendererList.size();
	for (ULONG i = 0; i < nCount; i++)
	{
		if (g_RendererList[i].IsDCMatched(hdc))
		{
			Renderer *pRenderer = &g_RendererList[i];
			return pRenderer;
		}
	}
	
	if (GINFO_ENABLED)
	{
		bool StereoOn = false;
		DCMap_t::iterator iterDC = g_StereoPixelFormat.find(hdc);
		if (iterDC != g_StereoPixelFormat.end())
		{
			StereoOn = iterDC->second;
		}
		if(StereoOn)
		{
			DEBUG_TRACE1("Create new Renderer(hdc = 0x%X)\n", hdc);
			g_RendererList.push_back(Renderer());
			Renderer* pRenderer = &g_RendererList.back();
			BOOL bResult = pRenderer->SetApplicationDC(hdc);
			if (bResult)
				return pRenderer;
			else
			{
				DEBUG_MESSAGE("SetApplicationDC(hdc = 0x%X) returns FALSE\n", hdc);
				g_RendererList.pop_back();
				return NULL;
			}
		}
		else
			return NULL;
	} else
		return NULL;
}

BOOL WINAPI HwglSwapBuffers(HDC hdc)
{
	//gInfo.Trace = (gInfo.Debug && GetAsyncKeyState(VK_SCROLL) == 0x8001);
	DEBUG_TRACE2("wglSwapBuffers\n");
	Renderer *pRenderer = GetRenderer(hdc);
	if (pRenderer)
		return pRenderer->SwapBuffers();
	else
		return pfnOrig_wglSwapBuffers(hdc);
}

void WINAPI HglDrawBuffer(GLenum mode)
{
	DEBUG_TRACE2("glDrawBuffer(mode = %s)\n", GetDrawBufferFlagsString(mode));
#ifdef _DEBUG
	static std::string desc;
	desc = GetDrawBufferFlagsString(mode);
#endif
	HDC hdc = pfnOrig_wglGetCurrentDC();
	Renderer *pRenderer = GetRenderer(hdc);
	if (pRenderer)
		pRenderer->DrawBuffer(mode);
	else
		pfnOrig_glDrawBuffer (mode);
}

BOOL WINAPI HwglMakeCurrent(HDC hdc, HGLRC hglrc)
{
	Renderer *pRenderer = GetRenderer(hdc);
	BOOL bResult;
	DEBUG_TRACE1("wglMakeCurrent(hdc = 0x%X, hglrc = 0x%X)\n", hdc, hglrc);
	if (pRenderer)
		bResult = pRenderer->MakeCurrent(hglrc);
	else
		bResult = pfnOrig_wglMakeCurrent (hdc, hglrc);
	return bResult;
}

HGLRC WINAPI HwglCreateContext(HDC hdc)
{
	HGLRC hContext = pfnOrig_wglCreateContext(hdc);
	DEBUG_TRACE1("wglCreateContext(hdc = 0x%X) returns 0x%X\n", hdc, hContext);
	return hContext;
}

BOOL WINAPI HwglDeleteContext(HGLRC hrc)
{
	BOOL bResult = pfnOrig_wglDeleteContext(hrc);
	DEBUG_TRACE1("wglDeleteContext(hrc = 0x%X) returns %d\n", hrc, bResult);
	return bResult;
}

int WINAPI HReleaseDC(HWND hWnd, HDC hDC)
{
	size_t nCount = g_RendererList.size();
	for (std::vector<Renderer>::iterator it = g_RendererList.begin(); it != g_RendererList.end(); it++)
	{
		if (it->IsDCMatched(hDC))
		{
			g_RendererList.erase(it);
		}
	}
	int nRes = pfnOrig_ReleaseDC(hWnd, hDC);
	DEBUG_TRACE1("ReleaseDC(hWnd = 0x%X, hDC = 0x%X) returns 0x%X\n", hWnd, hDC, nRes);
	return nRes;
}

HDC WINAPI HwglGetCurrentDC(VOID)
{
	HDC hDC = pfnOrig_wglGetCurrentDC();
	Renderer *pRenderer = GetRenderer(hDC);
	if (pRenderer && pRenderer->m_bGlInit)
	{
		hDC = pRenderer->GetApplicationDC();
	}
	DEBUG_TRACE2("wglGetCurrentDC() returns 0x%X\n", hDC);
	return hDC;
}

HGLRC WINAPI HwglGetCurrentContext(VOID)
{
	HGLRC hContext = pfnOrig_wglGetCurrentContext();
	HDC hDC = pfnOrig_wglGetCurrentDC();
	Renderer *pRenderer = GetRenderer(hDC);
	if (pRenderer && pRenderer->m_bGlInit)
	{
		hContext = pRenderer->GetApplicationContext();
	}
	DEBUG_TRACE2("wglGetCurrentContext() returns 0x%X\n", hContext);
	return hContext;
}

void WINAPI HglReadBuffer (GLenum mode)
{
#ifdef _DEBUG
	static std::string desc;
	desc = GetDrawBufferFlagsString(mode);
#endif
	pfnOrig_glReadBuffer(mode);
	DEBUG_TRACE1("glReadBuffer(mode = %s)\n", GetDrawBufferFlagsString(mode));
}

void WINAPI HglViewport (GLint x, GLint y, GLsizei width, GLsizei height)
{
	pfnOrig_glViewport (x, y, width, height);
	DEBUG_TRACE3("glViewport (x = %d, y = %d, width = %d, height = %d)\n", 
		x, y, width, height);
}

void WINAPI HglScissor (GLint x, GLint y, GLsizei width, GLsizei height)
{
	pfnOrig_glScissor (x, y, width, height);
	DEBUG_TRACE3("glScissor (x = %d, y = %d, width = %d, height = %d)\n", 
		x, y, width, height);
}

void WINAPI HglGetBooleanv(GLenum pname, GLboolean *params)
{
	pfnOrig_glGetBooleanv(pname, params);
	DEBUG_TRACE2("glGetBooleanv(pname = %d, *params = %d)\n", 
		pname, *params);
	if (GINFO_ENABLED && pname == GL_STEREO)
	{
		*params = TRUE;
	}
}

int WINAPI HwglChoosePixelFormat(HDC hdc, CONST PIXELFORMATDESCRIPTOR *pfd)
{
	int iPixelFormat = pfnOrig_wglChoosePixelFormat(hdc, pfd);
	if (pfd)
	{
		DEBUG_TRACE1("wglChoosePixelFormat(hdc = 0x%X, pfd->dwFlags = %s) returns %d\n", 
			hdc, GetPixelFormatDescriptorFlagsString(pfd->dwFlags), iPixelFormat);
	}
	if (iPixelFormat == 0)
	{
		DEBUG_TRACE1("GetLastErrorString() returns %s\n", GetLastErrorString());
	}
	return iPixelFormat;
}

BOOL WINAPI HwglSetPixelFormat(HDC hdc, int iPixelFormat, CONST PIXELFORMATDESCRIPTOR *pfd)
{
	bool bStereoOn = false;
	PIXELFORMATDESCRIPTOR new_pfd;
	if (GINFO_ENABLED)
	{
		if (pfd && ((pfd->dwFlags & PFD_STEREO) == PFD_STEREO))
		{
			bStereoOn = true;
			if (gInfo.EmulateQB)
			{
				new_pfd = *pfd;
				pfd = &new_pfd;
				new_pfd.dwFlags &= ~PFD_STEREO; 
			}
		} 
	}
	BOOL bResult = pfnOrig_wglSetPixelFormat(hdc, iPixelFormat, pfd);
	if (pfd)
	{
		DEBUG_TRACE1("wglSetPixelFormat(hdc = 0x%X, iPixelFormat = %d, pfd->dwFlags = %s) returns %d\n", 
			hdc, iPixelFormat, GetPixelFormatDescriptorFlagsString(pfd->dwFlags), bResult);
	}
	if (bStereoOn)
	{
		if (!gInfo.EmulateQB)
		{
			pfnOrig_wglDescribePixelFormat(hdc, iPixelFormat, 
				sizeof(PIXELFORMATDESCRIPTOR), &new_pfd); 
			bStereoOn = (new_pfd.dwFlags & PFD_STEREO) == PFD_STEREO;
		}
		if (bStereoOn)
		{
			g_StereoPixelFormat[hdc] = TRUE;
			DEBUG_TRACE1("g_iStereoPixelFormat = %d\n", iPixelFormat);
		}
	}
	if (bResult == 0)
	{
		DEBUG_TRACE1("GetLastErrorString() returns %s\n", GetLastErrorString());
	}
	return bResult;
}

int WINAPI HwglDescribePixelFormat(HDC hdc, int iPixelFormat, UINT nBytes, LPPIXELFORMATDESCRIPTOR ppfd)
{
	int iMaxPixelFormat = pfnOrig_wglDescribePixelFormat(hdc, iPixelFormat, nBytes, ppfd);
	if (ppfd)
	{
		DEBUG_TRACE2("wglDescribePixelFormat(hdc = 0x%X, iPixelFormat = %d, nBytes = %u, pfd->dwFlags = %s) returns %d\n", 
			hdc, iPixelFormat, nBytes, GetPixelFormatDescriptorFlagsString(ppfd->dwFlags), iMaxPixelFormat);
	}
	if (iMaxPixelFormat == 0)
	{
		DEBUG_TRACE2("GetLastErrorString() returns %s\n", GetLastErrorString());
	}
	if (ppfd && (ppfd->dwFlags & PFD_SUPPORT_OPENGL) && GINFO_ENABLED && gInfo.EmulateQB)
	{
		ppfd->dwFlags |= PFD_STEREO;
		DEBUG_TRACE2("\tnew pfd->dwFlags = %s\n", GetPixelFormatDescriptorFlagsString(ppfd->dwFlags));
	}
	return iMaxPixelFormat;
}

int WINAPI HwglGetPixelFormat(HDC hdc)
{
	int iPixelFormat = pfnOrig_wglGetPixelFormat(hdc);
	DEBUG_TRACE3("wglGetPixelFormat(hdc = 0x%X) returns %d\n", hdc, iPixelFormat);
	if (iPixelFormat == 0)
	{
		DEBUG_TRACE1("GetLastErrorString() returns %s\n", GetLastErrorString());
	}
	return iPixelFormat;
}

PROC WINAPI HwglGetProcAddress(LPCSTR lpszProcName)
{
	PROC proc = pfnOrig_wglGetProcAddress(lpszProcName);
	DEBUG_TRACE2("wglGetProcAddress(lpszProcName = %s) returns 0x%X\n", lpszProcName, proc);
	return proc;
}

BOOL WINAPI HookOGL()
{
	CollectHooks();			
	HookAPI(("gdi32.dll"), "ReleaseDC", HReleaseDC, (PVOID*) &pfnOrig_ReleaseDC, HOOKING_FLAG);
	HookAPI(("opengl32.dll"), "glGetBooleanv", HglGetBooleanv, (PVOID*) &pfnOrig_glGetBooleanv, HOOKING_FLAG);
	HookAPI(("opengl32.dll"), "wglChoosePixelFormat", HwglChoosePixelFormat, (PVOID*) &pfnOrig_wglChoosePixelFormat, HOOKING_FLAG);
	HookAPI(("opengl32.dll"), "glViewport", HglViewport, (PVOID*) &pfnOrig_glViewport, HOOKING_FLAG);
	HookAPI(("opengl32.dll"), "glScissor", HglScissor, (PVOID*) &pfnOrig_glScissor, HOOKING_FLAG);
	HookAPI(("opengl32.dll"), "glDrawBuffer", HglDrawBuffer, (PVOID*) &pfnOrig_glDrawBuffer, HOOKING_FLAG);
	HookAPI(("opengl32.dll"), "glReadBuffer", HglReadBuffer, (PVOID*) &pfnOrig_glReadBuffer, HOOKING_FLAG);
	HookAPI(("opengl32.dll"), "wglCreateContext", HwglCreateContext, (PVOID*) &pfnOrig_wglCreateContext, HOOKING_FLAG);
	HookAPI(("opengl32.dll"), "wglDeleteContext", HwglDeleteContext, (PVOID*) &pfnOrig_wglDeleteContext, HOOKING_FLAG);
	HookAPI(("opengl32.dll"), "wglGetCurrentContext", HwglGetCurrentContext, (PVOID*) &pfnOrig_wglGetCurrentContext, HOOKING_FLAG);
	HookAPI(("opengl32.dll"), "wglGetCurrentDC", HwglGetCurrentDC, (PVOID*) &pfnOrig_wglGetCurrentDC, HOOKING_FLAG);
	HookAPI(("opengl32.dll"), "wglMakeCurrent", HwglMakeCurrent, (PVOID*) &pfnOrig_wglMakeCurrent, HOOKING_FLAG);
	HookAPI(("opengl32.dll"), "wglSwapBuffers", HwglSwapBuffers, (PVOID*) &pfnOrig_wglSwapBuffers, HOOKING_FLAG);
	//HookAPI(_M("opengl32.dll"), "wglSwapLayerBuffers", HwglSwapLayerBuffers, (PVOID*) &pfnOrig_wglSwapLayerBuffers, HOOKING_FLAG);
	//HookAPI(_M("opengl32.dll"), "wglSwapMultipleBuffers", HwglSwapMultipleBuffers, (PVOID*) &pfnOrig_wglSwapMultipleBuffers, HOOKING_FLAG);
	HookAPI(("opengl32.dll"), "wglDescribePixelFormat", HwglDescribePixelFormat, (PVOID*) &pfnOrig_wglDescribePixelFormat, HOOKING_FLAG);
	HookAPI(("opengl32.dll"), "wglGetPixelFormat", HwglGetPixelFormat, (PVOID*) &pfnOrig_wglGetPixelFormat, HOOKING_FLAG);
	HookAPI(("opengl32.dll"), "wglSetPixelFormat", HwglSetPixelFormat, (PVOID*) &pfnOrig_wglSetPixelFormat, HOOKING_FLAG);
	HookAPI(("opengl32.dll"), "wglGetProcAddress", HwglGetProcAddress, (PVOID*) &pfnOrig_wglGetProcAddress, HOOKING_FLAG);
	FlushHooks();
	return TRUE;
}
