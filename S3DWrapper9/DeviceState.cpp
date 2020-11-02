#include "StdAfx.h"
#include "DeviceState.h"
#include "CommandDumper.h"
#include "Hook.h"

#ifndef FINAL_RELEASE

template<class T, int multiplier>
void block_array<T, multiplier>::add( UINT start,const T *data,UINT count )
{
	block* biCont = NULL;
	for(int i = 0; i < (int)m_blocks.size(); ++i)
	{
		block &bi = m_blocks[i];
		if (start >= bi.start && start <= bi.start + bi.count)
		{
			biCont = &bi;
			break;
		}
	}
	if (biCont)
		biCont->extend(start,count,data);
	else
	{
		block b(start,count,data);
		m_blocks.push_back(std::move(b));
		biCont = &m_blocks[m_blocks.size() - 1];
	}
	for(int i = (int)m_blocks.size() - 1; i >= 0; --i)
	{
		block &bi = m_blocks[i];
		// TODO: Support merging intersected but not overlapped blocks
		if (&bi != biCont)
		{
			if (biCont->isOverlap(bi))
				m_blocks.erase(m_blocks.begin() + i);
			else if (biCont->isIntersect(bi))
			{
				biCont->merge(bi);
				m_blocks.erase(m_blocks.begin() + i);
			}
		}
	}
}

CDeviceState::CDeviceState()
{
}

CDeviceState::~CDeviceState()
{
}

UINT64 SaveToBinaryFile(const void *data,int len);
UINT GetSizeOfBuffer(IDirect3DIndexBuffer9 *buf);
UINT GetSizeOfBuffer(IDirect3DVertexBuffer9 *buf);

HRESULT DumpSurface( IDirect3DDevice9* dev, IDirect3DSurface9* src, D3DSURFACE_DESC &desc, UINT64 &databin_pose )
{
	HRESULT hResult = E_FAIL;
	if (desc.Pool != D3DPOOL_DEFAULT || (desc.Usage & D3DUSAGE_DYNAMIC))
	{
		D3DLOCKED_RECT rect;
		NSCALL(src->LockRect(&rect,0,D3DLOCK_READONLY));
		if (SUCCEEDED(hResult))
		{
			if (IsCompressedFormat(desc.Format))
				rect.Pitch /= 4;

			databin_pose = SaveToBinaryFile(rect.pBits, rect.Pitch * desc.Height);

			NSCALL(src->UnlockRect());
		}
	}
	else if (desc.Usage & D3DUSAGE_RENDERTARGET)
	{
		IDirect3DSurface9* lockedSurface = src;
		CComPtr<IDirect3DSurface9> dst;
		D3DLOCKED_RECT rect;
		hResult = src->LockRect(&rect,0,D3DLOCK_READONLY); // Try lock
		if (FAILED(hResult)) // if RT not lockable
		{
			NSCALL(dev->CreateOffscreenPlainSurface(desc.Width,desc.Height,desc.Format,D3DPOOL_SYSTEMMEM,&dst,0));
			if (SUCCEEDED(hResult))
			{
				NSCALL(dev->GetRenderTargetData(src, dst));
				NSCALL(dst->LockRect(&rect,0,D3DLOCK_READONLY));
			}
			else
			{
				NSCALL(dev->CreateRenderTarget(desc.Width,desc.Height,desc.Format,D3DMULTISAMPLE_NONE,0,TRUE,&dst,0));
				if (SUCCEEDED(hResult)) 
				{
					NSCALL(dev->StretchRect(src, NULL, dst, NULL, D3DTEXF_NONE));
					NSCALL(dst->LockRect(&rect,0,D3DLOCK_READONLY));
				}
			}
			lockedSurface = dst;
		}
		if (SUCCEEDED(hResult))
		{
			if (IsCompressedFormat(desc.Format))
				rect.Pitch /= 4;

			databin_pose = SaveToBinaryFile(rect.pBits, rect.Pitch * desc.Height);

			NSCALL(lockedSurface->UnlockRect());
		}
		else
		{
			DEBUG_MESSAGE(_T("Dump surface failed, Desc: (Width = %u, Height = %u, Format = %s, Pool = %s)\n"), 
				desc.Width, desc.Height, GetFormatString(desc.Format), 
				GetPoolString(D3DPOOL_SYSTEMMEM));
		}
	}
	else
	{
		DEBUG_MESSAGE(_T("Cannot dump surface(Width = %u, Height = %u, Format = %s, Usage = %s, Pool = %s)\n"), 
			desc.Width, desc.Height, GetFormatString(desc.Format), GetUsageString(desc.Usage), GetPoolString(desc.Pool));
	}
	return hResult;
}

void DumpTexture(IDirect3DTexture9 *tex,IDirect3DBaseTexture9* pTexture)
{
	HRESULT hResult = S_OK;
	CComPtr<IDirect3DDevice9> dev;
	NSCALL(tex->GetDevice(&dev));

	DWORD levels = tex->GetLevelCount();
	UINT64 *databin_poses = new UINT64[levels];

	for(DWORD level = 0;level < levels;level++)
	{
		CComPtr<IDirect3DSurface9> src;
		NSCALL(tex->GetSurfaceLevel(level,&src));
		D3DSURFACE_DESC desc;
		NSCALL(src->GetDesc(&desc));
		
		if (desc.Usage & D3DUSAGE_DEPTHSTENCIL) 
		{
			delete[] databin_poses; // bad practice
			return;
		}

		if (FAILED(DumpSurface(dev, src, desc, databin_poses[level])))
		{
			delete[] databin_poses; // bad practice
			return;
		}
	}

	DatabinPoses poses(databin_poses,levels);
	DUMP_CMD_ALL(ResourceUpdateTexture2,pTexture,poses);
	delete[]databin_poses;
}

