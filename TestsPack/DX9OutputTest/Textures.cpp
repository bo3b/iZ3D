#include <Windows.h>
#include <mmsystem.h>
#include <d3dx9.h>
#include <tchar.h>
#pragma warning( disable : 4996 ) // disable deprecated warning 
#include <strsafe.h>
#pragma warning( default : 4996 )
#pragma warning( disable : 4995 ) // disable deprecated warning 
#include <atlbase.h>
#pragma warning( default : 4995 )
#include "SharedInclude.h" 

#define D3DFVF_D3DTVERTEX_1T  (D3DFVF_XYZRHW|D3DFVF_TEX1|D3DFVF_TEXCOORDSIZE2(0)) 
struct  D3DTVERTEX_2T         { float x,y,z,rhw; float tu,tv; };

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
LPDIRECT3D9             g_pD3D = NULL; // Used to create the D3DDevice
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL; // Our rendering device
D3DTVERTEX_2T			g_Vertex[4]; // Buffer to hold vertices
LPDIRECT3DTEXTURE9      g_pTexture = NULL; // Our texture
LPDIRECT3DTEXTURE9      g_pTextureL = NULL; // Our texture
LPDIRECT3DTEXTURE9      g_pTextureR = NULL; // Our texture
HWND					g_hWnd = NULL;
LPCTSTR					g_FileName = NULL;
//-----------------------------------------------------------------------------
// Name: InitD3D()
// Desc: Initializes Direct3D
//-----------------------------------------------------------------------------
HRESULT InitD3D( )
{
    // Create the D3D object.
    if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
        return E_FAIL;

    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory( &d3dpp, sizeof( d3dpp ) );
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.BackBufferWidth = 2;
	d3dpp.BackBufferHeight = 2;
	d3dpp.EnableAutoDepthStencil = FALSE;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

    // Create the D3DDevice
    if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_hWnd,
                                      D3DCREATE_HARDWARE_VERTEXPROCESSING,
                                      &d3dpp, &g_pd3dDevice ) ) )
    {
        return E_FAIL;
    }

	g_pd3dDevice->SetTexture(0, 0);
	g_pd3dDevice->SetTexture(1, 0);
	g_pd3dDevice->SetTexture(2, 0);
	g_pd3dDevice->SetRenderState(D3DRS_WRAP0,               0);
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE,             FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,    FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE,     FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_FOGENABLE,           FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_FILLMODE,            D3DFILL_SOLID);
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE,            D3DCULL_NONE);
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING,			FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_STENCILENABLE,		FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_CLIPPING,			FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_CLIPPLANEENABLE,		FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_SRGBWRITEENABLE,		FALSE);
	for(int i=0; i < 3; i++)
	{
		g_pd3dDevice->SetTextureStageState(i, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE); 
		g_pd3dDevice->SetTextureStageState(i, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );	
		g_pd3dDevice->SetTextureStageState(i, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		g_pd3dDevice->SetTextureStageState(i, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
		g_pd3dDevice->SetSamplerState( i, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
		g_pd3dDevice->SetSamplerState( i, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
		g_pd3dDevice->SetSamplerState( i, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
		g_pd3dDevice->SetSamplerState( i, D3DSAMP_MINFILTER, D3DTEXF_POINT );
		g_pd3dDevice->SetSamplerState( i, D3DSAMP_MIPMAPLODBIAS, 0 );
		g_pd3dDevice->SetSamplerState( i, D3DSAMP_MAXMIPLEVEL, 0);
		g_pd3dDevice->SetSamplerState( i, D3DSAMP_MAXANISOTROPY, 1);
		g_pd3dDevice->SetSamplerState( i, D3DSAMP_SRGBTEXTURE, 0);
		g_pd3dDevice->SetSamplerState( i, D3DSAMP_ELEMENTINDEX, 0);
		g_pd3dDevice->SetSamplerState( i, D3DSAMP_DMAPOFFSET, 0);
	}
	g_pd3dDevice->SetSamplerState( 2, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
	g_pd3dDevice->SetSamplerState( 2, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

	g_pd3dDevice->SetFVF( D3DFVF_D3DTVERTEX_1T );

    return S_OK;
}

typedef VOID (__stdcall *Func)();

BOOL WINAPI iZ3DDisableCreatingStereoResources()
{
	HMODULE hdll = GetModuleHandle(_T("S3DWrapperD3D9.dll"));
	if (hdll)
	{
		Func DisableCreatingStereoResources = (Func)GetProcAddress(hdll, "DisableCreatingStereoResources");
		if (DisableCreatingStereoResources)
		{
			DisableCreatingStereoResources();
			return true;
		}
	}
	return false;
}

BOOL WINAPI iZ3DRestoreCreatingStereoResourcesMode()
{
	HMODULE hdll = GetModuleHandle(_T("S3DWrapperD3D9.dll"));
	if (hdll)
	{
		Func RestoreCreatingStereoResourcesMode = (Func)GetProcAddress(hdll, "RestoreCreatingStereoResourcesMode");
		if (RestoreCreatingStereoResourcesMode)
		{
			RestoreCreatingStereoResourcesMode();
			return true;
		}
	}
	return false;
}

// {88D955D4-E26F-4092-8023-44D4621A92DD}
static const GUID BaseStereoRenderer_GUID = 
{ 0x88D955D4, 0xE26F, 0x4092, { 0x80, 0x23, 0x44, 0xD4, 0x62, 0x1A, 0x92, 0xDD } };

enum TextureType {Mono, Stereo, Cleared};
// {CCDB1CDD-1401-48cf-A763-6E362EB394D8}
static const GUID TextureType_GUID = 
{ 0xccdb1cdd, 0x1401, 0x48cf, { 0xa7, 0x63, 0x6e, 0x36, 0x2e, 0xb3, 0x94, 0xd8 } };

HRESULT SetStereoObject(IDirect3DSurface9 *pLeft, IDirect3DSurface9 *pRight)
{
	_ASSERT(pLeft);
	if (pRight)
	{
		TextureType type = Stereo;
		pLeft->SetPrivateData(TextureType_GUID, (void *)&type, sizeof(TextureType), 0);
		return pLeft->SetPrivateData(BaseStereoRenderer_GUID, pRight, sizeof(IUnknown *), D3DSPD_IUNKNOWN);
	}
	else
		return pLeft->SetPrivateData(BaseStereoRenderer_GUID, &pRight, sizeof(IUnknown *), 0);
}

HRESULT SetStereoObject(IDirect3DBaseTexture9 *pLeft, IDirect3DBaseTexture9 *pRight)
{
	_ASSERT(pLeft);
	if (pRight)
	{
		TextureType type = Stereo;
		pLeft->SetPrivateData(TextureType_GUID, (void *)&type, sizeof(TextureType), 0);
		return pLeft->SetPrivateData(BaseStereoRenderer_GUID, pRight, sizeof(IUnknown *), D3DSPD_IUNKNOWN);
	}
	else
		return pLeft->SetPrivateData(BaseStereoRenderer_GUID, &pRight, sizeof(IUnknown *), 0);
}

HRESULT LoadTexture( UINT Width, UINT Height, RECT &rect )
{
	iZ3DDisableCreatingStereoResources();
	// Use D3DX to create a texture from a file based image
	D3DXCreateTextureFromFileEx( g_pd3dDevice, g_FileName, 
		D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, D3DUSAGE_RENDERTARGET, D3DFMT_FROM_FILE,
		D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &g_pTexture );
	if( g_pTexture == NULL )
	{
		MessageBox( NULL, L"Could not load image", L"DX9OutputTest", MB_OK );
		return E_FAIL;
	}
	CComPtr<IDirect3DSurface9> pSurf;
	g_pTexture->GetSurfaceLevel(0, &pSurf);
	D3DSURFACE_DESC desc;
	pSurf->GetDesc(&desc);
	g_pd3dDevice->CreateTexture(Width, Height, 1, D3DUSAGE_RENDERTARGET,
		D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &g_pTextureL, NULL);
	g_pd3dDevice->CreateTexture(Width, Height, 1, D3DUSAGE_RENDERTARGET,
		D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &g_pTextureR, NULL);
	rect.left = 0;
	rect.top = 0;
	rect.right = desc.Width / 2;
	rect.bottom = desc.Height;
	CComPtr<IDirect3DSurface9> pSurfR;
	g_pTextureR->GetSurfaceLevel(0, &pSurfR);
	g_pd3dDevice->StretchRect(pSurf, &rect, pSurfR, NULL, D3DTEXF_LINEAR);
	rect.left = rect.right;
	rect.right *= 2;
	CComPtr<IDirect3DSurface9> pSurfL;
	g_pTextureL->GetSurfaceLevel(0, &pSurfL);
	g_pd3dDevice->StretchRect(pSurf, &rect, pSurfL, NULL, D3DTEXF_LINEAR);
	SetStereoObject(g_pTextureL, g_pTextureR);
	iZ3DRestoreCreatingStereoResourcesMode();
	return S_OK;
}


//-----------------------------------------------------------------------------
// Name: InitGeometry()
// Desc: Create the textures and vertex buffers
//-----------------------------------------------------------------------------
HRESULT InitGeometry()
{
	D3DXIMAGE_INFO info;
    if( FAILED( D3DXGetImageInfoFromFile( g_FileName, &info ) ) )
	{
		MessageBox( NULL, L"Could not find image", L"DX9OutputTest", MB_OK );
		return E_FAIL;
    }

	UINT ImageWidth = info.Width / 2;
	UINT ImageHeight = info.Height;
	float a = 1.0f * ImageWidth / ImageHeight;
	D3DDISPLAYMODE mode;
	g_pd3dDevice->GetDisplayMode(0, &mode);
	UINT Width = (UINT)(mode.Width * 0.90f);
	UINT Height = (UINT)(mode.Height * 0.85f);
	if (ImageWidth < Width)
	{
		Width = ImageWidth;
		UINT h = (UINT)(Width / a);
		if (h < Height)
			Height = h;
		else
			Width = (UINT)(Height * a);
	}
	if (ImageHeight < Height)
	{
		Height = ImageHeight;
		Width = (UINT)(Height * a);
	}
	RECT rect = { 0, 0, Width, Height };
	POINT pt = {0, 0};
	ClientToScreen(g_hWnd, &pt);
	OffsetRect(&rect, pt.x, pt.y);
	AdjustWindowRect(&rect, (DWORD)GetWindowLongPtr(g_hWnd, GWL_STYLE), FALSE);
	SetWindowPos(g_hWnd, NULL, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SW_SHOWDEFAULT);

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory( &d3dpp, sizeof( d3dpp ) );
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.EnableAutoDepthStencil = FALSE;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	g_pd3dDevice->Reset(&d3dpp);

	if (FAILED(LoadTexture(Width, Height, rect)))
		return E_FAIL;

	g_Vertex[0].x   = 0 - .5f;
	g_Vertex[0].y   = 0 - .5f;
	g_Vertex[0].z   = 0;
	g_Vertex[0].rhw = 1;
	g_Vertex[0].tu  = 0;    
	g_Vertex[0].tv  = 0;    

	g_Vertex[1].x   = Width - .5f;
	g_Vertex[1].y   = 0 - .5f;
	g_Vertex[1].z   = 0;
	g_Vertex[1].rhw = 1;
	g_Vertex[1].tu  = 1;    
	g_Vertex[1].tv  = 0;    

	g_Vertex[2].x   = Width - .5f;
	g_Vertex[2].y   = Height - .5f;
	g_Vertex[2].z   = 0;
	g_Vertex[2].rhw = 1;
	g_Vertex[2].tu  = 1;    
	g_Vertex[2].tv  = 1;    

	g_Vertex[3].x   = 0 - .5f;
	g_Vertex[3].y   = Height - .5f;
	g_Vertex[3].z   = 0;
	g_Vertex[3].rhw = 1;
	g_Vertex[3].tu  = 0;    
	g_Vertex[3].tv  = 1;  

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: Cleanup()
// Desc: Releases all previously initialized objects
//-----------------------------------------------------------------------------
VOID Cleanup()
{
    if( g_pTexture != NULL )
		g_pTexture->Release();

	if( g_pTextureL != NULL )
		g_pTextureL->Release();

	if( g_pTextureR != NULL )
		g_pTextureR->Release();

    if( g_pd3dDevice != NULL )
        g_pd3dDevice->Release();

    if( g_pD3D != NULL )
        g_pD3D->Release();
}

//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
VOID Render()
{
    // Clear the backbuffer and the zbuffer
    g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET,
                         D3DCOLOR_XRGB( 0, 0, 255 ), 1.0f, 0 );

    // Begin the scene
    if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
	{
		g_pd3dDevice->SetTexture(0, g_pTextureL);

		g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, g_Vertex, sizeof(g_Vertex[0]));

        // End the scene
        g_pd3dDevice->EndScene();
    }

    // Present the backbuffer contents to the display
    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}

//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: The window's message handler
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: The application's entry point
//-----------------------------------------------------------------------------
INT WINAPI wWinMain( HINSTANCE hInst, HINSTANCE, LPWSTR, INT )
{

#ifdef UNICODE
	__wargv = CommandLineToArgvW(GetCommandLineW(), &__argc);
#endif

	g_FileName = _T("test.jps");
	if (__argc >= 2 )
		g_FileName = __targv[1];
    // Register the window class
    WNDCLASSEX wc =
    {
        sizeof( WNDCLASSEX ), CS_CLASSDC, MsgProc, 0L, 0L,
        GetModuleHandle( NULL ), NULL, NULL, NULL, NULL,
        L"iZ3D Window", NULL
    };
    RegisterClassEx( &wc );

	RECT rcClient;
	SetRect(&rcClient, 30, 30, 31, 31);
	AdjustWindowRect (&rcClient, WS_OVERLAPPEDWINDOW, FALSE);
    // Create the application's window
	g_hWnd = CreateWindow( L"iZ3D Window", L"Output Test",
                              WS_OVERLAPPEDWINDOW, rcClient.left, rcClient.top, 
							  rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
                              NULL, NULL, wc.hInstance, NULL );

    // Initialize Direct3D
    if( SUCCEEDED( InitD3D( ) ) )
    {
        // Create the scene geometry
        if( SUCCEEDED( InitGeometry() ) )
        {
            // Show the window
            ShowWindow( g_hWnd, SW_SHOWDEFAULT );
            UpdateWindow( g_hWnd );

            // Enter the message loop
            MSG msg;
            ZeroMemory( &msg, sizeof( msg ) );
            while( msg.message != WM_QUIT )
            {
                if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
                {
                    TranslateMessage( &msg );
                    DispatchMessage( &msg );
                }
                else
                    Render();
            }
        }
    }

    UnregisterClass( L"iZ3D Window", wc.hInstance );
    return 0;
}
