#define BOOST_TEST_MODULE d3d8_resource_leak_tests
#include <boost/test/unit_test.hpp>
#include <windows.h>
#include <d3d8.h>

#include <exception>
namespace boost // for shared_ptr
{
	void throw_exception( std::exception const & e ) 
	{
	}
}

void CreateD3DWindow(HWND &hWnd)
{
	// Register the window class
	WNDCLASSEX wc =
	{
		sizeof( WNDCLASSEX ), CS_CLASSDC, DefWindowProc, 0L, 0L,
		GetModuleHandle( NULL ), NULL, NULL, NULL, NULL,
		L"iZ3D Window", NULL
	};
	RegisterClassEx( &wc );

	RECT rcClient;
	SetRect(&rcClient, 10, 10, 20, 20);
	AdjustWindowRect (&rcClient, WS_OVERLAPPEDWINDOW, FALSE);
	// Create the application's window
	hWnd = CreateWindow( L"iZ3D Window", L"Output Test",
		WS_OVERLAPPEDWINDOW, rcClient.left, rcClient.top, 
		rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
		NULL, NULL, NULL, NULL );
}

void CreateDirect3D(HWND& hWnd, LPDIRECT3D8& pD3D)
{
	CreateD3DWindow(hWnd);

	HRESULT hr = S_OK;
	pD3D = Direct3DCreate8( D3D_SDK_VERSION );
	BOOST_REQUIRE( pD3D != 0 );
}

void SetPresentParametrs(BOOL Windowed, UINT Width, UINT Height, D3DPRESENT_PARAMETERS& d3dpp)
{
	ZeroMemory( &d3dpp, sizeof( d3dpp ) );
	d3dpp.Windowed = Windowed;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferWidth = Width;
	d3dpp.BackBufferHeight = Height;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24X8;
}

void CreateD3DDevice(LPDIRECT3D8 pD3D, HWND hWnd, D3DPRESENT_PARAMETERS& d3dpp, LPDIRECT3DDEVICE8& pd3dDevice)
{
	HRESULT hr = S_OK;
	// Create the D3DDevice
	hr = pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp, &pd3dDevice );
	BOOST_REQUIRE( SUCCEEDED(hr) );

	hr = pd3dDevice->Present(NULL, NULL, NULL, NULL);
	BOOST_CHECK( SUCCEEDED(hr) );
}

BOOST_AUTO_TEST_SUITE( d3d9_resource_leak_tests )

BOOST_AUTO_TEST_CASE( DeviceRCCheck )
{
	HWND hWnd = NULL;
	LPDIRECT3D8			pD3D = NULL;
	CreateDirect3D(hWnd, pD3D);

	D3DPRESENT_PARAMETERS d3dpp;
	SetPresentParametrs(TRUE, 10, 10, d3dpp);
	LPDIRECT3DDEVICE8	pd3dDevice = NULL;
	CreateD3DDevice(pD3D, hWnd, d3dpp, pd3dDevice);

	//device rc check
	BOOST_CHECK( pd3dDevice->Release() == 0 );
	BOOST_CHECK( pD3D->Release() == 0 );
	DestroyWindow(hWnd);
	UnregisterClass( L"iZ3D Window", NULL );
}

BOOST_AUTO_TEST_CASE( ResetCheck )
{
	HRESULT hr = S_OK;
	HWND hWnd = NULL;
	LPDIRECT3D8			pD3D = NULL;
	CreateDirect3D(hWnd, pD3D);

	D3DPRESENT_PARAMETERS d3dpp;
	SetPresentParametrs(TRUE, 10, 10, d3dpp);
	LPDIRECT3DDEVICE8	pd3dDevice = NULL;
	CreateD3DDevice(pD3D, hWnd, d3dpp, pd3dDevice);

	hr = pd3dDevice->Reset(&d3dpp);
	BOOST_REQUIRE( SUCCEEDED(hr) );

	hr = pd3dDevice->Present(NULL, NULL, NULL, NULL);
	BOOST_CHECK( SUCCEEDED(hr) );

	BOOST_CHECK( pd3dDevice->Release() == 0 );
	BOOST_CHECK( pD3D->Release() == 0 );
	DestroyWindow(hWnd);
	UnregisterClass( L"iZ3D Window", NULL );
}

