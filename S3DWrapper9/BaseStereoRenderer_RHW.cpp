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
#include "BaseStereoRenderer.h"
#include "Trace.h"
#include "Globals.h"
#include "BaseStereoRenderer_Hook.h"

class CLockVBData: 
	public ILockVBData,
	public CComObjectRoot,
	public CComCoClass<CLockVBData>
{
public:
	BEGIN_COM_MAP(CLockVBData)
		COM_INTERFACE_ENTRY(ILockVBData)
	END_COM_MAP()
};

template<typename F>
HRESULT	 Proxy_LockVB(F origFunc, IDirect3DVertexBuffer9* This,UINT OffsetToLock,UINT SizeToLock,VOID ** ppbData,DWORD Flags)
{
	CComPtr<ILockVBData> pLockVBData;
	DWORD size = sizeof(IUnknown *);
	This->GetPrivateData(__uuidof(ILockVBData), (void *)&pLockVBData, &size);
	int states;
	if (pLockVBData)
	{
		states = pLockVBData->GetStates();
		if (states & vbNotUnlocked)
			This->Unlock();
	}
	HRESULT hr = (This->*origFunc)(OffsetToLock, SizeToLock, ppbData, Flags);
	if (pLockVBData)
	{
		if (Flags != D3DLOCK_READONLY)
		{
			DEBUG_TRACE3(_T("VertexBuffer->Lock(This = %p, OffsetToLock = %u, SizeToLock = %u, Flags = %x);\n"), This, OffsetToLock, SizeToLock, Flags);
			LockParams lp;
			lp.OffsetToLock = OffsetToLock;
			lp.SizeToLock = (SizeToLock ? SizeToLock : pLockVBData->GetVBSize());
			lp.pbData = *ppbData;
			pLockVBData->SetLockParams(&lp);
			states = states | vbVerticesChanged;
			if (Flags & D3DLOCK_DISCARD)
				states = states | vbDiscard;
			else
				states = states & ~vbDiscard;
			if (Flags & D3DLOCK_NOOVERWRITE)
				states = states | vbNoOverwrite;
			else
				states = states & ~vbNoOverwrite;
		}
		states = states & ~vbNotUnlocked;
		pLockVBData->SetStates(states);
	}
	return hr;																	
}	

template<typename F>
HRESULT	 Proxy_UnlockVB(F origFunc, IDirect3DVertexBuffer9* This)
{
	CComPtr<ILockVBData> pLockVBData;
	DWORD size = sizeof(IUnknown *);
	This->GetPrivateData(__uuidof(ILockVBData), (void *)&pLockVBData, &size);
	if (pLockVBData)
	{
		int states = pLockVBData->GetStates();
		if (states & vbVerticesChanged)
		{
			DEBUG_TRACE3(_T("VertexBuffer->Unlock(This = %p);\n"), This);
			states = states & ~vbModified;
			BOOL bStereoActive = pLockVBData->GetStereoActive();
			if (bStereoActive)
				states = states | vbNotUnlocked;
			pLockVBData->SetStates(states);
			if (bStereoActive)
				return S_OK;
		}
	}
	return (This->*origFunc)();																
}	

PROXYSTDMETHOD(VertexBuffer_Lock)( IDirect3DVertexBuffer9* This,UINT OffsetToLock,UINT SizeToLock,VOID ** ppbData,DWORD Flags)
{
	if (CalledFromD3D9())
		return (This->*Original_VertexBuffer_Lock)(OffsetToLock, SizeToLock, ppbData, Flags);
	return Proxy_LockVB(Original_VertexBuffer_Lock, This, OffsetToLock, SizeToLock, ppbData, Flags);
}

PROXYSTDMETHOD(VertexBuffer_Unlock)(IDirect3DVertexBuffer9* This)
{
	if (CalledFromD3D9())
		return (This->*Original_VertexBuffer_Unlock)();
	return Proxy_UnlockVB(Original_VertexBuffer_Unlock, This);
}

PROXYSTDMETHOD(DriverVertexBuffer_Lock)( IDirect3DVertexBuffer9* This,UINT OffsetToLock,UINT SizeToLock,VOID ** ppbData,DWORD Flags )
{
	if (CalledFromD3D9())
		return (This->*Original_DriverVertexBuffer_Lock)(OffsetToLock, SizeToLock, ppbData, Flags);
	return Proxy_LockVB(Original_DriverVertexBuffer_Lock, This, OffsetToLock, SizeToLock, ppbData, Flags);
}

