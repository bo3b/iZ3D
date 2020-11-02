#include "pcmp.h"
#include <time.h>
#include <shobjidl.h>
#include "global.h"
#include "render.h"

/************************************************************************/
/* WndProc                                                              */
/************************************************************************/
LRESULT CALLBACK WndProc( HWND _hWnd, UINT _message, WPARAM _wParam, LPARAM _lParam )
{
	switch( _message )
	{
	case WM_DESTROY:
		PostQuitMessage( 0 );
		break;

	default:
		return DefWindowProc( _hWnd, _message, _wParam, _lParam );
	}

	return 0;
}

enum Modes
{
	None = 0,
	//	Sample = 1,
	Test = 2,
	Master = 3,
};
Modes g_Mode = None;

/************************************************************************/
/* InitWindow                                                           */
/************************************************************************/
HRESULT InitWindow( HINSTANCE _hInstance )
{
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof( WNDCLASSEX );
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = _hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor( NULL, IDC_ARROW );
	wcex.hbrBackground = ( HBRUSH )( COLOR_WINDOW + 1 );
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"UserMode";
	wcex.hIconSm = NULL;
	if( !RegisterClassEx( &wcex ) )
		return E_FAIL;

	RECT rc = { 0, 0, g_nWndWidth, g_nWndHeight };
	AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );
	g_hWnd = CreateWindowEx(0, L"UserMode", 
		L"Test sample DX1x - Press ENTER to create a master screenshot"
#ifdef WIN64
		L" [x64]",
#else
		L" [x86]",
#endif
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, _hInstance,
		NULL );
	if( !g_hWnd )
		return E_FAIL;

	g_hInstance = _hInstance;
	
	int nX = GetSystemMetrics( SM_CXSCREEN );
	int nY = GetSystemMetrics( SM_CYSCREEN );
	int nOffsetX = ( nX - ( rc.right - rc.left ) ) / 2;
	int nOffsetY = ( nY - ( rc.bottom - rc.top ) ) / 2;

	// move window into top-left corner
	nOffsetX = 0;
	nOffsetY = 0;

	SetWindowPos( g_hWnd, HWND_TOP, nOffsetX, nOffsetY, rc.right - rc.left, rc.bottom - rc.top, SWP_SHOWWINDOW );

	return S_OK;
}

#include "IStereoAPI.h"

bool FileExists(const TCHAR *fileName)
{    
	return GetFileAttributes(fileName) != INVALID_FILE_ATTRIBUTES;
}

void Callback()
{
	BOOL bExit = TRUE;
	if (g_Mode != Test && g_Mode != Master)
	{
		bExit = IDYES == MessageBox(g_hWnd,
			L"Screenshot created successfully. Close the application?",L"Success",MB_YESNO | MB_ICONQUESTION);
	}
	if (bExit) ExitProcess(0);
}

/************************************************************************/
/* SetProgress                                                          */
/************************************************************************/
void SetProgress(int value)
{
#ifdef __ITaskbarList3_FWD_DEFINED__
	static bool bEnableProgress = false;
	static int prevValue = 0;
	static unsigned start;
	if(g_pTaskbarList)
	{
		if (value > 0)
		{
			if (prevValue == 0)
			{
				unsigned finish = clock();
				double duration = (double)(finish - start) / CLOCKS_PER_SEC;
				bEnableProgress = (duration > 0.5);
			}
			if (bEnableProgress)
				g_pTaskbarList->SetProgressValue ( g_hWnd, value, MaxCommandIndex );
		}
		else
			g_pTaskbarList->SetProgressState( g_hWnd, TBPF_NOPROGRESS );
	}
	prevValue = value;
	start = clock();
#endif
}

/************************************************************************/
/* WinMain                                                              */
/************************************************************************/
int WINAPI wWinMain( HINSTANCE _hInstance, HINSTANCE _hPrevInstance, LPWSTR _lpCmdLine, int _nCmdShow )
{
	if (wcscmp(_lpCmdLine,L"test") == 0 || wcscmp(_lpCmdLine,L"-test") == 0)
	{
		g_Mode = Test;
	}
	else if (wcscmp(_lpCmdLine,L"-master") == 0)
	{
		g_Mode = Master;
	}

	GetModuleFileName( _hInstance, g_sBinDir, MAX_PATH );
	int nLen = ( int )_tcslen( GetDirFromFileName( g_sBinDir ) ) + 1;
	memcpy( g_sBinDir, GetDirFromFileName( g_sBinDir ), nLen * sizeof( TCHAR ) );

	if( FAILED( InitWindow( _hInstance ) ) )
		return 0;

	HRESULT hr;
#ifdef __ITaskbarList3_FWD_DEFINED__
	hr = CoInitialize(NULL);
	hr = CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&g_pTaskbarList));
	if (g_pTaskbarList)
		g_pTaskbarList->SetProgressState(g_hWnd, TBPF_INDETERMINATE);
