#pragma once
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <map>
#include <vector>
#include <set>

// hard to forward windows & direct input primitives
#define DIRECTINPUT_VERSION 0x0800
#include <windef.h>
#include <dinput.h>

class ScalingAgent;
class ScalingHook;
typedef boost::shared_ptr<ScalingHook>		ScalingHookPtrT;
typedef boost::weak_ptr<ScalingHook>		ScalingHookWPtrT;
typedef boost::shared_ptr<ScalingAgent>		ScalingAgentPtrT;
typedef boost::weak_ptr<ScalingAgent>		ScalingAgentWPtrT;

/** Holds information about interfaces for scaling. Singleton. */
class ScalingAgent
{
friend class ScalingHook;
public:
	struct device_info
	{
		bool				ansiDevice;
		std::vector<size_t> xOffsets;
		std::vector<size_t> yOffsets;
		POINT				prevPos;
	};
	
private:
	typedef std::set<HWND>					hwnd_set;
	typedef std::map<void*, device_info>	device_info_map;

public:
	S3DAPI_API static ScalingAgentPtrT	Instance();
	S3DAPI_API static ScalingAgentPtrT	Initialize();
	S3DAPI_API ScalingHookPtrT			InitializeHook(TDisplayScalingMode mode, SIZE originalSize, SIZE scaledSize);
	S3DAPI_API ScalingHookPtrT			Hook();

	S3DAPI_API void	AddHWND(HWND hWnd);
	S3DAPI_API void	RemoveHWND(HWND hWnd);
	S3DAPI_API bool AffectHWND(HWND hWnd) const;
	S3DAPI_API bool AffectMonitor(HMONITOR hMonitor) const;
	
	template<typename IDevice> S3DAPI_API void RegisterDevice(IDevice* device, LPCDIDATAFORMAT format);
	template<typename IDevice> S3DAPI_API void RemoveDevice(IDevice* device);
	template<typename IDevice> S3DAPI_API bool AffectDevice(IDevice* device) const;
	template<typename IDevice> S3DAPI_API device_info& GetDeviceInfo(IDevice* device);

private:
	template<typename IDevice>
	device_info GatherDeviceInfo(LPCDIDATAFORMAT);

private:
	static ScalingAgentWPtrT	instance;
	hwnd_set					wnds;
	device_info_map				devices;
	ScalingHookWPtrT			hook;
};

