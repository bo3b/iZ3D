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
#include "stdafx.h"
#include <windows.h>
#include <stdlib.h>
#include "SetupS3DMonitor.h"

bool __declspec(dllexport) CALLBACK CheckS3DMonitor()
{
	MonitorPanel	first, second;
	MonitorPanel	*pPrimary, *pSecondary;
	pPrimary = &first;
	pSecondary = &second;
	DetectPanels(pPrimary, pSecondary);

	if(pPrimary->bFound && pSecondary->bFound)
		return pPrimary->bAttached != pSecondary->bAttached;
	else
		return false;
}
