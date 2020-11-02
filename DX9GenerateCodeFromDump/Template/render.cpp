#include "stdafx.h"
#include "main.h"

bool IsCompressedFormat(D3DFORMAT Format)
{
	return
		Format == D3DFMT_DXT1 ||
		Format == D3DFMT_DXT2 ||
		Format == D3DFMT_DXT3 ||
		Format == D3DFMT_DXT4 ||
		Format == D3DFMT_DXT5 ||
		Format == FOURCC_ATI1 ||
		Format == FOURCC_ATI2;
}

template<class T>
HRESULT  ResourceUpdateBuffer(T *buf,UINT64 pos)
{
	HRESULT hr = S_OK;
	void *data;
	V_(buf->Lock(0,0,&data,0));
	DWORD len;
	char *src = GetFromFile(pos,&len);
	memcpy(data,src,len);
	V_(buf->Unlock());

	return S_OK;
}

#pragma warning(disable:4239)

HRESULT ResourceUpdateLockableTexture(IDirect3DTexture9* tex, Poses &poses)
{
	HRESULT hr = S_OK;
	for(DWORD level = 0;level < poses.size();level++)
	{
		D3DSURFACE_DESC desc;
		V_(tex->GetLevelDesc(level,&desc));

		D3DLOCKED_RECT rect;
		DWORD flags = (desc.Usage & D3DUSAGE_DYNAMIC) != 0 ? D3DLOCK_DISCARD : 0;
		V_(tex->LockRect(level,&rect,0,flags));

		if (IsCompressedFormat(desc.Format)) rect.Pitch /= 4;

		DWORD len;
		char *src = GetFromFile(poses[level],&len);

		DWORD dstLen = rect.Pitch * desc.Height;
		if (len == dstLen * 2) len = dstLen; // wide RT. TODO: Save only left part of surface

		if (dstLen == len)
		{
			memcpy(rect.pBits,src,len);
		}
		else
		{
			int srcPitch = len / desc.Height;
			int pitch = min(srcPitch, rect.Pitch);
			char *dst = (char *)rect.pBits;				
			for (UINT i = 0; i < desc.Height; i++)
			{
				memcpy(dst,src,pitch);
				src += srcPitch;
				dst += rect.Pitch;
			}
		}

		V_(tex->UnlockRect(level));
	}
	return hr;
}

HRESULT ResourceUpdateRenderTargetTexture(IDirect3DTexture9 *tex, unsigned eid, Poses &poses, D3DSURFACE_DESC &desc, DWORD levels)
{
	HRESULT hr = S_OK;
	static std::map<unsigned,IDirect3DTexture9 *> texs;
	auto it = texs.find(eid);

	IDirect3DTexture9 *tex2;
	if (it == texs.end())
	{
		V_(device->CreateTexture(desc.Width,desc.Height,levels,0,desc.Format,D3DPOOL_SYSTEMMEM,&tex2,0));		
		V_(ResourceUpdateLockableTexture(tex2, poses));
		texs[eid] = tex2;
	}
	else
	{
		tex2 = it->second;
	}
	V_(device->UpdateTexture(tex2,tex));
	return hr;
}

HRESULT ResourceUpdateTexture2(IDirect3DTexture9 *tex,unsigned eid,Poses &poses)
{
	HRESULT hr = S_OK;
	DWORD levels = tex->GetLevelCount();
	_ASSERT(poses.size() == levels);

	D3DSURFACE_DESC desc;
	V_(tex->GetLevelDesc(0,&desc));

	if (desc.Usage & D3DUSAGE_RENDERTARGET)	
	{
		V_(ResourceUpdateRenderTargetTexture(tex, eid, poses, desc, levels ))
	}
	else if (desc.Pool != D3DPOOL_DEFAULT || (desc.Usage & D3DUSAGE_DYNAMIC))
	{
		V_(ResourceUpdateLockableTexture(tex, poses));
	}

	return S_OK;
}

HRESULT ResourceUpdateLockableCubeTexture(IDirect3DCubeTexture9 *tex, Poses &poses)
{
	HRESULT hr = S_OK;
	for(DWORD level = 0;level < poses.size() / 6;level++)
	{
		D3DSURFACE_DESC desc;
		V_(tex->GetLevelDesc(level,&desc));

		for(int f = 0;f < 6;f++)
		{
			D3DCUBEMAP_FACES face = (D3DCUBEMAP_FACES)f;

			D3DLOCKED_RECT rect;
			V_(tex->LockRect(face,level,&rect,0,0));

			if (IsCompressedFormat(desc.Format)) rect.Pitch /= 4;

			DWORD len;
			char *src = GetFromFile(poses[level * 6 + f],&len);

			DWORD dstLen = rect.Pitch * desc.Height;

			if (dstLen == len)
			{
				memcpy(rect.pBits,src,len);
			}
			else if (dstLen * 2 == len)
			{
				memcpy(rect.pBits,src,dstLen); // wide RT. TODO: Save only left part of surface
			}
			else
			{
				int srcPitch = len / desc.Height;
				int pitch = min(srcPitch, rect.Pitch);
				char *dst = (char *)rect.pBits;				
				for (UINT i = 0; i < desc.Height; i++)
				{
					memcpy(dst,src,pitch);
					src += srcPitch;
					dst += rect.Pitch;
				}
			}

			V_(tex->UnlockRect(face,level));
		}
	}
	return hr;
}

