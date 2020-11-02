#include "stdafx.h"
#include "CodeGenerator.h"
#include "Commands\CommandSet.h"
#include <string>
#include <fstream>
#include "..\commands\CreateElementLayout.h"

#define TAB		"\t"
#define TAB2	"\t\t"
#define TAB3	"\t\t\t"

#define VAR_RELEASE(var)	TAB << "if (" << var << ")" << endl			\
	<< TAB << "{" << endl						\
	<< TAB2 << var << "->Release();" << endl	\
	<< TAB2 << var << " = NULL;" << endl		\
	<< TAB << "}" << endl;

#define VAR_CHECKNULL(var)	TAB << "_ASSERT( !" << var << " );" << endl

#define VAR_SETNULL(var)	TAB << var << TAB2 << " = NULL;" << endl

using namespace std;

#define init(x) x << TAB << "// do nothing" << endl; //x.clear(); x 

void DependenciesData::SetDependenciesFromResource( void* resHnd )
{
	if (resHnd)
		depends.insert(GetLastResourceEvent(resHnd));
}

void DependenciesData::SetLastResourceEvent( void* resHnd, int eid )
{
	lastResourceUseEID[resHnd] = eid;
}

int DependenciesData::GetLastResourceEvent(void* resHnd)
{
	return lastResourceUseEID[resHnd];
}

bool DependenciesData::IsResourceUsed(void* resHnd)
{
	auto it = lastResourceUseEID.find(resHnd);
	if (it == lastResourceUseEID.end())
		return false;
	return depends.find((*it).second) != depends.end();
}

bool DependenciesData::IsResourcesUsed( const std::vector<void *>& resHndls )
{
	for(unsigned j = 0;j < resHndls.size();j++)
	{
		// skip NULL items
		if (resHndls[j] && !IsResourceUsed(resHndls[j]))
			return false;
	}
	return true;
}

CodeGenerator::CodeGenerator()
{
	init(m_varsSetNull);
	init(m_varsCheckNull);
	init(m_varsVBDecl);
	init(m_varsVBInit);
	init(m_varsVBRelease);
	init(m_varsIBDecl);
	init(m_varsIBInit);
	init(m_varsIBRelease);
	init(m_varsTextureDecl);
	init(m_varsTextureInit);
	init(m_varsTextureRelease);
	init(m_varsShaderDecl);
	init(m_varsShaderInit);
	init(m_varsShaderRelease);
}

CodeGenerator::~CodeGenerator()
{
}

