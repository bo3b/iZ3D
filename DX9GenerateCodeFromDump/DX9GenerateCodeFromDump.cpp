// DX9GenerateCodeFromDump.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "DX9GenerateCodeFromDump.h"
#include "resource.h"
#include <d3d9.h>
#include <fstream>
#include "../CommonUtils/System.h"
#include <time.h>

#include <LlamaXML/FileInputStream.h>
#include <LlamaXML/XMLReader.h>

using namespace LlamaXML;

#include <string>
#include <map>
#include <algorithm>
#include <set>

std::map<std::pair<std::string,std::string>,CommandHandler> HandlersMap;
std::map<std::string,unsigned> PtrToEidMap;
std::map<unsigned,unsigned> UsedMap;
std::map<std::pair<unsigned,int>,DWORD> LastTextureData;
std::map<unsigned,unsigned> TextureOfSurface;
std::map<unsigned,databin_poses> LastTextureData2;
std::set<unsigned> RTSurfaces;

namespace sprintf_ {
std::string sprintf(const char *format,...)
{
	const unsigned bufsize = 10000;
	static char buffer[bufsize];
	va_list args;
	va_start(args,format);
	vsprintf_s(buffer,bufsize,format,args);
	va_end(args);
	return std::string(buffer);
}
} using namespace sprintf_;

#define FOURCC_DF16 ((D3DFORMAT) MAKEFOURCC( 'D', 'F', '1', '6' ))
#define FOURCC_DF24 ((D3DFORMAT) MAKEFOURCC( 'D', 'F', '2', '4' ))
#define FOURCC_RAWZ ((D3DFORMAT) MAKEFOURCC( 'R', 'A', 'W', 'Z' ))
#define FOURCC_INTZ ((D3DFORMAT) MAKEFOURCC( 'I', 'N', 'T', 'Z' ))
#define FOURCC_NVCS ((D3DFORMAT) MAKEFOURCC( 'N', 'V', 'C', 'S' ))
#define FOURCC_ATI1 ((D3DFORMAT) MAKEFOURCC( 'A', 'T', 'I', '1' ))
#define FOURCC_ATI2 ((D3DFORMAT) MAKEFOURCC( 'A', 'T', 'I', '2' ))
#define FOURCC_AI44 ((D3DFORMAT) MAKEFOURCC( 'A', 'I', '4', '4' ))
#define FOURCC_IA44 ((D3DFORMAT) MAKEFOURCC( 'I', 'A', '4', '4' ))
#define FOURCC_NULL ((D3DFORMAT) MAKEFOURCC( 'N', 'U', 'L', 'L' ))
#define FOURCC_YV12 ((D3DFORMAT) MAKEFOURCC( 'Y', 'V', '1', '2' ))
#define FOURCC_RESZ ((D3DFORMAT) MAKEFOURCC( 'R', 'E', 'S', 'Z' ))
#define FOURCC_ATOC ((D3DFORMAT) MAKEFOURCC( 'A', 'T', 'O', 'C' ))
#define FOURCC_SSAA ((D3DFORMAT) MAKEFOURCC( 'S', 'S', 'A', 'A' ))
#define FOURCC_NVDB ((D3DFORMAT) MAKEFOURCC( 'N', 'V', 'D', 'B' ))
#define FOURCC_R2VB ((D3DFORMAT) MAKEFOURCC( 'R', '2', 'V', 'B' ))
#define FOURCC_INST ((D3DFORMAT) MAKEFOURCC( 'I', 'N', 'S', 'T' ))

std::map<std::string,unsigned> eidMap;
void SaveEid(unsigned eid,std::string value) { eidMap[value] = eid; }

unsigned GetEid(std::string value) 
{
	auto it = eidMap.find(value);
	if (it != eidMap.end())
		return it->second;
//	__debugbreak(); // missed resource creation
	return 0;
}

CMD_HANDLER_CREATE(CreateVertexBuffer,pVertexBuffer,IDirect3DVertexBuffer9,vbuf)
CMD_HANDLER_CREATE(CreateTexture,pTexture,IDirect3DTexture9,texture)
CMD_HANDLER_CREATE(CreateVertexDeclaration,pDecl,IDirect3DVertexDeclaration9,vdecl)
CMD_HANDLER_CREATE(CreateIndexBuffer,pIndexBuffer,IDirect3DIndexBuffer9,ibuf)
CMD_HANDLER_CREATE(CreateVertexShader,pShader,IDirect3DVertexShader9,vshader)
CMD_HANDLER_CREATE(CreatePixelShader,pShader,IDirect3DPixelShader9,pshader)
CMD_HANDLER_CREATE(CreateStateBlock,pSB,IDirect3DStateBlock9,sblock)
CMD_HANDLER_CREATE(EndStateBlock,pSB,IDirect3DStateBlock9,sblock)
CMD_HANDLER_CREATE(CreateCubeTexture,pCubeTexture,IDirect3DCubeTexture9,texture)
CMD_HANDLER_CREATE(CreateDepthStencilSurface,pSurface,IDirect3DSurface9,surf)
CMD_HANDLER_CREATE(CreateRenderTarget,pSurface,IDirect3DSurface9,surf);
CMD_HANDLER_CREATE(GetSurfaceLevel,pSurface,IDirect3DSurface9,surf)
CMD_HANDLER_CREATE(GetCubeMapSurface,pSurface,IDirect3DSurface9,surf)
CMD_HANDLER_CREATE_EX(GetRenderTarget,pRenderTarget,IDirect3DSurface9,surf,draw,"if (bFirst) ")
CMD_HANDLER_CREATE_EX(GetDepthStencilSurface,pZStencilSurface,IDirect3DSurface9,surf,draw,"if (bFirst) ")
CMD_HANDLER_CREATE(CreateOffscreenPlainSurface,pSurface,IDirect3DSurface9,surf)
CMD_HANDLER_CREATE(CreateVolumeTexture,pVolumeTexture,IDirect3DVolumeTexture9,texture)
CMD_HANDLER_CREATE(GetSwapChain,pSwapChain,IDirect3DSwapChain9,swchain)
CMD_HANDLER_CREATE(GetVolumeLevel,pVolume,IDirect3DVolume9,vol)

