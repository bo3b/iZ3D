#include "stdafx.h"
#include "CreateResourceBase.h"

#include "Utils.h"

UINT GetSubresourceSize(const D3D10DDIARG_CREATERESOURCE &cres,UINT mipLevel,int pitch = 0);

namespace Commands
{
	void CreateResourceBase::OnRead() const
	{
		if (!CreateResource10_.pInitialDataUP) CreateResource10_.pInitialDataUP = new D3D10_DDIARG_SUBRESOURCE_UP;
		((D3D10_DDIARG_SUBRESOURCE_UP *)CreateResource10_.pInitialDataUP)->pSysMem = 0;

		databin_poses.resize(CreateResource10_.MipLevels * CreateResource10_.ArraySize);

		int empty = 0;
		for(unsigned i = 0;i < CreateResource10_.MipLevels * CreateResource10_.ArraySize;i++)
		{
			if (!empty)
			{
				ReadStreamer::Value( databin_pos );
				if (databin_pos < 0) empty = 1;
			}

			if (databin_pos >= 0)
				((D3D10_DDIARG_SUBRESOURCE_UP *)CreateResource10_.pInitialDataUP)[i].pSysMem = CDumpState::LoadFromBinaryFile(databin_pos);
			databin_poses[i] = databin_pos;
		}
	}

	void CreateResourceBase::OnWrite() const
	{
		int pos = -1;
		if (CreateResource10_.pInitialDataUP)
		{
			for(unsigned j = 0;j < CreateResource10_.ArraySize;j++)
			{
				for(unsigned i = 0;i < CreateResource10_.MipLevels;i++)
				{
					int index = i + j * CreateResource10_.MipLevels;
					pos = Bd().SaveToBinaryFile(
						CreateResource10_.pInitialDataUP[index].pSysMem,
						GetSubresourceSize(CreateResource10_,i),
						CreateResource10_.pInitialDataUP[index].SysMemPitch,
						CreateResource10_.pInitialDataUP[index].SysMemSlicePitch);
					_ASSERT(pos >= 0);
					WriteStreamer::Value( "data.bin", pos );
				}
			}
		}
		else
		{
			WriteStreamer::Value( "data.bin", pos );
		}
	}

