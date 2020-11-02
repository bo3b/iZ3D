#pragma once
#include <boost/intrusive_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <map>
#include <set>
#include <tuple>
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <madCHook.h> // after windows.h
#include "S3DAPI.h"

/** Base class for reference counting objects */
class counted_base
{
public:
    counted_base() :
        refCount(0)
    {}

    counted_base(const counted_base& /*referenced*/) :
        refCount(0)
    {}

    // Don't modify reference counter
    counted_base& operator = (const counted_base& /*rhs*/) 
	{ 
		return *this; 
	}

    unsigned use_count() const 
	{ 
		return refCount; 
	}
    
    void add_ref() const
    {
        ++refCount;
    }

    void remove_ref() const
    {
        if (--refCount == 0) {
            delete this;
        }
    }

protected:
	virtual ~counted_base() {}

private:
    mutable int refCount;
};

namespace boost {

	inline void intrusive_ptr_add_ref(const counted_base* referenced) {
		referenced->add_ref();
	}

	inline void intrusive_ptr_release(const counted_base* referenced) {
		referenced->remove_ref();
	}

} // namespace boost

class hook_error :
	public std::runtime_error
{
public:
	hook_error(const std::string& message = "Can't hook function") :
		std::runtime_error(message)
	{
	}
};

// forward
class abstract_hook;
typedef boost::intrusive_ptr<abstract_hook> abstract_hook_ptr;

class abstract_hook :
	public boost::noncopyable,
	public counted_base
{
private:
	typedef std::map<HMODULE, size_t>	module_counter_map;

	static HMODULE get_this_module_handle()
	{
		MEMORY_BASIC_INFORMATION info;
		size_t len = VirtualQueryEx(GetCurrentProcess(), (void*)get_this_module_handle, &info, sizeof(info));
		assert(len == sizeof(info));
		return len ? (HMODULE)info.AllocationBase : NULL;
	}

public:
	abstract_hook()
	{
		HMODULE handle = get_this_module_handle();
		moduleHookCounterIter = moduleHookCounters.find(handle);
		if ( moduleHookCounterIter == moduleHookCounters.end() ) 
		{
			moduleHookCounterIter = moduleHookCounters.insert( std::make_pair(handle, 0) ).first;
			InitializeMadCHook();
		}

		moduleHookCounterIter->second++;
	}

	virtual ~abstract_hook() 
	{
		if (--moduleHookCounterIter->second == 0) 
		{
			FinalizeMadCHook();
			moduleHookCounters.erase(moduleHookCounterIter);
		}
	}

private:
	module_counter_map::iterator			moduleHookCounterIter;
	S3DAPI_API static module_counter_map	moduleHookCounters;		// count hooks per module
};

template<typename SrcSig,
		 SrcSig*  pCode,
		 typename DstSig,
		 DstSig*  pCallbackFunc>
class static_hook :
	public abstract_hook
{
private:
	typedef SrcSig*												src_signature;
	typedef DstSig*												dst_signature;
	typedef static_hook<SrcSig, pCode, DstSig, pCallbackFunc>	this_type;

	struct hook_impl :
		public counted_base
	{
		DWORD		  flags;
		src_signature pNextHook;

		hook_impl(DWORD flags_)
		:	flags(flags_)
		{ 
			if ( !HookCode(pCode, pCallbackFunc, (PVOID*)&pNextHook, flags) ) {
				throw hook_error();
			}
		}
		
		~hook_impl()			
		{ 
			this_type::pImpl_ = 0;
			if ( !UnhookCode((PVOID*)&pNextHook) ) {
				throw hook_error();
			}
		}
	};
	typedef boost::intrusive_ptr<hook_impl> hook_impl_ptr;

public:
	static_hook(DWORD flags)
	{
		if (!pImpl_) {
			pImpl_ = new hook_impl(flags);
		}
		pImpl.reset(pImpl_);
	}

	src_signature get_next_hook() { return pImpl->pNextHook; }

	static bool exists() { return pImpl_ != 0; }

private:
	hook_impl_ptr		pImpl;
	static hook_impl*	pImpl_;
};

template<typename SrcSig, SrcSig* pCode, typename DstSig, DstSig* pCallbackFunc>
typename static_hook<SrcSig, pCode, DstSig, pCallbackFunc>::hook_impl* static_hook<SrcSig, pCode, DstSig, pCallbackFunc>::pImpl_ = 0;

