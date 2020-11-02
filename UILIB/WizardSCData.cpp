#include "WizardSCData.h"
#include "..\CommonUtils\StringResourceManager.h"
#include "..\CommonUtils\System.h"
#include "DX9Device.h"
#include "DX10Device.h"
#include "ExtWindow.h"
#include "..\ArchiveFile\blob.h"

using namespace uilib;
using namespace iz3d::resources;

uilib::WizardSCData::WizardSCData()
	: m_pWizard( 0 ),
	m_pDesktop( 0 )
{
}

uilib::WizardSCData::~WizardSCData()
{
	//Clear();
	SAFE_DELETE( m_pDesktop );
}

void uilib::WizardSCData::RenderDX9( bool isStereoActive, RECT* pRightViewRect,
	RECT* pLeftViewRect, void* pBufferRight, void* pBufferLeft )
{
	CommonRenderBegin();
	RECT* pRect;
	// if is it stereo, first draw right buffer
	if ( isStereoActive )
	{
		pRect = pRightViewRect;
		m_pDesktop->SetXYPos( ( LONG )( pRect->left ), ( LONG )( pRect->top ) );
		m_pDesktop->SetRight();
		GetDevice()->SetRenderTarget( pBufferRight );
		m_pDesktop->PostMessage( WM_PAINT, 0, 0, NULL );
	}
	pRect = pLeftViewRect;
	m_pDesktop->SetXYPos( ( LONG )( pRect->left ), ( LONG )( pRect->top ) );
	m_pDesktop->SetLeft();
	GetDevice()->SetRenderTarget( pBufferLeft );
	m_pDesktop->PostMessage( WM_PAINT, 0, 0, NULL );
	CommonRenderEnd();
}

//////////////////////////////////////////////////////////////////////////
void uilib::WizardSCData::ProcessKeys()
{	
	if ( IsWizardHotKeyPressed() )
	{
		if ( m_pWizard->s_bEnabled )
			Hide();
		else
			Show();
	}
	if ( !m_pWizard->s_bEnabled )
		return;
	
	static WizardKeyInfo wk;
	WPARAM wParam = 0;
	for ( int i = 0; i < 8; ++i )
	{
		if ( gInfo.WizKeyInfo.keys[i].isPressed && !wk.keys[i].isPressed )
		{
			wParam = gInfo.WizKeyInfo.keys[i].wParam;
			break;
		}
	}
	wk = gInfo.WizKeyInfo;
	if( !wParam )
		return;

	if ( wParam == WIZARD_STARTUP_HOTKEY )
	{
		gInfo.Input.ShowWizardAtStartup = !gInfo.Input.ShowWizardAtStartup;
		return;
	}
	m_pDesktop->PostMessage( WM_KEYDOWN, wParam, 0, 0 );
}

//////////////////////////////////////////////////////////////////////////
void uilib::WizardSCData::addPictureSignatures( CXExtWindow* pStep )
{
	LONG Width = pStep->GetWidth() / 2;
	LONG Height = pStep->GetHeight() / 2;

	CXLabel* pLeftLabel = new CXLabel( Wizard::s_Notlf, GetDevice() );
	pStep->AddChildControl( pLeftLabel );
	pLeftLabel->SetCaption( L"Uncomfortable" );
	pLeftLabel->SetXYPos( 0, Height / 16 );
	pLeftLabel->SetWidthHeight( Width, Height / 4 );
	pLeftLabel->SetColour( UNCOMFORTABLE_TEXT_COLOR );

	CXLabel* pRightLabel = new CXLabel( Wizard::s_Notlf, GetDevice() );
	pStep->AddChildControl( pRightLabel );
	pRightLabel->SetCaption( L"Comfortable" );
	pRightLabel->SetXYPos( Width, Height / 16 );
	pRightLabel->SetWidthHeight( Width, Height / 4 );
	pRightLabel->SetColour( COMFORTABLE_TEXT_COLOR );
}

