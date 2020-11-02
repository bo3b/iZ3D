#pragma once

#include "LlamaXML/XMLReader.h"

#include <BaseTsd.h>
#include <string>

#define CAST(type,arg) ((arg).GetCustomData<type>(#type))

typedef std::vector<UINT64> databin_poses;

class CArgument
{
public:
	CArgument(void);
	~CArgument(void);

	template<class T>
	T &GetCustomData(const char *typeName)
	{
		if (!m_customData) __debugbreak();
		if (typeName != m_typeName) __debugbreak();
		return *(T *)m_customData;
	}

	void Read(LlamaXML::XMLReader &reader);

	std::string GetName() { return m_name; }

	operator std::string() { return val; }
	operator const char *() { return val.c_str(); }
	operator UINT64() { return databin_pos; }

protected:

	void *ReadCustomData(LlamaXML::XMLReader &reader);

	template<class T>
	void ReadCustomData(LlamaXML::XMLReader &reader, T &value) 
	{ 
		T *data = (T *)ReadCustomData(reader);
		value = *data;
	}

	template<class T>
	void ReadSubelement(LlamaXML::XMLReader &reader, T &value)
	{
		reader.ReadStartElement();
		ReadCustomData(reader,value);
		reader.ReadEndElement();
	}

	template<class T>
	void ReadAtom(LlamaXML::XMLReader &reader, T &value)
	{
		reader.ReadStartElement();
		value = reader.GetAttributeValue("val",(T)0);
	}

	template<class T>
	void ReadInteger(LlamaXML::XMLReader &reader, T &value)
	{
		ReadAtom(reader,(int &)value);
	}

protected:
	std::string val;
	UINT64 databin_pos;

	std::string m_name;
	void *m_customData;
	std::string m_typeName;
};

