#include "stdafx.h"
#include "Output_dx10.h"
#include <tinyxml.h>
#include "..\CommonUtils\StringUtils.h"
#include "..\CommonUtils\CommonResourceFolders.h"

using namespace DX10Output;
using namespace std;

#define NSCALL(func)						hResult = func;

OUTPUT_API void* CALLBACK CreateOutputDX10( DWORD mode, DWORD spanMode )
{
	return new S3DShutterOutput( mode, spanMode );
}

S3DShutterOutput::S3DShutterOutput( DWORD mode, DWORD spanMode )
	:	OutputMethod			( mode, spanMode )
	,	m_MarkerIndex			( 0 )
	,	m_NumberOfMarks			( 0 ) 
	,	m_NumberOfMarkedLines	( 0 )
	,	m_MarkerSurfaceWidth	( 0 )
	,	m_InverseAxisYDirection ( 0 )
	,	m_ScaleToFrame			( 0 )
	,	m_ScaleFactor			( 0.0f )
{	
}

S3DShutterOutput::~S3DShutterOutput()
{
}

void S3DShutterOutput::Output(bool bLeft_, D3DSwapChain* pSwapChain_, RECT* pDestRect_)
{
	HRESULT hResult						= S_OK;
	D3DDeviceWrapper* pD3DDevice		= pSwapChain_->m_pD3DDeviceWrapper;
	SwapChainResources* pRes			= pSwapChain_->GetCurrentPrimaryResource();
	D3D10DDI_DEVICEFUNCS& GET_ORIG		= pD3DDevice->OriginalDeviceFuncs;
	D3D10DDI_HDEVICE hDevice			= pD3DDevice->hDevice;
	
	D3D10DDI_HRESOURCE hOutput			= ( bLeft_ ) ?  pRes->GetMethodResourceLeft() : pRes->GetMethodResourceRight();
	D3D10DDI_HRESOURCE hMarkedResoure	= ( bLeft_ ) ?	m_hLeftMarkedSurfaces[ m_MarkerIndex ] : m_hRightMarkedSurfaces[ m_MarkerIndex ];

	GET_ORIG.pfnResourceCopyRegion( hDevice, pRes->GetBackBufferPrimary(), 0, 0, 0, 0, hOutput, 0, NULL );
	GET_ORIG.pfnResourceCopyRegion( hDevice, pRes->GetBackBufferPrimary(), 0, 0, 0, 0, hMarkedResoure, 0, NULL );

	m_MarkerIndex = (++m_MarkerIndex) % m_NumberOfMarks;
}

