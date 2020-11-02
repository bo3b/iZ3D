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
#include "UMDDeviceWrapper.h"

//------------------------------------------ Device unmodified hooks ------------------------------
HRESULT APIENTRY AuthenticatedChannelKeyExchangeCallback(HANDLE hDevice, D3DDDIARG_AUTHENTICATEDCHANNELKEYEXCHANGE* pKey)
{
    DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnAuthenticatedChannelKeyExchange(GetDevice(hDevice), pKey);
	return hResult;
}

HRESULT APIENTRY BufBltCallback(HANDLE hDevice, CONST D3DDDIARG_BUFFERBLT* pBlt)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnBufBlt(GetDevice(hDevice), pBlt);
	return hResult;
}

HRESULT APIENTRY CaptureToSysMemCallback(HANDLE hDevice, CONST D3DDDIARG_CAPTURETOSYSMEM* pCapture)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnCaptureToSysMem(GetDevice(hDevice), pCapture);
	return hResult;
}

HRESULT APIENTRY ClearCallback(HANDLE hDevice, CONST D3DDDIARG_CLEAR* pClear, UINT uCount, CONST RECT* pRect)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnClear(GetDevice(hDevice), pClear, uCount, pRect);
	return hResult;
}

HRESULT APIENTRY ColorFillCallback(HANDLE hDevice, CONST D3DDDIARG_COLORFILL* pFill)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnColorFill(GetDevice(hDevice), pFill);
	return hResult;
}

HRESULT APIENTRY ComposeRectsCallback(HANDLE hDevice, CONST D3DDDIARG_COMPOSERECTS* pRects)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnComposeRects(GetDevice(hDevice), pRects);
	return hResult;
}

HRESULT APIENTRY ConfigureAuthenticatedChannelCallback(HANDLE hDevice, CONST D3DDDIARG_CONFIGUREAUTHENTICATEDCHANNEL* pChannel)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnConfigureAuthenticatedChannel(GetDevice(hDevice), pChannel);
	return hResult;
}

HRESULT APIENTRY CreateAuthenticatedChannelCallback(HANDLE hDevice, D3DDDIARG_CREATEAUTHENTICATEDCHANNEL* pChannel)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnCreateAuthenticatedChannel(GetDevice(hDevice), pChannel);
	return hResult;
}

HRESULT APIENTRY CreateCryptoSessionCallback(HANDLE hDevice, D3DDDIARG_CREATECRYPTOSESSION* pSession)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnCreateCryptoSession(GetDevice(hDevice), pSession);
	return hResult;
}

HRESULT APIENTRY CreateDecodeDeviceCallback(HANDLE hDevice, D3DDDIARG_CREATEDECODEDEVICE* pData)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnCreateDecodeDevice(GetDevice(hDevice), pData);
	return hResult;
}

HRESULT APIENTRY CreateExtensionDeviceCallback(HANDLE hDevice, D3DDDIARG_CREATEEXTENSIONDEVICE* pData)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnCreateExtensionDevice(GetDevice(hDevice), pData);
	return hResult;
}

HRESULT APIENTRY CreateLightCallback(HANDLE hDevice, CONST D3DDDIARG_CREATELIGHT* pLight)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnCreateLight(GetDevice(hDevice), pLight);
	return hResult;
}

HRESULT APIENTRY CreateOverlayCallback(HANDLE hDevice, D3DDDIARG_CREATEOVERLAY* pOverlay)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnCreateOverlay(GetDevice(hDevice), pOverlay);
	return hResult;
}

HRESULT APIENTRY CreatePixelShaderCallback(HANDLE hDevice, D3DDDIARG_CREATEPIXELSHADER* pData, CONST UINT* pCode)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnCreatePixelShader(GetDevice(hDevice), pData, pCode);
	return hResult;
}

HRESULT APIENTRY CreateQueryCallback(HANDLE hDevice, D3DDDIARG_CREATEQUERY* pQuery)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnCreateQuery(GetDevice(hDevice), pQuery);
#ifndef FINAL_RELEASE
	if(SUCCEEDED(hResult))
		g_QueryType[pQuery->hQuery] = pQuery->QueryType;
#endif
	return hResult;
}

HRESULT APIENTRY CreateVertexShaderDeclCallback(HANDLE hDevice, D3DDDIARG_CREATEVERTEXSHADERDECL* pDecl, CONST D3DDDIVERTEXELEMENT* pElement)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnCreateVertexShaderDecl(GetDevice(hDevice), pDecl, pElement);
	return hResult;
}

