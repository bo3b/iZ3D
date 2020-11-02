#pragma once

#include "..\CommonUtils\SelfDeleter.h"
#include "CommonLine.h"
#include <d3dx9core.h>
#include <atlcomcli.h>
class DX9Line : public Deletable< DX9Line, ICommonLine >
{
	ID3DXLine* m_pLine;
public:
	DX9Line( LPD3DXLINE pLine );
	virtual ~DX9Line();

	virtual HRESULT Draw( POINTF* pVertexList, DWORD verCount, DWORD Colour );
};
