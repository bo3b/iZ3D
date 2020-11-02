/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#define TLS  __declspec( thread )

bool IsInternalCall();

class OurMethodCallGuard
{
public:
	OurMethodCallGuard(void);
	~OurMethodCallGuard(void);
};

#define WE_CALL OurMethodCallGuard cg