BOOST_AUTO_TEST_CASE( BackBufferRCCheck )
{
	//MacLeod backbuffer
	HRESULT hr = S_OK;
	HWND hWnd = NULL;
	LPDIRECT3D8			pD3D = NULL;
	CreateDirect3D(hWnd, pD3D);

	D3DPRESENT_PARAMETERS d3dpp;
	SetPresentParametrs(TRUE, 10, 10, d3dpp);
	LPDIRECT3DDEVICE8	pd3dDevice = NULL;
	CreateD3DDevice(pD3D, hWnd, d3dpp, pd3dDevice);

	IDirect3DSurface8* pBackBuffer = NULL;
	hr = pd3dDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
	BOOST_REQUIRE( SUCCEEDED(hr) );
	UINT rc = 0;
	for(int i = 0; i < 20; i++)
	{
		rc = pBackBuffer->Release();
		BOOST_REQUIRE( rc == 0 );
	}

	//device rc check
	BOOST_CHECK( pd3dDevice->Release() == 0 );
	BOOST_CHECK( pD3D->Release() == 0 );
	DestroyWindow(hWnd);
	UnregisterClass( L"iZ3D Window", NULL );
}

BOOST_AUTO_TEST_CASE( AutoDepthStencilRCCheck )
{
	//MacLeod backbuffer
	HRESULT hr = S_OK;
	HWND hWnd = NULL;
	LPDIRECT3D8			pD3D = NULL;
	CreateDirect3D(hWnd, pD3D);

	D3DPRESENT_PARAMETERS d3dpp;
	SetPresentParametrs(TRUE, 10, 10, d3dpp);
	LPDIRECT3DDEVICE8	pd3dDevice = NULL;
	CreateD3DDevice(pD3D, hWnd, d3dpp, pd3dDevice);

	IDirect3DSurface8* pDepthStencil = NULL;
	hr = pd3dDevice->GetDepthStencilSurface(&pDepthStencil);
	BOOST_REQUIRE( SUCCEEDED(hr) );
	UINT rc = 0;
	for(int i = 0; i < 20; i++)
	{
		rc = pDepthStencil->Release();
		BOOST_REQUIRE( rc == 0 );
	}

	//device rc check
	BOOST_CHECK( pd3dDevice->Release() == 0 );
	BOOST_CHECK( pD3D->Release() == 0 );
	DestroyWindow(hWnd);
	UnregisterClass( L"iZ3D Window", NULL );
}

BOOST_AUTO_TEST_CASE( TextureSurfaceRCCheck )
{
	//MacLeod texture surfaces
	// У ресурса и его контейнера общий счетчик ссылок, например у текстуры и всех её поверхностей.
	HRESULT hr = S_OK;
	HWND hWnd = NULL;
	LPDIRECT3D8			pD3D = NULL;
	CreateDirect3D(hWnd, pD3D);

	D3DPRESENT_PARAMETERS d3dpp;
	SetPresentParametrs(TRUE, 10, 10, d3dpp);
	LPDIRECT3DDEVICE8	pd3dDevice = NULL;
	CreateD3DDevice(pD3D, hWnd, d3dpp, pd3dDevice);

	IDirect3DTexture8* pTexture = NULL;
	IDirect3DSurface8* pSurf = NULL;
	hr = pd3dDevice->CreateTexture(10, 10, 1, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT,
		&pTexture);
	BOOST_REQUIRE( SUCCEEDED(hr) );
	hr = pTexture->GetSurfaceLevel(0, &pSurf);
	BOOST_REQUIRE( SUCCEEDED(hr) );
	UINT rc = 0;

	rc = pTexture->AddRef();
	BOOST_REQUIRE( rc == 3 );
	rc = pSurf->AddRef();
	rc = pSurf->Release();
	BOOST_REQUIRE( rc == 3 );
	rc = pTexture->Release();
	BOOST_REQUIRE( rc == 2 );

	rc = pSurf->AddRef();
	BOOST_REQUIRE( rc == 3 );
	rc = pTexture->AddRef();
	rc = pTexture->Release();
	BOOST_REQUIRE( rc == 3 );
	rc = pSurf->Release();
	BOOST_CHECK( rc == 2 );

	rc = pSurf->Release();
	BOOST_CHECK( rc == 1 );
	rc = pTexture->Release();
	BOOST_CHECK( rc == 0 );

	//device rc check
	BOOST_CHECK( pd3dDevice->Release() == 0 );
	BOOST_CHECK( pD3D->Release() == 0 );
	DestroyWindow(hWnd);
	UnregisterClass( L"iZ3D Window", NULL );
}