HRESULT APIENTRY CreateVertexShaderFuncCallback(HANDLE hDevice, D3DDDIARG_CREATEVERTEXSHADERFUNC* pFunc, CONST UINT* pCode)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnCreateVertexShaderFunc(GetDevice(hDevice), pFunc, pCode);
	return hResult;
}

HRESULT APIENTRY CreateVideoProcessDeviceCallback(HANDLE hDevice, D3DDDIARG_CREATEVIDEOPROCESSDEVICE* pData)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnCreateVideoProcessDevice(GetDevice(hDevice), pData);
	return hResult;
}

HRESULT APIENTRY CreateVideoProcessorCallback(HANDLE hDevice, D3DDDIARG_DXVAHD_CREATEVIDEOPROCESSOR* pProcessor)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnCreateVideoProcessor(GetDevice(hDevice), pProcessor);
	return hResult;
}

HRESULT APIENTRY CryptoSessionKeyExchangeCallback(HANDLE hDevice, D3DDDIARG_CRYPTOSESSIONKEYEXCHANGE* pKey) 
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnCryptoSessionKeyExchange(GetDevice(hDevice), pKey);
	return hResult;
}

HRESULT APIENTRY DecodeBeginFrameCallback(HANDLE hDevice, D3DDDIARG_DECODEBEGINFRAME* pFrame)  
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnDecodeBeginFrame(GetDevice(hDevice), pFrame);
	return hResult;
}

HRESULT APIENTRY DecodeEndFrameCallback(HANDLE hDevice, D3DDDIARG_DECODEENDFRAME* pFrame)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnDecodeEndFrame(GetDevice(hDevice), pFrame);
	return hResult;
}

HRESULT APIENTRY DecodeExecuteCallback(HANDLE hDevice, CONST D3DDDIARG_DECODEEXECUTE* pDecode)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnDecodeExecute(GetDevice(hDevice), pDecode);
	return hResult;
}

HRESULT APIENTRY DecodeExtensionExecuteCallback(HANDLE hDevice, CONST D3DDDIARG_DECODEEXTENSIONEXECUTE* pDecode)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnDecodeExtensionExecute(GetDevice(hDevice), pDecode);
	return hResult;
}

HRESULT APIENTRY DecryptionBltCallback(HANDLE hDevice, CONST D3DDDIARG_DECRYPTIONBLT* pBlt)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnDecryptionBlt(GetDevice(hDevice), pBlt);
	return hResult;
}

HRESULT APIENTRY DeletePixelShaderCallback(HANDLE hDevice, HANDLE hShader)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnDeletePixelShader(GetDevice(hDevice), hShader);
	return hResult;
}

HRESULT APIENTRY DeleteVertexShaderDeclCallback(HANDLE hDevice, HANDLE hShader)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnDeleteVertexShaderDecl(GetDevice(hDevice), hShader);
	return hResult;
}

HRESULT APIENTRY DeleteVertexShaderFuncCallback(HANDLE hDevice, HANDLE hShader)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnDeleteVertexShaderFunc(GetDevice(hDevice), hShader);
	return hResult;
}

HRESULT APIENTRY DepthFillCallback(HANDLE hDevice, CONST D3DDDIARG_DEPTHFILL* pFill)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnDepthFill(GetDevice(hDevice), pFill);
	return hResult;
}

HRESULT APIENTRY DestroyAuthenticatedChannelCallback(HANDLE hDevice, CONST D3DDDIARG_DESTROYAUTHENTICATEDCHANNEL* pChannel)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnDestroyAuthenticatedChannel(GetDevice(hDevice), pChannel);
	return hResult;
}

HRESULT APIENTRY DestroyCryptoSessionCallback(HANDLE hDevice, CONST D3DDDIARG_DESTROYCRYPTOSESSION* pSession)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnDestroyCryptoSession(GetDevice(hDevice), pSession);
	return hResult;
}

HRESULT APIENTRY DestroyDecodeDeviceCallback(HANDLE hDevice, HANDLE hDecode)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnDestroyDecodeDevice(GetDevice(hDevice), hDecode);
	return hResult;
}

HRESULT APIENTRY DestroyExtensionDeviceCallback(HANDLE hDevice, HANDLE hExtension)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnDestroyExtensionDevice(GetDevice(hDevice), hExtension);
	return hResult;
}

HRESULT APIENTRY DestroyLightCallback(HANDLE hDevice, CONST D3DDDIARG_DESTROYLIGHT* pLight)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnDestroyLight(GetDevice(hDevice), pLight);
	return hResult;
}

HRESULT APIENTRY DestroyOverlayCallback(HANDLE hDevice, CONST D3DDDIARG_DESTROYOVERLAY* pOverlay)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnDestroyOverlay(GetDevice(hDevice), pOverlay);
	return hResult;
}

