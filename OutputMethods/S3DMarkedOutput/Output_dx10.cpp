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

// Output.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "Output_dx10.h"

#include "S3DWrapper10\Utils.h"

#include "S3DWrapper10\Commands\Command.h"
#include "S3DWrapper10\D3DDeviceWrapper.h"

using namespace DX10Output;

#include "Shaders_dx10/shaderFullScreenQuadVS.h"

#include "Shaders_dx10/shaderS3D.h"
#include "Shaders_dx10/shaderS3DAndGamma.h"
#include "Shaders_dx10/shaderS3DCFL.h"
#include "Shaders_dx10/shaderS3DCFLAndGamma.h"
#include "Shaders_dx10/shaderS3DBWCFL.h"
#include "Shaders_dx10/shaderS3DBWCFLAndGamma.h"
#include "Shaders_dx10/shaderS3DFBCFL.h"
#include "Shaders_dx10/shaderS3DFBCFLAndGamma.h"

#include "Shaders_dx10/shaderS3DTable.h"
#include "Shaders_dx10/shaderS3DTableCFL.h"
#include "Shaders_dx10/shaderS3DTableAndGamma.h"
#include "Shaders_dx10/shaderS3DTableCFLAndGamma.h"
#include "Shaders_dx10/shaderS3DTableBWCFL.h"
#include "Shaders_dx10/shaderS3DTableBWCFLAndGamma.h"

OUTPUT_API void* CALLBACK CreateOutputDX10(DWORD mode, DWORD spanMode)
{
	return new S3DMarkedOutput(mode, spanMode);
}


S3DMarkedOutput::S3DMarkedOutput(DWORD mode, DWORD spanMode)
: PostprocessedOutput(mode, spanMode)
{
	 m_bProcessGammaRamp = true;
	//--- select learning method ---
	if(m_OutputMode == 0) // auto
	{
		m_OutputMode = 1; // CFL
	}
	m_SecondBBColor[0] = 0.73f;
	m_SecondBBColor[1] = 0.73f;
	m_SecondBBColor[2] = 0.73f;
	m_SecondBBColor[3] = 1.0f;

	//--- create signature ---
	SigVS.NumInputSignatureEntries  = 1;
	SigVS.NumOutputSignatureEntries = 2;
	SigVS.pInputSignature  = &SigEntryVS[0];
	SigEntryVS[0].SystemValue = D3D10_SB_NAME_VERTEX_ID;
	SigEntryVS[0].Register    = 0;
	SigEntryVS[0].Mask        = 1;
	SigVS.pOutputSignature = &SigEntryVS[SigVS.NumInputSignatureEntries];
	SigEntryVS[1].SystemValue = D3D10_SB_NAME_UNDEFINED;
	SigEntryVS[1].Register    = 0;
	SigEntryVS[1].Mask        = 0x03;
	SigEntryVS[2].SystemValue = D3D10_SB_NAME_POSITION;
	SigEntryVS[2].Register    = 1;
	SigEntryVS[2].Mask        = 0x0F;

	SigPS.NumInputSignatureEntries  = 1;
	SigPS.NumOutputSignatureEntries = 2;
	SigPS.pInputSignature  = &SigEntryPS[0];
	SigEntryPS[0].SystemValue = D3D10_SB_NAME_UNDEFINED;
	SigEntryPS[0].Register    = 0;
	SigEntryPS[0].Mask        = 0x03;
	SigPS.pOutputSignature = &SigEntryPS[SigPS.NumInputSignatureEntries];
	SigEntryPS[1].SystemValue = D3D10_SB_NAME_UNDEFINED;
	SigEntryPS[1].Register    = 0;
	SigEntryPS[1].Mask        = 0x07;
	SigEntryPS[2].SystemValue = D3D10_SB_NAME_UNDEFINED;
	SigEntryPS[2].Register    = 1;
	SigEntryPS[2].Mask        = 0x07;
}

S3DMarkedOutput::~S3DMarkedOutput(void)
{
}

UINT S3DMarkedOutput::GetOutputChainsNumber()
{ 
	if(m_SpanMode != 0)
		return 1; 
	return 2; 
}