//////////////////////////////////////////////////////////////////////////
void uilib::WizardSCData::ConfigureStereoSampleStep(
	CXExtWindow* pStep,
	const TCHAR* pLeftResName,
	const TCHAR* pRightResName )
{
	CXCanvas* pLeftStereoCanvas = new CXCanvas( GetDevice() );
	pLeftStereoCanvas->EnableStereo();
	pLeftStereoCanvas->SetTexture( GetImage( pLeftResName ) );

	CXCanvas* pRightStereoCanvas = new CXCanvas( GetDevice() );
	pRightStereoCanvas->SetTexture( GetImage( pRightResName ) );
	pRightStereoCanvas->EnableStereo();

	LONG Width = pStep->GetWidth() / 2;
	LONG Height = pStep->GetHeight() / 2;

	
	CXWindow* pLeftWindow = new CXWindow( GetDevice() );
		pLeftWindow->SetXYPos( 0, Height / 4 );
		pLeftWindow->SetWidthHeight( Width, Height );
		pLeftWindow->SetCanvas( pLeftStereoCanvas );
	

	CXWindow* pRightWindow = new CXWindow( GetDevice() );
	pRightWindow->SetXYPos( Width, Height / 4 );
	pRightWindow->SetWidthHeight( Width, Height );
	pRightWindow->SetCanvas( pRightStereoCanvas );

	pStep->AddChildControl( pLeftWindow );
	pStep->AddChildControl( pRightWindow );
}


void uilib::WizardSCData::ConfigureStereoStep( CXExtWindow* pStep, const TCHAR* pResName )
{
	CXCanvas* pStereoCanvas = new CXCanvas( GetDevice() );
	pStereoCanvas->EnableStereo();
	pStereoCanvas->SetTexture( GetImage( pResName ) );

	pStep->SetCanvas( pStereoCanvas );
}
//////////////////////////////////////////////////////////////////////////
void uilib::WizardSCData::Initialize( Wizard* pWizard, DWORD Width, DWORD Height )
{
	m_pWizard = pWizard;
	m_bResourcesLoaded = false;
	// creating desktop
	m_pDesktop = new CXDesktopWindow( pWizard->GetDevice() );
	m_pDesktop->SetVisible( false );
	// native size
	m_Width = Width;
	m_Height = Height;
}
//////////////////////////////////////////////////////////////////////////
void uilib::WizardSCData::LoadResources( )
{
	m_bResourcesLoaded = true;

	m_pDesktop->LoadCanvas( GetImage( _T( "BackGround.png" ) ) );
	m_pDesktop->SetXYPos( 0, 0 );
	m_pDesktop->SetWidthHeight( m_Width, m_Height );

	// Step 1
	CXExtWindow* pStep1 = new CXExtWindow( GetDevice() );

	// Step 2
	CXExtWindow* pStep2 = new CXExtWindow( GetDevice() );

	// Step 3 No
	CXExtWindow* pStep3No = new CXExtWindow( GetDevice() );

	// Step 3 Yes
	CXExtWindow* pStep3Yes = new CXExtWindow( GetDevice() );

	// Step 4 Yes
	CXExtWindow* pStep4Yes = new CXExtWindow( GetDevice() );
	CXExtWindow* pStep4SampleYes = new CXExtWindow( GetDevice() );

	// Step 4 No
	CXExtWindow* pStep4No = new CXExtWindow( GetDevice() );

	// Step 5
	CXExtWindow* pStep5 = new CXExtWindow( GetDevice() );
	CXExtWindow* pStep5Sample = new CXExtWindow( GetDevice() );

	// Step 6 No
	CXExtWindow* pStep6No = new CXExtWindow( GetDevice() );
	CXExtWindow* pStep6SampleNo = new CXExtWindow( GetDevice() );

	// Step 6 Yes
	CXExtWindow* pStep6Yes = new CXExtWindow( GetDevice() );
	CXExtWindow* pStep6SampleYes = new CXExtWindow( GetDevice() );

	// Step 7
	CXExtWindow* pStep7 = new CXExtWindow( GetDevice() );
	CXExtWindow* pStep7Sample = new CXExtWindow( GetDevice() );

	// Step 8
	CXExtWindow* pStep8 = new CXExtWindow( GetDevice() );

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

	// Step 6 No *Sample
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
	ConfigureStereoStep( pStep2, L"GrantMooreFINAL.JPG" );

	// 4 step stereo
	ConfigureStereoSampleStep( pStep4SampleYes, L"4-1.jps", L"4-2.jps" );

	// 5 step stereo
	ConfigureStereoSampleStep( pStep5Sample, L"5-1.jps", L"5-2.jps" );

	addPictureSignatures( pStep5Sample );

	// 6 yes step stereo
	ConfigureStereoSampleStep( pStep6SampleYes, L"6-1.jps", L"6-2.jps" );

	// 6 no step stereo
	ConfigureStereoSampleStep( pStep6SampleNo, L"6-1.jps", L"6-2.jps" );

	// 7 step stereo
	ConfigureStereoSampleStep( pStep7Sample, L"7-1.jps", L"7-2.jps" );

	SelectFirstControl();

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
	gInfo.WizardWasShown = true;
}

