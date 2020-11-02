#pragma once
#include "Hook.h"

enum PROXY_EXMODE
{
	EXMODE_NOT_INITIALIZED,
	EXMODE_NONE,
	EXMODE_EMULATE,		//--- emulate Direct3D9xxx interfaces in Direct3D9xxxEx mode 
	EXMODE_EX
};

//------------------------------------------------- Router class  ----------------------------------------------
template <class T>
class RouterTypeHookCallGuard
{
	T* m_pInterface;
public:
	RouterTypeHookCallGuard(T* pInterface)	{ m_pInterface = pInterface; InterlockedIncrement(&m_pInterface->m_RouterHookGuard); }
	~RouterTypeHookCallGuard()				{ InterlockedDecrement(&m_pInterface->m_RouterHookGuard); }
};

//------------------------------------------------- Base proxy class  ------------------------------------------
template <typename IHookedInterface, typename FunctionList>
class ProxyBase
{
public:
	ProxyBase() : m_MethodCount(0), m_pHookedInterface(0), m_RouterHookGuard(0), m_InternalReferenceCount(0), m_ExMode(EXMODE_NOT_INITIALIZED) {}
	~ProxyBase(); 

	void			setExMode(PROXY_EXMODE exMode) { m_ExMode = exMode; }	//--- MUST be called before initialize ---
	void			initialize(IHookedInterface* pHookedInterface); 
	void			refreshPointers();
	int				clear();
	BOOL			HookInterfaceEntry(void* pWrapper, DWORD entryIndex, PVOID pCallbackFunc);
	BOOL			UnhookInterfaceEntry(void* pWrapper, DWORD entryIndex);
	bool			checkAllHooked();
	inline			IHookedInterface* GetReal() {return m_pHookedInterface; }
	PROXY_EXMODE	getExMode() { _ASSERT(m_ExMode != EXMODE_NOT_INITIALIZED); return m_ExMode; }

	operator bool()	const { return m_pHookedInterface != NULL; }
	operator IHookedInterface*() const { return m_pHookedInterface; }

	LONG											m_RouterHookGuard;
	LONG											m_InternalReferenceCount;
	FunctionList									m_fpOriginal;

protected:
	DWORD											m_MethodCount;
	IHookedInterface*								m_pHookedInterface;
	PROXY_EXMODE									m_ExMode;
};

template <typename IHookedInterface, typename FunctionList>
inline ProxyBase<IHookedInterface, FunctionList>::~ProxyBase()
{ 
	int realRef = clear();
	DEBUG_TRACE1(_T("Function: %s - realRefCount = %i, m_InternalReferenceCount = %i\n"), _T(__FUNCTION__), realRef, m_InternalReferenceCount); 
}

template <typename IHookedInterface, typename FunctionList>
inline void ProxyBase<IHookedInterface, FunctionList>::initialize(IHookedInterface* pHookedInterface)
{
	_ASSERT(m_ExMode != EXMODE_NOT_INITIALIZED);
	m_pHookedInterface = pHookedInterface;
	m_MethodCount = (m_ExMode == EXMODE_NONE) ? FunctionList::MAX_METHODS : FunctionList::MAX_METHODS_EX;
	refreshPointers();
	//--- use AddRef() for external reference keeping ---
	m_pHookedInterface->AddRef();
}

template <typename IHookedInterface, typename FunctionList>
inline int ProxyBase<IHookedInterface, FunctionList>::clear() 
{ 
	int realRef = 0;
	if(m_pHookedInterface)
	{
		realRef = m_pHookedInterface->Release();
		m_pHookedInterface = 0;
		DEBUG_TRACE1(_T("Function: %s - realRefCount = %i, m_InternalReferenceCount = %i\n"), _T(__FUNCTION__), realRef, m_InternalReferenceCount); 
	}
	return realRef;
}

template <typename IHookedInterface, typename FunctionList>
inline void ProxyBase<IHookedInterface, FunctionList>::refreshPointers()
{
	_ASSERT(m_pHookedInterface);

	void** p = *(void***)m_pHookedInterface;
	for(DWORD i=0; i< m_MethodCount; i++)
		m_fpOriginal.func[i] = UniqueGetCode(p[i]);
}

template <typename IHookedInterface, typename FunctionList>
inline bool ProxyBase<IHookedInterface, FunctionList>::checkAllHooked()
{
	_ASSERT(m_pHookedInterface);

	bool result = true;
#ifndef FINAL_RELEASE
	void** p = *(void***)m_pHookedInterface;
	for(int i=0; i< m_MethodCount; i++)
		if (p[i] != m_fpOriginal.func[i]) // Only work when we don't hook
		{
			result = false;
			DEBUG_TRACE1(_T("Function changed %s\n"), getMethodName(i));
		}
#endif
	return result;
}

template <typename IHookedInterface, typename FunctionList>
inline BOOL ProxyBase<IHookedInterface, FunctionList>::HookInterfaceEntry(void* pWrapper, DWORD entryIndex, PVOID pCallbackFunc)
{
	_ASSERT(m_pHookedInterface);

	void** p = *(void***)m_pHookedInterface;												
	DEBUG_TRACE2(_T("Hook %s\n"), FunctionList::GetFunctionName(entryIndex));
	if(UniqueHookInterface(pWrapper, p[entryIndex], pCallbackFunc, &m_fpOriginal.func[entryIndex], entryIndex
#ifndef FINAL_RELEASE
		, FunctionList::GetFunctionName(entryIndex)
#endif
		))
	{
		return TRUE;
	}
	DEBUG_MESSAGE(_T("Error: HookCode(%s) function failed\n"), FunctionList::GetFunctionName(entryIndex));
	return FALSE;
}

template <typename IHookedInterface, typename FunctionList>
inline BOOL ProxyBase<IHookedInterface, FunctionList>::UnhookInterfaceEntry(void* pWrapper, DWORD entryIndex)
{
	_ASSERT(m_pHookedInterface);

	DEBUG_TRACE2(_T("Unhook %s\n"), FunctionList::GetFunctionName(entryIndex));
	if(UniqueUnhookInterface(pWrapper, &m_fpOriginal.func[entryIndex]))
	{
		return TRUE;
	}
	DEBUG_MESSAGE(_T("UnHookCode(%s) function failed or multiple hook\n"), FunctionList::GetFunctionName(entryIndex));
	return FALSE;
}