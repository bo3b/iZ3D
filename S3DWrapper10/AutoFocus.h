/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once
#include "..\S3DAPI\ImageProcessing.h"

//--- must be one object per swapchain ---
class ShiftFinder10 : public ShiftFinder
{
public:
	ShiftFinder10();
	~ShiftFinder10() { Clear(); }
	
	HRESULT Initialize( D3DDeviceWrapper* pD3DDeviceWrapper, RECT* pLeftRect, RECT* pRightRect );
	void	Clear();

	void	FindShift(D3D10DDI_HRESOURCE left, D3D10DDI_HRESOURCE right, UINT currentFrame);

protected:
	HRESULT	CreateMemoryBufferTexture(int width, int height);
	virtual bool	GetRTData();

	D3DDeviceWrapper*	m_pD3DDeviceWrapper;
	D3D10DDI_HRESOURCE	m_hMemoryBuffer;

	//--- view selection boxes's ---
	D3D10_DDI_BOX		m_srcBoxLeft, m_srcBoxRight;
};

inline void InitBoxFromRect(D3D10_DDI_BOX* pBox, RECT* pRect)
{
	pBox->left   = pRect->left;
	pBox->top    = pRect->top;
	pBox->right  = pRect->right;
	pBox->bottom = pRect->bottom;
	pBox->front  = 0;
	pBox->back   = 1;
}