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
#define GLH_EXT_SINGLE_FILE
#include <glh/glh_extensions.h>
#include "Renderer.h"
#include "Trace.h"
#include "Shaders.h"
#include "tchar.h"
#include "ProductNames.h"

std::vector<Renderer>	g_RendererList;
static HWND	m_hFrontWnd;

Renderer::Renderer(void)
{
	ZeroMemory((void *)this, sizeof(*this));
	m_nDrawBufferMode[0] = GL_BACK_LEFT;
	m_nDrawBufferMode[1] = GL_BACK_RIGHT;
	m_TextureID[0] = 0;
	m_TextureID[1] = 0;
	m_hBackBufferContext = 0;
	m_hVS = NULL;
	m_hFSBackScreen = NULL;
	m_hPOFrontScreen = NULL;
	m_hFSFrontScreen = NULL;
	m_hPOBackScreen = NULL;
	m_hFrontDC = NULL;
	m_hFrontWnd = NULL;
	m_SwapBuffersCount = 0;
}

Renderer::~Renderer(void)
{
	//DestroyPB();
	if (m_TextureID[0])
	{
		glDeleteTextures(1, &m_TextureID[0]);
		m_TextureID[0] = 0;
	}
	if (m_TextureID[1])
	{
		glDeleteTextures(1, &m_TextureID[1]);
		m_TextureID[1] = 0;
	}
	if (m_hBackBufferContext)
	{
		wglDeleteContext(m_hBackBufferContext);
		m_hBackBufferContext = 0;
	}
	// Sometimes fail
	//if (glDeleteObjectARB)
	//{
	//	if (m_hVS)
	//	{
	//		glDeleteObjectARB(m_hVS);
	//		m_hVS = NULL;
	//	}
	//	if (m_hFSBackScreen)
	//	{
	//		glDeleteObjectARB(m_hFSBackScreen);
	//		m_hFSBackScreen = NULL;
	//	}
	//	if (m_hPOFrontScreen)
	//	{
	//		glDeleteObjectARB(m_hPOFrontScreen);
	//		m_hPOFrontScreen = NULL;
	//	}
	//	if (m_hFSFrontScreen)
	//	{
	//		glDeleteObjectARB(m_hFSFrontScreen);
	//		m_hFSFrontScreen = NULL;
	//	}
	//	if (m_hPOBackScreen)
	//	{
	//		glDeleteObjectARB(m_hPOBackScreen);
	//		m_hPOBackScreen = NULL;
	//	}
	//}

	if(m_hFrontDC)
		ReleaseDC(m_hFrontWnd, m_hFrontDC);
	if (m_hFrontWnd)
		DestroyWindow(m_hFrontWnd);
}

BOOL Renderer::SetApplicationDC(HDC ApplicationDC) 
{ 
	DEBUG_TRACE1("%s(ApplicationDC = 0x%X)\n", _T( __FUNCTION__ ) , ApplicationDC);
	if (ApplicationDC == 0)
	{
		return TRUE;
	}
	m_hApplicationDC = ApplicationDC; 
	m_hWnd = WindowFromDC(ApplicationDC);
	DEBUG_TRACE1("WindowFromDC(ApplicationDC = 0x%X) returns m_hWnd = 0x%X\n", 
		m_hApplicationDC, m_hWnd);
	RECT Rect = { 0, 0, 0, 0 };
	BOOL bResult = GetClientRect(m_hWnd, &Rect); 
	DEBUG_TRACE1("GetClientRect(hWnd = 0x%X, Rect(%d, %d, %d, %d)) returns %d\n", 
		m_hWnd, Rect.left, Rect.top, Rect.right, Rect.bottom, bResult);
	m_nWindowWidth = Rect.right - Rect.left;
	m_nWindowHeight = Rect.bottom - Rect.top;
	DEBUG_TRACE1("m_nWindowWidth = %d, m_nWindowHeight = %d\n", m_nWindowWidth, m_nWindowHeight);
	HMONITOR hMonitor = MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
	MONITORINFO mi;
	mi.cbSize = sizeof(MONITORINFO);
	bResult = GetMonitorInfo(hMonitor, &mi);
	m_nMonitorWidth = mi.rcMonitor.right - mi.rcMonitor.left;
	m_nMonitorHeight = mi.rcMonitor.bottom - mi.rcMonitor.top;

	return TRUE;
}

