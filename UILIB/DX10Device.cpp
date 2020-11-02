#include "DX10Device.h"
#include "DX10Sprite.h"
#include "..\S3DWrapper10\stdafx.h"
#include <d3d10umddi.h>
#include <IL/il.h>
#include <IL/ilu.h>
#include "DX10Texture.h"
#include "DX10Font.h"

DX10Device::DX10Device( ICommonDrawer* pDrawer ):
	m_pDrawer( pDrawer )
{
}

DX10Device::~DX10Device()
{
	m_pDrawer->Release();
}

SIZE DX10Device::GetViewportWidthHeight()
{
	// TODO
	return SIZE();
}

HRESULT DX10Device::CreateTextureFromFile( LPCTSTR /*pSrcFile*/, ICommonTexture** ppTexture )
{
	// TODO
	*ppTexture = 0;
	return E_NOTIMPL;
}

HRESULT DX10Device::CreateSprite( ICommonSprite** ppSprite )
{
	*ppSprite = new DX10Sprite( m_pDrawer );
	return E_NOTIMPL;
}

HRESULT DX10Device::CreateTextureFromResource( HMODULE /*module*/, LPCTSTR /*pSrcResource*/, ICommonTexture** ppTexture )
{
	// TODO
	*ppTexture = 0;
	return E_NOTIMPL;
}

HRESULT DX10Device::CreateTextureFromMemory( const void* pData, size_t s, ICommonTexture** ppTexture )
{
	// Process Image
	ilInit();
	ILuint ImageName;
	ilGenImages(1, &ImageName);
	ilBindImage(ImageName);
	ilLoadL(IL_TYPE_UNKNOWN, pData, (ILuint)s);
	if(ilGetInteger(IL_IMAGE_FORMAT) != IL_RGBA || ilGetInteger(IL_IMAGE_BITS_PER_PIXEL) != 32)
	{
		ILboolean bConvert = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
		if ( !bConvert )
		{
			ilDeleteImage( ImageName );
			return E_FAIL;
		}
	}
	int Width = ilGetInteger(IL_IMAGE_WIDTH);
	int Height = ilGetInteger( IL_IMAGE_HEIGHT );
	//int Depth = ilGetInteger( IL_IMAGE_DEPTH );
	auto pSysMemData = ilGetData();
	int dataSize = ilGetInteger( IL_IMAGE_SIZE_OF_DATA );
	int bytePerPixel = ilGetInteger( IL_IMAGE_BYTES_PER_PIXEL );
	DX10Texture* pTextToReturn = new DX10Texture( m_pDrawer, Width, Height );
	pTextToReturn->LoadTextureFromMemory( pSysMemData, dataSize, bytePerPixel, Width, Height );
	ilDeleteImage( ImageName );
	*ppTexture = pTextToReturn;
	return S_OK;
}

HRESULT DX10Device::CreateFont( const LOGFONT& Font, ICommonFont** ppFont )
{
	auto pFf = m_Fonts.find( &Font );
	if( pFf != m_Fonts.end() )
	{// if font already exists
		*ppFont = pFf->second;
		return S_OK;
	}
	iz3d::graphicUI::FontTextureManager::instance().AddFont( &Font );
	iz3d::graphicUI::FontTextureManager::instance().SetCurFont( &Font );
	auto pBinData = iz3d::graphicUI::FontTextureManager::instance().getFontTextureBinaryData();
	//auto binDataSize = iz3d::graphicUI::FontTextureManager::instance().getFontTextureSize();
	auto pTexture = new DX10Texture( m_pDrawer, 1024, 1024 );
	ilInit();
	ilInit();
	ILuint ImageName;
	ilGenImages(1, &ImageName);
	ilBindImage(ImageName);
	ilTexImage( 1024, 1024, 1, 3, IL_RGB, IL_UNSIGNED_BYTE, const_cast<void*>(pBinData) );
	if(ilGetInteger(IL_IMAGE_FORMAT) != IL_RGBA || ilGetInteger(IL_IMAGE_BITS_PER_PIXEL) != 32)
	{
		ILboolean bConvert = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
		if ( !bConvert )
		{
			ilDeleteImage( ImageName );
			return E_FAIL;
		}
	}
	auto prBinData = ilGetData();
	int imageDataSize = ilGetInteger( IL_IMAGE_SIZE_OF_DATA );
	int bytesPerPixel = ilGetInteger( IL_IMAGE_BYTES_PER_PIXEL );
	int Width = ilGetInteger( IL_IMAGE_WIDTH );
	int Height = ilGetInteger( IL_IMAGE_HEIGHT );
	// alpha blending
	for( int i = 3; i < imageDataSize; i +=4 )
	{
		prBinData[i] = prBinData[ i-1 ];
		if( prBinData[i] )
		{
			prBinData[i - 3] = 0xff;
			prBinData[i - 2] = 0xff;
			prBinData[i - 1] = 0xff;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	pTexture->LoadTextureFromMemory( prBinData, imageDataSize, bytesPerPixel, Width, Height );
	auto pFont = new DX10Font( Font, pTexture );
	*ppFont = pFont;
	m_Fonts[&Font] =  pFont;
	ilDeleteImage( ImageName );
	return S_OK;
}

HRESULT DX10Device::CreateLine( ICommonLine** ppLine )
{
	*ppLine = 0;
	return E_NOTIMPL;
}

HRESULT DX10Device::BeginScene()
{
	return E_NOTIMPL;
}

HRESULT DX10Device::EndScene()
{
	// TODO
	return E_NOTIMPL;
}

HRESULT DX10Device::SetNeededRenderState()
{
	// TODO
	return E_NOTIMPL;
}

HRESULT DX10Device::ReturnRenderState()
{
	// TODO
	return E_NOTIMPL;
}

HRESULT DX10Device::SetRenderTarget( void* pHandle )
{
	bool* pIsLeft = ( bool* )pHandle;
	m_pDrawer->SetLeft( *pIsLeft );
	return S_OK;
}

HRESULT DX10Device::SetCurrentSwapChain( void* pSwapChain )
{
	m_pDrawer->SetCurrentSwapChain( pSwapChain );
	return S_OK;
}

void DX10Device::Release()
{
	delete this;
}

HRESULT DX10Device::DeleteSprite( ICommonSprite** ppSprite )
{
	if( !ppSprite )
		return S_OK;
	DX10Sprite* pToDelete = static_cast<DX10Sprite*>( *ppSprite );
	delete pToDelete;
	return S_OK;
}
