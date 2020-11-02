#include "stdafx.h"
#include "ChangeShaderMatrixView.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"
#include "ShaderWrapper.h"

namespace Commands
{

	void ChangeShaderMatrixView::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		for (int i = 0; i<SP_COUNT; ++i)
		{
			CurrentShaderData_[i]		  = 0;
			CurrentPixelShaderData_[i]	  = 0;
			CurrentModifiedShaderData_[i] = 0;
		}

		CurrentShaderCRC32_ = ShaderCRC32_;
		if (StereoShader_ != SP_NONE)
		{
			CurrentShaderData_[StereoShader_]		  = pData_;
			CurrentModifiedShaderData_[StereoShader_] = pModifiedShaderData_;
			CurrentPixelShaderData_[StereoShader_]	  = pPixelData_;
		}
		AFTER_EXECUTE(this); 
	}

	bool ChangeShaderMatrixView::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "ChangeShaderMatrixView" );
		WriteStreamer::Value( "StereoShader", StereoShader_ );
		WriteStreamer::Value( "pMatrices", *((DWORD*)&pData_) );
		//WriteStreamer::Value( "ModifiedShaderAvailable", pModifiedShaderData_ ? pModifiedShaderData_->ModifiedShaderAvailable : false );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool ChangeShaderMatrixView::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Value( StereoShader_ );
		ReadStreamer::Skip();
		//ReadStreamer::Skip();
		ReadStreamer::CmdEnd();
		return true;
	}

}
