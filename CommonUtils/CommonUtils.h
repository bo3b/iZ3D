#pragma once

#ifdef COMMONUTILS_EXPORTS
#define COMMONUTILS __declspec(dllexport)
#else
#define COMMONUTILS __declspec(dllimport)
#endif