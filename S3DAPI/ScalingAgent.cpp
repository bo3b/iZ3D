#include "stdafx.h"
#include <madCHook.h>
#include "ScalingAgent.h"
#include "ScalingHook.h"

namespace {	

	template<typename IDevice>
	struct is_device_ansi;

	template<> struct is_device_ansi<IDirectInputDevice8A> { static const bool value = true; };
	template<> struct is_device_ansi<IDirectInputDevice8W> { static const bool value = false; };

} // anonymous namespace
 
ScalingAgentWPtrT ScalingAgent::instance;

ScalingAgentPtrT ScalingAgent::Instance()
{
	return instance.lock();
}

ScalingAgentPtrT ScalingAgent::Initialize()
{
	ScalingAgentPtrT lockedInst = instance.lock();
	if (!lockedInst) 
	{
		lockedInst.reset(new ScalingAgent);
		instance = lockedInst;
	}
	return lockedInst;
}

ScalingHookPtrT ScalingAgent::InitializeHook(TDisplayScalingMode mode, SIZE originalSize, SIZE scaledSize)
{
	ScalingHookPtrT lockedHook = hook.lock();
	if (!lockedHook) 
	{
		lockedHook.reset( new ScalingHook(instance.lock(), mode, originalSize, scaledSize) );
		hook = lockedHook;

		// hook wnd procs
		std::for_each(wnds.begin(), wnds.end(), [&lockedHook](HWND hWnd) { lockedHook->HookWndProc(hWnd); } );

		// hook devices
		for (auto iter = devices.begin(); iter != devices.end(); ++iter)
		{
			if (iter->second.ansiDevice) {
				lockedHook->HookDevice( (IDirectInputDevice8A*)iter->first );
			}
			else {
				lockedHook->HookDevice( (IDirectInputDevice8W*)iter->first );
			}
		}
	}
	else {
		lockedHook->Setup(mode, originalSize, scaledSize);
	}

	return lockedHook;
}

ScalingHookPtrT	ScalingAgent::Hook() 
{ 
	return hook.lock(); 
}

void ScalingAgent::AddHWND(HWND hWnd)
{
	if (wnds.insert(hWnd).second)
	{
		if ( ScalingHookPtrT lockedHook = hook.lock() ) {
			lockedHook->HookWndProc(hWnd);
		}
	}
}

void ScalingAgent::RemoveHWND(HWND hWnd)
{
	if ( AffectHWND(hWnd) )
	{
		if ( ScalingHookPtrT lockedHook = hook.lock() ) {
			lockedHook->UnhookWndProc(hWnd);
		}
		wnds.erase(hWnd);
	}
}

bool ScalingAgent::AffectHWND(HWND hWnd) const
{
	return (wnds.count(hWnd) > 0);
}

bool ScalingAgent::AffectMonitor(HMONITOR hMonitor) const
{
	// check every monitor which is overlapped by application window
	for(auto iter = wnds.begin(); iter != wnds.end(); ++iter)
	{
		HMONITOR wndMonitor = MonitorFromWindow(*iter, MONITOR_DEFAULTTONULL);
		if (hMonitor == wndMonitor) {
			return true;
		}
	}

	return false;
}

template<typename IDevice>
ScalingAgent::device_info ScalingAgent::GatherDeviceInfo(LPCDIDATAFORMAT format)
{
	/*
	std::ostringstream ss;
	ss << format->dwSize << " ? " << sizeof(DIDATAFORMAT) << ";" << format->dwObjSize << " ? " << sizeof(DIOBJECTDATAFORMAT);
	if (format->dwSize != sizeof(DIDATAFORMAT) || format->dwObjSize != sizeof(DIOBJECTDATAFORMAT)) {
		MessageBoxA(0, ss.str().c_str(), "SD", MB_OK);
	}
	*/
	device_info info;
	info.ansiDevice = is_device_ansi<IDevice>::value;
	for (size_t i = 0; i<format->dwNumObjs; ++i)
	{
		//std::ostringstream ss;
		//ss << "pguid: " << format->rgodf[i].pguid << "; &GUID_XAxis == " << &GUID_XAxis << "; &GUID_YAxis == " << &GUID_YAxis;
		//MessageBoxA(0, ss.str().c_str(), "SD", MB_OK);
		if (format->rgodf[i].pguid)
		{
			if (*format->rgodf[i].pguid == GUID_XAxis) {
				info.xOffsets.push_back(format->rgodf[i].dwOfs);
			}
			else if (*format->rgodf[i].pguid == GUID_YAxis) {
				info.yOffsets.push_back(format->rgodf[i].dwOfs);
			}
		}
	}

	return info;
}

template<typename IDevice>
void ScalingAgent::RegisterDevice(IDevice* device, LPCDIDATAFORMAT format)
{
	device_info info = GatherDeviceInfo<IDevice>(format);
	if ( info.xOffsets.empty() && info.yOffsets.empty() ) {
		return;
	}

	auto iter = devices.find(device);
	if ( iter != devices.end() ) {
		iter->second = info;
	}
	else {	
		devices.insert( device_info_map::value_type(device, info) );
	}

	if ( ScalingHookPtrT lockedHook = hook.lock() ) {
		lockedHook->HookDevice(device);
	}
}

template<typename IDevice>
void ScalingAgent::RemoveDevice(IDevice* device)
{
	if ( AffectDevice(device) )
	{
		if ( ScalingHookPtrT lockedHook = hook.lock() ) {
			lockedHook->UnhookDevice(device);
		}
		devices.erase(device);
	}
}

template<typename IDevice>
bool ScalingAgent::AffectDevice(IDevice* device) const
{
	return (devices.count(device) > 0);
}

template<typename IDevice>
ScalingAgent::device_info& ScalingAgent::GetDeviceInfo(IDevice* device)
{
	return devices.find(device)->second;
}

template void ScalingAgent::RegisterDevice<IDirectInputDevice8A>(IDirectInputDevice8A* device, LPCDIDATAFORMAT format);
template void ScalingAgent::RemoveDevice<IDirectInputDevice8A>(IDirectInputDevice8A* device);
template bool ScalingAgent::AffectDevice<IDirectInputDevice8A>(IDirectInputDevice8A* device) const;
template ScalingAgent::device_info& ScalingAgent::GetDeviceInfo<IDirectInputDevice8A>(IDirectInputDevice8A* device);

template void ScalingAgent::RegisterDevice<IDirectInputDevice8W>(IDirectInputDevice8W* device, LPCDIDATAFORMAT format);
template void ScalingAgent::RemoveDevice<IDirectInputDevice8W>(IDirectInputDevice8W* device);
template bool ScalingAgent::AffectDevice<IDirectInputDevice8W>(IDirectInputDevice8W* device) const;
template ScalingAgent::device_info& ScalingAgent::GetDeviceInfo<IDirectInputDevice8W>(IDirectInputDevice8W* device);