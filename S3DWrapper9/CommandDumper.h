#pragma once

#include "LlamaXML/FileOutputStream.h"
#include "LlamaXML/XMLWriter.h"
#include <sstream>

template<class T,int dummy = 0> struct DataWrapper;

typedef DataWrapper<DWORD> ShaderFunction;
typedef DataWrapper<float> ShaderConstantsF;
typedef DataWrapper<int> ShaderConstantsI;
typedef DataWrapper<BOOL> ShaderConstantsB;
typedef DataWrapper<void> JustData;
typedef DataWrapper<UINT64> DatabinPoses;
typedef DataWrapper<float,4> ClipPlaneData;

#ifndef FINAL_RELEASE

class CCommandDumper
{
protected:
	CCommandDumper(void);

public:
	static CCommandDumper &GetInstance() { return m_instance; }
	~CCommandDumper(void);
	void Init();
	bool IsOK();
	
	BOOL CanDump();
	BOOL BeginCommand(const TCHAR *cmdName);
	void EndCommand();
	
	template<class T>
	void DumpCommandArgument(const char *argName,const T &value);

	template<class T>
	void WriteArgumentData(const T &value);

	template<class T>
	void WriteArgumentDataDefault(const T &value);

	template<class T>
	void WriteArgumentDataCustom(const T &value) { }

	BOOL SetForce(BOOL bForce) { BOOL ret = m_bForce; m_bForce = bForce; return ret; }

protected:
	LlamaXML::FileOutputStream *m_pOutputStream;
	LlamaXML::XMLWriter *m_pXMLWriter;

	CRITICAL_SECTION m_cs;
	UINT m_EventId;

	BOOL m_bForce; // force command dumping (even if the call was from inside the driver)
	
	static CCommandDumper m_instance;
};

template<class T>
void CCommandDumper::WriteArgumentDataDefault(const T &value)
{
	std::stringstream convert;
	convert << value;
	if (convert.fail())
	{
		m_pXMLWriter->WriteAttribute("val","");
	}
	else
	{
		m_pXMLWriter->WriteAttribute("val",convert.str());
	}
}

template<class T>
void CCommandDumper::WriteArgumentData(const T &value)
{
	WriteArgumentDataDefault(value);
	WriteArgumentDataCustom(value);
}

template<class T>
void CCommandDumper::DumpCommandArgument(const char *argName,const T &value)
{
	m_pXMLWriter->StartElement(argName);
	WriteArgumentData(value);
	m_pXMLWriter->EndElement();
}

template<class T,int defCount>
struct DataWrapper
{
	DataWrapper(const T *data,UINT count = defCount) : data(data), count(count) { }
	operator const T *() const { return data; }
	operator UINT() const { return count; }
protected:
	const T *data;
	UINT count;
};

template <> void CCommandDumper::WriteArgumentDataCustom<IDirect3DVertexBuffer9 *>(IDirect3DVertexBuffer9 *const &value);
template <> void CCommandDumper::WriteArgumentDataCustom<IDirect3DIndexBuffer9 *>(IDirect3DIndexBuffer9 *const &value);
template <> void CCommandDumper::WriteArgumentDataCustom<CONST D3DMATRIX *>(CONST D3DMATRIX *const &value);
template <> void CCommandDumper::WriteArgumentDataCustom<CONST D3DMATERIAL9 *>(CONST D3DMATERIAL9 *const &value);
template <> void CCommandDumper::WriteArgumentDataCustom<CONST D3DLIGHT9 *>(CONST D3DLIGHT9 *const &value);
template <> void CCommandDumper::WriteArgumentDataCustom<D3DPRESENT_PARAMETERS *>(D3DPRESENT_PARAMETERS *const &value);
template <> void CCommandDumper::WriteArgumentDataCustom<CONST D3DVERTEXELEMENT9 *>(CONST D3DVERTEXELEMENT9 *const &value);
template <> void CCommandDumper::WriteArgumentDataCustom<CONST RECT *>(CONST RECT *const &value);
template <> void CCommandDumper::WriteArgumentDataCustom<CONST POINT *>(CONST POINT *const &value);
template <> void CCommandDumper::WriteArgumentDataCustom<CONST D3DVIEWPORT9 *>(CONST D3DVIEWPORT9 *const &value);
template <> void CCommandDumper::WriteArgumentDataCustom<CONST D3DGAMMARAMP *>(CONST D3DGAMMARAMP *const &value);
template <> void CCommandDumper::WriteArgumentDataCustom<CONST D3DRECT *>(CONST D3DRECT *const &value);