PROXYSTDMETHOD(DriverVertexBuffer_Unlock)(IDirect3DVertexBuffer9* This)
{
	if (CalledFromD3D9())
		return (This->*Original_DriverVertexBuffer_Unlock)();
	return Proxy_UnlockVB(Original_DriverVertexBuffer_Unlock, This);
}

HRESULT CBaseStereoRenderer::TransformPrimitiveInit(UINT &StartVertex, UINT MinVertexIndex, UINT Count)
{
	HRESULT hResult = S_OK;
	m_pLockVBData.Release();
	if(!m_bRenderInStereo && !gInfo.WideRenderTarget)
		return hResult;

	//RouterTypeHookCallGuard<ProxyDevice9> RT(&m_Direct3DDevice);  //--- moved in Draw primitive for optimization ---

	m_pSavedVertexBuffer.Release();
	NSCALL(m_Direct3DDevice.GetStreamSource( m_dwStreamRHWVertices, &m_pSavedVertexBuffer, &m_nSavedOffsetInBytes, &m_nSavedStride ));
	if(SUCCEEDED(hResult) && m_pSavedVertexBuffer)
	{
		DWORD size = sizeof(IUnknown *);
		m_pSavedVertexBuffer->GetPrivateData(__uuidof(ILockVBData), (void *)&m_pLockVBData, &size);
		int states;
		m_pModifiedVertexBuffer.Release();
		if (!m_pLockVBData)
		{
			CLockVBData::CreateInstance(&m_pLockVBData);
			m_pLockVBData->Init();
			states = vbVerticesChanged;
			D3DVERTEXBUFFER_DESC vbDesc;
			NSCALL(m_pSavedVertexBuffer->GetDesc(&vbDesc));
			if (vbDesc.Pool == D3DPOOL_DEFAULT)
				states |= vbDynamic;
			UINT size = vbDesc.Size;
			m_pLockVBData->SetVBSize(size);
			DWORD Usage = (states & vbDynamic ? D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC : D3DUSAGE_WRITEONLY);
			NSCALL_TRACE1(m_Direct3DDevice.CreateVertexBuffer( size * 2, Usage, vbDesc.FVF, 
				vbDesc.Pool, &m_pModifiedVertexBuffer, NULL ),
				DEBUG_MESSAGE(_T("RHW: CreateVertexBuffer(Length = %d, Usage = %s (%s), FVF = %s, Pool = %s, ")
				_T("*ppVertexBuffer = %p, pSharedHandle = %p)"),
				size * 2, GetUsageString(Usage), GetUsageString(vbDesc.Usage), GetFVFString(vbDesc.FVF), GetPoolString(vbDesc.Pool), 
				m_pModifiedVertexBuffer, NULL));
			if (FAILED(hResult))
			{
				m_pLockVBData.Release();
				return hResult;
			}
			m_pLockVBData->SetStereoVB(m_pModifiedVertexBuffer);
			m_pLockVBData->SetStereoActiveVariable(&m_Input.StereoActive);
			NSCALL(m_pSavedVertexBuffer->SetPrivateData(__uuidof(ILockVBData), (void *)m_pLockVBData, sizeof(IUnknown *), D3DSPD_IUNKNOWN)); 
		}
		else
		{
			states = m_pLockVBData->GetStates();
			m_pModifiedVertexBuffer = m_pLockVBData->GetStereoVB();
		}
		
		if (states & vbVerticesChanged)
		{
			void* pSavedVertexBufferData;
			UINT nOffsetToLock = m_nSavedOffsetInBytes + (StartVertex + MinVertexIndex) * m_nSavedStride;
			UINT nSizeToLock = Count * m_nSavedStride;
			bool bAllVerticesChanged = false;
			if (states & vbNotUnlocked)
			{
				LockParams* pLP;
				m_pLockVBData->GetLockParams(&pLP);
				if (pLP->OffsetToLock > nOffsetToLock || (pLP->OffsetToLock + pLP->SizeToLock) < (nOffsetToLock + nSizeToLock))
				{
					NSCALL(m_pSavedVertexBuffer->Unlock());
					states = states & ~vbNotUnlocked;
				} else
				{
					if (pLP->OffsetToLock == nOffsetToLock && pLP->SizeToLock == nSizeToLock)
					{
						pSavedVertexBufferData = pLP->pbData;
						bAllVerticesChanged = true;
					}
					else
						pSavedVertexBufferData = (void*)((BYTE*)pLP->pbData + (nOffsetToLock - pLP->OffsetToLock));
				}
			}
			if (!(states & vbNotUnlocked))
				NSCALL(m_pSavedVertexBuffer->Lock( nOffsetToLock, nSizeToLock, &pSavedVertexBufferData, D3DLOCK_READONLY));
			bool bModifyRHW = false;
			if (SUCCEEDED(hResult))
			{
				RHWHEADER *s = (RHWHEADER*)((BYTE*)pSavedVertexBufferData + m_dwOffsetRHWVertices);
				CheckRenderInStereo(s);
				bModifyRHW = m_bRenderInStereo || gInfo.WideRenderTarget;

				if (bModifyRHW)
				{
					if(!DEBUG_SKIP_UPDATE_PROJECTION_MATRIX && m_bRenderInStereo)
						UpdateRHWMatrix();

					DWORD lockFlags = 0; 
					if (states & vbDynamic)	{
						if (states & vbDiscard)
							lockFlags = D3DLOCK_DISCARD;
						else if (states & vbNoOverwrite)
							lockFlags = 0;
					}
					void *pDest;
					UINT VBSize = m_pLockVBData->GetVBSize();
					bool bFullLock = (nOffsetToLock == 0 && nSizeToLock == VBSize);
					if (!bFullLock) {
						NSCALL(m_pModifiedVertexBuffer->Lock( nOffsetToLock, nSizeToLock, &pDest, lockFlags));
					} else {
						NSCALL(m_pModifiedVertexBuffer->Lock( nOffsetToLock, nSizeToLock * 2, &pDest, lockFlags));
					}
					memcpy( pDest, pSavedVertexBufferData, nSizeToLock );
					RHWHEADER *d = (RHWHEADER*)((BYTE*)pDest + m_dwOffsetRHWVertices);
					TransformVertices(d, s, Count, m_nSavedStride);

					if (!bFullLock) {
						NSCALL(m_pModifiedVertexBuffer->Unlock());
						if (states & vbDynamic)	{
							if (states & vbDiscard || states & vbNoOverwrite)
								lockFlags = D3DLOCK_NOOVERWRITE;
						}
						NSCALL(m_pModifiedVertexBuffer->Lock( nOffsetToLock + VBSize, nSizeToLock, &pDest, lockFlags));
					} else {
						pDest = (void*)((BYTE*)pDest + VBSize);
					}

					memcpy( pDest, pSavedVertexBufferData, nSizeToLock );
					if (m_bRenderInStereo)
					{
						VIEWINDEX oldView = m_CurrentView;
						m_CurrentView = VIEWINDEX_LEFT;
						d = (RHWHEADER*)((BYTE*)pDest + m_dwOffsetRHWVertices);
						TransformVertices(d, s, Count, m_nSavedStride);
						m_CurrentView = oldView;
					}

					states = states | vbModified;
					NSCALL(m_pModifiedVertexBuffer->Unlock());
				}
				NSCALL(m_pSavedVertexBuffer->Unlock());
			}
			states = states & ~vbNotUnlocked;
			if (bAllVerticesChanged || !bModifyRHW)
				states = states & ~vbVerticesChanged;
			m_pLockVBData->SetStates(states);
		}
		else if (states & vbNotUnlocked)
		{
			NSCALL(m_pSavedVertexBuffer->Unlock());
			states = states & ~vbNotUnlocked;
			m_pLockVBData->SetStates(states);
		}
		if (states & vbModified) {
			NSCALL(m_Direct3DDevice.SetStreamSource( m_dwStreamRHWVertices, m_pModifiedVertexBuffer, m_nSavedOffsetInBytes, m_nSavedStride ));
		} else {
			m_pLockVBData.Release();
		}
	}
	return hResult;
}

