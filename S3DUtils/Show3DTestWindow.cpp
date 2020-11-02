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
#include <d3d9.h>
#include <D3dx9tex.h>
#include "Show3DTestWindow.h"
#include "SetupS3DMonitor.h"
#include "resource.h"
#include "..\ArchiveFile\Blob.h"
#include "..\CommonUtils\StringResourceManager.h"
#include "..\CommonUtils\CommonResourceFolders.h"
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include "..\S3DAPI\iStereoAPI.h"

#define	 CLASS_NAME	_T("S3DWindow")
#define	 TEXT_OFFSET_TOP	68
#define	 TEXT_OFFSET_BOTTOM	70

bool __declspec(dllexport) CALLBACK FindMonitor( HMONITOR &hBack, HMONITOR &hFront )
{
	hBack = NULL;
	hFront = NULL;
	MonitorPanel	first, second;
	MonitorPanel	*pPrimary, *pSecondary;
	pPrimary = &first;
	pSecondary = &second;
	DetectPanels(pPrimary, pSecondary);
	if (!pPrimary->bFound || !pPrimary->bAttached || !pSecondary->bFound || !pSecondary->bAttached)
		return false;

	if(!pPrimary->bPrimary && pSecondary->bPrimary)
	{
		MonitorPanel	*pTemp;
		pTemp = pPrimary;
		pPrimary = pSecondary;
		pSecondary = pTemp;
	};

	pPrimary->GetDisplaySettings();
	hBack = pPrimary->GetMonitorHandle();
	pSecondary->GetDisplaySettings();
	hFront = pSecondary->GetMonitorHandle();

	return hBack && hFront;
}

