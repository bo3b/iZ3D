#pragma once

#include "Command.h"

namespace Commands
{

	class ChangeShaderMatrixView : public CommandWithAllocator<ChangeShaderMatrixView>
	{
	public:
		ChangeShaderMatrixView ()		
		{
			CommandId				= idChangeShaderMatrixView;
			StereoShader_			= SP_NONE;
			ShaderCRC32_			= 0;
			pData_					= 0;
			pModifiedShaderData_	= 0;
			pPixelData_				= 0;
		}

		ChangeShaderMatrixView( SHADER_PIPELINE NewStereoShader,
								DWORD ShaderCRC32,
								const ProjectionShaderData* pNewData, 
								const ModifiedShaderData* pModifiedShaderData,
								const UnprojectionShaderData* pPixelData)
		{
			CommandId				= idChangeShaderMatrixView;
			StereoShader_			= NewStereoShader;
			ShaderCRC32_			= ShaderCRC32;
			pData_					= pNewData;
			pModifiedShaderData_	= pModifiedShaderData;
			pPixelData_				= pPixelData;
		}

		bool operator == (const ChangeShaderMatrixView& rhs) const
		{
			return (StereoShader_ == rhs.StereoShader_) && (pData_ == rhs.pData_) && (pPixelData_ == rhs.pPixelData_) && (pModifiedShaderData_ == rhs.pModifiedShaderData_);
		}

		bool operator != (const ChangeShaderMatrixView& rhs) const
		{
			return !(*this == rhs);
		}

		virtual void Execute		( D3DDeviceWrapper *pWrapper );
		virtual	bool WriteToFile	( D3DDeviceWrapper *pWrapper ) const;
		virtual bool ReadFromFile	();

	public:
		SHADER_PIPELINE					StereoShader_;
		DWORD							ShaderCRC32_; 
		const ProjectionShaderData*		pData_;
		const UnprojectionShaderData*	pPixelData_;
		const ModifiedShaderData*		pModifiedShaderData_;
	};

}
