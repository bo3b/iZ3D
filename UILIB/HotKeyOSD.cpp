#include "HotKeyOSD.h"
#include "..\CommonUtils\COMLikeRepository.h"
#include "DX9Device.h"
#include "DX10Device.h"
#include "..\CommonUtils\StringResourceManager.h"

iz3d::UI::HotKeyOSD::HotKeyOSD( void ):
m_pDevice( 0 ),
	m_TexManager(),
	m_InformTable(),
	m_pMainWindow( 0 ),
	m_Width( 0 ),
	m_Height( 0 ),
	m_bResourcesLoaded( false )
{

}

iz3d::UI::HotKeyOSD::~HotKeyOSD( void )
{
	delete m_pMainWindow;
}

void iz3d::UI::HotKeyOSD::Clear()
{
	Uninitialize();
	if( m_pDevice )
	{
		m_pDevice->Release();
		m_pDevice = 0;
	}
}

HRESULT iz3d::UI::HotKeyOSD::Initialize( ICommonDrawer* pDrawer )
{
	if( !pDrawer )
		return E_FAIL;
	m_pDevice = new DX10Device( pDrawer );
	m_TexManager.SetDevice( m_pDevice );

	iz3d::resources::TStringResourceSingleton::instance().setLanguage( gInfo.Language );
	m_InformTable.ReadTextMessages();
	m_pMainWindow = new CXDesktopWindow( m_pDevice );
	m_pMainWindow->SetVisible( false );
	return S_OK;
}

HRESULT iz3d::UI::HotKeyOSD::Initialize( IDirect3DDevice9* pD3DDevice )
{
	if ( !pD3DDevice )
		return E_FAIL;
	m_pDevice = new DX9Device( pD3DDevice );
	m_TexManager.SetDevice( m_pDevice );

	iz3d::resources::TStringResourceSingleton::instance().setLanguage( gInfo.Language );
	m_InformTable.ReadTextMessages();
	m_pMainWindow = new CXDesktopWindow( m_pDevice );
	m_pMainWindow->SetVisible( false );
	return S_OK;
}

void iz3d::UI::HotKeyOSD::LoadResources()
{
	m_bResourcesLoaded = true;
	m_pMainWindow->LoadCanvas( m_TexManager.GetImage( _T( "BackGround.png" ) ) );
	m_pMainWindow->SetXYPos( m_Width / 10, m_Height / 8 );
	m_pMainWindow->SetWidthHeight( m_Width, m_Height );
	// fill the labels:
	LONG yStep = m_Height / (LONG)m_InformTable.GetLinesTable();
	for( size_t i = 0; i < m_InformTable.GetLinesTable(); ++i ){
		// first column
		CXLabel* pLabel = new CXLabel( ::uilib::Wizard::s_lf, m_pDevice );
		m_pMainWindow->AddChildControl( pLabel );
		pLabel->SetCaption( m_InformTable.GetItem( i, 0 ) );
		pLabel->SetXYPos( 0, yStep * (LONG)i );
		pLabel->SetWidthHeight( (LONG)(m_Width * 0.66f), yStep );
		pLabel->SetColour( D3DCOLOR_RGBA( 255, 255, 255, 255) );
		// second column
		pLabel = new CXLabel( ::uilib::Wizard::s_lf, m_pDevice );
		m_pMainWindow->AddChildControl( pLabel );
		pLabel->SetCaption( m_InformTable.GetItem( i, 1 ) );
		pLabel->SetXYPos( (LONG)(m_Width * 0.66f), yStep * (LONG)i );
		pLabel->SetWidthHeight( (LONG)(m_Width * 0.34f), yStep );
		pLabel->SetColour( D3DCOLOR_RGBA( 255, 255, 255, 255) );
	}
}

void iz3d::UI::HotKeyOSD::Render( bool isLeft )
{
	CommonRenderBegin();
	m_pDevice->SetRenderTarget( &isLeft );
	if( isLeft )
		m_pMainWindow->SetLeft();
	else
		m_pMainWindow->SetRight();
	m_pMainWindow->PostMessage( WM_PAINT, 0, 0, NULL );
	// restore states
	m_pDevice->ReturnRenderState();
}

void iz3d::UI::HotKeyOSD::Show()
{
	if( !m_pMainWindow )
		return;
	if( m_pMainWindow->GetVisible() )
		return;
	m_pMainWindow->SetVisible( true );
}

void iz3d::UI::HotKeyOSD::Hide()
{
	if( !m_pMainWindow )
		return;
	if( !m_pMainWindow->GetVisible() )
		return;
	m_pMainWindow->SetVisible( false );
}

void iz3d::UI::HotKeyOSD::RenderDX9(  bool isStereoActive, RECT* pRightViewRect,
	RECT* pLeftViewRect, void* pBufferRight, void* pBufferLeft)
{
	if ( !m_pMainWindow )
		return;
	if ( !m_bResourcesLoaded )
		LoadResources();

	m_pDevice->SetNeededRenderState();
	RECT* pRect;
	// if is it stereo, first draw right buffer
	if ( isStereoActive )
	{
		pRect = pRightViewRect;
		m_pMainWindow->SetXYPos( ( LONG )( pRect->left ) + m_Width / 10, ( LONG )( pRect->top ) + m_Height / 8 );
		m_pMainWindow->SetRight();
		m_pDevice->SetRenderTarget( pBufferRight );
		m_pMainWindow->PostMessage( WM_PAINT, 0, 0, NULL );
	}
	pRect = pLeftViewRect;
	m_pMainWindow->SetXYPos( ( LONG )( pRect->left ) + m_Width / 10, ( LONG )( pRect->top )  + m_Height / 8 );
	m_pMainWindow->SetLeft();
	m_pDevice->SetRenderTarget( pBufferLeft );
	m_pMainWindow->PostMessage( WM_PAINT, 0, 0, NULL );
}

void iz3d::UI::HotKeyOSD::SetSize( LONG Width, LONG Height )
{
	m_Width = Width / 2;
	m_Height = (LONG)(Height * 0.8f);
}

void iz3d::UI::HotKeyOSD::SetCurrentSwapChain( void* pSwapChain )
{
	if( m_pDevice )
		m_pDevice->SetCurrentSwapChain( pSwapChain );
}

void iz3d::UI::HotKeyOSD::Uninitialize()
{
	m_bResourcesLoaded = false;
	m_TexManager.Clear();
	COMLikeRepository::instance().Drop();
}

void iz3d::UI::HotKeyOSD::CommonRenderBegin()
{
	if ( !m_pMainWindow )
		return;
	if ( !m_bResourcesLoaded )
		LoadResources();

	m_pDevice->SetNeededRenderState();
}
