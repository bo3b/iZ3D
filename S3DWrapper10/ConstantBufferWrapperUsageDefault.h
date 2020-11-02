#pragma once

#include "ConstantBufferWrapper.h"

class ConstantBufferWrapperUsageDefault
	: public ConstantBufferWrapper
{
public:
	ConstantBufferWrapperUsageDefault( D3DDeviceWrapper* pWrapper, ResourceWrapper* pResurceWrapper );
	~ConstantBufferWrapperUsageDefault();

	// Override ConstantBufferWrapper
	void UpdateCurrentPool( D3DDeviceWrapper* pWrapper, VIEWINDEX index, bool mono );

private:
	FLOAT* m_pModifiedData;
};
