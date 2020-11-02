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
#include "BaseStereoRenderer_Hook.h"
#include "BaseStereoRenderer.h"
#include "ProxyMulti_Hook.h"

using namespace IDirect3DDevice9MethodNames;

//--- IDirect3DStateBlock9 interception data ---
HRESULT (__stdcall IDirect3DStateBlock9::*Original_StateBlock_Apply)();
#ifndef FINAL_RELEASE
HRESULT (__stdcall IDirect3DStateBlock9::*Original_StateBlock_Capture)();
#endif // FINAL_RELEASE
//--- IDirect3DSwapChain9 interception data ---
HRESULT (__stdcall IDirect3DSwapChain9::*Original_SwapChain_Present)(CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion, DWORD dwFlags);
HRESULT (__stdcall IDirect3DSwapChain9::*Original_SwapChain_GetBackBuffer)(UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface9** ppBackBuffer);
HRESULT (__stdcall IDirect3DSwapChain9::*Original_SwapChain_GetPresentParameters)(D3DPRESENT_PARAMETERS* pPresentationParameters);
//--- IDirect3DVertexBuffer9 interception data ---
VertexBuffer_Lock_t		Original_VertexBuffer_Lock;
VertexBuffer_Unlock_t	Original_VertexBuffer_Unlock;
VertexBuffer_Lock_t		Original_DriverVertexBuffer_Lock;
VertexBuffer_Unlock_t	Original_DriverVertexBuffer_Unlock;

//--- Interfaces AddRef() & Release() interception data ---
ULONG (__stdcall IDirect3DSurface9::*Original_Surface_AddRef)();
ULONG (__stdcall IDirect3DSurface9::*Original_Surface_Release)();
ULONG (__stdcall IDirect3DSurface9::*Original_TexSurf_AddRef)();
ULONG (__stdcall IDirect3DSurface9::*Original_TexSurf_Release)();
ULONG (__stdcall IDirect3DTexture9::*Original_Texture_AddRef)();
ULONG (__stdcall IDirect3DTexture9::*Original_Texture_Release)();
ULONG (__stdcall IDirect3DPixelShader9::*Original_PixelShader_AddRef)();
ULONG (__stdcall IDirect3DPixelShader9::*Original_PixelShader_Release)();
ULONG (__stdcall IDirect3DVertexShader9::*Original_VertexShader_AddRef)();
ULONG (__stdcall IDirect3DVertexShader9::*Original_VertexShader_Release)();

PROXYSTDMETHOD_(ULONG,Surface_AddRef)(IDirect3DSurface9* This)
{	
	ULONG rc = (This->*Original_Surface_AddRef)();
	DEBUG_TRACE3(_T("Surface::AddRef(This = %p [%s]) %d\n"),
		This, GetObjectName(This), rc);
	return rc;
}

PROXYSTDMETHOD_(ULONG,Surface_Release)(IDirect3DSurface9* This)
{	
	//bool b = false;
	//LPCTSTR objectName = GetObjectName(This);
	//if (_tcscmp(_T("StereoBBLeft-SC0"), objectName) == 0)
	//	b = true;
	//static IDirect3DSurface9* p = (IDirect3DSurface9*)-1;
	//if(p == This)
	//	_asm nop
	ULONG rc;
	if(IsMcClaud(This))
	{
		if(Original_Surface_AddRef)
			(This->*Original_Surface_AddRef)();
		else
			This->AddRef();
		rc = (This->*Original_Surface_Release)();
		//if(b)
		//{
		//	DEBUG_TRACE1(_T("\trc = %d. McClaud surface.\n"), rc);
		//}
		if(rc == 1)
		{
			//DEBUG_TRACE1(_T("\tKeep %s surface. Don't destroy now.\n"), GetObjectName(This));
			return 0;
		}
	}
	//--- use multi hook original instead ---
	rc = (This->*Original_Surface_Release)();
	//rc = (This->*Original_Surface_And_SwapChain_Release)();
	//if (b && rc == 0)
	//	__debugbreak();
	//if (b)
	//{
	//	DEBUG_TRACE1(_T("\trc %d\n"), rc);
	//}
	return rc;
}

PROXYSTDMETHOD_(ULONG,TexSurf_AddRef)(IDirect3DSurface9* This)
{	
	ULONG rc = (This->*Original_TexSurf_AddRef)();
	DEBUG_TRACE3(_T("TexSurf::AddRef(This = %p [%s]) %d\n"),
		This, GetObjectName(This), rc);
	return rc;
}