void CodeGenerator::InternalGenerateHFile()
{
	m_genHeader << "#pragma once\n" << endl;
	//m_genHeader << "#ifndef _FRAME_H_" << endl;
	//m_genHeader << "#define _FRAME_H_" << endl << endl << endl;

	for (unsigned i = 0;i < m_cmds.size();i++)
	{
		const char *s0 = CommandIDToString(m_cmds[i]->CommandId);
		size_t len = strlen(s0);
		if (len < 4) continue;
		const char *s = s0 + len - 4;
		if (0 == strcmp(s, "10_1"))
		{
			ds.IncreaseDxMode(CDumpState::Dx10_1);
		}
		else if (0 == strcmp(s, "11_0"))
		{
			ds.IncreaseDxMode(CDumpState::Dx11);
			break; // max dx level
		}
	}

	//m_genDyn << "#pragma once\n" << endl;
	std::stringstream &m_genDyn = m_genHeader;

	const char *sDxDevice = "ID3D10Device";
	const char *sDxCreate = 
		"D3D10CreateDeviceAndSwapChain(adap,drtype,0,D3D10_CREATE_DEVICE_SINGLETHREADED, \\\n\t"
		"D3D10_SDK_VERSION,&swdesc,&swchain,&dev);";
	const char *sDevContext = "m_pDevice";
	const char *sDxRTView = "ID3D10RenderTargetView";
	const char *sDxTexture1D = "ID3D10Texture1D";
	const char *sDxTexture2D = "ID3D10Texture2D";
	const char *sDxTexture3D = "ID3D10Texture3D";
	const char *sDxBuffer = "ID3D10Buffer";
	const char *sDescBuffer = "D3D10_BUFFER_DESC";
	const char *sDescTex1D = "D3D10_TEXTURE1D_DESC";	
	const char *sDescTex2D = "D3D10_TEXTURE2D_DESC";	
	const char *sDescTex3D = "D3D10_TEXTURE3D_DESC";
	const char *sDxUsage = "D3D10_USAGE";
	switch(ds.GetDxMode())
	{
	case CDumpState::Dx10_1:
		sDxDevice = "ID3D10Device1";
		sDxCreate = 
			"D3D10CreateDeviceAndSwapChain1(adap,drtype,0,D3D10_CREATE_DEVICE_SINGLETHREADED, \\\n\t"
			"D3D10_FEATURE_LEVEL_10_0,D3D10_1_SDK_VERSION,&swdesc,&swchain,&dev);";
		break;
	case CDumpState::Dx11:
		sDxDevice = "ID3D11Device";
		sDxCreate = 
			"D3D11CreateDeviceAndSwapChain(0,D3D_DRIVER_TYPE_HARDWARE,0,D3D11_CREATE_DEVICE_SINGLETHREADED, \\\n\t"
			"0,0,D3D11_SDK_VERSION,&swdesc,&swchain,&dev,0,&context);";
		sDevContext = "m_pDevContext";
		sDxRTView = "ID3D11RenderTargetView";
		sDxTexture1D = "ID3D11Texture1D";
		sDxTexture2D = "ID3D11Texture2D";
		sDxTexture3D = "ID3D11Texture3D";
		sDxBuffer = "ID3D11Buffer";
		sDescBuffer = "D3D11_BUFFER_DESC";
		sDescTex1D = "D3D11_TEXTURE1D_DESC";
		sDescTex2D = "D3D11_TEXTURE2D_DESC";
		sDescTex3D = "D3D11_TEXTURE3D_DESC";
		sDxUsage = "D3D11_USAGE";
		break;
	}

	m_genDyn << "#define DXDEVICE " << sDxDevice << endl;
	m_genDyn << "#define DXCREATE(adap,drtype,swdesc,swchain,dev,context) \\\n\t" << sDxCreate << endl;
	m_genDyn << "#define DXDEVCONTEXT " << sDevContext << endl;
	m_genDyn << "#define DXRTVIEW " << sDxRTView << endl;
	m_genDyn << "#define DXTEXTURE1D " << sDxTexture1D << endl;
	m_genDyn << "#define DXTEXTURE2D " << sDxTexture2D << endl;
	m_genDyn << "#define DXTEXTURE3D " << sDxTexture3D << endl;
	m_genDyn << "#define DXBUFFER " << sDxBuffer << endl;
	m_genDyn << "#define DXDESCBUFFER " << sDescBuffer << endl;
	m_genDyn << "#define DXDESCTEX1D " << sDescTex1D << endl;
	m_genDyn << "#define DXDESCTEX2D " << sDescTex2D << endl;
	m_genDyn << "#define DXDESCTEX3D " << sDescTex3D << endl;
	m_genDyn << "#define DXUSAGE " << sDxUsage << endl;

	const char *DXs[][3] = {
		{ "DXOPT", "", ",0,0" },
		{ "DXOPT1", "", ",0" },

		{ "DXSUBRESDATA", "D3D%d_SUBRESOURCE_DATA" },		
		{ "DXTOPOLOGY", "D3D%d_PRIMITIVE_TOPOLOGY" },		
		{ "DXSAMPSTATE", "ID3D%dSamplerState" },
		{ "DXVSHADER", "ID3D%dVertexShader" },
		{ "DXGSHADER", "ID3D%dGeometryShader" },
		{ "DXPSHADER", "ID3D%dPixelShader" },
		{ "DXLAYOUT", "ID3D%dInputLayout" },
		{ "DXRASTSTATE", "ID3D%dRasterizerState" },
		{ "DXVIEWPORT", "D3D%d_VIEWPORT" },
		{ "DXBLENDSTATE1", "ID3D10BlendState1", "ID3D11BlendState" },
		{ "DXDEPTHSTATE", "ID3D%dDepthStencilState" },
		{ "DXBOX", "D3D%d_BOX" },
		{ "DXBLENDDESC1", "D3D10_BLEND_DESC1", "D3D11_BLEND_DESC" },
		{ "DXDEPTHDESC", "D3D%d_DEPTH_STENCIL_DESC" },
		{ "DXRASTDESC", "D3D%d_RASTERIZER_DESC" },
		{ "DXSAMPDESC", "D3D%d_SAMPLER_DESC" },
		{ "DXBLEND", "D3D%d_BLEND" },
		{ "DXBLENDOP", "D3D%d_BLEND_OP" },
		{ "CREATEBLENDSTATE", "CreateBlendState1", "CreateBlendState" },
		{ "DXWRITEMASK", "D3D%d_DEPTH_WRITE_MASK" },
		{ "DXCMPFUNC", "D3D%d_COMPARISON_FUNC" },
		{ "DXSTENSILOP", "D3D%d_STENCIL_OP" },
		{ "DXCULLMODE", "D3D%d_CULL_MODE" },
		{ "DXFILLMODE", "D3D%d_FILL_MODE" },
		{ "DXELEMENTDESC", "D3D%d_INPUT_ELEMENT_DESC" },
		{ "DXFILTER", "D3D%d_FILTER" },
		{ "DXTEXADDRMODE", "D3D%d_TEXTURE_ADDRESS_MODE" },
		{ "DXINPUTCLASSIF", "D3D%d_INPUT_CLASSIFICATION" },
		{ "DXSHADERRESVIEW", "ID3D%dShaderResourceView" },
		{ "DXDESCRTVIEW", "D3D%d_RENDER_TARGET_VIEW_DESC" },
		{ "DXRTVDIM", "D3D%d_RTV_DIMENSION" },
		/*
		{ "", "" },
		*/
	};
	for(int i = 0;i < sizeof(DXs)/sizeof(*DXs);i++)
	{
		m_genDyn << "#define " << DXs[i][0] << " ";
		if (DXs[i][2])
		{
			m_genDyn << (ds.GetDxMode() == CDumpState::Dx11 ? DXs[i][2] : DXs[i][1]);
		}
		else
		{
			char buf[128];
			sprintf(buf,DXs[i][1],ds.GetDxMode() == CDumpState::Dx11 ? 11 : 10);
			m_genDyn << buf;
		}
		m_genDyn << endl;
	}

	m_genHeader << endl;


	m_genHeader << "class CFrame" << endl << "{" << endl << "public:" << endl;
	m_genHeader << TAB2 << "CFrame\t\t\t();" << endl;
	m_genHeader << TAB2 << "~CFrame\t\t\t();" << endl;
	m_genHeader << TAB << "void" << TAB << "FrameInit" << TAB2 << "(DXDEVICE *device);" << endl;
	m_genHeader << TAB << "void" << TAB << "FrameRelease" << TAB2 << "();" << endl;
	m_genHeader << TAB << "void" << TAB << "FrameDraw" << TAB2 << "(DXDEVICE *device,IDXGISwapChain *swapChain,DXRTVIEW *renderTargetView,DXTEXTURE2D* pBackBuffer,ID3D11DeviceContext *context);" << endl;
	m_genHeader << "protected:" << endl;
	m_genHeader << "private:" << endl << endl;

	/*
	// VB's declaration
	m_genHeader << TAB << "// Vertex buffers decl" << endl;
	m_genHeader << m_varsVBDecl.rdbuf() << TAB << "//" << endl << endl;
	// IB's declaration
	m_genHeader << TAB << "// Index buffers decl" << endl;
	m_genHeader << m_varsIBDecl.rdbuf() << TAB << "//" << endl << endl;
	// Texture's declaration
	m_genHeader << TAB << "// Textures decl( 1D, 2D, 3D, Cube )" << endl;
	m_genHeader << m_varsTextureDecl.rdbuf() << TAB << "//" << endl << endl;
	// Shaders's declaration
	m_genHeader << TAB << "// Shaders decl" << endl;
	m_genHeader << m_varsShaderDecl.rdbuf() << TAB << "//" << endl << endl;
	*/

	m_genHeader << "};" << endl << endl;
	//genHeader << "#endif//_FRAME_H_" << endl;	
}