void DumpCube(IDirect3DCubeTexture9 *cube,IDirect3DBaseTexture9* pTexture)
{
	HRESULT hResult = S_OK;
	CComPtr<IDirect3DDevice9> dev;
	NSCALL(cube->GetDevice(&dev));

	DWORD levels = cube->GetLevelCount();
	UINT64 *databin_poses = new UINT64[levels * 6];

	for(DWORD level = 0;level < cube->GetLevelCount();level++)
	{

		for(int f = 0;f < 6;f++)
		{
			D3DCUBEMAP_FACES face = (D3DCUBEMAP_FACES)f;
			CComPtr<IDirect3DSurface9> src;
			NSCALL(cube->GetCubeMapSurface(face,level,&src));

			D3DSURFACE_DESC desc;
			NSCALL(src->GetDesc(&desc));

			if (desc.Usage & D3DUSAGE_DEPTHSTENCIL) 
			{
				delete[] databin_poses; // bad practice
				return;
			}

			if (FAILED(DumpSurface(dev, src, desc, databin_poses[level * 6 + f])))
			{
				delete[] databin_poses; // bad practice
				return;
			}
		}
	}

	DatabinPoses poses(databin_poses,levels * 6);
	DUMP_CMD_ALL(ResourceUpdateCubeTexture2,pTexture,poses);
	delete[] databin_poses;
}

void DumpVolume(IDirect3DVolumeTexture9 *vol,IDirect3DBaseTexture9* pTexture)
{
	HRESULT hResult = S_OK;
	for(DWORD level = 0;level < vol->GetLevelCount();level++)
	{
		D3DVOLUME_DESC desc;
		NSCALL(vol->GetLevelDesc(level,&desc));

		if (desc.Usage & D3DUSAGE_RENDERTARGET) __debugbreak();
		if (desc.Usage & D3DUSAGE_DEPTHSTENCIL) return;

		D3DLOCKED_BOX box;
		NSCALL(vol->LockBox(level,&box,0,D3DLOCK_READONLY));
		if (SUCCEEDED(hResult))
		{
			if (IsCompressedFormat(desc.Format))
				box.SlicePitch /= 4;

			UINT64 databin_pos = SaveToBinaryFile(box.pBits,box.SlicePitch * desc.Depth);

			NSCALL(vol->UnlockBox(level));

			DUMP_CMD_ALL(ResourceUpdateVolumeTexture,pTexture,level,databin_pos);
		}
		else
		{
			DEBUG_MESSAGE(_T("Cannot dump volume(Width = %u, Height = %u, Depth = %u, Format = %s, Usage = %s, Pool = %s)\n"), 
				desc.Width, desc.Height, desc.Depth, GetFormatString(desc.Format), 
				GetUsageString(desc.Usage), GetPoolString(desc.Pool));
		}
	}
}

void CDeviceState::DumpResources()
{
	{
		DUMP_CMD_ALL(BeginDumpResources);
	}

	// SetTexture
	for(auto it = m_textures.begin();it != m_textures.end();++it)
	{
		IDirect3DBaseTexture9* pTexture = it->second.m_T;
		if (!pTexture) continue;

		CComQIPtr<IDirect3DTexture9> tex;
		CComQIPtr<IDirect3DCubeTexture9> cube;
		CComQIPtr<IDirect3DVolumeTexture9> vol;
		if (tex = pTexture)
			DumpTexture(tex,pTexture);	
		else if (cube = pTexture)
			DumpCube(cube,pTexture);	
		else if (vol = pTexture)
			DumpVolume(vol,pTexture);
		else
		{
			__debugbreak();
		}
	}

	// SetIndices
	if (m_pIndeces.HasValue() && m_pIndeces.m_T)
	{
		UINT size = GetSizeOfBuffer(m_pIndeces.m_T);
		void *pData;
		IDirect3DIndexBuffer9* pIndexData = m_pIndeces;
		HRESULT hr = pIndexData->Lock(0,size,&pData,D3DLOCK_READONLY);
		if (SUCCEEDED(hr))
		{
			UINT64 databin_pos = SaveToBinaryFile(pData,size);

			pIndexData->Unlock();

			DUMP_CMD_ALL(ResourceUpdateBuffer,pIndexData,databin_pos);
		}
		else
		{
			DEBUG_MESSAGE(_T("Cannot lock index buffer\n"));
		}
	}

	// SetStreamSource
	for(auto it = m_streamSources.begin();it != m_streamSources.end();++it)
	{
		IDirect3DVertexBuffer9* pStreamData = it->second.pStreamData;
		if (!pStreamData) continue;
		
		UINT size = GetSizeOfBuffer(pStreamData);
		void *pData;
		HRESULT hr = pStreamData->Lock(0,size,&pData,D3DLOCK_READONLY);
		if (SUCCEEDED(hr))
		{
			UINT64 databin_pos = SaveToBinaryFile(pData,size);

			pStreamData->Unlock();

			DUMP_CMD_ALL(ResourceUpdateBuffer,pStreamData,databin_pos);
		}
		else
		{
			DEBUG_MESSAGE(_T("Cannot lock vertex buffer\n"));
		}
	}

	DUMP_CMD_ALL(EndDumpResources);
}

