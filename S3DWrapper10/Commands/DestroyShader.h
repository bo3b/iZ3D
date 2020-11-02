#pragma once

#include "Command.h"

namespace Commands
{

	class DestroyShader : public CommandWithAllocator<DestroyShader>
	{
	public:
		EMPTY_OUTPUT();

		DestroyShader()		
		{
			CommandId = idDestroyShader;
		}

		DestroyShader( D3D10DDI_HSHADER hShader )
		{
			CommandId = idDestroyShader;
			hShader_ = hShader;
		}

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	WriteToFileSimple( D3DDeviceWrapper *pWrapper ) const
		{	return WriteToFile(pWrapper);	}
		virtual bool	ReadFromFile	();

		D3D10DDI_HSHADER hShader_;
	};

}

extern VOID ( APIENTRY DestroyShader )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HSHADER hShader );
