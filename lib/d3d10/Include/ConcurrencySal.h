/********************************************************************************
* Copyright (c) Microsoft Corporation. All Rights Reserved.
* 
* File: ConcurrencySal.h
*
* Comments: This file contains macros for Concurrency SAL annotations. Definitions
* starting with __internal are low level macros that are subject to change. 
* Users should not use those low level macros directly.
*********************************************************************************/
#pragma once

#if !defined(__midl) && defined(_PREFAST_) && _MSC_VER >= 1000

/*
 * pre-defined system locks
 */
extern int __system_interlock;
extern int __system_cancel_spinlock;
extern int __system_critical_region;

/*
 * Declare annotations.
 * Fail soft (by doing nothing) if necessary.
 */
#ifndef __ANNOTATION
#define __ANNOTATION(x) /* nothing */
#endif

__ANNOTATION(SAL_guarded_by(__xin __deferTypecheck void *));
__ANNOTATION(SAL_write_guarded_by(__xin __deferTypecheck void *));
__ANNOTATION(SAL_requires_lock_held(__xin __deferTypecheck void *));
__ANNOTATION(SAL_requires_shared_lock_held(__xin __deferTypecheck void *));
__ANNOTATION(SAL_requires_lock_not_held(__xin __deferTypecheck void *));
__ANNOTATION(SAL_requires_no_locks_held(void));
__ANNOTATION(SAL_acquires_lock(__xin __deferTypecheck void *));
__ANNOTATION(SAL_acquires_exclusive_lock(__xin __deferTypecheck void *));
__ANNOTATION(SAL_acquires_shared_lock(__xin __deferTypecheck void *));
__ANNOTATION(SAL_releases_lock(__xin __deferTypecheck void *));
__ANNOTATION(SAL_releases_exclusive_lock(__xin __deferTypecheck void *));
__ANNOTATION(SAL_releases_shared_lock(__xin __deferTypecheck void *));
__ANNOTATION(SAL_ignore_lock_match(__xin __deferTypecheck void *));
__ANNOTATION(SAL_has_lock_property(__xin char *));
__ANNOTATION(SAL_declare_lock_level(__xin char *));
__ANNOTATION(SAL_has_lock_level(__xin char *));
__ANNOTATION(SAL_lock_level_order(__xin __deferTypecheck void *, __xin __deferTypecheck void *));
__ANNOTATION(SAL_no_competing_thread(__xin __deferTypecheck void *));
__ANNOTATION(SAL_lock_implementation(__xin __deferTypecheck void *));
__ANNOTATION(SAL_requires_lock_equivalent(__xin __deferTypecheck void *, __xin __deferTypecheck void *));
__ANNOTATION(SAL_ensures_lock_equivalent(__xin __deferTypecheck void *, __xin __deferTypecheck void *));

/*
 * data protection
 */
#define __guarded_by(lock) __declspec("SAL_guarded_by(" # lock ")")
#define __write_guarded_by(lock) __declspec("SAL_write_guarded_by(" # lock ")")
#define __interlocked __guarded_by(__system_interlock)

/*
 * caller/callee locking contracts
 */
#define __requires_lock_held(lock) __post __declspec("SAL_requires_lock_held(" # lock ")")
#define __requires_exclusive_lock_held(lock) __post __declspec("SAL_requires_lock_held(" # lock ")")
#define __requires_shared_lock_held(lock) __post __declspec("SAL_requires_shared_lock_held(" # lock ")")

#define __requires_lock_not_held(lock) __post __declspec("SAL_requires_lock_not_held(" # lock ")")
#define __requires_no_locks_held __post __declspec("SAL_requires_no_locks_held")

/*
 * acquire/release locking side effects
 */
#define __acquires_lock(lock) __post __declspec("SAL_acquires_lock(" # lock ")")
#define __acquires_exclusive_lock(lock) __post __declspec("SAL_acquires_exclusive_lock(" # lock ")")
#define __acquires_shared_lock(lock) __post __declspec("SAL_acquires_shared_lock(" # lock ")")

#define __releases_lock(lock) __post __declspec("SAL_releases_lock(" # lock ")")
#define __releases_exclusive_lock(lock) __post __declspec("SAL_releases_exclusive_lock(" # lock ")")
#define __releases_shared_lock(lock) __post __declspec("SAL_releases_shared_lock(" # lock ")")

