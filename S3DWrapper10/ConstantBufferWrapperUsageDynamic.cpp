#include "StdAfx.h"
#include "ConstantBufferWrapperUsageDynamic.h"
#include "D3DDeviceWrapper.h"

ConstantBufferWrapperUsageDynamic::ConstantBufferWrapperUsageDynamic( D3DDeviceWrapper* pWrapper, ResourceWrapper* pResourceWrapper )
:	ConstantBufferWrapper(pWrapper, pResourceWrapper)
{
}

void ConstantBufferWrapperUsageDynamic::UpdateCurrentPool( D3DDeviceWrapper* pWrapper, VIEWINDEX vi, bool mono )
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

	D3D10DDI_MAPPED_SUBRESOURCE MappedSubResource;
	pWrapper->OriginalDeviceFuncs.pfnDynamicConstantBufferMapDiscard( 
		pWrapper->hDevice, 
		m_pResourceWrapper->GetViewHandle(vi), 
		0, 
		D3D10_DDI_MAP_WRITE_DISCARD, 
		0, 
		&MappedSubResource );
	UpdateMatrices(pWrapper, vi, mono, (FLOAT*)MappedSubResource.pData);
	pWrapper->OriginalDeviceFuncs.pfnDynamicConstantBufferUnmap(pWrapper->hDevice, m_pResourceWrapper->GetViewHandle(vi), 0);
}