HRESULT CBaseStereoRenderer::TransformPrimitive(UINT &StartVertex)
{
	HRESULT hResult = S_OK;
	if(m_pLockVBData)
	{
		UINT VBSize = m_pLockVBData->GetVBSize();
		if (VBSize % m_nSavedStride == 0) {
			StartVertex += VBSize / m_nSavedStride;
		} else {
			NSCALL(m_Direct3DDevice.SetStreamSource( m_dwStreamRHWVertices, m_pModifiedVertexBuffer, m_nSavedOffsetInBytes + VBSize, m_nSavedStride ));
		}
	}
	return hResult;
}

void CBaseStereoRenderer::RestoreVertices(void)
{
	if (m_pLockVBData) 
		m_Direct3DDevice.SetStreamSource( m_dwStreamRHWVertices, m_pSavedVertexBuffer, m_nSavedOffsetInBytes, m_nSavedStride );
	m_pSavedVertexBuffer = 0;
}

const void* CBaseStereoRenderer::TransformPrimitiveUPInit(UINT VertexCount, const void* pVertexStreamZeroData,
														  UINT VertexStreamZeroStride)
{
	RHWHEADER *s = (RHWHEADER*)((BYTE*)pVertexStreamZeroData  + m_dwOffsetRHWVertices);
	bool bModifyRHW = false;
	if (m_bRenderInStereo)
	{
		CheckRenderInStereo(s);
		bModifyRHW = m_bRenderInStereo != 0 || gInfo.WideRenderTarget != 0;
		if(!DEBUG_SKIP_UPDATE_PROJECTION_MATRIX && m_bRenderInStereo)
			UpdateRHWMatrix();
	} else
		bModifyRHW = gInfo.WideRenderTarget != 0;
	if (bModifyRHW) 
	{
		if (m_ModifiedVertexStreamZeroSize < VertexCount * VertexStreamZeroStride)
		{
			if (m_pModifiedVertexStreamZeroData)
				delete [] m_pModifiedVertexStreamZeroData;
			m_ModifiedVertexStreamZeroSize = VertexCount * VertexStreamZeroStride;
			m_pModifiedVertexStreamZeroData = DNew BYTE[m_ModifiedVertexStreamZeroSize];
		}
		memcpy( &m_pModifiedVertexStreamZeroData[0], pVertexStreamZeroData, VertexCount * VertexStreamZeroStride );
		
		RHWHEADER *d = (RHWHEADER*)((BYTE*)&m_pModifiedVertexStreamZeroData[0] + m_dwOffsetRHWVertices);
		TransformVertices(d, s, VertexCount, VertexStreamZeroStride);
		return &m_pModifiedVertexStreamZeroData[0];
	}
	else
		return pVertexStreamZeroData;
}

