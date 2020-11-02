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
#include "Output_dx9.h"
#include "S3DWrapper9\BaseSwapChain.h"
#include <tinyxml.h>
#include "ProductNames.h"
#include "..\CommonUtils\StringUtils.h"
#include "..\CommonUtils\CommonResourceFolders.h"

using namespace DX9Output;

using namespace std;

#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain( HMODULE hModule,
					   DWORD  ul_reason_for_call,
					   LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

OUTPUT_API void* CALLBACK CreateOutputDX9(DWORD mode, DWORD spanMode)
{
	return new S3DShutterOutput(mode, spanMode);
}

OUTPUT_API DWORD CALLBACK GetOutputCaps()
{
	return odShutterModeSimpleOnly;
}

OUTPUT_API void CALLBACK GetOutputName(char* name, DWORD size)
{
	strcpy_s(name, size, "Shutter mode");
}

OUTPUT_API BOOL CALLBACK EnumOutputModes(DWORD num, char* name, DWORD size)
{
	switch(num)
	{
	case 0:
		strcpy_s(name, size, "Marked shutter");
		return TRUE;
	default:
		return FALSE;
	}
}

S3DShutterOutput::S3DShutterOutput(DWORD mode, DWORD spanMode)
: OutputMethod(mode, spanMode)
{
}

S3DShutterOutput::~S3DShutterOutput(void)
{
}

HRESULT S3DShutterOutput::Initialize( IDirect3DDevice9* dev, bool MultiPass /*= false*/ )
{
	HRESULT hResult = OutputMethod::Initialize(dev, MultiPass);
	if(SUCCEEDED(hResult))
	{
		const char* specVersion;
		const char* format = "";
		m_MarkerIndex = 0;
		m_MarkerSurfaceWidth = 0;
		m_NumberOfMarkedLines = 0;
		m_NumberOfMarks = 0;
		m_InverseAxisYDirection = 0;
		m_ScaleToFrame = 0;
		m_ScaleFactor = .75;
		vector< vector<DWORD> > LeftMark, RightMark;
		TCHAR XMLFileName[MAX_PATH];

		hResult = E_FAIL;
		if(iz3d::resources::GetAllUsersDirectory( XMLFileName ))
		{
			PathAppend(XMLFileName, _T("MarkingSpec.xml") );
			TiXmlDocument MarkFile( common::utils::to_multibyte( XMLFileName ).c_str() );
			if(MarkFile.LoadFile())
			{
				TiXmlHandle   docHandle( &MarkFile );
				TiXmlHandle   frameHandle = docHandle.FirstChild("FrameMarkingSpec");
				TiXmlElement* element = frameHandle.Element();
				if(element)
				{
					specVersion = element->Attribute("version");
					if(strcmp(specVersion,"0.2") == 0)
					{
						ParseVersion_0_2(frameHandle, format, LeftMark, RightMark);
						hResult = S_OK;
					}
					if(strcmp(specVersion,"0.3") == 0)
					{
						ParseVersion_0_2(frameHandle, format, LeftMark, RightMark);
						ParseVersion_0_3(frameHandle);
						hResult = S_OK;
					}
				}
			}
		}

		//--- if some bug found while reading XML file - draw red line on top screen line ---
		if(FAILED(hResult))
		{
			m_NumberOfMarkedLines = 16;
			m_NumberOfMarks = 1;
			m_InverseAxisYDirection = 0;
			m_ScaleToFrame = 1;
			m_ScaleFactor = 1;
			LeftMark.clear();  LeftMark.push_back(vector<DWORD>(1, 0xFF0000));
			RightMark.clear(); RightMark.push_back(vector<DWORD>(1, 0xFF0000));
			m_MarkerSurfaceWidth = (UINT)LeftMark[0].size();
		}

		if(SUCCEEDED(dev->CreateOffscreenPlainSurface(m_MarkerSurfaceWidth, m_NumberOfMarks * 2, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &m_pMarkerSurface, NULL)))
		{
			D3DLOCKED_RECT rect;
			m_pMarkerSurface->LockRect(&rect, NULL, D3DLOCK_DISCARD);
			char* p1 = (char*)rect.pBits;
			char* p2 = p1 + rect.Pitch * m_NumberOfMarks;
			for(UINT i=0; i< m_NumberOfMarks; i++)
			{
				memcpy(p1,  &LeftMark[i][0], m_MarkerSurfaceWidth * sizeof(DWORD));
				memcpy(p2, &RightMark[i][0], m_MarkerSurfaceWidth * sizeof(DWORD));
				p1 += rect.Pitch;
				p2 += rect.Pitch;
			}
			m_pMarkerSurface->UnlockRect();
		}
	}
	return hResult;
}

