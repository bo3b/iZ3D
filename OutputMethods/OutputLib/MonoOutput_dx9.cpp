/* IZ3D_FILE: $Id: MonoOutput.cpp 1831 2009-03-18 21:20:45Z dragon $ 
*
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*
* $Author: dragon $
* $Revision: 1831 $
* $Date: 2009-03-19 00:20:45 +0300 (Чт, 19 мар 2009) $
* $LastChangedBy: dragon $
* $URL: svn://svn.neurok.ru/dev/driver/trunk/S3DWrapper9/MonoOutput.cpp $
*/
#include "StdAfx.h"
#include "MonoOutput_dx9.h"

using namespace DX9Output;

MonoOutput::MonoOutput(DWORD mode, DWORD spanMode)
: OutputMethod(mode, spanMode)
{
	m_Caps = ocRenderDirectlyToBB;
}

MonoOutput::~MonoOutput(void)
{
}

HRESULT MonoOutput::Output( CBaseSwapChain* pSwapChain )
{
	HRESULT hResult = S_OK;
	IDirect3DSurface9* left = pSwapChain->GetLeftBackBufferRT();
	RECT* pLeftRect = pSwapChain->GetLeftBackBufferRect();
	IDirect3DSurface9* right = pSwapChain->GetRightBackBufferRT();
	RECT* pRightRect = pSwapChain->GetRightBackBufferRect();
	IDirect3DSurface9* primary = pSwapChain->m_pPrimaryBackBuffer;
	IDirect3DSurface9* secondary = pSwapChain->m_pSecondaryBackBuffer;

	if (m_OutputMode == 0) {
		if (left != primary)
			NSCALL(m_pd3dDevice->StretchRect(left, pLeftRect, primary, NULL, D3DTEXF_NONE));
	} else {
		if (right != primary)
			NSCALL(m_pd3dDevice->StretchRect(right, pRightRect, primary, NULL, D3DTEXF_NONE));
	}
	return hResult;
}