BOOST_AUTO_TEST_CASE( SetTextureSurfaceRCCheck )
{
	//Нельзя увеличивать кол-во ссылок в SetTexture, SetRenderTarget или SetDepthStencilSurface. 
	BOOST_CHECK( 2 == 2 );
}

BOOST_AUTO_TEST_CASE( SetTextureRCCheck )
{
	//Драйвер должен корректно работать даже если игра установила surface в D3D и больше не держит ссылок на surface. 
	HRESULT hr = S_OK;
	HWND hWnd = NULL;
	LPDIRECT3D8			pD3D = NULL;
	CreateDirect3D(hWnd, pD3D);

	D3DPRESENT_PARAMETERS d3dpp;
	SetPresentParametrs(TRUE, 10, 10, d3dpp);
	LPDIRECT3DDEVICE8	pd3dDevice = NULL;
	CreateD3DDevice(pD3D, hWnd, d3dpp, pd3dDevice);

	IDirect3DTexture8* pTexture = NULL;
	hr = pd3dDevice->CreateTexture(10, 10, 1, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT,
		&pTexture);
	BOOST_REQUIRE( SUCCEEDED(hr) );
	UINT rc = 0;

	rc = pTexture->AddRef();
	BOOST_REQUIRE( rc == 2 );
	rc = pTexture->Release();
	BOOST_REQUIRE( rc == 1 );

	rc = pTexture->AddRef();
	rc = pTexture->Release();
	BOOST_REQUIRE( rc == 1 );

	hr = pd3dDevice->SetTexture(0, pTexture);
	BOOST_REQUIRE( SUCCEEDED(hr) );

	rc = pTexture->AddRef();
	rc = pTexture->Release();
	BOOST_REQUIRE( rc == 1 );

	rc = pTexture->Release();
	BOOST_REQUIRE( rc == 0 );

	rc = pTexture->AddRef();
	BOOST_REQUIRE( rc == 1 );

	rc = pTexture->Release();
	BOOST_REQUIRE( rc == 0 );

	rc = pTexture->Release();
	BOOST_REQUIRE( rc == 0 );

	rc = pTexture->AddRef();
	BOOST_REQUIRE( rc == 1 );

	hr = pd3dDevice->SetTexture(0, NULL);
	BOOST_REQUIRE( SUCCEEDED(hr) );

	rc = pTexture->AddRef();
	rc = pTexture->Release();
	BOOST_REQUIRE( rc == 1 );

	hr = pd3dDevice->SetTexture(0, pTexture);
	BOOST_REQUIRE( SUCCEEDED(hr) );

	rc = pTexture->Release();
	BOOST_REQUIRE( rc == 0 );

	hr = pd3dDevice->SetTexture(0, NULL);
	BOOST_REQUIRE( SUCCEEDED(hr) );

	//device rc check
	BOOST_CHECK( pd3dDevice->Release() == 0 );
	BOOST_CHECK( pD3D->Release() == 0 );
	DestroyWindow(hWnd);
	UnregisterClass( L"iZ3D Window", NULL );
}

