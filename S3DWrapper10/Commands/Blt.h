#pragma once

#include "Command.h"

namespace Commands
{

	class Blt : public CommandWithAllocator<Blt>
	{
	public:
		static bool IsPresentBlt;

	public:
		virtual void	Execute( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	ReadFromFile();		

		Blt ()		
			:	pBltData_ ( NULL )
		{
			CommandId = idBlt;
		}

		Blt( DXGI_DDI_HDEVICE hDevice, DXGI_DDI_ARG_BLT*  pBltData )
		{
			CommandId = idBlt;
			pBltData_ = pBltData;
			//MEMCOPY(&BltData_, pBltData, sizeof(DXGI_DDI_ARG_BLT));
			//BltData_.hDevice = hDevice;
		}
	public:
		DXGI_DDI_ARG_BLT*	pBltData_;
		//DXGI_DDI_ARG_BLT  BltData_;
		HRESULT				RetValue_;
	};

}

extern HRESULT ( APIENTRY  Blt )  (DXGI_DDI_ARG_BLT* pBltData);