/*typedef std::map<Commands::CreateElementLayout *,Commands::CreateVertexShader *> ShaderLayoutMap;
ShaderLayoutMap shaderLayoutMap;
Commands::CreateVertexShader *GetShader(Commands::CreateElementLayout *layout)
{
	_ASSERT(layout);
	_ASSERT(shaderLayoutMap.find(layout) != shaderLayoutMap.end());
	return shaderLayoutMap[layout];
}*/

std::vector<std::string> resourcesToFree;

/*
// returns TRUE if the resource was already added to the free-list
BOOL HaveToFreeResource(char *name)
{
	BOOL found = std::find(resourcesToFree.begin(),resourcesToFree.end(),name) != resourcesToFree.end();
	if (!found) resourcesToFree.push_back(name);
	return found;
	}*/

BOOL IsDrawCommand(UINT CommandId)
{
	return 
		CommandId == idDraw ||
		CommandId == idDrawAuto ||
		CommandId == idDrawIndexed ||
		CommandId == idDrawIndexedInstanced ||
		CommandId == idDrawIndexedInstancedIndirect11_0 ||
		CommandId == idDrawInstanced ||
		CommandId == idDrawInstancedIndirect11_0;
}

BOOL IsDispatchCommand(UINT CommandId)
{
	return 
		CommandId == idDispatch11_0 ||
		CommandId == idDispatchIndirect11_0;
}

