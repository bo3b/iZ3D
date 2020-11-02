#pragma once
#include <boost/intrusive_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <map>
#include <set>

// hard to forward windows & direct input primitives
#define DIRECTINPUT_VERSION 0x0800
#include <windef.h>
#include <dinput.h>
#include "HookAPI.h"

class abstract_hook;
typedef boost::intrusive_ptr<abstract_hook> abstract_hook_ptr;

class ScalingHook;
class ScalingAgent;
typedef boost::shared_ptr<ScalingAgent>		ScalingAgentPtrT;
typedef boost::shared_ptr<ScalingHook>		ScalingHookPtrT;
typedef boost::weak_ptr<ScalingHook>		ScalingHookWPtrT;

/** Hooks windows functions gathering display information to deceive app about actual resolution. Singleton. */
class ScalingHook
{
friend class ScalingAgent;
private:
	typedef std::map<HWND, WNDPROC>	wndproc_map;

	struct DInputDeviceHook
	{
		abstract_hook_ptr	getDeviceStateHook;
		abstract_hook_ptr	getDeviceDataHook;
	};
	typedef std::map<void*, DInputDeviceHook>	device_hook_map;
	
public:
	S3DAPI_API ~ScalingHook();

	S3DAPI_API void		IgnoreModeSwitch(bool toggle)	{ ignoreModeSwitch = toggle; }
	S3DAPI_API bool		IsModeSwithIgnored() 			{ return ignoreModeSwitch; }
	S3DAPI_API void		Setup(TDisplayScalingMode mode, SIZE originalSize, SIZE scaledSize);

public:
	ScalingHook(ScalingAgentPtrT agent, TDisplayScalingMode mode, SIZE originalSize, SIZE scaledSize);
	
	bool	HookWndProc(HWND hWnd);
	WNDPROC	GetWndProc(HWND hWnd);
	WNDPROC	ChangeWndProc(HWND hWnd, WNDPROC wndProc);
	void	UnhookWndProc(HWND hWnd);
	
	template<typename IDevice> bool HookDevice(IDevice* device);
	template<typename IDevice> void UnhookDevice(IDevice* device);

public:
	SIZE	originalSize;
	RECT	nativeRect;
	RECT	srcRect;
	RECT	dstRect;
	RECT	lastClipCursor;
	POINT	cursorPos;

private:
	ScalingAgentPtrT	agent;
	hook_storage		hooks;
	wndproc_map			wndProcs;
	device_hook_map		deviceHooks;
	bool				ignoreModeSwitch;
};