void SignImage(HDC hDC, RECT* rect)
{
	using namespace iz3d::resources;
	TStringResourceSingleton::instance().setLanguage( GetCurrentLanguage().c_str() );
	//--- read localized strings ---
	const wchar_t* strL = TStringResourceSingleton::instance().getString( _T("\\Utils\\StereoTestUp") );
	const wchar_t* strS = TStringResourceSingleton::instance().getString( _T("\\Utils\\StereoTestDown") );

	//--- modify the bitmap ---
	SetTextColor(hDC, RGB(0xFF, 0xFF, 0xFF));
	SetBkColor(hDC, 0);
	HFONT hFontL = CreateFontW(64,    // Height
		0,                            // Width
		0,                            // Escapement
		0,                            // Orientation
		FW_BOLD,                      // Weight
		FALSE,                        // Italic
		FALSE,                        // Underline
		0,                            // StrikeOut
		DEFAULT_CHARSET,              // CharSet
		OUT_DEFAULT_PRECIS,           // OutPrecision
		CLIP_DEFAULT_PRECIS,          // ClipPrecision
		ANTIALIASED_QUALITY,          // Quality
		DEFAULT_PITCH,                // PitchAndFamily
		_T("Arial Unicode MS"));      // Facename
	HFONT hFontS = CreateFontW(32,    // Height
		0,                            // Width
		0,                            // Escapement
		0,                            // Orientation
		FW_BOLD,                      // Weight
		FALSE,                        // Italic
		FALSE,                        // Underline
		0,                            // StrikeOut
		DEFAULT_CHARSET,              // CharSet
		OUT_DEFAULT_PRECIS,           // OutPrecision
		CLIP_DEFAULT_PRECIS,          // ClipPrecision
		ANTIALIASED_QUALITY,          // Quality
		DEFAULT_PITCH,                // PitchAndFamily
		_T("Arial Unicode MS"));      // Facename

	RECT rcL = { rect->left, rect->top, rect->right, TEXT_OFFSET_TOP };
	HGDIOBJ old = SelectObject(hDC, hFontL);
	DrawText(hDC, strL, (int)wcslen(strL), &rcL, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	RECT rcS = { rect->left, rect->bottom - TEXT_OFFSET_BOTTOM, rect->right, rect->bottom };
	SelectObject(hDC, hFontS);
	DrawText(hDC, strS, (int)wcslen(strS), &rcS, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	SelectObject(hDC, old);
	DeleteObject(hFontS);
	DeleteObject(hFontL);
}

HRESULT CreateTextureFromResourcePack(LPCTSTR szResName, IDirect3DDevice9* pd3dDevice, IDirect3DTexture9** texture, DWORD usage, D3DFORMAT format, D3DPOOL pool)
{
	HRESULT hr = E_FAIL;
	Blob blob;
	TCHAR* szTemp = (TCHAR*)szResName;
	if (!blob.Initialize( szResName ))
		return hr;

	D3DXIMAGE_INFO srcInfo;
	hr = D3DXCreateTextureFromFileInMemoryEx(pd3dDevice, blob.GetData(), (UINT)blob.GetSize(),  D3DX_DEFAULT, D3DX_DEFAULT,
		1, 0, format, pool, D3DX_DEFAULT , D3DX_DEFAULT, 0, &srcInfo, 0, texture);
	if(FAILED(hr))
		return hr;
	(*texture)->Release();
	return D3DXCreateTextureFromFileInMemoryEx(pd3dDevice, blob.GetData(), (UINT)blob.GetSize(), srcInfo.Width, 
		srcInfo.Height, 1, usage, format, pool, D3DX_DEFAULT , D3DX_DEFAULT, 0, &srcInfo, 0, texture);
}

//--- make a frame for signature ---
CComPtr<IDirect3DSurface9> textureSurface;

struct App3DResources
{
	App3DResources()	{ m_hWnd = 0; }
	~App3DResources()	{ Clear();    }
	HRESULT	Create();
	void	Clear();
	void	InitWnd(UINT width, UINT height);
	HRESULT	InitD3D();
	HRESULT	CreatePoolDefaultResources();
	void	ClearPoolDefaultResources();
	
	HWND						m_hWnd;
	CComPtr<IDirect3D9>			m_pD3D;			
	CComPtr<IDirect3DDevice9>	m_pd3dDevice;
	D3DPRESENT_PARAMETERS		m_d3dpp;

	CComPtr<IDirect3DSurface9>	m_backBuffer; 
	CComPtr<IDirect3DSurface9>	m_Surface; 
};

HRESULT	App3DResources::Create()
{
	if(FAILED(InitD3D()))
		return E_FAIL;

	ShowWindow(m_hWnd, SW_SHOW);
	return S_OK;
}

void App3DResources::Clear()
{	
	ClearPoolDefaultResources();
	m_pd3dDevice = 0;
	m_pD3D = 0;
	DestroyWindow(m_hWnd);
	UnregisterClass(CLASS_NAME, g_hModule);
}

HRESULT	App3DResources::CreatePoolDefaultResources()
{
	
	CComPtr<IDirect3DTexture9> image;
	CComPtr<IDirect3DSurface9> imageSurface;

	//--- load image ---
	if(FAILED(CreateTextureFromResourcePack(_T("Utils/StaticTestImage.jps"), m_pd3dDevice, &image, 0, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT)))
		return E_FAIL;

	if(FAILED(m_pd3dDevice->CreateRenderTarget(2 * m_d3dpp.BackBufferWidth, m_d3dpp.BackBufferHeight, D3DFMT_X8R8G8B8, D3DMULTISAMPLE_NONE, 0, TRUE, &m_Surface, NULL)))
		return E_FAIL;

	//--- resize image and keep aspect ratio ---
	image->GetSurfaceLevel(0, &imageSurface );
	if(FAILED(m_pd3dDevice->ColorFill(m_Surface, 0, 0)))
		return E_FAIL;

	//--- copy image ---
	D3DSURFACE_DESC desc;
	imageSurface->GetDesc(&desc);
	RECT scrRect = { 0, 0, desc.Width/2, desc.Height };
	RECT dstRect;
	LONG SrcDx = desc.Width / 2;
	LONG SrcDy = desc.Height;
	LONG DstDx = m_d3dpp.BackBufferWidth;
	LONG DstDy = m_d3dpp.BackBufferHeight - (TEXT_OFFSET_TOP + TEXT_OFFSET_BOTTOM);
	if(float(SrcDx) / SrcDy >= float(DstDx) /DstDy)
	{
		LONG delta = LONG((DstDy - float(SrcDy) * DstDx / SrcDx) / 2);
		dstRect.left = 0; 
		dstRect.top = TEXT_OFFSET_TOP + delta; 
		dstRect.right = m_d3dpp.BackBufferWidth; 
		dstRect.bottom = m_d3dpp.BackBufferHeight - TEXT_OFFSET_BOTTOM - delta; 
	}
	else
	{
		LONG delta = LONG((DstDx - float(SrcDx) * DstDy / SrcDy) / 2);
		dstRect.left = delta; 
		dstRect.top = TEXT_OFFSET_TOP; 
		dstRect.right = m_d3dpp.BackBufferWidth - delta; 
		dstRect.bottom = m_d3dpp.BackBufferHeight - TEXT_OFFSET_BOTTOM; 
	}
	if(FAILED(m_pd3dDevice->StretchRect(imageSurface, &scrRect, m_Surface, &dstRect, D3DTEXF_LINEAR)))
		return E_FAIL;
	OffsetRect(&scrRect, desc.Width/2, 0);
	OffsetRect(&dstRect, m_d3dpp.BackBufferWidth, 0);
	if(FAILED(m_pd3dDevice->StretchRect(imageSurface, &scrRect, m_Surface, &dstRect, D3DTEXF_LINEAR)))
		return E_FAIL;

	//--- sing image by text ---
	HDC hDC; 
	RECT rect = { 0, 0, m_d3dpp.BackBufferWidth, m_d3dpp.BackBufferHeight };
	m_Surface->GetDC(&hDC);
	SignImage(hDC, &rect);
	OffsetRect(&rect, m_d3dpp.BackBufferWidth, 0);
	SignImage(hDC, &rect);
	m_Surface->ReleaseDC(hDC);

	m_pd3dDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &m_backBuffer);

	return S_OK;
}

void App3DResources::ClearPoolDefaultResources()
{
	m_backBuffer = 0;
	m_Surface = 0;
}

void App3DResources::InitWnd(UINT width, UINT height)
{
	WNDCLASS wndClass;
	if(!GetClassInfo(g_hModule, CLASS_NAME, &wndClass))
	{
		WNDCLASS wClass = { 0,DefWindowProc,0,0,g_hModule,NULL,LoadCursor(NULL,IDC_ARROW),(HBRUSH)GetStockObject(BLACK_BRUSH),NULL,CLASS_NAME };
		RegisterClass( &wClass );
	}

	DWORD dwStyle = WS_POPUP;
	DWORD dwExStyle = WS_EX_TOPMOST|WS_EX_APPWINDOW;
	m_hWnd = CreateWindowEx(dwExStyle, CLASS_NAME, _T("3DShow"), dwStyle, 0, 0, width, height, 0, 0, g_hModule, 0);
}

HRESULT App3DResources::InitD3D()
{
	m_pD3D.Attach(Direct3DCreate9(D3D_SDK_VERSION));
	if(!m_pD3D)
		return E_FAIL;

	D3DDISPLAYMODE mode;
	m_pD3D->GetAdapterDisplayMode(0, &mode);

	InitWnd(mode.Width, mode.Height);
	if(!m_hWnd)
		return E_FAIL;

	ZeroMemory( &m_d3dpp, sizeof( m_d3dpp ) );
	m_d3dpp.Windowed = FALSE;
	m_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	m_d3dpp.BackBufferFormat = mode.Format;
	m_d3dpp.BackBufferWidth = mode.Width;
	m_d3dpp.BackBufferHeight = mode.Height;
	m_d3dpp.EnableAutoDepthStencil = FALSE;
	m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	if(FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &m_d3dpp, &m_pd3dDevice)))
		return E_FAIL;

	return S_OK;
}

