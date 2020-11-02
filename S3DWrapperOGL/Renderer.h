/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

// OpenGL Rendering Context (HGLRC) = 3D Device
// Device Context (HDC) = SwapChains

class Renderer
{
	HWND	m_hWnd;
	HDC		m_hApplicationDC;
	HGLRC	m_hApplicationContext;
	HGLRC	m_hBackBufferContext;
	UINT	m_nWindowWidth;
	UINT	m_nWindowHeight;
	UINT	m_nMonitorWidth;
	UINT	m_nMonitorHeight;
	UINT	m_nPBufferWidth;
	UINT	m_nPBufferHeight;

	HPBUFFERARB	m_hPBuffer;
	HDC		m_hPBufferDC;
	HGLRC	m_hPBufferContext;
	BOOL	m_bCreated;
	BOOL	Create();
	BOOL	CreatePB();
	void	DestroyPB();

	GLuint	m_TextureID[2];

	GLenum	m_nDrawBufferMode[2];
	BOOL	m_bStereoRender;
	BOOL	m_bSwapBuffersCalled;

	BOOL	m_bShadersInitialized;
	GLhandleARB m_hPOBackScreen;
	GLhandleARB	m_hFSBackScreen;
	GLhandleARB m_hPOFrontScreen;
	GLhandleARB m_hFSFrontScreen;
	GLhandleARB	m_hVS;
	BOOL	InitializeShaders();

	BOOL	m_bFrontWindowCreated;
	BOOL	CreateFrontWindow();
	HDC		m_hFrontDC;
	GLenum	m_MonoBuffer;
	UINT	m_SwapBuffersCount;
	HMONITOR	m_hBackScreen, m_hFrontScreen;
	RECT		m_BackScreenRect, m_FrontScreenRect;
	void	DumpBuffer( char * szFileName, GLenum buf, char* s );

public:
	Renderer(void);
	~Renderer(void);
	BOOL	SetApplicationDC(HDC ApplicationDC);
	HDC		GetApplicationDC() { return m_hApplicationDC; }
	BOOL	SetApplicationContext(HGLRC	hContext) { m_hApplicationContext = hContext; }
	HGLRC	GetApplicationContext() { return m_hApplicationContext; }

	BOOL	IsDCMatched(HDC hDC);
	void	DrawBuffer(GLenum mode);
	BOOL	SwapBuffers();

	void	SetStereoRender(BOOL bStereoRender);
	inline BOOL	GetStereoRender() { return m_bStereoRender; }
	BOOL    MakeCurrent(HGLRC hglrc);
	BOOL	CheckGLError();
	BOOL	CheckGLSLError(GLhandleARB Handle, GLenum Param);
	void	SaveBufferToFile(int width, int height, char* filename);
	BOOL	m_bGlInit;
	friend BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData);
};

inline BOOL	Renderer::IsDCMatched(HDC hDC)
{
	if (hDC == m_hApplicationDC || hDC == m_hPBufferDC)
	{
		return TRUE;
	}
	return FALSE;
}

extern std::vector<Renderer>	g_RendererList;

typedef void ( APIENTRY* PFNGLGETBOOLEANV )(GLenum pname, GLboolean *params);
typedef int ( APIENTRY* PFNGDIRELEASEDC )(HWND hWnd, HDC hDC);
typedef HGLRC ( APIENTRY* PFNWGLCREATECONTEX )(HDC hdc);
typedef BOOL ( APIENTRY* PFNWGLDELETECONTEX )(HGLRC hrc);
typedef HDC ( APIENTRY* PFNWGLGETCURRENTDC )(VOID);
typedef HGLRC ( APIENTRY* PFNWGLGETCURRENTCONTEX )(VOID);
typedef BOOL ( APIENTRY* PFNWGLMAKECURRENT )(HDC hdc, HGLRC hglrc);
typedef void ( APIENTRY* PFNWGLVIEWPORT )(GLint x, GLint y, GLsizei width, GLsizei height);
typedef void ( APIENTRY* PFNWGLSCISSOR )(GLint x, GLint y, GLsizei width, GLsizei height);
typedef BOOL ( APIENTRY* PFNWGLSWAPBUFFERS )(HDC hdc);
typedef void ( APIENTRY* PFNWGLDRAWBUFFER )(GLenum mode);
typedef void ( APIENTRY* PFNWGLREADBUFFER )(GLenum mode);
typedef int ( APIENTRY* PFNWGLCHOOSEPIXELFORMAT )( __in HDC hdc, __in CONST PIXELFORMATDESCRIPTOR *ppfd);
typedef int ( APIENTRY* PFNWGLDESCRIBEPIXELFORMAT )(  __in HDC hdc, __in int iPixelFormat, __in UINT nBytes, __out_bcount_opt(nBytes) LPPIXELFORMATDESCRIPTOR ppfd);
typedef int ( APIENTRY* PFNWGLGETPIXELFORMAT )(__in HDC hdc);
typedef BOOL ( APIENTRY* PFNWGLSETPIXELFORMAT )(__in HDC hdc, __in int format, __in CONST PIXELFORMATDESCRIPTOR * ppfd);
typedef PROC ( APIENTRY* PFNWGLAPICALL )( LPCSTR );

extern PFNWGLCREATECONTEX pfnOrig_wglCreateContext;
extern PFNWGLGETCURRENTDC pfnOrig_wglGetCurrentDC;
extern PFNWGLGETCURRENTCONTEX pfnOrig_wglGetCurrentContext;
extern PFNWGLMAKECURRENT pfnOrig_wglMakeCurrent;
extern PFNWGLVIEWPORT pfnOrig_glViewport;
extern PFNWGLSCISSOR pfnOrig_glScissor;
extern PFNWGLSWAPBUFFERS pfnOrig_wglSwapBuffers;
extern PFNWGLDRAWBUFFER pfnOrig_glDrawBuffer;
extern PFNWGLREADBUFFER pfnOrig_glReadBuffer;
extern PFNWGLCHOOSEPIXELFORMAT pfnOrig_wglChoosePixelFormat;
extern PFNWGLDESCRIBEPIXELFORMAT pfnOrig_wglDescribePixelFormat;
extern PFNWGLGETPIXELFORMAT pfnOrig_wglGetPixelFormat;
extern PFNWGLSETPIXELFORMAT pfnOrig_wglSetPixelFormat;
extern PFNWGLAPICALL pfnOrig_wglGetProcAddress;

inline UINT FindPow2( UINT x )
{
	UINT val = 2;	
	while (val < x)
		val <<= 1;
	return val;
}
