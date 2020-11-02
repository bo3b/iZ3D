#include "stdafx.h"
#include <mmsystem.h>
#include <time.h>
#include <shobjidl.h>
#include "main.h"

LPDIRECT3D9             g_pD3D = NULL; // Used to create the D3DDevice
LPDIRECT3DDEVICE9       device = NULL; // Our rendering device
D3DPRESENT_PARAMETERS   dpp;
IDirect3DStateBlock9*   defaultState = NULL;
struct ITaskbarList3*	g_pTaskbarList = NULL;
CComQIPtr<IStereoAPIInternal2> g_pAPI2;

@@GLOBALS@@

extern VOID Render();

extern HRESULT Init(HWND hWnd);

VOID Cleanup()
{
	if(device) device->Release();
	if(g_pD3D) g_pD3D->Release();
}

LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch( msg )
	{
		case WM_DESTROY:
			Cleanup();
			PostQuitMessage( 0 );
			return 0;
	}

	return DefWindowProc( hWnd, msg, wParam, lParam );
}

#include "IStereoAPI.h"

bool FileExists(const TCHAR *fileName)
{    
	return GetFileAttributes(fileName) != INVALID_FILE_ATTRIBUTES;
}

BOOL g_TestMode = FALSE;
BOOL g_MasterMode = FALSE;
TCHAR g_sBinDir[MAX_PATH];
HWND g_hWnd;

void Callback()
{
	BOOL bExit = TRUE;
	if (!g_TestMode && !g_MasterMode)
	{
		bExit = IDYES == MessageBox(g_hWnd,
			L"Screenshot created successfully. Close the application?",L"Success",MB_YESNO | MB_ICONQUESTION);
	}
	if (bExit) ExitProcess(0);
}

TCHAR*	GetDirFromFileName( TCHAR* _szFileName )
{
	static TCHAR szBuff[ MAX_PATH ];
	int nLen = ( int )_tcslen( _szFileName ) + 1;
	memcpy( szBuff, _szFileName, nLen * sizeof( TCHAR ) );
	int nLastSlashPos = -1;

	for ( int i = 0; i < nLen; i++ )
	{
		if ( _szFileName[ i ] == '\\' || _szFileName[ i ] == '/' )
			nLastSlashPos = i;
	}

	if ( nLastSlashPos == -1 )
		return szBuff;

	szBuff[ nLastSlashPos ] = '\0';
	return szBuff;
}

void  SetEID(UINT64 eid)
{
	if (g_pAPI2) g_pAPI2->SetEID(eid);
}

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
				g_pTaskbarList->SetProgressValue ( g_hWnd, value, @@MaxCommandIndex@@ );
		}
		else
			g_pTaskbarList->SetProgressState( g_hWnd, TBPF_NOPROGRESS );
	}
	prevValue = value;
	start = clock();
#endif
}

INT WINAPI wWinMain( HINSTANCE _hInstance, HINSTANCE, LPWSTR _lpCmdLine, INT )
{
	if (wcscmp(_lpCmdLine,L"test") == 0 || wcscmp(_lpCmdLine,L"-test") == 0)
	{
		g_TestMode = TRUE;
	}
	if (wcscmp(_lpCmdLine,L"-master") == 0)
	{
		g_MasterMode = TRUE;
	}

	GetModuleFileName( _hInstance, g_sBinDir, MAX_PATH );
	int nLen = ( int )_tcslen( GetDirFromFileName( g_sBinDir ) ) + 1;
	memcpy( g_sBinDir, GetDirFromFileName( g_sBinDir ), nLen * sizeof( TCHAR ) );

	// Register the window class
	WNDCLASSEX wc =
	{
		sizeof( WNDCLASSEX ), CS_CLASSDC, MsgProc, 0L, 0L,
		GetModuleHandle( NULL ), NULL, NULL, NULL, NULL,
		L"D3D Tutorial", NULL
	};
	RegisterClassEx( &wc );
	
	// Create the application's window
	RECT rc = { 0, 0, g_nWndWidth, g_nWndHeight };
	AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );
	g_hWnd = CreateWindowEx(0, L"D3D Tutorial", 
		L"@@APP_NAME@@ sample DX9 - Press ENTER to create a master screenshot"
#ifdef WIN64
		L" [x64]",
#else
		L" [x86]",
#endif
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, wc.hInstance,
		NULL );
	if( !g_hWnd )
		return E_FAIL;

	int nX = GetSystemMetrics( SM_CXSCREEN );
	int nY = GetSystemMetrics( SM_CYSCREEN );
	int nOffsetX = ( nX - ( rc.right - rc.left ) ) / 2;
	int nOffsetY = ( nY - ( rc.bottom - rc.top ) ) / 2;

	// move window into top-left corner
	nOffsetX = 0;
	nOffsetY = 0;

	SetWindowPos( g_hWnd, HWND_TOP, nOffsetX, nOffsetY, rc.right - rc.left, rc.bottom - rc.top, SWP_SHOWWINDOW );

	HRESULT hr;
