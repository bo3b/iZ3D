#include "StdAfx.h"
#include "Command.h"
#include <stdarg.h>
#include <d3dx9.h>

CCommand::CCommand(void)
{
	m_defaultTarget = draw;
	bSkip = FALSE;
}

CCommand::~CCommand(void)
{
	while (m_args.size())
	{
		delete m_args.back();
		m_args.pop_back();
	}
}

std::string CCommand::Disable()
{
	for (int i = 0;i < output_target_count;i++)
	{
		if (m_outputs[i].empty()) continue;
		for (unsigned j = 0;j < m_outputs[i].length();j++)
		{
			if (m_outputs[i][j] == _T('\n')) m_outputs[i][j] = _T(' ');
		}
		m_outputs[i] = "#pragma message(\"Warning! Disabled command: " + m_outputs[i] + "\")\n";
	}
	return "";
}

#define OUTPUT_BODY(param) \
	const unsigned bufsize = 10000; \
	static char buffer[bufsize]; \
	va_list args; \
	va_start(args,format); \
	vsprintf_s(buffer,bufsize,format,args); \
	va_end(args); \
	m_outputs[param] += buffer;

void CCommand::output(const char *format,...)
{
	OUTPUT_BODY(m_defaultTarget);
	m_outputs[m_defaultTarget] += "\n";
}

void CCommand::output(output_target target,const char *format,...)
{
	OUTPUT_BODY(target);
	m_outputs[target] += "\n";
}

void CCommand::output_(const char *format,...)
{
	OUTPUT_BODY(m_defaultTarget);
}

void CCommand::output_(output_target target,const char *format,...)
{
	OUTPUT_BODY(target);
}

void CCommand::PrepareMatrix(unsigned index)
{
	CArgument &arg = GetArg(index);
	D3DMATRIX &m = CAST(D3DMATRIX,arg);

	output(decl,"extern D3DMATRIX matrix_%d;",EventId);
	output(glob,"D3DMATRIX matrix_%d;",EventId);
	for(int i = 0;i < 4;i++)
	{
		for(int j = 0;j < 4;j++)
		{
			if (m.m[i][j] == 0) continue;

			output(init,"\tmatrix_%d.m[%d][%d] = %ff;",EventId,i,j,m.m[i][j]);
		}
	}
}

void CCommand::PrepareRect(unsigned index,const char *postfix)
{
	CArgument &arg = GetArg(index);
	RECT &rect = CAST(RECT,arg);

	output(decl,"extern RECT rect_%d%s;",EventId,postfix);
	output(glob,"RECT rect_%d%s;",EventId,postfix);
	if (rect.left) output(init,"\trect_%d%s.left = %d;",EventId,postfix,rect.left);
	if (rect.top) output(init,"\trect_%d%s.top = %d;",EventId,postfix,rect.top);
	if (rect.right) output(init,"\trect_%d%s.right = %d;",EventId,postfix,rect.right);
	if (rect.bottom) output(init,"\trect_%d%s.bottom = %d;",EventId,postfix,rect.bottom);
}

void CCommand::PreparePoint(unsigned index,const char *postfix)
{
	CArgument &arg = GetArg(index);
	POINT &pnt = CAST(POINT,arg);

	output(decl,"extern POINT pnt_%d%s;",EventId,postfix);
	output(glob,"POINT pnt_%d%s;",EventId,postfix);
	if (pnt.x) output(init,"\tpnt_%d%s.x = %d;",EventId,postfix,pnt.x);
	if (pnt.y) output(init,"\tpnt_%d%s.y = %d;",EventId,postfix,pnt.y);
}

void CCommand::Prepare3dRect(unsigned index)
{
	CArgument &arg = GetArg(index);
	D3DRECT &rect = CAST(D3DRECT,arg);

	output(decl,"extern D3DRECT rect_%d;",EventId);
	output(glob,"D3DRECT rect_%d;",EventId);
	if (rect.x1) output(init,"\trect_%d.x1 = %d;",EventId,rect.x1);
	if (rect.y1) output(init,"\trect_%d.y1 = %d;",EventId,rect.y1);
	if (rect.x2) output(init,"\trect_%d.x2 = %d;",EventId,rect.x2);
	if (rect.y2) output(init,"\trect_%d.y2 = %d;",EventId,rect.y2);
}