	void CreateResourceBase::BuildCommand(CDumpState *ds)
	{
		//int isMainTex = (CreateResource10_.BindFlags & 0xf0) == 0xa0; // 160==0xa0, 168==0xa8
		BOOL isMainTex = !!(CreateResource10_.BindFlags & D3D10_DDI_BIND_PRESENT);
		ds->SetUsedAsRenderTarget(hResource_,isMainTex);

		if (isMainTex)
		{
			output("// main texture");
			output("static DXTEXTURE2D *buffer_%d = pBackBuffer;",EventID_);
			ds->SetMainTextureEventId(EventID_);

			ds->SetMainTextureFormat(CreateResource10_.Format);
			ds->SetMainTextureWidth(CreateResource10_.pMipInfoList->PhysicalWidth);
			ds->SetMainTextureHeight(CreateResource10_.pMipInfoList->PhysicalHeight);

			output("// lock: buffer_%d",EventID_);
		}

		set_output_target(init);
		output("// ResourceDimension: %s",EnumToString(CreateResource10_.ResourceDimension));

		if (CreateResource10_.ResourceDimension == D3D10DDIRESOURCE_BUFFER)
		{
			output("bd.Usage = (DXUSAGE)%d;",CreateResource10_.Usage);
			output("bd.ByteWidth = %d;",CreateResource10_.pMipInfoList->PhysicalWidth);
			UINT bindFlag = CreateResource10_.BindFlags;
			if (bindFlag & D3D11_DDI_BIND_UNORDERED_ACCESS)
			{
				bindFlag &= ~D3D11_DDI_BIND_UNORDERED_ACCESS;
				bindFlag |= D3D11_BIND_UNORDERED_ACCESS;
			}
			output("bd.BindFlags = 0x%x;", bindFlag);

			output("bd.CPUAccessFlags = %s;",
				CreateResource10_.Usage == D3D10_USAGE_STAGING ? "D3D10_CPU_ACCESS_WRITE | D3D10_CPU_ACCESS_READ" : 
				(CreateResource10_.Usage == D3D10_USAGE_DYNAMIC ? "D3D10_CPU_ACCESS_WRITE" : "0"));

			output("bd.MiscFlags = %d;",CreateResource10_.MiscFlags);
			
			if (ds->GetDxMode() >= CDumpState::Dx11)
				output("bd.StructureByteStride = %d;",CreateResource11_.ByteStride);

			output(glob,"DXBUFFER *buffer_%d;",EventID_);
			if (databin_pos >= 0)
			{
				output("init_data.pSysMem = GetFromFile2(%d);",ds->ResaveBinaryData(databin_pos));
				output("V_(device->CreateBuffer(&bd,&init_data,&buffer_%d));",EventID_,EventID_);
			}
			else
			{
				output("V_(device->CreateBuffer(&bd,0,&buffer_%d));",EventID_,EventID_);
			}
		}
		else if (!isMainTex)
		{
			int nD = 2;
			char *td = "td";

			if (CreateResource10_.ResourceDimension == D3D10DDIRESOURCE_TEXTURECUBE)
			{
				CreateResource10_.MiscFlags |= D3D10_RESOURCE_MISC_TEXTURECUBE;
			}
			else if (CreateResource10_.ResourceDimension == D3D10DDIRESOURCE_TEXTURE3D)
			{
				nD = 3;
				td = "t3d";
			}
			else if (CreateResource10_.ResourceDimension == D3D10DDIRESOURCE_TEXTURE1D)
			{
				nD = 1;
				td = "t1d";
			}

			output("%s.Width = %d;",td,CreateResource10_.pMipInfoList[0].TexelWidth);
			if (nD > 1) output("%s.Height = %d;",td,CreateResource10_.pMipInfoList[0].TexelHeight);
			output("%s.MipLevels = %d;",td,CreateResource10_.MipLevels);
			if (nD < 3)
				output("%s.ArraySize = %d;",td,CreateResource10_.ArraySize);
			else
				output("%s.Depth = %d;",td,CreateResource10_.pMipInfoList[0].TexelDepth);
			output("%s.Format = %s;",td,EnumToString(CreateResource10_.Format));
			if (nD == 2)
			{
				output("%s.SampleDesc.Count = %d;",td,CreateResource10_.SampleDesc.Count);
				output("%s.SampleDesc.Quality = %d;",td,CreateResource10_.SampleDesc.Quality);

				output("V_(device->CheckMultisampleQualityLevels(td.Format,td.SampleDesc.Count,&levcount));");
				output("if (td.SampleDesc.Quality >= levcount) td.SampleDesc.Quality = levcount - 1;");
			}
			output("%s.Usage = (DXUSAGE)%d;",td,CreateResource10_.Usage);
			output("%s.BindFlags = 0x%x;",td,CreateResource10_.BindFlags);
			output("%s.CPUAccessFlags = %s;",td,
				CreateResource10_.Usage == D3D10_USAGE_STAGING ? "D3D10_CPU_ACCESS_WRITE | D3D10_CPU_ACCESS_READ" : 
				(CreateResource10_.Usage == D3D10_USAGE_DYNAMIC ? "D3D10_CPU_ACCESS_WRITE" : "0"));
			output("%s.MiscFlags = %d;",td,CreateResource10_.MiscFlags);

			output(decl,"extern DXTEXTURE%dD *buffer_%d;",nD,EventID_);
			output(glob,"DXTEXTURE%dD *buffer_%d;",nD,EventID_);
			if (databin_pos >= 0)
			{
				output(decl,"extern DXSUBRESDATA sd_%d[%d];",EventID_,CreateResource10_.MipLevels * CreateResource10_.ArraySize);
				output(glob,"DXSUBRESDATA sd_%d[%d];",EventID_,CreateResource10_.MipLevels * CreateResource10_.ArraySize);

				for(unsigned j = 0;j < CreateResource10_.ArraySize;j++)
				{
					for(unsigned i = 0;i < CreateResource10_.MipLevels;i++)
					{
						int i0 = i + j * CreateResource10_.MipLevels;

						int size = GetSubresourceSize(CreateResource10_,i);

						output("sd_%d[%d].pSysMem = GetFromFile2(%d);",EventID_,i0,ds->ResaveBinaryData(databin_poses[i]),i0);
						output("sd_%d[%d].SysMemPitch = %u;",EventID_,i0,CreateResource10_.pInitialDataUP[i].SysMemPitch);
						output("sd_%d[%d].SysMemSlicePitch = %u;",EventID_,i0,CreateResource10_.pInitialDataUP[i].SysMemSlicePitch);
					}
				}

				output("V_(device->CreateTexture%dD(&%s,sd_%d,&buffer_%d));",nD,td,EventID_,EventID_);
			}
			else
			{
				output("V_(device->CreateTexture%dD(&%s,0,&buffer_%d));",nD,td,EventID_,EventID_);
			}	
		}
		else
		{
			output("// skip_%d",EventID_);
		}

		if (hResource_.pDrvPrivate) ds->AddHandleEventId(hResource_,EventID_);
		ds->SaveCreateResourceStruct(hResource_,CreateResource10_);
	}
}


