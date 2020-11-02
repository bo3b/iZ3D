#pragma once
#include "Wizard.h"
#include "InformTable.h"
namespace iz3d{
	namespace uilib{
		class CommonDevice;
	}
	namespace UI{
		class HotKeyOSD
		{
			uilib::CommonDevice* m_pDevice;
			::uilib::TexturesManager m_TexManager;
			InformTable m_InformTable;
			CXDesktopWindow* m_pMainWindow;
			LONG m_Width;
			LONG m_Height;
			bool m_bResourcesLoaded;
		public:
			HRESULT Initialize( ICommonDrawer* pDrawer );
			HRESULT Initialize( IDirect3DDevice9* pD3DDevice );
			void Clear();
			void Render( bool isLeft );
			void RenderDX9(  bool isStereoActive, RECT* pRightViewRect,
				RECT* pLeftViewRect, void* pBufferRight, void* pBufferLeft );
			void Show();
			void Hide();
			void SetCurrentSwapChain( void* pSwapChain );
			void LoadResources();
			void SetSize( LONG Width, LONG Height );
			void Uninitialize();
			HotKeyOSD(void);
			~HotKeyOSD(void);
			void CommonRenderBegin();
		};
	}
}