CMD_HANDLER_USE(SetStreamSource,pStreamData,vbuf)
CMD_HANDLER_USE(SetTexture,pTexture,texture)
//CMD_HANDLER_USE(ResourceUpdateTexture,pTexture,texture)
CMD_HANDLER_USE(ResourceUpdateTexture2,pTexture,texture)
CMD_HANDLER_USE(SetIndices,pIndexData,ibuf)
CMD_HANDLER_USE(SetVertexDeclaration,pDecl,vdecl)
CMD_HANDLER_USE(SetPixelShader,pShader,pshader)
CMD_HANDLER_USE(SetVertexShader,pShader,vshader)
CMD_HANDLER_USE(StateBlock_Apply,pBlock,sblock)
CMD_HANDLER_USE(ResourceUpdateBuffer,pIndexData,ibuf)
CMD_HANDLER_USE(ResourceUpdateBuffer,pStreamData,vbuf)
CMD_HANDLER_USE(StateBlock_Capture,pBlock,sblock)
//CMD_HANDLER_USE(ResourceUpdateCubeTexture,pTexture,texture)
CMD_HANDLER_USE(ResourceUpdateCubeTexture2,pTexture,texture)
CMD_HANDLER_USE(SetRenderTarget,pRenderTarget,surf)
CMD_HANDLER_USE(SetDepthStencilSurface,pNewZStencil,surf)
CMD_HANDLER_USE(StretchRect,pSourceSurface,surf)
CMD_HANDLER_USE(StretchRect,pDestSurface,surf)
CMD_HANDLER_USE(GetSurfaceLevel,pTexture,texture)
CMD_HANDLER_USE(GetCubeMapSurface,pTexture,texture)
CMD_HANDLER_USE(ColorFill,pSurface,surf)
CMD_HANDLER_USE(ResourceUpdateVolumeTexture,pTexture,texture)
CMD_HANDLER_USE(GetVolumeLevel,pTexture,texture)
CMD_HANDLER_USE(GetRenderTargetData,pRenderTarget,surf)
CMD_HANDLER_USE(GetRenderTargetData,pDestSurface,surf)
CMD_HANDLER_USE(UpdateTexture,pSourceTexture,texture)
CMD_HANDLER_USE(UpdateTexture,pDestinationTexture,texture)
CMD_HANDLER_USE(UpdateSurface,pSourceSurface,surf)
CMD_HANDLER_USE(UpdateSurface,pDestinationSurface,surf)

CMD_HANDLER(GetSurfaceLevel,level)
{
	int eid_tex = command->GetEid(0);
	int eid_surf = command->EventId;
	if (TextureOfSurface.find(eid_surf) != TextureOfSurface.end() && 
		TextureOfSurface[eid_surf] != eid_tex)
	{
		__debugbreak();
	}
	TextureOfSurface[eid_surf] = eid_tex;
	return "";
}

CMD_HANDLER(GetCubeMapSurface,level)
{
	int eid_tex = command->GetEid(0);
	int eid_surf = command->EventId;
	if (TextureOfSurface.find(eid_surf) != TextureOfSurface.end() && 
		TextureOfSurface[eid_surf] != eid_tex)
	{
		__debugbreak();
	}
	TextureOfSurface[eid_surf] = eid_tex;
	return "";
}

CMD_HANDLER(GetVolumeLevel,level)
{
	int eid_tex = command->GetEid(0);
	int eid_surf = command->EventId;
	if (TextureOfSurface.find(eid_surf) != TextureOfSurface.end() && 
		TextureOfSurface[eid_surf] != eid_tex)
	{
		__debugbreak();
	}
	TextureOfSurface[eid_surf] = eid_tex;
	return "";
}

BOOL IsRenderTargetTexture(int tex_eid)
{
	for(auto it = TextureOfSurface.begin();it != TextureOfSurface.end();++it)
	{
		if (it->second != tex_eid) continue;
		if (RTSurfaces.find(it->first) != RTSurfaces.end()) return TRUE;
	}
	return FALSE;
}

CMD_HANDLER_BEFORE(SetRenderTarget)
{
	if (strcmp(command->GetArg(1),"00000000") != 0)
	{
		int eid = command->GetEid(1);
		RTSurfaces.insert(eid);
	}
	return "";
}

CMD_HANDLER_AFTER(GetRenderTarget)
{
	if (strcmp(command->GetArg(1),"00000000") != 0)
	{
		int eid = command->GetEid(1);
		RTSurfaces.insert(eid);
	}
	return "";
}

