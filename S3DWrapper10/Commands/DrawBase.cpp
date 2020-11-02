#include "stdafx.h"
#include "DrawBase.h"
#include "D3DDeviceWrapper.h"
#include "..\CommonUtils\System.h"

namespace Commands
{
	typedef std::map<int,void *> ResoucesMap;

	BOOL IsUpdateRequired(ResoucesMap& resourcesMap,int subres,void *newData,UINT size)
	{
		auto it = resourcesMap.find(subres);
		if (it != resourcesMap.end())
		{
			if (memcmp(it->second,newData,size) == 0)
			{
				return FALSE;
			}
		}

		resourcesMap[subres] = newData;
		return TRUE;
	}

	void DrawBase::BuildCommand(CDumpState *ds)
	{
		std::map<int,BOOL> already;
		static std::map<int, ResoucesMap> resourceLastUpdate;

		for(unsigned i0 = 0;i0 < databin_poses.size();i0++)
		{
			UINT eid = ds->GetHandleEventId(ResourcesSrc[i0]);
			if (already.find(eid) != already.end()) continue;
			already[eid] = 1;

			if (ds->IsUsedAsRenderTarget(ResourcesSrc[i0]))
			{
				//output("// buffer_%d has been used as a render target; skipped",eid);
				continue;
			}

			if (eid < 0) __debugbreak(); 
			D3D10DDIARG_CREATERESOURCE *pcres = ds->GetCreateResourceStruct(ResourcesSrc[i0]);
			if (!pcres) continue;
			D3D10DDIARG_CREATERESOURCE cres = *pcres;

			BOOL bImmutable = cres.Usage == D3D10_USAGE_IMMUTABLE;
			BOOL bDepthStensil = !!(cres.BindFlags & D3D10_BIND_DEPTH_STENCIL);
			BOOL bMultiSample = cres.SampleDesc.Count != 1;
			BOOL bConstBuf = !!(cres.BindFlags & D3D10_BIND_CONSTANT_BUFFER);
			BOOL bDynOrStage = cres.Usage == D3D10_USAGE_DYNAMIC || cres.Usage == D3D10_USAGE_STAGING;
			BOOL bCpuWritable = bDynOrStage;
		
			BOOL bCanUseMap = bCpuWritable && bDynOrStage;
			BOOL bCanUseUpdateSubres = !bImmutable && !bDepthStensil && !bMultiSample;

			if (databin_poses[i0].size() != cres.MipLevels * cres.ArraySize) __debugbreak();

			ResoucesMap& resourcesMap = resourceLastUpdate[eid];

			if (cres.Format != DXGI_FORMAT_UNKNOWN)
			{
				BOOL bUpdateRequired = FALSE;
				for(unsigned j = 0;j < databin_poses[i0].size();j++)
				{
					DWORD len;
					void *data = ds->LoadFromBinaryFileEx(databin_poses[i0][j],&len);
					bUpdateRequired |= IsUpdateRequired(resourcesMap,j,data,len);
				}
				if (!bUpdateRequired) continue;

				int n = cres.ResourceDimension == D3D10DDIRESOURCE_TEXTURE3D ? 3 : 2;
				output(decl,"extern DXTEXTURE%dD *texture_%d_%d;",n,EventID_,eid);
				output(glob,"DXTEXTURE%dD *texture_%d_%d;",n,EventID_,eid);

				std::vector<UINT64> poses;
				for(unsigned j = 0;j < databin_poses[i0].size();j++)
				{
					poses.push_back(ds->ResaveBinaryDataEx(databin_poses[i0][j]));
				}

				static std::map<std::vector<UINT64>,std::pair<int,int> > buffersCache;
				auto &it = buffersCache.find(poses);
				if (it != buffersCache.end())
				{
					output(draw,"ctx->CopyResource(buffer_%u,texture_%d_%d);",eid,it->second.first,it->second.second);
				}
				else
				{
					buffersCache[poses] = std::make_pair(EventID_,eid);

					output_(init,"const int poses_%d_%d[] = { ",EventID_,eid);
					for(unsigned j = 0;j < databin_poses[i0].size();j++)
					{
						output_(init,"%d, ",ds->ResaveBinaryDataEx(databin_poses[i0][j]));
					}
					output(init,"};");

					output(init,"texture_%d_%d = LoadTexture%dD(device,%u,%u,%u,%u,%s,poses_%d_%d);",
						EventID_,eid,n,
						cres.pMipInfoList->TexelWidth,cres.pMipInfoList->TexelHeight,
						cres.ArraySize * cres.pMipInfoList->TexelDepth,cres.MipLevels,
						EnumToString(cres.Format),EventID_,eid);

					output(draw,"ctx->CopyResource(buffer_%u,texture_%d_%d);",eid,EventID_,eid);
				}
			}		
			else if (bCanUseMap)
			{
				for(unsigned i = 0;i < cres.ArraySize;i++)
				{
					for(unsigned mip = 0;mip < cres.MipLevels;mip++)
					{
						unsigned k = mip + i * cres.MipLevels;

						DWORD len;
						void *data = ds->LoadFromBinaryFileEx(databin_poses[i0][k],&len);
						if (!IsUpdateRequired(resourcesMap,k,data,len)) continue;

						if (cres.ResourceDimension == D3D10DDIRESOURCE_BUFFER)
						{
							output("MapBuffer(buffer_%d,%d,ctx);",eid,ds->ResaveBinaryDataEx(databin_poses[i0][k], TRUE));
							/*int pos = (int)ResaveBinaryDataEx3(databin_poses[i0][k]);
						
							static std::map<UINT64,std::pair<int,int> > cache;
							auto &it = cache.find(pos);
							if (it != cache.end())
							{
								output(draw,"device->CopyResource(buffer_%u,buf_%d_%d);",eid,it->second.first,it->second.second);
							}
							else
							{
								cache[pos] = std::make_pair(EventID_,eid);
							.
								output(glob,"ID3D10Buffer *buf_%d_%d;",EventID_,eid);
								output(init,"buf_%d_%d = LoadBuffer(device,%d,%d);",EventID_,eid,pos,cres.BindFlags);
								output(draw,"device->CopyResource(buffer_%d,buf_%d_%d);",eid,EventID_,eid);
							}*/
						}
						else if (cres.ResourceDimension == D3D10DDIRESOURCE_TEXTURE2D)
						{
							output("MapTexture2D(buffer_%d,%d,%d);",eid,k,ds->ResaveBinaryDataEx(databin_poses[i0][k], TRUE));
						}
						else if (cres.ResourceDimension == D3D10DDIRESOURCE_TEXTURE1D)
						{
							output("MapTexture1D(buffer_%d,%d,%d);",eid,k,ds->ResaveBinaryDataEx(databin_poses[i0][k], TRUE));
						}
						else
						{
							__debugbreak(); // not implemented
						}
					}
				}
			}
			else if (bCanUseUpdateSubres)
			{
				for(unsigned i = 0;i < cres.ArraySize;i++)
				{
					for(unsigned mip = 0;mip < cres.MipLevels;mip++)
					{
						unsigned k = mip + i * cres.MipLevels;

						DWORD len;
						void *data = ds->LoadFromBinaryFileEx(databin_poses[i0][k],&len);
						if (!IsUpdateRequired(resourcesMap,k,data,len)) continue;

						if (mip != 0) __debugbreak();

						if (bConstBuf)
						{
							output("ctx->UpdateSubresource(buffer_%d,%u,0,GetFromFile2(%d),%d,%d);",
								eid,k,ds->ResaveBinaryDataEx(databin_poses[i0][k], TRUE),cres.pMipInfoList->PhysicalWidth,
								cres.pMipInfoList->PhysicalWidth * cres.pMipInfoList->PhysicalHeight);
						}
						else
						{
							/*static bool first = true;
							if (first)
							{
								output("D3D10_BOX box_ = { 0,0,0,%u,1,1 };",len);
								/*output("box.front = %u;",0);
								output("box.left = %u;",0);
								output("box.top = %u;",0);
								output("box.bottom = %u;",1);
								output("box.back = %u;",1);* /
								first = false;
							}
							else
							{
								output("box_.right = %u;",len);
							}*/

							output(decl,"extern DXBOX box_%d_%d;",EventID_,eid);
							output(glob,"DXBOX box_%d_%d = { 0,0,0,%u,1,1 };",EventID_,eid,len);

							output("ctx->UpdateSubresource(buffer_%d,%u,&box_%d_%d,GetFromFile2(%d),%d,%d);",
								eid,k,EventID_,eid,ds->ResaveBinaryDataEx(databin_poses[i0][k], TRUE),cres.pMipInfoList->PhysicalWidth,
								cres.pMipInfoList->PhysicalWidth * cres.pMipInfoList->PhysicalHeight);
						}
					}
				}
			}
			else
			{
	//			output("// don't know how to update buffer_%d",eid);
			}
		}
	}