void CCommand::PrepareViewport(unsigned index)
{
	CArgument &arg = GetArg(index);
	D3DVIEWPORT9 &vport = CAST(D3DVIEWPORT9,arg);

	output(decl,"extern D3DVIEWPORT9 vport_%d;",EventId);
	output(glob,"D3DVIEWPORT9 vport_%d;",EventId);
	if (vport.X) output(init,"\tvport_%d.X = %d;",EventId,vport.X);
	if (vport.Y) output(init,"\tvport_%d.Y = %d;",EventId,vport.Y);
	if (vport.Width) output(init,"\tvport_%d.Width = %d;",EventId,vport.Width);
	if (vport.Height) output(init,"\tvport_%d.Height = %d;",EventId,vport.Height);
	if (vport.MinZ) output(init,"\tvport_%d.MinZ = %ff;",EventId,vport.MinZ);
	if (vport.MaxZ) output(init,"\tvport_%d.MaxZ = %ff;",EventId,vport.MaxZ);
}

void CCommand::PrepareMaterial(unsigned index)
{
	CArgument &arg = GetArg(index);
	D3DMATERIAL9 &m = CAST(D3DMATERIAL9,arg);

	output(decl,"extern D3DMATERIAL9 material_%d;",EventId);
	output(glob,"D3DMATERIAL9 material_%d;",EventId);

	if (m.Diffuse.r != 0) output(init,"\tmaterial_%d.Diffuse.r = %ff;",EventId,m.Diffuse.r);
	if (m.Diffuse.g != 0) output(init,"\tmaterial_%d.Diffuse.g = %ff;",EventId,m.Diffuse.g);
	if (m.Diffuse.b != 0) output(init,"\tmaterial_%d.Diffuse.b = %ff;",EventId,m.Diffuse.b);
	if (m.Diffuse.a != 0) output(init,"\tmaterial_%d.Diffuse.a = %ff;",EventId,m.Diffuse.a);

	if (m.Ambient.r != 0) output(init,"\tmaterial_%d.Ambient.r = %ff;",EventId,m.Ambient.r);
	if (m.Ambient.g != 0) output(init,"\tmaterial_%d.Ambient.g = %ff;",EventId,m.Ambient.g);
	if (m.Ambient.b != 0) output(init,"\tmaterial_%d.Ambient.b = %ff;",EventId,m.Ambient.b);
	if (m.Ambient.a != 0) output(init,"\tmaterial_%d.Ambient.a = %ff;",EventId,m.Ambient.a);

	if (m.Specular.r != 0) output(init,"\tmaterial_%d.Specular.r = %ff;",EventId,m.Specular.r);
	if (m.Specular.g != 0) output(init,"\tmaterial_%d.Specular.g = %ff;",EventId,m.Specular.g);
	if (m.Specular.b != 0) output(init,"\tmaterial_%d.Specular.b = %ff;",EventId,m.Specular.b);
	if (m.Specular.a != 0) output(init,"\tmaterial_%d.Specular.a = %ff;",EventId,m.Specular.a);

	if (m.Emissive.r != 0) output(init,"\tmaterial_%d.Emissive.r = %ff;",EventId,m.Emissive.r);
	if (m.Emissive.g != 0) output(init,"\tmaterial_%d.Emissive.g = %ff;",EventId,m.Emissive.g);
	if (m.Emissive.b != 0) output(init,"\tmaterial_%d.Emissive.b = %ff;",EventId,m.Emissive.b);
	if (m.Emissive.a != 0) output(init,"\tmaterial_%d.Emissive.a = %ff;",EventId,m.Emissive.a);

	if (m.Power != 0) output(init,"\tmaterial_%d.Power = %ff;",EventId,m.Power);
}

