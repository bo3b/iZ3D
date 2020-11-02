/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "S3DAPI.h"
#include "Logging.h"
#include "DXGIType.h"
#include <tchar.h>
#include "iZ3DShutterServiceAPI.h"

//--- camera default values ---
#define DEFAULT_FOV			(D3DX_PI / (3.0f + 1.0f / 3.0f)) // 54 degrees
#define DEFAULT_ZNEAR		1.f
#define DEFAULT_ZFAR		1e+7f
#define DEFAULT_ONE_DIV_ZPS	1.f / (1.f + DEFAULT_ZNEAR)
#define STEP_STEREOBASE		PERCENT_TO_SEPARATION(2.5f)
#define STEP_ONE_DIV_ZPS	1.f			//--- in pixels ---
#define MIN_ONE_DIV_ZPS	   -1e+7f
#define MAX_ONE_DIV_ZPS		1e+7f
#define STEP_MULTIPLIER		0.001f

#define MAX_STEREOBASE				40.f
#define SEPARATION_MULTIPLAYER		(MAX_STEREOBASE / 50.f / 100.f)
#define SEPARATION_TO_PERCENT(x)	(x / SEPARATION_MULTIPLAYER)
#define PERCENT_TO_SEPARATION(x)	(x * SEPARATION_MULTIPLAYER)
#define DEFAULT_STEREOBASE			PERCENT_TO_SEPARATION(20)

enum RouterType
{
	ROUTER_TYPE_INTERFACE      = 0,
	ROUTER_TYPE_HOOK           = 1,
	ROUTER_TYPE_DISABLE_DRIVER = 2
};

enum DeviceModes
{
	DEVICE_MODE_FORCEFULLSCREEN,			//--- force fullscreen ---
	DEVICE_MODE_FORCEWINDOWED,				//--- force windowed ---
	DEVICE_MODE_RESERVED,
	DEVICE_MODE_AUTO,						//--- auto select on creation ---
};

enum MultiWindowsModes
{
	MULTI_WINDOWS_MODE_AUTO,						//--- auto select on creation ---
	MULTI_WINDOWS_MODE_MULTIHEAD,					//--- dual-head mode ---
	MULTI_WINDOWS_MODE_SWAPCHAINS,					//--- multi swap-chains windowed mode ---
	MULTI_WINDOWS_MODE_APPLICATION,					//--- single window mode as application set ---
	MULTI_WINDOWS_MODE_SIDEBYSIDEWINDOWED			//--- like DEVICE_MODE_SWAPCHAINS but on one monitor ---
};

enum PresenterMode
{
	SHUTTER_MODE_MULTITHREADED,
	SHUTTER_MODE_MULTIDEVICE,
	SHUTTER_MODE_ATIQB,
	SHUTTER_MODE_UMHOOK_D3D9,
	SHUTTER_MODE_UMHOOK_D3D10 = SHUTTER_MODE_UMHOOK_D3D9,
	SHUTTER_MODE_SIMPLE,
	SHUTTER_MODE_UMHOOK_D3D9EX,
	SHUTTER_MODE_IZ3DKMSERVICE,						//--- kernel mode iZ3D shutter service
	SHUTTER_MODE_MAX_VALUE = SHUTTER_MODE_IZ3DKMSERVICE,
	SHUTTER_MODE_AUTO = -1,
	SHUTTER_MODE_DISABLED = -2
};

enum SecondWindowMessage
{
	SWM_MOVE_SECOND_WINDOW,
	SWM_SHOW_SECOND_WINDOW,
	SWM_HIDE_SECOND_WINDOW,
	SWM_MOVE_APP_WINDOW,
	SWM_RESTORE_APP_WINDOW,
};

enum TDisplayScalingMode
{
	DISPLAY_SCALING_NONE,					// 0 == No Scaling (or Scaling Unknown)
	DISPLAY_SCALING_TOP_LEFT,				// 1 == Image started from top left corner. (black padding right bottom)
	DISPLAY_SCALING_SCALED,					// 2 == Scaling (N.B. Aspect Ratio may change)
	DISPLAY_SCALING_CENTERED,				// 3 == Centering (black padding around)
	DISPLAY_SCALING_SCALEDASPECT,			// 4 == Scaling (Fixed Aspect Ratio Touch-from-inside, black bars may appear)
	DISPLAY_SCALING_SCALEDASPECTEX			// 5 == Scaling (Fixed Aspect Ratio Touch-from-outside, some data loss is possible)
};