HRESULT APIENTRY DestroyQueryCallback(HANDLE hDevice, CONST HANDLE hQuery)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnDestroyQuery(GetDevice(hDevice), hQuery);
#ifndef FINAL_RELEASE
	g_QueryType.erase(hQuery);
#endif
	return hResult;
}

HRESULT APIENTRY DestroyVideoProcessDeviceCallback(HANDLE hDevice, HANDLE hVideoProcessor)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnDestroyVideoProcessDevice(GetDevice(hDevice), hVideoProcessor);
	return hResult;
}

HRESULT APIENTRY DestroyVideoProcessorCallback(HANDLE hDevice, HANDLE hVideoProcessor)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnDestroyVideoProcessor(GetDevice(hDevice), hVideoProcessor);
	return hResult;
}

HRESULT APIENTRY DrawIndexedPrimitive2Callback(HANDLE hDevice, CONST D3DDDIARG_DRAWINDEXEDPRIMITIVE2* pData, UINT dwIndicesSize, CONST VOID* pIndexBuffer, CONST UINT*  pFlagBuffer)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnDrawIndexedPrimitive2(GetDevice(hDevice), pData, dwIndicesSize, pIndexBuffer, pFlagBuffer);
	return hResult;
}

HRESULT APIENTRY DrawIndexedPrimitiveCallback(HANDLE hDevice, CONST D3DDDIARG_DRAWINDEXEDPRIMITIVE* pData)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnDrawIndexedPrimitive(GetDevice(hDevice), pData);
	return hResult;
}

HRESULT APIENTRY DrawPrimitive2Callback(HANDLE hDevice, CONST D3DDDIARG_DRAWPRIMITIVE2* pData)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnDrawPrimitive2(GetDevice(hDevice), pData);
	return hResult;
}

HRESULT APIENTRY DrawPrimitiveCallback(HANDLE hDevice,  CONST D3DDDIARG_DRAWPRIMITIVE* pData, CONST UINT* pFlagBuffer)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnDrawPrimitive(GetDevice(hDevice), pData, pFlagBuffer);
	return hResult;
}

HRESULT APIENTRY DrawRectPatchCallback(HANDLE hDevice, CONST D3DDDIARG_DRAWRECTPATCH* pData, CONST D3DDDIRECTPATCH_INFO* pInfo, CONST FLOAT* pPatch)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnDrawRectPatch(GetDevice(hDevice), pData, pInfo, pPatch);
	return hResult;
}

HRESULT APIENTRY DrawTriPatchCallback(HANDLE hDevice, CONST D3DDDIARG_DRAWTRIPATCH* pData, CONST D3DDDITRIPATCH_INFO* pInfo, CONST FLOAT* pPatch)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnDrawTriPatch(GetDevice(hDevice), pData, pInfo, pPatch);
	return hResult;
}

HRESULT APIENTRY EncryptionBltCallback(HANDLE hDevice, CONST D3DDDIARG_ENCRYPTIONBLT* pBlt)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnEncryptionBlt(GetDevice(hDevice), pBlt);
	return hResult;
}

HRESULT APIENTRY ExtensionExecuteCallback(HANDLE hDevice, CONST D3DDDIARG_EXTENSIONEXECUTE* pExecute)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnExtensionExecute(GetDevice(hDevice), pExecute);
	return hResult;
}

HRESULT APIENTRY FinishSessionKeyRefreshCallback(HANDLE hDevice, CONST D3DDDIARG_FINISHSESSIONKEYREFRESH* pKey)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnFinishSessionKeyRefresh(GetDevice(hDevice), pKey);
	return hResult;
}

HRESULT APIENTRY FlipOverlayCallback(HANDLE hDevice, CONST D3DDDIARG_FLIPOVERLAY* pFlip)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnFlipOverlay(GetDevice(hDevice), pFlip);
	return hResult;
}

HRESULT APIENTRY FlushCallback(HANDLE hDevice)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnFlush(GetDevice(hDevice));
	return hResult;
}

HRESULT APIENTRY GenerateMipSubLevelsCallback(HANDLE hDevice, CONST D3DDDIARG_GENERATEMIPSUBLEVELS* pLevel)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnGenerateMipSubLevels(GetDevice(hDevice), pLevel);
	return hResult;
}

HRESULT APIENTRY GetCaptureAllocationHandleCallback(HANDLE hDevice, D3DDDIARG_GETCAPTUREALLOCATIONHANDLE* pCapture)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnGetCaptureAllocationHandle(GetDevice(hDevice), pCapture);
	return hResult;
}

