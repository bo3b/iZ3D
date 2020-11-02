#include "stdafx.h"
#include <d3d9.h>
#include <d3dx9.h>
#include <WinDef.h>

#include "..\ArchiveFile\blob.h"
#include "..\CommonUtils\StringResourceManager.h"
#include "..\uilib\uilib.h"
#include "ExtWindow.h"
#include "Wizard.h"
#include "BaseSwapChain.h"

using namespace uilib;
using namespace iz3d::resources;

const TCHAR* Wizard::STR_THISDLLNAME = _T("S3DWrapperD3D9.dll");
const HMODULE Wizard::s_hModule = (HMODULE)GetModuleHandle(Wizard::STR_THISDLLNAME);

//////////////////////////////////////////////////////////////////////////
// constants

const LOGFONT Wizard::s_lf =
{
	-8,				// LONG lfHeight;
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
	PROOF_QUALITY,// BYTE lfQuality;
	VARIABLE_PITCH,		// BYTE lfPitchAndFamily; 
	_T("Arial")			// TCHAR lfFaceName[LF_FACESIZE]; 
};

const LOGFONT Wizard::s_Notlf =
{
	-7,				// LONG lfHeight;
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
	PROOF_QUALITY,// BYTE lfQuality;
	VARIABLE_PITCH,		// BYTE lfPitchAndFamily; 
	_T("Arial")			// TCHAR lfFaceName[LF_FACESIZE]; 
};

bool Wizard::s_bEnabled = false;

uilib::Wizard::Wizard(void): m_pDevice(0)
{
}

uilib::Wizard::~Wizard(void)
{
	Clear();
	m_pDevice = 0;
}
//////////////////////////////////////////////////////////////////////////
HRESULT uilib::Wizard::Initialize(IDirect3DDevice9* pD3DDevice)
{
	if (!pD3DDevice)
		return E_FAIL;
	m_pDevice = pD3DDevice;
	m_TexManager.SetDevice(pD3DDevice);

	TStringResourceSingleton::instance().setLanguage( gInfo.Language );
	CXExtWindow::SetCheckedText( TStringResourceSingleton::instance().getString(L"\\IDS_STARTUP_WIZARD_NO") );
	CXExtWindow::SetUncheckedText( TStringResourceSingleton::instance().getString(L"\\IDS_STARTUP_WIZARD" ) );
	return S_OK;
}

void uilib::Wizard::Clear()
{
	//m_TexManager.Clear();
}

uilib::WizardSCData::WizardSCData(void): m_pWizard(0),
	m_pDesktop(0)
{
}

uilib::WizardSCData::~WizardSCData(void)
{
	Clear();
	SAFE_DELETE(m_pDesktop);
}

void uilib::WizardSCData::Render(CBaseSwapChain* pSwapChain)
{
	if (!m_pWizard->s_bEnabled)
		return;

	HRESULT hResult;
	if (!m_pDesktop)
		return;
	if (!m_bReourcesLoaded)
		LoadResources();
	DWORD AlphaTestEnable;
	DWORD AlphaRef;
	DWORD SrcBlend;
	DWORD DestBlend;
	NSCALL( GetDevice()->GetRenderState( D3DRS_ALPHABLENDENABLE, &AlphaTestEnable ) );
	NSCALL( GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE ) );

	NSCALL( GetDevice()->GetRenderState( D3DRS_ALPHAREF, &AlphaRef ) );
	NSCALL( GetDevice()->SetRenderState( D3DRS_ALPHAREF, 0 ) );

	NSCALL( GetDevice()->GetRenderState( D3DRS_SRCBLEND, &SrcBlend ) );
	NSCALL( GetDevice()->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA ) );

	NSCALL( GetDevice()->GetRenderState( D3DRS_DESTBLEND, &DestBlend ) );
	NSCALL( GetDevice()->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA ) );

	RECT* pRect;
	// if is it stereo, first draw right buffer
	if (pSwapChain->IsStereoActive())
	{
		pRect = &pSwapChain->m_RightViewRect;
		m_pDesktop->SetXYPos((LONG)(pRect->left), (LONG)(pRect->top));
		m_pDesktop->SetRight();
		NSCALL(GetDevice()->SetRenderTarget(0, pSwapChain->m_pBackBufferRight));
		m_pDesktop->PostMessage(WM_PAINT, 0, 0, NULL);
	}
	pRect = &pSwapChain->m_LeftViewRect;
	m_pDesktop->SetXYPos((LONG)(pRect->left), (LONG)(pRect->top));
	m_pDesktop->SetLeft();
	NSCALL(GetDevice()->SetRenderTarget(0, pSwapChain->m_pBackBufferLeft));
	m_pDesktop->PostMessage(WM_PAINT, 0, 0, NULL);

	// restore states
	NSCALL( GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE, AlphaTestEnable ) );
	NSCALL( GetDevice()->SetRenderState( D3DRS_ALPHAREF, AlphaRef ) );
	NSCALL( GetDevice()->SetRenderState( D3DRS_SRCBLEND, SrcBlend ) );
	NSCALL( GetDevice()->SetRenderState( D3DRS_DESTBLEND, DestBlend ) );
}