PROXYSTDMETHOD_(ULONG,TexSurf_Release)(IDirect3DSurface9* This)
{	
	DEBUG_TRACE3(_T("TexSurf::Release(This = %p [%s])\n"),
		This, GetObjectName(This));
	ULONG rc = (This->*Original_TexSurf_Release)();
	DEBUG_TRACE3(_T("\trc %d\n"), rc);
	return rc;
}

PROXYSTDMETHOD_(ULONG,Texture_AddRef)(IDirect3DTexture9* This)
{
	ULONG rc = (This->*Original_Texture_AddRef)();
	DEBUG_TRACE3(_T("Texture::AddRef(This = %p [%s]) %d\n"),
		This, GetObjectName(This), rc);
	return rc;
}

PROXYSTDMETHOD_(ULONG,Texture_Release)(IDirect3DTexture9* This)
{
	DEBUG_TRACE3(_T("Texture::Release(This = %p [%s])\n"),
		This, GetObjectName(This));
	ULONG rc = (This->*Original_Texture_Release)();
	DEBUG_TRACE3(_T("\trc %d\n"), rc);
	return rc;
}

PROXYSTDMETHOD_(ULONG,PixelShader_AddRef)(IDirect3DPixelShader9* This)
{
	ULONG rc = (This->*Original_PixelShader_AddRef)();
	DEBUG_TRACE3(_T("PixelShader::AddRef(This = %p) %d\n"),
		This, rc);
	return rc;
}

PROXYSTDMETHOD_(ULONG,PixelShader_Release)(IDirect3DPixelShader9* This)
{
	ULONG rc = (This->*Original_PixelShader_Release)();
	DEBUG_TRACE3(_T("PixelShader::Release(This = %p) %d\n"),
		This, rc);
	return rc;
}

PROXYSTDMETHOD_(ULONG,VertexShader_AddRef)(IDirect3DVertexShader9* This)
{
	ULONG rc = (This->*Original_VertexShader_AddRef)();
	DEBUG_TRACE3(_T("VertexShader::AddRef(This = %p) %d\n"),
		This, rc);
	return rc;
}

PROXYSTDMETHOD_(ULONG,VertexShader_Release)(IDirect3DVertexShader9* This)
{
	ULONG rc = (This->*Original_VertexShader_Release)();
	DEBUG_TRACE3(_T("VertexShader::Release(This = %p) %d\n"),
		This, rc);
	return rc;
}

//////////////////////////////////////////////////////////////////////////

//--- only for D3D9Ex mode ---
void Proxy_Pitch(IDirect3DSurface9* surface, D3DLOCKED_RECT * pLockedRect, CONST RECT * pRect, DWORD Flags)
{
	ProxyPitchData p;
	DWORD dataSize = sizeof(p);
	if(SUCCEEDED(surface->GetPrivateData(PitchCorrectionGUID, &p, &dataSize)))
	{
		DEBUG_TRACE2(_T( __FUNCTION__ ) _T("(Surface = %p [%s], pLockedRect = %s, pRect = %s, Flags = %s)\n"),
			surface, GetObjectName(surface), GetLockedRectString(pLockedRect),
			GetRectString(pRect), GetLockFlagString(Flags));
		D3DSURFACE_DESC desc;
		surface->GetDesc(&desc);
		p.lockedRect = *pLockedRect;
		if(pRect)
			p.rect = *pRect;
		else
		{
			p.rect.left = 0;
			p.rect.top = 0;
			p.rect.right = desc.Width;
			p.rect.bottom = desc.Height;
		}
		UINT lineWidth = p.rect.right - p.rect.left;
		p.lineCount = p.rect.bottom - p.rect.top;
		if(IsCompressedFormat(desc.Format))
		{
			lineWidth = RoundUp(lineWidth, 4) * 4;
			p.lineCount = RoundUp(p.lineCount, 4) / 4;
		}
		p.dataPitch = lineWidth * BitsPerPixel(desc.Format) / 8;
		if (IsBadWritePtr(pLockedRect->pBits, pLockedRect->Pitch * p.lineCount)) {
			DEBUG_MESSAGE(_T("Crazy write pointer(Surface = %p [%s], pBits = %p, Pitch = %d, dataPitch = %d)\n"), 
				surface, GetObjectName(surface), pLockedRect->pBits, pLockedRect->Pitch, p.dataPitch);
			return;
		}
		if (p.dataPitch != pLockedRect->Pitch)
		{
			DEBUG_TRACE2(_T("\tPitch correction code\n"));
			p.pData = DNew char[p.dataPitch * p.lineCount];
			//--- copy data from surface to our memory ---
			if(!(Flags & D3DLOCK_DISCARD))
			{
				for(DWORD j = 0; j< p.lineCount;  j++)
				{
					void* s = (char*)pLockedRect->pBits + j * pLockedRect->Pitch;
					void* d = (char*)p.pData + j * p.dataPitch;
					memcpy(d, s, p.dataPitch);
				}
			}
			pLockedRect->pBits = p.pData;
			pLockedRect->Pitch = p.dataPitch;
		}
		surface->SetPrivateData(PitchCorrectionGUID, &p, sizeof(p), 0);
	}
}