CMD_HANDLER_AFTER(ResourceUpdateTexture2) // metro2033 fix
{
	int eid = command->GetEid(0);
	if (!eid) command->Disable();
	return "";
}

CMD_HANDLER_AFTER(StretchRect)
{
	int src_surf_eid = command->GetEid(0);
	int dst_surf_eid = command->GetEid(2);
	if (!src_surf_eid || !dst_surf_eid) command->Disable();
	return "";
}

CMD_HANDLER_AFTER(GetRenderTargetData)
{
	int src_surf_eid = command->GetEid(0);
	int dst_surf_eid = command->GetEid(1);
	if (!src_surf_eid || !dst_surf_eid) command->Disable();
	return "";
}

CMD_HANDLER_AFTER(SetDeviceGammaRamp)
{
	return command->Disable();
}

CMD_HANDLER_BEFORE(StretchRect)
{
	int src_surf_eid = command->GetEid(0);
	int dst_surf_eid = command->GetEid(2);

	if (src_surf_eid)
	{
		if (RTSurfaces.find(src_surf_eid) != RTSurfaces.end())
		{
			if (dst_surf_eid) RTSurfaces.insert(dst_surf_eid);
		}
		else 
		{
			if (dst_surf_eid) RTSurfaces.erase(dst_surf_eid);
			command->Skip();
		}
	}
	return "";
}

CMD_HANDLER_BEFORE(GetRenderTargetData)
{
	int src_surf_eid = command->GetEid(0);
	int dst_surf_eid = command->GetEid(1);

	if (!src_surf_eid || !dst_surf_eid) return command->Disable();

	if (src_surf_eid)
	{
		if (RTSurfaces.find(src_surf_eid) != RTSurfaces.end())
		{
			if (dst_surf_eid) RTSurfaces.insert(dst_surf_eid);
		}
		else 
		{
			if (dst_surf_eid) RTSurfaces.erase(dst_surf_eid);
			command->Skip();
		}
	}

	return "";
}

char *GetFromFile(UINT64 pos,DWORD *pLen = 0,DWORD *pitch1 = 0,DWORD *pitch2 = 0);

std::string ResourceUpdateHandler(CCommand *command,int index)
{
	DWORD len;
	UINT64 pos = _atoi64(command->GetArg(index));
	void *data = GetFromFile(pos,&len);
	DWORD crc = CalculateCRC32CFast(data,len);

	DWORD level = 0;
	if (command->ArgCount() > 2)
	{
		level = atoi(command->GetArg(1));
	}
	
	unsigned eid = command->GetEid(0);
	//if (RenderTargets.find(eid) != RenderTargets.end())
	if (IsRenderTargetTexture(eid))
	{
		command->Skip();
	}
	else
	{
		auto key = std::make_pair(eid,level);
		auto it = LastTextureData.find(key);
		if (it != LastTextureData.end() && it->second == crc)
		{
			command->Skip();
		}
		LastTextureData[key] = crc;
	}

	return "";
}
//CMD_HANDLER_ADD(ResourceUpdateTexture,databin_pos,ResourceUpdateHandler)
//CMD_HANDLER_ADD(ResourceUpdateCubeTexture,databin_pos,ResourceUpdateHandler)
CMD_HANDLER_ADD(ResourceUpdateBuffer,databin_pos,ResourceUpdateHandler)

std::string ResourceUpdateHandler2(CCommand *command,int index)
{
	unsigned eid = command->GetEid(0);
	//if (RenderTargets.find(eid) != RenderTargets.end())
	if (IsRenderTargetTexture(eid))
	{
		command->Skip();
	}
	else
	{
		databin_poses &poses = CAST(databin_poses,command->GetArg(index));

		auto it = LastTextureData2.find(eid);
		if (it != LastTextureData2.end() && it->second == poses)
		{
			command->Skip();
		}
		else
		{
			LastTextureData2[eid] = poses;

			command->output_("%d,Poses(",command->EventId);
			for(unsigned i = 0;i < poses.size();i++)
			{
				command->output_("%u,",poses[i]);
			}
			command->output_("0)");
		}
	}
	return "";
}

CMD_HANDLER_ADD(ResourceUpdateTexture2,poses,ResourceUpdateHandler2)
CMD_HANDLER_ADD(ResourceUpdateCubeTexture2,poses,ResourceUpdateHandler2)

BOOL bInScene = FALSE;
CMD_HANDLER_BEFORE(StateBlock_Capture)
{
	if (!bInScene) command->SetTarget(init);
	return "";
}

CMD_HANDLER(CreatePixelShader,function)
{
	return sprintf("(DWORD *)GetFromFile(%u)",(UINT64)command->GetArg(index));
}

CMD_HANDLER(CreateVertexShader,function)
{
	return sprintf("(DWORD *)GetFromFile(%u)",(UINT64)command->GetArg(index));
}

CMD_HANDLER(CreateVertexDeclaration,pVertexElements)
{
	return sprintf("(D3DVERTEXELEMENT9 *)GetFromFile(%u)",(UINT64)command->GetArg(index));
}

