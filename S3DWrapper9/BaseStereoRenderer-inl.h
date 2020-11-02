#pragma once

#include "CommandDumper.h"

#ifndef OUTPUT_LIBRARY

template <VIEWINDEX view>
void CBaseStereoRenderer::SetStereoTextures()
{
	INSIDE;
	HRESULT hResult = S_OK;
	DWORD PSStereoTexturesMask = m_PSPipelineData.m_StereoTexturesMask & m_PSPipelineData.m_CurrentShaderData.textures;
	if (PSStereoTexturesMask)
	{
		m_bUsedStereoTextures = true;
		DWORD CurrentMask = 1;
		for (DWORD stage = 0; stage < m_PSPipelineData.m_MaxTextures; stage++) 
		{
			if (PSStereoTexturesMask & CurrentMask)
			{
				NSCALL(m_Direct3DDevice.SetTexture(stage, m_PSPipelineData.m_Texture[stage].GetViewTex<view>()));
			}
			CurrentMask <<= 1;
		}
	}
	DWORD VSStereoTexturesMask = m_VSPipelineData.m_StereoTexturesMask & m_VSPipelineData.m_CurrentShaderData.textures;
	if (VSStereoTexturesMask)
	{
		m_bUsedStereoTextures = true;
		DWORD CurrentMask = 1;
		for (DWORD stage = 0; stage < m_VSPipelineData.m_MaxTextures; stage++) 
		{
			if (VSStereoTexturesMask & CurrentMask)
			{
				NSCALL(m_Direct3DDevice.SetTexture(D3DDMAPSAMPLER + stage, m_VSPipelineData.m_Texture[stage].GetViewTex<view>()));
			}
			CurrentMask <<= 1;
		}
	}
}

template <VIEWINDEX view>
HRESULT CBaseStereoRenderer::ModifyVSMatricesWVP()
{
	HRESULT hResult = S_OK;
	float m31 = m_CurrentMeshMultiplier * m_StereoCamera.GetCamera(view).TransformBeforeViewport._31;
	if (m31 != 0.0f && m_CurrentConvergenceShift != 0.0f)
	{
		float One_div_ZPS = m_Input.GetActivePreset()->One_div_ZPS;
		m31 = m31 / One_div_ZPS * (One_div_ZPS + m_CurrentConvergenceShift);
	}	
	float m41 = m_CurrentMeshMultiplier * m_StereoCamera.GetCamera(view).TransformBeforeViewport._41;
	if (view == VIEWINDEX_RIGHT && IsRightViewUnmodified())
		return hResult; // skip constant modification

	auto& data = m_VSPipelineData.m_CurrentShaderData.matricesData;
	for (DWORD i = 0; i < data.matrixSize; i++)
	{
		D3DXMATRIX &curShaderMatrix = (D3DXMATRIX&)*GetVSMatrix(i);
		if(data.matrix[i].matrixIsTransposed)
		{
			float stereoMatrix[4];
			stereoMatrix[0] = curShaderMatrix._11 + m31*curShaderMatrix._31 + m41*curShaderMatrix._41;
			stereoMatrix[1] = curShaderMatrix._12 + m31*curShaderMatrix._32 + m41*curShaderMatrix._42;
			stereoMatrix[2] = curShaderMatrix._13 + m31*curShaderMatrix._33 + m41*curShaderMatrix._43;
			stereoMatrix[3] = curShaderMatrix._14 + m31*curShaderMatrix._34 + m41*curShaderMatrix._44;
			hResult = m_Direct3DDevice.SetVertexShaderConstantF(data.matrix[i].matrixRegister, stereoMatrix, 1);

			DEBUG_TRACE3(_T("%s VS Modified matrix (transposed):\n"), GetViewString(view));
			DEBUG_TRACE3(
				_T("%12.7f %12.7f %12.7f %12.7f\n")
				_T("%12.7f %12.7f %12.7f %12.7f\n")
				_T("%12.7f %12.7f %12.7f %12.7f\n")
				_T("%12.7f %12.7f %12.7f %12.7f\n"),
				stereoMatrix[0], curShaderMatrix._21, curShaderMatrix._31, curShaderMatrix._41,
				stereoMatrix[1], curShaderMatrix._22, curShaderMatrix._32, curShaderMatrix._42,
				stereoMatrix[2], curShaderMatrix._23, curShaderMatrix._33, curShaderMatrix._43,
				stereoMatrix[3], curShaderMatrix._24, curShaderMatrix._34, curShaderMatrix._44 );
		}
		else
		{
			D3DXMATRIX stereoMatrix = curShaderMatrix;
			if (gInfo.GameSpecific == 1 && abs(curShaderMatrix._23) < 0.00001f ) 
			{
				DEBUG_TRACE3(_T("Mirror's Edge magic\n"));
				stereoMatrix._13 = stereoMatrix._14;
				stereoMatrix._23 = stereoMatrix._24;
				stereoMatrix._33 = stereoMatrix._34;
				stereoMatrix._43 = stereoMatrix._44 - 10.0f;
			}
			stereoMatrix._11 += m31*stereoMatrix._13 + m41*stereoMatrix._14;
			stereoMatrix._21 += m31*stereoMatrix._23 + m41*stereoMatrix._24;
			stereoMatrix._31 += m31*stereoMatrix._33 + m41*stereoMatrix._34;
			stereoMatrix._41 += m31*stereoMatrix._43 + m41*stereoMatrix._44;
			hResult = m_Direct3DDevice.SetVertexShaderConstantF(data.matrix[i].matrixRegister, stereoMatrix, 4);

			DEBUG_TRACE3(_T("%s VS Modified matrix (non transposed):\n"), GetViewString(view));
			TraceMatrix(&stereoMatrix);
		}
	}
	return hResult;
}