HRESULT APIENTRY GetEncryptionBltKeyCallback(HANDLE hDevice, CONST D3DDDIARG_GETENCRYPTIONBLTKEY* pKey)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnGetEncryptionBltKey(GetDevice(hDevice), pKey);
	return hResult;
}

HRESULT APIENTRY GetInfoCallback(HANDLE hDevice, UINT DevInfoID, VOID* pDevInfoStruct, UINT DevInfoSize)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnGetInfo(GetDevice(hDevice), DevInfoID, pDevInfoStruct, DevInfoSize);
	return hResult;
}

HRESULT APIENTRY GetOverlayColorControlsCallback(HANDLE hDevice, D3DDDIARG_GETOVERLAYCOLORCONTROLS* pControl)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnGetOverlayColorControls(GetDevice(hDevice), pControl);
	return hResult;
}

HRESULT APIENTRY GetPitchCallback(HANDLE hDevice, D3DDDIARG_GETPITCH* pPitch)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnGetPitch(GetDevice(hDevice), pPitch);
	return hResult;
}

HRESULT APIENTRY GetQueryDataCallback(HANDLE hDevice, CONST D3DDDIARG_GETQUERYDATA* pData)
{
	DEBUG_TRACEUM(hDevice);
	WCHAR* pFunctionName = __FUNCTIONW__ L"Unknown";
#ifndef FINAL_RELEASE
	QueriesTypeMap::const_iterator it = g_QueryType.find(pData->hQuery);
	if (it != g_QueryType.end())
	{
		D3DDDIQUERYTYPE type = it->second;
		if (type == D3DDDIQUERYTYPE_VCACHE)
			pFunctionName = __FUNCTIONW__ L"VCache";
		else if (type == D3DDDIQUERYTYPE_RESOURCEMANAGER)
			pFunctionName = __FUNCTIONW__ L"ResourceManager";
		else if (type == D3DDDIQUERYTYPE_VERTEXSTATS)
			pFunctionName = __FUNCTIONW__ L"VertexStats";
		else if (type == D3DDDIQUERYTYPE_DDISTATS)
			pFunctionName = __FUNCTIONW__ L"DDIStats";
		else if (type == D3DDDIQUERYTYPE_EVENT)
			pFunctionName = __FUNCTIONW__ L"Event";
		else if (type == D3DDDIQUERYTYPE_OCCLUSION)
			pFunctionName = __FUNCTIONW__ L"Occlusion";
		else if (type == D3DDDIQUERYTYPE_TIMESTAMP)
			pFunctionName = __FUNCTIONW__ L"TimeStamp";
		else if (type == D3DDDIQUERYTYPE_TIMESTAMPDISJOINT)
			pFunctionName = __FUNCTIONW__ L"TimeStampDisjoint";
		else if (type == D3DDDIQUERYTYPE_TIMESTAMPFREQ)
			pFunctionName = __FUNCTIONW__ L"TimeStampFreq";
		else if (type == D3DDDIQUERYTYPE_PIPELINETIMINGS)
			pFunctionName = __FUNCTIONW__ L"PipelineTimings";
		else if (type == D3DDDIQUERYTYPE_INTERFACETIMINGS)
			pFunctionName = __FUNCTIONW__ L"InterfaceTimings";
		else if (type == D3DDDIQUERYTYPE_VERTEXTIMINGS)
			pFunctionName = __FUNCTIONW__ L"VertexTimings";
		else if (type == D3DDDIQUERYTYPE_PIXELTIMINGS)
			pFunctionName = __FUNCTIONW__ L"PixelTimings";
		else if (type == D3DDDIQUERYTYPE_BANDWIDTHTIMINGS)
			pFunctionName = __FUNCTIONW__ L"BandwidthTimings";
		else if (type == D3DDDIQUERYTYPE_CACHEUTILIZATION)
			pFunctionName = __FUNCTIONW__ L"CacheUtilization";
	}
#endif
	UMCallGuard cg(hDevice, pFunctionName);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnGetQueryData(GetDevice(hDevice), pData);
	return hResult;
}

HRESULT APIENTRY GetVideoProcessBltStatePrivateCallback(HANDLE hDevice, D3DDDIARG_DXVAHD_GETVIDEOPROCESSBLTSTATEPRIVATE* pState)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnGetVideoProcessBltStatePrivate(GetDevice(hDevice), pState);
	return hResult;
}

HRESULT APIENTRY GetVideoProcessStreamStatePrivateCallback(HANDLE hDevice, D3DDDIARG_DXVAHD_GETVIDEOPROCESSSTREAMSTATEPRIVATE* pState)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnGetVideoProcessStreamStatePrivate(GetDevice(hDevice), pState);
	return hResult;
}

