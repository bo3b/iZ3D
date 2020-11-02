#ifndef _IZ3DSHUTTERAPI_H_
#define _IZ3DSHUTTERAPI_H_

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include "windows.h"

S3DAPI_API BOOL	iZ3DInitShutter				();
S3DAPI_API void	iZ3DReleaseShutter			();

S3DAPI_API void	iZ3DStartShutter			();
S3DAPI_API void	iZ3DStopShutter				();
S3DAPI_API BOOL	iZ3DWaitForPresent			( DWORD dwMilliseconds_ );

void	iZ3DOutputDebugString		( const char* szOutputStr_ );

void	iZ3DStartTest				( UINT nTestID_ );

#endif//_IZ3DSHUTTERAPI_H_