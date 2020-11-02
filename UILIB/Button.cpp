#include <d3d9.h>
#include <d3dx9.h>
#include "uilib.h"

static const LOGFONT st_lf =
{
	-70,				// LONG lfHeight;
	0,				// LONG lfWidth;
	0,					// LONG lfEscapement;
	0,					// LONG lfOrientation;
	FW_DONTCARE,		// LONG lfWeight;
	FALSE,				// BYTE lfItalic;
	FALSE,				// BYTE lfUnderline;
	FALSE,				// BYTE lfStrikeOut;
	DEFAULT_CHARSET,	// BYTE lfCharSet;
	OUT_DEFAULT_PRECIS, // BYTE lfOutPrecision;
	3,// BYTE lfClipPrecision;
	ANTIALIASED_QUALITY,// BYTE lfQuality;
	VARIABLE_PITCH,		// BYTE lfPitchAndFamily; 
	_T("Arial")			// TCHAR lfFaceName[LF_FACESIZE]; 
};

CXButton::CXButton( iz3d::uilib::CommonDevice* pDevice ) :
	CXControl( pDevice )
{
	m_pDefaultImage = new CXCanvas( pDevice );
	m_pPressedImage =  new CXCanvas( pDevice );
	m_pCaption = new CXLabel( st_lf, pDevice );

	m_Pressed = false;

	m_pCaption->SetWidthHeight( 50,50 );
	m_pCaption->SetFormat( DT_CENTER | DT_VCENTER );
}

CXButton::~CXButton()
{
	SAFE_DELETE( m_pCaption );

	DeleteObject( m_hFont );

	SAFE_DELETE( m_pDefaultImage );
	SAFE_DELETE( m_pPressedImage );

	SetCanvas( 0 );
}

bool CXButton::OnRender()
{
	if( m_Pressed )		
		SetCanvas( m_pPressedImage );
	else
		SetCanvas( m_pDefaultImage );

	RenderMe();


	if( !m_pCaption )
		return true;
	
	POINT Abs;
	
	Abs.x = 0;
	Abs.y = 0;

	GetAbsolutePosition( &Abs );

	m_pCaption->SetXYPos( 0,0 );
	Abs.x += m_pCaption->GetXPos();
	Abs.y += m_pCaption->GetYPos();

	m_pCaption->SetXYPos( Abs.x, Abs.y );
	m_pCaption->OnRender();

	return true;
}

void CXButton::OnMouseDown( int, int, int )
{
	m_Pressed = true;
}

void CXButton::OnMouseMove( int, int )
{
}

void CXButton::OnMouseUp( int , int , int )
{
	m_Pressed = false;
}

void CXButton::OnSetFocus()
{
}

void CXButton::OnLostFocus()
{
}

void CXButton::OnKeyDown( WPARAM , LPARAM )
{
}

void CXButton::OnKeyUp( WPARAM , LPARAM )
{
}

void CXButton::OnSysKey( WPARAM , LPARAM )
{
}

bool CXButton::SetDefaultImage( TCHAR* pStrFileName )
{
	D3DXIMAGE_INFO Info;

	if( !m_pDefaultImage )
		return false;

	auto hr = D3DXGetImageInfoFromFile( pStrFileName, &Info );
	if( FAILED( hr ) )
		return false;
	
	SetWidthHeight( Info.Width, Info.Height );
	hr = m_pDefaultImage->LoadFromFile( pStrFileName );
	if( FAILED( hr ) )
		return false;

	return true;
}

bool CXButton::SetDefaultImage( HMODULE hSrcModule, LPCTSTR pSrcResource )
{
	D3DXIMAGE_INFO Info;
	if ( !m_pDefaultImage )
		return false;

	auto hr = D3DXGetImageInfoFromResource( hSrcModule, pSrcResource, &Info );
	if( FAILED( hr ) )
		return false;

	SetWidthHeight( Info.Width, Info.Height );
	hr = m_pDefaultImage->LoadFromResource( hSrcModule, pSrcResource );
	if ( FAILED( hr ) )
		return false;
	return true;
}

bool CXButton::SetDefaultImage( size_t size, const void* pData )
{
	D3DXIMAGE_INFO Info;
	if( !m_pDefaultImage )
		return false;
	auto hr = D3DXGetImageInfoFromFileInMemory( pData, ( UINT )size, &Info );
	if( FAILED( hr ) )
		return false;
	SetWidthHeight( Info.Width, Info.Height );
	hr = m_pDefaultImage->LoadFromMemory( size, pData );
	if( FAILED( hr ) )
		return false;
	return true;
}

bool CXButton::SetDefaultImage( ICommonTexture* pTex )
{
	if (pTex == NULL)
		return false;
	m_pDefaultImage->SetTexture( pTex );
	return true;
}

bool CXButton::SetPressedImage( TCHAR* pStrFileName )
{
	D3DXIMAGE_INFO Info;

	if( !m_pPressedImage )
		return false;
	
	auto hr = D3DXGetImageInfoFromFile( pStrFileName, &Info );
	if( FAILED( hr ) )
		return false;
	
	hr = m_pPressedImage->LoadFromFile( pStrFileName );
	if( FAILED( hr ) )
		return false;
	
	return true;
}

bool CXButton::SetPressedImage( HMODULE hSrcModule, LPCTSTR pSrcResource )
{
	D3DXIMAGE_INFO Info;

	if( !m_pPressedImage)
		return false;
	
	auto hr = D3DXGetImageInfoFromResource( hSrcModule, pSrcResource, &Info);
	if( FAILED( hr ) )
		return false;

	hr = m_pPressedImage->LoadFromResource( hSrcModule, pSrcResource );
	if( FAILED( hr ) )
		return false;
	return true;
}

bool CXButton::SetPressedImage( size_t size, const void* pData )
{
	if( !m_pPressedImage )
		return false;
	D3DXIMAGE_INFO Info;
	auto hr = D3DXGetImageInfoFromFileInMemory( pData, ( UINT )size, &Info );
	if( FAILED( hr ) )
		return false;
	hr = m_pPressedImage->LoadFromMemory( size, pData );
	if( FAILED( hr ) )
		return false;
	return true;
}

bool CXButton::SetPressedImage( ICommonTexture* pTex )
{
	if ( pTex == NULL )
		return false;
	m_pPressedImage->SetTexture( pTex );
	return true;
}

void CXButton::SetWidth( LONG Width )
{
	CXControl::SetWidth( Width );
	m_pCaption->SetWidth( Width );
}

void CXButton::SetHeight( LONG Height )
{
	CXControl::SetHeight( Height );
	m_pCaption->SetHeight( Height );
}