void S3DMarkedOutput::InitializeResources(D3DDeviceWrapper* pD3DWrapper)
{	
	D3D10DDI_DEVICEFUNCS& GET_ORIG = pD3DWrapper->OriginalDeviceFuncs;
	D3D10DDI_HDEVICE hDevice = pD3DWrapper->hDevice;
	CONST UINT* pCode;
	::ParseShaderBlob(g_shaderFullScreenQuadVS, sizeof(g_shaderFullScreenQuadVS), &pCode);
	SIZE_T size = GET_ORIG.pfnCalcPrivateShaderSize(hDevice, pCode, &SigVS);
	NEW_HANDLE(m_hVSShader, size);
	D3D10DDI_HRTSHADER hRTShader = { NULL };	
	GET_ORIG.pfnCreateVertexShader(hDevice, pCode, m_hVSShader, hRTShader, &SigVS);

	size = 0;
	pCode = NULL;
	m_bTableMethod = false;
	switch (m_OutputMode)
	{
	case 2: // CF
		ParseShaderBlob(g_shaderS3D, sizeof(g_shaderS3D), &pCode);
		break;
	case 3: // Table CFL
		m_bTableMethod = true;
		ParseShaderBlob(g_shaderS3DTableCFL, sizeof(g_shaderS3DTableCFL), &pCode);
		break;
	case 4: // Table
		m_bTableMethod = true;
		ParseShaderBlob(g_shaderS3DTable, sizeof(g_shaderS3DTable), &pCode);
		break;
	case 5: // BW CFL
		ParseShaderBlob(g_shaderS3DBWCFL, sizeof(g_shaderS3DBWCFL), &pCode);
		break;
	case 6: // BW Table CFL
		m_bTableMethod = true;
		ParseShaderBlob(g_shaderS3DTableBWCFL, sizeof(g_shaderS3DTableBWCFL), &pCode);
		break;
	default:
		ParseShaderBlob(g_shaderS3DCFL, sizeof(g_shaderS3DCFL), &pCode);
		break;
	}
	size = GET_ORIG.pfnCalcPrivateShaderSize(hDevice, pCode, &SigPS);
	NEW_HANDLE(m_hPSShader, size);
	GET_ORIG.pfnCreatePixelShader(hDevice, pCode, m_hPSShader, hRTShader, &SigPS);
}

void S3DMarkedOutput::ClearResources( D3DDeviceWrapper* pD3DWrapper )
{
	D3D10DDI_DEVICEFUNCS& GET_ORIG = pD3DWrapper->OriginalDeviceFuncs;
	D3D10DDI_HDEVICE hDevice = pD3DWrapper->hDevice;

	GET_ORIG.pfnDestroyShader(hDevice, m_hVSShader);
	DELETE_HANDLE(m_hVSShader);
	GET_ORIG.pfnDestroyShader(hDevice, m_hPSShader);
	DELETE_HANDLE(m_hPSShader);
}

bool CheckBackPanel( const wchar_t* szModel_ )
{
	//#ifndef FINAL_RELEASE
	return true;
	//#else
	//		// iZ3D Old 17"
	//	return strcmp(model, "AAA0000") == 0 ||
	//		// iZ3D Old
	//		strcmp(model, "CMO3228") == 0 || strcmp(model, "CMO3229") == 0 ||
	//		// iZ3D New
	//		strcmp(model, "CMO3238") == 0 || strcmp(model, "CMO3237") == 0 ||
	//		// Matrox TripleHead2Go Digital Edition 
	//		strcmp(model, "MTX0510") == 0 || strcmp(model, "MTX0511") == 0;
	//#endif
}

bool S3DMarkedOutput::FindAdapter( IDXGIFactory* pDXGIFactory_ )
{
	IDXGIAdapter*		pDXGIAdapter	= NULL;
	IDXGIOutput*		pDXGIOutput		= NULL;
	DXGI_OUTPUT_DESC	OutputDesc;
	
	wchar_t szModel[ 8 ] = _T("");
	DISPLAY_DEVICEW	MonitorDevice;
	MonitorDevice.cb = sizeof( DISPLAY_DEVICEW );

	for ( UINT nAdapter = 0; pDXGIFactory_->EnumAdapters( nAdapter, &pDXGIAdapter ) != DXGI_ERROR_NOT_FOUND; ++nAdapter )
	{
		if ( pDXGIAdapter->EnumOutputs( 0, &pDXGIOutput ) != DXGI_ERROR_NOT_FOUND )
		{
			pDXGIOutput->GetDesc( &OutputDesc );
			pDXGIOutput->Release();
			
			for( int j = 0 ; EnumDisplayDevicesW( OutputDesc.DeviceName, j, &MonitorDevice, 0x0 ); j++ )
			{
				if( MonitorDevice.StateFlags & DISPLAY_DEVICE_ACTIVE &&
					MonitorDevice.StateFlags & DISPLAY_DEVICE_ATTACHED ) // for ATI, Windows XP
				{
					wchar_t* s = wcschr( MonitorDevice.DeviceID, _T('\\') ) + 1;
					size_t len = wcschr( s, _T('\\')) - s;
					if (len >= _countof(szModel))
						len = _countof(szModel) - 1;

					wcsncpy_s( szModel, s, len );
					
					if (CheckBackPanel(szModel))
					{
						return true;
					}
					
				}
			}
		}
		pDXGIAdapter->Release();
	}

#ifndef FINAL_RELEASE
	return true;
#else
	return false;
#endif
}