Commands::Command *GlobalCmd;

void CodeGenerator::InternalGenerateCppFile()
{
	m_genCpp << "#include \"pcmp.h\"" << endl;
	m_genCpp << "#include \"frame.h\"" << endl;
	m_genCpp << "#include \"global.h\"" << endl;
	m_genCpp << "#include \"frame_glob.h\"" << endl;
	m_genCpp << endl;

	m_genCppDraw << "#include \"pcmp.h\"" << endl;
	m_genCppDraw << "#include \"frame.h\"" << endl;
	m_genCppDraw << "#include \"global.h\"" << endl;
	m_genCppDraw << "#include \"frame_glob.h\"" << endl;
	m_genCppDraw << endl;

	m_genGlob << "char *GetFromFile2(int pos,DWORD *pLen = 0,DWORD *pitch1 = 0,DWORD *pitch2 = 0);" << endl;

	m_genGlob << "DXTEXTURE2D *LoadTexture2D(DXDEVICE *device," << endl;
	m_genGlob << "\tUINT width,UINT height,UINT layers,UINT mips,DXGI_FORMAT format,const int *poses);" << endl;

	m_genGlob << "DXTEXTURE3D *LoadTexture3D(DXDEVICE *device," << endl;
	m_genGlob << "\tUINT width,UINT height,UINT depth,UINT mips,DXGI_FORMAT format,const int *poses);" << endl;

	m_genGlob << "void SetProgress(int value);" << endl;

	/*m_genGlob << "void MapTexture2D(ID3D10Texture2D *buffer,int subres,int pos);" << endl;
	m_genGlob << "void MapTexture1D(ID3D10Texture1D *buffer,int subres,int pos);" << endl;*/
	m_genGlob << "void MapBuffer(ID3D10Buffer *buffer,int pos,ID3D10Device *);" << endl;
	m_genGlob << "void MapBuffer(ID3D11Buffer *buffer,int pos,ID3D11DeviceContext *ctx);" << endl;
	m_genGlob << "DXBUFFER *LoadBuffer(DXDEVICE *device,int pos,UINT bindFlags);" << endl;

	m_genGlob << "extern HRESULT hr;" << endl;
	m_genGlob << "extern DXDESCBUFFER bd;" << endl;
	m_genGlob << "extern DXSAMPDESC desc;" << endl;
	m_genGlob << "extern DXDESCTEX2D td;" << endl;
	m_genGlob << "extern DXDESCTEX3D t3d;" << endl;
	m_genGlob << "extern DXDESCTEX1D t1d;" << endl;
	m_genGlob << "extern D3D10_SHADER_RESOURCE_VIEW_DESC vdesc;" << endl;
	m_genGlob << "extern D3D10_SHADER_RESOURCE_VIEW_DESC1 vdesc1;" << endl;
	m_genGlob << "extern D3D11_SHADER_RESOURCE_VIEW_DESC vdesc11;" << endl;
	m_genGlob << "extern DXDEPTHDESC dsd;" << endl;
	m_genGlob << "extern D3D10_BLEND_DESC bld;" << endl;
	m_genGlob << "extern DXBLENDDESC1 bld1;" << endl;
	m_genGlob << "extern D3D10_DEPTH_STENCIL_VIEW_DESC dsvd;" << endl;
	m_genGlob << "extern D3D11_DEPTH_STENCIL_VIEW_DESC dsvd11;" << endl;
	m_genGlob << "extern D3D11_UNORDERED_ACCESS_VIEW_DESC uavd11;" << endl;
	m_genGlob << "extern DXDESCRTVIEW rtvd;" << endl;
	m_genGlob << "extern DXBOX box;" << endl;
	m_genGlob << "extern DXRASTDESC rasterizerState;" << endl;
	//m_genGlob << "extern D3D10_MAPPED_TEXTURE2D mt;" << endl;
	m_genGlob << "extern DXSUBRESDATA init_data;" << endl;
	m_genGlob << "extern UINT levcount;" << endl;
	m_genGlob << endl;

	m_genDecl << "HRESULT hr;" << endl;
	m_genDecl << "DXDESCBUFFER bd;" << endl;
	m_genDecl << "DXSAMPDESC desc;" << endl;
	m_genDecl << "DXDESCTEX2D td;" << endl;
	m_genDecl << "DXDESCTEX3D t3d;" << endl;
	m_genDecl << "DXDESCTEX1D t1d;" << endl;
	m_genDecl << "D3D10_SHADER_RESOURCE_VIEW_DESC vdesc;" << endl;
	m_genDecl << "D3D10_SHADER_RESOURCE_VIEW_DESC1 vdesc1;" << endl;
	m_genDecl << "D3D11_SHADER_RESOURCE_VIEW_DESC vdesc11;" << endl;
	m_genDecl << "DXDEPTHDESC dsd;" << endl;
	m_genDecl << "D3D10_BLEND_DESC bld;" << endl;
	m_genDecl << "DXBLENDDESC1 bld1;" << endl;
	m_genDecl << "D3D10_DEPTH_STENCIL_VIEW_DESC dsvd;" << endl;
	m_genDecl << "D3D11_DEPTH_STENCIL_VIEW_DESC dsvd11;" << endl;
	m_genDecl << "D3D11_UNORDERED_ACCESS_VIEW_DESC uavd11;" << endl;
	m_genDecl << "DXDESCRTVIEW rtvd;" << endl;
	m_genDecl << "DXBOX box;" << endl;
	m_genDecl << "DXRASTDESC rasterizerState;" << endl;
	//m_genDecl << "D3D10_MAPPED_TEXTURE2D mt;" << endl;
	m_genDecl << "DXSUBRESDATA init_data;" << endl;
	m_genDecl << "UINT levcount;" << endl;
	m_genDecl << endl;

	//.ctor()
	m_genCpp << endl;
	m_genCpp << "CFrame::CFrame()" << endl;		
	m_genCpp << "{" << endl;
	/*
	m_genCpp << TAB << "// Set all vars to NULL" << endl;
	m_genCpp << m_varsSetNull.rdbuf() << TAB << "//" << endl;
	*/
	m_genCpp << "}" << endl << endl;

	// .dtor
	m_genCpp << "CFrame::~CFrame()" << endl;
	m_genCpp << "{" << endl;
	/*
	m_genCpp << TAB << "// Check all vars for NULL" << endl;
	m_genCpp << m_varsCheckNull.rdbuf() << TAB << "//" << endl;
	*/
	m_genCpp << "}" << endl << endl;

	// FrameRelease()
	m_genCpp << "void CFrame::FrameRelease()" << endl;
	m_genCpp << "{" << endl;
	/*
	m_genCpp << TAB << "// Vertex buffers release" << endl;
	m_genCpp << m_varsVBRelease.rdbuf() << TAB << "//" << endl << endl;
	m_genCpp << TAB << "// Index buffers release" << endl;
	m_genCpp << m_varsIBRelease.rdbuf() << TAB << "//" << endl << endl;
	m_genCpp << TAB << "// Textures release" << endl;
	m_genCpp << m_varsTextureRelease.rdbuf() << TAB << "//" << endl << endl;
	m_genCpp << TAB << "// Shaders release" << endl;
	m_genCpp << m_varsShaderRelease.rdbuf() << TAB << "//" << endl << endl;
	*/
	m_genCpp << "}" << endl << endl;
	//

#ifndef FINAL_RELEASE
	m_genCpp << endl;
	m_genCpp << "void CFrame::FrameInit(DXDEVICE *device)" << endl;
	m_genCpp << "{" << endl;

	m_genCppDraw << "void CFrame::FrameDraw(DXDEVICE *device,IDXGISwapChain *swapChain,DXRTVIEW *renderTargetView,DXTEXTURE2D* pBackBuffer,ID3D11DeviceContext *context)" << endl;
	m_genCppDraw << "{" << endl;
	if (ds.GetDxMode() == CDumpState::Dx11)
	{
		m_genCppDraw << "ID3D11DeviceContext *ctx = context;" << endl;
	}
	else
	{
		m_genCppDraw << "DXDEVICE *ctx = device;" << endl;
	}

	// CUT command flow
	bool bSmthHasBeenDrawn = false;
	for (size_t i = 0;i < m_cmds.size();i++)
	{
		if (m_cmds[i]->CommandId == idPresent && bSmthHasBeenDrawn)
		{
			m_cmds.resize(i + 1);
			break;
		}
		if (IsDrawCommand(m_cmds[i]->CommandId) || IsDispatchCommand(m_cmds[i]->CommandId))
			bSmthHasBeenDrawn = true;
	}

	ProcessCommands();

	m_genCppDraw << "}" << endl << endl;
	m_genCpp << "}" << endl << endl;

	m_genCpp << "DXGI_FORMAT mainFormat = " << EnumToString(ds.GetMainTextureFormat()) << ";" << endl;
	m_genCpp << "int g_nWndWidth = " << ds.GetMainTextureWidth() << ";" << endl;
	m_genCpp << "int g_nWndHeight = " << ds.GetMainTextureHeight() << ";" << endl;
#endif // FINAL_RELEASE
}