void CDeviceState::Dump(BOOL bForce)
{
	BOOL oldValue;
	if (bForce)
		oldValue = CCommandDumper::GetInstance().SetForce(TRUE);

	{
		DUMP_CMD_ALL(BeginDeviceState);
	}

	// SetRenderState
	for(auto it = m_renderState.begin();it != m_renderState.end();++it)
	{
		D3DRENDERSTATETYPE State = it->first;
		DWORD Value = it->second;
		DUMP_CMD_ALL(SetRenderState,State,Value);
	}

	// SetTextureStageState
	for(auto it = m_textureStageStates.begin();it != m_textureStageStates.end();++it)
	{
		DWORD Stage = it->first.first;
		D3DTEXTURESTAGESTATETYPE Type = it->first.second;
		DWORD Value = it->second;

		DUMP_CMD_ALL(SetTextureStageState,Stage,Type,Value);
	}

	// SetSamplerState
	for(auto it = m_samplerStates.begin();it != m_samplerStates.end();++it)
	{
		DWORD Sampler = it->first.first;
		D3DSAMPLERSTATETYPE Type = it->first.second;
		DWORD Value = it->second;

		DUMP_CMD_ALL(SetSamplerState,Sampler,Type,Value);
	}

	// SetTexture
	for(auto it = m_textures.begin();it != m_textures.end();++it)
	{
		DWORD Stage = it->first;
		IDirect3DBaseTexture9* pTexture = it->second.m_T;
		DUMP_CMD_ALL(SetTexture,Stage,pTexture);
	}

	// SetMaterial
	if (m_material.HasValue())
	{
		CONST D3DMATERIAL9* pMaterial = &m_material;
		DUMP_CMD_ALL(SetMaterial,pMaterial);
	}

	// SetTransform
	for(auto it = m_transforms.begin();it != m_transforms.end();++it)
	{
		D3DTRANSFORMSTATETYPE State = it->first;
		CONST D3DMATRIX* pMatrix = &it->second;
		DUMP_CMD_ALL(SetTransform,State,pMatrix);
	}

	// SetStreamSource
	for(auto it = m_streamSources.begin();it != m_streamSources.end();++it)
	{
		UINT StreamNumber = it->first;
		IDirect3DVertexBuffer9* pStreamData = it->second.pStreamData;
		UINT OffsetInBytes = it->second.OffsetInBytes;
		UINT Stride = it->second.Stride;
		DUMP_CMD_ALL(SetStreamSource,StreamNumber,pStreamData,OffsetInBytes,Stride);
	}

	// SetStreamSourceFreq
	for(auto it = m_freqs.begin();it != m_freqs.end();++it)
	{
		UINT StreamNumber = it->first;
		UINT Divider = it->second;
		DUMP_CMD_ALL(SetStreamSourceFreq,StreamNumber,Divider);
	}

	// SetFVF
	if (m_fvf.HasValue())
	{
		DWORD FVF = m_fvf;
		DUMP_CMD_ALL(SetFVF,FVF);
	} // SetVertexDeclaration
	else if (m_pVDecl.HasValue())
	{
		IDirect3DVertexDeclaration9* pDecl = m_pVDecl;
		DUMP_CMD_ALL(SetVertexDeclaration,pDecl);
	}

	// SetViewport 
	if (m_viewport.HasValue())
	{
		CONST D3DVIEWPORT9 *pViewport = &m_viewport;
		DUMP_CMD_ALL(SetViewport,pViewport);
	}

	// SetScissorRect
	if (m_scissorRect.HasValue())
	{
		CONST RECT* pRect = &m_scissorRect;
		DUMP_CMD_ALL(SetScissorRect,pRect);
	}

	// SetPixelShader
	if (m_pPShader.HasValue())
	{
		IDirect3DPixelShader9* pShader = m_pPShader;
		DUMP_CMD_ALL(SetPixelShader,pShader);
	}

	// SetVertexShader
	if (m_pVShader.HasValue())
	{
		IDirect3DVertexShader9* pShader = m_pVShader;
		DUMP_CMD_ALL(SetVertexShader,pShader);
	}

	// SetIndices
	if (m_pIndeces.HasValue())
	{
		IDirect3DIndexBuffer9* pIndexData = m_pIndeces;
		DUMP_CMD_ALL(SetIndices,pIndexData);
	}

	// LightEnable
	for(auto it = m_lightEnables.begin();it != m_lightEnables.end();++it)
	{
		DWORD Index = it->first;
		BOOL Enable = it->second;
		DUMP_CMD_ALL(LightEnable,Index,Enable);
	}

	// SetLight
	for(auto it = m_lights.begin();it != m_lights.end();++it)
	{
		DWORD Index = it->first;
		CONST D3DLIGHT9* pLight = &it->second;
		DUMP_CMD_ALL(SetLight,Index,pLight);
	}

	// SetClipPlane
	for(auto it = m_clipPlanes.begin();it != m_clipPlanes.end();++it)
	{
		DWORD Index = it->first;
		CONST float* pPlane = it->second;
		ClipPlaneData plane(pPlane);
		DUMP_CMD_ALL(SetClipPlane,Index,plane);
	}

	// SetCurrentTexturePalette
	if (m_palette.HasValue())
	{
		UINT PaletteNumber = m_palette;
		DUMP_CMD_ALL(SetCurrentTexturePalette,PaletteNumber);
	}

	// SetNPatchMode
	if (m_nPatchMode.HasValue())
	{
		float nSegments = m_nPatchMode;
		DUMP_CMD_ALL(SetNPatchMode,nSegments);
	}

#define Dump_SetXxShaderConstantY(cmd,x,y,type) \
	for(UINT i = 0;i < m_##x##shaderConst##y.size();i++) \
	{ \
		UINT StartRegister; \
		const type *pConstantData; \
		UINT count; \
		m_##x##shaderConst##y.get(i,StartRegister,pConstantData,count); \
		ShaderConstants##y constantData(pConstantData,count); \
		DUMP_CMD_ALL(cmd,StartRegister,constantData,count); \
	}

	// Set***ShaderConstant*
	Dump_SetXxShaderConstantY(SetPixelShaderConstantF,p,F,float);
	Dump_SetXxShaderConstantY(SetPixelShaderConstantI,p,I,int);
	Dump_SetXxShaderConstantY(SetPixelShaderConstantB,p,B,BOOL);
	Dump_SetXxShaderConstantY(SetVertexShaderConstantF,v,F,float);
	Dump_SetXxShaderConstantY(SetVertexShaderConstantI,v,I,int);
	Dump_SetXxShaderConstantY(SetVertexShaderConstantB,v,B,BOOL);

#undef Dump_SetXxShaderConstantY

	{
		DUMP_CMD_ALL(EndDeviceState);
	}

	if (bForce) CCommandDumper::GetInstance().SetForce(oldValue);
}

void CDeviceState::DumpBaseSurfaces(IDirect3DDevice9 *dev)
{
	BOOL oldValue = CCommandDumper::GetInstance().SetForce(TRUE);

	for(DWORD RenderTargetIndex = 0;RenderTargetIndex < 4;RenderTargetIndex++)
	{
		CComPtr<IDirect3DSurface9> pRenderTarget;
		if (SUCCEEDED(dev->GetRenderTarget(RenderTargetIndex,&pRenderTarget)))
		{
			DUMP_CMD_ALL(GetRenderTarget,RenderTargetIndex,pRenderTarget);
		}
	}

	CComPtr<IDirect3DSurface9> pZStencilSurface;
	if (SUCCEEDED(dev->GetDepthStencilSurface(&pZStencilSurface)))
	{
		DUMP_CMD_ALL(GetDepthStencilSurface,pZStencilSurface);
	}

	CCommandDumper::GetInstance().SetForce(oldValue);
}

void CDeviceState::SetClipPlane(DWORD Index, CONST float* pPlane)
{
	m_clipPlanes[Index] = pPlane;
}

void CDeviceState::SetCurrentTexturePalette(UINT PaletteNumber)
{
	m_palette = PaletteNumber;
}
	
void CDeviceState::SetNPatchMode(float nSegments)
{
	m_nPatchMode = nSegments;
}

void CDeviceState::SetStreamSourceFreq(UINT StreamNumber, UINT Divider)
{
	m_freqs[StreamNumber] = Divider;
}

void CDeviceState::SetScissorRect(CONST RECT* pRect)
{
	m_scissorRect = *pRect;
}

void CDeviceState::SetViewport(CONST D3DVIEWPORT9* pViewport)
{
	m_viewport = *pViewport;
}

void CDeviceState::LightEnable(DWORD Index, BOOL Enable)
{
	m_lightEnables[Index] = Enable;
}

void CDeviceState::SetLight(DWORD Index, CONST D3DLIGHT9* pLight)
{
	m_lights[Index] = *pLight;
}

void CDeviceState::SetPixelShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount)
{
	m_pshaderConstF.add(StartRegister,pConstantData,Vector4fCount);
}

