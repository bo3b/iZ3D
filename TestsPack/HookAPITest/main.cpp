#define BOOST_TEST_MODULE HookAPITest
#include <boost/scoped_ptr.hpp>
#include <boost/test/unit_test.hpp>	
#include <iostream>
#include "../S3DAPI/HookApi.h"

hook_storage g_Hooks;
hook_guard	 g_HookGuard;

const DWORD HookingFlags = 0;

#ifdef BOOST_NO_EXCEPTIONS

// FIXME: temporal solution, disable exceptions for HookAPI in future
// to support compilation without exceptions /EHsc
void boost::throw_exception(const stdext::exception& ex)
{
	DebugBreak();
}

#endif // BOOST_NO_EXCEPTIONS

int __stdcall foo0(int hook)
{
	return hook;
}

int __stdcall foo0_hooked(int hook)
{
	STATIC_FUNCTION_HOOK(foo0, foo0_hooked);
	if (UniqueHook) {
		return CALL_ORIGINAL(hook) + 1;
	}
	return hook;
}

BOOST_AUTO_TEST_CASE(global_function_static_hook_0)
{
	BOOST_CHECK_EQUAL(foo0(0), 0);
	BOOST_CHECK_EQUAL(foo0_hooked(0), 0);
	abstract_hook_ptr hook = MAKE_STATIC_HOOK(foo0, foo0_hooked, HookingFlags);
	BOOST_CHECK_EQUAL(foo0(0), 1);
	BOOST_CHECK_EQUAL(foo0_hooked(0), 1);
	hook.reset();
	BOOST_CHECK_EQUAL(foo0(0), 0);
	BOOST_CHECK_EQUAL(foo0_hooked(0), 0);
}

class Base1
{
public:
	virtual int __stdcall foo(int hook) { return hook; };
	virtual ~Base1() {}
};

class Derived1 :
	public Base1
{
public:
	virtual int __stdcall foo(int hook) { return hook; }
};

REGISTER_FUNCTION_OFFSET(&Base1::foo, 0)
REGISTER_FUNCTION_OFFSET(&Derived1::foo, 0)

int __stdcall Hooked_Base1_foo(Base1* b0, int hook)
{
	UNIQUE_MEMBER_FUNCTION_HOOK(b0, &Base1::foo, Hooked_Base1_foo);
	if (INTERFACE_HOOKED) {
		hook = CALL_ORIGINAL_MEMBER(b0, hook) + 1;
	}
	return hook;
}

int __stdcall Hooked_Derived1_foo(Derived1* d0, int hook)
{
	UNIQUE_MEMBER_FUNCTION_HOOK(d0, &Derived1::foo, Hooked_Derived1_foo);
	if (INTERFACE_HOOKED) {
		hook = CALL_ORIGINAL_MEMBER(d0, hook) + 1;
	}
	return hook;
}

int (__stdcall Derived1::*Original_Derived1_foo1)(int hook);

int __stdcall Hooked_Derived1_foo1(Derived1* d0, int hook)
{
	hook = (d0->*Original_Derived1_foo1)(hook) + 1;
	return hook;
}

BOOST_AUTO_TEST_CASE(member_function_hook_1)
{
	boost::scoped_ptr<Derived1> d0(new Derived1);
	boost::scoped_ptr<Derived1> d1(new Derived1);
	
	Base1* b0 = d0.get();
	Base1* b1 = d1.get();
	
	BOOST_CHECK_EQUAL(d0->foo(0), 0);
	BOOST_CHECK_EQUAL(b0->foo(0), 0);
	BOOST_CHECK_EQUAL(d1->foo(0), 0);
	BOOST_CHECK_EQUAL(b1->foo(0), 0);
	abstract_hook_ptr d0Hook = MAKE_UNIQUE_MEMBER_HOOK(d0.get(), &Derived1::foo, Hooked_Derived1_foo, HookingFlags);
	BOOST_CHECK_EQUAL(d0->foo(0), 1);
	BOOST_CHECK_EQUAL(b0->foo(0), 1);
	BOOST_CHECK_EQUAL(d1->foo(0), 0);
	BOOST_CHECK_EQUAL(b1->foo(0), 0);
	abstract_hook_ptr d1Hook = MAKE_UNIQUE_MEMBER_HOOK(d1.get(), &Derived1::foo, Hooked_Derived1_foo, HookingFlags);
	BOOST_CHECK_EQUAL(d0->foo(0), 1);
	BOOST_CHECK_EQUAL(b0->foo(0), 1);
	BOOST_CHECK_EQUAL(d1->foo(0), 1);
	BOOST_CHECK_EQUAL(b1->foo(0), 1);
	d0Hook.reset();
	BOOST_CHECK_EQUAL(d0->foo(0), 0);
	BOOST_CHECK_EQUAL(b0->foo(0), 0);
	BOOST_CHECK_EQUAL(d1->foo(0), 1);
	BOOST_CHECK_EQUAL(b1->foo(0), 1);
	d1Hook.reset();
	BOOST_CHECK_EQUAL(d0->foo(0), 0);
	BOOST_CHECK_EQUAL(b0->foo(0), 0);
	BOOST_CHECK_EQUAL(d1->foo(0), 0);
	BOOST_CHECK_EQUAL(b1->foo(0), 0);

	BOOST_CHECK_EQUAL(d0->foo(0), 0);
	BOOST_CHECK_EQUAL(b0->foo(0), 0);
	BOOST_CHECK_EQUAL(d1->foo(0), 0);
	BOOST_CHECK_EQUAL(b1->foo(0), 0);
	abstract_hook_ptr b0Hook = MAKE_UNIQUE_MEMBER_HOOK(b0, &Base1::foo, Hooked_Base1_foo, HookingFlags);
	BOOST_CHECK_EQUAL(d0->foo(0), 1);
	BOOST_CHECK_EQUAL(b0->foo(0), 1);
	BOOST_CHECK_EQUAL(d1->foo(0), 0);
	BOOST_CHECK_EQUAL(b1->foo(0), 0);
	abstract_hook_ptr b1Hook = MAKE_UNIQUE_MEMBER_HOOK(b1, &Base1::foo, Hooked_Base1_foo, HookingFlags);
	BOOST_CHECK_EQUAL(d0->foo(0), 1);
	BOOST_CHECK_EQUAL(b0->foo(0), 1);
	BOOST_CHECK_EQUAL(d1->foo(0), 1);
	BOOST_CHECK_EQUAL(b1->foo(0), 1);
	b0Hook.reset();
	BOOST_CHECK_EQUAL(d0->foo(0), 0);
	BOOST_CHECK_EQUAL(b0->foo(0), 0);
	BOOST_CHECK_EQUAL(d1->foo(0), 1);
	BOOST_CHECK_EQUAL(b1->foo(0), 1);
	b1Hook.reset();
	BOOST_CHECK_EQUAL(d0->foo(0), 0);
	BOOST_CHECK_EQUAL(b0->foo(0), 0);
	BOOST_CHECK_EQUAL(d1->foo(0), 0);
	BOOST_CHECK_EQUAL(b1->foo(0), 0);
}