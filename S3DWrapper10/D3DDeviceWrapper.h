#pragma once

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the S3DWRAPPER10_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// S3DWRAPPER10_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef S3DWRAPPER10_EXPORTS
#define S3DWRAPPER10_API __declspec(dllexport)
#else
#define S3DWRAPPER10_API 
#endif

#include "D3DMonoDeviceWrapper.h"
#include "..\S3DAPI\GlobalData.h"
#include "..\S3DAPI\KeyboardHook.h"
#include "CallGuard.h"
#include <D3DX10Math.h>
#include <boost/ptr_container/ptr_map.hpp>
#include "D3DDeviceState.h"
#include "TreeAllocator.h"
#include "AutoFocus.h"
#include "LaserSight.h"
#include "../uilib/Wizard.h"
#include "../uilib/HotKeyOSD.h"
#include "../DX10SharedLibrary/ExternalConstantBuffer.h"
#include "Utils.h"
//#define DISABLE_WIZARD

struct IDXGISwapChain;
struct IDXGIDevice;
class D3DSwapChain;
class CBasePresenterX;
interface ID3D11Device;

class MonoCommandBuffer;
class StereoCommandBuffer;
typedef boost::shared_ptr<MonoCommandBuffer> TMonoCommandBufferPtr;
typedef boost::shared_ptr<StereoCommandBuffer> TStereoCommandBufferPtr;

typedef boost::ptr_map<IDXGISwapChain*, D3DSwapChain> SwapChainsMap;
enum DumpTypes { dtNone, dtTrace, dtCommands, dtOnlyRT, dtFull };

namespace DX10Output
{
	class OutputMethod;
}

namespace acGraphics
{
	class CFont;
	class CDynRender_dx10UM;
};

class D3DDeviceWrapper : public D3DMonoDeviceWrapper
{
public:
	D3DDeviceWrapper();
	~D3DDeviceWrapper();

	friend class D3DSwapChain;
	UINT							dwCreationFlags;
	inline UINT						GetCreationFlags()
	{
		return dwCreationFlags;
	}
	void CallFlush() { OriginalDeviceFuncs.pfnFlush(GetHandle()); ++m_nFlushCounter; }

	DataInput						m_Input;

	D3DDeviceState					m_DeviceState;
#ifndef FINAL_RELEASE
	D3DDeviceState					m_DeviceStateAtExecute;
#endif

	DeviceModes						m_DeviceMode;
	bool							m_bTwoWindows;
	bool							m_bUsingMonoOutput;
	bool							m_bFirstSwapChainCreated;
	bool							m_bIsLastRTStereo;
	DX10Output::OutputMethod*		m_pOutputMethod;	

	TMonoCommandBufferPtr			m_MonoBuffer;
	TStereoCommandBufferPtr			m_StereoBuffer;
	
	D3DSwapChain*					m_pTempSwapChain;
	D3DSwapChain*					m_pLastSwapChain;
	SwapChainsMap					m_SwapChainsMap;
#ifndef FINAL_RELEASE
	ResourceManager					m_ResourceManager;
#endif

	int								m_FrameIndex;
	CriticalSection					m_CSUMCall;
	CriticalSection					m_CSCB;

	// debug data
#ifndef FINAL_RELEASE
	TCHAR							m_DumpDirectory[MAX_PATH];
	DumpTypes						m_DumpType;
	zlog::SEVERITY					m_nOldTrace;
	size_t							m_MonoCommandsCount;
	size_t							m_StereoCommandsCount;
	int								m_DrawCount;
	int								m_StereoDrawCount;
	int								m_StereoDrawAutoCount;
	int								m_StereoDrawIndexedCount;
	int								m_StereoDrawInstancedCount;
	int								m_StereoDrawIndirectCount;
	int								m_SetRenderTargetCount;
	int								m_StereoBuffersCount;
	int								m_CBCount;
	int								m_CBSubIndexCount;
#endif

	size_t							m_MaxBufferSize;
	iz3d::ExternalConstantBuffer*	m_pECBuffer;
	
	LARGE_INTEGER					m_FlushTime;
	int								m_nFlushCounter;
	int								m_nGetDataCounter;
	FLOAT							m_PresenterFlushTime;
	bool							m_bFlushCommands;

