/* IZ3D_FILE: $Id$ 
*
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D LLC 2002 - 2009
*
* $Author$
* $Revision$
* $Date$
* $LastChangedBy$
* $URL$
*/
#ifndef EMADevice_Included 
#define EMADevice_Included 

// _LIB is defined by Visual Studio for static libraries projects 
#ifdef _LIB 
    // building as a static library 
    #pragma comment( lib, "hid.lib" )
    #pragma comment( lib, "setupapi.lib" )
    #define EMADEVICE_DLL_API 
#else
    // not a lib: building either as the DLL itself or a client application 
    #ifdef EMADEVICE_DLL_EXPORTS
        // this is the DLL build itself 
        #pragma comment( lib, "hid.lib" )
        #pragma comment( lib, "setupapi.lib" )
        #define EMADEVICE_DLL_API __declspec(dllexport)
    #else
        // We are a client application 
        #ifdef EMA_STATIC_LINK 
           // we want static linkage
           #pragma comment( lib, "hid.lib" )
           #pragma comment( lib, "setupapi.lib" )
           #define EMADEVICE_DLL_API 
        #else 
           // We are a client app using dynamic link 
           // in such case, we do not need the Windows XP DDK libs 
           #define EMADEVICE_DLL_API __declspec(dllimport)
        #endif // static link 
    #endif // client app 
#endif // if lib 

// Error Return Codes                                             
// 
#define EMA_INVALID_PARAMETER             0x80000001L
#define EMA_INVALID_HANDLE                0x80000002L
#define EMA_OUTOFMEMORY                   0x80000003L
#define EMA_EMPTY_FILE_PATH               0x80000004L
#define EMA_READ_FAULT                    0x80000005L
#define EMA_WRITE_FAULT                   0x80000006L
#define EMA_BUFFER_TOO_SMALL              0x80000007L
#define EMA_WAIT_TIMEOUT                  0x80000008L
#define EMA_NOT_IMPLEMENTED               0x80000009L
#define EMA_HID_CALL_FAILURE              0x8000000AL
#define EMA_READ_DATA_FLAG_FAILURE        0x8000000BL
#define EMA_GET_AD_STD_DATA_FAILURE       0x8000000CL
#define EMA_STARTPOLL_FAILURE             0x8000000DL
#define EMA_NOT_SUPPORTED                 0x8000000EL
#define EMA_DEVICE_IS_NOT_CONNECTED       0x8000000FL
#define EMA_STEREO_MODE_IS_NOT_SUPPORTED  0x80000010L
#define EMA_INVALID_DISPLAY_TYPE          0x80000011L
#define EMA_INVALID_VALUE                 0x80000012L
#define EMA_INVALID_PROPERTY_ID           0x80000013L
#define EMA_ALREADY_ENABLED               0x80000014L
#define EMA_ALREADY_DISABLED              0x80000015L
#define EMA_MOTION_DETECTOR_FAILURE       0x80000016L
#define EMA_NO_DEVICE_FOUND             0x80000017L

// Enumerated Types
// 
typedef enum _EMADeviceType
{
    EMA_ALL = 0 ,
    EMA_Z800 ,
	EMA_Z800B ,
    EMA_X800 ,
} EMADeviceType ;

typedef enum _EMAFlags
{
    EMA_NONE = 0 ,
    EMA_NO_HEADTRACKING = 0x1 ,
} EMAFlags ;

typedef enum _EMADisplayType
{
    EMA_DONT_CARE = 0 ,
    EMA_MONO_DISPLAY,
    EMA_LEFT_DISPLAY,
    EMA_RIGHT_DISPLAY,
    EMA_BOTH_DISPLAY
} EMADisplayType;

typedef enum _EMAProperty
{
    // Eeprom properties 
    EMA_AC_TIMEOUT = 0 ,
    EMA_PHASE ,
    EMA_PIXELCLOCK ,
    EMA_USB_TIMEOUT ,
	EMA_RED_GAIN ,
	EMA_GREEN_GAIN ,
	EMA_BLUE_GAIN ,
	EMA_RED_GAIN_OFFSET ,
	EMA_GREEN_GAIN_OFFSET ,
	EMA_BLUE_GAIN_OFFSET ,
    EMA_ENABLE_EXTERNAL_MONITOR ,
	EMA_HORIZONTAL_POSITION ,
    EMA_VERTICAL_POSITION ,
	EMA_BRIGHTNESS_RAMP_0 ,
	EMA_BRIGHTNESS_RAMP_1 ,
	EMA_BRIGHTNESS_RAMP_2 ,

    // registry properties 
    EMA_MOUSE_TRACKING_SPEED_X , 
	EMA_MOUSE_TRACKING_SPEED_Y , 
	EMA_MOUSE_TRACKING_X_AXIS_DIRECTION , 
	EMA_MOUSE_TRACKING_Y_AXIS_DIRECTION , 

} EMAProperty;