void CDeviceState::SetPixelShaderConstantI(UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount)
{
	m_pshaderConstI.add(StartRegister,pConstantData,Vector4iCount);
}

void CDeviceState::SetPixelShaderConstantB(UINT StartRegister, CONST BOOL* pConstantData, UINT BoolCount)
{
	m_pshaderConstB.add(StartRegister,pConstantData,BoolCount);
}

void CDeviceState::SetVertexShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount)
{
	m_vshaderConstF.add(StartRegister,pConstantData,Vector4fCount);
}

void CDeviceState::SetVertexShaderConstantI(UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount)
{
	m_vshaderConstI.add(StartRegister,pConstantData,Vector4iCount);
}

void CDeviceState::SetVertexShaderConstantB(UINT StartRegister, CONST BOOL* pConstantData, UINT BoolCount)
{
	m_vshaderConstB.add(StartRegister,pConstantData,BoolCount);
}

void CDeviceState::SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value)
{
	m_samplerStates[std::make_pair(Sampler,Type)] = Value;
}

void CDeviceState::SetIndices(IDirect3DIndexBuffer9* pIndexData)
{
	m_pIndeces = pIndexData;
}

void CDeviceState::SetVertexDeclaration(IDirect3DVertexDeclaration9* pDecl)
{
	m_fvf.clear();
	m_pVDecl = pDecl;
}

void CDeviceState::SetFVF(DWORD FVF)
{
	m_pVDecl.clear();
	m_fvf = FVF;
}

void CDeviceState::SetVertexShader(IDirect3DVertexShader9* pShader)
{
	m_pVShader = pShader;
}

void CDeviceState::SetPixelShader(IDirect3DPixelShader9* pShader)
{
	m_pPShader = pShader;
}

void CDeviceState::SetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9* pStreamData, UINT OffsetInBytes, UINT Stride)
{
	SetStreamSourceData data = { pStreamData, OffsetInBytes, Stride };
	m_streamSources[StreamNumber] = data;
}

void CDeviceState::SetRenderState(D3DRENDERSTATETYPE State, DWORD Value)
{
	m_renderState[State] = Value;
}

void CDeviceState::SetMaterial(CONST D3DMATERIAL9* pMaterial)
{
	m_material = *pMaterial;
}

void CDeviceState::SetTexture(DWORD Stage, IDirect3DBaseTexture9* pTexture)
{
	m_textures[Stage] = pTexture;
}

