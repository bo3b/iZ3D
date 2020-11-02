#include <d3d9.h>
#include <d3dx9.h>
#include "uilib.h"

CXCheckBox::CXCheckBox( const LOGFONT& lf, iz3d::uilib::CommonDevice* pDevice )
	: CXControl(pDevice)
{
	m_pTickBox = new CXButton( pDevice );
	m_pLabel = new CXLabel(lf, pDevice );

	m_Checked = false;
}

CXCheckBox::~CXCheckBox()
{
	SAFE_DELETE( m_pTickBox );
	SAFE_DELETE( m_pLabel );
}

bool CXCheckBox::LoadCheckedImage( TCHAR* pStrFileName )
{
	m_pTickBox->SetPressedImage( pStrFileName );
	return true;
}

bool CXCheckBox::LoadUncheckedImage( TCHAR* pStrFileName )
{
	m_pTickBox->SetDefaultImage( pStrFileName );
	return true;
}

void CXCheckBox::SetCaption(  const TCHAR* pStrText)
{
	m_pLabel->SetCaption( pStrText );
}

bool CXCheckBox::OnRender()
{
	POINT Abs;
	ZeroMemory( &Abs, sizeof( D3DXVECTOR2 ) );

	GetAbsolutePosition( &Abs );

	m_pTickBox->SetXYPos( Abs.x ,Abs.y );
	m_pTickBox->OnRender();

	m_pLabel->SetColour( D3DCOLOR_XRGB( 255,255,255 ) );
	m_pLabel->SetWidthHeight( GetWidth() - m_pTickBox->GetWidth(), GetHeight() );
	m_pLabel->SetXYPos( Abs.x + m_pTickBox->GetWidth(), Abs.y );
	m_pLabel->OnRender();

	return true;
}

void CXCheckBox::OnMouseDown( int Button, int X, int Y )
{
	if( m_Checked )
	{
		m_pTickBox->OnMouseUp( Button, X, Y );
		m_Checked = false;
	}
	else
	{
		m_pTickBox->OnMouseDown( Button, X, Y );
		m_Checked = true;
	}
}

void CXCheckBox::OnMouseMove( int , int )
{
}

void CXCheckBox::OnMouseUp( int , int , int )
{
}

void CXCheckBox::OnSetFocus()
{
}

void CXCheckBox::OnLostFocus()
{
}

void CXCheckBox::OnKeyDown( WPARAM , LPARAM )
{
}

void CXCheckBox::OnKeyUp( WPARAM , LPARAM )
{
}

void CXCheckBox::OnSysKey( WPARAM , LPARAM )
{
}

bool CXCheckBox::LoadCheckedImage( HMODULE hSrcModule, LPCTSTR pSrcResource )
{
	m_pTickBox->SetPressedImage( hSrcModule, pSrcResource );
	return true;
}

bool CXCheckBox::LoadCheckedImage( size_t size, const void* pData )
{
	m_pTickBox->SetPressedImage( size, pData );
	return true;
}

bool CXCheckBox::LoadCheckedImage( ICommonTexture* pTex )
{
	if ( pTex == NULL )
		return false;
	m_pTickBox->SetPressedImage( pTex );
	return true;
}

bool CXCheckBox::LoadUncheckedImage( HMODULE hSrcModule, LPCTSTR pSrcResource )
{
	m_pTickBox->SetDefaultImage( hSrcModule, pSrcResource );
	return true;
}

bool CXCheckBox::LoadUncheckedImage( size_t size, const void* pData )
{
	m_pTickBox->SetDefaultImage( size, pData );
	return true;
}

bool CXCheckBox::LoadUncheckedImage( ICommonTexture* pTex )
{
	if (pTex == NULL)
		return false;
	m_pTickBox->SetDefaultImage( pTex );
	return true;
}

void CXCheckBox::SetCaptionColour( D3DCOLOR colour )
{
	m_pLabel->SetColour( colour );
}