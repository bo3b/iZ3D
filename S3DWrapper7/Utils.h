#pragma once

#include "..\CommonUtils\System.h"

#define FOURCC_DF16 MAKEFOURCC( 'D', 'F', '1', '6' )
#define FOURCC_DF24 MAKEFOURCC( 'D', 'F', '2', '4' )
#define FOURCC_DF32 MAKEFOURCC( 'D', 'F', '3', '2' )
#define FOURCC_ATI1 MAKEFOURCC( 'A', 'T', 'I', '1' )
#define FOURCC_ATI2 MAKEFOURCC( 'A', 'T', 'I', '2' )
#define FOURCC_AI44 MAKEFOURCC( 'A', 'I', '4', '4' )
#define FOURCC_IA44 MAKEFOURCC( 'I', 'A', '4', '4' )
#define FOURCC_NULL MAKEFOURCC( 'N', 'U', 'L', 'L' )
#define FOURCC_YV12 MAKEFOURCC( 'Y', 'V', '1', '2' )

UINT BitsPerPixel( D3DFORMAT fmt );

void D3DFromat2DD(DDPIXELFORMAT *DDPixelFormat, D3DFORMAT format);
D3DFORMAT DD2D3DFormat(const DDPIXELFORMAT *DDPixelFormat);

UINT GetPrimitiveCount(D3DPRIMITIVETYPE PrimitiveType, UINT VertexCount);
DWORD GetVertexSize(DWORD FVF);
