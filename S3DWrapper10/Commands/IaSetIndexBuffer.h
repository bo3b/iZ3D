#pragma once

#include "Command.h"

namespace Commands
{

	class IaSetIndexBuffer : public CommandWithAllocator<IaSetIndexBuffer>
	{
	public:
		IaSetIndexBuffer()		
		{
			CommandId = idIaSetIndexBuffer;
			State_	= COMMAND_CHANGE_DEVICE_STATE;
		}

		IaSetIndexBuffer( D3D10DDI_HRESOURCE  hBuffer, DXGI_FORMAT  Format, UINT  Offset )
		{
			CommandId = idIaSetIndexBuffer;
			State_	= COMMAND_CHANGE_DEVICE_STATE;
			hBuffer_ = hBuffer;
			Format_ = Format;
			Offset_ = Offset;
		}

		virtual void Init()
		{
			hBuffer_ = MAKE_D3D10DDI_HRESOURCE(NULL);
			Format_ = DXGI_FORMAT_UNKNOWN;
			Offset_ = 0;
		}

		virtual void	Execute( D3DDeviceWrapper *pWrapper );

		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	ReadFromFile	();
		virtual void	UpdateDeviceState( D3DDeviceWrapper *pWrapper, D3DDeviceState* pState );

		void BuildCommand(CDumpState *ds);

	public:

		D3D10DDI_HRESOURCE  hBuffer_;
		DXGI_FORMAT  Format_;
		UINT  Offset_;

		DEPENDS_ON(hBuffer_);
	};

}

extern VOID ( APIENTRY IaSetIndexBuffer )( D3D10DDI_HDEVICE  hDevice, D3D10DDI_HRESOURCE  hBuffer, 
										  DXGI_FORMAT  Format, UINT  Offset );
