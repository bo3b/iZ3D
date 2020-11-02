#include "StdAfx.h"
#include "OutputMethod_dx10.h"
#include "CommonUtils\ResourceLoader.h"

using namespace DX10Output;

OutputMethod::OutputMethod(DWORD mode, DWORD spanMode)
: m_bProcessGammaRamp(false), m_OutputMode(mode), m_SpanMode(spanMode), m_bInitialized(false)
{
	m_SubMode = 0;
	m_SecondBBColor[0] = 0.0f;
	m_SecondBBColor[1] = 0.0f;
	m_SecondBBColor[2] = 0.0f;
	m_SecondBBColor[3] = 1.0f;
	m_hLogoResource.pDrvPrivate = NULL;
	m_hLogoResourceView.pDrvPrivate = NULL;

	m_nLogoWidth	= 0;
	m_nLogoHeight	= 0;
	m_bTrialMode = (GetTrialDaysLeft() >= 0);
}

OutputMethod::~OutputMethod(void)
{
}

void OutputMethod::Initialize( D3DDeviceWrapper* dev )
{
	m_bInitialized = true;
}

void OutputMethod::Clear( D3DDeviceWrapper* dev )
{
	m_bInitialized = false;
	DeleteLogoResources( dev );
}

int OutputMethod::GetTrialDaysLeft()
{
	int DaysLeft = -1; 
	TCHAR TempBuffer[100];
	if (GetEnvironmentVariable(_T("PCGI_ApplicationStatus"), TempBuffer, 100))
	{
		UINT ApplicationStatus = 1;
		_stscanf_s(TempBuffer, _T("%d"), &ApplicationStatus);
		if (ApplicationStatus == 0 && GetEnvironmentVariable(_T("PCGI_DemoModeActive"), TempBuffer, 100))
		{
			BOOL DemoActive = FALSE;
			_stscanf_s(TempBuffer, _T("%d"), &DemoActive);
			if (DemoActive && GetEnvironmentVariable(_T("PCGI_DemoDaysLeft"), TempBuffer, 100))
				_stscanf_s(TempBuffer, _T("%d"), &DaysLeft);
		}
	}
	return DaysLeft;
}

void OutputMethod::ModifyResizeBuffersParams( UINT& BufferCount, UINT& Width, UINT& Height, DXGI_FORMAT& NewFormat, UINT& SwapChainFlags )
{
	if (m_SpanMode)
	{
		Width *= 2;
	}
}

void OutputMethod::ModifyResizeTargetParams( DXGI_MODE_DESC *pNewTargetParameters )
{
	if (m_SpanMode)
	{
		pNewTargetParameters->Width *= 2;
	}
}

bool OutputMethod::FindAdapter( IDXGIFactory* pDXGIFactory_ )
{
	return true;
}