// Prevent hook deinitialization until end of app execution
class hook_guard :
	public abstract_hook
{};
typedef boost::intrusive_ptr<hook_guard> hook_guard_ptr;

template< typename SrcFunc, 
		  typename DstFunc,
		  typename Key = std::pair<SrcFunc, DstFunc> >
class unique_hook :
	public abstract_hook
{
private:
	typedef unique_hook<SrcFunc, DstFunc, Key>	this_type;
	typedef std::map<Key, this_type*>			hook_map;

	template<typename S, typename D, typename K>
	friend boost::intrusive_ptr< unique_hook<S, D, K> > get_unique_hook(K);

	template<typename S, typename D, typename K>
	friend boost::intrusive_ptr< unique_hook<S, D, K> > make_unique_hook(S, D, DWORD, K);

	template<typename I, typename S, typename D>
	friend class unique_member_hook;

private:
	unique_hook(SrcFunc pCode_, DstFunc pCallbackFunc_, DWORD flags_, Key key_)
	:	pCode(pCode_)
	,	pCallbackFunc(pCallbackFunc_)
	,	flags(flags_)
	,	key(key_)
	{
		if ( !HookCode( *(PVOID*)(&pCode), pCallbackFunc, (PVOID*)&pNextHook, flags) ) {
			throw hook_error();
		}
		hooks.insert( hook_map::value_type(key, this) );
	}

public:
	~unique_hook()
	{
		hooks.erase(key);
		UnhookCode((PVOID*)&pNextHook);
		// dragon: disable it, because this crash with Static Test
		//if ( !UnhookCode((PVOID*)&pNextHook) ) {
		//	throw hook_error();
		//}
	}

	SrcFunc get_next_hook() { return pNextHook; }

private:
	SrcFunc	pCode;
	DstFunc	pCallbackFunc;
	SrcFunc	pNextHook;
	DWORD	flags;
	Key		key;

private:
	static hook_map hooks;
};

template<typename SrcSig, typename DstSig, typename Key>
typename unique_hook<SrcSig, DstSig, Key>::hook_map unique_hook<SrcSig, DstSig, Key>::hooks;

template<typename Interface, typename SrcSig, typename DstFunc>
class unique_member_hook :
	public abstract_hook
{
private:
	typedef unique_member_hook<Interface, SrcSig, DstFunc>	this_type;
	typedef std::pair<void*, DstFunc>						key_type;
	typedef unique_hook<SrcSig, DstFunc, key_type>			hook_type;
	typedef boost::intrusive_ptr<hook_type>					hook_ptr;

	typedef std::map<Interface*, this_type*>				hook_map;
	
	template<typename S, S pMemberFunc, typename D, typename Interface>
	friend boost::intrusive_ptr< unique_member_hook<Interface, S, D> > get_unique_member_hook(Interface* iface, D pCallbackFunc);

	template<typename S, S pMemberFunc, typename D, typename Interface>
	friend boost::intrusive_ptr< unique_member_hook<Interface, S, D> > make_unique_member_hook(Interface* iface, D pCallbackFunc, DWORD flags);

private:
	unique_member_hook(Interface* iface_, const hook_ptr& hook_)
	:	iface(iface_)
	,	hook(hook_)
	{ 
		if (iface)
		{
			hook_map::value_type value(iface, this);
			hooks.insert(value);
		}
	}

public:
	~unique_member_hook()
	{
		hooks.erase(iface);
	}
	
	SrcSig get_next_hook() { return hook->get_next_hook(); }

	bool interface_hooked() const { return iface != 0; }

private:
	Interface*	iface;
	hook_ptr	hook;

private:
	static hook_map hooks;
};