struct CameraPreset
{
	float StereoBase;
	float One_div_ZPS;
	bool  AutoFocusEnable;
	float One_div_ZPS_Saved;				//--- save One_div_ZPS before autofocus enabled ---
											//--- and restore it after autofocus disabled ---
};

#define DEBUG_INFO_PAGES_COUNT 4

struct DataInput
{
	bool  StereoActive;
	bool  SwapEyes;
	bool  ShowWizardAtStartup;
	bool  ShowFPS;
	bool  ShowHotKeyOSD;
	bool  LaserSightEnable;
	CameraPreset Preset[3];
	DWORD ActivePreset;
	DWORD DebugInfoPage;
#ifndef FINAL_RELEASE
	float ZNear;
	float Multiplier;
	float ConvergenceShift;
#endif
	CameraPreset* GetActivePreset() { return &Preset[ActivePreset]; }
	void SwitchCurrentPresetAutofocus(); 
	bool IsChanged(DataInput& inputOther);
	bool IsStereoParamChanged(DataInput& inputOther);
	DataInput ()
	{
		ActivePreset = 0;
		Preset[0].StereoBase		= DEFAULT_STEREOBASE;
		Preset[0].One_div_ZPS		= DEFAULT_ONE_DIV_ZPS;
		Preset[0].AutoFocusEnable	= false;
		Preset[0].One_div_ZPS_Saved	= DEFAULT_ONE_DIV_ZPS;
		Preset[1].StereoBase		= DEFAULT_STEREOBASE;
		Preset[1].One_div_ZPS		= DEFAULT_ONE_DIV_ZPS;
		Preset[1].AutoFocusEnable	= false;
		Preset[1].One_div_ZPS_Saved	= DEFAULT_ONE_DIV_ZPS;
		Preset[2].StereoBase		= DEFAULT_STEREOBASE;
		Preset[2].One_div_ZPS		= DEFAULT_ONE_DIV_ZPS;
		Preset[2].AutoFocusEnable	= false;
		Preset[2].One_div_ZPS_Saved	= DEFAULT_ONE_DIV_ZPS;
		StereoActive		= false;
		SwapEyes			= false;
		ShowWizardAtStartup = false;
		DebugInfoPage		= 0;
		LaserSightEnable	= false;
		ShowFPS 			= false;
		ShowHotKeyOSD 		= false;
#ifndef FINAL_RELEASE
		ZNear				= 1.0f;
		Multiplier			= 1.0f;
		ConvergenceShift	= 0.0f;
#endif
	}
};

inline void DataInput::SwitchCurrentPresetAutofocus()
{ 
	CameraPreset* p = GetActivePreset(); 
	if(p->AutoFocusEnable)
	{
		p->AutoFocusEnable = FALSE;
		p->One_div_ZPS = p->One_div_ZPS_Saved;
	}
	else
	{
		p->One_div_ZPS_Saved = p->One_div_ZPS;
		p->AutoFocusEnable = TRUE;
	}
}

inline bool DataInput::IsChanged(DataInput& inputOther)
{
	return StereoActive						!= inputOther.StereoActive	||
		ActivePreset						!= inputOther.ActivePreset	|| 
		SwapEyes							!= inputOther.SwapEyes		||
		GetActivePreset()->StereoBase		!= inputOther.GetActivePreset()->StereoBase  ||
		GetActivePreset()->One_div_ZPS		!= inputOther.GetActivePreset()->One_div_ZPS ||
		GetActivePreset()->AutoFocusEnable	!= inputOther.GetActivePreset()->AutoFocusEnable;
}

inline bool DataInput::IsStereoParamChanged(DataInput& inputOther)
{
	return GetActivePreset()->StereoBase	!= inputOther.GetActivePreset()->StereoBase  ||
		GetActivePreset()->One_div_ZPS		!= inputOther.GetActivePreset()->One_div_ZPS ||
		SwapEyes							!= inputOther.SwapEyes;
}