#endif

	if ( FAILED( CRender::Init() ) )
		return 0;

	CComPtr<IStereoAPI> pAPI;
	HMODULE hModule = GetStereoLibraryHandle();
	hr = CreateStereoAPI(hModule, &pAPI);
	if(g_Mode != None && FAILED(hr)) return -2;
	CComPtr<IStereoAPIInternal>  pAPI2;
	hr = CreateStereoAPIInternal(hModule, &pAPI2);
	if(g_Mode != None && FAILED(hr)) return -2;
	g_pAPI2 = pAPI2;
	
	if (pAPI)
		pAPI->SetStereoActive(true);
	
	TCHAR cfg[MAX_PATH];
	wcscpy(cfg,g_sBinDir);
	wcscat(cfg,L"\\master.cfg");
	if (g_Mode == Test)
	{
		float fSep = 0.0f;
		float fCov = 1.0f;

		std::ifstream file;
		file.open(cfg, std::ios::in);
		if (!file.is_open())
			return -1;
		file >> fSep;
		if (file.fail())
			return -1;
		file >> fCov;
		if (file.fail())
			return -1;
		file.close();

		if (pAPI)
		{
			pAPI->SetSeparation(fSep);
			pAPI->SetConvergence(fCov);
		}
	}

	MSG msg = {0};
	while( WM_QUIT != msg.message )
	{
		if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			if (msg.message == WM_KEYDOWN && msg.wParam == VK_RETURN) 
			{
				if (IDYES == MessageBox(g_hWnd,L"Do you want to create a master screenshot?",
												L"Create master screenshot",MB_YESNO | MB_ICONQUESTION))
				{
					TCHAR path[MAX_PATH];
					wcscpy(path,g_sBinDir);
					wcscat(path,L"\\master.jpg");

					if (!FileExists(path) || IDYES == MessageBox(g_hWnd,L"Master screenshot already exists. Owerwrite?",
																	L"Already exists",MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2))
					{
						DeleteFile(path);
						_ASSERT(!FileExists(path));

						float fSep = pAPI->GetSeparation();
						float fCov = pAPI->GetConvergence();

						std::ofstream file;
						file.open(cfg, std::ios::out | std::ios::trunc);
						_ASSERT(file.is_open());
						file << fSep << std::endl;
						file << fCov << std::endl;
						file.close();

						pAPI2->MakeScreenshot(path,Callback);
					}
				}
			}
			else
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
		}
		else
		{
			static int counter = 0;
			CRender::DrawFrame();

			counter++;

			if (counter == 5)
			{
#ifdef __ITaskbarList3_FWD_DEFINED__
				if (g_pTaskbarList)
					g_pTaskbarList->SetProgressState(g_hWnd, TBPF_NORMAL);
				SetProgress(0);
#endif
				if (g_Mode == Test)
				{
					TCHAR path[MAX_PATH];
					wcscpy(path,g_sBinDir);
					wcscat(path,L"\\test.jpg");

					DeleteFile(path);
					_ASSERT(!FileExists(path));

					pAPI2->MakeScreenshot(path,Callback);
				}
				else if(g_Mode == Master)
				{
					TCHAR path[MAX_PATH];
					wcscpy(path,g_sBinDir);
					wcscat(path,L"\\master.jpg");

					if (FileExists(path)) DeleteFile(path);
					_ASSERT(!FileExists(path));

					float fSep = pAPI->GetSeparation();
					float fCov = pAPI->GetConvergence();

					std::ofstream file;
					file.open(cfg, std::ios::out | std::ios::trunc);
					_ASSERT(file.is_open());
					file << fSep << std::endl;
					file << fCov << std::endl;
					file.close();

					pAPI2->MakeScreenshot(path,Callback);
				}
			}
		}
	}
	
	g_pAPI2.Release();

	CRender::Release();

	return ( int )msg.wParam;
}