//////////////////////////////////////////////////////////////////////////
void uilib::WizardSCData::ProcessKeys()
{	
	if (IsWizardHotKeyPressed())
	{
		if (m_pWizard->s_bEnabled)
			Hide();
		else
			Show();
	}
	if (m_pWizard->s_bEnabled)
	{
		static WizardKeyInfo wk;
		WPARAM wParam = 0;
		for (int i = 0; i < 8; ++i)
		{
			if (gInfo.WizKeyInfo.keys[i].isPressed && !wk.keys[i].isPressed)
			{
				wParam = gInfo.WizKeyInfo.keys[i].wParam;
				break;
			}
		}
		wk = gInfo.WizKeyInfo;
		if (wParam)
		{
			if (wParam == 'C')
			{
				gInfo.Input.ShowWizardAtStartup = !gInfo.Input.ShowWizardAtStartup;
				return;
			}
			m_pDesktop->PostMessage(WM_KEYDOWN, wParam, 0, 0);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void uilib::WizardSCData::addPictureSignatures( CXExtWindow* pStep )
{
	LONG Width = pStep->GetWidth() / 2;
	LONG Height = pStep->GetHeight() / 2;

	CXLabel* pLeftLabel = DNew CXLabel( Wizard::s_Notlf, GetDevice() );
	pStep->AddChildControl( pLeftLabel );
	pLeftLabel->SetCaption( L"Uncomfortable" );
	pLeftLabel->SetXYPos( 0, Height / 16 );
	pLeftLabel->SetWidthHeight( Width, Height / 4 );
	pLeftLabel->SetColour(D3DCOLOR_RGBA(255, 0, 0, 255 ));

	CXLabel* pRightLabel = DNew CXLabel( Wizard::s_Notlf, GetDevice() );
	pStep->AddChildControl( pRightLabel );
	pRightLabel->SetCaption( L"Comfortable" );
	pRightLabel->SetXYPos( Width, Height / 16 );
	pRightLabel->SetWidthHeight( Width, Height / 4 );
	pRightLabel->SetColour(D3DCOLOR_RGBA(0, 255, 255, 255 ));
}

//////////////////////////////////////////////////////////////////////////
void uilib::WizardSCData::ConfigureStereoSampleStep(
	CXExtWindow* pStep,
	const TCHAR* pLeftResName,
	const TCHAR* pRightResName )
{
	CXCanvas* pLeftStereoCanvas = DNew CXCanvas( GetDevice() );
	pLeftStereoCanvas->EnableStereo();
	pLeftStereoCanvas->SetTexture( GetImage( pLeftResName ) );

	CXCanvas* pRightStereoCanvas = DNew CXCanvas( GetDevice() );
	pRightStereoCanvas->SetTexture( GetImage( pRightResName ) );
	pRightStereoCanvas->EnableStereo();

	LONG Width = pStep->GetWidth() / 2;
	LONG Height = pStep->GetHeight() / 2;

	
	CXWindow* pLeftWindow = DNew CXWindow( GetDevice() );
	pLeftWindow->SetXYPos( 0, Height / 4 );
	pLeftWindow->SetWidthHeight( Width, Height );
	pLeftWindow->SetCanvas( pLeftStereoCanvas );
	

	CXWindow* pRightWindow = DNew CXWindow( GetDevice() );
	pRightWindow->SetXYPos( Width, Height / 4 );
	pRightWindow->SetWidthHeight( Width, Height );
	pRightWindow->SetCanvas( pRightStereoCanvas );

	pStep->AddChildControl( pLeftWindow );
	pStep->AddChildControl( pRightWindow );

	
}


void uilib::WizardSCData::ConfigureStereoStep( CXExtWindow* pStep, const TCHAR* pResName )
{
	CXCanvas* pStereoCanvas = DNew CXCanvas( GetDevice() );
	pStereoCanvas->EnableStereo();
	pStereoCanvas->SetTexture( GetImage( pResName ) );

	pStep->SetCanvas( pStereoCanvas );
}
//////////////////////////////////////////////////////////////////////////
void uilib::WizardSCData::Initialize( Wizard* pWizard, CBaseSwapChain* pSwapChain )
{
	m_pWizard = pWizard;
	m_bReourcesLoaded = false;
	// creating desktop
	if (!m_pDesktop)
	{
		m_pDesktop = DNew CXDesktopWindow(pWizard->GetDevice());
		m_pDesktop->SetVisible(false);
	}
	else
	{
		DEBUG_TRACE1(_T("Wizard resources restored\n"));
		m_pDesktop->OnResetDevice();
	}

	// native size
	m_Width = pSwapChain->m_BackBufferSize.cx;
	m_Height = pSwapChain->m_BackBufferSize.cy;
}
//////////////////////////////////////////////////////////////////////////
void uilib::WizardSCData::LoadResources( )
{
	m_bReourcesLoaded = true;

	m_pDesktop->LoadCanvas( GetImage( _T( "BackGround.png" ) ) );
	m_pDesktop->SetXYPos(0, 0);
	m_pDesktop->SetWidthHeight(m_Width, m_Height);

	// Step 1
	CXExtWindow* pStep1 = DNew CXExtWindow( this );

	// Step 2
	CXExtWindow* pStep2 = DNew CXExtWindow( this );

	// Step 3 No
	CXExtWindow* pStep3No = DNew CXExtWindow( this );

	// Step 3 Yes
	CXExtWindow* pStep3Yes = DNew CXExtWindow( this );

	// Step 4 Yes
	CXExtWindow* pStep4Yes = DNew CXExtWindow( this );
	CXExtWindow* pStep4SampleYes = DNew CXExtWindow( this );

	// Step 4 No
	CXExtWindow* pStep4No = DNew CXExtWindow( this );

	// Step 5
	CXExtWindow* pStep5 = DNew CXExtWindow( this );
	CXExtWindow* pStep5Sample = DNew CXExtWindow( this );

	// Step 6 No
	CXExtWindow* pStep6No = DNew CXExtWindow( this );
	CXExtWindow* pStep6SampleNo = DNew CXExtWindow( this );

	// Step 6 Yes
	CXExtWindow* pStep6Yes = DNew CXExtWindow( this );
	CXExtWindow* pStep6SampleYes = DNew CXExtWindow( this );

	// Step 7
	CXExtWindow* pStep7 = DNew CXExtWindow( this );
	CXExtWindow* pStep7Sample = DNew CXExtWindow( this );

	// Step 8
	CXExtWindow* pStep8 = DNew CXExtWindow( this );

	ConfigureCommonStep( pStep1, L"\\IDS_STEP1", 0, L"\\IDS_NEXTBUTTON" );

	ConfigureCommonStep( pStep2, L"\\IDS_STEP2", L"\\IDS_YESBUTTON", L"\\IDS_NOBUTTON" );

	ConfigureCommonStep( pStep3No, L"\\IDS_STEP3NO", L"\\IDS_BACKBUTTON", L"\\IDS_FINISHBUTTON" );
	ConfigureCommonStep( pStep3Yes, L"\\IDS_STEP3YES", L"\\IDS_YESBUTTON", L"\\IDS_NOBUTTON" );

	ConfigureCommonStep( pStep4Yes,
		L"\\IDS_STEP4YES",
		L"\\IDS_BACKBUTTON",
		L"\\IDS_NEXTBUTTON",
		L"\\IDS_SHOWSAMPLE");
	ConfigureCommonStep( pStep4SampleYes, L"\\IDS_STEP4YES", L"\\IDS_HIDESAMPLE" );

	ConfigureCommonStep( pStep4No, L"\\IDS_STEP4NO", L"\\IDS_BACKBUTTON", L"\\IDS_FINISHBUTTON" );

	ConfigureCommonStep( pStep5,
		L"\\IDS_STEP5",
		L"\\IDS_YESBUTTON",
		L"\\IDS_NOBUTTON",
		L"\\IDS_BACKBUTTON",
		L"\\IDS_SHOWSAMPLE");
	ConfigureCommonStep( pStep5Sample, L"\\IDS_STEP5", L"\\IDS_HIDESAMPLE" );

	ConfigureCommonStep( pStep6Yes,
		L"\\IDS_STEP6YES",
		L"\\IDS_BACKBUTTON",
		L"\\IDS_NEXTBUTTON",
		L"\\IDS_SHOWSAMPLE" );
	ConfigureCommonStep( pStep6SampleYes, L"\\IDS_STEP6YES", L"\\IDS_HIDESAMPLE" );

	ConfigureCommonStep( pStep6No,
		L"\\IDS_STEP6NO",
		L"\\IDS_BACKBUTTON",
		L"\\IDS_NEXTBUTTON",
		L"\\IDS_SHOWSAMPLE" );
	ConfigureCommonStep( pStep6SampleNo, L"\\IDS_STEP6NO", L"\\IDS_HIDESAMPLE" );

	ConfigureCommonStep( pStep7,
		L"\\IDS_STEP7",
		L"\\IDS_BACKBUTTON",
		L"\\IDS_NEXTBUTTON",
		L"\\IDS_SHOWSAMPLE" );
	ConfigureCommonStep( pStep7Sample, L"\\IDS_STEP7", L"\\IDS_HIDESAMPLE" );

	ConfigureCommonStep( pStep8, L"\\IDS_STEP8", L"\\IDS_BACKBUTTON", L"\\IDS_FINISHBUTTON" );

	// Step 1
	pStep1->AddReact( ID_N_KEY, pStep2 );

	// Step 2
	pStep2->AddReact( ID_Y_KEY, pStep3Yes );
	pStep2->AddReact( ID_N_KEY, pStep3No );

	// Step 3 Yes
	pStep3Yes->AddReact( ID_Y_KEY, pStep4Yes );
	pStep3Yes->AddReact( ID_N_KEY, pStep4No );

	// Step 3 No
	pStep3No->AddReact(ID_B_KEY, pStep2 );

	// Step 4 Yes
	pStep4Yes->AddReact( ID_N_KEY, pStep5 );
	pStep4Yes->AddReact( ID_B_KEY, pStep3Yes );
	pStep4Yes->AddReact( ID_S_KEY, pStep4SampleYes );

	// Step 4 SampleYes
	pStep4SampleYes->AddReact( ID_S_KEY, pStep4Yes );

	// Step 4 No
	pStep4No->AddReact( ID_B_KEY, pStep3Yes );

	// Step 5 
	pStep5->AddReact( ID_Y_KEY, pStep6Yes );
	pStep5->AddReact( ID_N_KEY, pStep6No );
	pStep5->AddReact( ID_B_KEY, pStep4Yes );
	pStep5->AddReact( ID_S_KEY, pStep5Sample );

	// Step 5 Sample
	pStep5Sample->AddReact( ID_S_KEY, pStep5 );

	// Step 6 No
	pStep6No->AddReact( ID_N_KEY, pStep7 );
	pStep6No->AddReact( ID_B_KEY, pStep5 );
	pStep6No->AddReact( ID_S_KEY, pStep6SampleNo );

	// Step 6 No Sample
	pStep6SampleNo->AddReact( ID_S_KEY, pStep6No );

	// Step 6 Yes
	pStep6Yes->AddReact( ID_N_KEY, pStep7 );
	pStep6Yes->AddReact( ID_B_KEY, pStep5 );
	pStep6Yes->AddReact( ID_S_KEY, pStep6SampleYes );

	// Step 6 Yes Sample
	pStep6SampleYes->AddReact( ID_S_KEY, pStep6Yes );

	// Step 7
	pStep7->AddReact( ID_B_KEY, pStep5 );
	pStep7->AddReact( ID_N_KEY, pStep8 );
	pStep7->AddReact( ID_S_KEY, pStep7Sample );

	// Step 7 sample
	pStep7Sample->AddReact( ID_S_KEY, pStep7 );

	// Step 8
	pStep8->AddReact( ID_B_KEY, pStep7 );

	// Load pictures
	ConfigureStereoStep(pStep2, L"GrantMooreFINAL.JPG" );

	// 4 step stereo
	ConfigureStereoSampleStep( pStep4SampleYes, L"4-1.jps", L"4-2.jps" );

	// 5 step stereo
	ConfigureStereoSampleStep( pStep5Sample, L"5-1.jps", L"5-2.jps" );

	addPictureSignatures( pStep5Sample );

	// 6 yes step stereo
	ConfigureStereoSampleStep(pStep6SampleYes, L"6-1.jps", L"6-2.jps");

	// 6 no step stereo
	ConfigureStereoSampleStep(pStep6SampleNo, L"6-1.jps", L"6-2.jps");

	// 7 step stereo
	ConfigureStereoSampleStep(pStep7Sample, L"7-1.jps", L"7-2.jps");

	SelectFirstControl();

	DEBUG_TRACE1(_T("Wizard resources loaded\n"));
}

//////////////////////////////////////////////////////////////////////////
void uilib::WizardSCData::Show()
{
	if (!m_pDesktop)
		return;

	if (m_pDesktop->GetVisible())
		return;

	SelectFirstControl();
	m_pWizard->s_bEnabled = true;
}

//////////////////////////////////////////////////////////////////////////
void uilib::WizardSCData::Hide()
{
	if (!m_pDesktop)
		return;
	if (!m_pDesktop->GetVisible())
		return;
	m_pDesktop->SetVisible(false);
	m_pWizard->s_bEnabled = false;
}

void uilib::WizardSCData::Clear()
{
	m_bReourcesLoaded = false;
	if (m_pDesktop)
	{
		DEBUG_TRACE1(_T("Wizard resources released\n"));
		m_pDesktop->OnLostDevice();
	}
}

//////////////////////////////////////////////////////////////////////////
HRESULT uilib::WizardSCData::ConfigureCommonStep(CXExtWindow* pWindow,
										   const wchar_t* IDCaptionText,
										   const wchar_t* IDFirstButtonText,
										   const wchar_t* IDSecondButtonText,
										   const wchar_t* IDThirdButtonText,
										   const wchar_t* IDFourthButtonText)
{
	// startup notation
	m_pDesktop->AddChildControl(pWindow);
	int xPos = m_pDesktop->GetWidth() / 20;
	int yStep = m_pDesktop->GetHeight() / 20;
	pWindow->SetWidthHeight(m_pDesktop->GetWidth(), m_pDesktop->GetHeight());

	// startup notation
	CXCheckBox* pBox = DNew CXCheckBox(m_pWizard->s_Notlf, GetDevice());
	pBox->LoadCheckedImage( GetImage(_T("Checked.png")) );
	pBox->LoadUncheckedImage( GetImage(_T("UnChecked.png")) );
	pWindow->AddChildControl(pBox);
	pBox->SetXYPos((pWindow->GetWidth() - 600)  / 2, yStep / 2 );
	pBox->SetWidthHeight(600, 30);
	pBox->SetCaptionColour( D3DCOLOR_RGBA( 255, 255, 255, 255) );

	// caption
	CXLabel* pLabel = DNew CXLabel(m_pWizard->s_lf, GetDevice());
	pWindow->AddChildControl(pLabel);
	pLabel->SetCaption( TStringResourceSingleton::instance().getString( IDCaptionText ) );
	pLabel->SetYPos( (LONG) (yStep * 4.5f) );
	pLabel->SetWidthHeight( pWindow->GetWidth(), pWindow->GetHeight());

	// small notation
	CXLabel* pNotationLabel = DNew CXLabel(m_pWizard->s_Notlf, GetDevice());
	pWindow->AddChildControl(pNotationLabel);
	pNotationLabel->SetCaption(
		TStringResourceSingleton::instance().getString( L"\\IDS_EXITBUTTON" ) );
	pNotationLabel->SetColour( D3DCOLOR_RGBA(255, 255, 255, 255 ) );
	pNotationLabel->SetWidthHeight( pWindow->GetWidth(), yStep );

	// first button
	if (IDFirstButtonText)
		CreateButton(IDFirstButtonText,		_T("YesButton.png"), 
			xPos * 4, yStep * 16, xPos * 6, yStep * 2, pWindow);

	// second button
	if (IDSecondButtonText)
		CreateButton(IDSecondButtonText,	_T("NoButton.png"), 
			xPos * 10, yStep * 16, xPos * 6, yStep * 2, pWindow);

	// third button
	if( IDThirdButtonText )
		CreateButton(IDThirdButtonText,		_T("YesButton.png"), 
			xPos * 4, yStep * 18, xPos * 6, yStep * 2, pWindow);

	// fourth button
	if ( IDFourthButtonText )
		CreateButton(IDFourthButtonText,	_T("NoButton.png"), 
			xPos * 10, yStep * 18, xPos * 6, yStep * 2, pWindow);

	pWindow->SetVisible(false);
	return S_OK;
}

void uilib::WizardSCData::CreateButton(const wchar_t* IDButtonText, const wchar_t* pResName, LONG X, LONG Y, LONG Width, LONG Height, CXExtWindow* pWindow)
{
	CXButton* pButton = DNew CXButton(GetDevice());
	pWindow->AddChildControl(pButton);
	pButton->SetCaption(
		TStringResourceSingleton::instance().getString( IDButtonText ) );
	pButton->SetXYPos(X, Y);
	pButton->SetWidthHeight(Width, Height);
	
	pButton->SetDefaultImage( GetImage(pResName) );
}

bool uilib::WizardSCData::IsWizardHotKeyPressed()
{
	static bool pressed;
	bool ret = !pressed && gInfo.WizKeyInfo.shiftMultKey;
	pressed = gInfo.WizKeyInfo.shiftMultKey;
	return ret;
}

bool uilib::WizardSCData::Force3D( )
{
/*	if(m_pStep2)
		return m_pStep2->GetVisible();*/
	return 0;
}

void uilib::WizardSCData::SelectFirstControl()
{
	m_pDesktop->SetVisible(true);
	CXControl* pBufWindow = m_pDesktop->GetFirstChild();
	if (pBufWindow)
	{
		pBufWindow->SetVisible(true);
		m_pDesktop->SetFocusControl(pBufWindow);
		for(int i = 1; i < m_pDesktop->GetChildCount(); i++)
		{
			pBufWindow = pBufWindow->GetNextSibling();
			pBufWindow->SetVisible(false);
		}
	}
}

LPDIRECT3DTEXTURE9 uilib::WizardSCData::GetImage( const wchar_t* pResName )
{
	return m_pWizard->m_TexManager.GetImage(pResName);
}


LPDIRECT3DTEXTURE9 uilib::TexturesManager::GetImage( const wchar_t* pResName )
{
	TextureMap_t::const_iterator it = m_Data.find(pResName);
	if (it == m_Data.end())
		return LoadImage(pResName);
	else
		return it->second.m_T.p;
}

LPDIRECT3DTEXTURE9 uilib::TexturesManager::LoadImage( const wchar_t* pResName )
{
	Blob b;
	TCHAR fullPath[MAX_PATH];
	_tcscpy(fullPath, _T("Wizard/"));
	_tcscat(fullPath, pResName);
	if (b.Initialize( fullPath ))
	{
		CComPtr<IDirect3DTexture9> pTex = 0;
		HRESULT hr = LoadTextureFromMemory(m_pDevice, b.GetData(), b.GetSize(), &pTex);
		if( FAILED( hr ) )
			return 0;
		D3DSURFACE_DESC desc;
		pTex->GetLevelDesc( 0 , & desc);
		m_Data[pResName] = pTex;
		return pTex;
	}
	return NULL;
}