enum EventId
{
	SCREENSHOT,
	TOGGLE_STEREO,         
	INCREASE_ZPS,
	DECREASE_ZPS,
	INCREASE_STEREOBASE,
	DECREASE_STEREOBASE,
	SWAP_EYES,
	SET_PRESET_1,
	SET_PRESET_2,
	SET_PRESET_3,
	TOGGLE_AUTOFOCUS,
	TOGGLE_LASER_SIGHT,
	TOGGLE_WIZARD,
	TOGGLE_FPS,
	TOGGLE_HOTKEY,
	INCREASE_PRESENTER_SLEEP_TIME,
	DECREASE_PRESENTER_SLEEP_TIME,
	INCREASE_PRESENTER_FLUSH_TIME,
	DECREASE_PRESENTER_FLUSH_TIME,
#ifndef FINAL_RELEASE
	// DEBUG events!!!
	INCREASE_ZNEAR,	
	DECREASE_ZNEAR,
	INCREASE_MULTIPLIER,
	DECREASE_MULTIPLIER,
	INCREASE_CONVERGENCE_SHIFT,
	DECREASE_CONVERGENCE_SHIFT,
	SWITCH_DEBUG_INFO_PAGE,
#endif
	MAX_EVENTS 
};

struct HotKeyInfo
{
	char*   HotKeyName;
	EventId id;
	DWORD   code;
};

struct WizHotKey{
	WPARAM wParam;
	bool isPressed;
};

struct WizardKeyInfo
{
	bool shiftMultKey;
	WizHotKey keys[8];
};

typedef void (* MakeScreenshotCallback)();

class GlobalInfo
{
public:
	// app global data
	TCHAR		ProfileName[MAX_PATH];
	TCHAR		Params[MAX_PATH];
	TCHAR		DriverDirectory[MAX_PATH];
	TCHAR       ApplicationName[MAX_PATH];
	TCHAR       ApplicationFileName[MAX_PATH];
	LPTSTR      CmdLine;
	TCHAR       Vendor[20];
	HMODULE		hOutputDLL;
	DWORD		OutputCaps;
	bool	 	SpanModeOn;
	bool		WizardWasShown;
	UINT64		EID;

	// API
	DWORD		RenderTargetCreationMode;	// 0 - mono, 1 - stereo, 2 - auto
	DWORD		BltSrcEye;	// 0 - left, 1 - right
	DWORD		BltDstEye;	// 0 - left, 1 - right
	DWORD		DrawType;	// 0 - mono, 1 - stereo, 2 - auto
	bool		LastDrawStereo;

	// config data
#ifdef ZLOG_ALLOW_TRACING
	TCHAR       LogFileName[MAX_PATH];
	TCHAR		LogFileDirectory[MAX_PATH];
	zlog::SEVERITY*	pTrace;
#endif
#ifndef FINAL_RELEASE
	bool		ShowDebugInfo;
	bool		DumpOn;
	bool		DumpInitialData;
	bool		DumpVS;
	bool		DumpPS;
	bool		DumpMeshes;
	bool		DumpStereoImages;
	bool		UseR32FDepthTextures;
	bool		LockableDepthStencil;
	bool		ClearBeforeEveryDrawWhenDoDump;
	bool		ReplaceDSSurfacesByTextures;
	bool		TwoPassRendering;
#endif

	TCHAR       OutputMethodDll[40];
	DWORD       OutputMode;
	DWORD		OutputSpanMode;
	DWORD		DisplayScalingMode;
	DWORD		DisplayNativeWidth;
	DWORD		DisplayNativeHeight;
	DXGI_RATIONAL	DisplayNativeFrequency;
	INT			ShutterMode;
	INT			ShutterRefreshRate;
	bool		ExtractDepthBuffer;
	bool		DrawMonoImageOnSecondMonitor;
	bool		UseMonoDeviceWrapper;
	bool		CollectDebugInformation;
	DWORD       ScreenshotType;	
	bool		FixVistaSP1ResetBug;
	bool		DisableGammaApplying;
	TCHAR       Language[40];

	// profile data   
	DWORD		RouterType;
	bool		DisableD3D10Driver;
	bool		EnableStereo;
	bool		AlternativeSetFullscreenStateOrder;

	bool		ShowFPS;
	bool		ShowOSD;
	DWORD		SeparationMode;
	DWORD		FrustumAdjustMode;
	FLOAT		BlackAreaWidth;
	INT			PresenterSleepTime;
	FLOAT		PresenterFlushTime;
	FLOAT		PresenterMaxLagPercent;
	DWORD		DeviceMode;
	DWORD		MultiWindowsMode;
	DWORD		ModeObtainingDepthBuffer;
	bool		DisableFullscreenModeEmulation;
	bool		ForceVSyncOff;
	bool		EnableAA;
	bool		WideRenderTarget;
	bool		CommandBuffering;
	bool		RenderTargetBuffering;

