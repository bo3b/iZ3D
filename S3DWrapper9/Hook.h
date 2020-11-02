/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include <Psapi.h>
#include <d3d9.h>
#include <set>
#include "boost/unordered_map.hpp"
#include <hash_map>
#include "Trace.h"
#include "MadCHook.h"

#define HOOKING_FLAG (NO_SAFE_UNHOOKING)

extern "C" void * _ReturnAddress();
#pragma intrinsic(_ReturnAddress)

#ifdef _DEBUG
class ProxyGuard
{
public:
	ProxyGuard()	{ _InterlockedIncrement(&callLevel); }
	~ProxyGuard()	{ _InterlockedDecrement(&callLevel); }

	static volatile long callLevel;
};
	#define CHECKMODIFICATION()	 ProxyGuard pg; { if (pg.callLevel > 10) __debugbreak(); } // recursion
	//#define CHECKMODIFICATION()	{ /*if */(!checkAllHooked()) /*__debugbreak()*/; }
#else
	#define CHECKMODIFICATION()	
#endif

#define g_CallingModule reinterpret_cast<LONG_PTR>(_ReturnAddress())

#define CalledFromD3D9()		(CalledFromOnlyD3D9() || CalledFromWrapper())
#define CalledFromApp()			(!(CalledFromOnlyD3D9() || CalledFromWrapper()))
#define CalledFromOnlyD3D9()	(g_CallingModule >= g_RenderInfo.m_D3D9StartAddress && g_CallingModule <= g_RenderInfo.m_D3D9EndAddress)
#define CalledFromWrapper()		((g_CallingModule >= g_RenderInfo.m_Wrapper9StartAddress && g_CallingModule <= g_RenderInfo.m_Wrapper9EndAddress) || \
	(g_CallingModule >= g_RenderInfo.m_Wrapper9OutputStartAddress && g_CallingModule <= g_RenderInfo.m_Wrapper9OutputEndAddress))

#define HOOK_GET_DEVICE(ppobject, proxyName, pNextHook)								\
	if(SUCCEEDED(hResult) && ppobject && *ppobject &&								\
		gInfo.RouterType != ROUTER_TYPE_HOOK)										\
	{																				\
		void** p = *(void***)(*ppobject);											\
		UniqueHookCode(this, p[3], proxyName, (PVOID*)&pNextHook);					\
	}

#define UNHOOK_GET_DEVICE(pNextHook)												\
	if(gInfo.RouterType != ROUTER_TYPE_HOOK)										\
	{																				\
		UniqueUnhookCode(this, (PVOID*)&pNextHook);									\
	}

#define HOOK_ADDREF(object, proxyName, pNextHook)									\
	if(object)																		\
	{																				\
		void** p = *(void***)(object);												\
		UniqueHookCode(this, p[1], proxyName, (PVOID*)&pNextHook);					\
	}	

#define HOOK_RELEASE(object, proxyName, pNextHook)									\
	if(object)																		\
	{																				\
		void** p = *(void***)(object);												\
		UniqueHookCode(this, p[2], proxyName, (PVOID*)&pNextHook);					\
	}	



#define PROXYSTDMETHOD(a)			HRESULT	__stdcall Proxy_##a 
#define PROXYSTDMETHOD_(type, a)	type	__stdcall Proxy_##a
#define DECLAREPOINTER(a, b)		void** p = *(void***)a; void* pFunc = UniqueGetInterfaceEntry(p[_ ## b ## _], _ ## b ## _); _ASSERT(pFunc)
#define CALLFORPOINTER(a, b)		(a->*(*(b##_t*)&pFunc))

//--- exclude double hook functions ---
struct AddressSet
{ 
	PVOID	pCallbackFunc, pNextHook;
	DWORD	entryIndex;
	std::set<void*>	interfacesSet;
#ifndef FINAL_RELEASE
	std::set<const TCHAR*> functionName;	//--- list of proxy function. multiple proxy support ---
#endif
};

