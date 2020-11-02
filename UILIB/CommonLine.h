#pragma once

#include <OCIdl.h>
struct ICommonLine
{
	virtual HRESULT Draw( POINTF* pVertexList, DWORD verCount, DWORD Colour ) = 0;
};