HRESULT APIENTRY IssueQueryCallback(HANDLE hDevice, CONST D3DDDIARG_ISSUEQUERY* pData)
{
	DEBUG_TRACEUM(hDevice);
	WCHAR* pFunctionName = __FUNCTIONW__ L"Unknown";
#ifndef FINAL_RELEASE
	QueriesTypeMap::const_iterator it = g_QueryType.find(pData->hQuery);
	if (it != g_QueryType.end())
	{
		D3DDDIQUERYTYPE type = it->second;
		if (type == D3DDDIQUERYTYPE_VCACHE)
			pFunctionName = __FUNCTIONW__ L"VCache";
		else if (type == D3DDDIQUERYTYPE_RESOURCEMANAGER)
			pFunctionName = __FUNCTIONW__ L"ResourceManager";
		else if (type == D3DDDIQUERYTYPE_VERTEXSTATS)
			pFunctionName = __FUNCTIONW__ L"VertexStats";
		else if (type == D3DDDIQUERYTYPE_DDISTATS)
			pFunctionName = __FUNCTIONW__ L"DDIStats";
		else if (type == D3DDDIQUERYTYPE_EVENT)
			pFunctionName = __FUNCTIONW__ L"Event";
		else if (type == D3DDDIQUERYTYPE_OCCLUSION)
			pFunctionName = __FUNCTIONW__ L"Occlusion";
		else if (type == D3DDDIQUERYTYPE_TIMESTAMP)
			pFunctionName = __FUNCTIONW__ L"TimeStamp";
		else if (type == D3DDDIQUERYTYPE_TIMESTAMPDISJOINT)
			pFunctionName = __FUNCTIONW__ L"TimeStampDisjoint";
		else if (type == D3DDDIQUERYTYPE_TIMESTAMPFREQ)
			pFunctionName = __FUNCTIONW__ L"TimeStampFreq";
		else if (type == D3DDDIQUERYTYPE_PIPELINETIMINGS)
			pFunctionName = __FUNCTIONW__ L"PipelineTimings";
		else if (type == D3DDDIQUERYTYPE_INTERFACETIMINGS)
			pFunctionName = __FUNCTIONW__ L"InterfaceTimings";
		else if (type == D3DDDIQUERYTYPE_VERTEXTIMINGS)
			pFunctionName = __FUNCTIONW__ L"VertexTimings";
		else if (type == D3DDDIQUERYTYPE_PIXELTIMINGS)
			pFunctionName = __FUNCTIONW__ L"PixelTimings";
		else if (type == D3DDDIQUERYTYPE_BANDWIDTHTIMINGS)
			pFunctionName = __FUNCTIONW__ L"BandwidthTimings";
		else if (type == D3DDDIQUERYTYPE_CACHEUTILIZATION)
			pFunctionName = __FUNCTIONW__ L"CacheUtilization";
	}
#endif
	UMCallGuard cg(hDevice, pFunctionName);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnIssueQuery(GetDevice(hDevice), pData);
	return hResult;
}

HRESULT APIENTRY LockAsyncCallback(HANDLE hDevice, D3DDDIARG_LOCKASYNC* pLock)
{
	DEBUG_TRACEUM(hDevice);
	//static DWORD time1 = timeGetTime();
	//DWORD time2 = timeGetTime();
	//if(time2  - time1 > 1000 * 30)
	//	_asm nop
	WCHAR* pFunctionName = __FUNCTIONW__ L"Unknown";
#ifndef FINAL_RELEASE
	ResourcesTypeMap::const_iterator it = g_ResourceType.find(pLock->hResource);
	if (it != g_ResourceType.end())
	{
		D3DDDI_RESOURCEFLAGS flags = it->second;
		if (flags.RenderTarget)
			pFunctionName = __FUNCTIONW__ L"RenderTarget";
		else if (flags.ZBuffer)
			pFunctionName = __FUNCTIONW__ L"ZBuffer";
		else if (flags.Primary)
			pFunctionName = __FUNCTIONW__ L"Primary";
		else if (flags.Texture)
			pFunctionName = __FUNCTIONW__ L"Texture";
		else if (flags.CubeMap)
			pFunctionName = __FUNCTIONW__ L"CubeMap";
		else if (flags.Volume)
			pFunctionName = __FUNCTIONW__ L"Volume";
		else if (flags.VertexBuffer)
			pFunctionName = __FUNCTIONW__ L"VertexBuffer";
		else if (flags.IndexBuffer)
			pFunctionName = __FUNCTIONW__ L"IndexBuffer";
		else if (flags.SharedResource)
			pFunctionName = __FUNCTIONW__ L"SharedResource";
	}
#endif
	UMCallGuard cg(hDevice, pFunctionName);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnLockAsync(hDevice, pLock);
	return hResult;
}

