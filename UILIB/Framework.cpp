#include <d3d9.h>
#include <d3dx9.h>
#include "UILIB.h"

CXCanvas::CXCanvas(iz3d::uilib::CommonDevice* pDevice)
{
	m_Scaling.x = m_Scaling.y = 1.0f;
	init( pDevice );
	m_Path = _T( "\0" );
}

CXCanvas::CXCanvas(iz3d::uilib::CommonDevice* pDevice, const TCHAR* Path)
{
	m_Scaling.x = m_Scaling.y = 1.0f;
	init( pDevice );
	m_Path = Path;
	LoadFromFile( Path );
}

HRESULT CXCanvas::LoadFromFile( const TCHAR* Path)
{
	m_pTexture = 0;

	this->SetPath(Path);
	HRESULT hr = m_pDevice->CreateTextureFromFile( m_Path.c_str(), &m_pTexture );
	auto WithHeight = m_pTexture->GetWidthHeight();
	m_SrcRect.right = WithHeight.cx;
	m_SrcRect.left = WithHeight.cy;
	return hr;
}

HRESULT CXCanvas::InvalidateDeviceObjects()
{
	m_pTexture = 0;

	return S_OK;
}

HRESULT CXCanvas::RestoreDeviceObjects()
{
	m_pTexture = 0;

	return m_pDevice->CreateTextureFromFile( m_Path.c_str(), &m_pTexture );
}

CXCanvas::~CXCanvas()
{
}

HRESULT CXCanvas::LoadFromResource( HMODULE hSrcModule, LPCTSTR pSrcResource )
{
	m_pTexture = 0;

	HRESULT hr = m_pDevice->CreateTextureFromResource( hSrcModule, pSrcResource, &m_pTexture );
	return hr;
}

HRESULT CXCanvas::LoadFromMemory( size_t size, const void* pData )
{
	m_pTexture = 0;
	HRESULT hr = m_pDevice->CreateTextureFromMemory( pData, size, &m_pTexture );
	auto WidthHeight = m_pTexture->GetWidthHeight();
	m_SrcRect.right = WidthHeight.cx;
	m_SrcRect.left = WidthHeight.cy;
	return hr;
}

void CXCanvas::SetTexture( ICommonTexture* Texture )
{
	m_pTexture = Texture;
	if( !Texture )
	{
		m_SrcRect.right = m_SrcRect.bottom = 0;
		return;
	}
	auto WidthHeight = m_pTexture->GetWidthHeight();
	m_SrcRect.right = WidthHeight.cx;
	m_SrcRect.bottom = WidthHeight.cy;
}

void CXCanvas::init( iz3d::uilib::CommonDevice* pDevice )
{
	m_pDevice = pDevice;
	m_pTexture = 0;
	ZeroMemory( &m_SrcRect, sizeof(m_SrcRect) );
	m_Rotation = 0.0f;
	ZeroMemory( &m_RotationCenter, sizeof( m_RotationCenter ) );
	ZeroMemory( &m_Translation, sizeof( m_Translation ) );
	m_Scaling.x = m_Scaling.y = 1.0f;
	isStereo = isLeft = false;
}

POINT CXCanvas::GetTranslation() const
{
	return m_Translation;
}

POINTF CXCanvas::GetScaling() const
{
	if( !isStereo )
		return m_Scaling;
	POINTF toRet = m_Scaling;
	toRet.x *= 2;
	return toRet;
}

RECT CXCanvas::GetRect() const
{
	RECT r = m_SrcRect;
	if( isStereo )
	{
		if( isLeft )
		{
			r.right /= 2;
		}
		else
		{
			r.left = r.right / 2;
		}
	}
	return r;
}

CXPen::CXPen(iz3d::uilib::CommonDevice* pDevice)
	: m_pDevice( pDevice )
{
	m_pDevice->CreateSprite(&m_pSprite);
}

CXPen::~CXPen()
{
	//m_pDevice->DeleteSprite(&m_pSprite);
	m_pSprite = 0;
}

HRESULT CXPen::DrawTexture(CXCanvas* Texture)
{
	if ( !Texture || !m_pSprite )
		return E_FAIL;
	m_pSprite->Begin();
	POINTF Scaling = Texture->GetScaling();
	POINT Trans = Texture->GetTranslation();
	m_pSprite->SetTransform( Scaling, Trans );
	RECT ret = Texture->GetRect();
	HRESULT Result = m_pSprite->Draw( Texture->GetTexture(), &ret );

	m_pSprite->End();
	return Result;
}

HRESULT CXPen::InvalidateDeviceObjects()
{
	m_pSprite = 0;
	return S_OK;
}

HRESULT CXPen::RestoreDeviceObjects()
{
	//m_pDevice->DeleteSprite(&m_pSprite);
	m_pSprite = 0;
	return m_pDevice->CreateSprite( &m_pSprite );
}

HRESULT LoadTextureFromMemory( IDirect3DDevice9* pDevice,
							  const void* pSrcData,
							  size_t SrcDataSize,
							  LPDIRECT3DTEXTURE9* ppTexture )
{
	HRESULT hr = D3DXCreateTextureFromFileInMemoryEx(pDevice, pSrcData, (UINT)SrcDataSize, 
		D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 
		D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, ppTexture);
	return hr;
}