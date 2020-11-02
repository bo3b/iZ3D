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
#include "BaseStereoRenderer.h"
#include "Trace.h"
#include "CommandDumper.h"

#pragma optimize( "y", off ) // for Steam Community

STDMETHODIMP CBaseStereoRenderer::Present( CONST RECT * pSourceRect, CONST RECT * pDestRect, HWND hDestWindowOverride, 
										  CONST RGNDATA * pDirtyRegion ) 
{
	DUMP_CMD(Present,pSourceRect,pDestRect,hDestWindowOverride,pDirtyRegion);

	DEBUG_TRACE2(_T("%s\n"), _T( __FUNCTION__ ) );
	return GetBaseSC()->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion, 0);
}

STDMETHODIMP CBaseStereoRenderer::PresentEx(CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,
											CONST RGNDATA* pDirtyRegion,DWORD dwFlags)
{
	DUMP_CMD(PresentEx,pSourceRect,pDestRect,hDestWindowOverride,pDirtyRegion,dwFlags);

	DEBUG_TRACE2(_T("%s\n"), _T( __FUNCTION__ ) );
	return GetBaseSC()->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion, dwFlags);
}

STDMETHODIMP CBaseStereoRenderer::Reset(D3DPRESENT_PARAMETERS * pPresentationParameters ) 
{
	DUMP_CMD(Reset,pPresentationParameters);

	DEBUG_MESSAGE(_T("Entering %s\n"), _T( __FUNCTION__ ) );
	return DoReset(pPresentationParameters, NULL, false);
}

STDMETHODIMP CBaseStereoRenderer::ResetEx(D3DPRESENT_PARAMETERS* pPresentationParameters, D3DDISPLAYMODEEX *pFullscreenDisplayMode)
{
	DUMP_CMD(ResetEx,pPresentationParameters,pFullscreenDisplayMode);

	DEBUG_MESSAGE(_T("Entering %s\n"), _T( __FUNCTION__ ) );
	return DoReset(pPresentationParameters, pFullscreenDisplayMode, true);
}

STDMETHODIMP CBaseStereoRenderer::SetRenderState(D3DRENDERSTATETYPE State, DWORD Value) 
{
	DUMP_CMD_STATE(SetRenderState,State,Value);

	HRESULT hResult;
	if(!m_bInStateBlock)
	{
#ifndef FINAL_RELEASE
		switch(State)
		{
		case D3DRS_FOGSTART: 
		case D3DRS_FOGEND: 
		case D3DRS_FOGDENSITY: 
		case D3DRS_POINTSIZE_MIN: 
		case D3DRS_POINTSCALE_A: 
		case D3DRS_POINTSCALE_B: 
		case D3DRS_POINTSCALE_C: 
		case D3DRS_POINTSIZE_MAX: 
		case D3DRS_TWEENFACTOR: 
		case D3DRS_MINTESSELLATIONLEVEL: 
		case D3DRS_MAXTESSELLATIONLEVEL: 
			NSCALL_TRACE2(m_Direct3DDevice.SetRenderState(State, Value),
				DEBUG_MESSAGE(_T("SetRenderState(State = %s, Value = %f)"),
				GetRenderStateString(State), *reinterpret_cast<float*>(&Value)));
			break;
		case D3DRS_POINTSIZE: 
		case D3DRS_ADAPTIVETESS_X: 
		case D3DRS_ADAPTIVETESS_Y: 
		case D3DRS_ADAPTIVETESS_Z: 
		case D3DRS_ADAPTIVETESS_W:
			// D3D Hacks:
			// D3DRS_POINTSIZE A2M0, A2M1 - ATI Alpha to coverage http://developer.amd.com/gpu_assets/Advanced%20DX9%20Capabilities%20for%20ATI%20Radeon%20Cards_v2.pdf
			// D3DRS_POINTSIZE RESZ_CODE(0x7fa05000 ~ 1.#QNAN00) - ATI Multisampled Depth Buffer Resolve http://developer.amd.com/gpu_assets/Advanced%20DX9%20Capabilities%20for%20ATI%20Radeon%20Cards_v2.pdf
			// D3DRS_POINTSIZE - ATI R2VB http://developer.amd.com/media/gpu_assets/R2VB_programming.pdf
			// D3DRS_POINTSIZE INST - ATI Hardware Instancing http://zeuxcg.blogspot.com/2007/09/particle-rendering-revisited.html
			// D3DRS_ADAPTIVETESS_Y ATOC - NVidia Transparency Multisampling http://developer.amd.com/gpu_assets/Advanced%20DX9%20Capabilities%20for%20ATI%20Radeon%20Cards_v2.pdf
			// D3DRS_ADAPTIVETESS_Y SSAA - NVidia SSAA http://developer.amd.com/gpu_assets/Advanced%20DX9%20Capabilities%20for%20ATI%20Radeon%20Cards_v2.pdf
			// D3DRS_ADAPTIVETESS_X NVDB - NVidia Depth Bounds Test (Min D3DRS_ADAPTIVETESS_Z, Max D3DRS_ADAPTIVETESS_W) http://developer.download.nvidia.com/GPU_Programming_Guide/GPU_Programming_Guide_G80.pdf
			NSCALL_TRACE2(m_Direct3DDevice.SetRenderState(State, Value),
				DEBUG_MESSAGE(_T("SetRenderState(State = %s, Value = %f (\'%c%c%c%c\'))"),
				GetRenderStateString(State), *reinterpret_cast<float*>(&Value),
				(TCHAR)(Value & 0xFF), (TCHAR)((Value >> 8) & 0xFF), 
				(TCHAR)((Value >> 16) & 0xFF), (TCHAR)((Value >> 24) & 0xFF)));
			break;
		default:
#endif
			NSCALL_TRACE2(m_Direct3DDevice.SetRenderState(State, Value),
				DEBUG_MESSAGE(_T("SetRenderState(State = %s, Value = %08x)"),
				GetRenderStateString(State), Value));
#ifndef FINAL_RELEASE
			break;
		}
#endif
		switch(State)
		{
		case D3DRS_COLORWRITEENABLE:
			m_dwColorWriteEnable[0] = Value;
			break;
		case D3DRS_COLORWRITEENABLE1:
			m_dwColorWriteEnable[1] = Value;
			break;
		case D3DRS_COLORWRITEENABLE2:
			m_dwColorWriteEnable[2] = Value;
			break;
		case D3DRS_COLORWRITEENABLE3:
			m_dwColorWriteEnable[3] = Value;
			break;
		case D3DRS_ZWRITEENABLE:
			m_dwZWriteEnable = Value;
			break;
		case D3DRS_STENCILENABLE:
			m_dwStencilEnable = Value;
			break;
		case D3DRS_SCISSORTESTENABLE:
			m_dwScissorTestEnable = Value;
			break;
		}
	}
	else
		hResult = m_Direct3DDevice.SetRenderState(State, Value);
	return hResult;
}
