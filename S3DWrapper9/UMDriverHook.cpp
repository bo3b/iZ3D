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

#include "stdafx.h"
#define NTSTATUS LONG
#include "..\lib\d3d10\Include\d3dkmthk.h"
#include "UMDriverHook.h"
#include "UMDDeviceWrapper.h"
#include "BaseStereoRenderer.h"
#define  ASSIGN_CALLBACK(pFN, pCallbac)  if(pFN) pFN = pCallbac
#define  ADD_CUSTOM_FORMAT_COUNT 1

//------------------------------------ Adapter hooks -----------------------------------------------
HRESULT (APIENTRY GetCapsCallback)(HANDLE hAdapter, CONST D3DDDIARG_GETCAPS* pData)
{
	DEBUG_TRACE3(_T(__FUNCTION__)  _T("\n"));
	return ((UMWrapper*)hAdapter)->GetCaps(pData);
}

D3DKMT_HANDLE g_hKMAdapter = NULL;
D3DKMT_HANDLE g_hKMDevice = NULL;

HRESULT (APIENTRY *D3DKMTCreateDeviceNext)(_D3DKMT_CREATEDEVICE*) = NULL;
HRESULT APIENTRY D3DKMTCreateDeviceCallback( _D3DKMT_CREATEDEVICE* pData )
{
	HRESULT hr = D3DKMTCreateDeviceNext(pData);
	g_hKMAdapter = pData->hAdapter;
	g_hKMDevice = pData->hDevice;
	return hr;
}

_D3DKMT_PRESENT g_KMPresentData;
HRESULT (APIENTRY *D3DKMTPresentNext)(CONST _D3DKMT_PRESENT*) = NULL;
HRESULT APIENTRY D3DKMTPresentCallback(CONST _D3DKMT_PRESENT* pData )
{
	HRESULT hr = D3DKMTPresentNext(pData);
	if(SUCCEEDED(hr))
		g_KMPresentData = *pData;
	return hr;
}

HRESULT (APIENTRY CreateDeviceCallback)(HANDLE hAdapter, D3DDDIARG_CREATEDEVICE* pDevice)
{
	DEBUG_TRACE3(_T(__FUNCTION__)  _T("\n"));
	return ((UMWrapper*)hAdapter)->CreateDevice(pDevice);
}

HRESULT (APIENTRY CloseAdapterCallback)(HANDLE hAdapter)
{
	DEBUG_TRACE3(_T(__FUNCTION__)  _T("\n"));
	//--- call original function ---
	HRESULT hResult = GetAdapterFunctions(hAdapter)->pfnCloseAdapter(GetAdapter(hAdapter));
	if(SUCCEEDED(hResult))
		delete (UMWrapper*)hAdapter;
	return hResult;
}

HRESULT (WINAPI *OpenAdapterNext)(_D3DDDIARG_OPENADAPTER*) = NULL;

HRESULT WINAPI OpenAdapterCallback(_D3DDDIARG_OPENADAPTER* pOpenData)
{
	DEBUG_TRACE3(_T(__FUNCTION__)  _T("\n"));
	HRESULT hResult = OpenAdapterNext(pOpenData);
	if (SUCCEEDED(hResult))
	{
		UMWrapper* wrapper = DNew UMWrapper(pOpenData);
		pOpenData->hAdapter = wrapper; 
		ASSIGN_CALLBACK(pOpenData->pAdapterFuncs->pfnGetCaps, GetCapsCallback);
		ASSIGN_CALLBACK(pOpenData->pAdapterFuncs->pfnCreateDevice, CreateDeviceCallback);
		ASSIGN_CALLBACK(pOpenData->pAdapterFuncs->pfnCloseAdapter, CloseAdapterCallback);
	}
	return hResult;
}

//--------------------------------------------- Adapter wrapper -----------------------------------
UMWrapper::UMWrapper(_D3DDDIARG_OPENADAPTER* pOpenData)
: m_FormatsCount ( 0 )
{
	m_hAdapter = pOpenData->hAdapter;
	m_AdapterFunctions = *pOpenData->pAdapterFuncs;
}

typedef struct _FORMATOP_EX				// Custom format to render FourCC representation of formats
{
	union
	{
		D3DDDIFORMAT  Format;
		char		  FourCC[4];
	};
	UINT          Operations;
	UINT          FlipMsTypes;
	UINT          BltMsTypes;
	UINT          PrivateFormatBitCount;
} FORMATOP_EX;

