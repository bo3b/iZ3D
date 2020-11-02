#pragma once
#include "Singleton.h"
#include <Unknwn.h>
#include <list>
class COMLikeRepository : public iz3d::Singleton<COMLikeRepository>
{
	friend iz3d::Singleton<COMLikeRepository>;
	COMLikeRepository(void);
	~COMLikeRepository(void);
	std::list<IUnknown*> m_data;
public:
	void Add( IUnknown* pToAdd );
	void Remove( IUnknown* pToRemove );
	void Drop();
};
