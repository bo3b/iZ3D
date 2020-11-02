#include "DX9Line.h"
#include "d3dx9math.h"
DX9Line::DX9Line(LPD3DXLINE pLine ):
m_pLine( pLine )
{
}

DX9Line::~DX9Line()
{
	m_pLine->Release();
}

HRESULT DX9Line::Draw( POINTF* pVertexList, DWORD verCount, DWORD Colour )
{
	D3DXVECTOR2* vecs = new D3DXVECTOR2[verCount];
	for( DWORD i = 0; i < verCount; i++ )
	{
		vecs[i].x = pVertexList[i].x;
		vecs[i].y = pVertexList[i].y;
	}
	m_pLine->Draw( vecs, verCount, Colour );
	delete vecs;
	return E_NOTIMPL;
}