#pragma once

#include "ConstantBufferWrapper.h"

class ConstantBufferWrapperUsageDynamic
	: public ConstantBufferWrapper
{
public:
	ConstantBufferWrapperUsageDynamic( D3DDeviceWrapper* pWrapper, ResourceWrapper* pResurceWrapper );
	
	// Override ConstantBufferWrapper
	void UpdateCurrentPool( D3DDeviceWrapper* pWrapper, VIEWINDEX index, bool mono );
};