#ifndef FINAL_RELEASE
void CodeGenerator::SetDependenciesFromCommand( Commands::Command* pcmd, DependenciesData &dependenciesData )
{
	std::vector<void *> v = pcmd->DependsOn();
	for(unsigned j = 0;j < v.size();j++)
		dependenciesData.SetDependenciesFromResource(v[j]);
}

Commands::Command* CodeGenerator::GetCommandByEventId(int eventId)
{
	for (unsigned j = 0;j < m_cmds.size();j++)
	{
		if (m_cmds[j]->EventID_ == eventId)
		{
			return m_cmds[j];
		}
	}
	return NULL;
}

void CodeGenerator::BuildDepenciesSet( DependenciesData &dependenciesData )
{
	std::set<UINT> deviceStateChangingCommands;
	GenerateSetOfDeviceStateChangingCommands(deviceStateChangingCommands);

	for (unsigned i = 0;i < m_cmds.size();i++)
	{
		void *resHnd = m_cmds[i]->Creates();
		if (resHnd != (void *)-1)
			dependenciesData.SetLastResourceEvent(resHnd, m_cmds[i]->EventID_);

		// We add command to decencies set, if command change device state 
		if (deviceStateChangingCommands.count(m_cmds[i]->CommandId) > 0)
			SetDependenciesFromCommand(m_cmds[i], dependenciesData);

		// copy-before-present command flow type support (e.g. AvP)
		if (m_cmds[i]->CommandId == idResourceCopy || m_cmds[i]->CommandId == idResourceCopyRegion)
		{
			D3D10DDI_HRESOURCE hres;
			if (m_cmds[i]->CommandId == idResourceCopy)
				hres = ((Commands::ResourceCopy *)m_cmds[i])->hDstResource_;
			else
				hres = ((Commands::ResourceCopyRegion *)m_cmds[i])->hDstResource_;
			if (dependenciesData.IsResourceUsed(hres.pDrvPrivate))
			{
				UINT eventId = dependenciesData.GetLastResourceEvent(hres.pDrvPrivate);
				Commands::Command* pCmd = GetCommandByEventId(eventId);
				if (pCmd)
				{
					UINT BindFlags;
					if (pCmd->CommandId == idCreateResource || pCmd->CommandId == idCreateResource11_0)
					{
						BindFlags = ((Commands::CreateResourceBase *)pCmd)->CreateResource10_.BindFlags;
					}
					else __debugbreak();

					if (BindFlags & D3D10_DDI_BIND_PRESENT)
						dependenciesData.SetDependenciesFromEvent(eventId);
				}
			}
		}
	}

	size_t prevCount;
	do
	{
		prevCount = dependenciesData.GetDependenciesCount();

		for (size_t i = 0;i < m_cmds.size();i++)
		{
			void *resHnd = m_cmds[i]->Creates();
			if (resHnd != (void *)-1)
			{
				dependenciesData.SetLastResourceEvent(resHnd, m_cmds[i]->EventID_);
				if (dependenciesData.IsResourceUsed(resHnd))
					SetDependenciesFromCommand(m_cmds[i], dependenciesData);
			}
		}
	} // if we can't find new dependencies we exit from loop
	while (dependenciesData.GetDependenciesCount() != prevCount);
}

