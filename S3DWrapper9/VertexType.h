/* IZ3D_FILE: $Id$ 
*
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2009
*
* $Author$
* $Revision$
* $Date$
* $LastChangedBy$
* $URL$
*/
#pragma once
#include <d3d9.h>

#define D3DFVF_D3DTVERTEX_1T  (D3DFVF_XYZRHW|D3DFVF_TEX1|D3DFVF_TEXCOORDSIZE2(0)) 
struct  D3DTVERTEX_1T         { float x,y,z,rhw; float tu,tv; };
#define D3DFVF_D3DTVERTEX_2T  (D3DFVF_XYZRHW|D3DFVF_TEX2|D3DFVF_TEXCOORDSIZE2(0)|D3DFVF_TEXCOORDSIZE2(1)) 
struct  D3DTVERTEX_2T         { float x,y,z,rhw; float tu,tv,tu2,tv2; };

