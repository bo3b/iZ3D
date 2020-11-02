#pragma once

#include "OutputMethod_dx10.h"

class TiXmlHandle;
class TiXmlElement;

namespace DX10Output
{

	class S3DShutterOutput :
		public OutputMethod
	{
	public:
													S3DShutterOutput(DWORD mode, DWORD spanMode);
													virtual ~S3DShutterOutput();

		virtual void								Output					( bool bLeft_, D3DSwapChain* pSwapChain_, RECT* pDestRect_ );
		virtual void								Initialize				( D3DDeviceWrapper* pD3DWrapper_ );
		virtual void								Clear					( D3DDeviceWrapper* pD3DWrapper_ );


	private:
		UINT										m_MarkerIndex;
		UINT										m_NumberOfMarks;
		UINT										m_NumberOfMarkedLines;
		UINT										m_MarkerSurfaceWidth;
		UINT										m_InverseAxisYDirection;
		UINT										m_ScaleToFrame;
		float										m_ScaleFactor;

		std::vector<D3D10DDI_HRESOURCE>				m_hLeftMarkedSurfaces;
		std::vector<D3D10DDI_HRESOURCE>				m_hRightMarkedSurfaces;		
		
		void										ParseMarkLine			( TiXmlElement* element, std::vector<std::vector<DWORD> >& MarkArray );
		void										ParseVersion_0_2		( TiXmlHandle& frameHandle, const char* &format, std::vector< std::vector<DWORD> > &LeftMark, std::vector< std::vector<DWORD> > &RightMark );
		void										ParseVersion_0_3		( TiXmlHandle& frameHandle );
		HRESULT 									CreateMarkedSurface		( D3DDeviceWrapper* pD3DWrapper_, UINT nTexWidth_,UINT nTexHeight_, UINT nBitDepth_, UINT nRowBytes_, 
																				VOID* pData_, D3D10DDI_HRESOURCE& hResource_  );
	};	
}