void CDeviceState::SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value)
{
	m_textureStageStates[std::make_pair(Stage,Type)] = Value;
}

void CDeviceState::SetTransform(D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* pMatrix)
{
	m_transforms[State] = *pMatrix;
}

void CDeviceState::Create(D3DSTATEBLOCKTYPE Type, const CDeviceState &deviceState)
{
	HRESULT hResult = S_OK;
	Clear();
	bool bAll = (Type == D3DSBT_ALL);
	bool bPixel = bAll || (Type == D3DSBT_PIXELSTATE);
	bool bVertex = bAll || (Type == D3DSBT_VERTEXSTATE);
#define CAPTURE_RS(state) {auto it = deviceState.m_renderState.find(state); \
	if (it != deviceState.m_renderState.end()) m_renderState[state] = (*it).second;}

	if (bPixel)
	{
		CAPTURE_RS(D3DRS_ZENABLE);
		CAPTURE_RS(D3DRS_FILLMODE);
		CAPTURE_RS(D3DRS_SHADEMODE);
		CAPTURE_RS(D3DRS_ZWRITEENABLE);
		CAPTURE_RS(D3DRS_ALPHATESTENABLE);
		CAPTURE_RS(D3DRS_LASTPIXEL);
		CAPTURE_RS(D3DRS_SRCBLEND);
		CAPTURE_RS(D3DRS_DESTBLEND);
		CAPTURE_RS(D3DRS_ZFUNC);
		CAPTURE_RS(D3DRS_ALPHAREF);                  
		CAPTURE_RS(D3DRS_ALPHAFUNC);                 
		CAPTURE_RS(D3DRS_DITHERENABLE);              
		CAPTURE_RS(D3DRS_ALPHABLENDENABLE);                 
		CAPTURE_RS(D3DRS_SPECULARENABLE);    
		CAPTURE_RS(D3DRS_STENCILENABLE);             
		CAPTURE_RS(D3DRS_STENCILFAIL);               
		CAPTURE_RS(D3DRS_STENCILZFAIL);              
		CAPTURE_RS(D3DRS_STENCILPASS);               
		CAPTURE_RS(D3DRS_STENCILFUNC);               
		CAPTURE_RS(D3DRS_STENCILREF);                
		CAPTURE_RS(D3DRS_STENCILMASK);               
		CAPTURE_RS(D3DRS_STENCILWRITEMASK);          
		CAPTURE_RS(D3DRS_TEXTUREFACTOR);             
		CAPTURE_RS(D3DRS_WRAP0);                     
		CAPTURE_RS(D3DRS_WRAP1);                     
		CAPTURE_RS(D3DRS_WRAP2);                     
		CAPTURE_RS(D3DRS_WRAP3);                     
		CAPTURE_RS(D3DRS_WRAP4);                     
		CAPTURE_RS(D3DRS_WRAP5);                     
		CAPTURE_RS(D3DRS_WRAP6);                     
		CAPTURE_RS(D3DRS_WRAP7);                    
		CAPTURE_RS(D3DRS_COLORWRITEENABLE);         
		CAPTURE_RS(D3DRS_BLENDOP);      
		CAPTURE_RS(D3DRS_SCISSORTESTENABLE);         
		CAPTURE_RS(D3DRS_SLOPESCALEDEPTHBIAS);       
		CAPTURE_RS(D3DRS_ANTIALIASEDLINEENABLE); 
		CAPTURE_RS(D3DRS_TWOSIDEDSTENCILMODE);       
		CAPTURE_RS(D3DRS_CCW_STENCILFAIL);           
		CAPTURE_RS(D3DRS_CCW_STENCILZFAIL);          
		CAPTURE_RS(D3DRS_CCW_STENCILPASS);           
		CAPTURE_RS(D3DRS_CCW_STENCILFUNC);           
		CAPTURE_RS(D3DRS_COLORWRITEENABLE1);         
		CAPTURE_RS(D3DRS_COLORWRITEENABLE2);         
		CAPTURE_RS(D3DRS_COLORWRITEENABLE3);         
		CAPTURE_RS(D3DRS_BLENDFACTOR);               
		CAPTURE_RS(D3DRS_SRGBWRITEENABLE);          
		CAPTURE_RS(D3DRS_DEPTHBIAS);                 
		CAPTURE_RS(D3DRS_WRAP8);                     
		CAPTURE_RS(D3DRS_WRAP9);                     
		CAPTURE_RS(D3DRS_WRAP10);                    
		CAPTURE_RS(D3DRS_WRAP11);                    
		CAPTURE_RS(D3DRS_WRAP12);                    
		CAPTURE_RS(D3DRS_WRAP13);                    
		CAPTURE_RS(D3DRS_WRAP14);                    
		CAPTURE_RS(D3DRS_WRAP15);                    
		CAPTURE_RS(D3DRS_SEPARATEALPHABLENDENABLE);  
		CAPTURE_RS(D3DRS_SRCBLENDALPHA);             
		CAPTURE_RS(D3DRS_DESTBLENDALPHA);            
		CAPTURE_RS(D3DRS_BLENDOPALPHA);
	}

	if (bVertex)
	{
		CAPTURE_RS(D3DRS_CULLMODE);         
		CAPTURE_RS(D3DRS_FOGENABLE);            
		CAPTURE_RS(D3DRS_FOGCOLOR);                  
		CAPTURE_RS(D3DRS_FOGTABLEMODE);
		CAPTURE_RS(D3DRS_RANGEFOGENABLE);                    
		CAPTURE_RS(D3DRS_CLIPPING);                  
		CAPTURE_RS(D3DRS_LIGHTING);                  
		CAPTURE_RS(D3DRS_AMBIENT);                   
		CAPTURE_RS(D3DRS_FOGVERTEXMODE);             
		CAPTURE_RS(D3DRS_COLORVERTEX);               
		CAPTURE_RS(D3DRS_NORMALIZENORMALS); //???      
		CAPTURE_RS(D3DRS_VERTEXBLEND);               
		CAPTURE_RS(D3DRS_CLIPPLANEENABLE);           
		CAPTURE_RS(D3DRS_POINTSIZE);                 
		CAPTURE_RS(D3DRS_POINTSIZE_MIN);             
		CAPTURE_RS(D3DRS_POINTSPRITEENABLE);         
		CAPTURE_RS(D3DRS_POINTSCALEENABLE);          
		CAPTURE_RS(D3DRS_POINTSCALE_A);              
		CAPTURE_RS(D3DRS_POINTSCALE_B);              
		CAPTURE_RS(D3DRS_POINTSCALE_C);              
		CAPTURE_RS(D3DRS_MULTISAMPLEANTIALIAS);      
		CAPTURE_RS(D3DRS_MULTISAMPLEMASK);           
		CAPTURE_RS(D3DRS_PATCHEDGESTYLE);          
		CAPTURE_RS(D3DRS_POINTSIZE_MAX);             
		CAPTURE_RS(D3DRS_INDEXEDVERTEXBLENDENABLE);  
		CAPTURE_RS(D3DRS_TWEENFACTOR);              
		CAPTURE_RS(D3DRS_POSITIONDEGREE);            
		CAPTURE_RS(D3DRS_NORMALDEGREE);             
		CAPTURE_RS(D3DRS_MINTESSELLATIONLEVEL);      
		CAPTURE_RS(D3DRS_MAXTESSELLATIONLEVEL);      
		CAPTURE_RS(D3DRS_ADAPTIVETESS_X);            
		CAPTURE_RS(D3DRS_ADAPTIVETESS_Y);            
		CAPTURE_RS(D3DRS_ADAPTIVETESS_Z);            
		CAPTURE_RS(D3DRS_ADAPTIVETESS_W);            
		CAPTURE_RS(D3DRS_ENABLEADAPTIVETESSELLATION);
	}

	if (bPixel || bVertex)
	{               
		CAPTURE_RS(D3DRS_FOGSTART);                  
		CAPTURE_RS(D3DRS_FOGEND);              
		CAPTURE_RS(D3DRS_FOGDENSITY);        
		CAPTURE_RS(D3DRS_LOCALVIEWER);
		CAPTURE_RS(D3DRS_DIFFUSEMATERIALSOURCE);     
		CAPTURE_RS(D3DRS_SPECULARMATERIALSOURCE);    
		CAPTURE_RS(D3DRS_AMBIENTMATERIALSOURCE);     
		CAPTURE_RS(D3DRS_EMISSIVEMATERIALSOURCE); 
	} 

	if (bAll)
	{
		CAPTURE_RS(D3DRS_DEBUGMONITORTOKEN);  //???  
	}  

