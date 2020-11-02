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
#define COMPILE_MULTIMON_STUBS
#include <mmsystem.h>
#include "SynchroWindow.h"
#include "resource.h"
#include "ProductNames.h"
#include "..\ArchiveFile\Blob.h"
#include "..\CommonUtils\StringResourceManager.h"
#include "..\CommonUtils\CommonResourceFolders.h"
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>

#define	 CLASS_NAME	_T("S3DTestWindow")
#define	 SAFE_DESTROYWINDOW(a) if(a) { DestroyWindow(a); a = 0; }
#define	 SAFE_DELETEOBJECT(a)  if(a) { DeleteObject(a);  a = 0; }

HRESULT LoadBitmapFromResourcePack( LPCTSTR szResName, HBITMAP* hBitmap, IPicture** picture)
{
	HRESULT hr = E_FAIL;
	Blob blob;
	TCHAR* szTemp = (TCHAR*)szResName;
	if (!blob.Initialize( szResName ))
		return hr;

	DWORD nResSize		= (DWORD)blob.GetSize();
	HGLOBAL hGlobal		= GlobalAlloc( GMEM_MOVEABLE, nResSize );
	LPVOID pGlobalData	= GlobalLock( hGlobal );	
	memcpy( pGlobalData, blob.GetData(), nResSize );
	GlobalUnlock( hGlobal );
	CComPtr<IStream> pStream;
	(*picture) = NULL;
	if( SUCCEEDED( CreateStreamOnHGlobal( hGlobal, TRUE, &pStream )) )
	{
		if( SUCCEEDED( OleLoadPicture( pStream , nResSize, FALSE, IID_IPicture, (void**)picture )))
			hr = (*picture)->get_Handle((OLE_HANDLE*)hBitmap );
	}
	GlobalFree( hGlobal );

	return hr;
}

HRESULT LoadBitmapFromResource(LPCTSTR lpName, LPCTSTR lpType, HBITMAP* hBitmap, IPicture** picture)
{
	HRESULT hr = E_FAIL;
	if(HRSRC hRSRC = FindResource(g_hModule, lpName, lpType))
	{
		if(HGLOBAL hResource = LoadResource(g_hModule, hRSRC))
		{
			LPVOID pvResData, pvGlobalData;
			DWORD resSize = SizeofResource(g_hModule, hRSRC);
			HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, resSize);
			pvGlobalData = GlobalLock(hGlobal);
			pvResData = LockResource(hResource);
			memcpy(pvGlobalData, pvResData, resSize);
			GlobalUnlock(hGlobal);
			FreeResource(hResource); 
			CComPtr<IStream> pStream;
			(*picture) = NULL;
			if(SUCCEEDED(CreateStreamOnHGlobal(hGlobal, TRUE, &pStream)))
			{
				if(SUCCEEDED(OleLoadPicture(pStream , resSize, FALSE, IID_IPicture, (void**)picture)))
					hr = (*picture)->get_Handle((OLE_HANDLE*)hBitmap);
			}
			GlobalFree(hGlobal);
		}
	}
	return hr;
}

std::wstring GetConfigFullName()
{
	wchar_t path[MAX_PATH];
	iz3d::resources::GetAllUsersDirectory( path );
	std::wstring newPath(path);
	newPath.append(L"Config.xml");
	return newPath;
}

std::wstring GetCurrentLanguage()
{
	using namespace xercesc;
	using namespace std;
	XMLPlatformUtils::Initialize();

	XercesDOMParser* pParser = new XercesDOMParser();
	std::wstring configName = GetConfigFullName();
	pParser->parse( configName.c_str() );
	xercesc::DOMDocument* pXmlDoc = pParser->getDocument();
	DOMXPathEvaluator* xpath = (DOMXPathEvaluator*) pXmlDoc; 
	DOMXPathResult *xpres = xpath->evaluate(L"/Config/GlobalSettings/Language", 
		pXmlDoc->getDocumentElement(), NULL, DOMXPathResult::FIRST_ORDERED_NODE_TYPE , NULL); 
	DOMNode* node = xpres->getNodeValue();
	std::wstring str = node->getAttributes()->getNamedItem(L"Value")->getNodeValue();
	delete pParser;
	XMLPlatformUtils::Terminate();
	return str;
}

