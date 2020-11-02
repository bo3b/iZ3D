#pragma once

#include "CommonTexture.h"
#include <OCIdl.h>
struct ICommonSprite
{
	virtual HRESULT Begin() = 0;
	virtual HRESULT End() = 0;
	virtual HRESULT Draw( ICommonTexture* pText, RECT* rect ) = 0;
	virtual HRESULT SetTransform( POINTF& scale, POINT& translation ) = 0;
};