#undef CAPTURE_RS

#define CAPTURE_TSS(state) {if ((*it).first.second == state) \
	{ m_textureStageStates[(*it).first] = (*it).second; continue; } }

	for (auto it = deviceState.m_textureStageStates.begin(); it != deviceState.m_textureStageStates.end(); it++)
	{
		if (bPixel)
		{      
			CAPTURE_TSS(D3DTSS_COLOROP);
			CAPTURE_TSS(D3DTSS_COLORARG1);
			CAPTURE_TSS(D3DTSS_COLORARG2);
			CAPTURE_TSS(D3DTSS_ALPHAOP);
			CAPTURE_TSS(D3DTSS_ALPHAARG1);
			CAPTURE_TSS(D3DTSS_ALPHAARG2);
			CAPTURE_TSS(D3DTSS_BUMPENVMAT00);
			CAPTURE_TSS(D3DTSS_BUMPENVMAT01);
			CAPTURE_TSS(D3DTSS_BUMPENVMAT10);
			CAPTURE_TSS(D3DTSS_BUMPENVMAT11);
			CAPTURE_TSS(D3DTSS_BUMPENVLSCALE);
			CAPTURE_TSS(D3DTSS_BUMPENVLOFFSET);
			CAPTURE_TSS(D3DTSS_COLORARG0);
			CAPTURE_TSS(D3DTSS_ALPHAARG0);
			CAPTURE_TSS(D3DTSS_RESULTARG);
			CAPTURE_TSS(D3DTSS_CONSTANT); // ???
		}

		if (bPixel || bVertex)
		{      
			CAPTURE_TSS(D3DTSS_TEXCOORDINDEX);
			CAPTURE_TSS(D3DTSS_TEXTURETRANSFORMFLAGS);
		}
	}
#undef CAPTURE_TSS

#define CAPTURE_SS(state) {if ((*it).first.second == state) \
	{ m_samplerStates[(*it).first] = (*it).second; continue; } }
	for (auto it = deviceState.m_samplerStates.begin(); it != deviceState.m_samplerStates.end(); it++)
	{
		if (bPixel)
		{
			CAPTURE_SS(D3DSAMP_ADDRESSU);
			CAPTURE_SS(D3DSAMP_ADDRESSV);
			CAPTURE_SS(D3DSAMP_ADDRESSW);
			CAPTURE_SS(D3DSAMP_BORDERCOLOR);
			CAPTURE_SS(D3DSAMP_MAGFILTER);
			CAPTURE_SS(D3DSAMP_MINFILTER);
			CAPTURE_SS(D3DSAMP_MIPFILTER);
			CAPTURE_SS(D3DSAMP_MIPMAPLODBIAS);
			CAPTURE_SS(D3DSAMP_MAXMIPLEVEL);
			CAPTURE_SS(D3DSAMP_MAXANISOTROPY);
			CAPTURE_SS(D3DSAMP_SRGBTEXTURE);
			CAPTURE_SS(D3DSAMP_ELEMENTINDEX);
		}

		if (bVertex)
		{    
			CAPTURE_SS(D3DSAMP_DMAPOFFSET);
		}
	}