const void* CBaseStereoRenderer::TransformPrimitiveUP(UINT VertexCount, const void* pVertexStreamZeroData,
														  UINT VertexStreamZeroStride)
{
	if (m_bRenderInStereo) 
	{
		RHWHEADER *s = (RHWHEADER*)((BYTE*)pVertexStreamZeroData  + m_dwOffsetRHWVertices);
		RHWHEADER *d = (RHWHEADER*)((BYTE*)&m_pModifiedVertexStreamZeroData[0] + m_dwOffsetRHWVertices);
		TransformVertices(d, s, VertexCount, VertexStreamZeroStride);
		return &m_pModifiedVertexStreamZeroData[0];
	}
	else
		return pVertexStreamZeroData;
}

void CBaseStereoRenderer::TransformVertices(RHWHEADER* d, const RHWHEADER* s, UINT nCount, UINT nStride)
{
	if (m_bRenderInStereo) 
	{		
		FLOAT fXOffset, fYOffset;
		GetViewportOffset(fXOffset, fYOffset);				   
		float m31 = m_StereoCamera.GetCamera(m_CurrentView).TransformAfterViewport._31;
		fXOffset += m_StereoCamera.GetCamera(m_CurrentView).TransformAfterViewport._41;
		for(UINT i = 0; i < nCount; i++)
		{
			d->x = s->x + m31 * s->z + fXOffset;
			d->y = s->y + fYOffset;
			d = (RHWHEADER*)((BYTE*)d + nStride);
			s = (RHWHEADER*)((BYTE*)s + nStride);
		}
	}
	else if (gInfo.WideRenderTarget) // only in mono
	{				
		FLOAT fXOffset, fYOffset;
		GetViewportOffset(fXOffset, fYOffset);			
		for(UINT i = 0; i < nCount; i++)
		{
			d->x = s->x + fXOffset;
			d->y = s->y + fYOffset;
			d = (RHWHEADER*)((BYTE*)d + nStride);
			s = (RHWHEADER*)((BYTE*)s + nStride);
		}
	}
}

