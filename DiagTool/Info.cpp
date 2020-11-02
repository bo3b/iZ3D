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
/************************************************************************/
/* Project  -> Diagnostic Tool                                          */
/* Authors  -> Andrew Kolpakov aka x0ras                                */
/* E-Mail   -> x0ras@neurok.ru                                          */
/************************************************************************/
/* Copyright (c) 2007 iZ3D Inc.                                          */
/************************************************************************/

#include <sstream>
#include <time.h>
#include <math.h>

#include "Info.h"
#include "Error.h"
#include "Logger.h"
#include "d3d9.h"
#include "..\Shared\version.h"
#include "ProductNames.h"
#include "..\CommonUtils\System.h"
#include <HighLevelMonitorConfigurationAPI.h>

Info::Info()
{

}

Info::~Info()
{
  
}

void Info::WriteSystemInformation()
{
  Logger::Get() << "--------------------\nSystem Information\n---------------------\n\n";
  
  GetSysInfo();

  struct tm newtime;
  time_t ltime;
  char buf[26];

  time( &ltime );
  gmtime_s( &newtime, &ltime );
  asctime_s(buf, 26, &newtime);

  Logger::Get() << "Time of this report: " << buf << "\n";
  Logger::Get() << PRODUCT_NAME << " Version: " << VERSION << "\n";
  //Logger::Get() << "OS Extended String (Localized): " << sysInfo_.OSExLocalized << "\n";
  Logger::Get() << "OS Extended Long String (Localized): " << sysInfo_.OSExLongLocalized << "\n";
  Logger::Get() << "OS Version: " << sysInfo_.OSMajorVersion << "." << sysInfo_.OSMinorVersion << "." << sysInfo_.OSBuildNumber  << "." << sysInfo_.OSPlatformID << "\n";
  if (!IsWin64()) {
	  Logger::Get() << "System type: 32-bit\n";
  } else {
	  Logger::Get() << "System type: 64-bit\n";
  }
  Logger::Get() << "DirectX Version Long String (English): " << sysInfo_.DirectXVersionLongEnglish << "\n";
  Logger::Get() << "DirectX Version: " << sysInfo_.DirectXVersionMajor << "." << sysInfo_.DirectXVersionMinor  << sysInfo_.DirectXVersionLetter << "\n";
  Logger::Get() << "Physical Memory: " << sysInfo_.PhysicalMemory/1024/1024 << "(Mb)\n";
  Logger::Get() << "Used PageFile: " << sysInfo_.UsedPageFile/1024/1024 << "(Mb)\n";
  Logger::Get() << "Available PageFile: " << sysInfo_.AvailPageFile/1024/1024 << "(Mb)\n";
  Logger::Get() << "Direct3D8 Debug Runtime Available: " << sysInfo_.IsD3D8DebugRuntimeAvailable << "\n";
  Logger::Get() << "Direct3D Debug Runtime Running: " << sysInfo_.IsD3DDebugRuntime << "\n";
  Logger::Get() << "Direct3D Debug Level: " << sysInfo_.D3DDebugLevel << "\n";
  Logger::Get() << "Windows Directory: " << sysInfo_.WindowsDir << "\n";
  Logger::Get() << "BuildLab: " << sysInfo_.BuildLab << "\n";
  Logger::Get() << "DxDiag Version: " << sysInfo_.DxDiagVersion << "\n";
  Logger::Get() << "Processor String (English): " << sysInfo_.ProcessorEnglish << "\n";
  Logger::Get() << "System Manufacturer String (English): " << sysInfo_.SystemManufacturerEnglish << "\n";
  Logger::Get() << "System Model String (English): " << sysInfo_.SystemModelEnglish << "\n";
  Logger::Get() << "BIOS String (English): " << sysInfo_.BIOSEnglish << "\n";
  Logger::Get() << "Physical Memory String (English): " << sysInfo_.PhysicalMemoryEnglish << "\n";
  Logger::Get() << "DirectX Version String (English): " << sysInfo_.DirectXVersionEnglish << "\n";
  //Logger::Get() << "Machine Name (Localized): " << sysInfo_.MachineNameLocalized << "\n";
  //Logger::Get() << "OS String (Localized): " << sysInfo_.OSLocalized << "\n";
  Logger::Get() << "PageFile String (Localized): " << sysInfo_.PageFileLocalized << "\n";
  Logger::Get() << "MachineName (English): " << sysInfo_.MachineNameEnglish << "\n";
  Logger::Get() << "Languages String (English): " << sysInfo_.LanguagesEnglish << "\n";
  Logger::Get() << "PageFile String (English): " << sysInfo_.PageFileEnglish << "\n";
  Logger::Get() << "Time String (English): " << sysInfo_.TimeEnglish << "\n";
}

