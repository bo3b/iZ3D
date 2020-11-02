#pragma once

#include <d3d10umddi.h>
#include "ShaderInstruction.h"
namespace shader_analyzer
{
	class IsShaderTreeStart{
	public:
		IsShaderTreeStart( const D3D10_SB_OPERAND_TYPE opType, const unsigned index );

		bool operator () (const shader_analyzer::ShaderInstruction& inst);
	private:
		const D3D10_SB_OPERAND_TYPE opType_;
		const unsigned index_;
};

}