HRESULT APIENTRY MultiplyTransformCallback(HANDLE hDevice, CONST D3DDDIARG_MULTIPLYTRANSFORM* pTransform)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnMultiplyTransform(GetDevice(hDevice), pTransform);
	return hResult;
}

HRESULT APIENTRY QueryAuthenticatedChannelCallback(HANDLE hDevice, CONST D3DDDIARG_QUERYAUTHENTICATEDCHANNEL* pChannel) 
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnQueryAuthenticatedChannel(GetDevice(hDevice), pChannel);
	return hResult;
}

HRESULT APIENTRY QueryResourceResidencyCallback(HANDLE hDevice, CONST D3DDDIARG_QUERYRESOURCERESIDENCY* pResource)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnQueryResourceResidency(GetDevice(hDevice), pResource);
	return hResult;
}

HRESULT APIENTRY RenameCallback(HANDLE hDevice, CONST D3DDDIARG_RENAME* pRename)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnRename(GetDevice(hDevice), pRename);
	return hResult;
}

HRESULT APIENTRY ResolveSharedResourceCallback(HANDLE hDevice, CONST D3DDDIARG_RESOLVESHAREDRESOURCE* pResource)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnResolveSharedResource(GetDevice(hDevice), pResource);
	return hResult;
}

HRESULT APIENTRY SetClipPlaneCallback(HANDLE hDevice, CONST D3DDDIARG_SETCLIPPLANE* pPlane)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnSetClipPlane(GetDevice(hDevice), pPlane);
	return hResult;
}

HRESULT APIENTRY SetConvolutionKernelMonoCallback(HANDLE hDevice, CONST D3DDDIARG_SETCONVOLUTIONKERNELMONO* pConvolution)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnSetConvolutionKernelMono(GetDevice(hDevice), pConvolution);
	return hResult;
}

HRESULT APIENTRY SetDecodeRenderTargetCallback(HANDLE hDevice, CONST D3DDDIARG_SETDECODERENDERTARGET* pTarget)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnSetDecodeRenderTarget(GetDevice(hDevice), pTarget);
	return hResult;
}

HRESULT APIENTRY SetDepthStencilCallback(HANDLE hDevice, CONST D3DDDIARG_SETDEPTHSTENCIL* pStencil)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnSetDepthStencil(GetDevice(hDevice), pStencil);
	return hResult;
}

HRESULT APIENTRY SetDisplayModeCallback(HANDLE hDevice, CONST D3DDDIARG_SETDISPLAYMODE* pMode)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnSetDisplayMode(GetDevice(hDevice), pMode);
	return hResult;
}

HRESULT APIENTRY SetIndicesCallback(HANDLE hDevice, CONST D3DDDIARG_SETINDICES* pData)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnSetIndices(GetDevice(hDevice), pData);
	return hResult;
}

HRESULT APIENTRY SetIndicesUmCallback(HANDLE hDevice, UINT IndexSize, CONST VOID* pUMBuffer)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnSetIndicesUm(GetDevice(hDevice), IndexSize, pUMBuffer);
	return hResult;
}

HRESULT APIENTRY SetLightCallback(HANDLE hDevice, CONST D3DDDIARG_SETLIGHT* pData, CONST D3DDDI_LIGHT* pLightProperties)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnSetLight(GetDevice(hDevice), pData, pLightProperties);
	return hResult;
}

HRESULT APIENTRY SetMaterialCallback(HANDLE hDevice, CONST D3DDDIARG_SETMATERIAL* pMaterial)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnSetMaterial(GetDevice(hDevice), pMaterial);
	return hResult;
}

HRESULT APIENTRY SetOverlayColorControlsCallback(HANDLE hDevice, CONST D3DDDIARG_SETOVERLAYCOLORCONTROLS* pOverlay)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnSetOverlayColorControls(GetDevice(hDevice), pOverlay);
	return hResult;
}

HRESULT APIENTRY SetPaletteCallback(HANDLE hDevice, CONST D3DDDIARG_SETPALETTE* pPalette)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnSetPalette(GetDevice(hDevice), pPalette);
	return hResult;
}

HRESULT APIENTRY SetPixelShaderCallback(HANDLE hDevice, HANDLE hShader)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnSetPixelShader(GetDevice(hDevice), hShader);
	return hResult;
}