#define FLOAT2INTCAST(f) (*((int *)(&f)))
#define FLOAT2UINTCAST(f) (*((unsigned int *)(&f)))
#define ONEFLOAT    0x3F800000U

inline bool FloatEq(float f, float v, float p)
{
	return (f > (v - p) && f < (v + p));
}

inline bool FoundedRHW(float rhw)
{
	boost::unordered_set<float>::const_iterator i = g_ProfileData.MonoRHWValues.find(rhw);   
	if ( i != g_ProfileData.MonoRHWValues.end( ) )
	{
		DEBUG_TRACE1(_T("\t\tRHW founded %f\n"), rhw);
		return true;
	}
	else
		return false;
}

void CBaseStereoRenderer::CheckRenderInStereo(RHWHEADER *s)
{
	if (!m_bRenderInStereo)
		return; // mono

	if (gInfo.DrawType != 2)
		return; 

	//if(!gInfo.MonoRHWPipeline && 
	//    FLOAT2UINTCAST(p->rhw) != ONEFLOAT  && FLOAT2UINTCAST(p->rhw) > 0 && 
	//    FLOAT2UINTCAST(p->z) <= ONEFLOAT    && FLOAT2UINTCAST(p->z) > 0)

	// HACK: For performance reason check only first vertex
	switch(gInfo.MonoRHWCheckingMethodIndex)
	{
		case 0:
		default:
			if (!(s->rhw >= m_ViewPort.MinZ && s->rhw < (m_ViewPort.MaxZ - 0.001f) && 
				s->z > 0.f && s->z < (1.f - 0.001f) && !FoundedRHW(s->rhw))) 
				m_bRenderInStereo = false;
			DEBUG_TRACE3(_T("\tCheckRenderInStereo(0);\t%s\tz=%f rhw=%f\n"), GetModeString(), s->z, s->rhw);
			break; 
		case 1: // not used
			if (!(s->z > 0.f && s->z < (1.f - 0.001f) && !FoundedRHW(s->rhw))) 
				m_bRenderInStereo = false;
			DEBUG_TRACE3(_T("\tCheckRenderInStereo(1);\t%s\tz=%f rhw=%f\n"), GetModeString(), s->z, s->rhw);
	}
}

RHWEmulPair* CBaseStereoRenderer::GetRHWEmulData(IDirect3DVertexDeclaration9* pDecl)
{
	RHWEmulMap::iterator it = m_RHWEmulMap.find(pDecl);
	if (it != m_RHWEmulMap.end())
	{
		DEBUG_TRACE3(_T("Use RHW emulation data for VD %p, shader index = %i\n"), pDecl, it->second.RHWEmulShaderIndex);
		return &(*it);
	}

	HRESULT hResult = S_OK;
	static int s_RHWEmulShaderIndex = 0;
	DEBUG_MESSAGE(_T("Generating RHW emulation data for VD %p, shader index = %i\n"), pDecl, s_RHWEmulShaderIndex);
	m_RHWEmulMap.insert(RHWEmulMap::value_type(pDecl, RHWEmulData(s_RHWEmulShaderIndex)));
	it = m_RHWEmulMap.find(pDecl);
	D3DVERTEXELEMENT9 decl[MAXD3DDECLLENGTH];
	UINT numElements;
	HRESULT hr = pDecl->GetDeclaration( decl, &numElements);
	for(UINT i = 0; i < numElements; i++)  
	{
		if(decl[i].Usage == D3DDECLUSAGE_POSITIONT) 
			decl[i].Usage = D3DDECLUSAGE_POSITION;
	}
	RouterTypeHookCallGuard<ProxyDevice9> RTdev(&m_Direct3DDevice);
	NSCALL(m_Direct3DDevice.CreateVertexDeclaration(decl, &it->second.pDecl));
	GenerateRHWEmulVS(decl, &it->second.pVS, s_RHWEmulShaderIndex);
	s_RHWEmulShaderIndex++;
	return &(*it);
}

