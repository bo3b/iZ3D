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
#include <Shlobj.h>
#include <stdio.h>
#include "BaseStereoRenderer.h"
#include "Trace.h"
#include "ProductNames.h"
#include "WideStereoRenderer.h"
#include "..\Shared\version.h"
#include "..\S3DAPI\ReadData.h"
#include "..\Font\acgfx_dynrender_dx9.h"
#include "..\Font\acgfx_font.h"
#include "..\CommonUtils\StringUtils.h"
#include "..\CommonUtils\CommonResourceFolders.h"
#include "CommandDumper.h"

#if !defined(WIN64) && !defined(FINAL_RELESE)

#include <NVPerfSDK.h>
#include "NVTraceDisplay/tracedisplayd3d.h"
#include "NVTraceDisplay/trace.h"


struct  TTraceInfo
{
	const char*				szParamName;
	::CTrace< float >*		pTrace;
	float					fMin;
	float					fMax;
	CD3DTraceDisplay*		pDisplay;
	CTraceDisplay::DISPLAY	tDrawType;	

	~TTraceInfo()
	{
		if ( pDisplay )
		{
			delete pDisplay;
			pDisplay = NULL;
		}

		if ( pTrace )
		{
			pTrace->clear();
			delete pTrace;
			pTrace = NULL;
		}
	}
};