	// Resource creation
	bool        MonoRenderTargetTextures;
	bool        MonoRenderTargetSurfaces;
	bool        MonoDepthStencilTextures;
	bool        MonoDepthStencilSurfaces;

	// VS Creation
	bool		AlwaysModifyShaders;
	DWORD		VSSeparationUpdateMode;
	DWORD		VertexShaderModificationMethod;
	bool		ModifyIncorrectProjectionShaders;
	bool		AlwaysUseDefaultProjectionMatrixForVS;

	// RT & DS
	bool		CreateOneComponentRTInMono;
	bool		CreateSquareRTInMono;
	bool		CreateSquareDSInMono;
	bool		CreateRTThatLessThanBBInMono;
	bool		CreateDSThatLessThanBBInMono;
	bool		CreateBigSquareRTInStereo;
	bool		CreateBigSquareDSInStereo;
	bool		DisableNVCSTextureInWRTMode;

	DWORD		RenderToRightInMono;
	DWORD		GameSpecific;
	bool		HideAdditionalAdapters;
	bool		MonoFixedPipeline;
	DWORD		MonoFixedPipelineCheckingMethodIndex;
	bool		MonoDrawFromMemory;
	bool		MonoRHWPipeline;
	DWORD		MonoRHWCheckingMethodIndex;
	bool		RenderNonMatrixShadersInMono;
	bool		RenderIncorrectProjectionShadersInMono;
	bool		DX10MonoNonIndexedDraw;
	DWORD		RHWModificationMethod;
	bool		UseSimpleStereoProjectionMethod;

	// VS Matrix analyzing
	bool		SkipCheckOrthoMatrix;
	bool		CheckOnlyProjectionMatrix;
	bool		CheckExistenceInverseMatrix;
	bool		CheckShadowMatrix;
	bool		CheckInverseLR;

	bool		FixNone16x9TextPosition;
	FLOAT		SeparationScale;
	FLOAT		ScaleSeparationForSmallViewPorts;

	// Auto Focus variables
	INT			SearchFrameSizeX;
	INT			SearchFrameSizeY;
	FLOAT		ShiftSpeed;
	FLOAT		Max_ABS_rZPS;
	FLOAT		FrameYCoordinate;

	// Laser sight variables
	UINT		LaserSightIconIndex;
	TCHAR		CustomLaserSightIconPath[MAX_PATH];
	FLOAT		LaserSightXCoordinate;
	FLOAT		LaserSightYCoordinate;
	
	HotKeyInfo  HotKeyList[MAX_EVENTS];
	DataInput	Input;
	WizardKeyInfo WizKeyInfo;
	
	BOOL MakeScreenshot;
	TCHAR MakeScreenshotPath[MAX_PATH];
	MakeScreenshotCallback MakeScreenshotCallback;