char* CBaseStereoRenderer::VertexElementToString( const D3DVERTEXELEMENT9* pDeclElem, UINT i, char* p )
{
	switch(pDeclElem->Type)
	{
	case D3DDECLTYPE_FLOAT1:
		strcat(p, "	float1 ");
		break;
	case D3DDECLTYPE_FLOAT2:
	case D3DDECLTYPE_SHORT2:
	case D3DDECLTYPE_SHORT2N:
	case D3DDECLTYPE_FLOAT16_2:
		strcat(p, "	float2 ");
		break;
	case D3DDECLTYPE_FLOAT3:
	case D3DDECLTYPE_UDEC3:
	case D3DDECLTYPE_DEC3N:
		strcat(p, "	float3 ");
		break;
	case D3DDECLTYPE_FLOAT4:
	case D3DDECLTYPE_D3DCOLOR:
	case D3DDECLTYPE_UBYTE4:
	case D3DDECLTYPE_UBYTE4N:
	case D3DDECLTYPE_SHORT4:
	case D3DDECLTYPE_SHORT4N:
	case D3DDECLTYPE_FLOAT16_4:
		strcat(p, "	float4 ");
		break;
	default:
		_ASSERT(FALSE);
		break;
	}

	if(pDeclElem->Usage == D3DDECLUSAGE_POSITION) 
		strcat(p, "	pos : POSITION");
	else
	{
		strcat(p, "	v");
		p += strlen(p);
		itoa(i, p, 10);
		strcat(p, " : ");
		switch(pDeclElem->Usage)
		{
		case D3DDECLUSAGE_BLENDWEIGHT:
			strcat(p, "BLENDWEIGHT");
			break;
		case D3DDECLUSAGE_BLENDINDICES:
			strcat(p, "BLENDINDICES");
			break;
		case D3DDECLUSAGE_NORMAL:
			strcat(p, "NORMAL");
			break;
		case D3DDECLUSAGE_PSIZE:
			strcat(p, "PSIZE");
			break;
		case D3DDECLUSAGE_TEXCOORD:
			strcat(p, "TEXCOORD");
			break;
		case D3DDECLUSAGE_TANGENT:
			strcat(p, "TANGENT");
			break;
		case D3DDECLUSAGE_BINORMAL:
			strcat(p, "BINORMAL");
			break;
		case D3DDECLUSAGE_TESSFACTOR:
			strcat(p, "TESSFACTOR");
			break;
		case D3DDECLUSAGE_POSITIONT:
			strcat(p, "POSITIONT");
			break;
		case D3DDECLUSAGE_COLOR:
			strcat(p, "COLOR");
			break;
		case D3DDECLUSAGE_FOG:
			strcat(p, "FOG");
			break;
		case D3DDECLUSAGE_DEPTH:
			strcat(p, "DEPTH");
			break;
		case D3DDECLUSAGE_SAMPLE:
			strcat(p, "SAMPLE");
			break;
		default:
			_ASSERT(FALSE);
			break;
		}
		p += strlen(p);
		itoa(pDeclElem->UsageIndex, p, 10);
	}
	strcat(p, ";\n");

	p += strlen(p);
	return p;
}

