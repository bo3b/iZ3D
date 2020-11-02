#include "StdAfx.h"
#include "Argument.h"
#include <atlbase.h>
#include <d3dx9.h>

CArgument::CArgument(void)
{
	m_customData = 0;
}

CArgument::~CArgument(void)
{
}

#define PARSE_TYPE_FIRST(type) \
	if (typeName == L#type) \
	{ \
		std::string typeName = #type; \
		type *data = new type; /* and no `delete' anywhere... */ \
		reader.ReadStartElement();
#define PARSE_TYPE_END() \
		reader.ReadEndElement(); \
		m_typeName = typeName; \
		return data; \
	} 
#define PARSE_TYPE(type) \
	PARSE_TYPE_END() \
	else \
	PARSE_TYPE_FIRST(type)

void *CArgument::ReadCustomData(LlamaXML::XMLReader &reader)
{
	reader.IsStartElement();
	std::wstring typeName = reader.GetLocalName().c_str();

	PARSE_TYPE_FIRST(D3DMATRIX)
	{
		for(int i = 0;i < 4;i++)
		{
			for(int j = 0;j < 4;j++)
			{
				ReadAtom(reader,data->m[i][j]);
			}
		}
	}
	PARSE_TYPE(D3DMATERIAL9)
	{
		ReadSubelement(reader,data->Diffuse);
		ReadSubelement(reader,data->Ambient);
		ReadSubelement(reader,data->Specular);
		ReadSubelement(reader,data->Emissive);

		ReadAtom(reader,data->Power);
	}
	PARSE_TYPE(D3DCOLORVALUE)
	{
		ReadAtom(reader,data->r);
		ReadAtom(reader,data->g);
		ReadAtom(reader,data->b);
		ReadAtom(reader,data->a);
	}
	PARSE_TYPE(D3DLIGHT9)
	{
		ReadInteger(reader,data->Type);

		ReadSubelement(reader,data->Diffuse);
		ReadSubelement(reader,data->Specular);
		ReadSubelement(reader,data->Ambient);

		ReadSubelement(reader,data->Position);
		ReadSubelement(reader,data->Direction);

		ReadAtom(reader,data->Range);
		ReadAtom(reader,data->Falloff);
		ReadAtom(reader,data->Attenuation0);
		ReadAtom(reader,data->Attenuation1);
		ReadAtom(reader,data->Attenuation2);
		ReadAtom(reader,data->Theta);
		ReadAtom(reader,data->Phi);
	}
	PARSE_TYPE(D3DVECTOR)
	{
		ReadAtom(reader,data->x);
		ReadAtom(reader,data->y);
		ReadAtom(reader,data->z);
	}
	PARSE_TYPE(D3DPRESENT_PARAMETERS)
	{
		ReadAtom(reader,data->BackBufferWidth);
		ReadAtom(reader,data->BackBufferHeight);
		ReadInteger(reader,data->BackBufferFormat);
		ReadAtom(reader,data->BackBufferCount);
		ReadInteger(reader,data->MultiSampleType);
		ReadAtom(reader,data->MultiSampleQuality);
		ReadInteger(reader,data->SwapEffect);
		ReadInteger(reader,data->hDeviceWindow);//reader.Skip();//ReadAtom(reader,data->hDeviceWindow);
		ReadAtom(reader,data->Windowed);
		ReadAtom(reader,data->EnableAutoDepthStencil);
		ReadInteger(reader,data->AutoDepthStencilFormat);
		ReadAtom(reader,data->Flags);
		ReadAtom(reader,data->FullScreen_RefreshRateInHz);
		ReadAtom(reader,data->PresentationInterval);
	}
	PARSE_TYPE(RECT)
	{
		ReadAtom(reader,data->left);
		ReadAtom(reader,data->top);
		ReadAtom(reader,data->right);
		ReadAtom(reader,data->bottom);
	}
	PARSE_TYPE(POINT)
	{
		ReadAtom(reader,data->x);
		ReadAtom(reader,data->y);
	}
	PARSE_TYPE(D3DRECT)
	{
		ReadAtom(reader,data->x1);
		ReadAtom(reader,data->y1);
		ReadAtom(reader,data->x2);
		ReadAtom(reader,data->y2);
	}
	PARSE_TYPE(D3DVIEWPORT9)
	{
		ReadAtom(reader,data->X);
		ReadAtom(reader,data->Y);
		ReadAtom(reader,data->Width);
		ReadAtom(reader,data->Height);
		ReadAtom(reader,data->MinZ);
		ReadAtom(reader,data->MaxZ);
	}	
	PARSE_TYPE(databin_poses)
	{
		UINT count = reader.GetAttributeValue("count",0);
		data->resize(count);
		for(UINT i = 0;i < count;i++)
		{
			UINT64 databin_pos;
			ReadAtom(reader,databin_pos);
			(*data)[i] = databin_pos;
		}
	}
	PARSE_TYPE_END()

	__debugbreak();
	return 0;
}

void CArgument::Read(LlamaXML::XMLReader &reader)
{
	CW2AEX<MAX_PATH> conv(reader.GetLocalName().c_str());
	m_name = conv.m_psz;

	reader.ReadStartElement();

	if (reader.HasAttribute("val")) 
		val = reader.GetAttribute("val",LlamaXML::TextEncoding::Application());

	if (reader.HasAttribute("databin_pos")) 
		databin_pos = _atoi64(reader.GetAttribute("databin_pos",LlamaXML::TextEncoding::Application()).c_str());

	if (!reader.IsEmptyElement())
	{
		m_customData = ReadCustomData(reader);
		reader.ReadEndElement();
	}
}