LRESULT CALLBACK SynchroWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SynchroWindow* p = (SynchroWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	switch(uMsg)
	{
	case WM_CLOSE: 
		return 0;
		break;
	case WM_PAINT: 
		p->onPaint(hWnd);
		return 0;
		break;
	case WM_KEYDOWN: 
		if(p->m_ExitOnESC && wParam == VK_ESCAPE)
		{
			DestroyWindow(p->m_BackScreenWnd);
			DestroyWindow(p->m_FrontScreenWnd);
			p->m_BackScreenWnd = 0;
			p->m_FrontScreenWnd = 0;
			return 0;
		}
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

SynchroWindow::SynchroWindow(void)
{
	m_BackScreenBitmap = 0;
	m_FrontScreenBitmap = 0;
	m_FrontScreenWnd = 0;
	m_BackScreenWnd  = 0;
	m_ExitOnESC = TRUE;
}

SynchroWindow::~SynchroWindow(void)
{
	clear();
}

void SynchroWindow::SetMonitorData( BITMAP &info, RECT &mr, RECT &outImageRect )
{
	RECT imageRect;
	imageRect.left   = /*mr.left	+*/ (mr.right  - mr.left - info.bmWidth)  / 2;
	imageRect.right  = /*mr.left	+*/ (mr.right  - mr.left + info.bmWidth)  / 2;
	imageRect.top    = /*mr.top	+*/ (mr.bottom - mr.top  - info.bmHeight) / 2;
	imageRect.bottom = /*mr.top	+*/ (mr.bottom - mr.top  + info.bmHeight) / 2;
	mr.right  -= mr.left;
	mr.bottom -= mr.top;
	mr.left    = 0;
	mr.top     = 0;
	IntersectRect(&outImageRect, &imageRect, &mr);
}

void SynchroWindow::initialize(HMODULE hModule, HMONITOR hBack, HMONITOR hFront, HBITMAP backScreenBitmap, HBITMAP frontScreenBitmap, BOOL exitOnESC)
{
	m_ExitOnESC = exitOnESC;
	m_ThisModule = hModule;
	m_BackScreenBitmap = backScreenBitmap;
	m_FrontScreenBitmap = frontScreenBitmap;
	WNDCLASS wndClass = { 0,SynchroWindowProc,0,0,m_ThisModule,NULL,LoadCursor(NULL,IDC_ARROW),(HBRUSH)GetStockObject(BLACK_BRUSH),NULL,CLASS_NAME };
	m_WndClass = wndClass;
	RegisterClass( &m_WndClass );

	BITMAP info;
	MONITORINFOEX mi; 
	mi.cbSize = sizeof(MONITORINFOEX);

	GetMonitorInfo(hBack, &mi);
	m_BackScreenRect = mi.rcMonitor;
	GetObject(m_BackScreenBitmap, sizeof(BITMAP), &info);
	SetMonitorData(info, mi.rcMonitor, m_BackImageRect);

	GetMonitorInfo(hFront, &mi);
	m_FrontScreenRect = mi.rcMonitor;
	GetObject(m_FrontScreenBitmap, sizeof(BITMAP), &info);
	SetMonitorData(info, mi.rcMonitor, m_FrontImageRect);
}

bool SynchroWindow::show()
{
	if(m_BackScreenBitmap && m_FrontScreenBitmap && !m_BackScreenWnd && !m_FrontScreenWnd)
	{
		int x1 = m_BackScreenRect.left;
		int y1 = m_BackScreenRect.top;
		int w1 = m_BackScreenRect.right - m_BackScreenRect.left;
		int h1 = m_BackScreenRect.bottom - m_BackScreenRect.top;
#ifdef _DEBUG
		w1 /= 4;
		h1 /= 4;
#endif 
		if(!IsRectEmpty(&m_BackScreenRect))
			m_BackScreenWnd = CreateWindow( CLASS_NAME, _T("Back screen"), WS_POPUP, x1,y1,w1,h1,0, NULL,NULL, 0 );
		int x2 = m_FrontScreenRect.left;
		int y2 = m_FrontScreenRect.top;
		int w2 = m_FrontScreenRect.right - m_FrontScreenRect.left;
		int h2 = m_FrontScreenRect.bottom - m_FrontScreenRect.top;
#ifdef _DEBUG
		w2 /= 4;
		h2 /= 4;
#endif 
		if(!IsRectEmpty(&m_FrontScreenRect))
			m_FrontScreenWnd = CreateWindow( CLASS_NAME, _T("Front screen"), WS_POPUP, x2,y2,w2,h2,0, NULL,NULL, 0 );
		if(m_BackScreenWnd)
		{
			SetWindowLongPtr(m_BackScreenWnd,  GWLP_USERDATA, (LONG_PTR)this);
			SetWindowPos(m_BackScreenWnd,  HWND_TOPMOST, x1,y1,w1,h1, SWP_SHOWWINDOW);
			SetFocus(m_BackScreenWnd);
		}
		if(m_FrontScreenWnd)
		{
			SetWindowLongPtr(m_FrontScreenWnd, GWLP_USERDATA, (LONG_PTR)this);
			SetWindowPos(m_FrontScreenWnd, HWND_TOPMOST, x2,y2,w2,h2, SWP_SHOWWINDOW);
		}
	}
	return m_FrontScreenWnd && m_BackScreenWnd;
}

void SynchroWindow::clear()
{
	SAFE_DESTROYWINDOW(m_FrontScreenWnd);
	SAFE_DESTROYWINDOW(m_BackScreenWnd);
	SAFE_DELETEOBJECT(m_BackScreenBitmap);
	SAFE_DELETEOBJECT(m_FrontScreenBitmap);
	UnregisterClass(CLASS_NAME, m_ThisModule);
}

void SynchroWindow::onPaint(HWND hWnd)
{
	HDC			hdc; 
	PAINTSTRUCT ps;
	HGDIOBJ		old;
	RECT*		imageRect;
	hdc = BeginPaint(hWnd, &ps);
	HDC mdc = CreateCompatibleDC(hdc);
	if(hWnd == m_BackScreenWnd)
	{
		imageRect = &m_BackImageRect;
		old = SelectObject(mdc, (HGDIOBJ)m_BackScreenBitmap);
	}
	else if(hWnd == m_FrontScreenWnd)
	{
		imageRect = &m_FrontImageRect;
		old = SelectObject(mdc, (HGDIOBJ)m_FrontScreenBitmap);
	}
	else return;
	int x = imageRect->left;
	int y = imageRect->top;
	int w = imageRect->right  - imageRect->left;
	int h = imageRect->bottom - imageRect->top;
	BitBlt(hdc, x,y,w,h, mdc, 0,0, SRCCOPY);
	SelectObject(mdc, old);
	DeleteDC(mdc);
	EndPaint(hWnd, &ps); 
}