template <VIEWINDEX view>
HRESULT CBaseStereoRenderer::ModifyVSMatricesP()
{
	HRESULT hResult = S_OK;
	float m31 = m_CurrentMeshMultiplier * m_StereoCamera.GetCamera(view).A;
	if (m31 != 0.0f && m_CurrentConvergenceShift != 0.0f)
	{
		float One_div_ZPS = m_Input.GetActivePreset()->One_div_ZPS;
		m31 = m31 / One_div_ZPS * (One_div_ZPS + m_CurrentConvergenceShift);
	}
	float m41 = m_CurrentMeshMultiplier * m_StereoCamera.GetCamera(view).B;
	if (view == VIEWINDEX_RIGHT && IsRightViewUnmodified())
		return hResult; // skip constant modification

	auto& data = m_VSPipelineData.m_CurrentShaderData.matricesData;
	for (DWORD i = 0; i < data.matrixSize; i++)
	{
		D3DXMATRIX &curShaderMatrix = (D3DXMATRIX&)*GetVSMatrix(i);
		if(data.matrix[i].matrixIsTransposed)
		{
			D3DXMATRIX stereoMatrix = curShaderMatrix;
			stereoMatrix._13 += m31 * stereoMatrix._11;
			stereoMatrix._23 += m31 * stereoMatrix._21;
			stereoMatrix._33 += m31 * stereoMatrix._31;
			stereoMatrix._43 += m31 * stereoMatrix._41;
			stereoMatrix._14 += m41 * stereoMatrix._11;
			stereoMatrix._24 += m41 * stereoMatrix._21;
			stereoMatrix._34 += m41 * stereoMatrix._31;
			stereoMatrix._44 += m41 * stereoMatrix._41;
			hResult = m_Direct3DDevice.SetVertexShaderConstantF(data.matrix[i].matrixRegister, stereoMatrix, 4);

			DEBUG_TRACE3(_T("%s VS Modified matrix (transposed):\n"), GetViewString(view));
			TraceMatrix(&stereoMatrix, true);
		}
		else
		{
			D3DXMATRIX stereoMatrix = curShaderMatrix;
			stereoMatrix._31 += m31 * stereoMatrix._11;
			stereoMatrix._32 += m31 * stereoMatrix._12;
			stereoMatrix._33 += m31 * stereoMatrix._13;
			stereoMatrix._34 += m31 * stereoMatrix._14;
			stereoMatrix._41 += m41 * stereoMatrix._11;
			stereoMatrix._42 += m41 * stereoMatrix._12;
			stereoMatrix._43 += m41 * stereoMatrix._13;
			stereoMatrix._44 += m41 * stereoMatrix._14;
			hResult = m_Direct3DDevice.SetVertexShaderConstantF(data.matrix[i].matrixRegister, stereoMatrix, 4);

			DEBUG_TRACE3(_T("%s VS Modified matrix (non transposed):\n"), GetViewString(view));
			TraceMatrix(&stereoMatrix);
		}
	}
	return hResult;
}

