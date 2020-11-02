#include <WTypes.h>
#include "Wizard.h"
#include "..\CommonUtils\StringResourceManager.h"
#include "..\CommonUtils\System.h"
#include "DX9Device.h"
#include "DX10Device.h"
#include "ExtWindow.h"
#include "..\ArchiveFile\blob.h"

using namespace uilib;
using namespace iz3d::resources;

//////////////////////////////////////////////////////////////////////////
// constants

const LOGFONT Wizard::s_lf =
{
	-80,				// LONG lfHeight;
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

const LOGFONT Wizard::s_Notlf =
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

bool Wizard::s_bEnabled = false;

uilib::Wizard::Wizard():
m_pDevice( 0 )
{
}

uilib::Wizard::~Wizard()
{
	//SAFE_RELEASE( m_pDevice );
//	Clear();
}
//////////////////////////////////////////////////////////////////////////
HRESULT uilib::Wizard::Initialize( IDirect3DDevice9* pD3DDevice )
{
	if ( !pD3DDevice )
		return E_FAIL;
	m_pDevice = new DX9Device( pD3DDevice );
	HRESULT hr = CommonInitialize();
	return hr;
}

HRESULT uilib::Wizard::Initialize( ICommonDrawer* pDrawer )
{
	m_pDevice = new DX10Device( pDrawer );
	HRESULT hr = CommonInitialize();
	return hr;
}

void uilib::Wizard::Clear()
{
	SAFE_RELEASE( m_pDevice );
	m_TexManager.Clear();
	COMLikeRepository::instance().Drop();
}

HRESULT uilib::Wizard::CommonInitialize()
{
	m_TexManager.SetDevice( m_pDevice );

	TStringResourceSingleton::instance().setLanguage( gInfo.Language );
	CXExtWindow::SetCheckedText( TStringResourceSingleton::instance().getString( L"\\IDS_STARTUP_WIZARD_NO" ) );
	CXExtWindow::SetUncheckedText( TStringResourceSingleton::instance().getString( L"\\IDS_STARTUP_WIZARD" ) );
	return S_OK;
}

ICommonTexture* uilib::TexturesManager::GetImage( const wchar_t* pResName )
{
	TextureMap_t::const_iterator it = m_Data.find( pResName );
	if ( it != m_Data.end() )
		return it->second;
	Blob b;
	TCHAR fullPath[MAX_PATH];
	_tcscpy( fullPath, _T("Wizard/") );
	_tcscat( fullPath, pResName );
	if ( !b.Initialize( fullPath ) )
		return 0;
	
	ICommonTexture* pTex = 0;
	HRESULT hr = m_pDevice->CreateTextureFromMemory( b.GetData(), b.GetSize(), &pTex );
	if( FAILED( hr ) )
		return 0;
	m_Data[pResName] = pTex;
	return pTex;
}