void	Renderer::DrawBuffer(GLenum mode)
{
	BOOL bStereo = (mode == GL_FRONT_LEFT || mode == GL_FRONT_RIGHT ||	mode == GL_BACK_LEFT || mode == GL_BACK_RIGHT || mode == GL_LEFT || mode == GL_RIGHT);
	DEBUG_TRACE2("\tStereo(on = %d)\n", bStereo);
	SetStereoRender(bStereo);
	if (!bStereo)
	{
		if (mode != GL_FRONT)
			mode = GL_BACK;
		m_MonoBuffer = (mode == GL_FRONT ? WGL_FRONT_LEFT_ARB : WGL_BACK_LEFT_ARB);
		pfnOrig_glDrawBuffer(mode);
		return;
	}
	HGLRC hCurrentContext = pfnOrig_wglGetCurrentContext();
	if (hCurrentContext != m_hPBufferContext)
		m_hApplicationContext = hCurrentContext;
	UINT nIndex = (mode == GL_FRONT_RIGHT || mode == GL_BACK_RIGHT || mode == GL_RIGHT) ? 1 : 0;

	m_bSwapBuffersCalled = FALSE;
	m_bStereoRender = TRUE;
	m_nDrawBufferMode[nIndex] = mode;

	if (nIndex == 0)
	{
		if (!Create())
		{
			DEBUG_MESSAGE("Error: Create() returns FALSE\n");
			return;
		}
	}
	pfnOrig_wglMakeCurrent(m_hPBufferDC, m_hPBufferContext);
	pfnOrig_glDrawBuffer(nIndex == 0 ? GL_FRONT : GL_BACK);
	//wglCopyContext(hCurrentContext, m_hPBufferContext, GL_ALL_ATTRIB_BITS);
}

BOOL    Renderer::MakeCurrent(HGLRC hglrc)
{
	m_hApplicationContext = hglrc;
	BOOL bResult;
	if (!m_bGlInit)
		bResult = pfnOrig_wglMakeCurrent (m_hApplicationDC, m_hApplicationContext);
	if (Create()/* && GetStereoRender()*/)
	{
		bResult = pfnOrig_wglMakeCurrent(m_hPBufferDC, m_hPBufferContext);
		DEBUG_TRACE1("%s(): wglMakeCurrent(m_hPBufferDC = 0x%X, m_hPBufferContext = 0x%X) = %d\n", 
			_T( __FUNCTION__ ) , m_hPBufferDC, m_hPBufferContext, bResult);
	}
	else
	  bResult = pfnOrig_wglMakeCurrent (m_hApplicationDC, hglrc);
	return bResult;
}

inline void	Renderer::SetStereoRender(BOOL bStereoRender) 
{ 
	if (m_bStereoRender != bStereoRender)
	{
		//if (bStereoRender)
		//	pfnOrig_wglMakeCurrent(m_hPBufferDC, m_hPBufferContext);
		//else
		//	pfnOrig_wglMakeCurrent (m_hApplicationDC, m_hApplicationContext);
		m_bStereoRender = bStereoRender; 
	}
}

BOOL	Renderer::Create()
{
	if (!m_bGlInit)
	{
		if (glh_init_extension("GL_VERSION_1_5") && glh_init_extension("GL_ARB_shader_objects") && glh_init_extension("WGL_ARB_pbuffer") && 
			glh_init_extension("WGL_ARB_render_texture") && glh_init_extension("WGL_ARB_pixel_format") && glh_init_extension("GL_ARB_multitexture"))
		{
			DEBUG_MESSAGE("Status: Using GLH\n");
			m_bGlInit = true;
		}
		else
		{
			DEBUG_MESSAGE("Error\n");
		}
		const char *extension = (const char*)glGetString(GL_EXTENSIONS);
		DEBUG_TRACE1("glGetString(GL_EXTENSIONS) returns \n%s\n", extension); 
	}

	static RECT Rect = { 0, 0, 0, 0 };
	BOOL bResult = GetClientRect(m_hWnd, &Rect); 
	if (Rect.right != m_nWindowWidth || Rect.bottom != m_nWindowHeight)
	{
		DEBUG_TRACE1("GetClientRect(hWnd = 0x%X, Rect(%d, %d, %d, %d)) returns %d\n", 
			m_hWnd, Rect.left, Rect.top, Rect.right, Rect.bottom, bResult);
		m_nWindowWidth = Rect.right - Rect.left;
		m_nWindowHeight = Rect.bottom - Rect.top;
		DEBUG_TRACE1("Window size changed\n");
		DEBUG_TRACE1("m_nWindowWidth = %d, m_nWindowHeight = %d\n", m_nWindowWidth, m_nWindowHeight);
		if (m_nWindowWidth > m_nPBufferWidth || m_nWindowHeight > m_nPBufferHeight)
			m_bCreated = 0;
	}

	if (m_bCreated)
	{
		int flag = 0;
		wglQueryPbufferARB(m_hPBuffer, WGL_PBUFFER_LOST_ARB, &flag);
		if (flag != 0)
		{
			m_bCreated = FALSE;
		}
	}
	if (!m_bCreated)
	{
		BOOL bResult = CreatePB();
		if (!bResult)
		{
			DEBUG_MESSAGE("Error: CreatePB() returns FALSE\n");
			return FALSE;
		}
		else if (gInfo.EmulateQB)
		{
			if (!m_bFrontWindowCreated && !CreateFrontWindow())
			{
				DEBUG_MESSAGE("Error: CreateFrontWindow() returns FALSE\n");
			}
		}
	}
	return TRUE;
}