//--- only for D3D9Ex mode ---
void Proxy_UnPitch(IDirect3DSurface9* surface)
{
	ProxyPitchData p;
	DWORD dataSize = sizeof(p);
	if(SUCCEEDED(surface->GetPrivateData(PitchCorrectionGUID, &p, &dataSize)))
	{
		DEBUG_TRACE2(_T("Proxy_UnPitch: %s\n"), GetObjectName(surface));
		if(p.pData)
		{
			for(DWORD j = 0; j< p.lineCount;  j++)
			{
				void* s = (char*)p.pData + j * p.dataPitch;
				void* d = (char*)p.lockedRect.pBits + j * p.lockedRect.Pitch;
				memcpy(d, s, p.dataPitch);
			}
			delete [] p.pData;
			p.pData = NULL;
			//--- for safe double unlock ---
			surface->SetPrivateData(PitchCorrectionGUID, &p, sizeof(p), 0);
		}
	}
}

//////////////////////////////////////////////////////////////////////////

Volume_LockBox_t	Original_Volume_LockBox;
Volume_UnlockBox_t	Original_Volume_UnlockBox;

struct VolumeBox
{
	VolumeBox() {};
	VolumeBox(CONST D3DLOCKED_BOX * pLockedVolume, CONST D3DBOX * pBox) 
	{
		LockedVolume = *pLockedVolume;
		BoxIsNull = !pBox;
		if(pBox)
			Box = *pBox;
	};

	D3DLOCKED_BOX LockedVolume;
	BOOL BoxIsNull;
	D3DBOX Box;
	static const GUID ID;
};

const GUID VolumeBox::ID = { 0xb31801c3, 0x70c2, 0x415c, { 0x94, 0x5f, 0xfa, 0x84, 0x2c, 0xfe, 0xed, 0x72 } };

PROXYSTDMETHOD(Volume_LockBox)(IDirect3DVolume9* This, D3DLOCKED_BOX * pLockedVolume, CONST D3DBOX * pBox, DWORD Flags)
{
	HRESULT hResult;
	if (CalledFromD3D9())
	{
		hResult = (This->*Original_Volume_LockBox)(pLockedVolume, NULL, Flags);
		if (SUCCEEDED(hResult))
		{
			if (pBox) {
				UINT ll = 0;
				if (pBox->Left > 0)
				{
					D3DVOLUME_DESC desc;
					This->GetDesc(&desc);
					ll = BitsPerPixel(desc.Format) * pBox->Left / 8;
				}
				pLockedVolume->pBits = (PBYTE)pLockedVolume->pBits + pLockedVolume->SlicePitch * pBox->Front +
					pLockedVolume->RowPitch * pBox->Top + ll;
			}
			//CBaseStereoRenderer* pCurrentStereoRenderer;
			//GetCurrentStereoRenderer(This, pCurrentStereoRenderer);
			//if(!(Flags & D3DLOCK_READONLY))
			//{
			//	DEBUG_TRACE2(_T( __FUNCTION__ ) _T("(This = %p [%s], pLockedVolume = %p, pBox = %з, Flags = %s)\n"),
			//		This, GetObjectName(This),
			//		pLockedVolume, pBox, GetLockFlagString(Flags));

			//	VolumeBox sr(pLockedVolume, pBox);
			//	This->SetPrivateData(VolumeBox::ID, &sr, sizeof(VolumeBox), 0);
			//}
		}
	}
	else
		hResult = (This->*Original_Volume_LockBox)(pLockedVolume, pBox, Flags);
	return hResult;
}