//////////////////////////////////////////////////////////////////////////
void uilib::WizardSCData::Hide()
{
	if ( !m_pDesktop )
		return;
	if ( !m_pDesktop->GetVisible() )
		return;
	m_pDesktop->SetVisible( false );
	m_pWizard->s_bEnabled = false;
}

void uilib::WizardSCData::Clear()
{
	m_bResourcesLoaded = false;
	SAFE_DELETE( m_pDesktop);
}

//////////////////////////////////////////////////////////////////////////
HRESULT uilib::WizardSCData::ConfigureCommonStep( CXExtWindow* pWindow,
										   const wchar_t* IDCaptionText,
										   const wchar_t* IDFirstButtonText,
										   const wchar_t* IDSecondButtonText,
										   const wchar_t* IDThirdButtonText,
										   const wchar_t* IDFourthButtonText )
{
	// startup notation
	m_pDesktop->AddChildControl( pWindow );
	int xPos = m_pDesktop->GetWidth() / 20;
	int yStep = m_pDesktop->GetHeight() / 20;
	pWindow->SetWidthHeight( m_pDesktop->GetWidth(), m_pDesktop->GetHeight() );

	// startup notation
	CXCheckBox* pBox = new CXCheckBox( m_pWizard->s_Notlf, GetDevice() );
	pBox->LoadCheckedImage( GetImage( _T("Checked.png") ) );
	pBox->LoadUncheckedImage( GetImage(_T("UnChecked.png") ) );
	pWindow->AddChildControl( pBox );
	pBox->SetXYPos( ( pWindow->GetWidth() - 600 )  / 2, yStep / 2 );
	pBox->SetWidthHeight( 600, 30 );
	pBox->SetCaptionColour( CAPTION_TEXT_COLOR );

	// caption
	CXLabel* pLabel = new CXLabel( m_pWizard->s_lf, GetDevice() );
	pWindow->AddChildControl( pLabel );
	pLabel->SetCaption( TStringResourceSingleton::instance().getString( IDCaptionText ) );
	pLabel->SetYPos( yStep * 12 );
	pLabel->SetWidthHeight( pWindow->GetWidth(), yStep * 4 );

	// small notation
	CXLabel* pNotationLabel = new CXLabel( m_pWizard->s_Notlf, GetDevice() );
	pWindow->AddChildControl( pNotationLabel );
	pNotationLabel->SetCaption(
		TStringResourceSingleton::instance().getString( L"\\IDS_EXITBUTTON" ) );
	pNotationLabel->SetColour( CAPTION_TEXT_COLOR );
	pNotationLabel->SetXPos( pWindow->GetWidth() / 4 );
	pNotationLabel->SetWidthHeight( pWindow->GetWidth() / 2, yStep / 2 );

	// first button
	if ( IDFirstButtonText )
		CreateButton( IDFirstButtonText,		_T("YesButton.png"), 
			xPos * 4, yStep * 16, xPos * 6, yStep * 2, pWindow );

	// second button
	if ( IDSecondButtonText )
		CreateButton(IDSecondButtonText, _T("NoButton.png"), 
			xPos * 10, yStep * 16, xPos * 6, yStep * 2, pWindow );

	// third button
	if( IDThirdButtonText )
		CreateButton( IDThirdButtonText, _T("YesButton.png"), 
			xPos * 4, yStep * 18, xPos * 6, yStep * 2, pWindow );

	// fourth button
	if ( IDFourthButtonText )
		CreateButton( IDFourthButtonText, _T("NoButton.png"), 
			xPos * 10, yStep * 18, xPos * 6, yStep * 2, pWindow );

	pWindow->SetVisible(  false);
	return S_OK;
}

