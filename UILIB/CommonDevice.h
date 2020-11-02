#pragma once

#include <wtypes.h>
#include <list>
#include "CommonFont.h"
struct ICommonSprite;
struct ICommonViewPort;
struct ICommonTexture;
struct ICommonLine;

namespace iz3d
{
	namespace uilib
	{
		class CommonDevice
		{
		public:
			virtual SIZE GetViewportWidthHeight() = 0;
			virtual HRESULT CreateTextureFromFile( LPCTSTR pSrcFile, ICommonTexture** ppTexture ) = 0;
			virtual HRESULT CreateSprite( ICommonSprite** ) = 0;
			virtual HRESULT DeleteSprite( ICommonSprite** ) = 0;
			virtual HRESULT CreateTextureFromResource( HMODULE module, LPCTSTR pSrcResource, ICommonTexture** ppTexture ) = 0;
			virtual HRESULT CreateTextureFromMemory( const void* pData, size_t s, ICommonTexture** ppTexture ) = 0;
			virtual HRESULT CreateFont( const LOGFONT& Font, ICommonFont** ppFont ) = 0;
			virtual HRESULT CreateLine( ICommonLine** ppLine ) = 0;
			virtual HRESULT BeginScene() = 0;
			virtual HRESULT EndScene() = 0;
			virtual HRESULT SetNeededRenderState() = 0;
			virtual HRESULT ReturnRenderState() = 0;
			virtual HRESULT SetRenderTarget( void* pHandle ) = 0;
			virtual HRESULT SetCurrentSwapChain( void* pSwapChain ) = 0;
			virtual void Release() = 0;
		};
	}
}