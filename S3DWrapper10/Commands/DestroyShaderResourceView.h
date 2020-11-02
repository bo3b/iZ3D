#pragma once

#include "Command.h"

namespace Commands
{

	class DestroyShaderResourceView : public CommandWithAllocator<DestroyShaderResourceView>
	{
	public:
		EMPTY_OUTPUT();

		DestroyShaderResourceView()		
		{
			CommandId = idDestroyShaderResourceView;
		}

		DestroyShaderResourceView( D3D10DDI_HSHADERRESOURCEVIEW hShaderResourceView )
		{
			CommandId = idDestroyShaderResourceView;
			hShaderResourceView_ = hShaderResourceView;
		}

		virtual	void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual	bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	WriteToFileSimple( D3DDeviceWrapper *pWrapper ) const
		{	return WriteToFile(pWrapper);	}
		virtual bool	ReadFromFile	();

		D3D10DDI_HSHADERRESOURCEVIEW hShaderResourceView_;
	};

}

extern VOID ( APIENTRY DestroyShaderResourceView )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HSHADERRESOURCEVIEW hShaderResourceView );