BOOL	Renderer::CreatePB()
{
	DEBUG_TRACE1("%s()\n", _T( __FUNCTION__ ) );
	DestroyPB();

	PIXELFORMATDESCRIPTOR  pfd;
	int  iPixelFormat = pfnOrig_wglGetPixelFormat(m_hApplicationDC); 
	pfnOrig_wglDescribePixelFormat(m_hApplicationDC, iPixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd); 
	DEBUG_TRACE1("%s\n", GetPixelFormatDescriptorString(&pfd));
	int attr[] =
	{
		WGL_SUPPORT_OPENGL_ARB, TRUE,
		WGL_DRAW_TO_PBUFFER_ARB, TRUE,
		WGL_BIND_TO_TEXTURE_RGBA_ARB, TRUE,
		WGL_DRAW_TO_WINDOW_ARB, TRUE,
		WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
//		WGL_PIXEL_TYPE_ARB, pfd.iPixelType,
		WGL_COLOR_BITS_ARB, pfd.cColorBits,
		WGL_RED_BITS_ARB, pfd.cRedBits,
//		WGL_RED_SHIFT_ARB, pfd.cRedShift,
		WGL_GREEN_BITS_ARB, pfd.cGreenBits,
//		WGL_GREEN_SHIFT_ARB, pfd.cGreenShift,
		WGL_BLUE_BITS_ARB, pfd.cBlueBits,
//		WGL_BLUE_SHIFT_ARB, pfd.cBlueShift,
		WGL_ALPHA_BITS_ARB, pfd.cAlphaBits,
//		WGL_ALPHA_SHIFT_ARB, pfd.cAlphaShift,
		WGL_DEPTH_BITS_ARB, pfd.cDepthBits,
		WGL_STENCIL_BITS_ARB, pfd.cStencilBits,
		WGL_ACCUM_BITS_ARB, pfd.cAccumBits,
		WGL_ACCUM_RED_BITS_ARB, pfd.cAccumRedBits,
		WGL_ACCUM_GREEN_BITS_ARB, pfd.cAccumGreenBits,
		WGL_ACCUM_BLUE_BITS_ARB, pfd.cAccumBlueBits,
		WGL_ACCUM_ALPHA_BITS_ARB, pfd.cAccumAlphaBits,
		WGL_DOUBLE_BUFFER_ARB, TRUE,
//		WGL_STEREO_ARB, TRUE,
		0
	};
	unsigned int count = 0;
	int pixelFormat;
	wglChoosePixelFormatARB(m_hApplicationDC, (const int *)attr, NULL, 1, &pixelFormat, &count);
	if (count == 0)
	{
		DEBUG_TRACE1("P-buffer Error: Unable to find an acceptable pixel format");
		return FALSE;
	}
	int pAttrib[] =
	{
		WGL_TEXTURE_FORMAT_ARB,
		WGL_TEXTURE_RGBA_ARB,
		WGL_TEXTURE_TARGET_ARB,
		WGL_TEXTURE_2D_ARB,
		WGL_PBUFFER_LARGEST_ARB,
		TRUE,
		0
	};

	if (m_nWindowWidth == 0 || m_nWindowHeight == 0)
	{
		return FALSE;
	}
	UINT nHeight = FindPow2(max(m_nWindowHeight, m_nMonitorHeight));
	UINT nWidth = FindPow2(max(m_nWindowWidth, m_nMonitorWidth));
	m_hPBuffer = wglCreatePbufferARB(m_hApplicationDC, pixelFormat, 
		nWidth, nHeight, pAttrib);
	DEBUG_TRACE1("wglCreatePbufferARB(m_hApplicationDC = 0x%X) "
		"returns m_hPBuffer = 0x%X\n", m_hApplicationDC, m_hPBuffer);
	if (m_hPBuffer == 0)
	{
		DEBUG_TRACE1("P-buffer Error: Unable to create P-buffer");
		return FALSE;
	}
	m_hPBufferDC = wglGetPbufferDCARB(m_hPBuffer);
	DEBUG_TRACE1("wglGetPbufferDCARB(m_hPBuffer = 0x%X) "
		"returns m_hPBufferDC = 0x%X\n", m_hPBuffer, m_hPBufferDC);

	wglQueryPbufferARB(m_hPBuffer, WGL_PBUFFER_WIDTH_ARB, (int *)&m_nPBufferWidth);
	wglQueryPbufferARB(m_hPBuffer, WGL_PBUFFER_HEIGHT_ARB, (int *)&m_nPBufferHeight);
	DEBUG_TRACE1("m_nPBufferWidth = %d, m_nPBufferHeight = %d\n", 
		m_nPBufferWidth, m_nPBufferHeight);
	m_bCreated = TRUE;

	HGLRC hPBufferContext = m_hPBufferContext;
	m_hPBufferContext = pfnOrig_wglCreateContext(m_hPBufferDC);
	if (hPBufferContext != 0)
	{
		wglDeleteContext(hPBufferContext);
	}

	DEBUG_TRACE1("%s() returns TRUE\n", _T( __FUNCTION__ ) );
	return TRUE;
}