PROXYSTDMETHOD(Volume_UnlockBox)(IDirect3DVolume9* This)																
{
	/*if (CalledFromD3D9())
	{
		CBaseStereoRenderer* pCurrentStereoRenderer;
		GetCurrentStereoRenderer(This, pCurrentStereoRenderer);
		VolumeBox sr;
		DWORD size = sizeof(VolumeBox);
		if (SUCCEEDED(This->GetPrivateData(VolumeBox::ID, (void *)&sr, &size)))
		{
			DEBUG_TRACE2(_T( __FUNCTION__ ) _T("(This = %p [%s])\n"),	This, GetObjectName(This));
			This->FreePrivateData(VolumeBox::ID);
			//UINT size = sr.LockedVolume.SlicePitch * (sr.Box.Back - sr.Box.Front);
			//PBYTE data = DNew BYTE[size];
			//memcpy(data, sr.LockedVolume.pBits, size);
			HRESULT hResult = (This->*Original_Volume_UnlockBox)();
			// for test
			//hResult = (This->*Original_Volume_LockBox)(&sr.LockedVolume, NULL, D3DLOCK_READONLY);
			//PBYTE src = (PBYTE)sr.LockedVolume.pBits + sr.LockedVolume.SlicePitch * sr.Box.Front;
			////_ASSERT(memcmp(data, src, size) == 0);
			//delete [] data;
			//hResult = (This->*Original_Volume_UnlockBox)();
			return hResult;	
		}
	}*/
	HRESULT hResult = (This->*Original_Volume_UnlockBox)();
	return hResult;	
}

PROXYSTDMETHOD(StateBlock_Apply)(IDirect3DStateBlock9* pBlock)
{
	HRESULT hResult;
	if (CalledFromOnlyD3D9())
	{
		hResult = (pBlock->*Original_StateBlock_Apply)();
		return hResult;
	}
	CBaseStereoRenderer* pCurrentStereoRenderer;
	GetCurrentStereoRenderer(pBlock, &pCurrentStereoRenderer);
	if (pCurrentStereoRenderer)
	{
		if (CalledFromWrapper())
		{
			pCurrentStereoRenderer->m_bInStateBlock = true;
			hResult = (pBlock->*Original_StateBlock_Apply)();
			pCurrentStereoRenderer->m_bInStateBlock = false;
		}
		else
			hResult = pCurrentStereoRenderer->StateBlock_Apply(pBlock);
	}
	else
		hResult = (pBlock->*Original_StateBlock_Apply)();
	return hResult;
}

#ifndef FINAL_RELEASE
PROXYSTDMETHOD(StateBlock_Capture)(IDirect3DStateBlock9* pBlock)
{
	HRESULT hResult;
	if (CalledFromOnlyD3D9())
	{
		hResult = (pBlock->*Original_StateBlock_Apply)();
		return hResult;
	}
	CBaseStereoRenderer* pCurrentStereoRenderer;
	GetCurrentStereoRenderer(pBlock, &pCurrentStereoRenderer);
	if (pCurrentStereoRenderer)
	{
		if (CalledFromWrapper())
		{
			pCurrentStereoRenderer->m_bInStateBlock = true;
			hResult = (pBlock->*Original_StateBlock_Capture)();
			pCurrentStereoRenderer->m_bInStateBlock = false;
		}
		else
			hResult = pCurrentStereoRenderer->StateBlock_Capture(pBlock);
	}
	else
		hResult = (pBlock->*Original_StateBlock_Capture)();
	return hResult;
}
#endif // FINAL_RELEASE

PROXYSTDMETHOD(SetVertexDeclaration)(IDirect3DDevice9* pDevice, IDirect3DVertexDeclaration9* pDecl)
{
	CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
	if (pCurrentStereoRenderer)
	{
		if (!CalledFromWrapper())
			return pCurrentStereoRenderer->SetVertexDeclaration(pDecl);
		else
			return pCurrentStereoRenderer->m_Direct3DDevice.SetVertexDeclaration(pDecl);
	}
	else
	{
		DECLAREPOINTER(pDevice, SetVertexDeclaration);
		HRESULT hResult = CALLFORPOINTER(pDevice, SetVertexDeclaration)(pDecl);
		return hResult;
	}
}