	tree_allocator					m_ConstantBufferAllocator;

	__forceinline void				CalculateMatrix( VIEWINDEX viewIndex, float A, float B, 
		const D3DXMATRIXA16* pProjMatrix, const D3DXMATRIXA16* pInvProjection );
	void							UpdateProjectionMatrix();
	D3DXMATRIX&						GetTransformationMatrix(VIEWINDEX index)
	{
		return m_TransformBeforeViewport[index];
	}	
	void							UpdateCBMatrices( bool bNewMode );

	void							InitFont();
	void							DrawOSD( D3DSwapChain* pSwapChain_, float fFrameTimeDelta );
	void							DrawLogo( D3DSwapChain* pSwapChain_ );
	void							DrawInfo( D3DSwapChain* pSwapChain_ );

	void							HookDeviceFuncs(struct D3D10DDI_DEVICEFUNCS*  pDeviceFunctions);
	void							HookDeviceFuncs(struct D3D10_1DDI_DEVICEFUNCS*  pDeviceFunctions);
	void							HookDeviceFuncs(struct D3D11DDI_DEVICEFUNCS*  pDeviceFunctions);
	void							HookDXGIFuncs(struct DXGI_DDI_BASE_FUNCTIONS  *pDXGIDDIBaseFunctions);
	void							HookDXGIFuncs(struct DXGI1_1_DDI_BASE_FUNCTIONS  *pDXGIDDIBaseFunctions);
	void							CreateD3DSwapChain();
	void							RemoveSwapChain( D3D10DDI_HRESOURCE hResource, bool bLeft = true );
	bool							RemoveAppBackBuffer( SwapChainsMap::iterator it, D3DSwapChain* pSwapChain, D3D10DDI_HRESOURCE &hResource );
	D3DSwapChain*					MapDXGISwapChain( IDXGIFactory* pFactory, IDXGISwapChain* pSwapChain );
	bool							IsCurrentThreadMain();

	void							InitializeDXGIDevice( IDXGIDevice* pDevice_ );
	void							InitializeSwapChainRelatedData();
	void							SetSecondBackBuffer( D3D10DDI_HRESOURCE hResource );	

	template <typename T>
	void							AddCommand( T* pCmd, bool bForceProcess = false );
	void							ProcessCB();
	bool							IsStereoActive() const { return m_Input.StereoActive; }
	bool							UseStereoCommandBuffer() const;

	// cache resource commands
	void							WrapCommand( Commands::CreateResource* pCmd_ );
	void							WrapCommand( Commands::CreateResource11_0* pCmd_ );
	void							WrapCommand( Commands::xxUpdateSubresourceUP* pCmd_ );
	void							WrapCommand( Commands::xxMap* pCmd_ );
	void							WrapCommand( Commands::xxUnmap* pCmd_ );
	void							WrapCreateResource( D3D10DDI_HRESOURCE hResource );
	size_t							GetBufferingMaxSize() const { return m_BufferingMaxSize; }

	void							StereoModeChanged(bool bNewMode);

	inline DX10Output::OutputMethod*	GetOutputMethod () const { return m_pOutputMethod; }
	D3DSwapChain*					GetD3DSwapChain( );
	void							DoPresent();

	bool							SkipPresenterThread		();
	void							RestoreStates();
	void							UpdateSecondSwapChain( D3D10DDI_HRESOURCE hResource );
	void							UpdateFrameStatistic( D3DSwapChain* pSwapChain_ );
	void							SetNextFlushTime( LARGE_INTEGER &time );
	bool							CheckFlush( );
	void							FlushCommands( );
	
#ifndef FINAL_RELEASE
	void							LogStatistics();
#endif

