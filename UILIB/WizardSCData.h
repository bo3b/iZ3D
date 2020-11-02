#pragma once

#include "Wizard.h"

namespace uilib
{
	class WizardSCData
	{
		Wizard* m_pWizard;

		// hot keys codes
		static const int ID_Y_KEY = 0x59;
		static const int ID_N_KEY = 0x4e;
		static const int ID_P_KEY = 0x50;
		static const int ID_F_KEY = 0x46;
		static const int ID_R_KEY = 0x52;
		static const int ID_S_KEY = 0x53;
		static const int ID_B_KEY = 0x42;

		static const char WIZARD_STARTUP_HOTKEY = 'C';
		static const DWORD COMFORTABLE_TEXT_COLOR = D3DCOLOR_RGBA(0, 255, 255, 255 );
		static const DWORD UNCOMFORTABLE_TEXT_COLOR = D3DCOLOR_RGBA(255, 0, 0, 255 );
		static const DWORD CAPTION_TEXT_COLOR = D3DCOLOR_RGBA( 255, 255, 255, 255);

		// fields
		bool m_bResourcesLoaded;
		DWORD m_Width;
		DWORD m_Height;
		CXDesktopWindow* m_pDesktop;

		ICommonTexture* GetImage( const wchar_t* pResName );

		void SelectFirstControl();
		static bool IsWizardHotKeyPressed();
		void addPictureSignatures( CXExtWindow* pStep );
		void CommonRenderBegin();
		void CommonRenderEnd();
	public:
		void GetSize(DWORD& Width, DWORD& Height) { Width = m_Width; Height = m_Height; };
		iz3d::uilib::CommonDevice* GetDevice() { return m_pWizard->GetDevice(); }

		// configuring wizard
		HRESULT ConfigureCommonStep( CXExtWindow* pWindow,
			const wchar_t* IDCaptionText,
			const wchar_t* IDFirstButtonText = 0,
			const wchar_t* IDSecondButtonText = 0,
			const wchar_t* IDThirdButtonText = 0,
			const wchar_t* IDFourthButtonText = 0 );
		void CreateButton( const wchar_t* IDButtonText, const wchar_t* pResName, 
			LONG X, LONG Y, LONG Width, LONG Height, CXExtWindow* pWindow );
		//////////////////////////////////////////////////////////////////////////
		void ConfigureStereoSampleStep( CXExtWindow* pStep,
			const TCHAR* pLeftResName,
			const TCHAR* pRightResName );

		void ConfigureStereoStep( CXExtWindow* pStep,
			const TCHAR* pResName );

		void ProcessKeys();
		void Initialize( Wizard* pWizard, DWORD Width, DWORD Height );
		bool IsInitialized() { return m_pWizard != NULL; }
		void LoadResources( );
		void Show();
		void Hide();
		void RenderDX9( bool isStereoActive, RECT* pRightViewRect, RECT* pLeftViewRect, void* pBufferRight, void* pBufferLeft );

		void RenderDX10( bool isStereoActive, bool isLeft );
		bool Force3D();
		void Clear();
		WizardSCData();
		~WizardSCData();
		void SetCurSC( void* pSwapChain );
	};
}