void CodeGenerator::AnalyzeCommandsForInputLayout()
{
	//CDumpState::ShaderLayoutMap &map = ds.GetShaderLayoutMap();
	std::map<void *,Commands::CreateElementLayout *> layouts;
	std::map<void *,Commands::CreateVertexShader *> shaders;

	Commands::CreateElementLayout *layout = 0;
	Commands::CreateVertexShader *shader = 0;
	for (unsigned i = 0;i < m_cmds.size();i++)
	{
		switch (m_cmds[i]->CommandId)
		{
		case idCreateElementLayout:
			{
				Commands::CreateElementLayout *l = (Commands::CreateElementLayout *)m_cmds[i];
				layouts[l->hElementLayout_.pDrvPrivate] = l;
			}
			break;
		case idIaSetInputLayout:
			{
				Commands::IaSetInputLayout *ia = (Commands::IaSetInputLayout *)m_cmds[i];
				if (ia->hInputLayout_.pDrvPrivate)
				{
					_ASSERT(layouts.find(ia->hInputLayout_.pDrvPrivate) != layouts.end());
					layout = layouts[ia->hInputLayout_.pDrvPrivate];
				}
				else
					layout = 0;
			}
			break;
		case idCreateVertexShader:
			{
				Commands::CreateVertexShader *l = (Commands::CreateVertexShader *)m_cmds[i];
				shaders[l->hShader_.pDrvPrivate] = l;
			}
			break;
		case idVsSetShader:
		case idVsSetShaderWithIfaces11_0:
			{
				Commands::xxSetShader *ia = (Commands::xxSetShader *)m_cmds[i];

				VOID* hShader = ia->hShader_.pDrvPrivate;
				_ASSERT(hShader == NULL || shaders.find(ia->hShader_.pDrvPrivate) != shaders.end());

				shader = shaders[ia->hShader_.pDrvPrivate];
			}
			break;
		}

		if (IsDrawCommand(m_cmds[i]->CommandId))
			ds.SetShadersOfLayout(layout,shader);
	}
}

