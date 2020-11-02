/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "PostprocessedOutput_dx10.h"

namespace DX10Output
{

class AnaglyphOutput :
	public PostprocessedOutput
{
public:
	AnaglyphOutput(DWORD mode, DWORD spanMode);
	~AnaglyphOutput(void);

	virtual void InitializeResources(D3DDeviceWrapper* pD3DWrapper);
	virtual void ClearResources( D3DDeviceWrapper* pD3DWrapper );

	//--- Signature part ---
	D3D10DDIARG_STAGE_IO_SIGNATURES SigPS;
	D3D10DDIARG_SIGNATURE_ENTRY		SigEntryPS[2];
	D3D10DDIARG_STAGE_IO_SIGNATURES SigVS;
	D3D10DDIARG_SIGNATURE_ENTRY		SigEntryVS[3];
};

}