template<int N>
std::string SetXxShaderConstantXHandler(CCommand *command,int index)
{
	const char *s[] = { "float", "int", "BOOL" };
	return sprintf_::sprintf("(const %s *)GetFromFile(%u)",s[N],(UINT64)command->GetArg(index));
}
CMD_HANDLER_ADD(SetVertexShaderConstantF,constantData,SetXxShaderConstantXHandler<0>)
CMD_HANDLER_ADD(SetVertexShaderConstantI,constantData,SetXxShaderConstantXHandler<1>)
CMD_HANDLER_ADD(SetVertexShaderConstantB,constantData,SetXxShaderConstantXHandler<2>)
CMD_HANDLER_ADD(SetPixelShaderConstantF,constantData,SetXxShaderConstantXHandler<0>)
CMD_HANDLER_ADD(SetPixelShaderConstantI,constantData,SetXxShaderConstantXHandler<1>)
CMD_HANDLER_ADD(SetPixelShaderConstantB,constantData,SetXxShaderConstantXHandler<2>)

CMD_HANDLER(DrawPrimitiveUP,vertexStreamZeroData)
{
	return sprintf("GetFromFile(%u)",(UINT64)command->GetArg(index));
}

CMD_HANDLER(DrawIndexedPrimitiveUP,vertexStreamZeroData)
{
	return sprintf("GetFromFile(%u)",(UINT64)command->GetArg(index));
}

CMD_HANDLER(DrawIndexedPrimitiveUP,indexData)
{
	return sprintf("GetFromFile(%u)",(UINT64)command->GetArg(index));
}

CMD_HANDLER_BEFORE(SetViewport)
{
	command->PrepareViewport(0);
	return "";
}

CMD_HANDLER(SetViewport,pViewport)
{
	return sprintf("&vport_%d",command->EventId);
}

CMD_HANDLER_BEFORE(SetScissorRect)
{
	command->PrepareRect(0);
	return "";
}

CMD_HANDLER_BEFORE(UpdateSurface)
{
	if (strcmp(command->GetArg(1),"00000000") != 0) command->PrepareRect(1);
	if (strcmp(command->GetArg(3),"00000000") != 0)	command->PreparePoint(3);
	return "";
}

CMD_HANDLER(UpdateSurface,pSourceRect)
{
	if (strcmp(command->GetArg(index),"00000000") == 0) return "0";
	return sprintf("&rect_%d",command->EventId);
}

CMD_HANDLER(UpdateSurface,pDestPoint)
{
	if (strcmp(command->GetArg(index),"00000000") == 0) return "0";
	return sprintf("&pnt_%d",command->EventId);
}

CMD_HANDLER(SetScissorRect,pRect)
{
	return sprintf("&rect_%d",command->EventId);
}

CMD_HANDLER_BEFORE(SetTransform)
{
	command->PrepareMatrix(1);
	return "";
}

CMD_HANDLER(SetTransform,pMatrix)
{
	return sprintf("&matrix_%d",command->EventId);
}

CMD_HANDLER_BEFORE(SetMaterial)
{
	command->PrepareMaterial(0);
	return "";
}

CMD_HANDLER(SetMaterial,pMaterial)
{
	return sprintf("&material_%d",command->EventId);
}

CMD_HANDLER_BEFORE(SetLight)
{
	command->PrepareLight(1);
	return "";
}

CMD_HANDLER(SetLight,pLight)
{
	return sprintf("&light_%d",command->EventId);
}

CMD_HANDLER(SetClipPlane,plane)
{
	return sprintf("(const float *)GetFromFile(%d)",(UINT64)command->GetArg(index));
}

BOOL IsBadCreateDeviceCommand(CCommand *command)
{
	CArgument &arg = command->GetArg(1);
	D3DDEVTYPE devType = (D3DDEVTYPE)atoi(arg);
	return devType != D3DDEVTYPE_HAL;
}

CMD_HANDLER_BEFORE(CreateDevice)
{
	if (IsBadCreateDeviceCommand(command))
	{
		command->output_("//");
	}
	else
	{
		command->PrepareCreateDevice(4);
		command->SetTarget(init);
	}
	return "";
}

CMD_HANDLER(CreateDevice,hFocusWindow)
{
	return "hWnd";
}

CMD_HANDLER(CreateDevice,pPresentationParameters)
{
	return "&dpp";
}

CMD_HANDLER(CreateDevice,pReturnedDeviceInterface)
{
	return "&device";
}

CMD_HANDLER_AFTER(CreateDevice)
{
	return IsBadCreateDeviceCommand(command) ? "" :
		"\tV_(device->CreateStateBlock(D3DSBT_ALL,&defaultState));\n"
		"\tV_(defaultState->Capture());";
}

CMD_HANDLER(SetGammaRamp,pRamp)
{
	return sprintf("(CONST D3DGAMMARAMP *)GetFromFile(%d)",(UINT64)command->GetArg(index));
}

CMD_HANDLER(Clear,Count)
{
	if (atoi(command->GetArg(index)) > 1) __debugbreak(); // do we need rects array support?
	return "";
}

CMD_HANDLER(Clear,pRects)
{
	if (strcmp(command->GetArg(index),"00000000") == 0) return "0";

	command->Prepare3dRect(index);
	return sprintf("&rect_%d",command->EventId);
}

CMD_HANDLER(StretchRect,pSourceRect)
{
	if (strcmp(command->GetArg(index),"00000000") == 0) return "0";

	command->PrepareRect(index,"_src");
	return sprintf("&rect_%d_src",command->EventId); 
}

