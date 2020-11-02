#include "stdafx.h"
#include "resourceloader.h"

#include <IL/il.h>
#include <IL/ilu.h>

ResourceLoader::ResourceLoader()
	:	m_pMem( NULL )
	,	m_Size( 0 )
{
}

bool ResourceLoader::Load( HMODULE hModule, LPCTSTR lpName, LPCTSTR lpType )
{
	HRSRC hRes = FindResource(hModule, lpName, lpType); 
	if ( hRes == NULL )
		return false;

	HGLOBAL hMem = LoadResource(hModule, hRes); 
	m_Size = SizeofResource(hModule, hRes);
	m_pMem = LockResource(hMem);  
	return true;
}

PNGResource::PNGResource( HMODULE hModule_, LPCTSTR lpName_ )
	:	ResourceLoader	()
	,	m_nTexWidth		( 0 )
	,	m_nTexHeight	( 0 )
	,	m_nBitDepth		( 0 )
	,	m_nRowBytes		( 0 )
	,	m_pTexData		( NULL )
{
	if ( !Load( hModule_, lpName_, _T( "PNG" ) ))
		return;

	m_nImageId = ilGenImage();
	ilBindImage( m_nImageId );

	ILboolean bLoadImg = ilLoadL( IL_PNG, GetData(), (ILuint)GetSize() );

	if ( !bLoadImg )
		return;

	m_nTexWidth  = ilGetInteger ( IL_IMAGE_WIDTH  );
	m_nTexHeight = ilGetInteger ( IL_IMAGE_HEIGHT );
	m_nBitDepth  = ilGetInteger ( IL_IMAGE_BITS_PER_PIXEL  );
	m_pTexData   = ilGetData();
	m_nRowBytes  = m_nTexWidth * m_nBitDepth / 8;
}

PNGResource::~PNGResource()
{
	ilDeleteImage( m_nImageId );
}