template <VIEWINDEX view>
HRESULT CBaseStereoRenderer::SetModifiedShaderConstants()
{
	HRESULT hResult = S_OK;
	if (gInfo.VertexShaderModificationMethod < 2)
	{
		float m31 = m_CurrentMeshMultiplier * m_StereoCamera.GetCamera(view).TransformBeforeViewport._31;
		if (m31 != 0.0f && m_CurrentConvergenceShift != 0.0f)
		{
			float One_div_ZPS = m_Input.GetActivePreset()->One_div_ZPS;
			m31 = m31 / One_div_ZPS * (One_div_ZPS + m_CurrentConvergenceShift);
		}
		float m41 = m_CurrentMeshMultiplier * m_StereoCamera.GetCamera(view).TransformBeforeViewport._41;
		FLOAT shiftVector[4] = {1, 0, m31, m41};
		hResult = m_Direct3DDevice.SetVertexShaderConstantF(m_VSPipelineData.m_CurrentShaderData.dp4VectorRegister, shiftVector, 1);
		if (m_VSPipelineData.m_CurrentShaderData.ZNearRegister != 65535)
		{
			FLOAT ZNearVector[4] = {0.0f, 0.0f, 0.0f, m_StereoZNear};
			hResult = m_Direct3DDevice.SetVertexShaderConstantF(m_VSPipelineData.m_CurrentShaderData.ZNearRegister, ZNearVector , 1);
		}
	}
	else
	{
		float conv = -(m_Input.GetActivePreset()->One_div_ZPS + m_CurrentConvergenceShift);
		float sep = m_Input.GetActivePreset()->StereoBase * m_CurrentMeshMultiplier;
		if (view == VIEWINDEX_LEFT)
			sep *= -1;
		FLOAT vector[4] = {sep, conv * conv, 0.0f, m_StereoZNear};
		hResult = m_Direct3DDevice.SetVertexShaderConstantF(m_VSPipelineData.m_CurrentShaderData.dp4VectorRegister, vector, 1);
	}
	return hResult;
}

