#pragma once

// Can't forward win and dx typedefs, so include def headers
#include <windef.h>
#include <d3d9types.h>

enum PIPELINE;
enum TextureType;

const TCHAR *GetMatrixString(CONST D3DMATRIX *mat, bool bTranspose);
const TCHAR *GetFormatString(D3DFORMAT format);
const TCHAR *GetResourceTypeString(D3DRESOURCETYPE ResType);
const TCHAR *GetMultiSampleString(D3DMULTISAMPLE_TYPE MultiSample);
const TCHAR *GetUsageString(DWORD Usage);
const TCHAR *GetPoolString(DWORD Pool);
const TCHAR *GetTransformTypeString(D3DTRANSFORMSTATETYPE State);
const TCHAR *GetClearRectString(DWORD Count, CONST D3DRECT * pRects);
const TCHAR *GetClearFlagsString(DWORD Flags);
const TCHAR *GetPrimitiveTypeString(D3DPRIMITIVETYPE PrimitiveType);
const TCHAR *GetTextureFilterTypeString(D3DTEXTUREFILTERTYPE Filter);
const TCHAR *GetFVFString(DWORD FVF);
const TCHAR *GetDeviceModeString(const DEVMODE* pMode);
const TCHAR *GetDisplayModeString(const D3DDISPLAYMODE *pMode);
const TCHAR *GetDisplayModeExString(const D3DDISPLAYMODEEX *pMode);
const TCHAR *GetDisplayModeRotationString(const D3DDISPLAYROTATION *pRotation);
const TCHAR *GetPresentationParametersString(const D3DPRESENT_PARAMETERS *pPresentationParameters);
const TCHAR *GetSwapEffectString(D3DSWAPEFFECT SwapEffect);
const TCHAR *GetPresentFlagString(DWORD Flags);
const TCHAR *GetRenderStateString(D3DRENDERSTATETYPE State);
const TCHAR *GetTextureStageStateTypeString( D3DTEXTURESTAGESTATETYPE Type );
const TCHAR *GetSamplerStateTypeString(D3DSAMPLERSTATETYPE Type);
const TCHAR *GetOffsetString(const SIZE *pOffset);
const TCHAR *GetViewPortString(const D3DVIEWPORT9 *pViewport);
const TCHAR *GetDeviceTypeString(D3DDEVTYPE DeviceType);
const TCHAR *GetBehaviorFlagsString(DWORD BehaviorFlags);
const TCHAR *GetPresentationIntervalString(UINT Present);
const TCHAR *GetPresentFlagsString(UINT Present);
const TCHAR *GetLockedRectString(CONST D3DLOCKED_RECT* pLockedRect);
const TCHAR *GetLockFlagString(DWORD Flags);
const TCHAR *GetCubeMapFaceTypeString(D3DCUBEMAP_FACES FaceType);
const TCHAR* GetPipelineString(PIPELINE Pipeline);
const TCHAR* GetTextureTypeString(TextureType Type);
const TCHAR* GetDirect3D9MethodName(int id);
const TCHAR* GetSwapChain9MethodName(int id);
const TCHAR* GetD3DDevice9MethodName(int id);
const TCHAR *GetFlipIntervalString( DWORD Interval );
const TCHAR *GetPipelineString(PIPELINE Pipeline);
const TCHAR *GetTextureTypeString(TextureType Type);
const TCHAR *GetQueryTypeString(D3DQUERYTYPE Type);