CMD_HANDLER(StretchRect,pDestRect)
{
	if (strcmp(command->GetArg(index),"00000000") == 0) return "0";

	command->PrepareRect(index,"_dst");
	return sprintf("&rect_%d_dst",command->EventId); 
}

CMD_HANDLER(CreateTexture,Format)
{
	const char *s = command->GetArg(index);
	DWORD n = atoi(s);
	switch (n)
	{
	case FOURCC_NVCS: // NVidia only
		return "D3DFMT_A16B16G16R16";
	case FOURCC_DF16: // ATI only
		return "FOURCC_INTZ";
	case FOURCC_DF24: // ATI only
		return "D3DFMT_D24S8"; // dirt2 - use FOURCC_INTZ in future
	case FOURCC_RESZ: // ATI only
		__debugbreak();
		command->output_("(D3DFORMAT)");
		return "";
	case FOURCC_NVDB: // NVidia only
		__debugbreak();
		command->output_("(D3DFORMAT)");
		return "";
	case FOURCC_RAWZ: // Old NVidia only
		__debugbreak();
		command->output_("(D3DFORMAT)");
		return "";
	default:
		command->output_("(D3DFORMAT)");
		return "";
	}
}

CMD_HANDLER(CreateTexture,Usage)
{
	const char *s = command->GetArg(index);
	int n = atoi(s);
	if (n & D3DUSAGE_DMAP) n &= ~D3DUSAGE_DMAP;
	return sprintf("0x%x",n);
}

CMD_HANDLER_CAST(CreateVertexBuffer,D3DPOOL,Pool)
CMD_HANDLER_CAST(CreateIndexBuffer,D3DPOOL,Pool)
CMD_HANDLER_CAST(CreateIndexBuffer,D3DFORMAT,Format)
CMD_HANDLER_CAST(DrawPrimitive,D3DPRIMITIVETYPE,PrimitiveType)
CMD_HANDLER_CAST(DrawPrimitiveUP,D3DPRIMITIVETYPE,PrimitiveType)
CMD_HANDLER_CAST(SetRenderState,D3DRENDERSTATETYPE,State)
CMD_HANDLER_CAST(SetTransform,D3DTRANSFORMSTATETYPE,State)
CMD_HANDLER_CAST(CreateDevice,D3DDEVTYPE,DeviceType)
CMD_HANDLER_CAST(SetTextureStageState,D3DTEXTURESTAGESTATETYPE,Type)
//CMD_HANDLER_CAST(CreateTexture,D3DFORMAT,Format)
CMD_HANDLER_CAST(CreateTexture,D3DPOOL,Pool)
CMD_HANDLER_CAST(DrawIndexedPrimitive,D3DPRIMITIVETYPE,PrimitiveType)
CMD_HANDLER_CAST(SetSamplerState,D3DSAMPLERSTATETYPE,Type)
CMD_HANDLER_CAST(CreateStateBlock,D3DSTATEBLOCKTYPE,Type)
CMD_HANDLER_CAST(CreateCubeTexture,D3DFORMAT,Format)
CMD_HANDLER_CAST(CreateCubeTexture,D3DPOOL,Pool)
//CMD_HANDLER_CAST(ResourceUpdateCubeTexture,D3DCUBEMAP_FACES,face)
CMD_HANDLER_CAST(CreateRenderTarget,D3DFORMAT,Format)
CMD_HANDLER_CAST(CreateRenderTarget,D3DMULTISAMPLE_TYPE,MultiSample)
CMD_HANDLER_CAST(CreateDepthStencilSurface,D3DFORMAT,Format)
CMD_HANDLER_CAST(CreateDepthStencilSurface,D3DMULTISAMPLE_TYPE,MultiSample)
CMD_HANDLER_CAST(GetCubeMapSurface,D3DCUBEMAP_FACES,face)
CMD_HANDLER_CAST(StretchRect,D3DTEXTUREFILTERTYPE,Filter)
CMD_HANDLER_CAST(CreateOffscreenPlainSurface,D3DPOOL,Pool)
CMD_HANDLER_CAST(CreateOffscreenPlainSurface,D3DFORMAT,Format)
CMD_HANDLER_CAST(CreateVolumeTexture,D3DPOOL,Pool)
CMD_HANDLER_CAST(CreateVolumeTexture,D3DFORMAT,Format)
CMD_HANDLER_CAST(DrawIndexedPrimitiveUP,D3DPRIMITIVETYPE,PrimitiveType)
CMD_HANDLER_CAST(DrawIndexedPrimitiveUP,D3DFORMAT,IndexDataFormat)




CMD_HANDLER_COMMENT(ValidateDevice)



CMD_HANDLER_BEFORE(BeginScene)
{
	bInScene = TRUE;
	//command->output_("//");
	return "";
}

CMD_HANDLER_BEFORE(EndScene)
{
	bInScene = FALSE;
	//command->output_("//");
	return "";
}

inline std::string replace(std::string text, std::string s, std::string d)
{
	for(size_t index=0; index=text.find(s, index), index!=std::string::npos;)
	{
		text.replace(index, s.length(), d);
		index+=d.length();
	}
	return text;
}