PROXYSTDMETHOD(SetFVF)(IDirect3DDevice9* pDevice, DWORD FVF)
{
	CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
	if (pCurrentStereoRenderer)
	{
		if (!CalledFromWrapper())
			return pCurrentStereoRenderer->SetFVF(FVF);
		else
			return pCurrentStereoRenderer->m_Direct3DDevice.SetFVF(FVF);
	}
	else
	{
		DECLAREPOINTER(pDevice, SetFVF);
		HRESULT hResult = CALLFORPOINTER(pDevice, SetFVF)(FVF);
		return hResult;
	}
}

PROXYSTDMETHOD(SetVertexShader)(IDirect3DDevice9* pDevice, IDirect3DVertexShader9* pShader)
{
	CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
	if (pCurrentStereoRenderer)
	{
		if (!CalledFromWrapper())
			return pCurrentStereoRenderer->SetVertexShader(pShader);
		else
			return pCurrentStereoRenderer->m_Direct3DDevice.SetVertexShader(pShader);
	}
	else
	{
		DECLAREPOINTER(pDevice, SetVertexShader);
		HRESULT hResult = CALLFORPOINTER(pDevice, SetVertexShader)(pShader);
		return hResult;
	}
}

PROXYSTDMETHOD(GetVertexShader)(IDirect3DDevice9* pDevice, IDirect3DVertexShader9** ppShader)
{
	CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
	if (pCurrentStereoRenderer)
	{
		if (!CalledFromWrapper())
			return pCurrentStereoRenderer->GetVertexShader(ppShader);
		else
			return pCurrentStereoRenderer->m_Direct3DDevice.GetVertexShader(ppShader);
	}
	else
	{
		DECLAREPOINTER(pDevice, GetVertexShader);
		HRESULT hResult = CALLFORPOINTER(pDevice, GetVertexShader)(ppShader);
		return hResult;
	}
}

PROXYSTDMETHOD(SetPixelShader)(IDirect3DDevice9* pDevice, IDirect3DPixelShader9* pShader)
{
	CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
	if (pCurrentStereoRenderer)
	{
		if (!CalledFromWrapper())
			return pCurrentStereoRenderer->SetPixelShader(pShader);
		else
			return pCurrentStereoRenderer->m_Direct3DDevice.SetPixelShader(pShader);
	}
	else
	{
		DECLAREPOINTER(pDevice, SetPixelShader);
		HRESULT hResult = CALLFORPOINTER(pDevice, SetPixelShader)(pShader);
		return hResult;
	}
}

PROXYSTDMETHOD(SetStreamSource)(IDirect3DDevice9* pDevice, UINT StreamNumber, IDirect3DVertexBuffer9 * pStreamData, UINT OffsetInBytes, UINT Stride)
{
	CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
	if (pCurrentStereoRenderer)
	{
		if (!CalledFromWrapper())
			return pCurrentStereoRenderer->SetStreamSource(StreamNumber, pStreamData, OffsetInBytes, Stride);
		else
			return pCurrentStereoRenderer->m_Direct3DDevice.SetStreamSource(StreamNumber, pStreamData, OffsetInBytes, Stride);
	}
	else
	{
		DECLAREPOINTER(pDevice, SetStreamSource);
		HRESULT hResult = CALLFORPOINTER(pDevice, SetStreamSource)(StreamNumber, pStreamData, OffsetInBytes, Stride);
		return hResult;
	}
}

PROXYSTDMETHOD(SetTexture)(IDirect3DDevice9* pDevice, DWORD Sampler, IDirect3DBaseTexture9 * pTexture)
{
	CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
	if (pCurrentStereoRenderer)
	{
		if (!CalledFromWrapper())
			return pCurrentStereoRenderer->SetTexture(Sampler, pTexture);
		else
			return pCurrentStereoRenderer->m_Direct3DDevice.SetTexture(Sampler, pTexture);
	}
	else
	{
		DECLAREPOINTER(pDevice, SetTexture);
		HRESULT hResult = CALLFORPOINTER(pDevice, SetTexture)(Sampler, pTexture);
		return hResult;
	}
}

PROXYSTDMETHOD(SetRenderState)(IDirect3DDevice9* pDevice, D3DRENDERSTATETYPE State, DWORD Value)
{
	CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
	if (pCurrentStereoRenderer)
	{
		if (!CalledFromWrapper())
			return pCurrentStereoRenderer->SetRenderState(State, Value);
		else
			return pCurrentStereoRenderer->m_Direct3DDevice.SetRenderState(State, Value);
	}
	else
	{
		DECLAREPOINTER(pDevice, SetRenderState);
		HRESULT hResult = CALLFORPOINTER(pDevice, SetRenderState)(State, Value);
		return hResult;
	}
}