#define DIVISOR1 (IsCompressedFormat(cres.Format) ? 4 : 1)
#define DIVISOR2 (IsCompressedFormat(cres.Format) ? 16 : 1)

UINT GetSubresourceSize(const D3D10DDIARG_CREATERESOURCE &cres,UINT mipLevel,int pitch)
{
	UINT elementSize;
	switch (cres.ResourceDimension)
	{
	case D3D10DDIRESOURCE_BUFFER:
		return cres.pMipInfoList[mipLevel].PhysicalWidth;

	case D3D10DDIRESOURCE_TEXTURE1D:
		elementSize = GetBitWidthOfDXGIFormat(cres.Format) / 8;
		return 
			elementSize * 
			cres.pMipInfoList[mipLevel].PhysicalWidth / DIVISOR2;

	case D3D10DDIRESOURCE_TEXTURE2D:
	case D3D10DDIRESOURCE_TEXTURECUBE:
		if (cres.pInitialDataUP && cres.pInitialDataUP->pSysMem)
		{
			return 
				cres.pInitialDataUP[mipLevel].SysMemPitch * 
				cres.pMipInfoList[mipLevel].PhysicalHeight / DIVISOR1;
		}
		else if (pitch > 0)
		{
			return 
				pitch * 
				cres.pMipInfoList[mipLevel].PhysicalHeight / DIVISOR1;
		}
		else 
		{
			elementSize = GetBitWidthOfDXGIFormat(cres.Format) / 8;
			return 
				elementSize * 
				cres.pMipInfoList[mipLevel].PhysicalWidth * 
				cres.pMipInfoList[mipLevel].PhysicalHeight / DIVISOR2;
		}

	case D3D10DDIRESOURCE_TEXTURE3D:
		if (cres.pInitialDataUP && cres.pInitialDataUP->pSysMem)
		{
			return 
				cres.pInitialDataUP[mipLevel].SysMemSlicePitch * 
				cres.pMipInfoList[mipLevel].PhysicalDepth / DIVISOR1;
		}
		else if (pitch > 0)
		{
			return 
				pitch * 
				cres.pMipInfoList[mipLevel].PhysicalDepth / DIVISOR1;
		}
		else
		{
			elementSize = GetBitWidthOfDXGIFormat(cres.Format) / 8;
			return 
				elementSize * 
				cres.pMipInfoList[mipLevel].PhysicalWidth * 
				cres.pMipInfoList[mipLevel].PhysicalHeight  * 
				cres.pMipInfoList[mipLevel].PhysicalDepth / DIVISOR2;
		}

	default:
		__debugbreak();
		return 0;
	}
}
