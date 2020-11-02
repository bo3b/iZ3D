/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

/************************************************************************/
/* Project  -> Diagnostic Tool                                          */
/* Authors  -> Andrew Kolpakov aka x0ras                                */
/* E-Mail   -> x0ras@neurok.ru                                          */
/************************************************************************/
/* Copyright (c) 2007 iZ3D Inc.                                          */
/************************************************************************/

#ifndef __DIAGTOOL_INFO_H__
#define __DIAGTOOL_INFO_H__

#include <string>

#include <dxdiag.h>
#include <errno.h>
#include <vector>

#include "Types.h"

class Info
{
public:

  struct DisplayInfo
  {
    char    DeviceName[100];
    char    Description[200];
    char    KeyDeviceID[200];
    char    KeyDeviceKey[200];
    char    Manufacturer[200];
    char    ChipType[100];
    char    DACType[100];
    char    Revision[100];
    char    DisplayMemoryEnglish[100];
    char    DisplayModeEnglish[100];

    dword   Width;
    dword   Height;
    dword   Bpp;
    dword   RefreshRate;

    char    MonitorName[100];
    char    MonitorMaxRes[100];

    char    DriverName[100];
    char    DriverVersion[100];
    char    DriverAttributes[100];
    char    DriverLanguageEnglish[100];
    char    DriverDateEnglish[100];
      
    dword   Adapter;
    char    VendorId[50];
    char    DeviceId[50];
       
    dword   ElementCount;
  };

  struct CPUExFB 
  {
    dword   Bits0_31; 
    dword   Bits32_63; 
    dword   Bits64_95; 
    dword   Bits96_127;
  };

  struct SysInfo
  {
    dword       OSMajorVersion;         
    dword       OSMinorVersion;         
    dword       OSBuildNumber;          
    dword       OSPlatformID;           
    dword       DirectXVersionMajor;    
    dword       DirectXVersionMinor;    
    char        DirectXVersionLetter[2]; 
    qword       PhysicalMemory;        
    qword       UsedPageFile;          
    qword       AvailPageFile;    
    bool        IsD3D8DebugRuntimeAvailable;
    bool        IsD3DDebugRuntime;
    long        D3DDebugLevel;
    char        WindowsDir[MAX_PATH];           
    char        BuildLab[100];                   
    char        DxDiagVersion[100];              
    char        ProcessorEnglish[200];           
    char        SystemManufacturerEnglish[200];  
    char        SystemModelEnglish[200];         
    char        BIOSEnglish[200];                 
    char        PhysicalMemoryEnglish[100];       
    char        DirectXVersionEnglish[100];       
    char        DirectXVersionLongEnglish[100];  
    char        MachineNameLocalized[200];        
    char        OSLocalized[100];                 
    char        OSExLocalized[100];               
    char        OSExLongLocalized[300];          
    char        PageFileLocalized[100];         
    char        MachineNameEnglish[200];         
    char        OSEnglish[100];                   
    char        OSExEnglish[100];                 
    char        OSExLongEnglish[300];             
    char        LanguagesEnglish[200];            
    char        PageFileEnglish[100];             
    char        TimeEnglish[100];                 
    CPUExFB     ExtFuncBitmasks[16]; 
    dword       ElementCount;
  };
  
  Info();
  ~Info();

  void WriteSystemInformation();
  void WriteDisplayInformation();
  //const std::string& GetInfo(); 
  

private:

  void GetSysInfo();
  void GetDisplayInfo();

  void GetValue(IDxDiagContainer *p_object, const wchar_t *p_property, uint *p_value);
  void GetValue(IDxDiagContainer *p_object, const wchar_t *p_property, dword *p_value);
  void GetValue(IDxDiagContainer *p_object, const wchar_t *p_property,  bool *p_value);
  void GetValue(IDxDiagContainer *p_object, const wchar_t *p_property,  qword *p_value);
  void GetValue(IDxDiagContainer *p_object, const wchar_t *p_property,  int *p_value);
  void GetValue(IDxDiagContainer *p_object, const wchar_t *p_property,  long *p_value);
  template <int LENGTH> void GetValue(IDxDiagContainer *p_object, const wchar_t *p_property, char (&p_value)[LENGTH]);


  SysInfo     sysInfo_;
  std::vector<DisplayInfo> displayInfo_;
  std::string cachedInfo_;
};


template <int LENGTH> void Info::GetValue(IDxDiagContainer *p_object, const wchar_t *p_property, char (&p_value)[LENGTH])
{
  VARIANT var;
  VariantInit( &var );

  p_value[0] = '\0';
  if(SUCCEEDED(p_object->GetProp( p_property, &var ) ) )
  {
	  if( var.vt != VT_BSTR )
		  throw WrongVariantException("The requesting value is not of type string!\n");

	  size_t size;
	  errno_t err = wcstombs_s(&size, p_value, LENGTH, var.bstrVal, _TRUNCATE);
  }
  //else
  //    throw SystemException("Error getting  DxDiag property!\n");
  VariantClear( &var );
}


#endif