	GlobalInfo()
	{
		ZeroMemory(this, sizeof(GlobalInfo));
#ifdef ZLOG_ALLOW_TRACING
		zlog::GetSeverityPtr(&pTrace);
#endif
		Input = DataInput();
		RenderTargetCreationMode = 2;
		DrawType = 2;
		DeviceMode = DEVICE_MODE_AUTO;
		MultiWindowsMode = MULTI_WINDOWS_MODE_AUTO;
		SeparationScale = 1.0f;
		ScaleSeparationForSmallViewPorts = 1.0f;
		// Auto Focus variables
		SearchFrameSizeX = 512;
		SearchFrameSizeY = 64;
		FrameYCoordinate = 0.495f;
		ShiftSpeed		 = 1.f;
		Max_ABS_rZPS	 = 0.2f;
		// Laser sight variables
		LaserSightXCoordinate = 0.5f;
		LaserSightYCoordinate = 0.5f;
		// Wizard variables
		WizardWasShown	= false;

		_tcscpy_s<40>(Language, _T("English") );

		HotKeyInfo hotKeyList[MAX_EVENTS] = 
		{
			{ "Screenshot", SCREENSHOT,                                     44 },	//--- PrtScn				---
			{ "ToggleStereo", TOGGLE_STEREO,                                106 },  //--- Num *					---
			{ "ZPSIncrease", INCREASE_ZPS,                                  363 },  //--- Shift + Num +			---
			{ "ZPSDecrease", DECREASE_ZPS,                                  365 },  //--- Shift + Num -			---
			{ "StereoBaseIncrease", INCREASE_STEREOBASE,                    107 }, 	//--- Num +					---
			{ "StereoBaseDecrease", DECREASE_STEREOBASE,                    109 }, 	//--- Num -					---
			{ "SwapEyes", SWAP_EYES,                                        631 },  //--- Ctrl + F8				---
			{ "SetPreset1", SET_PRESET_1,                                   103 },	//--- Num 7					---
			{ "SetPreset2", SET_PRESET_2,                                   104 },	//--- Num 8					---
			{ "SetPreset3", SET_PRESET_3,                                   105 },	//--- Num 9					---
			{ "ToggleAutoFocus", TOGGLE_AUTOFOCUS,                          111 },	//--- Num /					---
			{ "ToggleLaserSight", TOGGLE_LASER_SIGHT,                       623 },	//--- Ctrl + Num /			---
			{ "ToggleWizard", TOGGLE_WIZARD,                                362 },	//--- Shift + Num*			---
			{ "ToggleFPS", TOGGLE_FPS,                                      0 },	//---           			---
			{ "ToggleHotkeysOSD", TOGGLE_HOTKEY,						    368 },	//--- Shift + F1			---
			{ "PresenterSleepTimeIncrease", INCREASE_PRESENTER_SLEEP_TIME,  221 },  //--- ]				        ---
			{ "PresenterSleepTimeDecrease", DECREASE_PRESENTER_SLEEP_TIME,  219 },  //--- [				        ---
			{ "PresenterFlushTimeIncrease", INCREASE_PRESENTER_FLUSH_TIME,	186 },  //--- ;				        ---
			{ "PresenterFlushTimeDecrease", DECREASE_PRESENTER_FLUSH_TIME,	76 },   //--- l				        ---
#ifndef FINAL_RELEASE
			{ "ZNearIncrease", INCREASE_ZNEAR,                              875 },	//--- Ctrl + Shift + Num +  ---
			{ "ZNearDecrease", DECREASE_ZNEAR,                              877 },	//--- Ctrl + Shift + Num -  ---
			{ "MultiplierIncrease", INCREASE_MULTIPLIER,                    619 }, 	//--- Ctrl + Num +			---
			{ "MultiplierDecrease", DECREASE_MULTIPLIER,                    621 }, 	//--- Ctrl + Num 	-		---
			{ "ConvergenceShiftIncrease", INCREASE_CONVERGENCE_SHIFT,       1131 },	//--- Alt + Num +			---
			{ "ConvergenceShiftDecrease", DECREASE_CONVERGENCE_SHIFT,       1133 },	//--- Alt + Num -			---
			{ "ShowDebugInfo", SWITCH_DEBUG_INFO_PAGE,                      618 },	//--- Ctrl + *		        ---
#endif
		};
#if _DEBUG
		for (int i = 0; i < MAX_EVENTS; i++)
		{
			_ASSERT((EventId)i == hotKeyList[i].id);
		}
#endif
		memcpy(HotKeyList, hotKeyList, sizeof(HotKeyList));
	}
};

#ifndef FINAL_RELEASE
#define GET_DEBUG_VALUE(x)	x
#else
#define GET_DEBUG_VALUE(x)	0
#endif

extern S3DAPI_API GlobalInfo _gInfo;

extern GlobalInfo& gInfo;

// you should declare gInfo in project
//GlobalInfo& gInfo = _gInfo;

#define	USE_MULTI_THREAD_PRESENTER			(gData.ShutterMode == SHUTTER_MODE_MULTITHREADED)
#define	USE_MULTI_DEVICE_PRESENTER			(gData.ShutterMode == SHUTTER_MODE_MULTIDEVICE)
#define	USE_ATI_PRESENTER					(gData.ShutterMode == SHUTTER_MODE_ATIQB)
#define	USE_UM_PRESENTER_D3D9				(gData.ShutterMode == SHUTTER_MODE_UMHOOK_D3D9)
#define	USE_UM_PRESENTER_D3D9EX				(gData.ShutterMode == SHUTTER_MODE_UMHOOK_D3D9EX)
#define	USE_UM_PRESENTER					(USE_UM_PRESENTER_D3D9 || USE_UM_PRESENTER_D3D9EX)
#define	USE_SIMPLE_PRESENTER				(gData.ShutterMode == SHUTTER_MODE_SIMPLE)
#define USE_IZ3DKMSERVICE_PRESENTER			(gData.ShutterMode == SHUTTER_MODE_IZ3DKMSERVICE)
#define	IS_SHUTTER_OUTPUT					(gInfo.OutputCaps & odShutterMode)
