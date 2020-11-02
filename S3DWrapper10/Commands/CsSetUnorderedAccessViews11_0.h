#pragma once
#include "Command.h"

namespace Commands
{
	class CsSetUnorderedAccessViews11_0 : public CommandWithAllocator<CsSetUnorderedAccessViews11_0, Command>
	{
	public:
		CsSetUnorderedAccessViews11_0()
		:	Offset_(0)
		,	NumViews_(0)
		,	bNeedCheckStereoResources_(false)
		,	bIsStereo_(false)
		{
			CommandId = idCsSetUnorderedAccessViews11_0;
			State_	= COMMAND_CHANGE_DEVICE_STATE;
		}

		CsSetUnorderedAccessViews11_0( UINT									Offset, 
									   UINT									NumViews, 
									   CONST D3D11DDI_HUNORDEREDACCESSVIEW* phUnorderedAccessView, 
									   CONST UINT*							phSizes )
		:	Offset_(Offset)
		,	NumViews_(NumViews)
		,	phUnorderedAccessView_(phUnorderedAccessView, phUnorderedAccessView + NumViews)
		,	phTempUnorderedAccessView_(NumViews)
		,	pUAVInitialCounts_(phSizes, phSizes + NumViews)
		,	bNeedCheckStereoResources_(true)
		,	bIsStereo_(false)
		{
			CommandId = idCsSetUnorderedAccessViews11_0;
			State_	= COMMAND_CHANGE_DEVICE_STATE;
		}

		virtual void Init()
		{
			Offset_ = 0;
			NumViews_ = 0;
			bNeedCheckStereoResources_ = false;
			bIsStereo_ = false;
			phUnorderedAccessView_.clear();
			pUAVInitialCounts_.clear();
		}

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	ReadFromFile	();

		virtual void	UpdateDeviceState( D3DDeviceWrapper *pWrapper, D3DDeviceState* pState );
		virtual bool	IsUsedStereoResources( D3DDeviceWrapper* pWrapper ) const override;

		void BuildCommand(CDumpState *ds);

		TCsSetUnorderedAccessViewsCmdPtr Clone() const;

	public:
		UINT												Offset_;
		UINT												NumViews_;
		fast_vector<D3D11DDI_HUNORDEREDACCESSVIEW>::type	phUnorderedAccessView_;
		fast_vector<D3D11DDI_HUNORDEREDACCESSVIEW>::type	phTempUnorderedAccessView_;
		fast_vector<UINT>::type								pUAVInitialCounts_;
		
		DEPENDS_ON_VECTOR(phUnorderedAccessView_);

	private:
		mutable bool	bNeedCheckStereoResources_;
		mutable bool	bIsStereo_;
	};

}

extern VOID ( APIENTRY CsSetUnorderedAccessViews11_0 )( D3D10DDI_HDEVICE  hDevice, UINT  Offset, UINT  NumViews,
								CONST D3D11DDI_HUNORDEREDACCESSVIEW*  phUnorderedAccessView, CONST UINT*  phSizes );