BOOL GetMonitorSizeFromEDID(DISPLAY_DEVICEA& ddMon, DWORD& Width, DWORD& Height)
{
	//read edid
	bool result = false;
	Width = 0;
	Height = 0;

	CHAR model[8];
	CHAR* s = strchr(ddMon.DeviceID, '\\') + 1;
	size_t len = strchr(s, '\\') - s;
	if (len >= _countof(model))
		len = _countof(model) - 1;
	strncpy_s(model, s, len);

	CHAR *path = strchr(ddMon.DeviceID, '\\') + 1;
	CHAR str[MAX_PATH] = "SYSTEM\\CurrentControlSet\\Enum\\DISPLAY\\";
	strncat_s(str, path, strchr(path, '\\')-path);
	path = strchr(path, '\\') + 1;
	HKEY hKey;
	if(RegOpenKeyExA(HKEY_LOCAL_MACHINE, str, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		DWORD i = 0;
		DWORD size = MAX_PATH;
		FILETIME ft;
		while(RegEnumKeyExA(hKey, i, str, &size, NULL, NULL, NULL, &ft) == ERROR_SUCCESS)
		{
			HKEY hKey2;
			if(RegOpenKeyExA(hKey, str, 0, KEY_READ, &hKey2) == ERROR_SUCCESS)
			{
				size = MAX_PATH;
				if(RegQueryValueExA(hKey2, "Driver", NULL, NULL, (LPBYTE)&str, &size) == ERROR_SUCCESS)
				{
					if (strcmp(str, path) == 0)
					{
						HKEY hKey3;
						if(RegOpenKeyExA(hKey2, "Device Parameters", 0, KEY_READ, &hKey3) == ERROR_SUCCESS)
						{
							BYTE EDID[256];
							size = 256;
							if(RegQueryValueExA(hKey3, "EDID", NULL, NULL, (LPBYTE)&EDID, &size) == ERROR_SUCCESS)
							{
								DWORD p = 8;
								CHAR model2[9];

								char byte1 = EDID[p];
								char byte2 = EDID[p+1];
								model2[0]=((byte1 & 0x7C) >> 2) + 64;
								model2[1]=((byte1 & 3) << 3) + ((byte2 & 0xE0) >> 5) + 64;
								model2[2]=(byte2 & 0x1F) + 64;
								sprintf(model2 + 3, "%X%X%X%X", (EDID[p+3] & 0xf0) >> 4, EDID[p+3] & 0xf, (EDID[p+2] & 0xf0) >> 4, EDID[p+2] & 0x0f);
								if (strcmp(model, model2) == 0)
								{
									Width = EDID[21];
									Height = EDID[22];
									result = true;
								}
								else
								{
									// EDID incorrect
								}
							}
							RegCloseKey(hKey3);
						}
					}
				}
				RegCloseKey(hKey2);
			}
			i++;
		}
		RegCloseKey(hKey);
	}

	return result;
}

typedef BOOL (WINAPI *GetNumberOfPhysicalMonitorsFromHMONITOR_t)( HMONITOR hMonitor, LPDWORD pdwNumberOfPhysicalMonitors);
typedef BOOL (WINAPI *GetPhysicalMonitorsFromHMONITOR_t)( HMONITOR hMonitor, DWORD dwPhysicalMonitorArraySize, LPPHYSICAL_MONITOR pPhysicalMonitorArray);
typedef BOOL (WINAPI *GetMonitorCapabilities_t)( HANDLE hMonitor, LPDWORD pdwMonitorCapabilities, LPDWORD pdwSupportedColorTemperatures );
typedef BOOL (WINAPI *GetMonitorTechnologyType_t)( HANDLE hMonitor, LPMC_DISPLAY_TECHNOLOGY_TYPE pdtyDisplayTechnologyType );
typedef BOOL (WINAPI *DestroyPhysicalMonitors_t)( DWORD dwPhysicalMonitorArraySize, LPPHYSICAL_MONITOR pPhysicalMonitorArray );
typedef BOOL (WINAPI *GetMonitorDisplayAreaSize_t)( HANDLE hMonitor, MC_SIZE_TYPE stSizeType,
			  LPDWORD pdwMinimumWidthOrHeight, LPDWORD pdwCurrentWidthOrHeight, LPDWORD pdwMaximumWidthOrHeight );

#define FOURCC_DF16 ((D3DFORMAT) MAKEFOURCC('D','F','1','6'))
#define FOURCC_DF24 ((D3DFORMAT) MAKEFOURCC('D','F','2','4'))
#define FOURCC_RAWZ ((D3DFORMAT) MAKEFOURCC('R','A','W','Z'))
#define FOURCC_INTZ ((D3DFORMAT) MAKEFOURCC('I','N','T','Z'))
#define FOURCC_AQBS ((D3DFORMAT) MAKEFOURCC('A','Q','B','S'))

void Info::WriteDisplayInformation()
{
  Logger::Get() << "\n--------------------\nDisplay Information\n---------------------\n\n";

  GetDisplayInfo();

  for (int i = 0; i < (int)displayInfo_.size(); i++)
  {
	  Logger::Get() << "Display adapter: " << i << "\n";
	  Logger::Get() << "Device name: " << displayInfo_[i].DeviceName << "\n";
	  Logger::Get() << "Description: " <<  displayInfo_[i].Description << "\n";
	  Logger::Get() << "Key device ID: " << displayInfo_[i].KeyDeviceID << "\n";
	  Logger::Get() << "Key device key: " << displayInfo_[i].KeyDeviceKey << "\n";
	  Logger::Get() << "Manufacturer: " << displayInfo_[i].Manufacturer << "(Mb)\n";
	  Logger::Get() << "Chip type: " << displayInfo_[i].ChipType << "(Mb)\n";
	  Logger::Get() << "DACType: " << displayInfo_[i].DACType << "(Mb)\n";
	  Logger::Get() << "Revision: " << displayInfo_[i].Revision << "\n";
	  Logger::Get() << "DisplayMemory (English): " << displayInfo_[i].DisplayMemoryEnglish << "\n";
	  Logger::Get() << "DisplayMode (English): " << displayInfo_[i].DisplayModeEnglish << "\n";
	  Logger::Get() << "Screen width: " << displayInfo_[i].Width << "\n";
	  Logger::Get() << "Screen height: " << displayInfo_[i].Height << "\n";
	  Logger::Get() << "Bits per pixel: " << displayInfo_[i].Bpp << "\n";
	  Logger::Get() << "Refresh rate: " << displayInfo_[i].RefreshRate << "\n";
	  Logger::Get() << "Monitor name: " << displayInfo_[i].MonitorName << "\n";
	  Logger::Get() << "Monitor maximum resolution: " << displayInfo_[i].MonitorMaxRes << "\n";
	  Logger::Get() << "Driver name: " << displayInfo_[i].DriverName << "\n";
	  Logger::Get() << "Driver version: " << displayInfo_[i].DriverVersion << "\n";
	  Logger::Get() << "Driver attributes: " << displayInfo_[i].DriverAttributes << "\n";
	  Logger::Get() << "DriverLanguage (English): " << displayInfo_[i].DriverLanguageEnglish << "\n";
	  Logger::Get() << "DriverDate (English): " << displayInfo_[i].DriverDateEnglish << "\n";
	  Logger::Get() << "Adapter: " << displayInfo_[i].Adapter << "\n";
	  Logger::Get() << "VendorId: " << displayInfo_[i].VendorId << "\n";
	  Logger::Get() << "DeviceId: " << displayInfo_[i].DeviceId << "\n";

	  HMODULE hDXVA2 = LoadLibrary(L"dxva2.dll");
	  GetNumberOfPhysicalMonitorsFromHMONITOR_t GetNumberOfPhysicalMonitorsFromHMONITOR = NULL;
	  GetPhysicalMonitorsFromHMONITOR_t GetPhysicalMonitorsFromHMONITOR = NULL;
	  GetMonitorCapabilities_t GetMonitorCapabilities = NULL;
	  GetMonitorTechnologyType_t GetMonitorTechnologyType = NULL;
	  GetMonitorDisplayAreaSize_t GetMonitorDisplayAreaSize = NULL;
	  DestroyPhysicalMonitors_t DestroyPhysicalMonitors = NULL;

	  if (hDXVA2)
	  {
		  GetNumberOfPhysicalMonitorsFromHMONITOR = (GetNumberOfPhysicalMonitorsFromHMONITOR_t)GetProcAddress(hDXVA2, "GetNumberOfPhysicalMonitorsFromHMONITOR");
		  GetPhysicalMonitorsFromHMONITOR = (GetPhysicalMonitorsFromHMONITOR_t)GetProcAddress(hDXVA2, "GetPhysicalMonitorsFromHMONITOR");
		  GetMonitorCapabilities = (GetMonitorCapabilities_t)GetProcAddress(hDXVA2, "GetMonitorCapabilities");
		  GetMonitorTechnologyType = (GetMonitorTechnologyType_t)GetProcAddress(hDXVA2, "GetMonitorTechnologyType");
		  GetMonitorDisplayAreaSize = (GetMonitorDisplayAreaSize_t)GetProcAddress(hDXVA2, "GetMonitorDisplayAreaSize");
		  DestroyPhysicalMonitors = (DestroyPhysicalMonitors_t)GetProcAddress(hDXVA2, "DestroyPhysicalMonitors");

		  if (!GetNumberOfPhysicalMonitorsFromHMONITOR ||
			  !GetPhysicalMonitorsFromHMONITOR ||
			  !GetMonitorCapabilities ||
			  !GetMonitorTechnologyType ||
			  !GetMonitorDisplayAreaSize ||
			  !DestroyPhysicalMonitors)
		  {
			  FreeLibrary(hDXVA2);
			  hDXVA2 = NULL;
		  }
	  }

	  DISPLAY_DEVICEA	MonitorDevice;
	  MonitorDevice.cb = sizeof(DISPLAY_DEVICEA);
	  char model[8] = "";
	  for( int j=0; EnumDisplayDevicesA(displayInfo_[i].DeviceName, j, &MonitorDevice, 0x0 ); j++ ) {
		  if( MonitorDevice.StateFlags & DISPLAY_DEVICE_ACTIVE &&
			  MonitorDevice.StateFlags & DISPLAY_DEVICE_ATTACHED) // for ATI, Windows XP
		  {
			  char* s = strchr(MonitorDevice.DeviceID, '\\') + 1;
			  size_t len = strchr(s, '\\') - s;
			  Logger::Get() << "Attached Monitor DeviceId: " << MonitorDevice.DeviceID << "\n";
			  if (len < _countof(model))
			  {
				  strncpy_s(model, _countof(model), s, len); 
				  Logger::Get() << "Attached Monitor Model: " << model << "\n";
			  }

			  DWORD Width, Height;
			  if (GetMonitorSizeFromEDID(MonitorDevice, Width, Height))
			  {
				  float diag = sqrt((Width / 2.54f) * (Width / 2.54f) + (Height / 2.54f) * (Height / 2.54f));
				  Logger::Get() << "Monitor display area size: " << Width << "cm x " << Height << "cm (diag " << diag << "\")\n";
			  }

			  if (hDXVA2)
			  {
				  // get information about the display's position and the current display mode
				  DEVMODEA dm;
				  ZeroMemory(&dm, sizeof(dm));
				  dm.dmSize = sizeof(dm);
				  if (EnumDisplaySettingsExA(MonitorDevice.DeviceName, ENUM_CURRENT_SETTINGS, &dm, 0) == FALSE)
					  EnumDisplaySettingsExA(MonitorDevice.DeviceName, ENUM_REGISTRY_SETTINGS, &dm, 0);

				  POINT pt = { dm.dmPosition.x, dm.dmPosition.y };
				  HMONITOR hMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONULL);

				  if (hMonitor)
				  {
					  DWORD dwNumberOfPhysicalMonitors = 0;
					  BOOL result = GetNumberOfPhysicalMonitorsFromHMONITOR(
						  hMonitor, &dwNumberOfPhysicalMonitors);

					  PHYSICAL_MONITOR* pPhysicalMonitorArray = new PHYSICAL_MONITOR[dwNumberOfPhysicalMonitors];
					  result = GetPhysicalMonitorsFromHMONITOR(
						  hMonitor, dwNumberOfPhysicalMonitors, pPhysicalMonitorArray);

					  DWORD dwMonitorCapabilities = 0;
					  DWORD dwSupportedColorTemperatures = 0;
					  result = GetMonitorCapabilities(
						  pPhysicalMonitorArray[0].hPhysicalMonitor, &dwMonitorCapabilities, &dwSupportedColorTemperatures);
					
					  if (dwMonitorCapabilities & MC_CAPS_MONITOR_TECHNOLOGY_TYPE)
					  {
						  MC_DISPLAY_TECHNOLOGY_TYPE type = MC_SHADOW_MASK_CATHODE_RAY_TUBE;
						  result = GetMonitorTechnologyType(
							  pPhysicalMonitorArray[0].hPhysicalMonitor, &type);
						  Logger::Get() << "Monitor technology: ";
						  switch(type)
						  {
						  case MC_SHADOW_MASK_CATHODE_RAY_TUBE:
							  Logger::Get() << "CRT (shadow mask)\n";
							  break;
						  case MC_APERTURE_GRILL_CATHODE_RAY_TUBE:
							  Logger::Get() << "CRT (aperture grill)\n";
							  break;
						  case MC_THIN_FILM_TRANSISTOR:
							  Logger::Get() << "TFT\n";
							  break;
						  case MC_LIQUID_CRYSTAL_ON_SILICON:
							  Logger::Get() << "LCOS\n";
							  break;
						  case MC_PLASMA:
							  Logger::Get() << "Plasma\n";
							  break;
						  case MC_ORGANIC_LIGHT_EMITTING_DIODE:
							  Logger::Get() << "LED\n";
							  break;
						  case MC_ELECTROLUMINESCENT:
							  Logger::Get() << "Electroluminescent display\n";
							  break;
						  case MC_MICROELECTROMECHANICAL:
							  Logger::Get() << "Microelectromechanical display\n";
							  break;
						  case MC_FIELD_EMISSION_DEVICE:
							  Logger::Get() << "FED\n";
							  break;
						  }
					  }

					  if (dwMonitorCapabilities & MC_CAPS_DISPLAY_AREA_SIZE)
					  {
						  DWORD dwMinimumWidth = 0;
						  DWORD dwCurrentWidth = 0;
						  DWORD dwMaximumWidth = 0;
						  DWORD dwMinimumHeight = 0;
						  DWORD dwCurrentHeight = 0;
						  DWORD dwMaximumHeight = 0;
						  result = GetMonitorDisplayAreaSize(
							  pPhysicalMonitorArray[0].hPhysicalMonitor, MC_WIDTH,
							  &dwMinimumWidth, &dwCurrentWidth, &dwMaximumWidth);
						  result = GetMonitorDisplayAreaSize(
							  pPhysicalMonitorArray[0].hPhysicalMonitor, MC_HEIGHT,
							  &dwMinimumHeight, &dwCurrentHeight, &dwMaximumHeight);
						  Logger::Get() << "Monitor display area size: " << dwCurrentWidth << " x " << dwCurrentHeight << "\n";
					  }

					  result = DestroyPhysicalMonitors(
						  dwNumberOfPhysicalMonitors, pPhysicalMonitorArray);

					  delete [] pPhysicalMonitorArray;
				  }
			  }
		  }
	  }
	  if (hDXVA2)
		  FreeLibrary(hDXVA2);

	  IDirect3D9* pDirect3D = Direct3DCreate9(D3D_SDK_VERSION);
	  if (pDirect3D)
	  {
		  if (SUCCEEDED(pDirect3D->CheckDeviceFormat( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,	
			  D3DFMT_X8R8G8B8, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_TEXTURE, D3DFMT_D16)))
			  Logger::Get() << "DS texture format D16: supported\n";
		  else
			  Logger::Get() << "DS texture format D16: unsupported\n";
		  if (SUCCEEDED(pDirect3D->CheckDeviceFormat( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,	
			  D3DFMT_X8R8G8B8, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_TEXTURE, D3DFMT_D16_LOCKABLE)))
			  Logger::Get() << "DS texture format D16 LOCKABLE: supported\n";
		  else
			  Logger::Get() << "DS texture format D16 LOCKABLE: unsupported\n";
		  if (SUCCEEDED(pDirect3D->CheckDeviceFormat( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,	
			  D3DFMT_X8R8G8B8, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_TEXTURE, D3DFMT_D24S8)))
			  Logger::Get() << "DS texture format D24S8: supported\n";
		  else
			  Logger::Get() << "DS texture format D24S8: unsupported\n";
		  if (SUCCEEDED(pDirect3D->CheckDeviceFormat( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,	
			  D3DFMT_X8R8G8B8, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_TEXTURE, D3DFMT_D24X8)))
			  Logger::Get() << "DS texture format D24X8: supported\n";
		  else
			  Logger::Get() << "DS texture format D24X8: unsupported\n";

		  if (SUCCEEDED(pDirect3D->CheckDeviceFormat( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,	
			  D3DFMT_X8R8G8B8, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_TEXTURE, FOURCC_DF16)))
			  Logger::Get() << "DS texture format ATI DF16: supported\n";
		  else
			  Logger::Get() << "DS texture format ATI DF16: unsupported\n";
		  if (SUCCEEDED(pDirect3D->CheckDeviceFormat( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,	
			  D3DFMT_X8R8G8B8, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_TEXTURE, FOURCC_DF24)))
			  Logger::Get() << "DS texture format ATI DF24: supported\n";
		  else
			  Logger::Get() << "DS texture format ATI DF24: unsupported\n";

		  if (SUCCEEDED(pDirect3D->CheckDeviceFormat( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,	
			  D3DFMT_X8R8G8B8, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_TEXTURE, FOURCC_INTZ)))
			  Logger::Get() << "DS texture format NVidia INTZ: supported\n";
		  else
			  Logger::Get() << "DS texture format NVidia INTZ: unsupported\n";
		  if (SUCCEEDED(pDirect3D->CheckDeviceFormat( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,	
			  D3DFMT_X8R8G8B8, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_TEXTURE, FOURCC_RAWZ)))
			  Logger::Get() << "DS texture format NVidia RAWZ: supported\n";
		  else
			  Logger::Get() << "DS texture format NVidia RAWZ: unsupported\n";

		  if (SUCCEEDED(pDirect3D->CheckDeviceFormat( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,	
			  D3DFMT_X8R8G8B8, 0, D3DRTYPE_SURFACE, FOURCC_AQBS)))
			  Logger::Get() << "Surface format ATI AQBS: supported\n";
		  else
			  Logger::Get() << "Surface format ATI AQBS: unsupported\n";
		  pDirect3D->Release();
	  }
	  Logger::Get() << "\n";
  } 
  Logger::Get().Flush(); 
}
/*
const std::string& Info::GetInfo()
{
	if (!cachedInfo_.empty())
		cachedInfo_.clear();
	
	GetSysInfo();

	std::stringstream ss;

	ss << "OS Major Version: " << sysInfo_.OSMajorVersion << "\n";
	ss << "OS Minor Version: " << sysInfo_.OSMinorVersion << "\n";
	ss << "OS Build Number: " << sysInfo_.OSBuildNumber << "\n";
	ss << "OS Platform ID: " << sysInfo_.OSPlatformID << "\n";
	ss << "DirectX Major Version: " << sysInfo_.DirectXVersionMajor << "\n";
	ss << "DirectX Minor Version: " << sysInfo_.DirectXVersionMinor << "\n";
	ss << "DirectX Version Letter: " << sysInfo_.DirectXVersionLetter << "\n";
	ss << "Physical Memory: " << sysInfo_.PhysicalMemory << "\n";
	ss << "Used PageFile: " << sysInfo_.UsedPageFile << "\n";
	ss << "Available PageFile: " << sysInfo_.AvailPageFile << "\n";
	ss << "Direct3D8 Debug Runtime Available: " << sysInfo_.IsD3D8DebugRuntimeAvailable << "\n";
	ss << "Direct3D Debug Runtime Running: " << sysInfo_.IsD3DDebugRuntime << "\n";
	ss << "Direct3D Debug Level: " << sysInfo_.D3DDebugLevel << "\n";
	ss << "Windows Directory: " << sysInfo_.WindowsDir << "\n";
	ss << "BuildLab: " << sysInfo_.BuildLab << "\n";
	ss << "DxDiag Version: " << sysInfo_.DxDiagVersion << "\n";
	ss << "Processor String (English): " << sysInfo_.ProcessorEnglish << "\n";
	ss << "System Manufacturer String (English): " << sysInfo_.SystemManufacturerEnglish << "\n";
	ss << "System Model String (English): " << sysInfo_.SystemModelEnglish << "\n";
	ss << "BIOS String (English): " << sysInfo_.BIOSEnglish << "\n";
	ss << "Physical Memory String (English): " << sysInfo_.PhysicalMemoryEnglish << "\n";
	ss << "DirectX Version String (English): " << sysInfo_.DirectXVersionEnglish << "\n";
	ss << "DirectX Version Long String (English): " << sysInfo_.DirectXVersionLongEnglish << "\n";
	ss << "Machine Name (Localized): " << sysInfo_.MachineNameLocalized << "\n";
	ss << "OS String (Localized): " << sysInfo_.OSLocalized << "\n";
	ss << "OS Extended String (Localized): " << sysInfo_.OSExLocalized << "\n";
	ss << "OS Extended Long String (Localized): " << sysInfo_.OSExLongLocalized << "\n";
	ss << "PageFile String (Localized): " << sysInfo_.PageFileLocalized << "\n";
	ss << "MachineName (English): " << sysInfo_.MachineNameEnglish << "\n";
	ss << "OS String (English): " << sysInfo_.OSEnglish << "\n";
	ss << "OS Extended String (English): " << sysInfo_.OSExEnglish << "\n";
	ss << "OS Extended Long String (English): " << sysInfo_.OSExLongEnglish << "\n";
	ss << "Languages String (English): " << sysInfo_.LanguagesEnglish << "\n";
	ss << "PageFile String (English): " << sysInfo_.PageFileEnglish << "\n";
	ss << "Time String (English): " << sysInfo_.TimeEnglish << "\n";

	return ss.str();
}
*/
void Info::GetSysInfo()
{
  HRESULT hResult;
  IDxDiagProvider *p_provider;
  IDxDiagContainer *p_container, *p_object;

  hResult = CoInitialize(null);

  //ERROR_OUTPUT(hResult, "Error calling \"CoInitialize\"!\n");

  hResult = CoCreateInstance(CLSID_DxDiagProvider, null, CLSCTX_INPROC_SERVER, IID_IDxDiagProvider, reinterpret_cast<void**>(&p_provider));

  ERROR_OUTPUT(hResult, "Error calling \"CoCreateInstance\"!\n");

  DXDIAG_INIT_PARAMS initParams;

  initParams.bAllowWHQLChecks       = false;
  initParams.dwDxDiagHeaderVersion  = DXDIAG_DX9_SDK_VERSION;
  initParams.pReserved              = null;
  initParams.dwSize                 = sizeof(DXDIAG_INIT_PARAMS);

  hResult = p_provider->Initialize(&initParams);

  ERROR_OUTPUT(hResult, "Error initializing DxDiagProvider!\n");

  hResult = p_provider->GetRootContainer(&p_container);

  ERROR_OUTPUT(hResult, "Error getting root container!\n");

  hResult = p_container->GetChildContainer(L"DxDiag_SystemInfo", &p_object);

  ERROR_OUTPUT(hResult, "Error getting system information from DxDiag");

  GetValue( p_object, L"dwOSMajorVersion", &sysInfo_.OSMajorVersion ); 
  GetValue( p_object, L"dwOSMinorVersion", &sysInfo_.OSMinorVersion );
  GetValue( p_object, L"dwOSBuildNumber", &sysInfo_.OSBuildNumber );
  GetValue( p_object, L"dwOSPlatformID", &sysInfo_.OSPlatformID );
  GetValue( p_object, L"dwDirectXVersionMajor", &sysInfo_.DirectXVersionMajor );
  GetValue( p_object, L"dwDirectXVersionMinor", &sysInfo_.DirectXVersionMinor );
  GetValue( p_object, L"szDirectXVersionLetter", sysInfo_.DirectXVersionLetter);
  GetValue( p_object, L"ullPhysicalMemory", &sysInfo_.PhysicalMemory );
  GetValue( p_object, L"ullUsedPageFile", &sysInfo_.UsedPageFile );
  GetValue( p_object, L"ullAvailPageFile", &sysInfo_.AvailPageFile );
  GetValue( p_object, L"bIsD3D8DebugRuntimeAvailable", &sysInfo_.IsD3D8DebugRuntimeAvailable );
  GetValue( p_object, L"bIsD3DDebugRuntime", &sysInfo_.IsD3DDebugRuntime );
  GetValue( p_object, L"nD3DDebugLevel", &sysInfo_.D3DDebugLevel );
  GetValue( p_object, L"szWindowsDir", sysInfo_.WindowsDir);
  GetValue( p_object, L"szBuildLab", sysInfo_.BuildLab);
  GetValue( p_object, L"szDxDiagVersion", sysInfo_.DxDiagVersion);
  GetValue( p_object, L"szProcessorEnglish", sysInfo_.ProcessorEnglish);
  GetValue( p_object, L"szSystemManufacturerEnglish", sysInfo_.SystemManufacturerEnglish);
  GetValue( p_object, L"szSystemModelEnglish", sysInfo_.SystemModelEnglish);
  GetValue( p_object, L"szBIOSEnglish", sysInfo_.BIOSEnglish);
  GetValue( p_object, L"szPhysicalMemoryEnglish", sysInfo_.PhysicalMemoryEnglish);
  GetValue( p_object, L"szDirectXVersionEnglish", sysInfo_.DirectXVersionEnglish);
  GetValue( p_object, L"szDirectXVersionLongEnglish", sysInfo_.DirectXVersionLongEnglish);
  GetValue( p_object, L"szMachineNameLocalized", sysInfo_.MachineNameLocalized);
  GetValue( p_object, L"szOSLocalized", sysInfo_.OSLocalized);
  GetValue( p_object, L"szOSExLocalized", sysInfo_.OSExLocalized);
  GetValue( p_object, L"szOSExLongLocalized", sysInfo_.OSExLongLocalized);
  GetValue( p_object, L"szPageFileLocalized", sysInfo_.PageFileLocalized);
  GetValue( p_object, L"szMachineNameEnglish", sysInfo_.MachineNameEnglish);
  GetValue( p_object, L"szOSEnglish", sysInfo_.OSEnglish);
  GetValue( p_object, L"szOSExEnglish", sysInfo_.OSExEnglish);
  GetValue( p_object, L"szOSExLongEnglish", sysInfo_.OSExLongEnglish);
  GetValue( p_object, L"szLanguagesEnglish", sysInfo_.LanguagesEnglish);
  GetValue( p_object, L"szPageFileEnglish", sysInfo_.PageFileEnglish);
  GetValue( p_object, L"szTimeEnglish", sysInfo_.TimeEnglish);
 

  if (p_container)
	p_container->Release();

  if (p_object)
	p_object->Release();

  if (p_provider)
	p_provider->Release();
}

