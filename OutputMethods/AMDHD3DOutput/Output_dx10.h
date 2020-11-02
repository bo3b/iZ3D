#pragma once

#include "PostprocessedOutput_dx10.h"

namespace DX10Output
{

	class AMDHD3DOutput :
		public OutputMethod
	{
	public:
								AMDHD3DOutput			( DWORD nMode_, DWORD nSpanMode_ );
		virtual					~AMDHD3DOutput			();

		virtual void			Output					( bool bLeft_, D3DSwapChain* pSwapChain_, RECT* pDestRect_ );\

		virtual int				GetPreferredRefreshRate	() const;
	};

}