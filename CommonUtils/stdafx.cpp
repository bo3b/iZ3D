/* IZ3D_FILE: $Id$ 
*
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*
* $Author$
* $Revision$
* $Date$
* $LastChangedBy$
* $URL$
*/
// stdafx.cpp : source file that includes just the standard includes
// CommonUtils.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

#if 0 // just for test
#include "Singleton.h"
#include "PerThreadSingleton.h"

class Test1 : public iz3d::Singleton<Test1>
{
	friend class iz3d::Singleton<Test1>;
public:
	int x;
};

class Test2 : public iz3d::PerThreadSingleton<Test2>
{
	friend class iz3d::PerThreadSingleton<Test2>;
public:
	int x;
};

#endif
