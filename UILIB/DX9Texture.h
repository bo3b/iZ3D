#pragma once
#include "commontexture.h"
#include "..\CommonUtils\SelfDeleter.h"
#include <d3d9.h>
#include <atlcomcli.h>
class DX9Texture :
	public Deletable< DX9Texture, ICommonTexture >
{
	IDirect3DTexture9* m_pTexture;
public:
	DX9Texture( LPDIRECT3DTEXTURE9 pTexture );
	virtual ~DX9Texture();
	virtual SIZE GetWidthHeight() const;
	LPDIRECT3DTEXTURE9 GetTexture9();
};