#ifdef _DEBUG // to simplify debugging
typedef stdext::hash_map	<LPVOID, AddressSet> AddressSetMap;
#else
typedef boost::unordered_map<LPVOID, AddressSet> AddressSetMap;
#endif
typedef AddressSetMap::iterator AddressSetIterator;
typedef std::set<const TCHAR*>::iterator FunctionNameIterator;
extern  AddressSetMap g_UniqueHooks;

inline AddressSetIterator FindHookByCode(PVOID pCode)
{
	return g_UniqueHooks.find(pCode);
}

inline AddressSetIterator FindHookByNextHook(PVOID pNextHook)
{	
	AddressSetIterator _First = g_UniqueHooks.begin();
	AddressSetIterator _Last  = g_UniqueHooks.end();
	for (; _First != _Last; ++_First)
		if (_First->second.pNextHook == pNextHook)
			break;
	return (_First);
}

inline PVOID UniqueGetCode(PVOID pCode)
{
	AddressSetIterator it = FindHookByCode(pCode);
	AddressSetIterator _Last = g_UniqueHooks.end();
	if(it != _Last)
		return it->second.pNextHook;
	return pCode;
}

inline PVOID UniqueGetInterfaceEntry(PVOID pCode, DWORD entryIndex)
{
	AddressSetIterator it = FindHookByCode(pCode);
	AddressSetIterator _Last = g_UniqueHooks.end();
	if(it != _Last)
		return it->second.pNextHook;
	else
	{
		it = g_UniqueHooks.begin();
		for (; it != _Last; ++it)
			if (it->second.entryIndex == entryIndex)
				return it->second.pNextHook;
	}
	_ASSERT(FALSE);
	return pCode;
}

inline BOOL UniqueHookInterface(void* pWrapper, PVOID pCode, PVOID pCallbackFunc, PVOID * pNextHook, DWORD entryIndex
#ifndef FINAL_RELEASE
	, const TCHAR* func = NULL
#endif
	)
{
	AddressSetIterator it = FindHookByCode(pCode);
	if(it == g_UniqueHooks.end())
	{
		if(HookCode(pCode, pCallbackFunc, pNextHook, HOOKING_FLAG))
		{
			AddressSet addr = { pCallbackFunc, *pNextHook, entryIndex };
			addr.interfacesSet.insert(pWrapper);
#ifndef FINAL_RELEASE
			addr.functionName.insert(func);
#endif
			g_UniqueHooks.insert(AddressSetMap::value_type( pCode, addr ));
			return TRUE;
		}
#ifndef FINAL_RELEASE
		DEBUG_MESSAGE(_T("Error: UniqueHook(%s) function failed\n"), func);
#endif
		return FALSE;
	}
	it->second.interfacesSet.insert(pWrapper);
#ifndef FINAL_RELEASE
	it->second.functionName.insert(func);
#endif
#ifndef FINAL_RELEASE
	if (pCallbackFunc != it->second.pCallbackFunc) {
		DEBUG_TRACE2(_T("Warning! Must write MultiProxy for:\n"));
		for(FunctionNameIterator fn = it->second.functionName.begin(); fn !=  it->second.functionName.end(); fn++)	{
			DEBUG_TRACE2(_T("	%s\n"), *fn);
		}
	}
#endif
	*pNextHook = it->second.pNextHook;
	return TRUE;
}

inline bool IsCodeHookedByProxy(PVOID pCode, PVOID pCallBack)
{
	AddressSetIterator it = FindHookByCode(pCode);
	if(it != g_UniqueHooks.end() && it->second.pCallbackFunc == pCallBack)
		return true;
	return false;
}

//--- function named incorrectly, but this name very suitable for using in code ---
inline bool IsAlreadyHooked(PVOID pCode, PVOID *pNextHook)
{
	if (*pNextHook != NULL)
	{
		AddressSetIterator it = FindHookByNextHook(*pNextHook);
		if(it != g_UniqueHooks.end() && it->first != pCode) {
			return true;
		}
	}
	return false;
}