bool __declspec(dllexport) CALLBACK Show3DTestWindow()
{
	App3DResources AppRes;
	if(FAILED(AppRes.Create()))
		return false;

	//--- we must load 3D API  after creating D3D9 ---
	CComPtr<IStereoAPI> pAPI;
	HMODULE hModule = GetStereoLibraryHandle();
	if(SUCCEEDED(CreateStereoAPI(hModule, &pAPI)))
	{		
		pAPI->SetShowOSDOn(false);
		pAPI->SetShowFPSOn(false);
		pAPI->SetStereoActive(true);
		pAPI->SetRenderTargetCreationMode(0);
	}

	if(FAILED(AppRes.CreatePoolDefaultResources()))
		return false;

	while(1)
	{
		MSG msg;
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT || (msg.message == WM_KEYDOWN && (msg.wParam == VK_ESCAPE)))
				break;
			DispatchMessage(&msg);
		}

		HRESULT hr = AppRes.m_pd3dDevice->TestCooperativeLevel();
		if(hr == D3D_OK)
		{
			RECT rect = { 0, 0, AppRes.m_d3dpp.BackBufferWidth, AppRes.m_d3dpp.BackBufferHeight };
			if(pAPI)
			{
				//--- blit right eye ---
				pAPI->SetBltDstEye(1); 
				AppRes.m_pd3dDevice->StretchRect(AppRes.m_Surface, &rect, AppRes.m_backBuffer, 0, D3DTEXF_LINEAR);
				//--- blit left eye ---
				pAPI->SetBltDstEye(0); 
				OffsetRect(&rect, AppRes.m_d3dpp.BackBufferWidth, 0);
			}
			AppRes.m_pd3dDevice->StretchRect(AppRes.m_Surface, &rect, AppRes.m_backBuffer, 0, D3DTEXF_LINEAR);
			AppRes.m_pd3dDevice->Present(0, 0, 0, 0);
		}
		else if(hr == D3DERR_DEVICENOTRESET)
		{
			AppRes.ClearPoolDefaultResources();
			AppRes.m_pd3dDevice->Reset(&AppRes.m_d3dpp);
			AppRes.CreatePoolDefaultResources();
		}

		Sleep(5);
	}
	
	if(hModule)
		pAPI = 0;
	return true;
}

void __declspec(dllexport) CALLBACK Show3D(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow)
{
	Show3DTestWindow();
}
