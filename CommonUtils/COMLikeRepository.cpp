#include "StdAfx.h"
#include "COMLikeRepository.h"
#include <algorithm>


COMLikeRepository::COMLikeRepository(void)
{
}

COMLikeRepository::~COMLikeRepository(void)
{
	//Should only called when D3D device destroying
	//Drop();
}

void COMLikeRepository::Add( IUnknown* pToAdd )
{
	m_data.push_back( pToAdd );
}

void COMLikeRepository::Remove( IUnknown* pToRemove )
{
	pToRemove->Release();
	m_data.remove( pToRemove );
}

void COMLikeRepository::Drop()
{
	struct Del{
	public:
		static void Remove(IUnknown* pToDelete){
			pToDelete->Release();
		}
	};
	std::for_each(m_data.begin(), m_data.end(), Del::Remove );
	m_data.clear();
}