#ifdef FINAL_RELEASE
#define UniqueHookCode(pDev, pCode, pCallbackFunc, pNextHook) _UniqueHookCode(pDev, pCode, pCallbackFunc, pNextHook)
#else
#define UniqueHookCode(pDev, pCode, pCallbackFunc, pNextHook) _UniqueHookCode(pDev, pCode, pCallbackFunc, pNextHook, _T(#pCallbackFunc))
#endif

inline void _UniqueHookCode(void* pWrapper, PVOID pCode, PVOID pCallbackFunc, PVOID *pNextHook
#ifndef FINAL_RELEASE
							, const TCHAR* func = NULL
#endif
							)
{
#ifndef FINAL_RELEASE
	if (*pNextHook != NULL)
	{
		AddressSetIterator it = FindHookByNextHook(*pNextHook);
		if(it != g_UniqueHooks.end()) {
			LPVOID pPrevCode = it->first;
			AddressSet& prevSet = it->second;
			if (pPrevCode != pCode) {
				DEBUG_MESSAGE(_T("Error: pCode not equal to already hooked"));
			}
		}
	}
#endif
	UniqueHookInterface(pWrapper, pCode, pCallbackFunc, pNextHook, -1
#ifndef FINAL_RELEASE
		, func
#endif
		);
}

inline BOOL UniqueUnhookInterface(void* pWrapper, PVOID *pNextHook)
{
	AddressSetIterator it = FindHookByNextHook(*pNextHook);
	if(it != g_UniqueHooks.end())
	{
		it->second.interfacesSet.erase(pWrapper);
		if(!it->second.interfacesSet.empty()) {
#ifndef FINAL_RELEASE
			DEBUG_MESSAGE(_T("RC = %d. Functions:\n"), it->second.interfacesSet.size());
			for(FunctionNameIterator fn = it->second.functionName.begin(); fn !=  it->second.functionName.end(); fn++)	{
				DEBUG_TRACE2(_T("	%s\n"), *fn);
			}
#endif
			return FALSE;
		} 
		else 
		{
			if(UnhookCode(pNextHook))
			{
				g_UniqueHooks.erase(it);
				return TRUE;
			}
		}
#ifndef FINAL_RELEASE
		DEBUG_MESSAGE(_T("Functions already unhooked:\n"));
		for(FunctionNameIterator fn = it->second.functionName.begin(); fn !=  it->second.functionName.end(); fn++)	{
			DEBUG_TRACE2(_T("	%s\n"), *fn);
		}
#endif
		return TRUE;
	}
	return FALSE;
}

inline void UniqueUnhookCode(void* pWrapper, PVOID *pNextHook)
{
	if(*pNextHook)																					
	{																								
		if (UniqueUnhookInterface(pWrapper, pNextHook))
		{
			*pNextHook = 0;
		}
	}
}

class RenderDLLInfo
{
public:
	RenderDLLInfo();

	//--- execute it only one time in DLLMain ---
	void		OneTimeInitialize();
	//--- execute it each time then output changed ---
	void		SetOutputDll(HMODULE hDLL) { initializeAddress(hDLL, m_Wrapper9OutputStartAddress, m_Wrapper9OutputEndAddress); }

	//--- non-device router data ---
	LONG_PTR	m_D3D9StartAddress, m_D3D9EndAddress;
	LONG_PTR	m_Wrapper9StartAddress, m_Wrapper9EndAddress;
	LONG_PTR	m_Wrapper9OutputStartAddress, m_Wrapper9OutputEndAddress;

private:
	void		initializeAddress(HMODULE hDLL, LONG_PTR& start, LONG_PTR& end);
};

extern RenderDLLInfo g_RenderInfo;
class  CDirect3D9;
class  CMonoRenderer;
class  CBaseStereoRenderer;
class  CMonoSwapChain;
class  CBaseSwapChain;

//------------------------------------------------- Direct3D interface wrapper list  ------------------------------------------
template <typename IDirectInteface, typename Wrapper>
class WrapperList
{
public:
	typedef std::pair<IDirectInteface*, Wrapper*>		AddressPair;
	typedef	typename std::vector<AddressPair>::iterator	AddressPairIterator;
	std::vector<AddressPair>							m_WrapperList;