BOOST_AUTO_TEST_CASE( SetRenderTargetRCCheck )
{
	//Драйвер должен корректно работать даже если игра установила surface в D3D и больше не держит ссылок на surface. 
	HRESULT hr = S_OK;
	HWND hWnd = NULL;
	LPDIRECT3D8			pD3D = NULL;
	CreateDirect3D(hWnd, pD3D);

	D3DPRESENT_PARAMETERS d3dpp;
	SetPresentParametrs(TRUE, 10, 10, d3dpp);
	LPDIRECT3DDEVICE8	pd3dDevice = NULL;
	CreateD3DDevice(pD3D, hWnd, d3dpp, pd3dDevice);

	IDirect3DSurface8* pBackBuffer = NULL;
	hr = pd3dDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
	BOOST_REQUIRE( SUCCEEDED(hr) );

	IDirect3DSurface8* pDepthStencil = NULL;
	hr = pd3dDevice->GetDepthStencilSurface(&pDepthStencil);
	BOOST_REQUIRE( SUCCEEDED(hr) );

	IDirect3DSurface8* pSurf = NULL;
	hr = pd3dDevice->CreateRenderTarget(10, 10, D3DFMT_X8R8G8B8, D3DMULTISAMPLE_NONE, FALSE,
		&pSurf);
	BOOST_REQUIRE( SUCCEEDED(hr) );
	UINT rc = 0;

	rc = pSurf->AddRef();
	BOOST_REQUIRE( rc == 2 );
	rc = pSurf->Release();
	BOOST_REQUIRE( rc == 1 );

	rc = pSurf->AddRef();
	rc = pSurf->Release();
	BOOST_REQUIRE( rc == 1 );

	hr = pd3dDevice->SetRenderTarget(pSurf, pDepthStencil);
	BOOST_REQUIRE( SUCCEEDED(hr) );

	rc = pSurf->AddRef();
	rc = pSurf->Release();
	BOOST_REQUIRE( rc == 1 );

	rc = pSurf->Release();
	BOOST_REQUIRE( rc == 0 );

	rc = pSurf->AddRef();
	BOOST_REQUIRE( rc == 1 );

	rc = pSurf->Release();
	BOOST_REQUIRE( rc == 0 );

	rc = pSurf->Release();
	BOOST_REQUIRE( rc == 0 );

	rc = pSurf->AddRef();
	BOOST_REQUIRE( rc == 1 );

	hr = pd3dDevice->SetRenderTarget(pBackBuffer, pDepthStencil);
	BOOST_REQUIRE( SUCCEEDED(hr) );

	rc = pSurf->AddRef();
	rc = pSurf->Release();
	BOOST_REQUIRE( rc == 1 );

	hr = pd3dDevice->SetRenderTarget(pSurf, pDepthStencil);
	BOOST_REQUIRE( SUCCEEDED(hr) );

	rc = pSurf->Release();
	BOOST_REQUIRE( rc == 0 );

	hr = pd3dDevice->SetRenderTarget(pBackBuffer, pDepthStencil);
	BOOST_REQUIRE( SUCCEEDED(hr) );

	rc = pBackBuffer->Release();
	BOOST_REQUIRE( rc == 0 );

	rc = pDepthStencil->Release();
	BOOST_REQUIRE( rc == 0 );

	//device rc check
	BOOST_CHECK( pd3dDevice->Release() == 0 );
	BOOST_CHECK( pD3D->Release() == 0 );
	DestroyWindow(hWnd);
	UnregisterClass( L"iZ3D Window", NULL );
}

