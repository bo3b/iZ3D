#include "stdafx.h"
#include "SetDestResourceType.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void SetDestResourceType::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		m_cmd->SetDestResourceType(m_type);
		AFTER_EXECUTE(this); 
#ifndef FINAL_RELEASE
		DestResourcesSet res;
		m_cmd->GetDestResources(res);
		int dstResID = -1;
		TextureType tt = TT_SETLIKESRC;
		if (!res.empty())
		{
			ResourceWrapper* pDstWrapper = (ResourceWrapper*)(*res.cbegin()).pDrvPrivate;
			if (pDstWrapper)
			{
				dstResID = pWrapper->m_ResourceManager.GetID(pDstWrapper->GetHandle());
				tt = pDstWrapper->m_Type;
			}
		}
		DEBUG_TRACE3(L"\t\t%S (Dst RenderTarget%d) - %S (%S)\n", 
			CommandIDToString(m_cmd->CommandId), dstResID, EnumToString(m_type), EnumToString(tt));
#endif
	}

	bool SetDestResourceType::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		return true;
	}

	bool SetDestResourceType::ReadFromFile()
	{
		return true;
	}

}