	void DrawBase::OnWrite(D3DDeviceWrapper *pWrapper) const
	{
#ifndef FINAL_RELEASE
		if ( pWrapper->m_DumpType != dtCommands )
			return;

		Bd().GetLastResourcesArray(pWrapper,ResourcesSrc);
		//DEBUG_TRACE1(_T("--> DrawCommand: Let's store %d resources\n"),ResourcesSrc.size());

		databin_poses.resize(ResourcesSrc.size());
		for(unsigned j = 0;j < ResourcesSrc.size();j++)
		{
			//DEBUG_TRACE1(_T("--> DrawCommand: Storing resource %d: 0x%08x\n"),j,ResourcesSrc[j].pDrvPrivate);
			databin_poses[j] = Bd().SaveResourceToBinaryFileEx(pWrapper,ResourcesSrc[j]);
		}

		///////

		WriteStreamer::Value( "COUNT", (UINT)databin_poses.size() );
		for(unsigned i = 0;i < databin_poses.size();i++)
		{
			WriteStreamer::Reference( "hResourceSrc", ResourcesSrc[i] );

			std::vector<int> poses = databin_poses[i];
			WriteStreamer::Value( "count", (UINT)poses.size() );
			for(unsigned k = 0;k < poses.size();k++)
			{
				WriteStreamer::Value( "data.bin", poses[k] );
			}
		}
#endif // FINAL_RELEASE
	}

	void DrawBase::OnRead()
	{
		UINT count;
		ReadStreamer::Value( count );

		databin_poses.resize(count);
		ResourcesSrc.resize(count);

		for(unsigned i0 = 0;i0 < count;i0++)
		{
			ReadStreamer::Reference( ResourcesSrc[i0] );
			
			UINT cnt;
			ReadStreamer::Value( cnt );

			databin_poses[i0].resize(cnt);
			for(unsigned k = 0;k < cnt;k++)
			{
				int databin_pos;
				ReadStreamer::Value( databin_pos );
				databin_poses[i0][k] = databin_pos;
			}
		}
	}
}