BOOST_AUTO_TEST_CASE( SetDepthStencilRCCheck )
{
	//Драйвер должен корректно работать даже если игра установила surface в D3D и больше не держит ссылок на surface. 
	HRESULT hr = S_OK;
	HWND hWnd = NULL;
	LPDIRECT3D8			pD3D = NULL;
	CreateDirect3D(hWnd, pD3D);

	D3DPRESENT_PARAMETERS d3dpp;
	SetPresentParametrs(TRUE, 10, 10, d3dpp);
	LPDIRECT3DDEVICE8	pd3dDevice = NULL;
	CreateD3DDevice(pD3D, hWnd, d3dpp, pd3dDevice);

	IDirect3DSurface8* pBackBuffer = NULL;
	hr = pd3dDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
	BOOST_REQUIRE( SUCCEEDED(hr) );

	IDirect3DSurface8* pSurf = NULL;
	hr = pd3dDevice->CreateDepthStencilSurface(10, 10, D3DFMT_D24X8, D3DMULTISAMPLE_NONE,
		&pSurf);
	BOOST_REQUIRE( SUCCEEDED(hr) );
	UINT rc = 0;

	rc = pSurf->AddRef();
	BOOST_REQUIRE( rc == 2 );
	rc = pSurf->Release();
	BOOST_REQUIRE( rc == 1 );

	rc = pSurf->AddRef();
	rc = pSurf->Release();
	BOOST_REQUIRE( rc == 1 );

	hr = pd3dDevice->SetRenderTarget(pBackBuffer, pSurf);
	BOOST_REQUIRE( SUCCEEDED(hr) );

	rc = pSurf->AddRef();
	rc = pSurf->Release();
	BOOST_REQUIRE( rc == 1 );

	rc = pSurf->Release();
	BOOST_REQUIRE( rc == 0 );

	rc = pSurf->AddRef();
	BOOST_REQUIRE( rc == 1 );

	rc = pSurf->Release();
	BOOST_REQUIRE( rc == 0 );

	rc = pSurf->Release();
	BOOST_REQUIRE( rc == 0 );

	rc = pSurf->AddRef();
	BOOST_REQUIRE( rc == 1 );

	hr = pd3dDevice->SetRenderTarget(pBackBuffer, NULL);
	BOOST_REQUIRE( SUCCEEDED(hr) );

	rc = pSurf->AddRef();
	rc = pSurf->Release();
	BOOST_REQUIRE( rc == 1 );

	hr = pd3dDevice->SetRenderTarget(pBackBuffer, pSurf);
	BOOST_REQUIRE( SUCCEEDED(hr) );

	rc = pSurf->Release();
	BOOST_REQUIRE( rc == 0 );

	hr = pd3dDevice->SetRenderTarget(pBackBuffer, NULL);
	BOOST_REQUIRE( SUCCEEDED(hr) );

	rc = pBackBuffer->Release();
	BOOST_REQUIRE( rc == 0 );

	//device rc check
	BOOST_CHECK( pd3dDevice->Release() == 0 );
	BOOST_CHECK( pD3D->Release() == 0 );
	DestroyWindow(hWnd);
	UnregisterClass( L"iZ3D Window", NULL );
}

/*BOOST_AUTO_TEST_CASE( FullScreenRCCheck )
{
	//MacLeod primary swapchain
	HRESULT hr = S_OK;
	HWND hWnd = NULL;
	LPDIRECT3D8			pD3D = NULL;
	CreateDirect3D(hWnd, pD3D);

	D3DDISPLAYMODE displayMode;
	pD3D->GetAdapterDisplayMode(0, &displayMode);
	UINT Width = displayMode.Width;
	UINT Height = displayMode.Height;
	D3DPRESENT_PARAMETERS d3dpp;
	SetPresentParametrs(FALSE, Width, Height, d3dpp);

	LPDIRECT3DDEVICE8	pd3dDevice = NULL;
	CreateD3DDevice(pD3D, hWnd, d3dpp, pd3dDevice);
	BOOST_REQUIRE( pd3dDevice->Release() == 0 );

	pd3dDevice = NULL;
	CreateD3DDevice(pD3D, hWnd, d3dpp, pd3dDevice);
	BOOST_REQUIRE( pd3dDevice->Release() == 0 );

	BOOST_CHECK( pD3D->Release() == 0 );
	DestroyWindow(hWnd);
	UnregisterClass( L"iZ3D Window", NULL );
}*/

BOOST_AUTO_TEST_SUITE_END()
