#pragma once
#include "D3DSwapChain.h"
#include "../uilib/ICommonDrawer.h"
#include "../CommonUtils/Function.h"

class WizardDrawer : public ICommonDrawer
{
	acGraphics::CDynRender_dx10UM* m_pRender;
	SwapChainResources* m_pCurSwapChain;
	typedef std::vector< acGraphics::CDynTexture* > TTexturesList;
	TTexturesList m_Textures;
	~WizardDrawer();
public:
	WizardDrawer( acGraphics::CDynRender_dx10UM* pRender );
	virtual void PrepareTexture( void* pTextureID );
	virtual void DrawTexture( POINTF* pTextureData, POINTF* pRenderTarget );
	virtual void EndTexture( void* pTextureID );
	virtual void SetLeft( bool isLeft );
	virtual void CreateTextureFromMemory( const void* pData,
		UINT nDataSize_,
		UINT Width, 
		UINT Height, 
		UINT bytesPerPixel, 
		void*& pTextID );
	virtual void SetCurrentSwapChain( void* pSwapChain );
	virtual void Release();
};