void Info::GetDisplayInfo()
{
  IDxDiagProvider *p_provider;
  IDxDiagContainer* p_container = 0, *p_object = 0, *p_child = 0;
  displayInfo_.clear();

  HRESULT hResult = CoCreateInstance(CLSID_DxDiagProvider, null, CLSCTX_INPROC_SERVER, IID_IDxDiagProvider, reinterpret_cast<void**>(&p_provider));

  ERROR_OUTPUT(hResult, "Error calling \"CoCreateInstance\"!\n");

  DXDIAG_INIT_PARAMS initParams;

  initParams.bAllowWHQLChecks       = false;
  initParams.dwDxDiagHeaderVersion  = DXDIAG_DX9_SDK_VERSION;
  initParams.pReserved              = null;
  initParams.dwSize                 = sizeof(DXDIAG_INIT_PARAMS);

  hResult = p_provider->Initialize(&initParams);

  ERROR_OUTPUT(hResult, "Error initializing DxDiagProvider!\n");

  hResult = p_provider->GetRootContainer(&p_container);

  ERROR_OUTPUT(hResult, "Error getting root container!\n");

  hResult = p_container->GetChildContainer(L"DxDiag_DisplayDevices", &p_child);

  ERROR_OUTPUT(hResult, "Error getting system information from DxDiag");

  unsigned long instances;
  
  hResult = p_child->GetNumberOfChildContainers(&instances);

  ERROR_OUTPUT(hResult, "Error getting child container instances");

  for (UINT i = 0; i < instances; i++)
  {
	  wchar_t containerName[256];

	  hResult = p_child->EnumChildContainerNames(i, containerName, 256);

	  ERROR_OUTPUT(hResult, "Error enumerating container names");

	  hResult = p_child->GetChildContainer(containerName, &p_object);

	  ERROR_OUTPUT(hResult, "Error getting last child container");

	  DisplayInfo info;
	  GetValue( p_object, L"szDeviceName", info.DeviceName ); 
	  GetValue( p_object, L"szDescription", info.Description );
	  GetValue( p_object, L"szKeyDeviceID", info.KeyDeviceID );
	  GetValue( p_object, L"szKeyDeviceKey", info.KeyDeviceKey );
	  GetValue( p_object, L"szManufacturer", info.Manufacturer );
	  GetValue( p_object, L"szChipType", info.ChipType );
	  GetValue( p_object, L"szDACType", info.DACType);
	  GetValue( p_object, L"szRevision", info.Revision );
	  GetValue( p_object, L"szDisplayMemoryEnglish", info.DisplayMemoryEnglish );
	  GetValue( p_object, L"szDisplayModeEnglish", info.DisplayModeEnglish );
	  GetValue( p_object, L"dwWidth", &info.Width );
	  GetValue( p_object, L"dwHeight", &info.Height );
	  GetValue( p_object, L"dwBpp", &info.Bpp );
	  GetValue( p_object, L"dwRefreshRate", &info.RefreshRate);
	  GetValue( p_object, L"szMonitorName", info.MonitorName );
	  GetValue( p_object, L"szMonitorMaxRes", info.MonitorMaxRes );
	  GetValue( p_object, L"szDriverName", info.DriverName);
	  GetValue( p_object, L"szDriverVersion", info.DriverVersion );
	  GetValue( p_object, L"szDriverAttributes", info.DriverAttributes);
	  GetValue( p_object, L"szDriverLanguageEnglish", info.DriverLanguageEnglish );
	  GetValue( p_object, L"szDriverDateEnglish", info.DriverDateEnglish );
	  GetValue( p_object, L"iAdapter", &info.Adapter );
	  GetValue( p_object, L"szVendorId", info.VendorId );
	  GetValue( p_object, L"szDeviceId", info.DeviceId );
	  displayInfo_.push_back(info); 
  } 
}