void CCommand::PrepareLight(unsigned index)
{
	CArgument &arg = GetArg(index);
	D3DLIGHT9 &l = CAST(D3DLIGHT9,arg);

	output(decl,"extern D3DLIGHT9 light_%d;",EventId);
	output(glob,"D3DLIGHT9 light_%d;",EventId);

	if (l.Type != 0) output(init,"\tlight_%d.Type = (D3DLIGHTTYPE)%d;",EventId,l.Type);

	if (l.Diffuse.r != 0) output(init,"\tlight_%d.Diffuse.r = %ff;",EventId,l.Diffuse.r);
	if (l.Diffuse.g != 0) output(init,"\tlight_%d.Diffuse.g = %ff;",EventId,l.Diffuse.g);
	if (l.Diffuse.b != 0) output(init,"\tlight_%d.Diffuse.b = %ff;",EventId,l.Diffuse.b);
	if (l.Diffuse.a != 0) output(init,"\tlight_%d.Diffuse.a = %ff;",EventId,l.Diffuse.a);

	if (l.Ambient.r != 0) output(init,"\tlight_%d.Ambient.r = %ff;",EventId,l.Ambient.r);
	if (l.Ambient.g != 0) output(init,"\tlight_%d.Ambient.g = %ff;",EventId,l.Ambient.g);
	if (l.Ambient.b != 0) output(init,"\tlight_%d.Ambient.b = %ff;",EventId,l.Ambient.b);
	if (l.Ambient.a != 0) output(init,"\tlight_%d.Ambient.a = %ff;",EventId,l.Ambient.a);

	if (l.Specular.r != 0) output(init,"\tlight_%d.Specular.r = %ff;",EventId,l.Specular.r);
	if (l.Specular.g != 0) output(init,"\tlight_%d.Specular.g = %ff;",EventId,l.Specular.g);
	if (l.Specular.b != 0) output(init,"\tlight_%d.Specular.b = %ff;",EventId,l.Specular.b);
	if (l.Specular.a != 0) output(init,"\tlight_%d.Specular.a = %ff;",EventId,l.Specular.a);

	if (l.Position.x != 0) output(init,"\tlight_%d.Position.x = %ff;",EventId,l.Position.x);
	if (l.Position.y != 0) output(init,"\tlight_%d.Position.y = %ff;",EventId,l.Position.y);
	if (l.Position.z != 0) output(init,"\tlight_%d.Position.z = %ff;",EventId,l.Position.z);

	if (l.Direction.x != 0) output(init,"\tlight_%d.Direction.x = %ff;",EventId,l.Direction.x);
	if (l.Direction.y != 0) output(init,"\tlight_%d.Direction.y = %ff;",EventId,l.Direction.y);
	if (l.Direction.z != 0) output(init,"\tlight_%d.Direction.z = %ff;",EventId,l.Direction.z);

	if (l.Range != 0) output(init,"\tlight_%d.Range = %ff;",EventId,l.Range);
	if (l.Falloff != 0) output(init,"\tlight_%d.Falloff = %ff;",EventId,l.Falloff);
	if (l.Attenuation0 != 0) output(init,"\tlight_%d.Attenuation0 = %ff;",EventId,l.Attenuation0);
	if (l.Attenuation1 != 0) output(init,"\tlight_%d.Attenuation1 = %ff;",EventId,l.Attenuation1);
	if (l.Attenuation2 != 0) output(init,"\tlight_%d.Attenuation2 = %ff;",EventId,l.Attenuation2);
	if (l.Theta != 0) output(init,"\tlight_%d.Theta = %ff;",EventId,l.Theta);
	if (l.Phi != 0) output(init,"\tlight_%d.Phi = %ff;",EventId,l.Phi);
}

extern int g_nWndWidth;
extern int g_nWndHeight;

void CCommand::PrepareCreateDevice(unsigned index)
{
	CArgument &arg = GetArg(index);
	D3DPRESENT_PARAMETERS &dpp = CAST(D3DPRESENT_PARAMETERS,arg);

	//output(init,"\tD3DPRESENT_PARAMETERS dpp;");

	/*g_nWndWidth = dpp.BackBufferWidth;
	g_nWndHeight = dpp.BackBufferHeight;*/

	output(decl, "const int g_nWndWidth = %d;", dpp.BackBufferWidth);
	output(decl, "const int g_nWndHeight = %d;", dpp.BackBufferHeight);

	output(init,"\tdpp.BackBufferWidth = %d;",dpp.BackBufferWidth);
	output(init,"\tdpp.BackBufferHeight = %d;",dpp.BackBufferHeight);
	output(init,"\tdpp.BackBufferFormat = (D3DFORMAT)%d;",dpp.BackBufferFormat);
	output(init,"\tdpp.BackBufferCount = %d;",dpp.BackBufferCount);
	output(init,"\tdpp.MultiSampleType = (D3DMULTISAMPLE_TYPE)%d;",dpp.MultiSampleType);
	output(init,"\tdpp.MultiSampleQuality = %d;",dpp.MultiSampleQuality);
	output(init,"\tdpp.SwapEffect = (D3DSWAPEFFECT)%d;",dpp.SwapEffect);
	output(init,"\tdpp.hDeviceWindow = hWnd;");//%d;",dpp.hDeviceWindow);
	output(init,"\tdpp.Windowed = 1; // original: %d",dpp.Windowed);
	output(init,"\tdpp.EnableAutoDepthStencil = 1; // original: %d",dpp.EnableAutoDepthStencil/* || dpp.AutoDepthStencilFormat*/);
	output(init,"\tdpp.AutoDepthStencilFormat = (D3DFORMAT)%d; // original: %d",
		dpp.AutoDepthStencilFormat ? dpp.AutoDepthStencilFormat : 75,dpp.AutoDepthStencilFormat);
	output(init,"\tdpp.Flags = %d;",dpp.Flags);
	output(init,"\tdpp.FullScreen_RefreshRateInHz = 0;",dpp.FullScreen_RefreshRateInHz);
	output(init,"\tdpp.PresentationInterval = 0; // original: %u",dpp.PresentationInterval);
}

void CCommand::SetTarget(output_target target)
{
	m_defaultTarget = target;
}

const std::string& CCommand::GetOutput(output_target target)
{
	return m_outputs[target];
}