#undef CAPTURE_SS

	if (bAll)
		m_textures = deviceState.m_textures;

	if (bPixel)
	{
		m_pPShader = deviceState.m_pPShader;
		m_pshaderConstF = deviceState.m_pshaderConstF;
		m_pshaderConstI = deviceState.m_pshaderConstI;
		m_pshaderConstB = deviceState.m_pshaderConstB;
	}

	if (bVertex)
	{
		m_pVShader = deviceState.m_pVShader;
		m_vshaderConstF = deviceState.m_vshaderConstF;
		m_vshaderConstI = deviceState.m_vshaderConstI;
		m_vshaderConstB = deviceState.m_vshaderConstB;

		m_fvf = deviceState.m_fvf;
		m_pVDecl = deviceState.m_pVDecl;
		m_nPatchMode = deviceState.m_nPatchMode;
		//m_lightEnables = deviceState.m_lightEnables;
		//m_lights = deviceState.m_lights;
		m_freqs = deviceState.m_freqs;
	}

	if (bAll)
	{
		m_palette = deviceState.m_palette;
		m_pIndeces = deviceState.m_pIndeces;
		m_viewport = deviceState.m_viewport;
		m_scissorRect = deviceState.m_scissorRect;

		m_streamSources = deviceState.m_streamSources;
		m_transforms = deviceState.m_transforms;
		m_clipPlanes = deviceState.m_clipPlanes;
		m_material = deviceState.m_material;
	}
}

void CDeviceState::Clear()
{
	m_renderState.clear();
	m_textures.clear();
	m_textureStageStates.clear();
	m_transforms.clear();
	m_samplerStates.clear();
	m_streamSources.clear();
	m_freqs.clear();
	m_clipPlanes.clear();
	m_lights.clear();
	m_lightEnables.clear();

	m_material.clear();
	m_fvf.clear();
	m_pVDecl.clear();
	m_pVShader.clear();
	m_pPShader.clear();
	m_pIndeces.clear();
	m_viewport.clear();
	m_scissorRect.clear();
	m_nPatchMode.clear();
	m_palette.clear();

	m_pshaderConstF.clear();
	m_pshaderConstI.clear();
	m_pshaderConstB.clear();
	m_vshaderConstF.clear();
	m_vshaderConstI.clear();
	m_vshaderConstB.clear();
}

void CDeviceState::operator =(const CDeviceState &state)
{
	m_renderState 		 = state.m_renderState;
	m_textures 			 = state.m_textures;
	m_textureStageStates = state.m_textureStageStates;
	m_transforms 		 = state.m_transforms;
	m_samplerStates 	 = state.m_samplerStates;
	m_streamSources 	 = state.m_streamSources;
	m_freqs 			 = state.m_freqs;
	m_clipPlanes 		 = state.m_clipPlanes;

	m_lights 			 = state.m_lights;
	m_lightEnables 		 = state.m_lightEnables;

	m_material 	  		= state.m_material;
	m_fvf 		  		= state.m_fvf;
	m_pVDecl 	  		= state.m_pVDecl;

	m_pVShader 	  		= state.m_pVShader;
	m_pPShader 	  		= state.m_pPShader;
	m_pIndeces 	  		= state.m_pIndeces;

	m_viewport 	  		= state.m_viewport;
	m_scissorRect 		= state.m_scissorRect;
	m_nPatchMode		= state.m_nPatchMode;
	m_palette			= state.m_palette;

	m_pshaderConstF 	= state.m_pshaderConstF;
	m_pshaderConstI 	= state.m_pshaderConstI;
	m_pshaderConstB 	= state.m_pshaderConstB;
	m_vshaderConstF 	= state.m_vshaderConstF;
	m_vshaderConstI 	= state.m_vshaderConstI;
	m_vshaderConstB 	= state.m_vshaderConstB;
}

CDeviceState& CDeviceState::operator=( CDeviceState&& state )
{
	m_renderState 		 = std::move(state.m_renderState);
	m_textures 			 = std::move(state.m_textures);
	m_textureStageStates = std::move(state.m_textureStageStates);
	m_transforms 		 = std::move(state.m_transforms);
	m_samplerStates 	 = std::move(state.m_samplerStates);
	m_streamSources 	 = std::move(state.m_streamSources);
	m_freqs 			 = std::move(state.m_freqs);
	m_clipPlanes 		 = std::move(state.m_clipPlanes);

	m_lights 			 = std::move(state.m_lights);
	m_lightEnables 		 = std::move(state.m_lightEnables);

	m_material			 = std::move(state.m_material);
	m_fvf			 	 = std::move(state.m_fvf);
	m_pVDecl			 = std::move(state.m_pVDecl);
	m_pVShader			 = std::move(state.m_pVShader);
	m_pPShader			 = std::move(state.m_pPShader);
	m_pIndeces			 = std::move(state.m_pIndeces);

	m_viewport			 = std::move(state.m_viewport);
	m_scissorRect		 = std::move(state.m_scissorRect);
	m_nPatchMode		 = std::move(state.m_nPatchMode);
	m_palette		 	 = std::move(state.m_palette);

	m_pshaderConstF 	 = std::move(state.m_pshaderConstF);
	m_pshaderConstI 	 = std::move(state.m_pshaderConstI);
	m_pshaderConstB 	 = std::move(state.m_pshaderConstB);
	m_vshaderConstF 	 = std::move(state.m_vshaderConstF);
	m_vshaderConstI 	 = std::move(state.m_vshaderConstI);
	m_vshaderConstB 	 = std::move(state.m_vshaderConstB);
	return *this;
}

