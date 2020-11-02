#pragma once
#include <d3dumddi.h>
#include <vector>
#include "UMDriverHook.h"
#include "..\CommonUtils\System.h"
#include "atiqbstereo.h"
#include "UMDShutterPresenter.h"

class CBaseStereoRenderer;

class UMDDeviceWrapper
{

public:
	void KillPresenterThread();
	HRESULT APIENTRY Unlock			( CONST D3DDDIARG_UNLOCK* pUnLock );
	HRESULT APIENTRY OpenResource	( D3DDDIARG_OPENRESOURCE* pResource );
	HRESULT APIENTRY CreateResource	( D3DDDIARG_CREATERESOURCE* pResource );
	HRESULT APIENTRY DestroyResource( HANDLE hVideoProcessor);
	HRESULT APIENTRY Present		( CONST D3DDDIARG_PRESENT* pPresent );
	HRESULT APIENTRY Lock			( D3DDDIARG_LOCK* pLock);
	HRESULT APIENTRY Blt			( CONST D3DDDIARG_BLT* pBlt );

	UMDDeviceWrapper(UMWrapper* pAdapter, D3DDDIARG_CREATEDEVICE* pDevice, 
		D3DKMT_HANDLE hKMAdapter, D3DKMT_HANDLE hKMDevice);
	~UMDDeviceWrapper();

	void	CreatePresenter			();
	UMDShutterPresenter*			GetPresenter()	{ return m_pPresenter; }
	HRESULT	DoPresent				( CONST D3DDDIARG_PRESENT* pPresent, CONST D3DDDIARG_BLT* pBlt );
	HRESULT	DoKMPresent				( CONST D3DDDIARG_PRESENT* pPresent );
	HRESULT	DoKMBlt					( CONST D3DDDIARG_BLT* pBlt );

	void	CreateUMPresenter		( UMDShutterPresenterSCData* pSCData, CBaseSwapChain* pBaseSwapChain );
	CBaseStereoRenderer*			GetStereoRenderer( )	{ return m_pStereoRenderer; }
	void							SetStereoRenderer( CBaseStereoRenderer* pRenderer )	{ m_pStereoRenderer = pRenderer; }
	void	SaveCommandBufferStats	() { m_CommandCountStats.push_back(m_dwCommandCount); m_dwCommandCount = 0; } 

//protected:
	CBaseStereoRenderer*		m_pStereoRenderer;
	D3DKMT_HANDLE				m_hKMAdapter;
	D3DKMT_HANDLE				m_hKMDevice;
	HANDLE						m_hDevice;
	D3DDDI_DEVICEFUNCS			m_DeviceFunctions;
	UMWrapper*					m_AdapterWrapper;
	UMDShutterPresenter*		m_pPresenter;
	CriticalSection				m_CS;
	HANDLE						m_CommunicationSurface;
	ATIDX9STEREOCOMMPACKET*		m_pAtiStereoCommPacket;
	D3DDDIARG_CREATERESOURCE	m_DoubledBackbuffer;
	D3DDDIFORMAT                m_BackbufferFormat;
	SIZE						m_BackbufferSize;
	CResourceHandleVector		m_Backbuffer;
	bool						m_isStereoEnable;
	bool						m_UMCallInProgress;
	DWORD						m_dwPresentBufferIndex;

	DWORD							m_dwCommandCount;
	boost::circular_buffer<DWORD>	m_CommandCountStats;

public:
	//--- data access from callback functions ---
	friend HANDLE						GetDevice(HANDLE hDevice);         
	friend CriticalSection&				GetCriticalSection(HANDLE hDevice);         
	friend D3DDDI_DEVICEFUNCS* const	GetDeviceFunctions(HANDLE hDevice);
	bool IsBackBuffer( HANDLE hResource );
	void AddBackBuffer(HANDLE hResource, DWORD SubResourceCount);
	friend class						UMCallGuard;
};

inline HANDLE							GetDevice(HANDLE hDevice)				{ return ((UMDDeviceWrapper*)hDevice)->m_hDevice;			}
inline CriticalSection&					GetCriticalSection(HANDLE hDevice)		{ return ((UMDDeviceWrapper*)hDevice)->m_CS;				}        
inline D3DDDI_DEVICEFUNCS* const		GetDeviceFunctions(HANDLE hDevice)		{ return &((UMDDeviceWrapper*)hDevice)->m_DeviceFunctions;	}
inline void								KillPresenterThread( HANDLE hDevice )	{ ((UMDDeviceWrapper*)hDevice)->KillPresenterThread();		}