	void							CheckEngine( );
	void							CreateOutput();	
#ifndef FINAL_RELEASE
	void							DumpRT(bool b2RT, bool bStereo, DumpView view);
	void							DumpUAVs(bool b2RT, bool bStereo, DumpView view);
	void							DumpTextures(bool b2RT, DumpView view);
	void							DumpCSTextures(bool b2RT, DumpView view);
	void							DumpPipelineTextures( const ShaderPipelineStates &pipeline, const TCHAR* prefix, bool b2RT, DumpView view );
	void							DumpPipelineTextures( const Commands::xxSetShaderResources* xxRes, const std::bitset<128>& mask, const TCHAR* prefix, bool bRT, DumpView view );
#endif
	UINT							GetInterfaceVersion () const { return m_nInterfaceVersion; };
	IDXGIDevice*					GetDXGIInterface	() const { return m_pDXGIDevice; };
	void							SetInterfaceVersion ( UINT nVersion );

	inline ULONG					GetScreenShotCount	() const { return m_nScreenShotCounter; };
	inline ULONG					IncScreenShotCount	() { return m_nScreenShotCounter++; };

	UINT							AQBSGetLineOffset			( D3DSwapChain* pSwapChain_ );
	static HRESULT					FindShutterClosestMatchingMode	( IDXGIDevice *pDevice, DXGI_MODE_DESC* pMode_, IDXGIOutput* pDXGIOutput_ = NULL );
	template <typename T>
	static bool						OpenAMDStereoInterfaceX		( IDXGIDevice* pDXGIDevice, T* pDevice_, const char* pFncName );
	static bool						OpenAMDStereoInterface		( IDXGIDevice* pDXGIDevice, ID3D10Device* pDevice_ );
	static bool						OpenAMDStereoInterface		( IDXGIDevice* pDXGIDevice, ID3D11Device* pDevice_ );
	static void						CloseAMDStereoInterface		();
	static bool						IsAmdStereoInited			( IDXGIDevice* pDevice_ ) { if ( pDevice_ != m_pAQBSDXGIDevice ) return false; return m_pAmdStereo ? true : false; };
	static bool						IsAmdStereoInited			( ) { return m_pAmdStereo ? true : false; };
	static void						ResetAQBSStates				( ) { m_bUpdateAQBSDriver = false; m_bUnsupported3DDevice = false; };

private:
	IDXGIDevice*									m_pDXGIDevice;
	bool											m_bStereoModeChanged;
	ULONG											m_nScreenShotCounter;
	AsyncButton					    				m_ScreenShotButton;

	D3D10_DDI_VIEWPORT								m_tViewPort;
	acGraphics::CFont*								m_pFont;
	acGraphics::CDynRender_dx10UM*					m_pDynRender;
#ifndef DISABLE_WIZARD
	uilib::Wizard									m_Wizard;
	iz3d::UI::HotKeyOSD								m_HotKeyOSD;
#endif

	//--- draw stereo-camera parameters related variables ---
	LARGE_INTEGER									m_nOSDShowTimeLeft;
	LARGE_INTEGER									m_nWriteSettingsAfter;

	bool											m_bCantFindStereoDevice;
	static bool										m_bUpdateAQBSDriver;
	static bool										m_bUnsupported3DDevice;
	LARGE_INTEGER 									m_nGlobalFrameTimeDelta;
	LARGE_INTEGER 									m_nGlobalLastFrameTime;
	LARGE_INTEGER									m_nFreq;
	DWORD											m_MainThreadId;
	D3DXMATRIX										m_TransformBeforeViewport[2];

	//--- per device laser sight data and auto-focus camera control ---
	AutoFocus										m_AutoFocus;
	LaserSight										m_LaserSight;

#pragma region CommandBuffering
	// buffering policy
	bool											m_BufferingOnlyBuffers;
	bool											m_BufferingOnlyCB;
	size_t											m_BufferingMaxSize;

	// allocator min/max size
	size_t											m_BufferingMinSizeUSU;
	size_t											m_BufferingMaxSizeUSU;
#pragma endregion

#ifndef FINAL_RELEASE
	bool											m_bDebugUseSimplePresenter;
#endif

	static HMODULE									m_hAQBSModule;

	static class		IAmdDxExt*					m_pAmdExt;
	static class		IAmdDxExtQuadBufferStereo*	m_pAmdStereo;
	static interface	IDXGIDevice*				m_pAQBSDXGIDevice;
};

extern TLS			D3DDeviceWrapper* g_pLastD3DDevice;
