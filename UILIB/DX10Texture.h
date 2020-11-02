#pragma once
#include "commontexture.h"
#include "../CommonUtils/SelfDeleter.h"
#include "..\S3DWrapper10\stdafx.h"
#include <d3d10umddi.h>
#include "ICommonDrawer.h"
class DX10Texture : public Deletable< DX10Texture, ICommonTexture >
{
	UINT m_Width, m_Height;
	ICommonDrawer* m_pCommonDrawer;
	void* m_pTextID;
public:
	DX10Texture( ICommonDrawer* pCommonDrawer, UINT Width, UINT Height);
	virtual ~DX10Texture();
	virtual SIZE GetWidthHeight() const;
	HRESULT LoadTextureFromMemory( void* pData,
		size_t dataSize,
		UINT bytesPerPixel,
		UINT Width, UINT Height );
	void Draw( RECT* pTexture, RECT* pRenderTarget );
	void BeginTexture();
	void EndTexture();
};

