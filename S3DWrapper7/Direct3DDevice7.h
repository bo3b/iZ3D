/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "Wrapper.h"
#include "Direct3D7.h"
#include <map>
#include <vector>

struct IDirect3DDevice9;
class CDirectDrawSurface7;
class CDirect3DVertexBuffer7;

class DECLSPEC_UUID("28B8629B-3BD9-4AFE-A8C9-3348FD604891")
	CDirect3DDevice7: 
	public CWrapper,
	public IDirect3DDevice7,
	public IDirect3DDevice3,
	public IDirect3DDevice2,
	public IDirect3DDevice,
	public CComCoClass<CDirect3DDevice7>
{
public:
	friend class CDirect3D7;
	CDirect3DDevice7();
	virtual ~CDirect3DDevice7();
	BEGIN_COM_MAP(CDirect3DDevice7)
		COM_INTERFACE_ENTRY(IWrapper)
		COM_INTERFACE_ENTRY(IDirect3DDevice7)
		COM_INTERFACE_ENTRY(IDirect3DDevice3)
		COM_INTERFACE_ENTRY(IDirect3DDevice2)
		COM_INTERFACE_ENTRY(IDirect3DDevice)
	END_COM_MAP()

public:
	STDMETHODIMP Initialize( CDirect3D7* pDirect3D, REFCLSID rclsid, LPDIRECTDRAWSURFACE7 lpDDS );

	/*** IDirect3DDevice7 methods ***/
	STDMETHODIMP GetCaps(LPD3DDEVICEDESC7 lpD3DDevDesc);
	STDMETHODIMP GetCaps(LPD3DDEVICEDESC lpD3DHWDevDesc, LPD3DDEVICEDESC lpD3DHELDevDesc);
	STDMETHODIMP EnumTextureFormats(LPD3DENUMPIXELFORMATSCALLBACK lpd3dEnumPixelProc, LPVOID lpArg);
	STDMETHODIMP EnumTextureFormats(LPD3DENUMTEXTUREFORMATSCALLBACK lpd3dEnumTextureProc, LPVOID lpArg);
	STDMETHODIMP BeginScene();
	STDMETHODIMP EndScene();
	STDMETHODIMP GetDirect3D(LPDIRECT3D7 *lplpD3D);
	STDMETHODIMP GetDirect3D(LPDIRECT3D3 *lplpD3D);
	STDMETHODIMP GetDirect3D(LPDIRECT3D2 *lplpD3D);
	STDMETHODIMP GetDirect3D(LPDIRECT3D *lplpD3D);
	STDMETHODIMP SetRenderTarget(LPDIRECTDRAWSURFACE7 lpNewRenderTarget, DWORD dwFlags);
	STDMETHODIMP SetRenderTarget(LPDIRECTDRAWSURFACE4 lpNewRenderTarget, DWORD dwFlags);
	STDMETHODIMP SetRenderTarget(LPDIRECTDRAWSURFACE lpNewRenderTarget,DWORD dwFlags);
	STDMETHODIMP GetRenderTarget(LPDIRECTDRAWSURFACE7 *lplpRenderTarget);
	STDMETHODIMP GetRenderTarget(LPDIRECTDRAWSURFACE4 *lplpRenderTarget);
	STDMETHODIMP GetRenderTarget(LPDIRECTDRAWSURFACE *lplpRenderTarget);
	STDMETHODIMP Clear(DWORD dwCount, LPD3DRECT lpRects, DWORD dwFlags, DWORD dwColor, D3DVALUE  dvZ, DWORD dwStencil);
	STDMETHODIMP SetTransform(D3DTRANSFORMSTATETYPE dtstTransformStateType, LPD3DMATRIX lpD3DMatrix);
	STDMETHODIMP GetTransform(D3DTRANSFORMSTATETYPE dtstTransformStateType, LPD3DMATRIX lpD3DMatrix);
	STDMETHODIMP SetViewport(LPD3DVIEWPORT7 lpViewport);
	STDMETHODIMP MultiplyTransform(D3DTRANSFORMSTATETYPE dtstTransformStateType, LPD3DMATRIX lpD3DMatrix);
	STDMETHODIMP GetViewport(LPD3DVIEWPORT7 lpViewport);
	STDMETHODIMP SetMaterial(LPD3DMATERIAL7 lpMaterial);
	STDMETHODIMP GetMaterial(LPD3DMATERIAL7 lpMaterial);
	STDMETHODIMP SetLight(DWORD dwLightIndex, LPD3DLIGHT7 lpLight);
	STDMETHODIMP GetLight(DWORD dwLightIndex, LPD3DLIGHT7 lpLight);
	STDMETHODIMP SetRenderState(D3DRENDERSTATETYPE dwRenderStateType, DWORD dwRenderState);
	STDMETHODIMP GetRenderState(D3DRENDERSTATETYPE dwRenderStateType, LPDWORD lpdwRenderState);
	STDMETHODIMP BeginStateBlock();
	STDMETHODIMP EndStateBlock(LPDWORD lpdwBlockHandle);
	STDMETHODIMP PreLoad(LPDIRECTDRAWSURFACE7 lpddsTexture);
	STDMETHODIMP DrawPrimitive(D3DPRIMITIVETYPE dptPrimitiveType, DWORD  dwVertexTypeDesc, 
		LPVOID lpvVertices, DWORD dwVertexCount, DWORD dwFlags);
	STDMETHODIMP DrawPrimitive(D3DPRIMITIVETYPE dptPrimitiveType, D3DVERTEXTYPE dvtVertexType,
		PVOID lpvVertices, DWORD dwVertexCount, DWORD dwFlags);
	STDMETHODIMP DrawIndexedPrimitive(D3DPRIMITIVETYPE d3dptPrimitiveType, DWORD  dwVertexTypeDesc, 
		LPVOID lpvVertices, DWORD dwVertexCount, LPWORD lpwIndices, DWORD dwIndexCount, DWORD dwFlags);
	STDMETHODIMP DrawIndexedPrimitive(D3DPRIMITIVETYPE dptPrimitiveType, D3DVERTEXTYPE dvtVertexType,
		LPVOID lpvVertices, DWORD dwVertexCount, LPWORD lpwIndices, DWORD dwIndexCount, DWORD dwFlags);
	STDMETHODIMP SetClipStatus(LPD3DCLIPSTATUS lpD3DClipStatus);
	STDMETHODIMP GetClipStatus(LPD3DCLIPSTATUS lpD3DClipStatus);
	STDMETHODIMP DrawPrimitiveStrided(D3DPRIMITIVETYPE dptPrimitiveType, DWORD  dwVertexTypeDesc, 
		LPD3DDRAWPRIMITIVESTRIDEDDATA lpVertexArray, DWORD  dwVertexCount, DWORD  dwFlags);
	STDMETHODIMP DrawIndexedPrimitiveStrided(D3DPRIMITIVETYPE d3dptPrimitiveType, DWORD  dwVertexTypeDesc,
		LPD3DDRAWPRIMITIVESTRIDEDDATA lpVertexArray, DWORD  dwVertexCount, LPWORD lpwIndices, 
		DWORD  dwIndexCount,  DWORD  dwFlags);
	STDMETHODIMP DrawPrimitiveVB(D3DPRIMITIVETYPE d3dptPrimitiveType, LPDIRECT3DVERTEXBUFFER7 lpd3dVertexBuffer,
		DWORD dwStartVertex, DWORD dwNumVertices, DWORD dwFlags);
	STDMETHODIMP DrawPrimitiveVB(D3DPRIMITIVETYPE d3dptPrimitiveType, LPDIRECT3DVERTEXBUFFER lpd3dVertexBuffer,
		DWORD dwStartVertex, DWORD dwNumVertices, DWORD dwFlags);
	STDMETHODIMP DrawIndexedPrimitiveVB(D3DPRIMITIVETYPE d3dptPrimitiveType, LPDIRECT3DVERTEXBUFFER7 lpd3dVertexBuffer,
		DWORD  dwStartVertex, DWORD  dwNumVertices, LPWORD lpwIndices, DWORD  dwIndexCount, DWORD  dwFlags );
	STDMETHODIMP DrawIndexedPrimitiveVB(D3DPRIMITIVETYPE d3dptPrimitiveType, LPDIRECT3DVERTEXBUFFER lpd3dVertexBuffer,
		LPWORD lpwIndices, DWORD dwIndexCount, DWORD dwFlags);
	STDMETHODIMP ComputeSphereVisibility(LPD3DVECTOR lpCenters, LPD3DVALUE  lpRadii, DWORD dwNumSpheres,
		DWORD dwFlags, LPDWORD lpdwReturnValues);
	STDMETHODIMP GetTexture(DWORD dwStage, LPDIRECTDRAWSURFACE7 * lplpTexture);
	STDMETHODIMP GetTexture(DWORD dwStage,LPDIRECT3DTEXTURE2 * lplpTexture);
	STDMETHODIMP SetTexture(DWORD dwStage, LPDIRECTDRAWSURFACE7 lpTexture);
	STDMETHODIMP SetTexture(DWORD dwStage,LPDIRECT3DTEXTURE2 lpTexture);
	STDMETHODIMP GetTextureStageState(DWORD dwStage, D3DTEXTURESTAGESTATETYPE dwState, LPDWORD lpdwValue);
	STDMETHODIMP SetTextureStageState(DWORD dwStage, D3DTEXTURESTAGESTATETYPE dwState, DWORD dwValue);
	STDMETHODIMP ValidateDevice(LPDWORD lpdwPasses);
	STDMETHODIMP ApplyStateBlock(DWORD dwBlockHandle);
	STDMETHODIMP CaptureStateBlock(DWORD dwBlockHandle);
	STDMETHODIMP DeleteStateBlock(DWORD dwBlockHandle);
	STDMETHODIMP CreateStateBlock(D3DSTATEBLOCKTYPE d3dsbType, LPDWORD lpdwBlockHandle);
	STDMETHODIMP Load(LPDIRECTDRAWSURFACE7 lpDestTex, LPPOINT lpDestPoint, LPDIRECTDRAWSURFACE7 lpSrcTex, 
		LPRECT lprcSrcRect, DWORD dwFlags);
	STDMETHODIMP LightEnable(DWORD dwLightIndex, BOOL bEnable);
	STDMETHODIMP GetLightEnable(DWORD dwLightIndex, BOOL* pbEnable);
	STDMETHODIMP SetClipPlane(DWORD dwIndex, D3DVALUE* pPlaneEquation);
	STDMETHODIMP GetClipPlane(DWORD dwIndex, D3DVALUE* pPlaneEquation);
	STDMETHODIMP GetInfo(DWORD  dwDevInfoID, LPVOID pDevInfoStruct, DWORD  dwSize);

	/*** IDirect3DDevice1-3 methods ***/
	STDMETHODIMP SwapTextureHandles(LPDIRECT3DTEXTURE2 lpD3DTex1, LPDIRECT3DTEXTURE2 lpD3DTex2);
	STDMETHODIMP SwapTextureHandles(LPDIRECT3DTEXTURE lpD3DTex1, LPDIRECT3DTEXTURE lpD3DTex2);
	STDMETHODIMP CreateExecuteBuffer(LPD3DEXECUTEBUFFERDESC lpDesc, LPDIRECT3DEXECUTEBUFFER *lplpDirect3DExecuteBuffer,
		IUnknown *pUnkOuter);
	STDMETHODIMP GetStats(LPD3DSTATS lpD3DStats);
	STDMETHODIMP Execute(LPDIRECT3DEXECUTEBUFFER lpDirect3DExecuteBuffer, LPDIRECT3DVIEWPORT lpDirect3DViewport, DWORD dwFlags);
	STDMETHODIMP AddViewport(LPDIRECT3DVIEWPORT3 lpDirect3DViewport);
	STDMETHODIMP AddViewport(LPDIRECT3DVIEWPORT2 lpDirect3DViewport);
	STDMETHODIMP AddViewport(LPDIRECT3DVIEWPORT lpDirect3DViewport);
	STDMETHODIMP DeleteViewport(LPDIRECT3DVIEWPORT3 lpDirect3DViewport);
	STDMETHODIMP DeleteViewport(LPDIRECT3DVIEWPORT2 lpDirect3DViewport);
	STDMETHODIMP DeleteViewport(LPDIRECT3DVIEWPORT lpDirect3DViewport);
	STDMETHODIMP NextViewport(LPDIRECT3DVIEWPORT3 lpDirect3DViewport, LPDIRECT3DVIEWPORT3 *lplpAnotherViewport, DWORD dwFlags);
	STDMETHODIMP NextViewport(LPDIRECT3DVIEWPORT2 lpDirect3DViewport, LPDIRECT3DVIEWPORT2 *lplpAnotherViewport, DWORD dwFlags);
	STDMETHODIMP NextViewport(LPDIRECT3DVIEWPORT lpDirect3DViewport, LPDIRECT3DVIEWPORT *lplpAnotherViewport, DWORD dwFlags);
	STDMETHODIMP Pick(LPDIRECT3DEXECUTEBUFFER lpDirect3DExecuteBuffer, LPDIRECT3DVIEWPORT lpDirect3DViewport,
		DWORD dwFlags, LPD3DRECT lpRect);
	STDMETHODIMP GetPickRecords(LPDWORD lpCount, LPD3DPICKRECORD lpD3DPickRec);
	STDMETHODIMP CreateMatrix(LPD3DMATRIXHANDLE lpD3DMatHandle);
	STDMETHODIMP SetMatrix(D3DMATRIXHANDLE d3dMatHandle, const LPD3DMATRIX lpD3DMatrix);
	STDMETHODIMP GetMatrix(D3DMATRIXHANDLE D3DMatHandle, LPD3DMATRIX lpD3DMatrix);
	STDMETHODIMP DeleteMatrix(D3DMATRIXHANDLE d3dMatHandle);
	STDMETHODIMP SetCurrentViewport(LPDIRECT3DVIEWPORT3 lpd3dViewport);
	STDMETHODIMP SetCurrentViewport(LPDIRECT3DVIEWPORT2 lpd3dViewport);
	STDMETHODIMP GetCurrentViewport(LPDIRECT3DVIEWPORT3 *lplpd3dViewport);
	STDMETHODIMP GetCurrentViewport(LPDIRECT3DVIEWPORT2 *lplpd3dViewport);
	STDMETHODIMP Begin(D3DPRIMITIVETYPE d3dpt, DWORD dwVertexTypeDesc, DWORD dwFlags);
	STDMETHODIMP Begin(D3DPRIMITIVETYPE d3dpt, D3DVERTEXTYPE d3dvt, DWORD dwFlags);
	STDMETHODIMP BeginIndexed(D3DPRIMITIVETYPE dptPrimitiveType, DWORD  dwVertexTypeDesc,
		LPVOID lpvVertices, DWORD  dwNumVertices, DWORD  dwFlags);
	STDMETHODIMP BeginIndexed(D3DPRIMITIVETYPE dptPrimitiveType, D3DVERTEXTYPE dvtVertexType,
		LPVOID lpvVertices, DWORD dwNumVertices, DWORD dwFlags);
	STDMETHODIMP Vertex(LPVOID lpVertex);
	STDMETHODIMP Index(WORD wVertexIndex);
	STDMETHODIMP End(DWORD dwFlags);
	STDMETHODIMP GetLightState(D3DLIGHTSTATETYPE dwLightStateType, LPDWORD lpdwLightState);
	STDMETHODIMP SetLightState(D3DLIGHTSTATETYPE dwLightStateType, DWORD dwLightState);

	STDMETHODIMP Initialize(LPDIRECT3D lpd3d, LPGUID lpGUID, LPD3DDEVICEDESC lpd3ddvdesc);

	void SetFVF(DWORD Fvf);
	void UpdatedIndexBuffer( LPWORD lpwIndices, DWORD dwIndexCount );
private:
	CDirect3D7* m_pDirect3D7;
	IDirect3DDevice9* m_pReal;
	std::vector<DWORD> m_SBHandle;
	DWORD m_Fvf;
	CDirect3DVertexBuffer7* m_VB;
	DWORD m_IndexCount;
	CComPtr<IDirect3DIndexBuffer9> m_pIB9;
	DWORD m_VertexSize;
	
	CDirectDrawSurface7* m_pRenderTarget;
	CDirectDrawSurface7* m_pTextures[8];

	CComPtr<IDirect3DDevice7>	m_pD3DDevice7;	// D3D7
	CComPtr<IDirect3DDevice3>	m_pD3DDevice3;	// D3D6
	CComPtr<IDirect3DDevice2>	m_pD3DDevice2;	// D3D5
	CComPtr<IDirect3DDevice>	m_pD3DDevice1;	// D3D2
};