void S3DShutterOutput::ParseMarkLine( TiXmlElement* element, vector<vector<DWORD> >& MarkArray)
{
	for(int index = 0; element; index++, element = element->NextSiblingElement())
	{
		MarkArray.push_back(vector<DWORD>());
		if(const char* s = element->GetText())
		{
			const char* t = s;
			UINT length = (UINT)strlen(s);
			while(t < s + length)
			{
				DWORD color;
				sscanf(t, "%x", &color); t+= 7;
				MarkArray[index].push_back(color);  
			}
		}
	}
}

void S3DShutterOutput::ParseVersion_0_2(TiXmlHandle& frameHandle, const char* &format, vector< vector<DWORD> > &LeftMark, vector< vector<DWORD> > &RightMark)
{
	TiXmlElement* element = frameHandle.Element();
	element->QueryIntAttribute("NumberOfMarkedLines", (int*)&m_NumberOfMarkedLines);
	TiXmlHandle lineHandle = frameHandle.FirstChild();
	element = lineHandle.Element();
	if(element)
	{
		format = element->Attribute("ColorFormat");
		element->QueryIntAttribute("NumberOfMarks", (int*)&m_NumberOfMarks);
	}
	if(strcmpi(format, "r8g8b8_hex") == 0)
	{
		element = lineHandle.Child("LeftFrameMarks", 0).FirstChild().Element();
		ParseMarkLine(element, LeftMark);
		element = lineHandle.Child("RightFrameMarks", 0).FirstChild().Element();
		ParseMarkLine(element, RightMark);
		m_MarkerSurfaceWidth = (UINT)LeftMark[0].size();
	}
}

void S3DShutterOutput::ParseVersion_0_3(TiXmlHandle& frameHandle)
{
	TiXmlHandle lineHandle = frameHandle.FirstChild();
	TiXmlElement* element = lineHandle.Element();
	if(element)
	{
		element->QueryIntAttribute("InverseAxisYDirection", (int*)&m_InverseAxisYDirection);
		element->QueryIntAttribute("ScaleToFrame", (int*)&m_ScaleToFrame);
		element->QueryFloatAttribute("ScaleFactor", (float*)&m_ScaleFactor);
	}
}

void S3DShutterOutput::Clear()
{
	OutputMethod::Clear();
	m_pMarkerSurface.Release();
}

HRESULT S3DShutterOutput::Output(bool bLeft, CBaseSwapChain* pSwapChain, RECT* pDestRect)
{
	HRESULT hResult = S_OK;
	IDirect3DSurface9* view = pSwapChain->GetViewRT(bLeft);
	RECT* pViewRect = pSwapChain->GetViewRect(bLeft);
	IDirect3DSurface9* primary = pSwapChain->GetPresenterBackBuffer();
	IDirect3DSurface9* secondary = pSwapChain->m_pSecondaryBackBuffer;

	SIZE bbSize = pSwapChain->m_BackBufferSize;
	
	RECT MarkRectSrc, MarkRectDst;
	int width = min(bbSize.cx, (int)m_MarkerSurfaceWidth);
	if (bLeft)
		SetRect(&MarkRectSrc,  0, m_MarkerIndex, width, m_MarkerIndex + 1);
	else
		SetRect(&MarkRectSrc, 0, m_MarkerIndex + m_NumberOfMarks, width, m_MarkerIndex + m_NumberOfMarks + 1);
	int destSizeX = width, destStartY = 0;
	if(m_InverseAxisYDirection)
		destStartY = bbSize.cy - m_NumberOfMarkedLines;
	if(m_ScaleToFrame)
		destSizeX = (int)(m_ScaleFactor * bbSize.cx);
	SetRect(&MarkRectDst,  0,  destStartY, destSizeX, destStartY + m_NumberOfMarkedLines);
	if (pDestRect)
	{
		MarkRectDst.left	+= pDestRect->left;
		MarkRectDst.top		+= pDestRect->top;
		MarkRectDst.right	+= pDestRect->left;
		MarkRectDst.bottom	+= pDestRect->top;
	}
	m_MarkerIndex = (++m_MarkerIndex) % m_NumberOfMarks;

	NSCALL(m_pd3dDevice->StretchRect(view, pViewRect, primary, pDestRect, D3DTEXF_NONE));
	NSCALL(m_pd3dDevice->StretchRect(m_pMarkerSurface, &MarkRectSrc, primary, &MarkRectDst, D3DTEXF_POINT));

	return hResult;
}