HRESULT CBaseStereoRenderer::GenerateRHWEmulVS(D3DVERTEXELEMENT9* pDecl, IDirect3DVertexShader9** ppVS, int RHWEmulShaderIndex)
{
	//RouterTypeHookCallGuard<ProxyDevice9> RTdev(&m_Direct3DDevice);	//--- optimization.  this call already made in GetRHWEmulData() ---

	HRESULT hResult = S_OK;
#if 0
	const char shaderProlog[] = 
	"float4 xStereo : register(c250);					\n\
	float4x4 m : register(c251);						\n\
														\n\
	struct VS_OUTPUT									\n\
	{\n";
	const char shaderTemplate[] = 
	"};													\n\
														\n\
	VS_OUTPUT main( VS_OUTPUT i )						\n\
	{													\n\
		VS_OUTPUT o;									\n\
		o = i;											\n\
		float oldW = 1 / i.pos.w;						\n\
		float4 v = { i.pos.xyz * oldW, oldW };			\n\
		if (i.pos.w > 0.999f)							\n\
			o.pos.x = mul(v, m).x;						\n\
		else											\n\
			o.pos.x = dot(v, xStereo);					\n\
		o.pos.yzw = mul(v, m).yzw;						\n\
		return o;										\n\
	}";
#else // simplified (may contain errors)
	const char shaderProlog[] = 
	"float4 xStereo : register(c250);					\n\
	float4 m[3] : register(c251);						\n\
														\n\
	struct VS_OUTPUT									\n\
	{\n";
	const char shaderTemplate[] = 
	"};													\n\
														\n\
	VS_OUTPUT main( VS_OUTPUT i )						\n\
	{													\n\
		VS_OUTPUT o;									\n\
		o = i;											\n\
		float oldW = 1 / i.pos.w;						\n\
		float4 v = { i.pos.xyz * oldW, oldW };			\n\
		if (i.pos.w > 0.999f)							\n\
			o.pos.x = v.x * m[0].x + v.w * m[0].w;		\n\
		else											\n\
		{												\n\
			float4 s = xStereo;							\n\
			s.y = 0;									\n\
			o.pos.x = dot(v, s);						\n\
		}												\n\
		o.pos.y = v.y * m[1].y + v.w * m[1].w;			\n\
		o.pos.z = v.z * m[2].z + v.w * m[2].w;			\n\
		o.pos.w = oldW;									\n\
		return o;										\n\
	}";
#endif

	char shaderText[1024];
	strcpy(shaderText, shaderProlog);
	char* p = shaderText + strlen(shaderText);
	UINT i = 0;
	while(pDecl[i].Stream != 0xFF)  
	{
		p = VertexElementToString(&pDecl[i], i, p);
		i++;
	}
	strcpy(p, shaderTemplate);
	const char* vsShaderVersionStr = "vs_1_1";
	if (m_PSPipelineData.m_bShaderDataAvailable)
	{
		DWORD version = m_PSPipelineData.m_ShaderVersion & 0xFFFF;
		if (version >= 0x03ff)
			vsShaderVersionStr = "vs_3_sw";
		else if (version >= 0x0300)
			vsShaderVersionStr = "vs_3_0";
		else if (version >= 0x02ff)
			vsShaderVersionStr = "vs_2_sw";
		else if (version >= 0x0201)
			vsShaderVersionStr = "vs_2_a";
		else if (version >= 0x0200)
			vsShaderVersionStr = "vs_2_0";
	}
	CComPtr<ID3DXBuffer> pBuffer, pError;
	NSCALL(D3DXCompileShader(shaderText, (UINT)strlen(shaderText), NULL, NULL, "main", 
		vsShaderVersionStr, 0, &pBuffer, &pError, NULL));
	if(FAILED(hResult))
	{
		DEBUG_MESSAGE(_T("RHW shader error: %S"), pError->GetBufferPointer());
	}
	else
	{
		NSCALL(m_Direct3DDevice.CreateVertexShader((DWORD*)pBuffer->GetBufferPointer(), ppVS));

#ifndef FINAL_RELEASE
		const char* asmText = NULL;
		CComPtr<ID3DXBuffer> pDisassembledShader;
		if( SUCCEEDED(D3DXDisassembleShader((DWORD*)pBuffer->GetBufferPointer(), 0, 0, &pDisassembledShader)) )
			asmText = (const char*)pDisassembledShader->GetBufferPointer();

		WriteRHWEmulVS(shaderText, asmText, RHWEmulShaderIndex);
#endif
	}

	return hResult;
}

void CBaseStereoRenderer::WriteRHWEmulVS(const char* shaderText, const char* asmText, int RHWEmulShaderIndex)
{
#ifndef FINAL_RELEASE
	FILE * fp;

	TCHAR fileName[ MAX_PATH ];
	_tcscpy(fileName, gData.DumpDirectory);
	PathAppend(fileName, _T("vs.rhw_emul.txt"));
	if (RHWEmulShaderIndex == 0) 
		fp = _tfopen( fileName, _T("w") );
	else 
		fp = _tfopen( fileName, _T("a") );

	if(!shaderText) 
	{
		if ( fp ) 
			fclose( fp );
		return;
	}
	if ( fp ) 
	{
		fprintf( fp, ";*******************************************************************************\n"
			"; Shader #%d\n\n", RHWEmulShaderIndex);

		fprintf( fp, "\n%s\n", shaderText);
		fprintf( fp, "*****\n");
		fprintf( fp, "\n%s\n", asmText);

		fclose( fp );
	}
#endif
}