template<typename T>
void CaptureFromMap(T& dst, const T& src)
{
	for (auto it = dst.begin(); it != dst.end(); it++)
	{
		auto srcIt = src.find((*it).first);
		if (srcIt != src.end())
			(*it).second = (*srcIt).second;
	}
}

void CDeviceState::Capture( const CDeviceState &deviceState )
{
	CaptureFromMap(m_renderState, 		 deviceState.m_renderState);
	CaptureFromMap(m_textures, 			 deviceState.m_textures);
	CaptureFromMap(m_textureStageStates, deviceState.m_textureStageStates);
	CaptureFromMap(m_transforms, 		 deviceState.m_transforms);
	CaptureFromMap(m_samplerStates, 	 deviceState.m_samplerStates);
	CaptureFromMap(m_streamSources, 	 deviceState.m_streamSources);
	CaptureFromMap(m_freqs, 			 deviceState.m_freqs);
	CaptureFromMap(m_clipPlanes, 		 deviceState.m_clipPlanes);

	CaptureFromMap(m_lights, 			 deviceState.m_lights);
	CaptureFromMap(m_lightEnables, 		 deviceState.m_lightEnables);

	m_material.Capture(deviceState.m_material);
	m_fvf.Capture(deviceState.m_fvf);
	m_pVDecl.Capture(deviceState.m_pVDecl);
	m_pVShader.Capture(deviceState.m_pVShader);
	m_pPShader.Capture(deviceState.m_pPShader);
	m_pIndeces.Capture(deviceState.m_pIndeces);
	m_viewport.Capture(deviceState.m_viewport);
	m_scissorRect.Capture(deviceState.m_scissorRect);
	m_nPatchMode.Capture(deviceState.m_nPatchMode);
	m_palette.Capture(deviceState.m_palette);

#define Capture_SetXxShaderConstantY(x,y,type) \
	if (m_##x##shaderConst##y.size() > 0)\
	{\
		for(size_t i = 0;i < deviceState.m_##x##shaderConst##y.size();i++) \
		{ \
			UINT StartRegister; \
			const type *pConstantData; \
			UINT count; \
			deviceState.m_##x##shaderConst##y.get(i,StartRegister,pConstantData,count); \
			m_##x##shaderConst##y.add(StartRegister,pConstantData,count); \
		} \
	}
	Capture_SetXxShaderConstantY(p,F,float);
	Capture_SetXxShaderConstantY(p,I,int);
	Capture_SetXxShaderConstantY(p,B,BOOL);
	Capture_SetXxShaderConstantY(v,F,float);
	Capture_SetXxShaderConstantY(v,I,int);
	Capture_SetXxShaderConstantY(v,B,BOOL);
#undef Capture_SetXxShaderConstantY
}

template<typename T>
void ApplyFromMap(T& dst, const T& src)
{
	for (auto it = src.begin(); it != src.end(); it++)
		dst[(*it).first] = (*it).second;
}

void CDeviceState::Apply( const CDeviceState &state )
{
	ApplyFromMap(m_renderState, 	   state.m_renderState);
	ApplyFromMap(m_textures, 		   state.m_textures);
	ApplyFromMap(m_textureStageStates, state.m_textureStageStates);
	ApplyFromMap(m_transforms, 		   state.m_transforms);
	ApplyFromMap(m_samplerStates, 	   state.m_samplerStates);
	ApplyFromMap(m_streamSources, 	   state.m_streamSources);
	ApplyFromMap(m_freqs, 			   state.m_freqs);
	ApplyFromMap(m_clipPlanes, 		   state.m_clipPlanes);

	ApplyFromMap(m_lights, 			   state.m_lights);
	ApplyFromMap(m_lightEnables, 	   state.m_lightEnables);
	
	m_material.Apply(state.m_material);
	m_fvf.Apply(state.m_fvf);
	m_pVDecl.Apply(state.m_pVDecl);
	m_pVShader.Apply(state.m_pVShader);
	m_pPShader.Apply(state.m_pPShader);
	m_pIndeces.Apply(state.m_pIndeces);
	m_viewport.Apply(state.m_viewport);
	m_scissorRect.Apply(state.m_scissorRect);
	m_nPatchMode.Apply(state.m_nPatchMode);
	m_palette.Apply(state.m_palette);
	
#define Apply_SetXxShaderConstantY(x,y,type) \
	for(size_t i = 0;i < state.m_##x##shaderConst##y.size();i++) \
	{ \
		UINT StartRegister; \
		const type *pConstantData; \
		UINT count; \
		state.m_##x##shaderConst##y.get(i,StartRegister,pConstantData,count); \
		m_##x##shaderConst##y.add(StartRegister,pConstantData,count); \
	} 
	Apply_SetXxShaderConstantY(p,F,float);
	Apply_SetXxShaderConstantY(p,I,int);
	Apply_SetXxShaderConstantY(p,B,BOOL);
	Apply_SetXxShaderConstantY(v,F,float);
	Apply_SetXxShaderConstantY(v,I,int);
	Apply_SetXxShaderConstantY(v,B,BOOL);
#undef Apply_SetXxShaderConstantY
}
#endif // FINAL_RELEASE