template <VIEWINDEX view>
void CBaseStereoRenderer::SetViewStages() 
{
	HRESULT hResult = S_OK;

	m_CurrentView = view;

	if (!DEBUG_SKIP_SET_STEREO_RENDERTARGET)
		SetStereoRenderTarget<view>();

	if (GINFO_CLEARBEFOREEVERYDRAW)
	{
		HRESULT hResult = S_OK;
		//if (m_DepthStencil.pLeftSurf)
		//{
		//	hResult = m_pDirect3DDevice.Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0x00000000, 1.0f, 0);
		//	if (FAILED(hResult))
		//		hResult = m_pDirect3DDevice.Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
		//} else
		hResult = m_Direct3DDevice.Clear(0, NULL, D3DCLEAR_TARGET, 0x00000000, 1.0f, 0);	
	}

	if (!DEBUG_SKIP_SET_STEREO_TEXTURES)
		SetStereoTextures<view>();

	if (m_bRenderInStereo)
	{
		if (m_Pipeline == Shader) 
		{
			if(m_VSPipelineData.m_CurrentShaderData.stereoShader == NULL)
			{
				if (gInfo.CheckOnlyProjectionMatrix && m_bOnlyProjectionMatrix)
					hResult = ModifyVSMatricesP<view>();
				else
					hResult = ModifyVSMatricesWVP<view>();
			}
			else
				hResult = SetModifiedShaderConstants<view>();
		}
		else if (m_Pipeline == Fixed && !(view == VIEWINDEX_RIGHT && gInfo.SeparationMode == 2))
		{
			//--- multiply non-transposed matrix ---
			if (m_CurrentMeshMultiplier == 1.0f && m_CurrentConvergenceShift == 0.0f)
			{
				hResult = m_Direct3DDevice.SetTransform( D3DTS_PROJECTION, &m_StereoCamera.GetCamera(view).ProjectionMatrix );
			}
			else
			{
				D3DXMATRIX stereoMatrix = m_StereoCamera.GetCamera(view).ProjectionMatrix;
				stereoMatrix._31 *= m_CurrentMeshMultiplier;
				if (stereoMatrix._31 != 0.0f && m_CurrentConvergenceShift != 0.0f)
				{
					float One_div_ZPS = m_Input.GetActivePreset()->One_div_ZPS;
					stereoMatrix._31 = stereoMatrix._31 / One_div_ZPS * (One_div_ZPS + m_CurrentConvergenceShift);
				}
				stereoMatrix._41 *= m_CurrentMeshMultiplier;
				hResult = m_Direct3DDevice.SetTransform( D3DTS_PROJECTION, &stereoMatrix );
			}
		}
		else if (m_Pipeline == RHWEmul) 
		{		
			const float* pMatrix = &m_StereoCamera.GetCamera(view).TransformAfterViewportVS._11;
			hResult = m_Direct3DDevice.SetVertexShaderConstantF(250, pMatrix, 1);
			const float* pMonoMatrix = &m_TransformAfterViewportVSMono._11;
			//hResult = m_pDirect3DDevice->SetVertexShaderConstantF(251, pMonoMatrix, 4);
			hResult = m_Direct3DDevice.SetVertexShaderConstantF(251, pMonoMatrix, 3);
		}
	}

	if(m_bRenderInStereoPS)
	{
		float m31 = m_CurrentPSMultiplier * m_StereoCamera.GetCamera(view).TransformBeforeViewport._31;
		float m41 = m_CurrentPSMultiplier * m_StereoCamera.GetCamera(view).TransformBeforeViewport._41;
		auto& data = m_PSPipelineData.m_CurrentShaderData.matricesData;
		for (DWORD i = 0; i < 1 /*data.matrixSize*/; i++)
		{
			D3DXMATRIX stereoMatrix = m_PSMatrix;
			if(data.matrix[i].matrixIsTransposed)
			{
				stereoMatrix._13 -= m31*m_PSMatrix._11;
				stereoMatrix._23 -= m31*m_PSMatrix._21;
				stereoMatrix._33 -= m31*m_PSMatrix._31;
				stereoMatrix._43 -= m31*m_PSMatrix._41;
				stereoMatrix._14 -= m41*m_PSMatrix._11;
				stereoMatrix._24 -= m41*m_PSMatrix._21;
				stereoMatrix._34 -= m41*m_PSMatrix._31;
				stereoMatrix._44 -= m41*m_PSMatrix._41;
				DEBUG_TRACE3(_T("%s PS modified matrix (transposed):\n"), GetViewString(view));
				TraceMatrix(&stereoMatrix, true);
			}
			else
			{
				stereoMatrix._31 -= m31*m_PSMatrix._11;
				stereoMatrix._32 -= m31*m_PSMatrix._12;
				stereoMatrix._33 -= m31*m_PSMatrix._13;
				stereoMatrix._34 -= m31*m_PSMatrix._14;
				stereoMatrix._41 -= m41*m_PSMatrix._11;
				stereoMatrix._42 -= m41*m_PSMatrix._12;
				stereoMatrix._43 -= m41*m_PSMatrix._13;
				stereoMatrix._44 -= m41*m_PSMatrix._14;
				DEBUG_TRACE3(_T("%s PS modified matrix (non transposed):\n"), GetViewString(view));
				TraceMatrix(&stereoMatrix, false);
			}
			hResult = m_Direct3DDevice.SetPixelShaderConstantF(data.matrix[i].matrixRegister, stereoMatrix, 4);
		}
	}
}

template <typename T, typename Q>
void CBaseStereoRenderer::DumpPipelineTextures( ShaderPipelineData<T, Q>& pipelineData, BOOL bRenderTo2RT, const TCHAR* prefix  )
{
	for(UINT Stage = 0; Stage < pipelineData.m_MaxTextures; Stage++)
	{
		TexData& texData = pipelineData.m_Texture[Stage];
		if(texData.m_pLeft && pipelineData.IsTextureUsed(Stage))
		{
			DWORD samplerIndex = pipelineData.TexToSamplerIndex(Stage);
			const ResourceCRCMultiplier* pCRCMultiplier = pipelineData.m_TextureCRCMultiplier[Stage];
			DumpPipelineTexture(texData, bRenderTo2RT, prefix, samplerIndex, pCRCMultiplier);
		}
	}
}