void S3DShutterOutput::Initialize( D3DDeviceWrapper* pD3DWrapper_ )
{	
	const char* specVersion;
	const char* format = "";
	m_MarkerIndex = 0;
	m_MarkerSurfaceWidth = 0;
	m_NumberOfMarkedLines = 0;
	m_NumberOfMarks = 0;
	m_InverseAxisYDirection = 0;
	m_ScaleToFrame = 0;
	m_ScaleFactor = .75;
	vector< vector<DWORD> > LeftMark, RightMark;
	TCHAR XMLFileName[MAX_PATH];

	HRESULT hResult = E_FAIL;
	if(iz3d::resources::GetAllUsersDirectory( XMLFileName ))
	{
		PathAppend(XMLFileName, _T("MarkingSpec.xml") );
		TiXmlDocument MarkFile( common::utils::to_multibyte( XMLFileName ).c_str() );
		if(MarkFile.LoadFile())
		{
			TiXmlHandle   docHandle( &MarkFile );
			TiXmlHandle   frameHandle = docHandle.FirstChild("FrameMarkingSpec");
			TiXmlElement* element = frameHandle.Element();
			if(element)
			{
				specVersion = element->Attribute("version");
				if(strcmp(specVersion,"0.2") == 0)
				{
					ParseVersion_0_2(frameHandle, format, LeftMark, RightMark);
					hResult = S_OK;
				}
				if(strcmp(specVersion,"0.3") == 0)
				{
					ParseVersion_0_2(frameHandle, format, LeftMark, RightMark);
					ParseVersion_0_3(frameHandle);
					hResult = S_OK;
				}
			}
		}
	}

	if(FAILED(hResult))
	{
		m_NumberOfMarkedLines = 16;
		m_NumberOfMarks = 1;
		m_InverseAxisYDirection = 0;
		m_ScaleToFrame = 1;
		m_ScaleFactor = 1;
		m_MarkerSurfaceWidth = 1;

		LeftMark.clear();  LeftMark.push_back(vector<DWORD>(m_MarkerSurfaceWidth, 0x000000FF ));
		RightMark.clear(); RightMark.push_back(vector<DWORD>( m_MarkerSurfaceWidth, 0x000000FF ));		
	}	

	UINT	nTexWidth	= m_MarkerSurfaceWidth;
	UINT	nTexHeight	= m_NumberOfMarkedLines;	
	UINT	nBitDepth	= 32;
	UINT	nRowBytes	= nTexWidth * nBitDepth / 8;	
	BYTE*	pData		= new BYTE [ nTexHeight * nRowBytes ];

	for( UINT i = 0; i < m_NumberOfMarks; i++ )
	{
		for ( UINT j = 0; j < nTexHeight; j++ )
		{
			BYTE* p = pData + nRowBytes * j;
			memcpy( p,  &LeftMark[i][0], nRowBytes );			
		}
		D3D10DDI_HRESOURCE				hResource		= { NULL };
		CreateMarkedSurface( pD3DWrapper_, nTexWidth, nTexHeight, nBitDepth, nRowBytes, pData, hResource );
		m_hLeftMarkedSurfaces.push_back( hResource );

		for ( UINT j = 0; j < nTexHeight; j++ )
		{
			BYTE* p	= pData + nRowBytes * j;
			memcpy( p,  &RightMark[i][0], nRowBytes );			
		}
		hResource.pDrvPrivate		=  NULL;
		CreateMarkedSurface( pD3DWrapper_, nTexWidth, nTexHeight, nBitDepth, nRowBytes, pData, hResource );
		m_hRightMarkedSurfaces.push_back( hResource );		
	}	

	delete[] pData;
}

void S3DShutterOutput::Clear( D3DDeviceWrapper* pD3DWrapper_ )
{
	D3D10DDI_HDEVICE hDevice = pD3DWrapper_->hDevice;	

	for( UINT i = 0; i < m_hLeftMarkedSurfaces.size(); i++ )
	{
		D3D10DDI_HRESOURCE hResource = m_hLeftMarkedSurfaces[ i ];
		if ( hResource.pDrvPrivate )
		{
			pD3DWrapper_->OriginalDeviceFuncs.pfnDestroyResource( hDevice, hResource );
			DELETE_HANDLE( hResource );
		}
	}	

	for( UINT i = 0; i < m_hRightMarkedSurfaces.size(); i++ )
	{
		D3D10DDI_HRESOURCE hResource = m_hRightMarkedSurfaces[ i ];
		if ( hResource.pDrvPrivate )
		{
			pD3DWrapper_->OriginalDeviceFuncs.pfnDestroyResource( hDevice, hResource );
			DELETE_HANDLE( hResource );
		}
	}
}

void S3DShutterOutput::ParseMarkLine(TiXmlElement* element, vector<vector<DWORD> >& MarkArray)
{
	for(int index = 0; element; index++, element = element->NextSiblingElement())
	{
		MarkArray.push_back(vector<DWORD>());
		if(const char* s = element->GetText())
		{
			const char* t = s;
			UINT length = (UINT)strlen(s);
			while(t < s + length)
			{
				DWORD dwXRGBColor;
				sscanf(t, "%x", &dwXRGBColor); t+= 7;
				DWORD dwAlpha		= dwXRGBColor >> 24;
				DWORD dwBlue		= dwXRGBColor & 0xFF;
				DWORD dwGreen		= ( dwXRGBColor >> 8 ) & 0xFF;
				DWORD dwRed			= ( dwXRGBColor >> 16 ) & 0xFF;
				DWORD dwABGRColor	=  ( dwAlpha << 24 ) + ( dwBlue << 16 ) + ( dwGreen << 8 ) + dwRed;
				MarkArray[index].push_back( dwABGRColor );				
			}
		}
	}
}