template<typename Interface, typename SrcSig, typename DstSig>
typename unique_member_hook<Interface, SrcSig, DstSig>::hook_map unique_member_hook<Interface, SrcSig, DstSig>::hooks;
/*
class window_proc_hook :
	public abstract_hook
{
private:
	typedef window_proc_hook				this_type;
	typedef std::map<WNDPROC, this_type*>	hook_map;
	
	template<typename S, S pMemberFunc, typename D, typename Interface>
	friend boost::intrusive_ptr<window_proc_hook> get_window_proc_hook(HWND hWnd);

	friend boost::intrusive_ptr<window_proc_hook> make_window_proc_hook(HWND hWnd, WNDPROC originalWndProc);

private:
	unique_member_hook(Interface* iface_, const hook_ptr& hook_)
	:	iface(iface_)
	,	hook(hook_)
	{ 
		if (iface)
		{
			hook_map::value_type value(iface, this);
			hooks.insert(value);
		}
	}

public:
	WNDPROC get_next_hook() { return originalWndProc; }

private:
	HWND	hWnd;
	WNDPROC	originalWndProc;
};
*/
/** Simple container of hooks */
class hook_storage
{
public:
	void add_hook(const abstract_hook_ptr& hook)	{ hooks.insert(hook); }
	bool remove_hook(const abstract_hook_ptr& hook) { return (hooks.erase(hook) > 0); }
	void clear()									{ hooks.clear(); }

private:
	std::set<abstract_hook_ptr> hooks;
};

template<typename Signature, Signature>
struct function_offset;

template<typename SrcSig, SrcSig* pCode, typename DstSig, DstSig* pCallbackFunc>
boost::intrusive_ptr< static_hook<SrcSig, pCode, DstSig, pCallbackFunc> > get_static_hook()
{
	boost::intrusive_ptr< static_hook<SrcSig, pCode, DstSig, pCallbackFunc> > hook;
	if ( static_hook<SrcSig, pCode, DstSig, pCallbackFunc>::exists() ) {
		hook.reset( new static_hook<SrcSig, pCode, DstSig, pCallbackFunc>(0) );
	}

	return hook;
}

template<typename SrcSig, SrcSig* pCode, typename DstSig, DstSig* pCallbackFunc>
boost::intrusive_ptr< static_hook<SrcSig, pCode, DstSig, pCallbackFunc> > make_static_hook(SrcSig, DstSig, DWORD flags)
{
	return boost::intrusive_ptr< static_hook<SrcSig, pCode, DstSig, pCallbackFunc> >( new static_hook<SrcSig, pCode, DstSig, pCallbackFunc>(flags) );
}

template<typename SrcFunc, typename DstFunc, typename Key>
boost::intrusive_ptr< unique_hook<SrcFunc, DstFunc, Key> > get_unique_hook(Key key)
{
	typedef unique_hook<SrcFunc, DstFunc, Key> hook_type;

	auto iter = hook_type::hooks.find(key);
	if ( iter != hook_type::hooks.end() ) {
		return boost::intrusive_ptr<hook_type>(iter->second);
	}

	return boost::intrusive_ptr<hook_type>();
}

template<typename SrcFunc, typename DstFunc, typename Key>
boost::intrusive_ptr< unique_hook<SrcFunc, DstFunc, Key> > get_unique_hook(SrcFunc pCode, DstFunc pCallbackFunc, Key key)
{
	return get_unique_hook<SrcFunc, DstFunc>(key);
}

template<typename SrcFunc, typename DstFunc>
boost::intrusive_ptr< unique_hook<SrcFunc, DstFunc> > get_unique_hook(SrcFunc pCode, DstFunc pCallbackFunc)
{
	return get_unique_hook(pCode, pCallbackFunc, std::make_pair(pCode, pCallbackFunc));
}

template<typename SrcFunc, typename DstFunc, typename Key>
boost::intrusive_ptr< unique_hook<SrcFunc, DstFunc, Key> > make_unique_hook(SrcFunc pCode, DstFunc pCallbackFunc, DWORD flags, Key key)
{
	typedef unique_hook<SrcFunc, DstFunc, Key> hook_type;

	boost::intrusive_ptr<hook_type> hook;

	auto iter = hook_type::hooks.find(key);
	if ( iter != hook_type::hooks.end() ) {
		hook.reset(iter->second);
	}
	else {
		hook.reset( new hook_type(pCode, pCallbackFunc, flags, key) );
	}

	return hook;
}

template<typename SrcFunc, typename DstFunc>
boost::intrusive_ptr< unique_hook<SrcFunc, DstFunc> > make_unique_hook(SrcFunc pCode, DstFunc pCallbackFunc, DWORD flags)
{
	return make_unique_hook(pCode, pCallbackFunc, flags, std::make_pair(pCode, pCallbackFunc));
}

template
<
	typename SrcSig, 
	SrcSig   pMemberFunc, 
	typename DstFunc,
	typename Interface
