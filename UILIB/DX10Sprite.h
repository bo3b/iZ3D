#pragma once

#include "CommonSprite.h"
#include "../CommonUtils/SelfDeleter.h"
#include "..\S3DWrapper10\stdafx.h"
#include <d3d10umddi.h>
#include "ICommonDrawer.h"
struct D3D10DDI_DEVICEFUNCS;
struct D3D10DDIARG_STAGE_IO_SIGNATURES;
struct D3D10DDI_HDEVICE;

class DX10Sprite : public Deletable< DX10Sprite, ICommonSprite >
{
	ICommonDrawer* m_pDrawer;
	POINT* m_pTransform;
	POINTF* m_pScale;
public:
	DX10Sprite( ICommonDrawer* pDrawer );
	virtual ~DX10Sprite(void);
	virtual HRESULT Begin();
	virtual HRESULT End();
	virtual HRESULT Draw( ICommonTexture* pText, RECT* rect );
	virtual HRESULT SetTransform( POINTF& scale, POINT& translation );
};