void uilib::WizardSCData::CreateButton( const wchar_t* IDButtonText, const wchar_t* pResName,
	LONG X, LONG Y, LONG Width, LONG Height, CXExtWindow* pWindow )
{
	CXButton* pButton = new CXButton( GetDevice() );
	pWindow->AddChildControl( pButton );
	pButton->SetCaption(
		TStringResourceSingleton::instance().getString( IDButtonText ) );
	pButton->SetXYPos( X, Y );
	pButton->SetWidthHeight( Width, Height );
	
	pButton->SetDefaultImage( GetImage( pResName ) );
}

bool uilib::WizardSCData::IsWizardHotKeyPressed()
{
	static bool pressed;
	bool ret = !pressed && gInfo.WizKeyInfo.shiftMultKey;
	pressed = gInfo.WizKeyInfo.shiftMultKey;
	return ret;
}

bool uilib::WizardSCData::Force3D()
{
	return 0;
}

void uilib::WizardSCData::SelectFirstControl()
{
	m_pDesktop->SetVisible( true );
	CXControl* pBufWindow = m_pDesktop->GetFirstChild();
	if ( !pBufWindow )
		return;
	
	pBufWindow->SetVisible( true );
	m_pDesktop->SetFocusControl( pBufWindow );
	for( int i = 1; i < m_pDesktop->GetChildCount(); i++ )
	{
		pBufWindow = pBufWindow->GetNextSibling();
		pBufWindow->SetVisible( false );
	}
}

ICommonTexture* uilib::WizardSCData::GetImage( const wchar_t* pResName )
{
	return m_pWizard->m_TexManager.GetImage( pResName );
}

void uilib::WizardSCData::RenderDX10( bool /*isStereoActive*/, bool isLeft )
{
	if( !m_pWizard->s_bEnabled )
		return;
	CommonRenderBegin();
	GetDevice()->SetRenderTarget( &isLeft );
	if( isLeft )
		m_pDesktop->SetLeft();
	else
		m_pDesktop->SetRight();
	m_pDesktop->PostMessage( WM_PAINT, 0, 0, NULL );
	// restore states
	GetDevice()->ReturnRenderState();
}

void uilib::WizardSCData::CommonRenderBegin()
{
	if ( !m_pWizard->s_bEnabled )
		return;
	if ( !m_pDesktop )
		return;
	if ( !m_bResourcesLoaded )
		LoadResources();

	GetDevice()->SetNeededRenderState();
}

void uilib::WizardSCData::CommonRenderEnd()
{
}

void uilib::WizardSCData::SetCurSC( void* pSwapChain )
{
	m_pWizard->GetDevice()->SetCurrentSwapChain( pSwapChain );
}

