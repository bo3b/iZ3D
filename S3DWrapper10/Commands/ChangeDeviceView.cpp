#include "stdafx.h"
#include "ChangeDeviceView.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void ChangeDeviceView::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		CurrentView_ = NewView_;
		AFTER_EXECUTE(this); 
	}

	bool ChangeDeviceView::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "ChangeDeviceView" );
		WriteStreamer::Value( "NewView", NewView_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool ChangeDeviceView::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Value( NewView_ );
		ReadStreamer::CmdEnd();
		return true;
	}
	
	void ChangeDeviceView::Dump(std::ostream& os) const
	{
		os << "ChangeDeviceView(NewView_ = " << EnumToString(NewView_) << ")";
	}

}
