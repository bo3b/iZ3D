#pragma once

#include "Command.h"
#include <memory.h>

namespace Commands
{

	class CalcDeferredContextHandleSize11_0 : public CommandWithAllocator<CalcDeferredContextHandleSize11_0>
	{
	public:

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	WriteToFileSimple( D3DDeviceWrapper *pWrapper ) const
		{	return WriteToFile(pWrapper);	}
		virtual bool	ReadFromFile	();

		CalcDeferredContextHandleSize11_0 ()		
		{
			CommandId = idCalcDeferredContextHandleSize11_0;
		}

		CalcDeferredContextHandleSize11_0( D3D11DDI_HANDLETYPE  HandleType, VOID*  pICObject )
		{
			CommandId = idCalcDeferredContextHandleSize11_0;
			HandleType_ = HandleType;
			pICObject_ = pICObject;
		}

	public:

		D3D11DDI_HANDLETYPE					HandleType_;
		VOID*								pICObject_;
		SIZE_T								RetValue_;
	};

}

SIZE_T ( APIENTRY CalcDeferredContextHandleSize11_0 )( D3D10DDI_HDEVICE  hDevice, 
													  D3D11DDI_HANDLETYPE  HandleType, 
													  VOID*  pICObject );
