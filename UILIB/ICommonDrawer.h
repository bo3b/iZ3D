#pragma once
#include "CommonTexture.h"
#include <ocidl.h>

struct ICommonDrawer
{
	virtual void PrepareTexture( void* pTextureID ) = 0;
	virtual void DrawTexture( POINTF* pTextureData, POINTF* pRenderTarget ) = 0;
	virtual void EndTexture( void* pTextureID ) = 0;
	virtual void SetLeft( bool isLeft ) = 0;
	virtual void CreateTextureFromMemory( const void* pData,
		UINT nDataSize_,
		UINT Width, 
		UINT Height, 
		UINT bytesPerPixel, 
		void*& pTextID ) = 0;
	virtual void SetCurrentSwapChain( void* pSwapChain ) = 0;
	virtual void Release() = 0;
};

