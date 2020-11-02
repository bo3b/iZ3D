#include "StdAfx.h"
#include "ConstantBufferWrapperUsageDefault.h"
#include "D3DDeviceWrapper.h"

ConstantBufferWrapperUsageDefault::ConstantBufferWrapperUsageDefault( D3DDeviceWrapper* pWrapper, ResourceWrapper* pResourceWrapper )
:	ConstantBufferWrapper(pWrapper, pResourceWrapper)
{
	m_pModifiedData = (FLOAT*)_aligned_malloc(sizeof(FLOAT) * m_ElementCount, 16);
	if (!m_pModifiedData) {
		ZLOG_MESSAGE( zlog::SV_FATAL_ERROR, _T("Can't allocate memory for temp data in constant buffer wrapper") );
	}
}

ConstantBufferWrapperUsageDefault::~ConstantBufferWrapperUsageDefault()
{
	if (m_pModifiedData) {
		_aligned_free(m_pModifiedData);
	}
}

void ConstantBufferWrapperUsageDefault::UpdateCurrentPool( D3DDeviceWrapper* pWrapper, VIEWINDEX vi, bool mono )
{
#ifndef FINAL_RELEASE
	if(GINFO_DUMP_ON && pWrapper->m_DumpType >= dtOnlyRT)
	{
		if (mono)
		{
			DEBUG_TRACE3(_T("\t\tUpdate CB Matrices [%p] - Mono\n"), m_pResourceWrapper->GetWrapperHandle());
			WriteStreamer::Comment("Update CB Matrices - Mono");
		} 
		else if (vi == VIEWINDEX_LEFT)
		{
			DEBUG_TRACE3(_T("\t\tUpdate CB Matrices [%p] - Left\n"), m_pResourceWrapper->GetWrapperHandle());
			WriteStreamer::Comment("Update CB Matrices - Left");
		}
		else
		{
			DEBUG_TRACE3(_T("\t\tUpdate CB Matrices [%p] - Right\n"), m_pResourceWrapper->GetWrapperHandle());
			WriteStreamer::Comment("Update CB Matrices - Right");
		}
	}
#endif

	UpdateMatrices(pWrapper, vi, mono, m_pModifiedData);
	pWrapper->OriginalDeviceFuncs.pfnDefaultConstantBufferUpdateSubresourceUP ( 
		pWrapper->hDevice, 
		m_pResourceWrapper->GetViewHandle(vi), 
		0, 
		NULL, 
		m_pModifiedData, 
		m_pResourceWrapper->m_MipInfoList[0].PhysicalWidth, 
		m_pResourceWrapper->m_MipInfoList[0].PhysicalWidth 
	);
}