// Structures                                              
//
#ifdef __cplusplus
extern "C" 
{ 
#endif

#pragma pack(push, 8)

typedef struct _EMADeviceCap
{
    unsigned short EMA_HEAD_TRACKER : 1;
    unsigned short EMA_MONOCULAR    : 1;
    unsigned short EMA_BINOCULAR    : 1;
    unsigned short EMA_CANDO3D      : 1;
} EMADeviceCap;


typedef struct _EMADeviceData
{
    float AccX ;
    float AccY ;
    float AccZ ;
    float AngX ;
    float AngY ;
    float AngZ ;
    float North ;
    float Yaw ;
    float Pitch ;
    float Roll ;

#ifdef __cplusplus
public:
	void ToDirectX ( )
	{
		// Convert to DirectX coordinate system
		float placeHolder ;
		placeHolder = AccX ;
		AccX = AccY ;
		AccY = - AccZ ;
		AccZ = - placeHolder ;

		placeHolder = AngX ;
		AngX = AngY ;
		AngY = - AngZ ;
		AngZ = - placeHolder ;

		Yaw = - Yaw ;
		Pitch = - Pitch ;
		Roll = - Roll ;
	}
#endif

} EMADeviceData ;

typedef struct _EMADeviceInfo
{
    EMADeviceType Type ;
    EMADeviceCap  Capabilities ;

    int        FirmwareVersion ;
	int        DisplayFrequency ;
	int        ScreenX ;
	int        ScreenY ;
    long       VendorID ;
    long       ProductID ;
    char       Path [ MAX_PATH ] ;
} EMADeviceInfo ;

typedef struct _EMAGainOffset
{
    BYTE RedGain ;
    BYTE GreenGain ;
    BYTE BlueGain ;
    BYTE RedOffset ;
    BYTE GreenOffset ;
    BYTE BlueOffset ;
} EMAGainOffset ;

#pragma pack(pop)

#ifdef __cplusplus
} // extern C  
#endif

// Functions : The C interface 
//
EXTERN_C EMADEVICE_DLL_API HRESULT GetEMADeviceVersion ( int * version , int * build ) ; 

EXTERN_C EMADEVICE_DLL_API HANDLE  OpenFirst ( ) ;
EXTERN_C EMADEVICE_DLL_API HANDLE  OpenFirstEx ( unsigned int flags ) ;
EXTERN_C EMADEVICE_DLL_API HANDLE  Open ( EMADeviceInfo * deviceInfo ) ;
EXTERN_C EMADEVICE_DLL_API HANDLE  OpenEx ( EMADeviceInfo * deviceInfo , unsigned int flags ) ;
EXTERN_C EMADEVICE_DLL_API void    Close ( HANDLE handle ) ;
EXTERN_C EMADEVICE_DLL_API HRESULT EnumerateDevices ( EMADeviceInfo * devInfos , int maxDevices, int * foundDevices ) ; 
EXTERN_C EMADEVICE_DLL_API HRESULT GetDeviceInfo ( HANDLE handle , EMADeviceInfo * devInfos ) ; 
EXTERN_C EMADEVICE_DLL_API HRESULT GetEMALastError ( ) ; 

// Mouse and head tracking 
// -----------------------
//
EXTERN_C EMADEVICE_DLL_API HRESULT EnableMouseTracking ( HANDLE handle , BOOL enable ) ;
EXTERN_C EMADEVICE_DLL_API HRESULT ResetMouseTracking ( HANDLE handle ) ;
EXTERN_C EMADEVICE_DLL_API HRESULT PollHeadTrackerRawData ( HANDLE handle , EMADeviceData * deviceData ) ; 

// 3D and Misc USB commands 
// ------------------------
//
EXTERN_C EMADEVICE_DLL_API HRESULT EnableExternalMonitor ( HANDLE handle, BOOL enable ) ;
EXTERN_C EMADEVICE_DLL_API HRESULT EnableStereoVision ( HANDLE handle , BOOL Enable ) ;
EXTERN_C EMADEVICE_DLL_API HRESULT GoSleep ( HANDLE handle ) ;
EXTERN_C EMADEVICE_DLL_API HRESULT KeepAlive ( HANDLE handle ) ;
EXTERN_C EMADEVICE_DLL_API HRESULT RestoreFactorySettings ( HANDLE handle ) ;
EXTERN_C EMADEVICE_DLL_API HRESULT ToggleLeftRightDisplays ( HANDLE handle ) ;
EXTERN_C EMADEVICE_DLL_API HRESULT CycleBrightnessLevel ( HANDLE handle ) ;
EXTERN_C EMADEVICE_DLL_API HRESULT ResetEEPROM ( HANDLE handle ) ;

