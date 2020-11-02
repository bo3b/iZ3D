#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#pragma comment(lib,"d3d10.lib")
#pragma comment(lib,"d3d10_1.lib")
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"d3dx10d.lib")
#pragma comment(lib,"dxgi.lib")

extern HINSTANCE					g_hInstance;
extern HWND							g_hWnd;
extern int							g_nWndWidth;
extern int							g_nWndHeight;
extern bool							g_bFullscreen;
extern TCHAR						g_sBinDir[ MAX_PATH ];
extern struct ITaskbarList3*		g_pTaskbarList;
extern CComQIPtr<IStereoAPIInternal2>  g_pAPI2;

TCHAR*								GetDirFromFileName( TCHAR* _szFileName );
TCHAR*								GetShaderPath( TCHAR* _szSahderName );
void								LoadFileToMemory();
void  								SetEID(UINT64 eid);

#ifndef SAFE_RELEASE
	#define SAFE_RELEASE(p)			{ if (p) { (p)->Release(); (p)=NULL; } }
#endif

#define V_(func)					hr = func; _ASSERT(SUCCEEDED(hr))

#endif//_GLOBAL_H_