void CodeGenerator::RemoveCommandsWithoutDependencies()
{
	size_t removedCommands = 0;
	std::set<void *> createdResourcesHandles;
	for (int i = (int)m_cmds.size() - 1;i >= 0;i--)
	{
		std::vector<void *> v = m_cmds[i]->DependsOn();
		for(unsigned j = 0;j < v.size();j++)
			createdResourcesHandles.erase(v[j]);

		void *resHnd = m_cmds[i]->Creates();
		if (!resHnd || resHnd == (void *)-1)
			continue;

		if (createdResourcesHandles.find(resHnd) != createdResourcesHandles.end())
		{
			removedCommands++;
			auto it = m_cmds.begin() + i;
			m_cmds.erase(it);
			continue;
		}

		createdResourcesHandles.insert(resHnd);
	}
}

void CodeGenerator::ProcessCommands()
{
	AnalyzeCommandsForInputLayout();

	RemoveCommandsWithoutDependencies();

	DependenciesData dependenciesData;
	BuildDepenciesSet(dependenciesData);

	for (unsigned i = 0;i < m_cmds.size();i++) 
	{
		void *resHnd = m_cmds[i]->Creates();
		if (resHnd != (void *)-1)
			dependenciesData.SetLastResourceEvent(resHnd, m_cmds[i]->EventID_);

		if (resHnd == (void *)-1 || dependenciesData.IsResourceUsed(resHnd))
			ds.AddClearTarget(m_cmds[i]->EventID_,m_cmds[i]->CommandId);
	}

	/*//m_genCpp << "const float black[4] = { 0,0,0,0 };" << endl;
	std::string sClearAll = "const float black[4] = { 0,0,0,0 };\n";
	for (unsigned i = 0;i < m_cmds.size();i++)
	{
		void *c = m_cmds[i]->Creates();

		if (m_cmds[i]->CommandId != idCreateRenderTargetView) continue;
		hresToEid[c] = m_cmds[i]->EventID_;

		//m_genCpp << "ctx->ClearRenderTargetView(rtview_" << m_cmds[i]->EventID_ << ",black);" << endl;
	}*/

	int commandIndex = 0;
	bool bRender = true;
	std::set<UINT> skipped;
	float cmdsPerc = 100.0f / m_cmds.size();
	for (unsigned i = 0;i < m_cmds.size();i++)
	{
		printf("\x0d%7d (%.2f%%): %-60s",m_cmds[i]->EventID_,i * cmdsPerc,CommandIDToString(m_cmds[i]->CommandId));

		if (m_cmds[i]->CommandId == idChangeDeviceView)
			bRender = !bRender;
		if (!bRender)
			continue;

		void *resHnd = m_cmds[i]->Creates();
		bool bIsCommandUseful = false;
		if (resHnd != (void *)-1)
		{
			dependenciesData.SetLastResourceEvent(resHnd, m_cmds[i]->EventID_);
			bIsCommandUseful = dependenciesData.IsResourceUsed(resHnd);
		}
		else
		{
			std::vector<void *> v = m_cmds[i]->DependsOn();
			bIsCommandUseful = v.empty() || dependenciesData.IsResourcesUsed(v);
		}
		if (bIsCommandUseful)
		{
			string sinit,sglob,sdecl;

			string& scommand = m_cmds[i]->GetBuiltCommand(&ds,sinit,sdecl,sglob,skipped);
			m_genCppDraw << scommand;// << endl;
			m_genCpp << sinit;
			m_genGlob << sdecl;
			m_genDecl << sglob;
			if (!scommand.empty())
			{
				if (commandIndex % 100 == 99)
				{
					m_genCppDraw << "SetProgress(" << commandIndex + 1 << ");" << endl;
				}
				commandIndex++;
			}
		}
		/*else
		{
		m_genCpp << "// SKIP_" << m_cmds[i]->EventID_ << ": " << m_cmds[i]->getCmdName() << endl;
		}*/
	}

	printf("\n");

	m_genCppDraw << "SetProgress(" << 0 << ");" << endl;
	m_genHeader << "const int MaxCommandIndex = " << commandIndex + 1 << ";" << endl;

	while (!resourcesToFree.empty())
	{
		m_genCpp << resourcesToFree.back().c_str() << "->Release();" << endl;
		resourcesToFree.pop_back();
	}

	m_genCppDraw << "// *** Skipped commands [" << skipped.size() << "] ***" << endl;	
	for(auto it = skipped.begin();it != skipped.end();++it)
	{
		char buf[128];
		sprintf(buf,"// %s",CommandIDToString((CommandsId)*it));
		m_genCppDraw << buf << endl;
	}
}

