#pragma once

#include "UMHandleWrapper.h"

template <typename RESOURCE, typename CLASS>
class StereoResourceWrapper : public UMHandleWrapper<RESOURCE>
{
public:
	StereoResourceWrapper()
	{
		m_hLeftHandle.pDrvPrivate = NULL;
		m_hRightHandle.pDrvPrivate = NULL;
	}

public:
	RESOURCE			m_hRightHandle;
	RESOURCE			m_hLeftHandle;
	bool				IsStereo				()	{ return m_hRightHandle.pDrvPrivate != NULL;	}
	RESOURCE			GetHandle				()	{ RESOURCE hOrig = { (( BYTE* ) this + sizeof(CLASS)) }; return hOrig;	}
	RESOURCE			GetCurrentViewHandle	();
	RESOURCE			GetViewHandle			(VIEWINDEX vi);
};

template <typename RESOURCE, typename CLASS>
inline RESOURCE StereoResourceWrapper<RESOURCE, CLASS>::GetViewHandle(VIEWINDEX vi)
{
	if (vi == VIEWINDEX_LEFT && m_hLeftHandle.pDrvPrivate) {
		return m_hLeftHandle;
	}
	else if (vi == VIEWINDEX_RIGHT && m_hRightHandle.pDrvPrivate) {
		return m_hRightHandle;
	}

	return GetHandle();
}


template <typename RESOURCE, typename CLASS>
inline RESOURCE StereoResourceWrapper<RESOURCE, CLASS>::GetCurrentViewHandle()
{
	return GetViewHandle(Commands::Command::CurrentView_);
}