void	Renderer::DestroyPB()
{
	if (m_hPBuffer != 0)
	{
		if (m_hPBufferDC != 0)
		{    
			wglReleasePbufferDCARB(m_hPBuffer, m_hPBufferDC);
			m_hPBufferDC = 0;
		}
		wglDestroyPbufferARB(m_hPBuffer);
		m_hPBuffer = 0;
	}
}

BOOL	Renderer::CheckGLError()
{
	GLenum err = glGetError();
	if (err != GL_NO_ERROR)
	{   
#ifdef _DEBUG
		DEBUG_MESSAGE("OpenGL error\n");
#endif
		return FALSE;
	}  
	return TRUE;
}

inline BOOL	Renderer::CheckGLSLError(GLhandleARB Handle, GLenum Param)
{
	if (!CheckGLError())
		return FALSE;
	GLint glsl_ok;
	glGetObjectParameterivARB(Handle, Param, &glsl_ok);
	char s[1000];
	GLint l;
	glGetInfoLogARB(Handle, _countof(s), &l, s);
	DEBUG_MESSAGE("\t%s", s);
	return glsl_ok == 0;
}

BOOL	Renderer::InitializeShaders()
{
	m_bShadersInitialized = TRUE;
	HDC hCurrentDC = pfnOrig_wglGetCurrentDC();
	HGLRC hCurrentContext = pfnOrig_wglGetCurrentContext();
	_TRY_BEGIN
		m_hBackBufferContext = pfnOrig_wglCreateContext(m_hApplicationDC);
		pfnOrig_wglMakeCurrent(m_hApplicationDC, m_hBackBufferContext);
		glGenTextures(2, (GLuint *)&m_TextureID);
		glBindTexture(GL_TEXTURE_2D, m_TextureID[0]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, m_TextureID[1]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

		m_hPOBackScreen = glCreateProgramObjectARB();
		m_hPOFrontScreen = glCreateProgramObjectARB();

		m_hVS = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);    
		GLcharARB *str[1];
		str[0] = g_szVertexShaderText;
		glShaderSourceARB(m_hVS, 1, (const char**)str, NULL);  
		glCompileShaderARB(m_hVS);
		CheckGLSLError(m_hVS, GL_OBJECT_COMPILE_STATUS_ARB);

		m_hFSBackScreen = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB); 
		//if (gInfo.NSMode != 3)
			str[0] = g_szBackScreenShaderText;
		//else
		//	str[0] = g_szRAWLeftShaderText;
		glShaderSourceARB(m_hFSBackScreen, 1, (const char**)str, NULL);  
		glCompileShaderARB(m_hFSBackScreen);
		CheckGLSLError(m_hFSBackScreen, GL_OBJECT_COMPILE_STATUS_ARB);

		glAttachObjectARB(m_hPOBackScreen, m_hFSBackScreen);
		glAttachObjectARB(m_hPOBackScreen, m_hVS);
		glLinkProgramARB(m_hPOBackScreen);  
		CheckGLSLError(m_hPOBackScreen, GL_OBJECT_LINK_STATUS_ARB);
		glValidateProgramARB(m_hPOBackScreen);
		CheckGLSLError(m_hPOBackScreen, GL_OBJECT_VALIDATE_STATUS_ARB);

		m_hFSFrontScreen = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
		//if (gInfo.NSMode != 3)
		{
			//str[0] = g_szFrontScreenShaderText;
			str[0] = g_szFrontScreenCFLShaderText;
		}
		//else
		//	str[0] = g_szRAWRightShaderText;
		glShaderSourceARB(m_hFSFrontScreen, 1, (const char**)str, NULL);
		glCompileShaderARB(m_hFSFrontScreen);  
		CheckGLSLError(m_hFSFrontScreen, GL_OBJECT_COMPILE_STATUS_ARB);

		glAttachObjectARB(m_hPOFrontScreen, m_hFSFrontScreen);
		glAttachObjectARB(m_hPOFrontScreen, m_hVS);
		glLinkProgramARB(m_hPOFrontScreen);  
		CheckGLSLError(m_hPOFrontScreen, GL_OBJECT_LINK_STATUS_ARB);
		glValidateProgramARB(m_hPOFrontScreen);
		CheckGLSLError(m_hPOFrontScreen, GL_OBJECT_VALIDATE_STATUS_ARB);
	_CATCH_ALL
		pfnOrig_wglMakeCurrent(hCurrentDC, hCurrentContext);
		return FALSE;
	_CATCH_END
	pfnOrig_wglMakeCurrent(hCurrentDC, hCurrentContext);
	return TRUE;
}