PROXYSTDMETHOD(SetVertexShaderConstantF)(IDirect3DDevice9* pDevice, UINT StartRegister, CONST float * pConstantData, UINT Vector4fCount)
{
	CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
	if (pCurrentStereoRenderer)
	{
		if (!CalledFromWrapper())
			return pCurrentStereoRenderer->SetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);
		else
			return pCurrentStereoRenderer->m_Direct3DDevice.SetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);
	}
	else
	{
		DECLAREPOINTER(pDevice, SetVertexShaderConstantF);
		HRESULT hResult = CALLFORPOINTER(pDevice, SetVertexShaderConstantF)(StartRegister, pConstantData, Vector4fCount);
		return hResult;
	}
}

PROXYSTDMETHOD(GetVertexShaderConstantF)(IDirect3DDevice9* pDevice, UINT StartRegister, float * pConstantData, UINT Vector4fCount)
{
	CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
	if (pCurrentStereoRenderer)
	{
		if (!CalledFromWrapper())
			return pCurrentStereoRenderer->GetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);
		else
			return pCurrentStereoRenderer->m_Direct3DDevice.GetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);
	}
	else
	{
		DECLAREPOINTER(pDevice, GetVertexShaderConstantF);
		HRESULT hResult = CALLFORPOINTER(pDevice, GetVertexShaderConstantF)(StartRegister, pConstantData, Vector4fCount);
		return hResult;
	}
}

PROXYSTDMETHOD(SetTransform)(IDirect3DDevice9* pDevice, D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX * pMatrix)
{
	CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
	if (pCurrentStereoRenderer)
	{
		if (!CalledFromWrapper())
			return pCurrentStereoRenderer->SetTransform(State, pMatrix);
		else
			return pCurrentStereoRenderer->m_Direct3DDevice.SetTransform(State, pMatrix);
	}
	else
	{
		DECLAREPOINTER(pDevice, SetTransform);
		HRESULT hResult = CALLFORPOINTER(pDevice, SetTransform)(State, pMatrix);
		return hResult;
	}
}

PROXYSTDMETHOD(MultiplyTransform)(IDirect3DDevice9* pDevice, D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX * pMatrix)
{
	CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
	if (pCurrentStereoRenderer)
	{
		if (!CalledFromWrapper())
			return pCurrentStereoRenderer->MultiplyTransform(State, pMatrix);
		else
			return pCurrentStereoRenderer->m_Direct3DDevice.MultiplyTransform(State, pMatrix);
	}
	else
	{
		DECLAREPOINTER(pDevice, MultiplyTransform);
		HRESULT hResult = CALLFORPOINTER(pDevice, MultiplyTransform)(State, pMatrix);
		return hResult;
	}
}

PROXYSTDMETHOD(SetViewport)(IDirect3DDevice9* pDevice, CONST D3DVIEWPORT9 * pViewport)
{
	CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
	if (pCurrentStereoRenderer)
	{
		if (!CalledFromWrapper())
			return pCurrentStereoRenderer->SetViewport(pViewport);
		else
			return pCurrentStereoRenderer->m_Direct3DDevice.SetViewport(pViewport);
	}
	else
	{
		DECLAREPOINTER(pDevice, SetViewport);
		HRESULT hResult = CALLFORPOINTER(pDevice, SetViewport)(pViewport);
		return hResult;
	}
}

PROXYSTDMETHOD(SetScissorRect)(IDirect3DDevice9* pDevice, CONST RECT * pRect)
{
	CBaseStereoRenderer* pCurrentStereoRenderer = FindStereoRenderer(pDevice);
	if (pCurrentStereoRenderer)
	{
		if (!CalledFromWrapper())
			return pCurrentStereoRenderer->SetScissorRect(pRect);
		else
			return pCurrentStereoRenderer->m_Direct3DDevice.SetScissorRect(pRect);
	}
	else
	{
		DECLAREPOINTER(pDevice, SetScissorRect);
		HRESULT hResult = CALLFORPOINTER(pDevice, SetScissorRect)(pRect);
		return hResult;
	}
}