template <typename T, typename Q>
HRESULT CBaseStereoRenderer::CRCTextureCheck( ShaderPipelineData<T, Q>& pipelineData, DWORD Sampler, IDirect3DBaseTexture9 * pBaseTexture )
{
	HRESULT hResult = S_OK;
	bool bCalculateForAll = 
#ifdef ZLOG_ALLOW_TRACING
		(zlog::GetSeverity() == zlog::SV_FLOOD);
#else
		false;
#endif
	if(pBaseTexture && !DEBUG_SKIP_CRC_CHECK && (!g_ProfileData.TextureCRCMultiplier.empty() || bCalculateForAll))
	{
		const ResourceCRCMultiplier *pCRCMultiplier = GetCRCMultiplier(pBaseTexture);
		if (bCalculateForAll && pCRCMultiplier != NULL && pCRCMultiplier->first == 0) 
			pCRCMultiplier = NULL;
		if (pCRCMultiplier != NULL)
			pipelineData.m_TextureCRCMultiplier[Sampler] = pCRCMultiplier;
		else
		{
			CComQIPtr<IDirect3DTexture9> pTexture(pBaseTexture);
			CComPtr<IDirect3DSurface9> pSurface;
			if (pTexture)
			{
				pTexture->GetSurfaceLevel(0, &pSurface);
				if (pSurface)
					pipelineData.m_TextureCRCMultiplier[Sampler] = CRCTextureFromSurfaceCheck(pSurface, NULL, pTexture);
			} else
			{
				CComQIPtr<IDirect3DCubeTexture9> pCubeTexture(pBaseTexture);
				if (pCubeTexture)
				{
					pCubeTexture->GetCubeMapSurface(D3DCUBEMAP_FACE_POSITIVE_X, 0, &pSurface);
					if (pSurface)
						pipelineData.m_TextureCRCMultiplier[Sampler] = CRCTextureFromSurfaceCheck(pSurface, NULL, pCubeTexture);
				}
			}
			if (!pSurface)
			{
				ResourceCRCMultiplier pair;
				pair.first = 0;
				pipelineData.m_TextureCRCMultiplier[Sampler] = SetCRCMultiplier(pBaseTexture, &pair);
			}
		}
	}
	else
		pipelineData.m_TextureCRCMultiplier[Sampler] = NULL;
	return hResult;
}

inline UINT	CBaseStereoRenderer::GetCorrectSwapChainIndex(UINT iSwapChain)
{
	if (m_bTwoWindows)
		return (iSwapChain <=0 ? 0 : iSwapChain + 1);
	else
		return iSwapChain;
}

MIDL_INTERFACE("CEACF920-7530-4481-90B3-D4BB1F35BB1F")
IInfo: public IUnknown
{
	ResourceCRCMultiplier m_CRCMultiplier;
public:
	inline void Set(const ResourceCRCMultiplier *info) { m_CRCMultiplier = *info; };
	inline const ResourceCRCMultiplier* Get() { return &m_CRCMultiplier; };
};

class CInfo: 
	public IInfo,
	public CComObjectRoot,
	public CComCoClass<CInfo>
{
public:
	BEGIN_COM_MAP(CInfo)
		COM_INTERFACE_ENTRY(IInfo)
	END_COM_MAP()
};

inline const ResourceCRCMultiplier* CBaseStereoRenderer::SetCRCMultiplier(IDirect3DResource9 *pInterface, const ResourceCRCMultiplier *pCRCMultiplier)
{
	if (pInterface) 
	{
		CComPtr<IInfo> pInfo;
		CInfo::CreateInstance(&pInfo);
		pInfo->Set(pCRCMultiplier);
		pInterface->SetPrivateData(__uuidof(IInfo), pInfo, sizeof(IUnknown *), D3DSPD_IUNKNOWN);
		return pInfo->Get();
	}
	return NULL;
}