void Renderer::SaveBufferToFile(int width, int height, char* filename)
{
	BYTE* bmpBuffer = (BYTE*)malloc(width*height*3);
	if (!bmpBuffer)
		return;

	HDC hCurrentDC = pfnOrig_wglGetCurrentDC();
	int iPixelFormat = pfnOrig_wglGetPixelFormat(hCurrentDC);
	PIXELFORMATDESCRIPTOR pfd;
	pfnOrig_wglDescribePixelFormat(hCurrentDC, iPixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
	
	glReadPixels((GLint)0, (GLint)0,
		(GLint)width-1, (GLint)height-1,
		(pfd.cRedShift == 0 ? GL_RGB : GL_BGR), GL_UNSIGNED_BYTE, bmpBuffer);

	FILE *filePtr;	
	if (fopen_s(&filePtr, filename, "wb"))
		return;

	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;

	bitmapFileHeader.bfType = 0x4D42; //"BM"
	bitmapFileHeader.bfSize = width*height*3;
	bitmapFileHeader.bfReserved1 = 0;
	bitmapFileHeader.bfReserved2 = 0;
	bitmapFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	bitmapInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmapInfoHeader.biWidth = width-1;
	bitmapInfoHeader.biHeight = height-1;
	bitmapInfoHeader.biPlanes = 1;
	bitmapInfoHeader.biBitCount = 24;
	bitmapInfoHeader.biCompression = BI_RGB;
	bitmapInfoHeader.biSizeImage = 0;
	bitmapInfoHeader.biXPelsPerMeter = 0; // ?
	bitmapInfoHeader.biYPelsPerMeter = 0; // ?
	bitmapInfoHeader.biClrUsed = 0;
	bitmapInfoHeader.biClrImportant = 0;

	fwrite(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	fwrite(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	fwrite(bmpBuffer, width*height*3, 1, filePtr);
	fclose(filePtr);

	free(bmpBuffer);
}

void Renderer::DumpBuffer( char * szFileName, GLenum buf, char* s )
{
	pfnOrig_glReadBuffer(buf);
	sprintf_s(szFileName, MAX_PATH, "%s\\%s (%s).bmp", gInfo.LogFileDirectory, s, GetDrawBufferFlagsString(buf));
	SaveBufferToFile(m_nWindowWidth, m_nWindowHeight, szFileName);
	DEBUG_MESSAGE("Dump buffer %s (%s)\n", s, GetDrawBufferFlagsString(buf));
}

BOOL CALLBACK MonitorEnumProc( HMONITOR hMonitor,  HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
	Renderer* rend = (Renderer*)dwData;
	MONITORINFOEX mi; 
	mi.cbSize = sizeof(MONITORINFOEX);
	GetMonitorInfo(hMonitor, &mi);
	if(mi.dwFlags == MONITORINFOF_PRIMARY)
	{
		rend->m_BackScreenRect = mi.rcMonitor;
		rend->m_hBackScreen = hMonitor;
	}
	else
	{
		rend->m_FrontScreenRect = mi.rcMonitor;
		rend->m_hFrontScreen = hMonitor;
	}
	if (IsRectEmpty(&rend->m_BackScreenRect) || IsRectEmpty(&rend->m_FrontScreenRect))
		return TRUE;
	else
		return FALSE;
}

BOOL	Renderer::SwapBuffers()
{
	BOOL bStereo = GetStereoRender();
	DEBUG_TRACE2("\tStereo(on = %d)\n", bStereo);
	if (!m_hPBuffer)
		return pfnOrig_wglSwapBuffers(m_hApplicationDC);
	m_bSwapBuffersCalled = TRUE;

	HDC hCurrentDC = pfnOrig_wglGetCurrentDC();
	HGLRC hCurrentContext = pfnOrig_wglGetCurrentContext();

	if (!m_bShadersInitialized && !InitializeShaders())
	{
		DEBUG_MESSAGE("Error: InitializeShaders() returns FALSE\n");
		return FALSE;
	}

	if(m_SwapBuffersCount % 30 == 0)
	{
		MONITORINFOEX mi;
		mi.cbSize = sizeof MONITORINFOEX;
		GetMonitorInfo(m_hBackScreen, &mi);				
		DEVMODE deviceMode;
		deviceMode.dmSize = sizeof(DEVMODE);
		deviceMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL | DM_DISPLAYFREQUENCY;
		EnumDisplaySettings(mi.szDevice, ENUM_CURRENT_SETTINGS, &deviceMode);

		MONITORINFOEX mi2;
		mi2.cbSize = sizeof MONITORINFOEX;
		GetMonitorInfo(m_hFrontScreen, &mi2);
		DEVMODE deviceMode2;
		deviceMode2.dmSize = sizeof(DEVMODE);
		deviceMode2.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL | DM_DISPLAYFREQUENCY;
		EnumDisplaySettings(mi2.szDevice, ENUM_CURRENT_SETTINGS, &deviceMode2);

		if(deviceMode.dmBitsPerPel != deviceMode2.dmBitsPerPel ||
			deviceMode.dmPelsWidth != deviceMode2.dmPelsWidth ||
			deviceMode.dmPelsHeight != deviceMode2.dmPelsHeight)
		{
			DEBUG_MESSAGE("WARNING: Different display mode for secondary adapter\n");
			DEBUG_MESSAGE("Set display mode on second adapter\n");
			if (deviceMode2.dmPosition.x < 0)
				deviceMode2.dmPosition.x += deviceMode2.dmPelsWidth - deviceMode.dmPelsWidth;
			deviceMode.dmPosition = deviceMode2.dmPosition;
			ChangeDisplaySettingsEx((LPTSTR)mi2.szDevice, &deviceMode, NULL, CDS_FULLSCREEN, NULL);

			SetRectEmpty(&m_BackScreenRect);
			SetRectEmpty(&m_FrontScreenRect);
			EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, (LPARAM)this);
			if (IsRectEmpty(&m_FrontScreenRect))
			{
				m_hFrontScreen = m_hBackScreen;
				SetRect(&m_FrontScreenRect, m_BackScreenRect.right, m_BackScreenRect.top, m_BackScreenRect.left + m_BackScreenRect.right, m_BackScreenRect.bottom);
			}
		} else if(deviceMode.dmDisplayFrequency != deviceMode2.dmDisplayFrequency)
		{
			DEBUG_MESSAGE("WARNING: Refresh rate doesn't match\n");
		}
	}

	if (gInfo.EmulateQB && m_bFrontWindowCreated)
	{
		WINDOWINFO wi;
		ZeroMemory(&wi, sizeof(WINDOWINFO));
		wi.cbSize = sizeof(WINDOWINFO);
		if (!GetWindowInfo(m_hWnd, &wi))
		{
			DEBUG_MESSAGE("Error: GetWindowInfo() returns FALSE\n");
			DestroyWindow(m_hFrontWnd);
			m_hFrontWnd = 0;
			m_bFrontWindowCreated = false;
		}
		else
		{
			SetWindowPos(m_hFrontWnd, HWND_TOP, 
				wi.rcClient.left + m_FrontScreenRect.left, wi.rcClient.top + m_FrontScreenRect.top, 
				wi.rcClient.right - wi.rcClient.left,
				wi.rcClient.bottom - wi.rcClient.top,
				SWP_NOZORDER | SWP_NOACTIVATE);	
		}	
	}
	BOOL bDoDump = FALSE;
	if(GINFO_DEBUG && IsKeyDown(VK_SCROLL))
	{
		bDoDump = TRUE;
		char szFileName[MAX_PATH];
		GLenum oldBuf;
		glGetIntegerv(GL_READ_BUFFER, (GLint*)&oldBuf);
		DumpBuffer(szFileName, GL_FRONT, "1. Left");
		DumpBuffer(szFileName, GL_BACK, "1. Right");
		pfnOrig_glReadBuffer(oldBuf);
	}
	pfnOrig_wglMakeCurrent(m_hApplicationDC, m_hBackBufferContext);

	//glClearColor(0, 1, 0, 0);
	//glClear(GL_COLOR_BUFFER_BIT);
	pfnOrig_glViewport(0, 0, m_nWindowWidth, m_nWindowHeight);
	float fTextureCoordX = (float)m_nWindowWidth / m_nPBufferWidth;
	float fTextureCoordY = (float)m_nWindowHeight / m_nPBufferHeight;
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);

	glEnable(GL_TEXTURE_2D);
	glActiveTextureARB( GL_TEXTURE0_ARB );
	glBindTexture(GL_TEXTURE_2D, m_TextureID[0]);
	wglBindTexImageARB(m_hPBuffer, bStereo ? WGL_FRONT_LEFT_ARB : m_MonoBuffer);
	glActiveTextureARB( GL_TEXTURE1_ARB );
	glBindTexture(GL_TEXTURE_2D, m_TextureID[1]);
	wglBindTexImageARB(m_hPBuffer, bStereo ? WGL_BACK_LEFT_ARB : m_MonoBuffer);

	// render to front screen (second adapter)
	GLenum buf;
	if (gInfo.EmulateQB)
	{
		if (m_bFrontWindowCreated)
		{
			pfnOrig_wglMakeCurrent(m_hFrontDC, m_hBackBufferContext);
			buf = m_nDrawBufferMode[1] == GL_BACK || m_nDrawBufferMode[1] == GL_BACK_LEFT || m_nDrawBufferMode[1] == GL_BACK_RIGHT ? GL_BACK : GL_FRONT;
			DEBUG_TRACE2("\tFront = %s\n", GetDrawBufferFlagsString(buf));
			pfnOrig_glDrawBuffer(buf);
		}
	}
	else
	{
		buf = m_nDrawBufferMode[1];
		DEBUG_TRACE2("\tFront = %s\n", GetDrawBufferFlagsString(buf));
		pfnOrig_glDrawBuffer(m_nDrawBufferMode[1]); // RIGHT
	}

	glUseProgramObjectARB(m_hPOFrontScreen);
	GLint sL = glGetUniformLocationARB(m_hPOFrontScreen, "sL");
	if (sL != -1)
		glUniform1iARB(sL, 0);
	GLint sR = glGetUniformLocationARB(m_hPOFrontScreen, "sR");
	if (sR != -1)
		glUniform1iARB(sR, 1);
	CheckGLError();
	glBegin( GL_QUADS );
	glTexCoord2f(0, 0);
	glVertex2f(-1, -1);
	glTexCoord2f(0, fTextureCoordY);
	glVertex2f(-1, 1);
	glTexCoord2f(fTextureCoordX, fTextureCoordY);
	glVertex2f( 1, 1);
	glTexCoord2f(fTextureCoordX, 0);
	glVertex2f( 1, -1);
	glEnd();
	if(bDoDump)
	{
		char szFileName[MAX_PATH];
		GLenum oldBuf;
		glGetIntegerv(GL_READ_BUFFER, (GLint*)&oldBuf);
		DumpBuffer(szFileName, buf, "2. Front screen");
		pfnOrig_glReadBuffer(oldBuf);
	}

	// render to back screen (primary adapter)
	if (gInfo.EmulateQB && m_bFrontWindowCreated)
	{
		pfnOrig_wglSwapBuffers(m_hFrontDC);
		pfnOrig_wglMakeCurrent(m_hApplicationDC, m_hBackBufferContext);
	}
	DEBUG_TRACE2("\nBack = %s\n", GetDrawBufferFlagsString(m_nDrawBufferMode[0]));
	pfnOrig_glDrawBuffer(m_nDrawBufferMode[0]); // LEFT

	glUseProgramObjectARB(m_hPOBackScreen);
	sL = glGetUniformLocationARB(m_hPOBackScreen, "sL");
	if (sL != -1)
		glUniform1iARB(sL, 0);
	sR = glGetUniformLocationARB(m_hPOBackScreen, "sR");
	if (sR != -1)
		glUniform1iARB(sR, 1);
	CheckGLError();
	glBegin( GL_QUADS );
	glTexCoord2f(0, 0);
	glVertex2f(-1, -1);
	glTexCoord2f(0, fTextureCoordY);
	glVertex2f(-1, 1);
	glTexCoord2f(fTextureCoordX, fTextureCoordY);
	glVertex2f( 1, 1);
	glTexCoord2f(fTextureCoordX, 0);
	glVertex2f( 1, -1);
	glEnd();	
	if(bDoDump)
	{
		char szFileName[MAX_PATH];
		GLenum oldBuf;
		glGetIntegerv(GL_READ_BUFFER, (GLint*)&oldBuf);
		DumpBuffer(szFileName, m_nDrawBufferMode[0], "2. Back screen");
		pfnOrig_glReadBuffer(oldBuf);
	}

	glActiveTextureARB( GL_TEXTURE0_ARB );
	glBindTexture(GL_TEXTURE_2D, m_TextureID[0]);
	wglReleaseTexImageARB(m_hPBuffer, bStereo ? WGL_FRONT_LEFT_ARB : m_MonoBuffer);
	glActiveTextureARB( GL_TEXTURE1_ARB );
	glBindTexture(GL_TEXTURE_2D, m_TextureID[1]);
	wglReleaseTexImageARB(m_hPBuffer, bStereo ? WGL_BACK_LEFT_ARB : m_MonoBuffer);
	glActiveTextureARB( GL_TEXTURE0_ARB );

	glUseProgramObjectARB(0);
	glDisable(GL_TEXTURE_2D);
	//----
#if 0
	//if (gInfo.Debug)
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, m_TextureID[0]);
		wglBindTexImageARB(m_hPBuffer[0], WGL_FRONT_LEFT_ARB);
		pfnOrig_glDrawBuffer(m_nDrawBufferMode[0]);
		glBegin( GL_QUADS );
		glTexCoord2f(0, 0);
		glVertex2f(-1, 0.5);
		glTexCoord2f(0, fTextureCoordY);
		glVertex2f(-1, 1);
		glTexCoord2f(fTextureCoordX, fTextureCoordY);
		glVertex2f( -0.5, 1);
		glTexCoord2f(fTextureCoordX, 0);
		glVertex2f( -0.5, 0.5);
		glEnd();	
		wglReleaseTexImageARB(m_hPBuffer[0], WGL_FRONT_LEFT_ARB);
		glBindTexture(GL_TEXTURE_2D, m_TextureID[1]);
		wglBindTexImageARB(m_hPBuffer[1], WGL_FRONT_LEFT_ARB);
		pfnOrig_glDrawBuffer(m_nDrawBufferMode[1]);
		glBegin( GL_QUADS );
		glTexCoord2f(0, 0);
		glVertex2f(-1, 0.5);
		glTexCoord2f(0, fTextureCoordY);
		glVertex2f(-1, 1);
		glTexCoord2f(fTextureCoordX, fTextureCoordY);
		glVertex2f( -0.5, 1);
		glTexCoord2f(fTextureCoordX, 0);
		glVertex2f( -0.5, 0.5);
		glEnd();	
		wglReleaseTexImageARB(m_hPBuffer[1], WGL_FRONT_LEFT_ARB);
		glDisable(GL_TEXTURE_2D);
	}