// Properties 
// ----------
//
EXTERN_C EMADEVICE_DLL_API HRESULT GetProperty ( HANDLE handle , EMADisplayType Display , EMAProperty PropertyID , int * Value ) ;
EXTERN_C EMADEVICE_DLL_API HRESULT SetProperty ( HANDLE handle , EMADisplayType Display , EMAProperty PropertyID , int Value ) ;
EXTERN_C EMADEVICE_DLL_API HRESULT GetPropertyRange ( EMAProperty PropertyID , int * MinValue , int * MaxValue ) ;
EXTERN_C EMADEVICE_DLL_API HRESULT RestorePropertyFactorySetting ( HANDLE handle , EMADisplayType Display , EMAProperty PropertyID ) ;
EXTERN_C EMADEVICE_DLL_API HRESULT GetPropertyFactorySetting ( EMAProperty PropertyID , int * propertyDefault ) ;

// Gain and Offset 
// ---------------
//
EXTERN_C EMADEVICE_DLL_API HRESULT GetGainOffset ( HANDLE handle , EMADisplayType Display , EMAGainOffset * Data ) ;
EXTERN_C EMADEVICE_DLL_API HRESULT SetGainOffset ( HANDLE handle , EMADisplayType Display , EMAGainOffset * Data ) ;
EXTERN_C EMADEVICE_DLL_API HRESULT GetGainRange ( BYTE * MinGain , BYTE * MaxGain ) ;
EXTERN_C EMADEVICE_DLL_API HRESULT GetOffsetRange ( BYTE * MinOffset , BYTE * MaxOffset ) ;
EXTERN_C EMADEVICE_DLL_API HRESULT GetGainOffsetFactorySetting ( BYTE * defaultGain , BYTE * defaultOffset ) ;
EXTERN_C EMADEVICE_DLL_API HRESULT RestoreGainOffsetFactorySetting ( HANDLE handle , EMADisplayType display ) ;

// Class 

#ifdef __cplusplus

class EMADEVICE_DLL_API EMADevice
{
private:
    void * InternalConstructor ( EMADeviceInfo * deviceInfo , unsigned int flags ) ; 
public:
    EMADevice ( ) ;
    EMADevice ( EMADeviceInfo * deviceInfo ) ;
    EMADevice ( EMAFlags flags ) ;
    EMADevice ( EMADeviceInfo * deviceInfo , EMAFlags flags ) ;
	~EMADevice ( ) ;

    void * operator new ( size_t size ) ;
    void operator delete ( void * p ) ;

    static HRESULT GetLastError ( void ) ;
	static HRESULT GetVersion ( int * version , int * build ) ;
	static HRESULT EnumerateDevices ( EMADeviceInfo * devInfos , int maxDevices, int * foundDevices ) ;
    HRESULT GetDeviceInfo ( EMADeviceInfo * devInfos ) ; 

    // Mouse and head tracking 
    // -----------------------
    //
    HRESULT EnableMouseTracking ( bool enable ) ;
	HRESULT ResetMouseTracking ( ) ;
    HRESULT PollHeadTrackerRawData ( EMADeviceData * deviceData ) ; 

    // 3D and Misc USB commands 
    // ------------------------
    //
    HRESULT EnableExternalMonitor ( bool enable ) ;
	HRESULT EnableStereoVision ( bool enable ) ;
	HRESULT GoSleep ( ) ;
	HRESULT KeepAlive ( ) ;
	HRESULT ToggleLeftRightDisplays ( ) ;
    HRESULT CycleBrightnessLevel ( ) ;
	HRESULT ResetEEPROM ( ) ;

    // Properties 
    // ----------
    //
	HRESULT GetProperty ( EMADisplayType display , EMAProperty propertyID , int * propertyValue ) ;
	HRESULT SetProperty ( EMADisplayType display , EMAProperty propertyID , int propertyValue ) ;
	static HRESULT GetPropertyRange ( EMAProperty PropertyID , int * minValue , int * maxValue ) ;
	static HRESULT GetPropertyFactorySetting ( EMAProperty propertyID , int * propertyDefault ) ;
	HRESULT RestorePropertyFactorySetting ( EMADisplayType display , EMAProperty propertyID ) ;
   	HRESULT RestoreFactorySettings ( ) ;

    // Gain and Offset 
    // ---------------
    //
    HRESULT GetGainOffset ( EMADisplayType Display , EMAGainOffset * data ) ;
	HRESULT SetGainOffset ( EMADisplayType display , EMAGainOffset * data ) ;
	static HRESULT GetGainRange ( BYTE * minGain , BYTE * maxGain ) ;
	static HRESULT GetOffsetRange ( BYTE * minOffset , BYTE * maxOffset ) ;
	static HRESULT GetGainOffsetFactorySetting ( BYTE * defaultGain , BYTE * defaultOffset ) ;
	HRESULT RestoreGainOffsetFactorySetting ( EMADisplayType display ) ;

} ;

#endif

#endif
