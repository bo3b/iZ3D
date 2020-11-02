/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "PostprocessedOutput_dx10.h"
#include <vector>

namespace DX10Output
{

class S3DOutput :
	public PostprocessedOutput
{
public:
	S3DOutput(DWORD mode, DWORD spanMode);
	virtual ~S3DOutput(void);

	virtual UINT	GetOutputChainsNumber	();
	virtual void	InitializeResources		( D3DDeviceWrapper* pD3DWrapper );
	virtual void	ClearResources			( D3DDeviceWrapper* pD3DWrapper );
	virtual bool	FindAdapter				( IDXGIFactory* pDXGIFactory_ );

private:
	bool m_bTableMethod;

	//--- Signature part ---
	D3D10DDIARG_STAGE_IO_SIGNATURES SigPS;
	D3D10DDIARG_SIGNATURE_ENTRY		SigEntryPS[3];
	D3D10DDIARG_STAGE_IO_SIGNATURES SigVS;
	D3D10DDIARG_SIGNATURE_ENTRY		SigEntryVS[3];
};

}