inline const ResourceCRCMultiplier * CBaseStereoRenderer::GetCRCMultiplier(IDirect3DResource9 *pInterface)
{
	if (pInterface) 
	{
		CComPtr<IInfo> pInfo;
		DWORD size = sizeof(IUnknown *);
		pInterface->GetPrivateData(__uuidof(IInfo), (void *)&pInfo, &size); 
		if (pInfo)
			return pInfo->Get();
	}
	return NULL;
}

inline  bool CBaseStereoRenderer::IsStereoRender()
{
	DEBUG_TRACE3(_T("\tUsed stereo textures: %d\n"), m_bUsedStereoTextures);
	DEBUG_TRACE3(_T("\tUsed stereo DS: %d\n"), m_DepthStencil.GetType() == Stereo);
	if (m_bRenderInStereo)
		return true;
	else if (m_bUsedStereoTextures)
		return true;
	else 
		return m_DepthStencil.GetType() == Stereo;
}

inline  bool CBaseStereoRenderer::ShadowFormat( D3DFORMAT Format ) { 
	if (Format == D3DFMT_R32F/* || Format == D3DFMT_R16F*/)
	{
		m_nRenderTargetR32FTextureCounter++;
		return gInfo.CreateOneComponentRTInMono != 0; 
	}
	else
		return false;
}

inline  bool CBaseStereoRenderer::RenderToRight() 
{ 
	if (m_Input.StereoActive)
		return true;
	else
	{
		switch(gInfo.RenderToRightInMono)
		{
		case 0:
			return false;
		case 1:
			return RenderToTextures();
		default:
			return true;
		}
	}
}

inline bool CBaseStereoRenderer::NeedDumpMeshes( UINT PrimitiveCount, D3DPRIMITIVETYPE PrimitiveType )
{
#ifndef FINAL_RELEASE
	if (DO_RTDUMP && gInfo.DumpMeshes && m_bRenderInStereo) {
		return (PrimitiveType == D3DPT_TRIANGLEFAN || PrimitiveType == D3DPT_TRIANGLESTRIP || PrimitiveType == D3DPT_TRIANGLELIST)
			&& PrimitiveCount <= 300 // skip big meshes
			;
	} else
#endif
	return false;
}

inline bool CBaseStereoRenderer::IsSquareSize( UINT Width, UINT Height, BOOL CreateBigInStereo )
{
	if (Width == Height)
	{
		// if square backbuffer and equal to size
		if (GetBaseSC()->m_BackBufferSizeBeforeScaling.cx == GetBaseSC()->m_BackBufferSizeBeforeScaling.cy && Width == (UINT)GetBaseSC()->m_BackBufferSizeBeforeScaling.cx)
			return false;
		if (CreateBigInStereo && (Width >= (UINT)GetBaseSC()->m_BackBufferSizeBeforeScaling.cx && Height >= (UINT)GetBaseSC()->m_BackBufferSizeBeforeScaling.cy))
			return false;
		return true;
	}
	return false;
}

inline bool CBaseStereoRenderer::IsLessThanBB( UINT Width, UINT Height )
{
	if ((Width != Height) && ((Width + 5) < (UINT)GetBaseSC()->m_BackBufferSizeBeforeScaling.cx && (Height + 5) < (UINT)GetBaseSC()->m_BackBufferSizeBeforeScaling.cy))
	{
		float aRT = (1.0f * Width / Height);
		float aBB = (1.0f * GetBaseSC()->m_BackBufferSizeBeforeScaling.cx / GetBaseSC()->m_BackBufferSizeBeforeScaling.cy);
		float f = abs(aRT - aBB);
		if (f >= 0.01f)
		{
			DEBUG_TRACE1(_T("\tAspect RT=%f (aspect BB=%f, diff = %f)\n"), aRT, aBB, f);
			return true;
		}
	}
	return false;
}

inline CONST TCHAR* CBaseStereoRenderer::GetModeString()
{
	return m_bRenderInStereo ? _T("Stereo") : _T("Mono");
}

inline CONST TCHAR* CBaseStereoRenderer::GetViewString( VIEWINDEX view )
{
	return view != VIEWINDEX_RIGHT ? _T("Left") : _T("Right");
}

#endif