bool ExtractFileFromResource( const char* szOutputFile_, UINT nRes_, replacement R[] )
{	
	HMODULE hModule = GetModuleHandle(0);	
	HRSRC hRes = FindResource(hModule,MAKEINTRESOURCE(nRes_),_T("FILE"));
	if(!hRes) __debugbreak();

	DWORD dwSize = SizeofResource( hModule, hRes );
	if (!dwSize) __debugbreak();

	HGLOBAL hData = LoadResource( hModule, hRes );
	if (!hData) __debugbreak();

	LPVOID pData = LockResource( hData );
	if (!pData) __debugbreak();

	std::string sData((char *)pData,dwSize);
	if (sData.size() != dwSize) __debugbreak();

	if (R)
	{
		for(replacement *r = R;r->from && r->to;r++)
		{
			sData = replace(sData,r->from,r->to);
		}
	}

	FILE* pFile;
	fopen_s( &pFile, szOutputFile_, "wb" );
	if (!pFile )
		return false;

	fwrite( sData.c_str(), sData.size(), 1, pFile );
	fclose( pFile );

	return true;
}

std::vector<CCommand *> cmds;

std::string MergeOutputs(output_target target)
{
	// prepare
	for(unsigned i = 0;i < cmds.size();i++)
	{
		if (cmds[i]->bSkip) continue;

		std::string& c = cmds[i]->Creates;
		if (!c.empty())
		{
			if (UsedMap.find(cmds[i]->EventId) == UsedMap.end())
			{
				cmds[i]->Skip();

				if (cmds[i]->Name == "EndStateBlock")
				{
					for(int j = i - 1;j >= 0;j--)
					{
						cmds[j]->Skip();
						if (cmds[j]->Name == "BeginStateBlock") break;
					}
				}
			}
		}
	}

	std::ostringstream s;
	for(unsigned i = 0;i < cmds.size();i++)
	{
		if (cmds[i]->bSkip) continue;
		s << replace(cmds[i]->GetOutput(target),"\n","\r\n");
	}
	return s.str();
}

/*int g_nWndWidth = 1024;
int g_nWndHeight = 768;*/