HRESULT APIENTRY SetPixelShaderConstCallback(HANDLE hDevice, CONST D3DDDIARG_SETPIXELSHADERCONST* pData, CONST FLOAT* pRegisters)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnSetPixelShaderConst(GetDevice(hDevice), pData, pRegisters);
	return hResult;
}

HRESULT APIENTRY SetPixelShaderConstBCallback(HANDLE hDevice, CONST D3DDDIARG_SETPIXELSHADERCONSTB* pData, CONST BOOL* pRegisters)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnSetPixelShaderConstB(GetDevice(hDevice), pData, pRegisters);
	return hResult;
}

HRESULT APIENTRY SetPixelShaderConstICallback(HANDLE hDevice, CONST D3DDDIARG_SETPIXELSHADERCONSTI* pData, CONST INT* pRegisters)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnSetPixelShaderConstI(GetDevice(hDevice), pData, pRegisters);
	return hResult;
}

HRESULT APIENTRY SetPriorityCallback(HANDLE hDevice, CONST D3DDDIARG_SETPRIORITY* pPriority)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnSetPriority(GetDevice(hDevice), pPriority);
	return hResult;
}

HRESULT APIENTRY SetRenderStateCallback(HANDLE hDevice, CONST D3DDDIARG_RENDERSTATE* pState)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnSetRenderState(GetDevice(hDevice), pState);
	return hResult;
}

HRESULT APIENTRY SetRenderTargetCallback(HANDLE hDevice, CONST D3DDDIARG_SETRENDERTARGET* pTarget)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnSetRenderTarget(GetDevice(hDevice), pTarget);
	return hResult;
}

HRESULT APIENTRY SetScissorRectCallback(HANDLE hDevice, CONST RECT* pRect)    
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnSetScissorRect(GetDevice(hDevice), pRect);
	return hResult;
}

HRESULT APIENTRY SetStreamSourceCallback(HANDLE hDevice, CONST D3DDDIARG_SETSTREAMSOURCE* pStream)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnSetStreamSource(GetDevice(hDevice), pStream);
	return hResult;
}

HRESULT APIENTRY SetStreamSourceFreqCallback(HANDLE hDevice, CONST D3DDDIARG_SETSTREAMSOURCEFREQ* pFreq)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnSetStreamSourceFreq(GetDevice(hDevice), pFreq);
	return hResult;
}

HRESULT APIENTRY SetStreamSourceUmCallback(HANDLE hDevice, CONST D3DDDIARG_SETSTREAMSOURCEUM* pData, CONST VOID* pUMBuffer)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnSetStreamSourceUm(GetDevice(hDevice), pData, pUMBuffer);
	return hResult;
}

HRESULT APIENTRY SetTextureCallback(HANDLE hDevice, UINT Stage, HANDLE hTexture)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnSetTexture(GetDevice(hDevice), Stage, hTexture);
	return hResult;
}

HRESULT APIENTRY SetTextureStageStateCallback(HANDLE hDevice, CONST D3DDDIARG_TEXTURESTAGESTATE* pState)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnSetTextureStageState(GetDevice(hDevice), pState);
	return hResult;
}

HRESULT APIENTRY SetTransformCallback(HANDLE hDevice, CONST D3DDDIARG_SETTRANSFORM* pTransform)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnSetTransform(GetDevice(hDevice), pTransform);
	return hResult;
}

HRESULT APIENTRY SetVertexShaderConstCallback(HANDLE hDevice, CONST D3DDDIARG_SETVERTEXSHADERCONST* pData, CONST VOID* pRegisters)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnSetVertexShaderConst(GetDevice(hDevice), pData, pRegisters);
	return hResult;
}

HRESULT APIENTRY SetVertexShaderConstBCallback(HANDLE hDevice, CONST D3DDDIARG_SETVERTEXSHADERCONSTB* pData, CONST BOOL* pRegisters)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnSetVertexShaderConstB(GetDevice(hDevice), pData, pRegisters);
	return hResult;
}

HRESULT APIENTRY SetVertexShaderConstICallback(HANDLE hDevice, CONST D3DDDIARG_SETVERTEXSHADERCONSTI* pData, CONST INT* pRegisters)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnSetVertexShaderConstI(GetDevice(hDevice), pData, pRegisters);
	return hResult;
}

HRESULT APIENTRY SetVertexShaderDeclCallback(HANDLE hDevice, HANDLE hDeclaration)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnSetVertexShaderDecl(GetDevice(hDevice), hDeclaration);
	return hResult;
}

HRESULT APIENTRY SetVertexShaderFuncCallback(HANDLE hDevice, HANDLE hFunction)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnSetVertexShaderFunc(GetDevice(hDevice), hFunction);
	return hResult;
}