template <> void CCommandDumper::WriteArgumentData<D3DCOLORVALUE>(D3DCOLORVALUE const &value);
template <> void CCommandDumper::WriteArgumentData<D3DVECTOR>(D3DVECTOR const &value);
template <> void CCommandDumper::WriteArgumentData<ShaderFunction>(ShaderFunction const &value);
template <> void CCommandDumper::WriteArgumentData<ShaderConstantsF>(ShaderConstantsF const &value);
template <> void CCommandDumper::WriteArgumentData<ShaderConstantsI>(ShaderConstantsI const &value);
template <> void CCommandDumper::WriteArgumentData<JustData>(JustData const &value);
template <> void CCommandDumper::WriteArgumentData<ClipPlaneData>(ClipPlaneData const &value);
template <> void CCommandDumper::WriteArgumentData<DatabinPoses>(DatabinPoses const &value);
//template <> void CCommandDumper::WriteArgumentData<Rects>(Rects const &value);

class CArgumentWrapperBase 
{ 
public:
	virtual void SetName(const char *name) = 0;
	virtual void Dump(CCommandDumper &dumper) = 0;
	virtual void Delete() = 0;
};

template<class T>
class CArgumentWrapper : public CArgumentWrapperBase
{
public:
	CArgumentWrapper(const T &value) : m_value(value) { }
	void SetName(const char *name) { m_name = name; }
	void Dump(CCommandDumper &dumper) { dumper.DumpCommandArgument(m_name.c_str(),m_value); }
	void Delete() { delete (CArgumentWrapper<T> *)this; }
protected:
	std::string m_name;
	const T &m_value;
};

class CArgumentWrappers
{
public:
	~CArgumentWrappers()
	{
		for(auto &it = m_args.begin();it != m_args.end();++it) (*it)->Delete();//delete *it;
		m_args.clear();
	}

	void Set() { }

	template<class T1>
	void Set(const T1 &a1)
	{
		AddArg(new CArgumentWrapper<T1>(a1));
	}

	template<class T1,class T2>
	void Set(const T1 &a1,const T2 &a2)
	{
		AddArg(new CArgumentWrapper<T1>(a1));
		AddArg(new CArgumentWrapper<T2>(a2));
	}

	template<class T1,class T2,class T3>
	void Set(const T1 &a1,const T2 &a2,const T3 &a3)
	{
		AddArg(new CArgumentWrapper<T1>(a1));
		AddArg(new CArgumentWrapper<T2>(a2));
		AddArg(new CArgumentWrapper<T3>(a3));
	}

	template<class T1,class T2,class T3,class T4>
	void Set(const T1 &a1,const T2 &a2,const T3 &a3,const T4 &a4)
	{
		AddArg(new CArgumentWrapper<T1>(a1));
		AddArg(new CArgumentWrapper<T2>(a2));
		AddArg(new CArgumentWrapper<T3>(a3));
		AddArg(new CArgumentWrapper<T4>(a4));
	}

	template<class T1,class T2,class T3,class T4,class T5>
	void Set(const T1 &a1,const T2 &a2,const T3 &a3,const T4 &a4,const T5 &a5)
	{
		AddArg(new CArgumentWrapper<T1>(a1));
		AddArg(new CArgumentWrapper<T2>(a2));
		AddArg(new CArgumentWrapper<T3>(a3));
		AddArg(new CArgumentWrapper<T4>(a4));
		AddArg(new CArgumentWrapper<T5>(a5));
	}

	template<class T1,class T2,class T3,class T4,class T5,class T6>
	void Set(const T1 &a1,const T2 &a2,const T3 &a3,const T4 &a4,const T5 &a5,const T6 &a6)
	{
		AddArg(new CArgumentWrapper<T1>(a1));
		AddArg(new CArgumentWrapper<T2>(a2));
		AddArg(new CArgumentWrapper<T3>(a3));
		AddArg(new CArgumentWrapper<T4>(a4));
		AddArg(new CArgumentWrapper<T5>(a5));
		AddArg(new CArgumentWrapper<T6>(a6));
	}

	template<class T1,class T2,class T3,class T4,class T5,class T6,class T7>
	void Set(const T1 &a1,const T2 &a2,const T3 &a3,const T4 &a4,const T5 &a5,const T6 &a6,const T7 &a7)
	{
		AddArg(new CArgumentWrapper<T1>(a1));
		AddArg(new CArgumentWrapper<T2>(a2));
		AddArg(new CArgumentWrapper<T3>(a3));
		AddArg(new CArgumentWrapper<T4>(a4));
		AddArg(new CArgumentWrapper<T5>(a5));
		AddArg(new CArgumentWrapper<T6>(a6));
		AddArg(new CArgumentWrapper<T7>(a7));
	}