void S3DShutterOutput::ParseVersion_0_2(TiXmlHandle& frameHandle, const char* &format, vector< vector<DWORD> > &LeftMark, vector< vector<DWORD> > &RightMark)
{
	TiXmlElement* element = frameHandle.Element();
	element->QueryIntAttribute("NumberOfMarkedLines", (int*)&m_NumberOfMarkedLines);
	TiXmlHandle lineHandle = frameHandle.FirstChild();
	element = lineHandle.Element();
	if(element)
	{
		format = element->Attribute("ColorFormat");
		element->QueryIntAttribute("NumberOfMarks", (int*)&m_NumberOfMarks);
	}
	if(strcmpi(format, "r8g8b8_hex") == 0)
	{
		element = lineHandle.Child("LeftFrameMarks", 0).FirstChild().Element();
		ParseMarkLine(element, LeftMark);
		element = lineHandle.Child("RightFrameMarks", 0).FirstChild().Element();
		ParseMarkLine(element, RightMark);
		m_MarkerSurfaceWidth = (UINT)LeftMark[0].size();
	}
}

void S3DShutterOutput::ParseVersion_0_3(TiXmlHandle& frameHandle)
{
	TiXmlHandle lineHandle = frameHandle.FirstChild();
	TiXmlElement* element = lineHandle.Element();
	if(element)
	{
		element->QueryIntAttribute("InverseAxisYDirection", (int*)&m_InverseAxisYDirection);
		element->QueryIntAttribute("ScaleToFrame", (int*)&m_ScaleToFrame);
		element->QueryFloatAttribute("ScaleFactor", (float*)&m_ScaleFactor);
	}
}


HRESULT S3DShutterOutput::CreateMarkedSurface( D3DDeviceWrapper* pD3DWrapper_, UINT nTexWidth_,UINT nTexHeight_, UINT nBitDepth_, UINT nRowBytes_, 
												VOID* pData_, D3D10DDI_HRESOURCE& hResource_ )
{
	D3D10DDI_HDEVICE hDevice = pD3DWrapper_->GetHandle();

	D3D11DDIARG_CREATERESOURCE CreateTexRes;
	memset( &CreateTexRes, 0, sizeof( D3D11DDIARG_CREATERESOURCE ) );

	D3D10DDI_MIPINFO TexMipInfo;
	TexMipInfo.TexelWidth				= nTexWidth_;
	TexMipInfo.TexelHeight				= nTexHeight_;
	TexMipInfo.TexelDepth				= 1;
	TexMipInfo.PhysicalWidth			= nTexWidth_;
	TexMipInfo.PhysicalHeight			= nTexHeight_;
	TexMipInfo.PhysicalDepth			= 1;

	D3D10_DDIARG_SUBRESOURCE_UP InitialTexTes;
	InitialTexTes.pSysMem				= pData_;
	InitialTexTes.SysMemPitch			= (UINT)nRowBytes_;
	InitialTexTes.SysMemSlicePitch		= (UINT)nRowBytes_ * nTexHeight_;

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

	D3D10DDI_HRTRESOURCE hRTResource		= { NULL };
	SIZE_T nSize = pD3DWrapper_->OriginalDeviceFuncs11.pfnCalcPrivateResourceSize( hDevice, &CreateTexRes );
	NEW_HANDLE( hResource_, nSize );
	pD3DWrapper_->OriginalDeviceFuncs11.pfnCreateResource( hDevice, &CreateTexRes, hResource_, hRTResource );

	return S_OK;
}