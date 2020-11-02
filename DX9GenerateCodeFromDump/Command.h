#pragma once

#include <BaseTsd.h>
#include <string>
#include <map>
#include <vector>

#include "argument.h"

enum output_target
{
	draw,
	init,
	glob,
	decl,

	output_target_count
};

void SaveEid(unsigned eid,std::string value);
unsigned GetEid(std::string value);

class CCommand
{
public:
	CCommand(void);
	~CCommand(void);

	void output(const char *format,...);
	void output(output_target target,const char *format,...);
	void output_(const char *format,...);
	void output_(output_target target,const char *format,...);

	void PushArg(CArgument *arg) { m_args.push_back(arg); }
	size_t ArgCount() { return m_args.size(); }

	std::string ArgName(unsigned index) { return m_args[index]->GetName(); }
	CArgument &GetArg(unsigned index) { if (index < 0 || index >= m_args.size()) __debugbreak(); return *m_args[index]; }

	void PrepareMatrix(unsigned index);
	void PrepareRect(unsigned index,const char *postfix = "");
	void PreparePoint(unsigned index,const char *postfix = "");
	void Prepare3dRect(unsigned index);
	void PrepareViewport(unsigned index);
	void PrepareMaterial(unsigned index);
	void PrepareLight(unsigned index);
	void PrepareCreateDevice(unsigned index);	

	void SetTarget(output_target target);
	output_target GetDefaultTarget() { return m_defaultTarget; }
	const std::string& GetOutput(output_target target);

	void SaveEid(int index) { ::SaveEid(EventId,GetArg(index)); }
	unsigned GetEid(int index) { return ::GetEid(GetArg(index)); }

	std::string Skip() { bSkip = TRUE; return ""; }
	std::string Disable();

public:
	std::string Name;
	unsigned EventId;

	std::string Creates;
	BOOL bSkip;

protected:
	std::vector<CArgument *> m_args;

	output_target m_defaultTarget;
	std::string m_outputs[output_target_count];
};

