#pragma once

#include "../CommonUtils/SelfDeleter.h"
#include "CommonSprite.h"
#include <d3dx9core.h>
#include <atlcomcli.h>
class DX9Sprite : public Deletable< DX9Sprite, ICommonSprite >
{
	ID3DXSprite* m_pSprite;
public:
	DX9Sprite( LPD3DXSPRITE pSprite );
	virtual ~DX9Sprite();

	virtual HRESULT Begin();
	virtual HRESULT End();
	virtual HRESULT Draw( ICommonTexture* pText, RECT* rect );
	virtual HRESULT SetTransform( POINTF& scale, POINT& translation );
};
