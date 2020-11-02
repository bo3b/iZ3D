#ifndef _RENDER_H_
#define _RENDER_H_

#include "frame.h"

//////////////////////////////////////////////////////////////////////////

class CRender: public CFrame
{
public:
											CRender						();
											~CRender					(); 

	static HRESULT							Init						();
	static void								Release						();
	static HRESULT							DrawFrame					();
protected:
	HRESULT									InternalInit				();
	void									InternalRelease				();
	HRESULT									InternalDrawFrame			();

	HRESULT									InternalInitDevice			();
	HRESULT									InternalInitViewport		();
	HRESULT									InternalCreateRenderTarget	();
	HRESULT									InternalCreateTechnique		();
	HRESULT									InternalCreateMesh			();
private:
	struct SimpleVertex
	{
		D3DXVECTOR3 Pos;
	};
private:
	static CRender *						m_pRender;

	interface	IDXGISwapChain *			m_pSwapChain;
	interface	DXDEVICE *				m_pDevice;
    ID3D11DeviceContext *m_pDevContext;
	interface	DXRTVIEW*		m_pRenderTargetView;

	interface	ID3D10Effect *				m_pSimpleEffect;
	interface	ID3D10EffectTechnique *		m_pSimpleTechnique;
	interface	ID3D10InputLayout *			m_pInputLayout;

    DXTEXTURE2D* pBackBuffer;

};

//////////////////////////////////////////////////////////////////////////

#endif// _RENDER_H_