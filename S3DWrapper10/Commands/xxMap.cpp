#include "stdafx.h"
#include "xxMap.h"
#include "ResourceWrapper.h"

namespace Commands 
{
	xxMap::xxMap()
	:   Subresource_(0)
	,   Flags_(0)
	,   pMappedSubResource_(0)
	{
	}

	void xxMap::SetDestResourceType( TextureType type )
	{
		ResourceWrapper* pRes;
		InitWrapper(hResource_, pRes);
		pRes->m_Type = type;
	}

	void xxMap::GetDestResources(DestResourcesSet &res)
	{
		res.insert(hResource_);
	}
}