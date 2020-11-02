#include "stdafx.h"

TLS long g_InternalCall = 0;

OurMethodCallGuard::OurMethodCallGuard( void )
{
	InterlockedIncrement(&g_InternalCall); 
	//DEBUG_TRACE2(_T("WE_CALL %d\n"), g_InternalCall);
}

OurMethodCallGuard::~OurMethodCallGuard(void) 
{ 
	InterlockedDecrement(&g_InternalCall); 
	//DEBUG_TRACE2(_T("~WE_CALL %d\n"), g_InternalCall); 
}

bool IsInternalCall()
{
	return g_InternalCall > 0;
}