extern UMDDeviceWrapper*	g_pUMDeviceWrapper;

//------------------------------------- Caller Profiler and Critical section ----------------------
struct CallTimeData
{
	CallTimeData() { totalTime = 0; maxTime = 0; callCount = 0; }
	DWORD totalTime;
	DWORD maxTime;
	DWORD callCount;
};

extern const wchar_t* g_CurrentUMFunctionName;

typedef std::pair<const wchar_t*, CallTimeData> RecordData;
struct SortRecordData
{
	bool operator()(const RecordData& rd1, const RecordData& rd2)
	{
		float averTime1 = (1.0f * rd1.second.totalTime) / rd1.second.callCount;
		float averTime2 = (1.0f * rd2.second.totalTime) / rd2.second.callCount;
		return averTime1 > averTime2;
	}
};

struct SortbyMaxRecordData
{
	bool operator()(const RecordData& rd1, const RecordData& rd2)
	{
		return rd1.second.maxTime > rd2.second.maxTime;
	}
};

typedef boost::unordered_map<std::wstring, CallTimeData>	CallProfiler;
extern CallProfiler g_Profiler;

#ifdef _DEBUG
typedef stdext::hash_map	<HANDLE, D3DDDI_RESOURCEFLAGS>	ResourcesTypeMap;
#else
typedef boost::unordered_map<HANDLE, D3DDDI_RESOURCEFLAGS>	ResourcesTypeMap;
#endif
extern ResourcesTypeMap	g_ResourceType;
#ifdef _DEBUG
typedef stdext::hash_map	<HANDLE, D3DDDIQUERYTYPE>		QueriesTypeMap;
#else
typedef boost::unordered_map<HANDLE, D3DDDIQUERYTYPE>		QueriesTypeMap;
#endif
extern QueriesTypeMap	g_QueryType;

class UMCallGuard
{
public:
	UMCallGuard(HANDLE hDevice, wchar_t* functionName) 
	{ 
		m_pDevice = (UMDDeviceWrapper*)hDevice; 
#ifndef FINAL_RELEASE
		g_CurrentUMFunctionName = functionName;
#endif
		m_pDevice->m_CS.Enter();
	}
	~UMCallGuard()
	{
		m_pDevice->m_CS.Leave();
		//m_pDevice->m_dwCommandCount++;
	}
protected:
	UMDDeviceWrapper*	m_pDevice;
};
//-------------------------------------------------------------------------------------------------