#define __requires_lock_equivalent(a,b) __post __declspec("SAL_requires_lock_equivalent(" #a "," #b ")")
#define __ensures_lock_equivalent(a,b) __post __declspec("SAL_ensures_lock_equivalent(" #a "," #b ")")


/*
 * lock properties
 * The following kind options are supported:
 * __has_lock_property(MUTEX)
 * __has_lock_property(EVENT)
 * __has_lock_property(SEMAPHORE)
 * __has_lock_property(OTHER_HANDLE)
 * __has_lock_property(REENTRANT)
 * __has_lock_property(NON_REENTRANT)
 */
#define __has_lock_property(kind) __declspec( "SAL_has_lock_property(" # kind ")" )

/*
 * lock level
 */
#define __declare_lock_level(level) __declspec( "SAL_declare_lock_level(" # level ")" )
#define __has_lock_level(level) __declspec( "SAL_has_lock_level(" # level ")" )

#define __internal_lock_level_order(a,b) __declspec("SAL_lock_level_order(" #a "," #b ")")
#define CSALCAT1(x,y) x##y
#define CSALCAT2(x,y) CSALCAT1(x,y)

#define __lock_level_order(a,b) \
    extern __internal_lock_level_order(a,b) void __sal_order_##a##_##b(void*a,void*b); \
    static __inline void CSALCAT2(CSAL_LockOrder,__COUNTER__)(void){__sal_order_##a##_##b(#a,#b);}

/*
 * threading context
 */
#define __no_competing_thread __post __declspec("SAL_no_competing_thread")

/*
 * refinement and suppression
 */
extern __acquires_lock(*plock) void __internal_acquires_lock(void* plock);
extern __releases_lock(*plock) void __internal_releases_lock(void* plock);

#define __analysis_assume_lock_acquired(lock) __internal_acquires_lock((void*)(&lock))
#define __analysis_assume_lock_released(lock) __internal_releases_lock((void*)(&lock))

#define __function_ignore_lock_checking(lock) __post __declspec("SAL_ignore_lock_match(" # lock ")")
extern __function_ignore_lock_checking(*plock) void __internal_suppress_lock_checking(void* plock);

#define __analysis_suppress_lock_checking(lock) __internal_suppress_lock_checking((void*)(&(lock)));
#define __lock_implementation __post __declspec("SAL_lock_implementation")

#define BENIGN_RACE_BEGIN __pragma(warning(push)) __pragma(warning(disable:26100 26150 26130 26180 26131 26181))
#define BENIGN_RACE_END __pragma(warning(pop))

#define NO_COMPETING_THREAD_BEGIN __pragma(warning(push)) __pragma(warning(disable:26100 26150 26101 26151 26110 26160 26130 26180 26131 26181))
#define NO_COMPETING_THREAD_END __pragma(warning(pop))

#else

#define __guarded_by(lock)
#define __write_guarded_by(lock)
#define __interlocked
#define __requires_lock_held(lock)
#define __requires_exclusive_lock_held(lock)
#define __requires_shared_lock_held(lock)
#define __requires_lock_not_held(lock)
#define __requires_no_locks_held
#define __acquires_lock(lock)
#define __acquires_exclusive_lock(lock)
#define __acquires_shared_lock(lock)
#define __releases_lock(lock)
#define __releases_exclusive_lock(lock)
#define __releases_shared_lock(lock)

#define __requires_lock_equivalent(lock1,lock2)
#define __ensures_lock_equivalent(lock1,lock2)

#define __has_lock_property(kind)
#define __declare_lock_level(level)
#define __has_lock_level(level)
#define __internal_lock_level_order(a,b)
#define CSALCAT1(x,y)
#define CSALCAT2(x,y)
#define __lock_level_order(a,b)
#define __no_competing_thread
#define __analysis_assume_lock_acquired(lock)
#define __analysis_assume_lock_released(lock)
#define __function_ignore_lock_checking(lock)
#define __analysis_suppress_lock_checking(lock) 
#define __lock_implementation

#define BENIGN_RACE_BEGIN __pragma(warning(push))
#define BENIGN_RACE_END __pragma(warning(pop))

#define NO_COMPETING_THREAD_BEGIN __pragma(warning(push))
#define NO_COMPETING_THREAD_END __pragma(warning(pop))

#endif