HRESULT OutputMethod::CreateLogoResources( HMODULE hModule_, UINT nResID_, D3DDeviceWrapper* pD3DWrapper_ )
{
	PNGResource	pngResource( hModule_, MAKEINTRESOURCE( nResID_ ) );
	if ( !pngResource.GetTexData() )
		return E_FAIL;	

	m_nLogoWidth		= pngResource.GetTexWidth();
	m_nLogoHeight		= pngResource.GetTexHeight();
	UINT	nTexDepth	= 1;
	UINT	nBitDepth	= pngResource.GetBitDepth();
	UINT	nRowBytes	= pngResource.GetRowBytes();
	UINT	nSliceBytes	= nRowBytes * m_nLogoHeight;

	D3D10DDI_HDEVICE hDevice			= pD3DWrapper_->GetHandle();
	D3D10DDI_DEVICEFUNCS& ORIG_DX10		= pD3DWrapper_->OriginalDeviceFuncs;
	D3D10_1DDI_DEVICEFUNCS& ORIG_DX10_1	= pD3DWrapper_->OriginalDeviceFuncs10_1;
	D3D11DDI_DEVICEFUNCS& ORIG_DX11		= pD3DWrapper_->OriginalDeviceFuncs11;
	TD3DVersion tVersion				= pD3DWrapper_->GetD3DVersion();	

	D3D11DDIARG_CREATERESOURCE CreateTexRes;
	memset( &CreateTexRes, 0, sizeof( D3D11DDIARG_CREATERESOURCE ) );

	D3D10DDI_MIPINFO TexMipInfo;
	TexMipInfo.TexelWidth				= m_nLogoWidth;
	TexMipInfo.TexelHeight				= m_nLogoHeight;
	TexMipInfo.TexelDepth				= 1;
	TexMipInfo.PhysicalWidth			= m_nLogoWidth;
	TexMipInfo.PhysicalHeight			= m_nLogoHeight;
	TexMipInfo.PhysicalDepth			= 1;

	D3D10_DDIARG_SUBRESOURCE_UP InitialTexTes;
	InitialTexTes.pSysMem				= pngResource.GetTexData();
	//InitialTexTes.pSysMem				= pData;
	InitialTexTes.SysMemPitch			= nRowBytes;
	InitialTexTes.SysMemSlicePitch		= nSliceBytes;

	CreateTexRes.pMipInfoList			= &TexMipInfo;
	CreateTexRes.pInitialDataUP			= &InitialTexTes;
	CreateTexRes.ResourceDimension		= D3D10DDIRESOURCE_TEXTURE2D;
	CreateTexRes.Usage					= D3D10_DDI_USAGE_DEFAULT;
	CreateTexRes.BindFlags				= D3D10_DDI_BIND_SHADER_RESOURCE;
	CreateTexRes.Format					= DXGI_FORMAT_R8G8B8A8_UNORM;
	CreateTexRes.SampleDesc.Count		= 1;
	CreateTexRes.SampleDesc.Quality		= 0;
	CreateTexRes.MipLevels				= 1;
	CreateTexRes.ArraySize				= 1;

	D3D10DDI_HRTRESOURCE hRTResource	= { NULL };	
	SIZE_T nSize = ORIG_DX11.pfnCalcPrivateResourceSize( hDevice, &CreateTexRes );
	NEW_HANDLE( m_hLogoResource, nSize );
	ORIG_DX11.pfnCreateResource( hDevice, &CreateTexRes, m_hLogoResource, hRTResource );

	D3D11DDIARG_CREATESHADERRESOURCEVIEW	CreateResView;
	memset( &CreateResView, 0, sizeof( D3D11DDIARG_CREATESHADERRESOURCEVIEW ) );
	CreateResView.hDrvResource			= m_hLogoResource;
	CreateResView.Format				= DXGI_FORMAT_R8G8B8A8_UNORM;
	CreateResView.ResourceDimension		= D3D10DDIRESOURCE_TEXTURE2D;
	CreateResView.Tex2D.MostDetailedMip	= 0;
	CreateResView.Tex2D.FirstArraySlice	= 0;
	CreateResView.Tex2D.MipLevels		= 1;
	CreateResView.Tex2D.ArraySize		= 1;

	D3D10DDI_HRTSHADERRESOURCEVIEW	hRTResView	= { NULL };
	nSize = ORIG_DX11.pfnCalcPrivateShaderResourceViewSize( hDevice, &CreateResView );
	NEW_HANDLE( m_hLogoResourceView, nSize );
	ORIG_DX11.pfnCreateShaderResourceView( hDevice, &CreateResView, m_hLogoResourceView, hRTResView );

	return S_OK;
}

void OutputMethod::DeleteLogoResources( D3DDeviceWrapper* pD3DWrapper_ )
{
	D3D10DDI_DEVICEFUNCS& ORIG_DX10 = pD3DWrapper_->OriginalDeviceFuncs;
	D3D10DDI_HDEVICE hDevice = pD3DWrapper_->hDevice;

	if ( m_hLogoResourceView.pDrvPrivate )
	{
		ORIG_DX10.pfnDestroyShaderResourceView( hDevice, m_hLogoResourceView );
		DELETE_HANDLE( m_hLogoResourceView );
	}

	if ( m_hLogoResource.pDrvPrivate )
	{
		ORIG_DX10.pfnDestroyResource( hDevice, m_hLogoResource );
		DELETE_HANDLE( m_hLogoResource );
	}	
}