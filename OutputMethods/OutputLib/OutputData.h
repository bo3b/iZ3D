/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the OUTPUT_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// OUTPUT_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef OUTPUT_EXPORTS
#define OUTPUT_API __declspec(dllexport)
#else
#define OUTPUT_API __declspec(dllimport)
#endif

enum OutputDllCaps
{
	odResetGDIGammaRamp		= 0x01,
	//--- begin shutter flags group ---
	odShutterMode			= 0x02,
	odShutterModeSimpleOnly	= 0x04,
	odShutterModeAMDOnly	= 0x08,
	odShutterModeKMAllowed	= 0x10,
	//--- end   shutter flags group ---
};

enum OutputMethodCaps
{
	ocHardwareMouseCursorNotSupported	= 0x01,
	ocNeedMultiThreadedDevice			= 0x02,
	ocOutputFromTexture					= 0x04,
	ocRenderDirectlyToBB				= 0x08,
	ocRequiredDepthMap					= 0x10,
};

enum SecondWindowCaps
{
	swcInvertHorizontal = 1,
	swcInvertVertical	= 2,
};