int _tmain(int argc, _TCHAR* argv[])
{
	FileInputStream stream("./CommandFlow.xml");
	XMLReader reader(stream,TextEncoding::System());

	reader.ReadStartElement("DX9CommandFlow");
	std::string appName = reader.GetAttribute("appName",TextEncoding::Application());
	appName = appName.substr(0,appName.find_last_of("."));

	// read commands
	printf("Reading command flow...\n");
	unsigned lastCreateDevice = 0;
	while(reader.IsStartElement())
	{
		reader.ReadStartElement("COMMAND");
		UINT EventId = reader.GetIntAttribute("eid");
		std::string name = reader.GetAttribute("name",TextEncoding::Application());

		printf("\x0d%9d\t%-50s",EventId,name.c_str());

		CCommand *cmd = new CCommand();
		cmds.push_back(cmd);
		
		cmd->Name = name;
		cmd->EventId = EventId;

		if (cmd->Name == "CreateDevice")
			lastCreateDevice = cmds.size() - 1;

		if (!reader.IsEmptyElement())
		{
			while(reader.IsStartElement())
			{
				CArgument *arg = new CArgument;
				arg->Read(reader);
				cmd->PushArg(arg);
			}

			reader.ReadEndElement();
		}
	}
	printf("\nDone.\n");
	
	BOOL bAddSetEidCommands = TRUE;
	if ((argc >= 2 && _tcscmp(argv[1],_T("-noseteid")) == 0) ||
		(argc >= 3 && _tcscmp(argv[2],_T("-noseteid")) == 0))
	{
		bAddSetEidCommands = FALSE;
	}

	// process commands
	BOOL bInDeviceState = FALSE;
	BOOL bInStateBlock = FALSE;
	BOOL bInUpdateResources = FALSE;
	int commandIndex = 0;
	CCommand *cmd = NULL;
	printf("\nGenerating code...\n");
	float cmdsPerc = 100.0f / cmds.size();
	for(unsigned i = lastCreateDevice;i < cmds.size();i++)
	{
		cmd = cmds[i];
		printf("\x0d%9d (%.2f%%)\t%-50s",cmd->EventId,i * cmdsPerc,cmd->Name.c_str());
		if (cmd->Name == "Present") break;

		// hot fix
		//if (i > 0 && cmd->Name == "BeginStateBlock" && cmds[i - 1]->Name == "BeginStateBlock") continue;

		if (cmd->Name == "BeginDeviceState") { bInDeviceState = TRUE;  /*continue;*/ }
		if (cmd->Name == "EndDeviceState")   { bInDeviceState = FALSE; continue; }
		if (bInDeviceState) cmd->SetTarget(init);

		if (cmd->Name == "BeginStateBlock") bInStateBlock = TRUE;
		if (bInStateBlock) cmd->SetTarget(init);
		if (cmd->Name == "EndStateBlock")   bInStateBlock = FALSE;

		if (cmd->Name == "BeginDumpResources") { bInUpdateResources = TRUE;  continue; }
		if (cmd->Name == "EndDumpResources")   { bInUpdateResources = FALSE; continue; }

		/*BOOL bSeparate = FALSE;
		{
			std::string s = cmd->GetOutput(draw);
			if (std::count(s.begin(),s.end(),'\n') > 1)
			{
				cmd->output(draw,"\n\t// [%d] %s",cmd->EventId,cmd->Name.c_str());
				bSeparate = TRUE;
			}
		}*/

		std::string s = CALL_HANDLER(cmd->Name,"__before",cmd,-1);
		bool bComment = (cmd->GetOutput(draw) == "//");
		if (!s.empty())
			cmd->output("%s",s.c_str());
		/*s = CALL_HANDLER(cmd->Name,"__before2",cmd,-1);
		if (!s.empty()) cmd->output("%s",s.c_str());*/

		if (cmd->Name == "BeginStateBlock" ||
			i > 0 && i < cmds.size() - 1 && cmd->Name == cmds[i + 1]->Name && cmd->Name != cmds[i - 1]->Name)
		{
			cmd->output_("\n");
		}

		if (bAddSetEidCommands)
		{
			cmd->output("\tSetEID(%d);", cmd->EventId);
			if (bComment)
				cmd->output_("//");
		}

		const char *prefix = "device->";
		if (cmd->Name == "CreateDevice") prefix = "g_pD3D->";
		if (bInUpdateResources || 
			cmd->Name == "StateBlock_Apply" ||
			cmd->Name == "StateBlock_Capture" ||
			cmd->Name == "GetSurfaceLevel" ||
			cmd->Name == "GetCubeMapSurface" ||
			cmd->Name == "BeginDeviceState" ||
			cmd->Name == "SetGammaRamp" ||
			cmd->Name == "GetVolumeLevel") 
		{
			prefix = "::";
		}

		cmd->output_("\tV_(%s%s(",prefix,cmd->Name.c_str());

		for(unsigned i = 0;i < cmd->ArgCount();i++)
		{
			if (i > 0) cmd->output_(",");

			std::string s = CALL_HANDLER(cmd->Name,cmd->ArgName(i),cmd,i);
			if (s.empty())
			{
				const char *nstr = (const char *)cmd->GetArg(i);
				/*if (strlen(nstr) > 0)
				{
					int n = atoi(nstr);
					cmd->output_("%d",n);
				}*/
				char tmpstr[2] = { 0,0 };
				for(int pos = 0;nstr[pos];pos++)
				{
					char ch = nstr[pos];
					if (ch < '0' || ch > '9') continue;
					tmpstr[0] = ch;
					cmd->output_(tmpstr);
				}
			}
			else
			{
				cmd->output_("%s",s.c_str());
			}
		}

		cmd->output(")); // [%d]",cmd->EventId);
		
		//if (bSeparate) cmd->output_(draw,"\n");

		if (cmd->GetDefaultTarget() == draw)
		{
			if (commandIndex % 100 == 99)
			{
				cmd->output("\tSetProgress(%d);", commandIndex + 1);
			}
			commandIndex++;
		}

		std::string sAfter = CALL_HANDLER(cmd->Name,"__after",cmd,-1);
		if (!sAfter.empty()) cmd->output("%s",sAfter.c_str());

		if (cmd->Name == "EndStateBlock")
		{
			cmd->output_("\n");
		}
	}
	
	printf("\nDone.\n");

	// merge strings
	printf("\nMerging strings...\n");
	std::string sGlob = MergeOutputs(glob);
	std::string sDecl = MergeOutputs(decl);
	std::string sInit = MergeOutputs(init);
	std::string sDraw = MergeOutputs(draw);
	printf("\nDone.\n");

	printf("\nWriting files...\n");
	// FarCry2 DX9Test 2010-12-24 19.36.07.22
	time_t tt = time(0);
	tm t;
	localtime_s(&t,&tt);
	std::string sTestCase = appName + " DX9Test " + 
		sprintf("%4d-%02d-%02d %02d.%02d.%02d",
		t.tm_year + 1900,t.tm_mon + 1,t.tm_mday,t.tm_hour,t.tm_min,t.tm_sec);

	std::string maxCommandIndex = sprintf("%d",commandIndex + 1);
	replacement Replace[] = 
	{
		{ "@@APP_NAME@@",		appName.c_str() },
		{ "@@DECLARATION@@",	sDecl.c_str() },
		{ "@@GLOBALS@@",		sGlob.c_str() },
		{ "@@INITIALIZE@@",		sInit.c_str() },
		{ "@@RENDER@@",			sDraw.c_str() },
		{ "@@TESTCASE_NAME@@",	sTestCase.c_str() },
		{ "@@MaxCommandIndex@@", maxCommandIndex.c_str() },
		{ 0, 0 }
	};

	CreateDirectory(L"./src",0);
	ExtractFileFromResource("./src/project.sln",IDR_PROJECT_SLN,Replace);
	ExtractFileFromResource("./src/project.vcxproj",IDR_PROJECT_VCXPROJ,Replace);
	ExtractFileFromResource("./src/main.cpp",IDR_MAIN_CPP,Replace);
	ExtractFileFromResource("./src/stdafx.cpp",IDR_STDAFX_CPP,Replace);
	ExtractFileFromResource("./src/stdafx.h",IDR_STDAFX_H,Replace);
	ExtractFileFromResource("./src/main.h",IDR_MAIN_H,Replace);
	ExtractFileFromResource("./src/init.cpp",IDR_INIT_CPP,Replace);
	ExtractFileFromResource("./src/render.cpp",IDR_RENDER_CPP,Replace);
	
	ExtractFileFromResource("./src/IStereoAPI.h",IDR_ISTEREOAPI_H,0);
	ExtractFileFromResource("./src/IStereoAPIInternal.h",IDR_ISTEREOAPIINTERNAL_H,0);

	ExtractFileFromResource("./src/build_project.cmd",IDR_BUILD_CMD,Replace);

	printf("\nDone.\n");
	
	if (argc >= 2 && _tcscmp(argv[1],_T("-noautobuild")) == 0)
	{
		FILE* pFile = fopen(("./src/" + appName + ".name").c_str(),"wb"); // dx10 generator compatibility
		if (!pFile) return -5;
		fclose(pFile);
	}
	else
	{
		printf("\nStarting a build script...");
		ShellExecute(0,L"open",L"build_project.cmd",L"",L"src",SW_SHOWNORMAL);
	}

	return 0;
}