	//--- add renderer to list ---
	inline void AddWrapper(IDirectInteface* pDirect, Wrapper* pRenderer)
	{
		m_WrapperList.push_back(AddressPair(pDirect, pRenderer));
	}

	//--- remove renderer from list ---
	inline void RemoveWrapper(Wrapper* pWrapper)
	{
		AddressPairIterator _First = m_WrapperList.begin();
		AddressPairIterator _Last  = m_WrapperList.end();
		for (; _First != _Last; ++_First)
			if (_First->second == pWrapper)
			{
				m_WrapperList.erase(_First);
				break;
			}
	}

	inline Wrapper* FindWrapper(IDirectInteface* pDirect)
	{
		AddressPairIterator _First = m_WrapperList.begin();
		AddressPairIterator _Last  = m_WrapperList.end();
		for (; _First != _Last; ++_First)
			if (_First->first == pDirect)
				return _First->second;
		return NULL;
	}

	inline bool IsWrapperInList(Wrapper* pRenderer)
	{
		AddressPairIterator _First = m_WrapperList.begin();
		AddressPairIterator _Last  = m_WrapperList.end();
		for (; _First != _Last; ++_First)
			if (_First->second == pRenderer)
				return true;
		return false;
	}
};
																						
typedef WrapperList<IDirect3D9, CDirect3D9>					DirectWrapperClass;
extern DirectWrapperClass									g_pDirectWrapperList;

typedef WrapperList<IDirect3DSwapChain9, CMonoSwapChain>	SwapChainWrapperClass;
extern SwapChainWrapperClass								g_pSwapChainWrapperList;

typedef WrapperList<IDirect3DDevice9, CMonoRenderer>		RendererListClass;
extern RendererListClass									g_pRendererList;

template <typename IDirectInteface>
inline CDirect3D9* FindDirectWrapper(IDirectInteface* pDirect)
{
	CDirect3D9* pWrapper = (CDirect3D9*)g_pDirectWrapperList.FindWrapper(pDirect);
	//if (pWrapper && pWrapper->m_pHookedDirect.GetRealDirect() == pDirect)
	//{
	//	DEBUG_MESSAGE(_T("WARNING: Call from original direct\n"));
	//}
	return pWrapper;
}

template <typename IDirectInteface>
inline CBaseSwapChain* FindSwapChainWrapper(IDirectInteface* pDirect)
{
	CBaseSwapChain* pWrapper = (CBaseSwapChain*)g_pSwapChainWrapperList.FindWrapper(pDirect);
	//if (pWrapper && pWrapper->m_pHookedSwapChain.GetSwapChain() == pDirect)
	//{
	//	DEBUG_MESSAGE(_T("WARNING: Call from original swapchain\n"));
	//}
	return pWrapper;
}

template <typename IDirectInteface>
inline CBaseStereoRenderer* FindStereoRenderer(IDirectInteface* pDirect)
{
	CBaseStereoRenderer* renderer = (CBaseStereoRenderer*)g_pRendererList.FindWrapper(pDirect);
	//if (renderer && renderer->m_pDirect3DDevice.GetRealDevice() == pDevice)
	//{
	//	DEBUG_MESSAGE(_T("WARNING: Call from original device\n"));
	//}
	return renderer;
}

template <typename IDirectObject>
void GetCurrentStereoRenderer(IDirectObject* object, CBaseStereoRenderer** renderer)														
{
	IDirect3DDevice9* pDevice;
	HRESULT hrGetDevice = object->GetDevice(&pDevice);
	*renderer = FindStereoRenderer(pDevice);
	pDevice->Release();
#ifndef FINAL_RELEASE
	if(*renderer == NULL)
	{
		ZLOG_MESSAGE(zlog::SV_ERROR, _T("Error: Can't find StereoRenderer. Object = %p, Device = %p\n"), object, pDevice);
	}
#endif
}