HRESULT APIENTRY SetVideoProcessBltStateCallback(HANDLE hDevice, CONST D3DDDIARG_DXVAHD_SETVIDEOPROCESSBLTSTATE* pState)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnSetVideoProcessBltState(GetDevice(hDevice), pState);
	return hResult;
}

HRESULT APIENTRY SetVideoProcessRenderTargetCallback(HANDLE hDevice, CONST D3DDDIARG_SETVIDEOPROCESSRENDERTARGET* pTarget)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnSetVideoProcessRenderTarget(GetDevice(hDevice), pTarget);
	return hResult;
}

HRESULT APIENTRY SetVideoProcessStreamStateCallback(HANDLE hDevice, CONST D3DDDIARG_DXVAHD_SETVIDEOPROCESSSTREAMSTATE* pState)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnSetVideoProcessStreamState(GetDevice(hDevice), pState);
	return hResult;
}

HRESULT APIENTRY SetViewportCallback(HANDLE hDevice, CONST D3DDDIARG_VIEWPORTINFO* pViewPort)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnSetViewport(GetDevice(hDevice), pViewPort);
	return hResult;
}

HRESULT APIENTRY SetZRangeCallback(HANDLE hDevice, CONST D3DDDIARG_ZRANGE* pRange)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnSetZRange(GetDevice(hDevice), pRange);
	return hResult;
}

HRESULT APIENTRY StartSessionKeyRefreshCallback(HANDLE hDevice, CONST D3DDDIARG_STARTSESSIONKEYREFRESH* pKey)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnStartSessionKeyRefresh(GetDevice(hDevice), pKey);
	return hResult;
}

HRESULT APIENTRY StateSetCallback(HANDLE hDevice, D3DDDIARG_STATESET* pState)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnStateSet(GetDevice(hDevice), pState);
	return hResult;
}

HRESULT APIENTRY TexBltCallback(HANDLE hDevice, CONST D3DDDIARG_TEXBLT* pBlt)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnTexBlt(GetDevice(hDevice), pBlt);
	return hResult;
}

HRESULT APIENTRY UnlockAsyncCallback(HANDLE hDevice, CONST D3DDDIARG_UNLOCKASYNC* pUnLock)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnUnlockAsync(GetDevice(hDevice), pUnLock);
	return hResult;
}

HRESULT APIENTRY UpdateOverlayCallback(HANDLE hDevice, CONST D3DDDIARG_UPDATEOVERLAY* pOverlay)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnUpdateOverlay(GetDevice(hDevice), pOverlay);
	return hResult;
}

HRESULT APIENTRY UpdatePaletteCallback(HANDLE hDevice, CONST D3DDDIARG_UPDATEPALETTE* pData, CONST PALETTEENTRY* pPalette)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnUpdatePalette(GetDevice(hDevice), pData, pPalette);
	return hResult;
}

HRESULT APIENTRY UpdateWInfoCallback(HANDLE hDevice, CONST D3DDDIARG_WINFO* pInfo)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnUpdateWInfo(GetDevice(hDevice), pInfo);
	return hResult;
}

HRESULT APIENTRY ValidateDeviceCallback(HANDLE hDevice, D3DDDIARG_VALIDATETEXTURESTAGESTATE* pData)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnValidateDevice(GetDevice(hDevice), pData);
	return hResult;
}

HRESULT APIENTRY VideoProcessBeginFrameCallback(HANDLE hDevice, HANDLE hVideoProcess)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnVideoProcessBeginFrame(GetDevice(hDevice), hVideoProcess);
	return hResult;
}

HRESULT APIENTRY VideoProcessBltCallback(HANDLE hDevice, CONST D3DDDIARG_VIDEOPROCESSBLT* pBlt)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnVideoProcessBlt(GetDevice(hDevice), pBlt);
	return hResult;
}

HRESULT APIENTRY VideoProcessBltHDCallback(HANDLE hDevice, CONST D3DDDIARG_DXVAHD_VIDEOPROCESSBLTHD* pBlt)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnVideoProcessBltHD(GetDevice(hDevice), pBlt);
	return hResult;
}

HRESULT APIENTRY VideoProcessEndFrameCallback(HANDLE hDevice, D3DDDIARG_VIDEOPROCESSENDFRAME* pFrame)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnVideoProcessEndFrame(GetDevice(hDevice), pFrame);
	return hResult;
}

HRESULT APIENTRY VolBltCallback(HANDLE hDevice, CONST D3DDDIARG_VOLUMEBLT* pBlt)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	HRESULT hResult = GetDeviceFunctions(hDevice)->pfnVolBlt(GetDevice(hDevice), pBlt);
	return hResult;
}