HRESULT ResourceUpdateRenderTargetCubeTexture(IDirect3DCubeTexture9 *tex,unsigned eid,Poses &poses, D3DSURFACE_DESC &desc, DWORD levels)
{
	HRESULT hr = S_OK;
	static std::map<unsigned,IDirect3DCubeTexture9 *> texs;
	auto it = texs.find(eid);

	IDirect3DCubeTexture9 *tex2;
	if (it == texs.end())
	{
		V_(device->CreateCubeTexture(desc.Width,levels,0,desc.Format,D3DPOOL_SYSTEMMEM,&tex2,0));
		V_(ResourceUpdateLockableCubeTexture(tex2, poses));
		texs[eid] = tex2;
	}
	else
	{
		tex2 = it->second;
	}
	V_(device->UpdateTexture(tex2,tex));
	return hr;
}

HRESULT ResourceUpdateCubeTexture2(IDirect3DCubeTexture9 *tex,unsigned eid,Poses &poses)
{
	HRESULT hr = S_OK;
	DWORD levels = tex->GetLevelCount();
	_ASSERT(poses.size() == levels * 6);

	D3DSURFACE_DESC desc;
	V_(tex->GetLevelDesc(0,&desc));

	if (desc.Usage & D3DUSAGE_RENDERTARGET)	
	{
		V_(ResourceUpdateRenderTargetCubeTexture(tex, eid, poses, desc, levels))
	}
	else
	{
		V_(ResourceUpdateLockableCubeTexture(tex, poses));
	}

	return S_OK;
}

HRESULT ResourceUpdateVolumeTexture(IDirect3DVolumeTexture9 *tex,DWORD level,UINT64 pos)
{
	HRESULT hr = S_OK;
	D3DVOLUME_DESC desc;
	V_(tex->GetLevelDesc(level,&desc));

	D3DLOCKED_BOX box;
	V_(tex->LockBox(level,&box,0,0));

	if (IsCompressedFormat(desc.Format)) box.SlicePitch /= 4;

	DWORD len;
	char *src = GetFromFile(pos,&len);

	int dstLen = box.SlicePitch * desc.Depth;

	if (dstLen == len)
	{
		memcpy(box.pBits,src,len);
	}
	else
	{
		int srcSlicePitch = len / desc.Depth;
		int srcRowPitch = srcSlicePitch / desc.Height;
		int slicePitch = min(srcSlicePitch, box.SlicePitch);
		int rowPitch = min(srcRowPitch, box.RowPitch);
		for (UINT i = 0; i < desc.Depth; i++)
		{	
			char *srcSlice = src + i * srcSlicePitch;				
			char *dstSlice = (char *)box.pBits + i * box.SlicePitch;				
			for (UINT j = 0; j < desc.Height; j++)
			{
				memcpy(dstSlice,srcSlice,rowPitch);
				srcSlice += srcRowPitch;
				dstSlice += box.RowPitch;
			}
		}
	}

	V_(tex->UnlockBox(level));

	return S_OK;
}

HRESULT SetGammaRamp(UINT iSwapChain,DWORD Flags,CONST D3DGAMMARAMP* pRamp)
{
	device->SetGammaRamp(iSwapChain,Flags,pRamp);
	return S_OK;
}

VOID Render()
{
	static BOOL bFirst = TRUE;
	
	HRESULT hr = S_OK;
	//IDirect3DSurface9 *surf_0;
	//device->GetRenderTarget(0,&surf_0);

	SetEID(0);
	V_(device->Clear(0,0,D3DCLEAR_TARGET,0,1,1));
	//V_(device->Clear(0,0,D3DCLEAR_ZBUFFER,0,1,1));
	//V_(device->Clear(0,0,D3DCLEAR_STENCIL,0,1,1));

@@RENDER@@

	SetProgress(0);

	hr = device->Present(0,0,0,0);
	if(hr == D3DERR_DEVICELOST)
	{
		hr = device->TestCooperativeLevel();
	}
	V_(hr);

	bFirst = FALSE;
}
