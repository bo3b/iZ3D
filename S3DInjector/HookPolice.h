#pragma once

extern bool isHookPoliceWorking;
HANDLE __cdecl StartHookPolice( PVOID* pNextHooksToRenew );