#ifdef __ITaskbarList3_FWD_DEFINED__
	hr = CoInitialize(NULL);
	hr = CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&g_pTaskbarList));
	if (g_pTaskbarList)
		g_pTaskbarList->SetProgressState(g_hWnd, TBPF_INDETERMINATE);
#endif

	// Initialize Direct3D
	if(!SUCCEEDED(Init(g_hWnd))) return -1;

	UpdateWindow( g_hWnd );

	CComPtr<IStereoAPI> pAPI;
	HMODULE hModule = GetStereoLibraryHandle();
	hr = CreateStereoAPI(hModule, &pAPI);
	if((g_TestMode || g_MasterMode) && FAILED(hr)) return -2;
	CComPtr<IStereoAPIInternal> pAPI2;
	hr = CreateStereoAPIInternal(hModule, &pAPI2);
	if((g_TestMode || g_MasterMode) && FAILED(hr)) return -2;
	g_pAPI2 = pAPI2;

	if (pAPI)
		pAPI->SetStereoActive(true);
	
	TCHAR cfg[MAX_PATH];
	wcscpy(cfg,g_sBinDir);
	wcscat(cfg,L"\\master.cfg");
	if (g_TestMode)
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

	// Enter the message loop
	MSG msg;
	ZeroMemory( &msg, sizeof( msg ) );
	while( msg.message != WM_QUIT )
	{
		if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
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
			Render();

			counter++;

			if (counter == 5)
			{
#ifdef __ITaskbarList3_FWD_DEFINED__
				if (g_pTaskbarList)
					g_pTaskbarList->SetProgressState(g_hWnd, TBPF_NORMAL);
				SetProgress(0);
#endif
				if (g_TestMode)
				{
					TCHAR path[MAX_PATH];
					wcscpy(path,g_sBinDir);
					wcscat(path,L"\\test.jpg");

					DeleteFile(path);
					_ASSERT(!FileExists(path));

					pAPI2->MakeScreenshot(path,Callback);
				}

				if (g_MasterMode)
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

	UnregisterClass( L"D3D Tutorial", wc.hInstance );
	return 0;
}

/*char *GetFromFile(UINT64 pos,DWORD *pLen,DWORD *pitch1,DWORD *pitch2)
{
	pitch1 = pitch1;
	pitch2 = pitch2;

	_ASSERT(pos >= 0);
	using namespace std;

	static char *fileData = 0;
	static UINT64 fileLen;
	if (!fileData)
	{
		ifstream myFile;
		myFile.open("data.bin", ios::in | ios::binary);
		if (!myFile.is_open())
		{
			myFile.open("..\\data.bin", ios::in | ios::binary);
			if (!myFile.is_open())
			{
				myFile.open("..\\..\\data.bin", ios::in | ios::binary);
				_ASSERT(myFile.is_open());
			}
		}

		UINT64 begin = myFile.tellg();
		myFile.seekg(0,ios::end);
		UINT64 end = myFile.tellg();
		myFile.seekg(0,ios::beg);

		fileLen = end - begin;
		fileData = new char[(UINT)fileLen];

		myFile.read(fileData,fileLen);
		myFile.close();

		const char buf[] = { 0,0,0,0,'i','Z','3','D',0,0,0,2 };
		_ASSERT(memcmp(fileData,buf,sizeof(buf)) == 0);
	}

	_ASSERT(pos + sizeof(DWORD) * 3 <= fileLen);
	//_ASSERT(*(DWORD *)(fileData + pos) == 2);
	pos += sizeof(DWORD);

	/*if (pitch1) *pitch1 = *(DWORD *)(fileData + pos);
	pos += sizeof(DWORD);

	if (pitch2) *pitch2 = *(DWORD *)(fileData + pos);
	pos += sizeof(DWORD);* /

	_ASSERT(pos + sizeof(DWORD) <= fileLen);
	DWORD *len = (DWORD *)(fileData + pos);
	pos += sizeof(DWORD);

	if (pLen) *pLen = *len;

	_ASSERT(pos + *len <= fileLen);
	return fileData + pos;
}*/


struct LoadedData
{
	void *data;
	DWORD len;
	/*DWORD *params;
	DWORD count;*/

	UINT size;
};

std::map<UINT64,LoadedData> LoadedDataCache;
UINT m_cacheSize = 0;
int m_newFileFormatFlag = 0;

char *GetFromFile(UINT64 pos,DWORD *pLen,DWORD *pitch1,DWORD *pitch2)
{
	_ASSERT(pos >= 0);

	if (LoadedDataCache.find(pos) != LoadedDataCache.end())
	{
		LoadedData ld = LoadedDataCache[pos];
		if (pLen) *pLen = ld.len;
		/*if (pitch1) *pitch1 = ld.count > 0 ? ld.params[0] : 0;
		if (pitch2) *pitch2 = ld.count > 1 ? ld.params[1] : 0;*/
		return (char *)ld.data;
	}

	using namespace std;

	ifstream myFile;
	myFile.open("data.bin", ios::in | ios::binary);
	if (!myFile.is_open())
	{
		myFile.open("..\\data.bin", ios::in | ios::binary);
		if (!myFile.is_open())
		{
			myFile.open("..\\..\\data.bin", ios::in | ios::binary);
			if (!myFile.is_open())
				exit(-3);
		}
	}

	if (!m_newFileFormatFlag)
	{
		DWORD null;
		myFile.read((char *)&null,sizeof(null));
		if (null == 0)
		{
			char str[4];
			myFile.read(str,4);
			_ASSERT(memcmp(str,"iZ3D",4) == 0);
			myFile.read(str,4);
			_ASSERT(str[0] == 0 && str[1] == 0 && str[2] == 0 && str[3] == 2);
			m_newFileFormatFlag = 1;
		}
		else
		{
			m_newFileFormatFlag = -1;
		}
	}

	myFile.seekg(pos,ios::beg);
	LoadedData ld;
	if (m_newFileFormatFlag == 1)
	{
		DWORD count;
		myFile.read((char *)&count,sizeof(count));
		_ASSERT(count == 0);
		/*DWORD *params = new DWORD[count];
		myFile.read((char *)params,sizeof(DWORD) * count);
		delete[]params;*/

		/*ld.count = count;
		ld.params = params;*/
	}
	else
	{
		/*ld.count = 0;
		ld.params = 0;*/
	}

	DWORD len;
	myFile.read((char *)&len,sizeof(len));
	ld.len = len;

	char *data = new char[len];
	myFile.read(data,len);
	ld.data = data;

	myFile.close();

	ld.size = sizeof(ld)/* + sizeof(*ld.params) * ld.count*/ + ld.len;
	m_cacheSize += ld.size;
	LoadedDataCache[pos] = ld;

	if (pLen) *pLen = ld.len;
	/*if (pitch1) *pitch1 = ld.count > 0 ? ld.params[0] : 0;
	if (pitch2) *pitch2 = ld.count > 1 ? ld.params[1] : 0;*/
	return (char *)ld.data;
}