void Info::GetValue(IDxDiagContainer *p_object, const wchar_t *p_property, bool  *p_value)
{
  VARIANT var;

  VariantInit(&var);

  if (FAILED(p_object->GetProp(p_property, &var)))
	throw SystemException("Error getting  DxDiag property!\n");

  if (var.vt != VT_BOOL)
	throw WrongVariantException("The requesting value is not of type !\n");

  *p_value = (var.boolVal != 0);

  VariantClear(&var);
}

void Info::GetValue(IDxDiagContainer *p_object, const wchar_t *p_property, uint  *p_value)
{
  VARIANT var;

  VariantInit(&var);

  if (FAILED(p_object->GetProp(p_property, &var)))
	throw SystemException("Error getting  DxDiag property!\n");

  if (var.vt != VT_I4)
	throw WrongVariantException("The requesting value is not of type 32!\n");

  *p_value = var.lVal;

  VariantClear(&var);
}

void Info::GetValue(IDxDiagContainer *p_object, const wchar_t *p_property, qword *p_value)
{
  VARIANT var;

  VariantInit(&var);

  if (FAILED(p_object->GetProp(p_property, &var)))
	throw SystemException("Error getting  DxDiag property!\n");

  if (var.vt != VT_BSTR)
	throw WrongVariantException("The requesting value is not of type qword!\n");

  *p_value = _wtoi64(var.bstrVal);

  VariantClear(&var);
}



void Info::GetValue(IDxDiagContainer *p_object, const wchar_t *p_property, dword *p_value)
{
  VARIANT var;

  VariantInit(&var);

  if (FAILED(p_object->GetProp(p_property, &var)))
	throw SystemException("Error getting  DxDiag property!\n");

  if (var.vt != VT_UI4)
	throw WrongVariantException("The requesting value is not of type 32!\n");

  *p_value = var.lVal;

  VariantClear(&var);
}

void Info::GetValue(IDxDiagContainer *p_object, const wchar_t *p_property, long *p_value)
{
  VARIANT var;

  VariantInit(&var);

  if (FAILED(p_object->GetProp(p_property, &var)))
	throw SystemException("Error getting  DxDiag property!\n");

  if (var.vt != VT_I4)
	throw WrongVariantException("The requesting value is not of type 32!\n");

  *p_value = var.lVal;

  VariantClear(&var);
}