TTraceInfo				g_pD3DTracePage[] =
{
	// LEFT
	{ "D3D FPS",								new ::CTrace< float >(), 0.0f, 120.0f, new CD3DTraceDisplay( 0.05f, 0.0125f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },
	{ "D3D frame time",							new ::CTrace< float >(), 0.0f, 100.0f, new CD3DTraceDisplay( 0.05f, 0.1375f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },
	{ "D3D time in driver",						new ::CTrace< float >(), 0.0f, 100.0f, new CD3DTraceDisplay( 0.05f, 0.2625f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },
	{ "D3D driver sleeping",					new ::CTrace< float >(), 0.0f, 100.0f, new CD3DTraceDisplay( 0.05f, 0.3875f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },
	{ "D3D triangle count",						new ::CTrace< float >(), 0.0f, 100.0f, new CD3DTraceDisplay( 0.05f, 0.5125f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },
	{ "D3D triangle count instanced",			new ::CTrace< float >(), 0.0f, 100.0f, new CD3DTraceDisplay( 0.05f, 0.6375f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },
	{ "D3D batch count",						new ::CTrace< float >(), 0.0f, 100.0f, new CD3DTraceDisplay( 0.05f, 0.7625f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },
	{ "D3D Locked Render Targets",				new ::CTrace< float >(), 0.0f, 100.0f, new CD3DTraceDisplay( 0.05f, 0.8875f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },

	// RUGHT
	{ "D3D agpmem MB",							new ::CTrace< float >(), 0.0f, 120.0f, new CD3DTraceDisplay( 0.55f, 0.0125f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },
	{ "D3D vidmem MB",							new ::CTrace< float >(), 0.0f, 100.0f, new CD3DTraceDisplay( 0.55f, 0.1375f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },
	{ "D3D vidmem total MB",					new ::CTrace< float >(), 0.0f, 100.0f, new CD3DTraceDisplay( 0.55f, 0.2625f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },
	{ "D3D SLI P2P transfers",					new ::CTrace< float >(), 0.0f, 100.0f, new CD3DTraceDisplay( 0.55f, 0.3875f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },
	{ "D3D SLI P2P Bytes",						new ::CTrace< float >(), 0.0f, 100.0f, new CD3DTraceDisplay( 0.55f, 0.5125f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },
	{ "D3D SLI Linear Buffer Syncs",			new ::CTrace< float >(), 0.0f, 100.0f, new CD3DTraceDisplay( 0.55f, 0.6375f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },
	{ "D3D SLI Render Target Sync",				new ::CTrace< float >(), 0.0f, 100.0f, new CD3DTraceDisplay( 0.55f, 0.7625f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },
	{ "D3D SLI Texture Syncs",					new ::CTrace< float >(), 0.0f, 100.0f, new CD3DTraceDisplay( 0.55f, 0.8875f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },
};

TTraceInfo				g_pGPUTracePage1[] =
{
	// LEFT
	{ "gpu_idle",								new ::CTrace< float >(), 0.0f, 120.0f, new CD3DTraceDisplay( 0.05f, 0.0125f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },
	{ "gpu_busy",								new ::CTrace< float >(), 0.0f, 100.0f, new CD3DTraceDisplay( 0.05f, 0.1375f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },
	{ "input_assembler_busy",					new ::CTrace< float >(), 0.0f, 100.0f, new CD3DTraceDisplay( 0.05f, 0.2625f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },
	{ "input_assembler_waits_for_fb",			new ::CTrace< float >(), 0.0f, 100.0f, new CD3DTraceDisplay( 0.05f, 0.3875f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },
	{ "vertex_attribute_count",					new ::CTrace< float >(), 0.0f, 100.0f, new CD3DTraceDisplay( 0.05f, 0.5125f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },
	{ "geom_busy",								new ::CTrace< float >(), 0.0f, 100.0f, new CD3DTraceDisplay( 0.05f, 0.6375f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },
	{ "geom_waits_for_shader",					new ::CTrace< float >(), 0.0f, 100.0f, new CD3DTraceDisplay( 0.05f, 0.7625f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },
	{ "geom_vertex_in_count",					new ::CTrace< float >(), 0.0f, 100.0f, new CD3DTraceDisplay( 0.05f, 0.8875f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },

	// RUGHT
	{ "geom_primitive_in_count",				new ::CTrace< float >(), 0.0f, 120.0f, new CD3DTraceDisplay( 0.55f, 0.0125f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },
	{ "vertex_shader_busy",						new ::CTrace< float >(), 0.0f, 100.0f, new CD3DTraceDisplay( 0.55f, 0.1375f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },
	{ "vertex_shader_instruction_rate",			new ::CTrace< float >(), 0.0f, 100.0f, new CD3DTraceDisplay( 0.55f, 0.2625f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },
	{ "geometry_shader_shader_busy",			new ::CTrace< float >(), 0.0f, 100.0f, new CD3DTraceDisplay( 0.55f, 0.3875f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },
	{ "geometry_shader_instruction_rate",		new ::CTrace< float >(), 0.0f, 100.0f, new CD3DTraceDisplay( 0.55f, 0.5125f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },
	{ "geom_vertex_out_count",					new ::CTrace< float >(), 0.0f, 100.0f, new CD3DTraceDisplay( 0.55f, 0.6375f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },
	{ "geom_primitive_out_count",				new ::CTrace< float >(), 0.0f, 100.0f, new CD3DTraceDisplay( 0.55f, 0.7625f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },
	{ "stream_out_busy",						new ::CTrace< float >(), 0.0f, 100.0f, new CD3DTraceDisplay( 0.55f, 0.8875f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },
};

TTraceInfo				g_pGPUTracePage2[] =
{
	// LEFT
	{ "setup_primitive_culled_count",			new ::CTrace< float >(), 0.0f, 120.0f, new CD3DTraceDisplay( 0.05f, 0.0125f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },
	{ "setup_primitive_count",					new ::CTrace< float >(), 0.0f, 100.0f, new CD3DTraceDisplay( 0.05f, 0.1375f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },
	{ "setup_triangle_count",					new ::CTrace< float >(), 0.0f, 100.0f, new CD3DTraceDisplay( 0.05f, 0.2625f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },
	{ "setup_point_count",						new ::CTrace< float >(), 0.0f, 100.0f, new CD3DTraceDisplay( 0.05f, 0.3875f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },
	{ "setup_line_count",						new ::CTrace< float >(), 0.0f, 100.0f, new CD3DTraceDisplay( 0.05f, 0.5125f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },
	{ "shader_pixel_count",						new ::CTrace< float >(), 0.0f, 100.0f, new CD3DTraceDisplay( 0.05f, 0.6375f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },
	{ "rasterizer_pixels_killed_zcull_count",	new ::CTrace< float >(), 0.0f, 100.0f, new CD3DTraceDisplay( 0.05f, 0.7625f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },
	{ "pixel_shader_busy",						new ::CTrace< float >(), 0.0f, 100.0f, new CD3DTraceDisplay( 0.05f, 0.8875f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },

	// RUGHT
	{ "pixel_shader_instruction_rate",			new ::CTrace< float >(), 0.0f, 120.0f, new CD3DTraceDisplay( 0.55f, 0.0125f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },
	{ "shader_busy",							new ::CTrace< float >(), 0.0f, 100.0f, new CD3DTraceDisplay( 0.55f, 0.1375f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },
	{ "shader_waits_for_texture",				new ::CTrace< float >(), 0.0f, 100.0f, new CD3DTraceDisplay( 0.55f, 0.2625f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },
	{ "shader_waits_for_geom",					new ::CTrace< float >(), 0.0f, 100.0f, new CD3DTraceDisplay( 0.55f, 0.3875f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },
	{ "shader_waits_for_rop",					new ::CTrace< float >(), 0.0f, 100.0f, new CD3DTraceDisplay( 0.55f, 0.5125f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },
	{ "texture_busy",							new ::CTrace< float >(), 0.0f, 100.0f, new CD3DTraceDisplay( 0.55f, 0.6375f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },
	{ "texture_waits_for_fb",					new ::CTrace< float >(), 0.0f, 100.0f, new CD3DTraceDisplay( 0.55f, 0.7625f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },
	{ "texture_waits_for_shader",				new ::CTrace< float >(), 0.0f, 100.0f, new CD3DTraceDisplay( 0.55f, 0.8875f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },
};


TTraceInfo				g_pGPUTracePage3[] =
{
	// LEFT
	{ "texture_sample_base_level_rate",			new ::CTrace< float >(), 0.0f, 120.0f, new CD3DTraceDisplay( 0.05f, 0.0125f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },
	{ "texture_sample_average_level",			new ::CTrace< float >(), 0.0f, 100.0f, new CD3DTraceDisplay( 0.05f, 0.1375f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },
	{ "rop_busy",								new ::CTrace< float >(), 0.0f, 100.0f, new CD3DTraceDisplay( 0.05f, 0.2625f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },
	{ "rop_waits_for_fb",						new ::CTrace< float >(), 0.0f, 100.0f, new CD3DTraceDisplay( 0.05f, 0.3875f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },
	{ "rop_waits_for_shader",					new ::CTrace< float >(), 0.0f, 100.0f, new CD3DTraceDisplay( 0.05f, 0.5125f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },
	{ "rop_pixels_killed_eartyz_count",			new ::CTrace< float >(), 0.0f, 100.0f, new CD3DTraceDisplay( 0.05f, 0.6375f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },
	{ "rop_pixels_killed_latez_count",			new ::CTrace< float >(), 0.0f, 100.0f, new CD3DTraceDisplay( 0.05f, 0.7625f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },
	//	{ "pixel_shader_busy",						new ::CTrace< float >(), 0.0f, 100.0f, new CD3DTraceDisplay( 0.05f, 0.8875f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },

	// RUGHT
	//	{ "pixel_shader_instruction_rate",			new ::CTrace< float >(), 0.0f, 120.0f, new CD3DTraceDisplay( 0.55f, 0.0125f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },
	//	{ "shader_busy",							new ::CTrace< float >(), 0.0f, 100.0f, new CD3DTraceDisplay( 0.55f, 0.1375f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },
	//	{ "shader_waits_for_texture",				new ::CTrace< float >(), 0.0f, 100.0f, new CD3DTraceDisplay( 0.55f, 0.2625f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },
	//	{ "shader_waits_for_geom",					new ::CTrace< float >(), 0.0f, 100.0f, new CD3DTraceDisplay( 0.55f, 0.3875f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },
	//	{ "shader_waits_for_rop",					new ::CTrace< float >(), 0.0f, 100.0f, new CD3DTraceDisplay( 0.55f, 0.5125f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },
	//	{ "texture_busy",							new ::CTrace< float >(), 0.0f, 100.0f, new CD3DTraceDisplay( 0.55f, 0.6375f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },
	//	{ "texture_waits_for_fb",					new ::CTrace< float >(), 0.0f, 100.0f, new CD3DTraceDisplay( 0.55f, 0.7625f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },
	//	{ "texture_waits_for_shader",				new ::CTrace< float >(), 0.0f, 100.0f, new CD3DTraceDisplay( 0.55f, 0.8875f, 0.4f, 0.1f ),	CD3DTraceDisplay::LINE_STREAM },
};

TTraceInfo*	GetTracePage( int nID, int& nNumElements )
{
	nID -= 1;
	switch ( nID )
	{
	case 0:
		nNumElements = _countof( g_pD3DTracePage );
		return g_pD3DTracePage;
	case 1:
		nNumElements = _countof( g_pGPUTracePage1 );
		return g_pGPUTracePage1;
	case 2:
		nNumElements = _countof( g_pGPUTracePage2 );
		return g_pGPUTracePage2;
	case 3:
		nNumElements = _countof( g_pGPUTracePage3 );
		return g_pGPUTracePage3;
	}
	nNumElements = -1;
	_ASSERT( false );
	return NULL;
}

DWORD GetTracePageCount()
{
	return 4;
}

#endif

#if !defined(WIN64) && !defined(FINAL_RELEASE)
int MyEnumFunc(UINT unCounterIndex, char *pcCounterName)
{
	char zString[200], zLine[400];
	unsigned int unLen;
	float fValue;

	unLen = 200;
	if(NVPMGetCounterDescription(unCounterIndex, zString, &unLen) != NVPM_OK)
		return(NVPM_OK);

	sprintf(zLine, "Counter %u [%s] : ", unCounterIndex, zString);

	unLen = 200;
	if(NVPMGetCounterName(unCounterIndex, zString, &unLen) == NVPM_OK)
		strcat(zLine, zString); // Append the short name
	else
		strcat(zLine, "Error retrieving short name");

	NVPMGetCounterClockRate(zString, &fValue);
	sprintf(zString, " %.2f\n", fValue);
	strcat(zLine, zString);

	//OutputDebugStringA(zLine);
	return(NVPM_OK);
}
#endif

void CBaseStereoRenderer::InitPerfHUD()
{
#if !defined(WIN64) && !defined(FINAL_RELEASE)

	NVPMRESULT hr = NVPMInit(); //NVPM_LIBRARY_NOT_FOUND;
	if (hr == NVPM_FAILURE_DISABLED)	{
		DEBUG_MESSAGE(_T("NVPerfKit: Instrumental driver disabled\n"));
	} else if (hr == NVPM_LIBRARY_NOT_FOUND)	{
		DEBUG_MESSAGE(_T("NVPerfKit: Not installed\n"));
	}

	if ( hr != NVPM_OK )
		return;

	int nNumPages = GetTracePageCount();
	for ( int i = 1; i <= nNumPages; i++ )
	{
		int nNumCounters = 0;
		TTraceInfo* pTracePage = GetTracePage( i, nNumCounters );

		for(int nIndex = 0; nIndex < nNumCounters; nIndex++)
		{
			TTraceInfo& tInfo = pTracePage[ nIndex ];
			NVPMAddCounterByName( ( char* )tInfo.szParamName );
			tInfo.pTrace->name( tInfo.szParamName );
			tInfo.pDisplay->SetDevice( m_Direct3DDevice, NULL );
			tInfo.pDisplay->DrawText( false );
			tInfo.pDisplay->BackgroundColor( 0.0f, 0.0f, 0.0f, 0.4f );
			tInfo.pTrace->resize();
			tInfo.pTrace->minVal( tInfo.fMin );
			tInfo.pTrace->maxVal( tInfo.fMax );
			tInfo.pDisplay->Insert( tInfo.pTrace, 1.0f, 1.0f, 0.0f );		
		}
	}	

	m_bNVPerfHUDInitOk = true;

#endif
}

void CBaseStereoRenderer::ReleasePerfHUD()
{
#if !defined(WIN64) && !defined(FINAL_RELEASE)
	NVPMShutdown();
#endif
}

void CBaseStereoRenderer::DrawPerfHUDStatistics( CBaseSwapChain* pSwapChain )
{
	INSIDE;
#if !defined(WIN64) && !defined(FINAL_RELEASE)

	if ( !m_bNVPerfHUDInitOk )
		return;

	static float fLastTime = 0.0f;

	float fTime = 1000.0f * GetBaseSC()->m_nFrameTimeDelta.QuadPart / m_nFreq.QuadPart;

	if(fTime != fLastTime)
	{
		UINT64 nValue = 0;
		UINT64 nCycle = 0;
		unsigned int nCount = 0;
		NVPMSample(NULL, &nCount);

		int nNumCounters = 0;
		TTraceInfo* pTracePage = GetTracePage( m_Input.DebugInfoPage, nNumCounters );		

		for(int nIndex = 0; nIndex < nNumCounters; nIndex++)
		{
			const TTraceInfo& tInfo = pTracePage[ nIndex ];

			float x = tInfo.pDisplay->GetLeft() * pSwapChain->m_BackBufferSize.cx;
			float y = tInfo.pDisplay->GetBottom() * pSwapChain->m_BackBufferSize.cy - m_pFont->GetHeight();

			m_pFont->Draw( x , y, 1.0f, tInfo.szParamName, 0, 0 );
		}

		HRESULT hResult = S_OK;
		NSCALL(m_pTextState->Apply());

		for(int nIndex = 0; nIndex < nNumCounters; nIndex++)
		{
			const TTraceInfo& tInfo = pTracePage[ nIndex ];
			NVPMGetCounterValueByName( ( char* )tInfo.szParamName, 0, &nValue, &nCycle );
			tInfo.pTrace->insert( ( float )nValue );
			tInfo.pDisplay->Display( tInfo.tDrawType );
		}
		fLastTime = fTime;
	}
#endif
}

void CBaseStereoRenderer::DrawGraph(CBaseSwapChain* pSwapChain)
{
	INSIDE;
	CPresentHistory* pHistory = NULL;
	if (GetBaseDevice()->m_pPresenter)
		pHistory = GetBaseDevice()->m_pPresenter->m_pPresentHistory;

	if (!pHistory ||
		pSwapChain->m_BackBufferSize.cx < 640 ||
		pSwapChain->m_BackBufferSize.cy < 480)
		return;

	if (!m_pLine)
	{
		D3DXCreateLine(m_Direct3DDevice, &m_pLine);
		//m_pLine->SetWidth(2.2);        // Width 
		//m_pLine->SetAntialias(1);
		m_pLine->SetWidth(2.5);          // Width 
		m_pLine->SetAntialias(1);
		//m_pLine->SetGLLines(1);
	}

	if (!m_pLine)
		return;

	D3DXVECTOR2	Points[120];

	int StartX = 0;
	int StartY = 0;
	int ScaleX = 1;
	int ScaleY = 1;
	int DrawWidth = 625 * ScaleX + 50;
	int GraphCount = 6;
	int DrawHeight = (GraphCount * 80 + 20) * ScaleY;
	int Alpha = 80;
	StartX = pSwapChain->m_BackBufferSize.cx - (DrawWidth + 20);
	StartY = pSwapChain->m_BackBufferSize.cy - (DrawHeight + 20);

	if (pHistory)
	{
		m_pFont->Draw(StartX + 5.0f, StartY + (DrawHeight-450.0f-5)*ScaleY, 1.0f, _T("Flush() count"), 0, 0);
		m_pFont->Draw(StartX + 5.0f, StartY + (DrawHeight-370.0f-5)*ScaleY, 1.0f, _T("Sleep() Time"), 0, 0);
		m_pFont->Draw(StartX + 5.0f, StartY + (DrawHeight-290.0f-5)*ScaleY, 1.0f, _T("Op's Time"), 0, 0);
		m_pFont->Draw(StartX + 5.0f, StartY + (DrawHeight-210.0f-5)*ScaleY, 1.0f, _T("Present() Time"), 0, 0);
		m_pFont->Draw(StartX + 5.0f, StartY + (DrawHeight-140.0f-5)*ScaleY, 1.0f, _T("Frame Time (rel. VSync Time)"), 0, 0);
	}
	m_pFont->Draw(StartX + 5.0f, StartY + (DrawHeight-50.0f-5)*ScaleY, 1.0f, _T("Lag count"), 0, 0);

	//DrawRect(RGB(0,0,0), Alpha, CRect(StartX, StartY, StartX + DrawWidth, StartY + DrawHeight));
	m_pLine->Begin();

	for (int i=10; i<DrawHeight; i+= 20*ScaleY)
	{
		Points[0].x = (FLOAT)StartX;
		Points[0].y = (FLOAT)(StartY + i);
		int offsetX = ((i-10) % 80) ? 50 : (625 * ScaleX);
		Points[1].x = (FLOAT)(StartX + offsetX);
		Points[1].y = (FLOAT)(StartY + i);
		if (i == 250) Points[1].x += 50;
		m_pLine->Draw (Points, 2, D3DCOLOR_XRGB(100,100,255));
	}

	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);

	int	nCount = 120;
	if (pHistory)
	{
		double expectedFrameTime = 1000.0 / pHistory->GetRefreshRate();
		int nOffset = nCount > 120 ? (int)pHistory->GetBufferSize() - 120 : 0;
		nCount = std::min(120, (int)pHistory->GetBufferSize());

		// Flush() count
		for (int i=0; i<nCount; i++)
		{
			PresentData* presentData = pHistory->GetItem(i + nOffset);
			int count = presentData->flushCount;
			Points[i].x  = (FLOAT)(StartX + (i*5*ScaleX+5));
			Points[i].y  = (FLOAT)(StartY + (-count*ScaleY + (DrawHeight-410)*ScaleY));
		}		
		m_pLine->Draw (Points, nCount, D3DCOLOR_XRGB(128,255,128));

		// Sleep() Time
		for (int i=0; i<nCount; i++)
		{
			PresentData* presentData = pHistory->GetItem(i + nOffset);
			LONGLONG time = presentData->afterSleep.time.QuadPart;
			double frameTime = 1000.0 * time / freq.QuadPart / expectedFrameTime;
			Points[i].x  = (FLOAT)(StartX + (i*5*ScaleX+5));
			Points[i].y  = (FLOAT)(StartY + (-frameTime*ScaleY*40 + (DrawHeight-290)*ScaleY));
		}		
		m_pLine->Draw (Points, nCount, D3DCOLOR_XRGB(255,128,128));

		// Op's Time
		for (int i=0; i<nCount; i++)
		{
			PresentData* presentData = pHistory->GetItem(i + nOffset);
			LONGLONG time = presentData->beforePresent.time.QuadPart - presentData->afterSleep.time.QuadPart;
			double frameTime = 1000.0 * time / freq.QuadPart / expectedFrameTime;
			Points[i].x  = (FLOAT)(StartX + (i*5*ScaleX+5));
			Points[i].y  = (FLOAT)(StartY + (-frameTime*ScaleY*40 + (DrawHeight-250)*ScaleY));
		}		
		m_pLine->Draw (Points, nCount, D3DCOLOR_XRGB(10,255,255));

		// Present() Time
		for (int i=0; i<nCount; i++)
		{
			PresentData* presentData = pHistory->GetItem(i + nOffset);
			LONGLONG time = presentData->afterPresent.time.QuadPart - presentData->beforePresent.time.QuadPart;
			double frameTime = 1000.0 * time / freq.QuadPart / expectedFrameTime;
			Points[i].x  = (FLOAT)(StartX + (i*5*ScaleX+5));
			Points[i].y  = (FLOAT)(StartY + (-frameTime*ScaleY*40 + (DrawHeight-170)*ScaleY));
		}		
		m_pLine->Draw (Points, nCount, D3DCOLOR_XRGB(10,10,255));

		// Frame Time (rel. VSync Time)
		for (int i=0; i<nCount; i++)
		{
			PresentData* presentData = pHistory->GetItem(i + nOffset);
			LONGLONG time = presentData->afterPresent.time.QuadPart;
			double frameTime = 1000.0 * time / freq.QuadPart / expectedFrameTime - 1;
			Points[i].x  = (FLOAT)(StartX + (i*5*ScaleX+5));
			Points[i].y  = (FLOAT)(StartY + (-frameTime*ScaleY*40 + (DrawHeight-90)*ScaleY));
		}		
		m_pLine->Draw (Points, nCount, D3DCOLOR_XRGB(255,100,100));
	}

	CAverage* fpsData = NULL;
	if (pSwapChain->m_pBasePresenterData)
		fpsData = &pSwapChain->m_pBasePresenterData->m_AverageFPS;
	if (fpsData)
	{
		// Lag count
		int nfpsCount = (int)fpsData->GetBufferSize();
		DXGI_RATIONAL refreshRate = fpsData->GetRefreshRate();
		float vblankTime = 1000.0f * refreshRate.Denominator / refreshRate.Numerator;

		for (int i=0; i<nfpsCount; i++)
		{
			float time = fpsData->GetFrameTime(i);
			int count2 = (int)(2 * (time / vblankTime - 1) + 0.5f);
			double frameCount = count2/2.0f - 0.5f;
			Points[i].x  = (FLOAT)(StartX + ((i + nCount - nfpsCount)*5*ScaleX+5));
			Points[i].y  = (FLOAT)(StartY + (-frameCount*ScaleY*40 + (DrawHeight-10)*ScaleY));
		}		
		m_pLine->Draw(Points, nfpsCount, D3DCOLOR_XRGB(100,200,100));
	}

	m_pLine->End();
}

int getModAndRem(float f, float t, float &rem)
{
	float mod;
	rem = modff(f / t, &mod) * t;
	return (int)mod;
}

void CBaseStereoRenderer::WriteStatistic()
{
	TCHAR szPath[MAX_PATH];
	UpdateStatisticData();
	if (iz3d::resources::GetAllUsersDirectory( szPath ))
	{
		PathAppend(szPath, _T("Statistic.xml") );

		TiXmlDocument doc( common::utils::to_multibyte(szPath) );
		if (!doc.LoadFile())
		{
			TiXmlDeclaration dec;
			dec.Parse( "<?xml version='1.0' encoding='UTF-8'?>", 0, TIXML_ENCODING_UNKNOWN );
			doc.InsertEndChild(dec);
			TiXmlElement node = TiXmlElement("Statistic");
			node.SetAttribute("Version", "1");
			doc.InsertEndChild(node);
		}
		else
		{
			int nVersion = 0;
			TiXmlNode* rootNode = 0;
			TiXmlElement* itemElement = 0;
			rootNode = doc.FirstChild( "Statistic" );
			itemElement = rootNode->ToElement();
			if (itemElement->QueryIntAttribute("Version", &nVersion) != TIXML_SUCCESS)
			{
				DEBUG_MESSAGE(_T("Error reading Version = %d; in %s\n"), PROFILES_VERSION, szPath);
				return;
			}
			if (PROFILES_VERSION < nVersion)
			{
				DEBUG_MESSAGE(_T("Unsupported Version = %d in %s, required Version = %d\n"), 
					nVersion, szPath, PROFILES_VERSION);
				return;
			}
			DEBUG_MESSAGE(_T("Statistic version: %d\n"), nVersion); 
			std::basic_string<TCHAR> AppCmdLine = gInfo.CmdLine;	
			std::transform(AppCmdLine.begin(), AppCmdLine.end(), AppCmdLine.begin(), tolower);	
			TiXmlNode* node = 0;
			TCHAR szApplicationFileName[MAX_PATH];
			_tcscpy(szApplicationFileName, gInfo.ApplicationFileName);
			_tcslwr_s(szApplicationFileName, MAX_PATH);
			for( node = rootNode->FirstChild("Profile"); node;	node = node->NextSibling("Profile") )
			{
				for( TiXmlElement* fileElem = node->FirstChildElement("File");
					fileElem;
					fileElem = fileElem->NextSiblingElement("File") )
				{
					std::wstring Name = common::utils::to_unicode_simple(
						std::string(fileElem->Attribute("Name")));		
					std::transform(Name.begin(), Name.end(), Name.begin(), tolower);
					if (_tcsstr(szApplicationFileName, Name.c_str()) != 0)
					{
						const char* cmdLine = fileElem->Attribute("CmdLine");
						if (cmdLine != NULL)
						{
							std::string CmdLine = cmdLine;
							std::transform(CmdLine.begin(), CmdLine.end(), CmdLine.begin(), tolower);
							std::wstring CmdLineW = common::utils::to_unicode(CmdLine);
							if (_tcsstr(AppCmdLine.c_str(), CmdLineW.c_str()) != 0)
							{
								if (_tcscmp(gInfo.Params, CmdLineW.c_str()) == 0)
								{
									rootNode->RemoveChild(node);
									goto exit;
								}
							}
						}
						else if (_tcscmp(gInfo.Params, L"") == 0)
						{
							TiXmlElement* profileNode = node->FirstChildElement("Sessions");
							if (profileNode)
							{
								float fTotalMonoTime = 0.0f;
								float fTotalStereoTime = 0.0f;
								int nMonoSessionsCounter = 0;
								int nStereoSessionsCounter = 0;
								float fMaxMonoTime = 0.0f;
								float fMaxStereoTime = 0.0f;
								TiXmlElement* itemElement = profileNode->FirstChildElement("Count");
								itemElement->QueryIntAttribute("Mono", &nMonoSessionsCounter);
								itemElement->QueryIntAttribute("Stereo", &nStereoSessionsCounter);
								itemElement = profileNode->FirstChildElement("TotalTime");
								itemElement->QueryFloatAttribute("Mono", &fTotalMonoTime);
								itemElement->QueryFloatAttribute("Stereo", &fTotalStereoTime);
								itemElement = profileNode->FirstChildElement("MaxTime");
								itemElement->QueryFloatAttribute("Mono", &fMaxMonoTime);
								itemElement->QueryFloatAttribute("Stereo", &fMaxStereoTime);
								m_nMonoSessionsCounter += nMonoSessionsCounter;
								m_nStereoSessionsCounter += nStereoSessionsCounter;
								m_fTotalMonoTime += fTotalMonoTime;
								m_fTotalStereoTime += fTotalStereoTime;
								m_fMaxMonoTime = std::max(m_fMaxMonoTime, fMaxMonoTime);
								m_fMaxStereoTime = std::max(m_fMaxStereoTime, fMaxStereoTime);
							}

							rootNode->RemoveChild(node);
							goto exit;
						}
					}
				}
			}
		}

exit:	TiXmlNode* profileNode = doc.FirstChild( "Statistic" );
		TiXmlElement item = TiXmlElement("Profile");
		item.SetAttribute("Name", common::utils::to_multibyte(gInfo.ProfileName).c_str());
		TiXmlElement fileItem = TiXmlElement("File");
		fileItem.SetAttribute("Name",
			common::utils::to_multibyte(_tcsrchr(gInfo.ApplicationFileName, '\\')).c_str() + 1);
		if (_tcscmp(gInfo.Params, L"") != 0)
			fileItem.SetAttribute("CmdLine", common::utils::to_multibyte(gInfo.Params).c_str());
		item.InsertEndChild(fileItem);
		profileNode = profileNode->InsertEndChild(item);

		TiXmlNode* node;
		TiXmlElement* itemElement;
		itemElement = profileNode->InsertEndChild(TiXmlElement("DriverVersion"))->ToElement();
		itemElement->SetAttribute("Value", VERSION);
		itemElement = profileNode->InsertEndChild(TiXmlElement("CmdLine"))->ToElement();
		itemElement->SetAttribute("Value", common::utils::to_multibyte(gInfo.CmdLine).c_str());

		node = profileNode->InsertEndChild(TiXmlElement("Counters"));
		itemElement = node->InsertEndChild(TiXmlElement("RenderTargetSurface"))->ToElement();
		itemElement->SetAttribute("Count", m_nRenderTargetCounter);
		itemElement = node->InsertEndChild(TiXmlElement("DepthStencilSurface"))->ToElement();
		itemElement->SetAttribute("Count", m_nDepthStencilSurfaceCounter);
		itemElement = node->InsertEndChild(TiXmlElement("Texture"))->ToElement();
		itemElement->SetAttribute("Count", m_nTextureCounter);
		itemElement = node->InsertEndChild(TiXmlElement("TextureDefault"))->ToElement();
		itemElement->SetAttribute("Count", m_nTextureDefaultCounter);
		itemElement = node->InsertEndChild(TiXmlElement("RenderTargetTexture"))->ToElement();
		itemElement->SetAttribute("Count", m_nRenderTargetTextureCounter);
		itemElement = node->InsertEndChild(TiXmlElement("RenderTargetR32FTexture"))->ToElement();
		itemElement->SetAttribute("Count", m_nRenderTargetR32FTextureCounter);
		itemElement = node->InsertEndChild(TiXmlElement("RenderTargetSquareTexture"))->ToElement();
		itemElement->SetAttribute("Count", m_nRenderTargetSquareTextureCounter);
		itemElement = node->InsertEndChild(TiXmlElement("RenderTargetLessThanBBTexture"))->ToElement();
		itemElement->SetAttribute("Count", m_nRenderTargetLessThanBBTextureCounter);
		itemElement = node->InsertEndChild(TiXmlElement("DepthStencilTexture"))->ToElement();
		itemElement->SetAttribute("Count", m_nDepthStencilTextureCounter);
		itemElement = node->InsertEndChild(TiXmlElement("CubeTexture"))->ToElement();
		itemElement->SetAttribute("Count", m_nCubeTextureCounter);
		itemElement = node->InsertEndChild(TiXmlElement("CubeTextureDefault"))->ToElement();
		itemElement->SetAttribute("Count", m_nCubeTextureDefaultCounter);
		itemElement = node->InsertEndChild(TiXmlElement("RenderTargetCubeTexture"))->ToElement();
		itemElement->SetAttribute("Count", m_nRenderTargetCubeTextureCounter);
		itemElement = node->InsertEndChild(TiXmlElement("DepthStencilCubeTexture"))->ToElement();
		itemElement->SetAttribute("Count", m_nDepthStencilCubeTextureCounter);
		itemElement = node->InsertEndChild(TiXmlElement("VolumeTexture"))->ToElement();
		itemElement->SetAttribute("Count", m_nVolumeTextureCounter);
		itemElement = node->InsertEndChild(TiXmlElement("VolumeTextureDefault"))->ToElement();
		itemElement->SetAttribute("Count", m_nVolumeTextureDefaultCounter);

#ifndef FINAL_RELEASE
		node = profileNode->InsertEndChild(TiXmlElement("Memory"))->ToElement();
		itemElement = node->InsertEndChild(TiXmlElement("RenderTarget"))->ToElement();
		itemElement->SetAttribute("Mono", (m_nRenderTargetsMonoSize/1024/1024));
		itemElement->SetAttribute("Stereo", (m_nRenderTargetsStereoSize/1024/1024));
		itemElement = node->InsertEndChild(TiXmlElement("RenderTargetTextures"))->ToElement();
		itemElement->SetAttribute("Mono", (m_nRenderTargetTexturesMonoSize/1024/1024));
		itemElement->SetAttribute("Stereo", (m_nRenderTargetTexturesStereoSize/1024/1024));
		itemElement = node->InsertEndChild(TiXmlElement("DepthStencilSurfaces"))->ToElement();
		itemElement->SetAttribute("Mono", (m_nDepthStencilSurfacesMonoSize/1024/1024));
		itemElement->SetAttribute("Stereo", (m_nDepthStencilSurfacesStereoSize/1024/1024));
		itemElement = node->InsertEndChild(TiXmlElement("DepthStencilTextures"))->ToElement();
		itemElement->SetAttribute("Mono", (m_nDepthStencilTexturesMonoSize/1024/1024));
		itemElement->SetAttribute("Stereo", (m_nDepthStencilTexturesStereoSize/1024/1024));
		itemElement = node->InsertEndChild(TiXmlElement("Textures"))->ToElement();
		itemElement->SetAttribute("Mono", (m_nTexturesSize/1024/1024));
		itemElement = node->InsertEndChild(TiXmlElement("TexturesDef"))->ToElement();
		itemElement->SetAttribute("Mono", (m_nTexturesDefaultSize/1024/1024));
		itemElement = node->InsertEndChild(TiXmlElement("CubeTextures"))->ToElement();
		itemElement->SetAttribute("Mono", (m_nCubeTexturesSize/1024/1024));
		itemElement = node->InsertEndChild(TiXmlElement("CubeTexturesDef"))->ToElement();
		itemElement->SetAttribute("Mono", (m_nCubeTexturesDefaultSize/1024/1024));
		itemElement = node->InsertEndChild(TiXmlElement("RenderTargetCubeTextures"))->ToElement();
		itemElement->SetAttribute("Mono", (m_nRenderTargetCubeTexturesMonoSize/1024/1024));
		itemElement->SetAttribute("Stereo", (m_nRenderTargetCubeTexturesStereoSize/1024/1024));
		itemElement = node->InsertEndChild(TiXmlElement("DepthStencilCubeTextures"))->ToElement();
		itemElement->SetAttribute("Mono", (m_nDepthStencilCubeTexturesMonoSize/1024/1024));
		itemElement->SetAttribute("Stereo", (m_nDepthStencilCubeTexturesStereoSize/1024/1024));
#endif

		node = profileNode->InsertEndChild(TiXmlElement("Warnings"))->ToElement();
		itemElement = node->InsertEndChild(TiXmlElement("CantCreateWideSurface"))->ToElement();
		itemElement->SetAttribute("Value", m_bDebugWarningCantCreateWideSurface);
		itemElement = node->InsertEndChild(TiXmlElement("OutOfVideoMemory"))->ToElement();
		itemElement->SetAttribute("Value", m_bOutOfVideoMemory);
		itemElement = node->InsertEndChild(TiXmlElement("MonoDepthStencil"))->ToElement();
		itemElement->SetAttribute("Value", m_bDebugWarningMonoDepthStencil);
		itemElement = node->InsertEndChild(TiXmlElement("UseMultipleSwapChains"))->ToElement();
		itemElement->SetAttribute("Value", m_bUseMultipleSwapChains);
		itemElement = node->InsertEndChild(TiXmlElement("EmulateFullscreenMode"))->ToElement();
		itemElement->SetAttribute("Value", m_bDebugWarningEmulateFullscreenMode);

		node = profileNode->InsertEndChild(TiXmlElement("Sessions"))->ToElement();
		itemElement = node->InsertEndChild(TiXmlElement("Count"))->ToElement();
		itemElement->SetAttribute("Mono", m_nMonoSessionsCounter);
		itemElement->SetAttribute("Stereo", m_nStereoSessionsCounter);
		itemElement = node->InsertEndChild(TiXmlElement("TotalTime"))->ToElement();
		itemElement->SetDoubleAttribute("Mono", m_fTotalMonoTime);
		itemElement->SetDoubleAttribute("Stereo", m_fTotalStereoTime);
		itemElement = node->InsertEndChild(TiXmlElement("MaxTime"))->ToElement();
		itemElement->SetDoubleAttribute("Mono", m_fMaxMonoTime);
		itemElement->SetDoubleAttribute("Stereo", m_fMaxStereoTime);

		float m = 0.0f;
		DEBUG_MESSAGE(_T("\n------------- Sessions -------------\n"));
		DEBUG_MESSAGE(_T("Total mono:	\t%dh\t%fm\n"), getModAndRem(m_fTotalMonoTime, 60, m), m);
		DEBUG_MESSAGE(_T("Total stereo:	\t%dh\t%fm\n"), getModAndRem(m_fTotalStereoTime, 60, m), m);
		float averageMonoTime = m_fTotalMonoTime / std::max(m_nMonoSessionsCounter, 1l);
		DEBUG_MESSAGE(_T("Average mono:	\t%dh\t%fm\n"), getModAndRem(averageMonoTime, 60, m), m);
		float averageStereoTime = m_fTotalStereoTime / std::max(m_nStereoSessionsCounter, 1l);
		DEBUG_MESSAGE(_T("Average stereo:	\t%dh\t%fm\n"), getModAndRem(averageStereoTime, 60, m), m);
		DEBUG_MESSAGE(_T("Max mono:	\t%dh\t%fm\n"), getModAndRem(m_fMaxMonoTime, 60, m), m);
		DEBUG_MESSAGE(_T("Max stereo:	\t%dh\t%fm\n"), getModAndRem(m_fMaxStereoTime, 60, m), m);
		DEBUG_MESSAGE(_T("Mono sessions:	\t%d\n"), m_nMonoSessionsCounter);
		DEBUG_MESSAGE(_T("Stereo sessions:	%d\n\n"), m_nStereoSessionsCounter);

		m_fTotalMonoTime = 0.0f;
		m_fTotalStereoTime = 0.0f;
		m_nMonoSessionsCounter = 0;
		m_nStereoSessionsCounter = 0;
		m_fMaxMonoTime = 0.0f;
		m_fMaxStereoTime = 0.0f;

		doc.SaveFile();
		DEBUG_MESSAGE(_T("Statistic writen\n"));
	}
}

void CBaseStereoRenderer::GetStates(AllStates* states)
{
#ifndef FINAL_RELEASE
#define SAVERS(s, d) {m_Direct3DDevice.GetRenderState(s, (DWORD*)&states->rs.##d); };
#define SAVESS(n, s, d) {m_Direct3DDevice.GetSamplerState(n, s, (DWORD*)&states->ss[n].##d); };
#define SAVETSS(n, s, d) {m_Direct3DDevice.GetTextureStageState(n, s, (DWORD*)&states->tss[n].##d); };
	// Vertex States
	SAVERS(D3DRS_CULLMODE, CullMode);
	SAVERS(D3DRS_FOGCOLOR, FogColor);
	SAVERS(D3DRS_FOGTABLEMODE, FogTableMode);
	SAVERS(D3DRS_FOGSTART, FogStart);
	SAVERS(D3DRS_FOGEND, FogEnd);
	SAVERS(D3DRS_FOGDENSITY, FogDensity);
	SAVERS(D3DRS_RANGEFOGENABLE, RangeFogEnable);
	SAVERS(D3DRS_AMBIENT, Ambient);
	SAVERS(D3DRS_COLORVERTEX, ColorVertex);
	SAVERS(D3DRS_FOGVERTEXMODE, FogVertexMode);
	SAVERS(D3DRS_CLIPPING, Clipping);
	SAVERS(D3DRS_LIGHTING, Lighting);
	SAVERS(D3DRS_LOCALVIEWER, LocalViewer);
	SAVERS(D3DRS_EMISSIVEMATERIALSOURCE, EmissiveMaterialSource);
	SAVERS(D3DRS_AMBIENTMATERIALSOURCE, AmbientMaterialSource);
	SAVERS(D3DRS_DIFFUSEMATERIALSOURCE, DiffuseMaterialSource);
	SAVERS(D3DRS_SPECULARMATERIALSOURCE, SpecularMaterialSource);
	SAVERS(D3DRS_VERTEXBLEND, VertexBlend);
	SAVERS(D3DRS_CLIPPLANEENABLE, ClipPlaneEnable);
	SAVERS(D3DRS_POINTSIZE, PointSize);
	SAVERS(D3DRS_POINTSIZE_MIN, PointSizeMin);
	SAVERS(D3DRS_POINTSPRITEENABLE, PointSpriteEnable);
	SAVERS(D3DRS_POINTSCALEENABLE, PointScaleEnable);
	SAVERS(D3DRS_POINTSCALE_A, PointScaleA);
	SAVERS(D3DRS_POINTSCALE_B, PointScaleB);
	SAVERS(D3DRS_POINTSCALE_C, PointScaleC);
	SAVERS(D3DRS_MULTISAMPLEANTIALIAS, MultiSampleAntiAlias);
	SAVERS(D3DRS_MULTISAMPLEMASK, MultiSampleMask);
	SAVERS(D3DRS_PATCHEDGESTYLE, PatchEdgeStyle);
	SAVERS(D3DRS_POINTSIZE_MAX, PointSizeMax);
	SAVERS(D3DRS_INDEXEDVERTEXBLENDENABLE, IndexedVertexBlendEnable);
	SAVERS(D3DRS_TWEENFACTOR, TweenFactor);
	SAVERS(D3DRS_POSITIONDEGREE, PositionDegree);
	SAVERS(D3DRS_NORMALDEGREE, NormalDegree);
	SAVERS(D3DRS_MINTESSELLATIONLEVEL, MinTessellationLevel);
	SAVERS(D3DRS_MAXTESSELLATIONLEVEL, MaxTessellationLevel);
	SAVERS(D3DRS_ADAPTIVETESS_X, AdaptiveTessX);
	SAVERS(D3DRS_ADAPTIVETESS_Y, AdaptiveTessY);
	SAVERS(D3DRS_ADAPTIVETESS_Z, AdaptiveTessZ);
	SAVERS(D3DRS_ADAPTIVETESS_W, AdaptiveTessW);
	SAVERS(D3DRS_ENABLEADAPTIVETESSELLATION, EnableAdaptiveTesselation);

	SAVESS(0, D3DSAMP_DMAPOFFSET, DMapOffset);
	SAVETSS(0, D3DTSS_TEXCOORDINDEX, TexCoordIndex);
	SAVETSS(0, D3DTSS_TEXTURETRANSFORMFLAGS, TextureTransformFlags);
	m_Direct3DDevice.GetVertexShader(&states->pVS);
	if (states->pVS)
		states->pVS->Release();
	for (DWORD i = 0 ; i < m_dwMaxStreams; i++)
	{
		m_Direct3DDevice.GetStreamSource(i, &states->vb[i].pVB, &states->vb[i].Offset, &states->vb[i].Stride);
		if (states->vb[i].pVB)
			states->vb[i].pVB->Release();
		m_Direct3DDevice.GetStreamSourceFreq(i, &states->divider[i]);
	}
	m_Direct3DDevice.GetIndices(&states->pIB);
	if (states->pIB)
		states->pIB->Release();
	m_Direct3DDevice.GetFVF(&states->Fvf);
	m_Direct3DDevice.GetVertexDeclaration(&states->pVD);
	if (states->pVD)
		states->pVD->Release();
	D3DCAPS9 D3DCaps;
	m_Direct3DDevice.GetDeviceCaps( &D3DCaps );
	memcpy((float*)states->VSConsts, m_VertexShaderRegister, 4 * sizeof(float) * D3DCaps.MaxVertexShaderConst);

	// Pixel States
	states->vp = m_ViewPort;
	states->sr = m_ScissorRect;
#endif
}

void CBaseStereoRenderer::PrintStates(AllStates* states)
{
#ifndef FINAL_RELEASE
#define PRINTRS(s, d) { DEBUG_MESSAGE(_T("RS %S: %d\n"), #s, *((DWORD*)(&states->rs.##d))); }
#define PRINTSS(n, s, d) { DEBUG_MESSAGE(_T("SS%d %S: %d\n"), n, #s, *((DWORD*)(&states->ss[n].##d))); }
#define PRINTTSS(n, s, d) { DEBUG_MESSAGE(_T("TSS%d %S: %d\n"), n, #s, *((DWORD*)(&states->tss[n].##d))); }
	// Vertex States
	PRINTRS(D3DRS_CULLMODE, CullMode);
	PRINTRS(D3DRS_FOGCOLOR, FogColor);
	PRINTRS(D3DRS_FOGTABLEMODE, FogTableMode);
	PRINTRS(D3DRS_FOGSTART, FogStart);
	PRINTRS(D3DRS_FOGEND, FogEnd);
	PRINTRS(D3DRS_FOGDENSITY, FogDensity);
	PRINTRS(D3DRS_RANGEFOGENABLE, RangeFogEnable);
	PRINTRS(D3DRS_AMBIENT, Ambient);
	PRINTRS(D3DRS_COLORVERTEX, ColorVertex);
	PRINTRS(D3DRS_FOGVERTEXMODE, FogVertexMode);
	PRINTRS(D3DRS_CLIPPING, Clipping);
	PRINTRS(D3DRS_LIGHTING, Lighting);
	PRINTRS(D3DRS_LOCALVIEWER, LocalViewer);
	PRINTRS(D3DRS_EMISSIVEMATERIALSOURCE, EmissiveMaterialSource);
	PRINTRS(D3DRS_AMBIENTMATERIALSOURCE, AmbientMaterialSource);
	PRINTRS(D3DRS_DIFFUSEMATERIALSOURCE, DiffuseMaterialSource);
	PRINTRS(D3DRS_SPECULARMATERIALSOURCE, SpecularMaterialSource);
	PRINTRS(D3DRS_VERTEXBLEND, VertexBlend);
	PRINTRS(D3DRS_CLIPPLANEENABLE, ClipPlaneEnable);
	PRINTRS(D3DRS_POINTSIZE, PointSize);
	PRINTRS(D3DRS_POINTSIZE_MIN, PointSizeMin);
	PRINTRS(D3DRS_POINTSPRITEENABLE, PointSpriteEnable);
	PRINTRS(D3DRS_POINTSCALEENABLE, PointScaleEnable);
	PRINTRS(D3DRS_POINTSCALE_A, PointScaleA);
	PRINTRS(D3DRS_POINTSCALE_B, PointScaleB);
	PRINTRS(D3DRS_POINTSCALE_C, PointScaleC);
	PRINTRS(D3DRS_MULTISAMPLEANTIALIAS, MultiSampleAntiAlias);
	PRINTRS(D3DRS_MULTISAMPLEMASK, MultiSampleMask);
	PRINTRS(D3DRS_PATCHEDGESTYLE, PatchEdgeStyle);
	PRINTRS(D3DRS_POINTSIZE_MAX, PointSizeMax);
	PRINTRS(D3DRS_INDEXEDVERTEXBLENDENABLE, IndexedVertexBlendEnable);
	PRINTRS(D3DRS_TWEENFACTOR, TweenFactor);
	PRINTRS(D3DRS_POSITIONDEGREE, PositionDegree);
	PRINTRS(D3DRS_NORMALDEGREE, NormalDegree);
	PRINTRS(D3DRS_MINTESSELLATIONLEVEL, MinTessellationLevel);
	PRINTRS(D3DRS_MAXTESSELLATIONLEVEL, MaxTessellationLevel);
	PRINTRS(D3DRS_ADAPTIVETESS_X, AdaptiveTessX);
	PRINTRS(D3DRS_ADAPTIVETESS_Y, AdaptiveTessY);
	PRINTRS(D3DRS_ADAPTIVETESS_Z, AdaptiveTessZ);
	PRINTRS(D3DRS_ADAPTIVETESS_W, AdaptiveTessW);
	PRINTRS(D3DRS_ENABLEADAPTIVETESSELLATION, EnableAdaptiveTesselation);

	PRINTSS(0, D3DSAMP_DMAPOFFSET, DMapOffset);
	PRINTTSS(0, D3DTSS_TEXCOORDINDEX, TexCoordIndex);
	PRINTTSS(0, D3DTSS_TEXTURETRANSFORMFLAGS, TextureTransformFlags);
	DEBUG_MESSAGE(_T("eS %p\n"), states->pVS);
	for (DWORD i = 0 ; i < m_dwMaxStreams; i++)
	{
		DEBUG_MESSAGE(_T("VB %p Offset %d Stride %d Divider %d\n"), 
			states->vb[i].pVB, states->vb[i].Offset, 
			states->vb[i].Stride, states->divider[i]);
	}
	DEBUG_MESSAGE(_T("IB %p\n"), states->pIB);
	DEBUG_MESSAGE(_T("FVF 0x%X\n"), states->Fvf);
	DEBUG_MESSAGE(_T("VD %p\n"), states->pVD);

	// Pixel States
	DEBUG_MESSAGE(_T("ViewPort %s\n"), GetViewPortString(&m_ViewPort));
	DEBUG_MESSAGE(_T("ScissorRect %s\n"), GetRectString(&m_ScissorRect));
#endif
}

void CBaseStereoRenderer::CheckStates(AllStates* states)
{
#ifndef FINAL_RELEASE
#define CHECKRS(s, d) {if (*((DWORD*)(&states->rs.##d)) != *((DWORD*)(&newStates->rs.##d))) { DEBUG_MESSAGE(_T("Render state did't match. %S old = %d, new = %d\n"), #s, *((DWORD*)(&states->rs.##d)), *((DWORD*)(&newStates->rs.##d)));}}
#define CHECKSS(n, s, d) {if (*((DWORD*)(&states->ss[n].##d)) != *((DWORD*)(&newStates->ss[n].##d))) { DEBUG_MESSAGE(_T("Sampler state did't match. %d: %S old = %d, new = %d\n"), n, #s, *((DWORD*)(&states->ss[n].##d)), *((DWORD*)(&newStates->ss[n].##d)));}}
#define CHECKTSS(n, s, d) {if (*((DWORD*)(&states->tss[n].##d)) != *((DWORD*)(&newStates->tss[n].##d))) { DEBUG_MESSAGE(_T("Texture stage state did't match. %d: %S old = %d, new = %d\n"), n, #s, *((DWORD*)(&states->tss[n].##d)), *((DWORD*)(&newStates->tss[n].##d)));}}
	AllStates ns;
	AllStates* newStates = &ns;
	GetStates(newStates);
	// Vertex States
	CHECKRS(D3DRS_CULLMODE, CullMode);
	CHECKRS(D3DRS_FOGCOLOR, FogColor);
	CHECKRS(D3DRS_FOGTABLEMODE, FogTableMode);
	CHECKRS(D3DRS_FOGSTART, FogStart);
	CHECKRS(D3DRS_FOGEND, FogEnd);
	CHECKRS(D3DRS_FOGDENSITY, FogDensity);
	CHECKRS(D3DRS_RANGEFOGENABLE, RangeFogEnable);
	CHECKRS(D3DRS_AMBIENT, Ambient);
	CHECKRS(D3DRS_COLORVERTEX, ColorVertex);
	CHECKRS(D3DRS_FOGVERTEXMODE, FogVertexMode);
	CHECKRS(D3DRS_CLIPPING, Clipping);
	CHECKRS(D3DRS_LIGHTING, Lighting);
	CHECKRS(D3DRS_LOCALVIEWER, LocalViewer);
	CHECKRS(D3DRS_EMISSIVEMATERIALSOURCE, EmissiveMaterialSource);
	CHECKRS(D3DRS_AMBIENTMATERIALSOURCE, AmbientMaterialSource);
	CHECKRS(D3DRS_DIFFUSEMATERIALSOURCE, DiffuseMaterialSource);
	CHECKRS(D3DRS_SPECULARMATERIALSOURCE, SpecularMaterialSource);
	CHECKRS(D3DRS_VERTEXBLEND, VertexBlend);
	CHECKRS(D3DRS_CLIPPLANEENABLE, ClipPlaneEnable);
	CHECKRS(D3DRS_POINTSIZE, PointSize);
	CHECKRS(D3DRS_POINTSIZE_MIN, PointSizeMin);
	CHECKRS(D3DRS_POINTSPRITEENABLE, PointSpriteEnable);
	CHECKRS(D3DRS_POINTSCALEENABLE, PointScaleEnable);
	CHECKRS(D3DRS_POINTSCALE_A, PointScaleA);
	CHECKRS(D3DRS_POINTSCALE_B, PointScaleB);
	CHECKRS(D3DRS_POINTSCALE_C, PointScaleC);
	CHECKRS(D3DRS_MULTISAMPLEANTIALIAS, MultiSampleAntiAlias);
	CHECKRS(D3DRS_MULTISAMPLEMASK, MultiSampleMask);
	CHECKRS(D3DRS_PATCHEDGESTYLE, PatchEdgeStyle);
	CHECKRS(D3DRS_POINTSIZE_MAX, PointSizeMax);
	CHECKRS(D3DRS_INDEXEDVERTEXBLENDENABLE, IndexedVertexBlendEnable);
	CHECKRS(D3DRS_TWEENFACTOR, TweenFactor);
	CHECKRS(D3DRS_POSITIONDEGREE, PositionDegree);
	CHECKRS(D3DRS_NORMALDEGREE, NormalDegree);
	CHECKRS(D3DRS_MINTESSELLATIONLEVEL, MinTessellationLevel);
	CHECKRS(D3DRS_MAXTESSELLATIONLEVEL, MaxTessellationLevel);
	CHECKRS(D3DRS_ADAPTIVETESS_X, AdaptiveTessX);
	CHECKRS(D3DRS_ADAPTIVETESS_Y, AdaptiveTessY);
	CHECKRS(D3DRS_ADAPTIVETESS_Z, AdaptiveTessZ);
	CHECKRS(D3DRS_ADAPTIVETESS_W, AdaptiveTessW);
	CHECKRS(D3DRS_ENABLEADAPTIVETESSELLATION, EnableAdaptiveTesselation);

	CHECKSS(0, D3DSAMP_DMAPOFFSET, DMapOffset);
	CHECKTSS(0, D3DTSS_TEXCOORDINDEX, TexCoordIndex);
	CHECKTSS(0, D3DTSS_TEXTURETRANSFORMFLAGS, TextureTransformFlags);
	if (states->pVS != newStates->pVS)
		DEBUG_MESSAGE(_T("Vertex shader did't match. old = %p, new = %p\n"), states->pVS, newStates->pVS);
	for (DWORD i = 0 ; i < m_dwMaxStreams; i++)
	{
		if (states->vb[i].pVB != newStates->vb[i].pVB)
			DEBUG_MESSAGE(_T("Vertex buffer did't match. old = %p, new = %p\n"), states->vb[i].pVB, newStates->vb[i].pVB);
		if (states->vb[i].Offset != newStates->vb[i].Offset)
			DEBUG_MESSAGE(_T("Vertex buffer Offset did't match. old = %d, new = %d\n"), states->vb[i].Offset, newStates->vb[i].Offset);
		if (states->vb[i].Stride != newStates->vb[i].Stride)
			DEBUG_MESSAGE(_T("Vertex buffer Stride did't match. old = %d, new = %d\n"), states->vb[i].Stride, newStates->vb[i].Stride);
		if (states->divider[i] != newStates->divider[i])
			DEBUG_MESSAGE(_T("Vertex buffer Divider did't match. old = %d, new = %d\n"), states->divider[i], newStates->divider[i]);
	}
	if (states->pIB!= newStates->pIB)
		DEBUG_MESSAGE(_T("Index buffer did't match. old = %p, new = %p\n"), states->pIB, newStates->pIB);
	if (states->Fvf!= newStates->Fvf)
		DEBUG_MESSAGE(_T("FVF did't match. old = 0x%X, new = 0x%X\n"), states->Fvf, newStates->Fvf);
	if (states->pVD!= newStates->pVD)
		DEBUG_MESSAGE(_T("Vertex Declaration did't match. old = %p, new = %p\n"), states->pVD, newStates->pVD);
	D3DCAPS9 D3DCaps;
	m_Direct3DDevice.GetDeviceCaps( &D3DCaps );
	for (size_t i = 0; i < D3DCaps.MaxVertexShaderConst; i++)
	{
		if (states->VSConsts[i][0] != newStates->VSConsts[i][0])
			DEBUG_MESSAGE(_T("Vertex constant X did't match. old = %f, new = %f\n"), states->VSConsts[i][0], newStates->VSConsts[i][0]);
		if (states->VSConsts[i][1] != newStates->VSConsts[i][1])	
			DEBUG_MESSAGE(_T("Vertex constant Y did't match. old = %f, new = %f\n"), states->VSConsts[i][1], newStates->VSConsts[i][1]);
		if (states->VSConsts[i][2] != newStates->VSConsts[i][2])	
			DEBUG_MESSAGE(_T("Vertex constant Z did't match. old = %f, new = %f\n"), states->VSConsts[i][2], newStates->VSConsts[i][2]);
		if (states->VSConsts[i][3] != newStates->VSConsts[i][3])	
			DEBUG_MESSAGE(_T("Vertex constant W did't match. old = %f, new = %f\n"), states->VSConsts[i][3], newStates->VSConsts[i][3]);
	}

	// Pixel States
	states->vp = m_ViewPort;
	if (states->vp.X != newStates->vp.X || states->vp.Y != newStates->vp.Y || states->vp.Width != newStates->vp.Width || states->vp.Height != newStates->vp.Height || 
		states->vp.MinZ != newStates->vp.MinZ || states->vp.MaxZ != newStates->vp.MaxZ)
		DEBUG_MESSAGE(_T("ViewPort did't match. old = %s, new = %s\n"), GetViewPortString(&states->vp), GetViewPortString(&newStates->vp));
	states->sr = m_ScissorRect;
	if (states->sr.left != newStates->sr.left || states->sr.right != newStates->sr.right || states->sr.bottom != newStates->sr.bottom || states->sr.top != newStates->sr.top)
		DEBUG_MESSAGE(_T("ScissorRect did't match. old = %s, new = %s\n"), GetRectString(&states->sr), GetRectString(&newStates->sr));
#endif
}

void CBaseStereoRenderer::DumpSurface(TCHAR *pComment, IDirect3DSurface9 *pSurface)
{
	TCHAR szFileName[MAX_PATH];
	if (pSurface)
	{
		_stprintf_s(szFileName, L"%s\\%s", METHOD, pComment);
		SaveSurfaceToFile(szFileName, pSurface);
	}
}

void CBaseStereoRenderer::DumpTexture(TCHAR *pComment, IDirect3DBaseTexture9 *pTexture)
{
	TCHAR szFileName[MAX_PATH];
	if (pTexture)
	{
		_stprintf_s(szFileName, L"%s\\%s", METHOD, pComment);
		SaveTextureToFile(szFileName, pTexture);
	}
}

HRESULT CBaseStereoRenderer::DumpSurfaces(TCHAR *comment, IDirect3DSurface9 *pLeft, IDirect3DSurface9 *pRight)
{
	if (!pLeft)
		return S_OK;

	HRESULT hResult = S_OK;
	TCHAR szFileName[MAX_PATH];
	_stprintf_s(szFileName, L"%s\\%.4d.%s.%s.Left", 
		DRAW_HISTORY, m_nEventCounter, comment, GetObjectName(pLeft));
	if (SUCCEEDED(SaveSurfaceToFile(szFileName, pLeft))) {
		DEBUG_TRACE3(_T("Dumping %s\n"), GetFileNameFromPath(szFileName));
	} else {
		DEBUG_TRACE3(_T("Dumping error %s\n"), GetFileNameFromPath(szFileName));
	}

	if (RenderToRight() && pRight)
	{
		_stprintf_s(szFileName, L"%s\\%.4d.%s.%s.Right", 
			DRAW_HISTORY, m_nEventCounter, comment, GetObjectName(pLeft));
		if (SUCCEEDED(SaveSurfaceToFile(szFileName, pRight))) {
			DEBUG_TRACE3(_T("Dumping %s\n"), GetFileNameFromPath(szFileName));
		} else {
			DEBUG_TRACE3(_T("Dumping error %s\n"), GetFileNameFromPath(szFileName));
		}
	}
	return hResult;
}

void CBaseStereoRenderer::DumpRTSurfaces( BOOL bRenderTo2RT, TCHAR* drawName, TCHAR* szAdditionalData, BOOL bClear )
{
#ifndef FINAL_RELEASE
	TCHAR szFileName[MAX_PATH];
	TCHAR EIDText[20];
	EIDText[0] = '\0';
	if (gInfo.EID != 0)
		_stprintf_s(EIDText, L"[%.4I64d].", gInfo.EID);
	// RT
	if (bRenderTo2RT)
	{
		for (size_t i = 0; i < m_dwNumSimultaneousRTs; i++)
		{
			if (m_RenderTarget[i].m_pMainSurf)
			{
				if (bClear || m_dwColorWriteEnable[i])
				{
					_stprintf_s(szFileName, L"%s\\%s%.4d.%s.RT%d.%s.%s%s", 
						m_DumpDirectory, EIDText, m_nEventCounter, drawName, i, GetModeString(),
						GetObjectName(m_RenderTarget[i].m_pMainSurf), szAdditionalData);
					SaveStereoSurfaceToFile(szFileName, m_RenderTarget[i].m_pMainSurf, m_RenderTarget[i].m_pRightSurf);
				}
				else
				{
					DEBUG_TRACE3(_T("CW disabled for RT %i %s\n"), i, GetObjectName(m_RenderTarget[i].m_pMainSurf));
				}
			}
		}
	}
	else
	{
		for (size_t i = 0; i < m_dwNumSimultaneousRTs; i++)
		{
			if (m_RenderTarget[i].m_pMainSurf)
			{
				if (bClear || m_dwColorWriteEnable[i])
				{
					_stprintf_s(szFileName, L"%s\\%s%.4d.%s.RT%d.%s.%s%s.Left", 
						m_DumpDirectory, EIDText, m_nEventCounter, drawName, i, _T("Mono"),
						GetObjectName(m_RenderTarget[i].m_pMainSurf), szAdditionalData);

					if (SUCCEEDED(SaveSurfaceToFile(szFileName, m_RenderTarget[i].m_pMainSurf))) {
						DEBUG_TRACE3(_T("Dumping %s\n"), GetFileNameFromPath(szFileName));
					} else {
						DEBUG_TRACE3(_T("Dumping error %s\n"), GetFileNameFromPath(szFileName));
					}

					//if (m_RenderTarget[i].pRightSurf)
					//{
					//	_stprintf_s(szFileName, "%s\\%s\\%.4d.%s.RT%d.%s.%s%s.Right", 
					//		DUMPDIR, DRAW_HISTORY, m_nEventCounter, drawName, i, _T("Mono"),
					//		GetObjectName(m_RenderTarget[i].pRightSurf), szAdditionalData);

					//	if (SUCCEEDED(SaveSurfaceToFile(szFileName, m_RenderTarget[i].pRightSurf))) {
					//	DEBUG_TRACE3("Dumping %s\n", GetFileNameFromPath(szFileName));
					//} else {
					//	DEBUG_TRACE3("Dumping error %s\n", GetFileNameFromPath(szFileName));
					//}
					//}
				}
				else
				{
					DEBUG_TRACE3(_T("CW disabled for RT %i %s\n"), i, GetObjectName(m_RenderTarget[i].m_pMainSurf));
				}
			}
		}
	}
#endif
}

bool CBaseStereoRenderer::CloneDepthSurfaces(IDirect3DTexture9* pDepthTexure, CComPtr<IDirect3DSurface9> &pLeft, CComPtr<IDirect3DSurface9> &pRight)
{
	INSIDE;
	if (gInfo.MonoDepthStencilTextures || !pDepthTexure)
		return false;

	bool bSuccess = CopyDepthSurfaces(pDepthTexure, pLeft, pRight);
	if (m_bUseR32FDepthTextures)
		return bSuccess;

	HRESULT hResult = S_OK;

	if (bSuccess)
	{
		pLeft.Release();
		pRight.Release();
	}

	IDirect3DTexture9* pWideTex = NULL;
	CComPtr<IDirect3DTexture9> pLeftTex;
	CComPtr<IDirect3DTexture9> pRightTex;
	GetStereoObject(pDepthTexure, &pRightTex);
	CONST SIZE*	 pOffset = NULL;
	if (pRightTex)
		pLeftTex = pDepthTexure;
	else // wide mode
	{
		pWideTex = pDepthTexure;
		GetLeftTexture(pDepthTexure, &pLeftTex);
		GetStereoObject(pLeftTex, &pRightTex);			
	}
	if (!pLeftTex || !pRightTex)
		return false;

	D3DSURFACE_DESC desc;
	pLeftTex->GetLevelDesc(0, &desc);

	// copy from wide texture because we can't read from left and right
	if (pWideTex && (desc.Format == D3DFMT_D16 ||desc.Format == D3DFMT_D24S8 || desc.Format == D3DFMT_D24X8))
	{
		CComPtr<IDirect3DSurface9> pWideSurf;
		pDepthTexure->GetSurfaceLevel(0, &pWideSurf);
		pOffset = CWideStereoRenderer::GetOffset(pWideSurf);
		pLeftTex.Release();
		pLeftTex = pWideTex;
		pRightTex.Release();
		pRightTex = pWideTex;
	}

	vbs* vb;
	CaptureAll(vb);
	NSCALL(m_pCopyState->Apply());
	DWORD rt[4] = { true, false, false, false };
	if(RenderToRight())
	{
		RECT rightTexRect = { 0, 0, desc.Width, desc.Height };
		if (pOffset)
			OffsetRect(&rightTexRect, pOffset->cx, pOffset->cy);
		CComPtr<IDirect3DTexture9> pRightDestTex;
		m_Direct3DDevice.CreateTexture(desc.Width, desc.Height, 1, D3DUSAGE_RENDERTARGET, 
			D3DFMT_R32F, D3DPOOL_DEFAULT, &pRightDestTex, NULL);
		LPCTSTR name = GetObjectName(pRightTex);
		SetObjectName(pRightDestTex, name);
		NSCALL(pRightDestTex->GetSurfaceLevel(0, &pRight));
		NSCALL(m_Direct3DDevice.SetRenderTarget(0, pRight));
		OneToOneRender(pRightTex, &rightTexRect, NULL);
	}
	RECT leftTexRect = { 0, 0, desc.Width, desc.Height };
	CComPtr<IDirect3DTexture9> pLeftDestTex;
	m_Direct3DDevice.CreateTexture(desc.Width, desc.Height, 1, D3DUSAGE_RENDERTARGET, 
		D3DFMT_R32F, D3DPOOL_DEFAULT, &pLeftDestTex, NULL);
	LPCTSTR name = GetObjectName(pLeftTex);
	SetObjectName(pLeftDestTex, name);
	NSCALL(pLeftDestTex->GetSurfaceLevel(0, &pLeft));
	NSCALL(m_Direct3DDevice.SetRenderTarget(0, pLeft));
	OneToOneRender(pLeftTex, &leftTexRect, NULL);
	ApplyAll(vb);
	return true;
}

void CBaseStereoRenderer::DumpDSSurfaces( BOOL bRenderTo2RT, TCHAR* drawName, TCHAR* szAdditionalData )
{
#ifndef FINAL_RELEASE
	// DS
	if (!m_DepthStencil.m_pMainSurf)
		return;

	TCHAR szFileName[MAX_PATH];
	CComPtr<IDirect3DSurface9> pLeft;
	CComPtr<IDirect3DSurface9> pRight;
	bool bSuccess = CloneDepthSurfaces(m_DepthStencil.m_pMainTex, pLeft, pRight);
	if (!pLeft)
		bSuccess = false;
	if (!bSuccess && GINFO_LOCKABLEDEPTHSTENCIL)
	{
		bSuccess = true;
		pLeft = m_DepthStencil.m_pMainSurf;
		pRight = m_DepthStencil.m_pRightSurf;
	}
	if (bSuccess)
	{
		TCHAR EIDText[20];
		EIDText[0] = '\0';
		if (gInfo.EID != 0)
			_stprintf_s(EIDText, L"[%.4I64d].", gInfo.EID);
		if (bRenderTo2RT)
		{
			_stprintf_s(szFileName, L"%s\\%s%.4d.%s.DS.%s.%s%s", 
				m_DumpDirectory, EIDText, m_nEventCounter, drawName, GetModeString(),  
				GetObjectName(pLeft), szAdditionalData);
			SaveStereoSurfaceToFile(szFileName, pLeft, pRight);
		}
		else
		{
			_stprintf_s(szFileName, L"%s\\%s%.4d.%s.DS.%s.%s%s.%s", 
				m_DumpDirectory, EIDText, m_nEventCounter, drawName, GetModeString(),  
				GetObjectName(pLeft), szAdditionalData, _T("Left"));
			if (SUCCEEDED(SaveSurfaceToFile(szFileName,pLeft))) {
				DEBUG_TRACE3(_T("Dumping %s\n"), GetFileNameFromPath(szFileName));
			} else {
				DEBUG_TRACE3(_T("Dumping error %s\n"), GetFileNameFromPath(szFileName));
			}
		}
	}
	else
	{
		DEBUG_TRACE3(_T("DS %s\n"), GetObjectName(m_DepthStencil.m_pMainSurf));
		if (m_DepthStencil.m_pRightSurf && bRenderTo2RT)
			DEBUG_TRACE3(_T("DS %s\n"), GetObjectName(m_DepthStencil.m_pRightSurf));
	}
#endif
}

HRESULT CBaseStereoRenderer::DumpTextures(TCHAR *comment, IDirect3DBaseTexture9 *pLeft, IDirect3DBaseTexture9 *pRight)
{
	if (!pLeft)
		return S_OK;

	HRESULT hResult = S_OK;
	TCHAR szFileName[MAX_PATH];
	_stprintf_s(szFileName, L"%s\\%.4d.%s.%s.Left", 
		DRAW_HISTORY, m_nEventCounter, comment, GetObjectName(pLeft));
	if (SUCCEEDED(SaveTextureToFile(szFileName, pLeft))) {
		DEBUG_TRACE3(_T("Dumping %s\n"), GetFileNameFromPath(szFileName));
	} else {
		DEBUG_TRACE3(_T("Dumping error %s\n"), GetFileNameFromPath(szFileName));
	}

	if (RenderToRight() && pRight)
	{
		_stprintf_s(szFileName, L"%s\\%.4d.%s.%s.Right", 
			DRAW_HISTORY, m_nEventCounter, comment, GetObjectName(pLeft));
		if (SUCCEEDED(SaveTextureToFile(szFileName, pRight))) {
			DEBUG_TRACE3(_T("Dumping %s\n"), GetFileNameFromPath(szFileName));
		} else {
			DEBUG_TRACE3(_T("Dumping error %s\n"), GetFileNameFromPath(szFileName));
		}
	}
	return hResult;
}

void CBaseStereoRenderer::SaveStereoTextureToFile(TCHAR* szFileName, IDirect3DBaseTexture9* pLeftTex, IDirect3DBaseTexture9* pRightTex)
{
	INSIDE;
	TCHAR szFullFileName[1024];
#ifndef FINAL_RELEASE
	if (!gInfo.DumpStereoImages)
	{
		_stprintf_s(szFullFileName, _T("%s.%s"), 
			szFileName, _T("Left"));

		if (SUCCEEDED(SaveTextureToFile(szFullFileName, pLeftTex))) {
			DEBUG_TRACE3(_T("Dumping %s\n"), GetFileNameFromPath(szFullFileName));
		} else {
			DEBUG_TRACE3(_T("Dumping error %s\n"), GetFileNameFromPath(szFullFileName));
		}

		if (pRightTex)
		{
			_stprintf_s(szFullFileName, _T("%s.%s"), 
				szFileName, _T("Right"));

			if (SUCCEEDED(SaveTextureToFile(szFullFileName, pRightTex))) {
				DEBUG_TRACE3(_T("Dumping %s\n"), GetFileNameFromPath(szFullFileName));
			} else {
				DEBUG_TRACE3(_T("Dumping error %s\n"), GetFileNameFromPath(szFullFileName));
			}
		}
	}
	else
#endif
	{
		HRESULT hResult;
		CComQIPtr<IDirect3DTexture9> pLeft = pLeftTex;
		if (!pLeft)
			return;

		CComQIPtr<IDirect3DTexture9> pRight = pRightTex;
		if (!pRight)
			return;

		CComPtr<IDirect3DSurface9> pLeftSurf;
		NSCALL(pLeft->GetSurfaceLevel(0, &pLeftSurf));
		CComPtr<IDirect3DSurface9> pRightSurf;
		NSCALL(pRight->GetSurfaceLevel(0, &pRightSurf));
		D3DSURFACE_DESC desc;
		NSCALL(pLeftSurf->GetDesc(&desc));

		CComPtr<IDirect3DSurface9> JPSSurface;
		UINT Width = desc.Width;
		UINT Height = desc.Height;
		NSCALL_ERRMSG(m_Direct3DDevice.CreateRenderTarget(Width * 2, Height, desc.Format, 
			D3DMULTISAMPLE_NONE, 0, FALSE, &JPSSurface, 0),
			DEBUG_MESSAGE(_T("CreateRenderTarget(Width = %d, Height = %d, Format = %s, MultiSample = D3DMULTISAMPLE_NONE,")
			_T("MultisampleQuality = 0, Lockable = FALSE, *ppSurface = %p, pSharedHandle = 0)"), 
			Width * 2, Height, GetFormatString(desc.Format), JPSSurface));
		if(SUCCEEDED(hResult))
		{
			RECT leftDestRect = { Width, 0, Width * 2, Height };
			NSCALL(m_Direct3DDevice.StretchRect(pLeftSurf, NULL, JPSSurface, &leftDestRect, D3DTEXF_NONE));
			RECT rightDestRect = { 0, 0, Width, Height };
			NSCALL(m_Direct3DDevice.StretchRect(pRightSurf, NULL, JPSSurface, &rightDestRect, D3DTEXF_NONE));

			_tcscpy(szFullFileName, szFileName);
			_tcscat(szFullFileName, GetStereoImageFileExtension());
			if (SUCCEEDED(SaveSurfaceToFile(szFullFileName, JPSSurface, GetImageFileFormat()))) {
				DEBUG_TRACE3(_T("Dumping %s\n"), GetFileNameFromPath(szFullFileName));
			} else {
				DEBUG_TRACE3(_T("Dumping error %s\n"), GetFileNameFromPath(szFullFileName));
			}
		}
	}
}

void CBaseStereoRenderer::SaveStereoSurfaceToFile(TCHAR* szFileName, IDirect3DSurface9* pLeftSurf, IDirect3DSurface9* pRightSurf)
{
	TCHAR szFullFileName[1024];
	_ASSERT(pLeftSurf);
#ifndef FINAL_RELEASE
	if (!gInfo.DumpStereoImages)
	{
		_stprintf_s(szFullFileName, _T("%s.%s"), 
			szFileName, (gInfo.WideRenderTarget ? _T("Wide") : _T("Left")));

		if (SUCCEEDED(SaveSurfaceToFile(szFullFileName, pLeftSurf))) {
			DEBUG_TRACE3(_T("Dumping %s\n"), GetFileNameFromPath(szFullFileName));
		} else {
			DEBUG_TRACE3(_T("Dumping error %s\n"), GetFileNameFromPath(szFullFileName));
		}

		if (pRightSurf && RenderToRight())
		{
			_stprintf_s(szFullFileName, _T("%s.%s"), 
				szFileName, _T("Right"));

			if (SUCCEEDED(SaveSurfaceToFile(szFullFileName, pRightSurf))) {
				DEBUG_TRACE3(_T("Dumping %s\n"), GetFileNameFromPath(szFullFileName));
			} else {
				DEBUG_TRACE3(_T("Dumping error %s\n"), GetFileNameFromPath(szFullFileName));
			}
		}
	}
	else
#endif
	{
		HRESULT hResult;
		RECT leftRect, rightRect;
		D3DFORMAT format;
		GetStereoSurfRects(pLeftSurf, pRightSurf, format, &leftRect, &rightRect);
		CComPtr<IDirect3DSurface9> JPSSurface;
		UINT Width = leftRect.right;
		UINT Height = leftRect.bottom;
		NSCALL(m_Direct3DDevice.CreateRenderTarget(Width * 2, Height, format, 
			D3DMULTISAMPLE_NONE, 0, FALSE, &JPSSurface, 0));
		if(SUCCEEDED(hResult))
		{
			RECT leftDestRect = { Width, 0, Width * 2, Height };
			NSCALL(m_Direct3DDevice.StretchRect(pLeftSurf, &leftRect, JPSSurface, &leftDestRect, D3DTEXF_NONE));
			RECT rightDestRect = { 0, 0, Width, Height };
			NSCALL(m_Direct3DDevice.StretchRect(pRightSurf, &rightRect, JPSSurface, &rightDestRect, D3DTEXF_NONE));

			_tcscpy(szFullFileName, szFileName);
			_tcscat(szFullFileName, GetStereoImageFileExtension());
			if (SUCCEEDED(SaveSurfaceToFile(szFullFileName, JPSSurface, GetImageFileFormat()))) {
				DEBUG_TRACE3(_T("Dumping %s\n"), GetFileNameFromPath(szFullFileName));
			} else {
				DEBUG_TRACE3(_T("Dumping error %s\n"), GetFileNameFromPath(szFullFileName));
			}
		}
	}
}

void CBaseStereoRenderer::DumpPipelineTexture( const TexData& texData, BOOL bRenderTo2RT, const TCHAR* prefix, DWORD samplerIndex, const ResourceCRCMultiplier* pCRCMultiplier ) 
{
#ifndef FINAL_RELEASE
	TCHAR szTextureCRC[MAX_PATH];
	if(pCRCMultiplier && pCRCMultiplier->first)
	{
		DWORD Size = GetTextureFirstSurfaceSize(texData.m_pLeft);

		DEBUG_TRACE3(_T("\t%s Stage %d %s CRC 0x%X Size %d\n"), prefix, samplerIndex, 
			GetObjectName(texData.m_pLeft), pCRCMultiplier->first, Size);
		_sntprintf_s(szTextureCRC, _TRUNCATE, _T("Texture CRC 0x%X Size %d"), 
			pCRCMultiplier->first, Size);
	}
	else
	{
		DEBUG_TRACE3(_T("\t%s Stage %d %s No CRC\n"), prefix, samplerIndex,
			GetObjectName(texData.m_pLeft));
		_tcscpy_s(szTextureCRC, _T("No CRC"));
	}

	if (!DO_FULLDUMP)
		return;

	D3DSURFACE_DESC Desc;
	CComQIPtr<IDirect3DTexture9> pLeftTex(texData.m_pLeft);
	HRESULT hr = E_FAIL;
	if (pLeftTex)
		hr = pLeftTex->GetLevelDesc(0, &Desc);
	if (FAILED(hr))
		return;
	TCHAR szFileName[MAX_PATH];
	TCHAR EIDText[20];
	EIDText[0] = 0;
	if (gInfo.EID != 0)
		_stprintf_s(EIDText, L"[%.4I64d].", gInfo.EID);
	if (!pLeftTex || !(Desc.Usage & D3DUSAGE_DEPTHSTENCIL))
	{
		if(texData.m_pRight && bRenderTo2RT)
		{
			_stprintf_s(szFileName, L"%s\\%s%.4d.TX.%s.%02d.%s.%s(%s)", 
				m_DumpDirectory, EIDText, m_nEventCounter, 
				prefix, samplerIndex, _T("Stereo"),
				GetObjectName(texData.m_pLeft), szTextureCRC);
			SaveStereoTextureToFile(szFileName, texData.m_pLeft, 
				texData.m_pRight);
		}
		else
		{
			_stprintf_s(szFileName, L"%s\\%s%.4d.TX.%s.%02d.%s.%s(%s)", 
				m_DumpDirectory, EIDText, m_nEventCounter, 
				prefix, samplerIndex, _T("Mono"),
				GetObjectName(texData.m_pLeft), szTextureCRC);

			if (SUCCEEDED(SaveTextureToFile(szFileName, texData.m_pLeft))) {
				DEBUG_TRACE3(_T("Dumping %s\n"), GetFileNameFromPath(szFileName));
			} else {
				DEBUG_TRACE3(_T("Dumping error %s\n"), GetFileNameFromPath(szFileName));
			}
		}
	}
	else
	{
		IDirect3DTexture9* pMainTex = texData.m_pWideTexture;
		if (pMainTex == NULL)
			pMainTex = pLeftTex;
		CComQIPtr<IDirect3DTexture9> pRightTex(texData.m_pRight);
		CComPtr<IDirect3DSurface9> pLeft;
		CComPtr<IDirect3DSurface9> pRight;
		bool bSuccess = CloneDepthSurfaces(pMainTex, pLeft, pRight);
		if (!pLeft)
			bSuccess = false;
		if (bSuccess)
		{
			if (bRenderTo2RT)
			{
				_stprintf_s(szFileName, L"%s\\%s%.4d.TX.%s.%02d.%s.%s(%s)", 
					m_DumpDirectory, EIDText, m_nEventCounter, 
					prefix, samplerIndex, _T("Stereo"),
					GetObjectName(texData.m_pLeft), szTextureCRC);
				SaveStereoSurfaceToFile(szFileName, pLeft, pRight);
			}
			else
			{
				_stprintf_s(szFileName, L"%s\\%s%.4d.TX.%s.%02d.%s.%s(%s)", 
					m_DumpDirectory, EIDText, m_nEventCounter, 
					prefix, samplerIndex, _T("Mono"),
					GetObjectName(texData.m_pLeft), szTextureCRC);
				if (SUCCEEDED(SaveSurfaceToFile(szFileName,pLeft))) {
					DEBUG_TRACE3(_T("Dumping %s\n"), GetFileNameFromPath(szFileName));
				} else {
					DEBUG_TRACE3(_T("Dumping error %s\n"), GetFileNameFromPath(szFileName));
				}
			}
		}
	}
#endif
}

void CBaseStereoRenderer::DumpAllStuff( BOOL bRenderTo2RT, TCHAR* drawName )
{
#ifndef FINAL_RELEASE
	if (IsKeyDown(VK_SHIFT) && IsKeyDown(VK_SCROLL))
	{
		DEBUG_MESSAGE(_T("CompleteDump canceled\n"));
		m_DumpType = dtNone;
		zlog::SetupSeverity(m_nOldTrace);
		return;
	}

	// Input Assembler Data
	DEBUG_TRACE3(_T("MeshCRC "));
	for(UINT i=0; i< m_dwMaxStreams; i++)
	{
		if(m_MeshCRCMultiplier[i] && m_MeshCRCMultiplier[i]->first)
		{
			if(i)
				DEBUG_TRACE3(_T(",\n\t"));
			UINT Size = 0;
			if (m_pVB[i])
			{
				D3DVERTEXBUFFER_DESC desc;
				m_pVB[i]->GetDesc(&desc);
				Size = desc.Size;
			}
			DEBUG_TRACE3(_T("Stream %d CRC 0x%X Size %d"), i, m_MeshCRCMultiplier[i]->first, Size);
		}
	}
	DEBUG_TRACE3(_T("\n"));

	// Textures
	DEBUG_TRACE3(_T("TextureCRC\n"));
	DumpPipelineTextures(m_PSPipelineData, bRenderTo2RT, _T("PS"));
	DumpPipelineTextures(m_VSPipelineData, bRenderTo2RT, _T("VS"));

	if (!DO_RTDUMP)
		return;

	// Shaders
	TCHAR szShaderName[128] = _T("");
	if(m_Pipeline == Shader)
	{
		if(m_VSPipelineData.m_bShaderDataAvailable)
			_sntprintf_s(szShaderName, _TRUNCATE, _T(" VS #%d (CRC 0x%X)"), 
			m_VSPipelineData.m_CurrentShaderData.shaderGlobalIndex, m_VSPipelineData.m_CurrentShaderData.CRC32);
		else
			_sntprintf_s(szShaderName, _TRUNCATE, _T(" Mono VS"));
	}
	else
	{
		_sntprintf_s(szShaderName, _TRUNCATE, _T(" %s Pipeline"), GetPipelineString(m_Pipeline));
	}
	TCHAR szPSName[64] = _T("");
	if(m_PSPipelineData.m_bShaderDataAvailable)
		_sntprintf_s(szPSName, _TRUNCATE, _T(" PS #%d (CRC 0x%X)"), 
		m_PSPipelineData.m_CurrentShaderData.shaderGlobalIndex, m_PSPipelineData.m_CurrentShaderData.CRC32);
	else
		_sntprintf_s(szPSName, _TRUNCATE, _T(" PS NULL"));
	TCHAR szSuffix[64] = _T("");
	if (m_ViewPort.MinZ != 0.0f || m_ViewPort.MaxZ != 1.0f)
		_sntprintf_s(szSuffix, _TRUNCATE, _T(" [MinZ %.4f MaxZ %.4f]"), m_ViewPort.MinZ, m_ViewPort.MaxZ);
	TCHAR szSuffix2[64] = _T("");
	if (m_ZNear != 0.0f || m_ZFar != 0.0f)
		_sntprintf_s(szSuffix2, _TRUNCATE, _T(" [Zn %.2f Zf %.2f]"), m_ZNear, m_ZFar);
	TCHAR szAdditionalData[MAX_PATH] = _T("");
	_stprintf_s(szAdditionalData, _T("(%s%s)%s%s"), szShaderName, szPSName, szSuffix, szSuffix2);

	DumpRTSurfaces(bRenderTo2RT, drawName, szAdditionalData, FALSE);
	DumpDSSurfaces(bRenderTo2RT, drawName, szAdditionalData);
#endif
}

void CBaseStereoRenderer::DumpTexturesAndRT(TCHAR* drawName, BOOL bRenderTo2RT, BOOL bHeavy)
{
#ifndef FINAL_RELEASE
	if (DO_DUMP)
		DumpAllStuff(bRenderTo2RT, drawName);

	// Counters
	DEBUG_TRACE3(_T("---------- Event %i complete ----------\n"), m_nEventCounter);
	m_nEventCounter++;
	m_nDrawCounter++;
	if(bHeavy)
		m_nDrawHeavyCounter++;
	if (IsWeaponDrawCall())
		m_nWeaponDrawCounter++;
	else if(m_bRenderInStereo)
	{
		switch(m_Pipeline)
		{
		case RHW:
		case RHWEmul:
			m_nDrawRHWCounter++;
			break;
		case Fixed:
			m_nDrawFixedCounter++;
			break;
		default: // VS
			m_nDrawVSCounter++;
			break;
		}
	}
	else if(!m_bUsedStereoTextures)
		m_nDrawMonoCounter++;
	//FLUSHLOGFILE();
#endif
}

void CBaseStereoRenderer::DumpIndexedMeshesUP(UINT MinIndex, 
		UINT IndexCount, const void* pIndexData, D3DFORMAT IndexDataFormat, 
		UINT VertexCount, const void* pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
	DEBUG_TRACE3(_T("DumpIndexedMeshesUP(IndexCount=%d, VertexCount=%d)\n"), IndexCount, VertexCount);
	UINT indexSize = IndexDataFormat == D3DFMT_INDEX16 ? 2 : 4;
	PBYTE pIndex = (PBYTE)pIndexData + MinIndex * indexSize;
	for(UINT i = 0; i < IndexCount; i++)
	{
		UINT index = IndexDataFormat == D3DFMT_INDEX16 ? *((WORD*)pIndex) : *((DWORD*)pIndex);
		index -= MinIndex;

		struct RHWHEADER { FLOAT x, y, z, rhw; } *s;
		// support only position with zero offset from vertex
		s = (RHWHEADER*)((BYTE*)pVertexStreamZeroData + VertexStreamZeroStride * index);
		if (!m_bRHWVertexDeclaration) {
			DEBUG_TRACE3(_T("\t%d: x=%f y=%f z=%f\n"), i, s->x, s->y, s->z);
		} else {
			DEBUG_TRACE3(_T("\t%d: x=%f y=%f z=%f rhw=%f\n"), i, s->x, s->y, s->z, s->rhw);
		}
		pIndex += indexSize;
	}	
}

void CBaseStereoRenderer::DumpMeshesUP(UINT VertexCount, const void* pVertexStreamZeroData,
													  UINT VertexStreamZeroStride)
{
	DEBUG_TRACE3(_T("DumpMeshesUP(VertexCount=%d)\n"), VertexCount);
	struct RHWHEADER { FLOAT x, y, z, rhw; } *s;
	// support only position with zero offset from vertex
	s = (RHWHEADER*)((BYTE*)pVertexStreamZeroData);
	for(UINT i = 0; i < VertexCount; i++)
	{
		if (!m_bRHWVertexDeclaration) {
			DEBUG_TRACE3(_T("\t%d: x=%f y=%f z=%f\n"), i, s->x, s->y, s->z);
		} else {
			DEBUG_TRACE3(_T("\t%d: x=%f y=%f z=%f rhw=%f\n"), i, s->x, s->y, s->z, s->rhw);
		}
		s = (RHWHEADER*)((BYTE*)s + VertexStreamZeroStride);
	}
}

void CBaseStereoRenderer::DumpIndexedMeshes( INT BaseVertexIndex, UINT MinVertexIndex, UINT IndexCount, 
									 UINT StartIndex, UINT VertexCount)
{
	DEBUG_TRACE3(_T("DumpIndexedMeshes\n"));
	HRESULT hResult = S_OK;
	void *pSrc;
	CComPtr<IDirect3DVertexBuffer9> pVertexBuffer;
	UINT OffsetInBytes, Stride;
	// support only position in zero stream
	NSCALL(m_Direct3DDevice.GetStreamSource( 0, &pVertexBuffer, &OffsetInBytes, &Stride ));

	if(SUCCEEDED(pVertexBuffer->Lock( (BaseVertexIndex + MinVertexIndex) * Stride, VertexCount * Stride, &pSrc, D3DLOCK_READONLY)))
		{
		void *pSrcIndex;
		CComPtr<IDirect3DIndexBuffer9> pIndexBuffer;
		NSCALL(m_Direct3DDevice.GetIndices( &pIndexBuffer ));
		D3DINDEXBUFFER_DESC desc;
		pIndexBuffer->GetDesc(&desc);
		UINT indexSize = desc.Format == D3DFMT_INDEX16 ? 2 : 4;
		if(SUCCEEDED(pIndexBuffer->Lock( StartIndex * indexSize, IndexCount * indexSize, &pSrcIndex, D3DLOCK_READONLY)))
		{
			DumpIndexedMeshesUP(MinVertexIndex, IndexCount, pSrcIndex, desc.Format,
				VertexCount, pSrc, Stride);
			pIndexBuffer->Unlock();
		}
		pVertexBuffer->Unlock();
	}
}

void CBaseStereoRenderer::DumpMeshes(UINT Start, UINT VertexCount)
{
	DEBUG_TRACE3(_T("DumpMeshes\n"));
	HRESULT hResult = S_OK;
	void *pSrc;
	CComPtr<IDirect3DVertexBuffer9> pVertexBuffer;
	UINT OffsetInBytes, Stride;
	// support only position in zero stream
	NSCALL(m_Direct3DDevice.GetStreamSource( 0, &pVertexBuffer, &OffsetInBytes, &Stride ));

	if(SUCCEEDED(pVertexBuffer->Lock( Start * Stride, VertexCount * Stride, &pSrc, D3DLOCK_READONLY)))
	{
		DumpMeshesUP(VertexCount, pSrc, Stride);
		pVertexBuffer->Unlock();
	}
}

void CBaseStereoRenderer::SaveDepthMapToFile( TCHAR * fileName, D3DSURFACE_DESC* pDesc, D3DLOCKED_RECT &lockedRect )
{
	FILE * pFile;
	pFile = _tfopen ( fileName , _T("wb") );
	fwrite (&pDesc->Width,	1, sizeof(pDesc->Width),	pFile );
	fwrite (&pDesc->Height,	1, sizeof(pDesc->Height),	pFile );
	DWORD d = pDesc->Width * sizeof(float);
	fwrite (&d,	1, sizeof(d),	pFile );
	fwrite (&lockedRect.Pitch,	1, sizeof(lockedRect.Pitch),	pFile );
	fwrite (lockedRect.pBits, 1, pDesc->Height * lockedRect.Pitch , pFile );
	fclose (pFile);
}

void CBaseStereoRenderer::GetMinMaxValues( float& minValue, float& maxValue, DWORD* pHistogram, 
										  IDirect3DTexture9* pLeftTexture, IDirect3DTexture9* pRightTexture,
										  bool bSaveToFile )
{
	INSIDE;
	HRESULT hResult = S_OK;
	minValue = 0.0f;
	maxValue = 1.0f;

	CComPtr<IDirect3DSurface9> pLeft;
	pLeftTexture->GetSurfaceLevel(0, &pLeft);
	D3DSURFACE_DESC desc;
	pLeft->GetDesc(&desc);
	D3DSURFACE_DESC* pDesc = &desc;

	CComPtr<IDirect3DSurface9> pMemSurfLeft;
	CComPtr<IDirect3DSurface9> pMemSurfRight;
	NSCALL(m_Direct3DDevice.CreateOffscreenPlainSurface(desc.Width, desc.Height, desc.Format, 
		D3DPOOL_SYSTEMMEM, &pMemSurfLeft, NULL));
	NSCALL(m_Direct3DDevice.GetRenderTargetData(pLeft, pMemSurfLeft));
	if (FAILED(hResult))
		return;
	D3DLOCKED_RECT lockedRectLeft, lockedRectRight;
	NSCALL(pMemSurfLeft->LockRect(&lockedRectLeft, NULL, 0));
	if (FAILED(hResult))
		return;
	minValue = 1.0f;
	maxValue = 0.0f;

	if (RenderToRight())
	{
		CComPtr<IDirect3DSurface9> pRight;
		pRightTexture->GetSurfaceLevel(0, &pRight);

		NSCALL(m_Direct3DDevice.CreateOffscreenPlainSurface(desc.Width, desc.Height, desc.Format, 
			D3DPOOL_SYSTEMMEM, &pMemSurfRight, NULL));
		NSCALL(m_Direct3DDevice.GetRenderTargetData(pRight, pMemSurfRight));
		if (SUCCEEDED(hResult)) {
			NSCALL(pMemSurfRight->LockRect(&lockedRectRight, NULL, 0));
		}
		if (FAILED(hResult))
			pMemSurfRight.Release();
	}

	for(UINT i = 0; i < 258 + 8; i++)
		pHistogram[i] = 0;
	DWORD& maxCount = pHistogram[256];
	DWORD& totalCount = pHistogram[257];
	totalCount = pDesc->Width * pDesc->Height * 2;
	for(UINT y = 0; y < pDesc->Height; y++)
	{
		PFLOAT pData = (PFLOAT)((PBYTE)lockedRectLeft.pBits + y * lockedRectLeft.Pitch);
		for(UINT x = 0; x < pDesc->Width; x++)
		{
			int ind = (int)(*pData * 255 + 0.5f);
			pHistogram[ind]++;
			if (pHistogram[ind] > pHistogram[maxCount])
				maxCount = ind;
			pData++;
		}
	}
	if (pMemSurfRight)
	{
		for(UINT y = 0; y < pDesc->Height; y++)
		{
			PFLOAT pData = (PFLOAT)((PBYTE)lockedRectRight.pBits + y * lockedRectRight.Pitch);
			for(UINT x = 0; x < pDesc->Width; x++)
			{
				int ind = (int)(*pData * 255 + 0.5f);
				pHistogram[ind]++;
				if (pHistogram[ind] > pHistogram[maxCount])
					maxCount = ind;
				pData++;
			}
		}
	}

	DWORD* miniHistogram = &pHistogram[258];
	for(UINT i = 0; i < 256; i++)
		miniHistogram[i / (256 / 8)] += pHistogram[i];

	for(UINT y = 0; y < pDesc->Height; y++)
	{
		PFLOAT pData = (PFLOAT)((PBYTE)lockedRectLeft.pBits + y * lockedRectLeft.Pitch);
		for(UINT x = 0; x < pDesc->Width; x++)
		{
			if (minValue > *pData)
				minValue = *pData;
			if (maxValue < *pData)
				maxValue = *pData;
			pData++;
		}
	}
	if (pMemSurfRight)
	{
		for(UINT y = 0; y < pDesc->Height; y++)
		{
			PFLOAT pData = (PFLOAT)((PBYTE)lockedRectRight.pBits + y * lockedRectRight.Pitch);
			for(UINT x = 0; x < pDesc->Width; x++)
			{
				if (minValue > *pData)
					minValue = *pData;
				if (maxValue < *pData)
					maxValue = *pData;
				pData++;
			}
		}
	}
	if (bSaveToFile)
	{
		using namespace std;
		TCHAR path[MAX_PATH];
		TCHAR fileName[MAX_PATH];
		SHGetFolderPath(NULL, CSIDL_MYPICTURES|CSIDL_FLAG_CREATE, NULL, 0, path);
		PathAppend(path, _T(PRODUCT_NAME) _T(" Screenshots") );
		CreateDirectory(path, 0);
		PathAppend(path, gInfo.ProfileName );
		CreateDirectory(path, 0);
		_stprintf_s(fileName, L"%s\\%s%s.depth", path, gInfo.ProfileName, 
			pMemSurfRight ? _T("-left") : _T(""));
		SaveDepthMapToFile(fileName, pDesc, lockedRectLeft);
		if (pMemSurfRight)
		{
			_stprintf_s(fileName, L"%s\\%s-right.depth", path, gInfo.ProfileName);
			SaveDepthMapToFile(fileName, pDesc, lockedRectRight);
		}
		if (pHistogram)
		{
			DWORD maxCount = pHistogram[256];
			maxCount = pHistogram[maxCount];
			CComPtr<IDirect3DSurface9> pHistogramSurf;
			NSCALL(m_Direct3DDevice.CreateOffscreenPlainSurface(512, 256, D3DFMT_X8R8G8B8, 
				D3DPOOL_SYSTEMMEM, &pHistogramSurf, NULL));
			D3DLOCKED_RECT lockedRect;
			NSCALL(pHistogramSurf->LockRect(&lockedRect, NULL, D3DLOCK_DISCARD));
			for(UINT y = 0; y < 256; y++)
			{
				PDWORD pData = (PDWORD)((PBYTE)lockedRect.pBits + y * lockedRect.Pitch);
				for(UINT x = 0; x < 512; x++)
				{
					float f = 255.0f * pHistogram[x / 2] / maxCount;
					bool bFiled = (256 - y) < f;
					*pData = bFiled ? 0xFF000000 : 0x00FFFFFF;
					pData++;
				}
			}
			NSCALL(pHistogramSurf->UnlockRect());
			_stprintf_s(fileName, L"%s\\%s-histogram", path, gInfo.ProfileName);
			SaveSurfaceToFile(fileName, pHistogramSurf);
		}
	}
	if (pMemSurfRight)
		pMemSurfRight->UnlockRect();
	pMemSurfLeft->UnlockRect();
}

HRESULT CBaseStereoRenderer::CaptureAll( vbs *&vb )
{
	HRESULT hResult = S_OK;
	DEBUG_TRACE3(_T("CaptureAll\n"));
	//DumpVSStates();		
	//D3DCAPS9 D3DCaps;
	//NSCALL(m_pDirect3DDevice.GetDeviceCaps( &D3DCaps ));
	//FLOAT* fVS = DNew float[4*D3DCaps.MaxVertexShaderConst]; 
	//memcpy(fVS, m_VertexShaderRegister, 4 * sizeof(float) * D3DCaps.MaxVertexShaderConst)
	//DWORD MaxPixelShaderConst = 0;
	//if (D3DCaps.PixelShaderVersion >= D3DPS_VERSION(3, 0))
	//	MaxPixelShaderConst = 224;
	//else if (D3DCaps.PixelShaderVersion >= D3DPS_VERSION(2, 0))
	//	MaxPixelShaderConst = 32;
	//else if (D3DCaps.PixelShaderVersion >= D3DPS_VERSION(1, 0))
	//	MaxPixelShaderConst = 8;
	//FLOAT* fPS = DNew float[4*MaxPixelShaderConst]; 
	//NSCALL(m_pDirect3DDevice.GetPixelShaderConstantF(0, fPS,MaxPixelShaderConst));

	vb = DNew vbs[m_dwMaxStreams]; 
	for(DWORD i = 0; i < m_dwMaxStreams; i++)
	{
		vb[i].pVB = NULL;
		vb[i].Offset = 0;
		NSCALL(m_Direct3DDevice.GetStreamSource(i, &vb[i].pVB, &vb[i].Offset, &vb[i].Stride));
	}

	if (m_pSavedState) NSCALL(m_pSavedState->Capture());
	return 	hResult;
}

HRESULT CBaseStereoRenderer::ApplyAll( vbs *&vb )
{
	INSIDE;
	HRESULT hResult = S_OK;
	DEBUG_TRACE3(_T("ApplyAll\n"));
	for (DWORD i = 0; i< m_dwNumSimultaneousRTs; i++)
		if (i < 2 ||  m_RenderTarget[i].m_pMainSurf)
			m_Direct3DDevice.SetRenderTarget(i,m_RenderTarget[i].m_pMainSurf);
	m_Direct3DDevice.SetDepthStencilSurface(m_DepthStencil.m_pMainSurf);
	if (m_pSavedState) NSCALL(m_pSavedState->Apply());
	NSCALL(m_Direct3DDevice.SetViewport(&m_ViewPort));
	NSCALL(m_Direct3DDevice.SetScissorRect(&m_ScissorRect));
	// restore steams offset because state block drop they to zero
	for(DWORD i = 0; i < m_dwMaxStreams; i++)
	{
		if (vb[i].Offset != 0)
			NSCALL(m_Direct3DDevice.SetStreamSource(i, vb[i].pVB, vb[i].Offset, vb[i].Stride));
		if (vb[i].pVB)
			vb[i].pVB->Release();
	}
	delete[] vb; 

	//NSCALL(m_pDirect3DDevice.SetVertexShaderConstantF(0, fVS, D3DCaps.MaxVertexShaderConst)); 
	//delete[] fVS;
	//NSCALL(m_pDirect3DDevice.SetPixelShaderConstantF(0, fPS,MaxPixelShaderConst));
	//delete[] fPS;
	//DumpVSStates();
	return 	hResult;
}

DWORD CBaseStereoRenderer::CalculateSurfaceCRC( D3DSURFACE_DESC* pDesc, IDirect3DSurface9* pSurface, CONST RECT* pSourceRect )
{
	HRESULT hResult = S_OK;
	CComPtr<IDirect3DSurface9> pSurf = pSurface;
	DWORD CRC32 = 0;
	UINT Width = pDesc->Width;
	UINT Height = pDesc->Height;

	D3DSURFACE_DESC Desc;
	D3DLOCKED_RECT LockedRect;
	hResult = pSurf->LockRect(&LockedRect, pSourceRect, D3DLOCK_READONLY);

	if (FAILED(hResult)) // non-lockable surface
	{
		CComPtr<IDirect3DSurface9> pUnpackedSurface;
		NSCALL_TRACE_RES(m_Direct3DDevice.CreateOffscreenPlainSurface(Width, Height, 
			D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &pUnpackedSurface, 0), pUnpackedSurface,
			DEBUG_MESSAGE(_T("CreateOffscreenPlainSurface(Width = %u, Height = %u, Format = %s, ")
			_T("Pool = %s, *ppSurface = %p, pSharedHandle = %p)"), 
			Width, Height, GetFormatString(D3DFMT_A8R8G8B8), 
			GetPoolString(D3DPOOL_SYSTEMMEM), pUnpackedSurface, 0));
		if(SUCCEEDED(hResult))
		{
			RECT DestRect;
			RECT* pDestRect = NULL;
			if (pSourceRect)
			{
				DestRect = *pSourceRect;
				pDestRect = &DestRect;
				DestRect.bottom -= DestRect.top;
				DestRect.top = 0;
				DestRect.right -= DestRect.left;
				DestRect.left = 0;
			}
			NSCALL_TRACE3(D3DXLoadSurfaceFromSurface(pUnpackedSurface, 0, pDestRect, 
				pSurf, 0, pSourceRect, D3DX_FILTER_NONE, 0),
				DEBUG_MESSAGE(_T("D3DXLoadSurfaceFromSurface(pDestSurface = %p[%s], 0, 0, pSourceSurface = %p[%s], 0, 0, D3DX_FILTER_NONE, 0)"),
				pUnpackedSurface, GetObjectName(pUnpackedSurface), pSurf, GetObjectName(pSurf)));
			if(SUCCEEDED(hResult))
			{
				pSurf.Release();
				pSurf = pUnpackedSurface;
				pDesc = &Desc;
				pSurf->GetDesc(pDesc);
				hResult = pSurf->LockRect(&LockedRect, pDestRect, D3DLOCK_READONLY);
			}
		}
	}

	if (SUCCEEDED(hResult))
	{
		if (IsCompressedFormat(pDesc->Format))
		{
			Width = RoundUp(Width, 4) * 4;
			Height = RoundUp(Height, 4) / 4;
		}
		UINT LineLength = Width * BitsPerPixel(pDesc->Format) / 8;
		if (LineLength == LockedRect.Pitch)
			CRC32 = CalculateCRC32((const BYTE *)LockedRect.pBits, LockedRect.Pitch * Height);
		else // lockable DYNAMIC surface
		{
			CRC32 = 0xFFFFFFFF;
			PBYTE pData = (PBYTE)LockedRect.pBits;
			for (UINT i = 0; i < Height; i++)
			{
				CRC32 = CalculateCRC32(pData, LineLength, CRC32);
				pData += LockedRect.Pitch;
			}
		}
		pSurf->UnlockRect();
	}	
	return CRC32;
}

const ResourceCRCMultiplier* CBaseStereoRenderer::CRCTextureFromSurfaceCheck( IDirect3DSurface9 * pSrcSurf, CONST RECT* pSourceRect, IDirect3DBaseTexture9 * pDestTex )
{
	HRESULT hResult = S_OK;
	const ResourceCRCMultiplier *pCRCMultiplier = GetCRCMultiplier(pDestTex);
	if (pCRCMultiplier != NULL)
		return pCRCMultiplier;
	bool bCalculateForAll = 
#ifdef ZLOG_ALLOW_TRACING
		(zlog::GetSeverity() == zlog::SV_FLOOD);
#else
		false;
#endif
	bool CantCalculateCRC = true;
#ifndef FINAL_RELEASE
	LARGE_INTEGER currentTime;
	if(GINFO_DEBUG)
		QueryPerformanceCounter(&currentTime);
#endif
	_ASSERT(pSrcSurf);
	D3DSURFACE_DESC Desc;
	pSrcSurf->GetDesc(&Desc);
	if (pSourceRect &&
		((Desc.Width != (pSourceRect->right - pSourceRect->left)) ||
		(Desc.Height != (pSourceRect->bottom - pSourceRect->top))))
		return NULL;
	ResourcesCRCMultiplierMapBySize::const_iterator si;
	DWORD Size = GetSurfaceSize(Desc);
	if (NeedCalculateTextureCRC(Size, si) || bCalculateForAll)
	{
		if (!(Desc.Format == D3DFMT_D16 || Desc.Format == D3DFMT_D24FS8 ||	
			Desc.Format == D3DFMT_D24S8 || Desc.Format == D3DFMT_D32 || 
			Desc.Format == D3DFMT_D24X4S4 || Desc.Format == D3DFMT_D24X8))
		{
			INSIDE;
			DWORD CRC32 = CalculateSurfaceCRC(&Desc, pSrcSurf, pSourceRect);
			if (CRC32)
			{
				ResourceCRCMultiplier pair;
				pair.first = CRC32;
				if (si != g_ProfileData.TextureCRCMultiplier.end())
				{
					ResourceCRCMultiplierMap::const_iterator iter = si->second.find(pair.first);
					if (iter != si->second.end())
					{
						pair.second = iter->second;
						DEBUG_TRACE3(_T("TextureCRC = 0x%X Size=%d Multiplier = %f detected\n"), 
							iter->first, Size, iter->second.m_Multiplier);
					}
				}
				pCRCMultiplier = SetCRCMultiplier(pDestTex, &pair);
				CantCalculateCRC = false;
			}
		}
	}
	if (CantCalculateCRC)
	{
		ResourceCRCMultiplier pair;
		pair.first = 0;
		pCRCMultiplier = SetCRCMultiplier(pDestTex, &pair);
	}
#ifndef FINAL_RELEASE
	if(GINFO_DEBUG)
	{
		LARGE_INTEGER stop;
		QueryPerformanceCounter(&stop);
		m_nCRCCalculatingTime.QuadPart += stop.QuadPart - currentTime.QuadPart;
	}
#endif
	return pCRCMultiplier;
}

DWORD CBaseStereoRenderer::CalculateVBCRC( IDirect3DVertexBuffer9* pStreamData, D3DVERTEXBUFFER_DESC* pDesc )
{
	DWORD CRC32 = 0;
	BYTE* p = NULL;
	if(SUCCEEDED(pStreamData->Lock(0, 0, (void**)&p, D3DLOCK_READONLY)))
	{
		CRC32 = CalculateCRC32(p, pDesc->Size);
		pStreamData->Unlock();
	}
	return CRC32;
}

void CBaseStereoRenderer::CheckYouTubeTexture( UINT Width, UINT Height, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9* pTexture )
{
	if (gInfo.GameSpecific != 2)
		return;

	if (!(Usage == 0 && (Format == D3DFMT_X8R8G8B8 || Format == D3DFMT_A8R8G8B8) && Pool == D3DPOOL_MANAGED)) // Flash 10.3
		return;

	//if (!((Width == 853 && Height == 533) || (Width == 854 && Height == 534) || 
	//	(Width == 1280 && Height == 800) || (Width == 1920 && Height == 1200) ||
	//	(Width == 1680 && Height == 1050) || // YouTube
	//	(Width == 768 && Height == 480) || (Width == 960 && Height == 600))) // YouTube 3D 
	//	return;
	if (Width < 768 || Height < 480)
		return;

	DEBUG_TRACE1(_T("\tYouTube magic texture\n"));
	HRESULT hResult;
	CComPtr<IDirect3DTexture9> pLeftTexture;
	NSCALL_TRACE1(m_Direct3DDevice.CreateTexture(Width, Height, 1, 0, Format, Pool, &pLeftTexture, NULL),
		DEBUG_MESSAGE(_T("\tCreateTexture(Width = %d, Height = %d, Levels = %d, Usage = %s, ")
			_T("Format = %s, Pool = %s, *ppTexture = %p, pSharedHandle = %p)"), 
			Width, Height, 1, GetUsageString(0), GetFormatString(Format), 
			GetPoolString(Pool), pLeftTexture, NULL));
	if (FAILED(hResult))
		return;
	hResult = pTexture->SetPrivateData(YouTubeLeftTexture_GUID, pLeftTexture, sizeof(IUnknown *), D3DSPD_IUNKNOWN);

	CComPtr<IDirect3DTexture9> pRightTexture;
	NSCALL_TRACE1(m_Direct3DDevice.CreateTexture(Width, Height, 1, 0, Format, Pool, &pRightTexture, NULL),
		DEBUG_MESSAGE(_T("\tCreateTexture(Width = %d, Height = %d, Levels = %d, Usage = %s, ")
			_T("Format = %s, Pool = %s, *ppTexture = %p, pSharedHandle = %p)"), 
			Width, Height, 1, GetUsageString(0), GetFormatString(Format), 
			GetPoolString(Pool), pRightTexture, NULL));
	if (FAILED(hResult))
	{
		hResult = pTexture->FreePrivateData(YouTubeLeftTexture_GUID);
		return;
	}
	hResult = pTexture->SetPrivateData(YouTubeRightTexture_GUID, pRightTexture, sizeof(IUnknown *), D3DSPD_IUNKNOWN);
}

void CopyRect(D3DLOCKED_RECT* pSrcLockedRect,CONST RECT* pSourceRect,D3DLOCKED_RECT* pDestLockedRect,CONST RECT* pDestRect)
{
	char* pSrc = (char*)pSrcLockedRect->pBits + pSourceRect->top * pSrcLockedRect->Pitch + pSourceRect->left * 4;
	char* pDst = (char*)pDestLockedRect->pBits + pDestRect->top * pDestLockedRect->Pitch + pDestRect->left * 4;
	for (int i = pSourceRect->top; i < pSourceRect->bottom; i++)
	{
		memcpy(pDst, pSrc, (pSourceRect->right - pSourceRect->left) * 4);
		pSrc += pSrcLockedRect->Pitch;
		pDst += pDestLockedRect->Pitch;
	}
}

void CBaseStereoRenderer::UpdateYouTubeTexture( DWORD Stage )
{
	INSIDE;
	if (gInfo.GameSpecific != 2 || !m_Input.StereoActive)
		return;

	CComQIPtr<IDirect3DTexture9> pSrcTexture = m_PSPipelineData.m_Texture[Stage].m_pLeft;
	if (!pSrcTexture)
		return;

	DWORD size = sizeof(IUnknown *);
	CComPtr<IDirect3DTexture9> pLeftTexture;
	pSrcTexture->GetPrivateData(YouTubeLeftTexture_GUID, &pLeftTexture, &size);
	if (!pLeftTexture)
		return;

	DEBUG_TRACE2(_T("\tYouTube magic synchronization\n"));
	m_PSPipelineData.m_Texture[Stage].m_pLeft = pLeftTexture;
	CComPtr<IDirect3DTexture9> pRightTexture;
	pSrcTexture->GetPrivateData(YouTubeRightTexture_GUID, &pRightTexture, &size);
	m_PSPipelineData.m_Texture[Stage].m_pRight = pRightTexture;

	CComPtr<IDirect3DSurface9> pSrcSurface;
	pSrcTexture->GetSurfaceLevel(0, &pSrcSurface);
	CComPtr<IDirect3DSurface9> pDstLeftSurface;
	pLeftTexture->GetSurfaceLevel(0, &pDstLeftSurface);
	CComPtr<IDirect3DSurface9> pDstRightSurface;
	pRightTexture->GetSurfaceLevel(0, &pDstRightSurface);

	D3DSURFACE_DESC desc;
	pSrcSurface->GetDesc(&desc);
	int fullWidth = desc.Width;
	int fullHeight = desc.Height;
	int width = fullWidth / 2;
	if (-m_Input.GetActivePreset()->One_div_ZPS > 0.0001f)
	{
		int o = std::min(width / 2, (int) (10 * -m_Input.GetActivePreset()->One_div_ZPS));
		width -= o;
	}
	int sep = (int) SEPARATION_TO_PERCENT(m_Input.GetActivePreset()->StereoBase);
	sep = std::max(0, std::min(100, sep));
	HRESULT hResult = S_OK;
	D3DLOCKED_RECT srcLockedRect;
	D3DLOCKED_RECT dstLeftLockedRect;
	D3DLOCKED_RECT dstRightLockedRect;
	NSCALL(pSrcSurface->LockRect(&srcLockedRect, NULL, D3DLOCK_READONLY));
	NSCALL(pDstLeftSurface->LockRect(&dstLeftLockedRect, NULL, 0));
	NSCALL(pDstRightSurface->LockRect(&dstRightLockedRect, NULL, 0));
	memset(dstLeftLockedRect.pBits, 0, dstLeftLockedRect.Pitch * fullHeight); //NSCALL(m_Direct3DDevice.ColorFill(pDstLeftSurface, NULL, 0));
	memset(dstRightLockedRect.pBits, 0, dstRightLockedRect.Pitch * fullHeight); //NSCALL(m_Direct3DDevice.ColorFill(pDstRightSurface, NULL, 0));
	int xo = (fullWidth - width) / 2;
	sep = std::min(sep, std::min(xo, width));
	int center = fullWidth / 2;
	RECT srcLeftRect = { center, 0, center + width, fullHeight };
	RECT dstLeftRect = { xo + sep, 0, xo + width + sep, fullHeight };
	CopyRect(&srcLockedRect, &srcLeftRect, &dstLeftLockedRect, &dstLeftRect);
	//NSCALL(m_Direct3DDevice.StretchRect(pSrcSurface, &srcLeftRect, pDstLeftSurface, &dstLeftRect, D3DTEXF_POINT));
	RECT panelRect = { 0, fullHeight - 45, fullWidth, fullHeight };
	CopyRect(&srcLockedRect, &panelRect, &dstLeftLockedRect, &panelRect);
	//NSCALL(m_Direct3DDevice.StretchRect(pSrcSurface, &panelRect, pDstLeftSurface, &panelRect, D3DTEXF_POINT));
	RECT srcRightRect = { center - width, 0, center, fullHeight };
	RECT dstRightRect = { xo - sep, 0, xo + width - sep, fullHeight };
	CopyRect(&srcLockedRect, &srcRightRect, &dstRightLockedRect, &dstRightRect);
	//NSCALL(m_Direct3DDevice.StretchRect(pSrcSurface, &srcRightRect, pDstRightSurface, &dstRightRect, D3DTEXF_POINT));
	CopyRect(&srcLockedRect, &panelRect, &dstRightLockedRect, &panelRect);
	//NSCALL(m_Direct3DDevice.StretchRect(pSrcSurface, &panelRect, pDstRightSurface, &panelRect, D3DTEXF_POINT));
	NSCALL(pSrcSurface->UnlockRect());
	NSCALL(pDstLeftSurface->UnlockRect());
	NSCALL(pDstRightSurface->UnlockRect());

	m_PSPipelineData.m_StereoTexturesMask |= 1 << Stage;
}

void CBaseStereoRenderer::DetectDepthStencilTextureFormat()
{
	if (SUCCEEDED(m_pDirect3D->CheckDeviceFormat( m_nAdapter[0], m_DeviceType,	D3DFMT_X8R8G8B8, 
		D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_TEXTURE, FOURCC_DF16)))
		m_DepthTexturesFormat16 = FOURCC_DF16;
	else if (SUCCEEDED(m_pDirect3D->CheckDeviceFormat( m_nAdapter[0], m_DeviceType,	D3DFMT_X8R8G8B8, 
		D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_TEXTURE, FOURCC_INTZ)))
		m_DepthTexturesFormat16 = FOURCC_INTZ;
	else if (SUCCEEDED(m_pDirect3D->CheckDeviceFormat( m_nAdapter[0], m_DeviceType, D3DFMT_X8R8G8B8, 
		D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_TEXTURE, FOURCC_DF24)))
		m_DepthTexturesFormat24 = FOURCC_DF24;
	else if (SUCCEEDED(m_pDirect3D->CheckDeviceFormat( m_nAdapter[0], m_DeviceType, D3DFMT_X8R8G8B8, 
		D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_TEXTURE, FOURCC_RAWZ)))
		m_DepthTexturesFormat16 = FOURCC_RAWZ;
	else
	{
		DEBUG_TRACE3(_T("Warning: DepthStencil16 texture format not supported\n"));
		m_DepthTexturesFormat16 = D3DFMT_UNKNOWN;
		//gInfo.MonoDepthStencilTextures = true;
		m_bUseR32FDepthTextures = true;
	}

	if (SUCCEEDED(m_pDirect3D->CheckDeviceFormat( m_nAdapter[0], m_DeviceType, D3DFMT_X8R8G8B8, 
		D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_TEXTURE, FOURCC_INTZ)))
		m_DepthTexturesFormat24 = FOURCC_INTZ;
	else if (SUCCEEDED(m_pDirect3D->CheckDeviceFormat( m_nAdapter[0], m_DeviceType, D3DFMT_X8R8G8B8, 
		D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_TEXTURE, FOURCC_DF24)))
		m_DepthTexturesFormat24 = FOURCC_DF24;
	else if (SUCCEEDED(m_pDirect3D->CheckDeviceFormat( m_nAdapter[0], m_DeviceType, D3DFMT_X8R8G8B8, 
		D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_TEXTURE, FOURCC_RAWZ)))
		m_DepthTexturesFormat24 = FOURCC_RAWZ;
	else 
	{
		DEBUG_TRACE3(_T("Warning: DepthStencil24 texture format not supported\n"));
		m_DepthTexturesFormat24 = D3DFMT_UNKNOWN;
		//gInfo.MonoDepthStencilTextures = true;
		m_bUseR32FDepthTextures = true;
	}
}

void CBaseStereoRenderer::OneToOneRender( IDirect3DTexture9* pTex, CONST RECT* pTexRect, CONST SIZE* pOffset )
{
	INSIDE;
	HRESULT hResult = S_OK;
	D3DTVERTEX_1T vertex[4];
	vertex[0].z   = 0;
	vertex[0].rhw = 1;
	vertex[1].z   = 0;
	vertex[1].rhw = 1;
	vertex[2].z   = 0;
	vertex[2].rhw = 1;
	vertex[3].z   = 0;
	vertex[3].rhw = 1;
	NSCALL(m_Direct3DDevice.SetTexture(0, pTex));
	D3DSURFACE_DESC Desc;
	RECT NarrowRect;
	NSCALL(pTex->GetLevelDesc(0, &Desc));
	SetRect(&NarrowRect, 0, 0, Desc.Width, Desc.Height);
	RECT TexRect;
	if (pTexRect)
		TexRect = *pTexRect;
	else
		TexRect = NarrowRect;
	SIZE Offset = {0, 0};
	if (pOffset) 
		Offset = *pOffset;

	vertex[0].x   = NarrowRect.left - .5f + Offset.cx;
	vertex[0].y   = NarrowRect.top - .5f + Offset.cy;
	vertex[0].tu  = 1.0f * TexRect.left / Desc.Width;
	vertex[0].tv  = 1.0f * TexRect.top / Desc.Height;    

	vertex[1].x   = NarrowRect.right - .5f + Offset.cx;
	vertex[1].y   = NarrowRect.top - .5f + Offset.cy;
	vertex[1].tu  = 1.0f * TexRect.right / Desc.Width;    
	vertex[1].tv  = 1.0f * TexRect.top / Desc.Height;    

	vertex[2].x   = NarrowRect.right - .5f + Offset.cx;
	vertex[2].y   = NarrowRect.bottom - .5f + Offset.cy;
	vertex[2].tu  = 1.0f * TexRect.right / Desc.Width;    
	vertex[2].tv  = 1.0f * TexRect.bottom / Desc.Height;    

	vertex[3].x   = NarrowRect.left - .5f + Offset.cx;
	vertex[3].y   = NarrowRect.bottom - .5f + Offset.cy;
	vertex[3].tu  = 1.0f * TexRect.left / Desc.Width;    
	vertex[3].tv  = 1.0f * TexRect.bottom / Desc.Height; 
	NSCALL(m_Direct3DDevice.DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertex, sizeof(vertex[0])));
}