HRESULT APIENTRY AuthenticatedChannelKeyExchangeCallback(HANDLE hDevice, D3DDDIARG_AUTHENTICATEDCHANNELKEYEXCHANGE* pKey);
HRESULT APIENTRY BltCallback(HANDLE hDevice, CONST D3DDDIARG_BLT* pBlt);
HRESULT APIENTRY BufBltCallback(HANDLE hDevice, CONST D3DDDIARG_BUFFERBLT* pBlt);
HRESULT APIENTRY CaptureToSysMemCallback(HANDLE hDevice, CONST D3DDDIARG_CAPTURETOSYSMEM* pCapture);
HRESULT APIENTRY ClearCallback(HANDLE hDevice, CONST D3DDDIARG_CLEAR* pClear, UINT uCount, CONST RECT* pRect);
HRESULT APIENTRY ColorFillCallback(HANDLE hDevice, CONST D3DDDIARG_COLORFILL* pFill);
HRESULT APIENTRY ComposeRectsCallback(HANDLE hDevice, CONST D3DDDIARG_COMPOSERECTS* pRects);
HRESULT APIENTRY ConfigureAuthenticatedChannelCallback(HANDLE hDevice, CONST D3DDDIARG_CONFIGUREAUTHENTICATEDCHANNEL* pChannel);
HRESULT APIENTRY CreateAuthenticatedChannelCallback(HANDLE hDevice, D3DDDIARG_CREATEAUTHENTICATEDCHANNEL* pChannel);
HRESULT APIENTRY CreateCryptoSessionCallback(HANDLE hDevice, D3DDDIARG_CREATECRYPTOSESSION* pSession);
HRESULT APIENTRY CreateDecodeDeviceCallback(HANDLE hDevice, D3DDDIARG_CREATEDECODEDEVICE* pData);
HRESULT APIENTRY CreateExtensionDeviceCallback(HANDLE hDevice, D3DDDIARG_CREATEEXTENSIONDEVICE* pData);
HRESULT APIENTRY CreateLightCallback(HANDLE hDevice, CONST D3DDDIARG_CREATELIGHT* pLight);
HRESULT APIENTRY CreateOverlayCallback(HANDLE hDevice, D3DDDIARG_CREATEOVERLAY* pOverlay);
HRESULT APIENTRY CreatePixelShaderCallback(HANDLE hDevice, D3DDDIARG_CREATEPIXELSHADER* pData, CONST UINT* pCode);
HRESULT APIENTRY CreateQueryCallback(HANDLE hDevice, D3DDDIARG_CREATEQUERY* pQuery);
HRESULT APIENTRY CreateResourceCallback(HANDLE hDevice, D3DDDIARG_CREATERESOURCE* pResource);
HRESULT APIENTRY CreateVertexShaderDeclCallback(HANDLE hDevice, D3DDDIARG_CREATEVERTEXSHADERDECL* pDecl, CONST D3DDDIVERTEXELEMENT* pElement);
HRESULT APIENTRY CreateVertexShaderFuncCallback(HANDLE hDevice, D3DDDIARG_CREATEVERTEXSHADERFUNC* pFunc, CONST UINT* pCode);
HRESULT APIENTRY CreateVideoProcessDeviceCallback(HANDLE hDevice, D3DDDIARG_CREATEVIDEOPROCESSDEVICE* pData);
HRESULT APIENTRY CreateVideoProcessorCallback(HANDLE hDevice, D3DDDIARG_DXVAHD_CREATEVIDEOPROCESSOR* pProcessor);
HRESULT APIENTRY CryptoSessionKeyExchangeCallback(HANDLE hDevice, D3DDDIARG_CRYPTOSESSIONKEYEXCHANGE* pKey); 
HRESULT APIENTRY DecodeBeginFrameCallback(HANDLE hDevice, D3DDDIARG_DECODEBEGINFRAME* pFrame);  
HRESULT APIENTRY DecodeEndFrameCallback(HANDLE hDevice, D3DDDIARG_DECODEENDFRAME* pFrame);
HRESULT APIENTRY DecodeExecuteCallback(HANDLE hDevice, CONST D3DDDIARG_DECODEEXECUTE* pDecode);
HRESULT APIENTRY DecodeExtensionExecuteCallback(HANDLE hDevice, CONST D3DDDIARG_DECODEEXTENSIONEXECUTE* pDecode);
HRESULT APIENTRY DecryptionBltCallback(HANDLE hDevice, CONST D3DDDIARG_DECRYPTIONBLT* pBlt);
HRESULT APIENTRY DeletePixelShaderCallback(HANDLE hDevice, HANDLE hShader);
HRESULT APIENTRY DeleteVertexShaderDeclCallback(HANDLE hDevice, HANDLE hShader);
HRESULT APIENTRY DeleteVertexShaderFuncCallback(HANDLE hDevice, HANDLE hShader);
HRESULT APIENTRY DepthFillCallback(HANDLE hDevice, CONST D3DDDIARG_DEPTHFILL* pFill);
HRESULT APIENTRY DestroyAuthenticatedChannelCallback(HANDLE hDevice, CONST D3DDDIARG_DESTROYAUTHENTICATEDCHANNEL* pChannel);
HRESULT APIENTRY DestroyCryptoSessionCallback(HANDLE hDevice, CONST D3DDDIARG_DESTROYCRYPTOSESSION* pSession);
HRESULT APIENTRY DestroyDecodeDeviceCallback(HANDLE hDevice, HANDLE hDecode);
HRESULT APIENTRY DestroyDeviceCallback(HANDLE hDevice);                                
HRESULT APIENTRY DestroyExtensionDeviceCallback(HANDLE hDevice, HANDLE hExtension);
HRESULT APIENTRY DestroyLightCallback(HANDLE hDevice, CONST D3DDDIARG_DESTROYLIGHT* pLight);
HRESULT APIENTRY DestroyOverlayCallback(HANDLE hDevice, CONST D3DDDIARG_DESTROYOVERLAY* pOverlay);
HRESULT APIENTRY DestroyQueryCallback(HANDLE hDevice, CONST HANDLE hQuery);
HRESULT APIENTRY DestroyResourceCallback(HANDLE hDevice, HANDLE hVideoProcessor);
HRESULT APIENTRY DestroyVideoProcessDeviceCallback(HANDLE hDevice, HANDLE hVideoProcessor);
HRESULT APIENTRY DestroyVideoProcessorCallback(HANDLE hDevice, HANDLE hVideoProcessor);
HRESULT APIENTRY DrawIndexedPrimitive2Callback(HANDLE hDevice, CONST D3DDDIARG_DRAWINDEXEDPRIMITIVE2* pData, UINT dwIndicesSize, CONST VOID* pIndexBuffer, CONST UINT*  pFlagBuffer);
HRESULT APIENTRY DrawIndexedPrimitiveCallback(HANDLE hDevice, CONST D3DDDIARG_DRAWINDEXEDPRIMITIVE* pData);
HRESULT APIENTRY DrawPrimitive2Callback(HANDLE hDevice, CONST D3DDDIARG_DRAWPRIMITIVE2* pData);
HRESULT APIENTRY DrawPrimitiveCallback(HANDLE hDevice,  CONST D3DDDIARG_DRAWPRIMITIVE* pData, CONST UINT* pFlagBuffer);
HRESULT APIENTRY DrawRectPatchCallback(HANDLE hDevice, CONST D3DDDIARG_DRAWRECTPATCH* pData, CONST D3DDDIRECTPATCH_INFO* pInfo, CONST FLOAT* pPatch);
HRESULT APIENTRY DrawTriPatchCallback(HANDLE hDevice, CONST D3DDDIARG_DRAWTRIPATCH* pData, CONST D3DDDITRIPATCH_INFO* pInfo, CONST FLOAT* pPatch);
HRESULT APIENTRY EncryptionBltCallback(HANDLE hDevice, CONST D3DDDIARG_ENCRYPTIONBLT* pBlt);
HRESULT APIENTRY ExtensionExecuteCallback(HANDLE hDevice, CONST D3DDDIARG_EXTENSIONEXECUTE* pExecute);
HRESULT APIENTRY FinishSessionKeyRefreshCallback(HANDLE hDevice, CONST D3DDDIARG_FINISHSESSIONKEYREFRESH* pKey);
HRESULT APIENTRY FlipOverlayCallback(HANDLE hDevice, CONST D3DDDIARG_FLIPOVERLAY* pFlip);
HRESULT APIENTRY FlushCallback(HANDLE hDevice);
HRESULT APIENTRY GenerateMipSubLevelsCallback(HANDLE hDevice, CONST D3DDDIARG_GENERATEMIPSUBLEVELS* pLevel);
HRESULT APIENTRY GetCaptureAllocationHandleCallback(HANDLE hDevice, D3DDDIARG_GETCAPTUREALLOCATIONHANDLE* pCapture);
HRESULT APIENTRY GetEncryptionBltKeyCallback(HANDLE hDevice, CONST D3DDDIARG_GETENCRYPTIONBLTKEY* pKey);
HRESULT APIENTRY GetInfoCallback(HANDLE hDevice, UINT DevInfoID, VOID* pDevInfoStruct, UINT DevInfoSize);
HRESULT APIENTRY GetOverlayColorControlsCallback(HANDLE hDevice, D3DDDIARG_GETOVERLAYCOLORCONTROLS* pControl);
HRESULT APIENTRY GetPitchCallback(HANDLE hDevice, D3DDDIARG_GETPITCH* pPitch);
HRESULT APIENTRY GetQueryDataCallback(HANDLE hDevice, CONST D3DDDIARG_GETQUERYDATA* pData);
HRESULT APIENTRY GetVideoProcessBltStatePrivateCallback(HANDLE hDevice, D3DDDIARG_DXVAHD_GETVIDEOPROCESSBLTSTATEPRIVATE* pState);
HRESULT APIENTRY GetVideoProcessStreamStatePrivateCallback(HANDLE hDevice, D3DDDIARG_DXVAHD_GETVIDEOPROCESSSTREAMSTATEPRIVATE* pState);
HRESULT APIENTRY IssueQueryCallback(HANDLE hDevice, CONST D3DDDIARG_ISSUEQUERY* pQuery);
HRESULT APIENTRY LockCallback(HANDLE hDevice, D3DDDIARG_LOCK* pLock);
HRESULT APIENTRY LockAsyncCallback(HANDLE hDevice, D3DDDIARG_LOCKASYNC* pLock);
HRESULT APIENTRY MultiplyTransformCallback(HANDLE hDevice, CONST D3DDDIARG_MULTIPLYTRANSFORM* pTransform);
HRESULT APIENTRY OpenResourceCallback(HANDLE hDevice, D3DDDIARG_OPENRESOURCE* pResource);
HRESULT APIENTRY PresentCallback(HANDLE hDevice, CONST D3DDDIARG_PRESENT* pPresent);
HRESULT APIENTRY QueryAuthenticatedChannelCallback(HANDLE hDevice, CONST D3DDDIARG_QUERYAUTHENTICATEDCHANNEL* pChannel); 
HRESULT APIENTRY QueryResourceResidencyCallback(HANDLE hDevice, CONST D3DDDIARG_QUERYRESOURCERESIDENCY* pResource);
HRESULT APIENTRY RenameCallback(HANDLE hDevice, CONST D3DDDIARG_RENAME* pRename);
HRESULT APIENTRY ResolveSharedResourceCallback(HANDLE hDevice, CONST D3DDDIARG_RESOLVESHAREDRESOURCE* pResource);
HRESULT APIENTRY SetClipPlaneCallback(HANDLE hDevice, CONST D3DDDIARG_SETCLIPPLANE* pPlane);
HRESULT APIENTRY SetConvolutionKernelMonoCallback(HANDLE hDevice, CONST D3DDDIARG_SETCONVOLUTIONKERNELMONO* pConvolution);
HRESULT APIENTRY SetDecodeRenderTargetCallback(HANDLE hDevice, CONST D3DDDIARG_SETDECODERENDERTARGET* pTarget);
HRESULT APIENTRY SetDepthStencilCallback(HANDLE hDevice, CONST D3DDDIARG_SETDEPTHSTENCIL* pStencil);
HRESULT APIENTRY SetDisplayModeCallback(HANDLE hDevice, CONST D3DDDIARG_SETDISPLAYMODE* pMode);
HRESULT APIENTRY SetIndicesCallback(HANDLE hDevice, CONST D3DDDIARG_SETINDICES* pData);
HRESULT APIENTRY SetIndicesUmCallback(HANDLE hDevice, UINT IndexSize, CONST VOID* pUMBuffer);
HRESULT APIENTRY SetLightCallback(HANDLE hDevice, CONST D3DDDIARG_SETLIGHT* pData, CONST D3DDDI_LIGHT* pLightProperties);
HRESULT APIENTRY SetMaterialCallback(HANDLE hDevice, CONST D3DDDIARG_SETMATERIAL* pMaterial);
HRESULT APIENTRY SetOverlayColorControlsCallback(HANDLE hDevice, CONST D3DDDIARG_SETOVERLAYCOLORCONTROLS* pOverlay);
HRESULT APIENTRY SetPaletteCallback(HANDLE hDevice, CONST D3DDDIARG_SETPALETTE* pPalette);
HRESULT APIENTRY SetPixelShaderCallback(HANDLE hDevice, HANDLE hShader);
HRESULT APIENTRY SetPixelShaderConstCallback(HANDLE hDevice, CONST D3DDDIARG_SETPIXELSHADERCONST* pData, CONST FLOAT* pRegisters);
HRESULT APIENTRY SetPixelShaderConstBCallback(HANDLE hDevice, CONST D3DDDIARG_SETPIXELSHADERCONSTB* pData, CONST BOOL* pRegisters);
HRESULT APIENTRY SetPixelShaderConstICallback(HANDLE hDevice, CONST D3DDDIARG_SETPIXELSHADERCONSTI* pData, CONST INT* pRegisters);
HRESULT APIENTRY SetPriorityCallback(HANDLE hDevice, CONST D3DDDIARG_SETPRIORITY* pPriority);
HRESULT APIENTRY SetRenderStateCallback(HANDLE hDevice, CONST D3DDDIARG_RENDERSTATE* pState);
HRESULT APIENTRY SetRenderTargetCallback(HANDLE hDevice, CONST D3DDDIARG_SETRENDERTARGET* pTarget);
HRESULT APIENTRY SetScissorRectCallback(HANDLE hDevice, CONST RECT* pRect);    
HRESULT APIENTRY SetStreamSourceCallback(HANDLE hDevice, CONST D3DDDIARG_SETSTREAMSOURCE* pStream);
HRESULT APIENTRY SetStreamSourceFreqCallback(HANDLE hDevice, CONST D3DDDIARG_SETSTREAMSOURCEFREQ* pFreq);
HRESULT APIENTRY SetStreamSourceUmCallback(HANDLE hDevice, CONST D3DDDIARG_SETSTREAMSOURCEUM* pData, CONST VOID* pUMBuffer);
HRESULT APIENTRY SetTextureCallback(HANDLE hDevice, UINT Stage, HANDLE hTexture);
HRESULT APIENTRY SetTextureStageStateCallback(HANDLE hDevice, CONST D3DDDIARG_TEXTURESTAGESTATE* pState);
HRESULT APIENTRY SetTransformCallback(HANDLE hDevice, CONST D3DDDIARG_SETTRANSFORM* pTransform);
HRESULT APIENTRY SetVertexShaderConstCallback(HANDLE hDevice, CONST D3DDDIARG_SETVERTEXSHADERCONST* pData, CONST VOID* pRegisters);
HRESULT APIENTRY SetVertexShaderConstBCallback(HANDLE hDevice, CONST D3DDDIARG_SETVERTEXSHADERCONSTB* pData, CONST BOOL* pRegisters);
HRESULT APIENTRY SetVertexShaderConstICallback(HANDLE hDevice, CONST D3DDDIARG_SETVERTEXSHADERCONSTI* pData, CONST INT* pRegisters);
HRESULT APIENTRY SetVertexShaderDeclCallback(HANDLE hDevice, HANDLE hDeclaration);
HRESULT APIENTRY SetVertexShaderFuncCallback(HANDLE hDevice, HANDLE hFunction);
HRESULT APIENTRY SetVideoProcessBltStateCallback(HANDLE hDevice, CONST D3DDDIARG_DXVAHD_SETVIDEOPROCESSBLTSTATE* pState);
HRESULT APIENTRY SetVideoProcessRenderTargetCallback(HANDLE hDevice, CONST D3DDDIARG_SETVIDEOPROCESSRENDERTARGET* pTarget);
HRESULT APIENTRY SetVideoProcessStreamStateCallback(HANDLE hDevice, CONST D3DDDIARG_DXVAHD_SETVIDEOPROCESSSTREAMSTATE* pState);
HRESULT APIENTRY SetViewportCallback(HANDLE hDevice, CONST D3DDDIARG_VIEWPORTINFO* pViewPort);
HRESULT APIENTRY SetZRangeCallback(HANDLE hDevice, CONST D3DDDIARG_ZRANGE* pRange);
HRESULT APIENTRY StartSessionKeyRefreshCallback(HANDLE hDevice, CONST D3DDDIARG_STARTSESSIONKEYREFRESH* pKey);
HRESULT APIENTRY StateSetCallback(HANDLE hDevice, D3DDDIARG_STATESET* pState);
HRESULT APIENTRY TexBltCallback(HANDLE hDevice, CONST D3DDDIARG_TEXBLT* pBlt);
HRESULT APIENTRY UnlockCallback(HANDLE hDevice, CONST D3DDDIARG_UNLOCK* pUnLock);
HRESULT APIENTRY UnlockAsyncCallback(HANDLE hDevice, CONST D3DDDIARG_UNLOCKASYNC* pUnLock);
HRESULT APIENTRY UpdateOverlayCallback(HANDLE hDevice, CONST D3DDDIARG_UPDATEOVERLAY* pOverlay);
HRESULT APIENTRY UpdatePaletteCallback(HANDLE hDevice, CONST D3DDDIARG_UPDATEPALETTE* pData, CONST PALETTEENTRY* pPalette);
HRESULT APIENTRY UpdateWInfoCallback(HANDLE hDevice, CONST D3DDDIARG_WINFO* pInfo);
HRESULT APIENTRY ValidateDeviceCallback(HANDLE hDevice, D3DDDIARG_VALIDATETEXTURESTAGESTATE* pData);
HRESULT APIENTRY VideoProcessBeginFrameCallback(HANDLE hDevice, HANDLE hVideoProcess);
HRESULT APIENTRY VideoProcessBltCallback(HANDLE hDevice, CONST D3DDDIARG_VIDEOPROCESSBLT* pBlt);
HRESULT APIENTRY VideoProcessBltHDCallback(HANDLE hDevice, CONST D3DDDIARG_DXVAHD_VIDEOPROCESSBLTHD* pBlt);
HRESULT APIENTRY VideoProcessEndFrameCallback(HANDLE hDevice, D3DDDIARG_VIDEOPROCESSENDFRAME* pFrame);
HRESULT APIENTRY VolBltCallback(HANDLE hDevice, CONST D3DDDIARG_VOLUMEBLT* pBlt);

#define DEBUG_TRACEUM(hDevice)  /*DEBUG_TRACE3( __FUNCTIONW__ _T(". hDevice = 0x%X \n"), hDevice)*/
