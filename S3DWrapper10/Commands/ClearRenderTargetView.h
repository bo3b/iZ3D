#pragma once

#include "Command.h"
#include <memory.h>

namespace Commands
{

	class ClearRenderTargetView : public CommandWithAllocator<ClearRenderTargetView>
	{
	public:
		ClearRenderTargetView()
		:	bIsUsedStereoResources_(false)
		{
			CommandId = idClearRenderTargetView;
			State_	= COMMAND_MAY_USE_STEREO_RESOURCES;
			pColorRGBA_[0] = 0.0f;
			pColorRGBA_[1] = 0.0f;
			pColorRGBA_[2] = 0.0f;
			pColorRGBA_[3] = 0.0f;
		}

		ClearRenderTargetView(D3D10DDI_HRENDERTARGETVIEW  hRenderTargetView, float  pColorRGBA[4])
		:	bIsUsedStereoResources_(false)
		{    
			CommandId = idClearRenderTargetView;
			State_	= COMMAND_MAY_USE_STEREO_RESOURCES;
			hRenderTargetView_ = hRenderTargetView;
			memcpy(pColorRGBA_, pColorRGBA, sizeof(float) * 4);
		}

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	ReadFromFile	();

		void BuildCommand(CDumpState *ds);

		virtual bool	IsUsedStereoResources( D3DDeviceWrapper* /*pWrapper*/ ) const;
		virtual void	SetDestResourceType( TextureType type );
		virtual void	GetDestResources(DestResourcesSet &res);

	public:

		bool bIsUsedStereoResources_;
		D3D10DDI_HRENDERTARGETVIEW  hRenderTargetView_;
		float  pColorRGBA_[4];

		DEPENDS_ON(hRenderTargetView_);
	};

}

VOID ( APIENTRY ClearRenderTargetView )( D3D10DDI_HDEVICE  hDevice, D3D10DDI_HRENDERTARGETVIEW  hRenderTargetView, 
										FLOAT  pColorRGBA[4] );