HRESULT UMWrapper::GetCaps(CONST D3DDDIARG_GETCAPS* pData)
{
	//--- call original function ---
	HRESULT hResult = m_AdapterFunctions.pfnGetCaps(m_hAdapter, pData);
	if(SUCCEEDED(hResult))
	{
		switch(pData->Type)
		{
		case D3DDDICAPS_GETFORMATCOUNT:
			// add our formats 
			*(DWORD*)(pData->pData) += ADD_CUSTOM_FORMAT_COUNT;
			m_FormatsCount = *(DWORD*)(pData->pData);
			break;
		case D3DDDICAPS_GETFORMATDATA:
			{
#ifndef FINAL_RELEASE
				FORMATOP_EX* formatListEx = (FORMATOP_EX*)pData->pData;
#endif
				FORMATOP* formatList = (FORMATOP*)pData->pData;
				DWORD originalFormatsCount = m_FormatsCount - ADD_CUSTOM_FORMAT_COUNT;
				//  add FOURCC('A','Q','B','S') format support
				FORMATOP* pAQBSFormat = &formatList[originalFormatsCount];
				pAQBSFormat->Format      = (D3DDDIFORMAT)MAKEFOURCC('A','Q','B','S');
				pAQBSFormat->Operations  = FORMATOP_CONVERT_TO_ARGB | FORMATOP_OFFSCREENPLAIN ;
				pAQBSFormat->FlipMsTypes = 0;
				pAQBSFormat->BltMsTypes  = 0;
				pAQBSFormat->PrivateFormatBitCount = 0;
				_ASSERT(pData->DataSize == sizeof( FORMATOP ) * (originalFormatsCount + ADD_CUSTOM_FORMAT_COUNT));
				// TODO: dump the list of supported formats to the log
				break;
			}
		}
	}
	return hResult;
}