#endif
	BOOL bResult = pfnOrig_wglSwapBuffers(m_hApplicationDC);
	pfnOrig_wglMakeCurrent(hCurrentDC, hCurrentContext);
	m_SwapBuffersCount++;

	return bResult;
}

#define WINDOW_CLASS_NAME (_T(SHORT_COMPANY_NAME) _T(" QBS Driver") )
BOOL	Renderer::CreateFrontWindow()
{
	if (m_hFrontWnd)
		DestroyWindow(m_hFrontWnd);
	WNDCLASS wcls;
	if (!GetClassInfo(g_hDllInstance, WINDOW_CLASS_NAME, &wcls))
	{
		memset(&wcls, 0, sizeof(wcls));
		wcls.lpfnWndProc = DefWindowProc;
		wcls.hInstance = g_hDllInstance;
		wcls.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcls.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wcls.lpszClassName = WINDOW_CLASS_NAME;
		RegisterClass(&wcls);
	}

	WINDOWINFO wi;
	ZeroMemory(&wi, sizeof(WINDOWINFO));
	wi.cbSize = sizeof(WINDOWINFO);
	if (!GetWindowInfo(m_hWnd, &wi))
	{
		DEBUG_MESSAGE("Error: GetWindowInfo() returns FALSE\n");
		return FALSE;
	}
#if 1
	SetRectEmpty(&m_BackScreenRect);
	SetRectEmpty(&m_FrontScreenRect);
	EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, (LPARAM)this);
	if (IsRectEmpty(&m_FrontScreenRect))
	{
		m_hFrontScreen = m_hBackScreen;
		SetRect(&m_FrontScreenRect, m_BackScreenRect.right, m_BackScreenRect.top, m_BackScreenRect.left + m_BackScreenRect.right, m_BackScreenRect.bottom);
	}
	MONITORINFO mi;
	mi.cbSize = sizeof(MONITORINFO);
	m_hFrontWnd = CreateWindowEx(WS_EX_NOACTIVATE|WS_EX_TOOLWINDOW|WS_EX_TOPMOST, WINDOW_CLASS_NAME, _T(MONITOR_NAME) _T(" Window"), 
		WS_POPUP | WS_VISIBLE, 
		wi.rcClient.left + m_FrontScreenRect.left, wi.rcClient.top + m_FrontScreenRect.top, 
		m_nWindowWidth, m_nWindowHeight, 0/*m_hWnd*/, 0, g_hDllInstance, 0);
	if (m_hFrontWnd == NULL)
	{
		DEBUG_MESSAGE("Error: CreateWindowA() returns NULL\n");
		return FALSE;
	}
	m_hFrontDC = GetDC(m_hFrontWnd);
	HDC hCurrentDC = pfnOrig_wglGetCurrentDC();
	int iPixelFormat = pfnOrig_wglGetPixelFormat(hCurrentDC);
	PIXELFORMATDESCRIPTOR pfd;
	pfnOrig_wglDescribePixelFormat(hCurrentDC, iPixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
	_TRY_BEGIN
		// fail on ATI when primary window on second adapter
		int iFrontPixelFormat = pfnOrig_wglChoosePixelFormat(m_hFrontDC, &pfd);
		pfnOrig_wglSetPixelFormat(m_hFrontDC, iFrontPixelFormat, &pfd);   
		m_bFrontWindowCreated = TRUE;
		DEBUG_MESSAGE("Front window created\n");
	_CATCH_ALL
		DEBUG_MESSAGE("Front window not created\n");
	_CATCH_END
#endif
	return TRUE;
}
