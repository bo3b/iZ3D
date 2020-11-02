#pragma once

#include "command.h"

typedef std::string (* CommandHandler)(CCommand *,int);

#define HANDLER_NAME(cmd,arg) \
	__##cmd##__##arg##__handler

#define HANDLER_DECLARATION(cmd,arg) \
	std::string HANDLER_NAME(cmd,arg)(CCommand *command,int index)

#define HANDLER_CLASS_NAME(cmd,arg) \
	HANDLER_NAME(cmd,arg) ## __class

#define CMD_HANDLER_ADD(cmd,arg,func) \
	class HANDLER_CLASS_NAME(cmd,arg) \
	{ \
	public: \
	HANDLER_CLASS_NAME(cmd,arg)() \
		{ \
			HandlersMap[std::make_pair<std::string,std::string>(#cmd,#arg)] = func; \
		} \
	}; \
	HANDLER_CLASS_NAME(cmd,arg) HANDLER_CLASS_NAME(cmd,arg) ## __instance;

#define CMD_HANDLER(cmd,arg) \
	HANDLER_DECLARATION(cmd,arg); \
	CMD_HANDLER_ADD(cmd,arg,HANDLER_NAME(cmd,arg)) \
	HANDLER_DECLARATION(cmd,arg)

#define CMD_HANDLER_BEFORE(cmd) \
	CMD_HANDLER(cmd,__before)

#define CMD_HANDLER_AFTER(cmd) \
	CMD_HANDLER(cmd,__after)

#define CMD_HANDLER_CAST(cmd,type,arg) \
	CMD_HANDLER(cmd,arg) \
	{ \
		command->output_("(" #type ")"); \
		return ""; \
	}

#define CMD_HANDLER_COMMENT(cmd) \
	CMD_HANDLER_BEFORE(cmd) \
	{ \
		command->output_("//"); \
		return ""; \
	}

#define CMD_HANDLER_CREATE_EX(cmd,arg,type,name,target,str) \
	CMD_HANDLER_BEFORE(cmd) \
    { \
	    command->output(decl,"extern " #type " *" #name "_%d;",command->EventId); \
	    command->output(glob,#type " *" #name "_%d;",command->EventId); \
		command->SetTarget(target); \
		return str; \
	} \
	CMD_HANDLER(cmd,arg) \
	{ \
		command->Creates = (const char *)command->GetArg(index); \
		PtrToEidMap[command->GetArg(index)] = command->EventId; \
		command->SaveEid(index); \
		return sprintf("&" #name "_%d",command->EventId); \
	}

#define CMD_HANDLER_CREATE(cmd,arg,type,name) \
	CMD_HANDLER_CREATE_EX(cmd,arg,type,name,init,"")

#define CMD_HANDLER_USE(cmd,arg,name) \
	CMD_HANDLER(cmd,arg) \
	{ \
		if (strcmp(command->GetArg(index),"00000000") == 0) \
		{ \
			return "0"; \
		} \
		else \
		{ \
			UsedMap[PtrToEidMap[command->GetArg(index)]] = command->EventId; \
			return sprintf(#name "_%d",command->GetEid(index)); \
		} \
	}

inline std::string CallHandler(std::map<std::pair<std::string,std::string>,CommandHandler>& map, std::pair<std::string,std::string>& pair, CCommand * info, int index)
{
	auto it = map.find(pair);
	if (it != map.end()) 
		return (it->second)(info,index);
	else
		return std::string();
}

#define CALL_HANDLER(cmd,arg,info,index) \
	CallHandler(HandlersMap, std::make_pair<std::string,std::string>((cmd),(arg)), info, index)

struct replacement
{
	const char *from, *to;
};