struct LoadedData
{
	void *data;
	DWORD len;
	DWORD *params;
	DWORD count;

	UINT size;
};

std::map<UINT64,LoadedData> LoadedDataCache;
UINT m_cacheSize = 0;
int m_newFileFormatFlag = 0;

char *GetFromFile(UINT64 pos,DWORD *pLen,DWORD *pitch1,DWORD *pitch2)
{
	_ASSERT(pos >= 0);

	if (LoadedDataCache.find(pos) != LoadedDataCache.end())
	{
		LoadedData ld = LoadedDataCache[pos];
		if (pLen) *pLen = ld.len;
		if (pitch1) *pitch1 = ld.count > 0 ? ld.params[0] : 0;
		if (pitch2) *pitch2 = ld.count > 1 ? ld.params[1] : 0;
		return (char *)ld.data;
	}

	using namespace std;

	ifstream myFile;
	myFile.open("data.bin", ios::in | ios::binary);
	if (!myFile.is_open()) __debugbreak();

	if (!m_newFileFormatFlag)
	{
		DWORD null;
		myFile.read((char *)&null,sizeof(null));
		if (null == 0)
		{
			char str[4];
			myFile.read(str,4);
			if (memcmp(str,"iZ3D",4) != 0) __debugbreak();
			myFile.read(str,4);
			if (str[0] != 0 || str[1] != 0 || str[2] != 0 || str[3] != 2) __debugbreak();
			m_newFileFormatFlag = 1;
		}
		else
		{
			m_newFileFormatFlag = -1;
		}
	}

	myFile.seekg(pos,ios::beg);
	LoadedData ld;
	if (m_newFileFormatFlag == 1)
	{
		DWORD count;
		myFile.read((char *)&count,sizeof(count));
		DWORD *params = new DWORD[count];
		myFile.read((char *)params,sizeof(DWORD) * count);

		ld.count = count;
		ld.params = params;
	}
	else
	{
		ld.count = 0;
		ld.params = 0;
	}

	DWORD len;
	myFile.read((char *)&len,sizeof(len));
	ld.len = len;

	char *data = new char[len];
	myFile.read(data,len);
	ld.data = data;

	myFile.close();

	ld.size = sizeof(ld) + sizeof(*ld.params) * ld.count + ld.len;
	m_cacheSize += ld.size;
	LoadedDataCache[pos] = ld;

	if (pLen) *pLen = ld.len;
	if (pitch1) *pitch1 = ld.count > 0 ? ld.params[0] : 0;
	if (pitch2) *pitch2 = ld.count > 1 ? ld.params[1] : 0;
	return (char *)ld.data;
}

/*
char *GetFromFile(UINT64 pos,DWORD *pLen,DWORD *pitch1,DWORD *pitch2)
{
	pitch1 = pitch1;
	pitch2 = pitch2;

	_ASSERT(pos >= 0);
	using namespace std;

	static char *fileData = 0;
	static UINT64 fileLen;
	if (!fileData)
	{
		ifstream myFile;
		myFile.open("data.bin", ios::in | ios::binary);
		if (!myFile.is_open())
		{
			myFile.open("..\\data.bin", ios::in | ios::binary);
			if (!myFile.is_open())
			{
				myFile.open("..\\..\\data.bin", ios::in | ios::binary);
				_ASSERT(myFile.is_open());
			}
		}

		UINT64 begin = myFile.tellg();
		myFile.seekg(0,ios::end);
		UINT64 end = myFile.tellg();
		myFile.seekg(0,ios::beg);

		fileLen = end - begin;
		fileData = new char[(UINT)fileLen];

		myFile.read(fileData,fileLen);
		myFile.close();

		const char buf[] = { 0,0,0,0,'i','Z','3','D',0,0,0,2 };
		_ASSERT(memcmp(fileData,buf,sizeof(buf)) == 0);
	}

	_ASSERT(pos + sizeof(DWORD) * 3 <= fileLen);
	//_ASSERT(*(DWORD *)(fileData + pos) == 2);
	pos += sizeof(DWORD);

	/*if (pitch1) *pitch1 = *(DWORD *)(fileData + pos);
	pos += sizeof(DWORD);

	if (pitch2) *pitch2 = *(DWORD *)(fileData + pos);
	pos += sizeof(DWORD);* /

	_ASSERT(pos + sizeof(DWORD) <= fileLen);
	DWORD *len = (DWORD *)(fileData + pos);
	pos += sizeof(DWORD);

	if (pLen) *pLen = *len;

	_ASSERT(pos + *len <= fileLen);
	return fileData + pos;
}*/