>
boost::intrusive_ptr< unique_member_hook<Interface, SrcSig, DstFunc> > get_unique_member_hook(Interface* iface, DstFunc pCallbackFunc)
{
	typedef unique_member_hook<Interface, SrcSig, DstFunc> hook_type;

	boost::intrusive_ptr<hook_type> hook;

	void** vtabl = *(void***)iface;
	void*  pCode = vtabl[function_offset<SrcSig, pMemberFunc>::value];
	
	auto key  = hook_type::key_type(pCode, pCallbackFunc);
	auto iter = hook_type::hooks.find(iface);
	if ( iter != hook_type::hooks.end() ) {
		hook.reset(iter->second);
	}
	else 
	{
		auto h = get_unique_hook(*((SrcSig*)&pCode), pCallbackFunc, key);
		if (h) {
			hook.reset( new hook_type(0, h) );
		}
	}

	return hook;
}

template
<
	typename SrcSig, 
	SrcSig   pMemberFunc, 
	typename DstFunc,
	typename Interface
>
boost::intrusive_ptr< unique_member_hook<Interface, SrcSig, DstFunc> > make_unique_member_hook(Interface* iface, DstFunc pCallbackFunc, DWORD flags)
{
	typedef unique_member_hook<Interface, SrcSig, DstFunc> hook_type;
	
	boost::intrusive_ptr<hook_type> hook;
	
	void** vtabl = *(void***)iface;
	void*  pCode = vtabl[function_offset<SrcSig, pMemberFunc>::value];

	auto key  = hook_type::key_type(pCode, pCallbackFunc);
	auto iter = hook_type::hooks.find(iface);
	if ( iter != hook_type::hooks.end() ) {
		hook.reset(iter->second);
	}
	else {
		hook.reset( new hook_type( iface, make_unique_hook(*((SrcSig*)&pCode), pCallbackFunc, flags, key) ) );
	}

	return hook;
}

#define REGISTER_FUNCTION_OFFSET(Func, Offset) template<> struct function_offset<decltype(Func), Func> { static const int value = Offset; };

#define DECLARE_STATIC_HOOK(Name, Function, HookedFunction)\
boost::intrusive_ptr< static_hook<decltype(Function), Function, decltype(HookedFunction), HookedFunction> > Name;

#define DECLARE_UNIQUE_HOOK(Name, Function, HookedFunction)\
boost::intrusive_ptr< unique_hook<decltype(Function), decltype(HookedFunction)> > Name;

#define DECLARE_UNIQUE_MEMBER_HOOK(Name, IFace, Function, HookedFunction)\
boost::intrusive_ptr< unique_member_hook<IFace, decltype(Function), decltype(HookedFunction)> > Name;

#define STATIC_FUNCTION_HOOK(Function, HookedFunction)\
typedef static_hook<decltype(Function), Function, decltype(HookedFunction), HookedFunction> UniqueHookT;\
auto UniqueHook = get_static_hook<decltype(Function), Function, decltype(HookedFunction), HookedFunction>();

#define UNIQUE_FUNCTION_HOOK(Function, HookedFunction)\
auto UniqueHook = get_unique_hook(Function, HookedFunction);

#define UNIQUE_MEMBER_FUNCTION_HOOK(IFace, Function, HookedFunction)\
auto UniqueHook = get_unique_member_hook<decltype(Function), Function>(IFace, HookedFunction);

#define MAKE_STATIC_HOOK(Function, HookedFunction, Flags)\
make_static_hook<decltype(Function), Function, decltype(HookedFunction), HookedFunction>(Function, HookedFunction, Flags)

#define MAKE_UNIQUE_HOOK(Function, HookedFunction, Flags)\
make_unique_hook(Function, HookedFunction, Flags)

#define MAKE_UNIQUE_MEMBER_HOOK(IFace, Function, HookedFunction, Flags)\
make_unique_member_hook<decltype(Function), Function>(IFace, HookedFunction, Flags)

#define GET_ORIGINAL						UniqueHook->get_next_hook()
#define CALL_ORIGINAL(...)					UniqueHook->get_next_hook()(__VA_ARGS__)
#define CALL_ORIGINAL_MEMBER(IFace, ...)	(IFace->*UniqueHook->get_next_hook())(__VA_ARGS__)
#define INTERFACE_HOOKED					UniqueHook->interface_hooked()
