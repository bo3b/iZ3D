#pragma once

#include "../CommonUtils/SelfDeleter.h"
#include "CommonDevice.h"
#include "ICommonDrawer.h"
#include <map>
struct D3D10DDI_HDEVICE;
struct D3D10DDI_DEVICEFUNCS;
struct D3D10DDI_HRENDERTARGETVIEW;
//class D3DDeviceWrapper;
class DX10Device : public iz3d::uilib::CommonDevice
{
	ICommonDrawer* m_pDrawer;
	typedef std::map< const LOGFONT*, ICommonFont* > TFonts;
	TFonts m_Fonts;
	virtual ~DX10Device();
public:
	DX10Device( ICommonDrawer* pDrawer );

	virtual SIZE GetViewportWidthHeight();
	virtual HRESULT CreateTextureFromFile( LPCTSTR pSrcFile, ICommonTexture** ppTexture );
	virtual HRESULT CreateSprite( ICommonSprite** ppSprite );
	virtual HRESULT DeleteSprite( ICommonSprite** ppSprite );
	virtual HRESULT CreateTextureFromResource( HMODULE module, LPCTSTR pSrcResource, ICommonTexture** ppTexture );
	virtual HRESULT CreateTextureFromMemory( const void* pData, size_t s, ICommonTexture** ppTexture );
	virtual HRESULT CreateFont( const LOGFONT& Font, ICommonFont** ppFont );
	virtual HRESULT CreateLine( ICommonLine** ppLine );
	virtual HRESULT BeginScene();
	virtual HRESULT EndScene();
	virtual HRESULT SetNeededRenderState();
	virtual HRESULT ReturnRenderState();
	virtual HRESULT SetRenderTarget( void* pHandle );
	virtual HRESULT SetCurrentSwapChain( void* pSwapChain );
	virtual void Release();
};
