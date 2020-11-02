/* IZ3D_FILE: $Id$ 
*
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*
* $Author$
* $Revision$
* $Date$
* $LastChangedBy$
* $URL$
*/
// Direct3DBaseTexture8.cpp : Implementation of CDx8wrpApp and DLL registration.

#include "stdafx.h"
#include "d3d9_8.h"
#include "Direct3DBaseTexture8.h"

/////////////////////////////////////////////////////////////////////////////
//

class CDirect3DBaseTexture8Tester: public CDirect3DBaseTexture8<IDirect3DBaseTexture8, IDirect3DBaseTexture9>
{
    
};
