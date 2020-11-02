#pragma once

#include "Command.h"

namespace Commands
{

	class OpenResource : public CommandWithAllocator<OpenResource>
	{
	public:
		OpenResource()
		{
			CommandId = idOpenResource;
		}

		OpenResource( CONST D3D10DDIARG_OPENRESOURCE*  pOpenResource, D3D10DDI_HRESOURCE  hResource, D3D10DDI_HRTRESOURCE  hRTResource )
		{
			CommandId = idOpenResource;
			OpenResource_ = *pOpenResource;
			hResource_ = hResource;
			hRTResource_ = hRTResource;
		}

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	WriteToFileSimple( D3DDeviceWrapper *pWrapper ) const 
		{	return WriteToFile(pWrapper);	}
		virtual bool	ReadFromFile	();

		void BuildCommand(CDumpState *ds);

	public:

		D3D10DDIARG_OPENRESOURCE  OpenResource_;
		D3D10DDI_HRESOURCE  hResource_;
		D3D10DDI_HRTRESOURCE  hRTResource_;
#ifndef FINAL_RELEASE
		HRESULT	RetValue_;
#endif
	};

}

extern VOID ( APIENTRY OpenResource )( D3D10DDI_HDEVICE  hDevice, CONST D3D10DDIARG_OPENRESOURCE*  pOpenResource, 
									  D3D10DDI_HRESOURCE  hResource, D3D10DDI_HRTRESOURCE  hRTResource );
