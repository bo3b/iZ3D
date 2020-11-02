/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include <vector>
#include "OutputMethod_dx9.h"

class TiXmlHandle;
class TiXmlElement;

namespace DX9Output
{

	class S3DShutterOutput :
		public OutputMethod
	{
	public:
		S3DShutterOutput(DWORD mode, DWORD spanMode);
		virtual ~S3DShutterOutput(void);

		virtual HRESULT	Initialize( IDirect3DDevice9* dev, bool MultiPass = false);
		virtual HRESULT Output(bool bLeft, CBaseSwapChain* pSwapChain, RECT* pDestRect);
		virtual void	Clear();

	protected:
		UINT						m_MarkerIndex;
		UINT						m_NumberOfMarks;
		UINT						m_NumberOfMarkedLines;
		UINT						m_MarkerSurfaceWidth;
		UINT						m_InverseAxisYDirection;
		UINT						m_ScaleToFrame;
		float						m_ScaleFactor;
		CComPtr<IDirect3DSurface9>	m_pMarkerSurface;

		void ParseMarkLine( TiXmlElement* element, std::vector<std::vector<DWORD> >& MarkArray );
		void ParseVersion_0_2(TiXmlHandle& frameHandle, const char* &format, std::vector< std::vector<DWORD> > &LeftMark, std::vector< std::vector<DWORD> > &RightMark);
		void ParseVersion_0_3(TiXmlHandle& frameHandle);
	};	
}
