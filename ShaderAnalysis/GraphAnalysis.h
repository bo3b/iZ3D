#pragma once

#include "ShaderGraph.h"
#include "AnalyzeData.h"
#include <bitset>

namespace shader_analyzer
{
	static const unsigned INVALID_REGISTER = 0xFFFFFFFF;

	void FindProjectionMatrixFromGraph( const TGraph& gr,
		const TStartVerticesArray& arr,
		ProjectionShaderCBs& matList );

	void FindProjectionMatrixFromShader( const TShaderList& shList,
		unsigned outputRegisterIndex,
		ProjectionShaderCBs& matList );

	void GetUsedResourcesMask( const TShaderList& shList, std::bitset<128> &mask );
}