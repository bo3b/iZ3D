#pragma once
#include "../uilib/ICommonDrawer.h"
#include <vector>
class DX9OsdDrawer : public ICommonDrawer
{
	CComPtr<IDirect3DDevice9> m_pDevice;
	struct CustomVertex{
		FLOAT x, y, z;
		FLOAT u, v;
	};
	static const unsigned VERTEX_FORMAT = D3DFVF_XYZ | D3DFVF_TEX0;
	std::vector<CustomVertex> m_VerticesToDraw;
	std::vector< LPDIRECT3DTEXTURE9 > m_Textures;
	~DX9OsdDrawer();
	void* m_pSwapChain;
	RECT* m_pLeftRect;
	RECT* m_pRightRect;
	IDirect3DSurface9* m_pLeftBuffer;
	IDirect3DSurface9* m_pRightBuffer;
	RECT* m_pCurRect;
public:
	DX9OsdDrawer( IDirect3DDevice9* pDevice, RECT* pLeftRect, RECT* pRightRect, IDirect3DSurface9* pLeftBuffer, IDirect3DSurface9* pRightBuffer );

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