HRESULT UMWrapper::CreateDevice(D3DDDIARG_CREATEDEVICE* pDevice)
{
	//--- call original function ---
	HRESULT hResult = m_AdapterFunctions.pfnCreateDevice(m_hAdapter, pDevice);
	if (SUCCEEDED(hResult) && (pDevice->Interface == 9) && 
		(pDevice->Version == D3D_UMD_INTERFACE_VERSION_VISTA || pDevice->Version == D3D_UMD_INTERFACE_VERSION_WIN7) &&
		g_pCurrentStereoRenderer)	//--- g_pCurrentStereoRenderer is NULL for D3DDEVTYPE_NULLREF and D3DDEVTYPE_SW devices. Skip it. ---
	{
		UMDDeviceWrapper* device = DNew UMDDeviceWrapper(this, pDevice, g_hKMAdapter, g_hKMDevice);
		pDevice->hDevice = device;
		device->SetStereoRenderer(g_pCurrentStereoRenderer);
		g_hKMAdapter = NULL;
		g_hKMDevice = NULL;
		g_pUMDeviceWrapper = device;
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnBlt, BltCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnBufBlt, BufBltCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnCaptureToSysMem, CaptureToSysMemCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnClear, ClearCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnColorFill, ColorFillCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnComposeRects, ComposeRectsCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnCreateDecodeDevice, CreateDecodeDeviceCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnCreateExtensionDevice, CreateExtensionDeviceCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnCreateLight, CreateLightCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnCreateOverlay, CreateOverlayCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnCreatePixelShader, CreatePixelShaderCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnCreateQuery, CreateQueryCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnCreateResource, CreateResourceCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnCreateVertexShaderDecl, CreateVertexShaderDeclCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnCreateVertexShaderFunc, CreateVertexShaderFuncCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnCreateVideoProcessDevice, CreateVideoProcessDeviceCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnDecodeBeginFrame, DecodeBeginFrameCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnDecodeEndFrame, DecodeEndFrameCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnDecodeExecute, DecodeExecuteCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnDecodeExtensionExecute, DecodeExtensionExecuteCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnDeletePixelShader, DeletePixelShaderCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnDeleteVertexShaderDecl, DeleteVertexShaderDeclCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnDeleteVertexShaderFunc, DeleteVertexShaderFuncCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnDepthFill, DepthFillCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnDestroyDecodeDevice, DestroyDecodeDeviceCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnDestroyDevice, DestroyDeviceCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnDestroyExtensionDevice, DestroyExtensionDeviceCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnDestroyLight, DestroyLightCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnDestroyOverlay, DestroyOverlayCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnDestroyQuery, DestroyQueryCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnDestroyResource, DestroyResourceCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnDestroyVideoProcessDevice, DestroyVideoProcessDeviceCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnDrawIndexedPrimitive2, DrawIndexedPrimitive2Callback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnDrawIndexedPrimitive, DrawIndexedPrimitiveCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnDrawPrimitive2, DrawPrimitive2Callback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnDrawPrimitive, DrawPrimitiveCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnDrawRectPatch, DrawRectPatchCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnDrawTriPatch, DrawTriPatchCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnExtensionExecute, ExtensionExecuteCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnFlipOverlay, FlipOverlayCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnFlush, FlushCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnGenerateMipSubLevels, GenerateMipSubLevelsCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnGetCaptureAllocationHandle, GetCaptureAllocationHandleCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnGetInfo, GetInfoCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnGetOverlayColorControls, GetOverlayColorControlsCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnGetQueryData, GetQueryDataCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnIssueQuery, IssueQueryCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnLock, LockCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnLockAsync, LockAsyncCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnMultiplyTransform, MultiplyTransformCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnOpenResource, OpenResourceCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnPresent, PresentCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnQueryResourceResidency, QueryResourceResidencyCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnRename, RenameCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnSetClipPlane, SetClipPlaneCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnSetConvolutionKernelMono, SetConvolutionKernelMonoCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnSetDecodeRenderTarget, SetDecodeRenderTargetCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnSetDepthStencil, SetDepthStencilCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnSetDisplayMode, SetDisplayModeCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnSetIndices, SetIndicesCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnSetIndicesUm, SetIndicesUmCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnSetLight, SetLightCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnSetMaterial, SetMaterialCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnSetOverlayColorControls, SetOverlayColorControlsCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnSetPalette, SetPaletteCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnSetPixelShader, SetPixelShaderCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnSetPixelShaderConst, SetPixelShaderConstCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnSetPixelShaderConstB, SetPixelShaderConstBCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnSetPixelShaderConstI, SetPixelShaderConstICallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnSetPriority, SetPriorityCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnSetRenderState, SetRenderStateCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnSetRenderTarget, SetRenderTargetCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnSetScissorRect, SetScissorRectCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnSetStreamSource, SetStreamSourceCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnSetStreamSourceFreq, SetStreamSourceFreqCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnSetStreamSourceUm, SetStreamSourceUmCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnSetTexture, SetTextureCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnSetTextureStageState, SetTextureStageStateCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnSetTransform, SetTransformCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnSetVertexShaderConst, SetVertexShaderConstCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnSetVertexShaderConstB, SetVertexShaderConstBCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnSetVertexShaderConstI, SetVertexShaderConstICallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnSetVertexShaderDecl, SetVertexShaderDeclCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnSetVertexShaderFunc, SetVertexShaderFuncCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnSetVideoProcessRenderTarget, SetVideoProcessRenderTargetCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnSetViewport, SetViewportCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnSetZRange, SetZRangeCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnStateSet, StateSetCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnTexBlt, TexBltCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnUnlock, UnlockCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnUnlockAsync, UnlockAsyncCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnUpdateOverlay, UpdateOverlayCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnUpdatePalette, UpdatePaletteCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnUpdateWInfo, UpdateWInfoCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnValidateDevice, ValidateDeviceCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnVideoProcessBeginFrame, VideoProcessBeginFrameCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnVideoProcessBlt, VideoProcessBltCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnVideoProcessEndFrame, VideoProcessEndFrameCallback);
		ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnVolBlt, VolBltCallback);

		if(pDevice->Version == D3D_UMD_INTERFACE_VERSION_WIN7)
		{
			ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnCreateVideoProcessor, CreateVideoProcessorCallback);
			ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnSetVideoProcessBltState, SetVideoProcessBltStateCallback);
			ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnGetVideoProcessBltStatePrivate, GetVideoProcessBltStatePrivateCallback);
			ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnSetVideoProcessStreamState, SetVideoProcessStreamStateCallback);
			ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnGetVideoProcessStreamStatePrivate, GetVideoProcessStreamStatePrivateCallback);
			ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnVideoProcessBltHD, VideoProcessBltHDCallback);
			ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnDestroyVideoProcessor, DestroyVideoProcessorCallback);
			ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnCreateAuthenticatedChannel, CreateAuthenticatedChannelCallback);
			ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnAuthenticatedChannelKeyExchange, AuthenticatedChannelKeyExchangeCallback);
			ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnQueryAuthenticatedChannel, QueryAuthenticatedChannelCallback);
			ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnConfigureAuthenticatedChannel, ConfigureAuthenticatedChannelCallback);
			ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnDestroyAuthenticatedChannel, DestroyAuthenticatedChannelCallback);
			ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnCreateCryptoSession, CreateCryptoSessionCallback);
			ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnCryptoSessionKeyExchange, CryptoSessionKeyExchangeCallback);
			ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnDestroyCryptoSession, DestroyCryptoSessionCallback);
			ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnEncryptionBlt, EncryptionBltCallback);
			ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnGetPitch, GetPitchCallback);
			ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnStartSessionKeyRefresh, StartSessionKeyRefreshCallback);
			ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnFinishSessionKeyRefresh, FinishSessionKeyRefreshCallback);
			ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnGetEncryptionBltKey, GetEncryptionBltKeyCallback);
			ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnDecryptionBlt, DecryptionBltCallback);
			ASSIGN_CALLBACK(pDevice->pDeviceFuncs->pfnResolveSharedResource, ResolveSharedResourceCallback);
		}
	}
	return hResult;
}