void CodeGenerator::GenerateSetOfDeviceStateChangingCommands(std::set<UINT>& base)
{
	static UINT baseCommands[] = 
	{
		idIaSetIndexBuffer,
		idIaSetVertexBuffers,
		idIaSetInputLayout,
		idSoSetTargets,

		idSetBlendState,
		idSetRasterizerState,
		idSetDepthStencilState,

		idGsSetConstantBuffers,
		idVsSetConstantBuffers,
		idPsSetConstantBuffers,
		idHsSetConstantBuffers11_0,
		idDsSetConstantBuffers11_0,
		idCsSetConstantBuffers11_0,

		idPsSetShaderResources,
		idVsSetShaderResources,
		idGsSetShaderResources,
		idHsSetShaderResources11_0,
		idDsSetShaderResources11_0,
		idCsSetShaderResources11_0,

		idPsSetSamplers,
		idVsSetSamplers,
		idGsSetSamplers,
		idHsSetSamplers11_0,
		idDsSetSamplers11_0,
		idCsSetSamplers11_0,

		idPsSetShader,
		idVsSetShader,
		idGsSetShader,
		idHsSetShader11_0,
		idDsSetShader11_0,
		idCsSetShader11_0,

		idPsSetShaderWithIfaces11_0,
		idVsSetShaderWithIfaces11_0,
		idGsSetShaderWithIfaces11_0,
		idHsSetShaderWithIfaces11_0,
		idDsSetShaderWithIfaces11_0,
		idCsSetShaderWithIfaces11_0,

		idSetRenderTargets,
		idSetRenderTargets11_0,
		idCsSetUnorderedAccessViews11_0,
	};
	for(int i = 0;i < _countof(baseCommands);i++)
	{
		base.insert(baseCommands[i]);
	}
}
#endif // FINAL_RELEASE

bool CodeGenerator::Save( const char* szCodeName_ )
{
	InternalGenerateHFile();
	InternalGenerateCppFile();

	_ASSERT( szCodeName_ );
	string str = szCodeName_;
	str += ".h";
	fstream file;
	file.open( str.c_str(), ios::out | ios::trunc );
	file << m_genHeader.rdbuf();
	file.close();

	str = szCodeName_;
	str += ".cpp";	
	file.open( str.c_str(), ios::out | ios::trunc );
	file << m_genCpp.rdbuf();
	file << m_genDecl.rdbuf();
	file.close();

	str = szCodeName_;
	str += "_draw.cpp";	
	file.open( str.c_str(), ios::out | ios::trunc );
	file << m_genCppDraw.rdbuf();
	file.close();

	str = szCodeName_;
	str += "_glob.h";	
	file.open( str.c_str(), ios::out | ios::trunc );
	file << m_genGlob.rdbuf();
	file.close();

	return true;
}