	template<class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8>
	void Set(const T1 &a1,const T2 &a2,const T3 &a3,const T4 &a4,const T5 &a5,const T6 &a6,const T7 &a7,const T8 &a8)
	{
		AddArg(new CArgumentWrapper<T1>(a1));
		AddArg(new CArgumentWrapper<T2>(a2));
		AddArg(new CArgumentWrapper<T3>(a3));
		AddArg(new CArgumentWrapper<T4>(a4));
		AddArg(new CArgumentWrapper<T5>(a5));
		AddArg(new CArgumentWrapper<T6>(a6));
		AddArg(new CArgumentWrapper<T7>(a7));
		AddArg(new CArgumentWrapper<T8>(a8));
	}

	template<class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9>
	void Set(const T1 &a1,const T2 &a2,const T3 &a3,const T4 &a4,const T5 &a5,const T6 &a6,const T7 &a7,const T8 &a8,const T9 &a9)
	{
		AddArg(new CArgumentWrapper<T1>(a1));
		AddArg(new CArgumentWrapper<T2>(a2));
		AddArg(new CArgumentWrapper<T3>(a3));
		AddArg(new CArgumentWrapper<T4>(a4));
		AddArg(new CArgumentWrapper<T5>(a5));
		AddArg(new CArgumentWrapper<T6>(a6));
		AddArg(new CArgumentWrapper<T7>(a7));
		AddArg(new CArgumentWrapper<T8>(a8));
		AddArg(new CArgumentWrapper<T9>(a9));
	}

	void SetNames(const char *names);
	void SetNames() { }
	void Store(CCommandDumper &dumper);

protected:
	void AddArg(CArgumentWrapperBase *argWrap) { m_args.push_back(argWrap); }

protected:
	std::vector<CArgumentWrapperBase *> m_args;
};

#define GET_DUMPER() \
	CCommandDumper::GetInstance()

#define DUMP_CMD_ARGS(...) \
	{ \
		CArgumentWrappers argWrap; \
		argWrap.Set(__VA_ARGS__); \
		argWrap.SetNames(#__VA_ARGS__); \
		argWrap.Store(GET_DUMPER()); \
	}

#define BEGIN_COMMAND(commandName) \
	GET_DUMPER().BeginCommand(_T(#commandName))

#define END_COMMAND() \
	GET_DUMPER().EndCommand()

#define DUMP_CMD_ALL(commandName,...) \
	if (GINFO_DUMP_ON && BEGIN_COMMAND(commandName)) \
	{ \
		DUMP_CMD_ARGS(__VA_ARGS__); \
		END_COMMAND(); \
	} \
	INSIDE;

#define DUMP_CMD_BASE(commandName,isdraw,...) \
	if (GINFO_DUMP_ON && (m_DumpType == dtCommands || m_bInStateBlock)) \
	{ \
		if (isdraw && GET_DUMPER().CanDump()) \
			m_deviceState.DumpResources(); \
		DUMP_CMD_ALL(commandName,__VA_ARGS__); \
	} \
	INSIDE;

#define DUMP_CMD_DRAW(commandName,...)	DUMP_CMD_BASE(commandName,true,__VA_ARGS__)
#define DUMP_CMD(commandName,...)		DUMP_CMD_BASE(commandName,false,__VA_ARGS__)

#define DUMP_CMD_STATE(commandName,...) \
	if (GET_DUMPER().CanDump()) \
	{ \
		if (GINFO_DUMP_ON) \
			if (!m_bInStateBlock) m_deviceState.commandName(__VA_ARGS__); \
			else m_tempDeviceState.commandName(__VA_ARGS__); \
		DUMP_CMD_BASE(commandName,false,__VA_ARGS__); \
	}

class CInsideDriverHelper
{
public:
	CInsideDriverHelper(int dummy) { ::InterlockedIncrement(&m_counter); }
	~CInsideDriverHelper() { ::InterlockedDecrement(&m_counter); }
	void dummy_call() { }
	static BOOL isInside() { return m_counter > 0; }

	/*debug only*/static LONG getCounter() { return m_counter; }
protected:
	static __declspec(thread) LONG m_counter;
};

#define INSIDE CInsideDriverHelper __inside__helper(0); __inside__helper.dummy_call()
//#define IS_INSIDE (CInsideDriverHelper::isInside())

#else // FINAL_RELEASE

#define DUMP_CMD_ALL(...)
#define DUMP_CMD(...)
#define DUMP_CMD_DRAW(...)
#define DUMP_CMD_STATE(...)
#define DUMP_CMD_UPDATE_STATE(...)
#define	INSIDE

template<class T,int dummy>
struct DataWrapper { public: DataWrapper(...) { } };

#endif // FINAL_RELEASE

typedef DataWrapper<DWORD> ShaderFunction;
typedef DataWrapper<float> ShaderConstantsF;
typedef DataWrapper<int> ShaderConstantsI;
typedef DataWrapper<BOOL> ShaderConstantsB;
typedef DataWrapper<void> JustData;
typedef DataWrapper<float,4> ClipPlaneData;
